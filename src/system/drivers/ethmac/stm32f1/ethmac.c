/*=========================================================================*//**
@file    ethmac.c

@author  Daniel Zorychta

@brief   This driver support Ethernet interface.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "ethmac_cfg.h"
#include "ethmac_def.h"
#include "stm32f10x.h"
#include "stm32_eth_driver/stm32_eth.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define ETH_DMARxDesc_FrameLengthShift          16

/*==============================================================================
  Local object types
==============================================================================*/
struct eth_mem {
        ETH_DMADESCTypeDef     *DMA_tx_descriptor_tab;
        ETH_DMADESCTypeDef     *DMA_rx_descriptor_tab;
        u8_t                    tx_buffer_count;
        u8_t                    rx_buffer_count;
        bool                    rx_data_ready;
        dev_lock_t              dev_lock;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
MODULE_NAME("ETHMAC");

static struct eth_mem *eth_mem;         /* required by IRQ */

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

        struct eth_mem *hdl = calloc(1, sizeof(struct eth_mem));
        if (!hdl) {
                return STD_RET_ERROR;
        }

        eth_mem        = hdl;
        *device_handle = hdl;

        SET_BIT(RCC->AHBENR, RCC_AHBENR_ETHMACRXEN | RCC_AHBENR_ETHMACTXEN | RCC_AHBENR_ETHMACEN);

        NVIC_EnableIRQ(ETH_IRQn);
        NVIC_SetPriority(ETH_IRQn, ETHMAC_IRQ_PRIORITY);

        return STD_RET_OK;
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
        struct eth_mem *hdl    = device_handle;
        stdret_t        status = STD_RET_ERROR;

        critical_section_begin();

        if (device_is_unlocked(&hdl->dev_lock)) {
                free(hdl);
                eth_mem = NULL;
                ETH_DeInit();
                ETH_SoftwareReset();
                while (ETH_GetSoftwareResetStatus() == SET);
                CLEAR_BIT(RCC->AHBENR, RCC_AHBENR_ETHMACRXEN | RCC_AHBENR_ETHMACTXEN | RCC_AHBENR_ETHMACEN);
                NVIC_DisableIRQ(ETH_IRQn);

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

        struct eth_mem *hdl = device_handle;

        if (device_lock(&hdl->dev_lock)) {
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
        struct eth_mem *hdl = device_handle;

        if (device_is_access_granted(&hdl->dev_lock) || force) {
                device_unlock(&hdl->dev_lock, force);
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
        UNUSED_ARG(device_handle);
        UNUSED_ARG(src);
        UNUSED_ARG(fpos);
        UNUSED_ARG(count);
        UNUSED_ARG(fattr);

        errno = EPERM;

        return 0;
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
        UNUSED_ARG(device_handle);
        UNUSED_ARG(dst);
        UNUSED_ARG(fpos);
        UNUSED_ARG(count);
        UNUSED_ARG(fattr);

        errno = EPERM;

        return 0;
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
        struct eth_mem *hdl = device_handle;

        if (device_is_access_granted(&hdl->dev_lock)) {
                switch (request) {
                case IOCTL_ETH__ETHERNET_INIT:
                        /* configure Ethernet */
                        ETH_DeInit();
                        ETH_SoftwareReset();
                        while (ETH_GetSoftwareResetStatus() == SET);

                        ETH_InitTypeDef ETH_InitStructure;
                        ETH_StructInit(&ETH_InitStructure);

                        /* MAC configuration */
                        ETH_InitStructure.ETH_AutoNegotiation          = ETH_AutoNegotiation_Enable;
                        ETH_InitStructure.ETH_LoopbackMode             = ETH_LoopbackMode_Disable;
                        ETH_InitStructure.ETH_RetryTransmission        = ETH_RetryTransmission_Disable;
                        ETH_InitStructure.ETH_AutomaticPadCRCStrip     = ETH_AutomaticPadCRCStrip_Disable;
                        ETH_InitStructure.ETH_ReceiveAll               = ETH_ReceiveAll_Disable;
                        ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
                        ETH_InitStructure.ETH_PromiscuousMode          = ETH_PromiscuousMode_Disable;
                        ETH_InitStructure.ETH_MulticastFramesFilter    = ETH_MulticastFramesFilter_Perfect;
                        ETH_InitStructure.ETH_UnicastFramesFilter      = ETH_UnicastFramesFilter_Perfect;
                        ETH_InitStructure.ETH_Speed                    = ETHMAC_SPEED;
                        if (ETHMAC_CHECKSUM_BY_HARDWARE != 0) {
                                ETH_InitStructure.ETH_ChecksumOffload  = ETH_ChecksumOffload_Enable;
                        }

                        /*
                         * Ethernet DMA configuration
                         * When we use the Checksum offload feature, we need to enable the Store and Forward mode:
                         * the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can
                         * insert/verify the checksum, if the checksum is OK the DMA can handle the frame otherwise
                         * the frame is dropped
                         */
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

                        if (ETH_Init(&ETH_InitStructure, ETHMAC_PHY_ADDRESS))
                        {
                              ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);
                              return STD_RET_OK;
                        }
                        errno = EIO;
                        break;

                case IOCTL_ETH__GET_RX_FLAG:
                        if (arg) {
                                *(bool *)arg = hdl->rx_data_ready;
                                return STD_RET_OK;
                        }
                        errno = EINVAL;
                        break;

                case IOCTL_ETH__CLEAR_RX_FLAG:
                        hdl->rx_data_ready = false;
                        return STD_RET_OK;

                case IOCTL_ETH__SET_MAC_ADR:
                        if (arg) {
                                u8_t *MAC = arg;
                                ETH_MACAddressConfig(ETH_MAC_Address0, MAC);
                                return STD_RET_OK;
                        }
                        errno = EINVAL;
                        break;

                case IOCTL_ETH__GET_RX_PACKET_SIZE:
                        if (arg) {
                                u32_t *packet_size = arg;
                                *packet_size = ETH_GetRxPktSize();
                                return STD_RET_OK;
                        }
                        errno = EINVAL;
                        break;

                case IOCTL_ETH__GET_RX_PACKET_CHAIN_MODE:
                        if (arg) {
                                struct ethmac_frame frame = {.length = 0, .buffer = NULL};

                                /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
                                if ((DMARxDescToGet->Status & ETH_DMARxDesc_OWN) != (u32) RESET) {
                                        frame.length = 0;

                                        if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32) RESET) {
                                                /* Clear RBUS ETHERNET DMA flag */
                                                ETH->DMASR = ETH_DMASR_RBUS;

                                                /* Resume DMA reception */
                                                ETH->DMARPDR = 0;
                                        }

                                        *(struct ethmac_frame *)arg = frame;
                                        return STD_RET_OK;
                                }

                                if (  ((DMARxDescToGet->Status & ETH_DMARxDesc_ES) == (u32) RESET)
                                   && ((DMARxDescToGet->Status & ETH_DMARxDesc_LS) != (u32) RESET)
                                   && ((DMARxDescToGet->Status & ETH_DMARxDesc_FS) != (u32) RESET) ) {

                                        /* Get the Frame Length of the received packet: substruct 4 bytes of the CRC */
                                        frame.length = ((DMARxDescToGet->Status & ETH_DMARxDesc_FL) >> ETH_DMARxDesc_FrameLengthShift) - 4;

                                        /* Get the addrees of the actual buffer */
                                        frame.buffer = (u8_t *)DMARxDescToGet->Buffer1Addr;
                                } else {
                                        /* Return ERROR */
                                        frame.length = 0;
                                }

                                /* Set Own bit of the Rx descriptor Status: gives the buffer back to ETHERNET DMA */
                                DMARxDescToGet->Status = ETH_DMARxDesc_OWN;

                                /* Update the ETHERNET DMA global Rx descriptor with next Rx decriptor */
                                /* Chained Mode */
                                /* Selects the next DMA Rx descriptor list for next buffer to read */
                                DMARxDescToGet = (ETH_DMADESCTypeDef*) (DMARxDescToGet->Buffer2NextDescAddr);

                                *(struct ethmac_frame *)arg = frame;
                                return STD_RET_OK;
                        }
                        errno = EINVAL;
                        break;

                case IOCTL_ETH__GET_RX_BUFFER_UNAVAILABLE_STATUS:
                        if (arg) {
                                bool *status = arg;
                                if (ETH->DMASR & ETH_DMASR_RBUS)
                                        *status = true;
                                else
                                        *status = false;

                                return STD_RET_OK;
                        }
                        errno = EINVAL;
                        break;

                case IOCTL_ETH__CLEAR_RX_BUFFER_UNAVAILABLE_STATUS:
                        ETH->DMASR = ETH_DMASR_RBUS;
                        return STD_RET_OK;

                case IOCTL_ETH__RESUME_DMA_RECEPTION:
                        ETH->DMARPDR = 0;
                        return STD_RET_OK;

                case IOCTL_ETH__SET_TX_FRAME_LENGTH_CHAIN_MODE:
                        if (arg) {
                                u16_t frame_length = *(int*)arg;

                                /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
                                if ((DMATxDescToSet->Status & ETH_DMATxDesc_OWN) != (u32) RESET) {
                                        return STD_RET_ERROR;
                                } else {
                                        /* Setting the Frame Length: bits[12:0] */
                                        DMATxDescToSet->ControlBufferSize = (frame_length & ETH_DMATxDesc_TBS1);

                                        /* Setting the last segment and first segment bits (in this case a frame is transmitted in one descriptor) */
                                        DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;

                                        /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
                                        DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;

                                        /* When Tx Buffer unavailable flag is set: clear it and resume transmission */
                                        if ((ETH->DMASR & ETH_DMASR_TBUS) != (u32) RESET) {
                                                /* Clear TBUS ETHERNET DMA flag */
                                                ETH->DMASR = ETH_DMASR_TBUS;

                                                /* Resume DMA transmission*/
                                                ETH->DMATPDR = 0;
                                        }

                                        /* Update the ETHERNET DMA global Tx descriptor with next Tx decriptor */
                                        /* Chained Mode */
                                        /* Selects the next DMA Tx descriptor list for next buffer to send */
                                        DMATxDescToSet = (ETH_DMADESCTypeDef*) (DMATxDescToSet->Buffer2NextDescAddr);

                                        return STD_RET_OK;
                                }
                        }
                        errno = EINVAL;
                        break;

                case IOCTL_ETH__GET_CURRENT_TX_BUFFER:
                        if (arg) {
                                u8_t **buffer = arg;

                                *buffer = (u8_t*)DMATxDescToSet->Buffer1Addr;

                                return STD_RET_OK;
                        }
                        errno = EINVAL;
                        break;

                case IOCTL_ETH__INIT_DMA_TX_DESC_LIST_CHAIN_MODE:
                        if (arg) {
                                struct ethmac_DMA_description DMA_desc = *(struct ethmac_DMA_description *)arg;
                                eth_mem->tx_buffer_count       = DMA_desc.buffer_count;
                                eth_mem->DMA_tx_descriptor_tab = malloc(sizeof(ETH_DMADESCTypeDef) * eth_mem->tx_buffer_count);

                                if (eth_mem->DMA_tx_descriptor_tab) {
                                        ETH_DMATxDescChainInit(eth_mem->DMA_tx_descriptor_tab,
                                                               DMA_desc.buffer,
                                                               eth_mem->tx_buffer_count);

                                        return STD_RET_OK;
                                }
                        } else {
                                errno = EINVAL;
                        }
                        break;

                case IOCTL_ETH__INIT_DMA_RX_DESC_LIST_CHAIN_MODE:
                        if (arg) {
                                struct ethmac_DMA_description DMA_desc = *(struct ethmac_DMA_description *)arg;
                                eth_mem->rx_buffer_count       = DMA_desc.buffer_count;
                                eth_mem->DMA_rx_descriptor_tab = malloc(sizeof(ETH_DMADESCTypeDef) * eth_mem->rx_buffer_count);

                                if (eth_mem->DMA_rx_descriptor_tab) {
                                        ETH_DMARxDescChainInit(eth_mem->DMA_rx_descriptor_tab,
                                                               DMA_desc.buffer,
                                                               eth_mem->rx_buffer_count);

                                        return STD_RET_OK;
                                }
                        } else {
                                errno = EINVAL;
                        }
                        break;

                case IOCTL_ETH__ENABLE_RX_IRQ:
                        if (eth_mem->DMA_rx_descriptor_tab != NULL) {
                                for (uint i = 0; i < eth_mem->rx_buffer_count; i++) {
                                        ETH_DMARxDescReceiveITConfig(&eth_mem->DMA_rx_descriptor_tab[i], ENABLE);
                                }

                                return STD_RET_OK;
                        }
                        errno = EINVAL;
                        break;

                case IOCTL_ETH__ENABLE_TX_HARDWARE_CHECKSUM:
                        if (eth_mem->DMA_tx_descriptor_tab != NULL) {
                                for (uint i = 0; i < eth_mem->tx_buffer_count; i++) {
                                        ETH_DMATxDescChecksumInsertionConfig(&eth_mem->DMA_tx_descriptor_tab[i],
                                                                             ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
                                }

                                return STD_RET_OK;
                        }
                        errno = EINVAL;
                        break;

                case IOCTL_ETH__ETHERNET_START:
                        ETH_Start();
                        return STD_RET_OK;

                case IOCTL_ETH__ETHERNET_DEINIT:
                        ETH_DeInit();
                        ETH_SoftwareReset();
                        while (ETH_GetSoftwareResetStatus() == SET);
                        break;

                default:
                        errno = EBADRQC;
                }
        } else {
                errno = EBUSY;
        }

        return STD_RET_ERROR;
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
        struct eth_mem *hdl = device_handle;

        if (device_is_access_granted(&hdl->dev_lock)) {
                ETH_FlushTransmitFIFO();
        } else {
                errno = EBUSY;
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
 * @brief Ethernet interrupt
 */
//==============================================================================
void ETH_IRQHandler(void)
{
      eth_mem->rx_data_ready = true;
      ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
      ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
}

/*==============================================================================
  End of file
==============================================================================*/
