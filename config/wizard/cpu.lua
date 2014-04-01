--[[============================================================================
@file    cpu.lua

@author  Daniel Zorychta

@brief   CPU configuration file.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the  Free Software  Foundation;  either version 2 of the License, or
         any later version.

         This  program  is  distributed  in the hope that  it will be useful,
         but  WITHOUT  ANY  WARRANTY;  without  even  the implied warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         You  should  have received a copy  of the GNU General Public License
         along  with  this  program;  if not,  write  to  the  Free  Software
         Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


*//*========================================================================--]]

require "defs"

--------------------------------------------------------------------------------
-- FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Function calculate a total configuration steps
--------------------------------------------------------------------------------
local function calculate_total_steps()
        local arch = get_cpu_arch()

        if (arch == "stm32f1") then
                set_total_steps(4)
        elseif (arch == "stm32f2") then
                set_total_steps(2)
        elseif (arch == "stm32f3") then
                set_total_steps(2)
        elseif (arch == "stm32f4") then
                set_total_steps(2)
        else
                set_total_steps(0)
        end
end

--------------------------------------------------------------------------------
-- @brief Function configure CPU architecture
--------------------------------------------------------------------------------
local function configure_cpu_arch()
        msg(progress() .. "CPU architecture configuration.")
        local choice = key_read("../project/Makefile", "PROJECT_CPU_ARCH")
        msg("Current CPU architecture is: " .. choice .. ".")
        add_item("stm32f1", "stm32f1 microcontroller family")
        add_item("stm32f2", "stm32f2 microcontroller family")
        add_item("stm32f3", "stm32f3 microcontroller family")
        add_item("stm32f4", "stm32f4 microcontroller family")
        choice = get_selection()
        if (can_be_saved(choice)) then
                set_cpu_arch(choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Configure CPU frequency
--------------------------------------------------------------------------------
local function configure_freq()
        msg(progress() .. "Configuration of oscillator frequency (generator, crystal, etc).")
        local value = key_read("../project/flags.h", "__CPU_OSC_FREQ__")
        msg("Current value is: " .. value .. " Hz.")
        value = get_number("dec", 0, 100e6)
        if (can_be_saved(value)) then
                key_save("../project/flags.h", "__CPU_OSC_FREQ__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Configure microcontroller for stm32f1 family
--------------------------------------------------------------------------------
local function configure_stm32f1_mcu()
        msg(progress() .. "Which microcontroller do you want to use?")
        local choice = key_read("../stm32f1/cpu.h", "__CPU_NAME__")
        msg("Current microcontroller is: " .. choice .. ".")
        add_item("STM32F100C4xx", "STM32F100C4xx")
        add_item("STM32F100C6xx", "STM32F100C6xx")
        add_item("STM32F100C8xx", "STM32F100C8xx")
        add_item("STM32F100R6xx", "STM32F100R6xx")
        add_item("STM32F100RBxx", "STM32F100RBxx")
        add_item("STM32F100RExx", "STM32F100RExx")
        add_item("STM32F100VBxx", "STM32F100VBxx")
        add_item("STM32F100ZCxx", "STM32F100ZCxx")
        add_item("STM32F101C6xx", "STM32F101C6xx")
        add_item("STM32F101C8xx", "STM32F101C8xx")
        add_item("STM32F101CBxx", "STM32F101CBxx")
        add_item("STM32F101R6xx", "STM32F101R6xx")
        add_item("STM32F101R8xx", "STM32F101R8xx")
        add_item("STM32F101RBxx", "STM32F101RBxx")
        add_item("STM32F101V8xx", "STM32F101V8xx")
        add_item("STM32F101VBxx", "STM32F101VBxx")
        add_item("STM32F103C6xx", "STM32F103C6xx")
        add_item("STM32F103C8xx", "STM32F103C8xx")
        add_item("STM32F103CBxx", "STM32F103CBxx")
        add_item("STM32F103R4xx", "STM32F103R4xx")
        add_item("STM32F103R6xx", "STM32F103R6xx")
        add_item("STM32F103R8xx", "STM32F103R8xx")
        add_item("STM32F103RBxx", "STM32F103RBxx")
        add_item("STM32F103RCxx", "STM32F103RCxx")
        add_item("STM32F103RDxx", "STM32F103RDxx")
        add_item("STM32F103RExx", "STM32F103RExx")
        add_item("STM32F103RGxx", "STM32F103RGxx")
        add_item("STM32F103T8xx", "STM32F103T8xx")
        add_item("STM32F103V8xx", "STM32F103V8xx")
        add_item("STM32F103VBxx", "STM32F103VBxx")
        add_item("STM32F103VCxx", "STM32F103VCxx")
        add_item("STM32F103VDxx", "STM32F103VDxx")
        add_item("STM32F103VExx", "STM32F103VExx")
        add_item("STM32F103VGxx", "STM32F103VGxx")
        add_item("STM32F103ZCxx", "STM32F103ZCxx")
        add_item("STM32F103ZExx", "STM32F103ZExx")
        add_item("STM32F103ZGxx", "STM32F103ZGxx")
        add_item("STM32F105RBxx", "STM32F105RBxx")
        add_item("STM32F105RCxx", "STM32F105RCxx")
        add_item("STM32F105VBxx", "STM32F105VBxx")
        add_item("STM32F105VCxx", "STM32F105VCxx")
        add_item("STM32F107RCxx", "STM32F107RCxx")
        add_item("STM32F107VBxx", "STM32F107VBxx")
        add_item("STM32F107VCxx", "STM32F107VCxx")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/cpu.h", "__CPU_NAME__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Configure IRQ priorities for stm32f1 family
--------------------------------------------------------------------------------
local function configure_stm32f1_priorities()
        local choice = key_read("../project/flags.h", "__IRQ_USER_PRIORITY__")
        msg(progress() .. "Select default priority for user's interrputs.")
        msg("Current priority is: priority " .. math.floor(tonumber(choice) / 16))
        add_item("0x0F", "Priority 0 (the highest)")
        add_item("0x1F", "Priority 1")
        add_item("0x2F", "Priority 2")
        add_item("0x3F", "Priority 3")
        add_item("0x4F", "Priority 4")
        add_item("0x5F", "Priority 5")
        add_item("0x6F", "Priority 6")
        add_item("0x7F", "Priority 7")
        add_item("0x8F", "Priority 8")
        add_item("0x9F", "Priority 9")
        add_item("0xAF", "Priority 10")
        add_item("0xBF", "Priority 11")
        add_item("0xCF", "Priority 12")
        add_item("0xDF", "Priority 13 (the lowest)")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../project/flags.h", "__IRQ_RTOS_KERNEL_PRIORITY__", "0xFF")
                key_save("../project/flags.h", "__IRQ_RTOS_SYSCALL_PRIORITY__", "0xEF")
                key_save("../project/flags.h", "__IRQ_USER_PRIORITY__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function execute configuration
--------------------------------------------------------------------------------
function cpu_configure()
        local arch = get_cpu_arch()

        calculate_total_steps()

        title("CPU configuration (" .. arch .. ")")

        ::common_1::
        if configure_cpu_arch() == back then
                return back
        else
                arch = get_cpu_arch()
                calculate_total_steps()
        end

        ::common_2::
        if configure_freq() == back then
                goto common_1
        end

        if     arch == "stm32f1" then goto stm32f1_1
        elseif arch == "stm32f2" then goto stm32f2_1
        elseif arch == "stm32f3" then goto stm32f3_1
        elseif arch == "stm32f4" then goto stm32f4_1
        else return next end

        -- stm32f1 --
        ::stm32f1_1::
        if configure_stm32f1_mcu() == back then
                goto common_2
        end

        ::_stm32f1_02_::
        if configure_stm32f1_priorities() == back then
                goto stm32f1_1
        end

        -- stm32f2 --
        ::stm32f2_1::

        -- stm32f3 --
        ::stm32f3_1::

        -- stm32f4 --
        ::stm32f4_1::

        return next
end

--------------------------------------------------------------------------------
-- Enable configuration if master wizard is not defined
--------------------------------------------------------------------------------
if (master ~= true) then
        while cpu_configure() == back do
        end

        configuration_finished()
end

--------------------------------------------------------------------------------
-- END OF FILE
--------------------------------------------------------------------------------
