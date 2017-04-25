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
    UART_PER["STM32F401CBxx"] = 1
    UART_PER["STM32F401RBxx"] = 1
    UART_PER["STM32F401VBxx"] = 1
    UART_PER["STM32F401CCxx"] = 1
    UART_PER["STM32F401RCxx"] = 1
    UART_PER["STM32F401VCxx"] = 1
    UART_PER["STM32F401CDxx"] = 1
    UART_PER["STM32F401RDxx"] = 1
    UART_PER["STM32F401VDxx"] = 1
    UART_PER["STM32F401CExx"] = 1
    UART_PER["STM32F401RExx"] = 1
    UART_PER["STM32F401VExx"] = 1
    UART_PER["STM32F405RGxx"] = 1
    UART_PER["STM32F405OExx"] = 1
    UART_PER["STM32F405OGxx"] = 1
    UART_PER["STM32F405VGxx"] = 1
    UART_PER["STM32F405ZGxx"] = 1
    UART_PER["STM32F407VExx"] = 1
    UART_PER["STM32F407VGxx"] = 1
    UART_PER["STM32F407ZExx"] = 1
    UART_PER["STM32F407ZGxx"] = 1
    UART_PER["STM32F407IExx"] = 1
    UART_PER["STM32F407IGxx"] = 1
    UART_PER["STM32F410T8Yx"] = 1
    UART_PER["STM32F410TBYx"] = 1
    UART_PER["STM32F410C8Ux"] = 1
    UART_PER["STM32F410CBUx"] = 1
    UART_PER["STM32F410C8Tx"] = 1
    UART_PER["STM32F410CBTx"] = 1
    UART_PER["STM32F410R8Tx"] = 1
    UART_PER["STM32F410RBTx"] = 1
    UART_PER["STM32F410R8Ix"] = 1
    UART_PER["STM32F410RBIx"] = 1
    UART_PER["STM32F411CCxx"] = 1
    UART_PER["STM32F411RCxx"] = 1
    UART_PER["STM32F411VCxx"] = 1
    UART_PER["STM32F411CExx"] = 1
    UART_PER["STM32F411RExx"] = 1
    UART_PER["STM32F411VExx"] = 1
    UART_PER["STM32F412CExx"] = 1
    UART_PER["STM32F412CGxx"] = 1
    UART_PER["STM32F412RExx"] = 1
    UART_PER["STM32F412RGxx"] = 1
    UART_PER["STM32F412VExx"] = 1
    UART_PER["STM32F412VGxx"] = 1
    UART_PER["STM32F412ZExx"] = 1
    UART_PER["STM32F412ZGxx"] = 1
    UART_PER["STM32F413CGxx"] = 1
    UART_PER["STM32F413RGxx"] = 1
    UART_PER["STM32F413MGxx"] = 1
    UART_PER["STM32F413VGxx"] = 1
    UART_PER["STM32F413ZGxx"] = 1
    UART_PER["STM32F413CHxx"] = 1
    UART_PER["STM32F413RHxx"] = 1
    UART_PER["STM32F413MHxx"] = 1
    UART_PER["STM32F413VHxx"] = 1
    UART_PER["STM32F413ZHxx"] = 1
    UART_PER["STM32F415RGxx"] = 1
    UART_PER["STM32F415OGxx"] = 1
    UART_PER["STM32F415VGxx"] = 1
    UART_PER["STM32F415ZGxx"] = 1
    UART_PER["STM32F417VExx"] = 1
    UART_PER["STM32F417VGxx"] = 1
    UART_PER["STM32F417ZExx"] = 1
    UART_PER["STM32F417ZGxx"] = 1
    UART_PER["STM32F417IExx"] = 1
    UART_PER["STM32F417IGxx"] = 1
    UART_PER["STM32F423CHxx"] = 1
    UART_PER["STM32F423RHxx"] = 1
    UART_PER["STM32F423MHxx"] = 1
    UART_PER["STM32F423VHxx"] = 1
    UART_PER["STM32F423ZHxx"] = 1
    UART_PER["STM32F427VGxx"] = 1
    UART_PER["STM32F427VIxx"] = 1
    UART_PER["STM32F427ZGxx"] = 1
    UART_PER["STM32F427ZIxx"] = 1
    UART_PER["STM32F427AGxx"] = 1
    UART_PER["STM32F427AIxx"] = 1
    UART_PER["STM32F427IGxx"] = 1
    UART_PER["STM32F427IIxx"] = 1
    UART_PER["STM32F429VExx"] = 1
    UART_PER["STM32F429VGxx"] = 1
    UART_PER["STM32F429VIxx"] = 1
    UART_PER["STM32F429ZExx"] = 1
    UART_PER["STM32F429ZGxx"] = 1
    UART_PER["STM32F429ZIxx"] = 1
    UART_PER["STM32F429AGxx"] = 1
    UART_PER["STM32F429AIxx"] = 1
    UART_PER["STM32F429IExx"] = 1
    UART_PER["STM32F429IGxx"] = 1
    UART_PER["STM32F429IIxx"] = 1
    UART_PER["STM32F429BExx"] = 1
    UART_PER["STM32F429BGxx"] = 1
    UART_PER["STM32F429BIxx"] = 1
    UART_PER["STM32F429NExx"] = 1
    UART_PER["STM32F429NGxx"] = 1
    UART_PER["STM32F429NIxx"] = 1
    UART_PER["STM32F437VGxx"] = 1
    UART_PER["STM32F437VIxx"] = 1
    UART_PER["STM32F437ZGxx"] = 1
    UART_PER["STM32F437ZIxx"] = 1
    UART_PER["STM32F437IGxx"] = 1
    UART_PER["STM32F437IIxx"] = 1
    UART_PER["STM32F437AIxx"] = 1
    UART_PER["STM32F439VGxx"] = 1
    UART_PER["STM32F439VIxx"] = 1
    UART_PER["STM32F439ZGxx"] = 1
    UART_PER["STM32F439ZIxx"] = 1
    UART_PER["STM32F439IGxx"] = 1
    UART_PER["STM32F439IIxx"] = 1
    UART_PER["STM32F439BGxx"] = 1
    UART_PER["STM32F439BIxx"] = 1
    UART_PER["STM32F439NGxx"] = 1
    UART_PER["STM32F439NIxx"] = 1
    UART_PER["STM32F439AIxx"] = 1
    UART_PER["STM32F446MCxx"] = 1
    UART_PER["STM32F446RCxx"] = 1
    UART_PER["STM32F446VCxx"] = 1
    UART_PER["STM32F446ZCxx"] = 1
    UART_PER["STM32F446MExx"] = 1
    UART_PER["STM32F446RExx"] = 1
    UART_PER["STM32F446VExx"] = 1
    UART_PER["STM32F446ZExx"] = 1
    UART_PER["STM32F469VExx"] = 1
    UART_PER["STM32F469VGxx"] = 1
    UART_PER["STM32F469VIxx"] = 1
    UART_PER["STM32F469ZExx"] = 1
    UART_PER["STM32F469ZGxx"] = 1
    UART_PER["STM32F469ZIxx"] = 1
    UART_PER["STM32F469AExx"] = 1
    UART_PER["STM32F469AGxx"] = 1
    UART_PER["STM32F469AIxx"] = 1
    UART_PER["STM32F469IExx"] = 1
    UART_PER["STM32F469IGxx"] = 1
    UART_PER["STM32F469IIxx"] = 1
    UART_PER["STM32F469BExx"] = 1
    UART_PER["STM32F469BGxx"] = 1
    UART_PER["STM32F469BIxx"] = 1
    UART_PER["STM32F469NExx"] = 1
    UART_PER["STM32F469NGxx"] = 1
    UART_PER["STM32F469NIxx"] = 1
    UART_PER["STM32F479VGxx"] = 1
    UART_PER["STM32F479VIxx"] = 1
    UART_PER["STM32F479ZGxx"] = 1
    UART_PER["STM32F479ZIxx"] = 1
    UART_PER["STM32F479AGxx"] = 1
    UART_PER["STM32F479AIxx"] = 1
    UART_PER["STM32F479IGxx"] = 1
    UART_PER["STM32F479IIxx"] = 1
    UART_PER["STM32F479BGxx"] = 1
    UART_PER["STM32F479BIxx"] = 1
    UART_PER["STM32F479NGxx"] = 1
    UART_PER["STM32F479NIxx"] = 1

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
this:AddExtraWidget("Label", "LabelPrio", "\nPriorities", -1, "bold")
this:AddExtraWidget("Void", "VoidPrio")
++*/
/*--
if this:PortExist(1) then
    this:AddWidget("Combobox", "UART1 IRQ priority")
    uC.AddPriorityItems(this)
end
--*/
#define __UART_UART1_IRQ_PRIORITY__ __CPU_IRQ_USER_PRIORITY__

/*--
if this:PortExist(2) then
    this:AddWidget("Combobox", "UART2 IRQ priority")
    uC.AddPriorityItems(this)
end
--*/
#define __UART_UART2_IRQ_PRIORITY__ __CPU_IRQ_USER_PRIORITY__

/*--
if this:PortExist(3) then
    this:AddWidget("Combobox", "UART3 IRQ priority")
    uC.AddPriorityItems(this)
end
--*/
#define __UART_UART3_IRQ_PRIORITY__ __CPU_IRQ_USER_PRIORITY__

/*--
if this:PortExist(4) then
    this:AddWidget("Combobox", "UART4 IRQ priority")
    uC.AddPriorityItems(this)
end
--*/
#define __UART_UART4_IRQ_PRIORITY__ __CPU_IRQ_USER_PRIORITY__

/*--
if this:PortExist(5) then
    this:AddWidget("Combobox", "UART5 IRQ priority")
    uC.AddPriorityItems(this)
end
--*/
#define __UART_UART5_IRQ_PRIORITY__ __CPU_IRQ_USER_PRIORITY__

/*--
if this:PortExist(6) then
    this:AddWidget("Combobox", "UART6 IRQ priority")
    uC.AddPriorityItems(this)
end
--*/
#define __UART_UART6_IRQ_PRIORITY__ __CPU_IRQ_USER_PRIORITY__

/*--
if this:PortExist(7) then
    this:AddWidget("Combobox", "UART7 IRQ priority")
    uC.AddPriorityItems(this)
end
--*/
#define __UART_UART7_IRQ_PRIORITY__ __CPU_IRQ_USER_PRIORITY__

/*--
if this:PortExist(8) then
    this:AddWidget("Combobox", "UART8 IRQ priority")
    uC.AddPriorityItems(this)
end
--*/
#define __UART_UART8_IRQ_PRIORITY__ __CPU_IRQ_USER_PRIORITY__

#endif /* _UART_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
