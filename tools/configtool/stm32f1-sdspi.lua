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
local ui         = {}
local ID         = {}
local cpu_name   = wizcore:key_read(config.arch.stm32f1.key.CPU_NAME)
local cpu_idx    = wizcore:get_cpu_index("stm32f1", cpu_name)
local spi_cfg    = config.arch.stm32f1.cpulist:Children()[cpu_idx].peripherals.SPI
local gpio       = require("stm32f1-gpio").get_handler()
local pin_list   = gpio:get_pin_list(true)
local prio_list  = wizcore:get_priority_list("stm32f1")


local clkdiv_str = {}
clkdiv_str["2"]   = 0
clkdiv_str["4"]   = 1
clkdiv_str["8"]   = 2
clkdiv_str["16"]  = 3
clkdiv_str["32"]  = 4
clkdiv_str["64"]  = 5
clkdiv_str["128"] = 6
clkdiv_str["256"] = 7



--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function loads all controls from configuration scripts
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_controls()
        -- load configuration from files
        local module_enable = wizcore:get_module_state("SDSPI")
        local spiport       = tonumber(wizcore:key_read(config.arch.stm32f1.key.SDSPI_SPI_PORT))
        local spidiv        = clkdiv_str[wizcore:key_read(config.arch.stm32f1.key.SDSPI_SPI_CLK_DIV)]
        local chip_select   = wizcore:get_string_index(pin_list, wizcore:key_read(config.arch.stm32f1.key.SDSPI_SD_CS_PIN))
        local timeout       = tonumber(wizcore:key_read(config.arch.stm32f1.key.SDSPI_TIMEOUT))
        local enable_DMA    = wizcore:yes_no_to_bool(wizcore:key_read(config.arch.stm32f1.key.SDSPI_ENABLE_DMA))
        local DMA_irq_prio  = wizcore:key_read(config.arch.stm32f1.key.SDSPI_DMA_IRQ_PRIORITY)

        -- check if port exist in the selected microcontroller (if not then warning value is selected)
        local port_exist = false
        for i = 0, ui.Choice_SPI_port:GetCount() - 1 do
                if ui.Choice_SPI_port:GetString(i) == "SPI"..spiport then
                        port_exist = true
                        spiport = i
                        break
                end
        end

        if not port_exist then
                spiport = ui.Choice_SPI_port:GetCount() - 1
        end

        -- analyze priority value
        if DMA_irq_prio == "CONFIG_USER_IRQ_PRIORITY" then
                DMA_irq_prio = #prio_list
        else
                DMA_irq_prio = math.floor(tonumber(DMA_irq_prio) / 16)
        end

        -- load controls
        ui.CheckBox_module_enable:SetValue(module_enable)
        ui.Panel1:Enable(module_enable)
        ui.Choice_SPI_port:SetSelection(spiport)
        ui.Choice_clkdiv:SetSelection(spidiv)
        ui.Choice_cs_pin_name:SetSelection(chip_select)
        ui.SpinCtrl_card_timeout:SetValue(timeout)
        ui.CheckBox_DMA_enable:SetValue(enable_DMA)
        ui.Choice_DMA_IRQ_prio:Enable(enable_DMA)
        ui.StaticText_DMA_irq_prio:Enable(enable_DMA)
        ui.Choice_DMA_IRQ_prio:SetSelection(DMA_irq_prio)
end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return On success true, otherwise false
--------------------------------------------------------------------------------
local function event_on_button_save_click()
        -- load values from contorls
        local module_enable = ui.CheckBox_module_enable:GetValue()
        local spiport       = ui.Choice_SPI_port:GetSelection() + 1
        local spidiv        = tostring(math.pow(2, ui.Choice_clkdiv:GetSelection() + 1))
        local chip_select   = pin_list[ui.Choice_cs_pin_name:GetSelection()]
        local timeout       = tostring(ui.SpinCtrl_card_timeout:GetValue())
        local DMA_enable    = wizcore:bool_to_yes_no(ui.CheckBox_DMA_enable:GetValue())
        local DMA_irq_prio  = ui.Choice_DMA_IRQ_prio:GetSelection() + 1

        -- check SPI port selection
        if spi_cfg:Children()[ui.Choice_SPI_port:GetSelection() + 1] ~= nil then
                spiport = spi_cfg:Children()[ui.Choice_SPI_port:GetSelection() + 1].name:GetValue()
        else
                wizcore:show_info_msg(wizcore.MAIN_WINDOW_NAME, "Selected SPI port is not present!\n\nSelect correct SPI port and try again.")
                return false
        end

        -- check Chip Select selection
        if chip_select == nil then
                wizcore:show_info_msg(wizcore.MAIN_WINDOW_NAME, "Selected pin is not defined!\n\nSelect correct pin and try again.")
                return false
        end

        -- convert DMA irq priority selection to configuration
        if DMA_irq_prio > #prio_list then
                DMA_irq_prio = config.project.def.DEFAULT_IRQ_PRIORITY:GetValue()
        else
                DMA_irq_prio = prio_list[DMA_irq_prio].value
        end

        -- save configuration
        wizcore:enable_module("SDSPI", module_enable)
        wizcore:key_write(config.arch.stm32f1.key.SDSPI_SPI_PORT, spiport)
        wizcore:key_write(config.arch.stm32f1.key.SDSPI_SPI_CLK_DIV, spidiv)
        wizcore:key_write(config.arch.stm32f1.key.SDSPI_SD_CS_PIN, chip_select)
        wizcore:key_write(config.arch.stm32f1.key.SDSPI_TIMEOUT, timeout)
        wizcore:key_write(config.arch.stm32f1.key.SDSPI_ENABLE_DMA, DMA_enable)
        wizcore:key_write(config.arch.stm32f1.key.SDSPI_DMA_IRQ_PRIORITY, DMA_irq_prio)

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
-- @brief  Event is called when DMA enable checkbox is changed
-- @param  this         event object
-- @return None
--------------------------------------------------------------------------------
local function event_checkbox_DMA_enable_updated(this)
        ui.StaticText_DMA_irq_prio:Enable(this:IsChecked())
        ui.Choice_DMA_IRQ_prio:Enable(this:IsChecked())
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
function sdspi:create_window(parent)
        ui = {}

        ID = {}
        ID.CHECKBOX_MODULE_ENABLE = wx.wxNewId()
        ID.CHOICE_SPI_PORT = wx.wxNewId()
        ID.CHOICE_CLKDIV = wx.wxNewId()
        ID.CHOICE_CS_PIN_NAME = wx.wxNewId()
        ID.SPINCTRL_CARD_TIMEOUT = wx.wxNewId()
        ID.CHECKBOX_DMA_ENABLE = wx.wxNewId()
        ID.CHOICE_DMA_IRQ_PRIO = wx.wxNewId()
        ID.PANEL1 = wx.wxNewId()
        ID.BUTTON_SAVE = wx.wxNewId()


        ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
        local this = ui.window

        ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.CheckBox_module_enable = wx.wxCheckBox(this, ID.CHECKBOX_MODULE_ENABLE, "Enable module", wx.wxDefaultPosition, wx.wxSize(wizcore.CONTROL_X_SIZE, -1), 0, wx.wxDefaultValidator, "ID.CHECKBOX_MODULE_ENABLE")
        ui.FlexGridSizer1:Add(ui.CheckBox_module_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Panel1 = wx.wxPanel(this, ID.PANEL1, wx.wxDefaultPosition, wx.wxSize(wizcore.CONTROL_X_SIZE, -1), wx.wxTAB_TRAVERSAL, "ID.PANEL1")
        ui.FlexGridSizer2 = wx.wxFlexGridSizer(0, 2, 0, 0)
        ui.StaticText1 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "SPI port", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer2:Add(ui.StaticText1, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_SPI_port = wx.wxChoice(ui.Panel1, ID.CHOICE_SPI_PORT, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_SPI_PORT")
        for i = 1, spi_cfg:NumChildren() do ui.Choice_SPI_port:Append("SPI"..spi_cfg:Children()[i].name:GetValue()) end
        ui.Choice_SPI_port:Append("*NOT PRESENT*")
        ui.FlexGridSizer2:Add(ui.Choice_SPI_port, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText2 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "SPI clock divider", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer2:Add(ui.StaticText2, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_clkdiv = wx.wxChoice(ui.Panel1, ID.CHOICE_CLKDIV, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_CLKDIV")
        ui.Choice_clkdiv:Append("Peripheral clock /2")
        ui.Choice_clkdiv:Append("Peripheral clock /4")
        ui.Choice_clkdiv:Append("Peripheral clock /8")
        ui.Choice_clkdiv:Append("Peripheral clock /16")
        ui.Choice_clkdiv:Append("Peripheral clock /32")
        ui.Choice_clkdiv:Append("Peripheral clock /64")
        ui.Choice_clkdiv:Append("Peripheral clock /128")
        ui.Choice_clkdiv:Append("Peripheral clock /256")
        ui.FlexGridSizer2:Add(ui.Choice_clkdiv, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText3 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Card Chip Select pin", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer2:Add(ui.StaticText3, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_cs_pin_name = wx.wxChoice(ui.Panel1, ID.CHOICE_CS_PIN_NAME, wx.wxDefaultPosition, wx.wxDefaultSize, {"*UNDEFINED*"}, 0, wx.wxDefaultValidator, "ID.CHOICE_CS_PIN_NAME")
        ui.Choice_cs_pin_name:Append(pin_list)
        ui.FlexGridSizer2:Add(ui.Choice_cs_pin_name, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText4 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Card timeout [ms]", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer2:Add(ui.StaticText4, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.SpinCtrl_card_timeout = wx.wxSpinCtrl(ui.Panel1, ID.SPINCTRL_CARD_TIMEOUT, "0", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 100, 1000, 0, "ID.SPINCTRL_CARD_TIMEOUT")
        ui.FlexGridSizer2:Add(ui.SpinCtrl_card_timeout, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.CheckBox_DMA_enable = wx.wxCheckBox(ui.Panel1, ID.CHECKBOX_DMA_ENABLE, "Enable DMA", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_DMA_ENABLE")
        ui.FlexGridSizer2:Add(ui.CheckBox_DMA_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText5 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "<none>", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.StaticText5:Disable()
        ui.StaticText5:Hide()
        ui.FlexGridSizer2:Add(ui.StaticText5, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_DMA_irq_prio = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "DMA IRQ priority", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer2:Add(ui.StaticText_DMA_irq_prio, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_DMA_IRQ_prio = wx.wxChoice(ui.Panel1, ID.CHOICE_DMA_IRQ_PRIO, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_DMA_IRQ_PRIO")
        for i, item in ipairs(prio_list) do ui.Choice_DMA_IRQ_prio:Append(item.name) end
        ui.Choice_DMA_IRQ_prio:Append("System default")
        ui.FlexGridSizer2:Add(ui.Choice_DMA_IRQ_prio, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Panel1:SetSizer(ui.FlexGridSizer2)
        ui.FlexGridSizer1:Add(ui.Panel1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticLine1 = wx.wxStaticLine(this, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxSize(10,-1), wx.wxLI_HORIZONTAL, "wx.wxID_ANY")
        ui.FlexGridSizer1:Add(ui.StaticLine1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Button_save = wx.wxButton(this, ID.BUTTON_SAVE, "Save", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.BUTTON_SAVE")
        ui.FlexGridSizer1:Add(ui.Button_save, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)

        --
        this:SetSizer(ui.FlexGridSizer1)
        this:SetScrollRate(50, 50)

        --
        this:Connect(ID.CHECKBOX_MODULE_ENABLE, wx.wxEVT_COMMAND_CHECKBOX_CLICKED, event_checkbox_module_enable_updated)
        this:Connect(ID.CHECKBOX_DMA_ENABLE,    wx.wxEVT_COMMAND_CHECKBOX_CLICKED, event_checkbox_DMA_enable_updated   )
        this:Connect(ID.CHOICE_SPI_PORT,        wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_CLKDIV,          wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_CS_PIN_NAME,     wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_DMA_IRQ_PRIO,    wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.SPINCTRL_CARD_TIMEOUT,  wx.wxEVT_COMMAND_SPINCTRL_UPDATED, event_value_updated                 )
        this:Connect(ID.SPINCTRL_CARD_TIMEOUT,  wx.wxEVT_COMMAND_TEXT_UPDATED,     event_value_updated                 )
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
