--[[============================================================================
@file    spi.lua

@author  Daniel Zorychta

@brief   Configuration script for SPI module

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
spi = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local ui                    = {}
local ID                    = {}
local NUMBER_OF_CS          = 8
local NUMBER_OF_SPI_DEVICES = 3
local cpu_name              = nil
local cpu_idx               = nil
local spi_cfg               = nil
local pin_list              = nil
local gpio                  = require("arch/stm32f1/gpio").get_handler()
local prio_list             = ct:get_priority_list("stm32f1")

local clkdiv_str = {}
clkdiv_str.SPI_CLK_DIV_2   = 0
clkdiv_str.SPI_CLK_DIV_4   = 1
clkdiv_str.SPI_CLK_DIV_8   = 2
clkdiv_str.SPI_CLK_DIV_16  = 3
clkdiv_str.SPI_CLK_DIV_32  = 4
clkdiv_str.SPI_CLK_DIV_64  = 5
clkdiv_str.SPI_CLK_DIV_128 = 6
clkdiv_str.SPI_CLK_DIV_256 = 7

local spimode_str = {}
spimode_str.SPI_MODE_0 = 0
spimode_str.SPI_MODE_1 = 1
spimode_str.SPI_MODE_2 = 2
spimode_str.SPI_MODE_3 = 3


--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Load configuration
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_configuration()
        local module_enable = ct:get_module_state("SPI")
        local dummy_byte    = ct:key_read(config.arch.stm32f1.key.SPI_DEFAULT_DUMMY_BYTE):gsub("0x", ""):upper()
        local clkdividx     = clkdiv_str[ct:key_read(config.arch.stm32f1.key.SPI_DEFAULT_CLK_DIV)]
        local spimode       = spimode_str[ct:key_read(config.arch.stm32f1.key.SPI_DEFAULT_MODE)]
        local bitorder      = ifs(ct:yes_no_to_bool(ct:key_read(config.arch.stm32f1.key.SPI_DEFAULT_MSB_FIRST)), 0, 1)

        ui.TextCtrl_dummy_byte:SetValue(dummy_byte)
        ui.Choice_clock_divider:SetSelection(clkdividx)
        ui.Choice_SPI_mode:SetSelection(spimode)
        ui.Choice_bitorder:SetSelection(bitorder)

        for SPI = 1, spi_cfg:NumChildren() do
                local SPI_enable   = ct:yes_no_to_bool(ct:key_read(config.arch.stm32f1.key["SPI_SPI"..SPI.."_ENABLE"]))
                local use_DMA      = ct:yes_no_to_bool(ct:key_read(config.arch.stm32f1.key["SPI_SPI"..SPI.."_USE_DMA"]))
                local IRQ_priority = ct:key_read(config.arch.stm32f1.key["SPI_SPI"..SPI.."_PRIORITY"])
                local number_of_CS = tonumber(ct:key_read(config.arch.stm32f1.key["SPI_SPI"..SPI.."_NUMBER_OF_CS"])) - 1

                ui.CheckBox_enable_peripheral[SPI]:SetValue(SPI_enable)
                ui.Panel_settings[SPI]:Enable(SPI_enable)
                ui.CheckBox_use_DMA[SPI]:SetValue(use_DMA)
                ui.Choice_number_of_CS[SPI]:SetSelection(number_of_CS)

                if IRQ_priority == config.project.def.DEFAULT_IRQ_PRIORITY:GetValue() then
                        IRQ_priority = #prio_list
                else
                        IRQ_priority = math.floor(tonumber(IRQ_priority) / 16)
                end
                ui.Choice_IRQ_priority[SPI]:SetSelection(IRQ_priority)

                for CS = 0, NUMBER_OF_CS - 1 do
                        local pin_name = ct:key_read(config.arch.stm32f1.key["SPI_SPI"..SPI.."_CS"..CS.."_PIN_NAME"])
                        ui.Choice_CS_pin[SPI][CS]:SetSelection(ct:get_string_index(pin_list, pin_name) - 1)
                        ui.Panel_CS[SPI][CS]:Enable(CS <= number_of_CS)
                end
        end

        ui.CheckBox_enable_module:SetValue(module_enable)
        ui.Panel_module:Enable(module_enable)
end


--------------------------------------------------------------------------------
-- @brief  Save configuration
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function save_configuration()
        -- load selected values
        local module_enable = ui.CheckBox_enable_module:GetValue()
        local dummy_byte    = "0x"..ui.TextCtrl_dummy_byte:GetValue():upper()
        local clkdiv        = "SPI_CLK_DIV_"..math.pow(2, ui.Choice_clock_divider:GetSelection() + 1)
        local mode          = "SPI_MODE_"..ui.Choice_SPI_mode:GetSelection()
        local bitorder      = ifs(ui.Choice_bitorder:GetSelection() == 0, config.project.def.YES:GetValue(), config.project.def.NO:GetValue())

        -- write default values
        ct:key_write(config.arch.stm32f1.key.SPI_DEFAULT_DUMMY_BYTE, dummy_byte)
        ct:key_write(config.arch.stm32f1.key.SPI_DEFAULT_CLK_DIV, clkdiv)
        ct:key_write(config.arch.stm32f1.key.SPI_DEFAULT_MODE, mode)
        ct:key_write(config.arch.stm32f1.key.SPI_DEFAULT_MSB_FIRST, bitorder)

        -- disables all SPI ports (will be enabled later)
        for SPI = 1, NUMBER_OF_SPI_DEVICES do
                ct:key_write(config.arch.stm32f1.key["SPI_SPI"..SPI.."_ENABLE"], config.project.def.NO:GetValue())
        end

        -- write settings for specified SPI
        for SPI = 1, spi_cfg:NumChildren() do
                local SPI_enable    = ct:bool_to_yes_no(ui.CheckBox_enable_peripheral[SPI]:GetValue())
                local use_DMA       = ct:bool_to_yes_no(ui.CheckBox_use_DMA[SPI]:GetValue())
                local IRQ_priority  = ui.Choice_IRQ_priority[SPI]:GetSelection() + 1
                local number_of_CS  = tostring(ui.Choice_number_of_CS[SPI]:GetSelection() + 1)

                if IRQ_priority + 1 > #prio_list then
                        IRQ_priority = config.project.def.DEFAULT_IRQ_PRIORITY:GetValue()
                else
                        IRQ_priority = prio_list[IRQ_priority].value
                end

                ct:key_write(config.arch.stm32f1.key["SPI_SPI"..SPI.."_USE_DMA"], use_DMA)
                ct:key_write(config.arch.stm32f1.key["SPI_SPI"..SPI.."_PRIORITY"], IRQ_priority)
                ct:key_write(config.arch.stm32f1.key["SPI_SPI"..SPI.."_NUMBER_OF_CS"], number_of_CS)
                ct:key_write(config.arch.stm32f1.key["SPI_SPI"..SPI.."_ENABLE"], SPI_enable)

                for CS = 0, NUMBER_OF_CS - 1 do
                        local pin_name = pin_list[ui.Choice_CS_pin[SPI][CS]:GetSelection() + 1]

                        if pin_name then
                                ct:key_write(config.arch.stm32f1.key["SPI_SPI"..SPI.."_CS"..CS.."_PIN_NAME"], pin_name)
                        end
                end
        end

        ct:enable_module("SPI", module_enable)

        ui.Button_save:Enable(false)
        return true
end


--==============================================================================
-- GLOBAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function spi:create_window(parent)
        cpu_name = ct:key_read(config.arch.stm32f1.key.CPU_NAME)
        cpu_idx  = ct:get_cpu_index("stm32f1", cpu_name)
        spi_cfg  = config.arch.stm32f1.cpulist:Children()[cpu_idx].peripherals.SPI
        pin_list = gpio:get_pin_list(true)

        ui                                      = {}
        ui.Panel_peripheral                     = {}
        ui.FlexGridSizer_peripheral             = {}
        ui.CheckBox_enable_peripheral           = {}
        ui.Panel_settings                       = {}
        ui.FlexGridSizer_settings               = {}
        ui.FlexGridSizer_peripheral_settings    = {}
        ui.CheckBox_use_DMA                     = {}
        ui.Choice_IRQ_priority                  = {}
        ui.Choice_number_of_CS                  = {}
        ui.FlexGridSizer_CS_settings            = {}
        ui.Panel_CS                             = {}
        ui.FlexGridSizer_CS                     = {}
        ui.Choice_CS_pin                        = {}

        ID                              = {}
        ID.CHECKBOX_ENABLE_MODULE       = wx.wxNewId()
        ID.PANEL_MODULE                 = wx.wxNewId()
        ID.TEXTCTRL_DUMMY_BYTE          = wx.wxNewId()
        ID.CHOICE_CLOCK_DIVIDER         = wx.wxNewId()
        ID.CHOICE_SPI_MODE              = wx.wxNewId()
        ID.CHOICE_BIT_ORDER             = wx.wxNewId()
        ID.NOTEBOOK_PERIPHERAL          = wx.wxNewId()
        ID.BUTTON_SAVE                  = wx.wxNewId()
        ID.PANEL_PERIPHERAL             = {}
        ID.CHECKBOX_ENABLE_PERIPHERAL   = {}
        ID.PANEL_SETTINGS               = {}
        ID.CHECKBOX_USE_DMA             = {}
        ID.CHOICE_IRQ_PRIORITY          = {}
        ID.CHOICE_NUMBER_OF_CS          = {}
        ID.PANEL_CS                     = {}
        ID.CHOICE_CS_PIN                = {}

        -- create new scrolled window
        ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)

        -- add main sizer
        ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)

        -- add module enable checkbox
        ui.CheckBox_enable_module = wx.wxCheckBox(ui.window, ID.CHECKBOX_ENABLE_MODULE, "Enable module", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer1:Add(ui.CheckBox_enable_module, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.window:Connect(ID.CHECKBOX_ENABLE_MODULE, wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function(event) ui.Panel_module:Enable(event:IsChecked()) end)

        -- add main panel
        ui.Panel_module = wx.wxPanel(ui.window, ID.PANEL_MODULE, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_module = wx.wxFlexGridSizer(0, 1, 0, 0)

        -- add default settings group
        ui.StaticBoxSizer_default_settings = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_module, "Default settings for all peripherals")
        ui.FlexGridSizer_default_settings = wx.wxFlexGridSizer(0, 2, 0, 0)

        -- add dummy byte controls
        ui.StaticText = wx.wxStaticText(ui.Panel_module, wx.wxID_ANY, "Dummy byte   0x", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_default_settings:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 2)
        ui.TextCtrl_dummy_byte = wx.wxTextCtrl(ui.Panel_module, ID.TEXTCTRL_DUMMY_BYTE, "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, ct.hexvalidator)
        ui.TextCtrl_dummy_byte:SetMaxLength(2)
        ui.FlexGridSizer_default_settings:Add(ui.TextCtrl_dummy_byte, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.window:Connect(ID.TEXTCTRL_DUMMY_BYTE, wx.wxEVT_COMMAND_TEXT_UPDATED, function(event) ui.Button_save:Enable(true) end)

        -- add clock divider controls
        ui.StaticText = wx.wxStaticText(ui.Panel_module, wx.wxID_ANY, "Clock divider", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_default_settings:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 2)
        ui.Choice_clock_divider = wx.wxChoice(ui.Panel_module, ID.CHOICE_CLOCK_DIVIDER, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
        ui.Choice_clock_divider:Append("Peripheral clock / 2")
        ui.Choice_clock_divider:Append("Peripheral clock / 4")
        ui.Choice_clock_divider:Append("Peripheral clock / 8")
        ui.Choice_clock_divider:Append("Peripheral clock / 16")
        ui.Choice_clock_divider:Append("Peripheral clock / 32")
        ui.Choice_clock_divider:Append("Peripheral clock / 64")
        ui.Choice_clock_divider:Append("Peripheral clock / 128")
        ui.Choice_clock_divider:Append("Peripheral clock / 256")
        ui.FlexGridSizer_default_settings:Add(ui.Choice_clock_divider, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.window:Connect(ID.CHOICE_CLOCK_DIVIDER, wx.wxEVT_COMMAND_CHOICE_SELECTED, function(event) ui.Button_save:Enable(true) end)

        -- add SPI mode controls
        ui.StaticText = wx.wxStaticText(ui.Panel_module, wx.wxID_ANY, "SPI mode", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_default_settings:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 2)
        ui.Choice_SPI_mode = wx.wxChoice(ui.Panel_module, ID.CHOICE_SPI_MODE, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
        ui.Choice_SPI_mode:Append("Mode 0 (SCK Low at idle; capture on leading edge)")
        ui.Choice_SPI_mode:Append("Mode 1 (SCK Low at idle; capture on trailing edge)")
        ui.Choice_SPI_mode:Append("Mode 2 (SCK High at idle; capture on leading edge)")
        ui.Choice_SPI_mode:Append("Mode 3 (SCK High at idle; capture on trailing edge)")
        ui.FlexGridSizer_default_settings:Add(ui.Choice_SPI_mode, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.window:Connect(ID.CHOICE_SPI_MODE, wx.wxEVT_COMMAND_CHOICE_SELECTED, function(event) ui.Button_save:Enable(true) end)

        -- add bit order controls
        ui.StaticText = wx.wxStaticText(ui.Panel_module, wx.wxID_ANY, "Bit order", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_default_settings:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 2)
        ui.Choice_bitorder = wx.wxChoice(ui.Panel_module, ID.CHOICE_BIT_ORDER, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator)
        ui.Choice_bitorder:Append("MSb first")
        ui.Choice_bitorder:Append("LSb first")
        ui.FlexGridSizer_default_settings:Add(ui.Choice_bitorder, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.window:Connect(ID.CHOICE_BIT_ORDER, wx.wxEVT_COMMAND_CHOICE_SELECTED, function(event) ui.Button_save:Enable(true) end)

        -- add default settings group to the main group sizer
        ui.StaticBoxSizer_default_settings:Add(ui.FlexGridSizer_default_settings, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

        -- add default settings group to the module's sizer
        ui.FlexGridSizer_module:Add(ui.StaticBoxSizer_default_settings, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- add notebook with peripherals settings
        ui.Notebook_peripheral = wx.wxNotebook(ui.Panel_module, ID.NOTEBOOK_PERIPHERAL, wx.wxDefaultPosition, wx.wxDefaultSize)

        -- add pages to the notebook
        for SPI = 1, spi_cfg:NumChildren() do
                ID.PANEL_PERIPHERAL[SPI]             = wx.wxNewId()
                ID.CHECKBOX_ENABLE_PERIPHERAL[SPI]   = wx.wxNewId()
                ID.PANEL_SETTINGS[SPI]               = wx.wxNewId()
                ID.CHECKBOX_USE_DMA[SPI]             = wx.wxNewId()
                ID.CHOICE_IRQ_PRIORITY[SPI]          = wx.wxNewId()
                ID.CHOICE_NUMBER_OF_CS[SPI]          = wx.wxNewId()

                -- create a new panel for entire peripheral
                ui.Panel_peripheral[SPI] = wx.wxPanel(ui.Notebook_peripheral, ID.PANEL_PERIPHERAL[SPI], wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
                ui.FlexGridSizer_peripheral[SPI] = wx.wxFlexGridSizer(0, 1, 0, 0)

                -- add peripheral enable checkbox
                ui.CheckBox_enable_peripheral[SPI] = wx.wxCheckBox(ui.Panel_peripheral[SPI], ID.CHECKBOX_ENABLE_PERIPHERAL[SPI], "Enable peripheral", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_peripheral[SPI]:Add(ui.CheckBox_enable_peripheral[SPI], 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.window:Connect(ID.CHECKBOX_ENABLE_PERIPHERAL[SPI], wx.wxEVT_COMMAND_CHECKBOX_CLICKED,
                        function(event)
                                ui.Panel_settings[SPI]:Enable(event:IsChecked())
                                ui.Button_save:Enable(true)
                        end
                )

                -- create a new panel for peripheral settings
                ui.Panel_settings[SPI] = wx.wxPanel(ui.Panel_peripheral[SPI], ID.PANEL_SETTINGS[SPI], wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
                ui.FlexGridSizer_peripheral_settings[SPI] = wx.wxFlexGridSizer(0, 2, 0, 0)

                -- add use DMA checkbox
                ui.CheckBox_use_DMA[SPI] = wx.wxCheckBox(ui.Panel_settings[SPI], ID.CHECKBOX_USE_DMA[SPI], "Use DMA", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_peripheral_settings[SPI]:Add(ui.CheckBox_use_DMA[SPI], 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer_peripheral_settings[SPI]:Add(0,0,1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.window:Connect(ID.CHECKBOX_USE_DMA[SPI], wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function(event) ui.Button_save:Enable(true) end)

                -- add IRQ priority controls
                ui.StaticText = wx.wxStaticText(ui.Panel_settings[SPI], wx.wxID_ANY, "IRQ priority", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_peripheral_settings[SPI]:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_IRQ_priority[SPI] = wx.wxChoice(ui.Panel_settings[SPI], ID.CHOICE_IRQ_PRIORITY[SPI], wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
                for _, priority in ipairs(prio_list) do
                        ui.Choice_IRQ_priority[SPI]:Append(priority.name)
                end
                ui.Choice_IRQ_priority[SPI]:Append("System default")
                ui.FlexGridSizer_peripheral_settings[SPI]:Add(ui.Choice_IRQ_priority[SPI], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.window:Connect(ID.CHOICE_IRQ_PRIORITY[SPI], wx.wxEVT_COMMAND_CHOICE_SELECTED, function(event) ui.Button_save:Enable(true) end)

                -- add number of chip select controls
                ui.StaticText = wx.wxStaticText(ui.Panel_settings[SPI], wx.wxID_ANY, "Number of Chip Selects", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_peripheral_settings[SPI]:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_number_of_CS[SPI] = wx.wxChoice(ui.Panel_settings[SPI], ID.CHOICE_NUMBER_OF_CS[SPI], wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
                ui.Choice_number_of_CS[SPI]:Append("1 (CS0 only)")
                ui.Choice_number_of_CS[SPI]:Append("2 (CS0..1)")
                ui.Choice_number_of_CS[SPI]:Append("3 (CS0..2)")
                ui.Choice_number_of_CS[SPI]:Append("4 (CS0..3)")
                ui.Choice_number_of_CS[SPI]:Append("5 (CS0..4)")
                ui.Choice_number_of_CS[SPI]:Append("6 (CS0..5)")
                ui.Choice_number_of_CS[SPI]:Append("7 (CS0..6)")
                ui.Choice_number_of_CS[SPI]:Append("8 (CS0..7)")
                ui.FlexGridSizer_peripheral_settings[SPI]:Add(ui.Choice_number_of_CS[SPI], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.window:Connect(ID.CHOICE_NUMBER_OF_CS[SPI], wx.wxEVT_COMMAND_CHOICE_SELECTED,
                        function(event)
                                local number_of_cs = event:GetSelection() + 1
                                for CS = 0, NUMBER_OF_CS - 1 do
                                        ui.Panel_CS[SPI][CS]:Enable(CS < number_of_cs)
                                end
                                ui.Button_save:Enable(true)
                        end
                )

                -- add peripheral settings sizer to general settings sizer
                ui.FlexGridSizer_settings[SPI] = wx.wxFlexGridSizer(0, 1, 0, 0)
                ui.FlexGridSizer_settings[SPI]:Add(ui.FlexGridSizer_peripheral_settings[SPI], 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.FlexGridSizer_CS_settings[SPI] = wx.wxFlexGridSizer(0, 1, 0, 0)

                -- add chip select controls
                ui.Panel_CS[SPI]         = {}
                ui.FlexGridSizer_CS[SPI] = {}
                ui.Choice_CS_pin[SPI]    = {}
                ID.PANEL_CS[SPI]         = {}
                ID.CHOICE_CS_PIN[SPI]    = {}

                for CS = 0, NUMBER_OF_CS - 1 do
                        ID.PANEL_CS[SPI][CS]      = wx.wxNewId()
                        ID.CHOICE_CS_PIN[SPI][CS] = wx.wxNewId()

                        -- create a new panel for specified chip select
                        ui.Panel_CS[SPI][CS] = wx.wxPanel(ui.Panel_settings[SPI], ID.PANEL_CS[SPI][CS], wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
                        ui.FlexGridSizer_CS[SPI][CS] = wx.wxFlexGridSizer(0, 3, 0, 0)

                        -- add Chip select pin controls
                        ui.StaticText = wx.wxStaticText(ui.Panel_CS[SPI][CS], wx.wxID_ANY, "Pin for Chip Select "..CS, wx.wxDefaultPosition, wx.wxDefaultSize)
                        ui.FlexGridSizer_CS[SPI][CS]:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                        ui.FlexGridSizer_CS[SPI][CS]:Add(0,0,1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                        ui.Choice_CS_pin[SPI][CS] = wx.wxChoice(ui.Panel_CS[SPI][CS], ID.CHOICE_CS_PIN[SPI][CS], wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
                        ui.Choice_CS_pin[SPI][CS]:Append(pin_list)
                        ui.FlexGridSizer_CS[SPI][CS]:Add(ui.Choice_CS_pin[SPI][CS], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                        ui.window:Connect(ID.CHOICE_CS_PIN[SPI][CS], wx.wxEVT_COMMAND_CHOICE_SELECTED, function(event) ui.Button_save:Enable(true) end)

                        -- set sizer of chip select panel
                        ui.Panel_CS[SPI][CS]:SetSizer(ui.FlexGridSizer_CS[SPI][CS])

                        -- add chip select configuration panel to then Chip select settings sizer
                        ui.FlexGridSizer_CS_settings[SPI]:Add(ui.Panel_CS[SPI][CS], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                end

                -- add chip select settings sizer to settings sizer
                ui.FlexGridSizer_settings[SPI]:Add(ui.FlexGridSizer_CS_settings[SPI], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

                -- set settings panel sizer
                ui.Panel_settings[SPI]:SetSizer(ui.FlexGridSizer_settings[SPI])

                -- add to the peripheral sizer the settings panel
                ui.FlexGridSizer_peripheral[SPI]:Add(ui.Panel_settings[SPI], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.Panel_peripheral[SPI]:SetSizer(ui.FlexGridSizer_peripheral[SPI])

                -- add peripheral page to the notebook
                ui.Notebook_peripheral:AddPage(ui.Panel_peripheral[SPI], "SPI"..spi_cfg:Children()[SPI].name:GetValue(), false)
        end

        -- add notebook to the modules's sizer
        ui.FlexGridSizer_module:Add(ui.Notebook_peripheral, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- set module's sizer
        ui.Panel_module:SetSizer(ui.FlexGridSizer_module)
        ui.FlexGridSizer1:Add(ui.Panel_module, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- add horizontal line
        ui.StaticLine = wx.wxStaticLine(ui.window, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxSize(10,-1), wx.wxLI_HORIZONTAL)
        ui.FlexGridSizer1:Add(ui.StaticLine, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- add save button
        ui.Button_save = wx.wxButton(ui.window, ID.BUTTON_SAVE, "Save", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer1:Add(ui.Button_save, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.window:Connect(ID.BUTTON_SAVE, wx.wxEVT_COMMAND_BUTTON_CLICKED, save_configuration)

        -- set main sizer
        ui.window:SetSizer(ui.FlexGridSizer1)
        ui.window:SetScrollRate(15, 15)

        -- load configuration
        load_configuration()
        ui.Button_save:Enable(false)

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @param  None
-- @return Module name
--------------------------------------------------------------------------------
function spi:get_window_name()
        return "SPI"
end


--------------------------------------------------------------------------------
-- @brief  Function is called by parent when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function spi:selected()
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

                for SPI = 1, spi_cfg:NumChildren() do
                        for CS = 0, NUMBER_OF_CS - 1 do
                                ui.Choice_CS_pin[SPI][CS]:Clear()
                                ui.Choice_CS_pin[SPI][CS]:Append(pin_list)
                                local pin_name = ct:key_read(config.arch.stm32f1.key["SPI_SPI"..SPI.."_CS"..CS.."_PIN_NAME"])
                                ui.Choice_CS_pin[SPI][CS]:SetSelection(ct:get_string_index(pin_list, pin_name) - 1)
                        end
                end
        end
end


--------------------------------------------------------------------------------
-- @brief  Function returns modify status
-- @param  None
-- @return If data is modified true is returned, otherwise false
--------------------------------------------------------------------------------
function spi:is_modified()
        return ui.Button_save:IsEnabled()
end


--------------------------------------------------------------------------------
-- @brief  Function save configuration
-- @return None
--------------------------------------------------------------------------------
function spi:save()
        save_configuration()
end


--------------------------------------------------------------------------------
-- @brief  Function returns module handler
-- @param  None
-- @return Module handler
--------------------------------------------------------------------------------
function get_handler()
        return spi
end
