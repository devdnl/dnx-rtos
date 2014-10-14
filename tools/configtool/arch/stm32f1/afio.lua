--[[============================================================================
@file    afio.lua

@author  Daniel Zorychta

@brief   Configuration script for AFIO module.

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
afio = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local modified         = ct:new_modify_indicator()
local ui               = {}
local ID               = {}
local cpu_name         = nil    -- loaded when creating the window
local cpu_idx          = nil    -- loaded when creating the window
local cpu              = nil    -- loaded when creating the window
local cpu_family       = nil    -- loaded when creating the window
local CPU_FAMILY_CL    = config.arch.stm32f1.def.STM32F10X_CL:GetValue()
local CPU_FAMILY_LD_VL = config.arch.stm32f1.def.STM32F10X_LD_VL:GetValue()
local CPU_FAMILY_MD_VL = config.arch.stm32f1.def.STM32F10X_MD_VL:GetValue()
local CPU_FAMILY_HD_VL = config.arch.stm32f1.def.STM32F10X_HD_VL:GetValue()
local CPU_FAMILY_LD    = config.arch.stm32f1.def.STM32F10X_LD:GetValue()
local CPU_FAMILY_MD    = config.arch.stm32f1.def.STM32F10X_MD:GetValue()
local CPU_FAMILY_HD    = config.arch.stm32f1.def.STM32F10X_HD:GetValue()
local CPU_FAMILY_XL    = config.arch.stm32f1.def.STM32F10X_XL:GetValue()
local UI_CHOICE_SIZE   = wx.wxSize(350, -1)
local PORT_LIST        = {"PA", "PB", "PC", "PD", "PE", "PF", "PG"}


--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function loads all controls from configuration scripts
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_configuration()
        -- load module state
        local module_enable = ct:get_module_state("AFIO")
        ui.CheckBox_module_enable:SetValue(module_enable)
        ui.Panel1:Enable(module_enable)

        -- load Cortex Event Output controls
        local CEO_enable = ct:yes_no_to_bool(ct:key_read(config.arch.stm32f1.key.AFIO_EVENT_OUT_ENABLE))
        ui.CheckBox_CEO_enable:SetValue(CEO_enable)
        ui.Panel_CEO:Enable(CEO_enable)
        ui.Choice_CEO_port:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_EVENT_OUT_PORT)))
        ui.Choice_CEO_pin:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_EVENT_OUT_PIN)))

        -- load remap controls (common)
        ui.Choice_remap_SPI1:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_SPI1)))
        ui.Choice_remap_I2C1:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_I2C1)))
        ui.Choice_remap_USART1:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_USART1)))
        ui.Choice_remap_USART2:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_USART2)))
        ui.Choice_remap_USART3:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_USART3)))
        ui.Choice_remap_TIM1:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_TIM1)))
        ui.Choice_remap_TIM2:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_TIM2)))
        ui.Choice_remap_TIM3:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_TIM3)))
        ui.Choice_remap_TIM4:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_TIM4)))
        ui.Choice_remap_TIM5CH4:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_TIM5CH4)))
        ui.Choice_remap_CAN:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_CAN)))
        ui.Choice_remap_PD01:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_PD01)))
        ui.Choice_remap_ADC1_ETRGINJ:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_ADC1_ETRGINJ)))
        ui.Choice_remap_ADC1_ETRGREG:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_ADC1_ETRGREG)))
        ui.Choice_remap_ADC2_ETRGINJ:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_ADC2_ETRGINJ)))
        ui.Choice_remap_ADC2_ETRGREG:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_ADC2_ETRGREG)))
        ui.Choice_remap_JTAG:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_SWJ_CFG)))

        if cpu_family == CPU_FAMILY_CL then
                ui.Choice_remap_ETH_IO:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_ETH)))
                ui.Choice_remap_ETH_MII_RMII_PHY:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_MII_RMII_SEL)))
                ui.Choice_remap_ETH_PTP_PPS:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_PTP_PPS)))
                ui.Choice_remap_CAN2:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_CAN2)))
                ui.Choice_remap_SPI3:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_SPI3)))
                ui.Choice_remap_TIM2ITR1:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_TIM2ITR1)))
        end

        if cpu_family == CPU_FAMILY_LD_VL or cpu_family == CPU_FAMILY_MD_VL or cpu_family == CPU_FAMILY_HD_VL then
                ui.Choice_remap_TIM15:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_TIM15)))
                ui.Choice_remap_TIM16:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_TIM16)))
                ui.Choice_remap_TIM17:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_TIM17)))
                ui.Choice_remap_CEC:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_CEC)))
                ui.Choice_remap_TIM1_DMA:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_TIM1_DMA)))
        end

        if cpu_family == CPU_FAMILY_HD_VL then
                ui.Choice_remap_TIM76_DAC_DMA:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_TIM76_DAC_DMA)))
                ui.Choice_remap_TIM12:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_TIM12)))
                ui.Choice_remap_misc:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_MISC)))
        end

        if cpu_family == CPU_FAMILY_LD or cpu_family == CPU_FAMILY_MD or cpu_family == CPU_FAMILY_HD or cpu_family == CPU_FAMILY_XL then
                ui.Choice_remap_TIM9:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_TIM9)))
                ui.Choice_remap_TIM10:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_TIM10)))
                ui.Choice_remap_TIM11:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_TIM11)))
        end

        if cpu_family == CPU_FAMILY_HD_VL or cpu_family == CPU_FAMILY_LD or cpu_family == CPU_FAMILY_MD or cpu_family == CPU_FAMILY_HD or cpu_family == CPU_FAMILY_XL then
                ui.Choice_remap_TIM13:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_TIM13)))
                ui.Choice_remap_TIM14:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_TIM14)))
                ui.Choice_remap_NADV:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key.AFIO_REMAP_FSMC_NADV)))
        end

        -- load EXTI controls
        for i = 0, 15 do
                ui.Choice_EXTI[i]:SetSelection(tonumber(ct:key_read(config.arch.stm32f1.key["AFIO_EXTI"..i.."_PORT"])))
        end
end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function save_configuration()
        -- save module state
        ct:enable_module("AFIO", ui.CheckBox_module_enable:GetValue())

        -- save Cortex Event Output configuration
        ct:key_write(config.arch.stm32f1.key.AFIO_EVENT_OUT_ENABLE, ct:bool_to_yes_no(ui.CheckBox_CEO_enable:GetValue()))
        ct:key_write(config.arch.stm32f1.key.AFIO_EVENT_OUT_PORT, tostring(ui.Choice_CEO_port:GetSelection()))
        ct:key_write(config.arch.stm32f1.key.AFIO_EVENT_OUT_PIN, tostring(ui.Choice_CEO_pin:GetSelection()))

        -- save remap configuration (common)
        ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_SPI1, tostring(ui.Choice_remap_SPI1:GetSelection()))
        ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_I2C1, tostring(ui.Choice_remap_I2C1:GetSelection()))
        ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_USART1, tostring(ui.Choice_remap_USART1:GetSelection()))
        ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_USART2, tostring(ui.Choice_remap_USART2:GetSelection()))
        ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_USART3, tostring(ui.Choice_remap_USART3:GetSelection()))
        ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_TIM1, tostring(ui.Choice_remap_TIM1:GetSelection()))
        ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_TIM2, tostring(ui.Choice_remap_TIM2:GetSelection()))
        ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_TIM3, tostring(ui.Choice_remap_TIM3:GetSelection()))
        ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_TIM4, tostring(ui.Choice_remap_TIM4:GetSelection()))
        ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_TIM5CH4, tostring(ui.Choice_remap_TIM5CH4:GetSelection()))
        ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_CAN, tostring(ui.Choice_remap_CAN:GetSelection()))
        ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_PD01, tostring(ui.Choice_remap_PD01:GetSelection()))
        ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_ADC1_ETRGINJ, tostring(ui.Choice_remap_ADC1_ETRGINJ:GetSelection()))
        ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_ADC1_ETRGREG, tostring(ui.Choice_remap_ADC1_ETRGREG:GetSelection()))
        ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_ADC2_ETRGINJ, tostring(ui.Choice_remap_ADC2_ETRGINJ:GetSelection()))
        ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_ADC2_ETRGREG, tostring(ui.Choice_remap_ADC2_ETRGREG:GetSelection()))
        ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_SWJ_CFG, tostring(ui.Choice_remap_JTAG:GetSelection()))

        if cpu_family == CPU_FAMILY_CL then
                ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_ETH, tostring(ui.Choice_remap_ETH_IO:GetSelection()))
                ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_MII_RMII_SEL, tostring(ui.Choice_remap_ETH_MII_RMII_PHY:GetSelection()))
                ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_PTP_PPS, tostring(ui.Choice_remap_ETH_PTP_PPS:GetSelection()))
                ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_CAN2, tostring(ui.Choice_remap_CAN2:GetSelection()))
                ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_SPI3, tostring(ui.Choice_remap_SPI3:GetSelection()))
                ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_TIM2ITR1, tostring(ui.Choice_remap_TIM2ITR1:GetSelection()))
        end

        if cpu_family == CPU_FAMILY_LD_VL or cpu_family == CPU_FAMILY_MD_VL or cpu_family == CPU_FAMILY_HD_VL then
                ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_TIM15, tostring(ui.Choice_remap_TIM15:GetSelection()))
                ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_TIM16, tostring(ui.Choice_remap_TIM16:GetSelection()))
                ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_TIM17, tostring(ui.Choice_remap_TIM17:GetSelection()))
                ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_CEC, tostring(ui.Choice_remap_CEC:GetSelection()))
                ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_TIM1_DMA, tostring(ui.Choice_remap_TIM1_DMA:GetSelection()))
        end

        if cpu_family == CPU_FAMILY_HD_VL then
                ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_TIM76_DAC_DMA, tostring(ui.Choice_remap_TIM76_DAC_DMA:GetSelection()))
                ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_TIM12, tostring(ui.Choice_remap_TIM12:GetSelection()))
                ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_MISC, tostring(ui.Choice_remap_misc:GetSelection()))
        end

        if cpu_family == CPU_FAMILY_LD or cpu_family == CPU_FAMILY_MD or cpu_family == CPU_FAMILY_HD or cpu_family == CPU_FAMILY_XL then
                ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_TIM9, tostring(ui.Choice_remap_TIM9:GetSelection()))
                ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_TIM10, tostring(ui.Choice_remap_TIM10:GetSelection()))
                ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_TIM11, tostring(ui.Choice_remap_TIM11:GetSelection()))
        end

        if cpu_family == CPU_FAMILY_HD_VL or cpu_family == CPU_FAMILY_LD or cpu_family == CPU_FAMILY_MD or cpu_family == CPU_FAMILY_HD or cpu_family == CPU_FAMILY_XL then
                ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_TIM13, tostring(ui.Choice_remap_TIM13:GetSelection()))
                ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_TIM14, tostring(ui.Choice_remap_TIM14:GetSelection()))
                ct:key_write(config.arch.stm32f1.key.AFIO_REMAP_FSMC_NADV, tostring(ui.Choice_remap_NADV:GetSelection()))
        end

        -- save EXTI configuration
        for i = 0, 15 do
                ct:key_write(config.arch.stm32f1.key["AFIO_EXTI"..i.."_PORT"], tostring(ui.Choice_EXTI[i]:GetSelection()))
        end

        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Event is called when module enable checkbox is changed
-- @param  this         event object
-- @return None
--------------------------------------------------------------------------------
local function event_checkbox_module_enable_updated(this)
        ui.Panel1:Enable(this:IsChecked())
        modified:yes()
end


--------------------------------------------------------------------------------
-- @brief  Event is called when Cortex Event Output enable checkbox is changed
-- @param  this         event object
-- @return None
--------------------------------------------------------------------------------
local function event_checkbox_CEO_enable_updated(this)
        ui.Panel_CEO:Enable(this:IsChecked())
        modified:yes()
end


--------------------------------------------------------------------------------
-- @brief  Event is called when value is changed (general)
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function event_value_updated()
        modified:yes()
end


--==============================================================================
-- GLOBAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function afio:create_window(parent)
        cpu_name   = ct:key_read(config.arch.stm32f1.key.CPU_NAME)
        cpu_idx    = ct:get_cpu_index("stm32f1", cpu_name)
        cpu        = config.arch.stm32f1.cpulist:Children()[cpu_idx]
        cpu_family = cpu.family:GetValue()

        ui = {}
        ui.StaticText_EXTI = {}
        ui.Choice_EXTI     = {}

        ID = {}
        ID.CHECKBOX_MODULE_ENABLE = wx.wxNewId()
        ID.CHECKBOX_CEO_ENABLE = wx.wxNewId()
        ID.CHOICE_CEO_PORT = wx.wxNewId()
        ID.CHOICE_CEO_PIN = wx.wxNewId()
        ID.PANEL2 = wx.wxNewId()
        ID.CHOICE_REMAP_SPI1 = wx.wxNewId()
        ID.CHOICE_REMAP_I2C1 = wx.wxNewId()
        ID.CHOICE_REMAP_USART1 = wx.wxNewId()
        ID.CHOICE_REMAP_USART2 = wx.wxNewId()
        ID.CHOICE_REMAP_USART3 = wx.wxNewId()
        ID.CHOICE_REMAP_TIM1 = wx.wxNewId()
        ID.CHOICE_REMAP_TIM2 = wx.wxNewId()
        ID.CHOICE_REMAP_TIM3 = wx.wxNewId()
        ID.CHOICE_REMAP_TIM4 = wx.wxNewId()
        ID.CHOICE_REMAP_TIM5CH4 = wx.wxNewId()
        ID.CHOICE_REMAP_CAN = wx.wxNewId()
        ID.CHOICE_REMAP_PD01 = wx.wxNewId()
        ID.CHOICE_REMAP_ADC1_ETRGINJ = wx.wxNewId()
        ID.CHOICE_REMAP_ADC1_ETRGREG = wx.wxNewId()
        ID.CHOICE_REMAP_ADC2_ETRGINJ = wx.wxNewId()
        ID.CHOICE_REMAP_ADC2_ETRGREG = wx.wxNewId()
        ID.CHOICE_REMAP_JTAG = wx.wxNewId()
        ID.CHOICE_REMAP_ETH_IO = wx.wxNewId()
        ID.CHOICE_REMAP_ETH_MII_RMII_PHY = wx.wxNewId()
        ID.CHOICE_REMAP_ETH_PTP_PPS = wx.wxNewId()
        ID.CHOICE_REMAP_CAN2 = wx.wxNewId()
        ID.CHOICE_REMAP_SPI3 = wx.wxNewId()
        ID.CHOICE_REMAP_TIM2ITR1 = wx.wxNewId()
        ID.CHOICE_REMAP_TIM15 = wx.wxNewId()
        ID.CHOICE_REMAP_TIM16 = wx.wxNewId()
        ID.CHOICE_REMAP_TIM17 = wx.wxNewId()
        ID.CHOICE_REMAP_CEC = wx.wxNewId()
        ID.CHOICE_REMAP_TIM1_DMA = wx.wxNewId()
        ID.CHOICE_REMAP_TIM13 = wx.wxNewId()
        ID.CHOICE_REMAP_TIM14 = wx.wxNewId()
        ID.CHOICE_REMAP_NADV = wx.wxNewId()
        ID.CHOICE_REMAP_TIM76_DAC_DMA = wx.wxNewId()
        ID.CHOICE_REMAP_TIM9 = wx.wxNewId()
        ID.CHOICE_REMAP_TIM10 = wx.wxNewId()
        ID.CHOICE_REMAP_TIM11 = wx.wxNewId()
        ID.CHOICE_REMAP_TIM12 = wx.wxNewId()
        ID.CHOICE_REMAP_MISC = wx.wxNewId()
        ID.CHOICE_EXTI = {}
        ID.PANEL1 = wx.wxNewId()


        ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
        local this = ui.window


        ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.CheckBox_module_enable = wx.wxCheckBox(this, ID.CHECKBOX_MODULE_ENABLE, "Enable module", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_MODULE_ENABLE")
        ui.FlexGridSizer1:Add(ui.CheckBox_module_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Panel1 = wx.wxPanel(this, ID.PANEL1, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL, "ID.PANEL1")
        ui.FlexGridSizer2 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.StaticBoxSizer1 = wx.wxStaticBoxSizer(wx.wxVERTICAL, ui.Panel1, "Cortex Event Output")
        ui.FlexGridSizer3 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.CheckBox_CEO_enable = wx.wxCheckBox(ui.Panel1, ID.CHECKBOX_CEO_ENABLE, "Enable", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_CEO_ENABLE")
        ui.FlexGridSizer3:Add(ui.CheckBox_CEO_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.FlexGridSizer4 = wx.wxFlexGridSizer(0, 2, 0, 0)
        ui.Panel_CEO = wx.wxPanel(ui.Panel1, ID.PANEL2, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL, "ID.PANEL2")
        ui.FlexGridSizer5 = wx.wxFlexGridSizer(0, 4, 0, 0)
        ui.StaticText1 = wx.wxStaticText(ui.Panel_CEO, wx.wxID_ANY, "Port", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer5:Add(ui.StaticText1, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_CEO_port = wx.wxChoice(ui.Panel_CEO, ID.CHOICE_CEO_PORT, wx.wxDefaultPosition, wx.wxDefaultSize, PORT_LIST, 0, wx.wxDefaultValidator, "ID.CHOICE_CEO_PORT")
        this:Connect(ID.CHOICE_CEO_PORT, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
        ui.FlexGridSizer5:Add(ui.Choice_CEO_port, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText2 = wx.wxStaticText(ui.Panel_CEO, wx.wxID_ANY, "Pin", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer5:Add(ui.StaticText2, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_CEO_pin = wx.wxChoice(ui.Panel_CEO, ID.CHOICE_CEO_PIN, wx.wxDefaultPosition, wx.wxDefaultSize, {"0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15"}, 0, wx.wxDefaultValidator, "ID.CHOICE_CEO_PIN")
        this:Connect(ID.CHOICE_CEO_PIN, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
        ui.FlexGridSizer5:Add(ui.Choice_CEO_pin, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Panel_CEO:SetSizer(ui.FlexGridSizer5)
        ui.FlexGridSizer4:Add(ui.Panel_CEO, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.FlexGridSizer3:Add(ui.FlexGridSizer4, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.StaticBoxSizer1:Add(ui.FlexGridSizer3, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.FlexGridSizer2:Add(ui.StaticBoxSizer1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- Remap configuration
        ui.StaticBoxSizer2 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel1, "Remap configuration")
        ui.FlexGridSizer6 = wx.wxFlexGridSizer(0, 2, 0, 0)
        ui.StaticText8 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap SPI1", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer6:Add(ui.StaticText8, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_remap_SPI1 = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_SPI1, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_SPI1")
        this:Connect(ID.CHOICE_REMAP_SPI1, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
        ui.Choice_remap_SPI1:Append("No (NSS/PA4, SCK/PA5, MISO/PA6, MOSI/PA7)")
        ui.Choice_remap_SPI1:Append("Yes (NSS/PA15, SCK/PB3, MISO/PB4, MOSI/PB5)")
        ui.Choice_remap_SPI1:SetMinSize(UI_CHOICE_SIZE)
        ui.Choice_remap_SPI1:SetMaxSize(UI_CHOICE_SIZE)
        ui.FlexGridSizer6:Add(ui.Choice_remap_SPI1, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText9 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap I2C1", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer6:Add(ui.StaticText9, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_remap_I2C1 = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_I2C1, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_I2C1")
        this:Connect(ID.CHOICE_REMAP_I2C1, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
        ui.Choice_remap_I2C1:Append("No (SCL/PB6, SDA/PB7)")
        ui.Choice_remap_I2C1:Append("Yes (SCL/PB8, SDA/PB9)")
        ui.Choice_remap_I2C1:SetMinSize(UI_CHOICE_SIZE)
        ui.Choice_remap_I2C1:SetMaxSize(UI_CHOICE_SIZE)
        ui.FlexGridSizer6:Add(ui.Choice_remap_I2C1, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText10 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap USART1", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer6:Add(ui.StaticText10, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_remap_USART1 = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_USART1, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_USART1")
        this:Connect(ID.CHOICE_REMAP_USART1, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
        ui.Choice_remap_USART1:Append("No (TX/PA9, RX/PA10)")
        ui.Choice_remap_USART1:Append("Yes (TX/PB6, RX/PB7)")
        ui.Choice_remap_USART1:SetMinSize(UI_CHOICE_SIZE)
        ui.Choice_remap_USART1:SetMaxSize(UI_CHOICE_SIZE)
        ui.FlexGridSizer6:Add(ui.Choice_remap_USART1, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText11 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap USART2", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer6:Add(ui.StaticText11, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_remap_USART2 = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_USART2, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_USART2")
        this:Connect(ID.CHOICE_REMAP_USART2, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
        ui.Choice_remap_USART2:Append("No (CTS/PA0, RTS/PA1, TX/PA2, RX/PA3, CK/PA4)")
        ui.Choice_remap_USART2:Append("Yes (CTS/PD3, RTS/PD4, TX/PD5, RX/PD6, CK/PD7)")
        ui.Choice_remap_USART2:SetMinSize(UI_CHOICE_SIZE)
        ui.Choice_remap_USART2:SetMaxSize(UI_CHOICE_SIZE)
        ui.FlexGridSizer6:Add(ui.Choice_remap_USART2, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText3 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap USART3", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer6:Add(ui.StaticText3, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_remap_USART3 = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_USART3, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_USART3")
        this:Connect(ID.CHOICE_REMAP_USART3, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
        ui.Choice_remap_USART3:Append("No (TX/PB10, RX/PB11, CK/PB12, CTS/PB13, RTS/PB14)")
        ui.Choice_remap_USART3:Append("Partial (CTS/PD3, RTS/PD4, TX/PD5, RX/PD6, CK/PD7)")
        ui.Choice_remap_USART3:Append("Full (TX/PD8, RX/PD9, CK/PD10, CTS/PD11, RTS/PD12)")
        ui.Choice_remap_USART3:SetMinSize(UI_CHOICE_SIZE)
        ui.Choice_remap_USART3:SetMaxSize(UI_CHOICE_SIZE)
        ui.FlexGridSizer6:Add(ui.Choice_remap_USART3, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText4 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap TIM1", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer6:Add(ui.StaticText4, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_remap_TIM1 = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_TIM1, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_TIM1")
        this:Connect(ID.CHOICE_REMAP_TIM1, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
        ui.Choice_remap_TIM1:Append("No (ETR/PA12, CH1/PA8, CH2/PA9, CH3/PA10, CH4/PA11, BKIN/PB12, CH1N/PB13, CH2N/PB14, CH3N/PB15)")
        ui.Choice_remap_TIM1:Append("Partial (ETR/PA12, CH1/PA8, CH2/PA9, CH3/PA10, CH4/PA11, BKIN/PA6, CH1N/PA7, CH2N/PB0, CH3N/PB1)")
        ui.Choice_remap_TIM1:Append("Full (ETR/PE7, CH1/PE9, CH2/PE11, CH3/PE13, CH4/PE14, BKIN/PE15, CH1N/PE8, CH2N/PE10, CH3N/PE12)")
        ui.Choice_remap_TIM1:SetMinSize(UI_CHOICE_SIZE)
        ui.Choice_remap_TIM1:SetMaxSize(UI_CHOICE_SIZE)
        ui.FlexGridSizer6:Add(ui.Choice_remap_TIM1, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText5 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap TIM2", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer6:Add(ui.StaticText5, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_remap_TIM2 = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_TIM2, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_TIM2")
        this:Connect(ID.CHOICE_REMAP_TIM2, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
        ui.Choice_remap_TIM2:Append("No (CH1/ETR/PA0, CH2/PA1, CH3/PA2, CH4/PA3)")
        ui.Choice_remap_TIM2:Append("Partial (CH1/ETR/PA15, CH2/PB3, CH3/PA2, CH4/PA3)")
        ui.Choice_remap_TIM2:Append("Partial (CH1/ETR/PA0, CH2/PA1, CH3/PB10, CH4/PB11)")
        ui.Choice_remap_TIM2:Append("Full (CH1/ETR/PA15, CH2/PB3, CH3/PB10, CH4/PB11)")
        ui.Choice_remap_TIM2:SetMinSize(UI_CHOICE_SIZE)
        ui.Choice_remap_TIM2:SetMaxSize(UI_CHOICE_SIZE)
        ui.FlexGridSizer6:Add(ui.Choice_remap_TIM2, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText6 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap TIM3", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer6:Add(ui.StaticText6, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_remap_TIM3 = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_TIM3, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_TIM3")
        this:Connect(ID.CHOICE_REMAP_TIM3, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
        ui.Choice_remap_TIM3:Append("No (CH1/PA6, CH2/PA7, CH3/PB0, CH4/PB1)")
        ui.Choice_remap_TIM3:Append("Partial (CH1/PB4, CH2/PB5, CH3/PB0, CH4/PB1)")
        ui.Choice_remap_TIM3:Append("Full (CH1/PC6, CH2/PC7, CH3/PC8, CH4/PC9)")
        ui.Choice_remap_TIM3:SetMinSize(UI_CHOICE_SIZE)
        ui.Choice_remap_TIM3:SetMaxSize(UI_CHOICE_SIZE)
        ui.FlexGridSizer6:Add(ui.Choice_remap_TIM3, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText12 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap TIM4", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer6:Add(ui.StaticText12, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_remap_TIM4 = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_TIM4, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_TIM4")
        this:Connect(ID.CHOICE_REMAP_TIM4, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
        ui.Choice_remap_TIM4:Append("No (TIM4_CH1/PB6, TIM4_CH2/PB7, TIM4_CH3/PB8, TIM4_CH4/PB9)")
        ui.Choice_remap_TIM4:Append("Yes (TIM4_CH1/PD12, TIM4_CH2/PD13, TIM4_CH3/PD14, TIM4_CH4/PD15)")
        ui.Choice_remap_TIM4:SetMinSize(UI_CHOICE_SIZE)
        ui.Choice_remap_TIM4:SetMaxSize(UI_CHOICE_SIZE)
        ui.FlexGridSizer6:Add(ui.Choice_remap_TIM4, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText14 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap TIM5CH4", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer6:Add(ui.StaticText14, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_remap_TIM5CH4 = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_TIM5CH4, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_TIM5CH4")
        this:Connect(ID.CHOICE_REMAP_TIM5CH4, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
        ui.Choice_remap_TIM5CH4:Append("No (TIM5_CH4 connected to PA3)")
        ui.Choice_remap_TIM5CH4:Append("Yes (LSI internal clock connected to TIM5_CH4)")
        ui.Choice_remap_TIM5CH4:SetMinSize(UI_CHOICE_SIZE)
        ui.Choice_remap_TIM5CH4:SetMaxSize(UI_CHOICE_SIZE)
        ui.FlexGridSizer6:Add(ui.Choice_remap_TIM5CH4, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText7 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap CAN", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer6:Add(ui.StaticText7, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_remap_CAN = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_CAN, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_CAN")
        this:Connect(ID.CHOICE_REMAP_CAN, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
        ui.Choice_remap_CAN:Append("No (CAN_RX/PA11, CAN_TX/PA12)")
        ui.Choice_remap_CAN:Append("Variant 1 (CAN_RX/PB8, CAN_TX/PB9)")
        ui.Choice_remap_CAN:Append("Variant 2 (CAN_RX/PD0, CAN_TX/PD1)")
        ui.Choice_remap_CAN:SetMinSize(UI_CHOICE_SIZE)
        ui.Choice_remap_CAN:SetMaxSize(UI_CHOICE_SIZE)
        ui.FlexGridSizer6:Add(ui.Choice_remap_CAN, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText13 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap PD0/PD1", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer6:Add(ui.StaticText13, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_remap_PD01 = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_PD01, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_PD01")
        this:Connect(ID.CHOICE_REMAP_PD01, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
        ui.Choice_remap_PD01:Append("No (PD0/PD1 are not present in the 36-,48-,64-pin packages)")
        ui.Choice_remap_PD01:Append("Yes (PD0/OSC_IN, PD1/OSC_OUT)")
        ui.Choice_remap_PD01:SetMinSize(UI_CHOICE_SIZE)
        ui.Choice_remap_PD01:SetMaxSize(UI_CHOICE_SIZE)
        ui.FlexGridSizer6:Add(ui.Choice_remap_PD01, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText15 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap ADC1 ETRGINJ", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer6:Add(ui.StaticText15, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_remap_ADC1_ETRGINJ = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_ADC1_ETRGINJ, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_ADC1_ETRGINJ")
        this:Connect(ID.CHOICE_REMAP_ADC1_ETRGINJ, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
        ui.Choice_remap_ADC1_ETRGINJ:Append("No (ADC1 External trigger injected conversion connected to EXTI15)")
        ui.Choice_remap_ADC1_ETRGINJ:Append("Yes (ADC1 External Event injected conversion connected to TIM8_CH4)")
        ui.Choice_remap_ADC1_ETRGINJ:SetMinSize(UI_CHOICE_SIZE)
        ui.Choice_remap_ADC1_ETRGINJ:SetMaxSize(UI_CHOICE_SIZE)
        ui.FlexGridSizer6:Add(ui.Choice_remap_ADC1_ETRGINJ, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText16 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap ADC1 ETRGREG", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer6:Add(ui.StaticText16, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_remap_ADC1_ETRGREG = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_ADC1_ETRGREG, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_ADC1_ETRGREG")
        this:Connect(ID.CHOICE_REMAP_ADC1_ETRGREG, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
        ui.Choice_remap_ADC1_ETRGREG:Append("No (ADC1 External trigger regular conversion connected to EXTI11)")
        ui.Choice_remap_ADC1_ETRGREG:Append("Yes (ADC1 External Event regular conversion connected to TIM8_TRGO)")
        ui.Choice_remap_ADC1_ETRGREG:SetMinSize(UI_CHOICE_SIZE)
        ui.Choice_remap_ADC1_ETRGREG:SetMaxSize(UI_CHOICE_SIZE)
        ui.FlexGridSizer6:Add(ui.Choice_remap_ADC1_ETRGREG, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText17 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap ADC2 ETRGINJ", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer6:Add(ui.StaticText17, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_remap_ADC2_ETRGINJ = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_ADC2_ETRGINJ, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_ADC2_ETRGINJ")
        this:Connect(ID.CHOICE_REMAP_ADC2_ETRGINJ, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
        ui.Choice_remap_ADC2_ETRGINJ:Append("No (ADC2 External trigger injected conversion connected to EXTI15)")
        ui.Choice_remap_ADC2_ETRGINJ:Append("Yes (ADC2 External Event injected conversion connected to TIM8_CH4)")
        ui.Choice_remap_ADC2_ETRGINJ:SetMinSize(UI_CHOICE_SIZE)
        ui.Choice_remap_ADC2_ETRGINJ:SetMaxSize(UI_CHOICE_SIZE)
        ui.FlexGridSizer6:Add(ui.Choice_remap_ADC2_ETRGINJ, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText18 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap ADC2 ETRGREG", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer6:Add(ui.StaticText18, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_remap_ADC2_ETRGREG = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_ADC2_ETRGREG, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_ADC2_ETRGREG")
        this:Connect(ID.CHOICE_REMAP_ADC2_ETRGREG, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
        ui.Choice_remap_ADC2_ETRGREG:Append("No (ADC2 External trigger regular conversion connected to EXTI11)")
        ui.Choice_remap_ADC2_ETRGREG:Append("Yes (ADC2 External Event regular conversion connected to TIM8_TRGO)")
        ui.Choice_remap_ADC2_ETRGREG:SetMinSize(UI_CHOICE_SIZE)
        ui.Choice_remap_ADC2_ETRGREG:SetMaxSize(UI_CHOICE_SIZE)
        ui.FlexGridSizer6:Add(ui.Choice_remap_ADC2_ETRGREG, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText19 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Serial wire JTAG setup", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer6:Add(ui.StaticText19, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_remap_JTAG = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_JTAG, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_JTAG")
        this:Connect(ID.CHOICE_REMAP_JTAG, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
        ui.Choice_remap_JTAG:Append("Full SWJ (JTAG-DP + SW-DP): Reset State")
        ui.Choice_remap_JTAG:Append("Full SWJ (JTAG-DP + SW-DP) w/o NJTRST")
        ui.Choice_remap_JTAG:Append("JTAG-DP Disabled and SW-DP Enabled")
        ui.Choice_remap_JTAG:Append("JTAG-DP Disabled and SW-DP Disabled")
        ui.Choice_remap_JTAG:SetMinSize(UI_CHOICE_SIZE)
        ui.Choice_remap_JTAG:SetMaxSize(UI_CHOICE_SIZE)
        ui.FlexGridSizer6:Add(ui.Choice_remap_JTAG, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        if cpu_family == CPU_FAMILY_CL then
                ui.StaticText20 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap Ethernet I/O", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer6:Add(ui.StaticText20, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_remap_ETH_IO = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_ETH_IO, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_ETH_IO")
                this:Connect(ID.CHOICE_REMAP_ETH_IO, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
                ui.Choice_remap_ETH_IO:Append("No (RX_DV-CRS_DV/PA7, RXD0/PC4, RXD1/PC5, RXD2/PB0, RXD3/PB1)")
                ui.Choice_remap_ETH_IO:Append("Yes (RX_DV-CRS_DV/PD8, RXD0/PD9, RXD1/PD10, RXD2/PD11, RXD3/PD12)")
                ui.Choice_remap_ETH_IO:SetMinSize(UI_CHOICE_SIZE)
                ui.Choice_remap_ETH_IO:SetMaxSize(UI_CHOICE_SIZE)
                ui.FlexGridSizer6:Add(ui.Choice_remap_ETH_IO, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText23 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Ethernet MII/RMII PHY", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer6:Add(ui.StaticText23, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_remap_ETH_MII_RMII_PHY = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_ETH_MII_RMII_PHY, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_ETH_MII_RMII_PHY")
                this:Connect(ID.CHOICE_REMAP_ETH_MII_RMII_PHY, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
                ui.Choice_remap_ETH_MII_RMII_PHY:Append("MII PHY")
                ui.Choice_remap_ETH_MII_RMII_PHY:Append("RMII PHY")
                ui.Choice_remap_ETH_MII_RMII_PHY:SetMinSize(UI_CHOICE_SIZE)
                ui.Choice_remap_ETH_MII_RMII_PHY:SetMaxSize(UI_CHOICE_SIZE)
                ui.FlexGridSizer6:Add(ui.Choice_remap_ETH_MII_RMII_PHY, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText25 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Ethernet PTP PPS", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer6:Add(ui.StaticText25, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_remap_ETH_PTP_PPS = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_ETH_PTP_PPS, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_ETH_PTP_PPS")
                this:Connect(ID.CHOICE_REMAP_ETH_PTP_PPS, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
                ui.Choice_remap_ETH_PTP_PPS:Append("No (PTP_PPS not output on PB5)")
                ui.Choice_remap_ETH_PTP_PPS:Append("Yes (PTP_PPS is output on PB5)")
                ui.Choice_remap_ETH_PTP_PPS:SetMinSize(UI_CHOICE_SIZE)
                ui.Choice_remap_ETH_PTP_PPS:SetMaxSize(UI_CHOICE_SIZE)
                ui.FlexGridSizer6:Add(ui.Choice_remap_ETH_PTP_PPS, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText21 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap CAN2", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer6:Add(ui.StaticText21, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_remap_CAN2 = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_CAN2, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_CAN2")
                this:Connect(ID.CHOICE_REMAP_CAN2, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
                ui.Choice_remap_CAN2:Append("No (CAN2_RX/PB12, CAN2_TX/PB13)")
                ui.Choice_remap_CAN2:Append("Yes (CAN2_RX/PB5, CAN2_TX/PB6)")
                ui.Choice_remap_CAN2:SetMinSize(UI_CHOICE_SIZE)
                ui.Choice_remap_CAN2:SetMaxSize(UI_CHOICE_SIZE)
                ui.FlexGridSizer6:Add(ui.Choice_remap_CAN2, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText22 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap SPI3", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer6:Add(ui.StaticText22, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_remap_SPI3 = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_SPI3, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_SPI3")
                this:Connect(ID.CHOICE_REMAP_SPI3, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
                ui.Choice_remap_SPI3:Append("No (SPI_NSS-I2S3_WS/PA15, SPI3_SCK-I2S3_CK/PB3, SPI3_MISO/PB4, SPI3_MOSI-I2S3_SD/PB5)")
                ui.Choice_remap_SPI3:Append("Yes (SPI3_NSS-I2S3_WS/PA4, SPI3_SCK-I2S3_CK/PC10, SPI3_MISO/PC11, SPI3_MOSI-I2S3_SD/PC12)")
                ui.Choice_remap_SPI3:SetMinSize(UI_CHOICE_SIZE)
                ui.Choice_remap_SPI3:SetMaxSize(UI_CHOICE_SIZE)
                ui.FlexGridSizer6:Add(ui.Choice_remap_SPI3, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText24 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap TIM2ITR1", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer6:Add(ui.StaticText24, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_remap_TIM2ITR1 = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_TIM2ITR1, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_TIM2ITR1")
                this:Connect(ID.CHOICE_REMAP_TIM2ITR1, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
                ui.Choice_remap_TIM2ITR1:Append("No (TIM2_ITR1 connected internally to the Ethernet PTP output for calibration purposes)")
                ui.Choice_remap_TIM2ITR1:Append("Yes (USB OTG SOF output connected to TIM2_ITR1 for calibration purposes)")
                ui.Choice_remap_TIM2ITR1:SetMinSize(UI_CHOICE_SIZE)
                ui.Choice_remap_TIM2ITR1:SetMaxSize(UI_CHOICE_SIZE)
                ui.FlexGridSizer6:Add(ui.Choice_remap_TIM2ITR1, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        end

        if cpu_family == CPU_FAMILY_LD_VL or cpu_family == CPU_FAMILY_MD_VL or cpu_family == CPU_FAMILY_HD_VL then
                ui.StaticText26 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap TIM15", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer6:Add(ui.StaticText26, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_remap_TIM15 = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_TIM15, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_TIM15")
                this:Connect(ID.CHOICE_REMAP_TIM15, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
                ui.Choice_remap_TIM15:Append("No (CH1/PA2, CH2/PA3)")
                ui.Choice_remap_TIM15:Append("Yes (CH1/PB14, CH2/PB15)")
                ui.Choice_remap_TIM15:SetMinSize(UI_CHOICE_SIZE)
                ui.Choice_remap_TIM15:SetMaxSize(UI_CHOICE_SIZE)
                ui.FlexGridSizer6:Add(ui.Choice_remap_TIM15, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText27 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap TIM16", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer6:Add(ui.StaticText27, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_remap_TIM16 = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_TIM16, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_TIM16")
                this:Connect(ID.CHOICE_REMAP_TIM16, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
                ui.Choice_remap_TIM16:Append("No (CH1/PB8)")
                ui.Choice_remap_TIM16:Append("Yes (CH1/PA6)")
                ui.Choice_remap_TIM16:SetMinSize(UI_CHOICE_SIZE)
                ui.Choice_remap_TIM16:SetMaxSize(UI_CHOICE_SIZE)
                ui.FlexGridSizer6:Add(ui.Choice_remap_TIM16, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText28 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap TIM17", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer6:Add(ui.StaticText28, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_remap_TIM17 = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_TIM17, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_TIM17")
                this:Connect(ID.CHOICE_REMAP_TIM17, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
                ui.Choice_remap_TIM17:Append("No (CH1/PB9)")
                ui.Choice_remap_TIM17:Append("Yes (CH1/PA7)")
                ui.Choice_remap_TIM17:SetMinSize(UI_CHOICE_SIZE)
                ui.Choice_remap_TIM17:SetMaxSize(UI_CHOICE_SIZE)
                ui.FlexGridSizer6:Add(ui.Choice_remap_TIM17, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText29 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap CEC", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer6:Add(ui.StaticText29, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_remap_CEC = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_CEC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_CEC")
                this:Connect(ID.CHOICE_REMAP_CEC, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
                ui.Choice_remap_CEC:Append("No (CEC/PB8)")
                ui.Choice_remap_CEC:Append("Yes (CEC/PB10)")
                ui.Choice_remap_CEC:SetMinSize(UI_CHOICE_SIZE)
                ui.Choice_remap_CEC:SetMaxSize(UI_CHOICE_SIZE)
                ui.FlexGridSizer6:Add(ui.Choice_remap_CEC, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText30 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap TIM1 DMA", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer6:Add(ui.StaticText30, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_remap_TIM1_DMA = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_TIM1_DMA, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_TIM1_DMA")
                this:Connect(ID.CHOICE_REMAP_TIM1_DMA, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
                ui.Choice_remap_TIM1_DMA:Append("No (TIM1_CH1 DMA request/DMA1 Channel 2, TIM1_CH2 DMA request/DMA1 Channel 3)")
                ui.Choice_remap_TIM1_DMA:Append("Yes (TIM1_CH1 DMA request/DMA1 Channel 6, TIM1_CH2 DMA request/DMA1 Channel 6)")
                ui.Choice_remap_TIM1_DMA:SetMinSize(UI_CHOICE_SIZE)
                ui.Choice_remap_TIM1_DMA:SetMaxSize(UI_CHOICE_SIZE)
                ui.FlexGridSizer6:Add(ui.Choice_remap_TIM1_DMA, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        end

        if cpu_family == CPU_FAMILY_HD_VL then
                ui.StaticText34 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap TIM76 DAC DMA", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer6:Add(ui.StaticText34, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_remap_TIM76_DAC_DMA = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_TIM76_DAC_DMA, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_TIM76_DAC_DMA")
                this:Connect(ID.CHOICE_REMAP_TIM76_DAC_DMA, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
                ui.Choice_remap_TIM76_DAC_DMA:Append("No (TIM6_DAC1 DMA request/DMA2 Channel 3, TIM7_DAC2 DMA request/DMA2 Channel 4)")
                ui.Choice_remap_TIM76_DAC_DMA:Append("Yes (TIM6_DAC1 DMA request/DMA1 Channel 3, TIM7_DAC2 DMA request/DMA1 Channel 4)")
                ui.Choice_remap_TIM76_DAC_DMA:SetMinSize(UI_CHOICE_SIZE)
                ui.Choice_remap_TIM76_DAC_DMA:SetMaxSize(UI_CHOICE_SIZE)
                ui.FlexGridSizer6:Add(ui.Choice_remap_TIM76_DAC_DMA, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText38 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap TIM12", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer6:Add(ui.StaticText38, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_remap_TIM12 = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_TIM12, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_TIM12")
                this:Connect(ID.CHOICE_REMAP_TIM12, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
                ui.Choice_remap_TIM12:Append("No (CH1/PC4, CH2/PC5)")
                ui.Choice_remap_TIM12:Append("Yes (CH1/PB12, CH2/PB13)")
                ui.Choice_remap_TIM12:SetMinSize(UI_CHOICE_SIZE)
                ui.Choice_remap_TIM12:SetMaxSize(UI_CHOICE_SIZE)
                ui.FlexGridSizer6:Add(ui.Choice_remap_TIM12, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText39 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap miscellaneous", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer6:Add(ui.StaticText39, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_remap_misc = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_MISC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_MISC")
                this:Connect(ID.CHOICE_REMAP_MISC, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
                ui.Choice_remap_misc:Append("No (DMA2 channel 5 interrupt is mapped with DMA2 channel 4 at position 59, TIM5 TRGO event is selected as DAC Trigger 3, TIM5 triggers TIM1/3)")
                ui.Choice_remap_misc:Append("Yes (DMA2 channel 5 interrupt is mapped separately at position 60 and TIM15 TRGO event is selected as DAC Trigger 3, TIM15 triggers TIM1/3)")
                ui.Choice_remap_misc:SetMinSize(UI_CHOICE_SIZE)
                ui.Choice_remap_misc:SetMaxSize(UI_CHOICE_SIZE)
                ui.FlexGridSizer6:Add(ui.Choice_remap_misc, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        end

        if cpu_family == CPU_FAMILY_LD or cpu_family == CPU_FAMILY_MD or cpu_family == CPU_FAMILY_HD or cpu_family == CPU_FAMILY_XL then
                ui.StaticText35 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap TIM9", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer6:Add(ui.StaticText35, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_remap_TIM9 = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_TIM9, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_TIM9")
                this:Connect(ID.CHOICE_REMAP_TIM9, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
                ui.Choice_remap_TIM9:Append("No (TIM9_CH1/PA2 and TIM9_CH2/PA3)")
                ui.Choice_remap_TIM9:Append("Yes (TIM9_CH1/PE5 and TIM9_CH2/PE6)")
                ui.Choice_remap_TIM9:SetMinSize(UI_CHOICE_SIZE)
                ui.Choice_remap_TIM9:SetMaxSize(UI_CHOICE_SIZE)
                ui.FlexGridSizer6:Add(ui.Choice_remap_TIM9, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText36 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap TIM10", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer6:Add(ui.StaticText36, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_remap_TIM10 = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_TIM10, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_TIM10")
                this:Connect(ID.CHOICE_REMAP_TIM10, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
                ui.Choice_remap_TIM10:Append("No (TIM10_CH1/PB8)")
                ui.Choice_remap_TIM10:Append("Yes (TIM10_CH1/PF6)")
                ui.Choice_remap_TIM10:SetMinSize(UI_CHOICE_SIZE)
                ui.Choice_remap_TIM10:SetMaxSize(UI_CHOICE_SIZE)
                ui.FlexGridSizer6:Add(ui.Choice_remap_TIM10, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText37 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap TIM11", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer6:Add(ui.StaticText37, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_remap_TIM11 = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_TIM11, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_TIM11")
                this:Connect(ID.CHOICE_REMAP_TIM11, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
                ui.Choice_remap_TIM11:Append("No (TIM11_CH1/PB9)")
                ui.Choice_remap_TIM11:Append("Yes (TIM11_CH1/PF7)")
                ui.Choice_remap_TIM11:SetMinSize(UI_CHOICE_SIZE)
                ui.Choice_remap_TIM11:SetMaxSize(UI_CHOICE_SIZE)
                ui.FlexGridSizer6:Add(ui.Choice_remap_TIM11, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        end

        if cpu_family == CPU_FAMILY_HD_VL or cpu_family == CPU_FAMILY_LD or cpu_family == CPU_FAMILY_MD or cpu_family == CPU_FAMILY_HD or cpu_family == CPU_FAMILY_XL then
                ui.StaticText31 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap TIM13", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer6:Add(ui.StaticText31, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_remap_TIM13 = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_TIM13, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_TIM13")
                this:Connect(ID.CHOICE_REMAP_TIM13, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
                ui.Choice_remap_TIM13:Append("No (TIM13_CH1/PC8 [VL] or PA6)")
                ui.Choice_remap_TIM13:Append("Yes (TIM13_CH1/PB0 [VL] or PF8)")
                ui.Choice_remap_TIM13:SetMinSize(UI_CHOICE_SIZE)
                ui.Choice_remap_TIM13:SetMaxSize(UI_CHOICE_SIZE)
                ui.FlexGridSizer6:Add(ui.Choice_remap_TIM13, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText32 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "Remap TIM14", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer6:Add(ui.StaticText32, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_remap_TIM14 = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_TIM14, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_TIM14")
                this:Connect(ID.CHOICE_REMAP_TIM14, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
                ui.Choice_remap_TIM14:Append("No (TIM14_CH1/PC9 [VL] or PA7)")
                ui.Choice_remap_TIM14:Append("Yes (TIM14_CH1/PB1 [VL] or PF9)")
                ui.Choice_remap_TIM14:SetMinSize(UI_CHOICE_SIZE)
                ui.Choice_remap_TIM14:SetMaxSize(UI_CHOICE_SIZE)
                ui.FlexGridSizer6:Add(ui.Choice_remap_TIM14, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText33 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "NADV connect", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer6:Add(ui.StaticText33, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_remap_NADV = wx.wxChoice(ui.Panel1, ID.CHOICE_REMAP_NADV, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_REMAP_NADV")
                this:Connect(ID.CHOICE_REMAP_NADV, wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
                ui.Choice_remap_NADV:Append("The NADV signal is connected to the output")
                ui.Choice_remap_NADV:Append("The NADV signal is NOT connected")
                ui.Choice_remap_NADV:SetMinSize(UI_CHOICE_SIZE)
                ui.Choice_remap_NADV:SetMaxSize(UI_CHOICE_SIZE)
                ui.FlexGridSizer6:Add(ui.Choice_remap_NADV, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        end

        -- External Interrupt port configuration
        ui.StaticBoxSizer2:Add(ui.FlexGridSizer6, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.FlexGridSizer2:Add(ui.StaticBoxSizer2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticBoxSizer3 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel1, "External Interrupt port configuration")
        ui.FlexGridSizer7 = wx.wxFlexGridSizer(0, 6, 0, 0)
        for i = 0, 15 do
                ui.StaticText_EXTI[i] = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "EXTI"..i.." port", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer7:Add(ui.StaticText_EXTI[i], 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ID.CHOICE_EXTI[i] = wx.wxNewId()
                ui.Choice_EXTI[i] = wx.wxChoice(ui.Panel1, ID.CHOICE_EXTI[i], wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_EXTI")
                for _, p in ipairs(PORT_LIST) do ui.Choice_EXTI[i]:Append(p..i) end
                ui.FlexGridSizer7:Add(ui.Choice_EXTI[i], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                this:Connect(ID.CHOICE_EXTI[i], wx.wxEVT_COMMAND_CHOICE_SELECTED, event_value_updated)
        end
        ui.StaticBoxSizer3:Add(ui.FlexGridSizer7, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.FlexGridSizer2:Add(ui.StaticBoxSizer3, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Panel1:SetSizer(ui.FlexGridSizer2)
        ui.FlexGridSizer1:Add(ui.Panel1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        --
        this:SetSizer(ui.FlexGridSizer1)
        this:SetScrollRate(50, 50)

        --
        this:Connect(ID.CHECKBOX_MODULE_ENABLE, wx.wxEVT_COMMAND_CHECKBOX_CLICKED, event_checkbox_module_enable_updated)
        this:Connect(ID.CHECKBOX_CEO_ENABLE,    wx.wxEVT_COMMAND_CHECKBOX_CLICKED, event_checkbox_CEO_enable_updated   )

        --
        load_configuration()
        modified:no()

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @param  None
-- @return Module name
--------------------------------------------------------------------------------
function afio:get_window_name()
        return "AFIO"
end


--------------------------------------------------------------------------------
-- @brief  Function is called by parent when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function afio:selected()
end


--------------------------------------------------------------------------------
-- @brief  Function returns modify status
-- @param  None
-- @return If data is modified true is returned, otherwise false
--------------------------------------------------------------------------------
function afio:is_modified()
        return modified:get_value()
end


--------------------------------------------------------------------------------
-- @brief  Function save configuration
-- @return None
--------------------------------------------------------------------------------
function afio:save()
        save_configuration()
end


--------------------------------------------------------------------------------
-- @brief  Function discard modified configuration
-- @return None
--------------------------------------------------------------------------------
function afio:discard()
        load_configuration()
        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Function returns module handler
-- @param  None
-- @return Module handler
--------------------------------------------------------------------------------
function get_handler()
        return afio
end
