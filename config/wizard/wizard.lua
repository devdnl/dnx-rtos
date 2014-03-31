--[[============================================================================
@file    wizard.lua

@author  Daniel Zorychta

@brief   Configuration wizard file. File used to configuration selection.

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

-- set this file as master file
master = true

require "defs"
require "cpu"
require "rtos"
require "memory"
require "project"

--------------------------------------------------------------------------------
-- FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Function ask user to select configuration
-- @return selected configuration
--------------------------------------------------------------------------------
function ask_for_configuration()
        title("Welcome to dnx RTOS configuration!")
        msg("The configuration is divided to few parts, that each configure some part of code.")
        msg("If none part was selected, then entire system will be configured. What the part would you like to configure?")
        add_item("*", "Entire project")
        add_item("project","Project (name, toolchain, etc)")
        add_item("cpu", "CPU (architecture, microcontroller selection)")
        add_item("rtos", "Operating System")
        add_item("mem", "Dynamic Memory Management")
        add_item("fs", "File systems")
        add_item("net", "Network")
        add_item("mod", "Modules (drivers)")
        local configure = get_selection()
        if (configure == "") then
                configure = "*"
        end

        return configure
end

--------------------------------------------------------------------------------
-- CONFIGURATION BEGIN
--------------------------------------------------------------------------------
configure = ask_for_configuration()

if (configure == "*" or configure == "project") then
        project_configure()
end

if (configure == "*" or configure == "cpu") then
        cpu_configure()
end

if (configure == "*" or configure == "rtos") then
        rtos_configure()
end

if (configure == "*" or configure == "mem") then
        mem_configure()
end

configuration_finished()
