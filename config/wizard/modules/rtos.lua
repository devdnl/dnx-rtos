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

require "modules/defs"

--------------------------------------------------------------------------------
-- GLOBAL OBJECTS
--------------------------------------------------------------------------------
-- public calls objects
rtos = {}

--------------------------------------------------------------------------------
-- FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Function calculate a total configuration steps
--------------------------------------------------------------------------------
local function calculate_total_steps()
        local steps = 24
        if (key_read("../project/flags.h", "__OS_MONITOR_NETWORK_MEMORY_USAGE__") == no) then
                steps = steps - 1
        end

        set_total_steps(steps)
end

--------------------------------------------------------------------------------
-- @brief Function configure task stack depth
--------------------------------------------------------------------------------
local function configure_task_stack_depth()
        local value = key_read("../project/flags.h", "__OS_TASK_MIN_STACK_DEPTH__")
        msg(progress() .. "Task stack depth configuration.")
        msg("Current stack depth is: " .. value .. " levels.")
        value = get_number("dec", 16, 4096)
        if (can_be_saved(value)) then
                key_save("../project/flags.h", "__OS_TASK_MIN_STACK_DEPTH__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Function configure file systems stack depth
--------------------------------------------------------------------------------
local function configure_fs_stack_depth()
        local value = key_read("../project/flags.h", "__OS_FILE_SYSTEM_STACK_DEPTH__")
        msg(progress() .. "File system stack depth configuration. This value determine how many stack is requred by file systems. The value is a part of total stack size of single task.")
        msg("Current stack depth is: " .. value .. " levels.")
        value = get_number("dec", 16, 4096)
        if (can_be_saved(value)) then
                key_save("../project/flags.h", "__OS_FILE_SYSTEM_STACK_DEPTH__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Function configure interrupts stack depth
--------------------------------------------------------------------------------
local function configure_irq_stack_depth()
        local value = key_read("../project/flags.h", "__OS_IRQ_STACK_DEPTH__")
        msg(progress() .. "Interrupts stack depth configuration. This value determine how many stack is requred by interrupts. The value is a part of total stack size of single task.")
        msg("Current stack depth is: " .. value .. " levels.")
        value = get_number("dec", 16, 4096)
        if (can_be_saved(value)) then
                key_save("../project/flags.h", "__OS_IRQ_STACK_DEPTH__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Function configure number of priorites
--------------------------------------------------------------------------------
local function configure_number_of_priorities()
        local value = key_read("../project/flags.h", "__OS_TASK_MAX_PRIORITIES__")
        msg(progress() .. "Configure a number of priorities supported by RTOS." ..
            " The best values are the odd values, because for negative and positive range falls the same count of numbers. " ..
            "E.g.: if 7 then -3..0..3.")
        msg("Current value is: " .. value)
        value = get_number("dec", 3, 255)
        if (can_be_saved(value)) then
                key_save("../project/flags.h", "__OS_TASK_MAX_PRIORITIES__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Function configure task name length
--------------------------------------------------------------------------------
local function configure_task_name_length()
        local value = key_read("../project/flags.h", "__OS_TASK_NAME_LEN__")
        msg(progress() .. "Configure a length of task name.")
        msg("Current name length is: " .. value .. " characters.")
        value = get_number("dec", 8, 256)
        if (can_be_saved(value)) then
                key_save("../project/flags.h", "__OS_TASK_NAME_LEN__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Function configure context switch frequency
--------------------------------------------------------------------------------
local function configure_context_switch_freq()
        local value = key_read("../project/flags.h", "__OS_TASK_SCHED_FREQ__")
        msg(progress() .. "Configure a context switch frequency. If microcontroller works on low frequency, then this value shall be not to big to save CPU time (less context switch).")
        msg("Current frequence is: " .. value .. " Hz.")
        value = get_number("dec", 1, 1000)
        if (can_be_saved(value)) then
                key_save("../project/flags.h", "__OS_TASK_SCHED_FREQ__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Function configure RTOS sleep on idle
--------------------------------------------------------------------------------
local function configure_sleep_on_idle()
        local choice = key_read("../project/flags.h", "__OS_SLEEP_ON_IDLE__")
        msg(progress() .. "Do you want to enable kernel sleep on idle task?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../project/flags.h", "__OS_SLEEP_ON_IDLE__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure printf enable
--------------------------------------------------------------------------------
local function configure_printf_enable()
        local choice = key_read("../project/flags.h", "__OS_PRINTF_ENABLE__")
        msg(progress() .. "Do you want to enable printf family functions?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../project/flags.h", "__OS_PRINTF_ENABLE__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure scanf enable
--------------------------------------------------------------------------------
local function configure_scanf_enable()
        local choice = key_read("../project/flags.h", "__OS_SCANF_ENABLE__")
        msg(progress() .. "Do you want to enable scanf family functions?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../project/flags.h", "__OS_SCANF_ENABLE__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure printk enable
--------------------------------------------------------------------------------
local function configure_printk_enable()
        local choice = key_read("../project/flags.h", "__OS_SYSTEM_MSG_ENABLE__")
        msg(progress() .. "Do you want to enable system message function (printk)?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../project/flags.h", "__OS_SYSTEM_MSG_ENABLE__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure color terminal enable
--------------------------------------------------------------------------------
local function configure_colorterm_enable()
        local choice = key_read("../project/flags.h", "__OS_COLOR_TERMINAL_ENABLE__")
        msg(progress() .. "Do you want to enable color terminal output?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../project/flags.h", "__OS_COLOR_TERMINAL_ENABLE__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure stream buffer length
--------------------------------------------------------------------------------
local function configure_stream_buffer_length()
        local value = key_read("../project/flags.h", "__OS_STREAM_BUFFER_LENGTH__")
        msg(progress() .. "Configure a length of stream buffer. The buffer is used in the printf and scanf family functions. Value set the BUFSIZ definition, that is localized in stdio.h library.")
        msg("Current buffer length is: " .. value .. " bytes.")
        value = get_number("dec", 16, 4096)
        if (can_be_saved(value)) then
                key_save("../project/flags.h", "__OS_STREAM_BUFFER_LENGTH__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Function configure pipe buffer length
--------------------------------------------------------------------------------
local function configure_pipe_buffer_length()
        local value = key_read("../project/flags.h", "__OS_PIPE_LENGTH__")
        msg(progress() .. "Configure a length of pipe buffer. The buffer is used in the FIFO files. If this value is bigger the pipe is more effective, but use more RAM.")
        msg("Current buffer length is: " .. value .. " bytes.")
        value = get_number("dec", 16, 4096)
        if (can_be_saved(value)) then
                key_save("../project/flags.h", "__OS_PIPE_LENGTH__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Function configure errno string length
--------------------------------------------------------------------------------
local function configure_errno_beauty()
        local choice = key_read("../project/flags.h", "__OS_ERRNO_STRING_LEN__")
        msg(progress() .. "Configure the error messages that can be printed from errno value.")

        local messages = {}
        messages["0"] = "Disabled (low memory consumption)"
        messages["1"] = "Only numbers (small memory consumption)"
        messages["2"] = "Short names (medium memory consumption)"
        messages["3"] = "Full names (high memory consumption)"

        msg("Current choice is: " .. messages[choice] .. ".")
        add_item("0", messages["0"])
        add_item("1", messages["1"])
        add_item("2", messages["2"])
        add_item("3", messages["3"])
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../project/flags.h", "__OS_ERRNO_STRING_LEN__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure task memory monitoring
--------------------------------------------------------------------------------
local function configure_monitor_task_memory_usage()
        local choice = key_read("../project/flags.h", "__OS_MONITOR_TASK_MEMORY_USAGE__")
        msg(progress() .. "Do you want to enable task memory monitoring?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../project/flags.h", "__OS_MONITOR_TASK_MEMORY_USAGE__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure task file usage monitoring
--------------------------------------------------------------------------------
local function configure_monitor_task_file_usage()
        local choice = key_read("../project/flags.h", "__OS_MONITOR_TASK_FILE_USAGE__")
        msg(progress() .. "Do you want to enable task file monitoring?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../project/flags.h", "__OS_MONITOR_TASK_FILE_USAGE__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure kernel memory monitoring
--------------------------------------------------------------------------------
local function configure_monitor_kernel_memory_usage()
        local choice = key_read("../project/flags.h", "__OS_MONITOR_KERNEL_MEMORY_USAGE__")
        msg(progress() .. "Do you want to enable kernel memory monitoring?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../project/flags.h", "__OS_MONITOR_KERNEL_MEMORY_USAGE__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure module memory monitoring
--------------------------------------------------------------------------------
local function configure_monitor_module_memory_usage()
        local choice = key_read("../project/flags.h", "__OS_MONITOR_MODULE_MEMORY_USAGE__")
        msg(progress() .. "Do you want to enable module memory monitoring?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../project/flags.h", "__OS_MONITOR_MODULE_MEMORY_USAGE__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure system memory monitoring
--------------------------------------------------------------------------------
local function configure_monitor_system_memory_usage()
        local choice = key_read("../project/flags.h", "__OS_MONITOR_SYSTEM_MEMORY_USAGE__")
        msg(progress() .. "Do you want to enable system memory monitoring?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../project/flags.h", "__OS_MONITOR_SYSTEM_MEMORY_USAGE__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure cpu load monitoring
--------------------------------------------------------------------------------
local function configure_monitor_cpu_load()
        local choice = key_read("../project/flags.h", "__OS_MONITOR_CPU_LOAD__")
        msg(progress() .. "Do you want to enable CPU load monitoring?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../project/flags.h", "__OS_MONITOR_CPU_LOAD__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure network memory monitoring
--------------------------------------------------------------------------------
local function configure_monitor_network_memory_usage()
        local choice = key_read("../project/flags.h", "__OS_MONITOR_NETWORK_MEMORY_USAGE__")
        msg(progress() .. "Do you want to enable network memory monitoring?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../project/flags.h", "__OS_MONITOR_NETWORK_MEMORY_USAGE__", choice)
                calculate_total_steps()
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure network memory usage limit
--------------------------------------------------------------------------------
local function configure_monitor_network_memory_usage_limit()
        local choice = key_read("../project/flags.h", "__OS_MONITOR_NETWORK_MEMORY_USAGE__")
        if (choice == yes) then
                local value = key_read("../project/flags.h", "__OS_MONITOR_NETWORK_MEMORY_USAGE_LIMIT__")
                msg(progress() .. "Configure a memory usage limit for network layer. To disable this limit enter 0.")

                if (value == "0") then value = "Disabled"
                else value = value .. " bytes" end

                msg("Current limit is: " .. value .. ".")
                value = get_number("dec", 0, 16*1024*1024)
                if (can_be_saved(value)) then
                        key_save("../project/flags.h", "__OS_MONITOR_NETWORK_MEMORY_USAGE_LIMIT__", value)
                end

                return value
        end
end

--------------------------------------------------------------------------------
-- @brief Function configure hostname
--------------------------------------------------------------------------------
local function configure_hostname()
        local str = key_read("../project/flags.h", "__OS_HOSTNAME__")
        msg(progress() .. "Configure a host name. This is a name that identify your board in network or terminal.")
        msg("Current name is: " .. str .. ".")
        str = get_string()
        if (can_be_saved(str)) then
                key_save("../project/flags.h", "__OS_HOSTNAME__", str)
        end

        return str
end

--------------------------------------------------------------------------------
-- @brief Function configure stop macro
--------------------------------------------------------------------------------
local function configure_stop_macro()
        local choice = key_read("../project/flags.h", "__OS_SYSTEM_STOP_MACRO__")
        msg(progress() .. "System stop macro configuration. Use only in debug purposes. The macro locks a task due to negative test condition. Do you want to enable stop macro?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../project/flags.h", "__OS_SYSTEM_STOP_MACRO__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function execute configuration
--------------------------------------------------------------------------------
function rtos:configure()
        title("Operating System Configuration")
        navigation("Home/Operating system")
        calculate_total_steps()

        ::_01_::
        if configure_task_stack_depth() == back then
                return back
        end

        ::_02_::
        if configure_fs_stack_depth() == back then
                goto _01_
        end

        ::_03_::
        if configure_irq_stack_depth() == back then
                goto _02_
        end

        ::_04_::
        if configure_number_of_priorities() == back then
                goto _03_
        end

        ::_05_::
        if configure_task_name_length() == back then
                goto _04_
        end

        ::_06_::
        if configure_context_switch_freq() == back then
                goto _05_
        end

        ::_07_::
        if configure_sleep_on_idle() == back then
                goto _06_
        end

        ::_08_::
        if configure_printf_enable() == back then
                goto _07_
        end

        ::_09_::
        if configure_scanf_enable() == back then
                goto _08_
        end

        ::_10_::
        if configure_printk_enable() == back then
                goto _09_
        end

        ::_11_::
        if configure_colorterm_enable() == back then
                goto _10_
        end

        ::_12_::
        if configure_stream_buffer_length() == back then
                goto _11_
        end

        ::_13_::
        if configure_pipe_buffer_length() == back then
                goto _12_
        end

        ::_14_::
        if configure_errno_beauty() == back then
                goto _13_
        end

        ::_15_::
        if configure_monitor_task_memory_usage() == back then
                goto _14_
        end

        ::_16_::
        if configure_monitor_task_file_usage() == back then
                goto _15_
        end

        ::_17_::
        if configure_monitor_kernel_memory_usage() == back then
                goto _16_
        end

        ::_18_::
        if configure_monitor_module_memory_usage() == back then
                goto _17_
        end

        ::_19_::
        if configure_monitor_system_memory_usage() == back then
                goto _18_
        end

        ::_20_::
        if configure_monitor_cpu_load() == back then
                goto _19_
        end

        ::_21_::
        if configure_monitor_network_memory_usage() == back then
                goto _20_
        end

        ::_22_::
        if configure_monitor_network_memory_usage_limit() == back then
                goto _21_
        end

        ::_23_::
        if configure_hostname() == back then
                if (key_read("../project/flags.h", "__OS_MONITOR_NETWORK_MEMORY_USAGE__") == yes) then
                        goto _22_
                else
                        goto _21_
                end
        end

        ::_24_::
        if configure_stop_macro() == back then
                goto _23_
        end

        return next
end

-- started without master file
if (master ~= true) then
        show_no_master_info()
end

--------------------------------------------------------------------------------
-- END OF FILE
--------------------------------------------------------------------------------
