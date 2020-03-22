#/*=============================================================================
# @file    cpu_flags.h
#
# @author  Daniel Zorychta
#
# @brief   This file contains CPU configuration flags.
#          Hybrid file: included both by Make and CC.
#
# @note    Copyright (C) 2019 Daniel Zorychta <daniel.zorychta@gmail.com>
#
#          This program is free software; you can redistribute it and/or modify
#          it under the terms of the GNU General Public License as published by
#          the Free Software Foundation and modified by the dnx RTOS exception.
#
#          NOTE: The modification  to the GPL is  included to allow you to
#                distribute a combined work that includes dnx RTOS without
#                being obliged to provide the source  code for proprietary
#                components outside of the dnx RTOS.
#
#          The dnx RTOS  is  distributed  in the hope  that  it will be useful,
#          but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
#          MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
#          GNU General Public License for more details.
#
#          Full license text is available on the following file: doc/license.txt.
#
#
#=============================================================================*/

#/*
#* NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as suffix
#*       and prefix) are exported to the single configuration file
#*       (by using Configtool) when entire project configuration is exported.
#*       All other flag definitions and statements are ignored.
#*/

#ifndef _CPU_FLAGS_H_
#define _CPU_FLAGS_H_

#/*--
# this:SetLayout("TitledGridBack", 2, "Home > Microcontroller > Selection",
#                function() this:LoadFile("arch/arch_flags.h") end)
#++*/

#/*-- Flag is set in __CPU_NAME__ event
# this:AddWidget("Value")
#--*/
#define __CPU_FAMILY__ _STM32F4XX_ADVANCEDLINE_

#/*-- Flag is set in __CPU_NAME__ event
# this:AddWidget("Value")
#--*/
#define __CPU_SERIES__ _STM32F429xx

#/*-- Flag is set in __CPU_NAME__ event
# this:AddWidget("Value")
#--*/
#define __LD_SCRIPT__ STM32F42xxIxx
#/*
__LD_SCRIPT__=STM32F42xxIxx
#*/

#/*--
# local CPULIST = {
#                  {"STM32F401CBxx", "_STM32F4XX_ACCESSLINE_", "_STM32F401xC", "STM32F401xBxx"},
#                  {"STM32F401RBxx", "_STM32F4XX_ACCESSLINE_", "_STM32F401xC", "STM32F401xBxx"},
#                  {"STM32F401VBxx", "_STM32F4XX_ACCESSLINE_", "_STM32F401xC", "STM32F401xBxx"},
#                  {"STM32F401CCxx", "_STM32F4XX_ACCESSLINE_", "_STM32F401xC", "STM32F401xCxx"},
#                  {"STM32F401RCxx", "_STM32F4XX_ACCESSLINE_", "_STM32F401xC", "STM32F401xCxx"},
#                  {"STM32F401VCxx", "_STM32F4XX_ACCESSLINE_", "_STM32F401xC", "STM32F401xCxx"},
#                  {"STM32F401CDxx", "_STM32F4XX_ACCESSLINE_", "_STM32F401xE", "STM32F401xDxx"},
#                  {"STM32F401RDxx", "_STM32F4XX_ACCESSLINE_", "_STM32F401xE", "STM32F401xDxx"},
#                  {"STM32F401VDxx", "_STM32F4XX_ACCESSLINE_", "_STM32F401xE", "STM32F401xDxx"},
#                  {"STM32F401CExx", "_STM32F4XX_ACCESSLINE_", "_STM32F401xE", "STM32F401xExx"},
#                  {"STM32F401RExx", "_STM32F4XX_ACCESSLINE_", "_STM32F401xE", "STM32F401xExx"},
#                  {"STM32F401VExx", "_STM32F4XX_ACCESSLINE_", "_STM32F401xE", "STM32F401xExx"},
#                  {"STM32F405RGxx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F405xx", "STM32F405xGxx"},
#                  {"STM32F405OExx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F405xx", "STM32F405xExx"},
#                  {"STM32F405OGxx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F405xx", "STM32F405xGxx"},
#                  {"STM32F405VGxx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F405xx", "STM32F405xGxx"},
#                  {"STM32F405ZGxx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F405xx", "STM32F405xGxx"},
#                  {"STM32F407VExx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F407xx", "STM32F407xExx"},
#                  {"STM32F407VGxx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F407xx", "STM32F407xGxx"},
#                  {"STM32F407ZExx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F407xx", "STM32F407xExx"},
#                  {"STM32F407ZGxx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F407xx", "STM32F407xGxx"},
#                  {"STM32F407IExx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F407xx", "STM32F407xExx"},
#                  {"STM32F407IGxx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F407xx", "STM32F407xGxx"},
#                  {"STM32F410T8xx", "_STM32F4XX_ACCESSLINE_", "_STM32F410Tx", "STM32F410x8xx"},
#                  {"STM32F410TBxx", "_STM32F4XX_ACCESSLINE_", "_STM32F410Tx", "STM32F410xBxx"},
#                  {"STM32F410C8xx", "_STM32F4XX_ACCESSLINE_", "_STM32F410Cx", "STM32F410x8xx"},
#                  {"STM32F410CBxx", "_STM32F4XX_ACCESSLINE_", "_STM32F410Cx", "STM32F410xBxx"},
#                  {"STM32F410R8xx", "_STM32F4XX_ACCESSLINE_", "_STM32F410Rx", "STM32F410x8xx"},
#                  {"STM32F410RBxx", "_STM32F4XX_ACCESSLINE_", "_STM32F410Rx", "STM32F410xBxx"},
#                  {"STM32F411CCxx", "_STM32F4XX_ACCESSLINE_", "_STM32F411xE", "STM32F411xCxx"},
#                  {"STM32F411RCxx", "_STM32F4XX_ACCESSLINE_", "_STM32F411xE", "STM32F411xCxx"},
#                  {"STM32F411VCxx", "_STM32F4XX_ACCESSLINE_", "_STM32F411xE", "STM32F411xCxx"},
#                  {"STM32F411CExx", "_STM32F4XX_ACCESSLINE_", "_STM32F411xE", "STM32F411xExx"},
#                  {"STM32F411RExx", "_STM32F4XX_ACCESSLINE_", "_STM32F411xE", "STM32F411xExx"},
#                  {"STM32F411VExx", "_STM32F4XX_ACCESSLINE_", "_STM32F411xE", "STM32F411xExx"},
#                  {"STM32F412CExx", "_STM32F4XX_ACCESSLINE_", "_STM32F412Cx", "STM32F412xExx"},
#                  {"STM32F412CGxx", "_STM32F4XX_ACCESSLINE_", "_STM32F412Cx", "STM32F412xGxx"},
#                  {"STM32F412RExx", "_STM32F4XX_ACCESSLINE_", "_STM32F412Rx", "STM32F412xExx"},
#                  {"STM32F412RGxx", "_STM32F4XX_ACCESSLINE_", "_STM32F412Rx", "STM32F412xGxx"},
#                  {"STM32F412VExx", "_STM32F4XX_ACCESSLINE_", "_STM32F412Vx", "STM32F412xExx"},
#                  {"STM32F412VGxx", "_STM32F4XX_ACCESSLINE_", "_STM32F412Vx", "STM32F412xGxx"},
#                  {"STM32F412ZExx", "_STM32F4XX_ACCESSLINE_", "_STM32F412Zx", "STM32F412xExx"},
#                  {"STM32F412ZGxx", "_STM32F4XX_ACCESSLINE_", "_STM32F412Zx", "STM32F412xGxx"},
#                  {"STM32F413CGxx", "_STM32F4XX_ACCESSLINE_", "_STM32F413xx", "STM32F413xGxx"},
#                  {"STM32F413RGxx", "_STM32F4XX_ACCESSLINE_", "_STM32F413xx", "STM32F413xGxx"},
#                  {"STM32F413MGxx", "_STM32F4XX_ACCESSLINE_", "_STM32F413xx", "STM32F413xGxx"},
#                  {"STM32F413VGxx", "_STM32F4XX_ACCESSLINE_", "_STM32F413xx", "STM32F413xGxx"},
#                  {"STM32F413ZGxx", "_STM32F4XX_ACCESSLINE_", "_STM32F413xx", "STM32F413xGxx"},
#                  {"STM32F413CHxx", "_STM32F4XX_ACCESSLINE_", "_STM32F413xx", "STM32F413xHxx"},
#                  {"STM32F413RHxx", "_STM32F4XX_ACCESSLINE_", "_STM32F413xx", "STM32F413xHxx"},
#                  {"STM32F413MHxx", "_STM32F4XX_ACCESSLINE_", "_STM32F413xx", "STM32F413xHxx"},
#                  {"STM32F413VHxx", "_STM32F4XX_ACCESSLINE_", "_STM32F413xx", "STM32F413xHxx"},
#                  {"STM32F413ZHxx", "_STM32F4XX_ACCESSLINE_", "_STM32F413xx", "STM32F413xHxx"},
#                  {"STM32F415RGxx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F415xx", "STM32F415xGxx"},
#                  {"STM32F415OGxx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F415xx", "STM32F415xGxx"},
#                  {"STM32F415VGxx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F415xx", "STM32F415xGxx"},
#                  {"STM32F415ZGxx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F415xx", "STM32F415xGxx"},
#                  {"STM32F417VExx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F417xx", "STM32F417xExx"},
#                  {"STM32F417VGxx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F417xx", "STM32F417xGxx"},
#                  {"STM32F417ZExx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F417xx", "STM32F417xExx"},
#                  {"STM32F417ZGxx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F417xx", "STM32F417xGxx"},
#                  {"STM32F417IExx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F417xx", "STM32F417xExx"},
#                  {"STM32F417IGxx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F417xx", "STM32F417xGxx"},
#                  {"STM32F423CHxx", "_STM32F4XX_ACCESSLINE_", "_STM32F423xx", "STM32F423xHxx"},
#                  {"STM32F423RHxx", "_STM32F4XX_ACCESSLINE_", "_STM32F423xx", "STM32F423xHxx"},
#                  {"STM32F423MHxx", "_STM32F4XX_ACCESSLINE_", "_STM32F423xx", "STM32F423xHxx"},
#                  {"STM32F423VHxx", "_STM32F4XX_ACCESSLINE_", "_STM32F423xx", "STM32F423xHxx"},
#                  {"STM32F423ZHxx", "_STM32F4XX_ACCESSLINE_", "_STM32F423xx", "STM32F423xHxx"},
#                  {"STM32F427VGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F427xx", "STM32F42xxGxx"},
#                  {"STM32F427VIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F427xx", "STM32F42xxIxx"},
#                  {"STM32F427ZGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F427xx", "STM32F42xxGxx"},
#                  {"STM32F427ZIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F427xx", "STM32F42xxIxx"},
#                  {"STM32F427AGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F427xx", "STM32F42xxGxx"},
#                  {"STM32F427AIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F427xx", "STM32F42xxIxx"},
#                  {"STM32F427IGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F427xx", "STM32F42xxGxx"},
#                  {"STM32F427IIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F427xx", "STM32F42xxIxx"},
#                  {"STM32F429VExx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F429xx", "STM32F42xxExx"},
#                  {"STM32F429VGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F429xx", "STM32F42xxGxx"},
#                  {"STM32F429VIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F429xx", "STM32F42xxIxx"},
#                  {"STM32F429ZExx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F429xx", "STM32F42xxExx"},
#                  {"STM32F429ZGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F429xx", "STM32F42xxGxx"},
#                  {"STM32F429ZIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F429xx", "STM32F42xxIxx"},
#                  {"STM32F429AGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F429xx", "STM32F42xxGxx"},
#                  {"STM32F429AIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F429xx", "STM32F42xxIxx"},
#                  {"STM32F429IExx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F429xx", "STM32F42xxExx"},
#                  {"STM32F429IGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F429xx", "STM32F42xxGxx"},
#                  {"STM32F429IIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F429xx", "STM32F42xxIxx"},
#                  {"STM32F429BExx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F429xx", "STM32F42xxExx"},
#                  {"STM32F429BGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F429xx", "STM32F42xxGxx"},
#                  {"STM32F429BIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F429xx", "STM32F42xxIxx"},
#                  {"STM32F429NExx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F429xx", "STM32F42xxExx"},
#                  {"STM32F429NGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F429xx", "STM32F42xxGxx"},
#                  {"STM32F429NIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F429xx", "STM32F42xxIxx"},
#                  {"STM32F437VGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F437xx", "STM32F43xxGxx"},
#                  {"STM32F437VIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F437xx", "STM32F43xxIxx"},
#                  {"STM32F437ZGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F437xx", "STM32F43xxGxx"},
#                  {"STM32F437ZIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F437xx", "STM32F43xxIxx"},
#                  {"STM32F437IGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F437xx", "STM32F43xxGxx"},
#                  {"STM32F437IIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F437xx", "STM32F43xxIxx"},
#                  {"STM32F437AIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F437xx", "STM32F43xxIxx"},
#                  {"STM32F439VGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F439xx", "STM32F43xxGxx"},
#                  {"STM32F439VIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F439xx", "STM32F43xxIxx"},
#                  {"STM32F439ZGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F439xx", "STM32F43xxGxx"},
#                  {"STM32F439ZIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F439xx", "STM32F43xxIxx"},
#                  {"STM32F439IGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F439xx", "STM32F43xxGxx"},
#                  {"STM32F439IIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F439xx", "STM32F43xxIxx"},
#                  {"STM32F439BGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F439xx", "STM32F43xxGxx"},
#                  {"STM32F439BIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F439xx", "STM32F43xxIxx"},
#                  {"STM32F439NGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F439xx", "STM32F43xxGxx"},
#                  {"STM32F439NIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F439xx", "STM32F43xxIxx"},
#                  {"STM32F439AIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F439xx", "STM32F43xxIxx"},
#                  {"STM32F446MCxx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F446xx", "STM32F446xCxx"},
#                  {"STM32F446RCxx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F446xx", "STM32F446xCxx"},
#                  {"STM32F446VCxx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F446xx", "STM32F446xCxx"},
#                  {"STM32F446ZCxx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F446xx", "STM32F446xCxx"},
#                  {"STM32F446MExx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F446xx", "STM32F446xExx"},
#                  {"STM32F446RExx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F446xx", "STM32F446xExx"},
#                  {"STM32F446VExx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F446xx", "STM32F446xExx"},
#                  {"STM32F446ZExx", "_STM32F4XX_FOUNDATIONLINE_", "_STM32F446xx", "STM32F446xExx"},
#                  {"STM32F469VExx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F469xx", "STM32F46xxExx"},
#                  {"STM32F469VGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F469xx", "STM32F46xxGxx"},
#                  {"STM32F469VIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F469xx", "STM32F46xxIxx"},
#                  {"STM32F469ZExx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F469xx", "STM32F46xxExx"},
#                  {"STM32F469ZGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F469xx", "STM32F46xxGxx"},
#                  {"STM32F469ZIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F469xx", "STM32F46xxIxx"},
#                  {"STM32F469AExx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F469xx", "STM32F46xxExx"},
#                  {"STM32F469AGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F469xx", "STM32F46xxGxx"},
#                  {"STM32F469AIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F469xx", "STM32F46xxIxx"},
#                  {"STM32F469IExx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F469xx", "STM32F46xxExx"},
#                  {"STM32F469IGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F469xx", "STM32F46xxGxx"},
#                  {"STM32F469IIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F469xx", "STM32F46xxIxx"},
#                  {"STM32F469BExx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F469xx", "STM32F46xxExx"},
#                  {"STM32F469BGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F469xx", "STM32F46xxGxx"},
#                  {"STM32F469BIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F469xx", "STM32F46xxIxx"},
#                  {"STM32F469NExx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F469xx", "STM32F46xxExx"},
#                  {"STM32F469NGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F469xx", "STM32F46xxGxx"},
#                  {"STM32F469NIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F469xx", "STM32F46xxIxx"},
#                  {"STM32F479VGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F479xx", "STM32F47xxGxx"},
#                  {"STM32F479VIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F479xx", "STM32F47xxIxx"},
#                  {"STM32F479ZGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F479xx", "STM32F47xxGxx"},
#                  {"STM32F479ZIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F479xx", "STM32F47xxIxx"},
#                  {"STM32F479AGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F479xx", "STM32F47xxGxx"},
#                  {"STM32F479AIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F479xx", "STM32F47xxIxx"},
#                  {"STM32F479IGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F479xx", "STM32F47xxGxx"},
#                  {"STM32F479IIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F479xx", "STM32F47xxIxx"},
#                  {"STM32F479BGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F479xx", "STM32F47xxGxx"},
#                  {"STM32F479BIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F479xx", "STM32F47xxIxx"},
#                  {"STM32F479NGxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F479xx", "STM32F47xxGxx"},
#                  {"STM32F479NIxx", "_STM32F4XX_ADVANCEDLINE_", "_STM32F479xx", "STM32F47xxIxx"},
#                 }
#
# this:AddWidget("Combobox", "Mirocontroller name")
# for i = 1, #CPULIST do this:AddItem(CPULIST[i][1], CPULIST[i][1]) end
# this:SetEvent("clicked",
#    function()
#        local CPUNAME = this:GetFlagValue("__CPU_NAME__")
#
#        for i = 1, #CPULIST do
#            if CPULIST[i][1] == CPUNAME then
#                this:SetFlagValue("__CPU_FAMILY__", CPULIST[i][2])
#                this:SetFlagValue("__CPU_SERIES__", CPULIST[i][3])
#                this:SetFlagValue("__LD_SCRIPT__",  CPULIST[i][4])
#                break
#            end
#        end
#    end
# )
#--*/
#define __CPU_NAME__ STM32F429BIxx
#/*
__CPU_NAME__=STM32F429BIxx
#*/

#/*
# * NOTE!
# * Any interrupt that uses the FreeRTOS API must be set to the same priority as the
# * RTOS kernel (as configured by the _CPU_IRQ_RTOS_KERNEL_PRIORITY_ macro), or at
# * or below _CPU_IRQ_RTOS_SYSCALL_PRIORITY_ for ports that include this functionality.
# */

#/*--
# this:AddWidget("Combobox", "RTOS Kernel IRQ priority")
# uC.AddPriorityItems(this, true)
#--*/
#define __CPU_IRQ_RTOS_KERNEL_PRIORITY_VAL__ 15

#/*--
# this:AddWidget("Combobox", "Max RTOS API call IRQ priority")
# uC.AddPriorityItems(this, true)
#--*/
#define __CPU_IRQ_RTOS_APICALL_PRIORITY_VAL__ 12

#/*--
# this:AddWidget("Combobox", "Default IRQ priority")
# uC.AddPriorityItems(this, true)
#--*/
#define __CPU_DEFAULT_IRQ_PRIORITY__ 14

#/*--
# this:AddWidget("Combobox", "Interruption of Multi-cycle instructions")
# this:AddItem("Enable", "_NO_")
# this:AddItem("Disable", "_YES_")
#--*/
#define __CPU_DISABLE_INTER_OF_MCYCLE_INSTR__ _YES_

#/*--
# this:AddWidget("Editline", false, "Interrupt vector position")
#--*/
#define __CPU_VTOR_TAB_POSITION__ 0x40000

#//-----------------------------------------------------------------------------
#// mandatory flags, not configurable
#//-----------------------------------------------------------------------------
#define _CPU_START_FREQUENCY_           (16000000UL)
#define _CPU_HEAP_ALIGN_                (4)
#define _CPU_IRQ_RTOS_KERNEL_PRIORITY_  (__CPU_IRQ_RTOS_KERNEL_PRIORITY_VAL__ << 4)
#define _CPU_IRQ_RTOS_APICALL_PRIORITY_ (__CPU_IRQ_RTOS_APICALL_PRIORITY_VAL__ << 4)
#define _CPU_IRQ_SAFE_PRIORITY_         (__CPU_IRQ_RTOS_KERNEL_PRIORITY_VAL__)
#define ARCH_stm32f4
#/*
CPUCONFIG_AFLAGS=-mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -ffast-math -fno-math-errno -mthumb-interwork -DGCC_ARMCM4
CPUCONFIG_CFLAGS=-mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -ffast-math -fno-math-errno -mthumb-interwork -DGCC_ARMCM4
CPUCONFIG_CXXFLAGS=-mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -ffast-math -fno-math-errno -mthumb-interwork -DGCC_ARMCM4
CPUCONFIG_LDFLAGS=-mcpu=cortex-m4 -mthumb -mthumb-interwork -nostartfiles -T./src/system/cpu/stm32f4/ld/$(__LD_SCRIPT__).ld
#*/

#// All CPU names definitions - general usage
#define STM32F401CBxx 0x8ddb344f
#define STM32F401RBxx 0x02d7f54f
#define STM32F401VBxx 0x1d6cd97b
#define STM32F401CCxx 0x8aae4440
#define STM32F401RCxx 0xd1fc1f0f
#define STM32F401VCxx 0x5cd74dec
#define STM32F401CDxx 0x7bcf949c
#define STM32F401RDxx 0x001e49c1
#define STM32F401VDxx 0x57b50fee
#define STM32F401CExx 0x9388fce4
#define STM32F401RExx 0x9a83e566
#define STM32F401VExx 0x18b7976a
#define STM32F405RGxx 0xb8d02c63
#define STM32F405OExx 0x99ae3fcb
#define STM32F405OGxx 0x1906c7c2
#define STM32F405VGxx 0x63bc681f
#define STM32F405ZGxx 0x31712729
#define STM32F407VExx 0x89838e0f
#define STM32F407VGxx 0x7e452809
#define STM32F407ZExx 0xcdc63acd
#define STM32F407ZGxx 0xac326b96
#define STM32F407IExx 0xb63c703b
#define STM32F407IGxx 0x1b630379
#define STM32F410T8xx 0x076b811c
#define STM32F410TBxx 0x8a7b6a68
#define STM32F410C8xx 0xe3ed5fce
#define STM32F410CBxx 0xfe3e404b
#define STM32F410R8xx 0x2f90adb0
#define STM32F410RBxx 0xad971a7f
#define STM32F411CCxx 0x8e499edd
#define STM32F411RCxx 0x6b6456e9
#define STM32F411VCxx 0x553a0f49
#define STM32F411CExx 0x52a5fe7c
#define STM32F411RExx 0x4055292b
#define STM32F411VExx 0x9f952ada
#define STM32F412CExx 0xb695b781
#define STM32F412CGxx 0x786fbc29
#define STM32F412RExx 0xd52f7d56
#define STM32F412RGxx 0x71007680
#define STM32F412VExx 0x3f623548
#define STM32F412VGxx 0xb750fd14
#define STM32F412ZExx 0xd73ebcdd
#define STM32F412ZGxx 0xd0688b11
#define STM32F413CGxx 0xe6c10e5b
#define STM32F413RGxx 0xbd6d6b35
#define STM32F413MGxx 0x4cfb2697
#define STM32F413VGxx 0x91c10878
#define STM32F413ZGxx 0x94c5551a
#define STM32F413CHxx 0xaf59930c
#define STM32F413RHxx 0x06dd58e9
#define STM32F413MHxx 0xd8f34eaf
#define STM32F413VHxx 0x93a4eda3
#define STM32F413ZHxx 0x9896aaf7
#define STM32F415RGxx 0x224aec55
#define STM32F415OGxx 0x266f2a4c
#define STM32F415VGxx 0xf297962e
#define STM32F415ZGxx 0x89245ab3
#define STM32F417VExx 0x01351e72
#define STM32F417VGxx 0xf04f8dc2
#define STM32F417ZExx 0x474db9cb
#define STM32F417ZGxx 0xc6e9152a
#define STM32F417IExx 0x27e179a1
#define STM32F417IGxx 0x3858f8b4
#define STM32F423CHxx 0x3d70526c
#define STM32F423RHxx 0x560c6e96
#define STM32F423MHxx 0x924f8f91
#define STM32F423VHxx 0xdd39f225
#define STM32F423ZHxx 0xa048affc
#define STM32F427VGxx 0x2adadb39
#define STM32F427VIxx 0xf621e823
#define STM32F427ZGxx 0x38c4ee49
#define STM32F427ZIxx 0x233b1e8b
#define STM32F427AGxx 0xc73d4723
#define STM32F427AIxx 0xe5d81e4e
#define STM32F427IGxx 0x0f7ec5b0
#define STM32F427IIxx 0x64724d80
#define STM32F429VExx 0xae589fe2
#define STM32F429VGxx 0x043d6cab
#define STM32F429VIxx 0x2fb3bdf5
#define STM32F429ZExx 0x61151451
#define STM32F429ZGxx 0xb8ea40b8
#define STM32F429ZIxx 0xcff034fe
#define STM32F429AGxx 0xede167b0
#define STM32F429AIxx 0xfc8ec601
#define STM32F429IExx 0xefc2dd3e
#define STM32F429IGxx 0xf04a9d3b
#define STM32F429IIxx 0xe6392931
#define STM32F429BExx 0x0020485d
#define STM32F429BGxx 0x4d55be47
#define STM32F429BIxx 0x6fe83da5
#define STM32F429NExx 0x2b7a2d52
#define STM32F429NGxx 0x766e0bff
#define STM32F429NIxx 0xd71d7f4f
#define STM32F437VGxx 0xab0a7a8a
#define STM32F437VIxx 0x0d97d8c6
#define STM32F437ZGxx 0x31c8a596
#define STM32F437ZIxx 0x504c978f
#define STM32F437IGxx 0xbadcd038
#define STM32F437IIxx 0x768679d4
#define STM32F437AIxx 0x057843fd
#define STM32F439VGxx 0x451ba5e2
#define STM32F439VIxx 0x49b3acb0
#define STM32F439ZGxx 0x44dea285
#define STM32F439ZIxx 0x62b56c24
#define STM32F439IGxx 0x0f0eb22a
#define STM32F439IIxx 0x4d2b0f2c
#define STM32F439BGxx 0xa154ac73
#define STM32F439BIxx 0x26325003
#define STM32F439NGxx 0x9116f6fe
#define STM32F439NIxx 0x21cdcfdd
#define STM32F439AIxx 0x58a1e5fa
#define STM32F446MCxx 0xd1e2ddb5
#define STM32F446RCxx 0x7c18c38f
#define STM32F446VCxx 0xf7042668
#define STM32F446ZCxx 0xb350ab41
#define STM32F446MExx 0xb2802300
#define STM32F446RExx 0x03910afc
#define STM32F446VExx 0x6c438ed9
#define STM32F446ZExx 0x08d316f7
#define STM32F469VExx 0xb328f67d
#define STM32F469VGxx 0xad89bd4a
#define STM32F469VIxx 0x88e70887
#define STM32F469ZExx 0x3c700f2c
#define STM32F469ZGxx 0x706c43cf
#define STM32F469ZIxx 0xc1cf1fd0
#define STM32F469AExx 0x0950c6d8
#define STM32F469AGxx 0xdc7dcfd0
#define STM32F469AIxx 0x46f6fdd0
#define STM32F469IExx 0x62c0aa09
#define STM32F469IGxx 0x77b87b7e
#define STM32F469IIxx 0xc1b57bd2
#define STM32F469BExx 0x33921015
#define STM32F469BGxx 0x6627c118
#define STM32F469BIxx 0xedc84160
#define STM32F469NExx 0x9df4d6f3
#define STM32F469NGxx 0x03285058
#define STM32F469NIxx 0xd4a513ce
#define STM32F479VGxx 0xd666a055
#define STM32F479VIxx 0x1220456d
#define STM32F479ZGxx 0xc34bb5ae
#define STM32F479ZIxx 0x40309daf
#define STM32F479AGxx 0x7e082105
#define STM32F479AIxx 0x16f944bf
#define STM32F479IGxx 0x2e18c204
#define STM32F479IIxx 0xb53b1cae
#define STM32F479BGxx 0x25408851
#define STM32F479BIxx 0xb4ad8b7f
#define STM32F479NGxx 0x472ba53e
#define STM32F479NIxx 0x15ca5794

#// CPU name definitions - required by stm32f4xx.h file.
#define _STM32F405xx 0x96ad9b3f
#define _STM32F415xx 0x9118fc01
#define _STM32F407xx 0x7bce51ae
#define _STM32F417xx 0x485e3d60
#define _STM32F427xx 0x141ccc15
#define _STM32F437xx 0xfb2e26b7
#define _STM32F429xx 0x82c72cde
#define _STM32F439xx 0xe4b4d3bb
#define _STM32F401xC 0x5b83a60f
#define _STM32F401xE 0x48809db4
#define _STM32F410Tx 0x744160f4
#define _STM32F410Cx 0x6f14d8f5
#define _STM32F410Rx 0x7a13c30e
#define _STM32F411xE 0x5a832cce
#define _STM32F446xx 0x87225faa
#define _STM32F469xx 0x1a8dd782
#define _STM32F479xx 0xb6e70484
#define _STM32F412Cx 0xc051f8b5
#define _STM32F412Rx 0xeab25c98
#define _STM32F412Vx 0x7c402b21
#define _STM32F412Zx 0x25faea4b
#define _STM32F413xx 0xa6a5298d
#define _STM32F423xx 0x5b87660e

#if   (__CPU_SERIES__ == _STM32F405xx)
#define STM32F405xx
#elif (__CPU_SERIES__ == _STM32F415xx)
#define STM32F415xx
#elif (__CPU_SERIES__ == _STM32F407xx)
#define STM32F407xx
#elif (__CPU_SERIES__ == _STM32F417xx)
#define STM32F417xx
#elif (__CPU_SERIES__ == _STM32F427xx)
#define STM32F427xx
#elif (__CPU_SERIES__ == _STM32F437xx)
#define STM32F437xx
#elif (__CPU_SERIES__ == _STM32F429xx)
#define STM32F429xx
#elif (__CPU_SERIES__ == _STM32F439xx)
#define STM32F439xx
#elif (__CPU_SERIES__ == _STM32F401xC)
#define STM32F401xC
#elif (__CPU_SERIES__ == _STM32F401xE)
#define STM32F401xE
#elif (__CPU_SERIES__ == _STM32F410Tx)
#define STM32F410Tx
#elif (__CPU_SERIES__ == _STM32F410Cx)
#define STM32F410Cx
#elif (__CPU_SERIES__ == _STM32F410Rx)
#define STM32F410Rx
#elif (__CPU_SERIES__ == _STM32F411xE)
#define STM32F411xE
#elif (__CPU_SERIES__ == _STM32F446xx)
#define STM32F446xx
#elif (__CPU_SERIES__ == _STM32F469xx)
#define STM32F469xx
#elif (__CPU_SERIES__ == _STM32F479xx)
#define STM32F479xx
#elif (__CPU_SERIES__ == _STM32F412Cx)
#define STM32F412Cx
#elif (__CPU_SERIES__ == _STM32F412Rx)
#define STM32F412Rx
#elif (__CPU_SERIES__ == _STM32F412Vx)
#define STM32F412Vx
#elif (__CPU_SERIES__ == _STM32F412Zx)
#define STM32F412Zx
#elif (__CPU_SERIES__ == _STM32F413xx)
#define STM32F413xx
#elif (__CPU_SERIES__ == _STM32F423xx)
#define STM32F423xx
#else
#error Unknown CPU name
#endif


#// current CPU family definitions
#define _STM32F4XX_ADVANCEDLINE_   0x3c0a227d
#define _STM32F4XX_FOUNDATIONLINE_ 0x28d9944c
#define _STM32F4XX_ACCESSLINE_     0x0616593c
#if   (__CPU_FAMILY__ == _STM32F4XX_ADVANCEDLINE_)
#define STM32F4XX_ADVANCEDLINE
#elif (__CPU_FAMILY__ == _STM32F4XX_FOUNDATIONLINE_)
#define STM32F4XX_FOUNDATIONLINE
#elif (__CPU_FAMILY__ == _STM32F4XX_ACCESSLINE_)
#define STM32F4XX_ACCESSLINE
#else
#error Wrong CPU family
#endif


#endif /* _CPU_FLAGS_H_ */
#/*=============================================================================
#  End of file
#=============================================================================*/
