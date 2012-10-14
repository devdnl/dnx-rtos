/*=============================================================================================*//**
@file    pll.c

@author  Daniel Zorychta

@brief   File support PLL

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

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "pll.h"
#include "stm32f10x.h"


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Initialize clocks
 *
 * NOTE: PLL2 and PLL3 not used
 */
//================================================================================================//
stdRet_t PLL_Init(dev_t dev)
{
      (void) dev;

      u32_t wait;

      /* turn on HSE oscillator */
      RCC->CR |= RCC_CR_HSEON;

      /* waiting for HSE ready */
      wait = UINT32_MAX;

      while (!(RCC->CR & RCC_CR_HSERDY) && wait)
      {
            wait--;
      }

      if (wait == 0)
            return PLL_STATUS_HSE_ERROR;

      /* wait states */
      if (CPU_TARGET_FREQ <= 24000000UL)
            FLASH->ACR |= (0x00 & FLASH_ACR_LATENCY);
      else if (CPU_TARGET_FREQ <= 48000000UL)
            FLASH->ACR |= (0x01 & FLASH_ACR_LATENCY);
      else if (CPU_TARGET_FREQ <= 72000000UL)
            FLASH->ACR |= (0x02 & FLASH_ACR_LATENCY);
      else
            FLASH->ACR |= (0x03 & FLASH_ACR_LATENCY);

      /* AHB prescaler  configuration (/1) */
      RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

      /* APB1 prescaler configuration (/2) */
      RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

      /* APB2 prescaler configuration (/1) */
      RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

      /* FCLK cortex free running clock */
      SysTick->CTRL |= SysTick_CTRL_CLKSOURCE;

      /* PLL source - HSE; PREDIV1 = 1; PLL x9 */
      RCC->CFGR2 |= RCC_CFGR2_PREDIV1SRC_HSE | RCC_CFGR2_PREDIV1_DIV1;
      RCC->CFGR  |= RCC_CFGR_PLLSRC_PREDIV1  | RCC_CFGR_PLLMULL9;

      /* OTG USB set to 48 MHz (72*2 / 3)*/
      RCC->CFGR &= ~RCC_CFGR_OTGFSPRE;

      /* I2S3 and I2S2 from SYSCLK */
      RCC->CFGR2 &= ~(RCC_CFGR2_I2S3SRC | RCC_CFGR2_I2S2SRC);

      /* enable PLL */
      RCC->CR |= RCC_CR_PLLON;

      /* waiting for PLL ready */
      wait = UINT32_MAX;

      while (!(RCC->CR & RCC_CR_PLLRDY) && wait)
      {
            wait--;
      }

      if (wait == 0)
            return PLL_STATUS_PLL_ERROR;

      /* set PLL as system clock */
      RCC->CFGR |= RCC_CFGR_SW_PLL;

      wait = UINT32_MAX;

      while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
      {
            wait--;
      }

      if (wait == 0)
            return PLL_STATUS_PLL_SW_ERROR;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Open device
 *
 * @param dev     device number
 *
 * @retval STD_STATUS_OK
 */
//================================================================================================//
stdRet_t PLL_Open(dev_t dev)
{
      (void)dev;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Close device
 *
 * @param dev     device number
 *
 * @retval STD_STATUS_OK
 */
//================================================================================================//
stdRet_t PLL_Close(dev_t dev)
{
      (void)dev;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Write to the device
 *
 * @param dev     device number
 * @param *src    source
 * @param size    size
 * @param seek    seek
 *
 * @retval STD_STATUS_OK
 */
//================================================================================================//
stdRet_t PLL_Write(dev_t dev, void *src, size_t size, size_t seek)
{
      (void)dev;
      (void)src;
      (void)size;
      (void)seek;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Read from device
 *
 * @param dev     device number
 * @param *dst    destination
 * @param size    size
 * @param seek    seek
 *
 * @retval STD_STATUS_OK
 */
//================================================================================================//
stdRet_t PLL_Read(dev_t dev, void *dst, size_t size, size_t seek)
{
      (void)dev;
      (void)dst;
      (void)size;
      (void)seek;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief IO control
 *
 * @param[in]     dev     device number
 * @param[in]     ioRq    IO reqest
 * @param[in,out] data    data pointer
 *
 * @retval STD_STATUS_OK
 */
//================================================================================================//
stdRet_t PLL_IOCtl(dev_t dev, IORq_t ioRq, void *data)
{
      (void)dev;
      (void)ioRq;
      (void)data;

      return STD_RET_OK;
}

/*==================================================================================================
                                            End of file
==================================================================================================*/
