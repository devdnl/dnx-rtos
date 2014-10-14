--[[============================================================================
@file    network.lua

@author  Daniel Zorychta

@brief   Network configuration script.

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
network = {}


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
-- @brief  Function loads all controls from configuration files
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_controls()
        local module_enabled = ct:get_module_state("NETWORK")
        ui.CheckBox_enable:SetValue(module_enabled)
        ui.Panel1:Enable(module_enabled)

        for i = 0, 5 do
                ui.TextCtrl_MAC[i]:SetValue(ct:key_read(config.project.key["NETWORK_MAC_ADDR_"..i]):gsub("0x", ""))
        end

        ui.ComboBox_path:SetValue(ct:key_read(config.project.key.NETWORK_ETHIF_FILE):gsub('"', ''))
end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function save_configuration()
        ct:enable_module("NETWORK", ui.CheckBox_enable:GetValue())

        for i = 0, 5 do
                ct:key_write(config.project.key["NETWORK_MAC_ADDR_"..i], "0x"..ui.TextCtrl_MAC[i]:GetValue())
        end

        ct:key_write(config.project.key.NETWORK_ETHIF_FILE, '"'..ui.ComboBox_path:GetValue()..'"')

        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Event is called when module enable checkbox is changed
-- @param  this         event object
-- @return None
--------------------------------------------------------------------------------
local function checkbox_enable_clicked(this)
        ui.Panel1:Enable(this:IsChecked())
        modified:yes()
end


--------------------------------------------------------------------------------
-- @brief  Event is called when any value is updated (general update)
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function event_value_updated()
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
function network:create_window(parent)
        if ui.window == nil then
                ui.TextCtrl_MAC = {}

                ID.TEXTCTRL_MAC = {}
                ID.CHECKBOX_ENABLE = wx.wxNewId()
                ID.PANEL1 = wx.wxNewId()
                ID.COMBOBOX_PATH = wx.wxNewId()

                ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
                local this = ui.window

                ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)

                ui.CheckBox_enable = wx.wxCheckBox(this, ID.CHECKBOX_ENABLE, "Enable", wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, -1))
                ui.FlexGridSizer1:Add(ui.CheckBox_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.Panel1 = wx.wxPanel(this, ID.PANEL1, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
                ui.BoxSizer1 = wx.wxBoxSizer(wx.wxVERTICAL)
                ui.StaticBoxSizer_MAC = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel1, "MAC address")
                ui.FlexGridSizer_MAC = wx.wxFlexGridSizer(0, 11, 0, 0)

                for i = 0, 5 do
                        ID.TEXTCTRL_MAC[i] = wx.wxNewId()
                        ui.TextCtrl_MAC[i] = wx.wxTextCtrl(ui.Panel1, ID.TEXTCTRL_MAC[i], "FF", wx.wxDefaultPosition, wx.wxSize(40,-1), 0, ct.hexvalidator)
                        ui.TextCtrl_MAC[i]:SetMaxLength(2)
                        ui.FlexGridSizer_MAC:Add(ui.TextCtrl_MAC[i], 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                        if i < 5 then
                                local StaticText = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize)
                                ui.FlexGridSizer_MAC:Add(StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                        end

                        this:Connect(ID.TEXTCTRL_MAC[i], wx.wxEVT_COMMAND_TEXT_UPDATED, event_value_updated)
                end

                ui.StaticBoxSizer_MAC:Add(ui.FlexGridSizer_MAC, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.BoxSizer1:Add(ui.StaticBoxSizer_MAC, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticBoxSizer_path = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel1, "Ethernet device path")
                ui.ComboBox_path = wx.wxComboBox(ui.Panel1, ID.COMBOBOX_PATH, "", wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
                ui.ComboBox_path:Append("/dev/eth0")
                ui.StaticBoxSizer_path:Add(ui.ComboBox_path, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.BoxSizer1:Add(ui.StaticBoxSizer_path, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Panel1:SetSizer(ui.BoxSizer1)
                ui.FlexGridSizer1:Add(ui.Panel1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                this:SetSizer(ui.FlexGridSizer1)
                this:SetScrollRate(5, 5)

                --
                this:Connect(ID.CHECKBOX_ENABLE, wx.wxEVT_COMMAND_CHECKBOX_CLICKED,  checkbox_enable_clicked)
                this:Connect(ID.COMBOBOX_PATH,   wx.wxEVT_COMMAND_COMBOBOX_SELECTED, event_value_updated    )
                this:Connect(ID.COMBOBOX_PATH,   wx.wxEVT_COMMAND_TEXT_UPDATED,      event_value_updated    )
        end

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @param  None
-- @return Module name
--------------------------------------------------------------------------------
function network:get_window_name()
        return "Network"
end


--------------------------------------------------------------------------------
-- @brief  Function is called when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function network:refresh()
        load_controls()
        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Function check if options are modified
-- @param  None
-- @return true if options are modified, otherwise false
--------------------------------------------------------------------------------
function network:is_modified()
        return modified:get_value()
end


--------------------------------------------------------------------------------
-- @brief  Function save configuration
-- @return None
--------------------------------------------------------------------------------
function network:save()
        save_configuration()
end
