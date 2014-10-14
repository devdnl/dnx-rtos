--[[============================================================================
@file    irq.lua

@author  Daniel Zorychta

@brief   Configuration script for EXTI module.

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
irq = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local modified         = ct:new_modify_indicator()
local ui               = {}
local ID               = {}
local NUMBER_OF_IRQ    = 16
local prio_list        = ct:get_priority_list("stm32f1")

-- table with IRQ modes and default value
local irq_mode = setmetatable({}, {__index = function() return 0 end})
irq_mode._IRQ_MODE_DISABLED                = 0
irq_mode._IRQ_MODE_FALLING_EDGE            = 1
irq_mode._IRQ_MODE_RISING_EDGE             = 2
irq_mode._IRQ_MODE_FALLING_AND_RISING_EDGE = 3


--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function loads all controls from configuration scripts
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_configuration()
        -- load IRQ mode and priorty
        for i = 0, NUMBER_OF_IRQ - 1 do
                local mode = irq_mode[ct:key_read(config.arch.stm32f1.key["IRQ_"..i.."_MODE"])]
                mode = ifs(mode < 0 or mode > 3, 0, mode)

                local prio = ct:key_read(config.arch.stm32f1.key["IRQ_"..i.."_PRIO"])
                if prio == config.project.def.DEFAULT_IRQ_PRIORITY:GetValue() then
                        prio = #prio_list
                else
                        prio = math.floor(tonumber(prio) / 16)
                end

                ui.Choice_EXTI_mode[i]:SetSelection(mode)
                ui.Choice_EXTI_prio[i]:SetSelection(prio)
        end

        -- load module state
        local module_enable = ct:get_module_state("IRQ")
        ui.CheckBox_module_enable:SetValue(module_enable)
        ui.Panel1:Enable(module_enable)
end


--------------------------------------------------------------------------------
-- @brief  Event is called when device enable checkbox is changed
-- @param  this     event object
-- @return None
--------------------------------------------------------------------------------
local function event_checkbox_module_enable_updated(this)
        ui.Panel1:Enable(this:IsChecked())
        modified:yes()
end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function save_configuration()
        -- save module state
        ct:enable_module("IRQ", ui.CheckBox_module_enable:GetValue())

        -- save IRQ mode and priorty
        for i = 0, NUMBER_OF_IRQ - 1 do
                local mode = ui.Choice_EXTI_mode[i]:GetSelection()
                for k, i in pairs(irq_mode) do
                        if i == mode then
                                mode = k
                                break
                        end
                end
                ct:key_write(config.arch.stm32f1.key["IRQ_"..i.."_MODE"], mode)

                local prio = ui.Choice_EXTI_prio[i]:GetSelection() + 1
                if prio > #prio_list then
                        prio = config.project.def.DEFAULT_IRQ_PRIORITY:GetValue()
                else
                        prio = prio_list[prio].value
                end
                ct:key_write(config.arch.stm32f1.key["IRQ_"..i.."_PRIO"], prio)
        end

        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Event is called when value is changed (general)
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function event_value_updated()
        modified:yes()
end


--------------------------------------------------------------------------------
-- @brief  Event is called when value is changed (IRQ5-9)
-- @param  this     Choice of priority
-- @return None
--------------------------------------------------------------------------------
local function event_value_updated_IRQ9_5(this)
        local selection = this:GetSelection()

        for i = 5, 9 do
                ui.Choice_EXTI_prio[i]:SetSelection(selection)
        end

        modified:yes()
end


--------------------------------------------------------------------------------
-- @brief  Event is called when value is changed (IRQ10-15)
-- @param  this     Choice of priority
-- @return None
--------------------------------------------------------------------------------
local function event_value_updated_IRQ10_15(this)
        local selection = this:GetSelection()

        for i = 10, 15 do
                ui.Choice_EXTI_prio[i]:SetSelection(selection)
        end

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
function irq:create_window(parent)
        ui = {}
        ui.Choice_EXTI_mode = {}
        ui.Choice_EXTI_prio = {}

        ID = {}
        ID.CHOICE_EXTI_MODE = {}
        ID.CHOICE_EXTI_PRIO = {}
        ID.PANEL1 = wx.wxNewId()
        ID.CHECKBOX_MODULE_ENABLE = wx.wxNewId()

        ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
        local this = ui.window

        ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.CheckBox_module_enable = wx.wxCheckBox(this, ID.CHECKBOX_MODULE_ENABLE, "Enable module", wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, -1))
        ui.FlexGridSizer1:Add(ui.CheckBox_module_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        ui.Panel1 = wx.wxPanel(this, ID.PANEL1, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer2 = wx.wxFlexGridSizer(0, 3, 0, 0)

        for i = 0, NUMBER_OF_IRQ - 1 do
                ui.StaticText = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "EXTI"..i, wx.wxDefaultPosition, wx.wxDefaultSize)

                -- add mode Choice
                ID.CHOICE_EXTI_MODE[i] = wx.wxNewId()
                ui.Choice_EXTI_mode[i] = wx.wxChoice(ui.Panel1, ID.CHOICE_EXTI_MODE[i], wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
                ui.Choice_EXTI_mode[i]:Append("Disabled")
                ui.Choice_EXTI_mode[i]:Append("Trigger on falling edge")
                ui.Choice_EXTI_mode[i]:Append("Trigger on rising edge")
                ui.Choice_EXTI_mode[i]:Append("Trigger on both edges")

                -- add priority Choice
                ID.CHOICE_EXTI_PRIO[i] = wx.wxNewId()
                ui.Choice_EXTI_prio[i] = wx.wxChoice(ui.Panel1, ID.CHOICE_EXTI_PRIO[i], wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
                for _, item in ipairs(prio_list) do
                        ui.Choice_EXTI_prio[i]:Append(item.name)
                end

                ui.Choice_EXTI_prio[i]:Append("System default priority")

                if i >= 10 then
                        ui.Choice_EXTI_prio[i]:SetToolTip("This interrupt is handled by vector that is assigned for interrupts from 10 to 15. All interrupts in this group have the same priority.")
                elseif i >= 5 then
                        ui.Choice_EXTI_prio[i]:SetToolTip("This interrupt is handled by vector that is assigned for interrupts from 5 to 9. All interrupts in this group have the same priority.")
                end

                -- add object to layout
                ui.FlexGridSizer2:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer2:Add(ui.Choice_EXTI_mode[i], 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer2:Add(ui.Choice_EXTI_prio[i], 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- set event connections
                local event_function
                if i >= 10 then -- IRQ15..10 have the same IRQ vector - priority set as group
                        event_function = event_value_updated_IRQ10_15
                elseif i >= 5 then -- IRQ15..10 have the same IRQ vector - priority set as group
                        event_function = event_value_updated_IRQ9_5
                else
                        event_function = event_value_updated
                end

                this:Connect(ID.CHOICE_EXTI_PRIO[i], wx.wxEVT_COMMAND_CHOICE_SELECTED, event_function)
                this:Connect(ID.CHOICE_EXTI_MODE[i], wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
        end

        ui.Panel1:SetSizer(ui.FlexGridSizer2)
        ui.FlexGridSizer1:Add(ui.Panel1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

        --
        this:SetSizer(ui.FlexGridSizer1)
        this:SetScrollRate(50, 50)

        --
        this:Connect(ID.CHECKBOX_MODULE_ENABLE, wx.wxEVT_COMMAND_CHECKBOX_CLICKED, event_checkbox_module_enable_updated)

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
function irq:get_window_name()
        return "IRQ"
end


--------------------------------------------------------------------------------
-- @brief  Function is called by parent when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function irq:selected()
end


--------------------------------------------------------------------------------
-- @brief  Function returns modify status
-- @param  None
-- @return If data is modified true is returned, otherwise false
--------------------------------------------------------------------------------
function irq:is_modified()
        return modified:get_value()
end


--------------------------------------------------------------------------------
-- @brief  Function save configuration
-- @return None
--------------------------------------------------------------------------------
function irq:save()
        save_configuration()
end


--------------------------------------------------------------------------------
-- @brief  Function discard modified configuration
-- @return None
--------------------------------------------------------------------------------
function irq:discard()
        load_configuration()
        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Function returns module handler
-- @param  None
-- @return Module handler
--------------------------------------------------------------------------------
function get_handler()
        return irq
end
