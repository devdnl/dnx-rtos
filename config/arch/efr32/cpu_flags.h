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
#define __CPU_CODE__ _EFR32MG1V132F256GM32
#/*
__CPU_CODE__=_EFR32MG1V132F256GM32
#*/

#/*-- Flag is set in __CPU_NAME__ event
# this:AddWidget("Value")
#--*/
#define __CPU_FAMILY__ MIGHTY_GECKO
#/*
__CPU_FAMILY__=MIGHTY_GECKO

#*/
#/*-- Flag is set in __CPU_NAME__ event
# this:AddWidget("Value")
#--*/
#define __LD_SCRIPT__ EFR32_256_32
#/*
__LD_SCRIPT__=EFR32_256_32
#*/

#/*--
# local CPULIST = {
#                  {"EFR32MG12P132F1024GL125", "_EFR32MG12P132F1024GL125", "EFR32_1024_128"},
#                  {"EFR32MG12P132F1024GM48" , "_EFR32MG12P132F1024GM48" , "EFR32_1024_128"},
#                  {"EFR32MG12P232F1024GL125", "_EFR32MG12P232F1024GL125", "EFR32_1024_128"},
#                  {"EFR32MG12P232F1024GM48" , "_EFR32MG12P232F1024GM48" , "EFR32_1024_128"},
#                  {"EFR32MG12P332F1024GL125", "_EFR32MG12P332F1024GL125", "EFR32_1024_256"},
#                  {"EFR32MG12P332F1024GM48" , "_EFR32MG12P332F1024GM48" , "EFR32_1024_256"},
#                  {"EFR32MG12P432F1024GL125", "_EFR32MG12P432F1024GL125", "EFR32_1024_256"},
#                  {"EFR32MG12P432F1024GM48" , "_EFR32MG12P432F1024GM48" , "EFR32_1024_256"},
#                  {"EFR32MG12P433F1024GL125", "_EFR32MG12P433F1024GL125", "EFR32_1024_256"},
#                  {"EFR32MG12P433F1024GM48" , "_EFR32MG12P433F1024GM48" , "EFR32_1024_256"},
#                  {"EFR32MG1B132F256GM32"   , "_EFR32MG1B132F256GM32"   , "EFR32_256_32"},
#                  {"EFR32MG1B132F256GM48"   , "_EFR32MG1B132F256GM48"   , "EFR32_256_32"},
#                  {"EFR32MG1B232F256GM32"   , "_EFR32MG1B232F256GM32"   , "EFR32_256_32"},
#                  {"EFR32MG1B232F256GM48"   , "_EFR32MG1B232F256GM48"   , "EFR32_256_32"},
#                  {"EFR32MG1B632F256GM32"   , "_EFR32MG1B632F256GM32"   , "EFR32_256_32"},
#                  {"EFR32MG1B732F256GM32"   , "_EFR32MG1B732F256GM32"   , "EFR32_256_32"},
#                  {"EFR32MG1P132F256GM32"   , "_EFR32MG1P132F256GM32"   , "EFR32_256_32"},
#                  {"EFR32MG1P132F256GM48"   , "_EFR32MG1P132F256GM48"   , "EFR32_256_32"},
#                  {"EFR32MG1P133F256GM48"   , "_EFR32MG1P133F256GM48"   , "EFR32_256_32"},
#                  {"EFR32MG1P232F256GM32"   , "_EFR32MG1P232F256GM32"   , "EFR32_256_32"},
#                  {"EFR32MG1P232F256GM48"   , "_EFR32MG1P232F256GM48"   , "EFR32_256_32"},
#                  {"EFR32MG1P233F256GM48"   , "_EFR32MG1P233F256GM48"   , "EFR32_256_32"},
#                  {"EFR32MG1P632F256GM32"   , "_EFR32MG1P632F256GM32"   , "EFR32_256_32"},
#                  {"EFR32MG1P732F256GM32"   , "_EFR32MG1P732F256GM32"   , "EFR32_256_32"},
#                  {"EFR32MG1V132F256GM32"   , "_EFR32MG1V132F256GM32"   , "EFR32_256_32"},
#                  {"EFR32MG1V132F256GM48"   , "_EFR32MG1V132F256GM48"   , "EFR32_256_32"},
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
#                this:SetFlagValue("__CPU_CODE__",  CPULIST[i][2])
#                this:SetFlagValue("__LD_SCRIPT__",  CPULIST[i][3])
#                break
#            end
#        end
#    end
# )
#--*/
#define __CPU_NAME__ EFR32MG1V132F256GM32
#/*
__CPU_NAME__=EFR32MG1V132F256GM32
#*/

#/*--
# this:AddWidget("Combobox", "Default IRQ priority")
# uC.AddPriorityItems(this, true)
#--*/
#define __CPU_DEFAULT_IRQ_PRIORITY__ 5


#//-----------------------------------------------------------------------------
#// mandatory flags, not configurable
#//-----------------------------------------------------------------------------
#define _CPU_START_FREQUENCY_           (19000000UL)
#define _CPU_HEAP_ALIGN_                (4)
#define _CPU_IRQ_RTOS_KERNEL_PRIORITY_  (7 << 5)
#define _CPU_IRQ_RTOS_SYSCALL_PRIORITY_ (6 << 5)
#define ARCH_efr32
#/*
CPUCONFIG_AFLAGS=-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -ffast-math -mthumb-interwork -DGCC_ARMCM4
CPUCONFIG_CFLAGS=-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -ffast-math -mthumb-interwork -DGCC_ARMCM4
CPUCONFIG_CXXFLAGS=-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -ffast-math -mthumb-interwork -DGCC_ARMCM4
CPUCONFIG_LDFLAGS=-mcpu=cortex-m4 -mthumb -mthumb-interwork -nostartfiles -T./src/system/portable/efr32/ld/$(__LD_SCRIPT__).ld
#*/

#// CPU family
#define MIGHTY_GECKO             0x8926f2ea

#// All CPU names definitions - general usage
#define _EFR32MG12P132F1024GL125 0x8ddb344f
#define _EFR32MG12P132F1024GM48  0x02d7f54f
#define _EFR32MG12P232F1024GL125 0x1d6cd97b
#define _EFR32MG12P232F1024GM48  0x8aae4440
#define _EFR32MG12P332F1024GL125 0xd1fc1f0f
#define _EFR32MG12P332F1024GM48  0x5cd74dec
#define _EFR32MG12P432F1024GL125 0x7bcf949c
#define _EFR32MG12P432F1024GM48  0x001e49c1
#define _EFR32MG12P433F1024GL125 0x57b50fee
#define _EFR32MG12P433F1024GM48  0x9388fce4
#define _EFR32MG1B132F256GM32    0x9a83e566
#define _EFR32MG1B132F256GM48    0x18b7976a
#define _EFR32MG1B232F256GM32    0xb8d02c63
#define _EFR32MG1B232F256GM48    0x99ae3fcb
#define _EFR32MG1B632F256GM32    0x1906c7c2
#define _EFR32MG1B732F256GM32    0x63bc681f
#define _EFR32MG1P132F256GM32    0x31712729
#define _EFR32MG1P132F256GM48    0x89838e0f
#define _EFR32MG1P133F256GM48    0x7e452809
#define _EFR32MG1P232F256GM32    0xcdc63acd
#define _EFR32MG1P232F256GM48    0xac326b96
#define _EFR32MG1P233F256GM48    0xb63c703b
#define _EFR32MG1P632F256GM32    0x1b630379
#define _EFR32MG1P732F256GM32    0x076b811c
#define _EFR32MG1V132F256GM32    0x8a7b6a68
#define _EFR32MG1V132F256GM48    0xe3ed5fce

#if __CPU_CODE__ == _EFR32MG12P132F1024GL125
#define EFR32MG12P132F1024GL125
#elif __CPU_CODE__ == _EFR32MG12P132F1024GM48
#define EFR32MG12P132F1024GM48
#elif __CPU_CODE__ == _EFR32MG12P232F1024GL125
#define EFR32MG12P232F1024GL125
#elif __CPU_CODE__ == _EFR32MG12P232F1024GM48
#define EFR32MG12P232F1024GM48
#elif __CPU_CODE__ == _EFR32MG12P332F1024GL125
#define EFR32MG12P332F1024GL125
#elif __CPU_CODE__ == _EFR32MG12P332F1024GM48
#define EFR32MG12P332F1024GM48
#elif __CPU_CODE__ == _EFR32MG12P432F1024GL125
#define EFR32MG12P432F1024GL125
#elif __CPU_CODE__ == _EFR32MG12P432F1024GM48
#define EFR32MG12P432F1024GM48
#elif __CPU_CODE__ == _EFR32MG12P433F1024GL125
#define EFR32MG12P433F1024GL125
#elif __CPU_CODE__ == _EFR32MG12P433F1024GM48
#define EFR32MG12P433F1024GM48
#elif __CPU_CODE__ == _EFR32MG1B132F256GM32
#define EFR32MG1B132F256GM32
#elif __CPU_CODE__ == _EFR32MG1B132F256GM48
#define EFR32MG1B132F256GM48
#elif __CPU_CODE__ == _EFR32MG1B232F256GM32
#define EFR32MG1B232F256GM32
#elif __CPU_CODE__ == _EFR32MG1B232F256GM48
#define EFR32MG1B232F256GM48
#elif __CPU_CODE__ == _EFR32MG1B632F256GM32
#define EFR32MG1B632F256GM32
#elif __CPU_CODE__ == _EFR32MG1B732F256GM32
#define EFR32MG1B732F256GM32
#elif __CPU_CODE__ == _EFR32MG1P132F256GM32
#define EFR32MG1P132F256GM32
#elif __CPU_CODE__ == _EFR32MG1P132F256GM48
#define EFR32MG1P132F256GM48
#elif __CPU_CODE__ == _EFR32MG1P133F256GM48
#define EFR32MG1P133F256GM48
#elif __CPU_CODE__ == _EFR32MG1P232F256GM32
#define EFR32MG1P232F256GM32
#elif __CPU_CODE__ == _EFR32MG1P232F256GM48
#define EFR32MG1P232F256GM48
#elif __CPU_CODE__ == _EFR32MG1P233F256GM48
#define EFR32MG1P233F256GM48
#elif __CPU_CODE__ == _EFR32MG1P632F256GM32
#define EFR32MG1P632F256GM32
#elif __CPU_CODE__ == _EFR32MG1P732F256GM32
#define EFR32MG1P732F256GM32
#elif __CPU_CODE__ == _EFR32MG1V132F256GM32
#define EFR32MG1V132F256GM32
#elif __CPU_CODE__ == _EFR32MG1V132F256GM48
#define EFR32MG1V132F256GM48
#else
#error "Unknown CPU"
#endif

#endif /* _CPU_FLAGS_H_ */
#/*=============================================================================
#  End of file
#=============================================================================*/
