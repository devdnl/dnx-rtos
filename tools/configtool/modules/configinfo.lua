--[[============================================================================
@file    configinfo.lua

@author  Daniel Zorychta

@brief   Script describe configurations

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
configinfo = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local ui = {}
local ID = {}


--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================


--==============================================================================
-- GLOBAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function configinfo:create_window(parent)
        if ui.window == nil then
                ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
                local this = ui.window

                ui.BoxSizer1 = wx.wxBoxSizer(wx.wxHORIZONTAL);
                ui.FlexGridSizer1 = wx.wxFlexGridSizer(3, 1, 0, 0)

                ui.StaticText = wx.wxStaticText(this, wx.wxID_ANY, "Configuration", wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxALIGN_LEFT)
                ui.StaticText:SetFont(wx.wxFont(14, wx.wxDEFAULT, wx.wxFONTSTYLE_NORMAL, wx.wxBOLD, false, "", wx.wxFONTENCODING_DEFAULT))
                ui.FlexGridSizer1:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText = wx.wxStaticText(this, wx.wxID_ANY, "\nThe configuration contains a few parts, each one configures a specified module.")
                ui.FlexGridSizer1:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText = wx.wxStaticText(this, wx.wxID_ANY, "\nThe specified options configures:")
                ui.FlexGridSizer1:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText = wx.wxStaticText(this, wx.wxID_ANY, "    - Project: general project configuration (project and toolchain name, CPU, etc)")
                ui.FlexGridSizer1:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText = wx.wxStaticText(this, wx.wxID_ANY, "    - Operating System: all settings of the Operating System")
                ui.FlexGridSizer1:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText = wx.wxStaticText(this, wx.wxID_ANY, "    - File Systems: configuration of all file systems existing in the system")
                ui.FlexGridSizer1:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText = wx.wxStaticText(this, wx.wxID_ANY, "    - Network: configuration of newtwork")
                ui.FlexGridSizer1:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText = wx.wxStaticText(this, wx.wxID_ANY, "    - Modules: configuration of modules existing in the selected microcontroller")
                ui.FlexGridSizer1:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText = wx.wxStaticText(this, wx.wxID_ANY, "    - System startup: configuration of initd")
                ui.FlexGridSizer1:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.BoxSizer1:Add(ui.FlexGridSizer1, 1, bit.bor(wx.wxALL,wx.wxALIGN_TOP,wx.wxALIGN_CENTER_HORIZONTAL), 0)

                this:SetSizer(ui.BoxSizer1)
                this:SetScrollRate(5, 5)
        end

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @param  None
-- @return Module name
--------------------------------------------------------------------------------
function configinfo:get_window_name()
        return "Configuration"
end


--------------------------------------------------------------------------------
-- @brief  Function is called when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function configinfo:refresh()
end


--------------------------------------------------------------------------------
-- @brief  Function check if options are modified
-- @param  None
-- @return true if options are modified, otherwise false
--------------------------------------------------------------------------------
function configinfo:is_modified()
        return false
end


--------------------------------------------------------------------------------
-- @brief  Function save configuration
-- @return None
--------------------------------------------------------------------------------
function configinfo:save()
end
