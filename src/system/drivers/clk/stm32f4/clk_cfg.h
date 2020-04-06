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
 * RCC_SYSCLKSource_PLLCLK (RCC_SYSCLKSource_PLLPCLK STM32F446xx)
 * RCC_SYSCLKSource_PLLRCLK (STM32F412xG, STM32F413_423xx and STM32F446xx)
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
 * RCC_MCO1Div_1
 * RCC_MCO1Div_2
 * RCC_MCO1Div_3
 * RCC_MCO1Div_4
 * RCC_MCO1Div_5
 */
#define _CLK_CFG__MCO1_DIV              __CLK_MC01_CLK_DIV__

/*
 * MCO1 clock source
 * RCC_MCO2Source_SYSCLK
 * RCC_MCO2Source_PLLI2SCLK
 * RCC_MCO2Source_I2SCLK
 * RCC_MCO2Source_HSE
 * RCC_MCO2Source_PLLCLK
 */
#define _CLK_CFG__MCO2_SRC              __CLK_MC02_CLK_SRC__

/*
 * MCO1 clock divider
 * RCC_MCO2Div_1..5
 */
#define _CLK_CFG__MCO2_DIV              __CLK_MC02_CLK_DIV__

/*
 * Clock source of I2S peripheral instance on APB1 Bus
 * (STM32F412xG, STM32F413_423xx, STM32F446xx)
 * RCC_I2SCLKSource_PLLI2S
 * RCC_I2SCLKSource_Ext
 * RCC_I2SCLKSource_PLL
 * RCC_I2SCLKSource_HSI_HSE
 */
#define _CLK_CFG__I2SAPB1_CLK_SRC       __CLK_I2SAPB1_CLK_SRC__

/*
 * Clock source of I2S peripheral instance on APB2 Bus
 * (STM32F412xG, STM32F413_423xx, STM32F446xx)
 * RCC_I2SCLKSource_PLLI2S
 * RCC_I2SCLKSource_Ext
 * RCC_I2SCLKSource_PLL
 * RCC_I2SCLKSource_HSI_HSE
 */
#define _CLK_CFG__I2SAPB2_CLK_SRC       __CLK_I2SAPB2_CLK_SRC__

/*
 * Clock source of I2S peripheral
 * (STM32F410xx):
 * RCC_I2SAPBCLKSOURCE_PLLR
 * RCC_I2SAPBCLKSOURCE_EXT
 * RCC_I2SAPBCLKSOURCE_PLLSRC
 *
 * (STM32F410xx, STM32F40_41xxx, STM32F427_437xx, STM32F429_439xx, STM32F401xx, STM32F411xE, STM32F469_479xx):
 * RCC_I2S2CLKSource_PLLI2S
 * RCC_I2SAPBCLKSOURCE_EXT
 */
#define _CLK_CFG__I2S_CLK_SRC           __CLK_I2S_CLK_SRC__

/*
 * SAI1 clock source
 * (STM32F446xx)
 * RCC_SAICLKSource_PLLSAI
 * RCC_SAICLKSource_PLLI2S
 * RCC_SAICLKSource_PLL
 * RCC_SAICLKSource_HSI_HSE
 */
#define _CLK_CFG__SAI1_CLK_SRC          __CLK_SAI1_CLK_SRC__

/*
 * SAI2 clock source
 * (STM32F446xx)
 * RCC_SAICLKSource_PLLSAI
 * RCC_SAICLKSource_PLLI2S
 * RCC_SAICLKSource_PLL
 * RCC_SAICLKSource_HSI_HSE
 */
#define _CLK_CFG__SAI2_CLK_SRC          __CLK_SAI2_CLK_SRC__

/*
 * Configures the SAI clock Divider coming from PLLSAI.
 * (STM32F42xxx/43xxx/446xx/469xx/479xx devices):
 * 1..32
 */
#define _CLK_CFG__SAI_PLLSAI_CLK_DIV    __CLK_SAI_PLLSAI_CLK_DIV__

/*
 * Configures the SAI clock Divider coming from PLLI2S.
 * (STM32F42xxx/43xxx/446xx/469xx/479xx devices):
 * 1..32
 */
#define _CLK_CFG__SAI_PLLI2S_CLK_DIV    __CLK_SAI_PLLI2S_CLK_DIV__

/*
 * Configures the SAI clock Divider coming from PLLI2S.
 * (STM32F413, STM32F423):
 * 1..32
 */
#define _CLK_CFG__SAI_PLLI2S_CLK_DIVR   __CLK_SAI_PLLI2S_CLK_DIVR__

/*
 * Configures the SAI clock Divider coming from PLL.
 * (STM32F413, STM32F423):
 * 1..32
 */
#define _CLK_CFG__SAI_PLL_CLK_DIVR      __CLK_SAI_PLL_CLK_DIVR__

/*
 * SAI Block A Clock Source
 * (STM32F40_41xxx, STM32F427_437xx, STM32F429_439xx, STM32F469_479xx):
 * RCC_SAIACLKSource_PLLI2S: PLLI2S_Q clock divided by PLLI2SDIVQ used as SAI1 Block A clock
 * RCC_SAIACLKSource_PLLSAI: PLLISAI_Q clock divided by PLLSAIDIVQ used as SAI1 Block A clock
 * RCC_SAIACLKSource_Ext:    External clock mapped on the I2S_CKIN pin used as SAI1 Block A clock
 *
 * (STM32F413_423xx):
 * RCC_SAIACLKSource_PLLI2SR: PLLI2SR clock used as SAI clock source
 * RCC_SAIACLKSource_PLLI2S:  PLLI2S clock used as SAI clock source
 * RCC_SAIACLKSource_PLL:     PLL clock used as SAI clock source
 * RCC_SAIACLKSource_HSI_HSE: HSI or HSE depends on PLLSRC used as SAI clock source
 */
#define _CLK_CFG__SAI_BLOCK_A_CLK_SRC   __CLK_SAI_BLOCK_A_CLK_SRC__

/*
 * SAI Block B Clock Source
 * (STM32F40_41xxx, STM32F427_437xx, STM32F429_439xx, STM32F469_479xx):
 * RCC_SAIACLKSource_PLLI2S: PLLI2S_Q clock divided by PLLI2SDIVQ used as SAI1 Block A clock
 * RCC_SAIACLKSource_PLLSAI: PLLISAI_Q clock divided by PLLSAIDIVQ used as SAI1 Block A clock
 * RCC_SAIACLKSource_Ext:    External clock mapped on the I2S_CKIN pin used as SAI1 Block A clock
 *
 * (STM32F413_423xx):
 * RCC_SAIACLKSource_PLLI2SR: PLLI2SR clock used as SAI clock source
 * RCC_SAIACLKSource_PLLI2S:  PLLI2S clock used as SAI clock source
 * RCC_SAIACLKSource_PLL:     PLL clock used as SAI clock source
 * RCC_SAIACLKSource_HSI_HSE: HSI or HSE depends on PLLSRC used as SAI clock source
 */
#define _CLK_CFG__SAI_BLOCK_B_CLK_SRC   __CLK_SAI_BLOCK_B_CLK_SRC__

/*
 * Configures the LTDC clock Divider coming from PLLSAI.
 * (STM32F427_437xx, STM32F429_439xx, STM32F446xx, STM32F469_479xx):
 * RCC_PLLSAIDivR_Div2
 * RCC_PLLSAIDivR_Div4
 * RCC_PLLSAIDivR_Div8
 * RCC_PLLSAIDivR_Div16
 */
#define _CLK_CFG__LTDC_CLK_DIV          __CLK_LTDC_DIV__

/*
 * Configures the DFSDM clock source (DFSDMCLK).
 * (STM32F412xG, STM32F413, STM32F423):
 * RCC_DFSDMCLKSource_APB
 * RCC_DFSDMCLKSource_SYS
 */
#define _CLK_CFG__DFSDM1_CLK_SRC        __CLK_DFSDM1_CLK_SRC__

/*
 * Configures the DFSDM Audio clock source (DFSDMACLK).
 * (STM32F412xG, STM32F413, STM32F423):
 * RCC_DFSDM1AUDIOCLKSOURCE_I2SAPB1
 * RCC_DFSDM1AUDIOCLKSOURCE_I2SAPB2
 */
#define _CLK_CFG__DFSDM1_ACLK_SRC       __CLK_DFSDM1_ACLK_SRC__

/*
 * Configures the DFSDM Audio clock source (DFSDMACLK).
 * (STM32F413, STM32F423):
 * RCC_DFSDM1AUDIOCLKSOURCE_I2SAPB1
 * RCC_DFSDM1AUDIOCLKSOURCE_I2SAPB2
 */
#define _CLK_CFG__DFSDM2_ACLK_SRC       __CLK_DFSDM2_ACLK_SRC__

/*
 * Configures the DSI clock Source.
 * (STM32F469, STM32F479):
 * RCC_DSICLKSource_PHY
 * RCC_DSICLKSource_PLLR
 */
#define _CLK_CFG__DSI_CLK_SRC           __CLK_DSI_SRC__

/*
 * Configures the 48MHz clock Source.
 * (STM32F412xG, STM32F413_423xx, STM32F446xx, STM32F469_479xx):
 * RCC_48MHZCLKSource_PLL
 * RCC_48MHZCLKSource_PLLSAI
 * RCC_CK48CLKSOURCE_PLLI2SQ
 */
#define _CLK_CFG__48MHZ_CLK_SRC         __CLK_48MHZ_SRC__

/*
 * Configures the SDIO clock Source.
 * (STM32F412xG, STM32F413_423xx, STM32F446xx, STM32F469_479xx):
 * RCC_SDIOCLKSource_48MHZ
 * RCC_SDIOCLKSource_SYSCLK
 */
#define _CLK_CFG__SDIO_CLK_SRC          __CLK_SDIO_SRC__

/*
 * Configures the SPDIFRX clock Source.
 * (STM32F446):
 * RCC_SPDIFRXCLKSource_PLLR
 * RCC_SPDIFRXCLKSource_PLLI2SP
 */
#define _CLK_CFG__SPDIF_CLK_SRC         __CLK_SPDIF_SRC__

/*
 * Configures the CEC clock Source.
 * (STM32F446):
 * RCC_CECCLKSource_HSIDiv488
 * RCC_CECCLKSource_LSE
 */
#define _CLK_CFG__CEC_CLK_SRC           __CLK_CEC_SRC__

/*
 * Configures the FMPI2C1 clock Source.
 * (STM32F410xx, STM32F412xG, STM32F413, STM32F423xx, STM32F446xx):
 * RCC_FMPI2C1CLKSource_APB1
 * RCC_FMPI2C1CLKSource_SYSCLK
 * RCC_FMPI2C1CLKSource_HSI
 */
#define _CLK_CFG__FMPI2C1_CLK_SRC        __CLK_FMPI2C1_SRC__

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
#define _CLK_CFG__PLL_SRC_DIV_M         __CLK_PLL_SRC_DIV_M__

/*
 * PLL multiplier (PLLN)
 * 50..432
 *
 * PLLN: specifies the multiplication factor for PLL VCO output clock
 *       This parameter must be a number between 50 and 432.
 * NOTE: You have to set the PLLN parameter correctly to ensure that the VCO
 *       output frequency is between 100 and 432 MHz.
 */
#define _CLK_CFG__PLL_N                 __CLK_PLL_N__

/*
 * PLL output division factor (PLLP)
 * {2, 4, 6, or 8}
 *
 * PLLP: specifies the division factor for main system clock (SYSCLK)
 *       This parameter must be a number in the range {2, 4, 6, or 8}.
 * NOTE: You have to set the PLLP parameter correctly to not exceed 168 MHz on
 *       the System clock frequency.
 */
#define _CLK_CFG__PLL_P                 __CLK_PLL_P__

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
#define _CLK_CFG__PLL_Q                 __CLK_PLL_Q__

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
#define _CLK_CFG__PLL_R                 __CLK_PLL_R__


/*------------------------------------------------------------------------------
 * PLLI2S CONFIGURATION
 *----------------------------------------------------------------------------*/
/*
 * PLLI2S Enable
 * ENABLE
 * DISABLE
 */
#define _CLK_CFG__PLLI2S_ON             __CLK_PLLI2S_ON__

/*
 * PLLI2S division factor for VCO input clock
 * 2..63
 *
 * PLLI2SM: specifies the division factor for PLLI2S VCO input clock
 *          This parameter must be a number between Min_Data = 2 and Max_Data = 63.
 *
 * NOTE:    You have to set the PLLI2SM parameter correctly to ensure that the VCO input
 *          frequency ranges from 1 to 2 MHz. It is recommended to select a frequency
 *          of 2 MHz to limit PLLI2S jitter.
 */
#define _CLK_CFG__PLLI2S_M              __CLK_PLLI2S_M__

/*
 * PLLI2S multiplication factor
 * 50..432
 *
 * PLLI2SN: specifies the multiplication factor for PLLI2S VCO output clock
 *          This parameter must be a number between 50 and 432.
 *
 * NOTE:    You have to set the PLLI2SN parameter correctly to ensure that the VCO
 *          output frequency is between 100 and 432 MHz.
 */
#define _CLK_CFG__PLLI2S_N              __CLK_PLLI2S_N__

/*
 * PLLI2S division factor for 48MHz
 * {2, 4, 6, or 8}
 *
 * PLLI2SP: specifies the division factor for PLL 48Mhz clock output
 *          This parameter must be a number in the range {2, 4, 6, or 8}.
 */
#define _CLK_CFG__PLLI2S_P              __CLK_PLLI2S_P__

/*
 * PLLI2S division factor for SAI1 clock
 * 2..15
 *
 * PLLI2SQ: specifies the division factor for SAI1 clock
 *          This parameter must be a number between 2 and 15.
 */
#define _CLK_CFG__PLLI2S_Q              __CLK_PLLI2S_Q__

/*
 * PLLI2S division factor for I2S clock
 * 2..7
 *
 * PLLI2SR: specifies the division factor for I2S clock
 *          This parameter must be a number between 2 and 7.
 *
 * NOTE:    You have to set the PLLI2SR parameter correctly to not exceed 192 MHz
 *          on the I2S clock frequency.
 */
#define _CLK_CFG__PLLI2S_R              __CLK_PLLI2S_R__


/*------------------------------------------------------------------------------
 * PLLSAI CONFIGURATION
 *----------------------------------------------------------------------------*/
/*
 * PLLSAI Enable
 * ENABLE
 * DISABLE
 */
#define _CLK_CFG__PLLSAI_ON             __CLK_PLLSAI_ON__

/*
 * PLLSAI division factor for VCO input clock
 * 2..63
 *
 * PLLSAIM: specifies the division factor for PLLSAI VCO input clock
 *          This parameter must be a number between Min_Data = 2 and Max_Data = 63.
 *
 * NOTE:    You have to set the PLLSAIM parameter correctly to ensure that the VCO input
 *          frequency ranges from 1 to 2 MHz. It is recommended to select a frequency
 *          of 2 MHz to limit PLLSAI jitter.
 */
#define _CLK_CFG__PLLSAI_M              __CLK_PLLSAI_M__

/*
 * PLLSAI multiplication factor
 * 50..432
 *
 * PLLSAIN: specifies the multiplication factor for PLLSAI VCO output clock
 *          This parameter must be a number between 50 and 432.
 *
 * NOTE:    You have to set the PLLSAIN parameter correctly to ensure that the VCO
 *          output frequency is between 100 and 432 MHz.
 */
#define _CLK_CFG__PLLSAI_N              __CLK_PLLSAI_N__

/*
 * PLLSAI division factor for 48MHz
 * {2, 4, 6, or 8}
 *
 * PLLSAIP: specifies the division factor for PLL 48Mhz clock output
 *          This parameter must be a number in the range {2, 4, 6, or 8}.
 */
#define _CLK_CFG__PLLSAI_P              __CLK_PLLSAI_P__

/*
 * PLLSAI division factor for SAI1 clock
 * 2..15
 *
 * PLLSAIQ: specifies the division factor for SAI1 clock
 *          This parameter must be a number between 2 and 15.
 */
#define _CLK_CFG__PLLSAI_Q              __CLK_PLLSAI_Q__

/*
 * PLLSAI division factor for LTDC clock
 * 2..7
 *
 * PLLSAIR: specifies the division factor for LTDC clock
 *          This parameter must be a number between 2 and 7.
 */
#define _CLK_CFG__PLLSAI_R              __CLK_PLLSAI_R__


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
