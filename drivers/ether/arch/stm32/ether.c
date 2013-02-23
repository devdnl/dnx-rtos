/*=============================================================================================*//**
@file    eth.c

@author  Daniel Zorychta

@brief   This file support ethernet peripheral

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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


*//*==============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "ether.h"
#include "stm32_eth.h"
#include "lwipopts.h"


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
#define ETH_DMARxDesc_FrameLengthShift          16
#define MTX_BLOCK_TIME                          0


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
struct eth_mem {
      bool_t              RxDataReady;
      u8_t                MACaddr[6];
      ETH_DMADESCTypeDef *DMATxDscrTab;
      u8_t                TxBfrCount;
      ETH_DMADESCTypeDef *DMARxDscrTab;
      u8_t                RxBfrCount;
      mutex_t             mtx;
};

/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
extern ETH_DMADESCTypeDef *DMARxDescToGet;
extern ETH_DMADESCTypeDef *DMATxDescToSet;

static struct eth_mem *eth_mem;


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Initialise unit
 *
 * @param dev                 Ethernet number
 * @param part                device part
 *
 * @retval STD_RET_OK         success
 * @retval STD_RET_ERROR      error
 */
//================================================================================================//
stdRet_t ETHER_Init(devx_t dev, fd_t part)
{
      (void)part;
      (void)dev;

      stdRet_t status = STD_RET_ERROR;

      if (eth_mem == NULL) {
            eth_mem = calloc(1, sizeof(struct eth_mem));

            if (eth_mem) {
                  eth_mem->mtx = new_recursive_mutex();

                  if (!eth_mem->mtx) {
                        free(eth_mem);
                        eth_mem = NULL;
                        goto ETHER_Init_End;
                  }
            } else {
                  goto ETHER_Init_End;
            }
      }

      /* enable Ethernet clock */
      RCC->AHBENR |= RCC_AHBENR_ETHMACRXEN | RCC_AHBENR_ETHMACTXEN | RCC_AHBENR_ETHMACEN;

      /* enable Ethernet IRQ */
      NVIC_EnableIRQ(ETH_IRQn);
      NVIC_SetPriority(ETH_IRQn, 0xDF);

      /* configure Ethernet --------------------------------------------------------------------- */
      ETH_InitTypeDef ETH_InitStructure;

      /* Reset ETHERNET on AHB Bus */
      ETH_DeInit();

      /* Software reset */
      ETH_SoftwareReset();

      /* Wait for software reset */
      while (ETH_GetSoftwareResetStatus() == SET);

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
      ETH_InitStructure.ETH_Speed                    = ETH_Speed_100M;
      #ifdef CHECKSUM_BY_HARDWARE
            ETH_InitStructure.ETH_ChecksumOffload    = ETH_ChecksumOffload_Enable;
      #endif

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

      /* Configure Ethernet */
      if (ETH_Init(&ETH_InitStructure, ETHER_PHY_ADDRESS) == TRUE)
      {
            /* Enable the Ethernet Rx Interrupt */
            ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);

            status = STD_RET_OK;
      } else {
            free(eth_mem);
            eth_mem = NULL;
      }

      ETHER_Init_End:
      return status;
}


//================================================================================================//
/**
 * @brief Release ethernet device
 *
 * @param eth                 ethernet number
 * @param part                device part
 *
 * @retval STD_RET_OK         success
 * @retval STD_RET_ERROR      error
 */
//================================================================================================//
stdRet_t ETHER_Release(devx_t dev, fd_t part)
{
      (void)dev;
      (void)part;

      stdRet_t status = STD_RET_ERROR;

      if (mutex_recursive_lock(eth_mem->mtx, MTX_BLOCK_TIME) == OS_OK) {
            TaskSuspendAll();
            mutex_recursive_unlock(eth_mem->mtx);
            delete_mutex(eth_mem->mtx);
            free(eth_mem);
            eth_mem = NULL;
            TaskResumeAll();
            ETH_DeInit();

            status = STD_RET_OK;
      }

      return status;
}


//================================================================================================//
/**
 * @brief Open device
 *
 * @param dev     device number
 * @param part    device part
 *
 * @retval STD_STATUS_OK
 */
//================================================================================================//
stdRet_t ETHER_Open(devx_t dev, fd_t part)
{
      (void)dev;
      (void)part;

      stdRet_t status = STD_RET_ERROR;

      if (mutex_recursive_lock(eth_mem->mtx, MTX_BLOCK_TIME) == OS_OK) {
            /* port reserved for this application, not release mutex at exit */

            status = STD_RET_OK;
      }

      return status;
}


//================================================================================================//
/**
 * @brief Close device
 *
 * @param dev     device number
 * @param part    device part
 *
 * @retval STD_STATUS_OK
 */
//================================================================================================//
stdRet_t ETHER_Close(devx_t dev, fd_t part)
{
      (void)dev;
      (void)part;

      stdRet_t status = STD_RET_ERROR;

      if (mutex_recursive_lock(eth_mem->mtx, MTX_BLOCK_TIME) == OS_OK) {
            /* mutex from open() */
            mutex_recursive_unlock(eth_mem->mtx);

            /* this mutex */
            mutex_recursive_unlock(eth_mem->mtx);

            status = STD_RET_OK;
      }

      return status;
}


//================================================================================================//
/**
 * @brief Write to the device
 *
 * @param dev     device number
 * @param part    device part
 * @param *src    source
 * @param size    size
 * @param seek    seek
 *
 * @retval number of written nitems
 */
//================================================================================================//
size_t ETHER_Write(devx_t dev, fd_t part, void *src, size_t size, size_t nitems, size_t seek)
{
      (void)dev;
      (void)part;
      (void)src;
      (void)size;
      (void)seek;
      (void)nitems;

      return 0;
}


//================================================================================================//
/**
 * @brief Read from device
 *
 * @param dev     device number
 * @param part    device part
 * @param *dst    destination
 * @param size    size
 * @param seek    seek
 *
 * @retval number of read nitems
 */
//================================================================================================//
size_t ETHER_Read(devx_t dev, fd_t part, void *dst, size_t size, size_t nitems, size_t seek)
{
      (void)dev;
      (void)part;
      (void)dst;
      (void)size;
      (void)seek;
      (void)nitems;

      return 0;
}


//================================================================================================//
/**
 * @brief IO control
 *
 * @param[in]     dev     device number
 * @param part    device part
 * @param[in]     ioRq    IO reqest
 * @param[in,out] data    data pointer
 *
 * @retval STD_STATUS_OK
 */
//================================================================================================//
stdRet_t ETHER_IOCtl(devx_t dev, fd_t part, IORq_t ioRq, void *data)
{
      (void)dev;
      (void)part;

      stdRet_t status = STD_RET_ERROR;

      if (mutex_recursive_lock(eth_mem->mtx, MTX_BLOCK_TIME) == OS_OK) {
            if (eth_mem) {
                  status = ETHER_STD_RET_NULL_DATA;

                  switch (ioRq) {
                  case ETHER_IORQ_GET_RX_FLAG:
                        if (data) {
                              *(bool_t*)data = eth_mem->RxDataReady;
                              status = STD_RET_OK;
                        }
                        break;

                  case ETHER_IORQ_CLEAR_RX_FLAG:
                        eth_mem->RxDataReady = FALSE;
                        status = STD_RET_OK;
                        break;

                  case ETHER_IORQ_SET_MAC_ADR:
                        if (data) {
                              u8_t *MAC = data;
                              eth_mem->MACaddr[0] = MAC[0];
                              eth_mem->MACaddr[1] = MAC[1];
                              eth_mem->MACaddr[2] = MAC[2];
                              eth_mem->MACaddr[3] = MAC[3];
                              eth_mem->MACaddr[4] = MAC[4];
                              eth_mem->MACaddr[5] = MAC[5];

                              ETH_MACAddressConfig(ETH_MAC_Address0, eth_mem->MACaddr);

                              status = STD_RET_OK;
                        }
                        break;

                  case ETHER_IORQ_GET_RX_PACKET_SIZE:
                        if (data) {
                              *(u32_t*)data = ETH_GetRxPktSize();
                              status = STD_RET_OK;
                        }
                        break;

                  /* returns ethernet rx buffer frame data */
                  case ETHER_IORQ_GET_RX_PACKET_CHAIN_MODE:
                        if (data) {
                              struct ether_frame frame = {0, 0};

                              /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
                              if ((DMARxDescToGet->Status & ETH_DMARxDesc_OWN) != (u32) RESET) {
                                    frame.length = 0;

                                    if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32) RESET) {
                                          /* Clear RBUS ETHERNET DMA flag */
                                          ETH->DMASR = ETH_DMASR_RBUS;

                                          /* Resume DMA reception */
                                          ETH->DMARPDR = 0;
                                    }

                                    /* Return error: OWN bit set */
                                    goto ETHER_IORQ_GET_RX_PACKET_CHAIN_MODE_End;
                              }

                              if (  ((DMARxDescToGet->Status & ETH_DMARxDesc_ES) == (u32) RESET)
                                 && ((DMARxDescToGet->Status & ETH_DMARxDesc_LS) != (u32) RESET)
                                 && ((DMARxDescToGet->Status & ETH_DMARxDesc_FS) != (u32) RESET) ) {
                                    /* Get the Frame Length of the received packet: substruct 4 bytes of the CRC */
                                    frame.length = ((DMARxDescToGet->Status & ETH_DMARxDesc_FL)
                                                    >> ETH_DMARxDesc_FrameLengthShift) - 4;

                                    /* Get the addrees of the actual buffer */
                                    frame.buffer = DMARxDescToGet->Buffer1Addr;
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

                              ETHER_IORQ_GET_RX_PACKET_CHAIN_MODE_End:
                              *(struct ether_frame*)data = frame;
                              status = STD_RET_OK;
                        }

                        break;

                  case ETHER_IORQ_GET_RX_BUFFER_UNAVAILABLE_STATUS:
                        if (data) {
                              if (ETH->DMASR & ETH_DMASR_RBUS) {
                                    *(bool_t*)data = TRUE;
                              } else {
                                    *(bool_t*)data = FALSE;
                              }

                              status = STD_RET_OK;
                        }

                        break;

                  case ETHER_IORQ_CLEAR_RX_BUFFER_UNAVAILABLE_STATUS:
                        ETH->DMASR = ETH_DMASR_RBUS;
                        status = STD_RET_OK;
                        break;

                  case ETHER_IORQ_RESUME_DMA_RECEPTION:
                        ETH->DMARPDR = 0;
                        status = STD_RET_OK;
                        break;

                  case ETHER_IORQ_SET_TX_FRAME_LENGTH_CHAIN_MODE:
                        if (data) {
                              u16_t FrameLength = *(u16_t*)data;

                              /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
                              if ((DMATxDescToSet->Status & ETH_DMATxDesc_OWN) != (u32)RESET) {
                                    /* Return ERROR: OWN bit set */
                                    status = STD_RET_ERROR;
                              } else {
                                    /* Setting the Frame Length: bits[12:0] */
                                    DMATxDescToSet->ControlBufferSize = (FrameLength & ETH_DMATxDesc_TBS1);

                                    /* Setting the last segment and first segment bits (in this case a frame is transmitted in one descriptor) */
                                    DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;

                                    /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
                                    DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;

                                    /* When Tx Buffer unavailable flag is set: clear it and resume transmission */
                                    if ((ETH->DMASR & ETH_DMASR_TBUS) != (u32)RESET) {
                                          /* Clear TBUS ETHERNET DMA flag */
                                          ETH->DMASR = ETH_DMASR_TBUS;

                                          /* Resume DMA transmission*/
                                          ETH->DMATPDR = 0;
                                    }

                                    /* Update the ETHERNET DMA global Tx descriptor with next Tx decriptor */
                                    /* Chained Mode */
                                    /* Selects the next DMA Tx descriptor list for next buffer to send */
                                    DMATxDescToSet = (ETH_DMADESCTypeDef*) (DMATxDescToSet->Buffer2NextDescAddr);

                                    /* Return SUCCESS */
                                    status = STD_RET_OK;
                              }
                        }

                        break;

                  case ETHER_IORQ_GET_CURRENT_TX_BUFFER:
                        if (data) {
                              u8_t **buffer = data;

                              *buffer = (u8_t*)DMATxDescToSet->Buffer1Addr;

                              status  = STD_RET_OK;
                        }

                        break;

                  case ETHER_IORQ_INIT_DMA_TX_DESC_LIST_CHAIN_MODE:
                        if (data) {
                              struct DMADesc DMADesc = *(struct DMADesc*)data;
                              eth_mem->TxBfrCount   = DMADesc.buffer_count;
                              eth_mem->DMATxDscrTab = malloc(sizeof(ETH_DMADESCTypeDef) * eth_mem->TxBfrCount);

                              if (eth_mem->DMATxDscrTab) {
                                    ETH_DMATxDescChainInit(eth_mem->DMATxDscrTab,
                                                           DMADesc.buffer,
                                                           eth_mem->TxBfrCount);

                                    status  = STD_RET_OK;
                              }
                        }

                        break;

                  case ETHER_IORQ_INIT_DMA_RX_DESC_LIST_CHAIN_MODE:
                        if (data) {
                              struct DMADesc DMADesc = *(struct DMADesc*)data;
                              eth_mem->RxBfrCount   = DMADesc.buffer_count;
                              eth_mem->DMARxDscrTab = malloc(sizeof(ETH_DMADESCTypeDef) * eth_mem->RxBfrCount);

                              if (eth_mem->DMARxDscrTab) {
                                    ETH_DMARxDescChainInit(eth_mem->DMARxDscrTab,
                                                           DMADesc.buffer,
                                                           eth_mem->RxBfrCount);

                                    status  = STD_RET_OK;
                              }
                        }

                        break;

                  case ETHER_IORQ_ENABLE_RX_IRQ:
                        if (eth_mem->DMARxDscrTab != NULL) {
                              for (uint i = 0; i < eth_mem->RxBfrCount; i++) {
                                    ETH_DMARxDescReceiveITConfig(&eth_mem->DMARxDscrTab[i], ENABLE);
                              }

                              status = STD_RET_OK;
                        } else {
                              status = STD_RET_ERROR;
                        }

                        break;

                  case ETHER_IORQ_ENABLE_TX_HARDWARE_CHECKSUM:
                        if (eth_mem->DMATxDscrTab != NULL) {
                              for (uint i = 0; i < eth_mem->TxBfrCount; i++) {
                                    ETH_DMATxDescChecksumInsertionConfig(&eth_mem->DMATxDscrTab[i],
                                                                         ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
                              }

                              status = STD_RET_OK;
                        } else {
                              status = STD_RET_ERROR;
                        }

                        break;

                  case ETHER_IORQ_ETHERNET_START:
                        ETH_Start();
                        status = STD_RET_OK;
                        break;

                  default:
                        status = ETHER_STD_RET_BAD_REQUEST;
                        break;
                  }
            }

            mutex_recursive_unlock(eth_mem->mtx);
      }

      return status;
}


//================================================================================================//
/**
 * @brief Ethernet Interrupt
 */
//================================================================================================//
void ETH_IRQHandler(void)
{
      eth_mem->RxDataReady = TRUE;

      /* Clear the Eth DMA Rx IT pending bits */
      ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
      ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
