/*=========================================================================*//**
@file    i2c_flags.h

@author  Daniel Zorychta

@brief   This driver support I2C peripherals.

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

#ifndef _I2C_FLAGS_H_
#define _I2C_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > I2C",
               function() this:LoadFile("arch/arch_flags.h") end)
++*/

/*------------------------------------------------------------------------------
this:AddExtraWidget("Label", "Label1A", "I2C1", -1, "bold")
this:AddExtraWidget("Label", "Label1B", "\t\t\t\t\t\t", -1, "bold")
++*/

/*--
this:AddWidget("Spinbox", 1e3, 1000e3, "SCL frequency [Hz]")
--*/
#define __I2C1_SCL_FREQ__ 100000

/*--
this:AddWidget("Combobox", "Analog filter")
this:AddItem("DISABLE", "I2C_CR1_ANFOFF")
this:AddItem("ENABLE", "0")
--*/
#define __I2C1_ANALOG_FILTER_DISABLE__ 0

/*--
this:AddWidget("Combobox", "Digital filter")
this:AddItem("DISABLE", "0")
for i=1,15 do this:AddItem(tostring(i).." clock periods", tostring(i)) end
--*/
#define __I2C1_DIGITAL_FILTER__ 0

/*--
this:AddWidget("Spinbox", 0, 1000, "SCL rising time [ns]")
--*/
#define __I2C1_SCL_RISE_TIME_ns__ 100

/*--
this:AddWidget("Spinbox", 0, 1000, "SCL falling time [ns]")
--*/
#define __I2C1_SCL_FALL_TIME_ns__ 100

/*--
this:AddWidget("Spinbox", 10, 1000, "Data hold time SCL-SDA [ns]")
--*/
#define __I2C1_DATA_HOLD_TIME_ns__ 10

/*--
this:AddWidget("Spinbox", 10, 1000, "Data setup time SCL-SDA [ns]")
--*/
#define __I2C1_DATA_SETUP_TIME_ns__ 10

/*--
this:AddWidget("Combobox", "IRQ priority")
uC.AddPriorityItems(this, false)
--*/
#define __I2C1_IRQ_PRIORITY__ __CPU_DEFAULT_IRQ_PRIORITY__

/*------------------------------------------------------------------------------
this:AddExtraWidget("Label", "Label2A", "\nI2C2", -1, "bold")
this:AddExtraWidget("Label", "Label2B", "\t\t\t\t\t\t", -1, "bold")
++*/

/*--
this:AddWidget("Spinbox", 1e3, 1000e3, "SCL frequency [Hz]")
--*/
#define __I2C2_SCL_FREQ__ 100000

/*--
this:AddWidget("Combobox", "Analog filter")
this:AddItem("DISABLE", "I2C_CR1_ANFOFF")
this:AddItem("ENABLE", "0")
--*/
#define __I2C2_ANALOG_FILTER_DISABLE__ 0

/*--
this:AddWidget("Combobox", "Digital filter")
this:AddItem("DISABLE", "0")
for i=1,15 do this:AddItem(tostring(i).." clock periods", tostring(i)) end
--*/
#define __I2C2_DIGITAL_FILTER__ 0

/*--
this:AddWidget("Spinbox", 0, 1000, "SCL rising time [ns]")
--*/
#define __I2C2_SCL_RISE_TIME_ns__ 100

/*--
this:AddWidget("Spinbox", 0, 1000, "SCL falling time [ns]")
--*/
#define __I2C2_SCL_FALL_TIME_ns__ 100

/*--
this:AddWidget("Spinbox", 10, 1000, "Data hold time SCL-SDA [ns]")
--*/
#define __I2C2_DATA_HOLD_TIME_ns__ 10

/*--
this:AddWidget("Spinbox", 10, 1000, "Data setup time SCL-SDA [ns]")
--*/
#define __I2C2_DATA_SETUP_TIME_ns__ 10

/*--
this:AddWidget("Combobox", "IRQ priority")
uC.AddPriorityItems(this, false)
--*/
#define __I2C2_IRQ_PRIORITY__ __CPU_DEFAULT_IRQ_PRIORITY__


/*------------------------------------------------------------------------------
this:AddExtraWidget("Label", "Label3A", "I2C3", -1, "bold")
this:AddExtraWidget("Label", "Label3B", "\t\t\t\t\t\t", -1, "bold")
++*/

/*--
this:AddWidget("Spinbox", 1e3, 1000e3, "SCL frequency [Hz]")
uC.AddPriorityItems(this)
--*/
#define __I2C3_SCL_FREQ__ 100000

/*--
this:AddWidget("Combobox", "Analog filter")
this:AddItem("DISABLE", "I2C_CR1_ANFOFF")
this:AddItem("ENABLE", "0")
--*/
#define __I2C3_ANALOG_FILTER_DISABLE__ 0

/*--
this:AddWidget("Combobox", "Digital filter")
this:AddItem("DISABLE", "0")
for i=1,15 do this:AddItem(tostring(i).." clock periods", tostring(i)) end
--*/
#define __I2C3_DIGITAL_FILTER__ 0

/*--
this:AddWidget("Spinbox", 0, 1000, "SCL rising time [ns]")
--*/
#define __I2C3_SCL_RISE_TIME_ns__ 100

/*--
this:AddWidget("Spinbox", 0, 1000, "SCL falling time [ns]")
--*/
#define __I2C3_SCL_FALL_TIME_ns__ 100

/*--
this:AddWidget("Spinbox", 10, 1000, "Data hold time SCL-SDA [ns]")
--*/
#define __I2C3_DATA_HOLD_TIME_ns__ 10

/*--
this:AddWidget("Spinbox", 10, 1000, "Data setup time SCL-SDA [ns]")
--*/
#define __I2C3_DATA_SETUP_TIME_ns__ 10

/*--
this:AddWidget("Combobox", "IRQ priority")
uC.AddPriorityItems(this, false)
--*/
#define __I2C3_IRQ_PRIORITY__ __CPU_DEFAULT_IRQ_PRIORITY__


#endif /* _I2C_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
