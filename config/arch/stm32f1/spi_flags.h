/*=========================================================================*//**
@file    spi_flags.h

@author  Daniel Zorychta

@brief   SPI module configuration flags.

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

#ifndef _SPI_FLAGS_H_
#define _SPI_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > SPI",
               function() this:LoadFile("arch/arch_flags.h") end)

this.PortExist = function(this, devNo)
    local SPI_PER = {}
    SPI_PER["STM32F100C8xx"] = 2
    SPI_PER["STM32F100RBxx"] = 2
    SPI_PER["STM32F100RExx"] = 3
    SPI_PER["STM32F100VBxx"] = 2
    SPI_PER["STM32F100ZCxx"] = 3
    SPI_PER["STM32F101C8xx"] = 2
    SPI_PER["STM32F101CBxx"] = 2
    SPI_PER["STM32F101R8xx"] = 2
    SPI_PER["STM32F101RBxx"] = 2
    SPI_PER["STM32F101V8xx"] = 2
    SPI_PER["STM32F101VBxx"] = 2
    SPI_PER["STM32F103C6xx"] = 1
    SPI_PER["STM32F103C8xx"] = 2
    SPI_PER["STM32F103CBxx"] = 2
    SPI_PER["STM32F103R6xx"] = 1
    SPI_PER["STM32F103R8xx"] = 2
    SPI_PER["STM32F103RBxx"] = 2
    SPI_PER["STM32F103RCxx"] = 3
    SPI_PER["STM32F103RDxx"] = 3
    SPI_PER["STM32F103RExx"] = 3
    SPI_PER["STM32F103RGxx"] = 3
    SPI_PER["STM32F103T8xx"] = 1
    SPI_PER["STM32F103TBxx"] = 1
    SPI_PER["STM32F103V8xx"] = 2
    SPI_PER["STM32F103VBxx"] = 2
    SPI_PER["STM32F103VCxx"] = 3
    SPI_PER["STM32F103VDxx"] = 3
    SPI_PER["STM32F103VExx"] = 3
    SPI_PER["STM32F103VGxx"] = 3
    SPI_PER["STM32F103ZCxx"] = 3
    SPI_PER["STM32F103ZExx"] = 3
    SPI_PER["STM32F103ZGxx"] = 3
    SPI_PER["STM32F105RBxx"] = 3
    SPI_PER["STM32F105RCxx"] = 3
    SPI_PER["STM32F105VBxx"] = 3
    SPI_PER["STM32F105VCxx"] = 3
    SPI_PER["STM32F107RCxx"] = 3
    SPI_PER["STM32F107VBxx"] = 3
    SPI_PER["STM32F107VCxx"] = 3

    return SPI_PER[uC.NAME] >= devNo
end
++*/

/*--
this:AddExtraWidget("Label", "LabelDefaults", "Defaults", -1, "bold")
this:AddExtraWidget("Void", "VoidDefaults")
++*/
/*--
this:AddWidget("Editline", false, "Flush Byte")
--*/
#define __SPI_DEFAULT_FLUSH_BYTE__ 0xFF

/*--
this:AddWidget("Combobox", "Clock divider")
this:AddItem("PCLK / 2", "SPI_CLK_DIV__2")
this:AddItem("PCLK / 4", "SPI_CLK_DIV__4")
this:AddItem("PCLK / 8", "SPI_CLK_DIV__8")
this:AddItem("PCLK / 16", "SPI_CLK_DIV__16")
this:AddItem("PCLK / 32", "SPI_CLK_DIV__32")
this:AddItem("PCLK / 64", "SPI_CLK_DIV__64")
this:AddItem("PCLK / 128", "SPI_CLK_DIV__128")
this:AddItem("PCLK / 256", "SPI_CLK_DIV__256")
--*/
#define __SPI_DEFAULT_CLK_DIV__ SPI_CLK_DIV__4

/*--
this:AddWidget("Combobox", "SPI mode")
this:AddItem("Mode 0: SCK Low at idle, capture on rising edge (CPOL = 0; CPHA = 0)", "SPI_MODE__0")
this:AddItem("Mode 1: SCK Low at idle, capture on falling edge (CPOL = 0; CPHA = 1)", "SPI_MODE__1")
this:AddItem("Mode 2: SCK High at idle, capture on falling edge (CPOL = 1; CPHA = 0)", "SPI_MODE__2")
this:AddItem("Mode 3: SCK High at idle, capture on rising edge (CPOL = 1; CPHA = 1)", "SPI_MODE__3")
--*/
#define __SPI_DEFAULT_MODE__ SPI_MODE__0

/*--
this:AddWidget("Combobox", "Bit order")
this:AddItem("LSb first", "_NO_")
this:AddItem("MSb first", "_YES_")
--*/
#define __SPI_DEFAULT_MSB_FIRST__ _YES_



/*--
if this:PortExist(1) then
    this:AddExtraWidget("Label", "LabelSPI1", "\nSPI1", -1, "bold")
    this:AddExtraWidget("Void", "VoidSPI1")

    this:AddWidget("Combobox", "Use DMA")
    this:AddItem("No", "_NO_")
    this:AddItem("Yes", "_YES_")
end
--*/
#define __SPI_SPI1_USE_DMA__ _YES_

/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "IRQ priority")
    uC.AddPriorityItems(this)
end
--*/
#define __SPI_SPI1_IRQ_PRIORITY__ __CPU_IRQ_USER_PRIORITY__



/*--
if this:PortExist(2) then
    this:AddExtraWidget("Label", "LabelSPI2", "\nSPI2", -1, "bold")
    this:AddExtraWidget("Void", "VoidSPI2")

    this:AddWidget("Combobox", "Use DMA")
    this:AddItem("No", "_NO_")
    this:AddItem("Yes", "_YES_")
end
--*/
#define __SPI_SPI2_USE_DMA__ _NO_

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "IRQ priority")
    uC.AddPriorityItems(this)
end
--*/
#define __SPI_SPI2_IRQ_PRIORITY__ __CPU_IRQ_USER_PRIORITY__



/*--
if this:PortExist(3) then
    this:AddExtraWidget("Label", "LabelSPI3", "\nSPI3", -1, "bold")
    this:AddExtraWidget("Void", "VoidSPI3")

    this:AddWidget("Combobox", "Use DMA")
    this:AddItem("No", "_NO_")
    this:AddItem("Yes", "_YES_")
end
--*/
#define __SPI_SPI3_USE_DMA__ _YES_

/*--
if this:PortExist(3) then
    this:AddWidget("Combobox", "IRQ priority")
    uC.AddPriorityItems(this)
end
--*/
#define __SPI_SPI3_IRQ_PRIORITY__ __CPU_IRQ_USER_PRIORITY__

#endif /* _SPI_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
