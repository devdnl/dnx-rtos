--[[============================================================================
@file    uart.lua

@author  Daniel Zorychta

@brief   Configuration script for UART module.

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
uart = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local modified  = false
local ui        = {}
local ID        = {}
local cpu_name  = nil    -- loaded when creating the window
local cpu_idx   = nil    -- loaded when creating the window
local cpu       = nil    -- loaded when creating the window
local uart_cfg  = nil    -- loaded when creating the window
local prio_list = ct:get_priority_list("stm32f1")

local baud_list = {"50",
                   "75",
                   "100",
                   "200",
                   "300",
                   "600",
                   "1200",
                   "2400",
                   "4800",
                   "9600",
                   "19200",
                   "38400",
                   "57600",
                   "115200",
                   "230400",
                   "460800",
                   "921600",
                   "1000000",
                   "2000000",
                   "3000000",
                   "4000000"}

local stop_bits_idx = {}
stop_bits_idx.UART_STOP_BIT_1 = 0
stop_bits_idx.UART_STOP_BIT_2 = 1

local parity_idx = {}
parity_idx.UART_PARITY_OFF  = 0
parity_idx.UART_PARITY_ODD  = 1
parity_idx.UART_PARITY_EVEN = 2


--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function loads all controls from configuration scripts
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_configuration()
        -- load module enable status
        local module_enable = ct:get_module_state("UART")
        ui.CheckBox_module_enable:SetValue(module_enable)
        ui.Panel1:Enable(module_enable)

        -- load default settings
        local baud = ct:get_string_index(baud_list, ct:key_read(config.arch.stm32f1.key.UART_DEFAULT_BAUD)) - 1
        ui.Choice_baud:SetSelection(baud)

        local stop_bits = stop_bits_idx[ct:key_read(config.arch.stm32f1.key.UART_DEFAULT_STOP_BITS)]
        if stop_bits == nil then stop_bits = 0 end
        ui.Choice_stop_bits:SetSelection(stop_bits)

        local parity = parity_idx[ct:key_read(config.arch.stm32f1.key.UART_DEFAULT_PARITY)]
        if parity == nil then parity = 0 end
        ui.Choice_parity:SetSelection(parity)

        local rx_buf_size = tonumber(ct:key_read(config.arch.stm32f1.key.UART_RX_BUFFER_LEN))
        ui.SpinCtrl_rx_buf_size:SetValue(rx_buf_size)

        -- load settings for each UART
        for i = 1, uart_cfg:NumChildren() do
                local uart_num = uart_cfg:Children()[i].name:GetValue()

                local uart_enable = ct:yes_no_to_bool(ct:key_read(config.arch.stm32f1.key["UART_UART"..uart_num.."_ENABLE"]))
                ui.UART[i].CheckBox_enable:SetValue(uart_enable)
                ui.UART[i].Panel:Enable(uart_enable)

                local irq_prio = ct:key_read(config.arch.stm32f1.key["UART_UART"..uart_num.."_PRIORITY"])
                if irq_prio == config.project.def.DEFAULT_IRQ_PRIORITY:GetValue() then
                irq_prio = #prio_list
                else
                        irq_prio = math.floor(tonumber(irq_prio) / 16)
                end
                ui.UART[i].Choice_irq_prio:SetSelection(irq_prio)
        end
end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function save_configuration()
        -- save module state
        ct:enable_module("UART", ui.CheckBox_module_enable:GetValue())

        -- save defaults
        local baud = baud_list[ui.Choice_baud:GetSelection() + 1]
        ct:key_write(config.arch.stm32f1.key.UART_DEFAULT_BAUD, baud)

        local stop_bits = ui.Choice_stop_bits:GetSelection()
        for f, i in pairs(stop_bits_idx) do
                if stop_bits_idx[f] == stop_bits then
                        stop_bits = f
                        break
                end
        end
        ct:key_write(config.arch.stm32f1.key.UART_DEFAULT_STOP_BITS, stop_bits)

        local parity = ui.Choice_parity:GetSelection()
        for f, i in pairs(parity_idx) do
                if parity_idx[f] == parity then
                        parity = f
                        break
                end
        end
        ct:key_write(config.arch.stm32f1.key.UART_DEFAULT_PARITY, parity)

        local rx_buf_size = tostring(ui.SpinCtrl_rx_buf_size:GetValue())
        ct:key_write(config.arch.stm32f1.key.UART_RX_BUFFER_LEN, rx_buf_size)

        -- save configuration not used by this script to default values
        ct:key_write(config.arch.stm32f1.key.UART_DEFAULT_TX_ENABLE, config.project.def.YES:GetValue())
        ct:key_write(config.arch.stm32f1.key.UART_DEFAULT_RX_ENABLE, config.project.def.YES:GetValue())
        ct:key_write(config.arch.stm32f1.key.UART_DEFAULT_LIN_MODE_ENABLE, config.project.def.NO:GetValue())
        ct:key_write(config.arch.stm32f1.key.UART_DEFAULT_HW_FLOW_CTRL, config.project.def.NO:GetValue())
        ct:key_write(config.arch.stm32f1.key.UART_DEFAULT_SINGLE_WIRE_MODE, config.project.def.NO:GetValue())

        -- save settings for each UART
        for i = 1, 5 do
                ct:key_write(config.arch.stm32f1.key["UART_UART"..i.."_ENABLE"], config.project.def.NO:GetValue())
        end

        for i = 1, uart_cfg:NumChildren() do
                local uart_num = uart_cfg:Children()[i].name:GetValue()

                local uart_enable = ct:bool_to_yes_no(ui.UART[i].CheckBox_enable:GetValue())
                ct:key_write(config.arch.stm32f1.key["UART_UART"..uart_num.."_ENABLE"], uart_enable)

                local irq_prio = ui.UART[i].Choice_irq_prio:GetSelection() + 1
                if irq_prio > #prio_list then
                        irq_prio = config.project.def.DEFAULT_IRQ_PRIORITY:GetValue()
                else
                        irq_prio = prio_list[irq_prio].value
                end
                ct:key_write(config.arch.stm32f1.key["UART_UART"..uart_num.."_PRIORITY"], irq_prio)
        end

        modified = false
end


--------------------------------------------------------------------------------
-- @brief  Event is called when module enable checkbox is changed
-- @param  this         event object
-- @return None
--------------------------------------------------------------------------------
local function event_checkbox_module_enable_updated(this)
        ui.Panel1:Enable(this:IsChecked())
        modified = true
end


--------------------------------------------------------------------------------
-- @brief  Event is called when value is changed (general)
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function event_value_updated()
        modified = true
end


--------------------------------------------------------------------------------
-- @brief  Event is called when UART enable checkbox is changed
-- @param  this     event object
-- @param  i        UART order number
-- @return None
--------------------------------------------------------------------------------
local function event_checkbox_UART_enable_updated(this, i)
        ui.UART[i].Panel:Enable(this:IsChecked())
        modified = true
end


--==============================================================================
-- GLOBAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function uart:create_window(parent)
        cpu_name   = ct:key_read(config.arch.stm32f1.key.CPU_NAME)
        cpu_idx    = ct:get_cpu_index("stm32f1", cpu_name)
        cpu        = config.arch.stm32f1.cpulist:Children()[cpu_idx]
        uart_cfg   = config.arch.stm32f1.cpulist:Children()[cpu_idx].peripherals.UART

        ui = {}
        ui.UART = {}

        ID = {}
        ID.CHECKBOX_MODULE_ENABLE = wx.wxNewId()
        ID.CHOICE_BAUD = wx.wxNewId()
        ID.CHOICE_STOP_BITS = wx.wxNewId()
        ID.CHOICE_PARITY = wx.wxNewId()
        ID.SPINCTRL_RX_BUF_SIZE = wx.wxNewId()
        ID.CHECKBOX_UART_ENABLE = {}
        ID.CHOICE_UART_IRQ_PRIO = {}
        ID.PANEL_UART = {}
        ID.PANEL1 = wx.wxNewId()

        ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
        local this = ui.window

        ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.CheckBox_module_enable = wx.wxCheckBox(this, ID.CHECKBOX_MODULE_ENABLE, "Enable module", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_MODULE_ENABLE")
        ui.FlexGridSizer1:Add(ui.CheckBox_module_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Panel1 = wx.wxPanel(this, ID.PANEL1, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL, "ID.PANEL1")
        ui.FlexGridSizer2 = wx.wxFlexGridSizer(0, 1, 0, 0)

        -- UARTs defaults
        ui.StaticBoxSizer1 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel1, "Default setting for all UARTs")
        ui.FlexGridSizer3 = wx.wxFlexGridSizer(0, 2, 0, 0)
        ui.StaticText1 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Baud [bps]", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText1, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_baud = wx.wxChoice(ui.Panel1, ID.CHOICE_BAUD, wx.wxDefaultPosition, wx.wxDefaultSize, baud_list, 0, wx.wxDefaultValidator, "ID.CHOICE_BAUD")
        ui.FlexGridSizer3:Add(ui.Choice_baud, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText2 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Stop bits", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText2, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_stop_bits = wx.wxChoice(ui.Panel1, ID.CHOICE_STOP_BITS, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_STOP_BITS")
        ui.Choice_stop_bits:Append("1 stop bit")
        ui.Choice_stop_bits:Append("2 stop bits")
        ui.FlexGridSizer3:Add(ui.Choice_stop_bits, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText3 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Parity", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText3, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_parity = wx.wxChoice(ui.Panel1, ID.CHOICE_PARITY, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_PARITY")
        ui.Choice_parity:Append("No parity bit")
        ui.Choice_parity:Append("Odd parity")
        ui.Choice_parity:Append("Even parity")
        ui.FlexGridSizer3:Add(ui.Choice_parity, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText4 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Rx buffer size [bytes]", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText4, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.SpinCtrl_rx_buf_size = wx.wxSpinCtrl(ui.Panel1, ID.SPINCTRL_RX_BUF_SIZE, "0", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 32, 2048, 0, "ID.SPINCTRL_RX_BUF_SIZE")
        ui.FlexGridSizer3:Add(ui.SpinCtrl_rx_buf_size, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticBoxSizer1:Add(ui.FlexGridSizer3, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.FlexGridSizer2:Add(ui.StaticBoxSizer1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- UART enable configuration
        ui.StaticBoxSizer_UART = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel1, "UART configuration")
        ui.FlexGridSizer_UART  = wx.wxFlexGridSizer(0, 2, 0, 0)
        for i = 1, uart_cfg:NumChildren() do
                ui.UART[i] = {}
                ID.CHECKBOX_UART_ENABLE[i] = wx.wxNewId()
                ui.UART[i].CheckBox_enable = wx.wxCheckBox(ui.Panel1, ID.CHECKBOX_UART_ENABLE[i], "Enable UART"..uart_cfg:Children()[i].name:GetValue(), wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_UART_ENABLE")
                ui.FlexGridSizer_UART:Add(ui.UART[i].CheckBox_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                this:Connect(ID.CHECKBOX_UART_ENABLE[i], wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function(this) event_checkbox_UART_enable_updated(this, i) end)

                ID.PANEL_UART[i] = wx.wxNewId()
                ui.UART[i].Panel = wx.wxPanel(ui.Panel1, ID.PANEL_UART[i], wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL, "ID.PANEL_UART")
                ui.UART[i].FlexGridSizer_irq_prio = wx.wxFlexGridSizer(0, 2, 0, 0)
                ui.UART[i].StaticText_irq_prio = wx.wxStaticText(ui.UART[i].Panel, wx.wxID_ANY, "IRQ priority", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.UART[i].FlexGridSizer_irq_prio:Add(ui.UART[i].StaticText_irq_prio, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ID.CHOICE_UART_IRQ_PRIO[i] = wx.wxNewId()
                ui.UART[i].Choice_irq_prio = wx.wxChoice(ui.UART[i].Panel, ID.CHOICE_UART_IRQ_PRIO[i], wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_UART_IRQ_PRIO")
                for _, item in ipairs(prio_list) do ui.UART[i].Choice_irq_prio:Append(item.name) end
                ui.UART[i].Choice_irq_prio:Append("System default")
                ui.UART[i].FlexGridSizer_irq_prio:Add(ui.UART[i].Choice_irq_prio, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                this:Connect(ID.CHOICE_UART_IRQ_PRIO[i], wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)

                ui.UART[i].Panel:SetSizer(ui.UART[i].FlexGridSizer_irq_prio)
                ui.FlexGridSizer_UART:Add(ui.UART[i].Panel, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        end
        ui.StaticBoxSizer_UART:Add(ui.FlexGridSizer_UART, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.FlexGridSizer2:Add(ui.StaticBoxSizer_UART, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        --
        ui.Panel1:SetSizer(ui.FlexGridSizer2)
        ui.FlexGridSizer1:Add(ui.Panel1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        --
        this:SetSizer(ui.FlexGridSizer1)
        this:SetScrollRate(50, 50)

        --
        this:Connect(ID.CHECKBOX_MODULE_ENABLE, wx.wxEVT_COMMAND_CHECKBOX_CLICKED, event_checkbox_module_enable_updated)
        this:Connect(ID.CHOICE_BAUD,            wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_STOP_BITS,       wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.CHOICE_PARITY,          wx.wxEVT_COMMAND_CHOICE_SELECTED,  event_value_updated                 )
        this:Connect(ID.SPINCTRL_RX_BUF_SIZE,   wx.wxEVT_COMMAND_SPINCTRL_UPDATED, event_value_updated                 )
        this:Connect(ID.SPINCTRL_RX_BUF_SIZE,   wx.wxEVT_COMMAND_TEXT_UPDATED,     event_value_updated                 )

        --
        load_configuration()
        modified = false

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @param  None
-- @return Module name
--------------------------------------------------------------------------------
function uart:get_window_name()
        return "UART"
end


--------------------------------------------------------------------------------
-- @brief  Function is called by parent when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function uart:selected()
end


--------------------------------------------------------------------------------
-- @brief  Function returns modify status
-- @param  None
-- @return If data is modified true is returned, otherwise false
--------------------------------------------------------------------------------
function uart:is_modified()
        return modified
end


--------------------------------------------------------------------------------
-- @brief  Function save configuration
-- @return None
--------------------------------------------------------------------------------
function uart:save()
        save_configuration()
end


--------------------------------------------------------------------------------
-- @brief  Function discard modified configuration
-- @return None
--------------------------------------------------------------------------------
function uart:discard()
        load_configuration()
        modified = false
end


--------------------------------------------------------------------------------
-- @brief  Function returns module handler
-- @param  None
-- @return Module handler
--------------------------------------------------------------------------------
function get_handler()
        return uart
end
