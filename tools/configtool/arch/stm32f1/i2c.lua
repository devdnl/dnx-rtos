--[[============================================================================
@file    i2c.lua

@author  Daniel Zorychta

@brief   Configuration script for I2C module.

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
i2c = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local ui = {}
local ID = {}
local prio_list = ct:get_priority_list("stm32f1")
local MAX_NUMBER_OF_PERIPHERALS = 2
local MAX_NUMBER_OF_DEVICES = 8

--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function loads all controls from configuration scripts
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_configuration(I2C)
--         local module_enabled = ct:get_module_state("I2C")
--         ui.CheckBox_enable:SetValue(module_enabled)
--
--         local I2C        = ui.Choice_I2C:GetSelection() + 1
--         local I2C_enable = ct:yes_no_to_bool(ct:key_read(config.arch.stm32f1.key["I2C"..I2C.."_ENABLE"]))
--         local use_DMA    = ct:yes_no_to_bool(ct:key_read(config.arch.stm32f1.key["I2C"..I2C.."_USE_DMA"]))
--         local IRQ_prio   = ct:key_read(config.arch.stm32f1.key["I2C"..I2C.."_IRQ_PRIO"])
--         local SCL_freq   = tonumber(ct:key_read(config.arch.stm32f1.key["I2C"..I2C.."_SCL_FREQ"]))/1000
--         local no_of_dev  = tonumber(ct:key_read(config.arch.stm32f1.key["I2C"..I2C.."_NUMBER_OF_DEVICES"]))
--
--         ui.CheckBox_I2C_enable:SetValue(I2C_enable)
--         ui.CheckBox_use_DMA:SetValue(use_DMA)
--         ui.SpinCtrl_SCL_freq:SetValue(SCL_freq)
--         ui.Choice_dev_no:SetSelection(no_of_dev - 1)
--
--         if IRQ_prio == config.project.def.DEFAULT_IRQ_PRIORITY:GetValue() then
--                 IRQ_prio = #prio_list
--         else
--                 IRQ_prio = math.floor(tonumber(IRQ_prio) / 16)
--         end
--         ui.Choice_IRQ_priority:SetSelection(IRQ_prio)
--
--         for dev = 0, MAX_NUMBER_OF_DEVICES - 1 do
--                 local address       = ct:key_read(config.arch.stm32f1.key["I2C"..I2C.."_DEV_"..dev.."_ADDRESS"]):gsub("0x", "")
--                 local addr10bit     = ct:yes_no_to_bool(ct:key_read(config.arch.stm32f1.key["I2C"..I2C.."_DEV_"..dev.."_10BIT_ADDR"]))
--                 local sub_addr_mode = tonumber(ct:key_read(config.arch.stm32f1.key["I2C"..I2C.."_DEV_"..dev.."_SEND_SUB_ADDR"]))
--
--                 ui.Panel_device[dev]:Enable(dev < no_of_dev)
--                 ui.TextCtrl_address[dev]:SetValue(address)
--                 ui.Choice_addr_mode[dev]:SetSelection(ifs(addr10bit, 1, 0))
--                 ui.Choice_subaddr_mode[dev]:SetSelection(sub_addr_mode)
--         end
--
--         ui.Panel_peripheral:Enable(I2C_enable)
--         ui.Panel_module:Enable(module_enabled)
end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function save_configuration()
--         local I2C = ui.Choice_I2C:GetSelection() + 1
--
--
--         ct:enable_module("I2C", ui.CheckBox_enable:GetValue())
--
--
--         -- enable existing I2C port and disable all not existing in this microcontroller
--         for i = 1, MAX_NUMBER_OF_PERIPHERALS do
--                 local exist = false
--                 for j = 1, i2c_cfg:NumChildren() do
--                         if tostring(i) == i2c_cfg:Children()[j].name:GetValue() then
--                                 exist = true
--                                 break
--                         end
--                 end
--
--                 if not exist then
--                         ct:key_write(config.arch.stm32f1.key["I2C"..i.."_ENABLE"], config.project.def.NO:GetValue())
--                 else
--                         local I2C_enable = ct:bool_to_yes_no(ui.CheckBox_I2C_enable:IsChecked())
--                         ct:key_write(config.arch.stm32f1.key["I2C"..I2C.."_ENABLE"], I2C_enable)
--                 end
--         end
--
--
--         local use_DMA = ct:bool_to_yes_no(ui.CheckBox_use_DMA:IsChecked())
--         ct:key_write(config.arch.stm32f1.key["I2C"..I2C.."_USE_DMA"], use_DMA)
--
--
--         local IRQ_prio = ui.Choice_IRQ_priority:GetSelection() + 1
--         if IRQ_prio > #prio_list then
--                 IRQ_prio = config.project.def.DEFAULT_IRQ_PRIORITY:GetValue()
--         else
--                 IRQ_prio = prio_list[IRQ_prio].value
--         end
--         ct:key_write(config.arch.stm32f1.key["I2C"..I2C.."_IRQ_PRIO"], IRQ_prio)
--
--
--         local SCL_freq = tostring(ui.SpinCtrl_SCL_freq:GetValue() * 1000)
--         ct:key_write(config.arch.stm32f1.key["I2C"..I2C.."_SCL_FREQ"], SCL_freq)
--
--
--         local no_of_dev = tostring(ui.Choice_dev_no:GetSelection() + 1)
--         ct:key_write(config.arch.stm32f1.key["I2C"..I2C.."_NUMBER_OF_DEVICES"], no_of_dev)
--
--
--         for dev = 0, MAX_NUMBER_OF_DEVICES - 1 do
--                 local address       = "0x"..ui.TextCtrl_address[dev]:GetValue()
--                 local addr10bit     = ct:bool_to_yes_no(ui.Choice_addr_mode[dev]:GetSelection() > 0)
--                 local sub_addr_mode = tostring(ui.Choice_subaddr_mode[dev]:GetSelection())
--
--                 ct:key_write(config.arch.stm32f1.key["I2C"..I2C.."_DEV_"..dev.."_ADDRESS"], address)
--                 ct:key_write(config.arch.stm32f1.key["I2C"..I2C.."_DEV_"..dev.."_10BIT_ADDR"], addr10bit)
--                 ct:key_write(config.arch.stm32f1.key["I2C"..I2C.."_DEV_"..dev.."_SEND_SUB_ADDR"], sub_addr_mode)
--         end


        ui.Button_save:Enable(false)
end


--==============================================================================
-- GLOBAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function i2c:create_window(parent)
        cpu_name = ct:key_read(config.arch.stm32f1.key.CPU_NAME)
        cpu_idx  = ct:get_cpu_index("stm32f1", cpu_name)
        i2c_cfg  = config.arch.stm32f1.cpulist:Children()[cpu_idx].peripherals.I2C

        ui                            = {}
        ui.Panel_I2C                  = {}
        ui.FlexGridSizer_I2C          = {}
        ui.CheckBox_enable_i2c        = {}
        ui.Panel_I2C_settings         = {}
        ui.FlexGridSizer_I2C_entire   = {}
        ui.FlexGridSizer_I2C_settings = {}
        ui.CheckBox_use_DMA           = {}
        ui.Choice_IRQ_priority        = {}
        ui.SpinCtrl_SCL_frequency     = {}
        ui.Choice_number_of_devices   = {}
        ui.Panel_I2C_device           = {}
        ui.FlexGridSizer_I2C_device   = {}
        ui.TextCtrl_device_address    = {}
        ui.Choice_address_mode        = {}
        ui.Choice_subaddress_mode     = {}

        ID                          = {}
        ID.CHECKBOX_MODULE_ENABLE   = wx.wxNewId()
        ID.PANEL_MODULE             = wx.wxNewId()
        ID.NOTEBOOK1                = wx.wxNewId()
        ID.BUTTON_SAVE              = wx.wxNewId()
        ID.PANEL_I2C                = {}
        ID.CHECKBOX_ENABLE_I2C      = {}
        ID.PANEL_I2C_SETTINGS       = {}
        ID.CHECKBOX_USE_DMA         = {}
        ID.CHOICE_IRQ_PRIORITY      = {}
        ID.SPINCTRL_SCL_FREQUENCY   = {}
        ID.CHOICE_NUMBER_OF_DEVICES = {}
        ID.PANEL_I2C_DEVICE         = {}
        ID.TEXTCTRL_DEVICE_ADDRESS  = {}
        ID.CHOICE_ADDRESS_MODE      = {}
        ID.CHOICE_SUBADDRESS_MODE   = {}

        -- main window
        ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
        local this = ui.window

        -- main sizer and module enable
        ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.CheckBox_module_enable = wx.wxCheckBox(this, ID.CHECKBOX_MODULE_ENABLE, "Module enable", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer1:Add(ui.CheckBox_module_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        this:Connect(ID.CHECKBOX_MODULE_ENABLE, wx.wxEVT_COMMAND_CHECKBOX_CLICKED,
                function(event)
                        ui.Panel_module:Enable(event:IsChecked())
                        ui.Button_save:Enable(true)
                end
        )

        -- panel used to disable entire module
        ui.Panel_module = wx.wxPanel(this, ID.PANEL_MODULE, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_module = wx.wxFlexGridSizer(0, 1, 0, 0)

        -- peripheral notebook
        ui.Notebook_peripheral = wx.wxNotebook(ui.Panel_module, ID.NOTEBOOK1, wx.wxDefaultPosition, wx.wxDefaultSize)

        -- add devices to notebook
        for I2C = 1, i2c_cfg:NumChildren() do
                -- create new ID for controls
                ID.PANEL_I2C[I2C]                = wx.wxNewId()
                ID.CHECKBOX_ENABLE_I2C[I2C]      = wx.wxNewId()
                ID.PANEL_I2C_SETTINGS[I2C]       = wx.wxNewId()
                ID.CHECKBOX_USE_DMA[I2C]         = wx.wxNewId()
                ID.CHOICE_IRQ_PRIORITY[I2C]      = wx.wxNewId()
                ID.SPINCTRL_SCL_FREQUENCY[I2C]   = wx.wxNewId()
                ID.CHOICE_NUMBER_OF_DEVICES[I2C] = wx.wxNewId()


                -- add peripheral's main panel
                ui.Panel_I2C[I2C] = wx.wxPanel(ui.Notebook_peripheral, ID.PANEL_I2C[I2C], wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
                ui.FlexGridSizer_I2C[I2C] = wx.wxFlexGridSizer(0, 1, 0, 0)

                -- add I2C enable checkbox
                ui.CheckBox_enable_i2c[I2C] = wx.wxCheckBox(ui.Panel_I2C[I2C], ID.CHECKBOX_ENABLE_I2C[I2C], "Enable peripheral", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_I2C[I2C]:Add(ui.CheckBox_enable_i2c[I2C], 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                this:Connect(ID.CHECKBOX_ENABLE_I2C[I2C], wx.wxEVT_COMMAND_CHECKBOX_CLICKED,
                        function(event)
                                ui.Panel_I2C_settings[I2C]:Enable(event:IsChecked())
                                ui.Button_save:Enable(true)
                        end
                )

                -- add I2C settings panel
                ui.Panel_I2C_settings[I2C] = wx.wxPanel(ui.Panel_I2C[I2C], ID.PANEL_I2C_SETTINGS[I2C], wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)

                -- add flex sizer to entire I2C configuration
                ui.FlexGridSizer_I2C_entire[I2C] = wx.wxFlexGridSizer(0, 1, 0, 0)

                -- add flex sizer to settings only
                ui.FlexGridSizer_I2C_settings[I2C] = wx.wxFlexGridSizer(0, 2, 0, 0)

                -- add Use DMA checkbox
                ui.CheckBox_use_DMA[I2C] = wx.wxCheckBox(ui.Panel_I2C_settings[I2C], ID.CHECKBOX_USE_DMA[I2C], "Use DMA", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_I2C_settings[I2C]:Add(ui.CheckBox_use_DMA[I2C], 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer_I2C_settings[I2C]:Add(0,0,1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                this:Connect(ID.CHECKBOX_USE_DMA[I2C], wx.wxEVT_COMMAND_CHECKBOX_CLICKED,
                        function(event)
                                ui.Button_save:Enable(true)
                        end
                )

                -- add IRQ priority label and choice
                ui.StaticText = wx.wxStaticText(ui.Panel_I2C_settings[I2C], wx.wxID_ANY, "IRQ priority", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_I2C_settings[I2C]:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_IRQ_priority[I2C] = wx.wxChoice(ui.Panel_I2C_settings[I2C], ID.CHOICE_IRQ_PRIORITY[I2C], wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
                for _, priority in ipairs(prio_list) do
                        ui.Choice_IRQ_priority[I2C]:Append(priority.name)
                end
                ui.Choice_IRQ_priority[I2C]:Append("System default")
                ui.FlexGridSizer_I2C_settings[I2C]:Add(ui.Choice_IRQ_priority[I2C], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                this:Connect(ID.CHOICE_IRQ_PRIORITY[I2C], wx.wxEVT_COMMAND_CHOICE_SELECTED,
                        function(event)
                                ui.Button_save:Enable(true)
                        end
                )

                -- add SCL frequency label and spinbox
                ui.StaticText = wx.wxStaticText(ui.Panel_I2C_settings[I2C], wx.wxID_ANY, "SCL frequency [kHz]", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_I2C_settings[I2C]:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.SpinCtrl_SCL_frequency[I2C] = wx.wxSpinCtrl(ui.Panel_I2C_settings[I2C], ID.SPINCTRL_SCL_FREQUENCY[I2C], "100", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 10, 400, 100)
                ui.FlexGridSizer_I2C_settings[I2C]:Add(ui.SpinCtrl_SCL_frequency[I2C], 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL,wx.wxEXPAND), 5)
                this:Connect(ID.SPINCTRL_SCL_FREQUENCY[I2C], wx.wxEVT_COMMAND_SPINCTRL_UPDATED,
                        function(event)
                                ui.Button_save:Enable(true)
                        end
                )

                -- add Number of devices label and choice
                ui.StaticText = wx.wxStaticText(ui.Panel_I2C_settings[I2C], wx.wxID_ANY, "Number of devices", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_I2C_settings[I2C]:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_number_of_devices[I2C] = wx.wxChoice(ui.Panel_I2C_settings[I2C], ID.CHOICE_NUMBER_OF_DEVICES[I2C], wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
                for n = 1, MAX_NUMBER_OF_DEVICES do
                        ui.Choice_number_of_devices[I2C]:Append(tostring(n))
                end
                ui.FlexGridSizer_I2C_settings[I2C]:Add(ui.Choice_number_of_devices[I2C], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                this:Connect(ID.CHOICE_NUMBER_OF_DEVICES[I2C], wx.wxEVT_COMMAND_CHOICE_SELECTED,
                        function(event)
                                local dev_number = event:GetSelection()

                                for i = 0, MAX_NUMBER_OF_DEVICES - 1 do
                                        ui.Panel_I2C_device[I2C][i]:Enable(i <= dev_number)
                                end

                                ui.Button_save:Enable(true)
                        end
                )

                -- add flex size to entire sizer
                ui.FlexGridSizer_I2C_entire[I2C]:Add(ui.FlexGridSizer_I2C_settings[I2C], 0, bit.bor(wx.wxALIGN_LEFT,wx.wxALIGN_TOP), 0)


                -- add controls for all devices
                ui.Panel_I2C_device[I2C]         = {}
                ui.FlexGridSizer_I2C_device[I2C] = {}
                ui.TextCtrl_device_address[I2C]  = {}
                ui.Choice_address_mode[I2C]      = {}
                ui.Choice_subaddress_mode[I2C]   = {}
                ID.PANEL_I2C_DEVICE[I2C]         = {}
                ID.TEXTCTRL_DEVICE_ADDRESS[I2C]  = {}
                ID.CHOICE_ADDRESS_MODE[I2C]      = {}
                ID.CHOICE_SUBADDRESS_MODE[I2C]   = {}

                for DEV = 0, MAX_NUMBER_OF_DEVICES - 1 do

                        ID.PANEL_I2C_DEVICE[I2C][DEV]        = wx.wxNewId()
                        ID.TEXTCTRL_DEVICE_ADDRESS[I2C][DEV] = wx.wxNewId()
                        ID.CHOICE_ADDRESS_MODE[I2C][DEV]     = wx.wxNewId()
                        ID.CHOICE_SUBADDRESS_MODE[I2C][DEV]  = wx.wxNewId()

                        -- add I2C device panel
                        ui.Panel_I2C_device[I2C][DEV] = wx.wxPanel(ui.Panel_I2C_settings[I2C], ID.PANEL_I2C_DEVICE[I2C][DEV], wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
                        ui.FlexGridSizer_I2C_device[I2C][DEV] = wx.wxFlexGridSizer(0, 4, 0, 0)

                        -- add device number and device address textctrl
                        ui.StaticText = wx.wxStaticText(ui.Panel_I2C_device[I2C][DEV], wx.wxID_ANY, "Device "..DEV..": 0x", wx.wxDefaultPosition, wx.wxDefaultSize, 0)
                        ui.FlexGridSizer_I2C_device[I2C][DEV]:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                        ui.TextCtrl_device_address[I2C][DEV] = wx.wxTextCtrl(ui.Panel_I2C_device[I2C][DEV], ID.TEXTCTRL_DEVICE_ADDRESS[I2C][DEV], "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, ct.hexvalidator)
                        ui.TextCtrl_device_address[I2C][DEV]:SetMaxLength(3)
                        ui.FlexGridSizer_I2C_device[I2C][DEV]:Add(ui.TextCtrl_device_address[I2C][DEV], 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 0)
                        this:Connect(ID.TEXTCTRL_DEVICE_ADDRESS[I2C][DEV], wx.wxEVT_COMMAND_TEXT_UPDATED,
                                function(event)
                                        ui.Button_save:Enable(true)
                                end
                        )

                        -- add address mode
                        ui.Choice_address_mode[I2C][DEV] = wx.wxChoice(ui.Panel_I2C_device[I2C][DEV], ID.CHOICE_ADDRESS_MODE[I2C][DEV], wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
                        ui.Choice_address_mode[I2C][DEV]:Append("7-bit address")
                        ui.Choice_address_mode[I2C][DEV]:Append("10-bit address")
                        ui.FlexGridSizer_I2C_device[I2C][DEV]:Add(ui.Choice_address_mode[I2C][DEV], 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                        this:Connect(ID.CHOICE_ADDRESS_MODE[I2C][DEV], wx.wxEVT_COMMAND_CHOICE_SELECTED,
                                function(event)
                                        ui.Button_save:Enable(true)
                                end
                        )

                        -- add sub address mode
                        ui.Choice_subaddress_mode[I2C][DEV] = wx.wxChoice(ui.Panel_I2C_device[I2C][DEV], ID.CHOICE_SUBADDRESS_MODE[I2C][DEV], wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0)
                        ui.Choice_subaddress_mode[I2C][DEV]:Append("No sub-address")
                        ui.Choice_subaddress_mode[I2C][DEV]:Append("1 byte sub-address")
                        ui.Choice_subaddress_mode[I2C][DEV]:Append("2 bytes sub-address")
                        ui.Choice_subaddress_mode[I2C][DEV]:Append("3 bytes sub-address")
                        ui.FlexGridSizer_I2C_device[I2C][DEV]:Add(ui.Choice_subaddress_mode[I2C][DEV], 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                        this:Connect(ID.CHOICE_SUBADDRESS_MODE[I2C][DEV], wx.wxEVT_COMMAND_CHOICE_SELECTED,
                                function(event)
                                        ui.Button_save:Enable(true)
                                end
                        )

                        -- set controls sizer
                        ui.Panel_I2C_device[I2C][DEV]:SetSizer(ui.FlexGridSizer_I2C_device[I2C][DEV])
                        ui.FlexGridSizer_I2C_entire[I2C]:Add(ui.Panel_I2C_device[I2C][DEV], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                end

                -- add device panel to main sizer
                ui.Panel_I2C_settings[I2C]:SetSizer(ui.FlexGridSizer_I2C_entire[I2C])
                ui.FlexGridSizer_I2C[I2C]:Add(ui.Panel_I2C_settings[I2C], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.Panel_I2C[I2C]:SetSizer(ui.FlexGridSizer_I2C[I2C])

                -- add page to notebook
                ui.Notebook_peripheral:AddPage(ui.Panel_I2C[I2C], "I2C"..i2c_cfg:Children()[I2C].name:GetValue(), false)
        end

        -- add notebook to the module's sizer
        ui.FlexGridSizer_module:Add(ui.Notebook_peripheral, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- set module's panel sizer
        ui.Panel_module:SetSizer(ui.FlexGridSizer_module)

        -- add panel to the main sizer
        ui.FlexGridSizer1:Add(ui.Panel_module, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- add line
        ui.StaticLine = wx.wxStaticLine(this, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxSize(10,-1), wx.wxLI_HORIZONTAL)
        ui.FlexGridSizer1:Add(ui.StaticLine, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- add save button
        ui.Button_save = wx.wxButton(this, ID.BUTTON_SAVE, "Save", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator)
        ui.FlexGridSizer1:Add(ui.Button_save, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        this:Connect(ID.BUTTON_SAVE, wx.wxEVT_COMMAND_BUTTON_CLICKED, function(event) save_configuration() end)

        --
        this:SetSizer(ui.FlexGridSizer1)
        this:SetScrollRate(10, 10)

        --
        load_configuration()
        ui.Button_save:Enable(false)

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @param  None
-- @return Module name
--------------------------------------------------------------------------------
function i2c:get_window_name()
        return "I2C"
end


--------------------------------------------------------------------------------
-- @brief  Function is called by parent when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function i2c:selected()
end


--------------------------------------------------------------------------------
-- @brief  Function returns modify status
-- @param  None
-- @return If data is modified true is returned, otherwise false
--------------------------------------------------------------------------------
function i2c:is_modified()
        return ui.Button_save:IsEnabled()
end


--------------------------------------------------------------------------------
-- @brief  Function save configuration
-- @return None
--------------------------------------------------------------------------------
function i2c:save()
        save_configuration()
end


--------------------------------------------------------------------------------
-- @brief  Function returns module handler
-- @param  None
-- @return Module handler
--------------------------------------------------------------------------------
function get_handler()
        return i2c
end
