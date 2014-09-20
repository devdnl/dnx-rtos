--[[============================================================================
@file    new_module.lua

@author  Daniel Zorychta

@brief   Script that helps create a new module

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


==============================================================================]]

--==============================================================================
-- EXTERNAL MODULES
--==============================================================================
require("wx")
require("modules/ctcore")


--==============================================================================
-- GLOBAL OBJECTS
--==============================================================================
new_module = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local ui = {}
local ID = {}


--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function loads all controls from configuration files
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_controls()

end


--==============================================================================
-- GLOBAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function new_module:create_window(parent)
        if ui.window == nil then
                ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
                local this = ui.window

                ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)


                local file = io.open("modules/new_module.lua", "r+")
                assert(file)


                local line = ct:found_line(file, "function new_module:create_window%(parent%)")
                if line ~= 0 then
                        print("Found in line "..line)
                end

--                 line = ct:found_line(file, "function new_module:create_window(parent)", 1)
--                 if line ~= 0 then
--                         print("Found in line "..line)
--                 end
--
--                 line = ct:found_line(file, "function new_module:create_window(parent)", 1)
--                 if line ~= 0 then
--                         print("Found in line "..line)
--                 end

                file:close()


                this:SetSizer(ui.FlexGridSizer1)
                this:SetScrollRate(5, 5)

                --
        end

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @param  None
-- @return Module name
--------------------------------------------------------------------------------
function new_module:get_window_name()
        return "New module"
end


--------------------------------------------------------------------------------
-- @brief  Function is called when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function new_module:refresh()
        load_controls()
end


--------------------------------------------------------------------------------
-- @brief  Function check if options are modified
-- @param  None
-- @return true if options are modified, otherwise false
--------------------------------------------------------------------------------
function new_module:is_modified()
        return false
end
