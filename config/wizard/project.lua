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

--------------------------------------------------------------------------------
-- FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Function calculate a total configuration steps
--------------------------------------------------------------------------------
local function calculate_total_steps()
        total_cfg = 2
end

--------------------------------------------------------------------------------
-- @brief Function configure project name
--------------------------------------------------------------------------------
function configure_project_name()
        msg(progress() .. "Project name configuration.")
        local str = key_read("../project/Makefile", "PROJECT_NAME")
        msg("Current project name is: " .. str .. ".")
        str = get_string()
        if (str ~= "") then
                key_save("../project/Makefile", "PROJECT_NAME", str)
        end
end

--------------------------------------------------------------------------------
-- @brief Function configure toolchain name
--------------------------------------------------------------------------------
function configure_toolchain_name()
        local message = "Toolchain name configuration."
        local curr_is = "Current toolchain name is: "

        msg(progress() .. message)
        current_cfg = current_cfg - 1

        local choice = key_read("../project/Makefile", "PROJECT_TOOLCHAIN")
        msg(curr_is .. choice)
        add_item("arm-none-eabi-", "arm-none-eabi - e.g. Linaro, CodeSourcery")
        add_item("other", "Other")
        local name = get_selection()
        if (name ~= "") then
                if (name == "other") then
                        msg(progress() .. message)
                        msg(curr_is .. choice)
                        name = get_string()
                end

                if (name ~= "") then
                        key_save("../project/Makefile", "PROJECT_TOOLCHAIN", name)
                end
        end
end

--------------------------------------------------------------------------------
-- @brief Main function that configure CPU according to current architecture
--------------------------------------------------------------------------------
function project_configure()
        current_cfg = 1
        calculate_total_steps()

        title("General project configuration")
        configure_project_name()
        configure_toolchain_name()
end

--------------------------------------------------------------------------------
-- Enable configuration if master wizard is not defined
--------------------------------------------------------------------------------
if (master ~= true) then
        project_configure()
        configuration_finished()
end

--------------------------------------------------------------------------------
-- END OF FILE
--------------------------------------------------------------------------------
