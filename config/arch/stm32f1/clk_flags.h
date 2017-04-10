/*=========================================================================*//**
@file    clk_flags.h

@author  Daniel Zorychta

@brief   PLL module configuration flags.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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
if uC.FAMILY == "STM32F10X_CL" then
    this:AddWidget("Combobox", "PREDIV2")
    for i = 1, 16 do
        this:AddItem("HSE/"..i,  "RCC_PREDIV2_Div"..i)
    end
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_PREDIV2_VAL", "")
end
--*/
#define __CLK_PREDIV2_VAL__ RCC_PREDIV2_Div5

/*--
if uC.FAMILY == "STM32F10X_CL" then
    this:AddWidget("Combobox", "PLL2")
    this:AddItem("Enable",  "ENABLE")
    this:AddItem("Disable", "DISABLE")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_PLL2_ON", "")
end
--*/
#define __CLK_PLL2_ON__ ENABLE

/*--
if uC.FAMILY == "STM32F10X_CL" then
    this:AddWidget("Combobox", "PLL2 multiplier")
    for i = 8, 14 do this:AddItem("PREDIV2 clock x"..i,  "RCC_PLL2Mul_"..i) end
    this:AddItem("PREDIV2 clock x16",  "RCC_PLL2Mul_16")
    this:AddItem("PREDIV2 clock x20",  "RCC_PLL2Mul_20")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_PLL2_MUL", "")
end
--*/
#define __CLK_PLL2_MUL__ RCC_PLL2Mul_8

/*--
if uC.FAMILY == "STM32F10X_CL" then
    this:AddWidget("Combobox", "PLL3")
    this:AddItem("Enable",  "ENABLE")
    this:AddItem("Disable", "DISABLE")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_PLL3_ON", "")
end
--*/
#define __CLK_PLL3_ON__ ENABLE

/*--
if uC.FAMILY == "STM32F10X_CL" then
    this:AddWidget("Combobox", "PLL3 multiplier")
    for i = 8, 14 do this:AddItem("PREDIV2 clock x"..i,  "RCC_PLL3Mul_"..i) end
    this:AddItem("PREDIV2 clock x16",  "RCC_PLL3Mul_16")
    this:AddItem("PREDIV2 clock x20",  "RCC_PLL3Mul_20")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_PLL3_MUL", "")
end
--*/
#define __CLK_PLL3_MUL__ RCC_PLL3Mul_10

/*--
if uC.FAMILY == "STM32F10X_CL" then
    this:AddWidget("Combobox", "PREDIV1 source")
    this:AddItem("HSE",  "RCC_PREDIV1_Source_HSE")
    this:AddItem("PLL2",  "RCC_PREDIV1_Source_PLL2")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_PREDIV1_SRC", "")
end
--*/
#define __CLK_PREDIV1_SRC__ RCC_PREDIV1_Source_PLL2

/*--
if uC.FAMILY == "STM32F10X_CL" then
    this:AddWidget("Combobox", "PREDIV1 factor")
    for i = 1, 16 do this:AddItem("/"..i,  "RCC_PREDIV1_Div"..i) end
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_PREDIV1_VAL", "")
end
--*/
#define __CLK_PREDIV1_VAL__ RCC_PREDIV1_Div5

/*--
this:AddWidget("Combobox", "PLL")
this:AddItem("Enable",  "ENABLE")
this:AddItem("Disable", "DISABLE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_ON", "")
--*/
#define __CLK_PLL_ON__ ENABLE

/*--
if uC.FAMILY == "STM32F10X_CL" then
    this:AddWidget("Combobox", "PLL clock source")
    this:AddItem("HSI/2 clock",  "RCC_PLLSource_HSI_Div2")
    this:AddItem("PREDIV1 clock", "RCC_PLLSource_PREDIV1")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_PLL_SRC_CL", "")
end
--*/
#define __CLK_PLL_SRC_CL__ RCC_PLLSource_PREDIV1

/*--
if not(uC.FAMILY == "STM32F10X_CL") then
    this:AddWidget("Combobox", "PLL clock source")
    this:AddItem("HSI/2 clock",  "RCC_PLLSource_HSI_Div2")
    this:AddItem("HSE clock", "RCC_PLLSource_HSE_Div1")
    this:AddItem("HSE/2 clock", "RCC_PLLSource_HSE_Div2")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_PLL_SRC", "")
end
--*/
#define __CLK_PLL_SRC__ RCC_PLLSource_HSI_Div2

/*--
if uC.FAMILY == "STM32F10X_CL" then
    this:AddWidget("Combobox", "PLL multiplier")
    this:AddItem("x4",  "RCC_PLLMul_4")
    this:AddItem("x5",  "RCC_PLLMul_5")
    this:AddItem("x6",  "RCC_PLLMul_6")
    this:AddItem("x6.5",  "RCC_PLLMul_6_5")
    this:AddItem("x7",  "RCC_PLLMul_7")
    this:AddItem("x8",  "RCC_PLLMul_8")
    this:AddItem("x9",  "RCC_PLLMul_9")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_PLL_MUL", "")
end
--*/
#define __CLK_PLL_MUL_CL__ RCC_PLLMul_9

/*--
if not(uC.FAMILY == "STM32F10X_CL") then
    this:AddWidget("Combobox", "PLL multiplier")
    for i = 2, 16 do this:AddItem("x"..i,  "RCC_PLLMul_"..i) end
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_PLL_MUL", "")
end
--*/
#define __CLK_PLL_MUL__ RCC_PLLMul_12

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
this:AddItem("HSE clock divided by 128", "RCC_RTCCLKSource_HSE_Div128")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_RTCCLK_SRC", "")
--*/
#define __CLK_RTC_CLK_SRC__ RCC_RTCCLKSource_LSE

/*--
if uC.FAMILY == "STM32F10X_CL" then
    this:AddWidget("Combobox", "MCO source")
    this:AddItem("No clock",  "RCC_MCO_NoClock")
    this:AddItem("System clock", "RCC_MCO_SYSCLK")
    this:AddItem("HSI oscillator clock", "RCC_MCO_HSI")
    this:AddItem("HSE oscillator clock", "RCC_MCO_HSE")
    this:AddItem("PLL clock divided by 2", "RCC_MCO_PLLCLK_Div2")
    this:AddItem("PLL2 clock", "RCC_MCO_PLL2CLK")
    this:AddItem("PLL3 clock divided by 2", "RCC_MCO_PLL3CLK_Div2")
    this:AddItem("External 3-25 MHz oscillator clock", "RCC_MCO_XT1")
    this:AddItem("PLL3 clock", "RCC_MCO_PLL3CLK")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_MCO_SRC", "")
end
--*/
#define __CLK_MCO_SRC_CL__ RCC_MCO_PLL3CLK

/*--
if not(uC.FAMILY == "STM32F10X_CL") then
    this:AddWidget("Combobox", "MCO source")
    this:AddItem("No clock",  "RCC_MCO_NoClock")
    this:AddItem("System clock", "RCC_MCO_SYSCLK")
    this:AddItem("HSI oscillator clock", "RCC_MCO_HSI")
    this:AddItem("HSE oscillator clock", "RCC_MCO_HSE")
    this:AddItem("PLL clock divided by 2", "RCC_MCO_PLLCLK_Div2")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_MCO_SRC", "")
end
--*/
#define __CLK_MCO_SRC__ RCC_MCO_NoClock

/*--
if uC.FAMILY == "STM32F10X_CL" then
    this:AddWidget("Combobox", "USB prescaler")
    this:AddItem("PLL VCO clock divided by 3",  "RCC_OTGFSCLKSource_PLLVCO_Div3")
    this:AddItem("PLL VCO clock divided by 2",  "RCC_OTGFSCLKSource_PLLVCO_Div2")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_USB_DIV", "")
end
--*/
#define __CLK_USB_DIV_CL__ RCC_OTGFSCLKSource_PLLVCO_Div3

/*--
if not(uC.FAMILY == "STM32F10X_CL") then
    this:AddWidget("Combobox", "USB prescaler")
    this:AddItem("PLL clock divided by 1.5",  "RCC_USBCLKSource_PLLCLK_1Div5")
    this:AddItem("PLL clock",  "RCC_USBCLKSource_PLLCLK_Div1")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_USB_DIV", "")
end
--*/
#define __CLK_USB_DIV__ RCC_USBCLKSource_PLLCLK_Div1

/*--
this:AddWidget("Combobox", "I2S2 clock source")
this:AddItem("System clock",  "RCC_I2S2CLKSource_SYSCLK")
if uC.FAMILY == "STM32F10X_CL" then
    this:AddItem("PLL3 VCO clock", "RCC_I2S2CLKSource_PLL3_VCO")
else
    this:Enable(false)
end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_I2S2_SRC", "")
--*/
#define __CLK_I2S2_SRC__ RCC_I2S2CLKSource_SYSCLK

/*--
this:AddWidget("Combobox", "I2S3 clock source")
this:AddItem("System clock",  "RCC_I2S3CLKSource_SYSCLK")
if uC.FAMILY == "STM32F10X_CL" then
    this:AddItem("PLL3 VCO clock", "RCC_I2S3CLKSource_PLL3_VCO")
else
    this:Enable(false)
end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_I2S3_SRC", "")
--*/
#define __CLK_I2S3_SRC__ RCC_I2S3CLKSource_SYSCLK

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
this:AddWidget("Combobox", "ADC prescaler")
this:AddItem("PCLK2 / 2",  "RCC_PCLK2_Div2")
this:AddItem("PCLK2 / 4",  "RCC_PCLK2_Div4")
this:AddItem("PCLK2 / 6",  "RCC_PCLK2_Div6")
this:AddItem("PCLK2 / 8",  "RCC_PCLK2_Div8")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_ADC_PRE", "")
--*/
#define __CLK_ADC_PRE__ RCC_PCLK2_Div4

/*--
-- Value calculated automatically
this:AddWidget("Value")
--*/
#define __CLK_FLASH_LATENCY__ 1

/*--
this.CalculateFreq = function(self)
    local PLLON      = this:GetFlagValue("__CLK_PLL_ON__")
    local PLLSRC     = this:GetFlagValue(iff(uC.FAMILY == "STM32F10X_CL", "__CLK_PLL_SRC_CL__", "__CLK_PLL_SRC__"))
    local PLLMUL     = this:GetFlagValue(iff(uC.FAMILY == "STM32F10X_CL", "__CLK_PLL_MUL_CL__", "__CLK_PLL_MUL__"))
    local PLL2ON     = this:GetFlagValue("__CLK_PLL2_ON__")
    local PLL2MUL    = this:GetFlagValue(iff(uC.FAMILY == "STM32F10X_CL", "__CLK_PLL2_MUL__", ""))
    local PLL3ON     = this:GetFlagValue("__CLK_PLL3_ON__")
    local PLL3MUL    = this:GetFlagValue(iff(uC.FAMILY == "STM32F10X_CL", "__CLK_PLL3_MUL__", ""))
    local PREDIV1SRC = this:GetFlagValue(iff(uC.FAMILY == "STM32F10X_CL", "__CLK_PREDIV1_SRC__", ""))
    local PREDIV1VAL = this:GetFlagValue(iff(uC.FAMILY == "STM32F10X_CL", "__CLK_PREDIV1_VAL__", ""))
    local PREDIV2VAL = this:GetFlagValue(iff(uC.FAMILY == "STM32F10X_CL", "__CLK_PREDIV2_VAL__", ""))
    local I2S2SRC    = this:GetFlagValue(iff(uC.FAMILY == "STM32F10X_CL", "__CLK_I2S2_SRC__", ""))
    local I2S3SRC    = this:GetFlagValue(iff(uC.FAMILY == "STM32F10X_CL", "__CLK_I2S3_SRC__", ""))
    local MCOSRC     = this:GetFlagValue(iff(uC.FAMILY == "STM32F10X_CL", "__CLK_MCO_SRC_CL__", "__CLK_MCO_SRC__"))
    local USBPRE     = this:GetFlagValue(iff(uC.FAMILY == "STM32F10X_CL", "__CLK_USB_DIV_CL__", "__CLK_USB_DIV__"))
    local HSEON      = iff(this:GetFlagValue("__CLK_HSE_ON__") == "RCC_HSE_OFF", 0, 1)
    local LSEON      = iff(this:GetFlagValue("__CLK_LSE_ON__") == "RCC_LSE_OFF", 0, 1)
    local LSION      = iff(this:GetFlagValue("__CLK_LSI_ON__") == "ENABLE", 1, 0)
    local SYSCLKSRC  = this:GetFlagValue("__CLK_SYS_CLK_SRC__")
    local RTCSRC     = this:GetFlagValue("__CLK_RTC_CLK_SRC__")
    local AHBPRE     = this:GetFlagValue("__CLK_AHB_PRE__")
    local APB1PRE    = this:GetFlagValue("__CLK_APB1_PRE__")
    local APB2PRE    = this:GetFlagValue("__CLK_APB2_PRE__")
    local ADCPRE     = this:GetFlagValue("__CLK_ADC_PRE__")

    local freq       = {}
    freq.HSE         = uC.OSCFREQ * HSEON
    freq.HSI         = 8e6
    freq.LSI         = 40e3 * LSION
    freq.LSE         = 32768 * LSEON
    freq.RTCCLK      = 0
    freq.PLLCLK      = 0
    freq.PLLVCO      = 0
    freq.USBCLK      = 0
    freq.I2S2CLK     = 0
    freq.I2S3CLK     = 0
    freq.SYSCLK      = 0
    freq.HCLK        = 0
    freq.FCLK        = 0
    freq.PCLK1       = 0
    freq.PCLK2       = 0
    freq.ADCCLK      = 0
    freq.TIMCLK1     = 0
    freq.TIMCLK2     = 0
    freq.MCOCLK      = 0
    freq.PLL2CLK     = 0
    freq.PLL3CLK     = 0
    freq.PLL3VCO     = 0
    freq.PREDIV1CLK  = 0
    freq.PREDIV2CLK  = 0
    freq.FLASHLAT    = 0

    -- calculate RTCCLK
    if RTCSRC == "RCC_RTCCLKSource_LSI" then
        freq.RTCCLK = freq.LSI
    elseif RTCSRC == "RCC_RTCCLKSource_LSE" then
        freq.RTCCLK = freq.LSE
    else
        freq.RTCCLK = freq.HSE / 128
    end

    -- calculate Conectivity Linie-specific frequencies
    if uC.FAMILY == "STM32F10X_CL" then
        PREDIV1VAL = PREDIV1VAL:gsub("RCC_PREDIV1_Div", "")
        PREDIV2VAL = PREDIV2VAL:gsub("RCC_PREDIV2_Div", "")
        PLL2MUL    = PLL2MUL:gsub("RCC_PLL2Mul_", "")
        PLL3MUL    = PLL3MUL:gsub("RCC_PLL3Mul_", "")
        PLL2ON     = iff(PLL2ON == "ENABLE", 1, 0)
        PLL3ON     = iff(PLL3ON == "ENABLE", 1, 0)

        freq.PLL2CLK    = (freq.HSE / PREDIV2VAL) * PLL2MUL * PLL2ON
        freq.PLL3CLK    = (freq.HSE / PREDIV2VAL) * PLL3MUL * PLL3ON
        freq.PLL3VCO    = freq.PLL3CLK * 2
        freq.PREDIV2CLK = freq.HSE / PREDIV2VAL
        freq.PREDIV1CLK = iff(PREDIV1SRC == "RCC_PREDIV1_Source_HSE", freq.HSE, freq.PLL2CLK) / PREDIV1VAL
    end

    -- calculate PLL Clk frequency
    PLLMUL = PLLMUL:gsub("RCC_PLLMul_", ""):gsub("6_5", "6.5")
    PLLON  = iff(PLLON == "ENABLE", 1, 0)

    if PLLSRC == "RCC_PLLSource_HSI_Div2" then
        freq.PLLCLK = (freq.HSI / 2) * PLLMUL * PLLON
    else
        if uC.FAMILY == "STM32F10X_CL" then
            freq.PLLCLK = freq.PREDIV1CLK * PLLMUL * PLLON
        else
            freq.PLLCLK = iff(PLLSRC == "RCC_PLLSource_HSE_Div2", (freq.HSE / 2), freq.HSE) * PLLMUL * PLLON
        end
    end
    freq.PLLVCO = 2 * freq.PLLCLK

    -- calculate SYSCLK
    if SYSCLKSRC == "RCC_SYSCLKSource_HSE" then
        freq.SYSCLK = freq.HSE
    elseif SYSCLKSRC == "RCC_SYSCLKSource_HSI" then
        freq.SYSCLK = freq.HSI
    else
        freq.SYSCLK = freq.PLLCLK
    end

    -- calculate USB frequency
    if uC.FAMILY == "STM32F10X_CL" then
        freq.USBCLK = freq.PLLVCO / USBPRE:gsub("RCC_OTGFSCLKSource_PLLVCO_Div", "")
    else
        freq.USBCLK = freq.PLLCLK / USBPRE:gsub("RCC_USBCLKSource_PLLCLK_", ""):gsub("1Div5", "1.5"):gsub("Div", "")
    end

    -- calculate I2S frequency
    freq.I2S2CLK = iff(I2S2SRC == "RCC_I2S2CLKSource_PLL3_VCO" and uC.FAMILY == "STM32F10X_CL", freq.PLL3VCO, freq.SYSCLK)
    freq.I2S3CLK = iff(I2S3SRC == "RCC_I2S3CLKSource_PLL3_VCO" and uC.FAMILY == "STM32F10X_CL", freq.PLL3VCO, freq.SYSCLK)

    -- calculate MCO frequency
    if MCOSRC == "RCC_MCO_NoClock" then
        freq.MCOCLK = 0
    elseif MCOSRC == "RCC_MCO_SYSCLK" then
        freq.MCOCLK = freq.SYSCLK
    elseif MCOSRC == "RCC_MCO_HSI" then
        freq.MCOCLK = freq.HSI
    elseif MCOSRC == "RCC_MCO_HSE" then
        freq.MCOCLK = freq.HSE
    elseif MCOSRC == "RCC_MCO_PLLCLK_Div2" then
        freq.MCOCLK = freq.PLLCLK / 2
    elseif MCOSRC == "RCC_MCO_PLL2CLK" then
        freq.MCOCLK = freq.PLL2CLK
    elseif MCOSRC == "RCC_MCO_PLL3CLK_Div2" then
        freq.MCOCLK = freq.PLL3CLK / 2
    elseif MCOSRC == "RCC_MCO_XT1" then
        freq.MCOCLK = freq.HSE
    elseif MCOSRC == "RCC_MCO_PLL3CLK" then
        freq.MCOCLK = freq.PLL3CLK
    end

    -- calculate some output clocks
    freq.HCLK     = freq.SYSCLK / AHBPRE:gsub("RCC_SYSCLK_Div", "")
    freq.FCLK     = freq.HCLK
    freq.PCLK1    = freq.HCLK / APB1PRE:gsub("RCC_HCLK_Div", "")
    freq.PCLK2    = freq.HCLK / APB2PRE:gsub("RCC_HCLK_Div", "")
    freq.ADCCLK   = freq.PCLK2 / ADCPRE:gsub("RCC_PCLK2_Div", "")
    freq.TIMxCLK1 = iff(APB1PRE == "RCC_HCLK_Div1", freq.PCLK1, freq.PCLK1 * 2)
    freq.TIMxCLK2 = iff(APB2PRE == "RCC_HCLK_Div1", freq.PCLK1, freq.PCLK1 * 2)

    -- calculate flash latency
    if freq.SYSCLK <= 24e6 then
        freq.FLASHLAT = 0
    elseif freq.SYSCLK <= 48e6 then
        freq.FLASHLAT = 1
    elseif freq.SYSCLK <= 72e6 then
        freq.FLASHLAT = 2
    else
        freq.FLASHLAT = 2
    end

    -- set labels
    if uC.FAMILY == "STM32F10X_CL" then
        this:SetFlagValue("LABEL_PREDIV2_VAL", PrintFrequency(freq.PREDIV2CLK).." (PREDIV2CLK)")
        this:SetFlagValue("LABEL_PLL2_MUL", PrintFrequency(freq.PLL2CLK).." (PLL2CLK)")
        this:SetFlagValue("LABEL_PLL3_MUL", PrintFrequency(freq.PLL3CLK).." (PLL3CLK)")
        this:SetFlagValue("LABEL_PREDIV1_VAL", PrintFrequency(freq.PREDIV1CLK).." (PREDIV1CLK)")
    end

    this:SetFlagValue("LABEL_LSI_ON", PrintFrequency(freq.LSI).." (LSI)")
    this:SetFlagValue("LABEL_LSE_ON", PrintFrequency(freq.LSE).." (LSE)")
    this:SetFlagValue("LABEL_HSE_ON", PrintFrequency(freq.HSE).." (HSE)")
    this:SetFlagValue("LABEL_PLL_MUL", PrintFrequency(freq.PLLCLK).." (PLLCLK)")
    this:SetFlagValue("LABEL_SYSCLK_SRC", PrintFrequency(freq.SYSCLK).." (SYSCLK)")
    this:SetFlagValue("LABEL_RTCCLK_SRC", PrintFrequency(freq.RTCCLK).." (RTCCLK)")
    this:SetFlagValue("LABEL_MCO_SRC", PrintFrequency(freq.MCOCLK).." (MCOCLK)")
    this:SetFlagValue("LABEL_USB_DIV", PrintFrequency(freq.USBCLK).." (USBCLK)")
    this:SetFlagValue("LABEL_I2S2_SRC", PrintFrequency(freq.I2S2CLK).." (I2S2CLK)")
    this:SetFlagValue("LABEL_I2S3_SRC", PrintFrequency(freq.I2S3CLK).." (I2S3CLK)")
    this:SetFlagValue("LABEL_AHB_PRE", PrintFrequency(freq.HCLK).." (HCLK)")
    this:SetFlagValue("LABEL_APB1_PRE", PrintFrequency(freq.PCLK1).." (PCLK1)")
    this:SetFlagValue("LABEL_APB2_PRE", PrintFrequency(freq.PCLK2).." (PCLK2)")
    this:SetFlagValue("LABEL_ADC_PRE", PrintFrequency(freq.ADCCLK).." (ADCCLK)")
    this:SetFlagValue("__CLK_FLASH_LATENCY__", tostring(freq.FLASHLAT))
end

-- trigger frequency calculation after loading all flags
this.CalculateFreq()
++*/

#endif /* _CLK_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
