#/*=============================================================================
# @file    cpu_flags.h
#
# @author  Daniel Zorychta
#
# @brief   This file contains CPU configuration flags.
#          Hybrid file: included both by Make and CC.
#
# @note    Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>
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
#                  {"STM32F410T8Yx", "_STM32F4XX_ACCESSLINE_", "_STM32F410Tx", "STM32F410x8xx"},
#                  {"STM32F410TBYx", "_STM32F4XX_ACCESSLINE_", "_STM32F410Tx", "STM32F410xBxx"},
#                  {"STM32F410C8Ux", "_STM32F4XX_ACCESSLINE_", "_STM32F410Cx", "STM32F410x8xx"},
#                  {"STM32F410CBUx", "_STM32F4XX_ACCESSLINE_", "_STM32F410Cx", "STM32F410xBxx"},
#                  {"STM32F410C8Tx", "_STM32F4XX_ACCESSLINE_", "_STM32F410Cx", "STM32F410x8xx"},
#                  {"STM32F410CBTx", "_STM32F4XX_ACCESSLINE_", "_STM32F410Cx", "STM32F410xBxx"},
#                  {"STM32F410R8Tx", "_STM32F4XX_ACCESSLINE_", "_STM32F410Rx", "STM32F410x8xx"},
#                  {"STM32F410RBTx", "_STM32F4XX_ACCESSLINE_", "_STM32F410Rx", "STM32F410xBxx"},
#                  {"STM32F410R8Ix", "_STM32F4XX_ACCESSLINE_", "_STM32F410Rx", "STM32F410x8xx"},
#                  {"STM32F410RBIx", "_STM32F4XX_ACCESSLINE_", "_STM32F410Rx", "STM32F410xBxx"},
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
#define __CPU_NAME__ STM32F429ZIxx
#/*
__CPU_NAME__=STM32F429ZIxx
#*/

#/*--
# this:AddWidget("Combobox", "Default IRQ priority")
# uC.AddPriorityItems(this, true)
#--*/
#define __CPU_IRQ_USER_PRIORITY__ 0xDF


#//-----------------------------------------------------------------------------
#// mandatory flags, not configurable
#//-----------------------------------------------------------------------------
#define _CPU_START_FREQUENCY_           (16000000UL)
#define _CPU_HEAP_ALIGN_                (4)
#define _CPU_IRQ_RTOS_KERNEL_PRIORITY_  (0xFF)
#define _CPU_IRQ_RTOS_SYSCALL_PRIORITY_ (0xEF)
#define ARCH_stm32f4
#/*
CPUCONFIG_AFLAGS=-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -ffast-math -mthumb-interwork -DGCC_ARMCM4
CPUCONFIG_CFLAGS=-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -ffast-math -mthumb-interwork -DGCC_ARMCM4
CPUCONFIG_CXXFLAGS=-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -ffast-math -mthumb-interwork -DGCC_ARMCM4
CPUCONFIG_LDFLAGS=-mcpu=cortex-m4 -mthumb -mthumb-interwork -nostartfiles -T./src/system/portable/stm32f4/ld/$(__LD_SCRIPT__).ld
#*/

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
