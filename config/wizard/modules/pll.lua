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

local configure   = {}
configure.stm32f1 = nil
configure.stm32f2 = nil
configure.stm32f3 = nil
configure.stm32f4 = nil

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
                                progress(1, 23)
                        else
                                progress(1, 14)
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
-- @brief PLL configuration for STM32F1
--------------------------------------------------------------------------------
configure.stm32f1 = function()
        local family = db:get_mcu_family(cpu:get_name())

        local function get_frequencies()
                local pllsource        = key_read("../stm32f1/pll_flags.h", "__PLL_PLL_SRC__")
                local sysclksource     = key_read("../stm32f1/pll_flags.h", "__PLL_SYS_CLK_SRC__")
                local prediv1source    = key_read("../stm32f1/pll_flags.h", "__PLL_PREDIV1_SRC__")
                local pllmull          = key_read("../stm32f1/pll_flags.h", "__PLL_PLL_MULL__"):gsub("6_5", "6.5"):gsub("RCC_PLLMul_", "")
                local prediv1factor    = key_read("../stm32f1/pll_flags.h", "__PLL_PREDIV1_VAL__"):gsub("RCC_PREDIV1_Div", "")
                local prediv2factor    = key_read("../stm32f1/pll_flags.h", "__PLL_PREDIV2_VAL__"):gsub("RCC_PREDIV2_Div", "")
                local pll2mull         = key_read("../stm32f1/pll_flags.h", "__PLL_PLL2_MULL__"):gsub("RCC_PLL2Mul_", "")
                local AHBprescaler     = key_read("../stm32f1/pll_flags.h", "__PLL_AHB_PRE__"):gsub("RCC_SYSCLK_Div", "")
                local APB1prescaler    = key_read("../stm32f1/pll_flags.h", "__PLL_APB1_PRE__"):gsub("RCC_HCLK_Div", "")
                local APB2prescaler    = key_read("../stm32f1/pll_flags.h", "__PLL_APB2_PRE__"):gsub("RCC_HCLK_Div", "")
                local ADCprescaler     = key_read("../stm32f1/pll_flags.h", "__PLL_ADC_PRE__"):gsub("RCC_PCLK2_Div", "")
                local HSE_Value        = key_read("../project/flags.h", "__CPU_OSC_FREQ__")
                local HSI_Value        = 8e6
                local LSI_Value        = 40e3
                local SYSCLK_Frequency = 0

                -- calculate SYSCLK frequency
                if sysclksource == "RCC_SYSCLKSource_HSI" then
                        SYSCLK_Frequency = HSI_Value

                elseif sysclksource == "RCC_SYSCLKSource_HSE" then
                        SYSCLK_Frequency = HSE_Value

                elseif sysclksource == "RCC_SYSCLKSource_PLLCLK" then
                        if pllsource == "RCC_PLLSource_HSI_Div2" then
                                SYSCLK_Frequency = (HSI_Value / 2) * pllmull
                        else
                                if family == "STM32F10X_CL" then
                                        if prediv1source == "RCC_PREDIV1_Source_HSE" then
                                                SYSCLK_Frequency = (HSE_Value / prediv1factor) * pllmull
                                        else
                                                SYSCLK_Frequency = (((HSE_Value / prediv2factor) * pll2mull) / prediv1factor) * pllmull
                                        end
                                else
                                        if pllsource == "RCC_PLLSource_HSE_Div2" then
                                                SYSCLK_Frequency = (HSE_Value / 2) * pllmull
                                        else
                                                SYSCLK_Frequency = HSE_Value * pllmull
                                        end
                                end
                        end
                end

                -- calculate USB frequency
                -- TODO

                -- calculate I2S frequency
                -- TODO

                -- calculate RTC frequency
                -- TODO

                -- calculate PLL Clk frequency
                -- TODO

                local HCLK_Frequency   = SYSCLK_Frequency / AHBprescaler
                local PCLK1_Frequency  = HCLK_Frequency / APB1prescaler
                local PCLK2_Frequency  = HCLK_Frequency / APB2prescaler
                local ADCCLK_Frequency = PCLK2_Frequency / ADCprescaler

                local freq  = {}
                freq.SYSCLK = SYSCLK_Frequency
                freq.HCLK   = HCLK_Frequency
                freq.PCLK1  = PCLK1_Frequency
                freq.PCLK2  = PCLK2_Frequency
                freq.ADCCLK = ADCCLK_Frequency
                freq.HSE    = HSE_Value
                freq.HSI    = HSI_Value
                freq.LSI    = LSI_Value

                return freq
        end

        local function configure_flash_latency()
                local freq          = get_frequencies()
                local flash_latency = 2
                local SYSCLK        = tonumber(freq.SYSCLK)

                if SYSCLK <= 24e6 then
                        flash_latency = 0
                elseif SYSCLK <= 48e6 then
                        flash_latency = 1
                elseif SYSCLK <= 72e6 then
                        flash_latency = 2
                end

                key_save("../stm32f1/pll_flags.h", "__PLL_FLASH_LATENCY__", flash_latency)
        end

        local function print_freq()
                local freq = get_frequencies()

                msg("PLL configuration summary:")
                msg("SYSCLK = "..freq.SYSCLK/1e6 .." MHz\n"..
                    "HCLK = "..freq.HCLK/1e6 .." MHz\n"..
                    "PCLK1 = "..freq.PCLK1/1e6 .." MHz\n"..
                    "PCLK2 = "..freq.PCLK2/1e6 .." MHz\n"..
                    "ADCCLK = "..freq.ADCCLK/1e6 .." MHz")

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
                local value = {}
                value.RCC_RTCCLKSource_LSE        = "LSE selected as RTC clock"
                value.RCC_RTCCLKSource_LSI        = "LSI selected as RTC clock"
                value.RCC_RTCCLKSource_HSE_Div128 = "HSE clock divided by 128 selected as RTC clock"

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
                local value = {}
                value.RCC_SYSCLKSource_HSI    = "HSI selected as system clock"
                value.RCC_SYSCLKSource_HSE    = "HSE selected as system clock"
                value.RCC_SYSCLKSource_PLLCLK = "PLL selected as system clock"

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
                local freq = get_frequencies()
                local value = {}
                value.RCC_MCO_NoClock      = "No clock selected (0 Hz)"
                value.RCC_MCO_SYSCLK       = "System clock selected ("..freq.SYSCLK/1e6 .." MHz)"
                value.RCC_MCO_HSI          = "HSI oscillator clock selected"
                value.RCC_MCO_HSE          = "HSE oscillator clock selected"
                value.RCC_MCO_PLLCLK_Div2  = "PLL clock divided by 2 selected"
                if family == "STM32F10X_CL" then
                value.RCC_MCO_PLL2CLK      = "PLL2 clock selected"
                value.RCC_MCO_PLL3CLK_Div2 = "PLL3 clock divided by 2 selected"
                value.RCC_MCO_XT1          = "External 3-25 MHz oscillator clock selected"
                value.RCC_MCO_PLL3CLK      = "PLL3 clock selected"
                end

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_MCO_SRC__")
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
                        key_save("../stm32f1/pll_flags.h", "__PLL_MCO_SRC__", choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_I2S2_src()
                local value = {}
                value.RCC_I2S2CLKSource_SYSCLK   = "System clock selected as I2S2 clock entry"
                value.RCC_I2S2CLKSource_PLL3_VCO = "PLL3 VCO clock selected as I2S2 clock entry"

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
                local value = {}
                value.RCC_I2S3CLKSource_SYSCLK   = "System clock selected as I2S3 clock entry"
                value.RCC_I2S3CLKSource_PLL3_VCO = "PLL3 VCO clock selected as I2S3 clock entry"

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_I2S3_SRC__")
                msg(progress() .. "Do you want to ?")
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
                local value = {}
                value.RCC_PLLSource_HSI_Div2 = "HSI oscillator clock divided by 2 selected as PLL clock entry"
                if family == "STM32F10X_CL" then
                value.RCC_PLLSource_PREDIV1  = "PREDIV1 clock selected as PLL clock entry"
                else
                value.RCC_PLLSource_HSE_Div1 = "HSE oscillator clock selected as PLL clock entry"
                value.RCC_PLLSource_HSE_Div2 = "HSE oscillator clock divided by 2 selected as PLL clock entry"
                end

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_PLL_SRC__")
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
                        key_save("../stm32f1/pll_flags.h", "__PLL_PLL_SRC__", choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_PLL_mul()
                local value = {}
                if family == "STM32F10X_CL" then
                value.RCC_PLLMul_4   = "x4"
                value.RCC_PLLMul_5   = "x5"
                value.RCC_PLLMul_6   = "x6"
                value.RCC_PLLMul_6_5 = "x6.5"
                value.RCC_PLLMul_7   = "x7"
                value.RCC_PLLMul_8   = "x8"
                value.RCC_PLLMul_9   = "x9"
                else
                value.RCC_PLLMul_2   = "x2"
                value.RCC_PLLMul_3   = "x3"
                value.RCC_PLLMul_4   = "x4"
                value.RCC_PLLMul_5   = "x5"
                value.RCC_PLLMul_6   = "x6"
                value.RCC_PLLMul_7   = "x7"
                value.RCC_PLLMul_8   = "x8"
                value.RCC_PLLMul_9   = "x9"
                value.RCC_PLLMul_10  = "x10"
                value.RCC_PLLMul_11  = "x11"
                value.RCC_PLLMul_12  = "x12"
                value.RCC_PLLMul_13  = "x13"
                value.RCC_PLLMul_14  = "x14"
                value.RCC_PLLMul_15  = "x15"
                value.RCC_PLLMul_16  = "x16"
                end

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_PLL_MULL__")
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
                        key_save("../stm32f1/pll_flags.h", "__PLL_PLL_MULL__", choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_USB_div()
                local value = {}
                if family == "STM32F10X_CL" then
                value.RCC_OTGFSCLKSource_PLLVCO_Div3 = "PLL VCO clock divided by 3 selected as USB OTG FS clock source"
                value.RCC_OTGFSCLKSource_PLLVCO_Div2 = "PLL VCO clock divided by 2 selected as USB OTG FS clock source"
                else
                value.RCC_USBCLKSource_PLLCLK_1Div5  = "PLL clock divided by 1.5 selected as USB clock source"
                value.RCC_USBCLKSource_PLLCLK_Div1   = "PLL clock selected as USB clock source"
                end

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_USB_DIV__")
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
                        key_save("../stm32f1/pll_flags.h", "__PLL_USB_DIV__", choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_prediv1_src()
                local value = {}
                value.RCC_PREDIV1_Source_HSE  = "HSE selected as PREDIV1 clock"
                value.RCC_PREDIV1_Source_PLL2 = "PLL2 selected as PREDIV1 clock"

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_PREDIV1_SRC__")
                msg(progress() .. "PLL pre-divider 1 clock source configuration.")
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
                local value = {}
                value.RCC_PREDIV1_Div1  = "/1"
                value.RCC_PREDIV1_Div2  = "/2"
                value.RCC_PREDIV1_Div3  = "/3"
                value.RCC_PREDIV1_Div4  = "/4"
                value.RCC_PREDIV1_Div5  = "/5"
                value.RCC_PREDIV1_Div6  = "/6"
                value.RCC_PREDIV1_Div7  = "/7"
                value.RCC_PREDIV1_Div8  = "/8"
                value.RCC_PREDIV1_Div9  = "/9"
                value.RCC_PREDIV1_Div10 = "/10"
                value.RCC_PREDIV1_Div11 = "/11"
                value.RCC_PREDIV1_Div12 = "/12"
                value.RCC_PREDIV1_Div13 = "/13"
                value.RCC_PREDIV1_Div14 = "/14"
                value.RCC_PREDIV1_Div15 = "/15"
                value.RCC_PREDIV1_Div16 = "/16"

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_PREDIV1_VAL__")
                msg(progress() .. "Pre-divider 1 divide value configuration.")
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
                local value = {}
                value.RCC_PREDIV2_Div1  = "/1"
                value.RCC_PREDIV2_Div2  = "/2"
                value.RCC_PREDIV2_Div3  = "/3"
                value.RCC_PREDIV2_Div4  = "/4"
                value.RCC_PREDIV2_Div5  = "/5"
                value.RCC_PREDIV2_Div6  = "/6"
                value.RCC_PREDIV2_Div7  = "/7"
                value.RCC_PREDIV2_Div8  = "/8"
                value.RCC_PREDIV2_Div9  = "/9"
                value.RCC_PREDIV2_Div10 = "/10"
                value.RCC_PREDIV2_Div11 = "/11"
                value.RCC_PREDIV2_Div12 = "/12"
                value.RCC_PREDIV2_Div13 = "/13"
                value.RCC_PREDIV2_Div14 = "/14"
                value.RCC_PREDIV2_Div15 = "/15"
                value.RCC_PREDIV2_Div16 = "/16"

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL_PREDIV2_VAL__")
                msg(progress() .. "Pre-divider 2 divide value configuration.")
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
                local value = {}
                value.RCC_PLL2Mul_8   = "x8"
                value.RCC_PLL2Mul_9   = "x9"
                value.RCC_PLL2Mul_10  = "x10"
                value.RCC_PLL2Mul_11  = "x11"
                value.RCC_PLL2Mul_12  = "x12"
                value.RCC_PLL2Mul_13  = "x13"
                value.RCC_PLL2Mul_14  = "x14"
                value.RCC_PLL2Mul_16  = "x16"
                value.RCC_PLL2Mul_20  = "x20"

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
                local value = {}
                value.RCC_PLL3Mul_8   = "x8"
                value.RCC_PLL3Mul_9   = "x9"
                value.RCC_PLL3Mul_10  = "x10"
                value.RCC_PLL3Mul_11  = "x11"
                value.RCC_PLL3Mul_12  = "x12"
                value.RCC_PLL3Mul_13  = "x13"
                value.RCC_PLL3Mul_14  = "x14"
                value.RCC_PLL3Mul_16  = "x16"
                value.RCC_PLL3Mul_20  = "x20"

                local choice = key_read("../stm32f1/pll_flags.h", "__PLL3_MULL__")
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
                        key_save("../stm32f1/pll_flags.h", "__PLL3_MULL__", choice)
                end

                configure_flash_latency()

                return choice
        end

        local function configure_AHB_pre()
                local freq  = get_frequencies()
                local value = {}
                value.RCC_SYSCLK_Div1   = "AHB clock = SYSCLK/1 ("..freq.SYSCLK/1/1e6 .." MHz)"
                value.RCC_SYSCLK_Div2   = "AHB clock = SYSCLK/2 ("..freq.SYSCLK/2/1e6 .." MHz)"
                value.RCC_SYSCLK_Div4   = "AHB clock = SYSCLK/4 ("..freq.SYSCLK/4/1e6 .." MHz)"
                value.RCC_SYSCLK_Div8   = "AHB clock = SYSCLK/8 ("..freq.SYSCLK/8/1e6 .." MHz)"
                value.RCC_SYSCLK_Div16  = "AHB clock = SYSCLK/16 ("..freq.SYSCLK/16/1e6 .." MHz)"
                value.RCC_SYSCLK_Div64  = "AHB clock = SYSCLK/64 ("..freq.SYSCLK/64/1e6 .." MHz)"
                value.RCC_SYSCLK_Div128 = "AHB clock = SYSCLK/128 ("..freq.SYSCLK/128/1e6 .." MHz)"
                value.RCC_SYSCLK_Div256 = "AHB clock = SYSCLK/256 ("..freq.SYSCLK/256/1e6 .." MHz)"
                value.RCC_SYSCLK_Div512 = "AHB clock = SYSCLK/512 ("..freq.SYSCLK/512/1e6 .." MHz)"

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
                local freq  = get_frequencies()
                local value = {}
                value.RCC_HCLK_Div1  = "APB1 clock = HCLK/1 ("..freq.HCLK/1/1e6 .." MHz)"
                value.RCC_HCLK_Div2  = "APB1 clock = HCLK/2 ("..freq.HCLK/2/1e6 .." MHz)"
                value.RCC_HCLK_Div4  = "APB1 clock = HCLK/4 ("..freq.HCLK/4/1e6 .." MHz)"
                value.RCC_HCLK_Div8  = "APB1 clock = HCLK/8 ("..freq.HCLK/8/1e6 .." MHz)"
                value.RCC_HCLK_Div16 = "APB1 clock = HCLK/16 ("..freq.HCLK/16/1e6 .." MHz)"

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
                local freq  = get_frequencies()
                local value = {}
                value.RCC_HCLK_Div1  = "APB2 clock = HCLK/1 ("..freq.HCLK/1/1e6 .." MHz)"
                value.RCC_HCLK_Div2  = "APB2 clock = HCLK/2 ("..freq.HCLK/2/1e6 .." MHz)"
                value.RCC_HCLK_Div4  = "APB2 clock = HCLK/4 ("..freq.HCLK/4/1e6 .." MHz)"
                value.RCC_HCLK_Div8  = "APB2 clock = HCLK/8 ("..freq.HCLK/8/1e6 .." MHz)"
                value.RCC_HCLK_Div16 = "APB2 clock = HCLK/16 ("..freq.HCLK/16/1e6 .." MHz)"

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
                local freq  = get_frequencies()
                local value = {}
                value.RCC_PCLK2_Div2 = "ADC clock = PCLK2/2 ("..freq.PCLK2/2/1e6 .." MHz)"
                value.RCC_PCLK2_Div4 = "ADC clock = PCLK2/4 ("..freq.PCLK2/4/1e6 .." MHz)"
                value.RCC_PCLK2_Div6 = "ADC clock = PCLK2/6 ("..freq.PCLK2/6/1e6 .." MHz)"
                value.RCC_PCLK2_Div8 = "ADC clock = PCLK2/8 ("..freq.PCLK2/8/1e6 .." MHz)"

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

                ::lsi_on::      if configure_LSI_on()      == back then return back     end
                ::lse_on::      if configure_LSE_on()      == back then goto lsi_on     end
                ::hse_on::      if configure_HSE_on()      == back then goto lse_on     end
                ::rtc_src::     if configure_RTC_clk_src() == back then goto hse_on     end
                ::sysclk_src::  if configure_sys_clk_src() == back then goto rtc_src    end
                ::mco_src::     if configure_MCO_src()     == back then goto sysclk_src end
                ::pll_on::      if configure_PLL_on()      == back then goto mco_src    end
                ::pll_mull::    if configure_PLL_mul()     == back then goto pll_on     end
                ::usb_div::     if configure_USB_div()     == back then goto pll_mull   end
                ::ahb_pre::     if configure_AHB_pre()     == back then goto usb_div    end
                ::apb1_pre::    if configure_APB1_pre()    == back then goto ahb_pre    end
                ::apb2_pre::    if configure_APB2_pre()    == back then goto apb1_pre   end
                ::adc_pre::     if configure_ADC_pre()     == back then goto apb2_pre   end

                if family == "STM32F10X_CL" then
                        ::i2s2_src::    if configure_I2S2_src()    == back then goto adc_pre     end
                        ::i2s3_src::    if configure_I2S3_src()    == back then goto i2s2_src    end
                        ::prediv1_src:: if configure_prediv1_src() == back then goto i2s3_src    end
                        ::prediv1_val:: if configure_prediv1_val() == back then goto prediv1_src end
                        ::prediv2_val:: if configure_prediv2_val() == back then goto prediv1_val end
                        ::pll2_on::     if configure_PLL2_on()     == back then goto prediv2_val end
                        ::pll2_mull::   if configure_PLL2_mul()    == back then goto pll2_on     end
                        ::pll3_on::     if configure_PLL3_on()     == back then goto pll2_mull   end
                        ::pll3_mull::   if configure_PLL3_mul()    == back then goto pll3_on     end
                end

                print_freq()
        end

        return next
end

--------------------------------------------------------------------------------
-- @brief Function execute configuration
--------------------------------------------------------------------------------
function pll:configure()
        title("PLL module configuration for ".. cpu:get_name())
        navigation("Home/Modules/PLL")
        calculate_total_steps()

        ::pll_enable::
        if ask_for_enable() == back then
                return back
        end

        if configure[cpu:get_arch()]() == back then
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
