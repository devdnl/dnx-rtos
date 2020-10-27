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
this:AddExtraWidget("Label", "LABEL_VoidOsc1", "\t\t\t\t\t\t\t", -1, "bold")
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
// PLL clock source
//==============================================================================
/*--
this:AddExtraWidget("Label", "LABEL_PLL_SRC", "\nPLL Clock source", -1, "bold")
this:AddExtraWidget("Void", "VoidPLLSRC1")
this:AddExtraWidget("Void", "VoidPLLSRC2")
++*/
/*--
this:AddWidget("Combobox", "PLL Clock source (ref_ck)")
this:AddItem("HSI", "LL_RCC_PLLSOURCE_HSI")
this:AddItem("CSI", "LL_RCC_PLLSOURCE_CSI")
this:AddItem("HSE", "LL_RCC_PLLSOURCE_HSE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_PLL_SRC", "")
--*/
#define __CLK_PLL_SRC__ LL_RCC_PLLSOURCE_HSI


//==============================================================================
// PLL1 clock configuration
//==============================================================================
/*--
this:AddExtraWidget("Label", "LABEL_PLL1_CLOCKS", "\nPLL1 Clocks", -1, "bold")
this:AddExtraWidget("Void", "VoidPLL1CLK1")
this:AddExtraWidget("Void", "VoidPLL1CLK2")
++*/
/*--
this:AddWidget("Combobox", "PLL Enable")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL1_ON", "")
--*/
#define __CLK_PLL1_ON__ _YES_

/*--
this:AddWidget("Spinbox", 0, 63, "PLL ref_ck divider (M)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL1_M", "")
--*/
#define __CLK_PLL1_M__ 8

/*--
this:AddWidget("Spinbox", 4, 512, "PLL multiplier (N)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL1_N", "")
--*/
#define __CLK_PLL1_N__ 60

/*--
this:AddWidget("Combobox", "PLL P clock enable")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL1_P_ON", "")
--*/
#define __CLK_PLL1_P_ENABLE__ _YES_

/*--
this:AddWidget("Spinbox", 1, 128, "PLL divider P (pll1_p_ck)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL1_P", "")
--*/
#define __CLK_PLL1_P__ 1

/*--
this:AddWidget("Combobox", "PLL Q clock enable")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL1_Q_ON", "")
--*/
#define __CLK_PLL1_Q_ENABLE__ _YES_

/*--
this:AddWidget("Spinbox", 1, 128, "PLL divider Q (pll1_q_ck)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL1_Q", "")
--*/
#define __CLK_PLL1_Q__ 1

/*--
this:AddWidget("Combobox", "PLL R clock enable")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL1_R_ON", "")
--*/
#define __CLK_PLL1_R_ENABLE__ _NO_

/*--
this:AddWidget("Spinbox", 1, 128, "PLL divider R (pll1_r_ck)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL1_R", "")
--*/
#define __CLK_PLL1_R__ 1


//==============================================================================
// PLL2 clock configuration
//==============================================================================
/*--
this:AddExtraWidget("Label", "LABEL_PLL2_CLOCKS", "\nPLL2 Clocks", -1, "bold")
this:AddExtraWidget("Void", "VoidPLL2CLK1")
this:AddExtraWidget("Void", "VoidPLL2CLK2")
++*/
/*--
this:AddWidget("Combobox", "PLL Enable")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL2_ON", "")
--*/
#define __CLK_PLL2_ON__ _YES_

/*--
this:AddWidget("Spinbox", 0, 63, "PLL ref_ck divider (M)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL2_M", "")
--*/
#define __CLK_PLL2_M__ 8

/*--
this:AddWidget("Spinbox", 4, 512, "PLL multiplier (N)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL2_N", "")
--*/
#define __CLK_PLL2_N__ 60

/*--
this:AddWidget("Combobox", "PLL P clock enable")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL2_P_ON", "")
--*/
#define __CLK_PLL2_P_ENABLE__ _YES_

/*--
this:AddWidget("Spinbox", 1, 128, "PLL divider P (pll2_p_ck)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL2_P", "")
--*/
#define __CLK_PLL2_P__ 1

/*--
this:AddWidget("Combobox", "PLL Q clock enable")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL2_Q_ON", "")
--*/
#define __CLK_PLL2_Q_ENABLE__ _YES_

/*--
this:AddWidget("Spinbox", 1, 128, "PLL divider Q (pll2_q_ck)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL2_Q", "")
--*/
#define __CLK_PLL2_Q__ 1

/*--
this:AddWidget("Combobox", "PLL R clock enable")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL2_R_ON", "")
--*/
#define __CLK_PLL2_R_ENABLE__ _NO_

/*--
this:AddWidget("Spinbox", 1, 128, "PLL divider R (pll2_r_ck)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL2_R", "")
--*/
#define __CLK_PLL2_R__ 1


//==============================================================================
// PLL3 clock configuration
//==============================================================================
/*--
this:AddExtraWidget("Label", "LABEL_PLL3_CLOCKS", "\nPLL3 Clocks", -1, "bold")
this:AddExtraWidget("Void", "VoidPLL3CLK1")
this:AddExtraWidget("Void", "VoidPLL3CLK2")
++*/
/*--
this:AddWidget("Combobox", "PLL Enable")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL3_ON", "")
--*/
#define __CLK_PLL3_ON__ _YES_

/*--
this:AddWidget("Spinbox", 0, 63, "PLL ref_ck divider (M)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL3_M", "")
--*/
#define __CLK_PLL3_M__ 8

/*--
this:AddWidget("Spinbox", 4, 512, "PLL multiplier (N)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL3_N", "")
--*/
#define __CLK_PLL3_N__ 60

/*--
this:AddWidget("Combobox", "PLL P clock enable")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL3_P_ON", "")
--*/
#define __CLK_PLL3_P_ENABLE__ _YES_

/*--
this:AddWidget("Spinbox", 1, 128, "PLL divider P (pll3_p_ck)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL3_P", "")
--*/
#define __CLK_PLL3_P__ 1

/*--
this:AddWidget("Combobox", "PLL Q clock enable")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL3_Q_ON", "")
--*/
#define __CLK_PLL3_Q_ENABLE__ _YES_

/*--
this:AddWidget("Spinbox", 1, 128, "PLL divider Q (pll3_q_ck)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL3_Q", "")
--*/
#define __CLK_PLL3_Q__ 1

/*--
this:AddWidget("Combobox", "PLL R clock enable")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL3_R_ON", "")
--*/
#define __CLK_PLL3_R_ENABLE__ _NO_

/*--
this:AddWidget("Spinbox", 1, 128, "PLL divider R (pll3_r_ck)")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_PLL3_R", "")
--*/
#define __CLK_PLL3_R__ 1


//==============================================================================
// Buses clock configuration
//==============================================================================
/*--
this:AddExtraWidget("Label", "LABEL_BUS_CLOCKS", "\nBus Clocks", -1, "bold")
this:AddExtraWidget("Void", "VoidBusCLK1")
this:AddExtraWidget("Void", "VoidBusCLK2")
++*/
/*--
this:AddWidget("Combobox", "SYSCLK source (sys_ck)")
this:AddItem("HSI (hsi_ck)", "LL_RCC_SYS_CLKSOURCE_HSI")
this:AddItem("CSI (csi_ck)", "LL_RCC_SYS_CLKSOURCE_CSI")
this:AddItem("HSE (hse_ck)", "LL_RCC_SYS_CLKSOURCE_HSE")
this:AddItem("PLL1 (pll1_p_ck)", "LL_RCC_SYS_CLKSOURCE_PLL1")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_SYSCLK_SRC", "")
--*/
#define __CLK_SYSCLK_SRC__ LL_RCC_SYS_CLKSOURCE_PLL1

/*--
this:AddWidget("Combobox", "SYSCLK prescaler")
this:AddItem("sys_ck / 1", "LL_RCC_SYSCLK_DIV_1")
this:AddItem("sys_ck / 2", "LL_RCC_SYSCLK_DIV_2")
this:AddItem("sys_ck / 4", "LL_RCC_SYSCLK_DIV_4")
this:AddItem("sys_ck / 8", "LL_RCC_SYSCLK_DIV_8")
this:AddItem("sys_ck / 16", "LL_RCC_SYSCLK_DIV_16")
this:AddItem("sys_ck / 64", "LL_RCC_SYSCLK_DIV_64")
this:AddItem("sys_ck / 128", "LL_RCC_SYSCLK_DIV_128")
this:AddItem("sys_ck / 256", "LL_RCC_SYSCLK_DIV_256")
this:AddItem("sys_ck / 512", "LL_RCC_SYSCLK_DIV_512")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_SYSCLK_DIV", "")
--*/
#define __CLK_SYSCLK_DIV__ LL_RCC_SYSCLK_DIV_1

/*--
this:AddWidget("Combobox", "AHB prescaler")
this:AddItem("sys_ck / 1", "LL_RCC_AHB_DIV_1")
this:AddItem("sys_ck / 2", "LL_RCC_AHB_DIV_2")
this:AddItem("sys_ck / 4", "LL_RCC_AHB_DIV_4")
this:AddItem("sys_ck / 8", "LL_RCC_AHB_DIV_8")
this:AddItem("sys_ck / 16", "LL_RCC_AHB_DIV_16")
this:AddItem("sys_ck / 64", "LL_RCC_AHB_DIV_64")
this:AddItem("sys_ck / 128", "LL_RCC_AHB_DIV_128")
this:AddItem("sys_ck / 256", "LL_RCC_AHB_DIV_256")
this:AddItem("sys_ck / 512", "LL_RCC_AHB_DIV_512")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_AHBCLK_DIV", "")
--*/
#define __CLK_AHB_DIV__ LL_RCC_AHB_DIV_2

/*--
this:AddWidget("Combobox", "APB1 prescaler")
this:AddItem("ahb_ck / 1", "LL_RCC_APB1_DIV_1")
this:AddItem("ahb_ck / 2", "LL_RCC_APB1_DIV_2")
this:AddItem("ahb_ck / 4", "LL_RCC_APB1_DIV_4")
this:AddItem("ahb_ck / 8", "LL_RCC_APB1_DIV_8")
this:AddItem("ahb_ck / 16", "LL_RCC_APB1_DIV_16")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_APB1CLK_DIV", "")
--*/
#define __CLK_APB1_DIV__ LL_RCC_APB1_DIV_8

/*--
this:AddWidget("Combobox", "APB2 prescaler")
this:AddItem("ahb_ck / 1", "LL_RCC_APB2_DIV_1")
this:AddItem("ahb_ck / 2", "LL_RCC_APB2_DIV_2")
this:AddItem("ahb_ck / 4", "LL_RCC_APB2_DIV_4")
this:AddItem("ahb_ck / 8", "LL_RCC_APB2_DIV_8")
this:AddItem("ahb_ck / 16", "LL_RCC_APB2_DIV_16")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_APB2CLK_DIV", "")
--*/
#define __CLK_APB2_DIV__ LL_RCC_APB2_DIV_8

/*--
this:AddWidget("Combobox", "APB3 prescaler")
this:AddItem("ahb_ck / 1", "LL_RCC_APB3_DIV_1")
this:AddItem("ahb_ck / 2", "LL_RCC_APB3_DIV_2")
this:AddItem("ahb_ck / 4", "LL_RCC_APB3_DIV_4")
this:AddItem("ahb_ck / 8", "LL_RCC_APB3_DIV_8")
this:AddItem("ahb_ck / 16", "LL_RCC_APB3_DIV_16")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_APB3CLK_DIV", "")
--*/
#define __CLK_APB3_DIV__ LL_RCC_APB3_DIV_8

/*--
this:AddWidget("Combobox", "APB4 prescaler")
this:AddItem("ahb_ck / 1", "LL_RCC_APB4_DIV_1")
this:AddItem("ahb_ck / 2", "LL_RCC_APB4_DIV_2")
this:AddItem("ahb_ck / 4", "LL_RCC_APB4_DIV_4")
this:AddItem("ahb_ck / 8", "LL_RCC_APB4_DIV_8")
this:AddItem("ahb_ck / 16", "LL_RCC_APB4_DIV_16")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_APB4CLK_DIV", "")
--*/
#define __CLK_APB4_DIV__ LL_RCC_APB4_DIV_8


//==============================================================================
// Calculated values
//==============================================================================
// Value is calculated according to SYSCLK source selection
#define __CLK_SYSCLK_SRC_STATUS__ LL_RCC_SYS_CLKSOURCE_STATUS_PLL1

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

    -- HSI & CSI clock frequency -----------------------------------------------
    freq.HSI = 64e6
    freq.CSI = 4e6

    -- LSE clock frequency -----------------------------------------------------
    local LSEON = iff(this:GetFlagValue("__CLK_LSE_ON__") == "DISABLE", 0, 1)
    freq.LSE = 32768 * LSEON
    this:SetFlagValue("LABEL_LSE_ON", PrintFrequency(freq.LSE).." (LSE)")

    -- LSI clock frequency -----------------------------------------------------
    local LSION = iff(this:GetFlagValue("__CLK_LSI_ON__") == "DISABLE", 0, 1)
    freq.LSI = 32e3 * LSION
    this:SetFlagValue("LABEL_LSI_ON", PrintFrequency(freq.LSI).." (LSI)")

    -- PLLs clock source -------------------------------------------------------
    local PLLSRC = this:GetFlagValue("__CLK_PLL_SRC__")
    if PLLSRC == "LL_RCC_PLLSOURCE_HSI" then
        freq.ref_ck = freq.HSI
    elseif PLLSRC == "LL_RCC_PLLSOURCE_HSE" then
        freq.ref_ck = freq.HSE
    elseif PLLSRC == "LL_RCC_PLLSOURCE_CSI" then
        freq.ref_ck = freq.CSI
    else
        freq.ref_ck = 0
    end
    this:SetFlagValue("LABEL_CLK_PLL_SRC", PrintFrequency(freq.ref_ck))

    -- PLL1 clock frequency ----------------------------------------------------
    local PLL1ON = iff(this:GetFlagValue("__CLK_PLL1_ON__") == "_NO_", 0, 1)
    local DIVM1  = this:GetFlagValue("__CLK_PLL1_M__")
    local MULN1  = this:GetFlagValue("__CLK_PLL1_N__")
    local DIVP1  = this:GetFlagValue("__CLK_PLL1_P__")
    local DIVQ1  = this:GetFlagValue("__CLK_PLL1_Q__")
    local DIVR1  = this:GetFlagValue("__CLK_PLL1_R__")
    local PLLPON = iff(this:GetFlagValue("__CLK_PLL1_P_ENABLE__") == "_NO_", 0, 1)
    local PLLQON = iff(this:GetFlagValue("__CLK_PLL1_Q_ENABLE__") == "_NO_", 0, 1)
    local PLLRON = iff(this:GetFlagValue("__CLK_PLL1_R_ENABLE__") == "_NO_", 0, 1)

    if tonumber(DIVP1) >= 2 then
        local p = tostring(DIVP1)
        if p % 2 == 1 then
            p = p + 1
        end
        this:SetFlagValue("__CLK_PLL1_P__", tostring(p))
        DIVP1 = tostring(p)
    end

    freq.pll1_ck = freq.ref_ck / tonumber(DIVM1) * tonumber(MULN1) * PLL1ON

    freq.pll1_p_ck = freq.pll1_ck / tonumber(DIVP1) * PLLPON
    this:SetFlagValue("LABEL_PLL1_P", PrintFrequency(freq.pll1_p_ck))

    freq.pll1_q_ck = freq.pll1_ck / tonumber(DIVQ1) * PLLQON
    this:SetFlagValue("LABEL_PLL1_Q", PrintFrequency(freq.pll1_q_ck))

    freq.pll1_r_ck = freq.pll1_ck / tonumber(DIVR1) * PLLRON
    this:SetFlagValue("LABEL_PLL1_R", PrintFrequency(freq.pll1_r_ck))


    -- PLL2 clock frequency ----------------------------------------------------
    local PLL2ON = iff(this:GetFlagValue("__CLK_PLL2_ON__") == "_NO_", 0, 1)
    local DIVM1  = this:GetFlagValue("__CLK_PLL2_M__")
    local MULN1  = this:GetFlagValue("__CLK_PLL2_N__")
    local DIVP1  = this:GetFlagValue("__CLK_PLL2_P__")
    local DIVQ1  = this:GetFlagValue("__CLK_PLL2_Q__")
    local DIVR1  = this:GetFlagValue("__CLK_PLL2_R__")
    local PLLPON = iff(this:GetFlagValue("__CLK_PLL2_P_ENABLE__") == "_NO_", 0, 1)
    local PLLQON = iff(this:GetFlagValue("__CLK_PLL2_Q_ENABLE__") == "_NO_", 0, 1)
    local PLLRON = iff(this:GetFlagValue("__CLK_PLL2_R_ENABLE__") == "_NO_", 0, 1)

    freq.pll2_ck = freq.ref_ck / tonumber(DIVM1) * tonumber(MULN1) * PLL2ON

    freq.pll2_p_ck = freq.pll2_ck / tonumber(DIVP1) * PLLPON
    this:SetFlagValue("LABEL_PLL2_P", PrintFrequency(freq.pll2_p_ck))

    freq.pll2_q_ck = freq.pll2_ck / tonumber(DIVQ1) * PLLQON
    this:SetFlagValue("LABEL_PLL2_Q", PrintFrequency(freq.pll2_q_ck))

    freq.pll2_r_ck = freq.pll2_ck / tonumber(DIVR1) * PLLRON
    this:SetFlagValue("LABEL_PLL2_R", PrintFrequency(freq.pll2_r_ck))


    -- PLL3 clock frequency ----------------------------------------------------
    local PLL3ON = iff(this:GetFlagValue("__CLK_PLL3_ON__") == "_NO_", 0, 1)
    local DIVM1  = this:GetFlagValue("__CLK_PLL3_M__")
    local MULN1  = this:GetFlagValue("__CLK_PLL3_N__")
    local DIVP1  = this:GetFlagValue("__CLK_PLL3_P__")
    local DIVQ1  = this:GetFlagValue("__CLK_PLL3_Q__")
    local DIVR1  = this:GetFlagValue("__CLK_PLL3_R__")
    local PLLPON = iff(this:GetFlagValue("__CLK_PLL3_P_ENABLE__") == "_NO_", 0, 1)
    local PLLQON = iff(this:GetFlagValue("__CLK_PLL3_Q_ENABLE__") == "_NO_", 0, 1)
    local PLLRON = iff(this:GetFlagValue("__CLK_PLL3_R_ENABLE__") == "_NO_", 0, 1)

    freq.pll3_ck = freq.ref_ck / tonumber(DIVM1) * tonumber(MULN1) * PLL3ON

    freq.pll3_p_ck = freq.pll3_ck / tonumber(DIVP1) * PLLPON
    this:SetFlagValue("LABEL_PLL3_P", PrintFrequency(freq.pll3_p_ck))

    freq.pll3_q_ck = freq.pll3_ck / tonumber(DIVQ1) * PLLQON
    this:SetFlagValue("LABEL_PLL3_Q", PrintFrequency(freq.pll3_q_ck))

    freq.pll3_r_ck = freq.pll3_ck / tonumber(DIVR1) * PLLRON
    this:SetFlagValue("LABEL_PLL3_R", PrintFrequency(freq.pll3_r_ck))


    -- SYSCLK source
    local SYSCLKSRC = this:GetFlagValue("__CLK_SYSCLK_SRC__"):gsub("LL_RCC_SYS_CLKSOURCE_", "")

    if SYSCLKSRC == "HSI" then
        freq.sys_ck = freq.HSI
    elseif SYSCLKSRC == "HSE" then
        freq.sys_ck = freq.HSE
    elseif SYSCLKSRC == "CSI" then
        freq.sys_ck = freq.CSI
    elseif SYSCLKSRC == "PLL1" then
        freq.sys_ck = freq.pll1_p_ck
    else
        freq.sys_ck = 0
    end
    this:SetFlagValue("LABEL_SYSCLK_SRC", PrintFrequency(freq.sys_ck))

    -- SYSCLK frequency
    local SYSCLK_DIV = this:GetFlagValue("__CLK_SYSCLK_DIV__"):gsub("LL_RCC_SYSCLK_DIV_", "")
    freq.sysclk = freq.sys_ck / tonumber(SYSCLK_DIV)
    this:SetFlagValue("LABEL_SYSCLK_DIV", PrintFrequency(freq.sysclk))

    -- AHB clk frequency
    local AHBCLK_DIV = this:GetFlagValue("__CLK_AHB_DIV__"):gsub("LL_RCC_AHB_DIV_", "")
    freq.ahbclk = freq.sys_ck / tonumber(AHBCLK_DIV)
    this:SetFlagValue("LABEL_AHBCLK_DIV", PrintFrequency(freq.ahbclk))

    -- APB1 clk frequency
    local APB1CLK_DIV = this:GetFlagValue("__CLK_APB1_DIV__"):gsub("LL_RCC_APB1_DIV_", "")
    freq.apb1clk = freq.ahbclk / tonumber(APB1CLK_DIV)
    this:SetFlagValue("LABEL_APB1CLK_DIV", PrintFrequency(freq.apb1clk))

    -- APB2 clk frequency
    local APB2CLK_DIV = this:GetFlagValue("__CLK_APB2_DIV__"):gsub("LL_RCC_APB2_DIV_", "")
    freq.apb2clk = freq.ahbclk / tonumber(APB2CLK_DIV)
    this:SetFlagValue("LABEL_APB2CLK_DIV", PrintFrequency(freq.apb2clk))

    -- APB3 clk frequency
    local APB3CLK_DIV = this:GetFlagValue("__CLK_APB3_DIV__"):gsub("LL_RCC_APB3_DIV_", "")
    freq.apb3clk = freq.ahbclk / tonumber(APB3CLK_DIV)
    this:SetFlagValue("LABEL_APB3CLK_DIV", PrintFrequency(freq.apb3clk))

    -- APB4 clk frequency
    local APB4CLK_DIV = this:GetFlagValue("__CLK_APB4_DIV__"):gsub("LL_RCC_APB4_DIV_", "")
    freq.apb4clk = freq.ahbclk / tonumber(APB4CLK_DIV)
    this:SetFlagValue("LABEL_APB4CLK_DIV", PrintFrequency(freq.apb4clk))


    -- end calculations
    this:SetFlagValue("__CLK_SYSCLK_SRC_STATUS__", "LL_RCC_SYS_CLKSOURCE_STATUS_"..SYSCLKSRC)
end

-- trigger frequency calculation after loading all flags
this.CalculateFreq()
++*/

#endif /* _CLK_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
