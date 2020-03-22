/*==============================================================================
File     fmc_flags.h

Author   Daniel Zorychta

Brief    Flexible Memory Controller

         Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the  Free Software  Foundation;  either version 2 of the License, or
         any later version.

         This  program  is  distributed  in the hope that  it will be useful,
         but  WITHOUT  ANY  WARRANTY;  without  even  the implied warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         You  should  have received a copy  of the GNU General Public License
         along  with  this  program;  if not,  write  to  the  Free  Software
         Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


==============================================================================*/

/*
 * NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as prefix
 *       and suffix) are exported to the single configuration file
 *       (by using Configtool) when entire project configuration is exported.
 *       All other flag definitions and statements are ignored.
 */

#ifndef _FMC_FLAGS_H_
#define _FMC_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > FMC",
               function() this:LoadFile("arch/arch_flags.h") end)
++*/

//------------------------------------------------------------------------------
// BANK 1
//------------------------------------------------------------------------------
/*--
this:AddExtraWidget("Label", "LABEL_BANK1", "Bank 1 (NOR/PSRAM)", -1, "bold")
this:AddExtraWidget("Void", "Void_BANK1")
++*/

//------------------------------------------------------------------------------
// BANK 2
//------------------------------------------------------------------------------
/*--
this:AddExtraWidget("Label", "LABEL_BANK2", "Bank 2 (NAND)", -1, "bold")
this:AddExtraWidget("Void", "Void_BANK2")
++*/

//------------------------------------------------------------------------------
// BANK 3
//------------------------------------------------------------------------------
/*--
this:AddExtraWidget("Label", "LABEL_BANK3", "Bank 3 (NAND)", -1, "bold")
this:AddExtraWidget("Void", "Void_BANK3")
++*/

//------------------------------------------------------------------------------
// BANK 4
//------------------------------------------------------------------------------
/*--
this:AddExtraWidget("Label", "LABEL_BANK4", "Bank 4 (PC Card)", -1, "bold")
this:AddExtraWidget("Void", "Void_BANK4")
++*/

//------------------------------------------------------------------------------
// BANK 5 & 6
//------------------------------------------------------------------------------
/*--
this:AddExtraWidget("Label", "LABEL_BANK5", "\nBank 5, 6 (SDRAM)", -1, "bold")
this:AddExtraWidget("Void", "Void_BANK5_6")
++*/
/*--
this:AddWidget("Combobox", "Read pipe delay")
this:AddItem("No delay", "0")
this:AddItem("1 HCLK delay", "1")
this:AddItem("2 HCLK delay", "2")
--*/
#define __FMC_SDRAM_RPIPE__ 0

/*--
this:AddWidget("Combobox", "Burst read mode")
this:AddItem("Disable", "0")
this:AddItem("Enable", "1")
--*/
#define __FMC_SDRAM_RBURST__ 1

/*--
this:AddWidget("Combobox", "SDRAM clock period")
this:AddItem("SDCLK disabled", "0")
this:AddItem("2 x HCLK periods", "2")
this:AddItem("3 x HCLK periods", "3")
--*/
#define __FMC_SDRAM_SDCLK__ 2

/*--
this:AddWidget("Spinbox", 1, 16, "Row precharge delay")
--*/
#define __FMC_SDRAM_TRP__ 16

/*--
this:AddWidget("Spinbox", 1, 16, "Row cycle delay")
--*/
#define __FMC_SDRAM_TRC__ 16

/*--
this:AddWidget("Spinbox", 1, 3, "CAS Latency")
--*/
#define __FMC_SDRAM_CAS__ 3

/*--
this:AddWidget("Spinbox", 1, 16, "Self refresh time")
--*/
#define __FMC_SDRAM_TRAS__ 16

/*--
this:AddWidget("Spinbox", 1, 16, "Exit Self-refresh time")
--*/
#define __FMC_SDRAM_TXSR__ 16

/*--
this:AddWidget("Spinbox", 1, 16, "Load Mode Register to Active")
this:SetToolTip("Define the delay between a Load Mode Register command and an "..
                "Active or Refresh command.")
--*/
#define __FMC_SDRAM_TMRD__ 16

/*--
this:AddWidget("Spinbox", 1, 15, "Number of Auto-refresh")
--*/
#define __FMC_SDRAM_NRFS__ 8

/*--
this:AddWidget("Spinbox", 1, 128, "Refresh rate [ms]")
--*/
#define __FMC_SDRAM_REFRESH_RATE_MS__ 64

//------------------------------------------------------------------------------
/*--
this:AddExtraWidget("Label", "LABEL_SDRAM1", "\nSDRAM 1", -1, "bold")
this:AddExtraWidget("Void", "Void_SDRAM1")
++*/
/*--
this:AddWidget("Combobox", "Memory bank")
this:AddItem("Disable", "0")
this:AddItem("Enable", "1")
this:SetEvent("clicked", function() this:SDRAMEn(1) end)
--*/
#define __FMC_SDRAM_1_ENABLE__ 1

/*--
this:AddWidget("Combobox", "Number of internal banks")
this:AddItem("2 internal banks", "0")
this:AddItem("4 internal banks", "1")
--*/
#define __FMC_SDRAM_1_NB__ 1

/*--
this:AddWidget("Combobox", "Data bus width")
this:AddItem("8 bits", "0")
this:AddItem("16 bits", "1")
this:AddItem("32 bits", "2")
--*/
#define __FMC_SDRAM_1_MWID__ 1

/*--
this:AddWidget("Combobox", "Number of row address bits")
this:AddItem("11 bits", "0")
this:AddItem("12 bits", "1")
this:AddItem("13 bits", "2")
--*/
#define __FMC_SDRAM_1_NR__ 1

/*--
this:AddWidget("Combobox", "Number of column address bits")
this:AddItem("8 bits", "0")
this:AddItem("9 bits", "1")
this:AddItem("10 bits", "2")
this:AddItem("11 bits", "3")
--*/
#define __FMC_SDRAM_1_NC__ 0

/*--
this:AddWidget("Spinbox", 1, 16, "Row to column delay")
--*/
#define __FMC_SDRAM_1_TRCD__ 16

//------------------------------------------------------------------------------
/*--
this:AddExtraWidget("Label", "LABEL_SDRAM2", "\nSDRAM 2", -1, "bold")
this:AddExtraWidget("Void", "Void_SDRAM2")
++*/
/*--
this:AddWidget("Combobox", "Memory bank")
this:AddItem("Disable", "0")
this:AddItem("Enable", "1")
this:SetEvent("clicked", function() this:SDRAMEn(2) end)
--*/
#define __FMC_SDRAM_2_ENABLE__ 0

/*--
this:AddWidget("Combobox", "Number of internal banks")
this:AddItem("2 internal banks", "0")
this:AddItem("4 internal banks", "1")
--*/
#define __FMC_SDRAM_2_NB__ 1

/*--
this:AddWidget("Combobox", "Data bus width")
this:AddItem("8 bits", "0")
this:AddItem("16 bits", "1")
this:AddItem("32 bits", "2")
--*/
#define __FMC_SDRAM_2_MWID__ 1

/*--
this:AddWidget("Combobox", "Number of row address bits")
this:AddItem("11 bits", "0")
this:AddItem("12 bits", "1")
this:AddItem("13 bits", "2")
--*/
#define __FMC_SDRAM_2_NR__ 1

/*--
this:AddWidget("Combobox", "Number of column address bits")
this:AddItem("8 bits", "0")
this:AddItem("9 bits", "1")
this:AddItem("10 bits", "2")
this:AddItem("11 bits", "3")
--*/
#define __FMC_SDRAM_2_NC__ 0

/*--
this:AddWidget("Spinbox", 1, 16, "Row to column delay")
--*/
#define __FMC_SDRAM_2_TRCD__ 16


//------------------------------------------------------------------------------
// FUNCTIONS
//------------------------------------------------------------------------------
/*--
this.SDRAMEn = function(self, SDRAM)
    local en = false
    if this:GetFlagValue("__FMC_SDRAM_"..SDRAM.."_ENABLE__") ~= "0" then
        en = true
    end

    this:Enable(en, "__FMC_SDRAM_"..SDRAM.."_NB__")
    this:Enable(en, "__FMC_SDRAM_"..SDRAM.."_MWID__")
    this:Enable(en, "__FMC_SDRAM_"..SDRAM.."_NR__")
    this:Enable(en, "__FMC_SDRAM_"..SDRAM.."_NC__")
    this:Enable(en, "__FMC_SDRAM_"..SDRAM.."_TRCD__")
end

this:SDRAMEn(1)
this:SDRAMEn(2)
++*/

#endif /* _FMC_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
