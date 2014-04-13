--[[============================================================================
@file    rtos.lua

@author  Daniel Zorychta

@brief   RTOS configuration file.

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
-- public calls objects
rtos = {}

local errno_cfg = {}
errno_cfg["0"]  = "Disabled (low memory consumption)"
errno_cfg["1"]  = "Only numbers (small memory consumption)"
errno_cfg["2"]  = "Short names (medium memory consumption)"
errno_cfg["3"]  = "Full names (high memory consumption)"

--------------------------------------------------------------------------------
-- FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Function calculate a total configuration steps
--------------------------------------------------------------------------------
local function calculate_total_steps()
        local steps = 24
        if (key_read(db.path.project.flags, "__OS_MONITOR_NETWORK_MEMORY_USAGE__") == no) then
                steps = steps - 1
        end

        set_total_steps(steps)
end

--------------------------------------------------------------------------------
-- @brief Function configure task stack depth
--------------------------------------------------------------------------------
local function configure_task_stack_depth()
        local value = key_read(db.path.project.flags, "__OS_TASK_MIN_STACK_DEPTH__")
        msg(progress() .. "Task stack depth configuration.")
        msg("Current stack depth is: " .. value .. " levels.")
        value = get_number("dec", 16, 4096)
        if (can_be_saved(value)) then
                key_save(db.path.project.flags, "__OS_TASK_MIN_STACK_DEPTH__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Function configure file systems stack depth
--------------------------------------------------------------------------------
local function configure_fs_stack_depth()
        local value = key_read(db.path.project.flags, "__OS_FILE_SYSTEM_STACK_DEPTH__")
        msg(progress() .. "File system stack depth configuration. This value determine how many stack is requred by file systems. The value is a part of total stack size of single task.")
        msg("Current stack depth is: " .. value .. " levels.")
        value = get_number("dec", 16, 4096)
        if (can_be_saved(value)) then
                key_save(db.path.project.flags, "__OS_FILE_SYSTEM_STACK_DEPTH__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Function configure interrupts stack depth
--------------------------------------------------------------------------------
local function configure_irq_stack_depth()
        local value = key_read(db.path.project.flags, "__OS_IRQ_STACK_DEPTH__")
        msg(progress() .. "Interrupts stack depth configuration. This value determine how many stack is requred by interrupts. The value is a part of total stack size of single task.")
        msg("Current stack depth is: " .. value .. " levels.")
        value = get_number("dec", 16, 4096)
        if (can_be_saved(value)) then
                key_save(db.path.project.flags, "__OS_IRQ_STACK_DEPTH__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Function configure number of priorities
--------------------------------------------------------------------------------
local function configure_number_of_priorities()
        local value = key_read(db.path.project.flags, "__OS_TASK_MAX_PRIORITIES__")
        msg(progress() .. "Configure a number of priorities supported by RTOS." ..
            " The best values are the odd values, because for negative and positive range falls the same count of numbers. " ..
            "E.g.: if 7 then -3..0..3.")
        msg("Current value is: " .. value)
        value = get_number("dec", 3, 255)
        if (can_be_saved(value)) then
                key_save(db.path.project.flags, "__OS_TASK_MAX_PRIORITIES__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Function configure task name length
--------------------------------------------------------------------------------
local function configure_task_name_length()
        local value = key_read(db.path.project.flags, "__OS_TASK_NAME_LEN__")
        msg(progress() .. "Configure a length of task name.")
        msg("Current name length is: " .. value .. " characters.")
        value = get_number("dec", 8, 256)
        if (can_be_saved(value)) then
                key_save(db.path.project.flags, "__OS_TASK_NAME_LEN__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Function configure context switch frequency
--------------------------------------------------------------------------------
local function configure_context_switch_freq()
        local value = key_read(db.path.project.flags, "__OS_TASK_SCHED_FREQ__")
        msg(progress() .. "Configure a context switch frequency. If microcontroller works on low frequency, then this value shall be not to big to save CPU time (less context switch).")
        msg("Current frequence is: " .. value .. " Hz.")
        value = get_number("dec", 1, 1000)
        if (can_be_saved(value)) then
                key_save(db.path.project.flags, "__OS_TASK_SCHED_FREQ__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Function configure RTOS sleep on idle
--------------------------------------------------------------------------------
local function configure_sleep_on_idle()
        local choice = key_read(db.path.project.flags, "__OS_SLEEP_ON_IDLE__")
        msg(progress() .. "Do you want to enable kernel sleep on idle task?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save(db.path.project.flags, "__OS_SLEEP_ON_IDLE__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure printf enable
--------------------------------------------------------------------------------
local function configure_printf_enable()
        local choice = key_read(db.path.project.flags, "__OS_PRINTF_ENABLE__")
        msg(progress() .. "Do you want to enable printf family functions?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save(db.path.project.flags, "__OS_PRINTF_ENABLE__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure scanf enable
--------------------------------------------------------------------------------
local function configure_scanf_enable()
        local choice = key_read(db.path.project.flags, "__OS_SCANF_ENABLE__")
        msg(progress() .. "Do you want to enable scanf family functions?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save(db.path.project.flags, "__OS_SCANF_ENABLE__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure printk enable
--------------------------------------------------------------------------------
local function configure_printk_enable()
        local choice = key_read(db.path.project.flags, "__OS_SYSTEM_MSG_ENABLE__")
        msg(progress() .. "Do you want to enable system message function (printk)?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save(db.path.project.flags, "__OS_SYSTEM_MSG_ENABLE__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure color terminal enable
--------------------------------------------------------------------------------
local function configure_colorterm_enable()
        local choice = key_read(db.path.project.flags, "__OS_COLOR_TERMINAL_ENABLE__")
        msg(progress() .. "Do you want to enable color terminal output?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save(db.path.project.flags, "__OS_COLOR_TERMINAL_ENABLE__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure stream buffer length
--------------------------------------------------------------------------------
local function configure_stream_buffer_length()
        local value = key_read(db.path.project.flags, "__OS_STREAM_BUFFER_LENGTH__")
        msg(progress() .. "Configure a length of stream buffer. The buffer is used in the printf and scanf family functions. Value set the BUFSIZ definition, that is localized in stdio.h library.")
        msg("Current buffer length is: " .. value .. " bytes.")
        value = get_number("dec", 16, 4096)
        if (can_be_saved(value)) then
                key_save(db.path.project.flags, "__OS_STREAM_BUFFER_LENGTH__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Function configure pipe buffer length
--------------------------------------------------------------------------------
local function configure_pipe_buffer_length()
        local value = key_read(db.path.project.flags, "__OS_PIPE_LENGTH__")
        msg(progress() .. "Configure a length of pipe buffer. The buffer is used in the FIFO files. If this value is bigger the pipe is more effective, but use more RAM.")
        msg("Current buffer length is: " .. value .. " bytes.")
        value = get_number("dec", 16, 4096)
        if (can_be_saved(value)) then
                key_save(db.path.project.flags, "__OS_PIPE_LENGTH__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Function configure errno string length
--------------------------------------------------------------------------------
local function configure_errno_beauty()
        local choice = key_read(db.path.project.flags, "__OS_ERRNO_STRING_LEN__")
        msg(progress() .. "Configure the error messages that can be printed from errno value.")
        msg("Current choice is: " .. errno_cfg[choice] .. ".")
        add_item("0", errno_cfg["0"])
        add_item("1", errno_cfg["1"])
        add_item("2", errno_cfg["2"])
        add_item("3", errno_cfg["3"])
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save(db.path.project.flags, "__OS_ERRNO_STRING_LEN__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure task memory monitoring
--------------------------------------------------------------------------------
local function configure_monitor_task_memory_usage()
        local choice = key_read(db.path.project.flags, "__OS_MONITOR_TASK_MEMORY_USAGE__")
        msg(progress() .. "Do you want to enable task memory monitoring?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save(db.path.project.flags, "__OS_MONITOR_TASK_MEMORY_USAGE__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure task file usage monitoring
--------------------------------------------------------------------------------
local function configure_monitor_task_file_usage()
        local choice = key_read(db.path.project.flags, "__OS_MONITOR_TASK_FILE_USAGE__")
        msg(progress() .. "Do you want to enable task file monitoring?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save(db.path.project.flags, "__OS_MONITOR_TASK_FILE_USAGE__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure kernel memory monitoring
--------------------------------------------------------------------------------
local function configure_monitor_kernel_memory_usage()
        local choice = key_read(db.path.project.flags, "__OS_MONITOR_KERNEL_MEMORY_USAGE__")
        msg(progress() .. "Do you want to enable kernel memory monitoring?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save(db.path.project.flags, "__OS_MONITOR_KERNEL_MEMORY_USAGE__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure module memory monitoring
--------------------------------------------------------------------------------
local function configure_monitor_module_memory_usage()
        local choice = key_read(db.path.project.flags, "__OS_MONITOR_MODULE_MEMORY_USAGE__")
        msg(progress() .. "Do you want to enable module memory monitoring?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save(db.path.project.flags, "__OS_MONITOR_MODULE_MEMORY_USAGE__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure system memory monitoring
--------------------------------------------------------------------------------
local function configure_monitor_system_memory_usage()
        local choice = key_read(db.path.project.flags, "__OS_MONITOR_SYSTEM_MEMORY_USAGE__")
        msg(progress() .. "Do you want to enable system memory monitoring?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save(db.path.project.flags, "__OS_MONITOR_SYSTEM_MEMORY_USAGE__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure cpu load monitoring
--------------------------------------------------------------------------------
local function configure_monitor_cpu_load()
        local choice = key_read(db.path.project.flags, "__OS_MONITOR_CPU_LOAD__")
        msg(progress() .. "Do you want to enable CPU load monitoring?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save(db.path.project.flags, "__OS_MONITOR_CPU_LOAD__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure network memory monitoring
--------------------------------------------------------------------------------
local function configure_monitor_network_memory_usage()
        local choice = key_read(db.path.project.flags, "__OS_MONITOR_NETWORK_MEMORY_USAGE__")
        msg(progress() .. "Do you want to enable network memory monitoring?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save(db.path.project.flags, "__OS_MONITOR_NETWORK_MEMORY_USAGE__", choice)
                calculate_total_steps()
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure network memory usage limit
--------------------------------------------------------------------------------
local function configure_monitor_network_memory_usage_limit()
        local value = key_read(db.path.project.flags, "__OS_MONITOR_NETWORK_MEMORY_USAGE_LIMIT__")
        msg(progress() .. "Configure a memory usage limit for network layer. To disable this limit enter 0.")

        if (value == "0") then value = "Disabled"
        else value = value .. " bytes" end

        msg("Current limit is: " .. value .. ".")
        value = get_number("dec", 0, 16*1024*1024)
        if (can_be_saved(value)) then
                key_save(db.path.project.flags, "__OS_MONITOR_NETWORK_MEMORY_USAGE_LIMIT__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Function configure hostname
--------------------------------------------------------------------------------
local function configure_hostname()
        local str = key_read(db.path.project.flags, "__OS_HOSTNAME__")
        msg(progress() .. "Configure a host name. This is a name that identify your board in network or terminal.")
        msg("Current name is: " .. str .. ".")
        str = get_string()
        if (can_be_saved(str)) then
                key_save(db.path.project.flags, "__OS_HOSTNAME__", str)
        end

        return str
end

--------------------------------------------------------------------------------
-- @brief Function configure stop macro
--------------------------------------------------------------------------------
local function configure_stop_macro()
        local choice = key_read(db.path.project.flags, "__OS_SYSTEM_STOP_MACRO__")
        msg(progress() .. "System stop macro configuration. Use only in debug purposes. The macro locks a task due to negative test condition. Do you want to enable stop macro?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save(db.path.project.flags, "__OS_SYSTEM_STOP_MACRO__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Configuration summary
--------------------------------------------------------------------------------
local function print_summary()
        local tsk_stack     = key_read(db.path.project.flags, "__OS_TASK_MIN_STACK_DEPTH__")
        local fs_stack      = key_read(db.path.project.flags, "__OS_FILE_SYSTEM_STACK_DEPTH__")
        local irq_stack     = key_read(db.path.project.flags, "__OS_IRQ_STACK_DEPTH__")
        local no_of_prio    = key_read(db.path.project.flags, "__OS_TASK_MAX_PRIORITIES__")
        local tsk_name_len  = key_read(db.path.project.flags, "__OS_TASK_NAME_LEN__")
        local sch_freq      = key_read(db.path.project.flags, "__OS_TASK_SCHED_FREQ__")
        local sleep_on_idle = key_read(db.path.project.flags, "__OS_SLEEP_ON_IDLE__")
        local printf_en     = key_read(db.path.project.flags, "__OS_PRINTF_ENABLE__")
        local scanf_en      = key_read(db.path.project.flags, "__OS_SCANF_ENABLE__")
        local printk_en     = key_read(db.path.project.flags, "__OS_SYSTEM_MSG_ENABLE__")
        local color_term    = key_read(db.path.project.flags, "__OS_COLOR_TERMINAL_ENABLE__")
        local stream_len    = key_read(db.path.project.flags, "__OS_STREAM_BUFFER_LENGTH__")
        local pipe_len      = key_read(db.path.project.flags, "__OS_PIPE_LENGTH__")
        local errno_look    = key_read(db.path.project.flags, "__OS_ERRNO_STRING_LEN__")
        local moni_tsk_mem  = key_read(db.path.project.flags, "__OS_MONITOR_TASK_MEMORY_USAGE__")
        local moni_tsk_file = key_read(db.path.project.flags, "__OS_MONITOR_TASK_FILE_USAGE__")
        local moni_krn_mem  = key_read(db.path.project.flags, "__OS_MONITOR_KERNEL_MEMORY_USAGE__")
        local moni_mod_mem  = key_read(db.path.project.flags, "__OS_MONITOR_MODULE_MEMORY_USAGE__")
        local moni_sys_mem  = key_read(db.path.project.flags, "__OS_MONITOR_SYSTEM_MEMORY_USAGE__")
        local moni_cpu_load = key_read(db.path.project.flags, "__OS_MONITOR_CPU_LOAD__")
        local moni_net_mem  = key_read(db.path.project.flags, "__OS_MONITOR_NETWORK_MEMORY_USAGE__")
        local moni_net_lim  = key_read(db.path.project.flags, "__OS_MONITOR_NETWORK_MEMORY_USAGE_LIMIT__")
        local hostname      = key_read(db.path.project.flags, "__OS_HOSTNAME__")
        local stop_macro    = key_read(db.path.project.flags, "__OS_SYSTEM_STOP_MACRO__")

        if tonumber(moni_net_lim) == 0 then moni_net_lim = "None" end

        msg("Operating system configuration summary:")
        msg("Task stack depth: "..tsk_stack.." levels\n"..
            "File system stack depth: "..fs_stack.." levels\n"..
            "IRQ stack depth: "..irq_stack.." levels\n"..
            "Number of priorities: "..no_of_prio.."\n"..
            "Task name length: "..tsk_name_len.." characters\n"..
            "Context switch frequency: "..funit(tonumber(sch_freq)).."\n"..
            "Sleep on idle task: "..filter_yes_no(sleep_on_idle).."\n"..
            "printf enabled: "..filter_yes_no(printf_en).."\n"..
            "scanf enabled: "..filter_yes_no(scanf_en).."\n"..
            "printk enabled: "..filter_yes_no(printk_en).."\n"..
            "Color terminal: "..filter_yes_no(color_term).."\n"..
            "Stream buffer length: "..stream_len.." bytes\n"..
            "Pipe buffer length: "..pipe_len.." bytes\n"..
            "Errors texts (errno): "..errno_cfg[errno_look].."\n"..
            "Task memory usage monitoring: "..filter_yes_no(moni_tsk_mem).."\n"..
            "Task file usage monitoring: "..filter_yes_no(moni_tsk_file).."\n"..
            "Kernel memory usage monitoring: "..filter_yes_no(moni_krn_mem).."\n"..
            "Module memory usage monitoring: "..filter_yes_no(moni_mod_mem).."\n"..
            "System memory usage monitoring: "..filter_yes_no(moni_sys_mem).."\n"..
            "CPU load measurement: "..filter_yes_no(moni_cpu_load).."\n"..
            "Network memory usage monitoring: "..filter_yes_no(moni_net_mem).."\n"..
            "Network memory usage limit: "..moni_net_lim.."\n"..
            "Hostname: "..hostname.."\n"..
            "Stop macro enabled: "..filter_yes_no(stop_macro))

        pause()
end

--------------------------------------------------------------------------------
-- @brief Function execute configuration
--------------------------------------------------------------------------------
function rtos:configure()
        title("Operating System Configuration")
        navigation("Home/Operating system")
        calculate_total_steps()

        local last = nil
        local main = {configure_task_stack_depth,
                       configure_fs_stack_depth,
                       configure_irq_stack_depth,
                       configure_number_of_priorities,
                       configure_task_name_length,
                       configure_context_switch_freq,
                       configure_sleep_on_idle,
                       configure_printf_enable,
                       configure_scanf_enable,
                       configure_printk_enable,
                       configure_colorterm_enable,
                       configure_stream_buffer_length,
                       configure_pipe_buffer_length,
                       configure_errno_beauty,
                       configure_stop_macro,
                       configure_hostname,
                       configure_monitor_task_memory_usage,
                       configure_monitor_task_file_usage,
                       configure_monitor_kernel_memory_usage,
                       configure_monitor_module_memory_usage,
                       configure_monitor_system_memory_usage,
                       configure_monitor_cpu_load,
                       configure_monitor_network_memory_usage}

        ::p_main::
        if show_pages(main, last) == back then
                return back
        end

        if key_read(db.path.project.flags, "__OS_MONITOR_NETWORK_MEMORY_USAGE__") == yes then
                if configure_monitor_network_memory_usage_limit() == back then
                        last = -1
                        goto p_main
                end
        end

        print_summary()

        return next
end

-- started without master file
if (master ~= true) then
        show_no_master_info()
end

--------------------------------------------------------------------------------
-- END OF FILE
--------------------------------------------------------------------------------
