/*=========================================================================*//**
@file    ethmac.c

@author  Daniel Zorychta

@brief   This driver support Ethernet interface.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the  Free Software  Foundation;  either version 2 of the License, or
         any later version.

         This  program  is  distributed  in the hope that  it will be useful,
         but  WITHOUT  ANY  WARRANTY;  without  even  the implied warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         You  should  have received a copy  of the GNU General Public License
         along  with  this  program;  if not,  write  to  the  Free  Software
         Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


*//*==========================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include "core/module.h"
#include <dnx/thread.h>
#include <dnx/misc.h>
#include <dnx/timer.h>
#include "ethmac_cfg.h"
#include "ethmac_def.h"
#include "stm32f10x.h"
#include "stm32_eth_driver/stm32_eth.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define NUMBER_OF_RX_BUFFERS    3
#define NUMBER_OF_TX_BUFFERS    2
#define INIT_TIMEOUT            2000
#define PHY_BSR_LINK_STATUS     (1 << 2)

/*==============================================================================
  Local object types
==============================================================================*/
struct ethmac {
        sem_t              *rx_data_ready;
        mutex_t            *rx_access;
        mutex_t            *tx_access;
        dev_lock_t         *dev_lock;
        ETH_DMADESCTypeDef  DMA_tx_descriptor[NUMBER_OF_TX_BUFFERS];
        ETH_DMADESCTypeDef  DMA_rx_descriptor[NUMBER_OF_RX_BUFFERS];
        u8_t                tx_buffer[NUMBER_OF_TX_BUFFERS][ETH_MAX_PACKET_SIZE];
        u8_t                rx_buffer[NUMBER_OF_RX_BUFFERS][ETH_MAX_PACKET_SIZE];
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static bool   is_Ethernet_started       ();
static void   send_packet               (size_t size);
static size_t wait_for_packet           (struct ethmac *hdl, uint timeout);
static void   give_Rx_buffer_to_DMA     ();
static bool   is_buffer_owned_by_DMA    (ETH_DMADESCTypeDef *DMA_descriptor);
static void   make_Rx_buffer_available  ();
static u8_t  *get_buffer_address        (ETH_DMADESCTypeDef *DMA_descriptor);
static void   ETH_Stop                  ();

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
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_INIT(ETHMAC, void **device_handle, u8_t major, u8_t minor)
{
        UNUSED_ARG(major);
        UNUSED_ARG(minor);

        struct ethmac *hdl          = calloc(1, sizeof(struct ethmac));
        sem_t         *rx_ready_sem = semaphore_new(1, 0);
        mutex_t       *rx_mtx       = mutex_new(MUTEX_NORMAL);
        mutex_t       *tx_mtx       = mutex_new(MUTEX_NORMAL);
        if (hdl && rx_ready_sem && rx_mtx && tx_mtx) {
                hdl->rx_data_ready = rx_ready_sem;
                hdl->rx_access     = rx_mtx;
                hdl->tx_access     = tx_mtx;

                // reset Ethernet
                ETH_DeInit();
                ETH_SoftwareReset();

                timer_t timeout = timer_reset();
                while (  ETH_GetSoftwareResetStatus() == SET
                      && timer_is_not_expired(timeout, INIT_TIMEOUT));

                if (timer_is_expired(timeout, INIT_TIMEOUT)) {
                        errno = ETIME;
                        goto error;
                }

                // enable interrupts
                SET_BIT(RCC->AHBENR, RCC_AHBENR_ETHMACRXEN | RCC_AHBENR_ETHMACTXEN | RCC_AHBENR_ETHMACEN);

                NVIC_EnableIRQ(ETH_IRQn);
                NVIC_SetPriority(ETH_IRQn, ETHMAC_IRQ_PRIORITY);

                // initialize Ethernet
                ETH_InitTypeDef ETH_InitStructure;
                ETH_StructInit(&ETH_InitStructure);

                /* MAC configuration */
                /*
                 * Ethernet DMA configuration
                 * When we use the Checksum offload feature, we need to enable the Store and Forward mode:
                 * the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can
                 * insert/verify the checksum, if the checksum is OK the DMA can handle the frame otherwise
                 * the frame is dropped
                 */
                ETH_InitStructure.ETH_AutoNegotiation             = ETH_AutoNegotiation_Enable;
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

                        ethmac         = hdl;
                        *device_handle = hdl;

                        ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);

                        ETH_DMATxDescChainInit(ethmac->DMA_tx_descriptor,
                                               &ethmac->tx_buffer[0][0],
                                               NUMBER_OF_TX_BUFFERS);

                        ETH_DMARxDescChainInit(ethmac->DMA_rx_descriptor,
                                               &ethmac->rx_buffer[0][0],
                                               NUMBER_OF_RX_BUFFERS);

                        if (__ETHMAC_CHECKSUM_BY_HARDWARE__ != 0) {
                                for (uint i = 0; i < NUMBER_OF_TX_BUFFERS; i++) {
                                        ETH_DMATxDescChecksumInsertionConfig(&ethmac->DMA_tx_descriptor[i],
                                                                             ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
                                }
                        }

                        for (uint i = 0; i < NUMBER_OF_RX_BUFFERS; i++) {
                                ETH_DMARxDescReceiveITConfig(&ethmac->DMA_rx_descriptor[i], ENABLE);
                        }

                        return STD_RET_OK;
                }
        }

        error:
        /* error occurred */
        if (hdl)
                free(hdl);

        if (rx_ready_sem)
                semaphore_delete(rx_ready_sem);

        if (rx_mtx)
                mutex_delete(rx_mtx);

        if (tx_mtx)
                mutex_delete(tx_mtx);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Release device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_RELEASE(ETHMAC, void *device_handle)
{
        struct ethmac *hdl    = device_handle;
        stdret_t       status = STD_RET_ERROR;

        critical_section_begin();

        if (_sys_device_is_unlocked(&hdl->dev_lock)) {
                ETH_DeInit();
                NVIC_DisableIRQ(ETH_IRQn);
                CLEAR_BIT(RCC->AHBENR, RCC_AHBENR_ETHMACRXEN | RCC_AHBENR_ETHMACTXEN | RCC_AHBENR_ETHMACEN);
                ethmac = NULL;
                free(hdl);

                status = STD_RET_OK;
        } else {
                errno = EBUSY;
        }

        critical_section_end();

        return status;
}

//==============================================================================
/**
 * @brief Open device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           flags                  file operation flags (O_RDONLY, O_WRONLY, O_RDWR)
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_OPEN(ETHMAC, void *device_handle, vfs_open_flags_t flags)
{
        UNUSED_ARG(flags);

        struct ethmac *hdl = device_handle;

        if (_sys_device_lock(&hdl->dev_lock)) {
                return STD_RET_OK;
        } else {
                errno = EBUSY;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Close device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           force                  device force close (true)
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_CLOSE(ETHMAC, void *device_handle, bool force)
{
        struct ethmac *hdl = device_handle;

        if (_sys_device_is_access_granted(&hdl->dev_lock) || force) {
                _sys_device_unlock(&hdl->dev_lock, force);
                return STD_RET_OK;
        } else {
                errno = EBUSY;
                return STD_RET_ERROR;
        }
}

//==============================================================================
/**
 * @brief Write data to device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ][out]     *fpos                   file position
 * @param[in ]           fattr                  file attributes
 *
 * @return number of written bytes, -1 if error
 */
//==============================================================================
API_MOD_WRITE(ETHMAC, void *device_handle, const u8_t *src, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(fpos);

        struct ethmac *hdl = device_handle;

        ssize_t n = -1;

        if (is_Ethernet_started()) {

                n = 0;

                if (mutex_lock(hdl->tx_access, fattr.non_blocking_wr ? 0 : MAX_DELAY_MS)) {
                        while (count) {
                                while (is_buffer_owned_by_DMA(DMATxDescToSet)) {

                                        if (fattr.non_blocking_wr)
                                                goto exit;

                                        sleep_ms(1);
                                }

                                size_t packet_size = min(count, ETH_MAX_PACKET_SIZE);

                                u8_t *buffer = get_buffer_address(DMATxDescToSet);
                                memcpy(buffer, src, packet_size);
                                send_packet(count);

                                n     += packet_size;
                                count -= packet_size;
                        }

                        mutex_unlock(hdl->tx_access);
                }

        } else {
                errno = EPERM;
        }

        exit:
        return n;
}

//==============================================================================
/**
 * @brief Read data from device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ][out]     *fpos                   file position
 * @param[in ]           fattr                  file attributes
 *
 * @return number of read bytes, -1 if error
 */
//==============================================================================
API_MOD_READ(ETHMAC, void *device_handle, u8_t *dst, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(fpos);

        struct ethmac *hdl = device_handle;

        ssize_t n = -1;

        if (count % ETH_MAX_PACKET_SIZE == 0 && is_Ethernet_started()) {

                n = 0;

                if (mutex_lock(hdl->rx_access, fattr.non_blocking_rd ? 0 : MAX_DELAY_MS)) {
                        size_t pkts = count / ETH_MAX_PACKET_SIZE;
                        while (pkts--) {
                                size_t pkt_size = wait_for_packet(hdl, fattr.non_blocking_rd ? 0 : MAX_DELAY_MS);
                                if (pkt_size) {
                                        u8_t *src = get_buffer_address(DMARxDescToGet);

                                        memcpy(dst, src, min(pkt_size, count));
                                        give_Rx_buffer_to_DMA();
                                        make_Rx_buffer_available();

                                        n += pkt_size;
                                } else {
                                        break;
                                }
                        }

                        mutex_unlock(hdl->rx_access);
                }
        } else {
                errno = EPERM;
        }

        return n;
}

//==============================================================================
/**
 * @brief IO control
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_IOCTL(ETHMAC, void *device_handle, int request, void *arg)
{
        struct ethmac *hdl = device_handle;

        int status = STD_RET_ERROR;

        switch (request) {
        case IOCTL_ETHMAC__WAIT_FOR_PACKET:
                if (arg) {
                        return wait_for_packet(hdl, reinterpret_cast(int, arg));
                } else {
                        errno = EINVAL;
                }
                break;

        case IOCTL_ETHMAC__SET_MAC_ADDR:
                if (arg) {
                        ETH_MACAddressConfig(ETH_MAC_Address0, arg);
                        return STD_RET_OK;
                } else {
                        errno = EINVAL;
                }
                break;

        case IOCTL_ETHMAC__SEND_PACKET_FROM_CHAIN:
                if (arg) {
                        if (mutex_lock(hdl->tx_access, MAX_DELAY_MS)) {
                                ethmac_packet_chain_t *pkt = reinterpret_cast(ethmac_packet_chain_t*, arg);

                                while (is_buffer_owned_by_DMA(DMATxDescToSet)) {
                                        sleep_ms(1);
                                }

                                if (  !is_buffer_owned_by_DMA(DMATxDescToSet)
                                   && pkt->payload
                                   && pkt->payload_size
                                   && pkt->total_size > 0
                                   && pkt->total_size <= ETH_MAX_PACKET_SIZE) {

                                        u8_t  *buffer = get_buffer_address(DMATxDescToSet);
                                        size_t offset = 0;

                                        for (ethmac_packet_chain_t *p = pkt; p != NULL; p = p->next) {
                                                memcpy(&buffer[offset], p->payload, p->payload_size);
                                                offset += p->payload_size;
                                        }

                                        send_packet(pkt->total_size);

                                        status = STD_RET_OK;
                                }

                                mutex_unlock(hdl->tx_access);
                        }
                }

                errno = EINVAL;
                break;

        case IOCTL_ETHMAC__RECEIVE_PACKET_TO_CHAIN:
                if (arg) {
                        if (mutex_lock(hdl->rx_access, MAX_DELAY_MS)) {
                                ethmac_packet_chain_t *pkt = reinterpret_cast(ethmac_packet_chain_t*, arg);

                                if (  pkt->payload
                                   && pkt->payload_size
                                   && pkt->total_size > 0
                                   && pkt->total_size <= ETH_MAX_PACKET_SIZE) {

                                        int n;
                                        if (is_buffer_owned_by_DMA(DMARxDescToGet)) {
                                                n = 0;

                                        } else if (ETH_GetRxPktSize() > 0) {

                                                u8_t  *buffer = get_buffer_address(DMARxDescToGet);
                                                size_t offset = 0;

                                                for (ethmac_packet_chain_t *p = pkt; p != NULL; p = p->next) {
                                                        memcpy(p->payload, &buffer[offset], p->payload_size);
                                                        offset += p->payload_size;
                                                }

                                                give_Rx_buffer_to_DMA();
                                                n = offset;
                                        } else {
                                                give_Rx_buffer_to_DMA();
                                                n = 0;
                                        }

                                        make_Rx_buffer_available();

                                        status = n;
                                }

                                mutex_unlock(hdl->rx_access);
                        }
                }

                errno = EINVAL;
                break;

        case IOCTL_ETHMAC__ETHERNET_START:
                ETH_Start();
                return STD_RET_OK;

        case IOCTL_ETHMAC__ETHERNET_STOP:
                ETH_Stop();
                return STD_RET_OK;

        case IOCTL_ETHMAC__GET_LINK_STATUS:
                if (ETH_ReadPHYRegister(ETHMAC_PHY_ADDRESS, PHY_BSR) & PHY_BSR_LINK_STATUS) {
                        return ETHMAC_LINK_STATUS_CONNECTED;
                } else {
                        return ETHMAC_LINK_STATUS_DISCONNECTED;
                }

        default:
                errno = EBADRQC;
        }

        return status;
}

//==============================================================================
/**
 * @brief Flush device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_FLUSH(ETHMAC, void *device_handle)
{
        UNUSED_ARG(device_handle);

        if (is_Ethernet_started()) {
                ETH_FlushTransmitFIFO();
        }

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Device information
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *device_stat            device status
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_STAT(ETHMAC, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG(device_handle);

        device_stat->st_size  = 0;
        device_stat->st_major = _ETHMAC_MAJOR_NUMBER;
        device_stat->st_minor = _ETHMAC_MINOR_NUMBER;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief  Function check if ethernet interface is started
 * @param  None
 * @return If started then true is returned, otherwise false
 */
//==============================================================================
static bool is_Ethernet_started()
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
        DMATxDescToSet = reinterpret_cast(ETH_DMADESCTypeDef*, DMATxDescToSet->Buffer2NextDescAddr);
}

//==============================================================================
/**
 * @brief  Function waits for a packet and return a size of recived packet
 * @param  hdl          driver context
 * @param  timeout      packet wait timeout
 * @return Size of received packet
 */
//==============================================================================
static size_t wait_for_packet(struct ethmac *hdl, uint timeout)
{
        size_t size = 0;

        if (!is_buffer_owned_by_DMA(DMARxDescToGet)) {
                semaphore_wait(hdl->rx_data_ready, 0);
                size = ETH_GetRxPktSize();

        } else {
                if (semaphore_wait(hdl->rx_data_ready, timeout)) {
                        if (!is_buffer_owned_by_DMA(DMARxDescToGet)) {
                                size = ETH_GetRxPktSize();
                        }
                }
        }

        return size;
}

//==============================================================================
/**
 * @brief  Give a current read buffer to the DMA controller
 * @param  None
 * @return None
 */
//==============================================================================
static void give_Rx_buffer_to_DMA()
{
        /* Set Own bit of the Rx descriptor Status: gives the buffer back to ETHERNET DMA */
        DMARxDescToGet->Status = ETH_DMARxDesc_OWN;

        /* Update the ETHERNET DMA global Rx descriptor with next Rx descriptor */
        /* Selects the next DMA Rx descriptor list for next buffer to read */
        DMARxDescToGet = reinterpret_cast(ETH_DMADESCTypeDef*, DMARxDescToGet->Buffer2NextDescAddr);
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
static void make_Rx_buffer_available()
{
        if (ETH->DMASR & ETH_DMASR_RBUS) {
                ETH_DMAClearFlag(ETH_DMA_FLAG_RBU);
                ETH_ResumeDMAReception();
        }
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
        return reinterpret_cast(u8_t*, DMA_descriptor->Buffer1Addr);
}

//==============================================================================
/**
 * @brief  Function stops Ethernet interface (opposition function to ETH_Start())
 * @param  None
 * @return None
 */
//==============================================================================
static void ETH_Stop()
{
        /* Enable transmit state machine of the MAC for transmission on the MII */
        ETH_MACTransmissionCmd(DISABLE);
        /* Flush Transmit FIFO */
        ETH_FlushTransmitFIFO();
        /* Enable receive state machine of the MAC for reception from the MII */
        ETH_MACReceptionCmd(DISABLE);

        /* Start DMA transmission */
        ETH_DMATransmissionCmd(DISABLE);
        /* Start DMA reception */
        ETH_DMAReceptionCmd(DISABLE);
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
                semaphore_signal_from_ISR(ethmac->rx_data_ready, &woken);

                ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS | ETH_DMA_IT_R);

                if (woken) {
                        task_yield_from_ISR();
                }
        }
}

/*==============================================================================
  End of file
==============================================================================*/
