--[[============================================================================
@file    sdspi.lua

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
require("modules/ctcore")


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
local NUMBER_OF_CARDS = 2

--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function loads all controls from configuration scripts
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_controls()
        local module_enable   = ct:get_module_state("SDSPI")
        local number_of_cards = tonumber(ct:key_read(config.arch.stm32f1.key.SDSPI_NUMBER_OF_CARDS))

        ui.CheckBox_module_enable:SetValue(module_enable)
        ui.Choice_card_count:SetSelection(number_of_cards - 1)

        for i = 0, NUMBER_OF_CARDS - 1 do
                local SPI_file     = ct:key_read(config.arch.stm32f1.key["SDSPI_CARD"..i.."_FILE"]):gsub("\"", "")
                local card_timeout = tonumber(ct:key_read(config.arch.stm32f1.key["SDSPI_CARD"..i.."_TIMEOUT"]))

                ui.ComboBox_card[i]:SetValue(SPI_file)
                ui.SpinCtrl_card[i]:SetValue(card_timeout)
                ui.Panel_card[i]:Enable(i <= number_of_cards - 1)
        end

        ui.Panel1:Enable(module_enable)
end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return On success true, otherwise false
--------------------------------------------------------------------------------
local function event_on_button_save_click()
        ct:enable_module("SDSPI", ui.CheckBox_module_enable:GetValue())
        ct:key_write(config.arch.stm32f1.key.SDSPI_NUMBER_OF_CARDS, tostring(ui.Choice_card_count:GetSelection() + 1))

        for i = 0, NUMBER_OF_CARDS - 1 do
                ct:key_write(config.arch.stm32f1.key["SDSPI_CARD"..i.."_FILE"], "\""..ui.ComboBox_card[i]:GetValue().."\"")
                ct:key_write(config.arch.stm32f1.key["SDSPI_CARD"..i.."_TIMEOUT"], tostring(ui.SpinCtrl_card[i]:GetValue()))
        end

        --
        ui.Button_save:Enable(false)

        return true
end


--------------------------------------------------------------------------------
-- @brief  Event is called when module enable checkbox is changed
-- @param  this         event object
-- @return None
--------------------------------------------------------------------------------
local function event_checkbox_module_enable_updated(this)
        ui.Panel1:Enable(this:IsChecked())
        ui.Button_save:Enable(true)
end


--------------------------------------------------------------------------------
-- @brief  Event is called when value is changed (general)
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function event_value_updated()
        ui.Button_save:Enable(true)
end


--------------------------------------------------------------------------------
-- @brief  Event is called when value is changed (general)
-- @param  event    event object
-- @return None
--------------------------------------------------------------------------------
local function event_number_of_cards_changed(event)
        local card_count = event:GetSelection()

        for i = 0, NUMBER_OF_CARDS - 1 do
                ui.Panel_card[i]:Enable(i <= card_count)
        end

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
        ui.Panel_card    = {}
        ui.ComboBox_card = {}
        ui.SpinCtrl_card = {}

        ID = {}
        ID.CHECKBOX_MODULE_ENABLE = wx.wxNewId()
        ID.PANEL1 = wx.wxNewId()
        ID.CHOICE_CARD_COUNT = wx.wxNewId()
        ID.COMBOBOX_CARD = {}
        ID.SPINCTRL_CARD = {}
        ID.BUTTON_SAVE = wx.wxNewId()

        ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
        local this = ui.window

        ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.CheckBox_module_enable = wx.wxCheckBox(this, ID.CHECKBOX_MODULE_ENABLE, "Enable module", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer1:Add(ui.CheckBox_module_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        ui.Panel1 = wx.wxPanel(this, ID.PANEL1, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizerPanel1 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.FlexGridSizer2 = wx.wxFlexGridSizer(0, 2, 0, 0)

        ui.StaticText1 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Number of cards", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer2:Add(ui.StaticText1, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_card_count = wx.wxChoice(ui.Panel1, ID.CHOICE_CARD_COUNT, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
        ui.Choice_card_count:Append("1 card")
        ui.Choice_card_count:Append("2 cards")
        ui.FlexGridSizer2:Add(ui.Choice_card_count, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        ui.FlexGridSizerPanel1:Add(ui.FlexGridSizer2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        for i = 0, NUMBER_OF_CARDS - 1 do
                ID.COMBOBOX_CARD[i] = wx.wxNewId()
                ID.SPINCTRL_CARD[i] = wx.wxNewId()

                ui.Panel_card[i] = wx.wxPanel(ui.Panel1, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
                ui.StaticBoxSizer_card = wx.wxStaticBoxSizer(wx.wxVERTICAL, ui.Panel_card[i], "Card "..i)
                ui.FlexGridSizer_card = wx.wxFlexGridSizer(0, 2, 0, 0)

                ui.StaticText_card_file = wx.wxStaticText(ui.Panel_card[i], wx.wxID_ANY, "SPI file path", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_card:Add(ui.StaticText_card_file, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.ComboBox_card[i] = wx.wxComboBox(ui.Panel_card[i], ID.COMBOBOX_CARD[i], "", wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
                ui.ComboBox_card[i]:Append("/dev/spi_sda")
                ui.ComboBox_card[i]:Append("/dev/spi_sdb")
                ui.FlexGridSizer_card:Add(ui.ComboBox_card[i], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText_card_timeout = wx.wxStaticText(ui.Panel_card[i], wx.wxID_ANY, "Card timeout [ms]", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_card:Add(ui.StaticText_card_timeout, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.SpinCtrl_card[i] = wx.wxSpinCtrl(ui.Panel_card[i], ID.SPINCTRL_CARD[i], "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 100, 2000)
                ui.FlexGridSizer_card:Add(ui.SpinCtrl_card[i], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticBoxSizer_card:Add(ui.FlexGridSizer_card, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Panel_card[i]:SetSizer(ui.StaticBoxSizer_card)

                ui.FlexGridSizerPanel1:Add(ui.Panel_card[i], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                this:Connect(ID.COMBOBOX_CARD[i], wx.wxEVT_COMMAND_COMBOBOX_SELECTED, event_value_updated)
                this:Connect(ID.COMBOBOX_CARD[i], wx.wxEVT_COMMAND_TEXT_UPDATED,      event_value_updated)
                this:Connect(ID.SPINCTRL_CARD[i], wx.wxEVT_COMMAND_SPINCTRL_UPDATED,  event_value_updated)
        end

        ui.StaticLine1 = wx.wxStaticLine(ui.Panel1, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, -1), wx.wxLI_HORIZONTAL)
        ui.FlexGridSizerPanel1:Add(ui.StaticLine1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        ui.StaticText4 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Note: clock frequency is configured in the SPI module", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizerPanel1:Add(ui.StaticText4, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        ui.Panel1:SetSizer(ui.FlexGridSizerPanel1)
        ui.FlexGridSizer1:Add(ui.Panel1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        ui.StaticLine1 = wx.wxStaticLine(this, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, -1), wx.wxLI_HORIZONTAL)
        ui.FlexGridSizer1:Add(ui.StaticLine1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        ui.Button_save = wx.wxButton(this, ID.BUTTON_SAVE, "Save", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer1:Add(ui.Button_save, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)

        --
        this:SetSizer(ui.FlexGridSizer1)
        this:SetScrollRate(50, 50)

        --
        this:Connect(ID.CHECKBOX_MODULE_ENABLE, wx.wxEVT_COMMAND_CHECKBOX_CLICKED, event_checkbox_module_enable_updated)
        this:Connect(ID.BUTTON_SAVE,            wx.wxEVT_COMMAND_BUTTON_CLICKED,   event_on_button_save_click          )
        this:Connect(ID.CHOICE_CARD_COUNT,      wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_number_of_cards_changed       )

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
        return "SDSPI"
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
