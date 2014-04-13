--[[============================================================================
@file    uart.lua

@author  Daniel Zorychta

@brief   UART configuration wizard.

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
-- OBJECTS
--------------------------------------------------------------------------------
uart = {}

local arch = {}
arch.stm32f1            = {}
arch.stm32f1.configure  = nil

arch.stm32f2            = {}
arch.stm32f2.configure  = nil

arch.stm32f3            = {}
arch.stm32f3.configure  = nil

arch.stm32f4            = {}
arch.stm32f4.configure  = nil

--------------------------------------------------------------------------------
-- FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Calculate total steps
--------------------------------------------------------------------------------
local function calculate_total_steps()
        local arch   = db:get_arch()
        local family = db:get_mcu_family(db:get_cpu_name())

        if key_read(db.path.project.mk, "ENABLE_UART") == yes then
                if arch == "stm32f1" then
                        progress(1, 12)
                elseif arch == "stm32f2" then
                elseif arch == "stm32f3" then
                elseif arch == "stm32f4" then
                end
        else
                progress(1, 1)
        end
end

--------------------------------------------------------------------------------
-- @brief Ask user to select enable/disable module
--------------------------------------------------------------------------------
local function ask_for_enable()
        local choice = key_read(db.path.project.flags, "__ENABLE_UART__")
        msg(progress().."Do you want to enable UART module?")
        msg("Current selection is: "..filter_yes_no(choice)..".")
        add_item(yes, "Yes")
        add_item(no, "No")
        choice = get_selection()
        if can_be_saved(choice) then
                key_save(db.path.project.flags, "__ENABLE_UART__", choice)
                key_save(db.path.project.mk, "ENABLE_UART", choice)
        end

        calculate_total_steps()
        progress(2)

        return choice
end

--------------------------------------------------------------------------------
-- @brief Configuration for STM32F1
--------------------------------------------------------------------------------
arch.stm32f1.configure = function()

        local function configure_uart_if_enable(uart)
                local key    = "__UART_UART"..uart.."_ENABLE__"
                local choice = key_read(db.path.stm32f1.uart.flags, key)
                msg(progress(1, 2).."Do you want to enable UART"..uart.." interface?")
                msg("Current selection is: "..filter_yes_no(choice)..".")
                add_item(yes, "Yes")
                add_item(no, "No")
                choice = get_selection()
                if can_be_saved(choice) then
                        key_save(db.path.stm32f1.uart.flags, key, choice)
                end

                return choice
        end

        local function configure_uart_priority(uart)
                local key    = "__UART_UART"..uart.."_PRIORITY__"
                local choice = key_read(db.path.stm32f1.uart.flags, key)

                msg(progress(2, 2).."UART"..uart.." IRQ priority configuration.")
                if choice == "CONFIG_USER_IRQ_PRIORITY" then
                        msg("Current value is: Default")
                else
                        msg("Current value is: Priority "..math.floor(tonumber(choice) / 16)..".")
                end

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
                add_item("CONFIG_USER_IRQ_PRIORITY", "Default")
                choice = get_selection()
                if can_be_saved(choice) then
                        key_save(db.path.stm32f1.uart.flags, key, choice)
                end

                return choice
        end

        local function configure_rx_buffer_len()
                local choice = key_read(db.path.stm32f1.uart.flags, "__UART_RX_BUFFER_LEN__")
                msg(progress() .. "UART Rx buffer length. This value is an default for all interfaces.")
                msg("Current choice is: " .. choice .. " bytes.")
                choice = get_number("dec", 16, 4096)
                if (can_be_saved(choice)) then
                        key_save(db.path.stm32f1.uart.flags, "__UART_RX_BUFFER_LEN__", choice)
                end

                return choice
        end

        local function configure_parity()
                local value = {}
                value.UART_PARITY_OFF  = "No parity bit"
                value.UART_PARITY_ODD  = "Odd parity"
                value.UART_PARITY_EVEN = "Even parity"
                value.Unknown          = "Unknown"

                local choice = key_read(db.path.stm32f1.uart.flags, "__UART_DEFAULT_PARITY__")
                if value[choice] == nil then choice = "Unknown" end

                msg(progress() .. "UART default parity configuration. This value is an default for all interfaces.")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("UART_PARITY_OFF",  value.UART_PARITY_OFF )
                add_item("UART_PARITY_ODD",  value.UART_PARITY_ODD )
                add_item("UART_PARITY_EVEN", value.UART_PARITY_EVEN)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save(db.path.stm32f1.uart.flags, "__UART_DEFAULT_PARITY__", choice)
                end

                return choice
        end

        local function configure_stop_bits()
                local value = {}
                value.UART_STOP_BIT_1  = "1 stop bit"
                value.UART_STOP_BIT_2  = "2 stop bits"
                value.Unknown          = "Unknown"

                local choice = key_read(db.path.stm32f1.uart.flags, "__UART_DEFAULT_STOP_BITS__")
                if value[choice] == nil then choice = "Unknown" end

                msg(progress() .. "UART default stop bit number configuration. This value is an default for all interfaces.")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("UART_STOP_BIT_1",  value.UART_STOP_BIT_1)
                add_item("UART_STOP_BIT_2",  value.UART_STOP_BIT_2)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save(db.path.stm32f1.uart.flags, "__UART_DEFAULT_STOP_BITS__", choice)
                end

                return choice
        end

        local function configure_lin_break_len()
                local value = {}
                value.UART_LIN_BREAK_10_BITS = "LIN break 10 bits long"
                value.UART_LIN_BREAK_11_BITS = "LIN break 11 bits long"
                value.Unknown                = "Unknown"

                local choice = key_read(db.path.stm32f1.uart.flags, "__UART_DEFAULT_LIN_BREAK_LEN__")
                if value[choice] == nil then choice = "Unknown" end

                msg(progress() .. "LIN default break length. This value is an default for all interfaces.")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("UART_LIN_BREAK_10_BITS",  value.UART_LIN_BREAK_10_BITS)
                add_item("UART_LIN_BREAK_11_BITS",  value.UART_LIN_BREAK_11_BITS)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save(db.path.stm32f1.uart.flags, "__UART_DEFAULT_LIN_BREAK_LEN__", choice)
                end

                return choice
        end

        local function configure_tx_enable()
                local choice = key_read(db.path.stm32f1.uart.flags, "__UART_DEFAULT_TX_ENABLE__")
                msg(progress().."Do you want to enable TX line? This value is an default for all interfaces.")
                msg("Current selection is: "..filter_yes_no(choice)..".")
                add_item(yes, "Yes")
                add_item(no, "No")
                choice = get_selection()
                if can_be_saved(choice) then
                        key_save(db.path.stm32f1.uart.flags, "__UART_DEFAULT_TX_ENABLE__", choice)
                end

                return choice
        end

        local function configure_rx_enable()
                local choice = key_read(db.path.stm32f1.uart.flags, "__UART_DEFAULT_RX_ENABLE__")
                msg(progress().."Do you want to enable RX line? This value is an default for all interfaces.")
                msg("Current selection is: "..filter_yes_no(choice)..".")
                add_item(yes, "Yes")
                add_item(no, "No")
                choice = get_selection()
                if can_be_saved(choice) then
                        key_save(db.path.stm32f1.uart.flags, "__UART_DEFAULT_RX_ENABLE__", choice)
                end

                return choice
        end

        local function configure_lin_mode_enable()
                local choice = key_read(db.path.stm32f1.uart.flags, "__UART_DEFAULT_LIN_MODE_ENABLE__")
                msg(progress().."Do you want to enable UART in LIN mode? This value is an default for all interfaces.")
                msg("Current selection is: "..filter_yes_no(choice)..".")
                add_item(yes, "Yes")
                add_item(no, "No")
                choice = get_selection()
                if can_be_saved(choice) then
                        key_save(db.path.stm32f1.uart.flags, "__UART_DEFAULT_LIN_MODE_ENABLE__", choice)
                end

                return choice
        end

        local function configure_hw_flow_ctr()
                local choice = key_read(db.path.stm32f1.uart.flags, "__UART_DEFAULT_HW_FLOW_CTRL__")
                msg(progress().."Do you want to enable hardware flow control? This value is an default for all interfaces.")
                msg("Current selection is: "..filter_yes_no(choice)..".")
                add_item(yes, "Yes")
                add_item(no, "No")
                choice = get_selection()
                if can_be_saved(choice) then
                        key_save(db.path.stm32f1.uart.flags, "__UART_DEFAULT_HW_FLOW_CTRL__", choice)
                end

                return choice
        end

        local function configure_single_wire_mode()
                local choice = key_read(db.path.stm32f1.uart.flags, "__UART_DEFAULT_SINGLE_WIRE_MODE__")
                msg(progress().."Do you want to enable UART in single wire mode? This value is an default for all interfaces.")
                msg("Current selection is: "..filter_yes_no(choice)..".")
                add_item(yes, "Yes")
                add_item(no, "No")
                choice = get_selection()
                if can_be_saved(choice) then
                        key_save(db.path.stm32f1.uart.flags, "__UART_DEFAULT_SINGLE_WIRE_MODE__", choice)
                end

                return choice
        end

        local function configure_baud()
                local baud   = {50, 75, 100, 200, 300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600, 1e6, 2e6, 3e6, 4e6}
                local choice = key_read(db.path.stm32f1.uart.flags, "__UART_DEFAULT_BAUD__")
                msg(progress().."Select default baud value. This value is an default for all interfaces.")
                msg("Note: on some peripheral frequencies may be impossible to achieve selected baud!")
                msg("Current selection is: "..choice..".")
                for i = 1, #baud do
                        add_item(i, baud[i].." bps")
                end
                choice = get_selection()
                if can_be_saved(choice) then
                        choice = tonumber(choice)
                        key_save(db.path.stm32f1.uart.flags, "__UART_DEFAULT_BAUD__", baud[choice])
                end

                return choice
        end

        local function uart_select()
                local uart_no = db:get_mcu_module_data(db:get_cpu_name(), "UART")[1]

                while true do
                        navigation("Home/Modules/UART")
                        msg(progress(12, 12).."Choose UART interface for configure.")
                        for i = 1, uart_no do
                                add_item(i, "UART"..i)
                        end
                        local choice = get_selection()

                        if choice == back then
                                progress(11)
                                return back
                        end

                        if choice == next then
                                return next
                        end

                        navigation("Home/Modules/UART/UART"..choice)
                        progress(1)

                        ::uart_if_en:: if configure_uart_if_enable(choice) == back then goto continue end
                        if key_read(db.path.stm32f1.uart.flags, "__UART_UART"..choice.."_ENABLE__") == yes then
                                if configure_uart_priority(choice) == back then goto uart_if_en end
                        end

                        ::continue::
                end
        end

        local function print_summary()
                local baud       = key_read(db.path.stm32f1.uart.flags, "__UART_DEFAULT_BAUD__")
                local single     = key_read(db.path.stm32f1.uart.flags, "__UART_DEFAULT_SINGLE_WIRE_MODE__")
                local hw_ctrl    = key_read(db.path.stm32f1.uart.flags, "__UART_DEFAULT_HW_FLOW_CTRL__")
                local lin_mode   = key_read(db.path.stm32f1.uart.flags, "__UART_DEFAULT_LIN_MODE_ENABLE__")
                local rx_en      = key_read(db.path.stm32f1.uart.flags, "__UART_DEFAULT_RX_ENABLE__")
                local tx_en      = key_read(db.path.stm32f1.uart.flags, "__UART_DEFAULT_TX_ENABLE__")
                local lin_break  = key_read(db.path.stm32f1.uart.flags, "__UART_DEFAULT_LIN_BREAK_LEN__")
                local stop_bits  = key_read(db.path.stm32f1.uart.flags, "__UART_DEFAULT_STOP_BITS__")
                local parity     = key_read(db.path.stm32f1.uart.flags, "__UART_DEFAULT_PARITY__")
                local rx_buf_len = key_read(db.path.stm32f1.uart.flags, "__UART_RX_BUFFER_LEN__")
                local uart_no    = db:get_mcu_module_data(db:get_cpu_name(), "UART")[1]

                msg("UART module configuration summary:")

                msg("Defaults:\n"..
                    "  Baud: "..baud.." bps\n"..
                    "  Stop bits: "..stop_bits:gsub("UART_STOP_BIT_", "").."\n"..
                    "  Parity: "..parity:gsub("UART_PARITY_", "").."\n"..
                    "  RX Line enabled: "..filter_yes_no(rx_en).."\n"..
                    "  TX Line enabled: "..filter_yes_no(tx_en).."\n"..
                    "  RX buffer length: "..rx_buf_len.." bytes\n"..
                    "  Hardware flow control: "..filter_yes_no(hw_ctrl).."\n"..
                    "  Single wire mode: "..filter_yes_no(single).."\n"..
                    "  LIN mode: "..filter_yes_no(lin_mode).."\n"..
                    "  LIN break length: "..lin_break:gsub("UART_LIN_BREAK_", ""):gsub("_BITS", "").." bits\n")

                for uart = 1, uart_no do
                        local enabled  = key_read(db.path.stm32f1.uart.flags, "__UART_UART"..uart.."_ENABLE__")
                        local priority = key_read(db.path.stm32f1.uart.flags, "__UART_UART"..uart.."_PRIORITY__")

                        if priority == "CONFIG_USER_IRQ_PRIORITY" then
                                priority = "Default"
                        else
                                priority = math.floor(tonumber(priority) / 16)
                        end

                        msg("UART"..uart..":\n"..
                            "  Enabled: "..filter_yes_no(enabled).."\n"..
                            "  IRQ priority: "..priority.."\n")
                end

                pause()
        end

        if key_read(db.path.project.mk, "ENABLE_UART") == yes then

                local pages = {configure_rx_buffer_len,
                               configure_parity,
                               configure_stop_bits,
                               configure_lin_break_len,
                               configure_tx_enable,
                               configure_rx_enable,
                               configure_lin_mode_enable,
                               configure_hw_flow_ctr,
                               configure_single_wire_mode,
                               configure_baud,
                               uart_select,
                               print_summary}

                return show_pages(pages)
        end

        return next
end

--------------------------------------------------------------------------------
-- METHODS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Function execute configuration
--------------------------------------------------------------------------------
function uart:configure()
        title("UART module configuration for " .. db:get_cpu_name())
        navigation("Home/Modules/UART")
        calculate_total_steps()

        ::enable::
        if ask_for_enable() == back then
                return back
        end

        if key_read(db.path.project.mk, "ENABLE_UART") == yes then
                if arch[db:get_arch()].configure() == back then
                        goto enable
                end
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
