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
-- @brief  Load controls of selected SPI
-- @param  spi_number       SPI number wich controls will be load
-- @param  spi_cs_count     optional argument, if set then spi cs number is not loaded from config
-- @return None
--------------------------------------------------------------------------------
-- local function load_controls_of_selected_SPI(spi_number, spi_cs_count)
--         -- SPI enable status
--         if spi_cs_count == nil then
--                 local deven = ct:yes_no_to_bool(ct:key_read(config.arch.stm32f1.key["SPI_SPI"..spi_number.."_ENABLE"]))
--                 ui.CheckBox_device_enable:SetValue(deven)
--                 ui.Panel2:Enable(deven)
--         end
--
--         -- DMA enable
--         local DMA_en = ct:yes_no_to_bool(ct:key_read(config.arch.stm32f1.key["SPI_SPI"..spi_number.."_USE_DMA"]))
--         ui.CheckBox_DMA_enable:SetValue(DMA_en)
--
--         -- load current selection of number of cs pins
--         local NUMBER_OF_CS_pins
--         if spi_cs_count ~= nil and spi_cs_count >= 1 and spi_cs_count <= NUMBER_OF_CS then
--                 NUMBER_OF_CS_pins = spi_cs_count
--         else
--                 NUMBER_OF_CS_pins = tonumber(ct:key_read(config.arch.stm32f1.key["SPI_SPI"..spi_number.."_NUMBER_OF_CS"]))
--         end
--         ui.Choice_csnum:SetSelection(NUMBER_OF_CS_pins - 1)
--         ui.Choice_csnum.OldSelection = NUMBER_OF_CS_pins - 1
--
--         -- load names of CS pins
--         for cs = 0, NUMBER_OF_CS - 1 do
--                 if spi_cs_count == nil then
--                         local devcspin = ct:key_read(config.arch.stm32f1.key["SPI_SPI"..spi_number.."_CS"..cs.."_PIN_NAME"])
--                         ui.Choice_cspin[cs]:SetSelection(ct:get_string_index(pin_list, devcspin))
--
--                 end
--
--                 if cs < NUMBER_OF_CS_pins then
--                         ui.StaticText_cspin[cs]:Enable(true)
--                         ui.Choice_cspin[cs]:Enable(true)
--                 else
--                         ui.StaticText_cspin[cs]:Enable(false)
--                         ui.Choice_cspin[cs]:Enable(false)
--                 end
--         end
--
--         -- load value of IRQ priority
--         if spi_cs_count == nil then
--                 local devprio = ct:key_read(config.arch.stm32f1.key["SPI_SPI"..spi_number.."_PRIORITY"])
--
--                 if devprio == config.project.def.DEFAULT_IRQ_PRIORITY:GetValue() then
--                         devprio = #prio_list
--                 else
--                         devprio = math.floor(tonumber(devprio) / 16)
--                 end
--
--                 ui.Choice_irqprio:SetSelection(devprio)
--         end
-- end

--------------------------------------------------------------------------------
-- @brief  Load controls with default configuration for all interfaces
-- @param  None
-- @return None
--------------------------------------------------------------------------------
-- local function load_controls_of_defaults()
--         local enable     = ct:get_module_state("SPI")
--         local dummy_byte = ct:key_read(config.arch.stm32f1.key.SPI_DEFAULT_DUMMY_BYTE):gsub("0x", ""):upper()
--         local clkdividx  = clkdiv_str[ct:key_read(config.arch.stm32f1.key.SPI_DEFAULT_CLK_DIV)]
--         local spimode    = spimode_str[ct:key_read(config.arch.stm32f1.key.SPI_DEFAULT_MODE)]
--         local bitorder   = ifs(ct:yes_no_to_bool(ct:key_read(config.arch.stm32f1.key.SPI_DEFAULT_MSB_FIRST)), 0, 1)
--         local spisel     = spi_cfg:Children()[ui.Choice_device:GetSelection() + 1].name:GetValue()
--
--         ui.TextCtrl_dummy_byte:SetValue(dummy_byte)
--         ui.Choice_clkdiv:SetSelection(clkdividx)
--         ui.Choice_mode:SetSelection(spimode)
--         ui.Choice_bitorder:SetSelection(bitorder)
--
--         --
--         ui.CheckBox_enable:SetValue(enable)
--         ui.Panel1:Enable(enable)
-- end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return On success true, otherwise false
--------------------------------------------------------------------------------
local function save_configuration()
--         -- load selected values
--         local enable     = ui.CheckBox_enable:GetValue()
--         local dummy_byte = "0x"..ui.TextCtrl_dummy_byte:GetValue():upper()
--         local clkdiv     = "SPI_CLK_DIV_"..math.pow(2, ui.Choice_clkdiv:GetSelection() + 1)
--         local mode       = "SPI_MODE_"..ui.Choice_mode:GetSelection()
--         local bitorder   = ifs(ui.Choice_bitorder:GetSelection() == 0, config.project.def.YES:GetValue(), config.project.def.NO:GetValue())
--         local spisel     = spi_cfg:Children()[ui.Choice_device:GetSelection() + 1].name:GetValue()
--         local spien      = ct:bool_to_yes_no(ui.CheckBox_device_enable:GetValue())
--         local DMA_en     = ct:bool_to_yes_no(ui.CheckBox_DMA_enable:GetValue())
--         local irqprio    = ui.Choice_irqprio:GetSelection() + 1
--         local numofcs    = tostring(ui.Choice_csnum:GetSelection() + 1)
--
--         -- convert priority selection to value
--         if ui.Choice_irqprio:GetSelection() + 1 > #prio_list then
--                 irqprio = config.project.def.DEFAULT_IRQ_PRIORITY:GetValue()
--         else
--                 irqprio = prio_list[irqprio].value
--         end
--
--         -- check if all pins has defined connection to GPIO's pin name
--         local cspin     = {}
--         local undef_pin = false
--         if ui.Panel2:IsEnabled() then
--                 for pin = 0, NUMBER_OF_CS - 1 do
--                         cspin[pin] = pin_list[ui.Choice_cspin[pin]:GetSelection()]
--                         if cspin[pin] == nil and ui.Choice_cspin[pin]:IsEnabled() then
--                                 undef_pin  = true
--                         end
--                 end
--
--                 if undef_pin == true then
--                         ct:show_info_msg(ct.MAIN_WINDOW_NAME, "Selected not existing pin as Chip Select!\n\nSelect defined pin name and try again.")
--                         return false
--                 end
--         end
--
--         -- write default values
--         ct:key_write(config.arch.stm32f1.key.SPI_DEFAULT_DUMMY_BYTE, dummy_byte)
--         ct:key_write(config.arch.stm32f1.key.SPI_DEFAULT_CLK_DIV, clkdiv)
--         ct:key_write(config.arch.stm32f1.key.SPI_DEFAULT_MODE, mode)
--         ct:key_write(config.arch.stm32f1.key.SPI_DEFAULT_MSB_FIRST, bitorder)
--
--         -- disables not existing ports
--         for i = 1, NUMBER_OF_SPI_DEVICES do
--                 local exist = false
--                 for j = 1, spi_cfg:NumChildren() do
--                         if tostring(i) == spi_cfg:Children()[j].name:GetValue() then
--                                 exist = true
--                                 break
--                         end
--                 end
--
--                 if not exist then
--                         ct:key_write(config.arch.stm32f1.key["SPI_SPI"..i.."_ENABLE"], config.project.def.NO:GetValue())
--                 end
--         end
--
--         ct:key_write(config.arch.stm32f1.key["SPI_SPI"..spisel.."_ENABLE"], spien)
--
--         -- save DMA enable, priority and Chip Selects
--         if ui.Panel2:IsEnabled() then
--                 ct:key_write(config.arch.stm32f1.key["SPI_SPI"..spisel.."_USE_DMA"], DMA_en)
--                 ct:key_write(config.arch.stm32f1.key["SPI_SPI"..spisel.."_PRIORITY"], irqprio)
--                 ct:key_write(config.arch.stm32f1.key["SPI_SPI"..spisel.."_NUMBER_OF_CS"], numofcs)
--
--                 for pin = 0, NUMBER_OF_CS - 1 do
--                         if ui.Choice_cspin[pin]:IsEnabled() then
--                                 ct:key_write(config.arch.stm32f1.key["SPI_SPI"..spisel.."_CS"..pin.."_PIN_NAME"], cspin[pin])
--                         end
--                 end
--         end
--
--         ct:enable_module("SPI", enable)

        --
        ui.Button_save:Enable(false)
        return true
end


--------------------------------------------------------------------------------
-- @brief  Event is called when module enable checkbox is changed
-- @param  this     event object
-- @return None
--------------------------------------------------------------------------------
-- local function checkbox_enable_updated(this)
--         ui.Button_save:Enable(true)
--         ui.Panel1:Enable(this:IsChecked())
-- end


--------------------------------------------------------------------------------
-- @brief  Event is called when device enable checkbox is changed
-- @param  this     event object
-- @return None
--------------------------------------------------------------------------------
-- local function checkbox_device_enable_updated(this)
--         ui.Panel2:Enable(this:IsChecked())
--         ui.Button_save:Enable(true)
-- end


--------------------------------------------------------------------------------
-- @brief  Event is called when value is changed in general
-- @param  None
-- @return None
--------------------------------------------------------------------------------
-- local function value_updated()
--         ui.Button_save:Enable(true)
-- end


--------------------------------------------------------------------------------
-- @brief  Event is called when SPI device is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
-- local function spi_device_selected()
--         if ui.Choice_device.OldSelection ~= ui.Choice_device:GetSelection() then
--
--                 local answer = wx.wxID_NO
--                 if ui.Button_save:IsEnabled() then
--                         answer = ct:show_question_msg(ct.MAIN_WINDOW_NAME, "Do you want to SAVE changes?", bit.bor(wx.wxYES_NO, wx.wxCANCEL))
--                 end
--
--                 if answer == wx.wxID_YES then
--                         local to_save = ui.Choice_device.OldSelection
--                         ui.Choice_device.OldSelection = ui.Choice_device:GetSelection()
--                         ui.Choice_device:SetSelection(to_save)
--                         if on_button_save_click() == false then
--                                 ui.Choice_device.OldSelection = to_save
--                                 return
--                         else
--                                 ui.Choice_device:SetSelection(ui.Choice_device.OldSelection)
--                         end
--                 elseif answer == wx.wxID_NO then
--                         ui.Choice_device.OldSelection = ui.Choice_device:GetSelection()
--                 elseif answer == wx.wxID_CANCEL then
--                         ui.Choice_device:SetSelection(ui.Choice_device.OldSelection)
--                         return
--                 end
--
--                 local spisel = spi_cfg:Children()[ui.Choice_device:GetSelection() + 1].name:GetValue()
--                 load_controls_of_selected_SPI(tonumber(spisel))
--
--                 if answer == wx.wxID_YES or answer == wx.wxID_NO then
--                         ui.Button_save:Enable(false)
--                 end
--         end
-- end


--------------------------------------------------------------------------------
-- @brief  Event is called when number of CS pins is selected to a new value
-- @param  None
-- @return None
--------------------------------------------------------------------------------
-- local function NUMBER_OF_CS_selected()
--         if ui.Choice_csnum.OldSelection ~= ui.Choice_csnum:GetSelection() then
--                 ui.Choice_csnum.OldSelection = ui.Choice_csnum:GetSelection()
--                 local spisel = spi_cfg:Children()[ui.Choice_device:GetSelection() + 1].name:GetValue()
--                 local csnum  = ui.Choice_csnum:GetSelection() + 1
--                 load_controls_of_selected_SPI(tonumber(spisel), csnum)
--                 ui.Button_save:Enable(true)
--         end
-- end


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

        -- add main panel
        ui.Panel_module = wx.wxPanel(ui.window, ID.PANEL_MODULE, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_module = wx.wxFlexGridSizer(0, 1, 0, 0)

        -- add default settings group
        ui.StaticBoxSizer_default_settings = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_module, "Default settings for all peripherals")
        ui.FlexGridSizer_default_settings = wx.wxFlexGridSizer(0, 2, 0, 0)

        -- add dummy byte controls
        ui.StaticText = wx.wxStaticText(ui.Panel_module, wx.wxID_ANY, "Dummy byte   0x", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_default_settings:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.TextCtrl_dummy_byte = wx.wxTextCtrl(ui.Panel_module, ID.TEXTCTRL_DUMMY_BYTE, "", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_default_settings:Add(ui.TextCtrl_dummy_byte, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- add clock divider controls
        ui.StaticText = wx.wxStaticText(ui.Panel_module, wx.wxID_ANY, "Clock divider", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_default_settings:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
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

        -- add SPI mode controls
        ui.StaticText = wx.wxStaticText(ui.Panel_module, wx.wxID_ANY, "SPI mode", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_default_settings:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_SPI_mode = wx.wxChoice(ui.Panel_module, ID.CHOICE_SPI_MODE, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
        ui.Choice_SPI_mode:Append("Mode 0 (SCK Low at idle; capture on leading edge)")
        ui.Choice_SPI_mode:Append("Mode 1 (SCK Low at idle; capture on trailing edge)")
        ui.Choice_SPI_mode:Append("Mode 2 (SCK High at idle; capture on leading edge)")
        ui.Choice_SPI_mode:Append("Mode 3 (SCK High at idle; capture on trailing edge)")
        ui.FlexGridSizer_default_settings:Add(ui.Choice_SPI_mode, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- add bit order controls
        ui.StaticText = wx.wxStaticText(ui.Panel_module, wx.wxID_ANY, "Bit order", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_default_settings:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_bitorder = wx.wxChoice(ui.Panel_module, ID.CHOICE_BIT_ORDER, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator)
        ui.Choice_bitorder:Append("MSb first")
        ui.Choice_bitorder:Append("LSb first")
        ui.FlexGridSizer_default_settings:Add(ui.Choice_bitorder, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

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
                ID.PANEL_CS[SPI]                     = wx.wxNewId()
                ID.CHOICE_CS_PIN[SPI]                = wx.wxNewId()

                -- create a new panel for entire peripheral
                ui.Panel_peripheral[SPI] = wx.wxPanel(ui.Notebook_peripheral, ID.PANEL_PERIPHERAL[SPI], wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
                ui.FlexGridSizer_peripheral[SPI] = wx.wxFlexGridSizer(0, 1, 0, 0)

                -- add peripheral enable checkbox
                ui.CheckBox_enable_peripheral[SPI] = wx.wxCheckBox(ui.Panel_peripheral[SPI], ID.CHECKBOX_ENABLE_PERIPHERAL[SPI], "Enable peripheral", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_peripheral[SPI]:Add(ui.CheckBox_enable_peripheral[SPI], 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- create a new panel for peripheral settings
                ui.Panel_settings[SPI] = wx.wxPanel(ui.Panel_peripheral[SPI], ID.PANEL_SETTINGS[SPI], wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
                ui.FlexGridSizer_peripheral_settings[SPI] = wx.wxFlexGridSizer(0, 2, 0, 0)

                -- add use DMA checkbox
                ui.CheckBox_use_DMA[SPI] = wx.wxCheckBox(ui.Panel_settings[SPI], ID.CHECKBOX_USE_DMA[SPI], "Use DMA", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_peripheral_settings[SPI]:Add(ui.CheckBox_use_DMA[SPI], 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer_peripheral_settings[SPI]:Add(0,0,1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add IRQ priority controls
                ui.StaticText = wx.wxStaticText(ui.Panel_settings[SPI], wx.wxID_ANY, "IRQ priority", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_peripheral_settings[SPI]:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_IRQ_priority[SPI] = wx.wxChoice(ui.Panel_settings[SPI], ID.CHOICE_IRQ_PRIORITY[SPI], wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
                ui.FlexGridSizer_peripheral_settings[SPI]:Add(ui.Choice_IRQ_priority[SPI], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

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

                -- add peripheral settings sizer to general settings sizer
                ui.FlexGridSizer_settings[SPI] = wx.wxFlexGridSizer(0, 1, 0, 0)
                ui.FlexGridSizer_settings[SPI]:Add(ui.FlexGridSizer_peripheral_settings[SPI], 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

                -- create a new panel for specified chip select
                ui.Panel_CS[SPI] = wx.wxPanel(ui.Panel_settings[SPI], ID.PANEL_CS[SPI], wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
                ui.FlexGridSizer_CS[SPI] = wx.wxFlexGridSizer(0, 3, 0, 0)

                -- add Chip select pin controls
                ui.StaticText = wx.wxStaticText(ui.Panel_CS[SPI], wx.wxID_ANY, "Pin for Chip Select 0", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_CS[SPI]:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer_CS[SPI]:Add(0,0,1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_CS_pin[SPI] = wx.wxChoice(ui.Panel_CS[SPI], ID.CHOICE_CS_PIN[SPI], wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
                ui.FlexGridSizer_CS[SPI]:Add(ui.Choice_CS_pin[SPI], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- set sizer of chip select panel
                ui.Panel_CS[SPI]:SetSizer(ui.FlexGridSizer_CS[SPI])


                -- add chip select configuration panel to then Chip select settings sizer
                ui.FlexGridSizer_CS_settings[SPI] = wx.wxFlexGridSizer(0, 1, 0, 0)
                ui.FlexGridSizer_CS_settings[SPI]:Add(ui.Panel_CS[SPI], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

                -- add chip select settings sizer to settings sizer
                ui.FlexGridSizer_settings[SPI]:Add(ui.FlexGridSizer_CS_settings[SPI], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

                -- set settings panel sizer
                ui.Panel_settings[SPI]:SetSizer(ui.FlexGridSizer_settings[SPI])

                -- add to the peripheral sizer the settings panel
                ui.FlexGridSizer_peripheral[SPI]:Add(ui.Panel_settings[SPI], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.Panel_peripheral[SPI]:SetSizer(ui.FlexGridSizer_peripheral[SPI])

                -- add peripheral page to the notebook
                ui.Notebook_peripheral:AddPage(ui.Panel_peripheral[SPI], "SPI"..spi_cfg:Children()[1].name:GetValue(), false)
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





--         ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)
--         ui.CheckBox_enable = wx.wxCheckBox(this, ID.CHECKBOX_ENABLE, "Enable module", wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, -1), 0, wx.wxDefaultValidator, "ID.CHECKBOX_ENABLE")
--         ui.FlexGridSizer1:Add(ui.CheckBox_enable, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
--         ui.Panel1 = wx.wxPanel(this, ID.PANEL1, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL, "ID.PANEL1")
--         ui.FlexGridSizer2 = wx.wxFlexGridSizer(0, 1, 0, 0)
--         ui.StaticBoxSizer1 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel1, "Default settings for all peripherals")
--         ui.FlexGridSizer3 = wx.wxFlexGridSizer(0, 2, 0, 0)
--         ui.StaticText1 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Dummy byte", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
--         ui.FlexGridSizer3:Add(ui.StaticText1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
--         ui.TextCtrl_dummy_byte = wx.wxTextCtrl(ui.Panel1, ID.TEXTCTRL_DUMMY_BYTE, "", wx.wxDefaultPosition, wx.wxSize(40,-1), 0, ct.hexvalidator)
--         ui.TextCtrl_dummy_byte:SetMaxLength(2)
--         ui.FlexGridSizer3:Add(ui.TextCtrl_dummy_byte, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
--         ui.StaticText2 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Clock divider", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
--         ui.FlexGridSizer3:Add(ui.StaticText2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
--         ui.Choice_clkdiv = wx.wxChoice(ui.Panel1, ID.CHOICE_CLKDIV, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_CLKDIV")
--         ui.Choice_clkdiv:Append("Peripheral clock /2")
--         ui.Choice_clkdiv:Append("Peripheral clock /4")
--         ui.Choice_clkdiv:Append("Peripheral clock /8")
--         ui.Choice_clkdiv:Append("Peripheral clock /16")
--         ui.Choice_clkdiv:Append("Peripheral clock /32")
--         ui.Choice_clkdiv:Append("Peripheral clock /64")
--         ui.Choice_clkdiv:Append("Peripheral clock /128")
--         ui.Choice_clkdiv:Append("Peripheral clock /256")
--         ui.FlexGridSizer3:Add(ui.Choice_clkdiv, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
--         ui.StaticText3 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "SPI mode", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
--         ui.FlexGridSizer3:Add(ui.StaticText3, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
--         ui.Choice_mode = wx.wxChoice(ui.Panel1, ID.CHOICE_MODE, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_MODE")
--         ui.Choice_mode:Append("Mode 0 (SCK Low at idle; capture on leading edge)")
--         ui.Choice_mode:Append("Mode 1 (SCK Low at idle; capture on trailing edge)")
--         ui.Choice_mode:Append("Mode 2 (SCK High at idle; capture on leading edge)")
--         ui.Choice_mode:Append("Mode 3 (SCK High at idle; capture on trailing edge)")
--         ui.FlexGridSizer3:Add(ui.Choice_mode, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
--         ui.StaticText4 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Bit order", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
--         ui.FlexGridSizer3:Add(ui.StaticText4, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
--         ui.Choice_bitorder = wx.wxChoice(ui.Panel1, ID.CHOICE_BITORDER, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_BITORDER")
--         ui.Choice_bitorder:Append("MSb first")
--         ui.Choice_bitorder:Append("LSb first")
--         ui.FlexGridSizer3:Add(ui.Choice_bitorder, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
--         ui.StaticBoxSizer1:Add(ui.FlexGridSizer3, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
--         ui.FlexGridSizer2:Add(ui.StaticBoxSizer1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
--
--         ui.StaticBoxSizer2 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel1, "Peripheral specific settings")
--         ui.FlexGridSizer4 = wx.wxFlexGridSizer(0, 1, 0, 0)
--
--         ui.Choice_device = wx.wxChoice(ui.Panel1, ID.CHOICE_DEVICE, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_DEVICE")
--         for i = 1, spi_cfg:NumChildren() do
--                 ui.Choice_device:Append("SPI"..spi_cfg:Children()[i].name:GetValue())
--         end
--         ui.Choice_device:SetSelection(0)
--         ui.Choice_device.OldSelection = 0
--         ui.FlexGridSizer4:Add(ui.Choice_device, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
--
--         ui.Panel2 = wx.wxPanel(ui.Panel1, ID.PANEL2, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL, "ID.PANEL2")
--         ui.FlexGridSizer5 = wx.wxFlexGridSizer(0, 1, 0, 0)
--         ui.CheckBox_device_enable = wx.wxCheckBox(ui.Panel1, ID.CHECKBOX_DEVICE_ENABLE, "Enable device", wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, -1), 0, wx.wxDefaultValidator, "ID.CHECKBOX_DEVICE_ENABLE")
--         ui.FlexGridSizer4:Add(ui.CheckBox_device_enable, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
--         ui.FlexGridSizer6 = wx.wxFlexGridSizer(0, 2, 0, 0)
--
--         ui.CheckBox_DMA_enable = wx.wxCheckBox(ui.Panel2, ID.CHECKBOX_DMA_ENABLE, "Enable DMA", wx.wxDefaultPosition, wx.wxDefaultSize)
--         ui.FlexGridSizer6:Add(ui.CheckBox_DMA_enable, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
--         ui.StaticText = wx.wxStaticText(ui.Panel2, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxDefaultSize)
--         ui.FlexGridSizer6:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
--
--         ui.StaticText5 = wx.wxStaticText(ui.Panel2, wx.wxID_ANY, "IRQ priority", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
--         ui.FlexGridSizer6:Add(ui.StaticText5, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
--         ui.Choice_irqprio = wx.wxChoice(ui.Panel2, ID.CHOICE_IRQPRIO, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_IRQPRIO")
--         for i, item in ipairs(prio_list) do
--                 ui.Choice_irqprio:Append(item.name)
--         end
--         ui.Choice_irqprio:Append("System default")
--         ui.FlexGridSizer6:Add(ui.Choice_irqprio, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
--
--         ui.StaticText6 = wx.wxStaticText(ui.Panel2, wx.wxID_ANY, "Number of Chip Selects", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
--         ui.FlexGridSizer6:Add(ui.StaticText6, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
--         ui.Choice_csnum = wx.wxChoice(ui.Panel2, ID.CHOICE_CSNUM, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_CSNUM")
--         ui.Choice_csnum:Append("1 (CS 0 only)")
--         ui.Choice_csnum:Append("2 (CS 0..1)")
--         ui.Choice_csnum:Append("3 (CS 0..2)")
--         ui.Choice_csnum:Append("4 (CS 0..3)")
--         ui.Choice_csnum:Append("5 (CS 0..4)")
--         ui.Choice_csnum:Append("6 (CS 0..5)")
--         ui.Choice_csnum:Append("7 (CS 0..6)")
--         ui.Choice_csnum:Append("8 (CS 0..7)")
--         ui.Choice_csnum.OldSelection = 0
--         ui.FlexGridSizer6:Add(ui.Choice_csnum, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
--
--         for i = 0, NUMBER_OF_CS - 1 do
--                 ui.StaticText_cspin[i] = wx.wxStaticText(ui.Panel2, wx.wxID_ANY, "Pin for Chip Select "..i, wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
--                 ui.FlexGridSizer6:Add(ui.StaticText_cspin[i], 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
--                 ID.CHOICE_CSPIN[i] = wx.wxNewId()
--                 ui.Choice_cspin[i] = wx.wxChoice(ui.Panel2, ID.CHOICE_CSPIN[i], wx.wxDefaultPosition, wx.wxDefaultSize, {"*UNDEFINED*"}, 0, wx.wxDefaultValidator, "ID.CHOICE_CSPIN")
--                 ui.Choice_cspin[i]:Append(pin_list)
--                 ui.FlexGridSizer6:Add(ui.Choice_cspin[i], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
--                 this:Connect(ID.CHOICE_CSPIN[i], wx.wxEVT_COMMAND_CHOICE_SELECTED, value_updated)
--         end
--
--         ui.FlexGridSizer5:Add(ui.FlexGridSizer6, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
--         ui.Panel2:SetSizer(ui.FlexGridSizer5)
--         ui.FlexGridSizer4:Add(ui.Panel2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
--         ui.StaticBoxSizer2:Add(ui.FlexGridSizer4, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
--         ui.FlexGridSizer2:Add(ui.StaticBoxSizer2, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
--         ui.Panel1:SetSizer(ui.FlexGridSizer2)
--         ui.FlexGridSizer2:Fit(ui.Panel1)
--         ui.FlexGridSizer2:SetSizeHints(ui.Panel1)
--         ui.FlexGridSizer1:Add(ui.Panel1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
--         ui.StaticLine1 = wx.wxStaticLine(this, ID.STATICLINE1, wx.wxDefaultPosition, wx.wxSize(10,-1), wx.wxLI_HORIZONTAL, "ID.STATICLINE1")
--         ui.FlexGridSizer1:Add(ui.StaticLine1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
--         ui.Button_save = wx.wxButton(this, ID.BUTTON_SAVE, "Save", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.BUTTON_SAVE")
--         ui.FlexGridSizer1:Add(ui.Button_save, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)

        --
        ui.window:SetSizer(ui.FlexGridSizer1)
        ui.window:SetScrollRate(15, 15)

        --
--         this:Connect(ID.CHECKBOX_ENABLE,        wx.wxEVT_COMMAND_CHECKBOX_CLICKED, checkbox_enable_updated       )
--         this:Connect(ID.CHOICE_CLKDIV,          wx.wxEVT_COMMAND_CHOICE_SELECTED,  value_updated                 )
--         this:Connect(ID.CHOICE_MODE,            wx.wxEVT_COMMAND_CHOICE_SELECTED,  value_updated                 )
--         this:Connect(ID.CHOICE_BITORDER,        wx.wxEVT_COMMAND_CHOICE_SELECTED,  value_updated                 )
--         this:Connect(ID.CHECKBOX_DEVICE_ENABLE, wx.wxEVT_COMMAND_CHECKBOX_CLICKED, checkbox_device_enable_updated)
--         this:Connect(ID.CHECKBOX_DMA_ENABLE,    wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_updated                 )
--         this:Connect(ID.CHOICE_IRQPRIO,         wx.wxEVT_COMMAND_CHOICE_SELECTED,  value_updated                 )
--         this:Connect(ID.CHOICE_DEVICE,          wx.wxEVT_COMMAND_CHOICE_SELECTED,  spi_device_selected           )
--         this:Connect(ID.CHOICE_CSNUM,           wx.wxEVT_COMMAND_CHOICE_SELECTED,  NUMBER_OF_CS_selected         )
--         this:Connect(ID.TEXTCTRL_DUMMY_BYTE,    wx.wxEVT_COMMAND_TEXT_UPDATED,     value_updated                 )
--         this:Connect(ID.BUTTON_SAVE,            wx.wxEVT_COMMAND_BUTTON_CLICKED,   on_button_save_click          )

        --
--         local spisel = spi_cfg:Children()[ui.Choice_device:GetSelection() + 1].name:GetValue()
--         load_controls_of_selected_SPI(spisel)
--         load_controls_of_defaults()
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
--         -- refreshes pin list
--         pin_list = gpio:get_pin_list(true)
--
--         for i = 0, NUMBER_OF_CS - 1 do
--                 ui.Choice_cspin[i]:Clear()
--                 ui.Choice_cspin[i]:Append("*UNDEFINED*")
--                 ui.Choice_cspin[i]:Append(pin_list)
--         end
--
--         local spisel = spi_cfg:Children()[ui.Choice_device:GetSelection() + 1].name:GetValue()
--         load_controls_of_selected_SPI(spisel)
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
        on_button_save_click()
end


--------------------------------------------------------------------------------
-- @brief  Function returns module handler
-- @param  None
-- @return Module handler
--------------------------------------------------------------------------------
function get_handler()
        return spi
end
