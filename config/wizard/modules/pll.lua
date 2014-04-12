--[[============================================================================
@file    crc.lua

@author  Daniel Zorychta

@brief   CRC configuration wizard.

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


*//*========================================================================--]]

require "defs"
require "cpu"
require "db"

--------------------------------------------------------------------------------
-- OBJECTS
--------------------------------------------------------------------------------
pll = {}

local arch = {}
arch.stm32f1                    = {}
arch.stm32f1.configure          = nil
arch.stm32f1.get_frequencies    = nil

arch.stm32f2                    = {}
arch.stm32f2.configure          = nil
arch.stm32f2.get_frequencies    = nil

arch.stm32f3                    = {}
arch.stm32f3.configure          = nil
arch.stm32f3.get_frequencies    = nil

arch.stm32f4                    = {}
arch.stm32f4.configure          = nil
arch.stm32f4.get_frequencies    = nil

--------------------------------------------------------------------------------
-- FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Calculate total steps
--------------------------------------------------------------------------------
local function calculate_total_steps()
        local arch   = cpu:get_arch()
        local family = db:get_mcu_family(cpu:get_name())

        if key_read("../project/Makefile", "ENABLE_PLL") == yes then
                if arch == "stm32f1" then
                        if family == "STM32F10X_CL" then
                                progress(1, 24)
                        else
                                progress(1, 15)
                        end

                elseif arch == "stm32f2" then
                elseif arch == "stm32f3" then
                elseif arch == "stm32f4" then
                end
        else
                progress(1, 1)
        end
end

--------------------------------------------------------------------------------
-- @brief Ask user to select enable/disable module
--------------------------------------------------------------------------------
local function ask_for_enable()
        local choice = key_read("../project/flags.h", "__ENABLE_PLL__")
        msg(progress().."Do you want to enable PLL module?")
        msg("Current selection is: "..filter_yes_no(choice)..".")
        add_item(yes, "Yes")
        add_item(no, "No")
        choice = get_selection()
        if can_be_saved(choice) then
                key_save("../project/flags.h", "__ENABLE_PLL__", choice)
                key_save("../project/Makefile", "ENABLE_PLL", choice)
        end

        calculate_total_steps()
        progress(2)

        return choice
end

--------------------------------------------------------------------------------
-- @brief Return all stm32f1 PLL frequencies
--------------------------------------------------------------------------------
arch.stm32f1.get_frequencies = function()
        local family            = db:get_mcu_family(cpu:get_name())

        local pll_src_key  = "__PLL_PLL_SRC__"
        local pll_mull_key = "__PLL_PLL_MULL__"
        local mco_src_key  = "__PLL_MCO_SRC__"
        local usb_div_key  = "__PLL_USB_DIV__"

        if family == "STM32F10X_CL" then
                pll_src_key  = "__PLL_PLL_SRC_CL__"
                pll_mull_key = "__PLL_PLL_MULL_CL__"
                mco_src_key  = "__PLL_MCO_SRC_CL__"
                usb_div_key  = "__PLL_USB_DIV_CL__"
        end

        local pll_source        = key_read("../stm32f1/pll_flags.h", pll_src_key)
        local pll_mull          = key_read("../stm32f1/pll_flags.h", pll_mull_key):gsub("6_5", "6.5"):gsub("RCC_PLLMul_", "")
        local sysclk_source     = key_read("../stm32f1/pll_flags.h", "__PLL_SYS_CLK_SRC__")
        local prediv1_source    = key_read("../stm32f1/pll_flags.h", "__PLL_PREDIV1_SRC__")
        local RTC_source        = key_read("../stm32f1/pll_flags.h", "__PLL_RTC_CLK_SRC__")
        local I2S2_source       = key_read("../stm32f1/pll_flags.h", "__PLL_I2S2_SRC__")
        local I2S3_source       = key_read("../stm32f1/pll_flags.h", "__PLL_I2S3_SRC__")
        local prediv1_factor    = key_read("../stm32f1/pll_flags.h", "__PLL_PREDIV1_VAL__"):gsub("RCC_PREDIV1_Div", "")
        local prediv2_factor    = key_read("../stm32f1/pll_flags.h", "__PLL_PREDIV2_VAL__"):gsub("RCC_PREDIV2_Div", "")
        local pll2_mull         = key_read("../stm32f1/pll_flags.h", "__PLL_PLL2_MULL__"):gsub("RCC_PLL2Mul_", "")
        local pll3_mull         = key_read("../stm32f1/pll_flags.h", "__PLL_PLL3_MULL__"):gsub("RCC_PLL3Mul_", "")
        local AHB_prescaler     = key_read("../stm32f1/pll_flags.h", "__PLL_AHB_PRE__"):gsub("RCC_SYSCLK_Div", "")
        local APB1_prescaler    = key_read("../stm32f1/pll_flags.h", "__PLL_APB1_PRE__"):gsub("RCC_HCLK_Div", "")
        local APB2_prescaler    = key_read("../stm32f1/pll_flags.h", "__PLL_APB2_PRE__"):gsub("RCC_HCLK_Div", "")
        local ADC_prescaler     = key_read("../stm32f1/pll_flags.h", "__PLL_ADC_PRE__"):gsub("RCC_PCLK2_Div", "")
        local USB_prescaler     = key_read("../stm32f1/pll_flags.h", usb_div_key):gsub("1Div5", "Div1.5"):gsub("RCC_.*CLKSource_PLL.*_Div", "")
        local MCO_source        = key_read("../stm32f1/pll_flags.h", mco_src_key)

        local freq              = {}
        freq.HSE                = tonumber(key_read("../project/flags.h", "__CPU_OSC_FREQ__"))
        freq.HSI                = 8e6
        freq.LSI                = 40e3
        freq.LSE                = 32768
        freq.SYSCLK             = 0
        freq.HCLK               = 0
        freq.PCLK1              = 0
        freq.PCLK2              = 0
        freq.ADCCLK             = 0
        freq.PLLCLK             = 0
        freq.PLLVCO             = 0
        freq.USBCLK             = 0
        freq.I2S2CLK            = 0
        freq.I2S3CLK            = 0
        freq.USBCLK             = 0
        freq.RTCCLK             = 0
        freq.PREDIV1OUTCLK      = 0
        freq.PREDIV1INCLK       = 0
        freq.PREDIV2INCLK       = freq.HSE
        freq.PREDIV2OUTCLK      = 0
        freq.PLLINCLK           = 0
        freq.PLL2CLK            = 0
        freq.PLL3CLK            = 0
        freq.PLL3VCO            = 0
        freq.MCOCLK             = 0

        -- calculate PLL Clk frequency
        if pll_source == "RCC_PLLSource_HSI_Div2" then
                freq.PLLINCLK = (freq.HSI / 2)
                freq.PLLCLK   = freq.PLLINCLK * pll_mull
        else
                if family == "STM32F10X_CL" then
                        if prediv1_source == "RCC_PREDIV1_Source_HSE" then
                                freq.PREDIV1INCLK = freq.HSE
                        else
                                freq.PREDIV1INCLK = (freq.HSE / prediv2_factor) * pll2_mull
                        end
                        freq.PREDIV1OUTCLK = freq.PREDIV1INCLK / prediv1_factor
                        freq.PLLINCLK      = freq.PREDIV1OUTCLK
                        freq.PLLCLK        = freq.PLLINCLK * pll_mull
                        freq.PREDIV2OUTCLK = freq.PREDIV2INCLK / prediv2_factor
                else
                        if pll_source == "RCC_PLLSource_HSE_Div2" then
                                freq.PLLINCLK = (freq.HSE / 2)
                        else
                                freq.PLLINCLK = (freq.HSE)
                        end
                        freq.PLLCLK = freq.PLLINCLK * pll_mull
                end
        end

        -- calculate USB frequency
        if family == "STM32F10X_CL" then
                freq.USBCLK = (2 * freq.PLLCLK) / USB_prescaler
        else
                freq.USBCLK = freq.PLLCLK / USB_prescaler
        end

        -- calculate SYSCLK frequency
        if sysclk_source == "RCC_SYSCLKSource_HSI" then
                freq.SYSCLK = freq.HSI
        elseif sysclk_source == "RCC_SYSCLKSource_HSE" then
                freq.SYSCLK = freq.HSE
        elseif sysclk_source == "RCC_SYSCLKSource_PLLCLK" then
                freq.SYSCLK = freq.PLLCLK
        end

        -- calculate PLL2CLK and PLL3CLK
        if family == "STM32F10X_CL" then
                freq.PLL2CLK = (freq.HSE / prediv2_factor) * pll2_mull
                freq.PLL3CLK = (freq.HSE / prediv2_factor) * pll3_mull
                freq.PLL3VCO = freq.PLL3CLK * 2
        end

        -- calculate I2S frequency
        if family == "STM32F10X_CL" then
                if I2S2_source == "RCC_I2S2CLKSource_PLL3_VCO" then
                        freq.I2S2CLK = freq.PLL3VCO
                else
                        freq.I2S2CLK = freq.SYSCLK
                end

                if I2S3_source == "RCC_I2S3CLKSource_PLL3_VCO" then
                        freq.I2S3CLK = freq.PLL3VCO
                else
                        freq.I2S3CLK = freq.SYSCLK
                end
        else
                freq.I2S2CLK = freq.SYSCLK
                freq.I2S3CLK = freq.SYSCLK
        end

        -- calculate RTC frequency
        if RTC_source == "RCC_RTCCLKSource_LSE" then
                freq.RTCCLK = freq.LSE
        elseif RTC_source == "RCC_RTCCLKSource_LSI" then
                freq.RTCCLK = freq.LSI
        elseif RTC_source == "RCC_RTCCLKSource_HSE_Div128" then
                freq.RTCCLK = freq.HSE / 128
        end

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

        freq.PLLVCO = freq.PLLCLK * 2
        freq.HCLK   = freq.SYSCLK / AHB_prescaler
        freq.PCLK1  = freq.HCLK / APB1_prescaler
        freq.PCLK2  = freq.HCLK / APB2_prescaler
        freq.ADCCLK = freq.PCLK2 / ADC_prescaler

        return freq
end

--------------------------------------------------------------------------------
-- @brief PLL configuration for STM32F1
--------------------------------------------------------------------------------
arch.stm32f1.configure = function()
        local family = db:get_mcu_family(cpu:get_name())

        local function configure_flash_latency()
                local freq          = arch.stm32f1.get_frequencies()
                local flash_latency = 2

                if freq.SYSCLK <= 24e6 then
                        flash_latency = 0
                elseif freq.SYSCLK <= 48e6 then
                        flash_latency = 1
                elseif freq.SYSCLK <= 72e6 then
                        flash_latency = 2
                end

                key_save("../stm32f1/pll_flags.h", "__PLL_FLASH_LATENCY__", flash_latency)
        end

        local function print_summary()
                local freq = arch.stm32f1.get_frequencies()

                msg("PLL configuration summary:")
                local str_com = "SYSCLK = "..funit(freq.SYSCLK).."\n"..
                                "HCLK = "..funit(freq.HCLK).."\n"..
                                "PCLK1 = "..funit(freq.PCLK1).."\n"..
                                "PCLK2 = "..funit(freq.PCLK2).."\n"..
                                "ADCCLK = "..funit(freq.ADCCLK).."\n"..
                                "USBCLK = "..funit(freq.USBCLK).."\n"..
                                "I2S2CLK = "..funit(freq.I2S2CLK).."\n"..
                                "I2S3CLK = "..funit(freq.I2S3CLK).."\n"..
                                "MCOCLK = "..funit(freq.MCOCLK)

                local str_cl  = "PLL2CLK = "..funit(freq.PLL2CLK).."\n"..
                                "PLL3CLK = "..funit(freq.PLL3CLK).."\n"..
                                "PLL3VCO = "..funit(freq.PLL3VCO)

                if family == "STM32F10X_CL" then
                        msg(str_com.."\n"..str_cl)
                else
                        msg(str_com)
                end

                pause()
        end

        local function configure_LSI_on()
                local value   = {}
                value.ENABLE  = "Enable"
                value.DISABLE = "Disable"

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_LSI_ON__")
                msg(progress() .. "Do you want to enable LSI?")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("ENABLE", value.ENABLE)
                add_item("DISABLE", value.DISABLE)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/pll_flags.h", "__PLL_LSI_ON__", choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_LSE_on()
                local value = {}
                value.RCC_LSE_OFF    = "LSE oscillator OFF"
                value.RCC_LSE_ON     = "LSE oscillator ON"
                value.RCC_LSE_Bypass = "LSE oscillator bypassed with external clock"

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_LSE_ON__")
                msg(progress() .. "Do you want to enable LSE?")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("RCC_LSE_OFF", value.RCC_LSE_OFF)
                add_item("RCC_LSE_ON", value.RCC_LSE_ON)
                add_item("RCC_LSE_Bypass", value.RCC_LSE_Bypass)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/pll_flags.h", "__PLL_LSE_ON__", choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_HSE_on()
                local value = {}
                value.RCC_HSE_OFF    = "HSE oscillator OFF"
                value.RCC_HSE_ON     = "HSE oscillator ON"
                value.RCC_HSE_Bypass = "HSE oscillator bypassed with external clock"

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_HSE_ON__")
                msg(progress() .. "Do you want to enable HSE?")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("RCC_HSE_OFF", value.RCC_HSE_OFF)
                add_item("RCC_HSE_ON", value.RCC_HSE_ON)
                add_item("RCC_HSE_Bypass", value.RCC_HSE_Bypass)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/pll_flags.h", "__PLL_HSE_ON__", choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_RTC_clk_src()
                local freq  = arch.stm32f1.get_frequencies()
                local value = {}
                value.RCC_RTCCLKSource_LSE        = "LSE selected as RTC clock ("..funit(freq.LSE)..")"
                value.RCC_RTCCLKSource_LSI        = "LSI selected as RTC clock ("..funit(freq.LSI)..")"
                value.RCC_RTCCLKSource_HSE_Div128 = "HSE clock divided by 128 selected as RTC clock ("..funit(freq.HSE/128)..")"

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_RTC_CLK_SRC__")
                msg(progress() .. "RTC clock source configuration.")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("RCC_RTCCLKSource_LSE", value.RCC_RTCCLKSource_LSE)
                add_item("RCC_RTCCLKSource_LSI", value.RCC_RTCCLKSource_LSI)
                add_item("RCC_RTCCLKSource_HSE_Div128", value.RCC_RTCCLKSource_HSE_Div128)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/pll_flags.h", "__PLL_RTC_CLK_SRC__", choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_sys_clk_src()
                local freq  = arch.stm32f1.get_frequencies()
                local value = {}
                value.RCC_SYSCLKSource_HSI    = "HSI selected as system clock ("..funit(freq.HSI)..")"
                value.RCC_SYSCLKSource_HSE    = "HSE selected as system clock ("..funit(freq.HSE)..")"
                value.RCC_SYSCLKSource_PLLCLK = "PLL selected as system clock ("..funit(freq.PLLCLK)..")"

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_SYS_CLK_SRC__")
                msg(progress() .. "System clock source configuration.")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("RCC_SYSCLKSource_HSI", value.RCC_SYSCLKSource_HSI)
                add_item("RCC_SYSCLKSource_HSE", value.RCC_SYSCLKSource_HSE)
                add_item("RCC_SYSCLKSource_PLLCLK", value.RCC_SYSCLKSource_PLLCLK)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/pll_flags.h", "__PLL_SYS_CLK_SRC__", choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_MCO_src()
                local freq  = arch.stm32f1.get_frequencies()
                local value = {}
                value.RCC_MCO_NoClock      = "No clock selected (0 Hz)"
                value.RCC_MCO_SYSCLK       = "System clock selected ("..funit(freq.SYSCLK)..")"
                value.RCC_MCO_HSI          = "HSI oscillator clock selected ("..funit(freq.HSI)..")"
                value.RCC_MCO_HSE          = "HSE oscillator clock selected ("..funit(freq.HSE)..")"
                value.RCC_MCO_PLLCLK_Div2  = "PLL clock divided by 2 selected ("..funit(freq.PLLCLK/2)..")"
                -- STM32F10X_CL
                value.RCC_MCO_PLL2CLK      = "PLL2 clock selected ("..funit(freq.PLL2CLK)..")"
                value.RCC_MCO_PLL3CLK_Div2 = "PLL3 clock divided by 2 selected ("..funit(freq.PLL3CLK/2)..")"
                value.RCC_MCO_XT1          = "External 3-25 MHz oscillator clock selected ("..funit(freq.HSE)..")"
                value.RCC_MCO_PLL3CLK      = "PLL3 clock selected ("..funit(freq.PLL3CLK)..")"

                local key
                if family == "STM32F10X_CL" then
                        key = "__PLL_MCO_SRC_CL__"
                else
                        key = "__PLL_MCO_SRC__"
                end

                local choice = key_read("../stm32f1/pll_flags.h", key)
                msg(progress() .. "MCO source configuration.")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("RCC_MCO_NoClock", value.RCC_MCO_NoClock)
                add_item("RCC_MCO_SYSCLK", value.RCC_MCO_SYSCLK)
                add_item("RCC_MCO_HSI", value.RCC_MCO_HSI)
                add_item("RCC_MCO_HSE", value.RCC_MCO_HSE)
                add_item("RCC_MCO_PLLCLK_Div2", value.RCC_MCO_PLLCLK_Div2)
                if family == "STM32F10X_CL" then
                add_item("RCC_MCO_PLL2CLK", value.RCC_MCO_PLL2CLK)
                add_item("RCC_MCO_PLL3CLK_Div2", value.RCC_MCO_PLL3CLK_Div2)
                add_item("RCC_MCO_XT1", value.RCC_MCO_XT1)
                add_item("RCC_MCO_PLL3CLK", value.RCC_MCO_PLL3CLK)
                end
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/pll_flags.h", key, choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_I2S2_src()
                local freq  = arch.stm32f1.get_frequencies()
                local value = {}
                value.RCC_I2S2CLKSource_SYSCLK   = "System clock selected as I2S2 clock entry ("..funit(freq.SYSCLK)..")"
                value.RCC_I2S2CLKSource_PLL3_VCO = "PLL3 VCO clock selected as I2S2 clock entry ("..funit(freq.PLL3VCO)..")"

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_I2S2_SRC__")
                msg(progress() .. "I2S2 clock source configuration.")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("RCC_I2S2CLKSource_SYSCLK", value.RCC_I2S2CLKSource_SYSCLK)
                add_item("RCC_I2S2CLKSource_PLL3_VCO", value.RCC_I2S2CLKSource_PLL3_VCO)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/pll_flags.h", "__PLL_I2S2_SRC__", choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_I2S3_src()
                local freq  = arch.stm32f1.get_frequencies()
                local value = {}
                value.RCC_I2S3CLKSource_SYSCLK   = "System clock selected as I2S3 clock entry ("..funit(freq.SYSCLK)..")"
                value.RCC_I2S3CLKSource_PLL3_VCO = "PLL3 VCO clock selected as I2S3 clock entry ("..funit(freq.PLL3VCO)..")"

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_I2S3_SRC__")
                msg(progress() .. "I2S3 clock source configuration.")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("RCC_I2S3CLKSource_SYSCLK", value.RCC_I2S3CLKSource_SYSCLK)
                add_item("RCC_I2S3CLKSource_PLL3_VCO", value.RCC_I2S3CLKSource_PLL3_VCO)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/pll_flags.h", "__PLL_I2S3_SRC__", choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_PLL_on()
                local value = {}
                value.ENABLE  = "Enable"
                value.DISABLE = "Disable"

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_PLL_ON__")
                msg(progress() .. "Do you want to enable PLL?")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("ENABLE", value.ENABLE)
                add_item("DISABLE", value.DISABLE)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/pll_flags.h", "__PLL_PLL_ON__", choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_PLL_src()
                local freq  = arch.stm32f1.get_frequencies()
                local value = {}
                value.RCC_PLLSource_HSI_Div2 = "HSI oscillator clock divided by 2 selected as PLL clock entry ("..funit(freq.HSI/2)..")"
                value.RCC_PLLSource_PREDIV1  = "PREDIV1 clock selected as PLL clock entry ("..funit(freq.PREDIV1OUTCLK)..")"
                value.RCC_PLLSource_HSE_Div1 = "HSE oscillator clock selected as PLL clock entry ("..funit(freq.HSE)..")"
                value.RCC_PLLSource_HSE_Div2 = "HSE oscillator clock divided by 2 selected as PLL clock entry ("..funit(freq.HSE/2)..")"

                local key
                if family == "STM32F10X_CL" then
                        key = "__PLL_PLL_SRC_CL__"
                else
                        key = "__PLL_PLL_SRC__"
                end

                local choice = key_read("../stm32f1/pll_flags.h", key)
                msg(progress() .. "PLL clock source configuration.")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("RCC_PLLSource_HSI_Div2", value.RCC_PLLSource_HSI_Div2)
                if family == "STM32F10X_CL" then
                add_item("RCC_PLLSource_PREDIV1", value.RCC_PLLSource_PREDIV1)
                else
                add_item("RCC_PLLSource_HSE_Div1", value.RCC_PLLSource_HSE_Div1)
                add_item("RCC_PLLSource_HSE_Div2", value.RCC_PLLSource_HSE_Div2)
                end
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/pll_flags.h", key, choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_PLL_mul()
                local freq  = arch.stm32f1.get_frequencies()
                local value = {}
                value.RCC_PLLMul_2   = "x2 ("..funit(freq.PLLINCLK*2)..")"
                value.RCC_PLLMul_3   = "x3 ("..funit(freq.PLLINCLK*3)..")"
                value.RCC_PLLMul_4   = "x4 ("..funit(freq.PLLINCLK*4)..")"
                value.RCC_PLLMul_5   = "x5 ("..funit(freq.PLLINCLK*5)..")"
                value.RCC_PLLMul_6   = "x6 ("..funit(freq.PLLINCLK*6)..")"
                value.RCC_PLLMul_6_5 = "x6.5 ("..funit(freq.PLLINCLK*6.5)..")"
                value.RCC_PLLMul_7   = "x7 ("..funit(freq.PLLINCLK*7)..")"
                value.RCC_PLLMul_8   = "x8 ("..funit(freq.PLLINCLK*8)..")"
                value.RCC_PLLMul_9   = "x9 ("..funit(freq.PLLINCLK*9)..")"
                value.RCC_PLLMul_10  = "x10 ("..funit(freq.PLLINCLK*10)..")"
                value.RCC_PLLMul_11  = "x11 ("..funit(freq.PLLINCLK*11)..")"
                value.RCC_PLLMul_12  = "x12 ("..funit(freq.PLLINCLK*12)..")"
                value.RCC_PLLMul_13  = "x13 ("..funit(freq.PLLINCLK*13)..")"
                value.RCC_PLLMul_14  = "x14 ("..funit(freq.PLLINCLK*14)..")"
                value.RCC_PLLMul_15  = "x15 ("..funit(freq.PLLINCLK*15)..")"
                value.RCC_PLLMul_16  = "x16 ("..funit(freq.PLLINCLK*16)..")"

                local key
                if family == "STM32F10X_CL" then
                        key = "__PLL_PLL_MULL_CL__"
                else
                        key = "__PLL_PLL_MULL__"
                end

                local choice = key_read("../stm32f1/pll_flags.h", key)
                msg(progress() .. "PLL multiplication factor configuration.")
                msg("Current choice is: " .. value[choice] .. ".")
                if family == "STM32F10X_CL" then
                add_item("RCC_PLLMul_4",   value.RCC_PLLMul_4)
                add_item("RCC_PLLMul_5",   value.RCC_PLLMul_5)
                add_item("RCC_PLLMul_6",   value.RCC_PLLMul_6)
                add_item("RCC_PLLMul_6_5", value.RCC_PLLMul_6_5)
                add_item("RCC_PLLMul_7",   value.RCC_PLLMul_7)
                add_item("RCC_PLLMul_8",   value.RCC_PLLMul_8)
                add_item("RCC_PLLMul_9",   value.RCC_PLLMul_9)
                else
                add_item("RCC_PLLMul_2",  value.RCC_PLLMul_2)
                add_item("RCC_PLLMul_3",  value.RCC_PLLMul_3)
                add_item("RCC_PLLMul_4",  value.RCC_PLLMul_4)
                add_item("RCC_PLLMul_5",  value.RCC_PLLMul_5)
                add_item("RCC_PLLMul_6",  value.RCC_PLLMul_6)
                add_item("RCC_PLLMul_7",  value.RCC_PLLMul_7)
                add_item("RCC_PLLMul_8",  value.RCC_PLLMul_8)
                add_item("RCC_PLLMul_9",  value.RCC_PLLMul_9)
                add_item("RCC_PLLMul_10", value.RCC_PLLMul_10)
                add_item("RCC_PLLMul_11", value.RCC_PLLMul_11)
                add_item("RCC_PLLMul_12", value.RCC_PLLMul_12)
                add_item("RCC_PLLMul_13", value.RCC_PLLMul_13)
                add_item("RCC_PLLMul_14", value.RCC_PLLMul_14)
                add_item("RCC_PLLMul_15", value.RCC_PLLMul_15)
                add_item("RCC_PLLMul_16", value.RCC_PLLMul_16)
                end
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/pll_flags.h", key, choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_USB_div()
                local freq  = arch.stm32f1.get_frequencies()
                local value = {}
                value.RCC_OTGFSCLKSource_PLLVCO_Div3 = "PLL VCO clock divided by 3 selected as USB OTG FS clock source ("..funit(freq.PLLVCO/3)..")"
                value.RCC_OTGFSCLKSource_PLLVCO_Div2 = "PLL VCO clock divided by 2 selected as USB OTG FS clock source ("..funit(freq.PLLVCO/2)..")"
                value.RCC_USBCLKSource_PLLCLK_1Div5  = "PLL clock divided by 1.5 selected as USB clock source ("..funit(freq.PLLCLK/1.5)..")"
                value.RCC_USBCLKSource_PLLCLK_Div1   = "PLL clock selected as USB clock source ("..funit(freq.PLLCLK)..")"

                local key
                if family == "STM32F10X_CL" then
                        key = "__PLL_USB_DIV_CL__"
                else
                        key = "__PLL_USB_DIV__"
                end

                local choice = key_read("../stm32f1/pll_flags.h", key)
                msg(progress() .. "USB clock prescaler configuration.")
                msg("Current choice is: " .. value[choice] .. ".")
                if family == "STM32F10X_CL" then
                add_item("RCC_OTGFSCLKSource_PLLVCO_Div3", value.RCC_OTGFSCLKSource_PLLVCO_Div3)
                add_item("RCC_OTGFSCLKSource_PLLVCO_Div2", value.RCC_OTGFSCLKSource_PLLVCO_Div2)
                else
                add_item("RCC_USBCLKSource_PLLCLK_1Div5", value.RCC_USBCLKSource_PLLCLK_1Div5)
                add_item("RCC_USBCLKSource_PLLCLK_Div1",  value.RCC_USBCLKSource_PLLCLK_Div1)
                end
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/pll_flags.h", key, choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_prediv1_src()
                local freq  = arch.stm32f1.get_frequencies()
                local value = {}
                value.RCC_PREDIV1_Source_HSE  = "HSE selected as PREDIV1 clock ("..funit(freq.HSE)..")"
                value.RCC_PREDIV1_Source_PLL2 = "PLL2 selected as PREDIV1 clock ("..funit(freq.PLL2CLK)..")"

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_PREDIV1_SRC__")
                msg(progress() .. "PREDIV1 clock source configuration.")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("RCC_PREDIV1_Source_HSE",  value.RCC_PREDIV1_Source_HSE)
                add_item("RCC_PREDIV1_Source_PLL2", value.RCC_PREDIV1_Source_PLL2)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/pll_flags.h", "__PLL_PREDIV1_SRC__", choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_prediv1_val()
                local freq  = arch.stm32f1.get_frequencies()
                local value = {}
                value.RCC_PREDIV1_Div1  = "/1 ("..funit(freq.PREDIV1INCLK/1)..")"
                value.RCC_PREDIV1_Div2  = "/2 ("..funit(freq.PREDIV1INCLK/2)..")"
                value.RCC_PREDIV1_Div3  = "/3 ("..funit(freq.PREDIV1INCLK/3)..")"
                value.RCC_PREDIV1_Div4  = "/4 ("..funit(freq.PREDIV1INCLK/4)..")"
                value.RCC_PREDIV1_Div5  = "/5 ("..funit(freq.PREDIV1INCLK/5)..")"
                value.RCC_PREDIV1_Div6  = "/6 ("..funit(freq.PREDIV1INCLK/6)..")"
                value.RCC_PREDIV1_Div7  = "/7 ("..funit(freq.PREDIV1INCLK/7)..")"
                value.RCC_PREDIV1_Div8  = "/8 ("..funit(freq.PREDIV1INCLK/8)..")"
                value.RCC_PREDIV1_Div9  = "/9 ("..funit(freq.PREDIV1INCLK/9)..")"
                value.RCC_PREDIV1_Div10 = "/10 ("..funit(freq.PREDIV1INCLK/10)..")"
                value.RCC_PREDIV1_Div11 = "/11 ("..funit(freq.PREDIV1INCLK/11)..")"
                value.RCC_PREDIV1_Div12 = "/12 ("..funit(freq.PREDIV1INCLK/12)..")"
                value.RCC_PREDIV1_Div13 = "/13 ("..funit(freq.PREDIV1INCLK/13)..")"
                value.RCC_PREDIV1_Div14 = "/14 ("..funit(freq.PREDIV1INCLK/14)..")"
                value.RCC_PREDIV1_Div15 = "/15 ("..funit(freq.PREDIV1INCLK/15)..")"
                value.RCC_PREDIV1_Div16 = "/16 ("..funit(freq.PREDIV1INCLK/16)..")"

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_PREDIV1_VAL__")
                msg(progress() .. "PREDIV1 divide factor configuration.")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("RCC_PREDIV1_Div1",  value.RCC_PREDIV1_Div1 )
                add_item("RCC_PREDIV1_Div2",  value.RCC_PREDIV1_Div2 )
                add_item("RCC_PREDIV1_Div3",  value.RCC_PREDIV1_Div3 )
                add_item("RCC_PREDIV1_Div4",  value.RCC_PREDIV1_Div4 )
                add_item("RCC_PREDIV1_Div5",  value.RCC_PREDIV1_Div5 )
                add_item("RCC_PREDIV1_Div6",  value.RCC_PREDIV1_Div6 )
                add_item("RCC_PREDIV1_Div7",  value.RCC_PREDIV1_Div7 )
                add_item("RCC_PREDIV1_Div8",  value.RCC_PREDIV1_Div8 )
                add_item("RCC_PREDIV1_Div9",  value.RCC_PREDIV1_Div9 )
                add_item("RCC_PREDIV1_Div10", value.RCC_PREDIV1_Div10)
                add_item("RCC_PREDIV1_Div11", value.RCC_PREDIV1_Div11)
                add_item("RCC_PREDIV1_Div12", value.RCC_PREDIV1_Div12)
                add_item("RCC_PREDIV1_Div13", value.RCC_PREDIV1_Div13)
                add_item("RCC_PREDIV1_Div14", value.RCC_PREDIV1_Div14)
                add_item("RCC_PREDIV1_Div15", value.RCC_PREDIV1_Div15)
                add_item("RCC_PREDIV1_Div16", value.RCC_PREDIV1_Div16)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/pll_flags.h", "__PLL_PREDIV1_VAL__", choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_prediv2_val()
                local freq  = arch.stm32f1.get_frequencies()
                local value = {}
                value.RCC_PREDIV2_Div1  = "/1 ("..funit(freq.HSE/1)..")"
                value.RCC_PREDIV2_Div2  = "/2 ("..funit(freq.HSE/2)..")"
                value.RCC_PREDIV2_Div3  = "/3 ("..funit(freq.HSE/3)..")"
                value.RCC_PREDIV2_Div4  = "/4 ("..funit(freq.HSE/4)..")"
                value.RCC_PREDIV2_Div5  = "/5 ("..funit(freq.HSE/5)..")"
                value.RCC_PREDIV2_Div6  = "/6 ("..funit(freq.HSE/6)..")"
                value.RCC_PREDIV2_Div7  = "/7 ("..funit(freq.HSE/7)..")"
                value.RCC_PREDIV2_Div8  = "/8 ("..funit(freq.HSE/8)..")"
                value.RCC_PREDIV2_Div9  = "/9 ("..funit(freq.HSE/9)..")"
                value.RCC_PREDIV2_Div10 = "/10 ("..funit(freq.HSE/10)..")"
                value.RCC_PREDIV2_Div11 = "/11 ("..funit(freq.HSE/11)..")"
                value.RCC_PREDIV2_Div12 = "/12 ("..funit(freq.HSE/12)..")"
                value.RCC_PREDIV2_Div13 = "/13 ("..funit(freq.HSE/13)..")"
                value.RCC_PREDIV2_Div14 = "/14 ("..funit(freq.HSE/14)..")"
                value.RCC_PREDIV2_Div15 = "/15 ("..funit(freq.HSE/15)..")"
                value.RCC_PREDIV2_Div16 = "/16 ("..funit(freq.HSE/16)..")"

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_PREDIV2_VAL__")
                msg(progress() .. "PREDIV2 divide factor configuration. The divider is connected to HSE.")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("RCC_PREDIV2_Div1",  value.RCC_PREDIV2_Div1 )
                add_item("RCC_PREDIV2_Div2",  value.RCC_PREDIV2_Div2 )
                add_item("RCC_PREDIV2_Div3",  value.RCC_PREDIV2_Div3 )
                add_item("RCC_PREDIV2_Div4",  value.RCC_PREDIV2_Div4 )
                add_item("RCC_PREDIV2_Div5",  value.RCC_PREDIV2_Div5 )
                add_item("RCC_PREDIV2_Div6",  value.RCC_PREDIV2_Div6 )
                add_item("RCC_PREDIV2_Div7",  value.RCC_PREDIV2_Div7 )
                add_item("RCC_PREDIV2_Div8",  value.RCC_PREDIV2_Div8 )
                add_item("RCC_PREDIV2_Div9",  value.RCC_PREDIV2_Div9 )
                add_item("RCC_PREDIV2_Div10", value.RCC_PREDIV2_Div10)
                add_item("RCC_PREDIV2_Div11", value.RCC_PREDIV2_Div11)
                add_item("RCC_PREDIV2_Div12", value.RCC_PREDIV2_Div12)
                add_item("RCC_PREDIV2_Div13", value.RCC_PREDIV2_Div13)
                add_item("RCC_PREDIV2_Div14", value.RCC_PREDIV2_Div14)
                add_item("RCC_PREDIV2_Div15", value.RCC_PREDIV2_Div15)
                add_item("RCC_PREDIV2_Div16", value.RCC_PREDIV2_Div16)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/pll_flags.h", "__PLL_PREDIV2_VAL__", choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_PLL2_on()
                local value = {}
                value.ENABLE  = "Enable"
                value.DISABLE = "Disable"

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_PLL2_ON__")
                msg(progress() .. "Do you want to enable PLL2?")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("ENABLE", value.ENABLE)
                add_item("DISABLE", value.DISABLE)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/pll_flags.h", "__PLL_PLL2_ON__", choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_PLL2_mul()
                local freq  = arch.stm32f1.get_frequencies()
                local value = {}
                value.RCC_PLL2Mul_8   = "x8 ("..funit(freq.PREDIV2OUTCLK*8)..")"
                value.RCC_PLL2Mul_9   = "x9 ("..funit(freq.PREDIV2OUTCLK*9)..")"
                value.RCC_PLL2Mul_10  = "x10 ("..funit(freq.PREDIV2OUTCLK*10)..")"
                value.RCC_PLL2Mul_11  = "x11 ("..funit(freq.PREDIV2OUTCLK*11)..")"
                value.RCC_PLL2Mul_12  = "x12 ("..funit(freq.PREDIV2OUTCLK*12)..")"
                value.RCC_PLL2Mul_13  = "x13 ("..funit(freq.PREDIV2OUTCLK*13)..")"
                value.RCC_PLL2Mul_14  = "x14 ("..funit(freq.PREDIV2OUTCLK*14)..")"
                value.RCC_PLL2Mul_16  = "x16 ("..funit(freq.PREDIV2OUTCLK*16)..")"
                value.RCC_PLL2Mul_20  = "x20 ("..funit(freq.PREDIV2OUTCLK*20)..")"

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_PLL2_MULL__")
                msg(progress() .. "PLL2 multiplication factor configuration.")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("RCC_PLL2Mul_8",  value.RCC_PLL2Mul_8)
                add_item("RCC_PLL2Mul_9",  value.RCC_PLL2Mul_9)
                add_item("RCC_PLL2Mul_10", value.RCC_PLL2Mul_10)
                add_item("RCC_PLL2Mul_11", value.RCC_PLL2Mul_11)
                add_item("RCC_PLL2Mul_12", value.RCC_PLL2Mul_12)
                add_item("RCC_PLL2Mul_13", value.RCC_PLL2Mul_13)
                add_item("RCC_PLL2Mul_14", value.RCC_PLL2Mul_14)
                add_item("RCC_PLL2Mul_16", value.RCC_PLL2Mul_16)
                add_item("RCC_PLL2Mul_20", value.RCC_PLL2Mul_20)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/pll_flags.h", "__PLL_PLL2_MULL__", choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_PLL3_on()
                local value = {}
                value.ENABLE  = "Enable"
                value.DISABLE = "Disable"

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_PLL3_ON__")
                msg(progress() .. "Do you want to enable PLL3?")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("ENABLE", value.ENABLE)
                add_item("DISABLE", value.DISABLE)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/pll_flags.h", "__PLL_PLL3_ON__", choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_PLL3_mul()
                local freq  = arch.stm32f1.get_frequencies()
                local value = {}
                value.RCC_PLL3Mul_8   = "x8 ("..funit(freq.PREDIV2OUTCLK*8)..")"
                value.RCC_PLL3Mul_9   = "x9 ("..funit(freq.PREDIV2OUTCLK*9)..")"
                value.RCC_PLL3Mul_10  = "x10 ("..funit(freq.PREDIV2OUTCLK*10)..")"
                value.RCC_PLL3Mul_11  = "x11 ("..funit(freq.PREDIV2OUTCLK*11)..")"
                value.RCC_PLL3Mul_12  = "x12 ("..funit(freq.PREDIV2OUTCLK*12)..")"
                value.RCC_PLL3Mul_13  = "x13 ("..funit(freq.PREDIV2OUTCLK*13)..")"
                value.RCC_PLL3Mul_14  = "x14 ("..funit(freq.PREDIV2OUTCLK*14)..")"
                value.RCC_PLL3Mul_16  = "x16 ("..funit(freq.PREDIV2OUTCLK*16)..")"
                value.RCC_PLL3Mul_20  = "x20 ("..funit(freq.PREDIV2OUTCLK*20)..")"

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_PLL3_MULL__")
                msg(progress() .. "PLL3 multiplication factor configuration.")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("RCC_PLL3Mul_8",  value.RCC_PLL3Mul_8)
                add_item("RCC_PLL3Mul_9",  value.RCC_PLL3Mul_9)
                add_item("RCC_PLL3Mul_10", value.RCC_PLL3Mul_10)
                add_item("RCC_PLL3Mul_11", value.RCC_PLL3Mul_11)
                add_item("RCC_PLL3Mul_12", value.RCC_PLL3Mul_12)
                add_item("RCC_PLL3Mul_13", value.RCC_PLL3Mul_13)
                add_item("RCC_PLL3Mul_14", value.RCC_PLL3Mul_14)
                add_item("RCC_PLL3Mul_16", value.RCC_PLL3Mul_16)
                add_item("RCC_PLL3Mul_20", value.RCC_PLL3Mul_20)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/pll_flags.h", "__PLL_PLL3_MULL__", choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_AHB_pre()
                local freq  = arch.stm32f1.get_frequencies()
                local value = {}
                value.RCC_SYSCLK_Div1   = "AHB clock = SYSCLK / 1 ("..funit(freq.SYSCLK/1)..")"
                value.RCC_SYSCLK_Div2   = "AHB clock = SYSCLK / 2 ("..funit(freq.SYSCLK/2)..")"
                value.RCC_SYSCLK_Div4   = "AHB clock = SYSCLK / 4 ("..funit(freq.SYSCLK/4)..")"
                value.RCC_SYSCLK_Div8   = "AHB clock = SYSCLK / 8 ("..funit(freq.SYSCLK/8)..")"
                value.RCC_SYSCLK_Div16  = "AHB clock = SYSCLK / 16 ("..funit(freq.SYSCLK/16)..")"
                value.RCC_SYSCLK_Div64  = "AHB clock = SYSCLK / 64 ("..funit(freq.SYSCLK/64)..")"
                value.RCC_SYSCLK_Div128 = "AHB clock = SYSCLK / 128 ("..funit(freq.SYSCLK/128)..")"
                value.RCC_SYSCLK_Div256 = "AHB clock = SYSCLK / 256 ("..funit(freq.SYSCLK/256)..")"
                value.RCC_SYSCLK_Div512 = "AHB clock = SYSCLK / 512 ("..funit(freq.SYSCLK/512)..")"

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_AHB_PRE__")
                msg(progress() .. "AHB prescaler configuration.")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("RCC_SYSCLK_Div1",   value.RCC_SYSCLK_Div1  )
                add_item("RCC_SYSCLK_Div2",   value.RCC_SYSCLK_Div2  )
                add_item("RCC_SYSCLK_Div4",   value.RCC_SYSCLK_Div4  )
                add_item("RCC_SYSCLK_Div8",   value.RCC_SYSCLK_Div8  )
                add_item("RCC_SYSCLK_Div16",  value.RCC_SYSCLK_Div16 )
                add_item("RCC_SYSCLK_Div64",  value.RCC_SYSCLK_Div64 )
                add_item("RCC_SYSCLK_Div128", value.RCC_SYSCLK_Div128)
                add_item("RCC_SYSCLK_Div256", value.RCC_SYSCLK_Div256)
                add_item("RCC_SYSCLK_Div512", value.RCC_SYSCLK_Div512)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/pll_flags.h", "__PLL_AHB_PRE__", choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_APB1_pre()
                local freq  = arch.stm32f1.get_frequencies()
                local value = {}
                value.RCC_HCLK_Div1  = "APB1 clock = HCLK / 1 ("..funit(freq.HCLK/1)..")"
                value.RCC_HCLK_Div2  = "APB1 clock = HCLK / 2 ("..funit(freq.HCLK/2)..")"
                value.RCC_HCLK_Div4  = "APB1 clock = HCLK / 4 ("..funit(freq.HCLK/4)..")"
                value.RCC_HCLK_Div8  = "APB1 clock = HCLK / 8 ("..funit(freq.HCLK/8)..")"
                value.RCC_HCLK_Div16 = "APB1 clock = HCLK / 16 ("..funit(freq.HCLK/16)..")"

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_APB1_PRE__")
                msg(progress() .. "APB1 prescaler configuration.")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("RCC_HCLK_Div1",  value.RCC_HCLK_Div1 )
                add_item("RCC_HCLK_Div2",  value.RCC_HCLK_Div2 )
                add_item("RCC_HCLK_Div4",  value.RCC_HCLK_Div4 )
                add_item("RCC_HCLK_Div8",  value.RCC_HCLK_Div8 )
                add_item("RCC_HCLK_Div16", value.RCC_HCLK_Div16)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/pll_flags.h", "__PLL_APB1_PRE__", choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_APB2_pre()
                local freq  = arch.stm32f1.get_frequencies()
                local value = {}
                value.RCC_HCLK_Div1  = "APB2 clock = HCLK / 1 ("..funit(freq.HCLK/1)..")"
                value.RCC_HCLK_Div2  = "APB2 clock = HCLK / 2 ("..funit(freq.HCLK/2)..")"
                value.RCC_HCLK_Div4  = "APB2 clock = HCLK / 4 ("..funit(freq.HCLK/4)..")"
                value.RCC_HCLK_Div8  = "APB2 clock = HCLK / 8 ("..funit(freq.HCLK/8)..")"
                value.RCC_HCLK_Div16 = "APB2 clock = HCLK / 16 ("..funit(freq.HCLK/16)..")"

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_APB2_PRE__")
                msg(progress() .. "APB2 prescaler configuration.")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("RCC_HCLK_Div1",  value.RCC_HCLK_Div1 )
                add_item("RCC_HCLK_Div2",  value.RCC_HCLK_Div2 )
                add_item("RCC_HCLK_Div4",  value.RCC_HCLK_Div4 )
                add_item("RCC_HCLK_Div8",  value.RCC_HCLK_Div8 )
                add_item("RCC_HCLK_Div16", value.RCC_HCLK_Div16)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/pll_flags.h", "__PLL_APB2_PRE__", choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_ADC_pre()
                local freq  = arch.stm32f1.get_frequencies()
                local value = {}
                value.RCC_PCLK2_Div2 = "ADC clock = PCLK2 / 2 ("..funit(freq.PCLK2/2)..")"
                value.RCC_PCLK2_Div4 = "ADC clock = PCLK2 / 4 ("..funit(freq.PCLK2/4)..")"
                value.RCC_PCLK2_Div6 = "ADC clock = PCLK2 / 6 ("..funit(freq.PCLK2/6)..")"
                value.RCC_PCLK2_Div8 = "ADC clock = PCLK2 / 8 ("..funit(freq.PCLK2/8)..")"

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_ADC_PRE__")
                msg(progress() .. "ADC prescaler configuration.")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("RCC_PCLK2_Div2", value.RCC_PCLK2_Div2)
                add_item("RCC_PCLK2_Div4", value.RCC_PCLK2_Div4)
                add_item("RCC_PCLK2_Div6", value.RCC_PCLK2_Div6)
                add_item("RCC_PCLK2_Div8", value.RCC_PCLK2_Div8)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/pll_flags.h", "__PLL_ADC_PRE__", choice)
                end

                configure_flash_latency()

                return choice
        end

        if key_read("../project/Makefile", "ENABLE_PLL") == yes then

                if family == "STM32F10X_CL" then
                        ::lsi_on::      if configure_LSI_on()      == back then return back      end
                        ::lse_on::      if configure_LSE_on()      == back then goto lsi_on      end
                        ::hse_on::      if configure_HSE_on()      == back then goto lse_on      end
                        ::prediv2_val:: if configure_prediv2_val() == back then goto hse_on      end
                        ::pll2_on::     if configure_PLL2_on()     == back then goto prediv2_val end
                        ::pll2_mull::   if configure_PLL2_mul()    == back then goto pll2_on     end
                        ::pll3_on::     if configure_PLL3_on()     == back then goto pll2_mull   end
                        ::pll3_mull::   if configure_PLL3_mul()    == back then goto pll3_on     end
                        ::prediv1_src:: if configure_prediv1_src() == back then goto pll3_mull   end
                        ::prediv1_val:: if configure_prediv1_val() == back then goto prediv1_src end
                        ::pll_on::      if configure_PLL_on()      == back then goto prediv1_val end
                        ::pll_src::     if configure_PLL_src()     == back then goto pll_on      end
                        ::pll_mull::    if configure_PLL_mul()     == back then goto pll_src     end
                        ::sysclk_src::  if configure_sys_clk_src() == back then goto pll_mull    end
                        ::rtc_src::     if configure_RTC_clk_src() == back then goto sysclk_src  end
                        ::mco_src::     if configure_MCO_src()     == back then goto rtc_src     end
                        ::usb_div::     if configure_USB_div()     == back then goto mco_src     end
                        ::ahb_pre::     if configure_AHB_pre()     == back then goto usb_div     end
                        ::apb1_pre::    if configure_APB1_pre()    == back then goto ahb_pre     end
                        ::apb2_pre::    if configure_APB2_pre()    == back then goto apb1_pre    end
                        ::adc_pre::     if configure_ADC_pre()     == back then goto apb2_pre    end
                        ::i2s2_src::    if configure_I2S2_src()    == back then goto adc_pre     end
                        ::i2s3_src::    if configure_I2S3_src()    == back then goto i2s2_src    end
                else
                        ::lsi_on::      if configure_LSI_on()      == back then return back      end
                        ::lse_on::      if configure_LSE_on()      == back then goto lsi_on      end
                        ::hse_on::      if configure_HSE_on()      == back then goto lse_on      end
                        ::pll_on::      if configure_PLL_on()      == back then goto hse_on      end
                        ::pll_src::     if configure_PLL_src()     == back then goto pll_on      end
                        ::pll_mull::    if configure_PLL_mul()     == back then goto pll_src     end
                        ::sysclk_src::  if configure_sys_clk_src() == back then goto pll_mull    end
                        ::rtc_src::     if configure_RTC_clk_src() == back then goto sysclk_src  end
                        ::mco_src::     if configure_MCO_src()     == back then goto rtc_src     end
                        ::usb_div::     if configure_USB_div()     == back then goto mco_src     end
                        ::ahb_pre::     if configure_AHB_pre()     == back then goto usb_div     end
                        ::apb1_pre::    if configure_APB1_pre()    == back then goto ahb_pre     end
                        ::apb2_pre::    if configure_APB2_pre()    == back then goto apb1_pre    end
                        ::adc_pre::     if configure_ADC_pre()     == back then goto apb2_pre    end
                end

                print_summary()
        end

        return next
end

--------------------------------------------------------------------------------
-- METHODS
--------------------------------------------------------------------------------
function pll:get_frequencies()
        return arch[cpu:get_arch()].get_frequencies()
end

--------------------------------------------------------------------------------
-- @brief Function execute configuration
--------------------------------------------------------------------------------
function pll:configure()
        title("PLL module configuration for " .. cpu:get_name())
        navigation("Home/Modules/PLL")
        calculate_total_steps()

        ::pll_enable::
        if ask_for_enable() == back then
                return back
        end

        if arch[cpu:get_arch()].configure() == back then
                goto pll_enable
        end

        return next
end

-- started without master file
if (master ~= true) then
        show_no_master_info()
end

--------------------------------------------------------------------------------
-- END OF FILE
--------------------------------------------------------------------------------
