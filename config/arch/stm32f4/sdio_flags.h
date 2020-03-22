/*==============================================================================
File     sdio_flags.h

Author   Daniel Zorychta

Brief    SD Card Interface Driver

         Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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

==============================================================================*/

/*
 * NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as prefix
 *       and suffix) are exported to the single configuration file
 *       (by using Configtool) when entire project configuration is exported.
 *       All other flag definitions and statements are ignored.
 */

#ifndef _SDIO_FLAGS_H_
#define _SDIO_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > SDIO",
               function() this:LoadFile("arch/arch_flags.h") end)
++*/

/*--
this:AddWidget("Combobox", "Clock polarity")
this:AddItem("SDIO_CK generated on rising edge", "0")
this:AddItem("SDIO_CK generated on falling edge", "SDIO_CLKCR_NEGEDGE")
--*/
#define __SDIO_CFG_NEGEDGE__ 0

/*--
this:AddWidget("Combobox", "Bus wide")
this:AddItem("1-bit wide bus", "(0 << SDIO_CLKCR_WIDBUS_Pos)")
this:AddItem("4-bit wide bus", "(1 << SDIO_CLKCR_WIDBUS_Pos)")
this:AddItem("8-bit wide bus", "(2 << SDIO_CLKCR_WIDBUS_Pos)")
--*/
#define __SDIO_CFG_BUS_WIDE__ (0 << SDIO_CLKCR_WIDBUS_Pos)

/*--
this:AddWidget("Spinbox", 1, 256, "Clock divide factor")
--*/
#define __SDIO_CFG_CKDIV__ 48

/*--
this:AddWidget("Spinbox", 100, 2000, "Card operation timeout [ms]")
--*/
#define __SDIO_CFG_CARD_TIMEOUT__ 500

/*--
this:AddWidget("Combobox", "Power save")
this:AddItem("Disable", "0")
this:AddItem("Enable", "SDIO_CLKCR_PWRSAV")
--*/
#define __SDIO_CFG_PWRSAVE__ 0

/*--
this:AddWidget("Combobox", "Use DMA")
this:AddItem("Never", "_SDIO_USE_DMA_NEVER")
this:AddItem("If available", "_SDIO_USE_DMA_IFAVAILABLE")
this:AddItem("Always", "_SDIO_USE_DMA_ALWAYS")
--*/
#define __SDIO_CFG_USEDMA__ _SDIO_USE_DMA_ALWAYS

/*--
this:AddWidget("Combobox", "IRQ priority")
uC.AddPriorityItems(this, false)
--*/
#define __SDIO_IRQ_PRIORITY__ 13

#endif /* _SDIO_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
