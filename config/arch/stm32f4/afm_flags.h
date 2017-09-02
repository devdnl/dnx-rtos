/*=========================================================================*//**
@file    afm_flags.h

@author  Daniel Zorychta

@brief   SYSCFG Module configuration.

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
this:AddItem("No swap", "(0 << 10)")
this:AddItem("Swap SDRAM, NAND/PCCARD swapped", "(1 << 10)")
this:SetToolTip("SDRAM banks and NAND Bank 2/PCCARD mapping are swapped. SDRAM\n "..
                "Bank 1 and 2 are mapped at NAND Bank 2 (0x8000 0000) and PCCARD Bank \n"..
                "(0x9000 0000) address, respectively. NAND Bank 2 and PCCARD Bank are \n"..
                "mapped at 0xC000 0000 and 0xD000 0000, respectively.")
--*/
#define __AFM_SWP_FMC__ (0 << 10)

/*--
this:AddWidget("Combobox", "Flash Bank mode")
this:AddItem("No map", "(0 << 8)")
this:AddItem("Bank 1 and 2 swapped", "(1 << 8)")
--*/
#define __AFM_FB_MODE__ (0 << 8)

/*--
this:AddWidget("Combobox", "Memory mapping")
this:AddItem("Selected by Boot pins", "_AFM_MEM_MODE_AUTO")
this:AddItem("Main Flash memory mapped at 0x0000 0000", "(0 << 0)")
this:AddItem("System Flash memory mapped at 0x0000 0000", "(1 << 0)")
this:AddItem("FMC Bank1 (NOR/PSRAM 1 and 2) mapped at 0x0000 0000", "(2 << 0)")
this:AddItem("Embedded SRAM (SRAM1) mapped at 0x0000 0000", "(3 << 0)")
this:AddItem("FMC/SDRAM Bank 1 and 2 mapped at 0x0000 0000", "(4 << 0)")
--*/
#define __AFM_MEM_MODE__ _AFM_MEM_MODE_AUTO


/*--
this:AddExtraWidget("Label", "LabelPMC", "\nPeripheral mode", -1, "bold")
this:AddExtraWidget("Void", "VoidPMC")
++*/
/*--
this:AddWidget("Combobox", "Ethernet PHY interface")
this:AddItem("MII interface", "(0 << 23)")
this:AddItem("RMII interface", "(1 << 23)")

--*/
#define __AFM_MII_RMII_SEL__ (0 << 23)


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

#endif /* _AFIO_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
