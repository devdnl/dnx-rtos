/*=========================================================================*//**
@file    spi_flags.h

@author  Daniel Zorychta

@brief   SPI module configuration flags.

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

#ifndef _SPI_FLAGS_H_
#define _SPI_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > SPI",
               function() this:LoadFile("arch/arch_flags.h") end)

this.PortExist = function(this, devNo)
    local SPI_PER = {}
        SPI_PER["STM32F722ICxx"] = 5
        SPI_PER["STM32F722IExx"] = 5
        SPI_PER["STM32F722RCxx"] = 3
        SPI_PER["STM32F722RExx"] = 3
        SPI_PER["STM32F722VCxx"] = 4
        SPI_PER["STM32F722VExx"] = 4
        SPI_PER["STM32F722ZCxx"] = 5
        SPI_PER["STM32F722ZExx"] = 5
        SPI_PER["STM32F723ICxx"] = 5
        SPI_PER["STM32F723IExx"] = 5
        SPI_PER["STM32F723VCxx"] = 0
        SPI_PER["STM32F723VExx"] = 4
        SPI_PER["STM32F723ZCxx"] = 5
        SPI_PER["STM32F723ZExx"] = 5
        SPI_PER["STM32F730I8xx"] = 5
        SPI_PER["STM32F730R8xx"] = 3
        SPI_PER["STM32F730V8xx"] = 4
        SPI_PER["STM32F730Z8xx"] = 5
        SPI_PER["STM32F732IExx"] = 5
        SPI_PER["STM32F732RExx"] = 3
        SPI_PER["STM32F732VExx"] = 4
        SPI_PER["STM32F732ZExx"] = 5
        SPI_PER["STM32F733IExx"] = 5
        SPI_PER["STM32F733VExx"] = 4
        SPI_PER["STM32F733ZExx"] = 5
        SPI_PER["STM32F745IExx"] = 6
        SPI_PER["STM32F745IGxx"] = 6
        SPI_PER["STM32F745VExx"] = 4
        SPI_PER["STM32F745VGxx"] = 4
        SPI_PER["STM32F745ZExx"] = 6
        SPI_PER["STM32F745ZGxx"] = 6
        SPI_PER["STM32F746BExx"] = 6
        SPI_PER["STM32F746BGxx"] = 6
        SPI_PER["STM32F746IExx"] = 6
        SPI_PER["STM32F746IGxx"] = 6
        SPI_PER["STM32F746NExx"] = 6
        SPI_PER["STM32F746NGxx"] = 6
        SPI_PER["STM32F746VExx"] = 4
        SPI_PER["STM32F746VGxx"] = 4
        SPI_PER["STM32F746ZExx"] = 6
        SPI_PER["STM32F746ZGxx"] = 6
        SPI_PER["STM32F750N8xx"] = 6
        SPI_PER["STM32F750V8xx"] = 4
        SPI_PER["STM32F750Z8xx"] = 6
        SPI_PER["STM32F756BGxx"] = 6
        SPI_PER["STM32F756IGxx"] = 6
        SPI_PER["STM32F756NGxx"] = 6
        SPI_PER["STM32F756VGxx"] = 4
        SPI_PER["STM32F756ZGxx"] = 6
        SPI_PER["STM32F765BGxx"] = 6
        SPI_PER["STM32F765BIxx"] = 6
        SPI_PER["STM32F765IGxx"] = 6
        SPI_PER["STM32F765IIxx"] = 6
        SPI_PER["STM32F765NGxx"] = 6
        SPI_PER["STM32F765NIxx"] = 6
        SPI_PER["STM32F765VGxx"] = 5
        SPI_PER["STM32F765VIxx"] = 5
        SPI_PER["STM32F765ZGxx"] = 6
        SPI_PER["STM32F765ZIxx"] = 6
        SPI_PER["STM32F767BGxx"] = 6
        SPI_PER["STM32F767BIxx"] = 6
        SPI_PER["STM32F767IGxx"] = 6
        SPI_PER["STM32F767IIxx"] = 6
        SPI_PER["STM32F767NGxx"] = 6
        SPI_PER["STM32F767NIxx"] = 6
        SPI_PER["STM32F767VGxx"] = 5
        SPI_PER["STM32F767VIxx"] = 5
        SPI_PER["STM32F767ZGxx"] = 6
        SPI_PER["STM32F767ZIxx"] = 6
        SPI_PER["STM32F769AIxx"] = 5
        SPI_PER["STM32F769BGxx"] = 6
        SPI_PER["STM32F769BIxx"] = 6
        SPI_PER["STM32F769IGxx"] = 6
        SPI_PER["STM32F769IIxx"] = 6
        SPI_PER["STM32F769NGxx"] = 6
        SPI_PER["STM32F769NIxx"] = 6
        SPI_PER["STM32F777BIxx"] = 6
        SPI_PER["STM32F777IIxx"] = 6
        SPI_PER["STM32F777NIxx"] = 6
        SPI_PER["STM32F777VIxx"] = 5
        SPI_PER["STM32F777ZIxx"] = 6
        SPI_PER["STM32F778AIxx"] = 5
        SPI_PER["STM32F779AIxx"] = 5
        SPI_PER["STM32F779BIxx"] = 6
        SPI_PER["STM32F779IIxx"] = 6
        SPI_PER["STM32F779NIxx"] = 6

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
this:AddExtraWidget("Label", "LabelDMA", "\nDMA Configuration", -1, "bold")
this:AddExtraWidget("Void", "VoidDMA")
++*/
/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "SPI1 DMA")
    this:AddItem("No", "_NO_")
    this:AddItem("Yes", "_YES_")
end
--*/
#define __SPI_SPI1_USE_DMA__ _YES_

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "SPI2 DMA")
    this:AddItem("No", "_NO_")
    this:AddItem("Yes", "_YES_")
end
--*/
#define __SPI_SPI2_USE_DMA__ _NO_

/*--
if this:PortExist(3) then
    this:AddWidget("Combobox", "SPI3 DMA")
    this:AddItem("No", "_NO_")
    this:AddItem("Yes", "_YES_")
end
--*/
#define __SPI_SPI3_USE_DMA__ _NO_

/*--
if this:PortExist(4) then
    this:AddWidget("Combobox", "SPI4 DMA")
    this:AddItem("No", "_NO_")
    this:AddItem("Yes", "_YES_")
end
--*/
#define __SPI_SPI4_USE_DMA__ _NO_

/*--
if this:PortExist(5) then
    this:AddWidget("Combobox", "SPI5 DMA")
    this:AddItem("No", "_NO_")
    this:AddItem("Yes", "_YES_")
end
--*/
#define __SPI_SPI5_USE_DMA__ _NO_

/*--
if this:PortExist(6) then
    this:AddWidget("Combobox", "SPI6 DMA")
    this:AddItem("No", "_NO_")
    this:AddItem("Yes", "_YES_")
end
--*/
#define __SPI_SPI6_USE_DMA__ _NO_

#endif /* _SPI_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
