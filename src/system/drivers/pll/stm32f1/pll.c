/*=========================================================================*//**
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


*//*==========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "system/dnxmodule.h"
#include "stm32f1/pll_cfg.h"
#include "stm32f1/stm32f10x.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize PLL module
 */
//==============================================================================
API_MOD_INIT(PLL, void **device_handle, u8_t major, u8_t minor)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(major);
        UNUSED_ARG(minor);

        u32_t wait;

        /* turn on HSE oscillator */
        RCC->CR |= RCC_CR_HSEON;

        /* waiting for HSE ready */
        wait = UINT32_MAX;
        while (!(RCC->CR & RCC_CR_HSERDY) && wait) {
                wait--;
        }

        if (wait == 0)
                return PLL_STATUS_HSE_ERROR;

        /* wait states */
        if (PLL_CPU_TARGET_FREQ <= 24000000UL)
                FLASH->ACR |= (0x00 & FLASH_ACR_LATENCY);
        else if (PLL_CPU_TARGET_FREQ <= 48000000UL)
                FLASH->ACR |= (0x01 & FLASH_ACR_LATENCY);
        else if (PLL_CPU_TARGET_FREQ <= 72000000UL)
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
        while (!(RCC->CR & RCC_CR_PLLRDY) && wait) {
                wait--;
        }

        if (wait == 0)
                return PLL_STATUS_PLL_ERROR;

        /* set PLL as system clock */
        RCC->CFGR |= RCC_CFGR_SW_PLL;

        wait = UINT32_MAX;
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {
                wait--;
        }

        if (wait == 0)
                return PLL_STATUS_PLL_SW_ERROR;

        return STD_RET_OK;
}


//==============================================================================
/**
 * @brief Release PLL device
 */
//==============================================================================
API_MOD_RELEASE(PLL, void *device_handle)
{
        UNUSED_ARG(device_handle);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Open device
 */
//==============================================================================
API_MOD_OPEN(PLL, void *device_handle, int flags)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(flags);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Close device
 */
//==============================================================================
API_MOD_CLOSE(PLL, void *device_handle, bool force, task_t *opened_by_task)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(force);
        UNUSED_ARG(opened_by_task);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Write data to device
 */
//==============================================================================
API_MOD_WRITE(PLL, void *device_handle, const u8_t *src, size_t count, u64_t *fpos)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(src);
        UNUSED_ARG(count);
        UNUSED_ARG(fpos);

        return 0;
}

//==============================================================================
/**
 * @brief Read data from device
 */
//==============================================================================
API_MOD_READ(PLL, void *device_handle, u8_t *dst, size_t count, u64_t *fpos)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(dst);
        UNUSED_ARG(count);
        UNUSED_ARG(fpos);

        return 0;
}

//==============================================================================
/**
 * @brief Device control
 */
//==============================================================================
API_MOD_IOCTL(PLL, void *device_handle, int iorq, void *arg)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(iorq);
        UNUSED_ARG(arg);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Flush device
 */
//==============================================================================
API_MOD_FLUSH(PLL, void *device_handle)
{
        UNUSED_ARG(device_handle);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Interface returns device informations
 */
//==============================================================================
API_MOD_STAT(PLL, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG(device_handle);

        device_stat->st_size  = 0;
        device_stat->st_major = 0;
        device_stat->st_minor = 0;

        return STD_RET_OK;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
