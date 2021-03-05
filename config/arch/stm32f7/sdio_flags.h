/*==============================================================================
File     sdio_flags.h

Author   Daniel Zorychta

Brief    SD Card Interface Driver

         Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

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
this:AddItem("SDIO_CK generated on rising edge", "SDMMC_CLOCK_EDGE_RISING")
this:AddItem("SDIO_CK generated on falling edge", "SDMMC_CLOCK_EDGE_FALLING")
--*/
#define __SDIO_CFG_CLOCK_EDGE__ SDMMC_CLOCK_EDGE_RISING

/*--
this:AddWidget("Combobox", "Hardware flow control")
this:AddItem("Disable", "SDMMC_HARDWARE_FLOW_CONTROL_DISABLE")
this:AddItem("Enable", "SDMMC_HARDWARE_FLOW_CONTROL_ENABLE")
--*/
#define __SDIO_CFG_HW_FLOW_CTRL__ SDMMC_HARDWARE_FLOW_CONTROL_ENABLE

/*--
this:AddWidget("Combobox", "Bus wide")
this:AddItem("1-bit wide bus", "SDMMC_BUS_WIDE_1B")
this:AddItem("4-bit wide bus", "SDMMC_BUS_WIDE_4B")
--*/
#define __SDIO_CFG_BUS_WIDE__ SDMMC_BUS_WIDE_4B

/*--
this:AddWidget("Spinbox", 1, 256, "Clock divide factor")
--*/
#define __SDIO_CFG_CKDIV__ 1

/*--
this:AddWidget("Spinbox", 100, 2000, "Card operation timeout [ms]")
--*/
#define __SDIO_CFG_CARD_TIMEOUT__ 500

/*--
this:AddWidget("Combobox", "Power save")
this:AddItem("Disable", "SDMMC_CLOCK_POWER_SAVE_DISABLE")
this:AddItem("Enable", "SDMMC_CLOCK_POWER_SAVE_ENABLE")
--*/
#define __SDIO_CFG_PWRSAVE__ SDMMC_CLOCK_POWER_SAVE_DISABLE

/*--
this:AddWidget("Combobox", "Driver mode")
this:AddItem("Polling", "_SDIO_MODE_POLLING")
this:AddItem("IRQ", "_SDIO_MODE_IRQ")
this:AddItem("DMA", "_SDIO_MODE_DMA")
--*/
#define __SDIO_CFG_MODE__ _SDIO_MODE_IRQ

/*--
this:AddWidget("Combobox", "IRQ priority")
uC.AddPriorityItems(this, false)
--*/
#define __SDIO_IRQ_PRIORITY__ __CPU_DEFAULT_IRQ_PRIORITY__

#endif /* _SDIO_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
