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
#define __CPU_FAMILY__ _STM32F10X_MD_

#/*--
# local CPULIST = {{"STM32F100C8xx", "_STM32F10X_MD_VL_"},
#                  {"STM32F100RBxx", "_STM32F10X_MD_VL_"},
#                  {"STM32F100RExx", "_STM32F10X_HD_VL_"},
#                  {"STM32F100VBxx", "_STM32F10X_MD_VL_"},
#                  {"STM32F100ZCxx", "_STM32F10X_HD_VL_"},
#                  {"STM32F101C8xx", "_STM32F10X_MD_"   },
#                  {"STM32F101CBxx", "_STM32F10X_MD_"   },
#                  {"STM32F101R8xx", "_STM32F10X_MD_"   },
#                  {"STM32F101RBxx", "_STM32F10X_MD_"   },
#                  {"STM32F101V8xx", "_STM32F10X_MD_"   },
#                  {"STM32F101VBxx", "_STM32F10X_MD_"   },
#                  {"STM32F103C6xx", "_STM32F10X_LD_"   },
#                  {"STM32F103C8xx", "_STM32F10X_MD_"   },
#                  {"STM32F103CBxx", "_STM32F10X_MD_"   },
#                  {"STM32F103R6xx", "_STM32F10X_LD_"   },
#                  {"STM32F103R8xx", "_STM32F10X_MD_"   },
#                  {"STM32F103RBxx", "_STM32F10X_MD_"   },
#                  {"STM32F103RCxx", "_STM32F10X_HD_"   },
#                  {"STM32F103RDxx", "_STM32F10X_HD_"   },
#                  {"STM32F103RExx", "_STM32F10X_HD_"   },
#                  {"STM32F103RGxx", "_STM32F10X_XL_"   },
#                  {"STM32F103T8xx", "_STM32F10X_MD_"   },
#                  {"STM32F103TBxx", "_STM32F10X_MD_"   },
#                  {"STM32F103V8xx", "_STM32F10X_MD_"   },
#                  {"STM32F103VBxx", "_STM32F10X_MD_"   },
#                  {"STM32F103VCxx", "_STM32F10X_HD_"   },
#                  {"STM32F103VDxx", "_STM32F10X_HD_"   },
#                  {"STM32F103VExx", "_STM32F10X_HD_"   },
#                  {"STM32F103VGxx", "_STM32F10X_XL_"   },
#                  {"STM32F103ZCxx", "_STM32F10X_HD_"   },
#                  {"STM32F103ZExx", "_STM32F10X_HD_"   },
#                  {"STM32F103ZGxx", "_STM32F10X_XL_"   },
#                  {"STM32F105RBxx", "_STM32F10X_CL_"   },
#                  {"STM32F105RCxx", "_STM32F10X_CL_"   },
#                  {"STM32F105VBxx", "_STM32F10X_CL_"   },
#                  {"STM32F105VCxx", "_STM32F10X_CL_"   },
#                  {"STM32F107RCxx", "_STM32F10X_CL_"   },
#                  {"STM32F107VBxx", "_STM32F10X_CL_"   },
#                  {"STM32F107VCxx", "_STM32F10X_CL_"   }}
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
#                break
#            end
#        end
#    end
# )
#--*/
#define __CPU_NAME__ STM32F103TBxx
#/*
__CPU_NAME__=STM32F103TBxx
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
#define __CPU_IRQ_RTOS_APICALL_PRIORITY_VAL__ 14

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
#define __CPU_DISABLE_INTER_OF_MCYCLE_INSTR__ _NO_

#/*--
# this:AddWidget("Editline", false, "Interrupt vector position")
#--*/
#define __CPU_VTOR_TAB_POSITION__ 0x00000

#//-----------------------------------------------------------------------------
#// mandatory flags, not configurable
#//-----------------------------------------------------------------------------
#define _CPU_START_FREQUENCY_           (8000000UL)
#define _CPU_HEAP_ALIGN_                (4)
#define _CPU_IRQ_RTOS_KERNEL_PRIORITY_  (__CPU_IRQ_RTOS_KERNEL_PRIORITY_VAL__ << 4)
#define _CPU_IRQ_RTOS_APICALL_PRIORITY_ (__CPU_IRQ_RTOS_APICALL_PRIORITY_VAL__ << 4)
#define _CPU_IRQ_SAFE_PRIORITY_         (__CPU_IRQ_RTOS_KERNEL_PRIORITY_VAL__)
#define ARCH_stm32f1
#/*
CPUCONFIG_AFLAGS=-mcpu=cortex-m3 -mthumb -ffast-math -fno-math-errno -mthumb-interwork -DGCC_ARMCM3
CPUCONFIG_CFLAGS=-mcpu=cortex-m3 -mthumb -ffast-math -fno-math-errno -mthumb-interwork -DGCC_ARMCM3
CPUCONFIG_CXXFLAGS=-mcpu=cortex-m3 -mthumb -ffast-math -fno-math-errno -mthumb-interwork -DGCC_ARMCM3
CPUCONFIG_LDFLAGS=-mcpu=cortex-m3 -mthumb -mthumb-interwork -nostartfiles -T./src/system/cpu/stm32f1/ld/$(__CPU_NAME__).ld
#*/

#// current CPU family definitions
#define _STM32F10X_LD_VL_ 0x124024b9
#define _STM32F10X_MD_VL_ 0x05293309
#define _STM32F10X_HD_VL_ 0x9df242be
#define _STM32F10X_LD_    0xcfd6148d
#define _STM32F10X_MD_    0xd1bd0a00
#define _STM32F10X_HD_    0x894592ef
#define _STM32F10X_XL_    0xe09e531d
#define _STM32F10X_CL_    0x570c51ca
#if   (__CPU_FAMILY__ == _STM32F10X_LD_VL_)
#define STM32F10X_LD_VL
#elif (__CPU_FAMILY__ == _STM32F10X_MD_VL_)
#define STM32F10X_MD_VL
#elif (__CPU_FAMILY__ == _STM32F10X_HD_VL_)
#define STM32F10X_HD_VL
#elif (__CPU_FAMILY__ == _STM32F10X_LD_)
#define STM32F10X_LD
#elif (__CPU_FAMILY__ == _STM32F10X_MD_)
#define STM32F10X_MD
#elif (__CPU_FAMILY__ == _STM32F10X_HD_)
#define STM32F10X_HD
#elif (__CPU_FAMILY__ == _STM32F10X_XL_)
#define STM32F10X_XL
#elif (__CPU_FAMILY__ == _STM32F10X_CL_)
#define STM32F10X_CL
#else
#error Wrong CPU family
#endif

#endif /* _CPU_FLAGS_H_ */
#/*=============================================================================
#  End of file
#=============================================================================*/
