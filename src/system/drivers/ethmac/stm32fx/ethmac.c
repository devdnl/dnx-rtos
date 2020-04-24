/*=========================================================================*//**
@file    ethmac.c

@author  Daniel Zorychta

@brief   This driver support Ethernet interface.

@note    Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes dnx RTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


*//*==========================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/driver.h"
#include "ethmac_cfg.h"
#include "ethmac_ioctl.h"
#include "stm32f4x7_eth.h"

#if defined(ARCH_stm32f1)
#include "stm32f10x.h"
#elif defined(ARCH_stm32f4)
#include "stm32f4xx.h"
#endif

/*==============================================================================
  Local macros
==============================================================================*/
#define INIT_TIMEOUT            2000
#define PHY_BSR_LINK_STATUS     (1 << 2)

#if defined(ARCH_stm32f1)
#define AHBxENR                  AHBENR
#define RCC_AHBxENR_ETHMACRXEN   RCC_AHBENR_ETHMACRXEN
#define RCC_AHBxENR_ETHMACTXEN   RCC_AHBENR_ETHMACTXEN
#define RCC_AHBxENR_ETHMACEN     RCC_AHBENR_ETHMACEN
#elif defined(ARCH_stm32f4)
#define AHBxENR                  AHB1ENR
#define RCC_AHBxENR_ETHMACRXEN   RCC_AHB1ENR_ETHMACRXEN
#define RCC_AHBxENR_ETHMACTXEN   RCC_AHB1ENR_ETHMACTXEN
#define RCC_AHBxENR_ETHMACEN     RCC_AHB1ENR_ETHMACEN
#endif

/*==============================================================================
  Local object types
==============================================================================*/
struct ethmac {
        sem_t              *rx_data_ready;
        mutex_t            *rx_access;
        mutex_t            *tx_access;
        dev_lock_t          dev_lock;
        ETH_DMADESCTypeDef  DMA_tx_descriptor[ETHMAC_TXBUFNB];
        ETH_DMADESCTypeDef  DMA_rx_descriptor[ETHMAC_RXBUFNB];
        u8_t                tx_buffer[ETHMAC_TXBUFNB][ETH_MAX_PACKET_SIZE];
        u8_t                rx_buffer[ETHMAC_RXBUFNB][ETH_MAX_PACKET_SIZE];
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static bool   is_Ethernet_started       (void);
static void   send_packet               (size_t size);
static size_t wait_for_packet           (struct ethmac *hdl, uint32_t timeout);
static void   give_Rx_buffer_to_DMA     (void);
static bool   is_buffer_owned_by_DMA    (ETH_DMADESCTypeDef *DMA_descriptor);
static void   make_Rx_buffer_available  (void);
static u8_t  *get_buffer_address        (ETH_DMADESCTypeDef *DMA_descriptor);

/*==============================================================================
  Local objects
==============================================================================*/
MODULE_NAME(ETHMAC);

static struct ethmac *ethmac;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/
extern ETH_DMADESCTypeDef *DMARxDescToGet;
extern ETH_DMADESCTypeDef *DMATxDescToSet;

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize device
 *
 * @param[out]          **device_handle        device allocated memory
 * @param[in ]            major                major device number
 * @param[in ]            minor                minor device number
 * @param[in ]            config               optional module configuration
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_INIT(ETHMAC, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        UNUSED_ARG1(config);

        if (major != 0 || minor != 0) {
                return ENODEV;
        }

        int err = sys_zalloc(sizeof(struct ethmac), device_handle);
        if (!err) {
                struct ethmac *hdl = *device_handle;

                err = sys_semaphore_create(1, 0, &hdl->rx_data_ready);
                if (err != ESUCC)
                        goto finish;

                err = sys_mutex_create(MUTEX_TYPE_NORMAL, &hdl->rx_access);
                if (err != ESUCC)
                        goto finish;

                err = sys_mutex_create(MUTEX_TYPE_NORMAL, &hdl->tx_access);
                if (err != ESUCC)
                        goto finish;

                // reset Ethernet
                ETH_DeInit();
                ETH_SoftwareReset();

                u32_t timeout = sys_time_get_reference();
                while (  ETH_GetSoftwareResetStatus() == SET
                      && !sys_time_is_expired(timeout, INIT_TIMEOUT));

                if (sys_time_is_expired(timeout, INIT_TIMEOUT)) {
                        err = ETIME;
                        goto finish;
                }

                // enable interrupts
                SET_BIT(RCC->AHBxENR, RCC_AHBxENR_ETHMACRXEN | RCC_AHBxENR_ETHMACTXEN | RCC_AHBxENR_ETHMACEN);

                NVIC_EnableIRQ(ETH_IRQn);
                NVIC_SetPriority(ETH_IRQn, _CPU_IRQ_SAFE_PRIORITY_);

                /* MAC configuration */
                /*
                 * Ethernet DMA configuration
                 * When we use the Checksum offload feature, we need to enable the Store and Forward mode:
                 * the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can
                 * insert/verify the checksum, if the checksum is OK the DMA can handle the frame otherwise
                 * the frame is dropped
                 */
                ETH_InitTypeDef ETH_InitStructure;
                ETH_StructInit(&ETH_InitStructure);
                ETH_InitStructure.ETH_AutoNegotiation             = ETHMAC_PHY_AUTONEGOTIATION;
                ETH_InitStructure.ETH_LoopbackMode                = ETH_LoopbackMode_Disable;
                ETH_InitStructure.ETH_RetryTransmission           = ETH_RetryTransmission_Disable;
                ETH_InitStructure.ETH_AutomaticPadCRCStrip        = ETH_AutomaticPadCRCStrip_Disable;
                ETH_InitStructure.ETH_ReceiveAll                  = ETH_ReceiveAll_Disable;
                ETH_InitStructure.ETH_BroadcastFramesReception    = ETH_BroadcastFramesReception_Enable;
                ETH_InitStructure.ETH_PromiscuousMode             = ETH_PromiscuousMode_Disable;
                ETH_InitStructure.ETH_MulticastFramesFilter       = ETH_MulticastFramesFilter_Perfect;
                ETH_InitStructure.ETH_UnicastFramesFilter         = ETH_UnicastFramesFilter_Perfect;
                ETH_InitStructure.ETH_Speed                       = ETHMAC_SPEED;
                ETH_InitStructure.ETH_ChecksumOffload             = ETHMAC_CHECKSUM_BY_HARDWARE ? ETH_ChecksumOffload_Enable : 0;
                ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;
                ETH_InitStructure.ETH_ReceiveStoreForward         = ETH_ReceiveStoreForward_Enable;
                ETH_InitStructure.ETH_TransmitStoreForward        = ETH_TransmitStoreForward_Enable;
                ETH_InitStructure.ETH_ForwardErrorFrames          = ETH_ForwardErrorFrames_Disable;
                ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;
                ETH_InitStructure.ETH_SecondFrameOperate          = ETH_SecondFrameOperate_Enable;
                ETH_InitStructure.ETH_AddressAlignedBeats         = ETH_AddressAlignedBeats_Enable;
                ETH_InitStructure.ETH_FixedBurst                  = ETH_FixedBurst_Enable;
                ETH_InitStructure.ETH_RxDMABurstLength            = ETH_RxDMABurstLength_32Beat;
                ETH_InitStructure.ETH_TxDMABurstLength            = ETH_TxDMABurstLength_32Beat;
                ETH_InitStructure.ETH_DMAArbitration              = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

                if (ETH_Init(&ETH_InitStructure, ETHMAC_PHY_ADDRESS)) {

                        ethmac = hdl;

                        ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);

                        ETH_DMATxDescChainInit(ethmac->DMA_tx_descriptor,
                                               &ethmac->tx_buffer[0][0],
                                               ETHMAC_TXBUFNB);

                        ETH_DMARxDescChainInit(ethmac->DMA_rx_descriptor,
                                               &ethmac->rx_buffer[0][0],
                                               ETHMAC_RXBUFNB);

                        if (__ETHMAC_CHECKSUM_BY_HARDWARE__ != 0) {
                                for (uint i = 0; i < ETHMAC_TXBUFNB; i++) {
                                        ETH_DMATxDescChecksumInsertionConfig(&ethmac->DMA_tx_descriptor[i],
                                                                             ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
                                }
                        }

                        for (uint i = 0; i < ETHMAC_RXBUFNB; i++) {
                                ETH_DMARxDescReceiveITConfig(&ethmac->DMA_rx_descriptor[i], ENABLE);
                        }

                        sys_sleep_ms(ETHMAC_PHY_CONFIG_DELAY);
                } else {
                        err = EIO;
                }

                finish:
                if (err != ESUCC) {
                        if (hdl->rx_data_ready)
                                sys_semaphore_destroy(hdl->rx_data_ready);

                        if (hdl->rx_access)
                                sys_mutex_destroy(hdl->rx_access);

                        if (hdl->tx_access)
                                sys_mutex_destroy(hdl->tx_access);

                        if (device_handle)
                                sys_free(device_handle);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Release device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_RELEASE(ETHMAC, void *device_handle)
{
        struct ethmac *hdl = device_handle;

        int err = sys_device_lock(&hdl->dev_lock);
        if (!err) {
                ETH_DeInit();
                NVIC_DisableIRQ(ETH_IRQn);
                CLEAR_BIT(RCC->AHBxENR, RCC_AHBxENR_ETHMACRXEN
                                      | RCC_AHBxENR_ETHMACTXEN
                                      | RCC_AHBxENR_ETHMACEN);
                sys_semaphore_destroy(hdl->rx_data_ready);
                sys_mutex_destroy(hdl->rx_access);
                sys_mutex_destroy(hdl->tx_access);
                sys_free(&device_handle);
                ethmac = NULL;
        }

        return err;
}

//==============================================================================
/**
 * @brief Open device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           flags                  file operation flags (O_RDONLY, O_WRONLY, O_RDWR)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_OPEN(ETHMAC, void *device_handle, u32_t flags)
{
        UNUSED_ARG1(flags);

        struct ethmac *hdl = device_handle;

        return sys_device_lock(&hdl->dev_lock);
}

//==============================================================================
/**
 * @brief Close device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           force                  device force close (true)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_CLOSE(ETHMAC, void *device_handle, bool force)
{
        struct ethmac *hdl = device_handle;

        int err = sys_device_get_access(&hdl->dev_lock);

        if (!err) {
                err = sys_device_unlock(&hdl->dev_lock, force);
        }

        return err;
}

//==============================================================================
/**
 * @brief Write data to device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *wrcnt                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_WRITE(ETHMAC,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG1(fpos);

        struct ethmac *hdl = device_handle;

        if (is_Ethernet_started()) {

                if (sys_mutex_lock(hdl->tx_access, fattr.non_blocking_wr ? 0 : MAX_DELAY_MS) == ESUCC) {

                        *wrcnt = 0;
                        int err;

                        while (count) {
                                while (is_buffer_owned_by_DMA(DMATxDescToSet)) {

                                        if (fattr.non_blocking_wr) {
                                                err = EAGAIN;
                                                goto exit;
                                        }

                                        sys_sleep_ms(1);
                                }

                                size_t packet_size = min(count, ETH_MAX_PACKET_SIZE);

                                u8_t *buffer = get_buffer_address(DMATxDescToSet);
                                memcpy(buffer, src, packet_size);
                                send_packet(packet_size);

                                *wrcnt += packet_size;
                                count  -= packet_size;
                        }

                        err = ESUCC;

                        exit:
                        sys_mutex_unlock(hdl->tx_access);

                        return err;
                } else {
                        return EAGAIN;
                }

        } else {
                return EIO;
        }
}

//==============================================================================
/**
 * @brief Read data from device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *rdcnt                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_READ(ETHMAC,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fpos);

        struct ethmac *hdl = device_handle;

        if (is_Ethernet_started()) {
                if (count % ETH_MAX_PACKET_SIZE == 0) {

                        *rdcnt = 0;

                        if (sys_mutex_lock(hdl->rx_access, fattr.non_blocking_rd ? 0 : MAX_DELAY_MS) == ESUCC) {
                                size_t pkts = count / ETH_MAX_PACKET_SIZE;
                                while (pkts--) {
                                        size_t pkt_size = wait_for_packet(hdl, fattr.non_blocking_rd ? 0 : MAX_DELAY_MS);
                                        if (pkt_size) {
                                                u8_t *src = get_buffer_address(DMARxDescToGet);

                                                memcpy(dst, src, min(pkt_size, count));
                                                give_Rx_buffer_to_DMA();
                                                make_Rx_buffer_available();

                                                *rdcnt += pkt_size;
                                        } else {
                                                break;
                                        }
                                }

                                sys_mutex_unlock(hdl->rx_access);

                                return ESUCC;
                        } else {
                                return EAGAIN;
                        }
                } else {
                        return EINVAL;
                }
        } else {
                return EIO;
        }
}

//==============================================================================
/**
 * @brief IO control
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_IOCTL(ETHMAC, void *device_handle, int request, void *arg)
{
        struct ethmac *hdl = device_handle;

        int err;

        switch (request) {
        case IOCTL_ETHMAC__WAIT_FOR_PACKET:

                if (arg) {
                        ETHMAC_packet_wait_t *pw = cast(ETHMAC_packet_wait_t*, arg);
                        pw->pkt_size = wait_for_packet(hdl, pw->timeout);
                        return ESUCC;
                } else {
                        return EINVAL;
                }
                break;

        case IOCTL_ETHMAC__SET_MAC_ADDR:
                if (arg) {
                        ETH_MACAddressConfig(ETH_MAC_Address0, arg);
                        return ESUCC;
                } else {
                        return EINVAL;
                }
                break;

        case IOCTL_ETHMAC__GET_MAC_ADDR:
                if (arg) {
                        u8_t *MAC = arg;

                        MAC[5] = ETH->MACA0HR >> 8;
                        MAC[4] = ETH->MACA0HR;
                        MAC[3] = ETH->MACA0LR >> 24;
                        MAC[2] = ETH->MACA0LR >> 16;
                        MAC[1] = ETH->MACA0LR >> 8;
                        MAC[0] = ETH->MACA0LR;

                        return ESUCC;
                } else {
                        return EINVAL;
                }
                break;

        case IOCTL_ETHMAC__SEND_PACKET:
                if (arg) {
                        if (sys_mutex_lock(hdl->tx_access, MAX_DELAY_MS) == ESUCC) {
                                ETHMAC_packet_t *pkt = arg;

                                while (is_buffer_owned_by_DMA(DMATxDescToSet)) {
                                        sys_sleep_ms(1);
                                }

                                if (  !is_buffer_owned_by_DMA(DMATxDescToSet)
                                   && pkt->payload
                                   && pkt->payload_size <= ETH_MAX_PACKET_SIZE) {

                                        u8_t *buffer = get_buffer_address(DMATxDescToSet);

                                        memcpy(buffer, pkt->payload, pkt->payload_size);

                                        send_packet(pkt->payload_size);

                                        err = ESUCC;
                                } else {
                                        printk("ETH: payload size too big or NULL");
                                        err = EAGAIN;
                                }

                                sys_mutex_unlock(hdl->tx_access);
                        } else {
                                err = EAGAIN;
                        }
                } else {
                        err = EINVAL;
                }
                break;

        case IOCTL_ETHMAC__RECEIVE_PACKET:
                if (arg) {
                        if (sys_mutex_lock(hdl->rx_access, MAX_DELAY_MS) == ESUCC) {
                                ETHMAC_packet_t *pkt = arg;

                                if (  pkt->payload
                                   && pkt->payload_size <= ETH_MAX_PACKET_SIZE) {

                                        // TEST check solution in tests
                                        while (is_buffer_owned_by_DMA(DMARxDescToGet)) {
                                                sys_sleep_ms(1);
                                        }

                                        if (ETH_GetRxPktSize(DMARxDescToGet) > 0) {

                                                u8_t *buffer = get_buffer_address(DMARxDescToGet);

                                                memcpy(pkt->payload, buffer, pkt->payload_size);

                                                give_Rx_buffer_to_DMA();

                                                err = ESUCC;
                                        } else {
                                                printk("ETH: received empty packet!");
                                                give_Rx_buffer_to_DMA();

                                                err = EIO;
                                        }

                                        make_Rx_buffer_available();

                                } else {
                                        err = EINVAL;
                                }

                                sys_mutex_unlock(hdl->rx_access);
                        } else {
                                err = EAGAIN;
                        }
                } else {
                        err = EINVAL;
                }
                break;

        case IOCTL_ETHMAC__ETHERNET_START:
                ETH_Start();
                return ESUCC;

        case IOCTL_ETHMAC__ETHERNET_STOP:
                ETH_Stop();
                return ESUCC;

        case IOCTL_ETHMAC__GET_LINK_STATUS:
                if (arg) {
                        ETHMAC_link_status_t *linkstat = cast(ETHMAC_link_status_t*, arg);

                        if (ETH_ReadPHYRegister(ETHMAC_PHY_ADDRESS, PHY_BSR) & PHY_BSR_LINK_STATUS) {
                                *linkstat = ETHMAC_LINK_STATUS__CONNECTED;
                        } else {
                                *linkstat = ETHMAC_LINK_STATUS__DISCONNECTED;
                        }

                        return ESUCC;
                } else {
                        return EINVAL;
                }

        default:
                return EBADRQC;
        }

        return err;
}

//==============================================================================
/**
 * @brief Flush device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_FLUSH(ETHMAC, void *device_handle)
{
        UNUSED_ARG1(device_handle);

        if (is_Ethernet_started()) {
                ETH_FlushTransmitFIFO();
        }

        return ESUCC;
}

//==============================================================================
/**
 * @brief Device information
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *device_stat            device status
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_STAT(ETHMAC, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG1(device_handle);

        device_stat->st_size  = 0;
        device_stat->st_major = 0;
        device_stat->st_minor = 0;

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function check if ethernet interface is started
 * @param  None
 * @return If started then true is returned, otherwise false
 */
//==============================================================================
static bool is_Ethernet_started(void)
{
        return (ETH->MACCR & ETH_MACCR_TE) && (ETH->MACCR & ETH_MACCR_RE);
}

//==============================================================================
/**
 * @brief  Send packet from current Tx buffer
 * @param  size         packet size
 * @return None
 */
//==============================================================================
static void send_packet(size_t size)
{
        sys_critical_section_begin();

        /* Setting the Frame Length: bits[12:0] */
        DMATxDescToSet->ControlBufferSize = (size & ETH_DMATxDesc_TBS1);

        /* Setting the last segment and first segment bits (in this case a frame is transmitted in one descriptor) */
        DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;

        /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
        DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;

        /* When Tx Buffer unavailable flag is set: clear it and resume transmission */
        if (ETH->DMASR & ETH_DMASR_TBUS) {
                /* Clear TBUS ETHERNET DMA flag */
                ETH->DMASR = ETH_DMASR_TBUS;

                /* Resume DMA transmission*/
                ETH->DMATPDR = 0;
        }

        /* Update the ETHERNET DMA global Tx descriptor with next Tx decriptor */
        /* Chained Mode */
        /* Selects the next DMA Tx descriptor list for next buffer to send */
        DMATxDescToSet = cast(ETH_DMADESCTypeDef*, DMATxDescToSet->Buffer2NextDescAddr);

        sys_critical_section_end();
}

//==============================================================================
/**
 * @brief  Function waits for a packet and return a size of recived packet
 * @param  hdl          driver context
 * @param  timeout      packet wait timeout
 * @return Size of received packet
 */
//==============================================================================
static size_t wait_for_packet(struct ethmac *hdl, uint32_t timeout)
{
        bool is_owned_by_DMA = is_buffer_owned_by_DMA(DMARxDescToGet);
        sys_semaphore_wait(hdl->rx_data_ready, is_owned_by_DMA ? timeout : 0);
        return ETH_GetRxPktSize(DMARxDescToGet);
}

//==============================================================================
/**
 * @brief  Give a current read buffer to the DMA controller
 * @param  None
 * @return None
 */
//==============================================================================
static void give_Rx_buffer_to_DMA(void)
{
        sys_critical_section_begin();

        /* Set Own bit of the Rx descriptor Status: gives the buffer back to ETHERNET DMA */
        DMARxDescToGet->Status = ETH_DMARxDesc_OWN;

        /* Update the ETHERNET DMA global Rx descriptor with next Rx descriptor */
        /* Selects the next DMA Rx descriptor list for next buffer to read */
        DMARxDescToGet = cast(ETH_DMADESCTypeDef*, DMARxDescToGet->Buffer2NextDescAddr);

        sys_critical_section_end();
}

//==============================================================================
/**
 * @brief  Function checks if buffer is owned by the DMA controller
 * @param  DMA_descriptor       DMA descriptor (Rx or Tx)
 * @return If buffer is owned the true is returned, otherwise false
 */
//==============================================================================
static bool is_buffer_owned_by_DMA(ETH_DMADESCTypeDef *DMA_descriptor)
{
        return DMA_descriptor->Status & ETH_DMARxDesc_OWN; // ETH_DMATxDesc_OWN
}

//==============================================================================
/**
 * @brief  Function makes Rx buffer available for the DMA controller and resume packet reception
 * @param  None
 * @return None
 */
//==============================================================================
static void make_Rx_buffer_available(void)
{
        sys_critical_section_begin();

        if (ETH->DMASR & ETH_DMASR_RBUS) {
                ETH_DMAClearFlag(ETH_DMA_FLAG_RBU);
                ETH_ResumeDMAReception();
        }

        sys_critical_section_end();
}

//==============================================================================
/**
 * @brief  Function returns buffer address of a selected DMA descriptor
 * @param  DMA_descriptor       DMA descriptor
 * @return Buffer address of selected DMA
 */
//==============================================================================
static u8_t *get_buffer_address(ETH_DMADESCTypeDef *DMA_descriptor)
{
        return cast(u8_t*, DMA_descriptor->Buffer1Addr);
}

//==============================================================================
/**
 * @brief  Function get speed and duplex from PHY
 * @param  PHYAddress           PHY address
 * @param  ETH_InitStruct       ETH init structure
 * @return Buffer address of selected DMA
 */
//==============================================================================
void ETH_EXTERN_GetSpeedAndDuplex(uint32_t PHYAddress, ETH_InitTypeDef *ETH_InitStruct)
{
        UNUSED_ARG2(PHYAddress, ETH_InitStruct);
        // empty
}

//==============================================================================
/**
 * @brief  Ethernet interrupt
 * @param  None
 * @return None
 */
//==============================================================================
void ETH_IRQHandler(void)
{
        if (ETH_GetDMAFlagStatus(ETH_DMA_FLAG_R)) {

                bool woken = false;
                sys_semaphore_signal_from_ISR(ethmac->rx_data_ready, &woken);

                ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS | ETH_DMA_IT_R);

                sys_thread_yield_from_ISR(woken);
        }
}

/*==============================================================================
  End of file
==============================================================================*/
