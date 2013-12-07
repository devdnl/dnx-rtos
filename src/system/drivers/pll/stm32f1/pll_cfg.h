/*=========================================================================*//**
@file    pll_cfg.h

@author  Daniel Zorychta

@brief   PLL configuration file

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

#ifndef _PLL_CFG_H_
#define _PLL_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "config.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/*------------------------------------------------------------------------------
 * OSCILLATORS
 *----------------------------------------------------------------------------*/
/*
 * LSI enable (1) or disable (0)
 */
#define _PLL_CFG__LSI_ON                0

/*
 * LSE enable (1) or disable (0)
 */
#define _PLL_CFG__LSE_ON                0

/*
 * HSE enable (1) or disable (0)
 */
#define _PLL_CFG__HSE_ON                1

/*
 * HSE bypass enable (1) or disable (0)
 *
 * Note: can be enabled only if HSE is disabled
 */
#define _PLL_CFG__HSE_BYPASS_ON         0


/*------------------------------------------------------------------------------
 * CLOCK SOURCES
 *----------------------------------------------------------------------------*/
/*
 * RTC Clock source
 * 0: LSI
 * 1: LSE
 * 2: HSI/128
 */
#define _PLL_CFG__RTCCLK_SRC            0

/*
 * System clock source
 * 0: HSE
 * 1: HSI
 * 2: PLLCLK
 */
#define _PLL_CFG__SYSCLK_SRC            2

/*
 * MCO source
 * 0: NO CLOCK
 * 1: HSE
 * 2: HSI
 * 3: SYSCLK
 * 4: PLLCLK/2
 * 5: PLL2CLK                           [Connectivity line only]
 * 6: PLL3CLK/2                         [Connectivity line only]
 * 7: PLL3CLK                           [Connectivity line only]
 * 8: XTAL                              [Connectivity line only]
 */
#define _PLL_CFG__MCO_SRC               0

#ifdef STM32F10X_CL
/*
 * I2S clock source [Connectivity line]
 * 0: SYSCLK
 * 1: 2 x PLL3CLK
 */
#define _PLL_CFG__I2S_SRC               0
#endif

/*------------------------------------------------------------------------------
 * PLL CONFIGURATION
 *----------------------------------------------------------------------------*/
#ifdef STM32F10X_CL
/*
 * PLL pre-divider 1 source (PREDIV1SRC) [Connectivity line]
 * 0: HSE
 * 1: PLL2CLK
 */
#define _PLL_CFG__PLL_PREDIV_SRC        0
#endif

#ifdef STM32F10X_CL
/*
 * PLL pre-divider 1 (PREDIV1) [Connectivity line]
 * 1-16: divider value
 */
#define _PLL_CFG__PLL_PREDIV            1
#endif

#ifndef STM32F10X_CL
/*
 * PLL pre-divider (PLLXTPRE) [Low-, medium-, high- and XL-density]
 * 0: HSE
 * 1: HSE/2
 */
#define _PLL_CFG__PLL_XTPRE_SRC         0
#endif

/*
 * PLL clock source (PLLSRC)
 * 0: HSI/2
 * 1: PLLXTPRE                          [Low-, medium-, high- and XL-density]
 * 1: PREDIV1                           [Connectivity line]
 */
#define _PLL_CFG__PLL_SRC               1

/*
 * PLL multiplication (PLLMUL)
 * 2-16                                 [Low-, medium-, high- and XL-density]
 * 4-9                                  [Connectivity line]
 * 10: x6.5                             [Connectivity line]
 */
#define _PLL_CFG__PLL_MUL               9

/*
 * USB prescaler
 * 1: PLLCLK / 1                        [Low-, medium-, high- and XL-density]
 * 2: PLLCLK / 1.5                      [Low-, medium-, high- and XL-density]
 * 2: (2 x PLLCLK) / 2                  [Connectivity line]
 * 3: (2 x PLLCLK) / 3                  [Connectivity line]
 */
#define _PLL_CFG__USB_DIV               3


/*------------------------------------------------------------------------------
 * PLL2 CONFIGURATION
 *----------------------------------------------------------------------------*/
#ifdef STM32F10X_CL
/*
 * PLL2 enable (1) or disable (0) [Connectivity line]
 */
#define _PLL_CFG__PLL2_ON               0

/*
 * PLL2 multiplication [Connectivity line]
 * 8-14: multiplier range
 * 16  : multiplier value
 * 20  : multiplier value
 */
#define _PLL_CFG__PLL2_MUL              8

/*
 * PLL2/3 pre-divider (PREDIV2) [Connectivity line]
 * 1-16: divider range
 */
#define _PLL_CFG__PLL23_PREDIV          1

#endif


/*------------------------------------------------------------------------------
 * PLL3 CONFIGURATION
 *----------------------------------------------------------------------------*/
#ifdef STM32F10X_CL
/*
 * PLL3 enable (1) or disable (0) [Connectivity line]
 */
#define _PLL_CFG__PLL3_ON               0

/*
 * PLL3 multiplication [Connectivity line]
 * 8-14: multiplier range
 * 16  : multiplier value
 * 20  : multiplier value
 */
#define _PLL_CFG__PLL3_MUL              8

#endif


/*------------------------------------------------------------------------------
 * SYSTEM PRESCALERS
 *----------------------------------------------------------------------------*/
/*
 * AHB prescaler (in: SYSCLK)
 * 1-512: prescaler range
 */
#define _PLL_CFG__AHB_PRE               1

/*
 * APB1 prescaler (in: clock from AHB prescaler)
 * 1, 2, 4, 8, 16: prescaler values
 */
#define _PLL_CFG__APB1_PRE              2

/*
 * APB2 prescaler (in: clock from AHB prescaler)
 * 1, 2, 4, 8, 16: prescaler values
 */
#define _PLL_CFG__APB2_PRE              1

/*
 * ADC prescaler (in: clock from APB2 prescaler)
 * 2, 4, 8: prescaler values
 */
#define _PLL_CFG__ADC_PRE               8

/*
 * Flash Latency (Flash wait-state)
 * 0: SYSCLK <= 24MHz
 * 1: SYSCLK <= 48MHz
 * 2: SYSCLK <= 72MHz
 */
#define _PLL_CFG__FLASH_LATENCY         2

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _PLL_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
