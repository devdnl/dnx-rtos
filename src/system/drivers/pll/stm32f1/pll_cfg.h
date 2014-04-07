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
 * LSI
 * ENABLE : enable oscillator
 * DISABLE: disable oscillator
 */
#define _PLL_CFG__LSI_ON                ENABLE

/*
 * LSE
 * RCC_LSE_OFF   : LSE oscillator OFF
 * RCC_LSE_ON    : LSE oscillator ON
 * RCC_LSE_Bypass: LSE oscillator bypassed with external clock
 */
#define _PLL_CFG__LSE_ON                RCC_LSE_OFF

/*
 * HSE
 * RCC_HSE_OFF   : HSE oscillator OFF
 * RCC_HSE_ON    : HSE oscillator ON
 * RCC_HSE_Bypass: HSE oscillator bypassed with external clock
 */
#define _PLL_CFG__HSE_ON                RCC_HSE_ON

/*------------------------------------------------------------------------------
 * CLOCK SOURCES
 *----------------------------------------------------------------------------*/
/*
 * RTC Clock source
 * RCC_RTCCLKSource_LSE       : LSE selected as RTC clock
 * RCC_RTCCLKSource_LSI       : LSI selected as RTC clock
 * RCC_RTCCLKSource_HSE_Div128: HSE clock divided by 128 selected as RTC clock
 */
#define _PLL_CFG__RTCCLK_SRC            RCC_RTCCLKSource_LSI

/*
 * System clock source
 * RCC_SYSCLKSource_HSI   : HSI selected as system clock
 * RCC_SYSCLKSource_HSE   : HSE selected as system clock
 * RCC_SYSCLKSource_PLLCLK: PLL selected as system clock
 */
#define _PLL_CFG__SYSCLK_SRC            RCC_SYSCLKSource_PLLCLK

#ifdef STM32F10X_CL
/*
 * MCO source
 * RCC_MCO_NoClock     : No clock selected
 * RCC_MCO_SYSCLK      : System clock selected
 * RCC_MCO_HSI         : HSI oscillator clock selected
 * RCC_MCO_HSE         : HSE oscillator clock selected
 * RCC_MCO_PLLCLK_Div2 : PLL clock divided by 2 selected
 * RCC_MCO_PLL2CLK     : PLL2 clock selected
 * RCC_MCO_PLL3CLK_Div2: PLL3 clock divided by 2 selected
 * RCC_MCO_XT1         : External 3-25 MHz oscillator clock selected
 * RCC_MCO_PLL3CLK     : PLL3 clock selected
 */
#define _PLL_CFG__MCO_SRC               RCC_MCO_NoClock
#else
/*
 * MCO source
 * RCC_MCO_NoClock     : No clock selected
 * RCC_MCO_SYSCLK      : System clock selected
 * RCC_MCO_HSI         : HSI oscillator clock selected
 * RCC_MCO_HSE         : HSE oscillator clock selected
 * RCC_MCO_PLLCLK_Div2 : PLL clock divided by 2 selected
 */
#define _PLL_CFG__MCO_SRC               RCC_MCO_NoClock
#endif

#ifdef STM32F10X_CL
/*
 * I2S2 clock source
 * RCC_I2S2CLKSource_SYSCLK  : system clock selected as I2S2 clock entry
 * RCC_I2S2CLKSource_PLL3_VCO: PLL3 VCO clock selected as I2S2 clock entry
 */
#define _PLL_CFG__I2S2_SRC              RCC_I2S2CLKSource_SYSCLK

/*
 * I2S3 clock source
 * RCC_I2S3CLKSource_SYSCLK  : system clock selected as I2S3 clock entry
 * RCC_I2S3CLKSource_PLL3_VCO: PLL3 VCO clock selected as I2S3 clock entry
 */
#define _PLL_CFG__I2S3_SRC              RCC_I2S3CLKSource_SYSCLK
#endif


/*------------------------------------------------------------------------------
 * PLL CONFIGURATION
 *----------------------------------------------------------------------------*/
/*
 * PLL
 * ENABLE : enable main PLL
 * DISABLE: disable main PLL
 */
#define _PLL_CFG__PLL_ON                ENABLE

#ifdef STM32F10X_CL
/*
 * PLL clock source (PLLSRC)
 * RCC_PLLSource_HSI_Div2: HSI oscillator clock divided by 2 selected as PLL clock entry
 * RCC_PLLSource_PREDIV1 : PREDIV1 clock selected as PLL clock entry
 */
#define _PLL_CFG__PLL_SRC               RCC_PLLSource_PREDIV1
#else
/*
 * PLL clock source (PLLSRC)
 * RCC_PLLSource_HSI_Div2: HSI oscillator clock divided by 2 selected as PLL clock entry
 * RCC_PLLSource_HSE_Div1: HSE oscillator clock selected as PLL clock entry
 * RCC_PLLSource_HSE_Div2: HSE oscillator clock divided by 2 selected as PLL clock entry
 */
#define _PLL_CFG__PLL_SRC               RCC_PLLSource_HSE_Div1
#endif

#ifdef STM32F10X_CL
/*
 * PLL multiplication factor (PLLMUL)
 * RCC_PLLMul_x where x:{[4,9], 6_5}
 */
#define _PLL_CFG__PLL_MUL               RCC_PLLMul_9
#else
/*
 * PLL multiplication (PLLMUL)
 * RCC_PLLMul_x where x:[2,16]          [Low-, medium-, high- and XL-density]
 */
#define _PLL_CFG__PLL_MUL               RCC_PLLMul_9
#endif

#ifdef STM32F10X_CL
/*
 * USB prescaler
 * RCC_OTGFSCLKSource_PLLVCO_Div3: PLL VCO clock divided by 3 selected as USB OTG FS clock source
 * RCC_OTGFSCLKSource_PLLVCO_Div2: PLL VCO clock divided by 2 selected as USB OTG FS clock source
 */
#define _PLL_CFG__USB_DIV               RCC_OTGFSCLKSource_PLLVCO_Div3
#else
/*
 * USB prescaler
 * RCC_USBCLKSource_PLLCLK_1Div5 : PLL clock divided by 1,5 selected as USB clock source
 * RCC_USBCLKSource_PLLCLK_Div1  : PLL clock selected as USB clock source
 */
#define _PLL_CFG__USB_DIV               RCC_USBCLKSource_PLLCLK_1Div5
#endif


#ifdef STM32F10X_CL
/*------------------------------------------------------------------------------
 * PREDIVIDERS CONFIGURATION
 *----------------------------------------------------------------------------*/
/*
 * PLL pre-divider 1 clock source
 * RCC_PREDIV1_Source_HSE : HSE selected as PREDIV1 clock
 * RCC_PREDIV1_Source_PLL2: PLL2 selected as PREDIV1 clock
 */
#define _PLL_CFG__PREDIV1_SRC           RCC_PREDIV1_Source_HSE

/*
 * PLL pre-divider 1 value
 * RCC_PREDIV1_Div1-16: divider value
 */
#define _PLL_CFG__PREDIV1_VAL           RCC_PREDIV1_Div1

/*
 * PLL pre-divider 2 value
 * RCC_PREDIV2_Divx where x:[1,16]
 */
#define _PLL_CFG__PLL_PREDIV2_VAL       RCC_PREDIV2_Div1


/*------------------------------------------------------------------------------
 * PLL2 CONFIGURATION
 *----------------------------------------------------------------------------*/
/*
 * PLL2
 * ENABLE : enable PLL2
 * DISABLE: disable PLL2
 */
#define _PLL_CFG__PLL2_ON               DISABLE

/*
 * PLL2 multiplication factor
 * RCC_PLL2Mul_x where x:{[8,14], 16, 20}
 */
#define _PLL_CFG__PLL2_MUL              RCC_PLL2Mul_8


/*------------------------------------------------------------------------------
 * PLL3 CONFIGURATION
 *----------------------------------------------------------------------------*/
/*
 * PLL3
 * ENABLE : enable PLL3
 * DISABLE: disable PLL3
 */
#define _PLL_CFG__PLL3_ON               DISABLE

/*
 * PLL3 multiplication factor
 * RCC_PLL3Mul_x where x:{[8,14], 16, 20}
 */
#define _PLL_CFG__PLL3_MUL              RCC_PLL3Mul_8
#endif


/*------------------------------------------------------------------------------
 * SYSTEM PRESCALERS
 *----------------------------------------------------------------------------*/
/*
 * AHB prescaler (in: SYSCLK; out: HCLK)
 * RCC_SYSCLK_Div1  : AHB clock = SYSCLK
 * RCC_SYSCLK_Div2  : AHB clock = SYSCLK/2
 * RCC_SYSCLK_Div4  : AHB clock = SYSCLK/4
 * RCC_SYSCLK_Div8  : AHB clock = SYSCLK/8
 * RCC_SYSCLK_Div16 : AHB clock = SYSCLK/16
 * RCC_SYSCLK_Div64 : AHB clock = SYSCLK/64
 * RCC_SYSCLK_Div128: AHB clock = SYSCLK/128
 * RCC_SYSCLK_Div256: AHB clock = SYSCLK/256
 * RCC_SYSCLK_Div512: AHB clock = SYSCLK/512
 */
#define _PLL_CFG__AHB_PRE               RCC_SYSCLK_Div1

/*
 * APB1 prescaler (in: HCLK; out: PCLK1) (low speed)
 * RCC_HCLK_Div1 : APB1 clock = HCLK
 * RCC_HCLK_Div2 : APB1 clock = HCLK/2
 * RCC_HCLK_Div4 : APB1 clock = HCLK/4
 * RCC_HCLK_Div8 : APB1 clock = HCLK/8
 * RCC_HCLK_Div16: APB1 clock = HCLK/16
 */
#define _PLL_CFG__APB1_PRE              RCC_HCLK_Div2

/*
 * APB2 prescaler (in: HCLK; out: PCLK2) (high speed)
 * RCC_HCLK_Div1 : APB2 clock = HCLK
 * RCC_HCLK_Div2 : APB2 clock = HCLK/2
 * RCC_HCLK_Div4 : APB2 clock = HCLK/4
 * RCC_HCLK_Div8 : APB2 clock = HCLK/8
 * RCC_HCLK_Div16: APB2 clock = HCLK/16
 */
#define _PLL_CFG__APB2_PRE              RCC_HCLK_Div1

/*
 * ADC prescaler (in: PCLK2; out: ADCCLK)
 * RCC_PCLK2_Div2: ADC clock = PCLK2/2
 * RCC_PCLK2_Div4: ADC clock = PCLK2/4
 * RCC_PCLK2_Div6: ADC clock = PCLK2/6
 * RCC_PCLK2_Div8: ADC clock = PCLK2/8
 */
#define _PLL_CFG__ADC_PRE               RCC_PCLK2_Div8

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
