/*=========================================================================*//**
@file    usbd_flags.h

@author  Daniel Zorychta

@brief   USB-Device module configuration flags.

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

#ifndef _USBD_FLAGS_H_
#define _USBD_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 4, "Home > Microcontroller > USBD",
               function() this:LoadFile("arch/arch_flags.h") end)
++*/

/*--
this:AddWidget("Combobox", "Pullup pin")
for i = 0, 6 do
    local c = string.char(65 + i)
    this:AddItem("P"..c, tostring(i))
end
--*/
#define __USBD_PULLUP_PORT_INDEX__ 0

/*--
this:AddWidget("Combobox")
for i = 0, 15 do
    this:AddItem(tostring(i), tostring(i))
end
--*/
#define __USBD_PULLUP_PIN_INDEX__ 0

/*--
this:AddWidget("Checkbox", "Active low")
this:SetToolTip("Pull-up pin activated by low level state.")
--*/
#define __USBD_PULLUP_NEGATIVE__ _YES_

/*--
this:AddWidget("Combobox", "Endpoint 0 size")
this:AddItem("8",  "8")
this:AddItem("16", "16")
this:AddItem("32", "32")
this:AddItem("64", "64")
this:AddExtraWidget("Void", "VoidEP0SZ1")
this:AddExtraWidget("Void", "VoidEP0SZ2")
--*/
#define __USBD_ENDPOINT0_SIZE__ 8

/*--
this:AddWidget("Combobox", "IRQ priority")
uC.AddPriorityItems(this)
this:AddExtraWidget("Void", "VoidPrio1")
this:AddExtraWidget("Void", "VoidPrio2")
--*/
#define __USBD_IRQ_PRIORITY__ __CPU_IRQ_USER_PRIORITY__

#endif /* _USBD_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
