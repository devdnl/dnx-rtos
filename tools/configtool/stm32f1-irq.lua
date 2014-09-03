--[[============================================================================
@file    stm32f1-irq.lua

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
require("ctcore")


--==============================================================================
-- GLOBAL OBJECTS
--==============================================================================
-- module's main object
irq = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
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
local function load_controls()
        -- load module state
        local module_enable = ct:get_module_state("IRQ")
        ui.CheckBox_module_enable:SetValue(module_enable)
        ui.Panel1:Enable(module_enable)

        -- load IRQ mode and priorty
        for i = 0, NUMBER_OF_IRQ - 1 do
                local key = config.arch.stm32f1.key.IRQ_GENERAL

                key.key:SetValue("__IRQ_LINE_"..i.."_MODE__")
                local mode = irq_mode[ct:key_read(key)]
                if mode < 0 or mode > 3 then mode = 0 end

                key.key:SetValue("__IRQ_LINE_"..i.."_PRIO__")
                local prio = ct:key_read(key)
                if prio == config.project.def.DEFAULT_IRQ_PRIORITY:GetValue() then
                        prio = #prio_list
                else
                        prio = math.floor(tonumber(prio) / 16)
                end

                ui.Choice_EXTI_mode[i]:SetSelection(mode)
                ui.Choice_EXTI_prio[i]:SetSelection(prio)
        end
end


--------------------------------------------------------------------------------
-- @brief  Event is called when device enable checkbox is changed
-- @param  this     event object
-- @return None
--------------------------------------------------------------------------------
local function event_checkbox_module_enable_updated(this)
        ui.Panel1:Enable(this:IsChecked())
        ui.Button_save:Enable(true)
end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function event_on_button_save_click()
        -- save module state
        ct:enable_module("IRQ", ui.CheckBox_module_enable:GetValue())

        -- save IRQ mode and priorty
        for i = 0, NUMBER_OF_IRQ - 1 do
                local key = config.arch.stm32f1.key.IRQ_GENERAL

                key.key:SetValue("__IRQ_LINE_"..i.."_MODE__")
                local mode = ui.Choice_EXTI_mode[i]:GetSelection()
                for k, i in pairs(irq_mode) do
                        if i == mode then
                                mode = k
                                break
                        end
                end
                ct:key_write(key, mode)

                key.key:SetValue("__IRQ_LINE_"..i.."_PRIO__")
                local prio = ui.Choice_EXTI_prio[i]:GetSelection() + 1
                if prio > #prio_list then
                        prio = config.project.def.DEFAULT_IRQ_PRIORITY:GetValue()
                else
                        prio = prio_list[prio].value
                end
                ct:key_write(key, prio)
        end

        ui.Button_save:Enable(false)
end


--------------------------------------------------------------------------------
-- @brief  Event is called when value is changed (general)
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function event_value_updated()
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
function irq:create_window(parent)
        ui = {}
        ui.Choice_EXTI_mode = {}
        ui.Choice_EXTI_prio = {}

        ID = {}
        ID.CHOICE_EXTI_MODE = {}
        ID.CHOICE_EXTI_PRIO = {}
        ID.PANEL1 = wx.wxNewId()
        ID.CHECKBOX_MODULE_ENABLE = wx.wxNewId()
        ID.STATICLINE1 = wx.wxNewId()
        ID.BUTTON_SAVE = wx.wxNewId()

        ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
        local this = ui.window

        ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.CheckBox_module_enable = wx.wxCheckBox(this, ID.CHECKBOX_MODULE_ENABLE, "Enable module", wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, -1))
        ui.FlexGridSizer1:Add(ui.CheckBox_module_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        ui.Panel1 = wx.wxPanel(this, ID.PANEL1, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer2 = wx.wxFlexGridSizer(0, 3, 0, 0)

        for i = 0, NUMBER_OF_IRQ - 1 do
                ui.StaticText = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "EXTI"..i, wx.wxDefaultPosition, wx.wxDefaultSize)

                ID.CHOICE_EXTI_MODE[i] = wx.wxNewId()
                ui.Choice_EXTI_mode[i] = wx.wxChoice(ui.Panel1, ID.CHOICE_EXTI_MODE[i], wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
                ui.Choice_EXTI_mode[i]:Append("Disabled")
                ui.Choice_EXTI_mode[i]:Append("Trigger on falling edge")
                ui.Choice_EXTI_mode[i]:Append("Trigger on rising edge")
                ui.Choice_EXTI_mode[i]:Append("Trigger on both edges")

                ID.CHOICE_EXTI_PRIO[i] = wx.wxNewId()
                ui.Choice_EXTI_prio[i] = wx.wxChoice(ui.Panel1, ID.CHOICE_EXTI_PRIO[i], wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
                for _, item in ipairs(prio_list) do
                        ui.Choice_EXTI_prio[i]:Append(item.name)
                end
                ui.Choice_EXTI_prio[i]:Append("System default priority")

                ui.FlexGridSizer2:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer2:Add(ui.Choice_EXTI_mode[i], 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer2:Add(ui.Choice_EXTI_prio[i], 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                this:Connect(ID.CHOICE_EXTI_MODE[i], wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
                this:Connect(ID.CHOICE_EXTI_PRIO[i], wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
        end

        ui.Panel1:SetSizer(ui.FlexGridSizer2)
        ui.FlexGridSizer1:Add(ui.Panel1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.StaticLine1 = wx.wxStaticLine(this, ID.STATICLINE1, wx.wxDefaultPosition, wx.wxSize(10,-1), wx.wxLI_HORIZONTAL)
        ui.FlexGridSizer1:Add(ui.StaticLine1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Button_save = wx.wxButton(this, ID.BUTTON_SAVE, "Save", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer1:Add(ui.Button_save, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)

        --
        this:SetSizer(ui.FlexGridSizer1)
        this:SetScrollRate(50, 50)

        --
        this:Connect(ID.CHECKBOX_MODULE_ENABLE, wx.wxEVT_COMMAND_CHECKBOX_CLICKED, event_checkbox_module_enable_updated)
        this:Connect(ID.BUTTON_SAVE,            wx.wxEVT_COMMAND_BUTTON_CLICKED,   event_on_button_save_click          )

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
        return ui.Button_save:IsEnabled()
end


--------------------------------------------------------------------------------
-- @brief  Function returns module handler
-- @param  None
-- @return Module handler
--------------------------------------------------------------------------------
function get_handler()
        return irq
end
