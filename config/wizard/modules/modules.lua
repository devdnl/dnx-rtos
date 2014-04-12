--[[============================================================================
@file    modules.lua

@author  Daniel Zorychta

@brief   Modules configuration wizard.

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
require "gpio"
require "afio"
require "crc"
require "pll"
require "eth"
require "sdspi"
require "spi"

--------------------------------------------------------------------------------
-- OBJECTS
--------------------------------------------------------------------------------
-- class definition
mod = {}

--------------------------------------------------------------------------------
-- FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Ask user to select module to configuration
--------------------------------------------------------------------------------
local function ask_for_module()
        local name = cpu:get_name()

        title("Module Configuration Menu for " .. name)
        navigation("Home/Modules")
        msg("Here are listed only implemented modules for selected microcontroller. Select module to configure.")
        for i, m in pairs(db:get_mcu_modules_list(name)) do
                add_item(m, db:get_module_description(m))
        end
        add_item(back, "Exit - previous menu")

        return get_selection()
end

--------------------------------------------------------------------------------
-- @brief Function execute configuration
--------------------------------------------------------------------------------
function mod:configure()
        while true do
                local choice = ask_for_module()
                if choice == back then
                        return back
                end

                set_current_step(1)

                if     choice == "GPIO" then
                        gpio:configure()
                elseif choice == "AFIO" then
                        afio:configure()
                elseif choice == "CRC" then
                        crc:configure()
                elseif choice == "ETH" then
                        eth:configure()
                elseif choice == "PLL" then
                        pll:configure()
                elseif choice == "SDSPI" then
                        sdspi:configure()
                elseif choice == "SPI" then
                        spi:configure()
                elseif choice == "TTY" then
                elseif choice == "UART" then
                elseif choice == "WDG" then
                elseif choice == "I2S" then
                elseif choice == "USB" then
                elseif choice == "USBOTG" then
                elseif choice == "I2C" then
                elseif choice == "ADC" then
                elseif choice == "DAC" then
                elseif choice == "SDIO" then
                elseif choice == "FSMC" then
                elseif choice == "HDMICEC" then
                end
        end
end

-- started without master file
if (master ~= true) then
        show_no_master_info()
end

--------------------------------------------------------------------------------
-- END OF FILE
--------------------------------------------------------------------------------
