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
    UART_PER["STM32F301C6xx"] = 3
    UART_PER["STM32F301C8xx"] = 3
    UART_PER["STM32F301K6xx"] = 2
    UART_PER["STM32F301K8xx"] = 2
    UART_PER["STM32F301R6xx"] = 3
    UART_PER["STM32F301R8xx"] = 3
    UART_PER["STM32F302C6xx"] = 3
    UART_PER["STM32F302C8xx"] = 3
    UART_PER["STM32F302CBxx"] = 3
    UART_PER["STM32F302CCxx"] = 3
    UART_PER["STM32F302K6xx"] = 2
    UART_PER["STM32F302K8xx"] = 2
    UART_PER["STM32F302R6xx"] = 3
    UART_PER["STM32F302R8xx"] = 3
    UART_PER["STM32F302RBxx"] = 5
    UART_PER["STM32F302RCxx"] = 5
    UART_PER["STM32F302RDxx"] = 5
    UART_PER["STM32F302RExx"] = 5
    UART_PER["STM32F302VBxx"] = 5
    UART_PER["STM32F302VCxx"] = 5
    UART_PER["STM32F302VDxx"] = 5
    UART_PER["STM32F302VExx"] = 5
    UART_PER["STM32F302ZDxx"] = 5
    UART_PER["STM32F302ZExx"] = 5
    UART_PER["STM32F303C6xx"] = 3
    UART_PER["STM32F303C8xx"] = 3
    UART_PER["STM32F303CBxx"] = 3
    UART_PER["STM32F303CCxx"] = 3
    UART_PER["STM32F303K6xx"] = 2
    UART_PER["STM32F303K8xx"] = 2
    UART_PER["STM32F303R6xx"] = 3
    UART_PER["STM32F303R8xx"] = 3
    UART_PER["STM32F303RBxx"] = 5
    UART_PER["STM32F303RCxx"] = 5
    UART_PER["STM32F303RDxx"] = 5
    UART_PER["STM32F303RExx"] = 5
    UART_PER["STM32F303VBxx"] = 5
    UART_PER["STM32F303VCxx"] = 5
    UART_PER["STM32F303VDxx"] = 5
    UART_PER["STM32F303VExx"] = 5
    UART_PER["STM32F303ZDxx"] = 5
    UART_PER["STM32F303ZExx"] = 5
    UART_PER["STM32F318C8xx"] = 3
    UART_PER["STM32F318K8xx"] = 2
    UART_PER["STM32F328C8xx"] = 3
    UART_PER["STM32F334C4xx"] = 3
    UART_PER["STM32F334C6xx"] = 3
    UART_PER["STM32F334C8xx"] = 3
    UART_PER["STM32F334K4xx"] = 2
    UART_PER["STM32F334K6xx"] = 2
    UART_PER["STM32F334K8xx"] = 2
    UART_PER["STM32F334R6xx"] = 3
    UART_PER["STM32F334R8xx"] = 3
    UART_PER["STM32F358CCxx"] = 3
    UART_PER["STM32F358RCxx"] = 5
    UART_PER["STM32F358VCxx"] = 5
    UART_PER["STM32F373C8xx"] = 3
    UART_PER["STM32F373CBxx"] = 3
    UART_PER["STM32F373CCxx"] = 3
    UART_PER["STM32F373R8xx"] = 3
    UART_PER["STM32F373RBxx"] = 3
    UART_PER["STM32F373RCxx"] = 3
    UART_PER["STM32F373V8xx"] = 3
    UART_PER["STM32F373VBxx"] = 3
    UART_PER["STM32F373VCxx"] = 3
    UART_PER["STM32F378CCxx"] = 3
    UART_PER["STM32F378RCxx"] = 3
    UART_PER["STM32F378VCxx"] = 3
    UART_PER["STM32F398VExx"] = 5

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
this:AddWidget("Combobox", "IRQ priority")
uC.AddPriorityItems(this, false)
--*/
#define __UART_IRQ_PRIORITY__ 13

#endif /* _UART_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
