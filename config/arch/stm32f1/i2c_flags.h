/*=========================================================================*//**
@file    i2c_flags.h

@author  Daniel Zorychta

@brief   This driver support I2C peripherals.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes FreeRTOS without
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

this.PortExist = function(this, devNo)
    local I2C_PER = {}
    I2C_PER["STM32F100C8xx"] = 2
    I2C_PER["STM32F100RBxx"] = 2
    I2C_PER["STM32F100RExx"] = 2
    I2C_PER["STM32F100VBxx"] = 2
    I2C_PER["STM32F100ZCxx"] = 2
    I2C_PER["STM32F101C8xx"] = 2
    I2C_PER["STM32F101CBxx"] = 2
    I2C_PER["STM32F101R8xx"] = 2
    I2C_PER["STM32F101RBxx"] = 2
    I2C_PER["STM32F101V8xx"] = 2
    I2C_PER["STM32F101VBxx"] = 2
    I2C_PER["STM32F103C6xx"] = 1
    I2C_PER["STM32F103C8xx"] = 2
    I2C_PER["STM32F103CBxx"] = 2
    I2C_PER["STM32F103R6xx"] = 1
    I2C_PER["STM32F103R8xx"] = 2
    I2C_PER["STM32F103RBxx"] = 2
    I2C_PER["STM32F103RCxx"] = 2
    I2C_PER["STM32F103RDxx"] = 2
    I2C_PER["STM32F103RExx"] = 2
    I2C_PER["STM32F103RGxx"] = 2
    I2C_PER["STM32F103T8xx"] = 1
    I2C_PER["STM32F103TBxx"] = 1
    I2C_PER["STM32F103V8xx"] = 2
    I2C_PER["STM32F103VBxx"] = 2
    I2C_PER["STM32F103VCxx"] = 2
    I2C_PER["STM32F103VDxx"] = 2
    I2C_PER["STM32F103VExx"] = 2
    I2C_PER["STM32F103VGxx"] = 2
    I2C_PER["STM32F103ZCxx"] = 2
    I2C_PER["STM32F103ZExx"] = 2
    I2C_PER["STM32F103ZGxx"] = 2
    I2C_PER["STM32F105RBxx"] = 2
    I2C_PER["STM32F105RCxx"] = 2
    I2C_PER["STM32F105VBxx"] = 2
    I2C_PER["STM32F105VCxx"] = 2
    I2C_PER["STM32F107RCxx"] = 1
    I2C_PER["STM32F107VBxx"] = 1
    I2C_PER["STM32F107VCxx"] = 1

    return I2C_PER[uC.NAME] >= devNo
end
++*/

/*--
if this:PortExist(1) then
    this:AddExtraWidget("Label", "Label1", "I2C1", -1, "bold")
    this:AddExtraWidget("Void", "Void1")

    this:AddWidget("Combobox", "Use DMA")
    this:AddItem("No", "_NO_")
    this:AddItem("Yes", "_YES_")
end
--*/
#define __I2C1_USE_DMA__ _YES_

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "IRQ priority")
    uC.AddPriorityItems(this)
end
--*/
#define __I2C1_IRQ_PRIO__ __CPU_IRQ_USER_PRIORITY__

/*--
if this:PortExist(1) then
    this:AddWidget("Spinbox", 1e3, 100e3, "SCL frequency [Hz]")
    uC.AddPriorityItems(this)
end
--*/
#define __I2C1_SCL_FREQ__ 100000


/*--
if this:PortExist(2) then
    this:AddExtraWidget("Label", "Label2", "\nI2C2", -1, "bold")
    this:AddExtraWidget("Void", "Void2")

    this:AddWidget("Combobox", "Use DMA")
    this:AddItem("No", "_NO_")
    this:AddItem("Yes", "_YES_")
end
--*/
#define __I2C2_USE_DMA__ _YES_

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "IRQ priority")
    uC.AddPriorityItems(this)
end
--*/
#define __I2C2_IRQ_PRIO__ __CPU_IRQ_USER_PRIORITY__

/*--
if this:PortExist(2) then
    this:AddWidget("Spinbox", 1e3, 100e3, "SCL frequency [Hz]")
    uC.AddPriorityItems(this)
end
--*/
#define __I2C2_SCL_FREQ__ 100000

#endif /* _I2C_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
