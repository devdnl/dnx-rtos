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

require "db"

--------------------------------------------------------------------------------
-- GLOBAL VARIABLES
--------------------------------------------------------------------------------
yes             = "__YES__"
no              = "__NO__"
current_step    = 1
total_steps     = 0
back            = "wXeVluSSUkF9sMtietKfvg=Back"
cancel          = "HDhfV99oIyr1kpJgiD+o=Cancel"
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
function progress(current, total)
        if current ~= nil then
                current_step = current
        end

        if total ~= nil then
                total_steps  = total
        end

        local msg = "(" .. current_step .. "/" .. total_steps .. ") \n"

        if current == nil and total == nil then
                current_step = current_step + 1
        end

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
                msg("Master wizard is not started. To start configuration wizard, please run wizard.lua file, or type 'make xconfig' or 'make config' in the command line.")
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
-- @brief Function remove additional characters from YES/NO answer
-- @return Return cleared YES/NO string
--------------------------------------------------------------------------------
function filter_yes_no(yes_no)
        return string.gsub(yes_no, "_", "")
end

--------------------------------------------------------------------------------
-- @brief Function find in table t key k (from subtable)
-- @param t     table
-- @param k     key
-- @return Return on success value, otherwise nil
--------------------------------------------------------------------------------
function table.find(t, k)
        for i,v in pairs(t) do
                if v[1] == k then
                        return v[2]
                end
        end

        return nil
end

--------------------------------------------------------------------------------
-- @brief Function round value
-- @param value         value to round
-- @param precision     number of digits
-- @return rounded value
--------------------------------------------------------------------------------
function round(value, precision)
        return math.floor(value * 10^(precision or 0)) / 10^(precision or 0)
end

--------------------------------------------------------------------------------
-- @brief Check time value and set unit
-- @param time          time
-- @return string with calculated time and unit
--------------------------------------------------------------------------------
function tunit(time)
        if time < 1e-6 then
                return time*1e9 .." ns"
        elseif time < 1e-3 then
                return time*1e6 .." us"
        elseif time < 1 then
                return time*1e3 .." ms"
        else
                return time.." s"
        end
end

--------------------------------------------------------------------------------
-- @brief Calculate frequency unit
-- @param freq          frequency
-- @param string of frequency with unit
--------------------------------------------------------------------------------
function funit(freq)
        if freq < 1e3 then
                return freq.." Hz"
        elseif freq < 1e6 then
                return freq/1e3 .." kHz"
        else
                return freq/1e6 .." MHz"
        end
end

--------------------------------------------------------------------------------
-- @brief Create pages from table
-- @param tab           table with functions that creates pages
-- @param begin         begin page (-1 for last etc)
-- @param step          current step
-- @param total         total steps
-- @return transaction (next, back, cancel)
--------------------------------------------------------------------------------
function show_pages(tab, begin, step, total)
        local current = 1

        if begin ~= nil then
                if begin ~= 0 then
                        if begin < 0 then
                                current = #tab + 1 + begin
                        else
                                if begin <= #tab then
                                        current = begin
                                end
                        end
                end
        end

        if step ~= nil or total ~= nil then
                progress(step, total)
        end

        repeat
                local ret = tab[current]()

                if ret == back then
                        current = current - 1
                elseif ret == cancel then
                        return cancel
                else
                        current = current + 1
                end
        until current <= 0 or current > #tab

        if current == 0 then
                return back
        else
                return next
        end
end

--------------------------------------------------------------------------------
-- @brief Function enable/disbale selected module
-- @param module  module name
-- @param enable  enable value
-- @return None
--------------------------------------------------------------------------------
function module_enable(module, enable)
        local choice

        if enable == true or enable == yes then
              choice = yes
        else
              choice = no
        end

        key_save(db.path.project.mk, "ENABLE_"..module, choice)
        key_save(db.path.project.flags, "__ENABLE_"..module.."__", choice)
end

--------------------------------------------------------------------------------
-- @brief Function return module enable status
-- @param module  module name
-- @return Return yes or no
--------------------------------------------------------------------------------
function module_get_enable(module)
        return key_read(db.path.project.mk, "ENABLE_"..module, choice)
end

--------------------------------------------------------------------------------
-- END OF FILE
--------------------------------------------------------------------------------
