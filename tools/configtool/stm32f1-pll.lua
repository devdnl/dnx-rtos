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
local ui               = {}
local ID               = {}
local cpu_name         = nil    -- loaded when creating the window
local cpu_idx          = nil    -- loaded when creating the window
local cpu              = nil    -- loaded when creating the window
local cpu_family       = nil    -- loaded when creating the window
local CPU_FAMILY_CL    = "__STM32F10X_CL__"
local CPU_FAMILY_LD_VL = "__STM32F10X_LD_VL__"
local CPU_FAMILY_MD_VL = "__STM32F10X_MD_VL__"
local CPU_FAMILY_HD_VL = "__STM32F10X_HD_VL__"
local CPU_FAMILY_LD    = "__STM32F10X_LD__"
local CPU_FAMILY_MD    = "__STM32F10X_MD__"
local CPU_FAMILY_HD    = "__STM32F10X_HD__"
local CPU_FAMILY_XL    = "__STM32F10X_XL__"
local LSI_FREQ         = 40e3
local LSE_FREQ         = 32768
local HSI_FREQ         = 8e6
local HSE_FREQ         = -1    -- loaded when creating the window

local LSI_on = {
        {"Disabled", "DISABLE", 0, 0       },
        {"Enabled",  "ENABLE",  1, LSI_FREQ}
}
                
local LSE_on = {
        {"Disabled",                     "RCC_LSE_OFF",    0, 0       },
        {"Enabled",                      "RCC_LSE_ON",     1, LSE_FREQ},
        {"Bypassed with external clock", "RCC_LSE_Bypass", 2, LSE_FREQ}
}
                
local HSE_on = {
        {"Disabled",                     "RCC_HSE_ON",     0, 0       },
        {"Enabled",                      "RCC_HSE_OFF",    1, HSE_FREQ},
        {"Bypassed with external clock", "RCC_HSE_Bypass", 2, HSE_FREQ}
}

local PLL_on = {
        {"Disabled", "DISABLE",  0 , 0 },
        {"x2" , "RCC_PLLMul_2",  1 , 2 },
        {"x3" , "RCC_PLLMul_3",  2 , 3 },
        {"x4" , "RCC_PLLMul_4",  3 , 4 },
        {"x5" , "RCC_PLLMul_5",  4 , 5 },
        {"x6" , "RCC_PLLMul_6",  5 , 6 },
        {"x7" , "RCC_PLLMul_7",  6 , 7 },
        {"x8" , "RCC_PLLMul_8",  7 , 8 },
        {"x9" , "RCC_PLLMul_9",  8 , 9 },
        {"x10", "RCC_PLLMul_10", 9 , 10},
        {"x11", "RCC_PLLMul_11", 10, 11},
        {"x12", "RCC_PLLMul_12", 11, 12},
        {"x13", "RCC_PLLMul_13", 12, 13},
        {"x14", "RCC_PLLMul_14", 13, 14},
        {"x15", "RCC_PLLMul_15", 14, 15},
        {"x16", "RCC_PLLMul_16", 15, 16}
}

local PLL_on_CL = {
        {"Disabled", "DISABLE",    0, 0  },
        {"x4",   "RCC_PLLMul_4",   1, 4  },
        {"x5",   "RCC_PLLMul_5",   2, 5  },
        {"x6",   "RCC_PLLMul_6",   3, 6  },
        {"x6.5", "RCC_PLLMul_6_5", 4, 6.5},
        {"x7",   "RCC_PLLMul_7",   5, 7  },
        {"x8",   "RCC_PLLMul_8",   6, 8  },
        {"x9",   "RCC_PLLMul_9",   7, 9  }
}

local PLL2_on = {
        {"Disabled",          "DISABLE",        0, 0 },
        {"PREDIV2 clock x8",  "RCC_PLL2Mul_8",  1, 8 },
        {"PREDIV2 clock x9",  "RCC_PLL2Mul_9",  2, 9 },
        {"PREDIV2 clock x10", "RCC_PLL2Mul_10", 3, 10},
        {"PREDIV2 clock x11", "RCC_PLL2Mul_11", 4, 11},
        {"PREDIV2 clock x12", "RCC_PLL2Mul_12", 5, 12},
        {"PREDIV2 clock x13", "RCC_PLL2Mul_13", 6, 13},
        {"PREDIV2 clock x14", "RCC_PLL2Mul_14", 7, 14},
        {"PREDIV2 clock x16", "RCC_PLL2Mul_16", 8, 16},
        {"PREDIV2 clock x20", "RCC_PLL2Mul_20", 9, 20}
}

local PLL3_on = {
        {"Disabled",          "DISABLE",        0, 0 },
        {"PREDIV2 clock x8",  "RCC_PLL3Mul_8",  1, 8 },
        {"PREDIV2 clock x9",  "RCC_PLL3Mul_9",  2, 9 },
        {"PREDIV2 clock x10", "RCC_PLL3Mul_10", 3, 10},
        {"PREDIV2 clock x11", "RCC_PLL3Mul_11", 4, 11},
        {"PREDIV2 clock x12", "RCC_PLL3Mul_12", 5, 12},
        {"PREDIV2 clock x13", "RCC_PLL3Mul_13", 6, 13},
        {"PREDIV2 clock x14", "RCC_PLL3Mul_14", 7, 14},
        {"PREDIV2 clock x16", "RCC_PLL3Mul_16", 8, 16},
        {"PREDIV2 clock x20", "RCC_PLL3Mul_20", 9, 20}
}

local PLL_clksrc = {
        {"HSI/2", "RCC_PLLSource_HSI_Div2", 0},
        {"HSE",   "RCC_PLLSource_HSE_Div1", 1},
        {"HSE/2", "RCC_PLLSource_HSE_Div2", 2}
}

local PLL_clksrc_CL = {
        {"HSI/2",         "RCC_PLLSource_HSI_Div2", 0},
        {"PREDIV1 clock", "RCC_PLLSource_PREDIV1",  1}
}

local MCO_clksrc = {
        {"No clock", "RCC_MCO_NoClock", 0},
        {"System clock", "RCC_MCO_SYSCLK", 1},
        {"HSI oscillator clock", "RCC_MCO_HSI", 2},
        {"HSE oscillator clock", "RCC_MCO_HSE", 3},
        {"PLL clock divided by 2", "RCC_MCO_PLLCLK_Div2", 4}
}

local MCO_clksrc_CL = {
        {"No clock", "RCC_MCO_NoClock", 0},
        {"System clock", "RCC_MCO_SYSCLK", 1},
        {"HSI oscillator clock", "RCC_MCO_HSI", 2},
        {"HSE oscillator clock", "RCC_MCO_HSE", 3},
        {"PLL clock divided by 2", "RCC_MCO_PLLCLK_Div2", 4},
        {"PLL2 clock", "RCC_MCO_PLL2CLK", 5},
        {"PLL3 clock divided by 2", "RCC_MCO_PLL3CLK_Div2", 6},
        {"PLL3 clock", "RCC_MCO_PLL3CLK", 7},
        {"External oscillator clock", "RCC_MCO_XT1", 8}
}
                    
local I2S2_clksrc = {
        {"System clock", "RCC_I2S2CLKSource_SYSCLK", 0},
        {"PLL3 VCO clock", "RCC_I2S2CLKSource_PLL3_VCO", 1}
}

local I2S3_clksrc = {
        {"System clock", "RCC_I2S3CLKSource_SYSCLK", 0},
        {"PLL3 VCO clock", "RCC_I2S3CLKSource_PLL3_VCO", 1}
}
                     
local RTC_clksrc = {
        {"LSE oscillator clock",                "RCC_RTCCLKSource_LSE",        0},
        {"LSI oscillator clock",                "RCC_RTCCLKSource_LSI",        1},
        {"HSE oscillator clock divided by 128", "RCC_RTCCLKSource_HSE_Div128", 2}
}

local SYSCLK_clksrc = {
        {"HSI", "RCC_SYSCLKSource_HSI",    0},
        {"HSE", "RCC_SYSCLKSource_HSE",    1},
        {"PLL", "RCC_SYSCLKSource_PLLCLK", 2}
}

local USB_clksrc = {
        {"PLL clock divided by 1",   "RCC_USBCLKSource_PLLCLK_Div1",  0, 1  },
        {"PLL clock divided by 1.5", "RCC_USBCLKSource_PLLCLK_1Div5", 1, 1.5}
}

local USB_clksrc_CL = {
        {"PLL VCO divided by 2", "RCC_OTGFSCLKSource_PLLVCO_Div2", 0, 2},
        {"PLL VCO divided by 3", "RCC_OTGFSCLKSource_PLLVCO_Div3", 1, 3}
}

local PREDIV1_clksrc = {
        {"HSE",  "RCC_PREDIV1_Source_HSE",  0},
        {"PLL2", "RCC_PREDIV1_Source_PLL2", 1}
}

local PREDIV1_val = {
        {"/1",  "RCC_PREDIV1_Div1",  0 , 1 },
        {"/2",  "RCC_PREDIV1_Div2",  1 , 2 },
        {"/3",  "RCC_PREDIV1_Div3",  2 , 3 },
        {"/4",  "RCC_PREDIV1_Div4",  3 , 4 },
        {"/5",  "RCC_PREDIV1_Div5",  4 , 5 },
        {"/6",  "RCC_PREDIV1_Div6",  5 , 6 },
        {"/7",  "RCC_PREDIV1_Div7",  6 , 7 },
        {"/8",  "RCC_PREDIV1_Div8",  7 , 8 },
        {"/9",  "RCC_PREDIV1_Div9",  8 , 9 },
        {"/10", "RCC_PREDIV1_Div10", 9 , 10},
        {"/11", "RCC_PREDIV1_Div11", 10, 11},
        {"/12", "RCC_PREDIV1_Div12", 11, 12},
        {"/13", "RCC_PREDIV1_Div13", 12, 13},
        {"/14", "RCC_PREDIV1_Div14", 13, 14},
        {"/15", "RCC_PREDIV1_Div15", 14, 15},
        {"/16", "RCC_PREDIV1_Div16", 15, 16}
}

local PREDIV2_val = {
        {"HSE/1",  "RCC_PREDIV2_Div1",  0 , 1 },
        {"HSE/2",  "RCC_PREDIV2_Div2",  1 , 2 },
        {"HSE/3",  "RCC_PREDIV2_Div3",  2 , 3 },
        {"HSE/4",  "RCC_PREDIV2_Div4",  3 , 4 },
        {"HSE/5",  "RCC_PREDIV2_Div5",  4 , 5 },
        {"HSE/6",  "RCC_PREDIV2_Div6",  5 , 6 },
        {"HSE/7",  "RCC_PREDIV2_Div7",  6 , 7 },
        {"HSE/8",  "RCC_PREDIV2_Div8",  7 , 8 },
        {"HSE/9",  "RCC_PREDIV2_Div9",  8 , 9 },
        {"HSE/10", "RCC_PREDIV2_Div10", 9 , 10},
        {"HSE/11", "RCC_PREDIV2_Div11", 10, 11},
        {"HSE/12", "RCC_PREDIV2_Div12", 11, 12},
        {"HSE/13", "RCC_PREDIV2_Div13", 12, 13},
        {"HSE/14", "RCC_PREDIV2_Div14", 13, 14},
        {"HSE/15", "RCC_PREDIV2_Div15", 14, 15},
        {"HSE/16", "RCC_PREDIV2_Div16", 15, 16}
}

local AHB_prescaler = {
        {"SYSCLK / 1",   "RCC_SYSCLK_Div1",   0, 1  },
        {"SYSCLK / 2",   "RCC_SYSCLK_Div2",   1, 2  },
        {"SYSCLK / 4",   "RCC_SYSCLK_Div4",   2, 4  },
        {"SYSCLK / 8",   "RCC_SYSCLK_Div8",   3, 8  },
        {"SYSCLK / 16",  "RCC_SYSCLK_Div16",  4, 16 },
        {"SYSCLK / 64",  "RCC_SYSCLK_Div64",  5, 64 },
        {"SYSCLK / 128", "RCC_SYSCLK_Div128", 6, 128},
        {"SYSCLK / 256", "RCC_SYSCLK_Div256", 7, 256},
        {"SYSCLK / 512", "RCC_SYSCLK_Div512", 9, 512}
}

local APB12_prescaler = {
        {"HCLK / 1",  "RCC_HCLK_Div1",  0, 1 },
        {"HCLK / 2",  "RCC_HCLK_Div2",  1, 2 },
        {"HCLK / 4",  "RCC_HCLK_Div4",  2, 4 },
        {"HCLK / 8",  "RCC_HCLK_Div8",  3, 8 },
        {"HCLK / 16", "RCC_HCLK_Div16", 4, 16}
}

local ADC_prescaler = {
        {"PCLK2 / 2", "RCC_PCLK2_Div2", 0, 2},
        {"PCLK2 / 4", "RCC_PCLK2_Div4", 1, 4},
        {"PCLK2 / 6", "RCC_PCLK2_Div6", 2, 6},
        {"PCLK2 / 8", "RCC_PCLK2_Div8", 3, 8}
}


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
        cpu_name   = wizcore:key_read(config.arch.stm32f1.key.CPU_NAME)
        cpu_idx    = wizcore:get_cpu_index("stm32f1", cpu_name)
        cpu        = config.arch.stm32f1.cpulist:Children()[cpu_idx]
        cpu_family = cpu.family:GetValue()
        HSE_FREQ   = tonumber(wizcore:key_read(config.project.key.CPU_OSC_FREQ))

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
        for i = 1, #LSI_on do ui.Choice_LSI:Append(LSI_on[i][1]) end
        ui.FlexGridSizer3:Add(ui.Choice_LSI, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_LSI = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_LSI, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        
        ui.StaticText2 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "LSE oscillator", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText2, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_LSE = wx.wxChoice(ui.Panel1, ID.CHOICE_LSE, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_LSE")
        for i = 1, #LSE_on do ui.Choice_LSE:Append(LSE_on[i][1]) end
        ui.FlexGridSizer3:Add(ui.Choice_LSE, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_LSE = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_LSE, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        
        ui.StaticText3 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "HSE oscillator", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText3, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_HSE = wx.wxChoice(ui.Panel1, ID.CHOICE_HSE, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_HSE")
        for i = 1, #HSE_on do ui.Choice_HSE:Append(HSE_on[i][1]) end
        ui.FlexGridSizer3:Add(ui.Choice_HSE, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_HSE = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_HSE, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        
        if cpu_family == CPU_FAMILY_CL then
                ui.StaticText4 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "PREDIV2", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText4, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_PREDIV2 = wx.wxChoice(ui.Panel1, ID.CHOICE_PREDIV2, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_PREDIV2")
                for i = 1, #PREDIV2_val do ui.Choice_PREDIV2:Append(PREDIV2_val[i][1]) end
                ui.FlexGridSizer3:Add(ui.Choice_PREDIV2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText_PREDIV2 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText_PREDIV2, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                
                ui.StaticText5 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "PLL2", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText5, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_PLL2 = wx.wxChoice(ui.Panel1, ID.CHOICE_PLL2, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_PLL2")
                for i = 1, #PLL2_on do ui.Choice_PLL2:Append(PLL2_on[i][1]) end
                ui.FlexGridSizer3:Add(ui.Choice_PLL2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText_PLL2 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText_PLL2, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                
                ui.StaticText6 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "PLL3", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText6, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_PLL3 = wx.wxChoice(ui.Panel1, ID.CHOICE_PLL3, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_PLL3")
                for i = 1, #PLL3_on do ui.Choice_PLL3:Append(PLL3_on[i][1]) end
                ui.FlexGridSizer3:Add(ui.Choice_PLL3, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText_PLL3 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText_PLL3, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        
                ui.StaticText8 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "PREDIV1", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText8, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.BoxSizer2 = wx.wxBoxSizer(wx.wxHORIZONTAL)
                ui.Choice_PREDIV1_clksrc = wx.wxChoice(ui.Panel1, ID.CHOICE_PREDIV1_CLKSRC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_PREDIV1_CLKSRC")
                for i = 1, #PREDIV1_clksrc do ui.Choice_PREDIV1_clksrc:Append(PREDIV1_clksrc[i][1]) end
                ui.Choice_PREDIV1_clksrc:SetToolTip("PREDIV1 clock source")
                ui.BoxSizer2:Add(ui.Choice_PREDIV1_clksrc, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_PREDIV1_value = wx.wxChoice(ui.Panel1, ID.CHOICE_PREDIV1_VALUE, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_PREDIV1_VALUE")
                for i = 1, #PREDIV1_val do ui.Choice_PREDIV1_value:Append(PREDIV1_val[i][1]) end
                ui.Choice_PREDIV1_value:SetToolTip("PREDIV1 prescaler value")
                ui.BoxSizer2:Add(ui.Choice_PREDIV1_value, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer3:Add(ui.BoxSizer2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.StaticText_PREDIV1_value = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText_PREDIV1_value, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        end
        
        ui.StaticText10 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "PLL", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText10, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.BoxSizer1 = wx.wxBoxSizer(wx.wxHORIZONTAL)
        ui.Choice_PLL_clksrc = wx.wxChoice(ui.Panel1, ID.CHOICE_PLL_CLKSRC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_PLL_CLKSRC")
        local PLL_clksrc_table = ifs(cpu_family == CPU_FAMILY_CL, PLL_clksrc_CL, PLL_clksrc)
        for i = 1, #PLL_clksrc_table do ui.Choice_PLL_clksrc:Append(PLL_clksrc_table[i][1]) end
        ui.Choice_PLL_clksrc:SetToolTip("PLL clock source")
        ui.BoxSizer1:Add(ui.Choice_PLL_clksrc, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_PLL = wx.wxChoice(ui.Panel1, ID.CHOICE_PLL, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_PLL")
        local PLL_on_table = ifs(cpu_family == CPU_FAMILY_CL, PLL_on_CL, PLL_on)
        for i = 1, #PLL_on_table do ui.Choice_PLL:Append(PLL_on_table[i][1]) end
        ui.Choice_PLL:SetToolTip("PLL multiplier")
        ui.BoxSizer1:Add(ui.Choice_PLL, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.FlexGridSizer3:Add(ui.BoxSizer1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.StaticText_PLL = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_PLL, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        
        ui.StaticText11 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "System clock source", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText11, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_system_clksrc = wx.wxChoice(ui.Panel1, ID.CHOICE_SYSTEM_CLKSRC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_SYSTEM_CLKSRC")
        for i = 1, #SYSCLK_clksrc do ui.Choice_system_clksrc:Append(SYSCLK_clksrc[i][1]) end
        ui.FlexGridSizer3:Add(ui.Choice_system_clksrc, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_system_clksrc = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_system_clksrc, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        
        ui.StaticText12 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "RTC clock source", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText12, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_RTC_clksrc = wx.wxChoice(ui.Panel1, ID.CHOICE_RTC_CLKSRC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_RTC_CLKSRC")
        for i = 1, #RTC_clksrc do ui.Choice_RTC_clksrc:Append(RTC_clksrc[i][1]) end
        ui.FlexGridSizer3:Add(ui.Choice_RTC_clksrc, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_RTC_clksrc = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_RTC_clksrc, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        
        ui.StaticText13 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "MCO clock source", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText13, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_MCO_clksrc = wx.wxChoice(ui.Panel1, ID.CHOICE_MCO_CLKSRC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_MCO_CLKSRC")
        local MCO_clksrc_table = ifs(cpu_family == CPU_FAMILY_CL, MCO_clksrc_CL, MCO_clksrc)
        for i = 1, #MCO_clksrc_table do ui.Choice_MCO_clksrc:Append(MCO_clksrc_table[i][1]) end
        ui.FlexGridSizer3:Add(ui.Choice_MCO_clksrc, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_MCO_clksrc = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_MCO_clksrc, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        
        ui.StaticText14 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "USB clock source", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText14, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_USB_clksrc = wx.wxChoice(ui.Panel1, ID.CHOICE_USB_CLKSRC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_USB_CLKSRC")
        local USB_div_table = ifs(cpu_family == CPU_FAMILY_CL, USB_clksrc_CL, USB_clksrc)
        for i = 1, #USB_div_table do ui.Choice_USB_clksrc:Append(USB_div_table[i][1]) end
        ui.FlexGridSizer3:Add(ui.Choice_USB_clksrc, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_USB_clksrc = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_USB_clksrc, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        
        if cpu_family == CPU_FAMILY_CL then
                ui.StaticText15 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "I2S2 clock source", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText15, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_I2S2_clksrc = wx.wxChoice(ui.Panel1, ID.CHOICE_I2S2_CLKSRC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_I2S2_CLKSRC")
                for i = 1, #I2S2_clksrc do ui.Choice_I2S2_clksrc:Append(I2S2_clksrc[i][1]) end
                ui.FlexGridSizer3:Add(ui.Choice_I2S2_clksrc, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText_I2S2_clksrc = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText_I2S2_clksrc, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        
                ui.StaticText16 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "I2S3 clock source", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText16, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_I2S3_clksrc = wx.wxChoice(ui.Panel1, ID.CHOICE_I2S3_CLKSRC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_I2S3_CLKSRC")
                for i = 1, #I2S3_clksrc do ui.Choice_I2S3_clksrc:Append(I2S3_clksrc[i][1]) end
                ui.FlexGridSizer3:Add(ui.Choice_I2S3_clksrc, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText_I2S3_clksrc = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText_I2S3_clksrc, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        end
        
        ui.StaticText17 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "AHB prescaler", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText17, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_AHB_prescaler = wx.wxChoice(ui.Panel1, ID.CHOICE_AHB_PRESCALER, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_AHB_PRESCALER")
        for i = 1, #AHB_prescaler do ui.Choice_AHB_prescaler:Append(AHB_prescaler[i][1]) end
        ui.FlexGridSizer3:Add(ui.Choice_AHB_prescaler, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_AHB_prescaler = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_AHB_prescaler, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        
        ui.StaticText18 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "APB1 prescaler", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText18, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_APB1_prescaler = wx.wxChoice(ui.Panel1, ID.CHOICE_APB1_PRESCALER, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_APB1_PRESCALER")
        for i = 1, #APB12_prescaler do ui.Choice_APB1_prescaler:Append(APB12_prescaler[i][1]) end
        ui.FlexGridSizer3:Add(ui.Choice_APB1_prescaler, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_APB1_prescaler = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_APB1_prescaler, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        
        ui.StaticText19 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "APB2 prescaler", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText19, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_APB2_prescaler = wx.wxChoice(ui.Panel1, ID.CHOICE_APB2_PRESCALER, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_APB2_PRESCALER")
        for i = 1, #APB12_prescaler do ui.Choice_APB2_prescaler:Append(APB12_prescaler[i][1]) end
        ui.FlexGridSizer3:Add(ui.Choice_APB2_prescaler, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_APB2_prescaler = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_APB2_prescaler, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        
        ui.StaticText20 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "ADC prescaler", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText20, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_ADC_prescaler = wx.wxChoice(ui.Panel1, ID.CHOICE_ADC_PRESCALER, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_ADC_PRESCALER")
        for i = 1, #ADC_prescaler do ui.Choice_ADC_prescaler:Append(ADC_prescaler[i][1]) end
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
