--[[============================================================================
@file    stm32f1-pll.lua

@author  Daniel Zorychta

@brief   Configuration script for PLL module.

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
pll = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local ui        = {}
local ID        = {}


--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function returns string suitable for labels that show calculated frequencies
-- @param  freq         frequency to print (Hz)
-- @return Converted frequnecy to string
--------------------------------------------------------------------------------
local function frequency(freq)
        return ": "..wizcore:print_freq(freq)
end


--------------------------------------------------------------------------------
-- @brief  Function loads all controls from configuration scripts
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_controls()
        -- load module enable status
        local module_enable = wizcore:get_module_state("PLL")
        ui.CheckBox_module_enable:SetValue(module_enable)
        ui.Panel1:Enable(module_enable)

end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function event_on_button_save_click()
        -- save module state
--         wizcore:enable_module("UART", ui.CheckBox_module_enable:GetValue())

        ui.Button_save:Enable(false)
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


--==============================================================================
-- GLOBAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function pll:create_window(parent)
        ui = {}
        ID = {}
        ID.CHECKBOX_MODULE_ENABLE = wx.wxNewId()
        ID.CHOICE_LSI = wx.wxNewId()
        ID.CHOICE_LSE = wx.wxNewId()
        ID.CHOICE_HSE = wx.wxNewId()
        ID.CHOICE_PREDIV2 = wx.wxNewId()
        ID.CHOICE_PLL2 = wx.wxNewId()
        ID.CHOICE_PLL3 = wx.wxNewId()
        ID.CHOICE_PREDIV1_CLKSRC = wx.wxNewId()
        ID.CHOICE_PREDIV1_VALUE = wx.wxNewId()
        ID.CHOICE_PLL_CLKSRC = wx.wxNewId()
        ID.CHOICE_PLL = wx.wxNewId()
        ID.CHOICE_SYSTEM_CLKSRC = wx.wxNewId()
        ID.CHOICE_RTC_CLKSRC = wx.wxNewId()
        ID.CHOICE_MCO_CLKSRC = wx.wxNewId()
        ID.CHOICE_USB_CLKSRC = wx.wxNewId()
        ID.CHOICE_I2S2_CLKSRC = wx.wxNewId()
        ID.CHOICE_I2S3_CLKSRC = wx.wxNewId()
        ID.CHOICE_AHB_PRESCALER = wx.wxNewId()
        ID.CHOICE_APB1_PRESCALER = wx.wxNewId()
        ID.CHOICE_APB2_PRESCALER = wx.wxNewId()
        ID.CHOICE_ADC_PRESCALER = wx.wxNewId()
        ID.PANEL1 = wx.wxNewId()
        ID.BUTTON_SAVE = wx.wxNewId()

        ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
        local this = ui.window

        ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.CheckBox_module_enable = wx.wxCheckBox(this, ID.CHECKBOX_MODULE_ENABLE, "Enable module", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_MODULE_ENABLE")
        ui.FlexGridSizer1:Add(ui.CheckBox_module_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Panel1 = wx.wxPanel(this, ID.PANEL1, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL, "ID.PANEL1")
        ui.FlexGridSizer2 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.FlexGridSizer3 = wx.wxFlexGridSizer(0, 3, 0, 0)
        ui.StaticText1 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "LSI oscillator", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText1, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_LSI = wx.wxChoice(ui.Panel1, ID.CHOICE_LSI, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_LSI")
        ui.Choice_LSI:Append("Disabled")
        ui.Choice_LSI:Append("Enabled")
        ui.FlexGridSizer3:Add(ui.Choice_LSI, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_LSI = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_LSI, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText2 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "LSE oscillator", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText2, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_LSE = wx.wxChoice(ui.Panel1, ID.CHOICE_LSE, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_LSE")
        ui.Choice_LSE:Append("Disabled")
        ui.Choice_LSE:Append("Enabled")
        ui.Choice_LSE:Append("Bypassed with external clock")
        ui.FlexGridSizer3:Add(ui.Choice_LSE, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_LSE = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_LSE, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText3 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "HSE oscillator", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText3, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_HSE = wx.wxChoice(ui.Panel1, ID.CHOICE_HSE, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_HSE")
        ui.Choice_HSE:Append("Disabled")
        ui.Choice_HSE:Append("Enabled")
        ui.Choice_HSE:Append("Bypassed with external clock")
        ui.FlexGridSizer3:Add(ui.Choice_HSE, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_HSE = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_HSE, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText4 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "PREDIV2", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText4, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_PREDIV2 = wx.wxChoice(ui.Panel1, ID.CHOICE_PREDIV2, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_PREDIV2")
        ui.Choice_PREDIV2:Append("HSE/1")
        ui.Choice_PREDIV2:Append("HSE/2")
        ui.Choice_PREDIV2:Append("HSE/3")
        ui.Choice_PREDIV2:Append("HSE/4")
        ui.Choice_PREDIV2:Append("HSE/5")
        ui.Choice_PREDIV2:Append("HSE/6")
        ui.Choice_PREDIV2:Append("HSE/7")
        ui.Choice_PREDIV2:Append("HSE/8")
        ui.Choice_PREDIV2:Append("HSE/9")
        ui.Choice_PREDIV2:Append("HSE/10")
        ui.Choice_PREDIV2:Append("HSE/11")
        ui.Choice_PREDIV2:Append("HSE/12")
        ui.Choice_PREDIV2:Append("HSE/13")
        ui.Choice_PREDIV2:Append("HSE/14")
        ui.Choice_PREDIV2:Append("HSE/15")
        ui.Choice_PREDIV2:Append("HSE/16")
        ui.FlexGridSizer3:Add(ui.Choice_PREDIV2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_PREDIV2 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_PREDIV2, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText5 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "PLL2", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText5, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_PLL2 = wx.wxChoice(ui.Panel1, ID.CHOICE_PLL2, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_PLL2")
        ui.Choice_PLL2:Append("Disabled")
        ui.Choice_PLL2:Append("PREDIV2 clock x8")
        ui.Choice_PLL2:Append("PREDIV2 clock x9")
        ui.Choice_PLL2:Append("PREDIV2 clock x10")
        ui.Choice_PLL2:Append("PREDIV2 clock x11")
        ui.Choice_PLL2:Append("PREDIV2 clock x12")
        ui.Choice_PLL2:Append("PREDIV2 clock x13")
        ui.Choice_PLL2:Append("PREDIV2 clock x14")
        ui.Choice_PLL2:Append("PREDIV2 clock x16")
        ui.Choice_PLL2:Append("PREDIV2 clock x20")
        ui.FlexGridSizer3:Add(ui.Choice_PLL2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_PLL2 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_PLL2, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText6 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "PLL3", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText6, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_PLL3 = wx.wxChoice(ui.Panel1, ID.CHOICE_PLL3, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_PLL3")
        ui.Choice_PLL3:Append("Disabled")
        ui.Choice_PLL3:Append("PREDIV2 clock x8")
        ui.Choice_PLL3:Append("PREDIV2 clock x9")
        ui.Choice_PLL3:Append("PREDIV2 clock x10")
        ui.Choice_PLL3:Append("PREDIV2 clock x11")
        ui.Choice_PLL3:Append("PREDIV2 clock x12")
        ui.Choice_PLL3:Append("PREDIV2 clock x13")
        ui.Choice_PLL3:Append("PREDIV2 clock x14")
        ui.Choice_PLL3:Append("PREDIV2 clock x16")
        ui.Choice_PLL3:Append("PREDIV2 clock x20")
        ui.FlexGridSizer3:Add(ui.Choice_PLL3, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_PLL3 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_PLL3, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText8 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "PREDIV1", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText8, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.BoxSizer2 = wx.wxBoxSizer(wx.wxHORIZONTAL)
        ui.Choice_PREDIV1_clksrc = wx.wxChoice(ui.Panel1, ID.CHOICE_PREDIV1_CLKSRC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_PREDIV1_CLKSRC")
        ui.Choice_PREDIV1_clksrc:Append("HSE")
        ui.Choice_PREDIV1_clksrc:Append("PLL2")
        ui.Choice_PREDIV1_clksrc:SetToolTip("PREDIV1 clock source")
        ui.BoxSizer2:Add(ui.Choice_PREDIV1_clksrc, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_PREDIV1_value = wx.wxChoice(ui.Panel1, ID.CHOICE_PREDIV1_VALUE, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_PREDIV1_VALUE")
        ui.Choice_PREDIV1_value:Append("/1")
        ui.Choice_PREDIV1_value:Append("/2")
        ui.Choice_PREDIV1_value:Append("/3")
        ui.Choice_PREDIV1_value:Append("/4")
        ui.Choice_PREDIV1_value:Append("/5")
        ui.Choice_PREDIV1_value:Append("/6")
        ui.Choice_PREDIV1_value:Append("/7")
        ui.Choice_PREDIV1_value:Append("/8")
        ui.Choice_PREDIV1_value:Append("/9")
        ui.Choice_PREDIV1_value:Append("/10")
        ui.Choice_PREDIV1_value:Append("/11")
        ui.Choice_PREDIV1_value:Append("/12")
        ui.Choice_PREDIV1_value:Append("/13")
        ui.Choice_PREDIV1_value:Append("/14")
        ui.Choice_PREDIV1_value:Append("/15")
        ui.Choice_PREDIV1_value:Append("/16")
        ui.Choice_PREDIV1_value:SetToolTip("PREDIV1 prescaler value")
        ui.BoxSizer2:Add(ui.Choice_PREDIV1_value, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.FlexGridSizer3:Add(ui.BoxSizer2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.StaticText_PREDIV1_value = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_PREDIV1_value, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText10 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "PLL", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText10, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.BoxSizer1 = wx.wxBoxSizer(wx.wxHORIZONTAL)
        ui.Choice_PLL_clksrc = wx.wxChoice(ui.Panel1, ID.CHOICE_PLL_CLKSRC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_PLL_CLKSRC")
        ui.Choice_PLL_clksrc:Append("HSI/2")
        ui.Choice_PLL_clksrc:Append("PREDIV1")
        ui.Choice_PLL_clksrc:SetToolTip("PLL clock source")
        ui.BoxSizer1:Add(ui.Choice_PLL_clksrc, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_PLL = wx.wxChoice(ui.Panel1, ID.CHOICE_PLL, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_PLL")
        ui.Choice_PLL:Append("Disabled")
        ui.Choice_PLL:Append("x4")
        ui.Choice_PLL:Append("x5")
        ui.Choice_PLL:Append("x6")
        ui.Choice_PLL:Append("x6.5")
        ui.Choice_PLL:Append("x7")
        ui.Choice_PLL:Append("x8")
        ui.Choice_PLL:Append("x9")
        ui.Choice_PLL:SetToolTip("PLL multiplier")
        ui.BoxSizer1:Add(ui.Choice_PLL, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.FlexGridSizer3:Add(ui.BoxSizer1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.StaticText_PLL = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_PLL, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText11 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "System clock source", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText11, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_system_clksrc = wx.wxChoice(ui.Panel1, ID.CHOICE_SYSTEM_CLKSRC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_SYSTEM_CLKSRC")
        ui.Choice_system_clksrc:Append("HSI")
        ui.Choice_system_clksrc:Append("HSE")
        ui.Choice_system_clksrc:Append("PLL")
        ui.FlexGridSizer3:Add(ui.Choice_system_clksrc, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_system_clksrc = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_system_clksrc, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText12 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "RTC clock source", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText12, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_RTC_clksrc = wx.wxChoice(ui.Panel1, ID.CHOICE_RTC_CLKSRC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_RTC_CLKSRC")
        ui.Choice_RTC_clksrc:Append("LSE oscillator clock")
        ui.Choice_RTC_clksrc:Append("LSI oscillator clock")
        ui.Choice_RTC_clksrc:Append("HSE oscillator clock divided by 128")
        ui.FlexGridSizer3:Add(ui.Choice_RTC_clksrc, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_RTC_clksrc = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_RTC_clksrc, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText13 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "MCO clock source", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText13, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_MCO_clksrc = wx.wxChoice(ui.Panel1, ID.CHOICE_MCO_CLKSRC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_MCO_CLKSRC")
        ui.Choice_MCO_clksrc:Append("No clock")
        ui.Choice_MCO_clksrc:Append("System clock")
        ui.Choice_MCO_clksrc:Append("HSI oscillator clock")
        ui.Choice_MCO_clksrc:Append("HSE oscillator clock")
        ui.Choice_MCO_clksrc:Append("PLL clock divided by 2")
        ui.Choice_MCO_clksrc:Append("PLL2 clock")
        ui.Choice_MCO_clksrc:Append("PLL3 clock divided by 2")
        ui.Choice_MCO_clksrc:Append("PLL3 clock")
        ui.Choice_MCO_clksrc:Append("External oscillator clock")
        ui.FlexGridSizer3:Add(ui.Choice_MCO_clksrc, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_MCO_clksrc = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_MCO_clksrc, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText14 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "USB clock source", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText14, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_USB_clksrc = wx.wxChoice(ui.Panel1, ID.CHOICE_USB_CLKSRC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_USB_CLKSRC")
        ui.Choice_USB_clksrc:Append("PLL VCO clock divided by 2")
        ui.Choice_USB_clksrc:Append("PLL VCO clock divided by 3")
        ui.FlexGridSizer3:Add(ui.Choice_USB_clksrc, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_USB_clksrc = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_USB_clksrc, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText15 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "I2S2 clock source", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText15, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_I2S2_clksrc = wx.wxChoice(ui.Panel1, ID.CHOICE_I2S2_CLKSRC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_I2S2_CLKSRC")
        ui.Choice_I2S2_clksrc:Append("System clock")
        ui.Choice_I2S2_clksrc:Append("PLL3 VCO clock")
        ui.FlexGridSizer3:Add(ui.Choice_I2S2_clksrc, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_I2S2_clksrc = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_I2S2_clksrc, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText16 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "I2S3 clock source", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText16, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_I2S3_clksrc = wx.wxChoice(ui.Panel1, ID.CHOICE_I2S3_CLKSRC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_I2S3_CLKSRC")
        ui.Choice_I2S3_clksrc:Append("System clock")
        ui.Choice_I2S3_clksrc:Append("PLL3 VCO clock")
        ui.FlexGridSizer3:Add(ui.Choice_I2S3_clksrc, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_I2S3_clksrc = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_I2S3_clksrc, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText17 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "AHB prescaler", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText17, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_AHB_prescaler = wx.wxChoice(ui.Panel1, ID.CHOICE_AHB_PRESCALER, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_AHB_PRESCALER")
        ui.Choice_AHB_prescaler:Append("SYSCLK / 1")
        ui.Choice_AHB_prescaler:Append("SYSCLK / 2")
        ui.Choice_AHB_prescaler:Append("SYSCLK / 4")
        ui.Choice_AHB_prescaler:Append("SYSCLK / 8")
        ui.Choice_AHB_prescaler:Append("SYSCLK / 16")
        ui.Choice_AHB_prescaler:Append("SYSCLK / 64")
        ui.Choice_AHB_prescaler:Append("SYSCLK / 128")
        ui.Choice_AHB_prescaler:Append("SYSCLK / 256")
        ui.Choice_AHB_prescaler:Append("SYSCLK / 512")
        ui.FlexGridSizer3:Add(ui.Choice_AHB_prescaler, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_AHB_prescaler = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_AHB_prescaler, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText18 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "APB1 prescaler", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText18, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_APB1_prescaler = wx.wxChoice(ui.Panel1, ID.CHOICE_APB1_PRESCALER, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_APB1_PRESCALER")
        ui.Choice_APB1_prescaler:Append("HCLK / 1")
        ui.Choice_APB1_prescaler:Append("HCLK / 2")
        ui.Choice_APB1_prescaler:Append("HCLK / 4")
        ui.Choice_APB1_prescaler:Append("HCLK / 8")
        ui.Choice_APB1_prescaler:Append("HCLK / 16")
        ui.FlexGridSizer3:Add(ui.Choice_APB1_prescaler, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_APB1_prescaler = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_APB1_prescaler, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText19 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "APB2 prescaler", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText19, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_APB2_prescaler = wx.wxChoice(ui.Panel1, ID.CHOICE_APB2_PRESCALER, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_APB2_PRESCALER")
        ui.Choice_APB2_prescaler:Append("HCLK / 1")
        ui.Choice_APB2_prescaler:Append("HCLK / 2")
        ui.Choice_APB2_prescaler:Append("HCLK / 4")
        ui.Choice_APB2_prescaler:Append("HCLK / 8")
        ui.Choice_APB2_prescaler:Append("HCLK / 16")
        ui.FlexGridSizer3:Add(ui.Choice_APB2_prescaler, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_APB2_prescaler = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_APB2_prescaler, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText20 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "ADC prescaler", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText20, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_ADC_prescaler = wx.wxChoice(ui.Panel1, ID.CHOICE_ADC_PRESCALER, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_ADC_PRESCALER")
        ui.Choice_ADC_prescaler:Append("PCLK2 / 2")
        ui.Choice_ADC_prescaler:Append("PCLK2 / 4")
        ui.Choice_ADC_prescaler:Append("PCLK2 / 6")
        ui.Choice_ADC_prescaler:Append("PCLK2 / 8")
        ui.FlexGridSizer3:Add(ui.Choice_ADC_prescaler, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_ADC_prescaler = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_ADC_prescaler, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.FlexGridSizer2:Add(ui.FlexGridSizer3, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.Panel1:SetSizer(ui.FlexGridSizer2)
        ui.FlexGridSizer1:Add(ui.Panel1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticLine1 = wx.wxStaticLine(this, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxSize(wizcore.CONTROL_X_SIZE, -1), wx.wxLI_HORIZONTAL, "wx.wxID_ANY")
        ui.FlexGridSizer1:Add(ui.StaticLine1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Button_save = wx.wxButton(this, ID.BUTTON_SAVE, "Save", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.BUTTON_SAVE")
        ui.FlexGridSizer1:Add(ui.Button_save, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)


        --
        this:SetSizer(ui.FlexGridSizer1)
        this:SetScrollRate(50, 50)

        --
        this:Connect(ID.CHECKBOX_MODULE_ENABLE, wx.wxEVT_COMMAND_CHECKBOX_CLICKED, event_checkbox_module_enable_updated)
        this:Connect(ID.CHOICE_LSI,             wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_LSE,             wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_HSE,             wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_PREDIV2,         wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_PLL2,            wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_PLL3,            wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_PREDIV1_CLKSRC,  wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_PREDIV1_VALUE,   wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_PLL_CLKSRC,      wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_PLL,             wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_SYSTEM_CLKSRC,   wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_RTC_CLKSRC,      wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_MCO_CLKSRC,      wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_USB_CLKSRC,      wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_I2S2_CLKSRC,     wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_I2S3_CLKSRC,     wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_AHB_PRESCALER,   wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_APB1_PRESCALER,  wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_APB2_PRESCALER,  wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_ADC_PRESCALER,   wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
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
function pll:get_window_name()
        return "PLL"
end


--------------------------------------------------------------------------------
-- @brief  Function is called by parent when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function pll:selected()
end


--------------------------------------------------------------------------------
-- @brief  Function returns modify status
-- @param  None
-- @return If data is modified true is returned, otherwise false
--------------------------------------------------------------------------------
function pll:is_modified()
        return ui.Button_save:IsEnabled()
end


--------------------------------------------------------------------------------
-- @brief  Function returns module handler
-- @param  None
-- @return Module handler
--------------------------------------------------------------------------------
function get_handler()
        return pll
end
