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
    UART_PER["STM32H723VExx"] = 10
    UART_PER["STM32H723VGxx"] = 10
    UART_PER["STM32H723ZExx"] = 10
    UART_PER["STM32H723ZGxx"] = 10
    UART_PER["STM32H725AExx"] = 10
    UART_PER["STM32H725AGxx"] = 10
    UART_PER["STM32H725IExx"] = 10
    UART_PER["STM32H725IGxx"] = 10
    UART_PER["STM32H725RExx"] = 10
    UART_PER["STM32H725RGxx"] = 10
    UART_PER["STM32H725VExx"] = 10
    UART_PER["STM32H725VGxx"] = 10
    UART_PER["STM32H725ZExx"] = 10
    UART_PER["STM32H725ZGxx"] = 10
    UART_PER["STM32H730ABxxQ"] = 10
    UART_PER["STM32H730IBxxQ"] = 10
    UART_PER["STM32H730VBxx"] = 10
    UART_PER["STM32H730ZBxx"] = 10
    UART_PER["STM32H733VGxx"] = 10
    UART_PER["STM32H733ZGxx"] = 10
    UART_PER["STM32H735AGxx"] = 10
    UART_PER["STM32H735IGxx"] = 10
    UART_PER["STM32H735RGxx"] = 10
    UART_PER["STM32H735VGxx"] = 10
    UART_PER["STM32H735ZGxx"] = 10
    UART_PER["STM32H742AGxx"] = 10
    UART_PER["STM32H742BGxx"] = 10
    UART_PER["STM32H742IGxx"] = 10
    UART_PER["STM32H742VGxx"] = 10
    UART_PER["STM32H742XGxx"] = 10
    UART_PER["STM32H742ZGxx"] = 10
    UART_PER["STM32H742AIxx"] = 10
    UART_PER["STM32H742BIxx"] = 10
    UART_PER["STM32H742IIxx"] = 10
    UART_PER["STM32H742VIxx"] = 10
    UART_PER["STM32H742XIxx"] = 10
    UART_PER["STM32H742ZIxx"] = 10
    UART_PER["STM32H743AGxx"] = 10
    UART_PER["STM32H743BGxx"] = 10
    UART_PER["STM32H743IGxx"] = 10
    UART_PER["STM32H743VGxx"] = 10
    UART_PER["STM32H743XGxx"] = 10
    UART_PER["STM32H743ZGxx"] = 10
    UART_PER["STM32H743AIxx"] = 10
    UART_PER["STM32H743BIxx"] = 10
    UART_PER["STM32H743IIxx"] = 10
    UART_PER["STM32H743VIxx"] = 10
    UART_PER["STM32H743XIxx"] = 10
    UART_PER["STM32H743ZIxx"] = 10
    UART_PER["STM32H745BGxx"] = 10
    UART_PER["STM32H745IGxx"] = 10
    UART_PER["STM32H745XGxx"] = 10
    UART_PER["STM32H745ZGxx"] = 10
    UART_PER["STM32H745BIxx"] = 10
    UART_PER["STM32H745IIxx"] = 10
    UART_PER["STM32H745XIxx"] = 10
    UART_PER["STM32H745ZIxx"] = 10
    UART_PER["STM32H747AGxx"] = 10
    UART_PER["STM32H747BGxx"] = 10
    UART_PER["STM32H747IGxx"] = 10
    UART_PER["STM32H747XGxx"] = 10
    UART_PER["STM32H747ZGxx"] = 10
    UART_PER["STM32H747AIxx"] = 10
    UART_PER["STM32H747BIxx"] = 10
    UART_PER["STM32H747IIxx"] = 10
    UART_PER["STM32H747XIxx"] = 10
    UART_PER["STM32H747ZIxx"] = 10
    UART_PER["STM32H750IBxx"] = 10
    UART_PER["STM32H750VBxx"] = 10
    UART_PER["STM32H750XBxx"] = 10
    UART_PER["STM32H750ZBxx"] = 10
    UART_PER["STM32H753AIxx"] = 10
    UART_PER["STM32H753BIxx"] = 10
    UART_PER["STM32H753IIxx"] = 10
    UART_PER["STM32H753VIxx"] = 10
    UART_PER["STM32H753XIxx"] = 10
    UART_PER["STM32H753ZIxx"] = 10
    UART_PER["STM32H755BIxx"] = 10
    UART_PER["STM32H755IIxx"] = 10
    UART_PER["STM32H755XIxx"] = 10
    UART_PER["STM32H755ZIxx"] = 10
    UART_PER["STM32H757AIxx"] = 10
    UART_PER["STM32H757BIxx"] = 10
    UART_PER["STM32H757IIxx"] = 10
    UART_PER["STM32H757XIxx"] = 10
    UART_PER["STM32H757ZIxx"] = 10
    UART_PER["STM32H7A3AGxxQ"] = 10
    UART_PER["STM32H7A3IGxx"] = 10
    UART_PER["STM32H7A3IGxxQ"] = 10
    UART_PER["STM32H7A3LGxxQ"] = 10
    UART_PER["STM32H7A3NGxx"] = 10
    UART_PER["STM32H7A3QGxxQ"] = 10
    UART_PER["STM32H7A3RGxx"] = 10
    UART_PER["STM32H7A3VGxx"] = 10
    UART_PER["STM32H7A3VGxxQ"] = 10
    UART_PER["STM32H7A3ZGxx"] = 10
    UART_PER["STM32H7A3ZGxxQ"] = 10
    UART_PER["STM32H7A3AIxxQ"] = 10
    UART_PER["STM32H7A3IIxx"] = 10
    UART_PER["STM32H7A3IIxxQ"] = 10
    UART_PER["STM32H7A3LIxxQ"] = 10
    UART_PER["STM32H7A3NIxx"] = 10
    UART_PER["STM32H7A3QIxxQ"] = 10
    UART_PER["STM32H7A3RIxx"] = 10
    UART_PER["STM32H7A3VIxx"] = 10
    UART_PER["STM32H7A3VIxxQ"] = 10
    UART_PER["STM32H7A3ZIxx"] = 10
    UART_PER["STM32H7A3ZIxxQ"] = 10
    UART_PER["STM32H7B0ABxxQ"] = 10
    UART_PER["STM32H7B0IBKxQ"] = 10
    UART_PER["STM32H7B0IBxx"] = 10
    UART_PER["STM32H7B0RBxx"] = 10
    UART_PER["STM32H7B0VBxx"] = 10
    UART_PER["STM32H7B0ZBxx"] = 10
    UART_PER["STM32H7B3AIxxQ"] = 10
    UART_PER["STM32H7B3IIxx"] = 10
    UART_PER["STM32H7B3IIxxQ"] = 10
    UART_PER["STM32H7B3LIxxQ"] = 10
    UART_PER["STM32H7B3NIxx"] = 10
    UART_PER["STM32H7B3QIxxQ"] = 10
    UART_PER["STM32H7B3RIxx"] = 10
    UART_PER["STM32H7B3VIxx"] = 10
    UART_PER["STM32H7B3VIxxQ"] = 10
    UART_PER["STM32H7B3ZIxx"] = 10
    UART_PER["STM32H7B3ZIxxQ"] = 10

    return UART_PER[uC.NAME] >= devNo
end
++*/


/*--
this:AddExtraWidget("Label", "LabelDefaults", "Defaults", -1, "bold")
this:AddExtraWidget("Void", "VoidDefaults")
++*/
/*--
this:AddWidget("Spinbox", 16, 65536, "Rx buffer length [B]")
--*/
#define __UART_RX_BUFFER_LEN__ 128

/*--
this:AddWidget("Spinbox", 16, 65536, "Tx buffer length [B]")
--*/
#define __UART_TX_BUFFER_LEN__ 128

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
if this:PortExist(1) then
this:AddExtraWidget("Label", "LabelIRQDMA1", "\nUART1 IRQ and DMA configuration", -1, "bold")
this:AddExtraWidget("Void", "VoidIRQDMA1")
end
++*/
/*--
if this:PortExist(1) then
this:AddWidget("Combobox", "IRQ priority")
uC.AddPriorityItems(this, false)
end
--*/
#define __UART_UART1_IRQ_PRIORITY__ __CPU_DEFAULT_IRQ_PRIORITY__

/*--
if this:PortExist(1) then
this:AddWidget("Combobox", "DMA mode")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
end
--*/
#define __UART_UART1_DMA_MODE__ _NO_

/*--
if this:PortExist(1) then
this:AddWidget("Textbox", "Rx DMA buffer length [B]")
this:AddItem("32", "")
this:AddItem("64", "")
this:AddItem("128", "")
this:AddItem("256", "")
end
--*/
#define __UART_UART1_DMA_BUF_LEN__ 64


/*--
if this:PortExist(2) then
this:AddExtraWidget("Label", "LabelIRQDMA2", "\nUART2 IRQ and DMA configuration", -1, "bold")
this:AddExtraWidget("Void", "VoidIRQDMA2")
end
++*/
/*--
if this:PortExist(2) then
this:AddWidget("Combobox", "IRQ priority")
uC.AddPriorityItems(this, false)
end
--*/
#define __UART_UART2_IRQ_PRIORITY__ __CPU_DEFAULT_IRQ_PRIORITY__

/*--
if this:PortExist(2) then
this:AddWidget("Combobox", "DMA mode")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
end
--*/
#define __UART_UART2_DMA_MODE__ _NO_

/*--
if this:PortExist(2) then
this:AddWidget("Textbox", "Rx DMA buffer length [B]")
this:AddItem("32", "")
this:AddItem("64", "")
this:AddItem("128", "")
this:AddItem("256", "")
end
--*/
#define __UART_UART2_DMA_BUF_LEN__ 64


/*--
if this:PortExist(3) then
this:AddExtraWidget("Label", "LabelIRQDMA3", "\nUART3 IRQ and DMA configuration", -1, "bold")
this:AddExtraWidget("Void", "VoidIRQDMA3")
end
++*/
/*--
if this:PortExist(3) then
this:AddWidget("Combobox", "IRQ priority")
uC.AddPriorityItems(this, false)
end
--*/
#define __UART_UART3_IRQ_PRIORITY__ __CPU_DEFAULT_IRQ_PRIORITY__

/*--
if this:PortExist(3) then
this:AddWidget("Combobox", "DMA mode")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
end
--*/
#define __UART_UART3_DMA_MODE__ _NO_

/*--
if this:PortExist(3) then
this:AddWidget("Textbox", "Rx DMA buffer length [B]")
this:AddItem("32", "")
this:AddItem("64", "")
this:AddItem("128", "")
this:AddItem("256", "")
end
--*/
#define __UART_UART3_DMA_BUF_LEN__ 64


/*--
if this:PortExist(4) then
this:AddExtraWidget("Label", "LabelIRQDMA4", "\nUART4 IRQ and DMA configuration", -1, "bold")
this:AddExtraWidget("Void", "VoidIRQDMA4")
end
++*/
/*--
if this:PortExist(4) then
this:AddWidget("Combobox", "IRQ priority")
uC.AddPriorityItems(this, false)
end
--*/
#define __UART_UART4_IRQ_PRIORITY__ __CPU_DEFAULT_IRQ_PRIORITY__

/*--
if this:PortExist(4) then
this:AddWidget("Combobox", "DMA mode")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
end
--*/
#define __UART_UART4_DMA_MODE__ _NO_

/*--
if this:PortExist(4) then
this:AddWidget("Textbox", "Rx DMA buffer length [B]")
this:AddItem("32", "")
this:AddItem("64", "")
this:AddItem("128", "")
this:AddItem("256", "")
end
--*/
#define __UART_UART4_DMA_BUF_LEN__ 64


/*--
if this:PortExist(5) then
this:AddExtraWidget("Label", "LabelIRQDMA5", "\nUART5 IRQ and DMA configuration", -1, "bold")
this:AddExtraWidget("Void", "VoidIRQDMA5")
end
++*/
/*--
if this:PortExist(5) then
this:AddWidget("Combobox", "IRQ priority")
uC.AddPriorityItems(this, false)
end
--*/
#define __UART_UART5_IRQ_PRIORITY__ __CPU_DEFAULT_IRQ_PRIORITY__

/*--
if this:PortExist(5) then
this:AddWidget("Combobox", "DMA mode")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
end
--*/
#define __UART_UART5_DMA_MODE__ _NO_

/*--
if this:PortExist(5) then
this:AddWidget("Textbox", "Rx DMA buffer length [B]")
this:AddItem("32", "")
this:AddItem("64", "")
this:AddItem("128", "")
this:AddItem("256", "")
end
--*/
#define __UART_UART5_DMA_BUF_LEN__ 64


/*--
if this:PortExist(6) then
this:AddExtraWidget("Label", "LabelIRQDMA6", "\nUART6 IRQ and DMA configuration", -1, "bold")
this:AddExtraWidget("Void", "VoidIRQDMA6")
end
++*/
/*--
if this:PortExist(6) then
this:AddWidget("Combobox", "IRQ priority")
uC.AddPriorityItems(this, false)
end
--*/
#define __UART_UART6_IRQ_PRIORITY__ __CPU_DEFAULT_IRQ_PRIORITY__

/*--
if this:PortExist(6) then
this:AddWidget("Combobox", "DMA mode")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
end
--*/
#define __UART_UART6_DMA_MODE__ _NO_

/*--
if this:PortExist(6) then
this:AddWidget("Textbox", "Rx DMA buffer length [B]")
this:AddItem("32", "")
this:AddItem("64", "")
this:AddItem("128", "")
this:AddItem("256", "")
end
--*/
#define __UART_UART6_DMA_BUF_LEN__ 64


/*--
if this:PortExist(7) then
this:AddExtraWidget("Label", "LabelIRQDMA7", "\nUART7 IRQ and DMA configuration", -1, "bold")
this:AddExtraWidget("Void", "VoidIRQDMA7")
end
++*/
/*--
if this:PortExist(7) then
this:AddWidget("Combobox", "IRQ priority")
uC.AddPriorityItems(this, false)
end
--*/
#define __UART_UART7_IRQ_PRIORITY__ __CPU_DEFAULT_IRQ_PRIORITY__

/*--
if this:PortExist(7) then
this:AddWidget("Combobox", "DMA mode")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
end
--*/
#define __UART_UART7_DMA_MODE__ _NO_

/*--
if this:PortExist(7) then
this:AddWidget("Textbox", "Rx DMA buffer length [B]")
this:AddItem("32", "")
this:AddItem("64", "")
this:AddItem("128", "")
this:AddItem("256", "")
end
--*/
#define __UART_UART7_DMA_BUF_LEN__ 64


/*--
if this:PortExist(8) then
this:AddExtraWidget("Label", "LabelIRQDMA8", "\nUART8 IRQ and DMA configuration", -1, "bold")
this:AddExtraWidget("Void", "VoidIRQDMA8")
end
++*/
/*--
if this:PortExist(8) then
this:AddWidget("Combobox", "IRQ priority")
uC.AddPriorityItems(this, false)
end
--*/
#define __UART_UART8_IRQ_PRIORITY__ __CPU_DEFAULT_IRQ_PRIORITY__

/*--
if this:PortExist(8) then
this:AddWidget("Combobox", "DMA mode")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
end
--*/
#define __UART_UART8_DMA_MODE__ _NO_

/*--
if this:PortExist(8) then
this:AddWidget("Textbox", "Rx DMA buffer length [B]")
this:AddItem("32", "")
this:AddItem("64", "")
this:AddItem("128", "")
this:AddItem("256", "")
end
--*/
#define __UART_UART8_DMA_BUF_LEN__ 64


/*--
if this:PortExist(9) then
this:AddExtraWidget("Label", "LabelIRQDMA9", "\nUART9 IRQ and DMA configuration", -1, "bold")
this:AddExtraWidget("Void", "VoidIRQDMA9")
end
++*/
/*--
if this:PortExist(9) then
this:AddWidget("Combobox", "IRQ priority")
uC.AddPriorityItems(this, false)
end
--*/
#define __UART_UART9_IRQ_PRIORITY__ __CPU_DEFAULT_IRQ_PRIORITY__

/*--
if this:PortExist(9) then
this:AddWidget("Combobox", "DMA mode")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
end
--*/
#define __UART_UART9_DMA_MODE__ _NO_

/*--
if this:PortExist(9) then
this:AddWidget("Textbox", "Rx DMA buffer length [B]")
this:AddItem("32", "")
this:AddItem("64", "")
this:AddItem("128", "")
this:AddItem("256", "")
end
--*/
#define __UART_UART9_DMA_BUF_LEN__ 64


/*--
if this:PortExist(10) then
this:AddExtraWidget("Label", "LabelIRQDMA10", "\nUART10 IRQ and DMA configuration", -1, "bold")
this:AddExtraWidget("Void", "VoidIRQDMA10")
end
++*/
/*--
if this:PortExist(10) then
this:AddWidget("Combobox", "IRQ priority")
uC.AddPriorityItems(this, false)
end
--*/
#define __UART_UART10_IRQ_PRIORITY__ __CPU_DEFAULT_IRQ_PRIORITY__

/*--
if this:PortExist(10) then
this:AddWidget("Combobox", "DMA mode")
this:AddItem("No", "_NO_")
this:AddItem("Yes", "_YES_")
end
--*/
#define __UART_UART10_DMA_MODE__ _NO_

/*--
if this:PortExist(10) then
this:AddWidget("Textbox", "Rx DMA buffer length [B]")
this:AddItem("32", "")
this:AddItem("64", "")
this:AddItem("128", "")
this:AddItem("256", "")
end
--*/
#define __UART_UART10_DMA_BUF_LEN__ 64

#endif /* _UART_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
