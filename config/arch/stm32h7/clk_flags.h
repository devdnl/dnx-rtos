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
this:AddExtraWidget("Label", "LABEL_SUPPLY_SOURCE", "Supply source", -1, "bold")
this:AddExtraWidget("Label", "LABEL_VoidSupplySource1", "\t\t\t\t\t\t\t", -1, "bold")
this:AddExtraWidget("Void", "VoidSupplySource2")
++*/
/*--
this:AddWidget("Combobox", "Supply source")
this:AddItem("LDO supply", "PWR_LDO_SUPPLY")
this:AddItem("External supply source", "PWR_EXTERNAL_SOURCE_SUPPLY")
this:AddItem("Direct SMPS supply", "PWR_DIRECT_SMPS_SUPPLY")
this:AddItem("SMPS supplies LDO 1.8V", "PWR_SMPS_1V8_SUPPLIES_LDO")
this:AddItem("SMPS supplies LDO 2.5V", "PWR_SMPS_2V5_SUPPLIES_LDO")
this:AddItem("SMPS supplies EXT and LDO 1.8V", "PWR_SMPS_1V8_SUPPLIES_EXT_AND_LDO")
this:AddItem("SMPS supplies EXT and LDO 2.5V", "PWR_SMPS_2V5_SUPPLIES_EXT_AND_LDO")
this:AddItem("SMPS supplies EXT 1.8V", "PWR_SMPS_1V8_SUPPLIES_EXT")
this:AddItem("SMPS supplies EXT 2.5V", "PWR_SMPS_2V5_SUPPLIES_EXT")
this:AddExtraWidget("Label", "LABEL_SUPPLY_SOURCE_EXTRA", "")
--*/
#define __CLK_SUPPLY_SOURCE__ PWR_LDO_SUPPLY

/*--
this:AddExtraWidget("Label", "LABEL_OSCILLATORS", "\nOscillators", -1, "bold")
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
this:AddWidget("Combobox", "PLL divider P (pll1_p_ck)")
this:AddItem("1", "1")
for i = 2, 128, 2 do this:AddItem(tostring(i), tostring(i)) end
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
#define __CLK_APB1_DIV__ LL_RCC_APB1_DIV_2

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
#define __CLK_APB2_DIV__ LL_RCC_APB2_DIV_2

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
#define __CLK_APB3_DIV__ LL_RCC_APB3_DIV_2

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
#define __CLK_APB4_DIV__ LL_RCC_APB4_DIV_4

//==============================================================================
// Peripheral clock configuration
//==============================================================================
/*--
this:AddExtraWidget("Label", "LABEL_PERIPH_CLOCKS", "\nPeripheral Clocks", -1, "bold")
this:AddExtraWidget("Void", "VoidPeriphCLK1")
this:AddExtraWidget("Void", "VoidPeriphCLK2")
++*/

/*--
this:AddWidget("Combobox", "USART1..6 clock source")
this:AddItem("PCLK2", "LL_RCC_USART16_CLKSOURCE_PCLK2")
this:AddItem("PLL2Q", "LL_RCC_USART16_CLKSOURCE_PLL2Q")
this:AddItem("PLL3Q", "LL_RCC_USART16_CLKSOURCE_PLL3Q")
this:AddItem("HSI", "LL_RCC_USART16_CLKSOURCE_HSI")
this:AddItem("CSI", "LL_RCC_USART16_CLKSOURCE_CSI")
this:AddItem("LSE", "LL_RCC_USART16_CLKSOURCE_LSE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_UART16", "")
--*/
#define __CLK_SRC_UART16__ LL_RCC_USART16_CLKSOURCE_PCLK2

/*--
this:AddWidget("Combobox", "USART2..5,7..8 clock source")
this:AddItem("PCLK1", "LL_RCC_USART234578_CLKSOURCE_PCLK1")
this:AddItem("PLL2Q", "LL_RCC_USART234578_CLKSOURCE_PLL2Q")
this:AddItem("PLL3Q", "LL_RCC_USART234578_CLKSOURCE_PLL3Q")
this:AddItem("HSI", "LL_RCC_USART234578_CLKSOURCE_HSI")
this:AddItem("CSI", "LL_RCC_USART234578_CLKSOURCE_CSI")
this:AddItem("LSE", "LL_RCC_USART234578_CLKSOURCE_LSE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_UART234578", "")
--*/
#define __CLK_SRC_UART234578__ LL_RCC_USART234578_CLKSOURCE_PCLK1

/*--
this:AddWidget("Combobox", "LPUART1 clock source")
this:AddItem("PCLK4", "LL_RCC_LPUART1_CLKSOURCE_PCLK4")
this:AddItem("PLL2Q", "LL_RCC_LPUART1_CLKSOURCE_PLL2Q")
this:AddItem("PLL3Q", "LL_RCC_LPUART1_CLKSOURCE_PLL3Q")
this:AddItem("HSI", "LL_RCC_LPUART1_CLKSOURCE_HSI")
this:AddItem("CSI", "LL_RCC_LPUART1_CLKSOURCE_CSI")
this:AddItem("LSE", "LL_RCC_LPUART1_CLKSOURCE_LSE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_LPUART1", "")
--*/
#define __CLK_SRC_LPUART1__ LL_RCC_LPUART1_CLKSOURCE_PCLK4

/*--
this:AddWidget("Combobox", "I2C1..3 clock source")
this:AddItem("PCLK1", "LL_RCC_I2C123_CLKSOURCE_PCLK1")
this:AddItem("PLL3R", "LL_RCC_I2C123_CLKSOURCE_PLL3R")
this:AddItem("HSI", "LL_RCC_I2C123_CLKSOURCE_HSI")
this:AddItem("CSI", "LL_RCC_I2C123_CLKSOURCE_CSI")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_I2C123", "")
--*/
#define __CLK_SRC_I2C123__ LL_RCC_I2C123_CLKSOURCE_PCLK1

/*--
this:AddWidget("Combobox", "I2C4 clock source")
this:AddItem("PCLK4", "LL_RCC_I2C4_CLKSOURCE_PCLK4")
this:AddItem("PLL3R", "LL_RCC_I2C4_CLKSOURCE_PLL3R")
this:AddItem("HSI", "LL_RCC_I2C4_CLKSOURCE_HSI")
this:AddItem("CSI", "LL_RCC_I2C4_CLKSOURCE_CSI")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_I2C4", "")
--*/
#define __CLK_SRC_I2C4__ LL_RCC_I2C4_CLKSOURCE_PCLK4

/*--
this:AddWidget("Combobox", "LPTIM1 clock source")
this:AddItem("PCLK1", "LL_RCC_LPTIM1_CLKSOURCE_PCLK1")
this:AddItem("PLL2P", "LL_RCC_LPTIM1_CLKSOURCE_PLL2P")
this:AddItem("PLL3R", "LL_RCC_LPTIM1_CLKSOURCE_PLL3R")
this:AddItem("LSE", "LL_RCC_LPTIM1_CLKSOURCE_LSE")
this:AddItem("LSI", "LL_RCC_LPTIM1_CLKSOURCE_LSI")
this:AddItem("CLKP", "LL_RCC_LPTIM1_CLKSOURCE_CLKP")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_LPTIM1", "")
--*/
#define __CLK_SRC_LPTIM1__ LL_RCC_LPTIM1_CLKSOURCE_LSE

/*--
this:AddWidget("Combobox", "LPTIM2 clock source")
this:AddItem("PCLK4", "LL_RCC_LPTIM2_CLKSOURCE_PCLK4")
this:AddItem("PLL2P", "LL_RCC_LPTIM2_CLKSOURCE_PLL2P")
this:AddItem("PLL3R", "LL_RCC_LPTIM2_CLKSOURCE_PLL3R")
this:AddItem("LSE", "LL_RCC_LPTIM2_CLKSOURCE_LSE")
this:AddItem("LSI", "LL_RCC_LPTIM2_CLKSOURCE_LSI")
this:AddItem("CLKP", "LL_RCC_LPTIM2_CLKSOURCE_CLKP")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_LPTIM2", "")
--*/
#define __CLK_SRC_LPTIM2__ LL_RCC_LPTIM2_CLKSOURCE_LSE

/*--
this:AddWidget("Combobox", "LPTIM345 clock source")
this:AddItem("PCLK4", "LL_RCC_LPTIM345_CLKSOURCE_PCLK4")
this:AddItem("PLL2P", "LL_RCC_LPTIM345_CLKSOURCE_PLL2P")
this:AddItem("PLL3R", "LL_RCC_LPTIM345_CLKSOURCE_PLL3R")
this:AddItem("LSE", "LL_RCC_LPTIM345_CLKSOURCE_LSE")
this:AddItem("LSI", "LL_RCC_LPTIM345_CLKSOURCE_LSI")
this:AddItem("CLKP", "LL_RCC_LPTIM345_CLKSOURCE_CLKP")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_LPTIM345", "")
--*/
#define __CLK_SRC_LPTIM345__ LL_RCC_LPTIM345_CLKSOURCE_LSE

/*--
this:AddWidget("Combobox", "CLKP clock source")
this:AddItem("HSI", "LL_RCC_CLKP_CLKSOURCE_HSI")
this:AddItem("CSI", "LL_RCC_CLKP_CLKSOURCE_CSI")
this:AddItem("HSE", "LL_RCC_CLKP_CLKSOURCE_HSE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_CLKP", "")
--*/
#define __CLK_SRC_CLKP__ LL_RCC_CLKP_CLKSOURCE_HSI

/*--
this:AddWidget("Combobox", "SPDIF clock source")
this:AddItem("PLL1Q", "LL_RCC_SPDIF_CLKSOURCE_PLL1Q")
this:AddItem("PLL2R", "LL_RCC_SPDIF_CLKSOURCE_PLL2R")
this:AddItem("PLL3R", "LL_RCC_SPDIF_CLKSOURCE_PLL3R")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_SPDIF", "")
--*/
#define __CLK_SRC_SPDIF__ LL_RCC_SPDIF_CLKSOURCE_PLL1Q

/*--
this:AddWidget("Combobox", "SAI1 clock source")
this:AddItem("PLL1Q", "LL_RCC_SAI1_CLKSOURCE_PLL1Q")
this:AddItem("PLL2P", "LL_RCC_SAI1_CLKSOURCE_PLL2P")
this:AddItem("PLL3P", "LL_RCC_SAI1_CLKSOURCE_PLL3P")
this:AddItem("I2S_CKIN", "LL_RCC_SAI1_CLKSOURCE_I2S_CKIN")
this:AddItem("CLKP", "LL_RCC_SAI1_CLKSOURCE_CLKP")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_SAI1", "")
--*/
#define __CLK_SRC_SAI1__ LL_RCC_SAI1_CLKSOURCE_PLL1Q

/*--
this:AddWidget("Combobox", "SAI23 clock source")
this:AddItem("PLL1Q", "LL_RCC_SAI23_CLKSOURCE_PLL1Q")
this:AddItem("PLL2P", "LL_RCC_SAI23_CLKSOURCE_PLL2P")
this:AddItem("PLL3P", "LL_RCC_SAI23_CLKSOURCE_PLL3P")
this:AddItem("I2S_CKIN", "LL_RCC_SAI23_CLKSOURCE_I2S_CKIN")
this:AddItem("CLKP", "LL_RCC_SAI23_CLKSOURCE_CLKP")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_SAI23", "")
--*/
#define __CLK_SRC_SAI23__ LL_RCC_SAI23_CLKSOURCE_PLL1Q

/*--
this:AddWidget("Combobox", "SAI2A clock source")
this:AddItem("PLL1Q", "LL_RCC_SAI2A_CLKSOURCE_PLL1Q")
this:AddItem("PLL2P", "LL_RCC_SAI2A_CLKSOURCE_PLL2P")
this:AddItem("PLL3P", "LL_RCC_SAI2A_CLKSOURCE_PLL3P")
this:AddItem("I2S_CKIN", "LL_RCC_SAI2A_CLKSOURCE_I2S_CKIN")
this:AddItem("CLKP", "LL_RCC_SAI2A_CLKSOURCE_CLKP")
this:AddItem("SPDIF", "LL_RCC_SAI2A_CLKSOURCE_SPDIF")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_SAI2A", "")
--*/
#define __CLK_SRC_SAI2A__ LL_RCC_SAI2A_CLKSOURCE_PLL1Q

/*--
this:AddWidget("Combobox", "SAI2B clock source")
this:AddItem("PLL1Q", "LL_RCC_SAI2B_CLKSOURCE_PLL1Q")
this:AddItem("PLL2P", "LL_RCC_SAI2B_CLKSOURCE_PLL2P")
this:AddItem("PLL3P", "LL_RCC_SAI2B_CLKSOURCE_PLL3P")
this:AddItem("I2S_CKIN", "LL_RCC_SAI2B_CLKSOURCE_I2S_CKIN")
this:AddItem("CLKP", "LL_RCC_SAI2B_CLKSOURCE_CLKP")
this:AddItem("SPDIF", "LL_RCC_SAI2B_CLKSOURCE_SPDIF")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_SAI2B", "")
--*/
#define __CLK_SRC_SAI2B__ LL_RCC_SAI2B_CLKSOURCE_PLL1Q

/*--
this:AddWidget("Combobox", "SAI4A clock source")
this:AddItem("PLL1Q", "LL_RCC_SAI4A_CLKSOURCE_PLL1Q")
this:AddItem("PLL2P", "LL_RCC_SAI4A_CLKSOURCE_PLL2P")
this:AddItem("PLL3P", "LL_RCC_SAI4A_CLKSOURCE_PLL3P")
this:AddItem("I2S_CKIN", "LL_RCC_SAI4A_CLKSOURCE_I2S_CKIN")
this:AddItem("CLKP", "LL_RCC_SAI4A_CLKSOURCE_CLKP")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_SAI4A", "")
--*/
#define __CLK_SRC_SAI4A__ LL_RCC_SAI4A_CLKSOURCE_PLL1Q

/*--
this:AddWidget("Combobox", "SAI4B clock source")
this:AddItem("PLL1Q", "LL_RCC_SAI4B_CLKSOURCE_PLL1Q")
this:AddItem("PLL2P", "LL_RCC_SAI4B_CLKSOURCE_PLL2P")
this:AddItem("PLL3P", "LL_RCC_SAI4B_CLKSOURCE_PLL3P")
this:AddItem("I2S_CKIN", "LL_RCC_SAI4B_CLKSOURCE_I2S_CKIN")
this:AddItem("CLKP", "LL_RCC_SAI4B_CLKSOURCE_CLKP")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_SAI4B", "")
--*/
#define __CLK_SRC_SAI4B__ LL_RCC_SAI4B_CLKSOURCE_PLL1Q

/*--
this:AddWidget("Combobox", "SDMMC clock source")
this:AddItem("PLL1Q", "LL_RCC_SDMMC_CLKSOURCE_PLL1Q")
this:AddItem("PLL2R", "LL_RCC_SDMMC_CLKSOURCE_PLL2R")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_SDMMC", "")
--*/
#define __CLK_SRC_SDMMC__ LL_RCC_SDMMC_CLKSOURCE_PLL1Q

/*--
this:AddWidget("Combobox", "RNG clock source")
this:AddItem("HSI48", "LL_RCC_RNG_CLKSOURCE_HSI48")
this:AddItem("PLL1Q", "LL_RCC_RNG_CLKSOURCE_PLL1Q")
this:AddItem("LSE", "LL_RCC_RNG_CLKSOURCE_LSE")
this:AddItem("LSI", "LL_RCC_RNG_CLKSOURCE_LSI")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_RNG", "")
--*/
#define __CLK_SRC_RNG__ LL_RCC_RNG_CLKSOURCE_PLL1Q

/*--
this:AddWidget("Combobox", "USB clock source")
this:AddItem("DISABLE", "LL_RCC_USB_CLKSOURCE_DISABLE")
this:AddItem("PLL1Q", "LL_RCC_USB_CLKSOURCE_PLL1Q")
this:AddItem("PLL3Q", "LL_RCC_USB_CLKSOURCE_PLL3Q")
this:AddItem("HSI48", "LL_RCC_USB_CLKSOURCE_HSI48")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_USB", "")
--*/
#define __CLK_SRC_USB__ LL_RCC_USB_CLKSOURCE_PLL1Q

/*--
this:AddWidget("Combobox", "CEC clock source")
this:AddItem("LSE", "LL_RCC_CEC_CLKSOURCE_LSE")
this:AddItem("LSI", "LL_RCC_CEC_CLKSOURCE_LSI")
this:AddItem("CSI_DIV122", "LL_RCC_CEC_CLKSOURCE_CSI_DIV122")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_CEC", "")
--*/
#define __CLK_SRC_CEC__ LL_RCC_CEC_CLKSOURCE_LSE

/*--
this:AddWidget("Combobox", "DFSDM1 clock source")
this:AddItem("PCLK2", "LL_RCC_DFSDM1_CLKSOURCE_PCLK2")
this:AddItem("SYSCLK", "LL_RCC_DFSDM1_CLKSOURCE_SYSCLK")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_DFSDM1", "")
--*/
#define __CLK_SRC_DFSDM1__ LL_RCC_DFSDM1_CLKSOURCE_PCLK2

/*--
this:AddWidget("Combobox", "DFSDM2 clock source")
this:AddItem("PCLK4", "LL_RCC_DFSDM2_CLKSOURCE_PCLK4")
this:AddItem("SYSCLK", "LL_RCC_DFSDM2_CLKSOURCE_SYSCLK")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_DFSDM2", "")
--*/
#define __CLK_SRC_DFSDM2__ LL_RCC_DFSDM2_CLKSOURCE_PCLK4

/*--
this:AddWidget("Combobox", "FMC clock source")
this:AddItem("HCLK", "LL_RCC_FMC_CLKSOURCE_HCLK")
this:AddItem("PLL1Q", "LL_RCC_FMC_CLKSOURCE_PLL1Q")
this:AddItem("PLL2R", "LL_RCC_FMC_CLKSOURCE_PLL2R")
this:AddItem("CLKP", "LL_RCC_FMC_CLKSOURCE_CLKP")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_FMC", "")
--*/
#define __CLK_SRC_FMC__ LL_RCC_FMC_CLKSOURCE_HCLK

/*--
this:AddWidget("Combobox", "QSPI clock source")
this:AddItem("HCLK", "LL_RCC_QSPI_CLKSOURCE_HCLK")
this:AddItem("PLL1Q", "LL_RCC_QSPI_CLKSOURCE_PLL1Q")
this:AddItem("PLL2R", "LL_RCC_QSPI_CLKSOURCE_PLL2R")
this:AddItem("CLKP", "LL_RCC_QSPI_CLKSOURCE_CLKP")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_QSPI", "")
--*/
#define __CLK_SRC_QSPI__ LL_RCC_QSPI_CLKSOURCE_HCLK

/*--
this:AddWidget("Combobox", "OSPI clock source")
this:AddItem("HCLK", "LL_RCC_OSPI_CLKSOURCE_HCLK")
this:AddItem("PLL1Q", "LL_RCC_OSPI_CLKSOURCE_PLL1Q")
this:AddItem("PLL2R", "LL_RCC_OSPI_CLKSOURCE_PLL2R")
this:AddItem("CLKP", "LL_RCC_OSPI_CLKSOURCE_CLKP")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_OSPI", "")
--*/
#define __CLK_SRC_OSPI__ LL_RCC_OSPI_CLKSOURCE_HCLK

/*--
this:AddWidget("Combobox", "DSI clock source")
this:AddItem("PHY", "LL_RCC_DSI_CLKSOURCE_PHY")
this:AddItem("PLL2Q", "LL_RCC_DSI_CLKSOURCE_PLL2Q")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_DSI", "")
--*/
#define __CLK_SRC_DSI__ LL_RCC_DSI_CLKSOURCE_PHY

/*--
this:AddWidget("Combobox", "SPI1..3 clock source")
this:AddItem("PLL1Q", "LL_RCC_SPI123_CLKSOURCE_PLL1Q")
this:AddItem("PLL2P", "LL_RCC_SPI123_CLKSOURCE_PLL2P")
this:AddItem("PLL3P", "LL_RCC_SPI123_CLKSOURCE_PLL3P")
this:AddItem("I2S_CKIN", "LL_RCC_SPI123_CLKSOURCE_I2S_CKIN")
this:AddItem("CLKP", "LL_RCC_SPI123_CLKSOURCE_CLKP")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_SPI123", "")
--*/
#define __CLK_SRC_SPI123__ LL_RCC_SPI123_CLKSOURCE_PLL1Q

/*--
this:AddWidget("Combobox", "SPI4..5 clock source")
this:AddItem("PCLK2", "LL_RCC_SPI45_CLKSOURCE_PCLK2")
this:AddItem("PLL2Q", "LL_RCC_SPI45_CLKSOURCE_PLL2Q")
this:AddItem("PLL3Q", "LL_RCC_SPI45_CLKSOURCE_PLL3Q")
this:AddItem("HSI", "LL_RCC_SPI45_CLKSOURCE_HSI")
this:AddItem("CSI", "LL_RCC_SPI45_CLKSOURCE_CSI")
this:AddItem("HSE", "LL_RCC_SPI45_CLKSOURCE_HSE")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_SPI45", "")
--*/
#define __CLK_SRC_SPI45__ LL_RCC_SPI45_CLKSOURCE_PCLK2

/*--
this:AddWidget("Combobox", "SPI6 clock source")
this:AddItem("PCLK4", "LL_RCC_SPI6_CLKSOURCE_PCLK4")
this:AddItem("PLL2Q", "LL_RCC_SPI6_CLKSOURCE_PLL2Q")
this:AddItem("PLL3Q", "LL_RCC_SPI6_CLKSOURCE_PLL3Q")
this:AddItem("HSI", "LL_RCC_SPI6_CLKSOURCE_HSI")
this:AddItem("CSI", "LL_RCC_SPI6_CLKSOURCE_CSI")
this:AddItem("HSE", "LL_RCC_SPI6_CLKSOURCE_HSE")
this:AddItem("I2S_CKIN", "LL_RCC_SPI6_CLKSOURCE_I2S_CKIN")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_SPI6", "")
--*/
#define __CLK_SRC_SPI6__ LL_RCC_SPI6_CLKSOURCE_PCLK4

/*--
this:AddWidget("Combobox", "FDCAN clock source")
this:AddItem("HSE", "LL_RCC_FDCAN_CLKSOURCE_HSE")
this:AddItem("PLL1Q", "LL_RCC_FDCAN_CLKSOURCE_PLL1Q")
this:AddItem("PLL2Q", "LL_RCC_FDCAN_CLKSOURCE_PLL2Q")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_FDCAN", "")
--*/
#define __CLK_SRC_FDCAN__ LL_RCC_FDCAN_CLKSOURCE_HSE

/*--
this:AddWidget("Combobox", "SWP clock source")
this:AddItem("PCLK1", "LL_RCC_SWP_CLKSOURCE_PCLK1")
this:AddItem("HSI", "LL_RCC_SWP_CLKSOURCE_HSI")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_SWP", "")
--*/
#define __CLK_SRC_SWP__ LL_RCC_SWP_CLKSOURCE_PCLK1

/*--
this:AddWidget("Combobox", "ADC clock source")
this:AddItem("PLL2P", "LL_RCC_ADC_CLKSOURCE_PLL2P")
this:AddItem("PLL3R", "LL_RCC_ADC_CLKSOURCE_PLL3R")
this:AddItem("CLKP", "LL_RCC_ADC_CLKSOURCE_CLKP")
this:SetEvent("clicked", function() this.CalculateFreq() end)
this:AddExtraWidget("Label", "LABEL_CLK_SRC_ADC", "")
--*/
#define __CLK_SRC_ADC__ LL_RCC_ADC_CLKSOURCE_PLL2P

//==============================================================================
// Calculated values
//==============================================================================
// Value is calculated according to SYSCLK source selection
#define __CLK_SYSCLK_SRC_STATUS__ LL_RCC_SYS_CLKSOURCE_STATUS_PLL1

// Value is calculated according to SYSCLK source selection
#define __CLK_HCLK_FREQ__ 240000000

//==============================================================================
// Calculation script
//==============================================================================
/*--
this.CalculateFreq = function(self)
    local freq = {}
    freq.NONE     = 0
    freq.DISABLE  = 0
    freq.PHY      = 100e6
    freq.I2S_CKIN = 192e3

    -- HSE clock frequency -----------------------------------------------------
    local HSEON = iff(this:GetFlagValue("__CLK_HSE_ON__") == "DISABLE", 0, 1)
    freq.HSE = uC.OSCFREQ * HSEON
    this:SetFlagValue("LABEL_HSE_ON", PrintFrequency(freq.HSE).." (HSE)")

    -- HSI & CSI clock frequency -----------------------------------------------
    freq.HSI = 64e6
    freq.CSI = 4e6
    freq.CSI_DIV122 = freq.CSI / 122
    freq.HSI48 = 48e6

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

    freq.pll1_ck = freq.ref_ck / tonumber(DIVM1) * tonumber(MULN1) * PLL1ON

    freq.pll1_p_ck = freq.pll1_ck / tonumber(DIVP1) * PLLPON
    freq.PLL1P = freq.pll1_p_ck
    this:SetFlagValue("LABEL_PLL1_P", PrintFrequency(freq.pll1_p_ck))

    freq.pll1_q_ck = freq.pll1_ck / tonumber(DIVQ1) * PLLQON
    freq.PLL1Q = freq.pll1_q_ck
    this:SetFlagValue("LABEL_PLL1_Q", PrintFrequency(freq.pll1_q_ck))

    freq.pll1_r_ck = freq.pll1_ck / tonumber(DIVR1) * PLLRON
    freq.PLL1R = freq.pll1_r_ck
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
    freq.PLL2P = freq.pll2_p_ck
    this:SetFlagValue("LABEL_PLL2_P", PrintFrequency(freq.pll2_p_ck))

    freq.pll2_q_ck = freq.pll2_ck / tonumber(DIVQ1) * PLLQON
    freq.PLL2Q = freq.pll2_q_ck
    this:SetFlagValue("LABEL_PLL2_Q", PrintFrequency(freq.pll2_q_ck))

    freq.pll2_r_ck = freq.pll2_ck / tonumber(DIVR1) * PLLRON
    freq.PLL2R = freq.pll2_r_ck
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
    freq.PLL3P = freq.pll3_p_ck
    this:SetFlagValue("LABEL_PLL3_P", PrintFrequency(freq.pll3_p_ck))

    freq.pll3_q_ck = freq.pll3_ck / tonumber(DIVQ1) * PLLQON
    freq.PLL3Q = freq.pll3_q_ck
    this:SetFlagValue("LABEL_PLL3_Q", PrintFrequency(freq.pll3_q_ck))

    freq.pll3_r_ck = freq.pll3_ck / tonumber(DIVR1) * PLLRON
    freq.PLL3R = freq.pll3_r_ck
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
    freq.SYSCLK = freq.sysclk
    this:SetFlagValue("LABEL_SYSCLK_DIV", PrintFrequency(freq.sysclk))

    -- AHB clk frequency
    local AHBCLK_DIV = this:GetFlagValue("__CLK_AHB_DIV__"):gsub("LL_RCC_AHB_DIV_", "")
    freq.ahbclk = freq.sys_ck / tonumber(AHBCLK_DIV)
    freq.HCLK = freq.ahbclk
    this:SetFlagValue("LABEL_AHBCLK_DIV", PrintFrequency(freq.ahbclk))

    -- APB1 clk frequency
    local APB1CLK_DIV = this:GetFlagValue("__CLK_APB1_DIV__"):gsub("LL_RCC_APB1_DIV_", "")
    freq.apb1clk = freq.ahbclk / tonumber(APB1CLK_DIV)
    freq.PCLK1 = freq.apb1clk
    this:SetFlagValue("LABEL_APB1CLK_DIV", PrintFrequency(freq.apb1clk))

    -- APB2 clk frequency
    local APB2CLK_DIV = this:GetFlagValue("__CLK_APB2_DIV__"):gsub("LL_RCC_APB2_DIV_", "")
    freq.apb2clk = freq.ahbclk / tonumber(APB2CLK_DIV)
    freq.PCLK2 = freq.apb2clk
    this:SetFlagValue("LABEL_APB2CLK_DIV", PrintFrequency(freq.apb2clk))

    -- APB3 clk frequency
    local APB3CLK_DIV = this:GetFlagValue("__CLK_APB3_DIV__"):gsub("LL_RCC_APB3_DIV_", "")
    freq.apb3clk = freq.ahbclk / tonumber(APB3CLK_DIV)
    freq.PCLK3 = freq.apb3clk
    this:SetFlagValue("LABEL_APB3CLK_DIV", PrintFrequency(freq.apb3clk))

    -- APB4 clk frequency
    local APB4CLK_DIV = this:GetFlagValue("__CLK_APB4_DIV__"):gsub("LL_RCC_APB4_DIV_", "")
    freq.apb4clk = freq.ahbclk / tonumber(APB4CLK_DIV)
    freq.PCLK4 = freq.apb4clk
    this:SetFlagValue("LABEL_APB4CLK_DIV", PrintFrequency(freq.apb4clk))

    -- PERIPHERAL CLOCK --------------------------------------------------------
    -- UART1..6 frequency
    local SEL = this:GetFlagValue("__CLK_SRC_UART16__"):gsub("LL_RCC_USART16_CLKSOURCE_", "")
    freq.uart16 = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_UART16", PrintFrequency(freq.uart16))

    -- UART234578 frequency
    local SEL = this:GetFlagValue("__CLK_SRC_UART234578__"):gsub("LL_RCC_USART234578_CLKSOURCE_", "")
    freq.uart234578 = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_UART234578", PrintFrequency(freq.uart234578))

    -- LPUART1 frequency
    local SEL = this:GetFlagValue("__CLK_SRC_LPUART1__"):gsub("LL_RCC_LPUART1_CLKSOURCE_", "")
    freq.lpuart1 = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_LPUART1", PrintFrequency(freq.lpuart1))

    -- I2C123 frequency
    local SEL = this:GetFlagValue("__CLK_SRC_I2C123__"):gsub("LL_RCC_I2C123_CLKSOURCE_", "")
    freq.i2c123 = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_I2C123", PrintFrequency(freq.i2c123))

    -- I2C4 frequency
    local SEL = this:GetFlagValue("__CLK_SRC_I2C4__"):gsub("LL_RCC_I2C4_CLKSOURCE_", "")
    freq.i2c4 = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_I2C4", PrintFrequency(freq.i2c4))

    -- LPTIM1 frequency
    local SEL = this:GetFlagValue("__CLK_SRC_LPTIM1__"):gsub("LL_RCC_LPTIM1_CLKSOURCE_", "")
    freq.LPTIM1 = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_LPTIM1", PrintFrequency(freq.LPTIM1))

    -- LPTIM2 frequency
    local SEL = this:GetFlagValue("__CLK_SRC_LPTIM2__"):gsub("LL_RCC_LPTIM2_CLKSOURCE_", "")
    freq.LPTIM2 = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_LPTIM2", PrintFrequency(freq.LPTIM2))

    -- LPTIM345 frequency
    local SEL = this:GetFlagValue("__CLK_SRC_LPTIM345__"):gsub("LL_RCC_LPTIM345_CLKSOURCE_", "")
    freq.LPTIM345 = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_LPTIM345", PrintFrequency(freq.LPTIM345))

    -- CLKP frequency
    local SEL = this:GetFlagValue("__CLK_SRC_CLKP__"):gsub("LL_RCC_CLKP_CLKSOURCE_", "")
    freq.CLKP = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_CLKP", PrintFrequency(freq.CLKP))

    -- SPDIF frequency
    local SEL = this:GetFlagValue("__CLK_SRC_SPDIF__"):gsub("LL_RCC_SPDIF_CLKSOURCE_", "")
    freq.SPDIF = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_SPDIF", PrintFrequency(freq.SPDIF))

    -- SAI1 frequency
    local SEL = this:GetFlagValue("__CLK_SRC_SAI1__"):gsub("LL_RCC_SAI1_CLKSOURCE_", "")
    freq.SAI1 = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_SAI1", PrintFrequency(freq.SAI1))

    -- SAI23 frequency
    local SEL = this:GetFlagValue("__CLK_SRC_SAI23__"):gsub("LL_RCC_SAI23_CLKSOURCE_", "")
    freq.SAI23 = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_SAI23", PrintFrequency(freq.SAI23))

    -- SAI2A frequency
    local SEL = this:GetFlagValue("__CLK_SRC_SAI2A__"):gsub("LL_RCC_SAI2A_CLKSOURCE_", "")
    freq.SAI2A = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_SAI2A", PrintFrequency(freq.SAI2A))

    -- SAI2B frequency
    local SEL = this:GetFlagValue("__CLK_SRC_SAI2B__"):gsub("LL_RCC_SAI2B_CLKSOURCE_", "")
    freq.SAI2B = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_SAI2B", PrintFrequency(freq.SAI2B))

    -- SAI4A frequency
    local SEL = this:GetFlagValue("__CLK_SRC_SAI4A__"):gsub("LL_RCC_SAI4A_CLKSOURCE_", "")
    freq.SAI4A = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_SAI4A", PrintFrequency(freq.SAI4A))

    -- SAI4B frequency
    local SEL = this:GetFlagValue("__CLK_SRC_SAI4B__"):gsub("LL_RCC_SAI4B_CLKSOURCE_", "")
    freq.SAI4B = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_SAI4B", PrintFrequency(freq.SAI4B))

    -- SDMMC frequency
    local SEL = this:GetFlagValue("__CLK_SRC_SDMMC__"):gsub("LL_RCC_SDMMC_CLKSOURCE_", "")
    freq.SDMMC = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_SDMMC", PrintFrequency(freq.SDMMC))

    -- RNG frequency
    local SEL = this:GetFlagValue("__CLK_SRC_RNG__"):gsub("LL_RCC_RNG_CLKSOURCE_", "")
    freq.RNG = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_RNG", PrintFrequency(freq.RNG))

    -- USB frequency
    local SEL = this:GetFlagValue("__CLK_SRC_USB__"):gsub("LL_RCC_USB_CLKSOURCE_", "")
    freq.USB = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_USB", PrintFrequency(freq.USB))

    -- CEC frequency
    local SEL = this:GetFlagValue("__CLK_SRC_CEC__"):gsub("LL_RCC_CEC_CLKSOURCE_", "")
    freq.CEC = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_CEC", PrintFrequency(freq.CEC))

    -- DFSDM1 frequency
    local SEL = this:GetFlagValue("__CLK_SRC_DFSDM1__"):gsub("LL_RCC_DFSDM1_CLKSOURCE_", "")
    freq.DFSDM1 = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_DFSDM1", PrintFrequency(freq.DFSDM1))

    -- DFSDM2 frequency
    local SEL = this:GetFlagValue("__CLK_SRC_DFSDM2__"):gsub("LL_RCC_DFSDM2_CLKSOURCE_", "")
    freq.DFSDM2 = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_DFSDM2", PrintFrequency(freq.DFSDM2))

    -- FMC frequency
    local SEL = this:GetFlagValue("__CLK_SRC_FMC__"):gsub("LL_RCC_FMC_CLKSOURCE_", "")
    freq.FMC = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_FMC", PrintFrequency(freq.FMC))

    -- QSPI frequency
    local SEL = this:GetFlagValue("__CLK_SRC_QSPI__"):gsub("LL_RCC_QSPI_CLKSOURCE_", "")
    freq.QSPI = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_QSPI", PrintFrequency(freq.QSPI))

    -- OSPI frequency
    local SEL = this:GetFlagValue("__CLK_SRC_OSPI__"):gsub("LL_RCC_OSPI_CLKSOURCE_", "")
    freq.OSPI = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_OSPI", PrintFrequency(freq.OSPI))

    -- DSI frequency
    local SEL = this:GetFlagValue("__CLK_SRC_DSI__"):gsub("LL_RCC_DSI_CLKSOURCE_", "")
    freq.DSI = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_DSI", PrintFrequency(freq.DSI))

    -- SPI123 frequency
    local SEL = this:GetFlagValue("__CLK_SRC_SPI123__"):gsub("LL_RCC_SPI123_CLKSOURCE_", "")
    freq.SPI123 = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_SPI123", PrintFrequency(freq.SPI123))

    -- SPI45 frequency
    local SEL = this:GetFlagValue("__CLK_SRC_SPI45__"):gsub("LL_RCC_SPI45_CLKSOURCE_", "")
    freq.SPI45 = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_SPI45", PrintFrequency(freq.SPI45))

    -- SPI6 frequency
    local SEL = this:GetFlagValue("__CLK_SRC_SPI6__"):gsub("LL_RCC_SPI6_CLKSOURCE_", "")
    freq.SPI6 = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_SPI6", PrintFrequency(freq.SPI6))

    -- FDCAN frequency
    local SEL = this:GetFlagValue("__CLK_SRC_FDCAN__"):gsub("LL_RCC_FDCAN_CLKSOURCE_", "")
    freq.FDCAN = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_FDCAN", PrintFrequency(freq.FDCAN))

    -- SWP frequency
    local SEL = this:GetFlagValue("__CLK_SRC_SWP__"):gsub("LL_RCC_SWP_CLKSOURCE_", "")
    freq.SWP = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_SWP", PrintFrequency(freq.SWP))

    -- ADC frequency
    local SEL = this:GetFlagValue("__CLK_SRC_ADC__"):gsub("LL_RCC_ADC_CLKSOURCE_", "")
    freq.ADC = freq[SEL];
    this:SetFlagValue("LABEL_CLK_SRC_ADC", PrintFrequency(freq.ADC))

    -- end calculations
    this:SetFlagValue("__CLK_SYSCLK_SRC_STATUS__", "LL_RCC_SYS_CLKSOURCE_STATUS_"..SYSCLKSRC)
    this:SetFlagValue("__CLK_HCLK_FREQ__", tostring(freq.HCLK))
end

-- trigger frequency calculation after loading all flags
this.CalculateFreq()
++*/

#endif /* _CLK_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
