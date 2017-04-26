/*=========================================================================*//**
@file    clk_cfg.h

@author  Daniel Zorychta

@brief   PLL configuration file

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

#ifndef _CLK_CFG_H_
#define _CLK_CFG_H_

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
#define _CLK_CFG__LSI_ON                __CLK_LSI_ON__

/*
 * LSE
 * RCC_LSE_OFF   : LSE oscillator OFF
 * RCC_LSE_ON    : LSE oscillator ON
 * RCC_LSE_Bypass: LSE oscillator bypassed with external clock
 */
#define _CLK_CFG__LSE_ON                __CLK_LSE_ON__

/*
 * HSE
 * RCC_HSE_OFF   : HSE oscillator OFF
 * RCC_HSE_ON    : HSE oscillator ON
 * RCC_HSE_Bypass: HSE oscillator bypassed with external clock
 */
#define _CLK_CFG__HSE_ON                __CLK_HSE_ON__

/*------------------------------------------------------------------------------
 * CLOCK SOURCES
 *----------------------------------------------------------------------------*/
/*
 * RTC Clock source
 * RCC_RTCCLKSource_LSI
 * RCC_RTCCLKSource_LSE
 * RCC_RTCCLKSource_HSE_Div2..31
 */
#define _CLK_CFG__RTCCLK_SRC            __CLK_RTC_CLK_SRC__

/*
 * System clock source
 * RCC_SYSCLKSource_HSI
 * RCC_SYSCLKSource_HSE
 * RCC_SYSCLKSource_PLLCLK
 */
#define _CLK_CFG__SYSCLK_SRC            __CLK_SYS_CLK_SRC__

/*
 * MCO1 clock source
 * RCC_MCO1Source_HSI
 * RCC_MCO1Source_HSE
 * RCC_MCO1Source_LSE
 * RCC_MCO1Source_PLLCLK
 */
#define _CLK_CFG__MCO1_SRC              __CLK_MC01_CLK_SRC__

/*
 * MCO1 clock divider
 * RCC_MCO1Div_1..5
 */
#define _CLK_CFG__MCO1_DIV              __CLK_MC01_CLK_DIV__

/*
 * MCO1 clock source
 * RCC_MCO2Source_HSE
 * RCC_MCO2Source_LSE
 * RCC_MCO2Source_PLLCLK
 * RCC_MCO2Source_SYSCLK
 */
#define _CLK_CFG__MCO2_SRC              __CLK_MC02_CLK_SRC__

/*
 * MCO1 clock divider
 * RCC_MCO2Div_1..5
 */
#define _CLK_CFG__MCO2_DIV              __CLK_MC02_CLK_DIV__

// TODO I2S, SAI, LCD clock sources

/*------------------------------------------------------------------------------
 * PLL CONFIGURATION
 *----------------------------------------------------------------------------*/
/*
 * PLL
 * ENABLE
 * DISABLE
 */
#define _CLK_CFG__PLL_ON                __CLK_PLL_ON__

/*
 * PLL clock source
 * RCC_PLLSource_HSI
 * RCC_PLLSource_HSE
 */
#define _CLK_CFG__PLL_SRC               __CLK_PLL_SRC__

/*
 * PLL source divider (PLLM)
 * 2..63
 *
 * PLLM: specifies the division factor for PLL VCO input clock
 *       This parameter must be a number between 0 and 63.
 * NOTE: You have to set the PLLM parameter correctly to ensure that the VCO input
 *       frequency ranges from 1 to 2 MHz. It is recommended to select a frequency
 *       of 2 MHz to limit PLL jitter.
 */
#define _CLK_CFG__PLL_SRC_DIV           __CLK_PLL_SRC_DIV__

/*
 * PLL multiplier (PLLN)
 * 50..432
 *
 * PLLN: specifies the multiplication factor for PLL VCO output clock
 *       This parameter must be a number between 50 and 432.
 * NOTE: You have to set the PLLN parameter correctly to ensure that the VCO
 *       output frequency is between 100 and 432 MHz.
 */
#define _CLK_CFG__PLL_MUL               __CLK_PLL_MUL__

/*
 * PLL output division factor (PLLP)
 * {2, 4, 6, or 8}
 *
 * PLLP: specifies the division factor for main system clock (SYSCLK)
 *       This parameter must be a number in the range {2, 4, 6, or 8}.
 * NOTE: You have to set the PLLP parameter correctly to not exceed 168 MHz on
 *       the System clock frequency.
 */
#define _CLK_CFG__PLL_OUT_SYSCLK_DIV    __CLK_PLL_OUT_SYSCLK_DIV__

/*
 * PLL OTG FS, SDIO, RNG output division factor (PLLQ)
 * 4..15
 *
 * PLLQ: specifies the division factor for OTG FS, SDIO and RNG clocks
 *       This parameter must be a number between 4 and 15.
 * NOTE: If the USB OTG FS is used in your application, you have to set the
 *       PLLQ parameter correctly to have 48 MHz clock for the USB. However,
 *       the SDIO and RNG need a frequency lower than or equal to 48 MHz to work
 *       correctly.
 */
#define _CLK_CFG__PLL_OUT_48MHz_DIV     __CLK_PLL_OUT_48MHz_DIV__

/*
 * PLL I2S, SAI, SYSTEM, SPDIF (PLLR)
 * 2..7
 *
 * PLLR: specifies the division factor for I2S, SAI, SYSTEM, SPDIF in STM32F446xx devices
 *       This parameter must be a number between 2 and 7.
 *
 * NOTE: If the USB OTG FS is used in your application, you have to set the
 *       PLLQ parameter correctly to have 48 MHz clock for the USB. However,
 *       the SDIO and RNG need a frequency lower than or equal to 48 MHz to work
 *       correctly.
 */
#define _CLK_CFG__PLL_OUT_I2S_DIV       __CLK_PLL_OUT_I2S_DIV__


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
#define _CLK_CFG__AHB_PRE               __CLK_AHB_PRE__

/*
 * APB1 prescaler (in: HCLK; out: PCLK1) (low speed)
 * RCC_HCLK_Div1 : APB1 clock = HCLK
 * RCC_HCLK_Div2 : APB1 clock = HCLK/2
 * RCC_HCLK_Div4 : APB1 clock = HCLK/4
 * RCC_HCLK_Div8 : APB1 clock = HCLK/8
 * RCC_HCLK_Div16: APB1 clock = HCLK/16
 */
#define _CLK_CFG__APB1_PRE              __CLK_APB1_PRE__

/*
 * APB2 prescaler (in: HCLK; out: PCLK2) (high speed)
 * RCC_HCLK_Div1 : APB2 clock = HCLK
 * RCC_HCLK_Div2 : APB2 clock = HCLK/2
 * RCC_HCLK_Div4 : APB2 clock = HCLK/4
 * RCC_HCLK_Div8 : APB2 clock = HCLK/8
 * RCC_HCLK_Div16: APB2 clock = HCLK/16
 */
#define _CLK_CFG__APB2_PRE              __CLK_APB2_PRE__

/*
 * Flash Latency (Flash wait-state)
 *
 * 2.7V - 3.6V
 * 0: SYSCLK <= 30MHz
 * 1: SYSCLK <= 60MHz
 * 2: SYSCLK <= 90MHz
 * 3: SYSCLK <= 120MHz
 * 4: SYSCLK <= 150MHz
 * 5: SYSCLK <= 180MHz
 *
 * 2.4V - 2.7V
 * 0: SYSCLK <= 24MHz
 * 1: SYSCLK <= 48MHz
 * 2: SYSCLK <= 72MHz
 * 3: SYSCLK <= 96MHz
 * 4: SYSCLK <= 120MHz
 * 5: SYSCLK <= 144MHz
 * 6: SYSCLK <= 168MHz
 * 7: SYSCLK <= 180MHz
 *
 * 2.1V - 2.4V
 * 0: SYSCLK <= 22MHz
 * 1: SYSCLK <= 44MHz
 * 2: SYSCLK <= 66MHz
 * 3: SYSCLK <= 88MHz
 * 4: SYSCLK <= 110MHz
 * 5: SYSCLK <= 132MHz
 * 6: SYSCLK <= 154MHz
 * 7: SYSCLK <= 176MHz
 * 8: SYSCLK <= 180MHz
 *
 * 1.8V - 2.1V
 * 0: SYSCLK <= 20MHz
 * 1: SYSCLK <= 40MHz
 * 2: SYSCLK <= 60MHz
 * 3: SYSCLK <= 80MHz
 * 4: SYSCLK <= 100MHz
 * 5: SYSCLK <= 120MHz
 * 6: SYSCLK <= 140MHz
 * 7: SYSCLK <= 160MHz
 * 8: SYSCLK <= 168MHz
 */
#define _CLK_CFG__FLASH_LATENCY         __CLK_FLASH_LATENCY__

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

#endif /* _CLK_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
