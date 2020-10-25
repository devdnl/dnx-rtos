/*=========================================================================*//**
@file    uart_flags.h

@author  Daniel Zorychta

@brief   UART module configuration flags.

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

#ifndef _UART_FLAGS_H_
#define _UART_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > UART",
               function() this:LoadFile("arch/arch_flags.h") end)

this.PortExist = function(this, devNo)
    local UART_PER = {}
    UART_PER["STM32H723VExx" ] = 8
    UART_PER["STM32H723VGxx" ] = 8
    UART_PER["STM32H723ZExx" ] = 8
    UART_PER["STM32H723ZGxx" ] = 8
    UART_PER["STM32H725AExx" ] = 8
    UART_PER["STM32H725AGxx" ] = 8
    UART_PER["STM32H725IExx" ] = 8
    UART_PER["STM32H725IGxx" ] = 8
    UART_PER["STM32H725RExx" ] = 8
    UART_PER["STM32H725RGxx" ] = 8
    UART_PER["STM32H725VExx" ] = 8
    UART_PER["STM32H725VGxx" ] = 8
    UART_PER["STM32H725ZExx" ] = 8
    UART_PER["STM32H725ZGxx" ] = 8
    UART_PER["STM32H730ABxxQ"] = 8
    UART_PER["STM32H730IBxxQ"] = 8
    UART_PER["STM32H730VBxx" ] = 8
    UART_PER["STM32H730ZBxx" ] = 8
    UART_PER["STM32H733VGxx" ] = 8
    UART_PER["STM32H733ZGxx" ] = 8
    UART_PER["STM32H735AGxx" ] = 8
    UART_PER["STM32H735IGxx" ] = 8
    UART_PER["STM32H735RGxx" ] = 8
    UART_PER["STM32H735VGxx" ] = 8
    UART_PER["STM32H735ZGxx" ] = 8
    UART_PER["STM32H742AGxx" ] = 8
    UART_PER["STM32H742BGxx" ] = 8
    UART_PER["STM32H742IGxx" ] = 8
    UART_PER["STM32H742VGxx" ] = 8
    UART_PER["STM32H742XGxx" ] = 8
    UART_PER["STM32H742ZGxx" ] = 8
    UART_PER["STM32H742AIxx" ] = 8
    UART_PER["STM32H742BIxx" ] = 8
    UART_PER["STM32H742IIxx" ] = 8
    UART_PER["STM32H742VIxx" ] = 8
    UART_PER["STM32H742XIxx" ] = 8
    UART_PER["STM32H742ZIxx" ] = 8
    UART_PER["STM32H743AGxx" ] = 8
    UART_PER["STM32H743BGxx" ] = 8
    UART_PER["STM32H743IGxx" ] = 8
    UART_PER["STM32H743VGxx" ] = 8
    UART_PER["STM32H743XGxx" ] = 8
    UART_PER["STM32H743ZGxx" ] = 8
    UART_PER["STM32H743AIxx" ] = 8
    UART_PER["STM32H743BIxx" ] = 8
    UART_PER["STM32H743IIxx" ] = 8
    UART_PER["STM32H743VIxx" ] = 8
    UART_PER["STM32H743XIxx" ] = 8
    UART_PER["STM32H743ZIxx" ] = 8
    UART_PER["STM32H745BGxx" ] = 8
    UART_PER["STM32H745IGxx" ] = 8
    UART_PER["STM32H745XGxx" ] = 8
    UART_PER["STM32H745ZGxx" ] = 8
    UART_PER["STM32H745BIxx" ] = 8
    UART_PER["STM32H745IIxx" ] = 8
    UART_PER["STM32H745XIxx" ] = 8
    UART_PER["STM32H745ZIxx" ] = 8
    UART_PER["STM32H747AGxx" ] = 8
    UART_PER["STM32H747BGxx" ] = 8
    UART_PER["STM32H747IGxx" ] = 8
    UART_PER["STM32H747XGxx" ] = 8
    UART_PER["STM32H747ZGxx" ] = 8
    UART_PER["STM32H747AIxx" ] = 8
    UART_PER["STM32H747BIxx" ] = 8
    UART_PER["STM32H747IIxx" ] = 8
    UART_PER["STM32H747XIxx" ] = 8
    UART_PER["STM32H747ZIxx" ] = 8
    UART_PER["STM32H750IBxx" ] = 8
    UART_PER["STM32H750VBxx" ] = 8
    UART_PER["STM32H750XBxx" ] = 8
    UART_PER["STM32H750ZBxx" ] = 8
    UART_PER["STM32H753AIxx" ] = 8
    UART_PER["STM32H753BIxx" ] = 8
    UART_PER["STM32H753IIxx" ] = 8
    UART_PER["STM32H753VIxx" ] = 8
    UART_PER["STM32H753XIxx" ] = 8
    UART_PER["STM32H753ZIxx" ] = 8
    UART_PER["STM32H755BIxx" ] = 8
    UART_PER["STM32H755IIxx" ] = 8
    UART_PER["STM32H755XIxx" ] = 8
    UART_PER["STM32H755ZIxx" ] = 8
    UART_PER["STM32H757AIxx" ] = 8
    UART_PER["STM32H757BIxx" ] = 8
    UART_PER["STM32H757IIxx" ] = 8
    UART_PER["STM32H757XIxx" ] = 8
    UART_PER["STM32H757ZIxx" ] = 8
    UART_PER["STM32H7A3AGxxQ"] = 8
    UART_PER["STM32H7A3IGxx" ] = 8
    UART_PER["STM32H7A3IGxxQ"] = 8
    UART_PER["STM32H7A3LGxxQ"] = 8
    UART_PER["STM32H7A3NGxx" ] = 8
    UART_PER["STM32H7A3QGxxQ"] = 8
    UART_PER["STM32H7A3RGxx" ] = 8
    UART_PER["STM32H7A3VGxx" ] = 8
    UART_PER["STM32H7A3VGxxQ"] = 8
    UART_PER["STM32H7A3ZGxx" ] = 8
    UART_PER["STM32H7A3ZGxxQ"] = 8
    UART_PER["STM32H7A3AIxxQ"] = 8
    UART_PER["STM32H7A3IIxx" ] = 8
    UART_PER["STM32H7A3IIxxQ"] = 8
    UART_PER["STM32H7A3LIxxQ"] = 8
    UART_PER["STM32H7A3NIxx" ] = 8
    UART_PER["STM32H7A3QIxxQ"] = 8
    UART_PER["STM32H7A3RIxx" ] = 8
    UART_PER["STM32H7A3VIxx" ] = 8
    UART_PER["STM32H7A3VIxxQ"] = 8
    UART_PER["STM32H7A3ZIxx" ] = 8
    UART_PER["STM32H7A3ZIxxQ"] = 8
    UART_PER["STM32H7B0ABxxQ"] = 8
    UART_PER["STM32H7B0IBKxQ"] = 8
    UART_PER["STM32H7B0IBxx" ] = 8
    UART_PER["STM32H7B0RBxx" ] = 8
    UART_PER["STM32H7B0VBxx" ] = 8
    UART_PER["STM32H7B0ZBxx" ] = 8
    UART_PER["STM32H7B3AIxxQ"] = 8
    UART_PER["STM32H7B3IIxx" ] = 8
    UART_PER["STM32H7B3IIxxQ"] = 8
    UART_PER["STM32H7B3LIxxQ"] = 8
    UART_PER["STM32H7B3NIxx" ] = 8
    UART_PER["STM32H7B3QIxxQ"] = 8
    UART_PER["STM32H7B3RIxx" ] = 8
    UART_PER["STM32H7B3VIxx" ] = 8
    UART_PER["STM32H7B3VIxxQ"] = 8
    UART_PER["STM32H7B3ZIxx" ] = 8
    UART_PER["STM32H7B3ZIxxQ"] = 8

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
