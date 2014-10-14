--[[============================================================================
@file    pll.lua

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
require("modules/ctcore")


--==============================================================================
-- GLOBAL OBJECTS
--==============================================================================
-- module's main object
pll = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local modified         = ct:new_modify_indicator()
local ui               = {}
local ID               = {}
local cpu_name         = nil    -- loaded when creating the window
local cpu_idx          = nil    -- loaded when creating the window
local cpu              = nil    -- loaded when creating the window
local cpu_family       = {}
cpu_family.value       = 0
cpu_family.is_LD_VL    = function(self) return self.value == config.arch.stm32f1.def.STM32F10X_LD_VL:GetValue() end
cpu_family.is_MD_VL    = function(self) return self.value == config.arch.stm32f1.def.STM32F10X_MD_VL:GetValue() end
cpu_family.is_HD_VL    = function(self) return self.value == config.arch.stm32f1.def.STM32F10X_HD_VL:GetValue() end
cpu_family.is_VL       = function(self) return self.value:match("_VL_") end
cpu_family.is_LD       = function(self) return self.value == config.arch.stm32f1.def.STM32F10X_LD:GetValue() end
cpu_family.is_MD       = function(self) return self.value == config.arch.stm32f1.def.STM32F10X_MD:GetValue() end
cpu_family.is_HD       = function(self) return self.value == config.arch.stm32f1.def.STM32F10X_HD:GetValue() end
cpu_family.is_XL       = function(self) return self.value == config.arch.stm32f1.def.STM32F10X_XL:GetValue() end
cpu_family.is_CL       = function(self) return self.value == config.arch.stm32f1.def.STM32F10X_CL:GetValue() end
local LSI_FREQ         = 40e3
local LSE_FREQ         = 32768
local HSI_FREQ         = 8e6
local HSE_FREQ         = -1    -- loaded when creating the window
local ENABLE           = "ENABLE"
local DISABLE          = "DISABLE"

local LSI_on = {
        {name = "Disabled", key = "DISABLE", value = 0       },
        {name = "Enabled",  key = "ENABLE",  value = LSI_FREQ}
}

local LSE_on = {
        {name = "Disabled",                     key = "RCC_LSE_OFF",    value = 0       },
        {name = "Enabled",                      key = "RCC_LSE_ON",     value = LSE_FREQ},
        {name = "Bypassed with external clock", key = "RCC_LSE_Bypass", value = LSE_FREQ}
}

local HSE_on = {
        {name = "Disabled",                     key = "RCC_HSE_OFF",    value = 0       },
        {name = "Enabled",                      key = "RCC_HSE_ON",     value = HSE_FREQ},
        {name = "Bypassed with external clock", key = "RCC_HSE_Bypass", value = HSE_FREQ}
}

local PLL_on = {
        {name = "Disabled", key = "DISABLE",       value = 0 },
        {name = "x2" ,      key = "RCC_PLLMul_2",  value = 2 },
        {name = "x3" ,      key = "RCC_PLLMul_3",  value = 3 },
        {name = "x4" ,      key = "RCC_PLLMul_4",  value = 4 },
        {name = "x5" ,      key = "RCC_PLLMul_5",  value = 5 },
        {name = "x6" ,      key = "RCC_PLLMul_6",  value = 6 },
        {name = "x7" ,      key = "RCC_PLLMul_7",  value = 7 },
        {name = "x8" ,      key = "RCC_PLLMul_8",  value = 8 },
        {name = "x9" ,      key = "RCC_PLLMul_9",  value = 9 },
        {name = "x10",      key = "RCC_PLLMul_10", value = 10},
        {name = "x11",      key = "RCC_PLLMul_11", value = 11},
        {name = "x12",      key = "RCC_PLLMul_12", value = 12},
        {name = "x13",      key = "RCC_PLLMul_13", value = 13},
        {name = "x14",      key = "RCC_PLLMul_14", value = 14},
        {name = "x15",      key = "RCC_PLLMul_15", value = 15},
        {name = "x16",      key = "RCC_PLLMul_16", value = 16}
}

local PLL_on_CL = {
        {name = "Disabled", key = "DISABLE",        value = 0  },
        {name = "x4",       key = "RCC_PLLMul_4",   value = 4  },
        {name = "x5",       key = "RCC_PLLMul_5",   value = 5  },
        {name = "x6",       key = "RCC_PLLMul_6",   value = 6  },
        {name = "x6.5",     key = "RCC_PLLMul_6_5", value = 6.5},
        {name = "x7",       key = "RCC_PLLMul_7",   value = 7  },
        {name = "x8",       key = "RCC_PLLMul_8",   value = 8  },
        {name = "x9",       key = "RCC_PLLMul_9",   value = 9  }
}

local PLL2_on = {
        {name = "Disabled",          key = "DISABLE",        value = 0 },
        {name = "PREDIV2 clock x8",  key = "RCC_PLL2Mul_8",  value = 8 },
        {name = "PREDIV2 clock x9",  key = "RCC_PLL2Mul_9",  value = 9 },
        {name = "PREDIV2 clock x10", key = "RCC_PLL2Mul_10", value = 10},
        {name = "PREDIV2 clock x11", key = "RCC_PLL2Mul_11", value = 11},
        {name = "PREDIV2 clock x12", key = "RCC_PLL2Mul_12", value = 12},
        {name = "PREDIV2 clock x13", key = "RCC_PLL2Mul_13", value = 13},
        {name = "PREDIV2 clock x14", key = "RCC_PLL2Mul_14", value = 14},
        {name = "PREDIV2 clock x16", key = "RCC_PLL2Mul_16", value = 16},
        {name = "PREDIV2 clock x20", key = "RCC_PLL2Mul_20", value = 20}
}

local PLL3_on = {
        {name = "Disabled",          key = "DISABLE",        value = 0 },
        {name = "PREDIV2 clock x8",  key = "RCC_PLL3Mul_8",  value = 8 },
        {name = "PREDIV2 clock x9",  key = "RCC_PLL3Mul_9",  value = 9 },
        {name = "PREDIV2 clock x10", key = "RCC_PLL3Mul_10", value = 10},
        {name = "PREDIV2 clock x11", key = "RCC_PLL3Mul_11", value = 11},
        {name = "PREDIV2 clock x12", key = "RCC_PLL3Mul_12", value = 12},
        {name = "PREDIV2 clock x13", key = "RCC_PLL3Mul_13", value = 13},
        {name = "PREDIV2 clock x14", key = "RCC_PLL3Mul_14", value = 14},
        {name = "PREDIV2 clock x16", key = "RCC_PLL3Mul_16", value = 16},
        {name = "PREDIV2 clock x20", key = "RCC_PLL3Mul_20", value = 20}
}

local PLL_clksrc = {
        {name = "HSI/2", key = "RCC_PLLSource_HSI_Div2"},
        {name = "HSE",   key = "RCC_PLLSource_HSE_Div1"},
        {name = "HSE/2", key = "RCC_PLLSource_HSE_Div2"}
}

local PLL_clksrc_CL = {
        {name = "HSI/2",         key = "RCC_PLLSource_HSI_Div2"},
        {name = "PREDIV1 clock", key = "RCC_PLLSource_PREDIV1" }
}

local MCO_clksrc = {
        {name = "No clock",               key = "RCC_MCO_NoClock"    },
        {name = "System clock",           key = "RCC_MCO_SYSCLK"     },
        {name = "HSI oscillator clock",   key = "RCC_MCO_HSI"        },
        {name = "HSE oscillator clock",   key = "RCC_MCO_HSE"        },
        {name = "PLL clock divided by 2", key = "RCC_MCO_PLLCLK_Div2"}
}

local MCO_clksrc_CL = {
        {name = "No clock",                  key = "RCC_MCO_NoClock"     },
        {name = "System clock",              key = "RCC_MCO_SYSCLK"      },
        {name = "HSI oscillator clock",      key = "RCC_MCO_HSI"         },
        {name = "HSE oscillator clock",      key = "RCC_MCO_HSE"         },
        {name = "PLL clock divided by 2",    key = "RCC_MCO_PLLCLK_Div2" },
        {name = "PLL2 clock",                key = "RCC_MCO_PLL2CLK"     },
        {name = "PLL3 clock divided by 2",   key = "RCC_MCO_PLL3CLK_Div2"},
        {name = "PLL3 clock",                key = "RCC_MCO_PLL3CLK"     },
        {name = "External oscillator clock", key = "RCC_MCO_XT1"         }
}

local I2S2_clksrc = {
        {name = "System clock",   key = "RCC_I2S2CLKSource_SYSCLK"  },
        {name = "PLL3 VCO clock", key = "RCC_I2S2CLKSource_PLL3_VCO"}
}

local I2S3_clksrc = {
        {name = "System clock",   key = "RCC_I2S3CLKSource_SYSCLK"  },
        {name = "PLL3 VCO clock", key = "RCC_I2S3CLKSource_PLL3_VCO"}
}

local RTC_clksrc = {
        {name = "LSE oscillator clock",                key = "RCC_RTCCLKSource_LSE"       },
        {name = "LSI oscillator clock",                key = "RCC_RTCCLKSource_LSI"       },
        {name = "HSE oscillator clock divided by 128", key = "RCC_RTCCLKSource_HSE_Div128"}
}

local SYSCLK_clksrc = {
        {name = "HSI", key = "RCC_SYSCLKSource_HSI"   },
        {name = "HSE", key = "RCC_SYSCLKSource_HSE"   },
        {name = "PLL", key = "RCC_SYSCLKSource_PLLCLK"}
}

local USB_clksrc = {
        {name = "PLL clock divided by 1",   key = "RCC_USBCLKSource_PLLCLK_Div1",  value = 1  },
        {name = "PLL clock divided by 1.5", key = "RCC_USBCLKSource_PLLCLK_1Div5", value = 1.5}
}

local USB_clksrc_CL = {
        {name = "PLL VCO divided by 2", key = "RCC_OTGFSCLKSource_PLLVCO_Div2", value = 2},
        {name = "PLL VCO divided by 3", key = "RCC_OTGFSCLKSource_PLLVCO_Div3", value = 3}
}

local PREDIV1_clksrc = {
        {name = "HSE",  key = "RCC_PREDIV1_Source_HSE" },
        {name = "PLL2", key = "RCC_PREDIV1_Source_PLL2"}
}

local PREDIV1_val = {
        {name = "/1",  key = "RCC_PREDIV1_Div1",  value = 1 },
        {name = "/2",  key = "RCC_PREDIV1_Div2",  value = 2 },
        {name = "/3",  key = "RCC_PREDIV1_Div3",  value = 3 },
        {name = "/4",  key = "RCC_PREDIV1_Div4",  value = 4 },
        {name = "/5",  key = "RCC_PREDIV1_Div5",  value = 5 },
        {name = "/6",  key = "RCC_PREDIV1_Div6",  value = 6 },
        {name = "/7",  key = "RCC_PREDIV1_Div7",  value = 7 },
        {name = "/8",  key = "RCC_PREDIV1_Div8",  value = 8 },
        {name = "/9",  key = "RCC_PREDIV1_Div9",  value = 9 },
        {name = "/10", key = "RCC_PREDIV1_Div10", value = 10},
        {name = "/11", key = "RCC_PREDIV1_Div11", value = 11},
        {name = "/12", key = "RCC_PREDIV1_Div12", value = 12},
        {name = "/13", key = "RCC_PREDIV1_Div13", value = 13},
        {name = "/14", key = "RCC_PREDIV1_Div14", value = 14},
        {name = "/15", key = "RCC_PREDIV1_Div15", value = 15},
        {name = "/16", key = "RCC_PREDIV1_Div16", value = 16}
}

local PREDIV2_val = {
        {name = "HSE/1",  key = "RCC_PREDIV2_Div1",  value = 1 },
        {name = "HSE/2",  key = "RCC_PREDIV2_Div2",  value = 2 },
        {name = "HSE/3",  key = "RCC_PREDIV2_Div3",  value = 3 },
        {name = "HSE/4",  key = "RCC_PREDIV2_Div4",  value = 4 },
        {name = "HSE/5",  key = "RCC_PREDIV2_Div5",  value = 5 },
        {name = "HSE/6",  key = "RCC_PREDIV2_Div6",  value = 6 },
        {name = "HSE/7",  key = "RCC_PREDIV2_Div7",  value = 7 },
        {name = "HSE/8",  key = "RCC_PREDIV2_Div8",  value = 8 },
        {name = "HSE/9",  key = "RCC_PREDIV2_Div9",  value = 9 },
        {name = "HSE/10", key = "RCC_PREDIV2_Div10", value = 10},
        {name = "HSE/11", key = "RCC_PREDIV2_Div11", value = 11},
        {name = "HSE/12", key = "RCC_PREDIV2_Div12", value = 12},
        {name = "HSE/13", key = "RCC_PREDIV2_Div13", value = 13},
        {name = "HSE/14", key = "RCC_PREDIV2_Div14", value = 14},
        {name = "HSE/15", key = "RCC_PREDIV2_Div15", value = 15},
        {name = "HSE/16", key = "RCC_PREDIV2_Div16", value = 16}
}

local AHB_prescaler = {
        {name = "SYSCLK / 1",   key = "RCC_SYSCLK_Div1",   value = 1  },
        {name = "SYSCLK / 2",   key = "RCC_SYSCLK_Div2",   value = 2  },
        {name = "SYSCLK / 4",   key = "RCC_SYSCLK_Div4",   value = 4  },
        {name = "SYSCLK / 8",   key = "RCC_SYSCLK_Div8",   value = 8  },
        {name = "SYSCLK / 16",  key = "RCC_SYSCLK_Div16",  value = 16 },
        {name = "SYSCLK / 64",  key = "RCC_SYSCLK_Div64",  value = 64 },
        {name = "SYSCLK / 128", key = "RCC_SYSCLK_Div128", value = 128},
        {name = "SYSCLK / 256", key = "RCC_SYSCLK_Div256", value = 256},
        {name = "SYSCLK / 512", key = "RCC_SYSCLK_Div512", value = 512}
}

local APB12_prescaler = {
        {name = "HCLK / 1",  key = "RCC_HCLK_Div1",  value = 1 },
        {name = "HCLK / 2",  key = "RCC_HCLK_Div2",  value = 2 },
        {name = "HCLK / 4",  key = "RCC_HCLK_Div4",  value = 4 },
        {name = "HCLK / 8",  key = "RCC_HCLK_Div8",  value = 8 },
        {name = "HCLK / 16", key = "RCC_HCLK_Div16", value = 16}
}

local ADC_prescaler = {
        {name = "PCLK2 / 2", key = "RCC_PCLK2_Div2", value = 2},
        {name = "PCLK2 / 4", key = "RCC_PCLK2_Div4", value = 4},
        {name = "PCLK2 / 6", key = "RCC_PCLK2_Div6", value = 6},
        {name = "PCLK2 / 8", key = "RCC_PCLK2_Div8", value = 8}
}


--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function returns string suitable for labels that show calculated frequencies
-- @param  freq         frequency to print (Hz)
-- @param  str          string before value (optional)
-- @return Converted frequnecy to string
--------------------------------------------------------------------------------
local function freqlabel(freq, str)
        if type(str) == "string" then
                return string.format(str, ct:print_freq(freq))
        else
                return ct:print_freq(freq)
        end
end


--------------------------------------------------------------------------------
-- @brief  Function search index of item in the selected table
-- @param  tab          table to find for
-- @param  field        field to inspect
-- @param  value        value to compare
-- @return On success index is returned, otherwise 0.
--------------------------------------------------------------------------------
local function get_table_index(tab, field, value)
        for i = 1, #tab do
                if tab[i][field] == value then
                        return i
                end
        end

        return 0
end


--------------------------------------------------------------------------------
-- @brief  Calculate frequencies
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function calculate_frequencies()
        local PLLSRC         if ui.Choice_PLL_clksrc then PLLSRC = ifs(cpu_family:is_CL(), PLL_clksrc_CL, PLL_clksrc)[ui.Choice_PLL_clksrc:GetSelection() + 1].key end
        local PLLMUL         if ui.Choice_PLL then PLLMUL = ifs(cpu_family:is_CL(), PLL_on_CL, PLL_on)[ui.Choice_PLL:GetSelection() + 1].value end
        local PLL2MUL        if ui.Choice_PLL2 then PLL2MUL = ifs(cpu_family:is_CL(), PLL2_on[ui.Choice_PLL2:GetSelection() + 1].value, nil) end
        local PLL3MUL        if ui.Choice_PLL3 then PLL3MUL = ifs(cpu_family:is_CL(), PLL3_on[ui.Choice_PLL3:GetSelection() + 1].value, nil) end
        local PREDIV1SRC     if ui.Choice_PREDIV1_clksrc then PREDIV1SRC = ifs(cpu_family:is_CL(), PREDIV1_clksrc[ui.Choice_PREDIV1_clksrc:GetSelection() + 1].key, nil) end
        local PREDIV1        if ui.Choice_PREDIV1_value then PREDIV1 = ifs(cpu_family:is_CL(), PREDIV1_val[ui.Choice_PREDIV1_value:GetSelection() + 1].value, nil) end
        local PREDIV2        if ui.Choice_PREDIV2 then PREDIV2 = ifs(cpu_family:is_CL(), PREDIV2_val[ui.Choice_PREDIV2:GetSelection() + 1].value, nil) end
        local I2S2SEL        if ui.Choice_I2S2_clksrc then I2S2SEL = ifs(cpu_family:is_CL(), I2S2_clksrc[ui.Choice_I2S2_clksrc:GetSelection() + 1].key, nil) end
        local I2S3SEL        if ui.Choice_I2S3_clksrc then I2S3SEL = ifs(cpu_family:is_CL(), I2S3_clksrc[ui.Choice_I2S3_clksrc:GetSelection() + 1].key, nil) end
        local MCO_source     = ifs(cpu_family:is_CL(), MCO_clksrc_CL, MCO_clksrc)[ui.Choice_MCO_clksrc:GetSelection() + 1].key
        local HSE            = HSE_on[ui.Choice_HSE:GetSelection() + 1].value
        local LSE            = LSE_on[ui.Choice_LSE:GetSelection() + 1].value
        local LSI            = LSI_on[ui.Choice_LSI:GetSelection() + 1].value
        local SW             = SYSCLK_clksrc[ui.Choice_system_clksrc:GetSelection() + 1].key
        local RTCSEL         = RTC_clksrc[ui.Choice_RTC_clksrc:GetSelection() + 1].key
        local AHB_prescaler  = AHB_prescaler[ui.Choice_AHB_prescaler:GetSelection() + 1].value
        local APB1_prescaler = APB12_prescaler[ui.Choice_APB1_prescaler:GetSelection() + 1].value
        local APB2_prescaler = APB12_prescaler[ui.Choice_APB2_prescaler:GetSelection() + 1].value
        local ADC_prescaler  = ADC_prescaler[ui.Choice_ADC_prescaler:GetSelection() + 1].value

        local USB_prescaler
        if cpu.peripherals.USB ~= nil or cpu.peripherals.USBOTG then
                USB_prescaler = ifs(cpu_family:is_CL(), USB_clksrc_CL, USB_clksrc)[ui.Choice_USB_clksrc:GetSelection() + 1].value
        end

        local freq         = {}
        freq.HSE           = HSE
        freq.HSI           = HSI_FREQ
        freq.LSI           = LSI
        freq.LSE           = LSE
        freq.IWDGCLK       = freq.LSI
        freq.FLITFCLK      = freq.HSI
        freq.RTCCLK        = 0
        freq.PLLCLK        = 0
        freq.PLLVCO        = 0
        freq.USBCLK        = 0
        freq.I2S2CLK       = 0
        freq.I2S3CLK       = 0
        freq.SYSCLK        = 0
        freq.HCLK          = 0
        freq.FCLK          = 0
        freq.PCLK1         = 0
        freq.PCLK2         = 0
        freq.ADCCLK        = 0
        freq.TIMxCLK1      = 0
        freq.TIMxCLK2      = 0
        freq.MCOCLK        = 0
        freq.PLL2CLK       = 0
        freq.PLL3CLK       = 0
        freq.PLL3VCO       = 0
        freq.PREDIV1CLK    = 0
        freq.PREDIV2CLK    = 0
        freq.flash_latency = 0

        -- calculate RTCCLK
        if RTCSEL == "RCC_RTCCLKSource_LSI" then
                freq.RTCCLK = freq.LSI
        elseif RTCSEL == "RCC_RTCCLKSource_LSE" then
                freq.RTCCLK = freq.LSE
        else
                freq.RTCCLK = freq.HSE / 128
        end

        -- calculate Conectivity Linie-specific frequencies
        if cpu_family:is_CL() then
                freq.PLL2CLK    = (freq.HSE / PREDIV2) * PLL2MUL
                freq.PLL3CLK    = (freq.HSE / PREDIV2) * PLL3MUL
                freq.PLL3VCO    = freq.PLL3CLK * 2
                freq.PREDIV2CLK = freq.HSE / PREDIV2
                freq.PREDIV1CLK = ifs(PREDIV1SRC == "RCC_PREDIV1_Source_HSE", freq.HSE / PREDIV1, freq.PLL2CLK / PREDIV1)
        end

        -- calculate PLL Clk frequency
        if PLLSRC == "RCC_PLLSource_HSI_Div2" then
                freq.PLLCLK = (freq.HSI / 2) * PLLMUL
        else
                if cpu_family:is_CL() then
                        freq.PLLCLK     = freq.PREDIV1CLK * PLLMUL
                else
                        freq.PLLCLK = ifs(PLLSRC == "RCC_PLLSource_HSE_Div2", (freq.HSE / 2) * PLLMUL, freq.HSE * PLLMUL)
                end
        end
        freq.PLLVCO = 2 * freq.PLLCLK

        -- calculate SYSCLK
        if SW == "RCC_SYSCLKSource_HSE" then
                freq.SYSCLK = freq.HSE
        elseif SW == "RCC_SYSCLKSource_HSI" then
                freq.SYSCLK = freq.HSI
        else
                freq.SYSCLK = freq.PLLCLK
        end

        -- calculate USB frequency (if USB exist)
        if cpu.peripherals.USB ~= nil or cpu.peripherals.USBOTG then
                freq.USBCLK = ifs(cpu_family:is_CL(), freq.PLLVCO / USB_prescaler, freq.PLLCLK / USB_prescaler)
        end

        -- calculate I2S frequency
        freq.I2S2CLK = ifs(I2S2SEL == "RCC_I2S2CLKSource_PLL3_VCO" and cpu_family:is_CL(), freq.PLL3VCO, freq.SYSCLK)
        freq.I2S3CLK = ifs(I2S3SEL == "RCC_I2S3CLKSource_PLL3_VCO" and cpu_family:is_CL(), freq.PLL3VCO, freq.SYSCLK)

        -- calculate MCO frequency
        if MCO_source == "RCC_MCO_NoClock" then
                freq.MCOCLK = 0
        elseif MCO_source == "RCC_MCO_SYSCLK" then
                freq.MCOCLK = freq.SYSCLK
        elseif MCO_source == "RCC_MCO_HSI" then
                freq.MCOCLK = freq.HSI
        elseif MCO_source == "RCC_MCO_HSE" then
                freq.MCOCLK = freq.HSE
        elseif MCO_source == "RCC_MCO_PLLCLK_Div2" then
                freq.MCOCLK = freq.PLLCLK / 2
        elseif MCO_source == "RCC_MCO_PLL2CLK" then
                freq.MCOCLK = freq.PLL2CLK
        elseif MCO_source == "RCC_MCO_PLL3CLK_Div2" then
                freq.MCOCLK = freq.PLL3CLK / 2
        elseif MCO_source == "RCC_MCO_XT1" then
                freq.MCOCLK = freq.HSE
        elseif MCO_source == "RCC_MCO_PLL3CLK" then
                freq.MCOCLK = freq.PLL3CLK
        end

        -- calculate some output clocks
        freq.HCLK     = freq.SYSCLK / AHB_prescaler
        freq.FCLK     = freq.HCLK
        freq.PCLK1    = freq.HCLK / APB1_prescaler
        freq.PCLK2    = freq.HCLK / APB2_prescaler
        freq.ADCCLK   = freq.PCLK2 / ADC_prescaler
        freq.TIMxCLK1 = ifs(APB1_prescaler == "RCC_HCLK_Div1", freq.PCLK1, freq.PCLK1 * 2)
        freq.TIMxCLK2 = ifs(APB2_prescaler == "RCC_HCLK_Div1", freq.PCLK1, freq.PCLK1 * 2)

        -- calculate flash latency
        if freq.SYSCLK <= 24e6 then
                freq.flash_latency = 0
        elseif freq.SYSCLK <= 48e6 then
                freq.flash_latency = 1
        elseif freq.SYSCLK <= 72e6 then
                freq.flash_latency = 2
        else
                freq.flash_latency = 2
        end

        return freq
end

--------------------------------------------------------------------------------
-- @brief  Function loads controls from configuration files
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_configuration()
        -- load module enable status
        local module_enable = ct:get_module_state("PLL")
        ui.CheckBox_module_enable:SetValue(module_enable)
        ui.Panel1:Enable(module_enable)

        -- load Conectivity Line specific controls
        if cpu_family:is_CL() then
                if ct:key_read(config.arch.stm32f1.key.PLL_PLL_ON) == DISABLE then
                        ui.Choice_PLL:SetSelection(0)
                else
                        ui.Choice_PLL:SetSelection(get_table_index(PLL_on_CL, "key", ct:key_read(config.arch.stm32f1.key.PLL_PLL_MUL_CL)) - 1)
                end

                if ct:key_read(config.arch.stm32f1.key.PLL_PLL2_ON) == DISABLE then
                        ui.Choice_PLL2:SetSelection(0)
                else
                        ui.Choice_PLL2:SetSelection(get_table_index(PLL2_on, "key", ct:key_read(config.arch.stm32f1.key.PLL_PLL2_MUL)) - 1)
                end

                if ct:key_read(config.arch.stm32f1.key.PLL_PLL3_ON) == DISABLE then
                        ui.Choice_PLL3:SetSelection(0)
                else
                        ui.Choice_PLL3:SetSelection(get_table_index(PLL3_on, "key", ct:key_read(config.arch.stm32f1.key.PLL_PLL3_MUL)) - 1)
                end

                if cpu.peripherals.USB ~= nil or cpu.peripherals.USBOTG then
                        ui.Choice_USB_clksrc:SetSelection(get_table_index(USB_clksrc_CL, "key", ct:key_read(config.arch.stm32f1.key.PLL_USB_DIV_CL)) - 1)
                end

                ui.Choice_PLL_clksrc:SetSelection(get_table_index(PLL_clksrc_CL, "key", ct:key_read(config.arch.stm32f1.key.PLL_PLL_SRC_CL)) - 1)
                ui.Choice_PREDIV2:SetSelection(get_table_index(PREDIV2_val, "key", ct:key_read(config.arch.stm32f1.key.PLL_PREDIV2_VAL)) - 1)
                ui.Choice_PREDIV1_clksrc:SetSelection(get_table_index(PREDIV1_clksrc, "key", ct:key_read(config.arch.stm32f1.key.PLL_PREDIV1_SRC)) - 1)
                ui.Choice_PREDIV1_value:SetSelection(get_table_index(PREDIV1_val, "key", ct:key_read(config.arch.stm32f1.key.PLL_PREDIV1_VAL)) - 1)
                ui.Choice_I2S2_clksrc:SetSelection(get_table_index(I2S2_clksrc, "key", ct:key_read(config.arch.stm32f1.key.PLL_I2S2_SRC)) - 1)
                ui.Choice_I2S3_clksrc:SetSelection(get_table_index(I2S3_clksrc, "key", ct:key_read(config.arch.stm32f1.key.PLL_I2S3_SRC)) - 1)
                ui.Choice_MCO_clksrc:SetSelection(get_table_index(MCO_clksrc_CL, "key", ct:key_read(config.arch.stm32f1.key.PLL_MCO_SRC_CL)) - 1)
        else
                if ct:key_read(config.arch.stm32f1.key.PLL_PLL_ON) == DISABLE then
                        ui.Choice_PLL:SetSelection(0)
                else
                        ui.Choice_PLL:SetSelection(get_table_index(PLL_on, "key", ct:key_read(config.arch.stm32f1.key.PLL_PLL_MUL)) - 1)
                end

                if cpu.peripherals.USB ~= nil or cpu.peripherals.USBOTG then
                        ui.Choice_USB_clksrc:SetSelection(get_table_index(USB_clksrc, "key", ct:key_read(config.arch.stm32f1.key.PLL_USB_DIV)) - 1)
                end

                ui.Choice_PLL_clksrc:SetSelection(get_table_index(PLL_clksrc, "key", ct:key_read(config.arch.stm32f1.key.PLL_PLL_SRC)) - 1)
                ui.Choice_MCO_clksrc:SetSelection(get_table_index(MCO_clksrc, "key", ct:key_read(config.arch.stm32f1.key.PLL_MCO_SRC)) - 1)
        end

        -- load common controls
        ui.Choice_LSI:SetSelection(get_table_index(LSI_on, "key", ct:key_read(config.arch.stm32f1.key.PLL_LSI_ON)) - 1)
        ui.Choice_LSE:SetSelection(get_table_index(LSE_on, "key", ct:key_read(config.arch.stm32f1.key.PLL_LSE_ON)) - 1)
        ui.Choice_HSE:SetSelection(get_table_index(HSE_on, "key", ct:key_read(config.arch.stm32f1.key.PLL_HSE_ON)) - 1)
        ui.Choice_system_clksrc:SetSelection(get_table_index(SYSCLK_clksrc, "key", ct:key_read(config.arch.stm32f1.key.PLL_SYS_CLK_SRC)) - 1)
        ui.Choice_RTC_clksrc:SetSelection(get_table_index(RTC_clksrc, "key", ct:key_read(config.arch.stm32f1.key.PLL_RTC_CLK_SRC)) - 1)
        ui.Choice_AHB_prescaler:SetSelection(get_table_index(AHB_prescaler, "key", ct:key_read(config.arch.stm32f1.key.PLL_AHB_PRE)) - 1)
        ui.Choice_APB1_prescaler:SetSelection(get_table_index(APB12_prescaler, "key", ct:key_read(config.arch.stm32f1.key.PLL_APB1_PRE)) - 1)
        ui.Choice_APB2_prescaler:SetSelection(get_table_index(APB12_prescaler, "key", ct:key_read(config.arch.stm32f1.key.PLL_APB2_PRE)) - 1)
        ui.Choice_ADC_prescaler:SetSelection(get_table_index(ADC_prescaler, "key", ct:key_read(config.arch.stm32f1.key.PLL_ADC_PRE)) - 1)
end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function save_configuration()
        -- save module state
        ct:enable_module("PLL", ui.CheckBox_module_enable:GetValue())

        -- save Conectivity Line specific controls
        if cpu_family:is_CL() then
                if ui.Choice_PLL:GetSelection() == 0 then
                        ct:key_write(config.arch.stm32f1.key.PLL_PLL_ON, DISABLE)
                else
                        ct:key_write(config.arch.stm32f1.key.PLL_PLL_ON, ENABLE)
                        ct:key_write(config.arch.stm32f1.key.PLL_PLL_MUL_CL, PLL_on_CL[ui.Choice_PLL:GetSelection() + 1].key)
                end

                if ui.Choice_PLL2:GetSelection() == 0 then
                        ct:key_write(config.arch.stm32f1.key.PLL_PLL2_ON, DISABLE)
                else
                        ct:key_write(config.arch.stm32f1.key.PLL_PLL2_ON, ENABLE)
                        ct:key_write(config.arch.stm32f1.key.PLL_PLL2_MUL, PLL2_on[ui.Choice_PLL2:GetSelection() + 1].key)
                end

                if ui.Choice_PLL3:GetSelection() == 0 then
                        ct:key_write(config.arch.stm32f1.key.PLL_PLL3_ON, DISABLE)
                else
                        ct:key_write(config.arch.stm32f1.key.PLL_PLL3_ON, ENABLE)
                        ct:key_write(config.arch.stm32f1.key.PLL_PLL3_MUL, PLL3_on[ui.Choice_PLL3:GetSelection() + 1].key)
                end

                if cpu.peripherals.USB ~= nil or cpu.peripherals.USBOTG then
                        ct:key_write(config.arch.stm32f1.key.PLL_USB_DIV_CL, USB_clksrc_CL[ui.Choice_USB_clksrc:GetSelection() + 1].key)
                end

                ct:key_write(config.arch.stm32f1.key.PLL_PLL_SRC_CL, PLL_clksrc_CL[ui.Choice_PLL_clksrc:GetSelection() + 1].key)
                ct:key_write(config.arch.stm32f1.key.PLL_PREDIV2_VAL, PREDIV2_val[ui.Choice_PREDIV2:GetSelection() + 1].key)
                ct:key_write(config.arch.stm32f1.key.PLL_PREDIV1_SRC, PREDIV1_clksrc[ui.Choice_PREDIV1_clksrc:GetSelection() + 1].key)
                ct:key_write(config.arch.stm32f1.key.PLL_PREDIV1_VAL, PREDIV1_val[ui.Choice_PREDIV1_value:GetSelection() + 1].key)
                ct:key_write(config.arch.stm32f1.key.PLL_I2S2_SRC, I2S2_clksrc[ui.Choice_I2S2_clksrc:GetSelection() + 1].key)
                ct:key_write(config.arch.stm32f1.key.PLL_I2S3_SRC, I2S3_clksrc[ui.Choice_I2S3_clksrc:GetSelection() + 1].key)
                ct:key_write(config.arch.stm32f1.key.PLL_MCO_SRC_CL, MCO_clksrc_CL[ui.Choice_MCO_clksrc:GetSelection() + 1].key)
        else
                if ui.Choice_PLL:GetSelection() == 0 then
                        ct:key_write(config.arch.stm32f1.key.PLL_PLL_ON, DISABLE)
                else
                        ct:key_write(config.arch.stm32f1.key.PLL_PLL_ON, ENABLE)
                        ct:key_write(config.arch.stm32f1.key.PLL_PLL_MUL, PLL_on[ui.Choice_PLL:GetSelection() + 1].key)
                end

                if cpu.peripherals.USB ~= nil or cpu.peripherals.USBOTG then
                        ct:key_write(config.arch.stm32f1.key.PLL_USB_DIV, USB_clksrc[ui.Choice_USB_clksrc:GetSelection() + 1].key)
                end

                ct:key_write(config.arch.stm32f1.key.PLL_PLL_SRC, PLL_clksrc[ui.Choice_PLL_clksrc:GetSelection() + 1].key)
                ct:key_write(config.arch.stm32f1.key.PLL_MCO_SRC, MCO_clksrc[ui.Choice_MCO_clksrc:GetSelection() + 1].key)
        end

        -- save common configuration
        ct:key_write(config.arch.stm32f1.key.PLL_LSI_ON, LSI_on[ui.Choice_LSI:GetSelection() + 1].key)
        ct:key_write(config.arch.stm32f1.key.PLL_LSE_ON, LSE_on[ui.Choice_LSE:GetSelection() + 1].key)
        ct:key_write(config.arch.stm32f1.key.PLL_HSE_ON, HSE_on[ui.Choice_HSE:GetSelection() + 1].key)
        ct:key_write(config.arch.stm32f1.key.PLL_SYS_CLK_SRC, SYSCLK_clksrc[ui.Choice_system_clksrc:GetSelection() + 1].key)
        ct:key_write(config.arch.stm32f1.key.PLL_RTC_CLK_SRC, RTC_clksrc[ui.Choice_RTC_clksrc:GetSelection() + 1].key)
        ct:key_write(config.arch.stm32f1.key.PLL_AHB_PRE, AHB_prescaler[ui.Choice_AHB_prescaler:GetSelection() + 1].key)
        ct:key_write(config.arch.stm32f1.key.PLL_APB1_PRE, APB12_prescaler[ui.Choice_APB1_prescaler:GetSelection() + 1].key)
        ct:key_write(config.arch.stm32f1.key.PLL_APB2_PRE, APB12_prescaler[ui.Choice_APB2_prescaler:GetSelection() + 1].key)
        ct:key_write(config.arch.stm32f1.key.PLL_ADC_PRE, ADC_prescaler[ui.Choice_ADC_prescaler:GetSelection() + 1].key)
        ct:key_write(config.arch.stm32f1.key.PLL_FLASH_LATENCY, tostring(calculate_frequencies().flash_latency))

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
-- @brief  Event is called when value is changed (general)
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function event_value_updated()
        freq = calculate_frequencies()
        ui.StaticText_LSI:SetLabel(freqlabel(freq.LSI, "%s (LSI)"))
        ui.StaticText_LSE:SetLabel(freqlabel(freq.LSE, "%s (LSE)"))
        ui.StaticText_HSE:SetLabel(freqlabel(freq.HSE, "%s (HSE)"))

        if cpu_family:is_CL() then
                ui.StaticText_PREDIV2:SetLabel(freqlabel(freq.PREDIV2CLK, "%s (PREDIV2CLK)"))
                ui.StaticText_PREDIV1_value:SetLabel(freqlabel(freq.PREDIV1CLK, "%s (PREDIV1CLK)"))
                ui.StaticText_PLL2:SetLabel(freqlabel(freq.PLL2CLK, "%s (PLL2CLK)"))
                ui.StaticText_PLL3:SetLabel(freqlabel(freq.PLL3CLK, "%s (PLL3CLK)"))
                ui.StaticText_I2S2_clksrc:SetLabel(freqlabel(freq.I2S2CLK, "%s (I2S2CLK)"))
                ui.StaticText_I2S3_clksrc:SetLabel(freqlabel(freq.I2S3CLK, "%s (I2S3CLK)"))
        end

        if cpu.peripherals.USB ~= nil or cpu.peripherals.USBOTG then
                ui.StaticText_USB_clksrc:SetLabel(freqlabel(freq.USBCLK, "%s (USBCLK)"))
        end

        ui.StaticText_PLL:SetLabel(freqlabel(freq.PLLCLK, "%s (PLLCLK)"))
        ui.StaticText_system_clksrc:SetLabel(freqlabel(freq.SYSCLK, "%s (SYSCLK)"))
        ui.StaticText_RTC_clksrc:SetLabel(freqlabel(freq.RTCCLK, "%s (RTCCLK)"))
        ui.StaticText_MCO_clksrc:SetLabel(freqlabel(freq.MCOCLK, "%s (MCOCLK)"))
        ui.StaticText_AHB_prescaler:SetLabel(freqlabel(freq.HCLK, "%s (HCLK)"))
        ui.StaticText_APB1_prescaler:SetLabel(freqlabel(freq.PCLK1, "%s (PCLK1)"))
        ui.StaticText_APB2_prescaler:SetLabel(freqlabel(freq.PCLK2, "%s (PCLK2)"))
        ui.StaticText_ADC_prescaler:SetLabel(freqlabel(freq.ADCCLK, "%s (ADCCLK)"))

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
function pll:create_window(parent)
        cpu_name         = ct:key_read(config.arch.stm32f1.key.CPU_NAME)
        cpu_idx          = ct:get_cpu_index("stm32f1", cpu_name)
        cpu              = config.arch.stm32f1.cpulist:Children()[cpu_idx]
        cpu_family.value = cpu.family:GetValue()
        HSE_FREQ         = tonumber(ct:key_read(config.project.key.CPU_OSC_FREQ))
        HSE_on[2].value  = HSE_FREQ
        HSE_on[3].value  = HSE_FREQ

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
        for i = 1, #LSI_on do ui.Choice_LSI:Append(LSI_on[i].name) end
        ui.FlexGridSizer3:Add(ui.Choice_LSI, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_LSI = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_LSI, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        ui.StaticText2 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "LSE oscillator", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText2, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_LSE = wx.wxChoice(ui.Panel1, ID.CHOICE_LSE, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_LSE")
        for i = 1, #LSE_on do ui.Choice_LSE:Append(LSE_on[i].name) end
        ui.FlexGridSizer3:Add(ui.Choice_LSE, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_LSE = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_LSE, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        ui.StaticText3 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "HSE oscillator", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText3, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_HSE = wx.wxChoice(ui.Panel1, ID.CHOICE_HSE, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_HSE")
        for i = 1, #HSE_on do ui.Choice_HSE:Append(HSE_on[i].name) end
        ui.FlexGridSizer3:Add(ui.Choice_HSE, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_HSE = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_HSE, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        if cpu_family:is_CL() then
                ui.StaticText4 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "PREDIV2", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText4, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_PREDIV2 = wx.wxChoice(ui.Panel1, ID.CHOICE_PREDIV2, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_PREDIV2")
                for i = 1, #PREDIV2_val do ui.Choice_PREDIV2:Append(PREDIV2_val[i].name) end
                ui.FlexGridSizer3:Add(ui.Choice_PREDIV2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText_PREDIV2 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText_PREDIV2, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText5 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "PLL2", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText5, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_PLL2 = wx.wxChoice(ui.Panel1, ID.CHOICE_PLL2, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_PLL2")
                for i = 1, #PLL2_on do ui.Choice_PLL2:Append(PLL2_on[i].name) end
                ui.FlexGridSizer3:Add(ui.Choice_PLL2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText_PLL2 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText_PLL2, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText6 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "PLL3", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText6, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_PLL3 = wx.wxChoice(ui.Panel1, ID.CHOICE_PLL3, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_PLL3")
                for i = 1, #PLL3_on do ui.Choice_PLL3:Append(PLL3_on[i].name) end
                ui.FlexGridSizer3:Add(ui.Choice_PLL3, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText_PLL3 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText_PLL3, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText8 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "PREDIV1", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText8, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.BoxSizer2 = wx.wxBoxSizer(wx.wxHORIZONTAL)
                ui.Choice_PREDIV1_clksrc = wx.wxChoice(ui.Panel1, ID.CHOICE_PREDIV1_CLKSRC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_PREDIV1_CLKSRC")
                for i = 1, #PREDIV1_clksrc do ui.Choice_PREDIV1_clksrc:Append(PREDIV1_clksrc[i].name) end
                ui.Choice_PREDIV1_clksrc:SetToolTip("PREDIV1 clock source")
                ui.BoxSizer2:Add(ui.Choice_PREDIV1_clksrc, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_PREDIV1_value = wx.wxChoice(ui.Panel1, ID.CHOICE_PREDIV1_VALUE, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_PREDIV1_VALUE")
                for i = 1, #PREDIV1_val do ui.Choice_PREDIV1_value:Append(PREDIV1_val[i].name) end
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
        local PLL_clksrc_table = ifs(cpu_family:is_CL(), PLL_clksrc_CL, PLL_clksrc)
        for i = 1, #PLL_clksrc_table do ui.Choice_PLL_clksrc:Append(PLL_clksrc_table[i].name) end
        ui.Choice_PLL_clksrc:SetToolTip("PLL clock source")
        ui.BoxSizer1:Add(ui.Choice_PLL_clksrc, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_PLL = wx.wxChoice(ui.Panel1, ID.CHOICE_PLL, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_PLL")
        local PLL_on_table = ifs(cpu_family:is_CL(), PLL_on_CL, PLL_on)
        for i = 1, #PLL_on_table do ui.Choice_PLL:Append(PLL_on_table[i].name) end
        ui.Choice_PLL:SetToolTip("PLL multiplier")
        ui.BoxSizer1:Add(ui.Choice_PLL, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.FlexGridSizer3:Add(ui.BoxSizer1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.StaticText_PLL = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_PLL, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        ui.StaticText11 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "System clock source", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText11, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_system_clksrc = wx.wxChoice(ui.Panel1, ID.CHOICE_SYSTEM_CLKSRC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_SYSTEM_CLKSRC")
        for i = 1, #SYSCLK_clksrc do ui.Choice_system_clksrc:Append(SYSCLK_clksrc[i].name) end
        ui.FlexGridSizer3:Add(ui.Choice_system_clksrc, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_system_clksrc = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_system_clksrc, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        ui.StaticText12 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "RTC clock source", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText12, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_RTC_clksrc = wx.wxChoice(ui.Panel1, ID.CHOICE_RTC_CLKSRC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_RTC_CLKSRC")
        for i = 1, #RTC_clksrc do ui.Choice_RTC_clksrc:Append(RTC_clksrc[i].name) end
        ui.FlexGridSizer3:Add(ui.Choice_RTC_clksrc, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_RTC_clksrc = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_RTC_clksrc, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        ui.StaticText13 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "MCO clock source", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText13, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_MCO_clksrc = wx.wxChoice(ui.Panel1, ID.CHOICE_MCO_CLKSRC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_MCO_CLKSRC")
        local MCO_clksrc_table = ifs(cpu_family:is_CL(), MCO_clksrc_CL, MCO_clksrc)
        for i = 1, #MCO_clksrc_table do ui.Choice_MCO_clksrc:Append(MCO_clksrc_table[i].name) end
        ui.FlexGridSizer3:Add(ui.Choice_MCO_clksrc, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_MCO_clksrc = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_MCO_clksrc, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        if cpu.peripherals.USB ~= nil or cpu.peripherals.USBOTG then
                ui.StaticText14 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "USB clock source", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText14, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_USB_clksrc = wx.wxChoice(ui.Panel1, ID.CHOICE_USB_CLKSRC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_USB_CLKSRC")
                local USB_div_table = ifs(cpu_family:is_CL(), USB_clksrc_CL, USB_clksrc)
                for i = 1, #USB_div_table do ui.Choice_USB_clksrc:Append(USB_div_table[i].name) end
                ui.FlexGridSizer3:Add(ui.Choice_USB_clksrc, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText_USB_clksrc = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText_USB_clksrc, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        end

        if cpu_family:is_CL() then
                ui.StaticText15 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "I2S2 clock source", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText15, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_I2S2_clksrc = wx.wxChoice(ui.Panel1, ID.CHOICE_I2S2_CLKSRC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_I2S2_CLKSRC")
                for i = 1, #I2S2_clksrc do ui.Choice_I2S2_clksrc:Append(I2S2_clksrc[i].name) end
                ui.FlexGridSizer3:Add(ui.Choice_I2S2_clksrc, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText_I2S2_clksrc = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText_I2S2_clksrc, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText16 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "I2S3 clock source", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText16, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_I2S3_clksrc = wx.wxChoice(ui.Panel1, ID.CHOICE_I2S3_CLKSRC, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_I2S3_CLKSRC")
                for i = 1, #I2S3_clksrc do ui.Choice_I2S3_clksrc:Append(I2S3_clksrc[i].name) end
                ui.FlexGridSizer3:Add(ui.Choice_I2S3_clksrc, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText_I2S3_clksrc = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
                ui.FlexGridSizer3:Add(ui.StaticText_I2S3_clksrc, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        end

        ui.StaticText17 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "AHB prescaler", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText17, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_AHB_prescaler = wx.wxChoice(ui.Panel1, ID.CHOICE_AHB_PRESCALER, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_AHB_PRESCALER")
        for i = 1, #AHB_prescaler do ui.Choice_AHB_prescaler:Append(AHB_prescaler[i].name) end
        ui.FlexGridSizer3:Add(ui.Choice_AHB_prescaler, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_AHB_prescaler = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_AHB_prescaler, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        ui.StaticText18 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "APB1 prescaler", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText18, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_APB1_prescaler = wx.wxChoice(ui.Panel1, ID.CHOICE_APB1_PRESCALER, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_APB1_PRESCALER")
        for i = 1, #APB12_prescaler do ui.Choice_APB1_prescaler:Append(APB12_prescaler[i].name) end
        ui.FlexGridSizer3:Add(ui.Choice_APB1_prescaler, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_APB1_prescaler = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_APB1_prescaler, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        ui.StaticText19 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "APB2 prescaler", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText19, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_APB2_prescaler = wx.wxChoice(ui.Panel1, ID.CHOICE_APB2_PRESCALER, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_APB2_PRESCALER")
        for i = 1, #APB12_prescaler do ui.Choice_APB2_prescaler:Append(APB12_prescaler[i].name) end
        ui.FlexGridSizer3:Add(ui.Choice_APB2_prescaler, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_APB2_prescaler = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_APB2_prescaler, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        ui.StaticText20 = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, "ADC prescaler", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText20, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Choice_ADC_prescaler = wx.wxChoice(ui.Panel1, ID.CHOICE_ADC_PRESCALER, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_ADC_PRESCALER")
        for i = 1, #ADC_prescaler do ui.Choice_ADC_prescaler:Append(ADC_prescaler[i].name) end
        ui.FlexGridSizer3:Add(ui.Choice_ADC_prescaler, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText_ADC_prescaler = wx.wxStaticText(ui.Panel1, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "wx.wxID_ANY")
        ui.FlexGridSizer3:Add(ui.StaticText_ADC_prescaler, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        ui.FlexGridSizer2:Add(ui.FlexGridSizer3, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.Panel1:SetSizer(ui.FlexGridSizer2)
        ui.FlexGridSizer1:Add(ui.Panel1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        --
        this:SetSizer(ui.FlexGridSizer1)
        this:SetScrollRate(5, 5)

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

        --
        load_configuration()
        event_value_updated()
        modified:no()

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
        return modified:get_value()
end


--------------------------------------------------------------------------------
-- @brief  Function save configuration
-- @return None
--------------------------------------------------------------------------------
function pll:save()
        save_configuration()
end


--------------------------------------------------------------------------------
-- @brief  Function discard modified configuration
-- @return None
--------------------------------------------------------------------------------
function pll:discard()
        load_configuration()
        event_value_updated()
        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Function returns module handler
-- @param  None
-- @return Module handler
--------------------------------------------------------------------------------
function get_handler()
        return pll
end
