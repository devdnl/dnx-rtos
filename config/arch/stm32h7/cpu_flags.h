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
#define __CPU_FAMILY__ _STM32H7XX_

#/*-- Flag is set in __CPU_NAME__ event
# this:AddWidget("Value")
#--*/
#define __CPU_SERIES__ _STM32H753xx

#/*-- Flag is set in __CPU_NAME__ event
# this:AddWidget("Value")
#--*/
#define __LD_SCRIPT__ STM32H753xIxx
#/*
__LD_SCRIPT__=STM32H753xIxx
#*/

#/*--
# local CPULIST = {
#                  {"STM32H723VExx",  "_STM32H723xx",  "STM32H7xxxExx"},
#                  {"STM32H723VGxx",  "_STM32H723xx",  "STM32H7xxxGxx"},
#                  {"STM32H723ZExx",  "_STM32H723xx",  "STM32H7xxxExx"},
#                  {"STM32H723ZGxx",  "_STM32H723xx",  "STM32H7xxxGxx"},
#                  {"STM32H725AExx",  "_STM32H725xx",  "STM32H7xxxExx"},
#                  {"STM32H725AGxx",  "_STM32H725xx",  "STM32H7xxxGxx"},
#                  {"STM32H725IExx",  "_STM32H725xx",  "STM32H7xxxExx"},
#                  {"STM32H725IGxx",  "_STM32H725xx",  "STM32H7xxxGxx"},
#                  {"STM32H725RExx",  "_STM32H725xx",  "STM32H7xxxExx"},
#                  {"STM32H725RGxx",  "_STM32H725xx",  "STM32H7xxxGxx"},
#                  {"STM32H725VExx",  "_STM32H725xx",  "STM32H7xxxExx"},
#                  {"STM32H725VGxx",  "_STM32H725xx",  "STM32H7xxxGxx"},
#                  {"STM32H725ZExx",  "_STM32H725xx",  "STM32H7xxxExx"},
#                  {"STM32H725ZGxx",  "_STM32H725xx",  "STM32H7xxxGxx"},
#                  {"STM32H730ABxxQ", "_STM32H730xxQ", "STM32H7xxxBxx"},
#                  {"STM32H730IBxxQ", "_STM32H730xxQ", "STM32H7xxxBxx"},
#                  {"STM32H730VBxx",  "_STM32H730xx",  "STM32H7xxxBxx"},
#                  {"STM32H730ZBxx",  "_STM32H730xx",  "STM32H7xxxBxx"},
#                  {"STM32H733VGxx",  "_STM32H733xx",  "STM32H7xxxGxx"},
#                  {"STM32H733ZGxx",  "_STM32H733xx",  "STM32H7xxxGxx"},
#                  {"STM32H735AGxx",  "_STM32H735xx",  "STM32H7xxxGxx"},
#                  {"STM32H735IGxx",  "_STM32H735xx",  "STM32H7xxxGxx"},
#                  {"STM32H735RGxx",  "_STM32H735xx",  "STM32H7xxxGxx"},
#                  {"STM32H735VGxx",  "_STM32H735xx",  "STM32H7xxxGxx"},
#                  {"STM32H735ZGxx",  "_STM32H735xx",  "STM32H7xxxGxx"},
#                  {"STM32H742AGxx",  "_STM32H742xx",  "STM32H742xGxx"},
#                  {"STM32H742BGxx",  "_STM32H742xx",  "STM32H742xGxx"},
#                  {"STM32H742IGxx",  "_STM32H742xx",  "STM32H742xGxx"},
#                  {"STM32H742VGxx",  "_STM32H742xx",  "STM32H742xGxx"},
#                  {"STM32H742XGxx",  "_STM32H742xx",  "STM32H742xGxx"},
#                  {"STM32H742ZGxx",  "_STM32H742xx",  "STM32H742xGxx"},
#                  {"STM32H742AIxx",  "_STM32H742xx",  "STM32H742xIxx"},
#                  {"STM32H742BIxx",  "_STM32H742xx",  "STM32H742xIxx"},
#                  {"STM32H742IIxx",  "_STM32H742xx",  "STM32H742xIxx"},
#                  {"STM32H742VIxx",  "_STM32H742xx",  "STM32H742xIxx"},
#                  {"STM32H742XIxx",  "_STM32H742xx",  "STM32H742xIxx"},
#                  {"STM32H742ZIxx",  "_STM32H742xx",  "STM32H742xIxx"},
#                  {"STM32H743AGxx",  "_STM32H743xx",  "STM32H743xGxx"},
#                  {"STM32H743BGxx",  "_STM32H743xx",  "STM32H743xGxx"},
#                  {"STM32H743IGxx",  "_STM32H743xx",  "STM32H743xGxx"},
#                  {"STM32H743VGxx",  "_STM32H743xx",  "STM32H743xGxx"},
#                  {"STM32H743XGxx",  "_STM32H743xx",  "STM32H743xGxx"},
#                  {"STM32H743ZGxx",  "_STM32H743xx",  "STM32H743xGxx"},
#                  {"STM32H743AIxx",  "_STM32H743xx",  "STM32H743xIxx"},
#                  {"STM32H743BIxx",  "_STM32H743xx",  "STM32H743xIxx"},
#                  {"STM32H743IIxx",  "_STM32H743xx",  "STM32H743xIxx"},
#                  {"STM32H743VIxx",  "_STM32H743xx",  "STM32H743xIxx"},
#                  {"STM32H743XIxx",  "_STM32H743xx",  "STM32H743xIxx"},
#                  {"STM32H743ZIxx",  "_STM32H743xx",  "STM32H743xIxx"},
#                  {"STM32H745BGxx",  "_STM32H745xx",  "STM32H745xGxx"},
#                  {"STM32H745IGxx",  "_STM32H745xx",  "STM32H745xGxx"},
#                  {"STM32H745XGxx",  "_STM32H745xx",  "STM32H745xGxx"},
#                  {"STM32H745ZGxx",  "_STM32H745xx",  "STM32H745xGxx"},
#                  {"STM32H745BIxx",  "_STM32H745xx",  "STM32H745xIxx"},
#                  {"STM32H745IIxx",  "_STM32H745xx",  "STM32H745xIxx"},
#                  {"STM32H745XIxx",  "_STM32H745xx",  "STM32H745xIxx"},
#                  {"STM32H745ZIxx",  "_STM32H745xx",  "STM32H745xIxx"},
#                  {"STM32H747AGxx",  "_STM32H747xx",  "STM32H747xGxx"},
#                  {"STM32H747BGxx",  "_STM32H747xx",  "STM32H747xGxx"},
#                  {"STM32H747IGxx",  "_STM32H747xx",  "STM32H747xGxx"},
#                  {"STM32H747XGxx",  "_STM32H747xx",  "STM32H747xGxx"},
#                  {"STM32H747ZGxx",  "_STM32H747xx",  "STM32H747xGxx"},
#                  {"STM32H747AIxx",  "_STM32H747xx",  "STM32H747xIxx"},
#                  {"STM32H747BIxx",  "_STM32H747xx",  "STM32H747xIxx"},
#                  {"STM32H747IIxx",  "_STM32H747xx",  "STM32H747xIxx"},
#                  {"STM32H747XIxx",  "_STM32H747xx",  "STM32H747xIxx"},
#                  {"STM32H747ZIxx",  "_STM32H747xx",  "STM32H747xIxx"},
#                  {"STM32H750IBxx",  "_STM32H750xx",  "STM32H750xBxx"},
#                  {"STM32H750VBxx",  "_STM32H750xx",  "STM32H750xBxx"},
#                  {"STM32H750XBxx",  "_STM32H750xx",  "STM32H750xBxx"},
#                  {"STM32H750ZBxx",  "_STM32H750xx",  "STM32H750xBxx"},
#                  {"STM32H753AIxx",  "_STM32H753xx",  "STM32H753xIxx"},
#                  {"STM32H753BIxx",  "_STM32H753xx",  "STM32H753xIxx"},
#                  {"STM32H753IIxx",  "_STM32H753xx",  "STM32H753xIxx"},
#                  {"STM32H753VIxx",  "_STM32H753xx",  "STM32H753xIxx"},
#                  {"STM32H753XIxx",  "_STM32H753xx",  "STM32H753xIxx"},
#                  {"STM32H753ZIxx",  "_STM32H753xx",  "STM32H753xIxx"},
#                  {"STM32H755BIxx",  "_STM32H755xx",  "STM32H755xIxx"},
#                  {"STM32H755IIxx",  "_STM32H755xx",  "STM32H755xIxx"},
#                  {"STM32H755XIxx",  "_STM32H755xx",  "STM32H755xIxx"},
#                  {"STM32H755ZIxx",  "_STM32H755xx",  "STM32H755xIxx"},
#                  {"STM32H757AIxx",  "_STM32H757xx",  "STM32H757xIxx"},
#                  {"STM32H757BIxx",  "_STM32H757xx",  "STM32H757xIxx"},
#                  {"STM32H757IIxx",  "_STM32H757xx",  "STM32H757xIxx"},
#                  {"STM32H757XIxx",  "_STM32H757xx",  "STM32H757xIxx"},
#                  {"STM32H757ZIxx",  "_STM32H757xx",  "STM32H757xIxx"},
#                  {"STM32H7A3AGxxQ", "_STM32H7A3xxQ", "STM32H7A3xGxx"},
#                  {"STM32H7A3IGxx",  "_STM32H7A3xx",  "STM32H7A3xGxx"},
#                  {"STM32H7A3IGxxQ", "_STM32H7A3xxQ", "STM32H7A3xGxx"},
#                  {"STM32H7A3LGxxQ", "_STM32H7A3xxQ", "STM32H7A3xGxx"},
#                  {"STM32H7A3NGxx",  "_STM32H7A3xx",  "STM32H7A3xGxx"},
#                  {"STM32H7A3QGxxQ", "_STM32H7A3xxQ", "STM32H7A3xGxx"},
#                  {"STM32H7A3RGxx",  "_STM32H7A3xx",  "STM32H7A3xGxx"},
#                  {"STM32H7A3VGxx",  "_STM32H7A3xx",  "STM32H7A3xGxx"},
#                  {"STM32H7A3VGxxQ", "_STM32H7A3xxQ", "STM32H7A3xGxx"},
#                  {"STM32H7A3ZGxx",  "_STM32H7A3xx",  "STM32H7A3xGxx"},
#                  {"STM32H7A3ZGxxQ", "_STM32H7A3xxQ", "STM32H7A3xGxx"},
#                  {"STM32H7A3AIxxQ", "_STM32H7A3xxQ", "STM32H7A3xIxx"},
#                  {"STM32H7A3IIxx",  "_STM32H7A3xx",  "STM32H7A3xIxx"},
#                  {"STM32H7A3IIxxQ", "_STM32H7A3xxQ", "STM32H7A3xIxx"},
#                  {"STM32H7A3LIxxQ", "_STM32H7A3xxQ", "STM32H7A3xIxx"},
#                  {"STM32H7A3NIxx",  "_STM32H7A3xx",  "STM32H7A3xIxx"},
#                  {"STM32H7A3QIxxQ", "_STM32H7A3xxQ", "STM32H7A3xIxx"},
#                  {"STM32H7A3RIxx",  "_STM32H7A3xx",  "STM32H7A3xIxx"},
#                  {"STM32H7A3VIxx",  "_STM32H7A3xx",  "STM32H7A3xIxx"},
#                  {"STM32H7A3VIxxQ", "_STM32H7A3xxQ", "STM32H7A3xIxx"},
#                  {"STM32H7A3ZIxx",  "_STM32H7A3xx",  "STM32H7A3xIxx"},
#                  {"STM32H7A3ZIxxQ", "_STM32H7A3xxQ", "STM32H7A3xIxx"},
#                  {"STM32H7B0ABxxQ", "_STM32H7B0AB",  "STM32H7B0xBxx"},
#                  {"STM32H7B0IBKxQ", "_STM32H7B0AB",  "STM32H7B0xBxx"},
#                  {"STM32H7B0IBxx",  "_STM32H7B0xx",  "STM32H7B0xBxx"},
#                  {"STM32H7B0RBxx",  "_STM32H7B0xx",  "STM32H7B0xBxx"},
#                  {"STM32H7B0VBxx",  "_STM32H7B0xx",  "STM32H7B0xBxx"},
#                  {"STM32H7B0ZBxx",  "_STM32H7B0xx",  "STM32H7B0xBxx"},
#                  {"STM32H7B3AIxxQ", "_STM32H7B3xxQ", "STM32H7B3xIxx"},
#                  {"STM32H7B3IIxx",  "_STM32H7B3xx",  "STM32H7B3xIxx"},
#                  {"STM32H7B3IIxxQ", "_STM32H7B3xxQ", "STM32H7B3xIxx"},
#                  {"STM32H7B3LIxxQ", "_STM32H7B3xxQ", "STM32H7B3xIxx"},
#                  {"STM32H7B3NIxx",  "_STM32H7B3xx",  "STM32H7B3xIxx"},
#                  {"STM32H7B3QIxxQ", "_STM32H7B3xxQ", "STM32H7B3xIxx"},
#                  {"STM32H7B3RIxx",  "_STM32H7B3xx",  "STM32H7B3xIxx"},
#                  {"STM32H7B3VIxx",  "_STM32H7B3xx",  "STM32H7B3xIxx"},
#                  {"STM32H7B3VIxxQ", "_STM32H7B3xxQ", "STM32H7B3xIxx"},
#                  {"STM32H7B3ZIxx",  "_STM32H7B3xx",  "STM32H7B3xIxx"},
#                  {"STM32H7B3ZIxxQ", "_STM32H7B3xxQ", "STM32H7B3xIxx"},
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
#define __CPU_NAME__ STM32H753ZIxx
#/*
__CPU_NAME__=STM32H753ZIxx
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
# this:AddWidget("Combobox", "ICACHE")
# this:AddItem("Enable", "_YES_")
# this:AddItem("Disable", "_NO_")
#--*/
#define __CPU_ICACHE_ENABLE__ _YES_

#/*--
# this:AddWidget("Combobox", "DCACHE")
# this:AddItem("Enable", "_YES_")
# this:AddItem("Disable", "_NO_")
# this:SetToolTip("This option can introduce cache coherency issues!")
#--*/
#define __CPU_DCACHE_ENABLE__ _NO_

#/*--
# this:AddWidget("Editline", false, "Interrupt vector position")
#--*/
#define __CPU_VTOR_TAB_POSITION__ 0x00000

#//-----------------------------------------------------------------------------
#// mandatory flags, not configurable
#//-----------------------------------------------------------------------------
#define _CPU_START_FREQUENCY_           (64000000UL)
#define _CPU_HEAP_ALIGN_                (4)
#define _CPU_IRQ_RTOS_KERNEL_PRIORITY_  (__CPU_IRQ_RTOS_KERNEL_PRIORITY_VAL__ << 4)
#define _CPU_IRQ_RTOS_APICALL_PRIORITY_ (__CPU_IRQ_RTOS_APICALL_PRIORITY_VAL__ << 4)
#define _CPU_IRQ_SAFE_PRIORITY_         (__CPU_IRQ_RTOS_KERNEL_PRIORITY_VAL__)
#define ARCH_stm32h7
#/*
CPUCONFIG_AFLAGS=-mcpu=cortex-m7 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -ffast-math -fno-math-errno -mthumb-interwork -DGCC_ARMCM7
CPUCONFIG_CFLAGS=-mcpu=cortex-m7 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -ffast-math -fno-math-errno -mthumb-interwork -DGCC_ARMCM7
CPUCONFIG_CXXFLAGS=-mcpu=cortex-m7 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -ffast-math -fno-math-errno -mthumb-interwork -DGCC_ARMCM7
CPUCONFIG_LDFLAGS=-mcpu=cortex-m7 -mthumb -mthumb-interwork -nostartfiles -T./src/system/cpu/stm32h7/ld/$(__LD_SCRIPT__).ld
#*/

#// All CPU names definitions - general usage
#define STM32H723VExx  0x8ddb344f
#define STM32H723VGxx  0x02d7f54f
#define STM32H723ZExx  0x1d6cd97b
#define STM32H723ZGxx  0x8aae4440
#define STM32H725AExx  0xd1fc1f0f
#define STM32H725AGxx  0x5cd74dec
#define STM32H725IExx  0x7bcf949c
#define STM32H725IGxx  0x001e49c1
#define STM32H725RExx  0x57b50fee
#define STM32H725RGxx  0x9388fce4
#define STM32H725VExx  0x9a83e566
#define STM32H725VGxx  0x18b7976a
#define STM32H725ZExx  0xb8d02c63
#define STM32H725ZGxx  0x99ae3fcb
#define STM32H730ABxxQ 0x1906c7c2
#define STM32H730IBxxQ 0x63bc681f
#define STM32H730VBxx  0x31712729
#define STM32H730ZBxx  0x89838e0f
#define STM32H733VGxx  0x7e452809
#define STM32H733ZGxx  0xcdc63acd
#define STM32H735AGxx  0xac326b96
#define STM32H735IGxx  0xb63c703b
#define STM32H735RGxx  0x1b630379
#define STM32H735VGxx  0x076b811c
#define STM32H735ZGxx  0x8a7b6a68
#define STM32H742AGxx  0xe3e82685
#define STM32H742BGxx  0x8ca40918
#define STM32H742IGxx  0xa6ede2d3
#define STM32H742VGxx  0xb22183b7
#define STM32H742XGxx  0x089b1410
#define STM32H742ZGxx  0x3255d45e
#define STM32H742AIxx  0xe3ed5fce
#define STM32H742BIxx  0xfe3e404b
#define STM32H742IIxx  0x2f90adb0
#define STM32H742VIxx  0xad971a7f
#define STM32H742XIxx  0x8e499edd
#define STM32H742ZIxx  0x6b6456e9
#define STM32H743AGxx  0x823ee586
#define STM32H743BGxx  0x40c8a819
#define STM32H743IGxx  0xde6ae3d2
#define STM32H743VGxx  0x182b27b3
#define STM32H743XGxx  0xb1809014
#define STM32H743ZGxx  0x5d235e54
#define STM32H743AIxx  0x553a0f49
#define STM32H743BIxx  0x52a5fe7c
#define STM32H743IIxx  0x4055292b
#define STM32H743VIxx  0x9f952ada
#define STM32H743XIxx  0xb695b781
#define STM32H743ZIxx  0x786fbc29
#define STM32H745BGxx  0xe21385ba
#define STM32H745IGxx  0x09de2c2c
#define STM32H745XGxx  0x75bf640a
#define STM32H745ZGxx  0x90122be6
#define STM32H745BIxx  0xd52f7d56
#define STM32H745IIxx  0x71007680
#define STM32H745XIxx  0x3f623548
#define STM32H745ZIxx  0xb750fd14
#define STM32H747AGxx  0x512a5399
#define STM32H747BGxx  0x559cf46b
#define STM32H747IGxx  0x591fa617
#define STM32H747XGxx  0xf3039ade
#define STM32H747ZGxx  0x34774fb0
#define STM32H747AIxx  0xd73ebcdd
#define STM32H747BIxx  0xd0688b11
#define STM32H747IIxx  0xe6c10e5b
#define STM32H747XIxx  0xbd6d6b35
#define STM32H747ZIxx  0x4cfb2697
#define STM32H750ZBxx  0x3f6fb4e6
#define STM32H750IBxx  0x91c10878
#define STM32H750VBxx  0x94c5551a
#define STM32H750XBxx  0xaf59930c
#define STM32H753AIxx  0xd8f34eaf
#define STM32H753BIxx  0x93a4eda3
#define STM32H753IIxx  0x9896aaf7
#define STM32H753VIxx  0x224aec55
#define STM32H753XIxx  0x266f2a4c
#define STM32H753ZIxx  0xf297962e
#define STM32H755BIxx  0x89245ab3
#define STM32H755IIxx  0x01351e72
#define STM32H755XIxx  0xf04f8dc2
#define STM32H755ZIxx  0x474db9cb
#define STM32H757AIxx  0xc6e9152a
#define STM32H757BIxx  0x27e179a1
#define STM32H757IIxx  0x3858f8b4
#define STM32H757XIxx  0x3d70526c
#define STM32H757ZIxx  0x560c6e96
#define STM32H7A3AIxxQ 0x924f8f91
#define STM32H7A3IIxx  0xdd39f225
#define STM32H7A3IIxxQ 0xa048affc
#define STM32H7A3LIxxQ 0x2adadb39
#define STM32H7A3NIxx  0xf621e823
#define STM32H7A3QIxxQ 0x38c4ee49
#define STM32H7A3RIxx  0x233b1e8b
#define STM32H7A3VIxx  0xc73d4723
#define STM32H7A3VIxxQ 0xe5d81e4e
#define STM32H7A3ZIxx  0x0f7ec5b0
#define STM32H7A3ZIxxQ 0x64724d80
#define STM32H7B0ABxxQ 0xae589fe2
#define STM32H7B0IBKxQ 0x043d6cab
#define STM32H7B0IBxx  0x2fb3bdf5
#define STM32H7B0RBxx  0x61151451
#define STM32H7B0VBxx  0xb8ea40b8
#define STM32H7B0ZBxx  0xcff034fe
#define STM32H7B3AIxxQ 0xede167b0
#define STM32H7B3IIxx  0xfc8ec601
#define STM32H7B3IIxxQ 0xefc2dd3e
#define STM32H7B3LIxxQ 0xf04a9d3b
#define STM32H7B3NIxx  0xe6392931
#define STM32H7B3QIxxQ 0x0020485d
#define STM32H7B3RIxx  0x4d55be47
#define STM32H7B3VIxx  0x2f32cf0d
#define STM32H7B3VIxxQ 0x82953c8e
#define STM32H7B3ZIxx  0x2de676c8
#define STM32H7B3ZIxxQ 0x69d740ab

#// CPU name definitions - required by stm32h7xx.h file.
#define _STM32H742xx  0x5b83a60f
#define _STM32H743xx  0x48809db4
#define _STM32H753xx  0x7a13c30e
#define _STM32H750xx  0x744160f4
#define _STM32H747xx  0x6f14d8f5
#define _STM32H757xx  0x7bce51ae
#define _STM32H745xx  0x9118fc01
#define _STM32H755xx  0x5a832cce
#define _STM32H7B0xx  0x96ad9b3f
#define _STM32H7A3xx  0x485e3d60
#define _STM32H7A3xxQ 0x141ccc15
#define _STM32H7B3xx  0xfb2e26b7
#define _STM32H7B3xxQ 0x82c72cde
#define _STM32H735xx  0xe4b4d3bb
#define _STM32H733xx  0x4d55be47
#define _STM32H730xx  0x2f32cf0d
#define _STM32H730xxQ 0x82953c8e
#define _STM32H725xx  0x2de676c8
#define _STM32H723xx  0x69d740ab

#if (__CPU_SERIES__ == _STM32H742xx )
#define STM32H742xx
#elif (__CPU_SERIES__ == _STM32H743xx )
#define STM32H743xx
#elif (__CPU_SERIES__ == _STM32H753xx )
#define STM32H753xx
#elif (__CPU_SERIES__ == _STM32H750xx )
#define STM32H750xx
#elif (__CPU_SERIES__ == _STM32H747xx )
#define STM32H747xx
#elif (__CPU_SERIES__ == _STM32H757xx )
#define STM32H757xx
#elif (__CPU_SERIES__ == _STM32H745xx )
#define STM32H745xx
#elif (__CPU_SERIES__ == _STM32H755xx )
#define STM32H755xx
#elif (__CPU_SERIES__ == _STM32H7B0xx )
#define STM32H7B0xx
#elif (__CPU_SERIES__ == _STM32H7A3xx )
#define STM32H7A3xx
#elif (__CPU_SERIES__ == _STM32H7A3xxQ)
#define STM32H7A3xxQ
#elif (__CPU_SERIES__ == _STM32H7B3xx )
#define STM32H7B3xx
#elif (__CPU_SERIES__ == _STM32H7B3xxQ)
#define STM32H7B3xxQ
#elif (__CPU_SERIES__ == _STM32H735xx )
#define STM32H735xx
#elif (__CPU_SERIES__ == _STM32H733xx )
#define STM32H733xx
#elif (__CPU_SERIES__ == _STM32H730xx )
#define STM32H730xx
#elif (__CPU_SERIES__ == _STM32H730xxQ)
#define STM32H730xxQ
#elif (__CPU_SERIES__ == _STM32H725xx )
#define STM32H725xx
#elif (__CPU_SERIES__ == _STM32H723xx )
#define STM32H723xx
#else
#error Unknown CPU name
#endif

#endif /* _CPU_FLAGS_H_ */
#/*=============================================================================
#  End of file
#=============================================================================*/
