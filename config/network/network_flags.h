#/*=============================================================================
# @file    network_flags.h
#
# @author  Daniel Zorychta
#
# @brief   This file contains list of existing network stacks.
#          Hybrid file: included both by Make and CC.
#
# @note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>
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

#ifndef _NETWORK_FLAGS_H_
#define _NETWORK_FLAGS_H_

#/*--
# this:SetLayout("TitledGridBack", 2, "Home > Network",
#                function() this:LoadFile("config.h", true) end)
#++*/

#/*--
# this:AddExtraWidget("Label", "LabelStacks", "\nStacks", -1, "bold")
# this:AddExtraWidget("Void", "VoidStacks")
#++*/

#/*--
# this:AddWidget("Checkbox", "Enable TCP/IP")
# this:AddExtraWidget("Hyperlink", "TCPIPConfigure", "Configure")
# this:SetEvent("clicked", "TCPIPConfigure", function() this:LoadFile("network/tcpip_flags.h") end)
#--*/
#define __ENABLE_TCPIP_STACK__ _NO_
#/*
__ENABLE_TCPIP_STACK__=_NO_
#*/

#/*--
# this:AddWidget("Checkbox", "Enable SIPC")
# this:AddExtraWidget("Hyperlink", "SIPCConfigure", "Configure")
# this:SetEvent("clicked", "SIPCConfigure", function() this:LoadFile("network/sipc_flags.h") end)
#--*/
#define __ENABLE_SIPC_STACK__ _NO_
#/*
__ENABLE_SIPC_STACK__=_NO_
#*/

#/*--
# this:AddWidget("Checkbox", "Enable CANNET")
# this:AddExtraWidget("Hyperlink", "CANNETConfigure", "Configure")
# this:SetEvent("clicked", "CANNETConfigure", function() this:LoadFile("network/cannet_flags.h") end)
#--*/
#define __ENABLE_CANNET_STACK__ _NO_
#/*
__ENABLE_CANNET_STACK__=_NO_
#*/

#if __ENABLE_TCPIP_STACK__ == _YES_
#include "tcpip_flags.h"
#/*
#include ./config/network/tcpip_flags.h
#*/
#endif

#if __ENABLE_SIPC_STACK__ == _YES_
#include "sipc_flags.h"
#/*
include ./config/network/sipc_flags.h
#*/
#endif

#if __ENABLE_CANNET_STACK__ == _YES_
#include "cannet_flags.h"
#/*
include ./config/network/cannet_flags.h
#*/
#endif



#/* MASTER NETWORK SUBSYSTEM ENABLE */
#define _ENABLE_NETWORK_ (__ENABLE_TCPIP_STACK__ || __ENABLE_SIPC_STACK__ || __ENABLE_CANNET_STACK__)
#/*
ifeq ($(__ENABLE_TCPIP_STACK__), _YES_)
_ENABLE_NETWORK_=_YES_
else ifeq ($(__ENABLE_SIPC_STACK__), _YES_)
_ENABLE_NETWORK_=_YES_
else ifeq ($(__ENABLE_CANNET_STACK__), _YES_)
_ENABLE_NETWORK_=_YES_
endif
#*/

#endif /* _NETWORK_FLAGS_H_ */
#/*=============================================================================
#  End of file
#=============================================================================*/
