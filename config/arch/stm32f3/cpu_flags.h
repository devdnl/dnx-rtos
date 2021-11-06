#/*=============================================================================
# @file    cpu_flags.h
#
# @author  Daniel Zorychta
#
# @brief   This file contains CPU configuration flags.
#          Hybrid file: included both by Make and CC.
#
# @note    Copyright (C) 2020 Daniel Zorychta <daniel.zorychta@gmail.com>
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
#define __CPU_FAMILY__ _STM32F3XX_

#/*-- Flag is set in __CPU_NAME__ event
# this:AddWidget("Value")
#--*/
#define __CPU_SERIES__ _STM32F303xE

#/*-- Flag is set in __CPU_NAME__ event
# this:AddWidget("Value")
#--*/
#define __LD_SCRIPT__ STM32F3xx_512_64_16
#/*
__LD_SCRIPT__=STM32F3xx_512_64_16
#*/

#/*--
# local CPULIST = {
#                  {"STM32F301C6xx", "_STM32F301x8", "STM32F3xx_32_16"},
#                  {"STM32F301C8xx", "_STM32F301x8", "STM32F3xx_64_16"},
#                  {"STM32F301K6xx", "_STM32F301x8", "STM32F3xx_32_16"},
#                  {"STM32F301K8xx", "_STM32F301x8", "STM32F3xx_64_16"},
#                  {"STM32F301R6xx", "_STM32F301x8", "STM32F3xx_32_16"},
#                  {"STM32F301R8xx", "_STM32F301x8", "STM32F3xx_64_16"},
#                  {"STM32F302C6xx", "_STM32F302x8", "STM32F3xx_32_16"},
#                  {"STM32F302C8xx", "_STM32F302x8", "STM32F3xx_64_16"},
#                  {"STM32F302CBxx", "_STM32F302x8", "STM32F3xx_128_32"},
#                  {"STM32F302CCxx", "_STM32F302x8", "STM32F3xx_256_40"},
#                  {"STM32F302K6xx", "_STM32F302x8", "STM32F3xx_32_16"},
#                  {"STM32F302K8xx", "_STM32F302x8", "STM32F3xx_64_16"},
#                  {"STM32F302R6xx", "_STM32F302x8", "STM32F3xx_32_16"},
#                  {"STM32F302R8xx", "_STM32F302x8", "STM32F3xx_64_16"},
#                  {"STM32F302RBxx", "_STM32F302xC", "STM32F3xx_128_32"},
#                  {"STM32F302RCxx", "_STM32F302xC", "STM32F3xx_256_40"},
#                  {"STM32F302RDxx", "_STM32F302xE", "STM32F3xx_384_64"},
#                  {"STM32F302RExx", "_STM32F302xE", "STM32F3xx_512_64"},
#                  {"STM32F302VBxx", "_STM32F302xC", "STM32F3xx_128_32"},
#                  {"STM32F302VCxx", "_STM32F302xC", "STM32F3xx_256_40"},
#                  {"STM32F302VDxx", "_STM32F302xE", "STM32F3xx_384_64"},
#                  {"STM32F302VExx", "_STM32F302xE", "STM32F3xx_512_64"},
#                  {"STM32F302ZDxx", "_STM32F302xE", "STM32F3xx_384_64"},
#                  {"STM32F302ZExx", "_STM32F302xE", "STM32F3xx_512_64"},
#                  {"STM32F303C6xx", "_STM32F303x8", "STM32F3xx_32_16_4"},
#                  {"STM32F303C8xx", "_STM32F303x8", "STM32F3xx_32_16_4"},
#                  {"STM32F303CBxx", "_STM32F303xC", "STM32F3xx_128_32_8"},
#                  {"STM32F303CCxx", "_STM32F303xC", "STM32F3xx_256_40_8"},
#                  {"STM32F303K6xx", "_STM32F303x8", "STM32F3xx_32_16_4"},
#                  {"STM32F303K8xx", "_STM32F303x8", "STM32F3xx_32_16_4"},
#                  {"STM32F303R6xx", "_STM32F303x8", "STM32F3xx_32_16_4"},
#                  {"STM32F303R8xx", "_STM32F303x8", "STM32F3xx_32_16_4"},
#                  {"STM32F303RBxx", "_STM32F303xC", "STM32F3xx_128_32_8"},
#                  {"STM32F303RCxx", "_STM32F303xC", "STM32F3xx_256_40_8"},
#                  {"STM32F303RDxx", "_STM32F303xE", "STM32F3xx_384_64_16"},
#                  {"STM32F303RExx", "_STM32F303xE", "STM32F3xx_512_64_16"},
#                  {"STM32F303VBxx", "_STM32F303xC", "STM32F3xx_128_32_8"},
#                  {"STM32F303VCxx", "_STM32F303xC", "STM32F3xx_256_40_8"},
#                  {"STM32F303VDxx", "_STM32F303xE", "STM32F3xx_384_64_16"},
#                  {"STM32F303VExx", "_STM32F303xE", "STM32F3xx_512_64_16"},
#                  {"STM32F303ZDxx", "_STM32F303xE", "STM32F3xx_384_64_16"},
#                  {"STM32F303ZExx", "_STM32F303xE", "STM32F3xx_512_64_16"},
#                  {"STM32F318C8xx", "_STM32F318xx", "STM32F3xx_64_16"},
#                  {"STM32F318K8xx", "_STM32F318xx", "STM32F3xx_64_16"},
#                  {"STM32F328C8xx", "_STM32F328xx", "STM32F3xx_64_12_4"},
#                  {"STM32F334C4xx", "_STM32F334x8", "STM32F3xx_16_12_4"},
#                  {"STM32F334C6xx", "_STM32F334x8", "STM32F3xx_32_12_4"},
#                  {"STM32F334C8xx", "_STM32F334x8", "STM32F3xx_64_12_4"},
#                  {"STM32F334K4xx", "_STM32F334x8", "STM32F3xx_16_12_4"},
#                  {"STM32F334K6xx", "_STM32F334x8", "STM32F3xx_32_12_4"},
#                  {"STM32F334K8xx", "_STM32F334x8", "STM32F3xx_64_12_4"},
#                  {"STM32F334R6xx", "_STM32F334x8", "STM32F3xx_32_12_4"},
#                  {"STM32F334R8xx", "_STM32F334x8", "STM32F3xx_64_12_4"},
#                  {"STM32F358CCxx", "_STM32F358xx", "STM32F3xx_256_40_8"},
#                  {"STM32F358RCxx", "_STM32F358xx", "STM32F3xx_256_40_8"},
#                  {"STM32F358VCxx", "_STM32F358xx", "STM32F3xx_256_40_8"},
#                  {"STM32F373C8xx", "_STM32F373xC", "STM32F3xx_64_16"},
#                  {"STM32F373CBxx", "_STM32F373xC", "STM32F3xx_128_24"},
#                  {"STM32F373CCxx", "_STM32F373xC", "STM32F3xx_256_32"},
#                  {"STM32F373R8xx", "_STM32F373xC", "STM32F3xx_64_16"},
#                  {"STM32F373RBxx", "_STM32F373xC", "STM32F3xx_128_24"},
#                  {"STM32F373RCxx", "_STM32F373xC", "STM32F3xx_256_32"},
#                  {"STM32F373V8xx", "_STM32F373xC", "STM32F3xx_64_16"},
#                  {"STM32F373VBxx", "_STM32F373xC", "STM32F3xx_128_24"},
#                  {"STM32F373VCxx", "_STM32F373xC", "STM32F3xx_256_32"},
#                  {"STM32F378CCxx", "_STM32F378xx", "STM32F3xx_256_32"},
#                  {"STM32F378RCxx", "_STM32F378xx", "STM32F3xx_256_32"},
#                  {"STM32F378VCxx", "_STM32F378xx", "STM32F3xx_256_32"},
#                  {"STM32F398VExx", "_STM32F398xx", "STM32F3xx_512_64_16"},
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
#                this:SetFlagValue("__CPU_SERIES__", CPULIST[i][2])
#                this:SetFlagValue("__LD_SCRIPT__",  CPULIST[i][3])
#                break
#            end
#        end
#    end
# )
#--*/
#define __CPU_NAME__ STM32F303ZExx
#/*
__CPU_NAME__=STM32F303ZExx
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
# this:AddWidget("Combobox", "Show and clear reset source")
# this:AddItem("Disable", "_NO_")
# this:AddItem("Enable", "_YES_")
#--*/
#define __CPU_SHOW_AND_CLEAR_RESET_SOURCE__ _YES_

#/*--
# this:AddWidget("Editline", false, "Interrupt vector position")
#--*/
#define __CPU_VTOR_TAB_POSITION__ 0x00000

#/*--
# this:AddWidget("Combobox", "Custom linker script")
# this:AddItem("No", "_NO_")
# this:AddItem("Yes", "_YES_")
#--*/
#define __CUSTOM_LD_SCRIPT__ _NO_
#/*
__CUSTOM_LD_SCRIPT__=_NO_
#*/

#/*--
# this:AddWidget("Editline", true, "Custom linker script file path")
#--*/
#define __CUSTOM_LD_SCRIPT_FILE__ "./config/custom_script.ld"
#/*
__CUSTOM_LD_SCRIPT_FILE__="./config/custom_script.ld"
#*/

#//-----------------------------------------------------------------------------
#// mandatory flags, not configurable
#//-----------------------------------------------------------------------------
#define _CPU_START_FREQUENCY_           (16000000UL)
#define _CPU_HEAP_ALIGN_                (4)
#define _CPU_IRQ_RTOS_KERNEL_PRIORITY_  (__CPU_IRQ_RTOS_KERNEL_PRIORITY_VAL__ << 4)
#define _CPU_IRQ_RTOS_APICALL_PRIORITY_ (__CPU_IRQ_RTOS_APICALL_PRIORITY_VAL__ << 4)
#define _CPU_IRQ_SAFE_PRIORITY_         (__CPU_IRQ_RTOS_KERNEL_PRIORITY_VAL__)
#define ARCH_stm32f3
#/*
CPUCONFIG_AFLAGS=-mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -ffast-math -fno-math-errno -mthumb-interwork -DGCC_ARMCM4
CPUCONFIG_CFLAGS=-mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -ffast-math -fno-math-errno -mthumb-interwork -DGCC_ARMCM4
CPUCONFIG_CXXFLAGS=-mcpu=cortex-m4 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -ffast-math -fno-math-errno -mthumb-interwork -DGCC_ARMCM4
CPUCONFIG_LDFLAGS=-mcpu=cortex-m4 -mthumb -mthumb-interwork -nostartfiles

ifeq ($(__CUSTOM_LD_SCRIPT__), _YES_)
CPUCONFIG_LDFLAGS += -T$(__CUSTOM_LD_SCRIPT_FILE__)
else
CPUCONFIG_LDFLAGS += -T./src/system/cpu/stm32f3/ld/$(__LD_SCRIPT__).ld
endif

#*/

#// All CPU names definitions - general usage
#define STM32F301C6xx 0xd1fc1f0f
#define STM32F301C8xx 0x5cd74dec
#define STM32F301K6xx 0x7bcf949c
#define STM32F301K8xx 0x001e49c1
#define STM32F301R6xx 0x57b50fee
#define STM32F301R8xx 0x9388fce4
#define STM32F302C6xx 0x9a83e566
#define STM32F302C8xx 0x18b7976a
#define STM32F302CBxx 0xb8d02c63
#define STM32F302CCxx 0x99ae3fcb
#define STM32F302K6xx 0x1906c7c2
#define STM32F302K8xx 0x63bc681f
#define STM32F302R6xx 0x31712729
#define STM32F302R8xx 0x89838e0f
#define STM32F302RBxx 0x8ddb344f
#define STM32F302RCxx 0x7e452809
#define STM32F302RDxx 0xcdc63acd
#define STM32F302RExx 0xac326b96
#define STM32F302VBxx 0xb63c703b
#define STM32F302VCxx 0x1b630379
#define STM32F302VDxx 0x076b811c
#define STM32F302VExx 0x8a7b6a68
#define STM32F302ZDxx 0xe3ed5fce
#define STM32F302ZExx 0xfe3e404b
#define STM32F303C6xx 0x2f90adb0
#define STM32F303C8xx 0xad971a7f
#define STM32F303CBxx 0x8e499edd
#define STM32F303CCxx 0x6b6456e9
#define STM32F303K6xx 0x553a0f49
#define STM32F303K8xx 0x52a5fe7c
#define STM32F303R6xx 0x4055292b
#define STM32F303R8xx 0x02d7f54f
#define STM32F303RBxx 0x9f952ada
#define STM32F303RCxx 0xb695b781
#define STM32F303RDxx 0x786fbc29
#define STM32F303RExx 0x1d6cd97b
#define STM32F303VBxx 0xd52f7d56
#define STM32F303VCxx 0x71007680
#define STM32F303VDxx 0x3f623548
#define STM32F303VExx 0xb750fd14
#define STM32F303ZDxx 0xd73ebcdd
#define STM32F303ZExx 0xd0688b11
#define STM32F318C8xx 0xe6c10e5b
#define STM32F318K8xx 0xbd6d6b35
#define STM32F328C8xx 0x4cfb2697
#define STM32F334C4xx 0x91c10878
#define STM32F334C6xx 0x94c5551a
#define STM32F334C8xx 0xaf59930c
#define STM32F334K4xx 0x06dd58e9
#define STM32F334K6xx 0xd8f34eaf
#define STM32F334K8xx 0x8aae4440
#define STM32F334R6xx 0x93a4eda3
#define STM32F334R8xx 0x9896aaf7
#define STM32F358CCxx 0x224aec55
#define STM32F358RCxx 0x266f2a4c
#define STM32F358VCxx 0xf297962e
#define STM32F373C8xx 0x89245ab3
#define STM32F373CBxx 0x01351e72
#define STM32F373CCxx 0xf04f8dc2
#define STM32F373R8xx 0x474db9cb
#define STM32F373RBxx 0xc6e9152a
#define STM32F373RCxx 0x27e179a1
#define STM32F373V8xx 0x3858f8b4
#define STM32F373VBxx 0x3d70526c
#define STM32F373VCxx 0x560c6e96
#define STM32F378CCxx 0x924f8f91
#define STM32F378RCxx 0xdd39f225
#define STM32F378VCxx 0xa048affc
#define STM32F398VExx 0x2adadb39

#// CPU name definitions - required by stm32f3xx.h file.
#define _STM32F301x8 0x96ad9b3f
#define _STM32F302x8 0x9118fc01
#define _STM32F302xC 0x7bce51ae
#define _STM32F302xE 0x485e3d60
#define _STM32F303x8 0x141ccc15
#define _STM32F303xC 0xfb2e26b7
#define _STM32F303xE 0x82c72cde
#define _STM32F373xC 0xe4b4d3bb
#define _STM32F334x8 0x5b83a60f
#define _STM32F318xx 0x48809db4
#define _STM32F328xx 0x744160f4
#define _STM32F358xx 0x6f14d8f5
#define _STM32F378xx 0x7a13c30e
#define _STM32F398xx 0x5a832cce

#if   (__CPU_SERIES__ == _STM32F301x8)
#define STM32F301x8
#elif (__CPU_SERIES__ == _STM32F302x8)
#define STM32F302x8
#elif (__CPU_SERIES__ == _STM32F302xC)
#define STM32F302xC
#elif (__CPU_SERIES__ == _STM32F302xE)
#define STM32F302xE
#elif (__CPU_SERIES__ == _STM32F303x8)
#define STM32F303x8
#elif (__CPU_SERIES__ == _STM32F303xC)
#define STM32F303xC
#elif (__CPU_SERIES__ == _STM32F303xE)
#define STM32F303xE
#elif (__CPU_SERIES__ == _STM32F373xC)
#define STM32F373xC
#elif (__CPU_SERIES__ == _STM32F334x8)
#define STM32F334x8
#elif (__CPU_SERIES__ == _STM32F318xx)
#define STM32F318xx
#elif (__CPU_SERIES__ == _STM32F328xx)
#define STM32F328xx
#elif (__CPU_SERIES__ == _STM32F358xx)
#define STM32F358xx
#elif (__CPU_SERIES__ == _STM32F378xx)
#define STM32F378xx
#elif (__CPU_SERIES__ == _STM32F398xx)
#define STM32F398xx
#else
#error Unknown CPU name
#endif

#endif /* _CPU_FLAGS_H_ */
#/*=============================================================================
#  End of file
#=============================================================================*/
