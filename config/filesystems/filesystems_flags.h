#/*=============================================================================
# @file    filesystems_flags.h
#
# @author  Daniel Zorychta
#
# @brief   This file enable selected file systems.
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

#ifndef _FILE_SYSTEMS_FLAGS_H_
#define _FILE_SYSTEMS_FLAGS_H_

#/*--
# this:SetLayout("TitledGridBack", 2, "Home > File Systems",
#                function() this:LoadFile("config.h") end)
#++*/

#/*--
# this:AddWidget("Checkbox", "Enable ramfs")
# this:SetToolTip("The ramfs is a general purpose RAM file system, that can be\n"..
#                 "used to store files, device-files, and pipes. The file system\n"..
#                 "is the best choice if you want to use only one file system for\n"..
#                 "each file operations.")
# this:AddExtraWidget("Hyperlink", "RAMFS_CONFIGURE", "Configure")
# this:SetEvent("clicked", "RAMFS_CONFIGURE", function() this:LoadFile("filesystems/ramfs_flags.h") end)
#--*/
#include "../filesystems/ramfs_flags.h"
#define __ENABLE_RAMFS__ _YES_
#/*
__ENABLE_RAMFS__=_YES_
#*/

#/*--
# this:AddWidget("Checkbox", "Enable procfs")
# this:SetToolTip("The procfs is a special file system, that provides special\n"..
#                 "functionality; by using this file system you can see all\n"..
#                 "tasks and their names and so on. In this file system are\n"..
#                 "stored special system files, that can be read to obtain system\n"..
#                 "specified settings, or microcontroller information. If you do\n"..
#                 "not need to read special information, then probably you do not\n"..
#                 "need this file system.")
# this:AddExtraWidget("Hyperlink", "PROCFS_CONFIGURE", "")
# this:SetEvent("clicked", "PROCFS_CONFIGURE", function() end)
#--*/
#define __ENABLE_PROCFS__ _NO_
#/*
__ENABLE_PROCFS__=_NO_
#*/

#/*--
# this:AddWidget("Checkbox", "Enable fatfs")
# this:SetToolTip("If you want to use FAT12, FAT16, and FAT32 in your system\n"..
#                 "enable this file system. The fatfs read/write data from/to\n"..
#                 "device-file e.g. SD cards and other bigger volumes. This file\n"..
#                 "system does not support special files.")
# this:AddExtraWidget("Hyperlink", "FATFS_CONFIGURE", "Configure")
# this:SetEvent("clicked", "FATFS_CONFIGURE", function() this:LoadFile("filesystems/fatfs_flags.h") end)
#--*/
#include "../filesystems/fatfs_flags.h"
#define __ENABLE_FATFS__ _NO_
#/*
__ENABLE_FATFS__=_NO_
#*/

#/*--
# this:AddWidget("Checkbox", "Enable eefs")
# this:SetToolTip("The eefs is small file system that support very small EEPROM memories.\n"..
#                 "Typical size of EEPROM memory connected to I2C bus is 1-64 KiB thus\n"..
#                 "requires very small file system.")
# this:AddExtraWidget("Hyperlink", "EEFS_CONFIGURE", "Configure")
# this:SetEvent("clicked", "EEFS_CONFIGURE", function() this:LoadFile("filesystems/eefs_flags.h") end)
#--*/
#include "../filesystems/eefs_flags.h"
#define __ENABLE_EEFS__ _NO_
#/*
__ENABLE_EEFS__=_NO_
#*/

#/*--
# this:AddWidget("Checkbox", "Enable ext4fs")
# this:SetToolTip("The EXT4FS is a file system that support Linux file systems\n "..
#                 "like EXT2, EXT3, and EXT4.")
# this:SetToolTip("EXT2,3,4 file systems.")
# this:AddExtraWidget("Hyperlink", "EXT4FS_CONFIGURE", "Configure")
# this:SetEvent("clicked", "EXT4FS_CONFIGURE", function() this:LoadFile("filesystems/ext4fs_flags.h") end)
#--*/
#include "../filesystems/ext4fs_flags.h"
#define __ENABLE_EXT4FS__ _NO_
#/*
__ENABLE_EXT4FS__=_NO_
#*/

#/*--
# this:AddWidget("Checkbox", "Enable romfs")
# this:SetToolTip("ROM File System")
# this:AddExtraWidget("Hyperlink", "ROMFS_CONFIGURE", "Configure")
# this:SetEvent("clicked", "ROMFS_CONFIGURE", function() this:LoadFile("filesystems/romfs_flags.h") end)
#--*/
#include "../filesystems/romfs_flags.h"
#define __ENABLE_ROMFS__ _YES_
#/*
__ENABLE_ROMFS__=_YES_
#*/

#/*--
# this:AddWidget("Checkbox", "Enable flatfs")
# this:SetToolTip("FLAT File System")
# this:AddExtraWidget("Hyperlink", "FLATFS_CONFIGURE", "Configure")
# this:SetEvent("clicked", "FLATFS_CONFIGURE", function() this:LoadFile("filesystems/flatfs_flags.h") end)
#--*/
#include "../filesystems/flatfs_flags.h"
#define __ENABLE_FLATFS__ _NO_
#/*
__ENABLE_FLATFS__=_NO_
#*/

#endif /* _FILE_SYSTEMS_FLAGS_H_ */
#/*=============================================================================
#  End of file
#=============================================================================*/
