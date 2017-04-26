/*=========================================================================*//**
@file    clk_flags.h

@author  Daniel Zorychta

@brief   PLL module configuration flags.

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

/*
 * NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as suffix
 *       and prefix) are exported to the single configuration file
 *       (by using Configtool) when entire project configuration is exported.
 *       All other flag definitions and statements are ignored.
 */
#ifndef _CLK_FLAGS_H_
#define _CLK_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 3, "Home > Microcontroller > PLL",
               function() this:LoadFile("arch/arch_flags.h") end)

this:SetEvent("PostDiscard", function() this:CalculateFreq() end)
++*/

/*--
this:AddWidget("Combobox", "LSI oscillator")
this:AddItem("Enable", "ENABLE")
this:AddItem("Disable", "DISABLE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_LSI_ON", "")
--*/
#define __CLK_LSI_ON__ DISABLE

/*--
this:AddWidget("Combobox", "LSE oscillator")
this:AddItem("Disable", "RCC_LSE_OFF")
this:AddItem("Enable", "RCC_LSE_ON")
this:AddItem("Baypass", "RCC_LSE_Bypass")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_LSE_ON", "")
--*/
#define __CLK_LSE_ON__ RCC_LSE_OFF

/*--
this:AddWidget("Combobox", "HSE oscillator")
this:AddItem("Disable", "RCC_HSE_OFF")
this:AddItem("Enable", "RCC_HSE_ON")
this:AddItem("Baypass", "RCC_HSE_Bypass")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_HSE_ON", "")
--*/
#define __CLK_HSE_ON__ RCC_HSE_OFF

/*--
this:AddWidget("Combobox", "PLL")
this:AddItem("Enable",  "ENABLE")
this:AddItem("Disable", "DISABLE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_ON", "")
--*/
#define __CLK_PLL_ON__ ENABLE

/*--
this:AddWidget("Combobox", "PLL clock source")
this:AddItem("HSI",  "RCC_PLLSource_HSI")
this:AddItem("HSE", "RCC_PLLSource_HSE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_SRC", "")
--*/
#define __CLK_PLL_SRC__ RCC_PLLSource_HSI

/*--
this:AddWidget("Spinbox", 2, 63, "PLL clock divider (PLLM)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_SRC_DIV", "")
--*/
#define __CLK_PLL_SRC_DIV__ 2

/*--
this:AddWidget("Spinbox", 50, 432, "PLL clock multiplier (PLLN)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_MUL", "")
--*/
#define __CLK_PLL_MUL__ 50

/*--
this:AddWidget("Combobox", "PLL out divider (PLLP)")
this:AddItem("PLL out divided by 2",  "2")
this:AddItem("PLL out divided by 4",  "4")
this:AddItem("PLL out divided by 6",  "6")
this:AddItem("PLL out divided by 8",  "8")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_SYSCLK_DIV", "")
--*/
#define __CLK_PLL_OUT_SYSCLK_DIV__ 2

/*--
this:AddWidget("Spinbox", 4, 15, "PLL USB, SDIO divider (PLLQ)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_OUT_48MHz_DIV", "")
--*/
#define __CLK_PLL_OUT_48MHz_DIV__ 4

/*--
this:AddWidget("Spinbox", 2, 7, "PLL I2S, SAI divider (PLLR)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_OUT_I2S_DIV", "")
--*/
#define __CLK_PLL_OUT_I2S_DIV__ 2

/*--
this:AddWidget("Combobox", "System clock source")
this:AddItem("HSI clock", "RCC_SYSCLKSource_HSI")
this:AddItem("HSE clock", "RCC_SYSCLKSource_HSE")
this:AddItem("PLL clock", "RCC_SYSCLKSource_PLLCLK")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_SYSCLK_SRC", "")
--*/
#define __CLK_SYS_CLK_SRC__ RCC_SYSCLKSource_PLLCLK

/*--
this:AddWidget("Combobox", "RTC Clock source")
this:AddItem("LSE clock", "RCC_RTCCLKSource_LSE")
this:AddItem("LSI clock", "RCC_RTCCLKSource_LSI")
for i = 2, 31 do this:AddItem("HSE clock divided by "..i, "RCC_RTCCLKSource_HSE_Div"..i) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_RTCCLK_SRC", "")
--*/
#define __CLK_RTC_CLK_SRC__ RCC_RTCCLKSource_LSE

/*--
this:AddWidget("Combobox", "MCO1 Clock source")
this:AddItem("HSI", "RCC_MCO1Source_HSI")
this:AddItem("HSE", "RCC_MCO1Source_HSE")
this:AddItem("LSE", "RCC_MCO1Source_LSE")
this:AddItem("PLLCLK", "RCC_MCO1Source_PLLCLK")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_MCO1_SRC", "")
--*/
#define __CLK_MC01_CLK_SRC__ RCC_MCO1Source_HSI

/*--
this:AddWidget("Combobox", "MCO1 Clock divisor (output)")
for i = 1, 5 do this:AddItem("MCO divided by "..i, tostring(i)) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_MCO1_DIV", "")
--*/
#define __CLK_MC01_CLK_DIV__ 1

/*--
this:AddWidget("Combobox", "MCO2 Clock source")
this:AddItem("HSE", "RCC_MCO2Source_HSE")
this:AddItem("LSE", "RCC_MCO2Source_LSE")
this:AddItem("PLLCLK", "RCC_MCO2Source_PLLCLK")
this:AddItem("SYSCLK", "RCC_MCO2Source_SYSCLK")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_MCO2_SRC", "")
--*/
#define __CLK_MC02_CLK_SRC__ RCC_MCO2Source_HSE

/*--
this:AddWidget("Combobox", "MCO1 Clock divisor (output)")
for i = 1, 5 do this:AddItem("MCO divided by "..i, tostring(i)) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_MCO2_DIV", "")
--*/
#define __CLK_MC02_CLK_DIV__ 1


/*--
this:AddWidget("Combobox", "AHB prescaler")
this:AddItem("SYSCLK / 1",  "RCC_SYSCLK_Div1")
this:AddItem("SYSCLK / 2",  "RCC_SYSCLK_Div2")
this:AddItem("SYSCLK / 4",  "RCC_SYSCLK_Div4")
this:AddItem("SYSCLK / 8",  "RCC_SYSCLK_Div8")
this:AddItem("SYSCLK / 16",  "RCC_SYSCLK_Div16")
this:AddItem("SYSCLK / 64",  "RCC_SYSCLK_Div64")
this:AddItem("SYSCLK / 128",  "RCC_SYSCLK_Div128")
this:AddItem("SYSCLK / 256",  "RCC_SYSCLK_Div256")
this:AddItem("SYSCLK / 512",  "RCC_SYSCLK_Div512")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_AHB_PRE", "")
--*/
#define __CLK_AHB_PRE__ RCC_SYSCLK_Div1

/*--
this:AddWidget("Combobox", "APB1 prescaler")
this:AddItem("HCLK / 1",  "RCC_HCLK_Div1")
this:AddItem("HCLK / 2",  "RCC_HCLK_Div2")
this:AddItem("HCLK / 4",  "RCC_HCLK_Div4")
this:AddItem("HCLK / 8",  "RCC_HCLK_Div8")
this:AddItem("HCLK / 16",  "RCC_HCLK_Div16")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_APB1_PRE", "")
--*/
#define __CLK_APB1_PRE__ RCC_HCLK_Div2

/*--
this:AddWidget("Combobox", "APB2 prescaler")
this:AddItem("HCLK / 1",  "RCC_HCLK_Div1")
this:AddItem("HCLK / 2",  "RCC_HCLK_Div2")
this:AddItem("HCLK / 4",  "RCC_HCLK_Div4")
this:AddItem("HCLK / 8",  "RCC_HCLK_Div8")
this:AddItem("HCLK / 16",  "RCC_HCLK_Div16")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_APB2_PRE", "")
--*/
#define __CLK_APB2_PRE__ RCC_HCLK_Div2

/*--
this:AddWidget("Combobox", "CPU voltage")
this:AddItem("2.7V - 3.6V",  "3p6V")
this:AddItem("2.4V - 2.7V",  "2p7V")
this:AddItem("2.1V - 2.4V",  "2p4V")
this:AddItem("1.8V - 2.1V",  "2p1V")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:SetToolTip("Select power voltage level to calculate FLASH latency.")
this:AddExtraWidget("Label", "LABEL_FLASH_LATENCY", "")
--*/
#define __CLK_CPU_VOLTAGE__ 3p6V

/*--
-- Value calculated automatically
this:AddWidget("Value")
--*/
#define __CLK_FLASH_LATENCY__ 1

/*--
this.CalculateFreq = function(self)
    local PLLON      = iff(this:GetFlagValue("__CLK_PLL_ON__") == "ENABLE", 1, 0)
    local PLLSRC     = this:GetFlagValue("__CLK_PLL_SRC__")
    local PLLM       = this:GetFlagValue("__CLK_PLL_SRC_DIV__")
    local PLLN       = this:GetFlagValue("__CLK_PLL_MUL__")
    local PLLP       = this:GetFlagValue("__CLK_PLL_OUT_SYSCLK_DIV__")
    local PLLQ       = this:GetFlagValue("__CLK_PLL_OUT_48MHz_DIV__")
    local PLLR       = this:GetFlagValue("__CLK_PLL_OUT_I2S_DIV__")
    local HSEON      = iff(this:GetFlagValue("__CLK_HSE_ON__") == "RCC_HSE_OFF", 0, 1)
    local LSEON      = iff(this:GetFlagValue("__CLK_LSE_ON__") == "RCC_LSE_OFF", 0, 1)
    local LSION      = iff(this:GetFlagValue("__CLK_LSI_ON__") == "ENABLE", 1, 0)
    local SYSCLKSRC  = this:GetFlagValue("__CLK_SYS_CLK_SRC__")
    local RTCSRC     = this:GetFlagValue("__CLK_RTC_CLK_SRC__")
    local AHBPRE     = this:GetFlagValue("__CLK_AHB_PRE__")
    local APB1PRE    = this:GetFlagValue("__CLK_APB1_PRE__")
    local APB2PRE    = this:GetFlagValue("__CLK_APB2_PRE__")
    local MCO1SRC    = this:GetFlagValue("__CLK_MC01_CLK_SRC__")
    local MCO1DIV    = this:GetFlagValue("__CLK_MC01_CLK_DIV__")
    local MCO2SRC    = this:GetFlagValue("__CLK_MC02_CLK_SRC__")
    local MCO2DIV    = this:GetFlagValue("__CLK_MC02_CLK_DIV__")
    local CPUVOLTAGE = this:GetFlagValue("__CLK_CPU_VOLTAGE__")

    local freq        = {}
    freq.HSE          = uC.OSCFREQ * HSEON
    freq.HSI          = 16e6
    freq.LSI          = 32e3 * LSION
    freq.LSE          = 32768 * LSEON
    freq.RTCCLK       = 0
    freq.SYSCLK       = 0
    freq.PLLCLK       = 0
    freq.PLLVCO       = 0
    freq.PLL48CLK     = 0
    freq.PLLI2SCLK    = 0
    freq.PLLM         = 0
    freq.HCLK         = 0
    freq.FCLK         = 0
    freq.PCLK1        = 0
    freq.PCLK2        = 0
    freq.TIMCLK1      = 0
    freq.TIMCLK2      = 0
    freq.MCO1CLK      = 0
    freq.MCO1CLKDIV   = 0
    freq.MCO2CLK      = 0
    freq.MCO2CLKDIV   = 0
    freq.FLASHLATENCY = 0

    -- calculate RTCCLK --------------------------------------------------------
    if RTCSRC == "RCC_RTCCLKSource_LSI" then
        freq.RTCCLK = freq.LSI
    elseif RTCSRC == "RCC_RTCCLKSource_LSE" then
        freq.RTCCLK = freq.LSE
    else
        freq.RTCCLK = freq.HSE / RTCSRC:gsub("RCC_RTCCLKSource_HSE_Div", "")
    end

    -- calculate PLL Clk frequency ---------------------------------------------
    if PLLON == 1 then
        if PLLSRC == "RCC_PLLSource_HSI" then
           this:SetFlagValue("LABEL_PLL_SRC", PrintFrequency(freq.HSI))
           freq.PLLM = freq.HSI / PLLM
        else
           this:SetFlagValue("LABEL_PLL_SRC", PrintFrequency(freq.HSE))
           freq.PLLM = freq.HSE / PLLM
        end

        if freq.PLLM < 1e6 or freq.PLLM > 2e6 then
           this:SetFlagValue("LABEL_PLL_SRC_DIV", PrintFrequency(freq.PLLM).." [Not in range (1-2 MHz)]")
        else
           this:SetFlagValue("LABEL_PLL_SRC_DIV", PrintFrequency(freq.PLLM))
        end

        freq.PLLVCO = freq.PLLM * PLLN
        if freq.PLLVCO < 100e6 or freq.PLLVCO > 432e6 then
           this:SetFlagValue("LABEL_PLL_MUL", PrintFrequency(freq.PLLVCO).." [Not in range (100-432 MHz)]")
        else
           this:SetFlagValue("LABEL_PLL_MUL", PrintFrequency(freq.PLLVCO))
        end

        freq.PLLCLK = freq.PLLVCO / PLLP
        this:SetFlagValue("LABEL_PLL_SYSCLK_DIV", PrintFrequency(freq.PLLCLK).." (PLLCLK)")

        freq.PLL48CLK = freq.PLLVCO / PLLQ
        this:SetFlagValue("LABEL_PLL_OUT_48MHz_DIV", PrintFrequency(freq.PLL48CLK).." (PLL48CK)")

        freq.PLLI2SCLK = freq.PLLVCO / PLLR
        this:SetFlagValue("LABEL_PLL_OUT_I2S_DIV", PrintFrequency(freq.PLLI2SCLK))
    end

    -- calculate SYSCLK --------------------------------------------------------
    if SYSCLKSRC == "RCC_SYSCLKSource_HSE" then
        freq.SYSCLK = freq.HSE
    elseif SYSCLKSRC == "RCC_SYSCLKSource_HSI" then
        freq.SYSCLK = freq.HSI
    else
        freq.SYSCLK = freq.PLLCLK
    end

    -- calculate MCO1 frequency ------------------------------------------------
    if MCO1SRC == "RCC_MCO1Source_HSI" then
        freq.MCO1CLK = freq.HSI
    elseif MCO1SRC == "RCC_MCO1Source_HSE" then
        freq.MCO1CLK = freq.HSE
    elseif MCO1SRC == "RCC_MCO1Source_LSE" then
        freq.MCO1CLK = freq.LSE
    else
        freq.MCO1CLK = freq.PLLCLK
    end
    freq.MCO1CLKDIV = freq.MCO1CLK / MCO1DIV

    -- calculate MCO2 frequency ------------------------------------------------
    if MCO2SRC == "RCC_MCO2Source_HSE" then
        freq.MCO2CLK = freq.HSE
    elseif MCO2SRC == "RCC_MCO2Source_LSE" then
        freq.MCO2CLK = freq.LSE
    elseif MCO2SRC == "RCC_MCO2Source_PLLCLK" then
        freq.MCO2CLK = freq.PLLCLK
    else
        freq.MCO2CLK = freq.SYSCLK
    end
    freq.MCO2CLKDIV = freq.MCO2CLK / MCO2DIV

    -- calculate output clocks -------------------------------------------------
    freq.HCLK     = freq.SYSCLK / AHBPRE:gsub("RCC_SYSCLK_Div", "")
    freq.FCLK     = freq.HCLK
    freq.PCLK1    = freq.HCLK / APB1PRE:gsub("RCC_HCLK_Div", "")
    freq.PCLK2    = freq.HCLK / APB2PRE:gsub("RCC_HCLK_Div", "")
    freq.TIMxCLK1 = iff(APB1PRE == "RCC_HCLK_Div1", freq.PCLK1, freq.PCLK1 * 2)
    freq.TIMxCLK2 = iff(APB2PRE == "RCC_HCLK_Div1", freq.PCLK1, freq.PCLK1 * 2)

    -- calculate flash latency -------------------------------------------------
    local FLV = 0
    if CPUVOLTAGE == "3p6V" then
        FLV = 30e6
    elseif CPUVOLTAGE == "2p7V" then
        FLV = 24e6
    elseif CPUVOLTAGE == "2p4V" then
        FLV = 22e6
    else --CPUVOLTAGE == "2p1V"
        FLV = 20e6
    end

    freq.FLASHLATENCY = math.ceil(freq.SYSCLK / FLV) - 1

    -- Results -----------------------------------------------------------------
    this:SetFlagValue("LABEL_LSI_ON", PrintFrequency(freq.LSI).." (LSI)")
    this:SetFlagValue("LABEL_LSE_ON", PrintFrequency(freq.LSE).." (LSE)")
    this:SetFlagValue("LABEL_HSE_ON", PrintFrequency(freq.HSE).." (HSE)")
    this:SetFlagValue("LABEL_SYSCLK_SRC", PrintFrequency(freq.SYSCLK).." (SYSCLK)")
    this:SetFlagValue("LABEL_RTCCLK_SRC", PrintFrequency(freq.RTCCLK).." (RTCCLK)")
    this:SetFlagValue("LABEL_AHB_PRE", PrintFrequency(freq.HCLK).." (HCLK)")
    this:SetFlagValue("LABEL_APB1_PRE", PrintFrequency(freq.PCLK1).." (PCLK1)")
    this:SetFlagValue("LABEL_APB2_PRE", PrintFrequency(freq.PCLK2).." (PCLK2)")
    this:SetFlagValue("LABEL_MCO1_SRC", PrintFrequency(freq.MCO1CLK))
    this:SetFlagValue("LABEL_MCO1_DIV", PrintFrequency(freq.MCO1CLKDIV))
    this:SetFlagValue("LABEL_MCO2_SRC", PrintFrequency(freq.MCO2CLK))
    this:SetFlagValue("LABEL_MCO2_DIV", PrintFrequency(freq.MCO2CLKDIV))
    this:SetFlagValue("LABEL_FLASH_LATENCY", freq.FLASHLATENCY.." delay cycles")
    this:SetFlagValue("__CLK_FLASH_LATENCY__", tostring(freq.FLASHLAT))
end

-- trigger frequency calculation after loading all flags
this.CalculateFreq()
++*/

#endif /* _CLK_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
