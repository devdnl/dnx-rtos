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
#include "netconf.h"
#include "lwipopts.h"


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
struct eth_mem {
      bool_t RxDataReady;
};

/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
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

            if (eth_mem == NULL) {
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

      return STD_RET_OK;
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

      return STD_RET_OK;
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

      if (eth_mem) {
            switch (ioRq) {
            case ETHER_IORQ_GET_RX_FLAG:
                  *(bool_t*)data = eth_mem->RxDataReady;
                  status = STD_RET_OK;
                  break;

            case ETHER_IORQ_CLEAR_RX_FLAG:
                  eth_mem->RxDataReady = FALSE;
                  status = STD_RET_OK;
                  break;
            }
      }

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

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Ethernet Interrupt
 */
//================================================================================================//
void ETH_IRQHandler(void)
{
//      LwIP_SetReceiveFlag();
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
