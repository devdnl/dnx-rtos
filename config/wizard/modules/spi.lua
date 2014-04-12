--[[============================================================================
@file    sdspi.lua

@author  Daniel Zorychta

@brief   SDSPI configuration wizard.

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
require "gpio"

--------------------------------------------------------------------------------
-- OBJECTS
--------------------------------------------------------------------------------
spi = {}

local arch = {}
arch.stm32f1            = {}
arch.stm32f1.configure  = nil

arch.stm32f2            = {}
arch.stm32f2.configure  = nil

arch.stm32f3            = {}
arch.stm32f3.configure  = nil

arch.stm32f4            = {}
arch.stm32f4.configure  = nil

--------------------------------------------------------------------------------
-- FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Calculate total steps
--------------------------------------------------------------------------------
local function calculate_total_steps()
        local arch   = cpu:get_arch()
        local family = db:get_mcu_family(cpu:get_name())

        if key_read("../project/Makefile", "ENABLE_SPI") == yes then
                if arch == "stm32f1" then
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
        local choice = key_read("../project/flags.h", "__ENABLE_SPI__")
        msg(progress(1, 6).."Do you want to enable SPI module?")
        msg("Current selection is: "..filter_yes_no(choice)..".")
        add_item(yes, "Yes")
        add_item(no, "No")
        choice = get_selection()
        if can_be_saved(choice) then
                key_save("../project/flags.h", "__ENABLE_SPI__", choice)
                key_save("../project/Makefile", "ENABLE_SPI", choice)
        end

        calculate_total_steps()
        progress(2)

        return choice
end

--------------------------------------------------------------------------------
-- @brief Configuration for STM32F1
--------------------------------------------------------------------------------
arch.stm32f1.configure = function()

        local function calc_number_of_steps(spi)
                local spi_en   = key_read("../stm32f1/spi_flags.h", "__SPI_SPI"..spi.."_ENABLE__")
                local no_of_cs = key_read("../stm32f1/spi_flags.h", "__SPI_SPI"..spi.."_NUMBER_OF_CS__")

                if spi_en == no then
                        progress(1, 1)
                else
                        progress(nil, 3 + no_of_cs)
                end
        end

        local function configure_dummy_byte()
                local choice = key_read("../stm32f1/spi_flags.h", "__SPI_DEFAULT_DUMMY_BYTE__")
                msg(progress(2, 6) .. "Enter value of dummy byte. This value is a default for all interfaces.")
                msg("Current choice is: " .. choice .. ".")
                choice = get_number("hex", 0x00, 0xFF)
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/spi_flags.h", "__SPI_DEFAULT_DUMMY_BYTE__", choice)
                end

                return choice
        end

        local function configure_clk_divider()
                local choice = key_read("../stm32f1/spi_flags.h", "__SPI_DEFAULT_CLK_DIV__")
                msg(progress(3, 6) .. "Default clock divider. This value is a default for all interfaces.")
                msg("Current choice is: /" .. choice:gsub("SPI_CLK_DIV_", "") .. ".")
                for i = 1, 8 do
                        add_item("SPI_CLK_DIV_"..math.pow(2, i), "/"..math.pow(2, i))
                end
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/spi_flags.h", "__SPI_DEFAULT_CLK_DIV__", choice)
                end

                return choice
        end

        local function configure_mode()
                local value = {}
                value.SPI_MODE_0 = "Mode 0: CPOL = 0; CPHA = 0 (SCK 0 at idle, capture on leading edge)"
                value.SPI_MODE_1 = "Mode 1: CPOL = 0; CPHA = 1 (SCK 0 at idle, capture on trailing edge)"
                value.SPI_MODE_2 = "Mode 2: CPOL = 1; CPHA = 0 (SCK 1 at idle, capture on leading edge)"
                value.SPI_MODE_3 = "Mode 3: CPOL = 1; CPHA = 1 (SCK 1 at idle, capture on trailing edge)"
                value.Unknown    = "Unknown"

                local choice = key_read("../stm32f1/spi_flags.h", "__SPI_DEFAULT_MODE__")
                if value[choice] == nil then choice = "Unknown" end

                msg(progress(4, 6) .. "Select default SPI mode. This value is a default for all interfaces.")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("SPI_MODE_0", value.SPI_MODE_0)
                add_item("SPI_MODE_1", value.SPI_MODE_1)
                add_item("SPI_MODE_2", value.SPI_MODE_2)
                add_item("SPI_MODE_3", value.SPI_MODE_3)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/spi_flags.h", "__SPI_DEFAULT_MODE__", choice)
                end

                return choice
        end

        local function configure_msb_first()
                local value = {}
                value.YES = "MSb first"
                value.NO  = "LSb first"

                local choice = key_read("../stm32f1/spi_flags.h", "__SPI_DEFAULT_MSB_FIRST__")
                msg(progress(5, 6).."Bit order configuration.")
                msg("Current selection is: "..value[filter_yes_no(choice)]..".")
                add_item(yes, value.YES)
                add_item(no, value.NO)
                choice = get_selection()
                if can_be_saved(choice) then
                        key_save("../stm32f1/spi_flags.h", "__SPI_DEFAULT_MSB_FIRST__", choice)
                end

                return choice
        end

        local function configure_spi_if_enable(spi)
                local key    = "__SPI_SPI"..spi.."_ENABLE__"
                local choice = key_read("../stm32f1/spi_flags.h", key)
                msg(progress().."Do you want to enable SPI"..spi.." interface?")
                msg("Current selection is: "..filter_yes_no(choice)..".")
                add_item(yes, "Yes")
                add_item(no, "No")
                choice = get_selection()
                if can_be_saved(choice) then
                        key_save("../stm32f1/spi_flags.h", key, choice)
                end

                calc_number_of_steps(spi)
                progress(2)

                return choice
        end

        local function configure_spi_irq_priority(spi)
                local key    = "__SPI_SPI"..spi.."_PRIORITY__"
                local choice = key_read("../stm32f1/spi_flags.h", key)

                msg(progress().."SPI"..spi.." IRQ priority configuration.")
                if choice == "CONFIG_USER_IRQ_PRIORITY" then
                        msg("Current value is: Default")
                else
                        msg("Current value is: Priority "..math.floor(tonumber(choice) / 16)..".")
                end

                add_item("0x0F", "Priority 0 (the highest)")
                add_item("0x1F", "Priority 1")
                add_item("0x2F", "Priority 2")
                add_item("0x3F", "Priority 3")
                add_item("0x4F", "Priority 4")
                add_item("0x5F", "Priority 5")
                add_item("0x6F", "Priority 6")
                add_item("0x7F", "Priority 7")
                add_item("0x8F", "Priority 8")
                add_item("0x9F", "Priority 9")
                add_item("0xAF", "Priority 10")
                add_item("0xBF", "Priority 11")
                add_item("0xCF", "Priority 12")
                add_item("0xDF", "Priority 13 (the lowest)")
                add_item("CONFIG_USER_IRQ_PRIORITY", "Default")
                choice = get_selection()
                if can_be_saved(choice) then
                        key_save("../stm32f1/spi_flags.h", key, choice)
                end

                return choice
        end

        local function configure_spi_number_of_cs(spi)
                local key    = "__SPI_SPI"..spi.."_NUMBER_OF_CS__"
                local choice = key_read("../stm32f1/spi_flags.h", key)
                msg(progress().."Select number of Chip Select lines used by SPI"..spi.." interface.")
                msg("Current selection is: "..choice..".")
                for i = 1, 8 do
                        add_item(i, i)
                end
                choice = get_selection()
                if can_be_saved(choice) then
                        key_save("../stm32f1/spi_flags.h", key, choice)
                end

                calc_number_of_steps(spi)

                return choice
        end

        local function configure_cs_pin_name(spi)
                local no_of_cs = key_read("../stm32f1/spi_flags.h", "__SPI_SPI"..spi.."_NUMBER_OF_CS__")
                local cs       = 0
                local pins     = gpio:get_pins()

                repeat
                        local key    = "__SPI_SPI"..spi.."_CS"..cs.."_PIN_NAME__"
                        local choice = key_read("../stm32f1/spi_flags.h", key)

                        msg(progress().."Select pin that is SPI"..spi..":CS"..cs..".")
                        msg("Current selection is: "..choice..".")
                        for i = 1, #pins do
                                add_item(pins[i], pins[i])
                        end
                        choice = get_selection()
                        if can_be_saved(choice) then
                                key_save("../stm32f1/spi_flags.h", key, choice)
                        end

                        if choice == back then
                                cs = cs - 1
                        elseif choice == next then
                                cs = cs + 1
                        end

                until cs <= -1 or cs >= tonumber(no_of_cs)

                if cs <= -1 then return back end
                if cs >=  8 then return next end
        end

        local function spi_select()
                local spi_no = db:get_mcu_module_data(cpu:get_name(), "SPI")[1]

                while true do
                        navigation("Home/Modules/SPI")
                        msg(progress(6, 6).."Choose SPI interface for configure.")
                        for i = 1, spi_no do
                                add_item(i, "SPI"..i)
                        end
                        local choice = get_selection()

                        if choice == back then
                                return back
                        end

                        if choice == next then
                                return next
                        end

                        navigation("Home/Modules/SPI/SPI"..choice)
                        progress(1)
                        calc_number_of_steps(choice)
                        ::spi_if_en:: if configure_spi_if_enable(choice) == back then goto continue end
                        if key_read("../stm32f1/spi_flags.h", "__SPI_SPI"..choice.."_ENABLE__") == yes then
                                ::spi_irq_prio:: if configure_spi_irq_priority(choice) == back then goto spi_if_en    end
                                ::spi_no_of_cs:: if configure_spi_number_of_cs(choice) == back then goto spi_irq_prio end
                                ::cs_pin_name::  if configure_cs_pin_name(choice)      == back then goto spi_no_of_cs end
                        end

                        ::continue::
                end
        end

        local function print_summary()
                local str
                local dummy_byte = key_read("../stm32f1/spi_flags.h", "__SPI_DEFAULT_DUMMY_BYTE__")
                local clk_div    = key_read("../stm32f1/spi_flags.h", "__SPI_DEFAULT_CLK_DIV__")
                local mode       = key_read("../stm32f1/spi_flags.h", "__SPI_DEFAULT_MODE__")
                local msb_first  = key_read("../stm32f1/spi_flags.h", "__SPI_DEFAULT_MSB_FIRST__")
                local spi_no     = db:get_mcu_module_data(cpu:get_name(), "SPI")[1]

                msg("Defaults:\n"..
                    "  Dummy byte: "..dummy_byte.."\n"..
                    "  Clock divider: "..clk_div:gsub("SPI_CLK_DIV_", "/").."\n"..
                    "  Mode: "..mode:gsub("SPI_MODE_", "").."\n"..
                    "  MSb first: "..filter_yes_no(msb_first).."\n")

                for spi = 1, spi_no do
                        local spi_en   = key_read("../stm32f1/spi_flags.h", "__SPI_SPI"..spi.."_ENABLE__")
                        local spi_prio = key_read("../stm32f1/spi_flags.h", "__SPI_SPI"..spi.."_PRIORITY__")
                        local no_of_cs = key_read("../stm32f1/spi_flags.h", "__SPI_SPI"..spi.."_NUMBER_OF_CS__")

                        str = "SPI"..spi..":\n"..
                              "  Enabled: ".. filter_yes_no(spi_en).."\n"

                        if spi_en == yes then
                                if spi_prio == "CONFIG_USER_IRQ_PRIORITY" then
                                        spi_prio =  "Default"
                                else
                                        spi_prio = math.floor(tonumber(choice) / 16)
                                end

                                str = str.."  IRQ priority: "..spi_prio.."\n"
                                str = str.."  Number of CS lines: "..no_of_cs.."\n"

                                for cs = 0, no_of_cs - 1 do
                                        local cs_name = key_read("../stm32f1/spi_flags.h", "__SPI_SPI"..spi.."_CS"..cs.."_PIN_NAME__")
                                        str = str.."    CS"..cs..": "..cs_name.."\n"
                                end
                        end

                        msg(str)
                end

                pause()
        end

        if key_read("../project/Makefile", "ENABLE_SPI") == yes then

                ::dummy::      if configure_dummy_byte()  == back then return back    end
                ::divider::    if configure_clk_divider() == back then goto dummy     end
                ::mode::       if configure_mode()        == back then goto divider   end
                ::msb_first::  if configure_msb_first()   == back then goto mode      end
                ::spi_select:: if spi_select()            == back then goto msb_first end

                print_summary()
        end

        return next
end

--------------------------------------------------------------------------------
-- METHODS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Function execute configuration
--------------------------------------------------------------------------------
function spi:configure()
        title("SPI module configuration for " .. cpu:get_name())
        navigation("Home/Modules/SPI")
        calculate_total_steps()

        ::enable::
        if ask_for_enable() == back then
                return back
        end

        if arch[cpu:get_arch()].configure() == back then
                goto enable
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
