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
                          "Analog input",
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
                         ["_GPIO_IN_ANALOG"]                = 12,
                         ["_GPIO_IN_FLOAT"]                 = 13,
                         ["_GPIO_IN_PULLED"]                = 14}

local port_state_in_pulled     = {"Down", "Up"}
local port_state_out_push_pull = {"Low", "High"}
local port_state_float         = {"Hi-Z"}

local MAX_NUMBER_OF_PINS       = config.arch.stm32f1.def.GPIO_max_number_of_pins:GetValue()
local MAX_NUMBER_OF_PORTS      = config.arch.stm32f1.def.GPIO_max_number_of_ports:GetValue()


--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function loads all controls from configuration scripts
-- @param  None
-- @return None
--------------------------------------------------------------------------------
-- local function enable_controls(state)
--         ui.Choice_port:Enable(state)
--         ui.StaticText1:Enable(state)
--         ui.StaticText2:Enable(state)
--         ui.StaticText3:Enable(state)
--         ui.StaticText4:Enable(state)
--
--         for pin = 1, MAX_NUMBER_OF_PINS do
--                 ui.StaticText_pin[pin]:Enable(state)
--                 ui.TextCtrl_pin_name[pin]:Enable(state)
--                 ui.Choice_mode[pin]:Enable(state)
--
--                 if not ui.Choice_state[pin].Inactive then
--                         ui.Choice_state[pin]:Enable(state)
--                 end
--         end
-- end


--------------------------------------------------------------------------------
-- @brief  Method of port_mode_index table to translate selection index to particular pin mode
-- @param  idx      mode index
-- @return On success correct mode string is returned. On error an empty string is returned.
--------------------------------------------------------------------------------
-- function port_mode_index:get_mode(idx)
--         for key, value in pairs(port_mode_index) do
--                 if idx == value then
--                         return key
--                 end
--         end
--
--         return ""
-- end


--------------------------------------------------------------------------------
-- @brief  Function sets pin state string according to selected pin mode
-- @param  pin          pin to update
-- @param  pin_state    pin state to set
-- @return None
--------------------------------------------------------------------------------
-- local function set_pin_state_by_pin_mode(pin, pin_state)
--         local pin_mode = port_mode_index:get_mode(ui.Choice_mode[pin + 1]:GetSelection())
--
--         ui.Choice_state[pin + 1]:Clear()
--         ui.Choice_state[pin + 1]:Enable(true)
--         ui.Choice_state[pin + 1].Inactive = false
--         if pin_mode:match("_GPIO_IN_PULLED") then
--                 ui.Choice_state[pin + 1]:Append(port_state_in_pulled)
--         elseif pin_mode:match("_OUT_OPEN_DRAIN_") then
--                 ui.Choice_state[pin + 1]:Append(port_state_out_push_pull)
--         elseif pin_mode:match("_OUT_PUSH_PULL_") then
--                 ui.Choice_state[pin + 1]:Append(port_state_out_push_pull)
--         else
--                 ui.Choice_state[pin + 1]:Append(port_state_float)
--                 ui.Choice_state[pin + 1]:Enable(false)
--                 ui.Choice_state[pin + 1].Inactive = true
--         end
--
--         if pin_state == "_HIGH" then
--                 ui.Choice_state[pin + 1]:SetSelection(1)
--         else
--                 ui.Choice_state[pin + 1]:SetSelection(0)
--         end
-- end


--------------------------------------------------------------------------------
-- @brief  Function load all controls from configuration files
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_configuration()
--         local port     = ui.Choice_port:GetSelection() + 1
--         local pin_mask = periph:Children()[port].pinmask:GetValue()
--
--         for pin = 0, MAX_NUMBER_OF_PINS - 1 do
--                 if bit.band(pin_mask, 1) == 1 then
--
--                         local pin_key = config.arch.stm32f1.key.GPIO
--
--                         ui.StaticText_pin[pin + 1]:Show()
--
--                         pin_key.key:SetValue("__GPIO_P"..periph:Children()[port].name:GetValue().."_PIN_"..pin.."_NAME__")
--                         local pin_name = ct:key_read(pin_key)
--                         ui.TextCtrl_pin_name[pin + 1]:SetValue(pin_name)
--                         ui.TextCtrl_pin_name[pin + 1]:Show(true)
--
--                         pin_key.key:SetValue("__GPIO_P"..periph:Children()[port].name:GetValue().."_PIN_"..pin.."_MODE__")
--                         local pin_mode = ct:key_read(pin_key)
--                         ui.Choice_mode[pin + 1]:SetSelection(port_mode_index[pin_mode])
--                         ui.Choice_mode[pin + 1]:Show(true)
--
--                         pin_key.key:SetValue("__GPIO_P"..periph:Children()[port].name:GetValue().."_PIN_"..pin.."_STATE__")
--                         local pin_state = ct:key_read(pin_key)
--                         ui.Choice_state[pin + 1]:Show()
--                         set_pin_state_by_pin_mode(pin, pin_state)
--                 else
--                         ui.StaticText_pin[pin + 1]:Hide()
--                         ui.TextCtrl_pin_name[pin + 1]:Hide()
--                         ui.Choice_mode[pin + 1]:Hide()
--                         ui.Choice_state[pin + 1]:Hide()
--                 end
--
--                 pin_mask = bit.rshift(pin_mask, 1)
--         end
--
--         ui.window:FitInside()
--
--         local gpio_enabled = ct:get_module_state("GPIO")
--         ui.CheckBox_enable:SetValue(gpio_enabled)
--         enable_controls(gpio_enabled)
end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function save_configuration()
--         local port_to_save = periph:Children()[ui.Choice_port:GetSelection() + 1].name:GetValue()
--         local pin_mask     = periph:Children()[ui.Choice_port:GetSelection() + 1].pinmask:GetValue()
--         local key          = config.arch.stm32f1.key.GPIO
--
--         -- save pins configuration for selected port
--         for pin = 0, MAX_NUMBER_OF_PINS - 1 do
--                 local pin_name
--                 local pin_mode
--                 local pin_state
--
--                 if bit.band(pin_mask, 1) == 1 then
--                         pin_name = ui.TextCtrl_pin_name[pin + 1]:GetValue()
--
--                         pin_mode = port_mode_index:get_mode(ui.Choice_mode[pin + 1]:GetSelection())
--
--                         if pin_mode == "_GPIO_IN_ANALOG" or pin_mode == "_GPIO_IN_FLOAT" then
--                                 pin_state = "_FLOAT"
--                         else
--                                 pin_state = ifs(ui.Choice_state[pin + 1]:GetSelection() == 0, "_LOW", "_HIGH")
--                         end
--                 else
--                         pin_name  = "NC_GPIO"..port_to_save.."_"..pin
--                         pin_mode  = port_mode_index:get_mode(13)
--                         pin_state = "_FLOAT"
--                 end
--
--                 -- save pin settings
--                 key.key:SetValue("__GPIO_P"..port_to_save.."_PIN_"..pin.."_NAME__")
--                 ct:key_write(key, pin_name)
--
--                 key.key:SetValue("__GPIO_P"..port_to_save.."_PIN_"..pin.."_MODE__")
--                 ct:key_write(key, pin_mode)
--
--                 key.key:SetValue("__GPIO_P"..port_to_save.."_PIN_"..pin.."_STATE__")
--                 ct:key_write(key, pin_state)
--
--                 pin_mask = bit.rshift(pin_mask, 1)
--         end
--
--         -- enable used ports by specified microcontroller
--         for i, port_name in ipairs(port_names) do
--                 local port_found = false
--                 for i = 1, #port_names do
--                         if periph:Children()[i] then
--                                 if periph:Children()[i].name:GetValue() == port_name then
--                                         port_found = true
--                                         break
--                                 end
--                         end
--                 end
--
--                 if port_found then
--                         key.key:SetValue("__GPIO_P"..port_name.."_ENABLE__")
--                         ct:key_write(key, config.project.def.YES:GetValue())
--                 else
--                         key.key:SetValue("__GPIO_P"..port_name.."_ENABLE__")
--                         ct:key_write(key, config.project.def.NO:GetValue())
--                 end
--         end
--
--         -- module enable
--         ct:enable_module("GPIO", ui.CheckBox_enable:IsChecked())

        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Event is called when module enable checkbox is changed
-- @param  this         event object
-- @return None
--------------------------------------------------------------------------------
-- local function checkbox_changed(this)
--         ui.Choice_port:Enable(this:IsChecked())
--         enable_controls(this:IsChecked())
--         modified:yes()
-- end


--------------------------------------------------------------------------------
-- @brief  Event is called when port number is changed
-- @param  this         event object
-- @return None
--------------------------------------------------------------------------------
-- local function port_number_changed(this)
--         if ui.Choice_port:GetSelection() == ui.Choice_port.OldSelection then
--                 return
--         end
--
--         local answer = wx.wxID_NO
--         if modified then
--                 answer = ct:show_question_msg(ct.MAIN_WINDOW_NAME, "Do you want to save changes?", bit.bor(wx.wxYES_NO, wx.wxCANCEL))
--         end
--
--         if answer == wx.wxID_YES then
--                 save_configuration()
--                 ui.Choice_port.OldSelection = ui.Choice_port:GetSelection()
--         elseif answer == wx.wxID_NO then
--                 ui.Choice_port.OldSelection = ui.Choice_port:GetSelection()
--         elseif answer == wx.wxID_CANCEL then
--                 ui.Choice_port:SetSelection(ui.Choice_port.OldSelection)
--                 return
--         end
--
--         ui.window:Freeze()
--         load_configuration()
--         ui.window:Thaw()
--
--         if answer == wx.wxID_YES or answer == wx.wxID_NO then
--                 modified:no()
--         end
-- end


--------------------------------------------------------------------------------
-- @brief  Event is called when pin name is changed
-- @param  None
-- @return None
--------------------------------------------------------------------------------
-- local function pin_name_updated()
--         modified:yes()
-- end


--------------------------------------------------------------------------------
-- @brief  Event is called when pin state is changed
-- @param  None
-- @return None
--------------------------------------------------------------------------------
-- local function pin_state_updated()
--         modified:yes()
-- end


--------------------------------------------------------------------------------
-- @brief  Event is called when port mode is changed
-- @param  None
-- @return None
--------------------------------------------------------------------------------
-- local port_mode_changed = {}
-- for i = 1, MAX_NUMBER_OF_PINS do port_mode_changed[i] = function() set_pin_state_by_pin_mode(i - 1) modified:yes() end end
--

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
        ui.Choice_preset      = {}
        ui.TextCtrl_pin_name  = {}
        ui.Choice_mode        = {}
        ui.Choice_state       = {}

        ID = {}
        ID.CHECKBOX_MODULE_ENABLE = wx.wxNewId()
        ID.PANEL_MODULE           = wx.wxNewId()
        ID.NOTEBOOK_PORTS         = wx.wxNewId()
        ID.TEXTCTRL_PIN_NAME      = {}
        ID.CHOICE_PRESET          = {}
        ID.CHOICE_MODE            = {}
        ID.CHOICE_STATE           = {}

        -- create window
        ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
        ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)

        -- create module enable checkbox
        ui.CheckBox_module_enable = wx.wxCheckBox(ui.window, ID.CHECKBOX_MODULE_ENABLE, "Module enable", wx.wxDefaultPosition, wx.wxDefaultSize)
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
        for GPIO = 1, gpio_cfg:NumChildren() do
                local GPIO_name = gpio_cfg:Children()[GPIO].name:GetValue()
                local PIN_mask  = tonumber(gpio_cfg:Children()[GPIO].pinmask:GetValue())

                ID.PANEL_PORT[GPIO] = wx.wxNewId()

                -- create a new panel
                ui.Panel_GPIO[GPIO] = wx.wxPanel(ui.Notebook_ports, ID.PANEL_PORT[GPIO], wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
                ui.FlexGridSizer_GPIO[GPIO] = wx.wxFlexGridSizer(0, 5, 0, 0)

                -- create configuration header
                ui.StaticText = wx.wxStaticText(ui.Panel_GPIO[GPIO], wx.wxID_ANY, "Pin", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_GPIO[GPIO]:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText = wx.wxStaticText(ui.Panel_GPIO[GPIO], wx.wxID_ANY, "Preset", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_GPIO[GPIO]:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText = wx.wxStaticText(ui.Panel_GPIO[GPIO], wx.wxID_ANY, "Name", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_GPIO[GPIO]:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText = wx.wxStaticText(ui.Panel_GPIO[GPIO], wx.wxID_ANY, "Mode", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_GPIO[GPIO]:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText = wx.wxStaticText(ui.Panel_GPIO[GPIO], wx.wxID_ANY, "State", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_GPIO[GPIO]:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add pins
                ui.Choice_preset[GPIO]     = {}
                ui.TextCtrl_pin_name[GPIO] = {}
                ui.Choice_mode[GPIO]       = {}
                ui.Choice_state[GPIO]      = {}
                ID.TEXTCTRL_PIN_NAME[GPIO] = {}
                ID.CHOICE_PRESET[GPIO]     = {}
                ID.CHOICE_MODE[GPIO]       = {}
                ID.CHOICE_STATE[GPIO]      = {}
                for PIN = 0, MAX_NUMBER_OF_PINS - 1 do
                        if PIN_mask % 2 == 1 then
                                ID.TEXTCTRL_PIN_NAME[GPIO][PIN] = wx.wxNewId()
                                ID.CHOICE_PRESET[GPIO][PIN]     = wx.wxNewId()
                                ID.CHOICE_MODE[GPIO][PIN]       = wx.wxNewId()
                                ID.CHOICE_STATE[GPIO][PIN]      = wx.wxNewId()

                                -- add pin number
                                ui.StaticText = wx.wxStaticText(ui.Panel_GPIO[GPIO], wx.wxID_ANY, "P"..GPIO_name..PIN..":", wx.wxDefaultPosition, wx.wxDefaultSize)
                                ui.FlexGridSizer_GPIO[GPIO]:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 1)

                                -- add preset options
                                ui.Choice_preset[GPIO][PIN] = wx.wxChoice(ui.Panel_GPIO[GPIO], ID.CHOICE_PRESET[GPIO][PIN], wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
                                ui.Choice_preset[GPIO][PIN]:Append("")
                                ui.FlexGridSizer_GPIO[GPIO]:Add(ui.Choice_preset[GPIO][PIN], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 1)

                                -- add pin name field
                                ui.TextCtrl_pin_name[GPIO][PIN] = wx.wxTextCtrl(ui.Panel_GPIO[GPIO], ID.TEXTCTRL_PIN_NAME[GPIO][PIN], "", wx.wxDefaultPosition, wx.wxDefaultSize)
                                ui.FlexGridSizer_GPIO[GPIO]:Add(ui.TextCtrl_pin_name[GPIO][PIN], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 1)

                                ui.window:Connect(ID.TEXTCTRL_PIN_NAME[GPIO][PIN], wx.wxEVT_COMMAND_TEXT_UPDATED,
                                        function()
                                                modified:yes()

--                                                 ui.Choice_mode[GPIO][PIN]:SetSelection(4)
--                                                 ui.Notebook_ports:Command(wx.wxCommandEvent(wx.wxEVT_COMMAND_CHOICE_SELECTED, ID.CHOICE_MODE[GPIO][PIN]))
                                        end
                                )

                                -- add pin mode selection
                                ui.Choice_mode[GPIO][PIN] = wx.wxChoice(ui.Panel_GPIO[GPIO], ID.CHOICE_MODE[GPIO][PIN], wx.wxDefaultPosition, wx.wxDefaultSize, port_mode_string, 0)
                                ui.FlexGridSizer_GPIO[GPIO]:Add(ui.Choice_mode[GPIO][PIN], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 1)
                                ui.window:Connect(ID.CHOICE_MODE[GPIO][PIN], wx.wxEVT_COMMAND_CHOICE_SELECTED,
                                        function(event)
                                                ui.Choice_state[GPIO][PIN]:Clear()
                                                ui.Choice_state[GPIO][PIN]:Enable(true)

                                                local mode = ui.Choice_mode[GPIO][PIN]:GetString(ui.Choice_mode[GPIO][PIN]:GetSelection())
                                                if mode:match("Push%-Pull") then
                                                        ui.Choice_state[GPIO][PIN]:Append(port_state_out_push_pull)
                                                elseif mode:match("Open drain") or mode:match("pulled") then
                                                        ui.Choice_state[GPIO][PIN]:Append(port_state_in_pulled)
                                                else
                                                        ui.Choice_state[GPIO][PIN]:Append(port_state_float)
                                                        ui.Choice_state[GPIO][PIN]:Enable(false)
                                                end

                                                ui.Choice_state[GPIO][PIN]:SetSelection(0)

                                                modified:yes()
                                        end
                                )

                                -- add pin state selection
                                ui.Choice_state[GPIO][PIN] = wx.wxChoice(ui.Panel_GPIO[GPIO], ID.CHOICE_STATE[GPIO][PIN], wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
                                ui.FlexGridSizer_GPIO[GPIO]:Add(ui.Choice_state[GPIO][PIN], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 1)
                                ui.window:Connect(ID.CHOICE_STATE[GPIO][PIN], wx.wxEVT_COMMAND_CHOICE_SELECTED,
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
        ui.window:SetScrollRate(25, 25)

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
        local periph   = config.arch.stm32f1.cpulist:Children()[cpu_idx].peripherals.GPIO
        local list     = {}

        for i = 1, periph:NumChildren() do
                local port_name = periph:Children()[i].name:GetValue()
                local pin_mask  = periph:Children()[i].pinmask:GetValue()

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
