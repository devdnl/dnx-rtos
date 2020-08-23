/*==============================================================================
File    irq_flags.h

Author  Daniel Zorychta

Brief   This driver support external interrupts (EXTI).

        Copyright (C) 2018 Daniel Zorychta <daniel.zorychta@gmail.com>

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
 * NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as suffix
 *       and prefix) are exported to the single configuration file
 *       (by using Configtool) when entire project configuration is exported.
 *       All other flag definitions and statements are ignored.
 */

#ifndef _IRQ_FLAGS_H_
#define _IRQ_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > IRQ",
               function() this:LoadFile("arch/arch_flags.h") end)

this:AddExtraWidget("Label", "LabelLine", "Line")
this:AddExtraWidget("Label", "LabelMode", "Trigger")

this.AddIrqMode = function(this)
    this:AddItem("Disabled", "_IRQ_MODE_DISABLED")
    this:AddItem("Trigger on falling edge", "_IRQ_MODE_FALLING_EDGE")
    this:AddItem("Trigger on rising edge", "_IRQ_MODE_RISING_EDGE")
    this:AddItem("Trigger on both edges", "_IRQ_MODE_FALLING_AND_RISING_EDGE")
end
++*/

/*
 * Interrupts Default Configuration
 * 0: IRQ DISABLED                              (_IRQ_MODE_DISABLED)
 * 1: TRIGGER ON FALLING EDGE                   (_IRQ_MODE_FALLING_EDGE)
 * 2: TRIGGER ON RISING EDGE                    (_IRQ_MODE_RISING_EDGE)
 * 3: TRIGGER ON FALLING AND RISING EDGE        (_IRQ_MODE_FALLING_AND_RISING_EDGE)
 */
/*--
this:AddWidget("Combobox", "IRQ 0")
this:AddIrqMode()
--*/
#define __IRQ_LINE_0_MODE__ _IRQ_MODE_DISABLED

/*--
this:AddWidget("Combobox", "IRQ 1")
this:AddIrqMode()
--*/
#define __IRQ_LINE_1_MODE__ _IRQ_MODE_DISABLED

/*--
this:AddWidget("Combobox", "IRQ 2")
this:AddIrqMode()
--*/
#define __IRQ_LINE_2_MODE__ _IRQ_MODE_DISABLED

/*--
this:AddWidget("Combobox", "IRQ 3")
this:AddIrqMode()
--*/
#define __IRQ_LINE_3_MODE__ _IRQ_MODE_DISABLED

/*--
this:AddWidget("Combobox", "IRQ 4")
this:AddIrqMode()
--*/
#define __IRQ_LINE_4_MODE__ _IRQ_MODE_DISABLED

/*--
this:AddWidget("Combobox", "IRQ 5")
this:AddIrqMode()
--*/
#define __IRQ_LINE_5_MODE__ _IRQ_MODE_DISABLED

/*--
this:AddWidget("Combobox", "IRQ 6")
this:AddIrqMode()
--*/
#define __IRQ_LINE_6_MODE__ _IRQ_MODE_DISABLED

/*--
this:AddWidget("Combobox", "IRQ 7")
this:AddIrqMode()
--*/
#define __IRQ_LINE_7_MODE__ _IRQ_MODE_DISABLED

/*--
this:AddWidget("Combobox", "IRQ 8")
this:AddIrqMode()
--*/
#define __IRQ_LINE_8_MODE__ _IRQ_MODE_DISABLED

/*--
this:AddWidget("Combobox", "IRQ 9")
this:AddIrqMode()
--*/
#define __IRQ_LINE_9_MODE__ _IRQ_MODE_DISABLED

/*--
this:AddWidget("Combobox", "IRQ 10")
this:AddIrqMode()
--*/
#define __IRQ_LINE_10_MODE__ _IRQ_MODE_DISABLED

/*--
this:AddWidget("Combobox", "IRQ 11")
this:AddIrqMode()
--*/
#define __IRQ_LINE_11_MODE__ _IRQ_MODE_DISABLED

/*--
this:AddWidget("Combobox", "IRQ 12")
this:AddIrqMode()
--*/
#define __IRQ_LINE_12_MODE__ _IRQ_MODE_DISABLED

/*--
this:AddWidget("Combobox", "IRQ 13")
this:AddIrqMode()
--*/
#define __IRQ_LINE_13_MODE__ _IRQ_MODE_DISABLED

/*--
this:AddWidget("Combobox", "IRQ 14")
this:AddIrqMode()
--*/
#define __IRQ_LINE_14_MODE__ _IRQ_MODE_DISABLED

/*--
this:AddWidget("Combobox", "IRQ 15")
this:AddIrqMode()
--*/
#define __IRQ_LINE_15_MODE__ _IRQ_MODE_DISABLED

#endif /* _IRQ_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
