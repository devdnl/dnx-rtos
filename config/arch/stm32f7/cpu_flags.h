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
#define __CPU_FAMILY__ _STM32F7XX_

#/*-- Flag is set in __CPU_NAME__ event
# this:AddWidget("Value")
#--*/
#define __CPU_SERIES__ _STM32F756xx

#/*-- Flag is set in __CPU_NAME__ event
# this:AddWidget("Value")
#--*/
#define __LD_SCRIPT__ STM32F75xxGxx
#/*
__LD_SCRIPT__=STM32F75xxGxx
#*/

#/*--
# local CPULIST = {
#                  {"STM32F722ICxx", "_STM32F722xx", "STM32F72xxCxx"},
#                  {"STM32F722IExx", "_STM32F722xx", "STM32F72xxExx"},
#                  {"STM32F722RCxx", "_STM32F722xx", "STM32F72xxCxx"},
#                  {"STM32F722RExx", "_STM32F722xx", "STM32F72xxExx"},
#                  {"STM32F722VCxx", "_STM32F722xx", "STM32F72xxCxx"},
#                  {"STM32F722VExx", "_STM32F722xx", "STM32F72xxExx"},
#                  {"STM32F722ZCxx", "_STM32F722xx", "STM32F72xxCxx"},
#                  {"STM32F722ZExx", "_STM32F722xx", "STM32F72xxExx"},
#                  {"STM32F723ICxx", "_STM32F723xx", "STM32F72xxCxx"},
#                  {"STM32F723IExx", "_STM32F723xx", "STM32F72xxExx"},
#                  {"STM32F723VCxx", "_STM32F723xx", "STM32F72xxCxx"},
#                  {"STM32F723VExx", "_STM32F723xx", "STM32F72xxExx"},
#                  {"STM32F723ZCxx", "_STM32F723xx", "STM32F72xxCxx"},
#                  {"STM32F723ZExx", "_STM32F723xx", "STM32F72xxExx"},
#                  {"STM32F730I8xx", "_STM32F730xx", "STM32F73xx8xx"},
#                  {"STM32F730R8xx", "_STM32F730xx", "STM32F73xx8xx"},
#                  {"STM32F730V8xx", "_STM32F730xx", "STM32F73xx8xx"},
#                  {"STM32F730Z8xx", "_STM32F730xx", "STM32F73xx8xx"},
#                  {"STM32F732IExx", "_STM32F732xx", "STM32F73xxExx"},
#                  {"STM32F732RExx", "_STM32F732xx", "STM32F73xxExx"},
#                  {"STM32F732VExx", "_STM32F732xx", "STM32F73xxExx"},
#                  {"STM32F732ZExx", "_STM32F732xx", "STM32F73xxExx"},
#                  {"STM32F733IExx", "_STM32F733xx", "STM32F73xxExx"},
#                  {"STM32F733VExx", "_STM32F733xx", "STM32F73xxExx"},
#                  {"STM32F733ZExx", "_STM32F733xx", "STM32F73xxExx"},
#                  {"STM32F745IExx", "_STM32F745xx", "STM32F74xxExx"},
#                  {"STM32F745IGxx", "_STM32F745xx", "STM32F74xxGxx"},
#                  {"STM32F745VExx", "_STM32F745xx", "STM32F74xxExx"},
#                  {"STM32F745VGxx", "_STM32F745xx", "STM32F74xxGxx"},
#                  {"STM32F745ZExx", "_STM32F745xx", "STM32F74xxExx"},
#                  {"STM32F745ZGxx", "_STM32F745xx", "STM32F74xxGxx"},
#                  {"STM32F746BExx", "_STM32F746xx", "STM32F74xxExx"},
#                  {"STM32F746BGxx", "_STM32F746xx", "STM32F74xxGxx"},
#                  {"STM32F746IExx", "_STM32F746xx", "STM32F74xxExx"},
#                  {"STM32F746IGxx", "_STM32F746xx", "STM32F74xxGxx"},
#                  {"STM32F746NExx", "_STM32F746xx", "STM32F74xxExx"},
#                  {"STM32F746NGxx", "_STM32F746xx", "STM32F74xxGxx"},
#                  {"STM32F746VExx", "_STM32F746xx", "STM32F74xxExx"},
#                  {"STM32F746VGxx", "_STM32F746xx", "STM32F74xxGxx"},
#                  {"STM32F746ZExx", "_STM32F746xx", "STM32F74xxExx"},
#                  {"STM32F746ZGxx", "_STM32F746xx", "STM32F74xxGxx"},
#                  {"STM32F750N8xx", "_STM32F750xx", "STM32F75xx8xx"},
#                  {"STM32F750V8xx", "_STM32F750xx", "STM32F75xx8xx"},
#                  {"STM32F750Z8xx", "_STM32F750xx", "STM32F75xx8xx"},
#                  {"STM32F756BGxx", "_STM32F756xx", "STM32F75xxGxx"},
#                  {"STM32F756IGxx", "_STM32F756xx", "STM32F75xxGxx"},
#                  {"STM32F756NGxx", "_STM32F756xx", "STM32F75xxGxx"},
#                  {"STM32F756VGxx", "_STM32F756xx", "STM32F75xxGxx"},
#                  {"STM32F756ZGxx", "_STM32F756xx", "STM32F75xxGxx"},
#                  {"STM32F765BGxx", "_STM32F765xx", "STM32F76xxGxx"},
#                  {"STM32F765BIxx", "_STM32F765xx", "STM32F76xxIxx"},
#                  {"STM32F765IGxx", "_STM32F765xx", "STM32F76xxGxx"},
#                  {"STM32F765IIxx", "_STM32F765xx", "STM32F76xxIxx"},
#                  {"STM32F765NGxx", "_STM32F765xx", "STM32F76xxGxx"},
#                  {"STM32F765NIxx", "_STM32F765xx", "STM32F76xxIxx"},
#                  {"STM32F765VGxx", "_STM32F765xx", "STM32F76xxGxx"},
#                  {"STM32F765VIxx", "_STM32F765xx", "STM32F76xxIxx"},
#                  {"STM32F765ZGxx", "_STM32F765xx", "STM32F76xxGxx"},
#                  {"STM32F765ZIxx", "_STM32F765xx", "STM32F76xxIxx"},
#                  {"STM32F767BGxx", "_STM32F767xx", "STM32F76xxGxx"},
#                  {"STM32F767BIxx", "_STM32F767xx", "STM32F76xxIxx"},
#                  {"STM32F767IGxx", "_STM32F767xx", "STM32F76xxGxx"},
#                  {"STM32F767IIxx", "_STM32F767xx", "STM32F76xxIxx"},
#                  {"STM32F767NGxx", "_STM32F767xx", "STM32F76xxGxx"},
#                  {"STM32F767NIxx", "_STM32F767xx", "STM32F76xxIxx"},
#                  {"STM32F767VGxx", "_STM32F767xx", "STM32F76xxGxx"},
#                  {"STM32F767VIxx", "_STM32F767xx", "STM32F76xxIxx"},
#                  {"STM32F767ZGxx", "_STM32F767xx", "STM32F76xxGxx"},
#                  {"STM32F767ZIxx", "_STM32F767xx", "STM32F76xxIxx"},
#                  {"STM32F768AIxx", "_STM32F769xx", "STM32F76xxIxx"},
#                  {"STM32F769AGxx", "_STM32F769xx", "STM32F76xxGxx"},
#                  {"STM32F769AIxx", "_STM32F769xx", "STM32F76xxIxx"},
#                  {"STM32F769BGxx", "_STM32F769xx", "STM32F76xxGxx"},
#                  {"STM32F769BIxx", "_STM32F769xx", "STM32F76xxIxx"},
#                  {"STM32F769IGxx", "_STM32F769xx", "STM32F76xxGxx"},
#                  {"STM32F769IIxx", "_STM32F769xx", "STM32F76xxIxx"},
#                  {"STM32F769NGxx", "_STM32F769xx", "STM32F76xxGxx"},
#                  {"STM32F769NIxx", "_STM32F769xx", "STM32F76xxIxx"},
#                  {"STM32F777BIxx", "_STM32F777xx", "STM32F77xxIxx"},
#                  {"STM32F777IIxx", "_STM32F777xx", "STM32F77xxIxx"},
#                  {"STM32F777NIxx", "_STM32F777xx", "STM32F77xxIxx"},
#                  {"STM32F777VIxx", "_STM32F777xx", "STM32F77xxIxx"},
#                  {"STM32F777ZIxx", "_STM32F777xx", "STM32F77xxIxx"},
#                  {"STM32F778AIxx", "_STM32F779xx", "STM32F77xxIxx"},
#                  {"STM32F779AIxx", "_STM32F779xx", "STM32F77xxIxx"},
#                  {"STM32F779BIxx", "_STM32F779xx", "STM32F77xxIxx"},
#                  {"STM32F779IIxx", "_STM32F779xx", "STM32F77xxIxx"},
#                  {"STM32F779NIxx", "_STM32F779xx", "STM32F77xxIxx"},
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
#define __CPU_NAME__ STM32F756ZGxx
#/*
__CPU_NAME__=STM32F756ZGxx
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
#define __CPU_VTOR_TAB_POSITION__ 0x00000

#//-----------------------------------------------------------------------------
#// mandatory flags, not configurable
#//-----------------------------------------------------------------------------
#define _CPU_START_FREQUENCY_           (16000000UL)
#define _CPU_HEAP_ALIGN_                (4)
#define _CPU_IRQ_RTOS_KERNEL_PRIORITY_  (__CPU_IRQ_RTOS_KERNEL_PRIORITY_VAL__ << 4)
#define _CPU_IRQ_RTOS_APICALL_PRIORITY_ (__CPU_IRQ_RTOS_APICALL_PRIORITY_VAL__ << 4)
#define _CPU_IRQ_SAFE_PRIORITY_         (__CPU_IRQ_RTOS_KERNEL_PRIORITY_VAL__)
#define ARCH_stm32f7
#/*
CPUCONFIG_AFLAGS=-mcpu=cortex-m7 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -ffast-math -fno-math-errno -mthumb-interwork -DGCC_ARMCM7
CPUCONFIG_CFLAGS=-mcpu=cortex-m7 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -ffast-math -fno-math-errno -mthumb-interwork -DGCC_ARMCM7
CPUCONFIG_CXXFLAGS=-mcpu=cortex-m7 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -ffast-math -fno-math-errno -mthumb-interwork -DGCC_ARMCM7
CPUCONFIG_LDFLAGS=-mcpu=cortex-m7 -mthumb -mthumb-interwork -nostartfiles -T./src/system/cpu/stm32f7/ld/$(__LD_SCRIPT__).ld
#*/

#// All CPU names definitions - general usage
#define STM32F722ICxx 0x8ddb344f
#define STM32F722IExx 0x02d7f54f
#define STM32F722RCxx 0x1d6cd97b
#define STM32F722RExx 0x8aae4440
#define STM32F722VCxx 0xd1fc1f0f
#define STM32F722VExx 0x5cd74dec
#define STM32F722ZCxx 0x7bcf949c
#define STM32F722ZExx 0x001e49c1
#define STM32F723ICxx 0x57b50fee
#define STM32F723IExx 0x9388fce4
#define STM32F723VCxx 0x9a83e566
#define STM32F723VExx 0x18b7976a
#define STM32F723ZCxx 0xb8d02c63
#define STM32F723ZExx 0x99ae3fcb
#define STM32F730I8xx 0x1906c7c2
#define STM32F730R8xx 0x63bc681f
#define STM32F730V8xx 0x31712729
#define STM32F730Z8xx 0x89838e0f
#define STM32F732IExx 0x7e452809
#define STM32F732RExx 0xcdc63acd
#define STM32F732VExx 0xac326b96
#define STM32F732ZExx 0xb63c703b
#define STM32F733IExx 0x1b630379
#define STM32F733VExx 0x076b811c
#define STM32F733ZExx 0x8a7b6a68
#define STM32F745IExx 0xe3ed5fce
#define STM32F745IGxx 0xfe3e404b
#define STM32F745VExx 0x2f90adb0
#define STM32F745VGxx 0xad971a7f
#define STM32F745ZExx 0x8e499edd
#define STM32F745ZGxx 0x6b6456e9
#define STM32F746BExx 0x553a0f49
#define STM32F746BGxx 0x52a5fe7c
#define STM32F746IExx 0x4055292b
#define STM32F746IGxx 0x9f952ada
#define STM32F746NExx 0xb695b781
#define STM32F746NGxx 0x786fbc29
#define STM32F746VExx 0xd52f7d56
#define STM32F746VGxx 0x71007680
#define STM32F746ZExx 0x3f623548
#define STM32F746ZGxx 0xb750fd14
#define STM32F750N8xx 0xd73ebcdd
#define STM32F750V8xx 0xd0688b11
#define STM32F750Z8xx 0xe6c10e5b
#define STM32F756BGxx 0xbd6d6b35
#define STM32F756IGxx 0x4cfb2697
#define STM32F756NGxx 0x91c10878
#define STM32F756VGxx 0x94c5551a
#define STM32F756ZGxx 0xaf59930c
#define STM32F765BGxx 0xd8f34eaf
#define STM32F765BIxx 0x93a4eda3
#define STM32F765IGxx 0x9896aaf7
#define STM32F765IIxx 0x224aec55
#define STM32F765NGxx 0x266f2a4c
#define STM32F765NIxx 0xf297962e
#define STM32F765VGxx 0x89245ab3
#define STM32F765VIxx 0x01351e72
#define STM32F765ZGxx 0xf04f8dc2
#define STM32F765ZIxx 0x474db9cb
#define STM32F767BGxx 0xc6e9152a
#define STM32F767BIxx 0x27e179a1
#define STM32F767IGxx 0x3858f8b4
#define STM32F767IIxx 0x3d70526c
#define STM32F767NGxx 0x560c6e96
#define STM32F767NIxx 0x924f8f91
#define STM32F767VGxx 0xdd39f225
#define STM32F767VIxx 0xa048affc
#define STM32F767ZGxx 0x2adadb39
#define STM32F767ZIxx 0xf621e823
#define STM32F768AIxx 0x38c4ee49
#define STM32F769AGxx 0x233b1e8b
#define STM32F769AIxx 0xc73d4723
#define STM32F769BGxx 0xe5d81e4e
#define STM32F769BIxx 0x0f7ec5b0
#define STM32F769IGxx 0x64724d80
#define STM32F769IIxx 0xae589fe2
#define STM32F769NGxx 0x043d6cab
#define STM32F769NIxx 0x2fb3bdf5
#define STM32F777BIxx 0x61151451
#define STM32F777IIxx 0xb8ea40b8
#define STM32F777NIxx 0xcff034fe
#define STM32F777VIxx 0xede167b0
#define STM32F777ZIxx 0xfc8ec601
#define STM32F778AIxx 0xefc2dd3e
#define STM32F779AIxx 0xf04a9d3b
#define STM32F779BIxx 0xe6392931
#define STM32F779IIxx 0x0020485d
#define STM32F779NIxx 0x4d55be47

#// CPU name definitions - required by stm32f7xx.h file.
#define _STM32F722xx 0x5b83a60f
#define _STM32F723xx 0x48809db4
#define _STM32F730xx 0x7a13c30e
#define _STM32F732xx 0x744160f4
#define _STM32F733xx 0x6f14d8f5
#define _STM32F745xx 0x7bce51ae
#define _STM32F746xx 0x9118fc01
#define _STM32F750xx 0x5a832cce
#define _STM32F756xx 0x96ad9b3f
#define _STM32F765xx 0x485e3d60
#define _STM32F767xx 0x141ccc15
#define _STM32F769xx 0xfb2e26b7
#define _STM32F777xx 0x82c72cde
#define _STM32F779xx 0xe4b4d3bb

#if   (__CPU_SERIES__ == _STM32F722xx)
#define STM32F722xx
#elif (__CPU_SERIES__ == _STM32F723xx)
#define STM32F723xx
#elif (__CPU_SERIES__ == _STM32F730xx)
#define STM32F730xx
#elif (__CPU_SERIES__ == _STM32F732xx)
#define STM32F732xx
#elif (__CPU_SERIES__ == _STM32F733xx)
#define STM32F733xx
#elif (__CPU_SERIES__ == _STM32F745xx)
#define STM32F745xx
#elif (__CPU_SERIES__ == _STM32F746xx)
#define STM32F746xx
#elif (__CPU_SERIES__ == _STM32F750xx)
#define STM32F750xx
#elif (__CPU_SERIES__ == _STM32F756xx)
#define STM32F756xx
#elif (__CPU_SERIES__ == _STM32F765xx)
#define STM32F765xx
#elif (__CPU_SERIES__ == _STM32F767xx)
#define STM32F767xx
#elif (__CPU_SERIES__ == _STM32F769xx)
#define STM32F769xx
#elif (__CPU_SERIES__ == _STM32F777xx)
#define STM32F777xx
#elif (__CPU_SERIES__ == _STM32F779xx)
#define STM32F779xx
#else
#error Unknown CPU name
#endif

#endif /* _CPU_FLAGS_H_ */
#/*=============================================================================
#  End of file
#=============================================================================*/
