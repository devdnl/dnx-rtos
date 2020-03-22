#/*=============================================================================
# @file    sipc_flags.h
#
# @author  Daniel Zorychta
#
# @brief   This file contains configuration of SIPC stack.
#          Hybrid file: included both by Make and CC.
#
# @note    Copyright (C) 2019 Daniel Zorychta <daniel.zorychta@gmail.com>
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

#ifndef _SIPC_FLAGS_H_
#define _SIPC_FLAGS_H_

#/*--
# this:SetLayout("TitledGridBack", 2, "Home > Network > SIPC Stack",
#     function() -- back button
#         this:LoadFile("network/network_flags.h")
#     end
# )
#++*/


#/*--
# this:AddExtraWidget("Label", "LabelIfc", "Interface", -1, "bold")
# this:AddExtraWidget("Label", "LabelVoid", "                     ", 24, "bold")
#
# this:AddWidget("Editline", true, "Interface device path")
#--*/
#define __NETWORK_SIPC_INTERFACE_PATH__ "/dev/nc"

#/*--
# this:AddWidget("Combobox", "Debug messages")
# this:AddItem("Disable", "_NO_")
# this:AddItem("Enable", "_YES_")
#--*/
#define __NETWORK_SIPC_DEBUG_ON__ _NO_

#/*--
# this:AddWidget("Spinbox", 128, 65536, "Receive buffer size [B]")
#--*/
#define __NETWORK_SIPC_RECV_BUF_SIZE__ 2048

#endif /* _SIPC_FLAGS_H_ */
#/*=============================================================================
#  End of file
#=============================================================================*/
