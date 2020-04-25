/*=========================================================================*//**
@file    afm_flags.h

@author  Daniel Zorychta

@brief   SYSCFG Module configuration.

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

#ifndef _AFM_FLAGS_H_
#define _AFM_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > AFM",
               function() this:LoadFile("arch/arch_flags.h") end)
++*/

/*--
this:AddExtraWidget("Label", "LabelMEMRMP", "\nMemory remap", -1, "bold")
this:AddExtraWidget("Void", "VoidMEMRMP")
++*/
/*--
this:AddWidget("Combobox", "FMC memory mapping swap")
this:AddItem("No swap", "(0 << SYSCFG_MEMRMP_SWP_FMC_Pos)")
this:AddItem("NOR/RAM and SDRAM memory swapped", "(1 << SYSCFG_MEMRMP_SWP_FMC_Pos)")
--*/
#define __AFM_SWP_FMC__ (0 << SYSCFG_MEMRMP_SWP_FMC_Pos)

/*--
this:AddWidget("Combobox", "Flash Bank mode")
this:AddItem("Default Flash Bank mapping", "(0 << SYSCFG_MEMRMP_SWP_FB_Pos)")
this:AddItem("Flash Bank swapped", "(1 << SYSCFG_MEMRMP_SWP_FB_Pos)")
--*/
#define __AFM_SWP_FB__ (0 << SYSCFG_MEMRMP_SWP_FB_Pos)

/*--
this:AddWidget("Combobox", "Memory boot mapping")
this:AddItem("Boot mem. base addr. defined by BOOT_ADD0 option byte", "(0 << SYSCFG_MEMRMP_MEM_BOOT_Pos)")
this:AddItem("Boot mem. base addr. defined by BOOT_ADD1 option byte", "(1 << SYSCFG_MEMRMP_MEM_BOOT_Pos)")
--*/
#define __AFM_MEM_BOOT__ (0 << SYSCFG_MEMRMP_MEM_BOOT_Pos)


/*--
this:AddExtraWidget("Label", "LabelPMC", "\nPeripheral mode", -1, "bold")
this:AddExtraWidget("Void", "VoidPMC")
++*/
/*--
this:AddWidget("Combobox", "Ethernet PHY interface")
this:AddItem("MII interface", "(0 << SYSCFG_PMC_MII_RMII_SEL_Pos)")
this:AddItem("RMII interface", "(1 << SYSCFG_PMC_MII_RMII_SEL_Pos)")
--*/
#define __AFM_MII_RMII_SEL__ (0 << SYSCFG_PMC_MII_RMII_SEL_Pos)

/*--
this:AddWidget("Spinbox", 0, 7, "ADCxDC2 (refer to AN4073)")
--*/
#define __AFM_ADCxDC2__ 0

/*--
this:AddWidget("Combobox", "PB9 Fast Mode + Enable")
this:AddItem("Default value", "(0 << SYSCFG_PMC_PB9_FMP_Pos)")
this:AddItem("It forces FM+ drive capability on PB9 pin", "(1 << SYSCFG_PMC_PB9_FMP_Pos)")
--*/
#define __AFM_PB9_FMP__ (0 << SYSCFG_PMC_PB9_FMP_Pos)

/*--
this:AddWidget("Combobox", "PB8 Fast Mode + Enable")
this:AddItem("Default value", "(0 << SYSCFG_PMC_PB8_FMP_Pos)")
this:AddItem("It forces FM+ drive capability on PB8 pin", "(1 << SYSCFG_PMC_PB8_FMP_Pos)")
--*/
#define __AFM_PB8_FMP__ (0 << SYSCFG_PMC_PB8_FMP_Pos)

/*--
this:AddWidget("Combobox", "PB7 Fast Mode + Enable")
this:AddItem("Default value", "(0 << SYSCFG_PMC_PB7_FMP_Pos)")
this:AddItem("It forces FM+ drive capability on PB7 pin", "(1 << SYSCFG_PMC_PB7_FMP_Pos)")
--*/
#define __AFM_PB7_FMP__ (0 << SYSCFG_PMC_PB7_FMP_Pos)

/*--
this:AddWidget("Combobox", "PB6 Fast Mode + Enable")
this:AddItem("Default value", "(0 << SYSCFG_PMC_PB6_FMP_Pos)")
this:AddItem("It forces FM+ drive capability on PB6 pin", "(1 << SYSCFG_PMC_PB6_FMP_Pos)")
--*/
#define __AFM_PB6_FMP__ (0 << SYSCFG_PMC_PB6_FMP_Pos)

/*--
this:AddWidget("Combobox", "I2C4 Fast Mode + Enable")
this:AddItem("Default value", "(0 << SYSCFG_PMC_I2C4_FMP_Pos)")
this:AddItem("It forces FM+ drive capability on I2C4", "(1 << SYSCFG_PMC_I2C4_FMP_Pos)")
--*/
#define __AFM_I2C4_FMP__ (0 << SYSCFG_PMC_I2C4_FMP_Pos)

/*--
this:AddWidget("Combobox", "I2C3 Fast Mode + Enable")
this:AddItem("Default value", "(0 << SYSCFG_PMC_I2C3_FMP_Pos)")
this:AddItem("It forces FM+ drive capability on I2C3", "(1 << SYSCFG_PMC_I2C3_FMP_Pos)")
--*/
#define __AFM_I2C3_FMP__ (0 << SYSCFG_PMC_I2C3_FMP_Pos)

/*--
this:AddWidget("Combobox", "I2C2 Fast Mode + Enable")
this:AddItem("Default value", "(0 << SYSCFG_PMC_I2C2_FMP_Pos)")
this:AddItem("It forces FM+ drive capability on I2C2", "(1 << SYSCFG_PMC_I2C2_FMP_Pos)")
--*/
#define __AFM_I2C2_FMP__ (0 << SYSCFG_PMC_I2C2_FMP_Pos)

/*--
this:AddWidget("Combobox", "I2C1 Fast Mode + Enable")
this:AddItem("Default value", "(0 << SYSCFG_PMC_I2C1_FMP_Pos)")
this:AddItem("It forces FM+ drive capability on I2C1", "(1 << SYSCFG_PMC_I2C1_FMP_Pos)")
--*/
#define __AFM_I2C1_FMP__ (0 << SYSCFG_PMC_I2C1_FMP_Pos)


/*--
this:AddExtraWidget("Label", "LabelEXTI", "\nEXTI ports", -1, "bold")
this:AddExtraWidget("Void", "VoidEXTI")
++*/
/*--
this:AddWidget("Combobox", "EXTI0 port")
for i = 0, 9 do local c = string.char(65 + i) this:AddItem("P"..c.."0", "("..tostring(i).." << 0)") end
--*/
#define __AFM_EXTI0_PORT__ (0 << 0)

/*--
this:AddWidget("Combobox", "EXTI1 port")
for i = 0, 9 do local c = string.char(65 + i) this:AddItem("P"..c.."1", "("..tostring(i).." << 4)") end
--*/
#define __AFM_EXTI1_PORT__ (0 << 4)

/*--
this:AddWidget("Combobox", "EXTI2 port")
for i = 0, 9 do local c = string.char(65 + i) this:AddItem("P"..c.."2", "("..tostring(i).." << 8)") end
--*/
#define __AFM_EXTI2_PORT__ (0 << 8)

/*--
this:AddWidget("Combobox", "EXTI3 port")
for i = 0, 9 do local c = string.char(65 + i) this:AddItem("P"..c.."3", "("..tostring(i).." << 12)") end
--*/
#define __AFM_EXTI3_PORT__ (0 << 12)

/*--
this:AddWidget("Combobox", "EXTI4 port")
for i = 0, 9 do local c = string.char(65 + i) this:AddItem("P"..c.."4", "("..tostring(i).." << 0)") end
--*/
#define __AFM_EXTI4_PORT__ (0 << 0)

/*--
this:AddWidget("Combobox", "EXTI5 port")
for i = 0, 9 do local c = string.char(65 + i) this:AddItem("P"..c.."5", "("..tostring(i).." << 4)") end
--*/
#define __AFM_EXTI5_PORT__ (0 << 4)

/*--
this:AddWidget("Combobox", "EXTI6 port")
for i = 0, 9 do local c = string.char(65 + i) this:AddItem("P"..c.."6", "("..tostring(i).." << 8)") end
--*/
#define __AFM_EXTI6_PORT__ (0 << 8)

/*--
this:AddWidget("Combobox", "EXTI7 port")
for i = 0, 9 do local c = string.char(65 + i) this:AddItem("P"..c.."7", "("..tostring(i).." << 12)") end
--*/
#define __AFM_EXTI7_PORT__ (0 << 12)

/*--
this:AddWidget("Combobox", "EXTI8 port")
for i = 0, 9 do local c = string.char(65 + i) this:AddItem("P"..c.."8", "("..tostring(i).." << 0)") end
--*/
#define __AFM_EXTI8_PORT__ (0 << 0)

/*--
this:AddWidget("Combobox", "EXTI9 port")
for i = 0, 9 do local c = string.char(65 + i) this:AddItem("P"..c.."9", "("..tostring(i).." << 4)") end
--*/
#define __AFM_EXTI9_PORT__ (0 << 4)

/*--
this:AddWidget("Combobox", "EXTI10 port")
for i = 0, 9 do local c = string.char(65 + i) this:AddItem("P"..c.."10", "("..tostring(i).." << 8)") end
--*/
#define __AFM_EXTI10_PORT__ (0 << 8)

/*--
this:AddWidget("Combobox", "EXTI11 port")
for i = 0, 9 do local c = string.char(65 + i) this:AddItem("P"..c.."11", "("..tostring(i).." << 12)") end
--*/
#define __AFM_EXTI11_PORT__ (0 << 12)

/*--
this:AddWidget("Combobox", "EXTI12 port")
for i = 0, 9 do local c = string.char(65 + i) this:AddItem("P"..c.."12", "("..tostring(i).." << 0)") end
--*/
#define __AFM_EXTI12_PORT__ (0 << 0)

/*--
this:AddWidget("Combobox", "EXTI13 port")
for i = 0, 9 do local c = string.char(65 + i) this:AddItem("P"..c.."13", "("..tostring(i).." << 4)") end
--*/
#define __AFM_EXTI13_PORT__ (0 << 4)

/*--
this:AddWidget("Combobox", "EXTI14 port")
for i = 0, 9 do local c = string.char(65 + i) this:AddItem("P"..c.."14", "("..tostring(i).." << 8)") end
--*/
#define __AFM_EXTI14_PORT__ (0 << 8)

/*--
this:AddWidget("Combobox", "EXTI15 port")
for i = 0, 9 do local c = string.char(65 + i) this:AddItem("P"..c.."15", "("..tostring(i).." << 12)") end
--*/
#define __AFM_EXTI15_PORT__ (0 << 12)


/*--
this:AddExtraWidget("Label", "LabelCBR", "\nClass B", -1, "bold")
this:AddExtraWidget("Void", "VoidCBR")
++*/
/*--
this:AddWidget("Combobox", "PVD Lock")
this:AddItem("PVD interrupt not connected to Break input TIMER1", "(0 << SYSCFG_CBR_PVDL_Pos)")
this:AddItem("PVD interrupt connected to Break input TIMER1/8", "(1 << SYSCFG_CBR_PVDL_Pos)")
--*/
#define __AFM_PVDL__ (0 << SYSCFG_CBR_PVDL_Pos)

/*--
this:AddWidget("Combobox", "Core Lockup Lock")
this:AddItem("Lockup output of Cortex-M7 is not connected with Break input of TIMER1/8", "(0 << SYSCFG_CBR_CLL_Pos)")
this:AddItem("Lockup output of Cortex-M7 is connected with Break input of TIMER1/8", "(1 << SYSCFG_CBR_CLL_Pos)")
--*/
#define __AFM_CLL__ (0 << SYSCFG_CBR_CLL_Pos)


/*--
this:AddExtraWidget("Label", "LabelCMPCR", "\nCompensation cell control", -1, "bold")
this:AddExtraWidget("Void", "VoidCMPCELL")
++*/
/*--
this:AddWidget("Combobox", "Compensation cell power-down")
this:AddItem("I/O compensation cell power-down mode", "(0 << SYSCFG_CMPCR_CMP_PD_Pos)")
this:AddItem("I/O compensation cell enabled", "(1 << SYSCFG_CMPCR_CMP_PD_Pos)")
--*/
#define __AFM_CMP_PD__ (0 << SYSCFG_CMPCR_CMP_PD_Pos)

#endif /* _AFIO_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
