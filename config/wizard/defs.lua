--[[============================================================================
@file    defs.lua

@author  Daniel Zorychta

@brief   Configuration definitions.

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

--------------------------------------------------------------------------------
-- GLOBAL VARIABLES
--------------------------------------------------------------------------------
yes             = "__YES__"
no              = "__NO__"
current_step    = 1
total_steps     = 0
back            = "wXeVluSSUkF9sMtietKfvg=Back"
skip            = ""
next            = skip

local no_master = false

--------------------------------------------------------------------------------
-- FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Function create progress string
-- @return progress string
--------------------------------------------------------------------------------
function progress()
        local msg = "(" .. current_step .. "/" .. total_steps .. ") \n"
        current_step = current_step + 1
        return msg
end

--------------------------------------------------------------------------------
-- @brief Function adds to current step value
--------------------------------------------------------------------------------
function modify_current_step(value)
        current_step = current_step + value
end

--------------------------------------------------------------------------------
-- @brief Function adds to total step value
--------------------------------------------------------------------------------
function modify_total_steps(value)
        total_steps = total_steps + value
end

--------------------------------------------------------------------------------
-- @brief Function set total step value
--------------------------------------------------------------------------------
function set_total_steps(steps)
        total_steps = steps
end

--------------------------------------------------------------------------------
-- @brief Function set current step value
--------------------------------------------------------------------------------
function set_current_step(step)
        current_step = step
end

--------------------------------------------------------------------------------
-- @brief The last message after configuration
--------------------------------------------------------------------------------
function configuration_finished()
        title("Configuration done!")
        msg("Your configuration is done. If you want to configure your project again, then run wizard as previously.")
        msg("To compile your project enter specified commands in the command line:")
        msg("        make clean")
        msg("        make")
        msg("")
        msg("Enjoy!")
        pause()
end

--------------------------------------------------------------------------------
-- @brief Function prints message if master wizard is not running
--------------------------------------------------------------------------------
function show_no_master_info()
        if (no_master == false) then
                title("Information")
                msg("Master wizard file is not started. To turn configuration please run wizard.lua file.")
                pause()
                no_master = true
        end
end

--------------------------------------------------------------------------------
-- @brief Function check if value can be saved
-- If value can be saved then true is returned, otherwise false. If value is the
-- back value then current config number is decremented.
-- @return Return true if can be saved, otherwise false
--------------------------------------------------------------------------------
function can_be_saved(value)
        if (value ~= back and value ~= skip) then
                return true
        else
                if (value == back) then
                        current_step = current_step - 2
                end

                return false
        end
end

--------------------------------------------------------------------------------
-- END OF FILE
--------------------------------------------------------------------------------
