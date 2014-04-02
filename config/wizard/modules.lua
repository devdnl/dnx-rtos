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

--------------------------------------------------------------------------------
-- FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Configure net enable
--------------------------------------------------------------------------------
local function ask_for_module()
        local arch = get_cpu_arch()

        title("Modules Configuration Menu for " .. arch)
        msg("There are listed only implemented modules for selected microcontroller architecture. Select module to configure.")
        if arch == "stm32f1" then
                add_item("gpio", "GPIO")
        elseif arch == "stm32f2" then
                add_item("gpio", "GPIO")
        elseif arch == "stm32f3" then
                add_item("gpio", "GPIO")
        elseif arch == "stm32f4" then
                add_item("gpio", "GPIO")
        else
                msg("Unknown architecture. Configure correct microcontroller architecture and try again.")
                pause()
                return skip
        end

        return get_selection()
end

--------------------------------------------------------------------------------
-- @brief Function execute configuration
--------------------------------------------------------------------------------
function mod_configure()
        while true do
                local choice = ask_for_module()
                if (choice == skip or choice == back) then
                        return back
                end

                if (choice == "ethmac") then
                        pause()
                end
        end
end

--------------------------------------------------------------------------------
-- Enable configuration if master wizard is not defined
--------------------------------------------------------------------------------
if (master ~= true) then
        while mod_configure() == back do
        end
        configuration_finished()
end

--------------------------------------------------------------------------------
-- END OF FILE
--------------------------------------------------------------------------------
