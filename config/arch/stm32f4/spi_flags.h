/*=========================================================================*//**
@file    spi_flags.h

@author  Daniel Zorychta

@brief   SPI module configuration flags.

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

#ifndef _SPI_FLAGS_H_
#define _SPI_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > SPI",
               function() this:LoadFile("arch/arch_flags.h") end)

this.PortExist = function(this, devNo)
    local SPI_PER = {}
    SPI_PER["STM32F401CBxx"] = 3
    SPI_PER["STM32F401CCxx"] = 3
    SPI_PER["STM32F401CDxx"] = 3
    SPI_PER["STM32F401CExx"] = 3
    SPI_PER["STM32F401RBxx"] = 3
    SPI_PER["STM32F401RCxx"] = 3
    SPI_PER["STM32F401RDxx"] = 3
    SPI_PER["STM32F401RExx"] = 3
    SPI_PER["STM32F401VBxx"] = 4
    SPI_PER["STM32F401VCxx"] = 4
    SPI_PER["STM32F401VDxx"] = 4
    SPI_PER["STM32F401VExx"] = 4
    SPI_PER["STM32F405OExx"] = 3
    SPI_PER["STM32F405OGxx"] = 3
    SPI_PER["STM32F405RGxx"] = 3
    SPI_PER["STM32F405VGxx"] = 3
    SPI_PER["STM32F405ZGxx"] = 3
    SPI_PER["STM32F407IExx"] = 3
    SPI_PER["STM32F407IGxx"] = 3
    SPI_PER["STM32F407VExx"] = 3
    SPI_PER["STM32F407VGxx"] = 3
    SPI_PER["STM32F407ZExx"] = 3
    SPI_PER["STM32F407ZGxx"] = 3
    SPI_PER["STM32F410C8xx"] = 3
    SPI_PER["STM32F410CBxx"] = 3
    SPI_PER["STM32F410R8xx"] = 3
    SPI_PER["STM32F410RBxx"] = 3
    SPI_PER["STM32F410T8xx"] = 1
    SPI_PER["STM32F410TBxx"] = 1
    SPI_PER["STM32F411CCxx"] = 5
    SPI_PER["STM32F411CExx"] = 5
    SPI_PER["STM32F411RCxx"] = 5
    SPI_PER["STM32F411RExx"] = 5
    SPI_PER["STM32F411VCxx"] = 5
    SPI_PER["STM32F411VExx"] = 5
    SPI_PER["STM32F412CExx"] = 5
    SPI_PER["STM32F412CGxx"] = 5
    SPI_PER["STM32F412RExx"] = 5
    SPI_PER["STM32F412RGxx"] = 5
    SPI_PER["STM32F412VExx"] = 5
    SPI_PER["STM32F412VGxx"] = 5
    SPI_PER["STM32F412ZExx"] = 5
    SPI_PER["STM32F412ZGxx"] = 5
    SPI_PER["STM32F413CGxx"] = 5
    SPI_PER["STM32F413CHxx"] = 5
    SPI_PER["STM32F413MGxx"] = 5
    SPI_PER["STM32F413MHxx"] = 5
    SPI_PER["STM32F413RGxx"] = 5
    SPI_PER["STM32F413RHxx"] = 5
    SPI_PER["STM32F413VGxx"] = 5
    SPI_PER["STM32F413VHxx"] = 5
    SPI_PER["STM32F413ZGxx"] = 5
    SPI_PER["STM32F413ZHxx"] = 5
    SPI_PER["STM32F415OGxx"] = 3
    SPI_PER["STM32F415RGxx"] = 3
    SPI_PER["STM32F415VGxx"] = 3
    SPI_PER["STM32F415ZGxx"] = 3
    SPI_PER["STM32F417IExx"] = 3
    SPI_PER["STM32F417IGxx"] = 3
    SPI_PER["STM32F417VExx"] = 3
    SPI_PER["STM32F417VGxx"] = 3
    SPI_PER["STM32F417ZExx"] = 3
    SPI_PER["STM32F417ZGxx"] = 3
    SPI_PER["STM32F423CHxx"] = 5
    SPI_PER["STM32F423MHxx"] = 5
    SPI_PER["STM32F423RHxx"] = 5
    SPI_PER["STM32F423VHxx"] = 5
    SPI_PER["STM32F423ZHxx"] = 5
    SPI_PER["STM32F427AGxx"] = 6
    SPI_PER["STM32F427AIxx"] = 6
    SPI_PER["STM32F427IGxx"] = 6
    SPI_PER["STM32F427IIxx"] = 6
    SPI_PER["STM32F427VGxx"] = 4
    SPI_PER["STM32F427VIxx"] = 4
    SPI_PER["STM32F427ZGxx"] = 6
    SPI_PER["STM32F427ZIxx"] = 6
    SPI_PER["STM32F429AGxx"] = 6
    SPI_PER["STM32F429AIxx"] = 6
    SPI_PER["STM32F429BExx"] = 6
    SPI_PER["STM32F429BGxx"] = 6
    SPI_PER["STM32F429BIxx"] = 6
    SPI_PER["STM32F429IExx"] = 6
    SPI_PER["STM32F429IGxx"] = 6
    SPI_PER["STM32F429IIxx"] = 6
    SPI_PER["STM32F429NExx"] = 6
    SPI_PER["STM32F429NGxx"] = 6
    SPI_PER["STM32F429NIxx"] = 6
    SPI_PER["STM32F429VExx"] = 4
    SPI_PER["STM32F429VGxx"] = 4
    SPI_PER["STM32F429VIxx"] = 4
    SPI_PER["STM32F429ZExx"] = 6
    SPI_PER["STM32F429ZGxx"] = 6
    SPI_PER["STM32F429ZIxx"] = 6
    SPI_PER["STM32F437AIxx"] = 6
    SPI_PER["STM32F437IGxx"] = 6
    SPI_PER["STM32F437IIxx"] = 6
    SPI_PER["STM32F437VGxx"] = 4
    SPI_PER["STM32F437VIxx"] = 4
    SPI_PER["STM32F437ZGxx"] = 6
    SPI_PER["STM32F437ZIxx"] = 6
    SPI_PER["STM32F439AIxx"] = 6
    SPI_PER["STM32F439BGxx"] = 6
    SPI_PER["STM32F439BIxx"] = 6
    SPI_PER["STM32F439IGxx"] = 6
    SPI_PER["STM32F439IIxx"] = 6
    SPI_PER["STM32F439NGxx"] = 6
    SPI_PER["STM32F439NIxx"] = 6
    SPI_PER["STM32F439VGxx"] = 4
    SPI_PER["STM32F439VIxx"] = 4
    SPI_PER["STM32F439ZGxx"] = 6
    SPI_PER["STM32F439ZIxx"] = 6
    SPI_PER["STM32F446MCxx"] = 4
    SPI_PER["STM32F446MExx"] = 4
    SPI_PER["STM32F446RCxx"] = 4
    SPI_PER["STM32F446RExx"] = 4
    SPI_PER["STM32F446VCxx"] = 4
    SPI_PER["STM32F446VExx"] = 4
    SPI_PER["STM32F446ZCxx"] = 4
    SPI_PER["STM32F446ZExx"] = 4
    SPI_PER["STM32F469AExx"] = 6
    SPI_PER["STM32F469AGxx"] = 6
    SPI_PER["STM32F469AIxx"] = 6
    SPI_PER["STM32F469BExx"] = 6
    SPI_PER["STM32F469BGxx"] = 6
    SPI_PER["STM32F469BIxx"] = 6
    SPI_PER["STM32F469IExx"] = 6
    SPI_PER["STM32F469IGxx"] = 6
    SPI_PER["STM32F469IIxx"] = 6
    SPI_PER["STM32F469NExx"] = 6
    SPI_PER["STM32F469NGxx"] = 6
    SPI_PER["STM32F469NIxx"] = 6
    SPI_PER["STM32F469VExx"] = 4
    SPI_PER["STM32F469VGxx"] = 4
    SPI_PER["STM32F469VIxx"] = 4
    SPI_PER["STM32F469ZExx"] = 4
    SPI_PER["STM32F469ZGxx"] = 4
    SPI_PER["STM32F469ZIxx"] = 4
    SPI_PER["STM32F479AGxx"] = 6
    SPI_PER["STM32F479AIxx"] = 6
    SPI_PER["STM32F479BGxx"] = 6
    SPI_PER["STM32F479BIxx"] = 6
    SPI_PER["STM32F479IGxx"] = 6
    SPI_PER["STM32F479IIxx"] = 6
    SPI_PER["STM32F479NGxx"] = 6
    SPI_PER["STM32F479NIxx"] = 6
    SPI_PER["STM32F479VGxx"] = 4
    SPI_PER["STM32F479VIxx"] = 4
    SPI_PER["STM32F479ZGxx"] = 4
    SPI_PER["STM32F479ZIxx"] = 4

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
#define __SPI_SPI1_USE_DMA__ _NO_

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
