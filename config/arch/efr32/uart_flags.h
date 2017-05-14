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
    UART_PER["EFR32MG12P132F1024GL125"] = 4
    UART_PER["EFR32MG12P132F1024GM48" ] = 4
    UART_PER["EFR32MG12P232F1024GL125"] = 4
    UART_PER["EFR32MG12P232F1024GM48" ] = 2
    UART_PER["EFR32MG12P332F1024GL125"] = 4
    UART_PER["EFR32MG12P332F1024GM48" ] = 4
    UART_PER["EFR32MG12P432F1024GL125"] = 4
    UART_PER["EFR32MG12P432F1024GM48" ] = 4
    UART_PER["EFR32MG12P433F1024GL125"] = 4
    UART_PER["EFR32MG12P433F1024GM48"]  = 4
    UART_PER["EFR32MG1B132F256GM32"]    = 2
    UART_PER["EFR32MG1B132F256GM48"]    = 2
    UART_PER["EFR32MG1B232F256GM32"]    = 2
    UART_PER["EFR32MG1B232F256GM48"]    = 2
    UART_PER["EFR32MG1B632F256GM32"]    = 1
    UART_PER["EFR32MG1B732F256GM32"]    = 1
    UART_PER["EFR32MG1P132F256GM32"]    = 2
    UART_PER["EFR32MG1P132F256GM48"]    = 2
    UART_PER["EFR32MG1P133F256GM48"]    = 2
    UART_PER["EFR32MG1P232F256GM32"]    = 2
    UART_PER["EFR32MG1P232F256GM48"]    = 2
    UART_PER["EFR32MG1P233F256GM48"]    = 2
    UART_PER["EFR32MG1P632F256GM32"]    = 1
    UART_PER["EFR32MG1P732F256GM32"]    = 1
    UART_PER["EFR32MG1V132F256GM32"]    = 2
    UART_PER["EFR32MG1V132F256GM48"]    = 2

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
this:AddExtraWidget("Label", "LabelPinLoc", "\nPin locations", -1, "bold")
this:AddExtraWidget("Void", "VoidPinLoc")
++*/
/*--
this:AddWidget("Spinbox", 0, 31, "Tx pin")
--*/
#define __UART_TX_LOC__ 0

/*--
this:AddWidget("Spinbox", 0, 31, "Rx pin")
--*/
#define __UART_RX_LOC__ 0

/*--
this:AddWidget("Spinbox", 0, 31, "CTS pin")
--*/
#define __UART_CTS_LOC__ 0

/*--
this:AddWidget("Spinbox", 0, 31, "RTS pin")
--*/
#define __UART_RTS_LOC__ 0

/*--
this:AddExtraWidget("Label", "LabelPrio", "\nPriorities", -1, "bold")
this:AddExtraWidget("Void", "VoidPrio")
++*/
/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "USART0 IRQ priority")
    uC.AddPriorityItems(this)
end
--*/
#define __UART_UART0_IRQ_PRIORITY__ __CPU_DEFAULT_IRQ_PRIORITY__

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "USART1 IRQ priority")
    uC.AddPriorityItems(this)
end
--*/
#define __UART_UART1_IRQ_PRIORITY__ __CPU_DEFAULT_IRQ_PRIORITY__

/*--
if this:PortExist(3) then
    this:AddWidget("Combobox", "USART2 IRQ priority")
    uC.AddPriorityItems(this)
end
--*/
#define __UART_UART2_IRQ_PRIORITY__ __CPU_DEFAULT_IRQ_PRIORITY__

/*--
if this:PortExist(4) then
    this:AddWidget("Combobox", "USART3 IRQ priority")
    uC.AddPriorityItems(this)
end
--*/
#define __UART_UART3_IRQ_PRIORITY__ __CPU_DEFAULT_IRQ_PRIORITY__

#endif /* _UART_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
