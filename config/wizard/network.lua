--[[============================================================================
@file    network.lua

@author  Daniel Zorychta

@brief   Network configuration file.

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

--------------------------------------------------------------------------------
-- FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Calculate total steps of this configuration
--------------------------------------------------------------------------------
local function calculate_total_steps()
        if (key_read("../project/flags.h", "__NETWORK_ENABLE__") == yes) then
                set_total_steps(3)
        else
                set_total_steps(1)
        end
end

--------------------------------------------------------------------------------
-- @brief Function validate MAC address
--------------------------------------------------------------------------------
local function is_MAC_valid(mac_str)
        if (mac_str:match("\"([a-fA-F0-9][a-fA-F0-9]:[a-fA-F0-9][a-fA-F0-9]:[a-fA-F0-9][a-fA-F0-9]:"..
                             "[a-fA-F0-9][a-fA-F0-9]:[a-fA-F0-9][a-fA-F0-9]:[a-fA-F0-9][a-fA-F0-9])\"") ) then

                return true
        else
                return false
        end
end

--------------------------------------------------------------------------------
-- @brief Function validate MAC address
--------------------------------------------------------------------------------
local function get_MAC_elements(mac_str)
        return mac_str:match("\"([a-fA-F0-9][a-fA-F0-9]):([a-fA-F0-9][a-fA-F0-9]):([a-fA-F0-9][a-fA-F0-9]):"..
                               "([a-fA-F0-9][a-fA-F0-9]):([a-fA-F0-9][a-fA-F0-9]):([a-fA-F0-9][a-fA-F0-9])\"")
end

--------------------------------------------------------------------------------
-- @brief Configure net enable
--------------------------------------------------------------------------------
local function configure_net_enable()
        local choice = key_read("../project/flags.h", "__NETWORK_ENABLE__")
        msg(progress() .. "Do you want to enable network?")
        msg("Current choice is: " .. string.gsub(choice, "_", "") .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save("../project/flags.h", "__NETWORK_ENABLE__", choice)
                key_save("../project/Makefile", "ENABLE_NETWORK", choice)
        end

        calculate_total_steps()

        return choice
end

--------------------------------------------------------------------------------
-- @brief Configure MAC address
--------------------------------------------------------------------------------
local function configure_mac()
        local MAC = {}
        MAC[0] = key_read("../project/flags.h", "__NETWORK_MAC_ADDR_0__")
        MAC[1] = key_read("../project/flags.h", "__NETWORK_MAC_ADDR_1__")
        MAC[2] = key_read("../project/flags.h", "__NETWORK_MAC_ADDR_2__")
        MAC[3] = key_read("../project/flags.h", "__NETWORK_MAC_ADDR_3__")
        MAC[4] = key_read("../project/flags.h", "__NETWORK_MAC_ADDR_4__")
        MAC[5] = key_read("../project/flags.h", "__NETWORK_MAC_ADDR_5__")

        for i = 0, 5 do
                MAC[i] = string.gsub(MAC[i], "0x", "")
        end

        local first_time = true
        local correct    = false

        while not correct do
                msg(progress() .. "MAC address configuration. Enter MAC address in format: xx:xx:xx:xx:xx:xx.")
                msg("Current MAC address is: " .. MAC[0] .. ":" .. MAC[1] .. ":" ..  MAC[2] .. ":" ..  MAC[3] .. ":" ..  MAC[4] .. ":" ..  MAC[5] .. ".")
                if not first_time then msg("You have entered an invalid address, try again.") end
                str = get_string()
                if (can_be_saved(str)) then
                        if is_MAC_valid(str) then
                                local a, b, c, d, e, f = get_MAC_elements(str)

                                key_save("../project/flags.h", "__NETWORK_MAC_ADDR_0__", "0x"..a:upper())
                                key_save("../project/flags.h", "__NETWORK_MAC_ADDR_1__", "0x"..b:upper())
                                key_save("../project/flags.h", "__NETWORK_MAC_ADDR_2__", "0x"..c:upper())
                                key_save("../project/flags.h", "__NETWORK_MAC_ADDR_3__", "0x"..d:upper())
                                key_save("../project/flags.h", "__NETWORK_MAC_ADDR_4__", "0x"..e:upper())
                                key_save("../project/flags.h", "__NETWORK_MAC_ADDR_5__", "0x"..f:upper())

                                correct = true
                        else
                                modify_current_step(-1)
                        end
                else
                        correct = true
                end

                first_time = false
        end

        return str
end

--------------------------------------------------------------------------------
-- @brief Configure Ethernet file
--------------------------------------------------------------------------------
local function configure_eth_file()
        local str = key_read("../project/flags.h", "__NETWORK_ETHIF_FILE__")
        msg(progress() .. "Enter Ethernet file path.")
        msg("Current file is: " .. str .. ".")
        str = get_string()
        if (can_be_saved(str)) then
                key_save("../project/flags.h", "__NETWORK_ETHIF_FILE__", str)
        end

        return str
end

--------------------------------------------------------------------------------
-- @brief Function execute configuration
--------------------------------------------------------------------------------
function net_configure()
        calculate_total_steps()

        title("Network Configuration")

        ::_01_::
        if configure_net_enable() == back then
                return back
        end

        if (key_read("../project/flags.h", "__NETWORK_ENABLE__") == no) then
                msg("To configure more network options, enable network.")
                pause()
                return next
        end

        ::_02_::
        if configure_mac() == back then
                goto _01_
        end

        ::_03_::
        if configure_eth_file() == back then
                goto _02_
        end

        return next
end

--------------------------------------------------------------------------------
-- Enable configuration if master wizard is not defined
--------------------------------------------------------------------------------
if (master ~= true) then
        while net_configure() == back do
        end
        configuration_finished()
end

--------------------------------------------------------------------------------
-- END OF FILE
--------------------------------------------------------------------------------
