--[[============================================================================
@file    <!module_name!>.lua

@author  <!author!>

@brief   <!module_description!>

@note    Copyright (C) <!year!> <!author!> <<!email!>>

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
module(..., package.seeall)


--==============================================================================
-- EXTERNAL MODULES
--==============================================================================
require("wx")
require("modules/ctcore")


--==============================================================================
-- GLOBAL OBJECTS
--==============================================================================
-- module's main object
<!module_name!> = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local modified = ct:new_modify_indicator()
local ui = {}
local ID = {}


--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function loads all controls from configuration scripts
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_configuration()
        ui.CheckBox_enable:SetValue(ct:get_module_state("<!module_name!>"))
end


--------------------------------------------------------------------------------
-- @brief  Function save configuration
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function save_configuration()
        ct:enable_module("<!module_name!>", ui.CheckBox_enable:GetValue())
        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Event is called when module enable checkbox is changed
-- @param  this         event object
-- @return None
--------------------------------------------------------------------------------
local function checkbox_enable_updated(this)
        modified:yes()
end


--==============================================================================
-- GLOBAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function <!module_name!>:create_window(parent)
        ui = {}

        ID = {}
        ID.CHECKBOX_ENABLE = wx.wxNewId()

        -- create new scrolled window
        ui.window = wx.wxScrolledWindow(parent, wx.wxID_ANY)

        -- add main sizer and module enable checkbox
        ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.CheckBox_enable = wx.wxCheckBox(ui.window, ID.CHECKBOX_ENABLE, "Enable module", wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, -1))
        ui.FlexGridSizer1:Add(ui.CheckBox_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- set main sizer and scroll rate
        ui.window:SetSizer(ui.FlexGridSizer1)
        ui.window:SetScrollRate(10, 10)

        -- connect signals
        ui.window:Connect(ID.CHECKBOX_ENABLE, wx.wxEVT_COMMAND_CHECKBOX_CLICKED, checkbox_enable_updated)

        -- load configuration
        load_configuration()
        modified:no()

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @param  None
-- @return Module name
--------------------------------------------------------------------------------
function <!module_name!>:get_window_name()
        return "<!MODULE_NAME!>"
end


--------------------------------------------------------------------------------
-- @brief  Function is called by parent when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function <!module_name!>:selected()
end


--------------------------------------------------------------------------------
-- @brief  Function returns modify status
-- @param  None
-- @return If data is modified true is returned, otherwise false
--------------------------------------------------------------------------------
function <!module_name!>:is_modified()
        return modified:get_value()
end


--------------------------------------------------------------------------------
-- @brief  Function save configuration
-- @return None
--------------------------------------------------------------------------------
function <!module_name!>:save()
        save_configuration()
end


--------------------------------------------------------------------------------
-- @brief  Function discard modified configuration
-- @return None
--------------------------------------------------------------------------------
function <!module_name!>:discard()
        load_configuration()
        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Function returns module handler
-- @param  None
-- @return Module handler
--------------------------------------------------------------------------------
function get_handler()
        return <!module_name!>
end
