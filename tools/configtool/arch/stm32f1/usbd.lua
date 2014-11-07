--[[============================================================================
@file    usbd.lua

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
require("modules/ctcore")


--==============================================================================
-- GLOBAL OBJECTS
--==============================================================================
usbd = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local modified  = ct:new_modify_indicator()
local ui        = {}
local ID        = {}
local gpio      = require("arch/stm32f1/gpio").get_handler()
local pin_list  = nil
local prio_list = ct:get_priority_list("stm32f1")

local ep0_size_idx = {}
ep0_size_idx["8"]  = 0
ep0_size_idx["16"] = 1
ep0_size_idx["32"] = 2
ep0_size_idx["64"] = 3

--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function loads all controls from configuration scripts
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_configuration()
        -- load module enable controls
        local module_enable = ct:get_module_state("USBD")
        ui.CheckBox_module_enable:SetValue(module_enable)
        ui.Panel1:Enable(module_enable)

        -- load endpoint 0 size controls
        local ep0_size = ep0_size_idx[ct:key_read(config.arch.stm32f1.key.USBD_ENDPOINT0_SIZE)]
        if ep0_size == nil then ep0_size = 0 end
        ui.Choice_EP0_size:SetSelection(ep0_size)

        -- load D+ pullup pin name
        local pullup_pin = ct:get_string_index(pin_list, ct:key_read(config.arch.stm32f1.key.USBD_PULLUP_PIN))
        ui.Choice_pullup_pin:SetSelection(pullup_pin - 1)

        -- load interrupt priority
        local irq_prio = ct:key_read(config.arch.stm32f1.key.USBD_IRQ_PRIORITY)
        if irq_prio == config.project.def.DEFAULT_IRQ_PRIORITY:GetValue() then
                irq_prio = #prio_list
        else
                irq_prio = math.floor(tonumber(irq_prio) / 16)
        end
        ui.Choice_irq_prio:SetSelection(irq_prio)
end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function save_configuration()
        -- save pullup pin name
        local pullup_pin = ui.Choice_pullup_pin:GetSelection() + 1
        if pullup_pin == 0 then
                pullup_pin = 1
        end
        ct:key_write(config.arch.stm32f1.key.USBD_PULLUP_PIN, pin_list[pullup_pin])
        
        -- save module enable settings
        ct:enable_module("USBD", ui.CheckBox_module_enable:GetValue())

        -- save endpoint 0 size
        local ep0_size = ui.Choice_EP0_size:GetSelection()
        for s, i in pairs(ep0_size_idx) do
                if i == ep0_size then
                        ep0_size = s
                        break
                end
        end
        if type(ep0_size) == "number" then ep0_size = "8" end
        ct:key_write(config.arch.stm32f1.key.USBD_ENDPOINT0_SIZE, ep0_size)

        -- save interrupt priority
        local irq_prio = ui.Choice_irq_prio:GetSelection() + 1
        if irq_prio > #prio_list then
                irq_prio = config.project.def.DEFAULT_IRQ_PRIORITY:GetValue()
        else
                irq_prio = prio_list[irq_prio].value
        end
        ct:key_write(config.arch.stm32f1.key.USBD_IRQ_PRIORITY, irq_prio)

        --
        modified:no()
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
function usbd:create_window(parent)
        pin_list = gpio:get_pin_list(true)

        ui = {}
        ID = {}
        ID.CHECKBOX_MODULE_ENABLE = wx.wxNewId()
        ID.CHOICE_EP0_SIZE = wx.wxNewId()
        ID.CHOICE_PULLUP_PIN = wx.wxNewId()
        ID.CHOICE_IRQ_PRIO = wx.wxNewId()
        ID.PANEL1 = wx.wxNewId()

        ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
        local this = ui.window

        ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.CheckBox_module_enable = wx.wxCheckBox(this, ID.CHECKBOX_MODULE_ENABLE, "Enable module", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer1:Add(ui.CheckBox_module_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Panel1 = wx.wxPanel(this, ID.PANEL1, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer2 = wx.wxFlexGridSizer(0, 2, 0, 0)
        ui.StaticText1 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Endpoint 0 size", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer2:Add(ui.StaticText1, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_EP0_size = wx.wxChoice(ui.Panel1, ID.CHOICE_EP0_SIZE, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
        ui.Choice_EP0_size:Append("8 bytes")
        ui.Choice_EP0_size:Append("16 bytes")
        ui.Choice_EP0_size:Append("32 bytes")
        ui.Choice_EP0_size:Append("64 bytes")
        ui.FlexGridSizer2:Add(ui.Choice_EP0_size, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText2 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "D+ pullup pin", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer2:Add(ui.StaticText2, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_pullup_pin = wx.wxChoice(ui.Panel1, ID.CHOICE_PULLUP_PIN, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
        ui.Choice_pullup_pin:Append(pin_list)
        ui.FlexGridSizer2:Add(ui.Choice_pullup_pin, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText3 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Interrupt priority", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer2:Add(ui.StaticText3, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_irq_prio = wx.wxChoice(ui.Panel1, ID.CHOICE_IRQ_PRIO, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
        for i, item in ipairs(prio_list) do ui.Choice_irq_prio:Append(item.name) end
        ui.Choice_irq_prio:Append("System default")
        ui.FlexGridSizer2:Add(ui.Choice_irq_prio, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Panel1:SetSizer(ui.FlexGridSizer2)
        ui.FlexGridSizer1:Add(ui.Panel1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        --
        this:SetSizer(ui.FlexGridSizer1)
        this:SetScrollRate(50, 50)

        --
        this:Connect(ID.CHECKBOX_MODULE_ENABLE, wx.wxEVT_COMMAND_CHECKBOX_CLICKED, event_checkbox_module_enable_updated)
        this:Connect(ID.CHOICE_EP0_SIZE,        wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_IRQ_PRIO,        wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_PULLUP_PIN,      wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )

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
function usbd:get_window_name()
        return "USB"
end


--------------------------------------------------------------------------------
-- @brief  Function is called by parent when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function usbd:selected()
        local new_pin_list = gpio:get_pin_list(true)

        local equal = true
        for i = 1, #new_pin_list do
                if pin_list[i] ~= new_pin_list[i] then
                        equal = false
                        break
                end
        end

        if not equal then
                pin_list = new_pin_list
                ui.Choice_pullup_pin:Clear()
                ui.Choice_pullup_pin:Append(pin_list)
                load_configuration()
        end
end


--------------------------------------------------------------------------------
-- @brief  Function returns modify status
-- @param  None
-- @return If data is modified true is returned, otherwise false
--------------------------------------------------------------------------------
function usbd:is_modified()
        return modified:get_value()
end


--------------------------------------------------------------------------------
-- @brief  Function save configuration
-- @return None
--------------------------------------------------------------------------------
function usbd:save()
        save_configuration()
end


--------------------------------------------------------------------------------
-- @brief  Function discard modified configuration
-- @return None
--------------------------------------------------------------------------------
function usbd:discard()
        load_configuration()
        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Function returns module handler
-- @param  None
-- @return Module handler
--------------------------------------------------------------------------------
function get_handler()
        return usbd
end
