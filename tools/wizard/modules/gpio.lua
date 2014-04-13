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
-- public calls objects
gpio = {}

local gpio_port                         = {}
gpio_port.stm32f1                       = {}
gpio_port.stm32f1.pins                  = 16
gpio_port.stm32f1.configure             = nil   -- configured below
gpio_port.stm32f1.disable_unused        = nil   -- ...
gpio_port.stm32f1.port_list             = {"PA", "PB", "PC", "PD", "PE", "PF", "PG"}
gpio_port.stm32f2                       = {}
gpio_port.stm32f2.pins                  = 16
gpio_port.stm32f2.configure             = nil   -- configured below
gpio_port.stm32f2.disable_unused        = nil   -- ...
gpio_port.stm32f2.port_list             = {"PA", "PB", "PC", "PD", "PE", "PF", "PG", "PH", "PI"}
gpio_port.stm32f3                       = {}
gpio_port.stm32f3.pins                  = 16
gpio_port.stm32f3.configure             = nil   -- configured below
gpio_port.stm32f3.disable_unused        = nil   -- ...
gpio_port.stm32f3.port_list             = {"PA", "PB", "PC", "PD", "PE", "PF"}
gpio_port.stm32f4                       = {}
gpio_port.stm32f4.pins                  = 16
gpio_port.stm32f4.configure             = nil   -- configured below
gpio_port.stm32f4.disable_unused        = nil   -- ...
gpio_port.stm32f4.port_list             = {"PA", "PB", "PC", "PD", "PE", "PF", "PG", "PH", "PI", "PJ", "PK"}

--------------------------------------------------------------------------------
-- LOCAL FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Function returns port count for selected architecture
-- @return Number of ports
--------------------------------------------------------------------------------
local function get_ports_count()
        return db:get_mcu_module_data(db:get_cpu_name(), "GPIO")[1]
end

--------------------------------------------------------------------------------
-- @brief Function configure CPU architecture
--------------------------------------------------------------------------------
local function configure_enable()
        local choice = key_read(db.path.project.mk, "ENABLE_GPIO")
        msg(progress(1, 2) .. "Do you want to enable GPIO module?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save(db.path.project.mk,    "ENABLE_GPIO",     choice)
                key_save(db.path.project.flags, "__ENABLE_GPIO__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Port configuration for stm32f1
--------------------------------------------------------------------------------
gpio_port.stm32f1.configure = function()
        local port_name = gpio_port.stm32f1.port_list
        local cfg_file  = db.path.stm32f1.gpio.flags
        local pin       = -1
        local mode      = {{"_GPIO_OUT_PUSH_PULL_2MHZ"      , "Output Push-Pull 2MHz"},
                           {"_GPIO_OUT_PUSH_PULL_10MHZ"     , "Output Push-Pull 10MHz"},
                           {"_GPIO_OUT_PUSH_PULL_50MHZ"     , "Output Push-Pull 50MHz"},
                           {"_GPIO_OUT_OPEN_DRAIN_2MHZ"     , "Output Open drain 2MHz"},
                           {"_GPIO_OUT_OPEN_DRAIN_10MHZ"    , "Output Open drain 10MHz"},
                           {"_GPIO_OUT_OPEN_DRAIN_50MHZ"    , "Output Open drain 50MHz"},
                           {"_GPIO_ALT_OUT_PUSH_PULL_2MHZ"  , "Alternative output Push-Pull 2MHz"},
                           {"_GPIO_ALT_OUT_PUSH_PULL_10MHZ" , "Alternative output Push-Pull 10MHz"},
                           {"_GPIO_ALT_OUT_PUSH_PULL_50MHZ" , "Alternative output Push-Pull 50MHz"},
                           {"_GPIO_ALT_OUT_OPEN_DRAIN_2MHZ" , "Alternative output Open drain 2MHz"},
                           {"_GPIO_ALT_OUT_OPEN_DRAIN_10MHZ", "Alternative output Open drain 10MHz"},
                           {"_GPIO_ALT_OUT_OPEN_DRAIN_50MHZ", "Alternative output Open drain 50MHz"},
                           {"_GPIO_IN_ANALOG"               , "Analog input"},
                           {"_GPIO_IN_FLOAT"                , "Float input"},
                           {"_GPIO_IN_PULLED"               , "Input pull-up/pull-down"}}

        local function pin_name_str(port, pin)
                return "__GPIO_"..port_name[port].."_PIN_"..pin.."_NAME__"
        end

        local function pin_mode_str(port, pin)
                return "__GPIO_"..port_name[port].."_PIN_"..pin.."_MODE__"
        end

        local function pin_state_str(port, pin)
                return "__GPIO_"..port_name[port].."_PIN_"..pin.."_STATE__"
        end

        local function configure_pin_name(port, pin)
                local pin_name = key_read(cfg_file, pin_name_str(port, pin))
                msg(progress(3 * pin + 1, gpio_port.stm32f1.pins * 3) .. port_name[port] .. pin .." name configuration.")
                msg("Current pin name: "..pin_name..".")
                pin_name = get_string()
                if can_be_saved(pin_name) then
                        pin_name = pin_name:gsub('"', "")
                        key_save(cfg_file, pin_name_str(port, pin), pin_name)
                end

                return pin_name
        end

        local function configure_pin_mode(port, pin)
                local pin_mode = key_read(cfg_file, pin_mode_str(port, pin))
                msg(progress(3 * pin + 2, gpio_port.stm32f1.pins * 3) .. port_name[port] .. pin .." mode configuration.")
                msg("Current pin mode: "..table.find(mode, pin_mode)..".")

                for i,v in pairs(mode) do
                        add_item(v[1], v[2])
                end

                pin_mode = get_selection()
                if can_be_saved(pin_mode) then
                        key_save(cfg_file, pin_mode_str(port, pin), pin_mode)
                end

                return pin_mode
        end

        local function configure_pin_state(port, pin)
                local str1      = progress(3 * pin + 3, gpio_port.stm32f1.pins * 3) .. port_name[port] .. pin .. " default state configuration."
                local str2      = "Current state: "
                local pin_state = key_read(cfg_file, pin_state_str(port, pin))
                local pin_mode  = key_read(cfg_file, pin_mode_str(port, pin))

                if pin_mode == "_GPIO_IN_PULLED" then
                        msg(str1)

                        if pin_state == "_LOW" or pin_state == "_FLOAT" then
                                msg(str2.."Pull-down.")
                        else
                                msg(str2.."Pull-up.")
                        end

                        add_item("_LOW", "Pull-down")
                        add_item("_HIGH", "Pull-up")
                        pin_state = get_selection()

                elseif pin_mode == "_GPIO_IN_ANALOG" or pin_mode == "_GPIO_IN_FLOAT" then
                        pin_state = "_FLOAT"

                else
                        msg(str1)

                        if pin_state == "_LOW" or pin_state == "_FLOAT" then
                                msg(str2.."Low (0)")
                        else
                                msg(str2.."High (1)")
                        end

                        add_item("_LOW", "Low (0)")
                        add_item("_HIGH", "High (1)")
                        pin_state = get_selection()
                end

                if can_be_saved(pin_state) then
                        key_save(cfg_file, pin_state_str(port, pin), pin_state)
                end

                return pin_state
        end

        local function port_configure(port)
                local pin = 0
                repeat
                        title("GPIO configuration for " .. db:get_cpu_name().." ("..port_name[port]..pin..")")

                        ::pin_name_cfg::
                        if configure_pin_name(port, pin) == back then
                                if pin == 0 then
                                        modify_current_step(-2)
                                        return back
                                else
                                        pin = pin - 1

                                        local pin_mode = key_read(cfg_file, pin_mode_str(port, pin))
                                        if pin_mode == "_GPIO_IN_ANALOG" or pin_mode == "_GPIO_IN_FLOAT" then
                                                goto pin_mode_cfg
                                        else
                                                goto pin_state_cfg
                                        end
                                end
                        end

                        ::pin_mode_cfg::  if configure_pin_mode(port, pin)  == back then goto pin_name_cfg end
                        ::pin_state_cfg:: if configure_pin_state(port, pin) == back then goto pin_mode_cfg end

                        pin = pin + 1
                until pin >= gpio_port.stm32f1.pins
        end

        local function print_summary()
                msg("GPIO module configuration summary:")

                local str = ""

                for port = 1, get_ports_count() do
                        str = str.."Port "..gpio_port.stm32f1.port_list[port].."\n"

                        for pin = 0, gpio_port.stm32f1.pins - 1 do
                                local pin_name = key_read(db.path.stm32f1.gpio.flags, "__GPIO_"..gpio_port.stm32f1.port_list[port].."_PIN_"..pin.."_NAME__")
                                local pin_mode = key_read(db.path.stm32f1.gpio.flags, "__GPIO_"..gpio_port.stm32f1.port_list[port].."_PIN_"..pin.."_MODE__")
                                pin_mode = table.find(mode, pin_mode)
                                str = str.."  "..gpio_port.stm32f1.port_list[port]..pin..": "..pin_name.."   ["..pin_mode.."]\n"
                        end

                        str = str.."\n"
                end

                msg(str)

                pause()
        end

        repeat
                title("GPIO configuration for " .. db:get_cpu_name())
                msg(progress(2, 2).."Select port to configure.")

                for i = 1, get_ports_count() do
                        add_item(port_name[i], "Port "..port_name[i])
                end
                local port = get_selection()

                for i = 1, get_ports_count() do
                        if port_name[i] == port then
                                port_configure(i)
                        end
                end

                if port == back then
                        return back
                else
                        print_summary()
                        return next
                end
        until false
end

--------------------------------------------------------------------------------
-- @brief Disable unused ports for stm32f1
--------------------------------------------------------------------------------
gpio_port.stm32f1.disable_unused = function()
        for i = 1, #gpio_port.stm32f1.port_list do
                local enable = no
                if i <= get_ports_count() then
                        enable = yes
                end

                key_save(db.path.stm32f1.gpio.flags, "__GPIO_"..gpio_port.stm32f1.port_list[i].."_ENABLE__", enable)
        end
end

--------------------------------------------------------------------------------
-- @brief Port configuration for stm32f2
--------------------------------------------------------------------------------
gpio_port.stm32f2.configure = function(port)
        msg("Not implemented")
        pause()
        return next
end

--------------------------------------------------------------------------------
-- @brief Disable unused ports for stm32f2
--------------------------------------------------------------------------------
gpio_port.stm32f2.disable_unused = function()
        for i = 1, #gpio_port.stm32f2.port_list do
                local enable = no
                if i <= get_ports_count() then
                        enable = yes
                end

                key_save(db.path.stm32f2.gpio.flags, "__GPIO_"..gpio_port.stm32f2.port_list[i].."_ENABLE__", enable)
        end
end

--------------------------------------------------------------------------------
-- @brief Port configuration for stm32f3
--------------------------------------------------------------------------------
gpio_port.stm32f3.configure = function(port)
        msg("Not implemented")
        pause()
        return next
end

--------------------------------------------------------------------------------
-- @brief Disable unused ports for stm32f3
--------------------------------------------------------------------------------
gpio_port.stm32f3.disable_unused = function()
        for i = 1, #gpio_port.stm32f3.port_list do
                local enable = no
                if i <= get_ports_count() then
                        enable = yes
                end

                key_save(db.path.stm32f3.gpio.flags, "__GPIO_"..gpio_port.stm32f3.port_list[i].."_ENABLE__", enable)
        end
end

--------------------------------------------------------------------------------
-- @brief Port configuration for stm32f4
--------------------------------------------------------------------------------
gpio_port.stm32f4.configure = function(port)
        msg("Not implemented")
        pause()
        return next
end

--------------------------------------------------------------------------------
-- @brief Disable unused ports for stm32f4
--------------------------------------------------------------------------------
gpio_port.stm32f4.disable_unused = function()
        for i = 1, #gpio_port.stm32f4.port_list do
                local enable = no
                if i <= get_ports_count() then
                        enable = yes
                end

                key_save(db.path.stm32f4.gpio.flags, "__GPIO_"..gpio_port.stm32f4.port_list[i].."_ENABLE__", enable)
        end
end

--------------------------------------------------------------------------------
-- PUBLIC METHODS
--------------------------------------------------------------------------------
function gpio:get_pins()
        local arch = db:get_arch()
        local pins = {}

        if arch == "stm32f1" then
                for port = 1, get_ports_count() do
                        for pin = 0, gpio_port.stm32f1.pins - 1 do
                                local name = key_read(db.path.stm32f1.gpio.flags, "__GPIO_"..gpio_port.stm32f1.port_list[port].."_PIN_"..pin.."_NAME__")
                                table.insert(pins, name)
                        end
                end
        end

        return pins
end

--------------------------------------------------------------------------------
-- @brief Function execute configuration
--------------------------------------------------------------------------------
function gpio:configure()
        title("GPIO configuration for " .. db:get_cpu_name())
        navigation("Home/Modules/GPIO")

        gpio_port[db:get_arch()].disable_unused()

        ::gpio_enable::
        if configure_enable() == back then
                return back
        end

        if key_read(db.path.project.mk, "ENABLE_GPIO") == yes then
                if gpio_port[db:get_arch()].configure() == back then
                        goto gpio_enable
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
