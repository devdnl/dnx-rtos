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
    UART_PER["STM32F722ICxx"] = 8
    UART_PER["STM32F722IExx"] = 8
    UART_PER["STM32F722RCxx"] = 6
    UART_PER["STM32F722RExx"] = 6
    UART_PER["STM32F722VCxx"] = 8
    UART_PER["STM32F722VExx"] = 8
    UART_PER["STM32F722ZCxx"] = 8
    UART_PER["STM32F722ZExx"] = 8
    UART_PER["STM32F723ICxx"] = 8
    UART_PER["STM32F723IExx"] = 8
    UART_PER["STM32F723VCxx"] = 8
    UART_PER["STM32F723VExx"] = 8
    UART_PER["STM32F723ZCxx"] = 8
    UART_PER["STM32F723ZExx"] = 8
    UART_PER["STM32F730I8xx"] = 8
    UART_PER["STM32F730R8xx"] = 6
    UART_PER["STM32F730V8xx"] = 8
    UART_PER["STM32F730Z8xx"] = 8
    UART_PER["STM32F732IExx"] = 8
    UART_PER["STM32F732RExx"] = 6
    UART_PER["STM32F732VExx"] = 8
    UART_PER["STM32F732ZExx"] = 8
    UART_PER["STM32F733IExx"] = 8
    UART_PER["STM32F733VExx"] = 8
    UART_PER["STM32F733ZExx"] = 8
    UART_PER["STM32F745IExx"] = 8
    UART_PER["STM32F745IGxx"] = 8
    UART_PER["STM32F745VExx"] = 8
    UART_PER["STM32F745VGxx"] = 8
    UART_PER["STM32F745ZExx"] = 8
    UART_PER["STM32F745ZGxx"] = 8
    UART_PER["STM32F746BExx"] = 8
    UART_PER["STM32F746BGxx"] = 8
    UART_PER["STM32F746IExx"] = 8
    UART_PER["STM32F746IGxx"] = 8
    UART_PER["STM32F746NExx"] = 8
    UART_PER["STM32F746NGxx"] = 8
    UART_PER["STM32F746VExx"] = 8
    UART_PER["STM32F746VGxx"] = 8
    UART_PER["STM32F746ZExx"] = 8
    UART_PER["STM32F746ZGxx"] = 8
    UART_PER["STM32F750N8xx"] = 8
    UART_PER["STM32F750V8xx"] = 8
    UART_PER["STM32F750Z8xx"] = 8
    UART_PER["STM32F756BGxx"] = 8
    UART_PER["STM32F756IGxx"] = 8
    UART_PER["STM32F756NGxx"] = 8
    UART_PER["STM32F756VGxx"] = 8
    UART_PER["STM32F756ZGxx"] = 8
    UART_PER["STM32F765BGxx"] = 8
    UART_PER["STM32F765BIxx"] = 8
    UART_PER["STM32F765IGxx"] = 8
    UART_PER["STM32F765IIxx"] = 8
    UART_PER["STM32F765NGxx"] = 8
    UART_PER["STM32F765NIxx"] = 8
    UART_PER["STM32F765VGxx"] = 8
    UART_PER["STM32F765VIxx"] = 8
    UART_PER["STM32F765ZGxx"] = 8
    UART_PER["STM32F765ZIxx"] = 8
    UART_PER["STM32F767BGxx"] = 8
    UART_PER["STM32F767BIxx"] = 8
    UART_PER["STM32F767IGxx"] = 8
    UART_PER["STM32F767IIxx"] = 8
    UART_PER["STM32F767NGxx"] = 8
    UART_PER["STM32F767NIxx"] = 8
    UART_PER["STM32F767VGxx"] = 8
    UART_PER["STM32F767VIxx"] = 8
    UART_PER["STM32F767ZGxx"] = 8
    UART_PER["STM32F767ZIxx"] = 8
    UART_PER["STM32F768AIxx"] = 8
    UART_PER["STM32F769AGxx"] = 8
    UART_PER["STM32F769AIxx"] = 8
    UART_PER["STM32F769BGxx"] = 8
    UART_PER["STM32F769BIxx"] = 8
    UART_PER["STM32F769IGxx"] = 8
    UART_PER["STM32F769IIxx"] = 8
    UART_PER["STM32F769NGxx"] = 8
    UART_PER["STM32F769NIxx"] = 8
    UART_PER["STM32F777BIxx"] = 8
    UART_PER["STM32F777IIxx"] = 8
    UART_PER["STM32F777NIxx"] = 8
    UART_PER["STM32F777VIxx"] = 8
    UART_PER["STM32F777ZIxx"] = 8
    UART_PER["STM32F778AIxx"] = 8
    UART_PER["STM32F779AIxx"] = 8
    UART_PER["STM32F779BIxx"] = 8
    UART_PER["STM32F779IIxx"] = 8
    UART_PER["STM32F779NIxx"] = 8

    return UART_PER[uC.NAME] >= devNo
end
++*/

/*--
this:AddExtraWidget("Label", "LabelDefaults", "Defaults", -1, "bold")
this:AddExtraWidget("Void", "VoidDefaults")
++*/
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
this:AddWidget("Spinbox", 16, 65536, "Rx buffer length [B]")
end
--*/
#define __UART_MAJOR0_RX_BUFFER_LEN__ 128

/*--
if this:PortExist(1) then
this:AddWidget("Spinbox", 16, 65536, "Tx buffer length [B]")
end
--*/
#define __UART_MAJOR0_TX_BUFFER_LEN__ 128

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
this:AddWidget("Spinbox", 16, 65536, "Rx buffer length [B]")
end
--*/
#define __UART_MAJOR1_RX_BUFFER_LEN__ 128

/*--
if this:PortExist(2) then
this:AddWidget("Spinbox", 16, 65536, "Tx buffer length [B]")
end
--*/
#define __UART_MAJOR1_TX_BUFFER_LEN__ 128
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
this:AddWidget("Spinbox", 16, 65536, "Rx buffer length [B]")
end
--*/
#define __UART_MAJOR2_RX_BUFFER_LEN__ 128

/*--
if this:PortExist(3) then
this:AddWidget("Spinbox", 16, 65536, "Tx buffer length [B]")
end
--*/
#define __UART_MAJOR2_TX_BUFFER_LEN__ 128
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
this:AddWidget("Spinbox", 16, 65536, "Rx buffer length [B]")
end
--*/
#define __UART_MAJOR3_RX_BUFFER_LEN__ 128

/*--
if this:PortExist(4) then
this:AddWidget("Spinbox", 16, 65536, "Tx buffer length [B]")
end
--*/
#define __UART_MAJOR3_TX_BUFFER_LEN__ 128
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
this:AddWidget("Spinbox", 16, 65536, "Rx buffer length [B]")
end
--*/
#define __UART_MAJOR4_RX_BUFFER_LEN__ 128

/*--
if this:PortExist(5) then
this:AddWidget("Spinbox", 16, 65536, "Tx buffer length [B]")
end
--*/
#define __UART_MAJOR4_TX_BUFFER_LEN__ 128
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
this:AddWidget("Spinbox", 16, 65536, "Rx buffer length [B]")
end
--*/
#define __UART_MAJOR5_RX_BUFFER_LEN__ 128

/*--
if this:PortExist(6) then
this:AddWidget("Spinbox", 16, 65536, "Tx buffer length [B]")
end
--*/
#define __UART_MAJOR5_TX_BUFFER_LEN__ 128
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
this:AddWidget("Spinbox", 16, 65536, "Rx buffer length [B]")
end
--*/
#define __UART_MAJOR6_RX_BUFFER_LEN__ 128

/*--
if this:PortExist(7) then
this:AddWidget("Spinbox", 16, 65536, "Tx buffer length [B]")
end
--*/
#define __UART_MAJOR6_TX_BUFFER_LEN__ 128
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
this:AddWidget("Spinbox", 16, 65536, "Rx buffer length [B]")
end
--*/
#define __UART_MAJOR7_RX_BUFFER_LEN__ 128

/*--
if this:PortExist(8) then
this:AddWidget("Spinbox", 16, 65536, "Tx buffer length [B]")
end
--*/
#define __UART_MAJOR7_TX_BUFFER_LEN__ 128
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

#endif /* _UART_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
