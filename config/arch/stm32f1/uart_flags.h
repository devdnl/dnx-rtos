/*=========================================================================*//**
@file    uart_flags.h

@author  Daniel Zorychta

@brief   UART module configuration flags.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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
    UART_PER["STM32F100C8xx"] = 3
    UART_PER["STM32F100RBxx"] = 3
    UART_PER["STM32F100RExx"] = 5
    UART_PER["STM32F100VBxx"] = 3
    UART_PER["STM32F100ZCxx"] = 5
    UART_PER["STM32F101C8xx"] = 3
    UART_PER["STM32F101CBxx"] = 3
    UART_PER["STM32F101R8xx"] = 3
    UART_PER["STM32F101RBxx"] = 3
    UART_PER["STM32F101V8xx"] = 3
    UART_PER["STM32F101VBxx"] = 3
    UART_PER["STM32F103C6xx"] = 2
    UART_PER["STM32F103C8xx"] = 3
    UART_PER["STM32F103CBxx"] = 3
    UART_PER["STM32F103R6xx"] = 2
    UART_PER["STM32F103R8xx"] = 3
    UART_PER["STM32F103RBxx"] = 3
    UART_PER["STM32F103RCxx"] = 5
    UART_PER["STM32F103RDxx"] = 5
    UART_PER["STM32F103RExx"] = 5
    UART_PER["STM32F103RGxx"] = 5
    UART_PER["STM32F103T8xx"] = 2
    UART_PER["STM32F103TBxx"] = 2
    UART_PER["STM32F103V8xx"] = 3
    UART_PER["STM32F103VBxx"] = 3
    UART_PER["STM32F103VCxx"] = 5
    UART_PER["STM32F103VDxx"] = 5
    UART_PER["STM32F103VExx"] = 5
    UART_PER["STM32F103VGxx"] = 5
    UART_PER["STM32F103ZCxx"] = 5
    UART_PER["STM32F103ZExx"] = 5
    UART_PER["STM32F103ZGxx"] = 5
    UART_PER["STM32F105RBxx"] = 5
    UART_PER["STM32F105RCxx"] = 5
    UART_PER["STM32F105VBxx"] = 5
    UART_PER["STM32F105VCxx"] = 5
    UART_PER["STM32F107RCxx"] = 5
    UART_PER["STM32F107VBxx"] = 5
    UART_PER["STM32F107VCxx"] = 5

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
#define __UART_IRQ_PRIORITY__ __CPU_DEFAULT_IRQ_PRIORITY__

#endif /* _UART_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
