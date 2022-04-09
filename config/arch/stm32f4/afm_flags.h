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
#define __AFM_MII_RMII_SEL__ (1 << 23)

/*--
this:AddExtraWidget("Label", "LabelBKPRAM", "\nBackup RAM", -1, "bold")
this:AddExtraWidget("Void", "VoidBKPRAM")
++*/
/*--
this:AddWidget("Combobox", "Backup RAM")
this:AddItem("Disable", "_NO_")
this:AddItem("Enable", "_YES_")
--*/
#define __AFM_BKPRAM_ENABLE__ _NO_

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
