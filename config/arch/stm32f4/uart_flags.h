/*=========================================================================*//**
@file    uart_flags.h

@author  Daniel Zorychta

@brief   UART module configuration flags.

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

#ifndef _UART_FLAGS_H_
#define _UART_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > UART",
               function() this:LoadFile("arch/arch_flags.h") end)

this.PortExist = function(this, devNo)
    local UART_PER = {}
    UART_PER["STM32F401CBxx"] = 3
    UART_PER["STM32F401RBxx"] = 3
    UART_PER["STM32F401VBxx"] = 3
    UART_PER["STM32F401CCxx"] = 3
    UART_PER["STM32F401RCxx"] = 3
    UART_PER["STM32F401VCxx"] = 3
    UART_PER["STM32F401CDxx"] = 3
    UART_PER["STM32F401RDxx"] = 3
    UART_PER["STM32F401VDxx"] = 3
    UART_PER["STM32F401CExx"] = 3
    UART_PER["STM32F401RExx"] = 3
    UART_PER["STM32F401VExx"] = 3
    UART_PER["STM32F405RGxx"] = 6
    UART_PER["STM32F405OExx"] = 6
    UART_PER["STM32F405OGxx"] = 6
    UART_PER["STM32F405VGxx"] = 6
    UART_PER["STM32F405ZGxx"] = 6
    UART_PER["STM32F407VExx"] = 6
    UART_PER["STM32F407VGxx"] = 6
    UART_PER["STM32F407ZExx"] = 6
    UART_PER["STM32F407ZGxx"] = 6
    UART_PER["STM32F407IExx"] = 6
    UART_PER["STM32F407IGxx"] = 6
    UART_PER["STM32F410T8Yx"] = 2
    UART_PER["STM32F410TBYx"] = 2
    UART_PER["STM32F410C8Ux"] = 3
    UART_PER["STM32F410CBUx"] = 3
    UART_PER["STM32F410C8Tx"] = 3
    UART_PER["STM32F410CBTx"] = 3
    UART_PER["STM32F410R8Tx"] = 3
    UART_PER["STM32F410RBTx"] = 3
    UART_PER["STM32F410R8Ix"] = 3
    UART_PER["STM32F410RBIx"] = 3
    UART_PER["STM32F411CCxx"] = 3
    UART_PER["STM32F411RCxx"] = 3
    UART_PER["STM32F411VCxx"] = 3
    UART_PER["STM32F411CExx"] = 3
    UART_PER["STM32F411RExx"] = 3
    UART_PER["STM32F411VExx"] = 3
    UART_PER["STM32F412CExx"] = 3
    UART_PER["STM32F412CGxx"] = 3
    UART_PER["STM32F412RExx"] = 4
    UART_PER["STM32F412RGxx"] = 4
    UART_PER["STM32F412VExx"] = 4
    UART_PER["STM32F412VGxx"] = 4
    UART_PER["STM32F412ZExx"] = 4
    UART_PER["STM32F412ZGxx"] = 4
    UART_PER["STM32F413CGxx"] = 6
    UART_PER["STM32F413RGxx"] = 7
    UART_PER["STM32F413MGxx"] = 7
    UART_PER["STM32F413VGxx"] = 10
    UART_PER["STM32F413ZGxx"] = 10
    UART_PER["STM32F413CHxx"] = 6
    UART_PER["STM32F413RHxx"] = 7
    UART_PER["STM32F413MHxx"] = 7
    UART_PER["STM32F413VHxx"] = 10
    UART_PER["STM32F413ZHxx"] = 10
    UART_PER["STM32F415RGxx"] = 6
    UART_PER["STM32F415OGxx"] = 6
    UART_PER["STM32F415VGxx"] = 6
    UART_PER["STM32F415ZGxx"] = 6
    UART_PER["STM32F417VExx"] = 6
    UART_PER["STM32F417VGxx"] = 6
    UART_PER["STM32F417ZExx"] = 6
    UART_PER["STM32F417ZGxx"] = 6
    UART_PER["STM32F417IExx"] = 6
    UART_PER["STM32F417IGxx"] = 6
    UART_PER["STM32F423CHxx"] = 6
    UART_PER["STM32F423RHxx"] = 7
    UART_PER["STM32F423MHxx"] = 7
    UART_PER["STM32F423VHxx"] = 10
    UART_PER["STM32F423ZHxx"] = 10
    UART_PER["STM32F427VGxx"] = 8
    UART_PER["STM32F427VIxx"] = 8
    UART_PER["STM32F427ZGxx"] = 8
    UART_PER["STM32F427ZIxx"] = 8
    UART_PER["STM32F427AGxx"] = 8
    UART_PER["STM32F427AIxx"] = 8
    UART_PER["STM32F427IGxx"] = 8
    UART_PER["STM32F427IIxx"] = 8
    UART_PER["STM32F429VExx"] = 8
    UART_PER["STM32F429VGxx"] = 8
    UART_PER["STM32F429VIxx"] = 8
    UART_PER["STM32F429ZExx"] = 8
    UART_PER["STM32F429ZGxx"] = 8
    UART_PER["STM32F429ZIxx"] = 8
    UART_PER["STM32F429AGxx"] = 8
    UART_PER["STM32F429AIxx"] = 8
    UART_PER["STM32F429IExx"] = 8
    UART_PER["STM32F429IGxx"] = 8
    UART_PER["STM32F429IIxx"] = 8
    UART_PER["STM32F429BExx"] = 8
    UART_PER["STM32F429BGxx"] = 8
    UART_PER["STM32F429BIxx"] = 8
    UART_PER["STM32F429NExx"] = 8
    UART_PER["STM32F429NGxx"] = 8
    UART_PER["STM32F429NIxx"] = 8
    UART_PER["STM32F437VGxx"] = 8
    UART_PER["STM32F437VIxx"] = 8
    UART_PER["STM32F437ZGxx"] = 8
    UART_PER["STM32F437ZIxx"] = 8
    UART_PER["STM32F437IGxx"] = 8
    UART_PER["STM32F437IIxx"] = 8
    UART_PER["STM32F437AIxx"] = 8
    UART_PER["STM32F439VGxx"] = 8
    UART_PER["STM32F439VIxx"] = 8
    UART_PER["STM32F439ZGxx"] = 8
    UART_PER["STM32F439ZIxx"] = 8
    UART_PER["STM32F439IGxx"] = 8
    UART_PER["STM32F439IIxx"] = 8
    UART_PER["STM32F439BGxx"] = 8
    UART_PER["STM32F439BIxx"] = 8
    UART_PER["STM32F439NGxx"] = 8
    UART_PER["STM32F439NIxx"] = 8
    UART_PER["STM32F439AIxx"] = 8
    UART_PER["STM32F446MCxx"] = 8
    UART_PER["STM32F446RCxx"] = 8
    UART_PER["STM32F446VCxx"] = 8
    UART_PER["STM32F446ZCxx"] = 8
    UART_PER["STM32F446MExx"] = 8
    UART_PER["STM32F446RExx"] = 8
    UART_PER["STM32F446VExx"] = 8
    UART_PER["STM32F446ZExx"] = 8
    UART_PER["STM32F469VExx"] = 7
    UART_PER["STM32F469VGxx"] = 7
    UART_PER["STM32F469VIxx"] = 7
    UART_PER["STM32F469ZExx"] = 7
    UART_PER["STM32F469ZGxx"] = 7
    UART_PER["STM32F469ZIxx"] = 7
    UART_PER["STM32F469AExx"] = 8
    UART_PER["STM32F469AGxx"] = 8
    UART_PER["STM32F469AIxx"] = 8
    UART_PER["STM32F469IExx"] = 8
    UART_PER["STM32F469IGxx"] = 8
    UART_PER["STM32F469IIxx"] = 8
    UART_PER["STM32F469BExx"] = 8
    UART_PER["STM32F469BGxx"] = 8
    UART_PER["STM32F469BIxx"] = 8
    UART_PER["STM32F469NExx"] = 8
    UART_PER["STM32F469NGxx"] = 8
    UART_PER["STM32F469NIxx"] = 8
    UART_PER["STM32F479VGxx"] = 7
    UART_PER["STM32F479VIxx"] = 7
    UART_PER["STM32F479ZGxx"] = 7
    UART_PER["STM32F479ZIxx"] = 7
    UART_PER["STM32F479AGxx"] = 8
    UART_PER["STM32F479AIxx"] = 8
    UART_PER["STM32F479IGxx"] = 8
    UART_PER["STM32F479IIxx"] = 8
    UART_PER["STM32F479BGxx"] = 8
    UART_PER["STM32F479BIxx"] = 8
    UART_PER["STM32F479NGxx"] = 8
    UART_PER["STM32F479NIxx"] = 8

    return UART_PER[uC.NAME] >= devNo
end
++*/

/*--
this:AddExtraWidget("Label", "LabelDefaults", "Defaults", -1, "bold")
this:AddExtraWidget("Void", "VoidDefaults")
++*/
/*--
this:AddWidget("Spinbox", 16, 1024, "Rx buffer length [B]")
--*/
#define __UART_RX_BUFFER_LEN__ 128

/*--
this:AddWidget("Combobox", "Parity bit")
this:AddItem("Off", "UART_PARITY__OFF")
this:AddItem("Odd", "UART_PARITY__ODD")
this:AddItem("Even", "UART_PARITY__EVEN")
--*/
#define __UART_DEFAULT_PARITY__ UART_PARITY__OFF

/*--
this:AddWidget("Combobox", "Stop bit")
this:AddItem("1", "UART_STOP_BIT__1")
this:AddItem("2", "UART_STOP_BIT__2")
--*/
#define __UART_DEFAULT_STOP_BITS__ UART_STOP_BIT__1

/*--
this:AddWidget("Combobox", "LIN mode enable")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
--*/
#define __UART_DEFAULT_LIN_MODE_ENABLE__ _NO_

/*--
this:AddWidget("Combobox", "LIN break length")
this:AddItem("10 bits", "UART_LIN_BREAK__10_BITS")
this:AddItem("11 bits", "UART_LIN_BREAK__11_BITS")
--*/
#define __UART_DEFAULT_LIN_BREAK_LEN__ UART_LIN_BREAK__10_BITS

/*--
this:AddWidget("Combobox", "Tx line enable")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
--*/
#define __UART_DEFAULT_TX_ENABLE__ _YES_

/*--
this:AddWidget("Combobox", "Rx line enable")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
--*/
#define __UART_DEFAULT_RX_ENABLE__ _YES_

/*--
this:AddWidget("Combobox", "HW flow control")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
--*/
#define __UART_DEFAULT_HW_FLOW_CTRL__ _NO_

/*--
this:AddWidget("Combobox", "Single wire mode")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
--*/
#define __UART_DEFAULT_SINGLE_WIRE_MODE__ _NO_

/*--
this:AddWidget("Textbox", "Baud [bps]")
this:AddItem("110", "")
this:AddItem("150", "")
this:AddItem("300", "")
this:AddItem("1200", "")
this:AddItem("2400", "")
this:AddItem("4800", "")
this:AddItem("9600", "")
this:AddItem("19200", "")
this:AddItem("38400", "")
this:AddItem("57600", "")
this:AddItem("115200", "")
this:AddItem("230400", "")
this:AddItem("460800", "")
this:AddItem("921600", "")
this:AddItem("1000000", "")
this:AddItem("2000000", "")
this:AddItem("4000000", "")
--*/
#define __UART_DEFAULT_BAUD__ 115200

/*--
this:AddWidget("Combobox", "IRQ priority")
uC.AddPriorityItems(this, false)
--*/
#define __UART_IRQ_PRIORITY__ 13

#endif /* _UART_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
