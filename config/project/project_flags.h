#/*=============================================================================
# @file    project_flags.h
#
# @author  Daniel Zorychta
#
# @brief   This file contains base project configuration.
#          Hybrid file: included both by Make and CC.
#
# @note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>
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


#ifndef _PROJECT_FLAGS_H_
#define _PROJECT_FLAGS_H_

#/*--
# this:SetLayout("TitledGridBack", 2, "Home > Project",
#                function() this:LoadFile("config.h") end)
#++*/

#/*--
# this:AddExtraWidget("Label", "LabelBasic", "Basic", -1, "bold")
# this:AddExtraWidget("Void", "VoidBasic")
#++*/

#/*--
# this:AddWidget("Editline", false, "Project name")
# this:SetToolTip("This is a name of your project. The string is used to generate names of output files.")
#--*/
#define __PROJECT_NAME__ dnx
#/*
__PROJECT_NAME__=dnx
#*/

#/*--
# this:AddWidget("Editline", false, "Toolchain prefix")
# this:SetToolTip("Enter a name of first part of your toolchain's name, example:\n"..
#                 " - Linaro, CodeSourcery: arm-none-eabi-\n"..
#                 " - GCC: $(none)")
#--*/
#define __PROJECT_TOOLCHAIN__ arm-none-eabi-
#/*
none=
__PROJECT_TOOLCHAIN__=arm-none-eabi-
#*/


#/*--
# this:AddExtraWidget("Label", "LabelBoard", "\nBoard", -1, "bold")
# this:AddExtraWidget("Void", "VoidBoard")
#++*/
#// Existing architectures
#define stm32f1 0x16F54349
#define stm32f2 0x2f31e2fc
#define stm32f3 0x483962d4
#define stm32f4 0x8dd787b6
#define stm32f7 0x3cb2ba7c
#define efr32   0xcd975039
#/*--
# this:AddWidget("Combobox", "CPU architecture")
# this:AddItem("STMicroelectronics STM32F1", "stm32f1")
# this:AddItem("STMicroelectronics STM32F4", "stm32f4")
# this:AddItem("STMicroelectronics STM32F7", "stm32f7")
# this:AddItem("Silicon Labs EFR32 Mighty Gecko (experimental)", "efr32")
#--*/
#define __CPU_ARCH__ stm32f7
#/*
__CPU_ARCH__=stm32f7
#*/

#/*--
# this:AddWidget("Spinbox", 1e6, 100e6, "Oscillator frequency [Hz]")
# this:SetToolTip("This is a frequency of a connected to the microcontroller external generator or crystal.")
#--*/
#define __CPU_OSC_FREQ__ 8000000

#endif /* _PROJECT_FLAGS_H_ */
#/*=============================================================================
#  End of file
#=============================================================================*/
