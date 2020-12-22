/*=========================================================================*//**
@file    eth.c

@author  Daniel Zorychta

@brief   This driver support Ethernet interface.

@note    Copyright (C) 2020 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "eth_ioctl.h"
#include "stm32f4xx_hal_eth.h"

#if defined(ARCH_stm32f1)
#include "stm32f10x.h"
#elif defined(ARCH_stm32f4)
#include "stm32f4xx.h"
#elif defined(ARCH_stm32f7)
#include "stm32f7xx.h"
// TODO cache clear/invalidate
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
#elif defined(ARCH_stm32f7)
#define AHBxENR                  AHB1ENR
#define RCC_AHBxENR_ETHMACRXEN   RCC_AHB1ENR_ETHMACRXEN
#define RCC_AHBxENR_ETHMACTXEN   RCC_AHB1ENR_ETHMACTXEN
#define RCC_AHBxENR_ETHMACEN     RCC_AHB1ENR_ETHMACEN
#endif

/*==============================================================================
  Local object types
==============================================================================*/
struct eth {
        dev_lock_t          dev_lock;
        ETH_HandleTypeDef   eth;
        ETH_DMADescTypeDef  DMARxDscrTab[__ETH_RXBUFNB__];
        ETH_DMADescTypeDef  DMATxDscrTab[__ETH_TXBUFNB__];
        uint8_t             Rx_Buff[__ETH_RXBUFNB__][ETH_RX_BUF_SIZE];
        uint8_t             Tx_Buff[__ETH_TXBUFNB__][ETH_TX_BUF_SIZE];
        sem_t               *semaphore;
        uint8_t             MAC_addr[6];
        bool                irq_yield;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int packet_receive(struct eth *hdl, ETH_packet_t *pkt);
static int packet_send(struct eth *hdl, ETH_packet_t *pkt);

/*==============================================================================
  Local objects
==============================================================================*/
MODULE_NAME(ETH);

static struct eth *eth;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

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
API_MOD_INIT(ETH, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        UNUSED_ARG1(config);

        if (major != 0 || minor != 0) {
                return ENODEV;
        }

        int err = sys_zalloc(sizeof(struct eth), device_handle);
        if (!err) {
                struct eth *hdl = *device_handle;

                hdl->MAC_addr[0] = 0xC2; // FIXME adres musi być już załadowany, albo przerobić init na konfigurację
                hdl->MAC_addr[1] = 0x70;
                hdl->MAC_addr[2] = 0x50;
                hdl->MAC_addr[3] = 0xFF;
                hdl->MAC_addr[4] = 0xFF;
                hdl->MAC_addr[5] = 0x78;

                hdl->eth.Instance = ETH;
                hdl->eth.Init.MACAddr = hdl->MAC_addr; // FIXME adres musi być już załadowany
                hdl->eth.Init.AutoNegotiation = __ETH_PHY_AUTONEGOTIATION__;
                hdl->eth.Init.Speed = __ETH_SPEED__;
                hdl->eth.Init.DuplexMode = ETH_MODE_FULLDUPLEX;
                hdl->eth.Init.RxMode = ETH_RXINTERRUPT_MODE;
                hdl->eth.Init.ChecksumMode = __ETH_CHECKSUM_BY_HARDWARE__;
                hdl->eth.Init.PhyAddress = __ETH_PHY_ADDRESS__;

                err = HAL_ETH_Init(&hdl->eth);
                if (!err) {
                        /* Initialize Tx Descriptors list: Chain Mode */
                        HAL_ETH_DMATxDescListInit(&hdl->eth, hdl->DMATxDscrTab,
                                                  &hdl->Tx_Buff[0][0], __ETH_TXBUFNB__);

                        /* Initialize Rx Descriptors list: Chain Mode  */
                        HAL_ETH_DMARxDescListInit(&hdl->eth, hdl->DMARxDscrTab,
                                                  &hdl->Rx_Buff[0][0], __ETH_RXBUFNB__);

                        err = sys_semaphore_create(1000, 0, &hdl->semaphore);
                        if (!err) {
                                eth = hdl;
                                HAL_ETH_Start(&hdl->eth);
                        }
                }

                if (err) {
                        if (hdl->semaphore) {
                                sys_semaphore_destroy(hdl->semaphore);
                        }

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
API_MOD_RELEASE(ETH, void *device_handle)
{
        struct eth *hdl = device_handle;

        int err = sys_device_lock(&hdl->dev_lock);
        if (!err) {
                HAL_ETH_Stop(&hdl->eth);
                HAL_ETH_DeInit(&hdl->eth);
                NVIC_DisableIRQ(ETH_IRQn);
                CLEAR_BIT(RCC->AHBxENR, RCC_AHBxENR_ETHMACRXEN
                                      | RCC_AHBxENR_ETHMACTXEN
                                      | RCC_AHBxENR_ETHMACEN);

                sys_free(&device_handle);
                eth = NULL;
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
API_MOD_OPEN(ETH, void *device_handle, u32_t flags)
{
        UNUSED_ARG1(flags);

        struct eth *hdl = device_handle;

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
API_MOD_CLOSE(ETH, void *device_handle, bool force)
{
        struct eth *hdl = device_handle;

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
API_MOD_WRITE(ETH,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG1(fpos);

        struct eth *hdl = device_handle;

        return EIO;
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
API_MOD_READ(ETH,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fpos);

        struct eth *hdl = device_handle;

        return EIO;
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
API_MOD_IOCTL(ETH, void *device_handle, int request, void *arg)
{
        struct eth *hdl = device_handle;

        int err = EINVAL;

        switch (request) {
        case IOCTL_ETH__WAIT_FOR_PACKET:
                if (arg) {
                }
                break;

        case IOCTL_ETH__SET_MAC_ADDR:
                if (arg) {
                }
                break;

        case IOCTL_ETH__GET_MAC_ADDR:
                if (arg) {
                        memcpy(arg, hdl->MAC_addr, sizeof(hdl->MAC_addr));
                        return ESUCC;
                } else {
                        return EINVAL;
                }
                break;

        case IOCTL_ETH__SEND_PACKET:
                if (arg) {
                        ETH_packet_t *pkt = arg;
                        err = packet_send(hdl, pkt);
                }
                break;

        case IOCTL_ETH__RECEIVE_PACKET:
                if (arg) {
                        ETH_packet_t *pkt = arg;

                        err = sys_semaphore_wait(hdl->semaphore, MAX_DELAY_MS);
                        if (!err) {
                                err = packet_receive(hdl, pkt);
                        }
                }
                break;

        case IOCTL_ETH__ETHERNET_START:
//                HAL_ETH_Start(&hdl->eth);
                return ESUCC;

        case IOCTL_ETH__ETHERNET_STOP:
//                HAL_ETH_Stop(&hdl->eth);
                return ESUCC;

        case IOCTL_ETH__GET_LINK_STATUS:
                if (arg) {
                        ETH_link_status_t *linkstat = cast(ETH_link_status_t*, arg);

                        u32_t reg;
                        err = HAL_ETH_ReadPHYRegister(__ETH_PHY_ADDRESS__, PHY_BSR, &reg);
                        if (reg & PHY_BSR_LINK_STATUS) {
                                *linkstat = ETH_LINK_STATUS__CONNECTED;
                        } else {
                                *linkstat = ETH_LINK_STATUS__DISCONNECTED;
                        }

                        return ESUCC;
                } else {
                        return EINVAL;
                }
                break;

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
API_MOD_FLUSH(ETH, void *device_handle)
{
        UNUSED_ARG1(device_handle);

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
API_MOD_STAT(ETH, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG1(device_handle);

        device_stat->st_size  = 0;
        device_stat->st_major = 0;
        device_stat->st_minor = 0;

        return ESUCC;
}

//==============================================================================
/**
 * @brief
 *
 * @param  ?
 *
 * @return ?
 */
//==============================================================================
static int packet_send(struct eth *hdl, ETH_packet_t *pkt)
{
        int err;
        __IO ETH_DMADescTypeDef *DmaTxDesc = hdl->eth.TxDesc;
        uint8_t *buffer = (uint8_t*) (DmaTxDesc->Buffer1Addr);

        /* Is this buffer available? If not, goto error */
//        if ((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t) RESET) {
//                err = EBUSY;
//                goto error;
//        }
        // FIXME timeout
        while ((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t) RESET) {
                sys_sleep_ms(1);
        }

        /* Copy the remaining bytes */
        memcpy(buffer, pkt->payload, pkt->payload_size);

        /* Prepare transmit descriptors to give to DMA */
        err = HAL_ETH_TransmitFrame(&hdl->eth, pkt->payload_size);

        /* When Transmit Underflow flag is set, clear it and issue a Transmit Poll Demand to resume transmission */
        if ((hdl->eth.Instance->DMASR & ETH_DMASR_TUS) != (uint32_t) RESET) {
                /* Clear TUS ETHERNET DMA flag */
                hdl->eth.Instance->DMASR = ETH_DMASR_TUS;

                /* Resume DMA transmission*/
                hdl->eth.Instance->DMATPDR = 0;
        }

        return err;
}

//==============================================================================
/**
 * @brief
 *
 * @param  ?
 *
 * @return ?
 */
//==============================================================================
static int packet_receive(struct eth *hdl, ETH_packet_t *pkt)
{
        /* get received frame */
        if (HAL_ETH_GetReceivedFrame_IT(&hdl->eth) != HAL_OK) {
                printk("ETH: frame receive error");
                return EIO;
        }

        /* Obtain the size of the packet and put it into the "len" variable. */
        uint16_t len = hdl->eth.RxFrameInfos.length;
        uint8_t *buffer = (uint8_t*)hdl->eth.RxFrameInfos.buffer;

        if (len > 0) {
                pkt->payload_size = len;
                memcpy(pkt->payload, buffer, len);
        }

        /* Release descriptors to DMA */
        __IO ETH_DMADescTypeDef *dmarxdesc = hdl->eth.RxFrameInfos.FSRxDesc;

        /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
        for (u32_t i = 0; i < hdl->eth.RxFrameInfos.SegCount; i++) {
                dmarxdesc->Status |= ETH_DMARXDESC_OWN;
                dmarxdesc = (ETH_DMADescTypeDef*)(dmarxdesc->Buffer2NextDescAddr);
        }

        /* Clear Segment_Count */
        hdl->eth.RxFrameInfos.SegCount = 0;

        /* When Rx Buffer unavailable flag is set: clear it and resume reception */
        if ((hdl->eth.Instance->DMASR & ETH_DMASR_RBUS) != (uint32_t) RESET) {
                /* Clear RBUS ETHERNET DMA flag */
                hdl->eth.Instance->DMASR = ETH_DMASR_RBUS;
                /* Resume DMA reception */
                hdl->eth.Instance->DMARPDR = 0;
        }

        return 0;
}

//==============================================================================
/**
 * @brief
 *
 * @param  ?
 *
 * @return ?
 */
//==============================================================================
void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
        UNUSED_ARG1(heth);

        /* Enable the Ethernet global Interrupt */
        NVIC_SetPriority(ETH_IRQn, _CPU_IRQ_SAFE_PRIORITY_);
        NVIC_EnableIRQ(ETH_IRQn);

        SET_BIT(RCC->AHBxENR, RCC_AHBxENR_ETHMACEN);
        volatile u32_t tmp = READ_BIT(RCC->AHBxENR, RCC_AHBxENR_ETHMACEN);

        SET_BIT(RCC->AHBxENR, RCC_AHBxENR_ETHMACTXEN);
        tmp = READ_BIT(RCC->AHBxENR, RCC_AHBxENR_ETHMACTXEN);

        SET_BIT(RCC->AHBxENR, RCC_AHBxENR_ETHMACRXEN);
        tmp = READ_BIT(RCC->AHBxENR, RCC_AHBxENR_ETHMACRXEN);

        UNUSED_ARG1(tmp);
}

//==============================================================================
/**
 * @brief
 *
 * @param  ?
 *
 * @return ?
 */
//==============================================================================
void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth)
{
        UNUSED_ARG1(heth);
        sys_semaphore_signal_from_ISR(eth->semaphore, &eth->irq_yield);
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
        HAL_ETH_IRQHandler(&eth->eth);
        sys_thread_yield_from_ISR(eth->irq_yield);
}

/*==============================================================================
  End of file
==============================================================================*/
