/*=========================================================================*//**
@file    i2c_flags.h

@author  Daniel Zorychta

@brief   This driver support I2C peripherals.

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

#ifndef _I2C_FLAGS_H_
#define _I2C_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > I2C",
               function() this:LoadFile("arch/arch_flags.h") end)

this.PortExist = function(this, devNo)
    local I2C_PER = {}
    I2C_PER["STM32F410C8xx"] = 3
    I2C_PER["STM32F410TBxx"] = 2
    I2C_PER["STM32F410T8xx"] = 2
    I2C_PER["STM32F410CBxx"] = 3
    I2C_PER["STM32F410R8xx"] = 3
    I2C_PER["STM32F410RBxx"] = 3
    I2C_PER["STM32F405RGxx"] = 3
    I2C_PER["STM32F405OGxx"] = 3
    I2C_PER["STM32F405VGxx"] = 3
    I2C_PER["STM32F415OGxx"] = 3
    I2C_PER["STM32F415RGxx"] = 3
    I2C_PER["STM32F415ZGxx"] = 3
    I2C_PER["STM32F405OExx"] = 3
    I2C_PER["STM32F405ZGxx"] = 3
    I2C_PER["STM32F415VGxx"] = 3
    I2C_PER["STM32F469BIxx"] = 3
    I2C_PER["STM32F469BGxx"] = 3
    I2C_PER["STM32F479NIxx"] = 3
    I2C_PER["STM32F479ZIxx"] = 3
    I2C_PER["STM32F469AIxx"] = 3
    I2C_PER["STM32F479IGxx"] = 3
    I2C_PER["STM32F479ZGxx"] = 3
    I2C_PER["STM32F469NIxx"] = 3
    I2C_PER["STM32F479BIxx"] = 3
    I2C_PER["STM32F469ZGxx"] = 3
    I2C_PER["STM32F479AIxx"] = 3
    I2C_PER["STM32F469ZExx"] = 3
    I2C_PER["STM32F479VIxx"] = 3
    I2C_PER["STM32F469VExx"] = 3
    I2C_PER["STM32F469IIxx"] = 3
    I2C_PER["STM32F469NExx"] = 3
    I2C_PER["STM32F469IExx"] = 3
    I2C_PER["STM32F479VGxx"] = 3
    I2C_PER["STM32F469ZIxx"] = 3
    I2C_PER["STM32F469AGxx"] = 3
    I2C_PER["STM32F479IIxx"] = 3
    I2C_PER["STM32F469BExx"] = 3
    I2C_PER["STM32F479NGxx"] = 3
    I2C_PER["STM32F469VIxx"] = 3
    I2C_PER["STM32F469NGxx"] = 3
    I2C_PER["STM32F479BGxx"] = 3
    I2C_PER["STM32F469AExx"] = 3
    I2C_PER["STM32F469VGxx"] = 3
    I2C_PER["STM32F469IGxx"] = 3
    I2C_PER["STM32F479AGxx"] = 3
    I2C_PER["STM32F411RExx"] = 3
    I2C_PER["STM32F411CCxx"] = 3
    I2C_PER["STM32F411VCxx"] = 3
    I2C_PER["STM32F411VExx"] = 3
    I2C_PER["STM32F411CExx"] = 3
    I2C_PER["STM32F411RCxx"] = 3
    I2C_PER["STM32F407VExx"] = 3
    I2C_PER["STM32F407ZGxx"] = 3
    I2C_PER["STM32F407IExx"] = 3
    I2C_PER["STM32F417IGxx"] = 3
    I2C_PER["STM32F417ZExx"] = 3
    I2C_PER["STM32F417VExx"] = 3
    I2C_PER["STM32F407IGxx"] = 3
    I2C_PER["STM32F417VGxx"] = 3
    I2C_PER["STM32F417IExx"] = 3
    I2C_PER["STM32F407VGxx"] = 3
    I2C_PER["STM32F407ZExx"] = 3
    I2C_PER["STM32F417ZGxx"] = 3
    I2C_PER["STM32F401VDxx"] = 3
    I2C_PER["STM32F401RDxx"] = 3
    I2C_PER["STM32F401RExx"] = 3
    I2C_PER["STM32F401CDxx"] = 3
    I2C_PER["STM32F401CExx"] = 3
    I2C_PER["STM32F401VBxx"] = 3
    I2C_PER["STM32F401VExx"] = 3
    I2C_PER["STM32F401CCxx"] = 3
    I2C_PER["STM32F401RCxx"] = 3
    I2C_PER["STM32F401RBxx"] = 3
    I2C_PER["STM32F401VCxx"] = 3
    I2C_PER["STM32F401CBxx"] = 3
    I2C_PER["STM32F446VCxx"] = 4
    I2C_PER["STM32F446RExx"] = 4
    I2C_PER["STM32F446MCxx"] = 4
    I2C_PER["STM32F446MExx"] = 4
    I2C_PER["STM32F446ZExx"] = 4
    I2C_PER["STM32F446RCxx"] = 4
    I2C_PER["STM32F446ZCxx"] = 4
    I2C_PER["STM32F446VExx"] = 4
    I2C_PER["STM32F427IIxx"] = 3
    I2C_PER["STM32F427ZGxx"] = 3
    I2C_PER["STM32F437IGxx"] = 3
    I2C_PER["STM32F437ZGxx"] = 3
    I2C_PER["STM32F427VGxx"] = 3
    I2C_PER["STM32F437VIxx"] = 3
    I2C_PER["STM32F427AGxx"] = 3
    I2C_PER["STM32F427VIxx"] = 3
    I2C_PER["STM32F437ZIxx"] = 3
    I2C_PER["STM32F437AIxx"] = 2
    I2C_PER["STM32F427ZIxx"] = 3
    I2C_PER["STM32F427AIxx"] = 3
    I2C_PER["STM32F437VGxx"] = 3
    I2C_PER["STM32F437IIxx"] = 3
    I2C_PER["STM32F427IGxx"] = 3
    I2C_PER["STM32F412VExx"] = 4
    I2C_PER["STM32F412ZGxx"] = 4
    I2C_PER["STM32F412CExx"] = 4
    I2C_PER["STM32F412RGxx"] = 4
    I2C_PER["STM32F412ZExx"] = 4
    I2C_PER["STM32F412RExx"] = 4
    I2C_PER["STM32F412VGxx"] = 4
    I2C_PER["STM32F412CGxx"] = 4
    I2C_PER["STM32F429IExx"] = 3
    I2C_PER["STM32F429AIxx"] = 3
    I2C_PER["STM32F429BGxx"] = 3
    I2C_PER["STM32F429VGxx"] = 3
    I2C_PER["STM32F429VExx"] = 3
    I2C_PER["STM32F439NGxx"] = 3
    I2C_PER["STM32F429IGxx"] = 3
    I2C_PER["STM32F429VIxx"] = 3
    I2C_PER["STM32F429ZIxx"] = 3
    I2C_PER["STM32F429NIxx"] = 3
    I2C_PER["STM32F439IGxx"] = 3
    I2C_PER["STM32F439BGxx"] = 3
    I2C_PER["STM32F439NIxx"] = 3
    I2C_PER["STM32F429IIxx"] = 3
    I2C_PER["STM32F429BExx"] = 3
    I2C_PER["STM32F429AGxx"] = 3
    I2C_PER["STM32F439ZIxx"] = 3
    I2C_PER["STM32F439BIxx"] = 3
    I2C_PER["STM32F429ZGxx"] = 3
    I2C_PER["STM32F429NExx"] = 2
    I2C_PER["STM32F429BIxx"] = 3
    I2C_PER["STM32F439VIxx"] = 3
    I2C_PER["STM32F439VGxx"] = 3
    I2C_PER["STM32F429NGxx"] = 3
    I2C_PER["STM32F429ZExx"] = 3
    I2C_PER["STM32F439IIxx"] = 3
    I2C_PER["STM32F439AIxx"] = 3
    I2C_PER["STM32F439ZGxx"] = 3
    I2C_PER["STM32F413ZGxx"] = 3
    I2C_PER["STM32F423ZHxx"] = 3
    I2C_PER["STM32F413CHxx"] = 3
    I2C_PER["STM32F413VHxx"] = 3
    I2C_PER["STM32F413MGxx"] = 3
    I2C_PER["STM32F423MHxx"] = 3
    I2C_PER["STM32F413ZHxx"] = 3
    I2C_PER["STM32F423VHxx"] = 3
    I2C_PER["STM32F413RHxx"] = 3
    I2C_PER["STM32F423RHxx"] = 3
    I2C_PER["STM32F423CHxx"] = 3
    I2C_PER["STM32F413CGxx"] = 3
    I2C_PER["STM32F413MHxx"] = 3
    I2C_PER["STM32F413RGxx"] = 3
    I2C_PER["STM32F413VGxx"] = 3

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
    this:AddWidget("Spinbox", 1e3, 400e3, "SCL frequency [Hz]")
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
    this:AddWidget("Spinbox", 1e3, 400e3, "SCL frequency [Hz]")
    uC.AddPriorityItems(this)
end
--*/
#define __I2C2_SCL_FREQ__ 100000


/*--
if this:PortExist(3) then
    this:AddExtraWidget("Label", "Label3", "\nI2C3", -1, "bold")
    this:AddExtraWidget("Void", "Void3")

    this:AddWidget("Combobox", "Use DMA")
    this:AddItem("No", "_NO_")
    this:AddItem("Yes", "_YES_")
end
--*/
#define __I2C3_USE_DMA__ _YES_

/*--
if this:PortExist(3) then
    this:AddWidget("Spinbox", 1e3, 400e3, "SCL frequency [Hz]")
    uC.AddPriorityItems(this)
end
--*/
#define __I2C3_SCL_FREQ__ 100000

#endif /* _I2C_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
