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
#         local save = this:IsMACFormatCorrect()
#         this:LoadFile("network/network_flags.h", save)
#     end
# )
#
# this:SetEvent("PreSave", function() return this:IsMACFormatCorrect() end)
#
# this.IsMACFormatCorrect = function(this)
#     local MAC = this:GetFlagValue("__NETWORK_TCPIP_MAC_ADDR__"):upper()
#     local EL  = "[A-F0-9][A-F0-9]"
#
#     if MAC:match("^\""..EL..":"..EL..":"..EL..":"..EL..":"..EL..":"..EL.."\"$") then
#         this:SetFlagValue("LabelMACInfo", "")
#
#         MAC0, MAC1, MAC2, MAC3, MAC4, MAC5 = MAC:match("^\"("..EL.."):"..
#                                                           "("..EL.."):"..
#                                                           "("..EL.."):"..
#                                                           "("..EL.."):"..
#                                                           "("..EL.."):"..
#                                                           "("..EL..")\"$")
#
#         this:SetFlagValue("__NETWORK_TCPIP_MAC_ADDR0__", "0x"..MAC0)
#         this:SetFlagValue("__NETWORK_TCPIP_MAC_ADDR1__", "0x"..MAC1)
#         this:SetFlagValue("__NETWORK_TCPIP_MAC_ADDR2__", "0x"..MAC2)
#         this:SetFlagValue("__NETWORK_TCPIP_MAC_ADDR3__", "0x"..MAC3)
#         this:SetFlagValue("__NETWORK_TCPIP_MAC_ADDR4__", "0x"..MAC4)
#         this:SetFlagValue("__NETWORK_TCPIP_MAC_ADDR5__", "0x"..MAC5)
#         return true
#     else
#         this:SetFlagValue("LabelMACInfo", "Invalid MAC format!")
#         return false
#     end
# end
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
# this:AddWidget("Editline", true, "MAC address")
# this:SetToolTip("MAC format: HH:HH:HH:HH:HH:HH")
# this:AddExtraWidget("Label", "LabelMACInfo", "")
#--*/
#define __NETWORK_TCPIP_MAC_ADDR__ "DE:AD:BE:EF:00:00"
#/*--
# this:AddWidget("Value")
#--*/
#define __NETWORK_TCPIP_MAC_ADDR0__ 0xDE
#/*--
# this:AddWidget("Value")
#--*/
#define __NETWORK_TCPIP_MAC_ADDR1__ 0xAD
#/*--
# this:AddWidget("Value")
#--*/
#define __NETWORK_TCPIP_MAC_ADDR2__ 0xBE
#/*--
# this:AddWidget("Value")
#--*/
#define __NETWORK_TCPIP_MAC_ADDR3__ 0xEF
#/*--
# this:AddWidget("Value")
#--*/
#define __NETWORK_TCPIP_MAC_ADDR4__ 0x00
#/*--
# this:AddWidget("Value")
#--*/
#define __NETWORK_TCPIP_MAC_ADDR5__ 0x00


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
