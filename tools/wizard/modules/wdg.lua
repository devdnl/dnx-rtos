--[[============================================================================
@file    eth.lua

@author  Daniel Zorychta

@brief   Ethernet configuration wizard.

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
require "pll"

--------------------------------------------------------------------------------
-- OBJECTS
--------------------------------------------------------------------------------
wdg = {}

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

        if key_read(db.path.project.mk, "ENABLE_ETH") == yes then
                if arch == "stm32f1" then
                        progress(1, 4)
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
        local choice = key_read(db.path.project.flags, "__ENABLE_WDG__")
        msg(progress().."Do you want to enable WDG module?")
        msg("Current selection is: "..filter_yes_no(choice)..".")
        add_item(yes, "Yes")
        add_item(no, "No")
        choice = get_selection()
        if can_be_saved(choice) then
                key_save(db.path.project.flags, "__ENABLE_WDG__", choice)
                key_save(db.path.project.mk, "ENABLE_WDG", choice)
        end

        calculate_total_steps()
        progress(2)

        return choice
end

--------------------------------------------------------------------------------
-- @brief Configuration for STM32F1
--------------------------------------------------------------------------------
arch.stm32f1.configure = function()

        local function configure_dev_lock_at_open()
                local choice = key_read(db.path.stm32f1.wdg.flags, "__WDG_DEVICE_LOCK_AT_OPEN__")
                msg(progress().."Do you want to lock WDG device at first open?")
                msg("Current selection is: "..filter_yes_no(choice)..".")
                add_item(yes, "Yes")
                add_item(no, "No")
                choice = get_selection()
                if can_be_saved(choice) then
                        key_save(db.path.stm32f1.wdg.flags, "__WDG_DEVICE_LOCK_AT_OPEN__", choice)
                end

                return choice
        end

        local function configure_wdg_timeout()
                local divtab  = {{25,    256, 3907},
                                 {20,    256, 3125},
                                 {15,    256, 2344},
                                 {10,    256, 1563},
                                 {5,     256, 782 },
                                 {1,     256, 157 },
                                 {0.5,   64,  313 },
                                 {0.25,  32,  313 },
                                 {0.125, 16,  313 },
                                 {0.064, 8,   320 },
                                 {0.032, 4,   320 },
                                 {0.016, 4,   160 },
                                 {0.008, 4,   80  },
                                 {0.004, 4,   40  },
                                }

                local divider = key_read(db.path.stm32f1.wdg.flags, "__WDG_CLK_DIVIDER__")
                local reload  = key_read(db.path.stm32f1.wdg.flags, "__WDG_RELOAD_VALUE__")
                local lsi     = pll:get_frequencies().LSI
                local choice  = tunit(round(1/(lsi/divider/reload), 3))
                msg(progress().."Select Watchdog timeout.")
                msg("Current selection is: "..choice..".")
                for i = 1, #divtab do
                        add_item(i, tunit(divtab[i][1]))
                end
                choice = get_selection()
                if can_be_saved(choice) then
                        choice = tonumber(choice)
                        key_save(db.path.stm32f1.wdg.flags, "__WDG_CLK_DIVIDER__",  divtab[choice][2])
                        key_save(db.path.stm32f1.wdg.flags, "__WDG_RELOAD_VALUE__", divtab[choice][3])
                end

                return choice
        end

        local function configure_behaviour_on_debug()
                local choice = key_read(db.path.stm32f1.wdg.flags, "__WDG_DISABLE_ON_DEBUG__")
                msg(progress().."Do you want to disable WDG on debug?")
                msg("Current selection is: "..filter_yes_no(choice)..".")
                add_item(yes, "Yes")
                add_item(no, "No")
                choice = get_selection()
                if can_be_saved(choice) then
                        key_save(db.path.stm32f1.wdg.flags, "__WDG_DISABLE_ON_DEBUG__", choice)
                end

                return choice
        end

        local function print_summary()
                local dev_lock = key_read(db.path.stm32f1.wdg.flags, "__WDG_DEVICE_LOCK_AT_OPEN__")
                local divider  = key_read(db.path.stm32f1.wdg.flags, "__WDG_CLK_DIVIDER__")
                local reload   = key_read(db.path.stm32f1.wdg.flags, "__WDG_RELOAD_VALUE__")
                local debug    = key_read(db.path.stm32f1.wdg.flags, "__WDG_DISABLE_ON_DEBUG__")
                local lsi      = pll:get_frequencies().LSI

                msg("WDG module configuration summary:")
                msg("Watchdog locked when device open: "..filter_yes_no(dev_lock).."\n"..
                    "Timeout: "..tunit(round(1/(lsi/divider/reload), 3)).."\n"..
                    "Disable on debug: "..filter_yes_no(dev_lock))

                pause()
        end

        if key_read(db.path.project.mk, "ENABLE_WDG") == yes then

                local pages = {configure_dev_lock_at_open, configure_wdg_timeout, configure_behaviour_on_debug, print_summary}
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
function wdg:configure()
        title("WDG module configuration for " .. db:get_cpu_name())
        navigation("Home/Modules/WDG")
        calculate_total_steps()

        ::eth_enable::
        if ask_for_enable() == back then
                return back
        end

        if key_read(db.path.project.mk, "ENABLE_WDG") == yes then
                if arch[db:get_arch()].configure() == back then
                        goto eth_enable
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
