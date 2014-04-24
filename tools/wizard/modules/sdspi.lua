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
require "db"
require "gpio"

--------------------------------------------------------------------------------
-- OBJECTS
--------------------------------------------------------------------------------
sdspi = {}

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
        local arch   = db:get_arch()
        local family = db:get_mcu_family(db:get_cpu_name())

        if key_read(db.path.project.mk, "ENABLE_SDSPI") == yes then
                if arch == "stm32f1" then
                        progress(1, 7)
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
        local choice = key_read(db.path.project.flags, "__ENABLE_SDSPI__")
        msg(progress().."Do you want to enable SDSPI module?")
        msg("Current selection is: "..filter_yes_no(choice)..".")
        add_item(yes, "Yes")
        add_item(no, "No")
        choice = get_selection()
        if can_be_saved(choice) then
                key_save(db.path.project.flags, "__ENABLE_SDSPI__", choice)
                key_save(db.path.project.mk, "ENABLE_SDSPI", choice)
        end

        calculate_total_steps()
        progress(2)

        return choice
end

--------------------------------------------------------------------------------
-- @brief PLL configuration for STM32F1
--------------------------------------------------------------------------------
arch.stm32f1.configure = function()

        local function configure_card_timeout()
                local choice = key_read(db.path.stm32f1.sdspi.flags, "__SDSPI_TIMEOUT__")
                msg(progress() .. "Card timeout configuration.")
                msg("Current choice is: " .. choice .. " ms.")
                choice = get_number("dec", 0, 1000)
                if (can_be_saved(choice)) then
                        key_save(db.path.stm32f1.sdspi.flags, "__SDSPI_TIMEOUT__", choice)
                end

                return choice
        end

        local function configure_SPI_port()
                local choice = key_read(db.path.stm32f1.sdspi.flags, "__SDSPI_SPI_PORT__")
                msg(progress() .. "Select specified SPI interface to handle by SDSPI module. Make sure that interface is not used by other module.")
                msg("Current choice is: SPI" .. choice .. ".")
                for i = 1, db:get_mcu_module_data(db:get_cpu_name(), "SPI")[1] do
                        add_item(i, "SPI"..i)
                end
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save(db.path.stm32f1.sdspi.flags, "__SDSPI_SPI_PORT__", choice)
                end

                return choice
        end

        local function configure_SPI_clock_prescaler()
                local choice = key_read(db.path.stm32f1.sdspi.flags, "__SDSPI_SPI_CLK_DIV__")
                msg(progress() .. "SPI clock prescaler configuration.")
                msg("Current choice is: /" .. choice .. ".")
                for i = 1, 8 do
                        add_item(math.pow(2, i), "/"..math.pow(2, i))
                end
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save(db.path.stm32f1.sdspi.flags, "__SDSPI_SPI_CLK_DIV__", choice)
                end

                return choice
        end

        local function configure_card_CS_pin()
                local choice = key_read(db.path.stm32f1.sdspi.flags, "__SDSPI_SD_CS_PIN__")
                msg(progress().."Card chip select pin name configuration.")
                msg("Current choice is: "..choice)
                local pins = gpio:get_pins()
                for i = 1, #pins do
                        add_item(pins[i], pins[i])
                end
                choice = get_selection()
                if can_be_saved(choice) then
                        key_save(db.path.stm32f1.sdspi.flags, "__SDSPI_SD_CS_PIN__", choice)
                end

                return choice
        end

        local function configure_enable_DMA()
                local choice = key_read(db.path.stm32f1.sdspi.flags, "__SDSPI_ENABLE_DMA__")
                msg(progress().."Do you want to enable DMA for SDSPI module? Make sure that DMA channels are free.")
                msg("Current selection is: "..filter_yes_no(choice)..".")
                add_item(yes, "Yes")
                add_item(no, "No")
                choice = get_selection()
                if can_be_saved(choice) then
                        key_save(db.path.stm32f1.sdspi.flags, "__SDSPI_ENABLE_DMA__", choice)
                end

                return choice
        end

        local function configure_DMA_IRQ_priority()
                local choice = key_read(db.path.stm32f1.sdspi.flags, "__SDSPI_DMA_IRQ_PRIORITY__")

                msg(progress().."SDSPI DMA IRQ priority configuration.")
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
                        key_save(db.path.stm32f1.sdspi.flags, "__SDSPI_DMA_IRQ_PRIORITY__", choice)
                end

                return choice
        end

        local function print_summary()
                local timeout  = key_read(db.path.stm32f1.sdspi.flags, "__SDSPI_TIMEOUT__")
                local spi_port = key_read(db.path.stm32f1.sdspi.flags, "__SDSPI_SPI_PORT__")
                local spi_div  = key_read(db.path.stm32f1.sdspi.flags, "__SDSPI_SPI_CLK_DIV__")
                local cs_pin   = key_read(db.path.stm32f1.sdspi.flags, "__SDSPI_SD_CS_PIN__")
                local dma_en   = key_read(db.path.stm32f1.sdspi.flags, "__SDSPI_ENABLE_DMA__")
                local dma_prio = key_read(db.path.stm32f1.sdspi.flags, "__SDSPI_DMA_IRQ_PRIORITY__")

                if dma_prio == "CONFIG_USER_IRQ_PRIORITY" then
                        dma_prio = "Default"
                else
                        dma_prio = math.floor(tonumber(dma_prio) / 16)
                end

                msg("SDSPI module configuration summary:")
                msg("SPI port: SPI"..spi_port.."\n"..
                    "SPI clock divider: "..spi_div.."\n"..
                    "Card CS pin: "..cs_pin.."\n"..
                    "Card timeout: "..timeout.." ms\n"..
                    "DMA enabled: "..filter_yes_no(dma_en).."\n"..
                    "DMA IRQ priority: "..dma_prio)

                    pause()
        end

        if key_read(db.path.project.mk, "ENABLE_SDSPI") == yes then

                local pages = {configure_SPI_port,
                               configure_SPI_clock_prescaler,
                               configure_card_CS_pin,
                               configure_card_timeout,
                               configure_enable_DMA,
                               configure_DMA_IRQ_priority,
                               print_summary}

                return show_pages(pages)
        end

        return next
end

--------------------------------------------------------------------------------
-- METHODS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Function execute configuration
--------------------------------------------------------------------------------
function sdspi:configure()
        title("SDSPI module configuration for " .. db:get_cpu_name())
        navigation("Home/Modules/SDSPI")
        calculate_total_steps()

        ::enable::
        if ask_for_enable() == back then
                return back
        end

        if key_read(db.path.project.mk, "ENABLE_SDSPI") == yes then
                if arch[db:get_arch()].configure() == back then
                        goto enable
                end
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
