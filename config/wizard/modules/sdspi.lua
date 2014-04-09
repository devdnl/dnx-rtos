--[[============================================================================
@file    sdspi.lua

@author  Daniel Zorychta

@brief   SDSPI configuration wizard.

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
require "cpu"
require "db"

--------------------------------------------------------------------------------
-- OBJECTS
--------------------------------------------------------------------------------
sdspi = {}

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
        local arch   = cpu:get_arch()
        local family = db:get_mcu_family(cpu:get_name())

        if key_read("../project/Makefile", "ENABLE_SDSPI") == yes then
                if arch == "stm32f1" then
                        progress(1, 8)
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
        local choice = key_read("../project/flags.h", "__ENABLE_SDSPI__")
        msg(progress().."Do you want to enable SDSPI module?")
        msg("Current selection is: "..filter_yes_no(choice)..".")
        add_item(yes, "Yes")
        add_item(no, "No")
        choice = get_selection()
        if can_be_saved(choice) then
                key_save("../project/flags.h", "__ENABLE_SDSPI__", choice)
                key_save("../project/Makefile", "ENABLE_SDSPI", choice)
        end

        calculate_total_steps()
        progress(2)

        return choice
end

--------------------------------------------------------------------------------
-- @brief PLL configuration for STM32F1
--------------------------------------------------------------------------------
arch.stm32f1.configure = function()

        local function print_summary()
        end

        if key_read("../project/Makefile", "ENABLE_SDSPI") == yes then

                print_summary()
        end

        return next
end

--------------------------------------------------------------------------------
-- METHODS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Function execute configuration
--------------------------------------------------------------------------------
function sdspi:configure()
        title("SDSPI module configuration for " .. cpu:get_name())
        navigation("Home/Modules/SDSPI")
        calculate_total_steps()

        ::eth_enable::
        if ask_for_enable() == back then
                return back
        end

        if arch[cpu:get_arch()].configure() == back then
                goto eth_enable
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
