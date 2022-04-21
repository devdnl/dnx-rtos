/*==============================================================================
File    sdmmc_flags.h

Author  Daniel Zorychta

Brief   SD/MMC driver

        Copyright (C) 2022 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _SDMMC_FLAGS_H_
#define _SDMMC_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > SDMMC",
               function() this:LoadFile("arch/arch_flags.h") end)
++*/

/*--
this:AddExtraWidget("Label", "LABEL_SDMMC1", "SDMMC1", -1, "bold")
this:AddExtraWidget("Void", "VoidSDMMC1")
++*/
/*--
this:AddWidget("Combobox", "Clock polarity")
this:AddItem("SDIO_CK generated on rising edge", "SDMMC_CLOCK_EDGE_RISING")
this:AddItem("SDIO_CK generated on falling edge", "SDMMC_CLOCK_EDGE_FALLING")
--*/
#define __SDMMC_CFG_SDMMC1_CLOCK_EDGE__ SDMMC_CLOCK_EDGE_RISING

/*--
this:AddWidget("Combobox", "Hardware flow control")
this:AddItem("Disable", "SDMMC_HARDWARE_FLOW_CONTROL_DISABLE")
this:AddItem("Enable", "SDMMC_HARDWARE_FLOW_CONTROL_ENABLE")
--*/
#define __SDMMC_CFG_SDMMC1_HW_FLOW_CTRL__ SDMMC_HARDWARE_FLOW_CONTROL_ENABLE

/*--
this:AddWidget("Combobox", "Bus wide")
this:AddItem("1-bit wide bus", "SDMMC_BUS_WIDE_1B")
this:AddItem("4-bit wide bus", "SDMMC_BUS_WIDE_4B")
this:AddItem("8-bit wide bus", "SDMMC_BUS_WIDE_8B")
--*/
#define __SDMMC_CFG_SDMMC1_BUS_WIDE__ SDMMC_BUS_WIDE_8B

/*--
this:AddWidget("Spinbox", 1, 256, "Clock divide factor")
--*/
#define __SDMMC_CFG_SDMMC1_CKDIV__ 1

/*--
this:AddWidget("Spinbox", 100, 2000, "Card operation timeout [ms]")
--*/
#define __SDMMC_CFG_SDMMC1_TIMEOUT__ 500

/*--
this:AddWidget("Combobox", "Power save")
this:AddItem("Disable", "SDMMC_CLOCK_POWER_SAVE_DISABLE")
this:AddItem("Enable", "SDMMC_CLOCK_POWER_SAVE_ENABLE")
--*/
#define __SDMMC_CFG_SDMMC1_PWRSAVE__ SDMMC_CLOCK_POWER_SAVE_DISABLE

/*--
this:AddWidget("Combobox", "Device type")
this:AddItem("SD Card", "_SDMMC_TYPE_SDCARD")
this:AddItem("MMC/eMMC", "_SDMMC_TYPE_MMC")
--*/
#define __SDMMC_CFG_SDMMC1_TYPE__ _SDMMC_TYPE_MMC

/*--
this:AddWidget("Combobox", "Driver mode")
this:AddItem("Polling", "_SDMMC_MODE_POLLING")
this:AddItem("IRQ", "_SDMMC_MODE_IRQ")
this:AddItem("DMA", "_SDMMC_MODE_DMA")
--*/
#define __SDMMC_CFG_SDMMC1_MODE__ _SDMMC_MODE_DMA

/*--
this:AddWidget("Combobox", "Speed mode")
this:AddItem("Default", "SDMMC_SPEED_MODE_DEFAULT")
this:AddItem("Auto", "SDMMC_SPEED_MODE_AUTO")
this:AddItem("High", "SDMMC_SPEED_MODE_HIGH")
this:AddItem("DDR (MMC only)", "SDMMC_SPEED_MODE_DDR")
this:AddItem("Ultra (SD only)", "SDMMC_SPEED_MODE_ULTRA")
--*/
#define __SDMMC_CFG_SDMMC1_SPEED_MODE__ SDMMC_SPEED_MODE_DDR

/*--
this:AddWidget("Combobox", "IRQ priority")
uC.AddPriorityItems(this, false)
--*/
#define __SDMMC_CFG_SDMMC1_IRQ_PRIORITY__ __CPU_DEFAULT_IRQ_PRIORITY__


/*--
this:AddExtraWidget("Label", "LABEL_SDMMC2", "\nSDMMC2", -1, "bold")
this:AddExtraWidget("Void", "VoidSDMMC2")
++*/
/*--
this:AddWidget("Combobox", "Clock polarity")
this:AddItem("SDIO_CK generated on rising edge", "SDMMC_CLOCK_EDGE_RISING")
this:AddItem("SDIO_CK generated on falling edge", "SDMMC_CLOCK_EDGE_FALLING")
--*/
#define __SDMMC_CFG_SDMMC2_CLOCK_EDGE__ SDMMC_CLOCK_EDGE_RISING

/*--
this:AddWidget("Combobox", "Hardware flow control")
this:AddItem("Disable", "SDMMC_HARDWARE_FLOW_CONTROL_DISABLE")
this:AddItem("Enable", "SDMMC_HARDWARE_FLOW_CONTROL_ENABLE")
--*/
#define __SDMMC_CFG_SDMMC2_HW_FLOW_CTRL__ SDMMC_HARDWARE_FLOW_CONTROL_ENABLE

/*--
this:AddWidget("Combobox", "Bus wide")
this:AddItem("1-bit wide bus", "SDMMC_BUS_WIDE_1B")
this:AddItem("4-bit wide bus", "SDMMC_BUS_WIDE_4B")
--*/
#define __SDMMC_CFG_SDMMC2_BUS_WIDE__ SDMMC_BUS_WIDE_1B

/*--
this:AddWidget("Spinbox", 1, 256, "Clock divide factor")
--*/
#define __SDMMC_CFG_SDMMC2_CKDIV__ 1

/*--
this:AddWidget("Spinbox", 100, 2000, "Card operation timeout [ms]")
--*/
#define __SDMMC_CFG_SDMMC2_TIMEOUT__ 500

/*--
this:AddWidget("Combobox", "Power save")
this:AddItem("Disable", "SDMMC_CLOCK_POWER_SAVE_DISABLE")
this:AddItem("Enable", "SDMMC_CLOCK_POWER_SAVE_ENABLE")
--*/
#define __SDMMC_CFG_SDMMC2_PWRSAVE__ SDMMC_CLOCK_POWER_SAVE_DISABLE

/*--
this:AddWidget("Combobox", "Device type")
this:AddItem("SD Card", "_SDMMC_TYPE_SDCARD")
this:AddItem("MMC/eMMC", "_SDMMC_TYPE_MMC")
--*/
#define __SDMMC_CFG_SDMMC2_TYPE__ _SDMMC_TYPE_MMC

/*--
this:AddWidget("Combobox", "Driver mode")
this:AddItem("Polling", "_SDMMC_MODE_POLLING")
this:AddItem("IRQ", "_SDMMC_MODE_IRQ")
this:AddItem("DMA", "_SDMMC_MODE_DMA")
--*/
#define __SDMMC_CFG_SDMMC2_MODE__ _SDMMC_MODE_IRQ

/*--
this:AddWidget("Combobox", "Speed mode")
this:AddItem("Default", "SDMMC_SPEED_MODE_DEFAULT")
this:AddItem("Auto", "SDMMC_SPEED_MODE_AUTO")
this:AddItem("High", "SDMMC_SPEED_MODE_HIGH")
this:AddItem("DDR (MMC only)", "SDMMC_SPEED_MODE_DDR")
this:AddItem("Ultra (SD only)", "SDMMC_SPEED_MODE_ULTRA")
--*/
#define __SDMMC_CFG_SDMMC2_SPEED_MODE__ SDMMC_SPEED_MODE_DEFAULT

/*--
this:AddWidget("Combobox", "IRQ priority")
uC.AddPriorityItems(this, false)
--*/
#define __SDMMC_CFG_SDMMC2_IRQ_PRIORITY__ __CPU_DEFAULT_IRQ_PRIORITY__

#endif /* _SDMMC_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
