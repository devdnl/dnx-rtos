--[[============================================================================
@file    eth.lua

@author  Daniel Zorychta

@brief   Configuration script for ETH module.

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
eth = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local modified   = ct:new_modify_indicator()
local ui         = {}
local ID         = {}
local prio_list  = ct:get_priority_list("stm32f1")
local PHY        = config.PHY.ethernet:Children()

local link_speed_idx = {}
link_speed_idx.ETH_Speed_10M  = 0
link_speed_idx.ETH_Speed_100M = 1

--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function loads all controls from configuration scripts
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_configuration()
        -- load configuration from files
        local module_enable = ct:get_module_state("ETHMAC")
        local hw_chsum_cal  = ct:yes_no_to_bool(ct:key_read(config.arch.stm32f1.key.ETHMAC_CHECKSUM_BY_HARDWARE))
        local irq_prio      = ct:key_read(config.arch.stm32f1.key.ETHMAC_IRQ_PRIORITY)
        local link_speed    = link_speed_idx[ct:key_read(config.arch.stm32f1.key.ETHMAC_SPEED)]
        local PHY_name      = ct:key_read(config.arch.stm32f1.key.ETHMAC_DEVICE)
        local PHY_address   = ct:key_read(config.arch.stm32f1.key.ETHMAC_PHY_ADDRESS):gsub("0x", "")
        local reset_delay   = tonumber(ct:key_read(config.arch.stm32f1.key.ETHMAC_PHY_RESET_DELAY))
        local setup_delay   = tonumber(ct:key_read(config.arch.stm32f1.key.ETHMAC_PHY_CONFIG_DELAY))

        -- convert IRQ selection to list index
        if irq_prio == config.project.def.DEFAULT_IRQ_PRIORITY:GetValue() then
                irq_prio = #prio_list
        else
                irq_prio = math.floor(tonumber(irq_prio) / 16)
        end

        -- find PHY name in the list
        for i = 0, ui.Choice_device:GetCount() do
                if ui.Choice_device:GetString(i) == PHY_name then
                        PHY_name = i
                        break
                end
        end
        if type(PHY_name) == "string" then PHY_name = -1 end

        -- check link speed
        if link_speed == nil then
                link_speed = -1
        end

        -- load controls
        ui.CheckBox_module_enable:SetValue(module_enable)
        ui.Panel1:Enable(module_enable)
        ui.CheckBox_hw_chsum_cal:SetValue(hw_chsum_cal)
        ui.Choice_IRQ_prio:SetSelection(irq_prio)
        ui.Choice_link_speed:SetSelection(link_speed)
        ui.Choice_device:SetSelection(PHY_name)
        ui.TextCtrl_PHY_addr:SetValue(PHY_address)
        ui.SpinCtrl_reset_delay:SetValue(reset_delay)
        ui.SpinCtrl_setup_delay:SetValue(setup_delay)
end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return On success true, otherwise false
--------------------------------------------------------------------------------
local function save_configuration()
        -- load configuration from controls
        local module_enable = ui.CheckBox_module_enable:GetValue()
        local hw_chsum_cal  = ct:bool_to_yes_no(ui.CheckBox_hw_chsum_cal:GetValue())
        local irq_prio      = ui.Choice_IRQ_prio:GetSelection() + 1
        local link_speed    = ui.Choice_link_speed:GetSelection()
        local PHY_index     = ui.Choice_device:GetSelection() + 1
        local PHY_address   = "0x"..ui.TextCtrl_PHY_addr:GetValue():upper()
        local reset_delay   = tostring(ui.SpinCtrl_reset_delay:GetValue())
        local setup_delay   = tostring(ui.SpinCtrl_setup_delay:GetValue())

        -- convert irq priority selection to configuration
        if irq_prio > #prio_list then
                irq_prio = config.project.def.DEFAULT_IRQ_PRIORITY:GetValue()
        else
                irq_prio = prio_list[irq_prio].value
        end

        -- convert link speed selection to string
        for k, i in pairs(link_speed_idx) do
                if i == link_speed then
                        link_speed = k
                        break
                end
        end

        -- write setup to configuration files
        ct:enable_module("ETHMAC", module_enable)
        ct:key_write(config.arch.stm32f1.key.ETHMAC_CHECKSUM_BY_HARDWARE, hw_chsum_cal)
        ct:key_write(config.arch.stm32f1.key.ETHMAC_IRQ_PRIORITY, irq_prio)
        ct:key_write(config.arch.stm32f1.key.ETHMAC_SPEED, link_speed)
        ct:key_write(config.arch.stm32f1.key.ETHMAC_PHY_ADDRESS, PHY_address)
        ct:key_write(config.arch.stm32f1.key.ETHMAC_PHY_RESET_DELAY, reset_delay)
        ct:key_write(config.arch.stm32f1.key.ETHMAC_PHY_CONFIG_DELAY, setup_delay)
        ct:key_write(config.arch.stm32f1.key.ETHMAC_DEVICE, PHY[PHY_index].name:GetValue())
        ct:key_write(config.arch.stm32f1.key.ETHMAC_PHY_SR, PHY[PHY_index].status_register_address:GetValue())
        ct:key_write(config.arch.stm32f1.key.ETHMAC_PHY_SPEED_STATUS_BM, PHY[PHY_index].speed_status_mask:GetValue())
        ct:key_write(config.arch.stm32f1.key.ETHMAC_PHY_DUPLEX_STATUS_BM, PHY[PHY_index].duplex_status_mask:GetValue())

        modified:no()

        return true
end


--------------------------------------------------------------------------------
-- @brief  Event is called when module enable checkbox is changed
-- @param  this         event object
-- @return None
--------------------------------------------------------------------------------
local function event_checkbox_module_enable_updated(this)
        ui.Panel1:Enable(this:IsChecked())
        modified:yes()
end


--------------------------------------------------------------------------------
-- @brief  Event is called when value is changed (general)
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
function eth:create_window(parent)
        ui = {}

        ID = {}
        ID.CHECKBOX_MODULE_ENABLE = wx.wxNewId()
        ID.CHECKBOX_HW_CHSUM_CAL = wx.wxNewId()
        ID.CHOICE_IRQ_PRIO = wx.wxNewId()
        ID.CHOICE_LINK_SPEED = wx.wxNewId()
        ID.CHOICE_DEVICE = wx.wxNewId()
        ID.TEXTCTRL_PHY_ADDR = wx.wxNewId()
        ID.SPINCTRL_RESET_DELAY = wx.wxNewId()
        ID.SPINCTRL_SETUP_DELAY = wx.wxNewId()
        ID.PANEL1 = wx.wxNewId()

        ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
        local this = ui.window

        ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.CheckBox_module_enable = wx.wxCheckBox(this, ID.CHECKBOX_MODULE_ENABLE, "Enable module", wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, -1), 0, wx.wxDefaultValidator, "ID.CHECKBOX_MODULE_ENABLE")
        ui.FlexGridSizer1:Add(ui.CheckBox_module_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Panel1 = wx.wxPanel(this, ID.PANEL1, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL, "ID.PANEL1")
        ui.FlexGridSizer2 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.StaticBoxSizer1 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel1, "General")
        ui.FlexGridSizer3 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.CheckBox_hw_chsum_cal = wx.wxCheckBox(ui.Panel1, ID.CHECKBOX_HW_CHSUM_CAL, "Hardware checksum calculation", wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, -1), 0, wx.wxDefaultValidator, "ID.CHECKBOX_HW_CHSUM_CAL")
        ui.FlexGridSizer3:Add(ui.CheckBox_hw_chsum_cal, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.FlexGridSizer4 = wx.wxFlexGridSizer(0, 2, 0, 0)
        ui.StaticText1 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "IRQ priority", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer4:Add(ui.StaticText1, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_IRQ_prio = wx.wxChoice(ui.Panel1, ID.CHOICE_IRQ_PRIO, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_IRQ_PRIO")
        for i, item in ipairs(prio_list) do ui.Choice_IRQ_prio:Append(item.name) end
        ui.Choice_IRQ_prio:Append("System default")
        ui.FlexGridSizer4:Add(ui.Choice_IRQ_prio, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText2 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Link speed", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer4:Add(ui.StaticText2, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_link_speed = wx.wxChoice(ui.Panel1, ID.CHOICE_LINK_SPEED, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_LINK_SPEED")
        ui.Choice_link_speed:Append("10 Mbps")
        ui.Choice_link_speed:Append("100 Mbps")
        ui.FlexGridSizer4:Add(ui.Choice_link_speed, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.FlexGridSizer3:Add(ui.FlexGridSizer4, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.StaticBoxSizer1:Add(ui.FlexGridSizer3, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.FlexGridSizer2:Add(ui.StaticBoxSizer1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticBoxSizer2 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel1, "PHY")
        ui.FlexGridSizer5 = wx.wxFlexGridSizer(0, 2, 0, 0)
        ui.StaticText6 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Device", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer5:Add(ui.StaticText6, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_device = wx.wxChoice(ui.Panel1, ID.CHOICE_DEVICE, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_DEVICE")
        for i = 1, #PHY do ui.Choice_device:Append(PHY[i].name:GetValue()) end
        ui.FlexGridSizer5:Add(ui.Choice_device, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText3 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Address [hex]", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer5:Add(ui.StaticText3, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.TextCtrl_PHY_addr = wx.wxTextCtrl(ui.Panel1, ID.TEXTCTRL_PHY_ADDR, "", wx.wxDefaultPosition, wx.wxSize(30,-1), 0, ct.hexvalidator)
        ui.TextCtrl_PHY_addr:SetMaxLength(2)
        ui.FlexGridSizer5:Add(ui.TextCtrl_PHY_addr, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText4 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Reset delay [ms]", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer5:Add(ui.StaticText4, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.SpinCtrl_reset_delay = wx.wxSpinCtrl(ui.Panel1, ID.SPINCTRL_RESET_DELAY, "0", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 100, 500, 0, "ID.SPINCTRL_RESET_DELAY")
        ui.FlexGridSizer5:Add(ui.SpinCtrl_reset_delay, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText5 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Setup delay [ms]", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer5:Add(ui.StaticText5, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.SpinCtrl_setup_delay = wx.wxSpinCtrl(ui.Panel1, ID.SPINCTRL_SETUP_DELAY, "0", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 100, 500, 0, "ID.SPINCTRL_SETUP_DELAY")
        ui.FlexGridSizer5:Add(ui.SpinCtrl_setup_delay, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticBoxSizer2:Add(ui.FlexGridSizer5, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.FlexGridSizer2:Add(ui.StaticBoxSizer2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Panel1:SetSizer(ui.FlexGridSizer2)
        ui.FlexGridSizer1:Add(ui.Panel1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        --
        this:SetSizer(ui.FlexGridSizer1)
        this:SetScrollRate(50, 50)

        --
        this:Connect(ID.CHECKBOX_MODULE_ENABLE, wx.wxEVT_COMMAND_CHECKBOX_CLICKED, event_checkbox_module_enable_updated)
        this:Connect(ID.CHECKBOX_HW_CHSUM_CAL,  wx.wxEVT_COMMAND_CHECKBOX_CLICKED, event_value_updated                 )
        this:Connect(ID.CHOICE_IRQ_PRIO,        wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_LINK_SPEED,      wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_DEVICE,          wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.TEXTCTRL_PHY_ADDR,      wx.wxEVT_COMMAND_TEXT_UPDATED,     event_value_updated                 )
        this:Connect(ID.SPINCTRL_RESET_DELAY,   wx.wxEVT_COMMAND_SPINCTRL_UPDATED, event_value_updated                 )
        this:Connect(ID.SPINCTRL_RESET_DELAY,   wx.wxEVT_COMMAND_TEXT_UPDATED,     event_value_updated                 )
        this:Connect(ID.SPINCTRL_SETUP_DELAY,   wx.wxEVT_COMMAND_SPINCTRL_UPDATED, event_value_updated                 )
        this:Connect(ID.SPINCTRL_SETUP_DELAY,   wx.wxEVT_COMMAND_TEXT_UPDATED,     event_value_updated                 )

        --
        load_configuration()
        modified:no()

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @param  None
-- @return Module name
--------------------------------------------------------------------------------
function eth:get_window_name()
        return "ETHMAC"
end


--------------------------------------------------------------------------------
-- @brief  Function is called by parent when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function eth:selected()
end


--------------------------------------------------------------------------------
-- @brief  Function returns modify status
-- @param  None
-- @return If data is modified true is returned, otherwise false
--------------------------------------------------------------------------------
function eth:is_modified()
        return modified:get_value()
end


--------------------------------------------------------------------------------
-- @brief  Function save configuration
-- @return None
--------------------------------------------------------------------------------
function eth:save()
        save_configuration()
end


--------------------------------------------------------------------------------
-- @brief  Function discard modified configuration
-- @return None
--------------------------------------------------------------------------------
function eth:discard()
        load_configuration()
        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Function returns module handler
-- @param  None
-- @return Module handler
--------------------------------------------------------------------------------
function get_handler()
        return eth
end
