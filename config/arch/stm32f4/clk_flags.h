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
#define __CLK_HSE_ON__ RCC_HSE_ON

//==============================================================================
// PLL Configuration
//==============================================================================
/*--
this:AddExtraWidget("Label", "LABEL_PLL_CLKSRC", "\nPLLs clock source", -1, "bold")
this:AddExtraWidget("Void", "VoidPLLSRC1")
this:AddExtraWidget("Void", "VoidPLLSRC2")
++*/
/*--
this:AddWidget("Combobox", "Clock source")
this:AddItem("HSI",  "RCC_PLLSource_HSI")
this:AddItem("HSE", "RCC_PLLSource_HSE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_SRC", "")
--*/
#define __CLK_PLL_SRC__ RCC_PLLSource_HSE

/*--
this:AddWidget("Spinbox", 2, 63, "Clock divider (M)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_SRC_DIV_M", "")
--*/
#define __CLK_PLL_SRC_DIV_M__ 4

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
this:AddWidget("Spinbox", 50, 432, "Clock multiplier (N)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_N", "")
--*/
#define __CLK_PLL_N__ 168

/*--
this:AddWidget("Combobox", "Main clock divider (P)")
this:AddItem("VCO divided by 2",  "2")
this:AddItem("VCO divided by 4",  "4")
this:AddItem("VCO divided by 6",  "6")
this:AddItem("VCO divided by 8",  "8")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_P", "")
--*/
#define __CLK_PLL_P__ 2

/*--
this:AddWidget("Spinbox", 2, 15, "USB, SDIO clock divider (Q)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_Q", "")
--*/
#define __CLK_PLL_Q__ 7

/*--
this:AddWidget("Spinbox", 2, 7, "I2S, SAI clock divider (R)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL_R", "")
--*/
#define __CLK_PLL_R__ 7

//==============================================================================
// PLLI2S Configuration
//==============================================================================
/*--
this.is_PLLI2S =  uC.NAME:match("STM32F40") or uC.NAME:match("STM32F41") or uC.NAME:match("STM32F401")
               or uC.NAME:match("STM32F411.E") or uC.NAME:match("STM32F427") or uC.NAME:match("STM32F437")
               or uC.NAME:match("STM32F429") or uC.NAME:match("STM32F439") or uC.NAME:match("STM32F469")
               or uC.NAME:match("STM32F479") or uC.NAME:match("STM32F412.G") or uC.NAME:match("STM32F413")
               or uC.NAME:match("STM32F423") or uC.NAME:match("STM32F446")

if this.is_PLLI2S ~= nil then
    this:AddExtraWidget("Label", "LABEL_PLLI2S", "\nPLLI2S Configuration", -1, "bold")
    this:AddExtraWidget("Void", "VoidPLLI2S1")
    this:AddExtraWidget("Void", "VoidPLLI2S2")
end
++*/
/*--
if this.is_PLLI2S ~= nil then
    this:AddWidget("Combobox", "PLLI2S")
    this:AddItem("Enable",  "ENABLE")
    this:AddItem("Disable", "DISABLE")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_PLLI2S_ON", "")
end
--*/
#define __CLK_PLLI2S_ON__ DISABLE

/*--
if this.is_PLLI2S ~= nil then
    this:AddWidget("Spinbox", 50, 432, "Clock multiplier (N)")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_PLLI2S_N", "")
end
--*/
#define __CLK_PLLI2S_N__ 192

/*--
if this.is_PLLI2S ~= nil then
    this:AddWidget("Combobox", "48MHz clock divider (P)")
    this:AddItem("VCO divided by 2",  "2")
    this:AddItem("VCO divided by 4",  "4")
    this:AddItem("VCO divided by 6",  "6")
    this:AddItem("VCO divided by 8",  "8")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_PLLI2S_P", "")
end
--*/
#define __CLK_PLLI2S_P__ 2

/*--
if this.is_PLLI2S ~= nil then
    this:AddWidget("Spinbox", 2, 15, "SAI1 clock divider (Q)")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_PLLI2S_Q", "")
end
--*/
#define __CLK_PLLI2S_Q__ 2

/*--
if this.is_PLLI2S ~= nil then
    this:AddWidget("Spinbox", 2, 7, "I2S clock divider (R)")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_PLLI2S_R", "")
end
--*/
#define __CLK_PLLI2S_R__ 2


//==============================================================================
// PLLSAI Configuration
//==============================================================================
/*--
this.is_PLLSAI =  uC.NAME:match("STM32F469") or uC.NAME:match("STM32F479")
               or uC.NAME:match("STM32F446") or uC.NAME:match("STM32F427")
               or uC.NAME:match("STM32F437") or uC.NAME:match("STM32F429")
               or uC.NAME:match("STM32F439")

if this.is_PLLSAI ~= nil then
        this:AddExtraWidget("Label", "LABEL_PLLSAI", "\nPLLSAI Configuration", -1, "bold")
        this:AddExtraWidget("Void", "VoidPLLSAI1")
        this:AddExtraWidget("Void", "VoidPLLSAI2")
end
++*/
/*--
if this.is_PLLSAI ~= nil then
    this:AddWidget("Combobox", "PLLSAI")
    this:AddItem("Enable",  "ENABLE")
    this:AddItem("Disable", "DISABLE")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_PLLSAI_ON", "")
end
--*/
#define __CLK_PLLSAI_ON__ DISABLE

/*--
if this.is_PLLSAI ~= nil then
    this:AddWidget("Spinbox", 50, 432, "Clock multiplier (N)")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_PLLSAI_N", "")
end
--*/
#define __CLK_PLLSAI_N__ 203

/*--
if this.is_PLLSAI ~= nil then
    this:AddWidget("Combobox", "48MHz clock divider (P)")
    this:AddItem("VCO divided by 2",  "2")
    this:AddItem("VCO divided by 4",  "4")
    this:AddItem("VCO divided by 6",  "6")
    this:AddItem("VCO divided by 8",  "8")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_PLLSAI_P", "")
end
--*/
#define __CLK_PLLSAI_P__ 8

/*--
if this.is_PLLSAI ~= nil then
    this:AddWidget("Spinbox", 2, 15, "SAI1 clock divider (Q)")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_PLLSAI_Q", "")
end
--*/
#define __CLK_PLLSAI_Q__ 12

/*--
if this.is_PLLSAI ~= nil then
    this:AddWidget("Spinbox", 2, 7, "LTDC clock divider (R)")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_PLLSAI_R", "")
end
--*/
#define __CLK_PLLSAI_R__ 7

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
this:AddItem("HSI clock", "RCC_SYSCLKSource_HSI")
this:AddItem("HSE clock", "RCC_SYSCLKSource_HSE")
if uC.NAME:match("STM32F446") then
    this:AddItem("PLL clock", "RCC_SYSCLKSource_PLLPCLK")
else
    this:AddItem("PLL clock", "RCC_SYSCLKSource_PLLCLK")
end

if  uC.NAME:match("STM32F412") or uC.NAME:match("STM32F413")
 or uC.NAME:match("STM32F423") or uC.NAME:match("STM32F446") then
    this:AddItem("PLL R clock", "RCC_SYSCLKSource_PLLRCLK")
end

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
if uC.NAME:match("STM32F410") then this:AddItem("Disable", "0") end
this:AddItem("HSI", "RCC_MCO1Source_HSI")
this:AddItem("HSE", "RCC_MCO1Source_HSE")
this:AddItem("LSE", "RCC_MCO1Source_LSE")
this:AddItem("PLLCLK", "RCC_MCO1Source_PLLCLK")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_MCO1_SRC", "")
--*/
#define __CLK_MC01_CLK_SRC__ RCC_MCO1Source_HSE

/*--
this:AddWidget("Combobox", "MCO1 Clock divisor (output)")
for i = 1, 5 do this:AddItem("MCO divided by "..i, "RCC_MCO1Div_"..tostring(i)) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_MCO1_DIV", "")
--*/
#define __CLK_MC01_CLK_DIV__ RCC_MCO1Div_2

/*--
this:AddWidget("Combobox", "MCO2 Clock source")
if uC.NAME:match("STM32F410") then this:AddItem("Disable", "0") end
this:AddItem("SYSCLK", "RCC_MCO2Source_SYSCLK")
this:AddItem("PLLI2SCLK", "RCC_MCO2Source_PLLI2SCLK")
this:AddItem("HSE", "RCC_MCO2Source_HSE")
this:AddItem("PLLCLK", "RCC_MCO2Source_PLLCLK")
if uC.NAME:match("STM32F410") then this:AddItem("I2SCLK", "RCC_MCO2Source_I2SCLK") end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_MCO2_SRC", "")
--*/
#define __CLK_MC02_CLK_SRC__ RCC_MCO2Source_HSE

/*--
this:AddWidget("Combobox", "MCO2 Clock divisor (output)")
for i = 1, 5 do this:AddItem("MCO divided by "..i, "RCC_MCO2Div_"..tostring(i)) end
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_MCO2_DIV", "")
--*/
#define __CLK_MC02_CLK_DIV__ RCC_MCO2Div_1

/*--
if   uC.NAME:match("STM32F412") or uC.NAME:match("STM32F413")
  or uC.NAME:match("STM32F423") or uC.NAME:match("STM32F446") then

    this:AddWidget("Combobox", "I2S1 clock source")
    this:AddItem("I2S PLL R clock", "RCC_I2SCLKSource_PLLI2S")
    this:AddItem("External clock", "RCC_I2SCLKSource_Ext")
    this:AddItem("PLL R clock", "RCC_I2SCLKSource_PLL")
    this:AddItem("HSI or HSE clock", "RCC_I2SCLKSource_HSI_HSE")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_I2S1_CLK_SRC", "")
end
--*/
#define __CLK_I2SAPB1_CLK_SRC__ RCC_I2SCLKSource_HSI_HSE

/*--
if   uC.NAME:match("STM32F412") or uC.NAME:match("STM32F413")
  or uC.NAME:match("STM32F423") or uC.NAME:match("STM32F446") then

    this:AddWidget("Combobox", "I2S2 clock source")
    this:AddItem("PLL I2S R clock", "RCC_I2SCLKSource_PLLI2S")
    this:AddItem("External clock", "RCC_I2SCLKSource_Ext")
    this:AddItem("PLL R clock", "RCC_I2SCLKSource_PLL")
    this:AddItem("HSI or HSE clock", "RCC_I2SCLKSource_HSI_HSE")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_I2S2_CLK_SRC", "")
end
--*/
#define __CLK_I2SAPB2_CLK_SRC__ RCC_I2SCLKSource_HSI_HSE

/*--
if uC.NAME:match("STM32F410") then
    this:AddWidget("Combobox", "I2S clock source")
    this:AddItem("I2S PLLR", "RCC_I2SAPBCLKSOURCE_PLLR")
    this:AddItem("External clock", "RCC_I2SAPBCLKSOURCE_EXT")
    this:AddItem("PLL clock source", "RCC_I2SAPBCLKSOURCE_PLLSRC")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_I2S_CLK_SRC", "")

elseif uC.NAME:match("STM32F405") or uC.NAME:match("STM32F407") or uC.NAME:match("STM32F415")
    or uC.NAME:match("STM32F417") or uC.NAME:match("STM32F427")
    or uC.NAME:match("STM32F437") or uC.NAME:match("STM32F429") or uC.NAME:match("STM32F439")
    or uC.NAME:match("STM32F401") or uC.NAME:match("STM32F411") or uC.NAME:match("STM32F469")
    or uC.NAME:match("STM32F479") then

    this:AddWidget("Combobox", "I2S clock source")
    this:AddItem("I2S PLL clock", "RCC_I2S2CLKSource_PLLI2S")
    this:AddItem("External clock", "RCC_I2SAPBCLKSOURCE_EXT")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_I2S_CLK_SRC", "")
end
--*/
#define __CLK_I2S_CLK_SRC__ RCC_I2S2CLKSource_PLLI2S

/*--
if uC.NAME:match("STM32F446") then
    this:AddWidget("Combobox", "SAI1 clock source")
    this:AddItem("SAI PLL clock", "RCC_SAICLKSource_PLLSAI")
    this:AddItem("I2S PLL clock", "RCC_SAICLKSource_PLLI2S")
    this:AddItem("PLL R clock", "RCC_SAICLKSource_PLL")
    this:AddItem("PLL clock source", "RCC_SAICLKSource_HSI_HSE")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_SAI1_CLK_SRC", "")
end
--*/
#define __CLK_SAI1_CLK_SRC__ RCC_SAICLKSource_HSI_HSE

/*--
if uC.NAME:match("STM32F446") then
    this:AddWidget("Combobox", "SAI2 clock source")
    this:AddItem("SAI PLL clock", "RCC_SAICLKSource_PLLSAI")
    this:AddItem("I2S PLL clock", "RCC_SAICLKSource_PLLI2S")
    this:AddItem("PLL R clock", "RCC_SAICLKSource_PLL")
    this:AddItem("PLL clock source", "RCC_SAICLKSource_HSI_HSE")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_SAI2_CLK_SRC", "")
end
--*/
#define __CLK_SAI2_CLK_SRC__ RCC_SAICLKSource_HSI_HSE

/*--
if   uC.NAME:match("STM32F427") or uC.NAME:match("STM32F437")
  or uC.NAME:match("STM32F429") or uC.NAME:match("STM32F439")
  or uC.NAME:match("STM32F469") or uC.NAME:match("STM32F479")
  or uC.NAME:match("STM32F446") then

    this:AddWidget("Spinbox", 1, 32, "PLLSAI SAI clock divider")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_SAI_PLLSAI_CLK_DIV", "")
end
--*/
#define __CLK_SAI_PLLSAI_CLK_DIV__ 3

/*--
if   uC.NAME:match("STM32F427") or uC.NAME:match("STM32F437")
  or uC.NAME:match("STM32F429") or uC.NAME:match("STM32F439")
  or uC.NAME:match("STM32F469") or uC.NAME:match("STM32F479")
  or uC.NAME:match("STM32F446") then

    this:AddWidget("Spinbox", 1, 32, "PLLI2S SAI clock divider")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_SAI_PLLI2S_CLK_DIV", "")
end
--*/
#define __CLK_SAI_PLLI2S_CLK_DIV__ 1

/*--
if uC.NAME:match("STM32F413") or uC.NAME:match("STM32F423") then
    this:AddWidget("Spinbox", 1, 32, "PLLI2S SAI clock divider")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_SAI_PLLI2S_CLK_DIVR", "")
end
--*/
#define __CLK_SAI_PLLI2S_CLK_DIVR__ 1

/*--
if uC.NAME:match("STM32F413") or uC.NAME:match("STM32F423") then
    this:AddWidget("Spinbox", 1, 32, "PLL SAI clock divider")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_SAI_PLL_CLK_DIVR", "")
end
--*/
#define __CLK_SAI_PLL_CLK_DIVR__ 1

/*--
if   uC.NAME:match("STM32F427") or uC.NAME:match("STM32F437")
  or uC.NAME:match("STM32F429") or uC.NAME:match("STM32F439")
  or uC.NAME:match("STM32F469") or uC.NAME:match("STM32F479") then

    this:AddWidget("Combobox", "SAI Block A clock source")
    this:AddItem("I2S PLL clock", "RCC_SAIACLKSource_PLLI2S")
    this:AddItem("SAI PLL clock", "RCC_SAIACLKSource_PLLSAI")
    this:AddItem("External clock", "RCC_SAIACLKSource_Ext")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_SAI_BLOCK_A_CLK_SRC", "")

elseif uC.NAME:match("STM32F413") or uC.NAME:match("STM32F423") then

    this:AddWidget("Combobox", "SAI Block A clock source")
    this:AddItem("I2S PLL R clock", "RCC_SAIACLKSource_PLLI2SR")
    this:AddItem("I2S PLL Q clock", "RCC_SAIACLKSource_PLLI2S")
    this:AddItem("PLL clock", "RCC_SAIACLKSource_PLL")
    this:AddItem("HSI or HSE clock", "RCC_SAIACLKSource_HSI_HSE")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_SAI_BLOCK_A_CLK_SRC", "")
end
--*/
#define __CLK_SAI_BLOCK_A_CLK_SRC__ RCC_SAIACLKSource_PLLSAI

/*--
if   uC.NAME:match("STM32F427") or uC.NAME:match("STM32F437")
  or uC.NAME:match("STM32F429") or uC.NAME:match("STM32F439")
  or uC.NAME:match("STM32F469") or uC.NAME:match("STM32F479") then

    this:AddWidget("Combobox", "SAI Block B clock source")
    this:AddItem("I2S PLL clock", "RCC_SAIBCLKSource_PLLI2S")
    this:AddItem("SAI PLL clock", "RCC_SAIBCLKSource_PLLSAI")
    this:AddItem("External clock", "RCC_SAIBCLKSource_Ext")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_SAI_BLOCK_B_CLK_SRC", "")

elseif uC.NAME:match("STM32F413") or uC.NAME:match("STM32F423") then

    this:AddWidget("Combobox", "SAI Block B clock source")
    this:AddItem("I2S PLL R clock", "RCC_SAIBCLKSource_PLLI2SR")
    this:AddItem("I2S PLL Q clock", "RCC_SAIBCLKSource_PLLI2S")
    this:AddItem("PLL clock", "RCC_SAIBCLKSource_PLL")
    this:AddItem("HSI or HSE clock", "RCC_SAIBACLKSource_HSI_HSE")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_SAI_BLOCK_B_CLK_SRC", "")
end
--*/
#define __CLK_SAI_BLOCK_B_CLK_SRC__ RCC_SAIBCLKSource_PLLSAI

/*--
if   uC.NAME:match("STM32F427") or uC.NAME:match("STM32F437")
  or uC.NAME:match("STM32F429") or uC.NAME:match("STM32F439")
  or uC.NAME:match("STM32F469") or uC.NAME:match("STM32F479") then

    this:AddWidget("Combobox", "LTDC clock divider")
    this:AddItem("PLLSAI R / 2",  "RCC_PLLSAIDivR_Div2")
    this:AddItem("PLLSAI R / 4",  "RCC_PLLSAIDivR_Div4")
    this:AddItem("PLLSAI R / 8",  "RCC_PLLSAIDivR_Div8")
    this:AddItem("PLLSAI R / 16", "RCC_PLLSAIDivR_Div16")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_LTDC_CLK_DIV", "")
end
--*/
#define __CLK_LTDC_DIV__ RCC_PLLSAIDivR_Div2

/*--
if   uC.NAME:match("STM32F412.G") or uC.NAME:match("STM32F413")
  or uC.NAME:match("STM32F423") then

    this:AddWidget("Combobox", "DFSDM1 clock source")
    this:AddItem("APB1 clock",  "RCC_DFSDMCLKSource_APB")
    this:AddItem("SYSCLK",  "RCC_DFSDMCLKSource_SYS")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_DFSDM1_CLK_SRC", "")
end
--*/
#define __CLK_DFSDM1_CLK_SRC__ RCC_DFSDMCLKSource_APB

/*--
if   uC.NAME:match("STM32F412.G") or uC.NAME:match("STM32F413")
  or uC.NAME:match("STM32F423") then

    this:AddWidget("Combobox", "DFSDM1 audio clock source")
    this:AddItem("APB1 clock",  "RCC_DFSDM1AUDIOCLKSOURCE_I2SAPB1")
    this:AddItem("APB2 clock",  "RCC_DFSDM1AUDIOCLKSOURCE_I2SAPB2")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_DFSDM1_ACLK_SRC", "")
end
--*/
#define __CLK_DFSDM1_ACLK_SRC__ RCC_DFSDM1AUDIOCLKSOURCE_I2SAPB1

/*--
if uC.NAME:match("STM32F413") or uC.NAME:match("STM32F423") then

    this:AddWidget("Combobox", "DFSDM2 audio clock source")
    this:AddItem("APB1 clock",  "RCC_DFSDM2AUDIOCLKSOURCE_I2SAPB1")
    this:AddItem("APB2 clock",  "RCC_DFSDM2AUDIOCLKSOURCE_I2SAPB2")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_DFSDM2_ACLK_SRC", "")
end
--*/
#define __CLK_DFSDM2_ACLK_SRC__ RCC_DFSDM2AUDIOCLKSOURCE_I2SAPB1

/*--
if uC.NAME:match("STM32F469") or uC.NAME:match("STM32F479") then

    this:AddWidget("Combobox", "DSI clock source")
    this:AddItem("PHY clock",  "RCC_DSICLKSource_PHY")
    this:AddItem("PLLR clock", "RCC_DSICLKSource_PLLR")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_DSI_CLK_SRC", "")
end
--*/
#define __CLK_DSI_SRC__ RCC_DSICLKSource_PHY

/*--
if   uC.NAME:match("STM32F412.G") or uC.NAME:match("STM32F446")
  or uC.NAME:match("STM32F413")   or uC.NAME:match("STM32F423")
  or uC.NAME:match("STM32F469")   or uC.NAME:match("STM32F479") then

    this:AddWidget("Combobox", "48MHz clock source")
    this:AddItem("PLL", "RCC_48MHZCLKSource_PLL")
    this:AddItem("PLLSAI", "RCC_48MHZCLKSource_PLLSAI")
    this:AddItem("PLLI2SQ", "RCC_CK48CLKSOURCE_PLLI2SQ")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_48MHZ_CLK_SRC", "")
end
--*/
#define __CLK_48MHZ_SRC__ RCC_48MHZCLKSource_PLL

/*--
if   uC.NAME:match("STM32F412.G") or uC.NAME:match("STM32F446")
  or uC.NAME:match("STM32F413")   or uC.NAME:match("STM32F423")
  or uC.NAME:match("STM32F469")   or uC.NAME:match("STM32F479") then

    this:AddWidget("Combobox", "SDIO clock source")
    this:AddItem("PLL48CLK", "RCC_SDIOCLKSource_48MHZ")
    this:AddItem("SYSCLK", "RCC_SDIOCLKSource_SYSCLK")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_SDIO_CLK_SRC", "")
end
--*/
#define __CLK_SDIO_SRC__ RCC_SDIOCLKSource_48MHZ

/*--
if uC.NAME:match("STM32F446") then

    this:AddWidget("Combobox", "SPDIFRX clock source")
    this:AddItem("PLL R", "RCC_SPDIFRXCLKSource_PLLR")
    this:AddItem("PLLI2S P", "RCC_SPDIFRXCLKSource_PLLI2SP")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_SPDIF_SRC", "")
end
--*/
#define __CLK_SPDIF_SRC__ RCC_SPDIFRXCLKSource_PLLR

/*--
if uC.NAME:match("STM32F446") then

    this:AddWidget("Combobox", "CEC clock source")
    this:AddItem("HSI / 488", "RCC_CECCLKSource_HSIDiv488")
    this:AddItem("LSE", "RCC_CECCLKSource_LSE")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_CEC_SRC", "")
end
--*/
#define __CLK_CEC_SRC__ RCC_CECCLKSource_HSIDiv488

/*--
if   uC.NAME:match("STM32F412.G") or uC.NAME:match("STM32F446")
  or uC.NAME:match("STM32F413")   or uC.NAME:match("STM32F423")
  or uC.NAME:match("STM32F410") then

    this:AddWidget("Combobox", "FMPI2C1 clock source")
    this:AddItem("APB1 clock", "RCC_FMPI2C1CLKSource_APB1")
    this:AddItem("SYSCLK", "RCC_FMPI2C1CLKSource_SYSCLK")
    this:AddItem("HSI", "RCC_FMPI2C1CLKSource_HSI")
    this:SetEvent("clicked", function() this.CalculateFreq() end)
    this:AddExtraWidget("Label", "LABEL_FMPI2C1_SRC", "")
end
--*/
#define __CLK_FMPI2C1_SRC__ RCC_FMPI2C1CLKSource_APB1


//==============================================================================
// System Clocks Prescallers Configuration
//==============================================================================
/*--
this:AddExtraWidget("Label", "LABEL_SysPre", "\nSystem prescalers", -1, "bold")
this:AddExtraWidget("Void", "VoidSysPre1")
this:AddExtraWidget("Void", "VoidSysPre2")
++*/
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
#define __CLK_FLASH_LATENCY__ 5


//==============================================================================
// Calculation script
//==============================================================================
/*--
this.CalculateFreq = function(self)
    local PLLON         = iff(this:GetFlagValue("__CLK_PLL_ON__") == "ENABLE", 1, 0)
    local PLLSRC        = this:GetFlagValue("__CLK_PLL_SRC__")
    local PLLM          = this:GetFlagValue("__CLK_PLL_SRC_DIV_M__")
    local PLLN          = this:GetFlagValue("__CLK_PLL_N__")
    local PLLP          = this:GetFlagValue("__CLK_PLL_P__")
    local PLLQ          = this:GetFlagValue("__CLK_PLL_Q__")
    local PLLR          = this:GetFlagValue("__CLK_PLL_R__")

    local PLLI2SON      = iff(this:GetFlagValue("__CLK_PLLI2S_ON__") == "ENABLE", 1, 0)
    local PLLI2SM       = this:GetFlagValue("__CLK_PLL_SRC_DIV_M__")
    local PLLI2SN       = this:GetFlagValue("__CLK_PLLI2S_N__")
    local PLLI2SP       = this:GetFlagValue("__CLK_PLLI2S_P__")
    local PLLI2SQ       = this:GetFlagValue("__CLK_PLLI2S_Q__")
    local PLLI2SR       = this:GetFlagValue("__CLK_PLLI2S_R__")

    local PLLSAION      = iff(this:GetFlagValue("__CLK_PLLSAI_ON__") == "ENABLE", 1, 0)
    local PLLSAIM       = this:GetFlagValue("__CLK_PLL_SRC_DIV_M__")
    local PLLSAIN       = this:GetFlagValue("__CLK_PLLSAI_N__")
    local PLLSAIP       = this:GetFlagValue("__CLK_PLLSAI_P__")
    local PLLSAIQ       = this:GetFlagValue("__CLK_PLLSAI_Q__")
    local PLLSAIR       = this:GetFlagValue("__CLK_PLLSAI_R__")

    local HSEON         = iff(this:GetFlagValue("__CLK_HSE_ON__") == "RCC_HSE_OFF", 0, 1)
    local LSEON         = iff(this:GetFlagValue("__CLK_LSE_ON__") == "RCC_LSE_OFF", 0, 1)
    local LSION         = iff(this:GetFlagValue("__CLK_LSI_ON__") == "ENABLE", 1, 0)

    local SYSCLKSRC     = this:GetFlagValue("__CLK_SYS_CLK_SRC__")
    local RTCSRC        = this:GetFlagValue("__CLK_RTC_CLK_SRC__")

    local AHBPRE        = this:GetFlagValue("__CLK_AHB_PRE__")
    local APB1PRE       = this:GetFlagValue("__CLK_APB1_PRE__")
    local APB2PRE       = this:GetFlagValue("__CLK_APB2_PRE__")

    local MCO1SRC       = this:GetFlagValue("__CLK_MC01_CLK_SRC__")
    local MCO1DIV       = this:GetFlagValue("__CLK_MC01_CLK_DIV__"):gsub("RCC_MCO1Div_", "")
    local MCO2SRC       = this:GetFlagValue("__CLK_MC02_CLK_SRC__")
    local MCO2DIV       = this:GetFlagValue("__CLK_MC02_CLK_DIV__"):gsub("RCC_MCO2Div_", "")
    local CPUVOLTAGE    = this:GetFlagValue("__CLK_CPU_VOLTAGE__")

    local I2S1SRC       = this:GetFlagValue("__CLK_I2SAPB1_CLK_SRC__")
    local I2S2SRC       = this:GetFlagValue("__CLK_I2SAPB2_CLK_SRC__")
    local I2SSRC        = this:GetFlagValue("__CLK_I2S_CLK_SRC__")

    local SAI1SRC       = this:GetFlagValue("__CLK_SAI1_CLK_SRC__")
    local SAI2SRC       = this:GetFlagValue("__CLK_SAI2_CLK_SRC__")

    local SAIPLLSAIDIV  = this:GetFlagValue("__CLK_SAI_PLLSAI_CLK_DIV__")
    local SAIPLLI2SDIV  = this:GetFlagValue("__CLK_SAI_PLLI2S_CLK_DIV__")
    local SAIPLLI2SDIVR = this:GetFlagValue("__CLK_SAI_PLLI2S_CLK_DIVR__")
    local SAIPLLDIVR    = this:GetFlagValue("__CLK_SAI_PLL_CLK_DIVR__")
    local SAIBLKASRC    = this:GetFlagValue("__CLK_SAI_BLOCK_A_CLK_SRC__")
    local SAIBLKBSRC    = this:GetFlagValue("__CLK_SAI_BLOCK_B_CLK_SRC__")

    local LTDCDIV       = this:GetFlagValue("__CLK_LTDC_DIV__"):gsub("RCC_PLLSAIDivR_Div", "")
    local DSISRC        = this:GetFlagValue("__CLK_DSI_SRC__")
    local CLK48MHZSRC   = this:GetFlagValue("__CLK_48MHZ_SRC__")
    local SDIOCLKSRC    = this:GetFlagValue("__CLK_SDIO_SRC__")
    local SPIDFSRC      = this:GetFlagValue("__CLK_SPDIF_SRC__")
    local CECSRC        = this:GetFlagValue("__CLK_CEC_SRC__")
    local FMPI2C1SRC    = this:GetFlagValue("__CLK_FMPI2C1_SRC__")

    local DFSDM1CLKSRC  = this:GetFlagValue("__CLK_DFSDM1_CLK_SRC__")
    local DFSDM1ACLKSRC = this:GetFlagValue("__CLK_DFSDM1_ACLK_SRC__")
    local DFSDM2ACLKSRC = this:GetFlagValue("__CLK_DFSDM2_ACLK_SRC__")

    local freq          = {}
    freq.HSE            = uC.OSCFREQ * HSEON
    freq.HSI            = 16e6
    freq.LSI            = 32e3 * LSION
    freq.LSE            = 32768 * LSEON
    freq.I2SCLKIN       = 2.048e6
    freq.DSIPHYCLK      = 62.5e6
    freq.RTCCLK         = 0
    freq.SYSCLK         = 0
    freq.PLLCLK         = 0
    freq.PLLVCO         = 0
    freq.PLL48CLK       = 0
    freq.PLLQ           = 0
    freq.PLLR           = 0
    freq.PLLM           = 0
    freq.PLLI2SM        = 0
    freq.PLLI2SVCO      = 0
    freq.PLLI2SP        = 0
    freq.PLLI2SQ        = 0
    freq.PLLI2SR        = 0
    freq.PLLSAIM        = 0
    freq.PLLSAIVCO      = 0
    freq.PLLSAIP        = 0
    freq.PLLSAIQ        = 0
    freq.PLLSAIR        = 0
    freq.HCLK           = 0
    freq.PCLK1          = 0
    freq.PCLK2          = 0
    freq.TIMCLK1        = 0
    freq.TIMCLK2        = 0
    freq.MCO1CLK        = 0
    freq.MCO1CLKDIV     = 0
    freq.MCO2CLK        = 0
    freq.MCO2CLKDIV     = 0
    freq.I2S1CLK        = 0
    freq.I2S2CLK        = 0
    freq.I2SCLK         = 0
    freq.SAI1CLK        = 0
    freq.SAI2CLK        = 0
    freq.SAIBLKACLK     = 0
    freq.SAIBLKBCLK     = 0
    freq.LTDCCLK        = 0
    freq.DFSDM1CLK      = 0
    freq.DFSDM1ACLK     = 0
    freq.DFSDM2ACLK     = 0
    freq.DSICLK         = 0
    freq.CLK48MHZ       = 0
    freq.SDIOCLK        = 0
    freq.SPDIF          = 0
    freq.CEC            = 0
    freq.FMPI2C1        = 0
    freq.FLASHLATENCY   = 0

    -- print base frequencies --------------------------------------------------
    this:SetFlagValue("LABEL_LSI_ON", PrintFrequency(freq.LSI).." (LSI)")
    this:SetFlagValue("LABEL_LSE_ON", PrintFrequency(freq.LSE).." (LSE)")
    this:SetFlagValue("LABEL_HSE_ON", PrintFrequency(freq.HSE).." (HSE)")

    -- calculate RTCCLK --------------------------------------------------------
    if RTCSRC == "RCC_RTCCLKSource_LSI" then
        freq.RTCCLK = freq.LSI
    elseif RTCSRC == "RCC_RTCCLKSource_LSE" then
        freq.RTCCLK = freq.LSE
    else
        freq.RTCCLK = freq.HSE / RTCSRC:gsub("RCC_RTCCLKSource_HSE_Div", "")
    end
    this:SetFlagValue("LABEL_RTCCLK_SRC", PrintFrequency(freq.RTCCLK).." (RTCCLK)")

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
           this:SetFlagValue("LABEL_PLL_SRC_DIV_M", PrintFrequency(freq.PLLM).." [Keep in range 1-2MHz]")
        else
           this:SetFlagValue("LABEL_PLL_SRC_DIV_M", PrintFrequency(freq.PLLM))
        end

        freq.PLLVCO = freq.PLLM * PLLN
        if freq.PLLVCO < 100e6 or freq.PLLVCO > 432e6 then
           this:SetFlagValue("LABEL_PLL_N", PrintFrequency(freq.PLLVCO).." [Keep in range 100-432MHz]")
        else
           this:SetFlagValue("LABEL_PLL_N", PrintFrequency(freq.PLLVCO))
        end

        freq.PLLCLK = freq.PLLVCO / PLLP
        this:SetFlagValue("LABEL_PLL_P", PrintFrequency(freq.PLLCLK).." (PLLCLK)")

        freq.PLLQ     = freq.PLLVCO / PLLQ
        freq.PLL48CLK = freq.PLLQ
        this:SetFlagValue("LABEL_PLL_Q", PrintFrequency(freq.PLL48CLK).." (PLL48CLK)")

        freq.PLLR = freq.PLLVCO / PLLR
        this:SetFlagValue("LABEL_PLL_R", PrintFrequency(freq.PLLR))
    else
        this:SetFlagValue("LABEL_PLL_SRC", "")
        this:SetFlagValue("LABEL_PLL_SRC_DIV_M", "")
        this:SetFlagValue("LABEL_PLL_N", "")
        this:SetFlagValue("LABEL_PLL_P", "")
        this:SetFlagValue("LABEL_PLL_Q", "")
        this:SetFlagValue("LABEL_PLL_R", "")
    end

    -- calculate PLLI2S Clk frequency ------------------------------------------
    if this.is_PLLI2S ~= nil then
        if PLLI2SON == 1 then
            if PLLSRC == "RCC_PLLSource_HSI" then
               this:SetFlagValue("LABEL_PLL_SRC", PrintFrequency(freq.HSI))
               freq.PLLI2SM = freq.HSI / PLLI2SM
            else
               this:SetFlagValue("LABEL_PLL_SRC", PrintFrequency(freq.HSE))
               freq.PLLI2SM = freq.HSE / PLLI2SM
            end

            freq.PLLI2SVCO = freq.PLLI2SM * PLLI2SN
            if freq.PLLI2SVCO < 100e6 or freq.PLLI2SVCO > 432e6 then
               this:SetFlagValue("LABEL_PLLI2S_N", PrintFrequency(freq.PLLI2SVCO).." [Keep in range 100-432MHz]")
            else
               this:SetFlagValue("LABEL_PLLI2S_N", PrintFrequency(freq.PLLI2SVCO))
            end

            freq.PLLI2SP = freq.PLLI2SVCO / PLLI2SP
            this:SetFlagValue("LABEL_PLLI2S_P", PrintFrequency(freq.PLLI2SP))

            freq.PLLI2SQ = freq.PLLI2SVCO / PLLI2SQ
            this:SetFlagValue("LABEL_PLLI2S_Q", PrintFrequency(freq.PLLI2SQ).." (PLLSAICLK)")

            freq.PLLI2SR = freq.PLLI2SVCO / PLLI2SR
            if freq.PLLI2SR > 192e6 then
                this:SetFlagValue("LABEL_PLLI2S_R", PrintFrequency(freq.PLLI2SR).." (PLLI2SCLK) [Max is 192 MHz!]")
            else
                this:SetFlagValue("LABEL_PLLI2S_R", PrintFrequency(freq.PLLI2SR).." (PLLI2SCLK)")
            end
        else
            this:SetFlagValue("LABEL_PLLI2S_N", "")
            this:SetFlagValue("LABEL_PLLI2S_P", "")
            this:SetFlagValue("LABEL_PLLI2S_Q", "")
            this:SetFlagValue("LABEL_PLLI2S_R", "")
        end
    end

    -- calculate PLLSAI Clk frequency ------------------------------------------
    if this.is_PLLSAI ~= nil then
        if PLLSAION == 1 then
            if PLLSRC == "RCC_PLLSource_HSI" then
               this:SetFlagValue("LABEL_PLL_SRC", PrintFrequency(freq.HSI))
               freq.PLLSAIM = freq.HSI / PLLSAIM
            else
               this:SetFlagValue("LABEL_PLL_SRC", PrintFrequency(freq.HSE))
               freq.PLLSAIM = freq.HSE / PLLSAIM
            end

            freq.PLLSAIVCO = freq.PLLSAIM * PLLSAIN
            if freq.PLLSAIVCO < 100e6 or freq.PLLSAIVCO > 432e6 then
               this:SetFlagValue("LABEL_PLLSAI_N", PrintFrequency(freq.PLLSAIVCO).." [Keep in range 100-432MHz]")
            else
               this:SetFlagValue("LABEL_PLLSAI_N", PrintFrequency(freq.PLLSAIVCO))
            end

            freq.PLLSAIP = freq.PLLSAIVCO / PLLSAIP
            this:SetFlagValue("LABEL_PLLSAI_P", PrintFrequency(freq.PLLSAIP))

            freq.PLLSAIQ = freq.PLLSAIVCO / PLLSAIQ
            this:SetFlagValue("LABEL_PLLSAI_Q", PrintFrequency(freq.PLLSAIQ).." (PLLSAICLK)")

            freq.PLLSAIR = freq.PLLSAIVCO / PLLSAIR
            this:SetFlagValue("LABEL_PLLSAI_R", PrintFrequency(freq.PLLSAIR).." (PLLLCDCLK)")
        else
            this:SetFlagValue("LABEL_PLLSAI_N", "")
            this:SetFlagValue("LABEL_PLLSAI_P", "")
            this:SetFlagValue("LABEL_PLLSAI_Q", "")
            this:SetFlagValue("LABEL_PLLSAI_R", "")
        end
    end

    -- calculate SYSCLK --------------------------------------------------------
    if SYSCLKSRC == "RCC_SYSCLKSource_HSE" then
        freq.SYSCLK = freq.HSE
    elseif SYSCLKSRC == "RCC_SYSCLKSource_HSI" then
        freq.SYSCLK = freq.HSI
    else
        freq.SYSCLK = freq.PLLCLK
    end
    this:SetFlagValue("LABEL_SYSCLK_SRC", PrintFrequency(freq.SYSCLK).." (SYSCLK)")

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
    this:SetFlagValue("LABEL_MCO1_SRC", PrintFrequency(freq.MCO1CLK))
    this:SetFlagValue("LABEL_MCO1_DIV", PrintFrequency(freq.MCO1CLKDIV).." (MCO1 pin)")

    -- calculate MCO2 frequency ------------------------------------------------
    if MCO2SRC == "RCC_MCO2Source_SYSCLK" then
        freq.MCO2CLK = freq.SYSCLK
    elseif MCO2SRC == "RCC_MCO2Source_PLLI2SCLK" then
        freq.MCO2CLK = freq.PLLI2SR
    elseif MCO2SRC == "RCC_MCO2Source_I2SCLK" then
        freq.MCO2CLK = freq.PLLI2SR
    elseif MCO2SRC == "RCC_MCO2Source_HSE" then
        freq.MCO2CLK = freq.HSE
    elseif MCO2SRC == "RCC_MCO2Source_PLLCLK" then
        freq.MCO2CLK = freq.PLLCLK
    else
        freq.MCO2CLK = freq.SYSCLK
    end
    freq.MCO2CLKDIV = freq.MCO2CLK / MCO2DIV
    this:SetFlagValue("LABEL_MCO2_SRC", PrintFrequency(freq.MCO2CLK))
    this:SetFlagValue("LABEL_MCO2_DIV", PrintFrequency(freq.MCO2CLKDIV).." (MCO2 pin)")

    -- calculate I2S1 clock frequency ------------------------------------------
    if   uC.NAME:match("STM32F412") or uC.NAME:match("STM32F413")
      or uC.NAME:match("STM32F423") or uC.NAME:match("STM32F446") then

        if I2S1SRC == "RCC_I2SCLKSource_PLLI2S" then
            freq.I2S1CLK = freq.PLLI2SR
        elseif I2S1SRC == "RCC_I2SCLKSource_Ext" then
            freq.I2S1CLK = freq.I2SCLKIN
        elseif I2S1SRC == "RCC_I2SCLKSource_PLL" then
            freq.I2S1CLK = freq.PLLR
        else -- RCC_I2SCLKSource_HSI_HSE
            if PLLSRC == "RCC_PLLSource_HSI" then
                freq.I2S1CLK = freq.HSI
            else
                freq.I2S1CLK = freq.HSE
            end
        end
        this:SetFlagValue("LABEL_I2S1_CLK_SRC", PrintFrequency(freq.I2S1CLK))
    end

    -- calculate I2S2 clock frequency ------------------------------------------
    if   uC.NAME:match("STM32F412") or uC.NAME:match("STM32F413")
      or uC.NAME:match("STM32F423") or uC.NAME:match("STM32F446") then

        if I2S2SRC == "RCC_I2SCLKSource_PLLI2S" then
            freq.I2S2CLK = freq.PLLI2SR
        elseif I2S2SRC == "RCC_I2SCLKSource_Ext" then
            freq.I2S2CLK = freq.I2SCLKIN
        elseif I2S2SRC == "RCC_I2SCLKSource_PLL" then
            freq.I2S2CLK = freq.PLLR
        else -- RCC_I2SCLKSource_HSI_HSE
            if PLLSRC == "RCC_PLLSource_HSI" then
                freq.I2S2CLK = freq.HSI
            else
                freq.I2S2CLK = freq.HSE
            end
        end
        this:SetFlagValue("LABEL_I2S2_CLK_SRC", PrintFrequency(freq.I2S2CLK))
    end

    -- calculate I2S clock frequency -------------------------------------------
    if uC.NAME:match("STM32F410") then
        if I2SSRC == "RCC_I2SAPBCLKSOURCE_PLLR" then
            freq.I2SCLK = freq.PLLI2SR
        elseif I2SSRC == "RCC_I2SAPBCLKSOURCE_EXT" then
            freq.I2SCLK = freq.I2SCLKIN
        elseif I2SSRC == "RCC_I2SAPBCLKSOURCE_PLLSRC" then
            if PLLSRC == "RCC_PLLSource_HSI" then
                freq.I2SCLK = freq.HSI
            else
                freq.I2SCLK = freq.HSE
            end
        else
            freq.I2SCLK = 0
        end
        this:SetFlagValue("LABEL_I2S_CLK_SRC", PrintFrequency(freq.I2SCLK))

    elseif uC.NAME:match("STM32F405") or uC.NAME:match("STM32F407") or uC.NAME:match("STM32F415")
        or uC.NAME:match("STM32F417") or uC.NAME:match("STM32F427")
        or uC.NAME:match("STM32F437") or uC.NAME:match("STM32F429") or uC.NAME:match("STM32F439")
        or uC.NAME:match("STM32F401") or uC.NAME:match("STM32F411") or uC.NAME:match("STM32F469")
        or uC.NAME:match("STM32F479") then

        if I2SSRC == "RCC_I2S2CLKSource_PLLI2S" then
            freq.I2SCLK = freq.PLLI2SR
        elseif I2SSRC == "RCC_I2SAPBCLKSOURCE_EXT" then
            freq.I2SCLK = freq.I2SCLKIN
        else
            freq.I2SCLK = 0
        end
        this:SetFlagValue("LABEL_I2S_CLK_SRC", PrintFrequency(freq.I2SCLK))
    end

    -- calculate SAI1 clock frequency ------------------------------------------
    if uC.NAME:match("STM32F446") then
        if SAI1SRC == "RCC_SAICLKSource_PLLSAI" then
            freq.SAI1CLK = freq.PLLSAIQ
        elseif SAI1SRC == "RCC_SAICLKSource_PLLI2S" then
            freq.SAI1CLK = freq.PLLI2SQ
        elseif SAI1SRC == "RCC_SAICLKSource_PLL" then
            freq.SAI1CLK = freq.PLLR
        elseif SAI1SRC == "RCC_SAICLKSource_HSI_HSE" then
            if PLLSRC == "RCC_PLLSource_HSI" then
                freq.SAI1CLK = freq.HSI
            else
                freq.SAI1CLK = freq.HSE
            end
        end
        this:SetFlagValue("LABEL_SAI1_CLK_SRC", PrintFrequency(freq.SAI1CLK))
    end

    -- calculate SAI2 clock frequency ------------------------------------------
    if uC.NAME:match("STM32F446") then
        if SAI2SRC == "RCC_SAICLKSource_PLLSAI" then
            freq.SAI2CLK = freq.PLLSAIQ
        elseif SAI2SRC == "RCC_SAICLKSource_PLLI2S" then
            freq.SAI2CLK = freq.PLLI2SQ
        elseif SAI2SRC == "RCC_SAICLKSource_PLL" then
            freq.SAI2CLK = freq.PLLR
        elseif SAI2SRC == "RCC_SAICLKSource_HSI_HSE" then
            if PLLSRC == "RCC_PLLSource_HSI" then
                freq.SAI2CLK = freq.HSI
            else
                freq.SAI2CLK = freq.HSE
            end
        end
        this:SetFlagValue("LABEL_SAI2_CLK_SRC", PrintFrequency(freq.SAI2CLK))
    end

    -- calculate SAI Block A clock frequency -----------------------------------
    if   uC.NAME:match("STM32F427") or uC.NAME:match("STM32F437")
      or uC.NAME:match("STM32F429") or uC.NAME:match("STM32F439")
      or uC.NAME:match("STM32F469") or uC.NAME:match("STM32F479") then

        if SAIBLKASRC == "RCC_SAIACLKSource_PLLI2S" then
            freq.SAIBLKACLK = freq.PLLI2SQ / SAIPLLI2SDIV
        elseif SAIBLKASRC == "RCC_SAIACLKSource_PLLSAI" then
            freq.SAIBLKACLK = freq.PLLSAIQ / SAIPLLSAIDIV
        elseif SAIBLKASRC == "RCC_SAIACLKSource_Ext" then
            freq.SAIBLKACLK = freq.I2SCLKIN
        else
            freq.SAIBLKACLK = 0
        end

        this:SetFlagValue("LABEL_SAI_BLOCK_A_CLK_SRC", PrintFrequency(freq.SAIBLKACLK))

    elseif uC.NAME:match("STM32F413") or uC.NAME:match("STM32F423") then
        if SAIBLKASRC == "RCC_SAIACLKSource_PLLI2SR" then
            freq.SAIBLKACLK = freq.PLLI2SR / SAIPLLI2SDIVR
        elseif SAIBLKASRC == "RCC_SAIACLKSource_PLLI2S" then
            freq.SAIBLKACLK = freq.PLLI2SR
        elseif SAIBLKASRC == "RCC_SAIACLKSource_PLL" then
            freq.SAIBLKACLK = freq.PLLR / SAIPLLDIVR
        elseif SAIBLKASRC == "RCC_SAIACLKSource_HSI_HSE" then
            freq.SAIBLKACLK = iff(PLLSRC == "RCC_PLLSource_HSI", freq.HSI, freq.HSE)
        else
            freq.SAIBLKACLK = 0
        end

        this:SetFlagValue("LABEL_SAI_BLOCK_A_CLK_SRC", PrintFrequency(freq.SAIBLKACLK))
    end

    -- calculate SAI Block B clock frequency -----------------------------------
    if   uC.NAME:match("STM32F427") or uC.NAME:match("STM32F437")
      or uC.NAME:match("STM32F429") or uC.NAME:match("STM32F439")
      or uC.NAME:match("STM32F469") or uC.NAME:match("STM32F479") then

        if SAIBLKBSRC == "RCC_SAIBCLKSource_PLLI2S" then
            freq.SAIBLKBCLK = freq.PLLI2SQ / SAIPLLI2SDIV
        elseif SAIBLKBSRC == "RCC_SAIBCLKSource_PLLSAI" then
            freq.SAIBLKBCLK = freq.PLLSAIQ / SAIPLLSAIDIV
        elseif SAIBLKBSRC == "RCC_SAIBCLKSource_Ext" then
            freq.SAIBLKBCLK = freq.I2SCLKIN
        else
            freq.SAIBLKBCLK = 0
        end

        this:SetFlagValue("LABEL_SAI_BLOCK_B_CLK_SRC", PrintFrequency(freq.SAIBLKBCLK))

    elseif uC.NAME:match("STM32F413") or uC.NAME:match("STM32F423") then
        if SAIBLKBSRC == "RCC_SAIBCLKSource_PLLI2SR" then
            freq.SAIBLKBCLK = freq.PLLI2SR / SAIPLLI2SDIVR
        elseif SAIBLKBSRC == "RCC_SAIBCLKSource_PLLI2S" then
            freq.SAIBLKBCLK = freq.PLLI2SR
        elseif SAIBLKBSRC == "RCC_SAIBCLKSource_PLL" then
            freq.SAIBLKBCLK = freq.PLLR / SAIPLLDIVR
        elseif SAIBLKBSRC == "RCC_SAIBCLKSource_HSI_HSE" then
            freq.SAIBLKBCLK = iff(PLLSRC == "RCC_PLLSource_HSI", freq.HSI, freq.HSE)
        else
            freq.SAIBLKBCLK = 0
        end

        this:SetFlagValue("LABEL_SAI_BLOCK_B_CLK_SRC", PrintFrequency(freq.SAIBLKBCLK))
    end

    -- calculate LTDC clock frequency ------------------------------------------
    if   uC.NAME:match("STM32F427") or uC.NAME:match("STM32F437")
      or uC.NAME:match("STM32F429") or uC.NAME:match("STM32F439")
      or uC.NAME:match("STM32F469") or uC.NAME:match("STM32F479") then

        freq.LTDCCLK = freq.PLLSAIR / LTDCDIV
        this:SetFlagValue("LABEL_LTDC_CLK_DIV", PrintFrequency(freq.LTDCCLK))
    end

    -- calculate output clocks -------------------------------------------------
    freq.HCLK     = freq.SYSCLK / AHBPRE:gsub("RCC_SYSCLK_Div", "")
    freq.PCLK1    = freq.HCLK / APB1PRE:gsub("RCC_HCLK_Div", "")
    freq.PCLK2    = freq.HCLK / APB2PRE:gsub("RCC_HCLK_Div", "")
    freq.TIMxCLK1 = iff(APB1PRE == "RCC_HCLK_Div1", freq.PCLK1, freq.PCLK1 * 2)
    freq.TIMxCLK2 = iff(APB2PRE == "RCC_HCLK_Div1", freq.PCLK1, freq.PCLK1 * 2)

    this:SetFlagValue("LABEL_AHB_PRE", PrintFrequency(freq.HCLK).." (HCLK)")
    this:SetFlagValue("LABEL_APB1_PRE", PrintFrequency(freq.PCLK1).." (PCLK1)")
    this:SetFlagValue("LABEL_APB2_PRE", PrintFrequency(freq.PCLK2).." (PCLK2)")

    -- calculate DFSDM1/2 clocks frequency -------------------------------------
    if   uC.NAME:match("STM32F412.G") or uC.NAME:match("STM32F413")
      or uC.NAME:match("STM32F423") then

        if DFSDM1CLKSRC == "RCC_DFSDMCLKSource_APB" then
            freq.DFSDM1CLK = freq.PCLK1
        elseif DFSDM1CLKSRC == "RCC_DFSDMCLKSource_SYS" then
            freq.DFSDM1CLK = freq.SYSCLK
        else
            freq.DFSDM1CLK = 0
        end
        this:SetFlagValue("LABEL_DFSDM1_CLK_SRC", PrintFrequency(freq.DFSDM1CLK))

        if DFSDM1ACLKSRC == "RCC_DFSDM1AUDIOCLKSOURCE_I2SAPB1" then
            freq.DFSDM1ACLK = freq.PCLK1
        elseif DFSDM1ACLKSRC == "RCC_DFSDM1AUDIOCLKSOURCE_I2SAPB2" then
            freq.DFSDM1ACLK = freq.PCLK2
        else
            freq.DFSDM1ACLK = 0
        end
        this:SetFlagValue("LABEL_DFSDM1_ACLK_SRC", PrintFrequency(freq.DFSDM1ACLK))
    end

    if uC.NAME:match("STM32F413") or uC.NAME:match("STM32F423") then
        if DFSDM2ACLKSRC == "RCC_DFSDM2AUDIOCLKSOURCE_I2SAPB1" then
            freq.DFSDM2ACLK = freq.PCLK1
        elseif DFSDM2ACLKSRC == "RCC_DFSDM2AUDIOCLKSOURCE_I2SAPB2" then
            freq.DFSDM2ACLK = freq.PCLK2
        else
            freq.DFSDM2ACLK = 0
        end
        this:SetFlagValue("LABEL_DFSDM2_ACLK_SRC", PrintFrequency(freq.DFSDM2ACLK))
    end

    -- calculate DSI clock -----------------------------------------------------
    if uC.NAME:match("STM32F469") or uC.NAME:match("STM32F479") then

        if DSISRC == "RCC_DSICLKSource_PHY" then
            freq.DSICLK = freq.DSIPHYCLK
        elseif DSISRC == "RCC_DSICLKSource_PLLR" then
            freq.DSICLK = freq.PLLR
        else
            freq.DSICLK = 0
        end

        this:SetFlagValue("LABEL_DSI_CLK_SRC", PrintFrequency(freq.DSICLK))
    end

    -- calculate 48MHz clock frequency -----------------------------------------
    if   uC.NAME:match("STM32F412.G") or uC.NAME:match("STM32F446")
      or uC.NAME:match("STM32F413")   or uC.NAME:match("STM32F423")
      or uC.NAME:match("STM32F469")   or uC.NAME:match("STM32F479") then

        if CLK48MHZSRC == "RCC_48MHZCLKSource_PLL" then
            freq.CLK48MHZ = freq.PLLQ
        elseif CLK48MHZSRC == "RCC_48MHZCLKSource_PLLSAI" then
            freq.CLK48MHZ = freq.PLLSAIP
        elseif CLK48MHZSRC == "RCC_CK48CLKSOURCE_PLLI2SQ" then
            freq.CLK48MHZ = freq.PLLI2SQ
        else
            freq.CLK48MHZ = 0
        end

        this:SetFlagValue("LABEL_48MHZ_CLK_SRC", PrintFrequency(freq.CLK48MHZ))
    end

    -- calculate SDIO clock frequency ------------------------------------------
    if   uC.NAME:match("STM32F412.G") or uC.NAME:match("STM32F446")
      or uC.NAME:match("STM32F413")   or uC.NAME:match("STM32F423")
      or uC.NAME:match("STM32F469")   or uC.NAME:match("STM32F479") then

        if SDIOCLKSRC == "RCC_SDIOCLKSource_48MHZ" then
            freq.SDIOCLK = freq.CLK48MHZ
        elseif SDIOCLKSRC == "RCC_SDIOCLKSource_SYSCLK" then
            freq.SDIOCLK = freq.SYSCLK
        else
            freq.SDIOCLK = 0
        end

        this:SetFlagValue("LABEL_SDIO_CLK_SRC", PrintFrequency(freq.SDIOCLK))
    end

    -- calculate SPDIF RX clock frequency --------------------------------------
    if uC.NAME:match("STM32F446") then

        if SPIDFSRC == "RCC_SPDIFRXCLKSource_PLLR" then
            freq.SPDIF = freq.PLLR
        elseif SPIDFSRC == "RCC_SPDIFRXCLKSource_PLLI2SP" then
            freq.SPDIF = freq.PLLI2SP
        else
            freq.SPDIF = 0
        end

        this:SetFlagValue("LABEL_SPDIF_SRC", PrintFrequency(freq.SPDIF))
    end

    -- calculate CEC clock frequency -------------------------------------------
    if uC.NAME:match("STM32F446") then

        if CECSRC == "RCC_CECCLKSource_HSIDiv488" then
            freq.CEC = freq.HSI / 488
        elseif CECSRC == "RCC_CECCLKSource_LSE" then
            freq.CEC = freq.LSE
        else
            freq.CEC =  0
        end

        this:SetFlagValue("LABEL_CEC_SRC", PrintFrequency(freq.CEC))
    end

    -- calculate FMPI2C clock frequency ----------------------------------------
    if   uC.NAME:match("STM32F412.G") or uC.NAME:match("STM32F446")
      or uC.NAME:match("STM32F413")   or uC.NAME:match("STM32F423")
      or uC.NAME:match("STM32F410") then

        if FMPI2C1SRC == "RCC_FMPI2C1CLKSource_APB1" then
            freq.FMPI2C1 = freq.PCLK1
        elseif FMPI2C1SRC == "RCC_FMPI2C1CLKSource_SYSCLK" then
            freq.FMPI2C1 = freq.SYSCLK
        elseif FMPI2C1SRC == "RCC_FMPI2C1CLKSource_HSI" then
            freq.FMPI2C1 = freq.HSI
        else
            freq.FMPI2C1 = 0
        end

        this:SetFlagValue("LABEL_FMPI2C1_SRC", PrintFrequency(freq.FMPI2C1))
    end

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

    freq.FLASHLATENCY = math.ceil(freq.HCLK / FLV) - 1
    if freq.FLASHLATENCY < 0 then freq.FLASHLATENCY = 0 end
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
