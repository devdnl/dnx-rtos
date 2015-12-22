#/*=============================================================================
# @file    config.h
#
# @author  Daniel Zorychta
#
# @brief   This file contains configuration.
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


#ifndef _CONFIG_CONFIG_H_
#define _CONFIG_CONFIG_H_

#// static definitions
#define _NO_  0
#define _YES_ 1
#/*
_NO_  = 0
_YES_ = 1
#*/

#// include all configuration flags
#include "./project/project_flags.h"
#include "./arch/arch_flags.h"
#include "./os/os_flags.h"
#include "./filesystems/filesystems_flags.h"
#include "./network/network_flags.h"
#/* include all configuration flags in Makefile
include ./config/project/project_flags.h
include ./config/arch/arch_flags.h
include ./config/filesystems/filesystems_flags.h
include ./config/network/network_flags.h
#*/

#/*--
# this:SetLayout("TitledGridBack", 2, "Home")
#++*/

#/*--
# -- import event used to disables all modules
# -- this flags must exists in the arch/arch_flags.h file
# this:SetEvent("ConfigImport",
#     function()
#         local MODULES = {
#             "__ENABLE_GPIO__",
#             "__ENABLE_AFIO__",
#             "__ENABLE_PLL__",
#             "__ENABLE_RTC__",
#             "__ENABLE_CRC__",
#             "__ENABLE_ETHMAC__",
#             "__ENABLE_SPI__",
#             "__ENABLE_UART__",
#             "__ENABLE_WDG__",
#             "__ENABLE_USBD__",
#             "__ENABLE_I2C__",
#             "__ENABLE_IRQ__",
#             "__ENABLE_LOOP__",
#             "__ENABLE_SDSPI__",
#             "__ENABLE_TTY__"
#         }
#
#         for i, flag in ipairs(MODULES) do
#             this:SetFlagValue(flag, "_NO_", "arch/arch_flags.h")
#         end
#     end)
#++*/

#/*--
# this:AddExtraWidget("Label", "LabelProject", "Project", -1, "bold")
# this:AddExtraWidget("Hyperlink", "HL_Project", "Configure")
# this:SetEvent("clicked", "HL_Project", function() this:LoadFile("project/project_flags.h") end)
#++*/

#/*--
# this:AddExtraWidget("Label", "LabelOS", "Operating System  ", -1, "bold")
# this:AddExtraWidget("Hyperlink", "HL_OS", "Configure")
# this:SetEvent("clicked", "HL_OS", function() this:LoadFile("os/os_flags.h") end)
#++*/

#/*--
# this:AddExtraWidget("Label", "LabelFS", "File Systems", -1, "bold")
# this:AddExtraWidget("Hyperlink", "HL_FS", "Configure")
# this:SetEvent("clicked", "HL_FS", function() this:LoadFile("filesystems/filesystems_flags.h") end)
#++*/

#/*--
# this:AddExtraWidget("Label", "LabelNet", "Network", -1, "bold")
# this:AddExtraWidget("Hyperlink", "HL_Net", "Configure")
# this:SetEvent("clicked", "HL_Net", function() this:LoadFile("network/network_flags.h") end)
#++*/

#/*--
# this:AddExtraWidget("Label", "LabelArch", "Microcontroller", -1, "bold")
# this:AddExtraWidget("Hyperlink", "HL_Arch", "Configure")
# this:SetEvent("clicked", "HL_Arch", function() this:LoadFile("arch/arch_flags.h") end)
#++*/

#endif /* _CONFIG_CONFIG_H_ */
#/*=============================================================================
#  End of file
#=============================================================================*/
