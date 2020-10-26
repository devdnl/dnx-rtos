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



#define __CLK_PLL_SRC__ LL_RCC_PLLSOURCE_HSI


#define __CLK_PLL1_P_ENABLE__ _YES_
#define __CLK_PLL1_Q_ENABLE__ _YES_
#define __CLK_PLL1_R_ENABLE__ _NO_
#define __CLK_PLL1_P__ 1
#define __CLK_PLL1_Q__ 1
#define __CLK_PLL1_R__ 1
#define __CLK_PLL1_M__ 8 // 0-63
#define __CLK_PLL1_N__ 60 // 4-512

#define __CLK_PLL2_P_ENABLE__ _YES_
#define __CLK_PLL2_Q_ENABLE__ _YES_
#define __CLK_PLL2_R_ENABLE__ _NO_
#define __CLK_PLL2_P__ 1
#define __CLK_PLL2_Q__ 1
#define __CLK_PLL2_R__ 1
#define __CLK_PLL2_M__ 8 // 0-63
#define __CLK_PLL2_N__ 60 // 4-512

#define __CLK_PLL3_P_ENABLE__ _YES_
#define __CLK_PLL3_Q_ENABLE__ _YES_
#define __CLK_PLL3_R_ENABLE__ _NO_
#define __CLK_PLL3_P__ 1
#define __CLK_PLL3_Q__ 1
#define __CLK_PLL3_R__ 1
#define __CLK_PLL3_M__ 8 // 0-63
#define __CLK_PLL3_N__ 60 // 4-512

#define __CLK_SYSCLK_DIV__ LL_RCC_SYSCLK_DIV_1
#define __CLK_AHB_DIV__ LL_RCC_AHB_DIV_2
#define __CLK_APB1_DIV__ LL_RCC_APB1_DIV_8
#define __CLK_APB2_DIV__ LL_RCC_APB2_DIV_8
#define __CLK_APB3_DIV__ LL_RCC_APB3_DIV_8
#define __CLK_APB4_DIV__ LL_RCC_APB4_DIV_8

// to musi być razem...
#define __CLK_SYSCLK_SRC__ LL_RCC_SYS_CLKSOURCE_PLL1
#define __CLK_SYSCLK_SRC_STATUS__ LL_RCC_SYS_CLKSOURCE_STATUS_PLL1


#endif /* _CLK_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
