#/*=============================================================================
# @file    tcpip_flags.h
#
# @author  Daniel Zorychta
#
# @brief   This file contains configuration of TCP/IP stacks.
#          Hybrid file: included both by Make and CC.
#
# @note    Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>
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

#ifndef _TCPIP_FLAGS_H_
#define _TCPIP_FLAGS_H_



#/* Existing stacks - CC definitions */
#define __STACK_LWIP__ 0xf986f02e
#/* Existing stacks - Make definitions
__STACK_LWIP__=0xf986f02e
#*/



#/*--
# this:SetLayout("TitledGridBack", 3, "Home > Network > TCP/IP Stack",
#     function() -- back button
#         this:LoadFile("network/network_flags.h")
#     end
# )
#++*/



#/*--
# this:AddExtraWidget("Label", "LabelGeneral", "General", -1, "bold")
# this:AddExtraWidget("Label", "LabelVoid0", "                                                  ")
# this:AddExtraWidget("Label", "LabelVoid1", "                                                  ")
#
# this:AddWidget("Combobox", "Stack selection")
# this:AddItem("lwIP", "__STACK_LWIP__")
# this:SetEvent("clicked", function() this:Reload() end)
#
# if this:GetFlagValue("__NETWORK_TCPIP_STACK__") == "__STACK_LWIP__" then
#       this:AddExtraWidget("Hyperlink", "lwIPConfigure", "Configure")
#       this:SetEvent("clicked", "lwIPConfigure", function() this:LoadFile("network/lwip_flags.h") end)
# else
#       this:AddExtraWidget("Void", "VoidConfigure")
# end
#--*/
#define __NETWORK_TCPIP_STACK__ __STACK_LWIP__
#/*
__NETWORK_TCPIP_STACK__=__STACK_LWIP__
#*/



#/*--
# this:AddExtraWidget("Label", "LabelDev", "\nDevices", -1, "bold")
# this:AddExtraWidget("Void", "VoidDev0")
# this:AddExtraWidget("Void", "VoidDev1")
#
# this:AddWidget("Editline", true, "Interface device")
# this:AddExtraWidget("Void", "VoidEth0", "")
# this:AddExtraWidget("Void", "VoidEth1", "")
#--*/
#define __NETWORK_TCPIP_DEVICE_PATH__ "/dev/ethmac"



#// Include configuration of selected task.
#if __NETWORK_TCPIP_STACK__ == __STACK_LWIP__
#include "lwip_flags.h"
#endif

#endif /* _TCPIP_FLAGS_H_ */
#/*=============================================================================
#  End of file
#=============================================================================*/
