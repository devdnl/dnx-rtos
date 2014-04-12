--[[============================================================================
@file    eth.lua

@author  Daniel Zorychta

@brief   Ethernet configuration wizard.

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
eth = {}

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

        if key_read("../project/Makefile", "ENABLE_ETH") == yes then
                if arch == "stm32f1" then
                        progress(1, 8)
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
        local choice = key_read("../project/flags.h", "__ENABLE_ETH__")
        msg(progress().."Do you want to enable ETH module?")
        msg("Current selection is: "..filter_yes_no(choice)..".")
        add_item(yes, "Yes")
        add_item(no, "No")
        choice = get_selection()
        if can_be_saved(choice) then
                key_save("../project/flags.h", "__ENABLE_ETH__", choice)
                key_save("../project/Makefile", "ENABLE_ETH", choice)
        end

        calculate_total_steps()
        progress(2)

        return choice
end

--------------------------------------------------------------------------------
-- @brief ETH configuration for STM32F1
--------------------------------------------------------------------------------
arch.stm32f1.configure = function()
        local function configure_priority()
                local choice = key_read("../stm32f1/eth_flags.h", "__ETH_IRQ_PRIORITY__")

                msg(progress().."Ethernet IRQ priority configuration.")
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
                        key_save("../stm32f1/eth_flags.h", "__ETH_IRQ_PRIORITY__", choice)
                end

                return choice
        end

        local function configure_hw_checksum()
                local choice = key_read("../stm32f1/eth_flags.h", "__ETH_CHECKSUM_BY_HARDWARE__")
                msg(progress() .. "Do you want to enable hardware checksum calculation?")
                msg("Current choice is: " .. filter_yes_no(choice) .. ".")
                add_item(yes, "YES")
                add_item(no, "NO")
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/eth_flags.h", "__ETH_CHECKSUM_BY_HARDWARE__", choice)
                end

                return choice
        end

        local function configure_speed()
                local value = {}
                value.ETH_Speed_10M  = "10 Mbps"
                value.ETH_Speed_100M = "100 Mbps"

                local choice = key_read("../stm32f1/eth_flags.h", "__ETH_SPEED__")
                msg(progress() .. "Ethernet speed configuration.")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("ETH_Speed_10M",  value.ETH_Speed_10M )
                add_item("ETH_Speed_100M", value.ETH_Speed_100M)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/eth_flags.h", "__ETH_SPEED__", choice)
                end

                return choice
        end

        local function configure_PHY_address()
                local choice = key_read("../stm32f1/eth_flags.h", "__ETH_PHY_ADDRESS__")
                msg(progress() .. "PHY address configuration.")
                msg("Current choice is: " .. choice .. ".")
                choice = get_number("hex", 0x00, 0xFF)
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/eth_flags.h", "__ETH_PHY_ADDRESS__", choice)
                end

                return choice
        end

        local function configure_reset_delay()
                local choice = key_read("../stm32f1/eth_flags.h", "__ETH_PHY_RESET_DELAY__")
                msg(progress() .. "PHY reset delay configuration.")
                msg("Current choice is: " .. choice .. " ms.")
                choice = get_number("dec", 0, 500)
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/eth_flags.h", "__ETH_PHY_RESET_DELAY__", choice)
                end

                return choice
        end

        local function configure_config_delay()
                local choice = key_read("../stm32f1/eth_flags.h", "__ETH_PHY_CONFIG_DELAY__")
                msg(progress() .. "PHY setup delay configuration.")
                msg("Current choice is: " .. choice .. " ms.")
                choice = get_number("dec", 0, 500)
                if (can_be_saved(choice)) then
                        key_save("../stm32f1/eth_flags.h", "__ETH_PHY_CONFIG_DELAY__", choice)
                end

                return choice
        end

        local function configure_PHY_device()
                local value = {}
                value.DP83848 = "DP83848"
                value.LAN8700 = "LAN8700"
                value.Unknown = "Unknown"

                local choice = key_read("../stm32f1/eth_flags.h", "__ETH_DEVICE__")
                if value[choice] == nil then choice = "Unknown" end

                msg(progress() .. "Ethernet speed configuration.")
                msg("Current choice is: " .. value[choice] .. ".")
                add_item("DP83848", value.DP83848)
                add_item("LAN8700", value.LAN8700)
                choice = get_selection()
                if (can_be_saved(choice)) then
                        local speed_status_bm    = 0
                        local duplex_status_bm   = 0
                        local status_reg_address = 0
                        if choice == "DP83848" then
                                status_reg_address = 16
                                speed_status_bm    = "0x0002"
                                duplex_status_bm   = "0x0004"
                        elseif choice == "LAN8700" then
                                status_reg_address = 31
                                speed_status_bm    = "0x0004"
                                duplex_status_bm   = "0x0010"
                        end

                        key_save("../stm32f1/eth_flags.h", "__ETH_DEVICE__", choice)
                        key_save("../stm32f1/eth_flags.h", "__ETH_PHY_SR__", status_reg_address)
                        key_save("../stm32f1/eth_flags.h", "__ETH_PHY_SPEED_STATUS_BM__", speed_status_bm)
                        key_save("../stm32f1/eth_flags.h", "__ETH_PHY_DUPLEX_STATUS_BM__", duplex_status_bm)
                end

                return choice
        end

        local function print_summary()
                msg("ETH module configuration summary:")

                local priority = key_read("../stm32f1/eth_flags.h", "__ETH_IRQ_PRIORITY__")
                local checksum = key_read("../stm32f1/eth_flags.h", "__ETH_CHECKSUM_BY_HARDWARE__")
                local ethspeed = key_read("../stm32f1/eth_flags.h", "__ETH_SPEED__")
                local phyaddr  = key_read("../stm32f1/eth_flags.h", "__ETH_PHY_ADDRESS__")
                local resetdly = key_read("../stm32f1/eth_flags.h", "__ETH_PHY_RESET_DELAY__")
                local setupdly = key_read("../stm32f1/eth_flags.h", "__ETH_PHY_CONFIG_DELAY__")
                local ethdev   = key_read("../stm32f1/eth_flags.h", "__ETH_DEVICE__")

                if priority == "CONFIG_USER_IRQ_PRIORITY" then
                        priority = "Default"
                else
                        priority = math.floor(tonumber(priority) / 16)
                end

                msg("Priority: " .. priority .. "\n"..
                    "Hardware checksum calculation: " .. filter_yes_no(checksum) .. "\n"..
                    "Ethernet speed: " .. ethspeed:gsub("ETH_Speed_", "") .. "bps\n"..
                    "PHY address: " .. phyaddr .. "\n"..
                    "PHY reset delay: " .. resetdly .. "ms\n"..
                    "PHY setup delay: " .. setupdly .. "ms\n"..
                    "PHY device: " .. ethdev .. ".")

                pause()
        end

        if key_read("../project/Makefile", "ENABLE_ETH") == yes then

                ::set_priority:: if configure_priority()     == back then return back       end
                ::checksum::     if configure_hw_checksum()  == back then goto set_priority end
                ::set_speed::    if configure_speed()        == back then goto checksum     end
                ::phy_address::  if configure_PHY_address()  == back then goto set_speed    end
                ::reset_delay::  if configure_reset_delay()  == back then goto phy_address  end
                ::config_delay:: if configure_config_delay() == back then goto reset_delay  end
                ::phy_device::   if configure_PHY_device()   == back then goto config_delay end

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
function eth:configure()
        title("ETH module configuration for " .. cpu:get_name())
        navigation("Home/Modules/ETH")
        calculate_total_steps()

        ::eth_enable::
        if ask_for_enable() == back then
                return back
        end

        if arch[cpu:get_arch()].configure() == back then
                goto eth_enable
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
