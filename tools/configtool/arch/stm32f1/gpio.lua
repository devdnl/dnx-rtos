--[[============================================================================
@file    gpio.lua

@author  Daniel Zorychta

@brief   Configuration script for GPIO module.

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
gpio = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local modified = ct:new_modify_indicator()
local ui = {}
local ID = {}

local port_mode_string = {"Output Push-Pull 2MHz",
                          "Output Push-Pull 10MHz",
                          "Output Push-Pull 50MHz",
                          "Output Open drain 2MHz",
                          "Output Open drain 10MHz",
                          "Output Open drain 50MHz",
                          "Alternative output Push-Pull 2MHz",
                          "Alternative output Push-Pull 10MHz",
                          "Alternative output Push-Pull 50MHz",
                          "Alternative output Open drain 2MHz",
                          "Alternative output Open drain 10MHz",
                          "Alternative output Open drain 50MHz",
                          "Analog",
                          "Float input",
                          "Input pulled"}

local port_mode_index = {["_GPIO_OUT_PUSH_PULL_2MHZ"]       = 0,
                         ["_GPIO_OUT_PUSH_PULL_10MHZ"]      = 1,
                         ["_GPIO_OUT_PUSH_PULL_50MHZ"]      = 2,
                         ["_GPIO_OUT_OPEN_DRAIN_2MHZ"]      = 3,
                         ["_GPIO_OUT_OPEN_DRAIN_10MHZ"]     = 4,
                         ["_GPIO_OUT_OPEN_DRAIN_50MHZ"]     = 5,
                         ["_GPIO_ALT_OUT_PUSH_PULL_2MHZ"]   = 6,
                         ["_GPIO_ALT_OUT_PUSH_PULL_10MHZ"]  = 7,
                         ["_GPIO_ALT_OUT_PUSH_PULL_50MHZ"]  = 8,
                         ["_GPIO_ALT_OUT_OPEN_DRAIN_2MHZ"]  = 9,
                         ["_GPIO_ALT_OUT_OPEN_DRAIN_10MHZ"] = 10,
                         ["_GPIO_ALT_OUT_OPEN_DRAIN_50MHZ"] = 11,
                         ["_GPIO_ANALOG"]                   = 12,
                         ["_GPIO_IN_FLOAT"]                 = 13,
                         ["_GPIO_IN_PULLED"]                = 14}

local port_state_pulled    = {"Down", "Up"}
local port_state_out_pp_od = {"Low", "High"}
local port_state_float     = {"Hi-Z"}

local MAX_NUMBER_OF_PINS   = config.arch.stm32f1.def.GPIO_max_number_of_pins:GetValue()
local MAX_NUMBER_OF_PORTS  = config.arch.stm32f1.def.GPIO_max_number_of_ports:GetValue()


--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Method of port_mode_index table to translate selection index of particular pin mode
-- @param  idx      mode index
-- @return On success correct mode string is returned. On error an empty string is returned.
--------------------------------------------------------------------------------
function port_mode_index.get_mode(self, idx)
        for key, value in pairs(self) do
                if idx == value then
                        return key
                end
        end

        return ""
end

--------------------------------------------------------------------------------
-- @brief  Function load all controls from configuration files
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_configuration()
        for GPIO = 1, gpio_cfg.layout:NumChildren() do
                local GPIO_name = gpio_cfg.layout:Children()[GPIO].name:GetValue()
                local PIN_mask  = tonumber(gpio_cfg.layout:Children()[GPIO].pinmask:GetValue())

                for PIN = 0, MAX_NUMBER_OF_PINS - 1 do
                        if PIN_mask % 2 == 1 then
                                local pin_name  = ct:key_read(config.arch.stm32f1.key["GPIO_P"..GPIO_name.."_PIN_"..PIN.."_NAME"])
                                local pin_mode  = ct:key_read(config.arch.stm32f1.key["GPIO_P"..GPIO_name.."_PIN_"..PIN.."_MODE"])
                                local pin_state = ct:key_read(config.arch.stm32f1.key["GPIO_P"..GPIO_name.."_PIN_"..PIN.."_STATE"])

                                ui.ComboBox_pin_name[GPIO][PIN]:SetValue(pin_name)
                                ui.Choice_pin_mode[GPIO][PIN]:SetSelection(port_mode_index[pin_mode])
                                ui.Notebook_ports:Command(wx.wxCommandEvent(wx.wxEVT_COMMAND_CHOICE_SELECTED, ID.CHOICE_PIN_MODE[GPIO][PIN]))
                                ui.Choice_pin_state[GPIO][PIN]:SetSelection(ifs(pin_state:match("_HIGH"), 1, 0))
                        end

                        PIN_mask = math.floor(PIN_mask / 2)

                        if PIN_mask == 0 then
                                break
                        end
                end
        end

        local module_enable = ct:get_module_state("GPIO")
        ui.CheckBox_module_enable:SetValue(module_enable)
        ui.Panel_module:Enable(module_enable)
end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function save_configuration()
        -- disable all ports
        for GPIO = 1, MAX_NUMBER_OF_PORTS - 1 do
                local GPIO_name = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J"}
                ct:key_write(config.arch.stm32f1.key["GPIO_P"..GPIO_name[GPIO].."_ENABLE"], config.project.def.NO:GetValue())
        end

        -- save pin configuration and enable ports
        for GPIO = 1, gpio_cfg.layout:NumChildren() do
                local GPIO_name = gpio_cfg.layout:Children()[GPIO].name:GetValue()
                local PIN_mask  = tonumber(gpio_cfg.layout:Children()[GPIO].pinmask:GetValue())

                ct:key_write(config.arch.stm32f1.key["GPIO_P"..GPIO_name.."_ENABLE"], config.project.def.YES:GetValue())

                for PIN = 0, MAX_NUMBER_OF_PINS - 1 do
                        if PIN_mask % 2 == 1 then
                                local pin_name  = ui.ComboBox_pin_name[GPIO][PIN]:GetValue()
                                local pin_mode  = port_mode_index:get_mode(ui.Choice_pin_mode[GPIO][PIN]:GetSelection())
                                local pin_state = ui.Choice_pin_state[GPIO][PIN]:GetSelection()

                                if pin_mode:match("FLOAT") or pin_mode:match("ANALOG") then
                                        pin_state = "_FLOAT"
                                else
                                        pin_state = ifs(pin_state > 0, "_HIGH", "_LOW")
                                end

                                ct:key_write(config.arch.stm32f1.key["GPIO_P"..GPIO_name.."_PIN_"..PIN.."_NAME"], pin_name)
                                ct:key_write(config.arch.stm32f1.key["GPIO_P"..GPIO_name.."_PIN_"..PIN.."_MODE"], pin_mode)
                                ct:key_write(config.arch.stm32f1.key["GPIO_P"..GPIO_name.."_PIN_"..PIN.."_STATE"], pin_state)
                        end

                        PIN_mask = math.floor(PIN_mask / 2)

                        if PIN_mask == 0 then
                                break
                        end
                end
        end

        ct:enable_module("GPIO", ui.CheckBox_module_enable:IsChecked())

        modified:no()
end

--==============================================================================
-- GLOBAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function gpio:create_window(parent)
        cpu_name = ct:key_read(config.arch.stm32f1.key.CPU_NAME)
        cpu_idx  = ct:get_cpu_index("stm32f1", cpu_name)
        gpio_cfg = config.arch.stm32f1.cpulist:Children()[cpu_idx].peripherals.GPIO

        ui = {}
        ui.Panel_GPIO         = {}
        ui.FlexGridSizer_GPIO = {}
        ui.ComboBox_pin_name  = {}
        ui.Choice_pin_mode    = {}
        ui.Choice_pin_state   = {}

        ID = {}
        ID.CHECKBOX_MODULE_ENABLE = wx.wxNewId()
        ID.PANEL_MODULE           = wx.wxNewId()
        ID.NOTEBOOK_PORTS         = wx.wxNewId()
        ID.COMBOBOX_PIN_NAME      = {}
        ID.CHOICE_PIN_MODE        = {}
        ID.CHOICE_PIN_STATE       = {}

        -- create window
        ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
        ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)

        -- create module enable checkbox
        ui.CheckBox_module_enable = wx.wxCheckBox(ui.window, ID.CHECKBOX_MODULE_ENABLE, "Enable module", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer1:Add(ui.CheckBox_module_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.window:Connect(ID.CHECKBOX_MODULE_ENABLE, wx.wxEVT_COMMAND_CHECKBOX_CLICKED,
                function(event)
                        ui.Panel_module:Enable(event:IsChecked())
                        modified:yes()
                end
        )

        -- create module's panel
        ui.Panel_module = wx.wxPanel(ui.window, ID.PANEL_MODULE, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_module = wx.wxFlexGridSizer(0, 1, 0, 0)

        -- create notebook for GPIOs
        ui.Notebook_ports = wx.wxNotebook(ui.Panel_module, ID.NOTEBOOK_PORTS, wx.wxDefaultPosition, wx.wxDefaultSize)

        -- add GPIO cards
        ID.PANEL_PORT = {}
        for GPIO = 1, gpio_cfg.layout:NumChildren() do
                local GPIO_name = gpio_cfg.layout:Children()[GPIO].name:GetValue()
                local PIN_mask  = tonumber(gpio_cfg.layout:Children()[GPIO].pinmask:GetValue())

                ID.PANEL_PORT[GPIO] = wx.wxNewId()

                -- create a new panel
                ui.Panel_GPIO[GPIO] = wx.wxPanel(ui.Notebook_ports, ID.PANEL_PORT[GPIO], wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
                ui.FlexGridSizer_GPIO[GPIO] = wx.wxFlexGridSizer(0, 4, 0, 0)

                -- create configuration header
                ui.StaticText = wx.wxStaticText(ui.Panel_GPIO[GPIO], wx.wxID_ANY, "Pin", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_GPIO[GPIO]:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText = wx.wxStaticText(ui.Panel_GPIO[GPIO], wx.wxID_ANY, "Name", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_GPIO[GPIO]:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText = wx.wxStaticText(ui.Panel_GPIO[GPIO], wx.wxID_ANY, "Mode", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_GPIO[GPIO]:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText = wx.wxStaticText(ui.Panel_GPIO[GPIO], wx.wxID_ANY, "State", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_GPIO[GPIO]:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add pins
                ui.ComboBox_pin_name[GPIO] = {}
                ui.Choice_pin_mode[GPIO]   = {}
                ui.Choice_pin_state[GPIO]  = {}
                ID.COMBOBOX_PIN_NAME[GPIO] = {}
                ID.CHOICE_PIN_MODE[GPIO]   = {}
                ID.CHOICE_PIN_STATE[GPIO]  = {}
                for PIN = 0, MAX_NUMBER_OF_PINS - 1 do
                        if PIN_mask % 2 == 1 then
                                ID.COMBOBOX_PIN_NAME[GPIO][PIN] = wx.wxNewId()
                                ID.CHOICE_PIN_MODE[GPIO][PIN]   = wx.wxNewId()
                                ID.CHOICE_PIN_STATE[GPIO][PIN]  = wx.wxNewId()

                                -- add pin number
                                ui.StaticText = wx.wxStaticText(ui.Panel_GPIO[GPIO], wx.wxID_ANY, "P"..GPIO_name..PIN..":", wx.wxDefaultPosition, wx.wxDefaultSize)
                                ui.FlexGridSizer_GPIO[GPIO]:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 1)

                                -- add pin name field
                                ui.ComboBox_pin_name[GPIO][PIN] = wx.wxComboBox(ui.Panel_GPIO[GPIO], ID.COMBOBOX_PIN_NAME[GPIO][PIN], "", wx.wxDefaultPosition, wx.wxDefaultSize, {})
                                ui.ComboBox_pin_name[GPIO][PIN]:SetMinSize(wx.wxSize(180, -1))
                                ui.FlexGridSizer_GPIO[GPIO]:Add(ui.ComboBox_pin_name[GPIO][PIN], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 1)

                                local alt = config.arch.stm32f1.footprint[gpio_cfg.footprint:GetValue()]["P"..GPIO_name..PIN]
                                if alt ~= nil then
                                        for i = 1, alt:NumChildren() do
                                                ui.ComboBox_pin_name[GPIO][PIN]:Append(alt:Children()[i].name:GetValue())
                                        end
                                end

                                ui.window:Connect(ID.COMBOBOX_PIN_NAME[GPIO][PIN], wx.wxEVT_COMMAND_TEXT_UPDATED,
                                        function()
                                                modified:yes()
                                        end
                                )

                                ui.window:Connect(ID.COMBOBOX_PIN_NAME[GPIO][PIN], wx.wxEVT_COMMAND_COMBOBOX_SELECTED,
                                        function(event)
                                                local preset = alt:Children()[event:GetSelection() + 1]
                                                ui.ComboBox_pin_name[GPIO][PIN]:SetValue(preset.name:GetValue())
                                                ui.Choice_pin_mode[GPIO][PIN]:SetSelection(port_mode_index[preset.mode:GetValue()])
                                                ui.Notebook_ports:Command(wx.wxCommandEvent(wx.wxEVT_COMMAND_CHOICE_SELECTED, ID.CHOICE_PIN_MODE[GPIO][PIN]))
                                                ui.Choice_pin_state[GPIO][PIN]:SetSelection(ifs(preset.state:GetValue():match("_HIGH"), 1, 0))
                                                modified:yes()
                                        end
                                )

                                -- add pin mode selection
                                ui.Choice_pin_mode[GPIO][PIN] = wx.wxChoice(ui.Panel_GPIO[GPIO], ID.CHOICE_PIN_MODE[GPIO][PIN], wx.wxDefaultPosition, wx.wxDefaultSize, port_mode_string, 0)
                                ui.FlexGridSizer_GPIO[GPIO]:Add(ui.Choice_pin_mode[GPIO][PIN], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 1)
                                ui.window:Connect(ID.CHOICE_PIN_MODE[GPIO][PIN], wx.wxEVT_COMMAND_CHOICE_SELECTED,
                                        function(event)
                                                ui.Choice_pin_state[GPIO][PIN]:Clear()
                                                ui.Choice_pin_state[GPIO][PIN]:Enable(true)

                                                local mode = ui.Choice_pin_mode[GPIO][PIN]:GetString(ui.Choice_pin_mode[GPIO][PIN]:GetSelection())
                                                if mode:match("Open drain") or mode:match("Push%-Pull") then
                                                        ui.Choice_pin_state[GPIO][PIN]:Append(port_state_out_pp_od)
                                                elseif mode:match("pulled") then
                                                        ui.Choice_pin_state[GPIO][PIN]:Append(port_state_pulled)
                                                else
                                                        ui.Choice_pin_state[GPIO][PIN]:Append(port_state_float)
                                                        ui.Choice_pin_state[GPIO][PIN]:Enable(false)
                                                end

                                                ui.Choice_pin_state[GPIO][PIN]:SetSelection(0)

                                                modified:yes()
                                        end
                                )

                                -- add pin state selection
                                ui.Choice_pin_state[GPIO][PIN] = wx.wxChoice(ui.Panel_GPIO[GPIO], ID.CHOICE_PIN_STATE[GPIO][PIN], wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
                                ui.FlexGridSizer_GPIO[GPIO]:Add(ui.Choice_pin_state[GPIO][PIN], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 1)
                                ui.window:Connect(ID.CHOICE_PIN_STATE[GPIO][PIN], wx.wxEVT_COMMAND_CHOICE_SELECTED,
                                        function (event)
                                                modified:yes()
                                        end
                                )
                        end

                        PIN_mask = math.floor(PIN_mask / 2)

                        if PIN_mask == 0 then
                                break
                        end
                end

                -- set panel's size
                ui.Panel_GPIO[GPIO]:SetSizer(ui.FlexGridSizer_GPIO[GPIO])

                -- add page to notebook
                ui.Notebook_ports:AddPage(ui.Panel_GPIO[GPIO], "GPIO "..GPIO_name, false)
        end

        -- add notebook to the modules's sizer
        ui.FlexGridSizer_module:Add(ui.Notebook_ports, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- set sizer of module's panel
        ui.Panel_module:SetSizer(ui.FlexGridSizer_module)

        -- add module's panel to the main flexsizer
        ui.FlexGridSizer1:Add(ui.Panel_module, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- set main sizer and set scroll rate
        ui.window:SetSizer(ui.FlexGridSizer1)
        ui.window:SetScrollRate(10, 10)

        load_configuration()
        modified:no()

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @param  None
-- @return Module name
--------------------------------------------------------------------------------
function gpio:get_window_name()
        return "GPIO"
end


--------------------------------------------------------------------------------
-- @brief  Function is called by parent when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function gpio:selected()
end


--------------------------------------------------------------------------------
-- @brief  Function returns modify status
-- @param  None
-- @return If data is modified true is returned, otherwise false
--------------------------------------------------------------------------------
function gpio:is_modified()
        return modified:get_value()
end


--------------------------------------------------------------------------------
-- @brief  Function save configuration
-- @return None
--------------------------------------------------------------------------------
function gpio:save()
        save_configuration()
end


--------------------------------------------------------------------------------
-- @brief  Function discard modified configuration
-- @return None
--------------------------------------------------------------------------------
function gpio:discard()
        load_configuration()
        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Function returns module handler
-- @param  None
-- @return Module handler
--------------------------------------------------------------------------------
function get_handler()
        return gpio
end


--==============================================================================
-- Module-specific methods
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function returns list with pin names for selected microcontroller
-- @param  sort_list        list sort enable (bool)
-- @return Pin name list
--------------------------------------------------------------------------------
function gpio:get_pin_list(sort_list)
        local cpu_name = ct:key_read(config.arch.stm32f1.key.CPU_NAME)
        local cpu_idx  = ct:get_cpu_index("stm32f1", cpu_name)
        local gpio_cfg = config.arch.stm32f1.cpulist:Children()[cpu_idx].peripherals.GPIO
        local list     = {}

        for i = 1, gpio_cfg.layout:NumChildren() do
                local port_name = gpio_cfg.layout:Children()[i].name:GetValue()
                local pin_mask  = gpio_cfg.layout:Children()[i].pinmask:GetValue()

                for pin = 0, MAX_NUMBER_OF_PINS - 1 do
                        if bit.band(pin_mask, 1) == 1 then
                                list[#list + 1] = ct:key_read(config.arch.stm32f1.key["GPIO_P"..port_name.."_PIN_"..pin.."_NAME"])
                        end

                        pin_mask = bit.rshift(pin_mask, 1)
                end
        end

        if sort_list then
                table.sort(list)
        end

        return list
end
