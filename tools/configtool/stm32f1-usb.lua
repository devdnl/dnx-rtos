--[[============================================================================
@file    stm32f1-usb.lua

@author  Daniel Zorychta

@brief   Configuration script for USB module

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
usb = {}


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
function usb:create_window(parent)
        ui = {}
        ID = {}

        ui.window  = wx.wxPanel(parent, wx.wxID_ANY)
        local this = ui.window

        choice = wx.wxChoice(this, -1, wx.wxPoint(10, 100), wx.wxSize(wizcore.CONTROL_X_SIZE, -1), {}, 0, wx.wxDefaultValidator, "ID.CHOICE_TIMEOUT")
        gpio = require("stm32f1-gpio").get_handler()
        choice:Append(gpio:get_pin_list(true))
        
        prio = wizcore:get_priority_list("stm32f1")
        for idx, item in ipairs(prio) do
                print(item.name, item.value)
        end

        button = wx.wxButton(this, wx.wxID_ANY, "USB")


--         ui.window:Enable(false)

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @param  None
-- @return Module name
--------------------------------------------------------------------------------
function usb:get_window_name()
        return "USB"
end


--------------------------------------------------------------------------------
-- @brief  Function is called by parent when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function usb:selected()
        print("usb:selected()")
end


--------------------------------------------------------------------------------
-- @brief  Function returns modify status
-- @param  None
-- @return If data is modified true is returned, otherwise false
--------------------------------------------------------------------------------
function usb:is_modified()
        return false
end


--------------------------------------------------------------------------------
-- @brief  Function returns module handler
-- @param  None
-- @return Module handler
--------------------------------------------------------------------------------
function get_handler()
        return usb
end
