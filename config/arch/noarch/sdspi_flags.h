/*=========================================================================*//**
@file    sdspi_flags.h

@author  Daniel Zorychta

@brief   SDSPI module configuration flags.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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


*//*==========================================================================*/

/*
 * NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as suffix
 *       and prefix) are exported to the single configuration file
 *       (by using Configtool) when entire project configuration is exported.
 *       All other flag definitions and statements are ignored.
 */

#ifndef _SDSPI_FLAGS_H_
#define _SDSPI_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > SDSPI",
               function() this:LoadFile("arch/arch_flags.h") end)
++*/

/*--
this:AddExtraWidget("Label", "LabelGeneral", "General", -1, "bold")
this:AddExtraWidget("Void", "VoidGeneral")
++*/
/*--
this:AddWidget("Combobox", "Number of cards")
this:AddItem("One SD card", "1")
this:AddItem("Two SD cards", "2")
--*/
#define __SDSPI_NUMBER_OF_CARDS__ 1


/*--
this:AddExtraWidget("Label", "LabelCard1", "\nCard 1", -1, "bold")
this:AddExtraWidget("Void", "VoidCard1")
++*/
/*--
this:AddWidget("Spinbox", 100, 1000, "Timeout [ms]")
--*/
#define __SDSPI_CARD0_TIMEOUT__ 500

/*--
this:AddWidget("Editline", true, "Device path")
--*/
#define __SDSPI_CARD0_FILE__ "/dev/spi_sda"


/*--
this:AddExtraWidget("Label", "LabelCard2", "\nCard 2", -1, "bold")
this:AddExtraWidget("Void", "VoidCard2")
++*/
/*--
this:AddWidget("Spinbox", 100, 1000, "Timeout [ms]")
--*/
#define __SDSPI_CARD1_TIMEOUT__ 500

/*--
this:AddWidget("Editline", true, "Device path")
--*/
#define __SDSPI_CARD1_FILE__ "/dev/spi_sdb"

/*--
this:AddExtraWidget("Void", "Void1")
this:AddExtraWidget("Label", "LabelNote1", "\nNotes:")

this:AddExtraWidget("Void", "Void2")
this:AddExtraWidget("Label", "LabelNote2", "- clock frequency and SPI mode can be configured in the SPI module")

this:AddExtraWidget("Void", "Void3")
this:AddExtraWidget("Label", "LabelNote4", "- make sure that the MISO pin is set to input pulled-up mode\n  or is connected to external pull-up resistor.")
++*/

#endif /* _SDSPI_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
