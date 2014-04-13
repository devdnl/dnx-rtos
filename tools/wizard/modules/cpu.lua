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
require "db"

--------------------------------------------------------------------------------
-- GLOBAL OBJECTS
--------------------------------------------------------------------------------
cpu = {}

local steps    = {}
steps.stm32f1  = 4
steps.stm32f2  = 4
steps.stm32f3  = 4
steps.stm32f4  = 4

--------------------------------------------------------------------------------
-- LOCAL FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Function calculate a total configuration steps
--------------------------------------------------------------------------------
local function calculate_total_steps()
        set_total_steps(steps[db:get_arch()])
end

--------------------------------------------------------------------------------
-- @brief Function configure CPU architecture
--------------------------------------------------------------------------------
local function configure_cpu_arch()
        msg(progress() .. "CPU architecture configuration.")
        local choice = db:get_arch()
        msg("Current CPU architecture is: " .. db:get_arch_description(choice) .. ".")
        for i,arch in pairs(db:get_arch_list()) do
                add_item(arch, db:get_arch_description(arch))
        end
        choice = get_selection()
        if (can_be_saved(choice)) then
                db:set_arch(choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Configure CPU frequency
--------------------------------------------------------------------------------
local function configure_freq()
        msg(progress() .. "Configuration of oscillator frequency (generator, crystal, etc).")
        local value = cpu:get_osc_freq()
        msg("Current value is: " .. value .. " Hz.")
        value = get_number("dec", 0, 100e6)
        if (can_be_saved(value)) then
                key_save(db.path.project.flags, "__CPU_OSC_FREQ__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Configure microcontroller name
--------------------------------------------------------------------------------
local function configure_mcu_name()
        local choice = db:get_cpu_name()
        msg(progress() .. "Which microcontroller do you want to use?")
        msg("Current microcontroller is: " .. choice .. ".")
        for i,m in pairs(db:get_mcu_list(db:get_arch())) do
                add_item(m, m)
        end
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save(db.path.stm32f1.cpu.flags, "__CPU_NAME__", choice)
                key_save(db.path.stm32f1.cpu.mk, "CPUCONFIG_CPUNAME", choice)
                key_save(db.path.stm32f1.cpu.flags, "__CPU_FAMILY__", "__" .. db:get_mcu_family(choice) .. "__")
                key_save(db.path.stm32f1.cpu.flags, "__CPU_RAM_MEM_SIZE__", db:get_mcu_ram_size(choice))

                -- disable not supported modules
                for i, m in pairs(db:get_modules_list()) do
                        if not db:is_module_supported(db:get_cpu_name(), m) then
                                key_save(db.path.project.mk, "ENABLE_"..m, no)
                                key_save(db.path.project.flags, "__ENABLE_"..m.."__", no)
                        end
                end
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Configure IRQ priorities
--------------------------------------------------------------------------------
local function configure_priorities()
        local choice = key_read(db.path.project.flags, "__IRQ_USER_PRIORITY__")
        local arch   = db:get_arch()
        msg(progress() .. "Select default priority for user's interrputs.")

        if (arch == "stm32f1") then
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
        end

        choice = get_selection()
        if (can_be_saved(choice)) then
                local kernel_priority
                local syscall_priority

                if (arch == "stm32f1") then
                        kernel_priority  = "0xFF"
                        syscall_priority = "0xEF"
                end

                key_save(db.path.project.flags, "__IRQ_RTOS_KERNEL_PRIORITY__", kernel_priority)
                key_save(db.path.project.flags, "__IRQ_RTOS_SYSCALL_PRIORITY__", syscall_priority)
                key_save(db.path.project.flags, "__IRQ_USER_PRIORITY__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- GLOBAL FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Function return CPU oscillator frequency
-- @return Oscillator frequency
--------------------------------------------------------------------------------
function cpu:get_osc_freq()
        return key_read(db.path.project.flags, "__CPU_OSC_FREQ__")
end

--------------------------------------------------------------------------------
-- @brief Function execute configuration
--------------------------------------------------------------------------------
function cpu:configure()
        title("CPU configuration (" .. db:get_arch() .. ")")
        navigation("Home/CPU")
        calculate_total_steps()

        ::cpu_arch::
        if configure_cpu_arch() == back then
                return back
        else
                calculate_total_steps()
                title("CPU configuration (" .. db:get_arch() .. ")")
        end

        ::cpu_freq::   if configure_freq()       == back then goto cpu_arch end
        ::mcu_name::   if configure_mcu_name()   == back then goto cpu_freq end
        ::priorities:: if configure_priorities() == back then goto mcu_name end

        return next
end

-- started without master file
if (master ~= true) then
        show_no_master_info()
end

--------------------------------------------------------------------------------
-- END OF FILE
--------------------------------------------------------------------------------
