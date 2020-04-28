/*=========================================================================*//**
@file    clk_flags.h

@author  Daniel Zorychta

@brief   PLL module configuration flags.

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

/*
 * NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as suffix
 *       and prefix) are exported to the single configuration file
 *       (by using Configtool) when entire project configuration is exported.
 *       All other flag definitions and statements are ignored.
 */
#ifndef _CLK_FLAGS_H_
#define _CLK_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 3, "Home > Microcontroller > CLK",
               function() this:LoadFile("arch/arch_flags.h") end)

this:SetEvent("PostDiscard", function() this:CalculateFreq() end)
++*/

//==============================================================================
// Oscillators Configuration
//==============================================================================
/*--
this:AddExtraWidget("Label", "LABEL_OSCILLATORS", "Oscillators", -1, "bold")
this:AddExtraWidget("Void", "VoidOsc1")
this:AddExtraWidget("Void", "VoidOsc2")
++*/
/*--
this:AddWidget("Combobox", "LSI oscillator")
this:AddItem("Disable", "DISABLE")
this:AddItem("Enable", "ENABLE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_LSI_ON", "")
--*/
#define __CLK_LSI_ON__ ENABLE

/*--
this:AddWidget("Combobox", "LSE oscillator")
this:AddItem("Disable", "DISABLE")
this:AddItem("Enable", "ENABLE")
this:AddItem("Baypass", "_CLK_BAYPASS")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_LSE_ON", "")
--*/
#define __CLK_LSE_ON__ ENABLE

/*--
this:AddWidget("Combobox", "HSE oscillator")
this:AddItem("Disable", "DISABLE")
this:AddItem("Enable", "ENABLE")
this:AddItem("Baypass", "_CLK_BAYPASS")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_HSE_ON", "")
--*/
#define __CLK_HSE_ON__ _CLK_BAYPASS

//==============================================================================
// PLL Clock source Configuration
//==============================================================================
/*--
this:AddExtraWidget("Label", "LABEL_PLL_CLKSRC", "\nPLLs clock source", -1, "bold")
this:AddExtraWidget("Void", "VoidPLLSRC1")
this:AddExtraWidget("Void", "VoidPLLSRC2")
++*/
/*--
this:AddWidget("Combobox", "Clock source")
this:AddItem("HSI",  "LL_RCC_PLLSOURCE_HSI")
this:AddItem("HSE", "LL_RCC_PLLSOURCE_HSE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_SRC", "")
--*/
#define __CLK_PLL_SRC__ LL_RCC_PLLSOURCE_HSI

/*--
this:AddWidget("Combobox", "Clock divider (M)")
for i=2,63 do this:AddItem(tostring(i), "LL_RCC_PLLM_DIV_"..tostring(i)) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_SRC_DIV_M", "")
--*/
#define __CLK_PLL_SRC_DIV_M__ LL_RCC_PLLM_DIV_8

//==============================================================================
// PLL Configuration
//==============================================================================
/*--
this:AddExtraWidget("Label", "LABEL_PLL", "\nMain PLL Configuration", -1, "bold")
this:AddExtraWidget("Void", "VoidPLL1")
this:AddExtraWidget("Void", "VoidPLL2")
++*/
/*--
this:AddWidget("Combobox", "PLL")
this:AddItem("Enable",  "ENABLE")
this:AddItem("Disable", "DISABLE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_ON", "")
--*/
#define __CLK_PLL_ON__ ENABLE

/*--
this:AddWidget("Combobox", "Clock multiplier (N)")
for i=50,432 do this:AddItem(tostring(i), tostring(i)) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_N", "")
--*/
#define __CLK_PLL_N__ 216

/*--
this:AddWidget("Combobox", "Main clock divider (P)")
this:AddItem("2",  "LL_RCC_PLLP_DIV_2")
this:AddItem("4",  "LL_RCC_PLLP_DIV_4")
this:AddItem("6",  "LL_RCC_PLLP_DIV_6")
this:AddItem("8",  "LL_RCC_PLLP_DIV_8")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_P", "")
--*/
#define __CLK_PLL_P__ LL_RCC_PLLP_DIV_2

/*--
this:AddWidget("Combobox", "48MHz domain clock divider (Q)")
for i=2,15 do this:AddItem(tostring(i), "LL_RCC_PLLQ_DIV_"..tostring(i)) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_Q", "")
--*/
#define __CLK_PLL_Q__ LL_RCC_PLLQ_DIV_9

/*--
this:AddWidget("Combobox", "DSI domain clock divider (R)")
for i=2,7 do this:AddItem(tostring(i), "LL_RCC_PLLR_DIV_"..tostring(i)) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_R", "")
--*/
#define __CLK_PLL_R__ LL_RCC_PLLR_DIV_7

//==============================================================================
// PLLI2S Configuration
//==============================================================================
/*--
this:AddExtraWidget("Label", "LABEL_PLLI2S", "\nPLLI2S Configuration", -1, "bold")
this:AddExtraWidget("Void", "VoidPLLI2S1")
this:AddExtraWidget("Void", "VoidPLLI2S2")
++*/
/*--
this:AddWidget("Combobox", "PLLI2S")
this:AddItem("Enable",  "ENABLE")
this:AddItem("Disable", "DISABLE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLI2S_ON", "")
--*/
#define __CLK_PLLI2S_ON__ ENABLE

/*--
this:AddWidget("Combobox", "Clock multiplier (N)")
for i=50,432 do this:AddItem(tostring(i), tostring(i)) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLI2S_N", "")
--*/
#define __CLK_PLLI2S_N__ 192

/*--
this:AddWidget("Combobox", "Clock divider (P)")
this:AddItem("2",  "LL_RCC_PLLI2SP_DIV_2")
this:AddItem("4",  "LL_RCC_PLLI2SP_DIV_4")
this:AddItem("6",  "LL_RCC_PLLI2SP_DIV_6")
this:AddItem("8",  "LL_RCC_PLLI2SP_DIV_8")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLI2S_P", "")
--*/
#define __CLK_PLLI2S_P__ LL_RCC_PLLI2SP_DIV_2

/*--
this:AddWidget("Combobox", "Clock divider (Q)")
for i=2,15 do this:AddItem(tostring(i), "LL_RCC_PLLI2SQ_DIV_"..tostring(i)) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLI2S_Q", "")
--*/
#define __CLK_PLLI2S_Q__ LL_RCC_PLLI2SQ_DIV_2

/*--
this:AddWidget("Combobox", "Clock divider (R)")
for i=2,7 do this:AddItem(tostring(i), "LL_RCC_PLLI2SR_DIV_"..tostring(i)) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLI2S_R", "")
--*/
#define __CLK_PLLI2S_R__ LL_RCC_PLLI2SR_DIV_2

/*--
this:AddWidget("Combobox", "PLLI2SQ Divider")
for i=1,32 do this:AddItem(tostring(i), "LL_RCC_PLLI2SDIVQ_DIV_"..tostring(i)) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLI2S_Q_DIV", "")
--*/
#define __CLK_PLLI2S_Q_DIV__ LL_RCC_PLLI2SDIVQ_DIV_2

//==============================================================================
// PLLSAI Configuration
//==============================================================================
/*--
this:AddExtraWidget("Label", "LABEL_PLLSAI", "\nPLLSAI Configuration", -1, "bold")
this:AddExtraWidget("Void", "VoidPLLSAI1")
this:AddExtraWidget("Void", "VoidPLLSAI2")
++*/
/*--
this:AddWidget("Combobox", "PLLSAI")
this:AddItem("Enable",  "ENABLE")
this:AddItem("Disable", "DISABLE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLSAI_ON", "")
--*/
#define __CLK_PLLSAI_ON__ ENABLE

/*--
this:AddWidget("Combobox", "Clock multiplier (N)")
for i=50,432 do this:AddItem(tostring(i), tostring(i)) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLSAI_N", "")
--*/
#define __CLK_PLLSAI_N__ 96

/*--
this:AddWidget("Combobox", "48M clock divider (P)")
this:AddItem("2",  "LL_RCC_PLLSAIP_DIV_2")
this:AddItem("4",  "LL_RCC_PLLSAIP_DIV_4")
this:AddItem("6",  "LL_RCC_PLLSAIP_DIV_6")
this:AddItem("8",  "LL_RCC_PLLSAIP_DIV_8")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLSAI_P", "")
--*/
#define __CLK_PLLSAI_P__ LL_RCC_PLLSAIP_DIV_4

/*--
this:AddWidget("Combobox", "SAI clock divider (Q)")
for i=2,15 do this:AddItem(tostring(i), "LL_RCC_PLLSAIQ_DIV_"..tostring(i)) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLSAI_Q", "")
--*/
#define __CLK_PLLSAI_Q__ LL_RCC_PLLSAIQ_DIV_12

/*--
this:AddWidget("Combobox", "LTDC clock divider (R)")
for i=2,7 do this:AddItem(tostring(i), "LL_RCC_PLLSAIR_DIV_"..tostring(i)) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLSAI_R", "")
--*/
#define __CLK_PLLSAI_R__ LL_RCC_PLLSAIR_DIV_7

/*--
this:AddWidget("Combobox", "PLLSAIQ Divider")
for i=1,32 do this:AddItem(tostring(i), "LL_RCC_PLLSAIDIVQ_DIV_"..tostring(i)) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLSAI_Q_DIV", "")
--*/
#define __CLK_PLLSAI_Q_DIV__ LL_RCC_PLLSAIDIVQ_DIV_2

/*--
this:AddWidget("Combobox", "PLLSAIR Divider")
this:AddItem("2",  "LL_RCC_PLLSAIDIVR_DIV_2")
this:AddItem("4",  "LL_RCC_PLLSAIDIVR_DIV_4")
this:AddItem("8",  "LL_RCC_PLLSAIDIVR_DIV_8")
this:AddItem("16",  "LL_RCC_PLLSAIDIVR_DIV_16")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLLSAI_R_DIV", "")
--*/
#define __CLK_PLLSAI_R_DIV__ LL_RCC_PLLSAIDIVR_DIV_2

//==============================================================================
// Clock Sources Configuration
//==============================================================================
/*--
this:AddExtraWidget("Label", "LABEL_ClkSrc", "\nClock sources", -1, "bold")
this:AddExtraWidget("Void", "VoidClkSrc1")
this:AddExtraWidget("Void", "VoidClkSrc2")
++*/
/*--
this:AddWidget("Combobox", "System clock source")
this:AddItem("HSI clock", "LL_RCC_SYS_CLKSOURCE_HSI")
this:AddItem("HSE clock", "LL_RCC_SYS_CLKSOURCE_HSE")
this:AddItem("PLL clock", "LL_RCC_SYS_CLKSOURCE_PLL")

this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_SYSCLK_SRC", "")
--*/
#define __CLK_SYS_CLK_SRC__ LL_RCC_SYS_CLKSOURCE_PLL

/*--
this:AddWidget("Combobox", "AHB prescaler")
this:AddItem("SYSCLK / 1",  "LL_RCC_SYSCLK_DIV_1")
this:AddItem("SYSCLK / 2",  "LL_RCC_SYSCLK_DIV_2")
this:AddItem("SYSCLK / 4",  "LL_RCC_SYSCLK_DIV_4")
this:AddItem("SYSCLK / 8",  "LL_RCC_SYSCLK_DIV_8")
this:AddItem("SYSCLK / 16", "LL_RCC_SYSCLK_DIV_16")
this:AddItem("SYSCLK / 64", "LL_RCC_SYSCLK_DIV_64")
this:AddItem("SYSCLK / 128", "LL_RCC_SYSCLK_DIV_128")
this:AddItem("SYSCLK / 256", "LL_RCC_SYSCLK_DIV_256")
this:AddItem("SYSCLK / 512", "LL_RCC_SYSCLK_DIV_512")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_AHB_PRE", "")
--*/
#define __CLK_AHB_PRE__ LL_RCC_SYSCLK_DIV_1

/*--
this:AddWidget("Combobox", "APB1 prescaler")
this:AddItem("HCLK / 1",  "LL_RCC_APB1_DIV_1")
this:AddItem("HCLK / 2",  "LL_RCC_APB1_DIV_2")
this:AddItem("HCLK / 4",  "LL_RCC_APB1_DIV_4")
this:AddItem("HCLK / 8",  "LL_RCC_APB1_DIV_8")
this:AddItem("HCLK / 16", "LL_RCC_APB1_DIV_16")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_APB1_PRE", "")
--*/
#define __CLK_APB1_PRE__ LL_RCC_APB1_DIV_4

/*--
this:AddWidget("Combobox", "APB2 prescaler")
this:AddItem("HCLK / 1",  "LL_RCC_APB2_DIV_1")
this:AddItem("HCLK / 2",  "LL_RCC_APB2_DIV_2")
this:AddItem("HCLK / 4",  "LL_RCC_APB2_DIV_4")
this:AddItem("HCLK / 8",  "LL_RCC_APB2_DIV_8")
this:AddItem("HCLK / 16", "LL_RCC_APB2_DIV_16")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_APB2_PRE", "")
--*/
#define __CLK_APB2_PRE__ LL_RCC_APB2_DIV_2

/*--
this:AddWidget("Combobox", "RTC Clock source")
this:AddItem("None", "LL_RCC_RTC_CLKSOURCE_NONE")
this:AddItem("LSE clock", "LL_RCC_RTC_CLKSOURCE_LSE")
this:AddItem("LSI clock", "LL_RCC_RTC_CLKSOURCE_LSI")
this:AddItem("HSE clock", "LL_RCC_RTC_CLKSOURCE_HSE_RTC")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_RTC_SRC", "")
--*/
#define __CLK_RTC_SRC__ LL_RCC_RTC_CLKSOURCE_LSE

/*--
this:AddWidget("Combobox", "RTC HSE Clock divider")
this:AddItem("No clock", "LL_RCC_RTC_NOCLOCK")
for i=2,31 do this:AddItem(tostring(i), "LL_RCC_RTC_HSE_DIV_"..tostring(i)) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_RTC_HSE_DIV", "")
--*/
#define __CLK_RTC_HSE_DIV__ LL_RCC_RTC_NOCLOCK

/*--
this:AddWidget("Combobox", "MCO1 Clock source")
if uC.NAME:match("STM32F410") then this:AddItem("Disable", "0") end
this:AddItem("HSI", "LL_RCC_MCO1_CLKSOURCE_HSI")
this:AddItem("LSE", "LL_RCC_MCO1_CLKSOURCE_LSE")
this:AddItem("HSE", "LL_RCC_MCO1_CLKSOURCE_HSE")
this:AddItem("PLLCLK", "LL_RCC_MCO1_CLKSOURCE_PLLCLK")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_MCO1_SRC", "")
--*/
#define __CLK_MCO1_SRC__ LL_RCC_MCO1_CLKSOURCE_HSE

/*--
this:AddWidget("Combobox", "MCO1 Clock divider (output)")
for i = 1, 5 do this:AddItem("MCO divided by "..i, "LL_RCC_MCO1_DIV_"..tostring(i)) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_MCO1_DIV", "")
--*/
#define __CLK_MCO1_DIV__ LL_RCC_MCO1_DIV_4

/*--
this:AddWidget("Combobox", "MCO2 Clock source")
this:AddItem("SYSCLK", "LL_RCC_MCO2_CLKSOURCE_SYSCLK")
this:AddItem("PLLI2S", "LL_RCC_MCO2_CLKSOURCE_PLLI2S")
this:AddItem("HSE", "LL_RCC_MCO2_CLKSOURCE_HSE")
this:AddItem("PLLCLK", "LL_RCC_MCO2_CLKSOURCE_PLLCLK")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_MCO2_SRC", "")
--*/
#define __CLK_MCO2_SRC__ LL_RCC_MCO2_CLKSOURCE_HSE

/*--
this:AddWidget("Combobox", "MCO2 Clock divisor (output)")
for i = 1, 5 do this:AddItem("MCO divided by "..i, "LL_RCC_MCO2_DIV_"..tostring(i)) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_MCO2_DIV", "")
--*/
#define __CLK_MCO2_DIV__ LL_RCC_MCO2_DIV_1

/*--
this:AddWidget("Combobox", "I2S clock source")
this:AddItem("I2S PLL clock", "LL_RCC_I2S1_CLKSOURCE_PLLI2SR")
this:AddItem("I2S_CKIN Pin", "LL_RCC_I2S1_CLKSOURCE_PIN")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_I2S1_SRC", "")
--*/
#define __CLK_I2S1_SRC__ LL_RCC_I2S1_CLKSOURCE_PLLI2SR

/*--
this:AddWidget("Combobox", "SAI1 clock source")
this:AddItem("SAI PLL clock", "LL_RCC_SAI1_CLKSOURCE_PLLSAIQDIV")
this:AddItem("I2S PLL clock", "LL_RCC_SAI1_CLKSOURCE_PLLI2SQDIV")
this:AddItem("I2S_CKIN Pin", "LL_RCC_SAI1_CLKSOURCE_PIN")
this:AddItem("PLL Source clock", "LL_RCC_SAI1_CLKSOURCE_PLLSRC")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_SAI1_SRC", "")
--*/
#define __CLK_SAI1_SRC__ LL_RCC_SAI1_CLKSOURCE_PLLSAIQDIV

/*--
this:AddWidget("Combobox", "SAI2 clock source")
this:AddItem("SAI PLL clock", "LL_RCC_SAI2_CLKSOURCE_PLLSAIQDIV")
this:AddItem("I2S PLL clock", "LL_RCC_SAI2_CLKSOURCE_PLLI2SQDIV")
this:AddItem("I2S_CKIN Pin", "LL_RCC_SAI2_CLKSOURCE_PIN")
this:AddItem("PLL Source clock", "LL_RCC_SAI2_CLKSOURCE_PLLSRC")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_SAI2_SRC", "")
--*/
#define __CLK_SAI2_SRC__ LL_RCC_SAI2_CLKSOURCE_PLLSAIQDIV

/*--
this:AddWidget("Combobox", "DFSDM Audio clock source")
this:AddItem("SAI1 clock", "LL_RCC_DFSDM1_AUDIO_CLKSOURCE_SAI1")
this:AddItem("SAI2 clock", "LL_RCC_DFSDM1_AUDIO_CLKSOURCE_SAI2")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_DFSDM1_AUDIO_SRC", "")
--*/
#define __CLK_DFSDM1_AUDIO_SRC__ LL_RCC_DFSDM1_AUDIO_CLKSOURCE_SAI1

/*--
this:AddWidget("Combobox", "DFSDM clock source")
this:AddItem("PCLK2 clock", "LL_RCC_DFSDM1_CLKSOURCE_PCLK2")
this:AddItem("SYSCLK clock", "LL_RCC_DFSDM1_CLKSOURCE_SYSCLK")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_DFSDM1_SRC", "")
--*/
#define __CLK_DFSDM1_SRC__ LL_RCC_DFSDM1_CLKSOURCE_PCLK2

/*--
this:AddWidget("Combobox", "DSI clock source")
this:AddItem("PHY clock",  "LL_RCC_DSI_CLKSOURCE_PHY")
this:AddItem("PLLR clock", "LL_RCC_DSI_CLKSOURCE_PLLR")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_DSI_SRC", "")
--*/
#define __CLK_DSI_SRC__ LL_RCC_DSI_CLKSOURCE_PHY

/*--
this:AddWidget("Combobox", "USB clock source")
this:AddItem("PLLQ", "LL_RCC_USB_CLKSOURCE_PLLQ")
this:AddItem("PLLSAIP", "LL_RCC_USB_CLKSOURCE_PLLSAIP")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_USB_SRC", "")
--*/
#define __CLK_USB_SRC__ LL_RCC_USB_CLKSOURCE_PLLQ

/*--
this:AddWidget("Combobox", "SDMMC1 clock source")
this:AddItem("PLL48CLK", "LL_RCC_SDMMC1_CLKSOURCE_PLL48CLK")
this:AddItem("SYSCLK", "LL_RCC_SDMMC1_CLKSOURCE_SYSCLK")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_SDMMC1_SRC", "")
--*/
#define __CLK_SDMMC1_SRC__ LL_RCC_SDMMC1_CLKSOURCE_PLL48CLK

/*--
this:AddWidget("Combobox", "SDMMC2 clock source")
this:AddItem("PLL48CLK", "LL_RCC_SDMMC2_CLKSOURCE_PLL48CLK")
this:AddItem("SYSCLK", "LL_RCC_SDMMC2_CLKSOURCE_SYSCLK")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_SDMMC2_SRC", "")
--*/
#define __CLK_SDMMC2_SRC__ LL_RCC_SDMMC2_CLKSOURCE_PLL48CLK

/*--
this:AddWidget("Combobox", "HDMI-CEC clock source")
this:AddItem("HSI / 488", "LL_RCC_CEC_CLKSOURCE_HSI_DIV488")
this:AddItem("LSE", "LL_RCC_CEC_CLKSOURCE_LSE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CEC_SRC", "")
--*/
#define __CLK_CEC_SRC__ LL_RCC_CEC_CLKSOURCE_HSI_DIV488

/*--
this:AddWidget("Combobox", "LPTimer clock source")
this:AddItem("PCLK1", "LL_RCC_LPTIM1_CLKSOURCE_PCLK1")
this:AddItem("LSI", "LL_RCC_LPTIM1_CLKSOURCE_LSI")
this:AddItem("HSI", "LL_RCC_LPTIM1_CLKSOURCE_HSI")
this:AddItem("LSE", "LL_RCC_LPTIM1_CLKSOURCE_LSE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_LPTIM1_SRC", "")
--*/
#define __CLK_LPTIM1_SRC__ LL_RCC_LPTIM1_CLKSOURCE_PCLK1

/*--
this:AddWidget("Combobox", "USART1 clock source")
this:AddItem("PCLK2", "LL_RCC_USART1_CLKSOURCE_PCLK2")
this:AddItem("SYSCLK", "LL_RCC_USART1_CLKSOURCE_SYSCLK")
this:AddItem("HSI", "LL_RCC_USART1_CLKSOURCE_HSI")
this:AddItem("LSE", "LL_RCC_USART1_CLKSOURCE_LSE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_USART1_SRC", "")
--*/
#define __CLK_USART1_SRC__ LL_RCC_USART1_CLKSOURCE_PCLK2

/*--
this:AddWidget("Combobox", "USART2 clock source")
this:AddItem("PCLK1", "LL_RCC_USART2_CLKSOURCE_PCLK1")
this:AddItem("SYSCLK", "LL_RCC_USART2_CLKSOURCE_SYSCLK")
this:AddItem("HSI", "LL_RCC_USART2_CLKSOURCE_HSI")
this:AddItem("LSE", "LL_RCC_USART2_CLKSOURCE_LSE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_USART2_SRC", "")
--*/
#define __CLK_USART2_SRC__ LL_RCC_USART2_CLKSOURCE_PCLK1

/*--
this:AddWidget("Combobox", "USART3 clock source")
this:AddItem("PCLK1", "LL_RCC_USART3_CLKSOURCE_PCLK1")
this:AddItem("SYSCLK", "LL_RCC_USART3_CLKSOURCE_SYSCLK")
this:AddItem("HSI", "LL_RCC_USART3_CLKSOURCE_HSI")
this:AddItem("LSE", "LL_RCC_USART3_CLKSOURCE_LSE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_USART3_SRC", "")
--*/
#define __CLK_USART3_SRC__ LL_RCC_USART3_CLKSOURCE_PCLK1

/*--
this:AddWidget("Combobox", "UART4 clock source")
this:AddItem("PCLK1", "LL_RCC_UART4_CLKSOURCE_PCLK1")
this:AddItem("SYSCLK", "LL_RCC_UART4_CLKSOURCE_SYSCLK")
this:AddItem("HSI", "LL_RCC_UART4_CLKSOURCE_HSI")
this:AddItem("LSE", "LL_RCC_UART4_CLKSOURCE_LSE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_UART4_SRC", "")
--*/
#define __CLK_UART4_SRC__ LL_RCC_UART4_CLKSOURCE_PCLK1

/*--
this:AddWidget("Combobox", "UART5 clock source")
this:AddItem("PCLK1", "LL_RCC_UART5_CLKSOURCE_PCLK1")
this:AddItem("SYSCLK", "LL_RCC_UART5_CLKSOURCE_SYSCLK")
this:AddItem("HSI", "LL_RCC_UART5_CLKSOURCE_HSI")
this:AddItem("LSE", "LL_RCC_UART5_CLKSOURCE_LSE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_UART5_SRC", "")
--*/
#define __CLK_UART5_SRC__ LL_RCC_UART5_CLKSOURCE_PCLK1

/*--
this:AddWidget("Combobox", "USART6 clock source")
this:AddItem("PCLK2", "LL_RCC_USART6_CLKSOURCE_PCLK2")
this:AddItem("SYSCLK", "LL_RCC_USART6_CLKSOURCE_SYSCLK")
this:AddItem("HSI", "LL_RCC_USART6_CLKSOURCE_HSI")
this:AddItem("LSE", "LL_RCC_USART6_CLKSOURCE_LSE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_USART6_SRC", "")
--*/
#define __CLK_USART6_SRC__ LL_RCC_USART6_CLKSOURCE_PCLK2

/*--
this:AddWidget("Combobox", "UART7 clock source")
this:AddItem("PCLK1", "LL_RCC_UART7_CLKSOURCE_PCLK1")
this:AddItem("SYSCLK", "LL_RCC_UART7_CLKSOURCE_SYSCLK")
this:AddItem("HSI", "LL_RCC_UART7_CLKSOURCE_HSI")
this:AddItem("LSE", "LL_RCC_UART7_CLKSOURCE_LSE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_UART7_SRC", "")
--*/
#define __CLK_UART7_SRC__ LL_RCC_UART7_CLKSOURCE_PCLK1

/*--
this:AddWidget("Combobox", "UART8 clock source")
this:AddItem("PCLK1", "LL_RCC_UART8_CLKSOURCE_PCLK1")
this:AddItem("SYSCLK", "LL_RCC_UART8_CLKSOURCE_SYSCLK")
this:AddItem("HSI", "LL_RCC_UART8_CLKSOURCE_HSI")
this:AddItem("LSE", "LL_RCC_UART8_CLKSOURCE_LSE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_UART8_SRC", "")
--*/
#define __CLK_UART8_SRC__ LL_RCC_UART8_CLKSOURCE_PCLK1

/*--
this:AddWidget("Combobox", "I2C1 clock source")
this:AddItem("PCLK1", "LL_RCC_I2C1_CLKSOURCE_PCLK1")
this:AddItem("SYSCLK", "LL_RCC_I2C1_CLKSOURCE_SYSCLK")
this:AddItem("HSI", "LL_RCC_I2C1_CLKSOURCE_HSI")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_I2C1_SRC", "")
--*/
#define __CLK_I2C1_SRC__ LL_RCC_I2C1_CLKSOURCE_PCLK1

/*--
this:AddWidget("Combobox", "I2C2 clock source")
this:AddItem("PCLK1", "LL_RCC_I2C2_CLKSOURCE_PCLK1")
this:AddItem("SYSCLK", "LL_RCC_I2C2_CLKSOURCE_SYSCLK")
this:AddItem("HSI", "LL_RCC_I2C2_CLKSOURCE_HSI")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_I2C2_SRC", "")
--*/
#define __CLK_I2C2_SRC__ LL_RCC_I2C2_CLKSOURCE_PCLK1

/*--
this:AddWidget("Combobox", "I2C3 clock source")
this:AddItem("PCLK1", "LL_RCC_I2C3_CLKSOURCE_PCLK1")
this:AddItem("SYSCLK", "LL_RCC_I2C3_CLKSOURCE_SYSCLK")
this:AddItem("HSI", "LL_RCC_I2C3_CLKSOURCE_HSI")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_I2C3_SRC", "")
--*/
#define __CLK_I2C3_SRC__ LL_RCC_I2C3_CLKSOURCE_PCLK1

/*--
this:AddWidget("Combobox", "I2C4 clock source")
this:AddItem("PCLK1", "LL_RCC_I2C4_CLKSOURCE_PCLK1")
this:AddItem("SYSCLK", "LL_RCC_I2C4_CLKSOURCE_SYSCLK")
this:AddItem("HSI", "LL_RCC_I2C4_CLKSOURCE_HSI")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_I2C4_SRC", "")
--*/
#define __CLK_I2C4_SRC__ LL_RCC_I2C4_CLKSOURCE_PCLK1

//==============================================================================
// Flash Latency Configuration
//==============================================================================
/*--
this:AddExtraWidget("Label", "LABEL_FlashLatency", "\nFlash latency", -1, "bold")
this:AddExtraWidget("Void", "VoidFL1")
this:AddExtraWidget("Void", "VoidFL2")
++*/
/*--
this:AddWidget("Combobox", "CPU voltage")
this:AddItem("2.7V - 3.6V",  "2p7V")
this:AddItem("2.4V - 2.7V",  "2p4V")
this:AddItem("2.1V - 2.4V",  "2p1V")
this:AddItem("1.8V - 2.1V",  "1p8V")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:SetToolTip("Select power voltage level to calculate FLASH latency.")
this:AddExtraWidget("Label", "LABEL_FLASH_LATENCY", "")
--*/
#define __CLK_CPU_VOLTAGE__ 2p7V

/*--
-- Value calculated automatically
this:AddWidget("Value")
--*/
#define __CLK_FLASH_LATENCY__ 7


//==============================================================================
// Calculation script
//==============================================================================
/*--
this.CalculateFreq = function(self)
    local freq = {}
    freq.NONE = 0

    -- HSE clock frequency -----------------------------------------------------
    local HSEON = iff(this:GetFlagValue("__CLK_HSE_ON__") == "DISABLE", 0, 1)
    freq.HSE = uC.OSCFREQ * HSEON
    this:SetFlagValue("LABEL_HSE_ON", PrintFrequency(freq.HSE).." (HSE)")

    local RTC_HSE_DIV = this:GetFlagValue("__CLK_RTC_HSE_DIV__"):gsub("LL_RCC_RTC_HSE_DIV_", "")
    if RTC_HSE_DIV == "LL_RCC_RTC_NOCLOCK" then
        freq.HSE_RTC = 0
    else
        freq.HSE_RTC = freq.HSE / RTC_HSE_DIV
    end

    -- HSI clock frequency -----------------------------------------------------
    freq.HSI = 16e6
    freq.HSI_DIV488 = freq.HSI / 488

    -- LSE clock frequency -----------------------------------------------------
    local LSEON = iff(this:GetFlagValue("__CLK_LSE_ON__") == "DISABLE", 0, 1)
    freq.LSE = 32768 * LSEON
    this:SetFlagValue("LABEL_LSE_ON", PrintFrequency(freq.LSE).." (LSE)")

    -- LSI clock frequency -----------------------------------------------------
    local LSION = iff(this:GetFlagValue("__CLK_LSI_ON__") == "DISABLE", 0, 1)
    freq.LSI = 32e3 * LSION
    this:SetFlagValue("LABEL_LSI_ON", PrintFrequency(freq.LSI).." (LSI)")

    -- I2S_CKIN/SAI_CKIN clock frequency ---------------------------------------
    freq.I2S_CKIN  = 12288000
    freq.SAI_CLKIN = 48000
    freq.PIN = freq.SAI_CLKIN

    -- DSI PHY clock frequency -------------------------------------------------
    freq.PHY  = 25.6e6

    -- PLLs clock source frequency ---------------------------------------------
    local PLLSRC = this:GetFlagValue("__CLK_PLL_SRC__")
    local PLLM   = this:GetFlagValue("__CLK_PLL_SRC_DIV_M__"):gsub("LL_RCC_PLLM_DIV_", "")

    if PLLSRC == "LL_RCC_PLLSOURCE_HSI" then
       this:SetFlagValue("LABEL_PLL_SRC", PrintFrequency(freq.HSI))
       freq.PLLM = freq.HSI / PLLM
       freq.PLL  = freq.HSI
    else
       this:SetFlagValue("LABEL_PLL_SRC", PrintFrequency(freq.HSE))
       freq.PLLM = freq.HSE / PLLM
       freq.PLL  = freq.HSE
    end

    freq.PLLSRC = freq.PLL

    if freq.PLLM < 1e6 or freq.PLLM > 2e6 then
       this:SetFlagValue("LABEL_PLL_SRC_DIV_M", PrintFrequency(freq.PLLM).." [Keep in range 1-2MHz]")
    else
       this:SetFlagValue("LABEL_PLL_SRC_DIV_M", PrintFrequency(freq.PLLM))
    end

    -- calculate PLL clock frequency -------------------------------------------
    local PLLON = iff(this:GetFlagValue("__CLK_PLL_ON__") == "DISABLE", 0, 1)
    local PLLN  = this:GetFlagValue("__CLK_PLL_N__")
    local PLLP  = this:GetFlagValue("__CLK_PLL_P__"):gsub("LL_RCC_PLLP_DIV_", "")
    local PLLQ  = this:GetFlagValue("__CLK_PLL_Q__"):gsub("LL_RCC_PLLQ_DIV_", "")
    local PLLR  = this:GetFlagValue("__CLK_PLL_R__"):gsub("LL_RCC_PLLR_DIV_", "")

    if PLLON == 1 then
        freq.PLLVCO = freq.PLLM * PLLN
        if freq.PLLVCO < 100e6 or freq.PLLVCO > 432e6 then
           this:SetFlagValue("LABEL_PLL_N", PrintFrequency(freq.PLLVCO).." [Keep in range 100-432MHz]")
        else
           this:SetFlagValue("LABEL_PLL_N", PrintFrequency(freq.PLLVCO))
        end

        freq.PLLCLK = freq.PLLVCO / PLLP
        this:SetFlagValue("LABEL_PLL_P", PrintFrequency(freq.PLLCLK).." (PLLCLK)")

        freq.PLLQ = freq.PLLVCO / PLLQ
        this:SetFlagValue("LABEL_PLL_Q", PrintFrequency(freq.PLLQ).." (PLLQ)")

        freq.PLLR = freq.PLLVCO / PLLR
        freq.PLLDSICLK = freq.PLLR
        this:SetFlagValue("LABEL_PLL_R", PrintFrequency(freq.PLLR))
    else
        freq.PLLCLK    = 0
        freq.PLLQ      = 0
        freq.PLLR      = 0
        freq.PLLDSICLK = 0
        this:SetFlagValue("LABEL_PLL_N", "")
        this:SetFlagValue("LABEL_PLL_P", "")
        this:SetFlagValue("LABEL_PLL_Q", "")
        this:SetFlagValue("LABEL_PLL_R", "")
    end

    -- calculate I2S clock frequency -------------------------------------------
    local PLLI2SON   = iff(this:GetFlagValue("__CLK_PLLI2S_ON__") == "DISABLE", 0, 1)
    local PLLI2SN    = this:GetFlagValue("__CLK_PLLI2S_N__")
    local PLLI2SP    = this:GetFlagValue("__CLK_PLLI2S_P__"):gsub("LL_RCC_PLLI2SP_DIV_", "")
    local PLLI2SQ    = this:GetFlagValue("__CLK_PLLI2S_Q__"):gsub("LL_RCC_PLLI2SQ_DIV_", "")
    local PLLI2SR    = this:GetFlagValue("__CLK_PLLI2S_R__"):gsub("LL_RCC_PLLI2SR_DIV_", "")
    local PLLI2SQDIV = this:GetFlagValue("__CLK_PLLI2S_Q_DIV__"):gsub("LL_RCC_PLLI2SDIVQ_DIV_", "")

    if PLLI2SON == 1 then
        freq.PLLI2SVCO = freq.PLLM * PLLI2SN
        if freq.PLLI2SVCO < 100e6 or freq.PLLI2SVCO > 432e6 then
           this:SetFlagValue("LABEL_PLLI2S_N", PrintFrequency(freq.PLLI2SVCO).." [Keep in range 100-432MHz]")
        else
           this:SetFlagValue("LABEL_PLLI2S_N", PrintFrequency(freq.PLLI2SVCO))
        end

        freq.PLLI2SP = freq.PLLI2SVCO / PLLI2SP
        freq.PLLI2S  = freq.PLLI2SP
        this:SetFlagValue("LABEL_PLLI2S_P", PrintFrequency(freq.PLLI2SP))

        freq.PLLI2SQ = freq.PLLI2SVCO / PLLI2SQ
        this:SetFlagValue("LABEL_PLLI2S_Q", PrintFrequency(freq.PLLI2SQ).." (SPDIFCLK)")

        freq.PLLI2SR = freq.PLLI2SVCO / PLLI2SR
        if freq.PLLI2SR > 192e6 then
            this:SetFlagValue("LABEL_PLLI2S_R", PrintFrequency(freq.PLLI2SR).." (PLLI2SQ) [Max is 192 MHz!]")
        else
            this:SetFlagValue("LABEL_PLLI2S_R", PrintFrequency(freq.PLLI2SR).." (PLLI2SQ)")
        end

        freq.PLLI2SQDIV = freq.PLLI2SQ / PLLI2SQDIV
        this:SetFlagValue("LABEL_PLLI2S_Q_DIV", PrintFrequency(freq.PLLI2SQDIV).." (PLLI2SQDIV)")

    else
        freq.PLLI2SP    = 0
        freq.PLLI2S     = 0
        freq.PLLI2SQ    = 0
        freq.PLLI2SR    = 0
        freq.PLLI2SQDIV = 0
        this:SetFlagValue("LABEL_PLLI2S_N", "")
        this:SetFlagValue("LABEL_PLLI2S_P", "")
        this:SetFlagValue("LABEL_PLLI2S_Q", "")
        this:SetFlagValue("LABEL_PLLI2S_R", "")
        this:SetFlagValue("LABEL_PLLI2S_Q_DIV", "")
    end

    -- calculate PLLSAI clock frequency ----------------------------------------
    local PLLSAION   = iff(this:GetFlagValue("__CLK_PLLSAI_ON__") == "DISABLE", 0, 1)
    local PLLSAIN    = this:GetFlagValue("__CLK_PLLSAI_N__")
    local PLLSAIP    = this:GetFlagValue("__CLK_PLLSAI_P__"):gsub("LL_RCC_PLLSAIP_DIV_", "")
    local PLLSAIQ    = this:GetFlagValue("__CLK_PLLSAI_Q__"):gsub("LL_RCC_PLLSAIQ_DIV_", "")
    local PLLSAIR    = this:GetFlagValue("__CLK_PLLSAI_R__"):gsub("LL_RCC_PLLSAIR_DIV_", "")
    local PLLSAIQDIV = this:GetFlagValue("__CLK_PLLSAI_Q_DIV__"):gsub("LL_RCC_PLLSAIDIVQ_DIV_", "")
    local PLLSAIRDIV = this:GetFlagValue("__CLK_PLLSAI_R_DIV__"):gsub("LL_RCC_PLLSAIDIVR_DIV_", "")

    if PLLSAION == 1 then
        freq.PLLSAIVCO = freq.PLLM * PLLSAIN
        if freq.PLLSAIVCO < 100e6 or freq.PLLSAIVCO > 432e6 then
           this:SetFlagValue("LABEL_PLLSAI_N", PrintFrequency(freq.PLLSAIVCO).." [Keep in range 100-432MHz]")
        else
           this:SetFlagValue("LABEL_PLLSAI_N", PrintFrequency(freq.PLLSAIVCO))
        end

        freq.PLLSAIP = freq.PLLSAIVCO / PLLSAIP
        this:SetFlagValue("LABEL_PLLSAI_P", PrintFrequency(freq.PLLSAIP))

        freq.PLLSAIQ = freq.PLLSAIVCO / PLLSAIQ
        this:SetFlagValue("LABEL_PLLSAI_Q", PrintFrequency(freq.PLLSAIQ).." (PLLSAIQ)")

        freq.PLLSAIR = freq.PLLSAIVCO / PLLSAIR
        this:SetFlagValue("LABEL_PLLSAI_R", PrintFrequency(freq.PLLSAIR).." (PLLSAIR)")

        freq.PLLSAIQDIV = freq.PLLSAIQ / PLLSAIQDIV
        this:SetFlagValue("LABEL_PLLSAI_Q_DIV", PrintFrequency(freq.PLLSAIQDIV).." (PLLSAI)")

        freq.PLLSAIRDIV = freq.PLLSAIR / PLLSAIRDIV
        this:SetFlagValue("LABEL_PLLSAI_R_DIV", PrintFrequency(freq.PLLSAIRDIV).." (LTDC Clock)")

    else
        freq.PLLSAIP    = 0
        freq.PLLSAIQ    = 0
        freq.PLLSAIR    = 0
        freq.PLLSAIQDIV = 0
        freq.PLLSAIRDIV = 0
        this:SetFlagValue("LABEL_PLLSAI_N", "")
        this:SetFlagValue("LABEL_PLLSAI_P", "")
        this:SetFlagValue("LABEL_PLLSAI_Q", "")
        this:SetFlagValue("LABEL_PLLSAI_R", "")
        this:SetFlagValue("LABEL_PLLSAI_Q_DIV", "")
        this:SetFlagValue("LABEL_PLLSAI_R_DIV", "")
    end

    -- calculate SYSCLK --------------------------------------------------------
    local SYSCLKSRC = this:GetFlagValue("__CLK_SYS_CLK_SRC__")
    if SYSCLKSRC == "LL_RCC_SYS_CLKSOURCE_HSE" then
        freq.SYSCLK = freq.HSE
    elseif SYSCLKSRC == "LL_RCC_SYS_CLKSOURCE_HSI" then
        freq.SYSCLK = freq.HSI
    else
        freq.SYSCLK = freq.PLLCLK
    end
    this:SetFlagValue("LABEL_SYSCLK_SRC", PrintFrequency(freq.SYSCLK).." (SYSCLK)")

    -- calculate core and peripheral clocks ------------------------------------
    local AHBPRE  = this:GetFlagValue("__CLK_AHB_PRE__"):gsub("LL_RCC_SYSCLK_DIV_", "")
    local APB1PRE = this:GetFlagValue("__CLK_APB1_PRE__"):gsub("LL_RCC_APB1_DIV_", "")
    local APB2PRE = this:GetFlagValue("__CLK_APB2_PRE__"):gsub("LL_RCC_APB2_DIV_", "")

    freq.HCLK     = freq.SYSCLK / AHBPRE
    freq.PCLK1    = freq.HCLK / APB1PRE
    freq.PCLK2    = freq.HCLK / APB2PRE
    freq.TIMxCLK1 = iff(APB1PRE == 1, freq.PCLK1, freq.PCLK1 * 2)
    freq.TIMxCLK2 = iff(APB2PRE == 1, freq.PCLK1, freq.PCLK1 * 2)

    if freq.HCLK > 216e6 then
       this:SetFlagValue("LABEL_AHB_PRE", PrintFrequency(freq.HCLK).." (HCLK) [max is 216MHz]")
    else
       this:SetFlagValue("LABEL_AHB_PRE", PrintFrequency(freq.HCLK).." (HCLK)")
    end

    if freq.PCLK1 > 54e6 then
       this:SetFlagValue("LABEL_APB1_PRE", PrintFrequency(freq.PCLK1).." (PCLK1) [max is 54MHz]")
    else
       this:SetFlagValue("LABEL_APB1_PRE", PrintFrequency(freq.PCLK1).." (PCLK1)")
    end

    if freq.PCLK2 > 108e6 then
       this:SetFlagValue("LABEL_APB2_PRE", PrintFrequency(freq.PCLK2).." (PCLK2) [max is 108MHz]")
    else
       this:SetFlagValue("LABEL_APB2_PRE", PrintFrequency(freq.PCLK2).." (PCLK2)")
    end

    -- peripherals clock frequency ---------------------------------------------
    local PERIPH = {"LPTIM1", "USB", "SDMMC1", "SDMMC2", "CEC", "RTC", "MCO1", "MCO2",
                    "SAI1", "SAI2", "DFSDM1", "DFSDM1_AUDIO", "I2S1", "DSI",
                    "USART1", "USART2", "USART3", "UART4", "UART5", "USART6", "UART7", "UART8",
                    "I2C1", "I2C2", "I2C3", "I2C4"}

    for i = 1, #PERIPH do
        local PERIPH_CLK_SRC = this:GetFlagValue("__CLK_"..PERIPH[i].."_SRC__")
              PERIPH_CLK_SRC = PERIPH_CLK_SRC:gsub("LL_RCC_"..PERIPH[i].."_CLKSOURCE_", "")

        freq[PERIPH[i]] = freq[PERIPH_CLK_SRC]

        if PERIPH[i] == "USB" then
            freq.PLL48CLK = freq.USB
        end

        this:SetFlagValue("LABEL_"..PERIPH[i].."_SRC", PrintFrequency(freq[PERIPH[i]]))
    end

    -- MCO1, MCO2 pin output frequency -----------------------------------------
    local MCO1_DIV = this:GetFlagValue("__CLK_MCO1_DIV__"):gsub("LL_RCC_MCO1_DIV_", "")
    this:SetFlagValue("LABEL_MCO1_DIV", PrintFrequency(freq.MCO1 / MCO1_DIV))

    local MCO2_DIV = this:GetFlagValue("__CLK_MCO2_DIV__"):gsub("LL_RCC_MCO2_DIV_", "")
    this:SetFlagValue("LABEL_MCO2_DIV", PrintFrequency(freq.MCO2 / MCO2_DIV))

    -- calculate flash latency -------------------------------------------------
    local CPUVOLTAGE = this:GetFlagValue("__CLK_CPU_VOLTAGE__")
    local FLV = 0
    if CPUVOLTAGE == "2p7V" then
        FLV = 30e6
    elseif CPUVOLTAGE == "2p4V" then
        FLV = 24e6
    elseif CPUVOLTAGE == "2p1V" then
        FLV = 22e6
    else --CPUVOLTAGE == "1p8V"
        FLV = 20e6
    end

    freq.FLASHLATENCY = math.ceil(freq.HCLK / FLV) - 1
    if freq.FLASHLATENCY < 0 then freq.FLASHLATENCY = 0 end
    if freq.FLASHLATENCY > 9 then freq.FLASHLATENCY = 9 end
    this:SetFlagValue("LABEL_FLASH_LATENCY", freq.FLASHLATENCY.." Flash wait-states")
    this:SetFlagValue("__CLK_FLASH_LATENCY__", tostring(freq.FLASHLATENCY))

end

-- trigger frequency calculation after loading all flags
this.CalculateFreq()
++*/

#endif /* _CLK_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
