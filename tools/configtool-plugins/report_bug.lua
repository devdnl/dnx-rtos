--[[============================================================================
@file    report_bug.lua

@author  Daniel Zorychta

@brief   Create menu to report dnx RTOS bug

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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

--------------------------------------------------------------------------------
-- OBJECTS
--------------------------------------------------------------------------------
local plugin = {}

--------------------------------------------------------------------------------
-- PLUGIN FUNCTIONS
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
-- PLUGIN INTERFACE
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief  Function is called when plugin is loaded
-- @param  none
-- @return none
--------------------------------------------------------------------------------
plugin.Loaded = function()
end

--------------------------------------------------------------------------------
-- @brief  Function is called when menu item is added
-- @param  AddItem              function that adds menu item to Plugins menu
-- @param  AddItem(name,        Item name (with shortcut e.g. "&Open\tCtrl-O")
--                 tip,         tip message
--                 icon,        icon path (optional, can be "" or nil)
--                 func)        event function when menu is clicked
-- @return none
--------------------------------------------------------------------------------
plugin.AddMenuItem = function(AddItem)
    AddItem("Report dnx RTOS bug",
            "Report dnx RTOS bug...",
            "pixmaps/16x16/tools-report-bug.png",
            function()
                wx.wxLaunchDefaultBrowser("https://bitbucket.org/devdnl/dnx-rtos/issues/new")
            end)
end

--------------------------------------------------------------------------------
-- @brief  Function is called when toolbar icon is added
-- @param  AddTool              function that adds menu item to Plugins menu
--         AddItem(name,        tool name
--                 tip,         tip message
--                 iconPath,    icon path
--                 func)        event function
-- @return none
--------------------------------------------------------------------------------
plugin.AddToolbarItem = function(AddTool)
end

return plugin
