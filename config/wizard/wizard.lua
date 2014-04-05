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

master = true

package.path = './modules/?.lua;'..package.path

require "defs"
require "cpu"
require "rtos"
require "memory"
require "project"
require "fs"
require "network"
require "modules"

--------------------------------------------------------------------------------
-- FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Function ask user to select configuration
-- @return selected configuration
--------------------------------------------------------------------------------
local function ask_for_configuration()
        title("Welcome to dnx RTOS configuration!")
        navigation("Home")
        msg("The configuration is divided to few parts, each one configure specified system module.")
        msg("What the part would you like to configure?")
        add_item("project","Project (name, toolchain, etc)")
        add_item("cpu", "CPU (architecture, microcontroller selection)")
        add_item("rtos", "Operating System")
        add_item("mem", "Dynamic Memory Management")
        add_item("fs", "File systems")
        add_item("net", "Network")
        add_item("mod", "Modules (drivers)")
        add_item("exit", "Exit - close wizard")
        return get_selection()
end

--------------------------------------------------------------------------------
-- CONFIGURATION BEGIN
--------------------------------------------------------------------------------
while true do
        repeat
                configure = ask_for_configuration()
                if (configure == "exit") then return end
        until not(configure == skip or configure == back)

        set_current_step(1)

        if (configure == "project") then
                pro:configure()

        elseif (configure == "cpu") then
                cpu:configure()

        elseif (configure == "rtos") then
                rtos:configure()

        elseif (configure == "mem") then
                mem:configure()

        elseif (configure == "fs") then
                fs:configure()

        elseif (configure == "net") then
                net:configure()

        elseif (configure == "mod") then
                mod:configure()
        end
end
