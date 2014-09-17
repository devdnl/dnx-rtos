--[[============================================================================
@file    i2c.lua

@author  Daniel Zorychta

@brief   Configuration script for I2C module.

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
require("modules/ctcore")


--==============================================================================
-- GLOBAL OBJECTS
--==============================================================================
-- module's main object
i2c = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local ui = {}
local ID = {}
local prio_list = ct:get_priority_list("stm32f1")

--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function loads all controls from configuration scripts
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_controls()
        ui.CheckBox_enable:SetValue(ct:get_module_state("I2C"))
end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function on_button_save_click()
        ct:enable_module("I2C", ui.CheckBox_enable:GetValue())
        ui.Button_save:Enable(false)
end


--------------------------------------------------------------------------------
-- @brief  Event is called when module enable checkbox is changed
-- @param  this         event object
-- @return None
--------------------------------------------------------------------------------
local function checkbox_enable_updated(this)
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
function i2c:create_window(parent)
        ui = {}
        ui.StaticText_device = {}
        ui.TextCtrl_address = {}
        ui.Choice_addr_mode = {}
        ui.Choice_subaddr_mode = {}

        ID = {}
        ID.CHECKBOX_ENABLE = wx.wxNewId()
        ID.STATICLINE1 = wx.wxNewId()
        ID.BUTTON_SAVE = wx.wxNewId()
        ID.CHOICE_I2C = wx.wxNewId()
        ID.CHECKBOX_I2C_ENABLE = wx.wxNewId()
        ID.CHECKBOX_USE_DMA = wx.wxNewId()
        ID.CHOICE_IRQ_PRIORITY = wx.wxNewId()
        ID.CHOICE_SCL_FREQ = wx.wxNewId()
        ID.CHOICE_DEV_NO = wx.wxNewId()
        ID.STATICTEXT_DEVICE = {}
        ID.TEXTCTRL_ADDRESS = {}
        ID.CHOICE_ADDR_MODE = {}
        ID.CHOICE_SUBADDR_MODE = {}
        

        ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
        local this = ui.window

        ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.CheckBox_enable = wx.wxCheckBox(this, ID.CHECKBOX_ENABLE, "Enable module", wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, -1))
        ui.FlexGridSizer1:Add(ui.CheckBox_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        
        ui.FlexGridSizer_module = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.Choice_I2C = wx.wxChoice(this, ID.CHOICE_I2C, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)        
        ui.FlexGridSizer_module:Add(ui.Choice_I2C, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.FlexGridSizer5 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.FlexGridSizer3 = wx.wxFlexGridSizer(0, 2, 0, 0)
        
        ui.CheckBox_I2C_enable = wx.wxCheckBox(this, ID.CHECKBOX_I2C_ENABLE, "Enable peripheral", wx.wxDefaultPosition, wx.wxDefaultSize, 0)
        ui.FlexGridSizer3:Add(ui.CheckBox_I2C_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        
        ui.CheckBox_use_DMA = wx.wxCheckBox(this, ID.CHECKBOX_USE_DMA, "Use DMA", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_use_DMA:SetValue(false)
        ui.FlexGridSizer3:Add(ui.CheckBox_use_DMA, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        
        ui.StaticText2 = wx.wxStaticText(this, wx.wxID_ANY, "IRQ priority", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer3:Add(ui.StaticText2, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_IRQ_priority = wx.wxChoice(this, ID.CHOICE_IRQ_PRIORITY, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
        for i, item in ipairs(prio_list) do ui.Choice_IRQ_priority:Append(item.name) end
        ui.Choice_IRQ_priority:Append("System default")
        ui.FlexGridSizer3:Add(ui.Choice_IRQ_priority, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        
        ui.StaticText3 = wx.wxStaticText(this, wx.wxID_ANY, "SCL frequency", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer3:Add(ui.StaticText3, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_SCL_freq = wx.wxChoice(this, ID.CHOICE_SCL_FREQ, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
        ui.Choice_SCL_freq:Append("10 kHz")
        ui.Choice_SCL_freq:Append("20 kHz")
        ui.Choice_SCL_freq:Append("30 kHz")
        ui.Choice_SCL_freq:Append("40 kHz")
        ui.Choice_SCL_freq:Append("50 kHz")
        ui.Choice_SCL_freq:Append("60 kHz")
        ui.Choice_SCL_freq:Append("70 kHz")
        ui.Choice_SCL_freq:Append("80 kHz")
        ui.Choice_SCL_freq:Append("90 kHz")
        ui.Choice_SCL_freq:Append("100 kHz")
        ui.Choice_SCL_freq:Append("150 kHz")
        ui.Choice_SCL_freq:Append("200 kHz")
        ui.Choice_SCL_freq:Append("250 kHz")
        ui.Choice_SCL_freq:Append("300 kHz")
        ui.Choice_SCL_freq:Append("350 kHz")
        ui.Choice_SCL_freq:Append("400 kHz")
        ui.FlexGridSizer3:Add(ui.Choice_SCL_freq, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        
        ui.StaticText1 = wx.wxStaticText(this, wx.wxID_ANY, "Number of devices", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer3:Add(ui.StaticText1, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_dev_no = wx.wxChoice(this, ID.CHOICE_DEV_NO, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
        ui.Choice_dev_no:Append("1")
        ui.Choice_dev_no:Append("2")
        ui.Choice_dev_no:Append("3")
        ui.Choice_dev_no:Append("4")
        ui.Choice_dev_no:Append("5")
        ui.Choice_dev_no:Append("6")
        ui.Choice_dev_no:Append("7")
        ui.Choice_dev_no:Append("8")
        ui.FlexGridSizer3:Add(ui.Choice_dev_no, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.FlexGridSizer5:Add(ui.FlexGridSizer3, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        
        ui.FlexGridSizer4 = wx.wxFlexGridSizer(0, 4, 0, 0)
        for i = 0, 7 do
                ID.STATICTEXT_DEVICE[i]   = wx.wxNewId()
                ID.TEXTCTRL_ADDRESS[i]    = wx.wxNewId()
                ID.CHOICE_ADDR_MODE[i]    = wx.wxNewId()
                ID.CHOICE_SUBADDR_MODE[i] = wx.wxNewId()
         
                ui.StaticText_device[i] = wx.wxStaticText(this, ID.STATICTEXT_DEVICE[i], "Device "..i..":  0x", wx.wxDefaultPosition, wx.wxDefaultSize)
                
                hexvalidator = wx.wxTextValidator(wx.wxFILTER_INCLUDE_CHAR_LIST)
                hexvalidator:SetIncludes({"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F", "a", "b", "c", "d", "e", "f"})
                
                ui.TextCtrl_address[i] = wx.wxTextCtrl(this, ID.TEXTCTRL_ADDRESS[i], "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, hexvalidator)
                ui.TextCtrl_address[i]:SetMinSize(wx.wxSize(50,-1))
                ui.TextCtrl_address[i]:SetMaxSize(wx.wxSize(50,-1))
                                
                ui.Choice_addr_mode[i] = wx.wxChoice(this, ID.CHOICE_ADDR_MODE[i], wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
                ui.Choice_addr_mode[i]:Append("7-bit address")
                ui.Choice_addr_mode[i]:Append("10-bit address")
                
                ui.Choice_subaddr_mode[i] = wx.wxChoice(this, ID.CHOICE_SUBADDR_MODE[i], wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
                ui.Choice_subaddr_mode[i]:Append("No sub-address")
                ui.Choice_subaddr_mode[i]:Append("1 byte sub-address")
                ui.Choice_subaddr_mode[i]:Append("2 bytes sub-address")
                ui.Choice_subaddr_mode[i]:Append("3 bytes sub-address")
                
                ui.FlexGridSizer4:Add(ui.StaticText_device[i], 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.FlexGridSizer4:Add(ui.TextCtrl_address[i], 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.FlexGridSizer4:Add(ui.Choice_addr_mode[i], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer4:Add(ui.Choice_subaddr_mode[i], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        end
        
        ui.FlexGridSizer5:Add(ui.FlexGridSizer4, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.FlexGridSizer_module:Add(ui.FlexGridSizer5, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.FlexGridSizer1:Add(ui.FlexGridSizer_module, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticLine1 = wx.wxStaticLine(this, ID.STATICLINE1, wx.wxDefaultPosition, wx.wxSize(10,-1), wx.wxLI_HORIZONTAL)
        ui.FlexGridSizer1:Add(ui.StaticLine1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Button_save = wx.wxButton(this, ID.BUTTON_SAVE, "Save", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer1:Add(ui.Button_save, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)

        --
        this:SetSizer(ui.FlexGridSizer1)
        this:SetScrollRate(50, 50)

        --
        this:Connect(ID.CHECKBOX_ENABLE, wx.wxEVT_COMMAND_CHECKBOX_CLICKED, checkbox_enable_updated)
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
function i2c:get_window_name()
        return "I2C"
end


--------------------------------------------------------------------------------
-- @brief  Function is called by parent when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function i2c:selected()
end


--------------------------------------------------------------------------------
-- @brief  Function returns modify status
-- @param  None
-- @return If data is modified true is returned, otherwise false
--------------------------------------------------------------------------------
function i2c:is_modified()
        return ui.Button_save:IsEnabled()
end


--------------------------------------------------------------------------------
-- @brief  Function returns module handler
-- @param  None
-- @return Module handler
--------------------------------------------------------------------------------
function get_handler()
        return i2c
end
