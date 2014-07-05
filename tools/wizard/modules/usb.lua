--[[============================================================================
@file    usb.lua

@author  Daniel Zorychta

@brief   USB configuration wizard.

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
usb = {}

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
-- @brief Ask user to select enable/disable module
--------------------------------------------------------------------------------
local function ask_for_enable()
        local choice = module_get_enable("USB")
        msg(progress(1, 4).."Do you want to enable USB module?")
        msg("Current selection is: "..filter_yes_no(choice)..".")
        add_item(yes, "Yes")
        add_item(no, "No")
        choice = get_selection()
        if can_be_saved(choice) then
                module_enable("USB", choice)
        end

        progress(2)

        return choice
end

--------------------------------------------------------------------------------
-- @brief Configuration for STM32F1
--------------------------------------------------------------------------------
arch.stm32f1.configure = function()
        
        local function configure_priority()
                local choice = key_read(db.path.stm32f1.usb.flags, "__USB_IRQ_PRIORITY__")

                msg(progress().."USB IRQ priority configuration.")
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
                        key_save(db.path.stm32f1.usb.flags, "__USB_IRQ_PRIORITY__", choice)
                end

                return choice
        end
        
        local function endpoint_0_size()
                local choice = key_read(db.path.stm32f1.usb.flags, "__USB_ENDPOINT0_SIZE__")
                msg(progress().."Configuration of the buffer size of the Endpoint 0")
                msg("Current selection is: "..choice.." bytes.")
                add_item("8",  "8 bytes" )
                add_item("16", "16 bytes")
                add_item("32", "32 bytes")
                add_item("64", "64 bytes")
                choice = get_selection()
                if can_be_saved(choice) then
                      key_save(db.path.stm32f1.usb.flags, "__USB_ENDPOINT0_SIZE__", choice)
                end
                
                return choice
        end
        
        local function visible_pullup_pin()
                local choice = key_read(db.path.stm32f1.usb.flags, "__USB_PULLUP_PIN__")
                msg(progress().."USB D+ pullup pin name selection.")
                msg("Current choice is: "..choice)
                local pins = gpio:get_pins()
                for i = 1, #pins do
                        add_item(pins[i], pins[i])
                end
                choice = get_selection()
                if can_be_saved(choice) then
                        key_save(db.path.stm32f1.usb.flags, "__USB_PULLUP_PIN__", choice)
                end

                return choice
        end

        local function print_summary()
                local ep0_size = key_read(db.path.stm32f1.usb.flags, "__USB_ENDPOINT0_SIZE__")
                local priority = key_read(db.path.stm32f1.usb.flags, "__USB_IRQ_PRIORITY__")
                local pullup   = key_read(db.path.stm32f1.usb.flags, "__USB_PULLUP_PIN__")
        
                if priority == "CONFIG_USER_IRQ_PRIORITY" then
                        priority = "Default"
                else
                        priority = math.floor(tonumber(priority) / 16)
                end
        
                msg("USB module configuration summary:")
                msg("  Endpoint 0 buffer size: "..ep0_size.." bytes\n"..
                    "  Interrupt priority: "..priority.."\n"..
                    "  D+ pullup pin name: "..pullup)
                
                pause()
        end

        if module_get_enable("USB") == yes then

                local pages = {configure_priority,
                               endpoint_0_size,
                               visible_pullup_pin,
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
function usb:configure()
        title("USB module configuration for " .. db:get_cpu_name())
        navigation("Home/Modules/USB")
        progress(1, 1)

        ::module_enable::
        if ask_for_enable() == back then
                return back
        end

        if module_get_enable("USB") == yes then
                if arch[db:get_arch()].configure() == back then
                        goto module_enable
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
