--[[============================================================================
@file    afio.lua

@author  Daniel Zorychta

@brief   AFIO module configuration.

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

require "modules/defs"
require "modules/cpu"
require "modules/db"

--------------------------------------------------------------------------------
-- GLOBAL OBJECTS
--------------------------------------------------------------------------------
afio = {}

local stm32f1_port_names = {"PA", "PB", "PC", "PD", "PE", "PF", "PG"}
local stm32f2_port_names = {"PA", "PB", "PC", "PD", "PE", "PF", "PG"}
local stm32f3_port_names = {"PA", "PB", "PC", "PD", "PE", "PF", "PG"}
local stm32f4_port_names = {"PA", "PB", "PC", "PD", "PE", "PF", "PG"}

--------------------------------------------------------------------------------
-- FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Function enable/disable Event Output
--------------------------------------------------------------------------------
local function stm32f1_configure_EVO_enable()
        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_EVENT_OUT_ENABLE__")
        if choice == yes then
                progress(1, 3)
        else
                progress(1, 1)
        end

        msg(progress() .. "Do you want to enable Cortex Event Output?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "Yes")
        add_item(no, "No")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_EVENT_OUT_ENABLE__", choice)
                progress(nil, 3)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure EVO port
--------------------------------------------------------------------------------
local function stm32f1_configure_EVO_port()
        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_EVENT_OUT_PORT__")
        msg(progress() .. "Select port for Cortex Event Output.")
        msg("Current choice is: " .. stm32f1_port_names[choice + 1] .. ".")
        for i = 0, 4 do
                add_item(tostring(i), "Port "..stm32f1_port_names[i + 1])
        end
        choice = get_selection()
        if can_be_saved(choice) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_EVENT_OUT_PORT__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function configure EVO port
--------------------------------------------------------------------------------
local function stm32f1_configure_EVO_pin()
        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_EVENT_OUT_PIN__")
        msg(progress() .. "Select pin for Cortex Event Output.")
        msg("Current choice is: " .. choice .. ".")
        for i = 0, 15 do
                add_item(tostring(i), "Pin "..i)
        end
        choice = get_selection()
        if can_be_saved(choice) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_EVENT_OUT_PIN__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap SPI1
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_spi1()
        local remap  = {}
        remap.NO     = "No (NSS/PA15, SCK/PB3, MISO/PB4, MOSI/PB5)"
        remap.YES    = "Yes (NSS/PA4, SCK/PA5, MISO/PA6, MOSI/PA7)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_SPI1__")
        msg(progress() .. "Do you want to remap SPI1?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_SPI1__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap I2C1
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_i2c1()
        local remap = {}
        remap.NO    = "No (SCL/PB8, SDA/PB9)"
        remap.YES   = "Yes (SCL/PB6, SDA/PB7)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_I2C1__")
        msg(progress() .. "Do you want to remap I2C1?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_I2C1__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap USART1
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_usart1()
        local remap = {}
        remap.NO    = "No (TX/PA9, RX/PA10)"
        remap.YES   = "Yes (TX/PB6, RX/PB7)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_USART1__")
        msg(progress() .. "Do you want to remap USART1?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_USART1__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap USART2
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_usart2()
        local remap = {}
        remap.NO    = "No (CTS/PA0, RTS/PA1, TX/PA2, RX/PA3, CK/PA4)"
        remap.YES   = "Yes (CTS/PD3, RTS/PD4, TX/PD5, RX/PD6, CK/PD7)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_USART2__")
        msg(progress() .. "Do you want to remap USART2?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_USART2__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap USART3
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_usart3()
        local remap = {}
        remap["0"]  = "No (TX/PB10, RX/PB11, CK/PB12, CTS/PB13, RTS/PB14)"
        remap["1"]  = "Partial (CTS/PD3, RTS/PD4, TX/PD5, RX/PD6, CK/PD7)"
        remap["2"]  = "Full (TX/PD8, RX/PD9, CK/PD10, CTS/PD11, RTS/PD12)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_USART3__")
        msg(progress() .. "Do you want to remap USART3?")
        msg("Current choice is: " .. remap[choice] .. ".")
        add_item("0", remap["0"])
        add_item("1", remap["1"])
        add_item("2", remap["2"])
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_USART3__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap TIM1
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_tim1()
        local remap = {}
        remap["0"]  = "No (ETR/PA12, CH1/PA8, CH2/PA9, CH3/PA10, CH4/PA11, BKIN/PB12, CH1N/PB13, CH2N/PB14, CH3N/PB15)"
        remap["1"]  = "Partial (ETR/PA12, CH1/PA8, CH2/PA9, CH3/PA10, CH4/PA11, BKIN/PA6, CH1N/PA7, CH2N/PB0, CH3N/PB1)"
        remap["2"]  = "Full (ETR/PE7, CH1/PE9, CH2/PE11, CH3/PE13, CH4/PE14, BKIN/PE15, CH1N/PE8, CH2N/PE10, CH3N/PE12)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM1__")
        msg(progress() .. "Do you want to remap TIM1?")
        msg("Current choice is: " .. remap[choice] .. ".")
        add_item("0", remap["0"])
        add_item("1", remap["1"])
        add_item("2", remap["2"])
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM1__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap TIM2
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_tim2()
        local remap = {}
        remap["0"]  = "No (CH1/ETR/PA0, CH2/PA1, CH3/PA2, CH4/PA3)"
        remap["1"]  = "Partial (CH1/ETR/PA15, CH2/PB3, CH3/PA2, CH4/PA3)"
        remap["2"]  = "Partial (CH1/ETR/PA0, CH2/PA1, CH3/PB10, CH4/PB11)"
        remap["3"]  = "Full (CH1/ETR/PA15, CH2/PB3, CH3/PB10, CH4/PB11)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM2__")
        msg(progress() .. "Do you want to remap TIM2?")
        msg("Current choice is: " .. remap[choice] .. ".")
        add_item("0", remap["0"])
        add_item("1", remap["1"])
        add_item("2", remap["2"])
        add_item("3", remap["3"])
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM2__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap TIM3
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_tim3()
        local remap = {}
        remap["0"]  = "No (CH1/PA6, CH2/PA7, CH3/PB0, CH4/PB1)"
        remap["1"]  = "Partial (CH1/PB4, CH2/PB5, CH3/PB0, CH4/PB1)"
        remap["2"]  = "Full (CH1/PC6, CH2/PC7, CH3/PC8, CH4/PC9)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM3__")
        msg(progress() .. "Do you want to remap TIM3?")
        msg("Current choice is: " .. remap[choice] .. ".")
        add_item("0", remap["0"])
        add_item("1", remap["1"])
        add_item("2", remap["2"])
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM3__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap TIM4
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_tim4()
        local remap = {}
        remap.NO    = "No (TIM4_CH1/PB6, TIM4_CH2/PB7, TIM4_CH3/PB8, TIM4_CH4/PB9)"
        remap.YES   = "Yes (TIM4_CH1/PD12, TIM4_CH2/PD13, TIM4_CH3/PD14, TIM4_CH4/PD15)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM4__")
        msg(progress() .. "Do you want to remap TIM4?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM4__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap CAN
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_can()
        local remap = {}
        remap["0"]  = "No (CAN_RX/PA11, CAN_TX/PA12)"
        remap["1"]  = "Variant 1 (CAN_RX/PB8, CAN_TX/PB9)"
        remap["2"]  = "Variant 2 (CAN_RX/PD0, CAN_TX/PD1)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_CAN__")
        msg(progress() .. "Do you want to remap CAN?")
        msg("Current choice is: " .. remap[choice] .. ".")
        add_item("0", remap["0"])
        add_item("1", remap["1"])
        add_item("2", remap["2"])
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_CAN__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap PD0/PD1
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_pd01()
        local remap = {}
        remap.NO    = "No"
        remap.YES   = "Yes (PD0/OSC_IN, PD1/OSC_OUT)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_PD01__")
        msg(progress() .. "Do you want to remap PD0 and PD1?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_PD01__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap TIM5_CH4
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_tim5ch4()
        local remap = {}
        remap.NO    = "No (TIM5_CH4 connected to PA3)"
        remap.YES   = "Yes (LSI internal clock connected to TIM5_CH4)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM5CH4__")
        msg(progress() .. "Do you want to remap TIM5_CH4?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM5CH4__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap ADC1
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_adc1_etrginj()
        local remap = {}
        remap.NO    = "No (ADC1 External trigger injected conversion connected to EXTI15)"
        remap.YES   = "Yes (ADC1 External Event injected conversion connected to TIM8_CH4)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_ADC1_ETRGINJ__")
        msg(progress() .. "Do you want to remap ADC1 External trigger injected conversion?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_ADC1_ETRGINJ__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap ADC1
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_adc1_etrgreg()
        local remap = {}
        remap.NO    = "No (ADC1 External trigger regular conversion connected to EXTI11)"
        remap.YES   = "Yes (ADC1 External Event regular conversion connected to TIM8_TRGO)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_ADC1_ETRGREG__")
        msg(progress() .. "Do you want to remap ADC1 External trigger regular conversion?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_ADC1_ETRGREG__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap ADC2
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_adc2_etrginj()
        local remap = {}
        remap.NO    = "No (ADC2 External trigger injected conversion connected to EXTI15)"
        remap.YES   = "Yes (ADC2 External Event injected conversion connected to TIM8_CH4)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_ADC2_ETRGINJ__")
        msg(progress() .. "Do you want to remap ADC2 External trigger injected conversion?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_ADC2_ETRGINJ__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap ADC2
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_adc2_etrgreg()
        local remap = {}
        remap.NO    = "No (ADC2 External trigger regular conversion connected to EXTI11)"
        remap.YES   = "Yes (ADC2 External Event regular conversion connected to TIM8_TRGO)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_ADC2_ETRGREG__")
        msg(progress() .. "Do you want to remap ADC2 External trigger regular conversion?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_ADC2_ETRGREG__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap SWJ configuration
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_swj_cfg()
        local remap = {}
        remap["0"]  = "Full SWJ (JTAG-DP + SW-DP): Reset State"
        remap["1"]  = "Full SWJ (JTAG-DP + SW-DP) but without NJTRST"
        remap["2"]  = "JTAG-DP Disabled and SW-DP Enabled"
        remap["3"]  = "JTAG-DP Disabled and SW-DP Disabled"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_CAN__")
        msg(progress() .. "Serial wire JTAG configuration.")
        msg("Current choice is: " .. remap[choice] .. ".")
        add_item("0", remap["0"])
        add_item("1", remap["1"])
        add_item("2", remap["2"])
        add_item("3", remap["3"])
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_CAN__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap ETH
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_eth()
        local remap = {}
        remap.NO    = "No (RX_DV-CRS_DV/PA7, RXD0/PC4, RXD1/PC5, RXD2/PB0, RXD3/PB1)"
        remap.YES   = "Yes (RX_DV-CRS_DV/PD8, RXD0/PD9, RXD1/PD10, RXD2/PD11, RXD3/PD12)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_ETH__")
        msg(progress() .. "Do you want to remap Ethernet MAC I/O?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_ETH__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap CAN2
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_can2()
        local remap = {}
        remap.NO    = "No (CAN2_RX/PB12, CAN2_TX/PB13)"
        remap.YES   = "Yes (CAN2_RX/PB5, CAN2_TX/PB6)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_CAN2__")
        msg(progress() .. "Do you want to remap CAN2 I/O?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_CAN2__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap MII/RMII SEL
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_mii_rmii_sel()
        local remap = {}
        remap.NO    = "Configure Ethernet MAC for connection with an MII PHY"
        remap.YES   = "Configure Ethernet MAC for connection with an RMII PHY"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_MII_RMII_SEL__")
        msg(progress() .. "MII or RMII PHY connection configuration.")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_MII_RMII_SEL__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap SPI3
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_spi3()
        local remap = {}
        remap.NO    = "No (SPI_NSS-I2S3_WS/PA15, SPI3_SCK-I2S3_CK/PB3, SPI3_MISO/PB4, SPI3_MOSI-I2S3_SD/PB5)"
        remap.YES   = "Yes (SPI3_NSS-I2S3_WS/PA4, SPI3_SCK-I2S3_CK/PC10, SPI3_MISO/PC11, SPI3_MOSI-I2S3_SD/PC12)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_SPI3__")
        msg(progress() .. "Do you want to remap SPI3?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_SPI3__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap TIM2_ITR1
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_tim2itr1()
        local remap = {}
        remap.NO    = "No (TIM2_ITR1 connected internally to the Ethernet PTP output for calibration purposes)"
        remap.YES   = "Yes (USB OTG SOF output connected to TIM2_ITR1 for calibration purposes)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM2ITR1__")
        msg(progress() .. "Do you want to remap TIM2_ITR1?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM2ITR1__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap Ethernet PTP PPS
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_ptp_pps()
        local remap = {}
        remap.NO    = "No (PTP_PPS not output on PB5)"
        remap.YES   = "Yes (PTP_PPS is output on PB5)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_PTP_PPS__")
        msg(progress() .. "Do you want to remap Ethernet PTP PPS?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_PTP_PPS__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap TIM15
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_tim15()
        local remap = {}
        remap.NO    = "No (CH1/PA2, CH2/PA3)"
        remap.YES   = "Yes (CH1/PB14, CH2/PB15)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM15__")
        msg(progress() .. "Do you want to remap TIM15?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM15__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap TIM16
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_tim16()
        local remap = {}
        remap.NO    = "No (CH1/PB8)"
        remap.YES   = "Yes (CH1/PA6)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM16__")
        msg(progress() .. "Do you want to remap TIM16?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM16__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap TIM17
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_tim17()
        local remap = {}
        remap.NO    = "No (CH1/PB9)"
        remap.YES   = "Yes (CH1/PA7)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM17__")
        msg(progress() .. "Do you want to remap TIM17?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM17__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap CEC
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_cec()
        local remap = {}
        remap.NO    = "No (CEC/PB8)"
        remap.YES   = "Yes (CEC/PB10)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_CEC__")
        msg(progress() .. "Do you want to remap CEC?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_CEC__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap TIM1 DMA
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_tim1_dma()
        local remap = {}
        remap.NO    = "No (TIM1_CH1 DMA request/DMA1 Channel 2, TIM1_CH2 DMA request/DMA1 Channel 3)"
        remap.YES   = "Yes (TIM1_CH1 DMA request/DMA1 Channel 6, TIM1_CH2 DMA request/DMA1 Channel 6)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM1_DMA__")
        msg(progress() .. "Do you want to remap TIM1 DMA?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM1_DMA__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap TIM13
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_tim13()
        local remap = {}
        remap.NO    = "No"
        remap.YES   = "Yes"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM13__")
        msg(progress() .. "Do you want to remap TIM13_CH1?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM13__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap TIM14
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_tim14()
        local remap = {}
        remap.NO    = "No"
        remap.YES   = "Yes"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM14__")
        msg(progress() .. "Do you want to remap TIM14_CH1?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM14__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap FSMC_NADV
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_fsmc_nadv()
        local remap = {}
        remap.NO    = "No (NADV signal is connected to the output)"
        remap.YES   = "Yes (NADV signal is not connected)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_FSMC_NADV__")
        msg(progress() .. "Do you want to remap NADV signal connection?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_FSMC_NADV__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap TIM7/TIM6 DAC DMA
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_tim76_dac_dma()
        local remap = {}
        remap.NO    = "No (TIM6_DAC1 DMA request/DMA2 Channel 3, TIM7_DAC2 DMA request/DMA2 Channel 4)"
        remap.YES   = "Yes (TIM6_DAC1 DMA request/DMA1 Channel 3, TIM7_DAC2 DMA request/DMA1 Channel 4)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM76_DAC_DMA__")
        msg(progress() .. "Do you want to remap TIM6/TIM7 DAC DMA?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM76_DAC_DMA__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap TIM9
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_tim9()
        local remap = {}
        remap.NO    = "No (TIM9_CH1/PA2 and TIM9_CH2/PA3)"
        remap.YES   = "Yes (TIM9_CH1/PE5 and TIM9_CH2/PE6)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM9__")
        msg(progress() .. "Do you want to remap TIM9_CH1 and TIM9_CH2?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM9__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap TIM10
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_tim10()
        local remap = {}
        remap.NO    = "No (TIM10_CH1/PB8)"
        remap.YES   = "Yes (TIM10_CH1/PF6)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM10__")
        msg(progress() .. "Do you want to remap TIM10_CH1?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM10__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap TIM11
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_tim11()
        local remap = {}
        remap.NO    = "No (TIM11_CH1/PB9)"
        remap.YES   = "Yes (TIM11_CH1/PF7)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM11__")
        msg(progress() .. "Do you want to remap TIM11_CH1?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM11__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap TIM12
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_tim12()
        local remap = {}
        remap.NO    = "No (CH1/PC4, CH2/PC5)"
        remap.YES   = "Yes (CH1/PB12, CH2/PB13)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM12__")
        msg(progress() .. "Do you want to remap TIM12_CH1 and TIM12_CH2?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_TIM12__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function remap MISC
--------------------------------------------------------------------------------
local function stm32f1_configure_remap_misc()
        local remap = {}
        remap.NO    = "No (DMA2 channel 5 interrupt is mapped with DMA2 channel 4 at position 59, TIM5 TRGO event is selected as DAC Trigger 3, TIM5 triggers TIM1/3)"
        remap.YES   = "Yes (DMA2 channel 5 interrupt is mapped separately at position 60 and TIM15 TRGO event is selected as DAC Trigger 3, TIM15 triggers TIM1/3)"

        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_REMAP_MISC__")
        msg(progress() .. "Do you want to remap miscellaneous features?")
        msg("Current choice is: " .. remap[filter_yes_no(choice)] .. ".")
        add_item(yes, remap.YES)
        add_item(no, remap.NO)
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../stm32f1/afio_flags.h", "__AFIO_REMAP_MISC__", choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief AFIO configuration for stm32f1
--------------------------------------------------------------------------------
local function stm32f1_configure()
        local family = db:get_mcu_family(cpu:get_name())

        local function configure_evo()
                ::stm32f1_evo_en::
                if stm32f1_configure_EVO_enable() == back then
                        return back
                end

                ::stm32f1_evo_port::
                if key_read("../stm32f1/afio_flags.h", "__AFIO_EVENT_OUT_ENABLE__") == yes then
                        if stm32f1_configure_EVO_port() == back then
                                goto stm32f1_evo_en
                        end
                end

                ::stm32f1_evo_pin::
                if key_read("../stm32f1/afio_flags.h", "__AFIO_EVENT_OUT_ENABLE__") == yes then
                        if stm32f1_configure_EVO_pin() == back then
                                goto stm32f1_evo_port
                        end
                end
        end

        local function configure_remap()
                if     family == "STM32F10X_CL" then
                        progress(1, 23)
                elseif family == "STM32F10X_LD_VL" or family == "STM32F10X_MD_VL" then
                        progress(1, 22)
                elseif family == "STM32F10X_HD_VL" then
                        progress(1, 28)
                elseif family == "STM32F10X_XL" then
                        progress(1, 23)
                end

                ::stm32f1_remap_spi1::
                if stm32f1_configure_remap_spi1() == back then
                        return back
                end

                ::stm32f1_remap_i2c1::
                if stm32f1_configure_remap_i2c1() == back then
                        goto stm32f1_remap_spi1
                end

                ::stm32f1_remap_usart1::
                if stm32f1_configure_remap_usart1() == back then
                        goto stm32f1_remap_i2c1
                end

                ::stm32f1_remap_usart2::
                if stm32f1_configure_remap_usart2() == back then
                        goto stm32f1_remap_usart1
                end

                ::stm32f1_remap_usart3::
                if stm32f1_configure_remap_usart3() == back then
                        goto stm32f1_remap_usart2
                end

                ::stm32f1_remap_tim1::
                if stm32f1_configure_remap_tim1() == back then
                        goto stm32f1_remap_usart3
                end

                ::stm32f1_remap_tim2::
                if stm32f1_configure_remap_tim2() == back then
                        goto stm32f1_remap_tim1
                end

                ::stm32f1_remap_tim3::
                if stm32f1_configure_remap_tim3() == back then
                        goto stm32f1_remap_tim2
                end

                ::stm32f1_remap_tim4::
                if stm32f1_configure_remap_tim4() == back then
                        goto stm32f1_remap_tim3
                end

                ::stm32f1_remap_can::
                if stm32f1_configure_remap_can() == back then
                        goto stm32f1_remap_tim4
                end

                ::stm32f1_remap_pd01::
                if stm32f1_configure_remap_pd01() == back then
                        goto stm32f1_remap_can
                end

                ::stm32f1_remap_tim5ch4::
                if stm32f1_configure_remap_tim5ch4() == back then
                        goto stm32f1_remap_pd01
                end

                ::stm32f1_remap_adc1_etrginj::
                if stm32f1_configure_remap_adc1_etrginj() == back then
                        goto stm32f1_remap_tim5ch4
                end

                ::stm32f1_remap_adc1_etrgreg::
                if stm32f1_configure_remap_adc1_etrgreg() == back then
                        goto stm32f1_remap_adc1_etrginj
                end

                ::stm32f1_remap_adc2_etrginj::
                if stm32f1_configure_remap_adc2_etrginj() == back then
                        goto stm32f1_remap_adc1_etrgreg
                end

                ::stm32f1_remap_adc2_etrgreg::
                if stm32f1_configure_remap_adc2_etrgreg() == back then
                        goto stm32f1_remap_adc2_etrginj
                end

                ::stm32f1_remap_swj_cfg::
                if stm32f1_configure_remap_swj_cfg() == back then
                        goto stm32f1_remap_adc2_etrgreg
                end

                if family == "STM32F10X_CL" then
                        ::stm32f1_cl_remap_eth::
                        if stm32f1_configure_remap_eth() == back then
                                goto stm32f1_remap_swj_cfg
                        end

                        ::stm32f1_cl_remap_can2::
                        if stm32f1_configure_remap_can2() == back then
                                goto stm32f1_cl_remap_eth
                        end

                        ::stm32f1_cl_remap_mii_rmii_sel::
                        if stm32f1_configure_remap_mii_rmii_sel() == back then
                                goto stm32f1_cl_remap_can2
                        end

                        ::stm32f1_cl_remap_spi3::
                        if stm32f1_configure_remap_spi3() == back then
                                goto stm32f1_cl_remap_mii_rmii_sel
                        end

                        ::stm32f1_cl_remap_tim2itr1::
                        if stm32f1_configure_remap_tim2itr1() == back then
                                goto stm32f1_cl_remap_spi3
                        end

                        ::stm32f1_remap_ptp_pps::
                        if stm32f1_configure_remap_ptp_pps() == back then
                                goto stm32f1_cl_remap_tim2itr1
                        end

                        return next
                end

                if family == "STM32F10X_LD_VL" or family == "STM32F10X_MD_VL" or family == "STM32F10X_HD_VL" then
                        ::stm32f1_ld_md_hd_vl_remap_tim15::
                        if stm32f1_configure_remap_tim15() == back then
                                goto stm32f1_remap_swj_cfg
                        end

                        ::stm32f1_ld_md_hd_vl_remap_tim16::
                        if stm32f1_configure_remap_tim16() == back then
                                goto stm32f1_ld_md_hd_vl_remap_tim15
                        end

                        ::stm32f1_ld_md_hd_vl_remap_tim17::
                        if stm32f1_configure_remap_tim17() == back then
                                goto stm32f1_ld_md_hd_vl_remap_tim16
                        end

                        ::stm32f1_ld_md_hd_vl_remap_cec::
                        if stm32f1_configure_remap_cec() == back then
                                goto stm32f1_ld_md_hd_vl_remap_tim17
                        end

                        ::stm32f1_ld_md_hd_vl_remap_tim1_dma::
                        if stm32f1_configure_remap_tim1_dma() == back then
                                goto stm32f1_ld_md_hd_vl_remap_cec
                        end

                        if family == "STM32F10X_HD_VL" then
                                ::stm32f1_hd_vl_remap_tim13::
                                if stm32f1_configure_remap_tim13() == back then
                                        goto stm32f1_ld_md_hd_vl_remap_tim1_dma
                                end

                                ::stm32f1_hd_vl_remap_tim14::
                                if stm32f1_configure_remap_tim14() == back then
                                        goto stm32f1_hd_vl_remap_tim13
                                end

                                ::stm32f1_hd_vl_remap_fsmc_nadv::
                                if stm32f1_configure_remap_fsmc_nadv() == back then
                                        goto stm32f1_hd_vl_remap_tim14
                                end

                                ::stm32f1_hd_vl_remap_tim76_dac_dma::
                                if stm32f1_configure_remap_tim76_dac_dma() == back then
                                        goto stm32f1_hd_vl_remap_fsmc_nadv
                                end

                                ::stm32f1_hd_vl_remap_tim12::
                                if stm32f1_configure_remap_tim12() == back then
                                        goto stm32f1_hd_vl_remap_tim76_dac_dma
                                end

                                ::stm32f1_hd_vl_remap_misc::
                                if stm32f1_configure_remap_misc() == back then
                                        goto stm32f1_hd_vl_remap_tim12
                                end
                        end

                        return next
                end

                if family == "STM32F10X_XL" then
                        ::stm32f1_xl_remap_tim9::
                        if stm32f1_configure_remap_tim9() == back then
                                goto stm32f1_remap_swj_cfg
                        end

                        ::stm32f1_xl_remap_tim10::
                        if stm32f1_configure_remap_tim10() == back then
                                goto stm32f1_xl_remap_tim9
                        end

                        ::stm32f1_xl_remap_tim11::
                        if stm32f1_configure_remap_tim11() == back then
                                goto stm32f1_xl_remap_tim10
                        end

                        ::stm32f1_xl_remap_tim13::
                        if stm32f1_configure_remap_tim13() == back then
                                goto stm32f1_xl_remap_tim11
                        end

                        ::stm32f1_xl_remap_tim14::
                        if stm32f1_configure_remap_tim14() == back then
                                goto stm32f1_xl_remap_tim13
                        end

                        ::stm32f1_xl_remap_fsmc_nadv::
                        if stm32f1_configure_remap_fsmc_nadv() == back then
                                goto stm32f1_xl_remap_tim14
                        end
                end
        end

        local function configure_exti()
                progress(1, 16)

                local exti = 0
                repeat
                        local choice = key_read("../stm32f1/afio_flags.h", "__AFIO_EXTI"..exti.."_PORT__")
                        msg(progress().."Select port source for EXTI"..exti..".")
                        msg("Current choice is: " .. stm32f1_port_names[choice + 1] .. ".")
                        for i = 0, 6 do
                                add_item(tostring(i), "Port "..stm32f1_port_names[i + 1])
                        end
                        choice = get_selection()
                        if can_be_saved(choice) then
                                key_save("../stm32f1/afio_flags.h", "__AFIO_EXTI"..exti.."_PORT__", choice)
                        end

                        if choice == back then
                                exti = exti - 1
                        elseif choice == next then
                                exti = exti + 1
                        end
                until exti < 0 or exti >= 16
        end

        while true do
                title("AFIO configuration for " .. cpu:get_name())
                msg("Select AFIO part to configure.")
                add_item("EVO", "Cortex Event Output")
                add_item("remap", "Peripheral remap")
                add_item("exti", "EXTI port assign")
                local choice = get_selection()
                if choice == back then
                        return back
                end

                if     choice == "EVO" then
                        title("AFIO configuration for " .. cpu:get_name().." (Cortex Event output)")
                        configure_evo()
                elseif choice == "remap" then
                        title("AFIO configuration for " .. cpu:get_name().." (Peripheral remap)")
                        configure_remap()
                elseif choice == "exti" then
                        title("AFIO configuration for " .. cpu:get_name().." (EXTI port assign)")
                        configure_exti()
                end
        end
end

--------------------------------------------------------------------------------
-- @brief Function execute configuration
--------------------------------------------------------------------------------
function afio:configure()
        navigation("Home/Modules/AFIO")

        local arch = cpu:get_arch()

        if arch == "stm32f1" then
                stm32f1_configure()
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
