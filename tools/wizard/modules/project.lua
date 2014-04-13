--[[============================================================================
@file    project.lua

@author  Daniel Zorychta

@brief   Project configuration file.

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
-- OBJECT DEFINITIONS
--------------------------------------------------------------------------------
pro = {}

--------------------------------------------------------------------------------
-- FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Function configure project name
--------------------------------------------------------------------------------
local function configure_project_name()
        msg(progress() .. "Project name configuration.")
        local str = key_read(db.path.project.mk, "PROJECT_NAME")
        msg("Current project name is: " .. str .. ".")
        str = get_string()
        if (can_be_saved(str)) then
                key_save(db.path.project.mk, "PROJECT_NAME", str)
        end

        return str
end

--------------------------------------------------------------------------------
-- @brief Function configure toolchain name
--------------------------------------------------------------------------------
local function configure_toolchain_name()
        local message = "Toolchain name configuration."
        local curr_is = "Current toolchain name is: "
        local choice  = key_read(db.path.project.mk, "PROJECT_TOOLCHAIN")

        msg(progress() .. message)
        msg(curr_is .. choice)
        add_item("arm-none-eabi-", "arm-none-eabi - e.g. Linaro, CodeSourcery")
        add_item("other", "Other")
        local name = get_selection()
        if (can_be_saved(name)) then
                if (name == "other") then
                        modify_current_step(-1)
                        msg(progress() .. message)
                        msg(curr_is .. choice)
                        name = get_string()
                end

                if (can_be_saved(name)) then
                        key_save(db.path.project.mk, "PROJECT_TOOLCHAIN", name)
                end
        end

        return name
end

--------------------------------------------------------------------------------
-- @brief Function execute configuration
--------------------------------------------------------------------------------
function pro:configure()
        title("General project configuration")
        navigation("Home/Project")
        set_total_steps(2)

        ::project_name::   if configure_project_name()   == back then return back       end
        ::toolchain_name:: if configure_toolchain_name() == back then goto project_name end

        return next
end

-- started without master file
if (master ~= true) then
        show_no_master_info()
end

--------------------------------------------------------------------------------
-- END OF FILE
--------------------------------------------------------------------------------
