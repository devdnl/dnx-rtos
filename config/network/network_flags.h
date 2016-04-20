#/*=============================================================================
# @file    network_flags.h
#
# @author  Daniel Zorychta
#
# @brief   This file contains list of existing modules
#          Hybrid file: included both by Make and CC.
#
# @note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>
#
#          This program is free software; you can redistribute it and/or modify
#          it under the terms of the GNU General Public License as published by
#          the  Free Software  Foundation;  either version 2 of the License, or
#          any later version.
#
#          This  program  is  distributed  in the hope that  it will be useful,
#          but  WITHOUT  ANY  WARRANTY;  without  even  the implied warranty of
#          MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
#          GNU General Public License for more details.
#
#          You  should  have received a copy  of the GNU General Public License
#          along  with  this  program;  if not,  write  to  the  Free  Software
#          Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
# this:SetLayout("TitledGridBack", 3, "Home > Network",
#     function() -- back button
#         local save = this:CheckFormatCorrections()
#         this:LoadFile("config.h", save)
#     end
# )
#
# this:SetEvent("PreSave",
#     function()
#         return this:CheckFormatCorrections()
#     end
# )
#
# this.CheckFormatCorrections = function(this)
#         local correct = true
#         if this:IsMACFormatCorrect("__NETWORK_TCPIP_MAC_ADDR__", "LabelMACInfo") == false then correct = false end
#         if this:IsIPFormatCorrect("__NETWORK_TCPIP_IP_ADDR__", "LabelIPInfo") == false then correct = false end
#         if this:IsIPFormatCorrect("__NETWORK_TCPIP_IP_MASK__", "LabelIPMaskInfo") == false then correct = false end
#         if this:IsIPFormatCorrect("__NETWORK_TCPIP_IP_GATEWAY__", "LabelIPGwInfo") == false then correct = false end
#         return correct
# end
#
# this.IsMACFormatCorrect = function(this, flag, flagInfo)
#     local MAC = this:GetFlagValue(flag):upper()
#     local EL  = "[A-F0-9][A-F0-9]"
#     if MAC:match("^\""..EL..":"..EL..":"..EL..":"..EL..":"..EL..":"..EL.."\"$") then
#         this:SetFlagValue("LabelMACInfo", "")
#         return true
#     else
#         this:SetFlagValue(flagInfo, "Invalid format (HH:HH:HH:HH:HH:HH)")
#         return false
#     end
# end
#
# this.IsIPFormatCorrect = function(this, flag, flagInfo)
#     local IP = this:GetFlagValue(flag)
#     if IP:match('^"%d+%.%d+%.%d+%.%d+"$') then
#         local IPa, IPb, IPc, IPd = IP:match('^"(%d+)%.(%d+)%.(%d+)%.(%d+)"$')
#         if  tonumber(IPa) <= 255 and tonumber(IPb) <= 255
#         and tonumber(IPc) <= 255 and tonumber(IPd) <= 255 then
#             this:SetFlagValue(flagInfo, "")
#             return true
#         end
#     end
#
#     this:SetFlagValue(flagInfo, "Invalid format (DDD.DDD.DDD.DDD)")
#     return false
# end
#++*/


#/*--
# this:AddExtraWidget("Label", "LabelGeneral", "General", -1, "bold")
# this:AddExtraWidget("Void", "VoidGeneral0")
# this:AddExtraWidget("Void", "VoidGeneral1")
#
# this:AddWidget("Combobox", "Enable network")
# this:AddItem("Disable", "_NO_")
# this:AddItem("Enable\t\t\t\t", "_YES_")
# this:AddExtraWidget("Void", "VoidEnable")
#--*/
#define __ENABLE_NETWORK__ _YES_
#/*
__ENABLE_NETWORK__=_YES_
#*/

#/* existing stacks */
#define STACK_LWIP 0xf986f02e
#/*--
# --this:AddWidget("Combobox", "TCP/IP stack")
# --this:AddItem("LwIP", "STACK_LWIP")
# --this:AddExtraWidget("Void", "VoidStack")
#--*/
#define __NETWORK_STACK__ STACK_LWIP


#/*--
# this:AddExtraWidget("Label", "LabelTCPIP", "\nTCP/IP", -1, "bold")
# this:AddExtraWidget("Void", "VoidTCPIP0")
# this:AddExtraWidget("Void", "VoidTCPIP1")
#
# this:AddExtraWidget("Label", "LabelLwIP", "LwIP stack")
# this:AddExtraWidget("Hyperlink", "HLLwIP", "Configure")
# this:SetEvent("clicked", "HLLwIP", function() this:LoadFile("network/lwip_flags.h") end)
# this:AddExtraWidget("Void", "VoidLwIPCfg")
#++*/

#/*--
# this:AddWidget("Editline", true, "MAC address")
# this:AddExtraWidget("Label", "LabelMACInfo", "")
#--*/
#define __NETWORK_TCPIP_MAC_ADDR__ "50:E5:49:37:B5:BD"

#/*--
# this:AddWidget("Editline", true, "IP address")
# this:AddExtraWidget("Label", "LabelIPInfo", "")
#--*/
#define __NETWORK_TCPIP_IP_ADDR__ "192.168.0.100"

#/*--
# this:AddWidget("Editline", true, "IP mask")
# this:AddExtraWidget("Label", "LabelIPMaskInfo", "")
#--*/
#define __NETWORK_TCPIP_IP_MASK__ "255.255.255.0"

#/*--
# this:AddWidget("Editline", true, "IP gateway")
# this:AddExtraWidget("Label", "LabelIPGwInfo", "")
#--*/
#define __NETWORK_TCPIP_IP_GATEWAY__ "192.168.0.1"


#/*--
# this:AddExtraWidget("Label", "LabelEthDev", "\nDevices", -1, "bold")
# this:AddExtraWidget("Void", "VoidEthDev0")
# this:AddExtraWidget("Void", "VoidEthDev1")
#
# this:AddWidget("Editline", true, "Ethernet device path")
# this:AddExtraWidget("Void", "VoidEth", "")
#--*/
#define __NETWORK_ETHIF_FILE__ "/dev/ethmac"

#if __NETWORK_STACK__ == STACK_LWIP
#include "lwip_flags.h"
#endif

#endif /* _NETWORK_FLAGS_H_ */
#/*=============================================================================
#  End of file
#=============================================================================*/
