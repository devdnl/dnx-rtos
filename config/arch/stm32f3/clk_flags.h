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
#define __CLK_PLL_SRC__ LL_RCC_PLLSOURCE_HSE

/*--
this:AddWidget("Combobox", "HSE Clock divider")
for i=1,16 do this:AddItem(tostring(i), "LL_RCC_PREDIV_DIV_"..tostring(i)) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_SRC_DIV_M", "")
--*/
#define __CLK_PLL_SRC_DIV_M__ LL_RCC_PREDIV_DIV_1

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
this:AddWidget("Combobox", "Clock multiplier")
for i=2,16 do this:AddItem(tostring(i), "LL_RCC_PLL_MUL_"..tostring(i)) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_N", "")
--*/
#define __CLK_PLL_N__ LL_RCC_PLL_MUL_9

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
#define __CLK_APB2_PRE__ LL_RCC_APB2_DIV_4

/*--
this:AddWidget("Combobox", "RTC Clock source")
this:AddItem("None", "LL_RCC_RTC_CLKSOURCE_NONE")
this:AddItem("LSE clock", "LL_RCC_RTC_CLKSOURCE_LSE")
this:AddItem("LSI clock", "LL_RCC_RTC_CLKSOURCE_LSI")
this:AddItem("HSE clock", "LL_RCC_RTC_CLKSOURCE_HSE_DIV32")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_RTC_SRC", "")
--*/
#define __CLK_RTC_SRC__ LL_RCC_RTC_CLKSOURCE_LSI

/*--
this:AddWidget("Combobox", "MCO Clock source")
this:AddItem("None", "LL_RCC_MCO1SOURCE_NOCLOCK")
this:AddItem("SYSCLK", "LL_RCC_MCO1SOURCE_SYSCLK")
this:AddItem("HSI", "LL_RCC_MCO1SOURCE_HSI")
this:AddItem("HSE", "LL_RCC_MCO1SOURCE_HSE")
this:AddItem("LSI", "LL_RCC_MCO1SOURCE_LSI")
this:AddItem("LSE", "LL_RCC_MCO1SOURCE_LSE")
this:AddItem("PLLCLK", "LL_RCC_MCO1SOURCE_PLLCLK")
this:AddItem("PLLCLK/2", "LL_RCC_MCO1SOURCE_PLLCLK_DIV_2")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_MCO1_SRC", "")
--*/
#define __CLK_MCO1_SRC__ LL_RCC_MCO1SOURCE_NOCLOCK

/*--
this:AddWidget("Combobox", "MCO Clock divider (output)")
this:AddItem("1", "LL_RCC_MCO1_DIV_1")
this:AddItem("2", "LL_RCC_MCO1_DIV_2")
this:AddItem("4", "LL_RCC_MCO1_DIV_4")
this:AddItem("8", "LL_RCC_MCO1_DIV_8")
this:AddItem("16", "LL_RCC_MCO1_DIV_16")
this:AddItem("32", "LL_RCC_MCO1_DIV_32")
this:AddItem("64", "LL_RCC_MCO1_DIV_64")
this:AddItem("128", "LL_RCC_MCO1_DIV_128")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_MCO1_DIV", "")
--*/
#define __CLK_MCO1_DIV__ LL_RCC_MCO1_DIV_1

/*--
this:AddWidget("Combobox", "I2S clock source")
this:AddItem("SYSCLK", "LL_RCC_I2S_CLKSOURCE_SYSCLK")
this:AddItem("I2S_CKIN Pin", "LL_RCC_I2S_CLKSOURCE_PIN")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_I2S_SRC", "")
--*/
#define __CLK_I2S_SRC__ LL_RCC_I2S_CLKSOURCE_SYSCLK

/*--
this:AddWidget("Combobox", "USB clock divider")
this:AddItem("PLL", "LL_RCC_USB_CLKSOURCE_PLL")
this:AddItem("PLL/1.5", "LL_RCC_USB_CLKSOURCE_PLL_DIV_1_5")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_USB_DIV", "")
--*/
#define __CLK_USB_DIV__ LL_RCC_USB_CLKSOURCE_PLL_DIV_1_5

/*--
this:AddWidget("Combobox", "HDMI-CEC clock source")
this:AddItem("HSI / 488", "LL_RCC_CEC_CLKSOURCE_HSI_DIV244")
this:AddItem("LSE", "LL_RCC_CEC_CLKSOURCE_LSE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CEC_SRC", "")
--*/
#define __CLK_CEC_SRC__ LL_RCC_CEC_CLKSOURCE_HSI_DIV244

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
this:AddWidget("Combobox", "I2C1 clock source")
this:AddItem("SYSCLK", "LL_RCC_I2C1_CLKSOURCE_SYSCLK")
this:AddItem("HSI", "LL_RCC_I2C1_CLKSOURCE_HSI")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_I2C1_SRC", "")
--*/
#define __CLK_I2C1_SRC__ LL_RCC_I2C1_CLKSOURCE_SYSCLK

/*--
this:AddWidget("Combobox", "I2C2 clock source")
this:AddItem("SYSCLK", "LL_RCC_I2C2_CLKSOURCE_SYSCLK")
this:AddItem("HSI", "LL_RCC_I2C2_CLKSOURCE_HSI")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_I2C2_SRC", "")
--*/
#define __CLK_I2C2_SRC__ LL_RCC_I2C2_CLKSOURCE_SYSCLK

/*--
this:AddWidget("Combobox", "ADC12 clock source")
this:AddItem("HCLK", "LL_RCC_ADC12_CLKSRC_HCLK")
this:AddItem("PCLK1/1", "LL_RCC_ADC12_CLKSRC_PLL_DIV_1")
this:AddItem("PCLK1/2", "LL_RCC_ADC12_CLKSRC_PLL_DIV_2")
this:AddItem("PCLK1/4", "LL_RCC_ADC12_CLKSRC_PLL_DIV_4")
this:AddItem("PCLK1/6", "LL_RCC_ADC12_CLKSRC_PLL_DIV_6")
this:AddItem("PCLK1/8", "LL_RCC_ADC12_CLKSRC_PLL_DIV_8")
this:AddItem("PCLK1/10", "LL_RCC_ADC12_CLKSRC_PLL_DIV_10")
this:AddItem("PCLK1/12", "LL_RCC_ADC12_CLKSRC_PLL_DIV_12")
this:AddItem("PCLK1/16", "LL_RCC_ADC12_CLKSRC_PLL_DIV_16")
this:AddItem("PCLK1/32", "LL_RCC_ADC12_CLKSRC_PLL_DIV_32")
this:AddItem("PCLK1/64", "LL_RCC_ADC12_CLKSRC_PLL_DIV_64")
this:AddItem("PCLK1/128", "LL_RCC_ADC12_CLKSRC_PLL_DIV_128")
this:AddItem("PCLK1/256", "LL_RCC_ADC12_CLKSRC_PLL_DIV_256")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_ADC12_SRC", "")
--*/
#define __CLK_ADC12_SRC__ LL_RCC_ADC12_CLKSRC_PLL_DIV_2

/*--
this:AddWidget("Combobox", "ADC34 clock source")
this:AddItem("HCLK", "LL_RCC_ADC34_CLKSRC_HCLK")
this:AddItem("PCLK2/1", "LL_RCC_ADC34_CLKSRC_PLL_DIV_1")
this:AddItem("PCLK2/2", "LL_RCC_ADC34_CLKSRC_PLL_DIV_2")
this:AddItem("PCLK2/4", "LL_RCC_ADC34_CLKSRC_PLL_DIV_4")
this:AddItem("PCLK2/6", "LL_RCC_ADC34_CLKSRC_PLL_DIV_6")
this:AddItem("PCLK2/8", "LL_RCC_ADC34_CLKSRC_PLL_DIV_8")
this:AddItem("PCLK2/10", "LL_RCC_ADC34_CLKSRC_PLL_DIV_10")
this:AddItem("PCLK2/12", "LL_RCC_ADC34_CLKSRC_PLL_DIV_12")
this:AddItem("PCLK2/16", "LL_RCC_ADC34_CLKSRC_PLL_DIV_16")
this:AddItem("PCLK2/32", "LL_RCC_ADC34_CLKSRC_PLL_DIV_32")
this:AddItem("PCLK2/64", "LL_RCC_ADC34_CLKSRC_PLL_DIV_64")
this:AddItem("PCLK2/128", "LL_RCC_ADC34_CLKSRC_PLL_DIV_128")
this:AddItem("PCLK2/256", "LL_RCC_ADC34_CLKSRC_PLL_DIV_256")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_ADC34_SRC", "")
--*/
#define __CLK_ADC34_SRC__ LL_RCC_ADC34_CLKSRC_PLL_DIV_2

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
#define __CLK_FLASH_LATENCY__ 2


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

    -- HSI clock frequency -----------------------------------------------------
    freq.HSI = 8e6

    -- LSE clock frequency -----------------------------------------------------
    local LSEON = iff(this:GetFlagValue("__CLK_LSE_ON__") == "DISABLE", 0, 1)
    freq.LSE = 32768 * LSEON
    this:SetFlagValue("LABEL_LSE_ON", PrintFrequency(freq.LSE).." (LSE)")

    -- LSI clock frequency -----------------------------------------------------
    local LSION = iff(this:GetFlagValue("__CLK_LSI_ON__") == "DISABLE", 0, 1)
    freq.LSI = 40e3 * LSION
    this:SetFlagValue("LABEL_LSI_ON", PrintFrequency(freq.LSI).." (LSI)")

    -- I2S_CKIN/SAI_CKIN clock frequency ---------------------------------------
    freq.I2S_CKIN  = 12288000
    freq.SAI_CLKIN = 48000
    freq.PIN = freq.SAI_CLKIN

    -- PLLs clock source frequency ---------------------------------------------
    local PLLSRC = this:GetFlagValue("__CLK_PLL_SRC__")
    local PLLM   = this:GetFlagValue("__CLK_PLL_SRC_DIV_M__"):gsub("LL_RCC_PREDIV_DIV_", "")

    if PLLSRC == "LL_RCC_PLLSOURCE_HSI" then
       this:SetFlagValue("LABEL_PLL_SRC", PrintFrequency(freq.HSI))
       freq.PLLSRC = freq.HSI / 2
    else
       this:SetFlagValue("LABEL_PLL_SRC", PrintFrequency(freq.HSE))
       freq.PLLSRC = freq.HSE / PLLM
    end

    this:SetFlagValue("LABEL_PLL_SRC_DIV_M", PrintFrequency(freq.PLLSRC))

    -- calculate PLL clock frequency -------------------------------------------
    local PLLON = iff(this:GetFlagValue("__CLK_PLL_ON__") == "DISABLE", 0, 1)
    local PLLN  = this:GetFlagValue("__CLK_PLL_N__"):gsub("LL_RCC_PLL_MUL_", "")

    if PLLON == 1 then
        freq.PLLCLK = freq.PLLSRC * PLLN
        this:SetFlagValue("LABEL_PLL_N", PrintFrequency(freq.PLLCLK))
    else
        freq.PLLCLK = 0
        this:SetFlagValue("LABEL_PLL_N", "")
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

    if freq.HCLK > 72e6 then
       this:SetFlagValue("LABEL_AHB_PRE", PrintFrequency(freq.HCLK).." (HCLK) [max is 72MHz]")
    else
       this:SetFlagValue("LABEL_AHB_PRE", PrintFrequency(freq.HCLK).." (HCLK)")
    end

    if freq.PCLK1 > 36e6 then
       this:SetFlagValue("LABEL_APB1_PRE", PrintFrequency(freq.PCLK1).." (PCLK1) [max is 36MHz]")
    else
       this:SetFlagValue("LABEL_APB1_PRE", PrintFrequency(freq.PCLK1).." (PCLK1)")
    end

    if freq.PCLK2 > 36e6 then
       this:SetFlagValue("LABEL_APB2_PRE", PrintFrequency(freq.PCLK2).." (PCLK2) [max is 36MHz]")
    else
       this:SetFlagValue("LABEL_APB2_PRE", PrintFrequency(freq.PCLK2).." (PCLK2)")
    end

    -- calculate RTCCLK --------------------------------------------------------
    local RTCSRC = this:GetFlagValue("__CLK_RTC_SRC__")
    if RTCSRC == "LL_RCC_RTC_CLKSOURCE_LSI" then
        freq.RTCCLK = freq.LSI
    elseif RTCSRC == "LL_RCC_RTC_CLKSOURCE_LSE" then
        freq.RTCCLK = freq.LSE
    elseif RTCSRC == "LL_RCC_RTC_CLKSOURCE_HSE_DIV32" then
        freq.RTCCLK = freq.HSE / 32
    else
        freq.RTCCLK = 0
    end
    this:SetFlagValue("LABEL_RTC_SRC", PrintFrequency(freq.RTCCLK).." (RTCCLK)")

    -- MCO1 pin output frequency -----------------------------------------------
    local MCO1_SRC = this:GetFlagValue("__CLK_MCO1_SRC__")
    if MCO1_SRC == "LL_RCC_MCO1SOURCE_NOCLOCK" then
        freq.MCO1 = 0
    elseif MCO1_SRC == "LL_RCC_MCO1SOURCE_SYSCLK" then
        freq.MCO1 = freq.SYSCLK
    elseif MCO1_SRC == "LL_RCC_MCO1SOURCE_HSI" then
        freq.MCO1 = freq.HSI
    elseif MCO1_SRC == "LL_RCC_MCO1SOURCE_HSE" then
        freq.MCO1 = freq.HSE
    elseif MCO1_SRC == "LL_RCC_MCO1SOURCE_LSI" then
        freq.MCO1 = freq.LSI
    elseif MCO1_SRC == "LL_RCC_MCO1SOURCE_LSE" then
        freq.MCO1 = freq.LSE
    elseif MCO1_SRC == "LL_RCC_MCO1SOURCE_PLLCLK" then
        freq.MCO1 = freq.PLLCLK
    elseif MCO1_SRC == "LL_RCC_MCO1SOURCE_PLLCLK_DIV_2" then
        freq.MCO1 = freq.PLLCLK / 2
    else
        freq.MCO1 = 0
    end

    local MCO1_DIV = this:GetFlagValue("__CLK_MCO1_DIV__"):gsub("LL_RCC_MCO1_DIV_", "")
    this:SetFlagValue("LABEL_MCO1_DIV", PrintFrequency(freq.MCO1 / MCO1_DIV))
    this:SetFlagValue("LABEL_MCO1_SRC", PrintFrequency(freq.MCO1))

    -- I2S clock ---------------------------------------------------------------
    local I2S_SRC = this:GetFlagValue("__CLK_I2S_SRC__")
    if I2S_SRC == "LL_RCC_I2S_CLKSOURCE_SYSCLK" then
        freq.I2S = freq.SYSCLK
    elseif I2S_SRC == "LL_RCC_I2S_CLKSOURCE_PIN" then
        freq.I2S = 49.56e6
    else
        freq.I2S = 0
    end
    this:SetFlagValue("LABEL_I2S_SRC", PrintFrequency(freq.I2S))

    -- USB clock ---------------------------------------------------------------
    local USB_DIV = this:GetFlagValue("__CLK_USB_DIV__")
    if USB_DIV == "LL_RCC_USB_CLKSOURCE_PLL" then
        freq.USB = freq.PLLCLK
    elseif USB_DIV == "LL_RCC_USB_CLKSOURCE_PLL_DIV_1_5" then
        freq.USB = freq.PLLCLK / 1.5
    else
        freq.USB = 0
    end
    this:SetFlagValue("LABEL_USB_DIV", PrintFrequency(freq.USB))

    -- CEC clock ---------------------------------------------------------------
    local CEC_SRC = this:GetFlagValue("__CLK_CEC_SRC__")
    if CEC_SRC == "LL_RCC_CEC_CLKSOURCE_HSI_DIV244" then
        freq.CEC = freq.HSI / 244
    elseif CEC_SRC == "LL_RCC_CEC_CLKSOURCE_LSE" then
        freq.CEC = freq.LSE
    else
        freq.CEC = 0
    end
    this:SetFlagValue("LABEL_CEC_SRC", PrintFrequency(freq.CEC))

    -- UARTx clock -------------------------------------------------------------
    local PERIPH = {"USART1", "USART2", "USART3", "UART4", "UART5", "I2C1", "I2C2"}

    for i = 1, #PERIPH do
        local PERIPH_CLK_SRC = this:GetFlagValue("__CLK_"..PERIPH[i].."_SRC__")
              PERIPH_CLK_SRC = PERIPH_CLK_SRC:gsub("LL_RCC_"..PERIPH[i].."_CLKSOURCE_", "")

        freq[PERIPH[i]] = freq[PERIPH_CLK_SRC]

        this:SetFlagValue("LABEL_"..PERIPH[i].."_SRC", PrintFrequency(freq[PERIPH[i]]))
    end

    -- ADC12 clock -------------------------------------------------------------
    local ADC_SRC = this:GetFlagValue("__CLK_ADC12_SRC__")
    if ADC_SRC == "LL_RCC_ADC12_CLKSRC_HCLK" then
        freq.ADC12 = freq.HCLK
    else
        freq.ADC12 = freq.PCLK2 / ADC_SRC:gsub("LL_RCC_ADC12_CLKSRC_PLL_DIV_", "")
    end
    this:SetFlagValue("LABEL_ADC12_SRC", PrintFrequency(freq.ADC12))

    -- ADC34 clock -------------------------------------------------------------
    local ADC_SRC = this:GetFlagValue("__CLK_ADC34_SRC__")
    if ADC_SRC == "LL_RCC_ADC34_CLKSRC_HCLK" then
        freq.ADC34 = freq.HCLK
    else
        freq.ADC34 = freq.PCLK2 / ADC_SRC:gsub("LL_RCC_ADC34_CLKSRC_PLL_DIV_", "")
    end
    this:SetFlagValue("LABEL_ADC34_SRC", PrintFrequency(freq.ADC34))

    -- calculate flash latency -------------------------------------------------
    local CPUVOLTAGE = this:GetFlagValue("__CLK_CPU_VOLTAGE__")
    local FLV = 1
    if CPUVOLTAGE == "2p7V" then
        FLV = 24e6
    elseif CPUVOLTAGE == "2p4V" then
        FLV = 22e6
    elseif CPUVOLTAGE == "2p1V" then
        FLV = 20e6
    else --CPUVOLTAGE == "1p8V"
        FLV = 18e6
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
