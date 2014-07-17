--[[============================================================================
@file    stm32f1-sdspi.lua

@author  Daniel Zorychta

@brief   Configuration script for SDSPI module.

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
module(..., package.seeall)


--==============================================================================
-- EXTERNAL MODULES
--==============================================================================
require("wx")
require("wizcore")


--==============================================================================
-- GLOBAL OBJECTS
--==============================================================================
-- module's main object
sdspi = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
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
local function load_controls()

        ui.CheckBox_enable:SetValue(enable)
        ui.Panel1:Enable(enable)
end


--------------------------------------------------------------------------------
-- @brief  x
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function on_button_save_click()
        local enable = ui.CheckBox_enable:GetValue()


        ui.Button_save:Enable(false)
end


--------------------------------------------------------------------------------
-- @brief  x
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function checkbox_enable_updated(this)
        ui.Button_save:Enable(true)
        ui.Panel1:Enable(this:IsChecked())
end


--------------------------------------------------------------------------------
-- @brief  x
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function value_updated()
        ui.Button_save:Enable(true)
end


--==============================================================================
-- GLOBAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function sdspi:create_window(parent)
        ui = {}

        ID = {}
        ID.CHECKBOX_ENABLE = wx.wxNewId()
        ID.PANEL1 = wx.wxNewId()


        ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
        local this = ui.window



        --
        this:SetSizer(ui.FlexGridSizer1)
        this:SetScrollRate(50, 50)

        --
        this:Connect(ID.CHECKBOX_ENABLE, wx.wxEVT_COMMAND_CHECKBOX_CLICKED, checkbox_enable_updated)
        this:Connect(ID.CHECKBOX_LOCK,   wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_updated          )
        this:Connect(ID.CHECKBOX_DEBUG,  wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_updated          )
        this:Connect(ID.CHOICE_TIMEOUT,  wx.wxEVT_COMMAND_CHOICE_SELECTED,  value_updated          )
        this:Connect(ID.BUTTON_SAVE,     wx.wxEVT_COMMAND_BUTTON_CLICKED,   on_button_save_click   )

        --
        load_controls()
        ui.Button_save:Enable(false)

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @param  None
-- @return Module name
--------------------------------------------------------------------------------
function sdspi:get_window_name()
        return "WDG"
end


--------------------------------------------------------------------------------
-- @brief  Function is called by parent when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function sdspi:selected()
end


--------------------------------------------------------------------------------
-- @brief  Function returns modify status
-- @param  None
-- @return If data is modified true is returned, otherwise false
--------------------------------------------------------------------------------
function sdspi:is_modified()
        return ui.Button_save:IsEnabled()
end


--------------------------------------------------------------------------------
-- @brief  Function returns module handler
-- @param  None
-- @return Module handler
--------------------------------------------------------------------------------
function get_handler()
        return sdspi
end
