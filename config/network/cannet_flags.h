#/*=============================================================================
# @file    cannet_flags.h
#
# @author  Daniel Zorychta
#
# @brief   This file contains configuration of CANNET stack.
#          Hybrid file: included both by Make and CC.
#
# @note    Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>
#
#          This program is free software; you can redistribute it and/or modify
#          it under the terms of the GNU General Public License as published by
#          the Free Software Foundation and modified by the dnx RTOS exception.
#
#          NOTE: The modification  to the GPL is  included to allow you to
#                distribute a combined work that includes dnx RTOS without
#                being obliged to provide the source  code for proprietary
#                components outside of the dnx RTOS.
#
#          The dnx RTOS  is  distributed  in the hope  that  it will be useful,
#          but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
#          MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
#          GNU General Public License for more details.
#
#          Full license text is available on the following file: doc/license.txt.
#
#
#=============================================================================*/

#/*
#* NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as suffix
#*       and prefix) are exported to the single configuration file
#*       (by using Configtool) when entire project configuration is exported.
#*       All other flag definitions and statements are ignored.
#*/

#ifndef _CANNET_FLAGS_H_
#define _CANNET_FLAGS_H_

#/*--
# this:SetLayout("TitledGridBack", 2, "Home > Network > CANNET Stack",
#     function() -- back button
#         this:LoadFile("network/network_flags.h")
#     end
# )
#++*/

#/*--
# this:AddWidget("Spinbox", 128, 65536, "Max socket buffer length [B]")
#--*/
#define __NETWORK_CANNET_MAX_SOCKET_BUFFER_LENGTH__ 256

#/*--
# this:AddWidget("Spinbox", 4, 256, "Maximum number of sockets")
#--*/
#define __NETWORK_CANNET_MAX_SOCKETS__ 16

#/*--
# this:AddWidget("Spinbox", 3, 16, "Packet send repetitions")
#--*/
#define __NETWORK_CANNET_SEND_REPETITIONS__ 3

#/*--
# this:AddWidget("Spinbox", 5, 1000, "Packet repetition interval min [ms]")
#--*/
#define __NETWORK_CANNET_SEND_REPETITIONS_INTERVAL_MIN__ 3

#/*--
# this:AddWidget("Spinbox", 5, 1000, "Packet repetition interval max [ms]")
#--*/
#define __NETWORK_CANNET_SEND_REPETITIONS_INTERVAL_MAX__ 250

#/*--
# this:AddWidget("Combobox", "Debug messages")
# this:AddItem("Disable", "_NO_")
# this:AddItem("Enable", "_YES_")
#--*/
#define __NETWORK_CANNET_DEBUG_ON__ _NO_

#endif /* _CANNET_FLAGS_H_ */
#/*=============================================================================
#  End of file
#=============================================================================*/