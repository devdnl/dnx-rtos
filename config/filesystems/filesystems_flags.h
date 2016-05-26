#/*=============================================================================
# @file    fs.h
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

#ifndef _FILE_SYSTEMS_FLAGS_H_
#define _FILE_SYSTEMS_FLAGS_H_

#include "../filesystems/fatfs_flags.h"
#include "../filesystems/ext2fs_flags.h"

#/*--
# this:SetLayout("TitledGridBack", 2, "Home > File Systems",
#                function() this:LoadFile("config.h") end)
#++*/


#/*--
# this:AddExtraWidget("Label", "LabelRAM", "RAM file systems", -1, "bold")
# this:AddExtraWidget("Void", "VoidRAM")
#++*/

#/*--
# this:AddWidget("Checkbox", "Enable ramfs")
# this:SetToolTip("The ramfs is a general purpose RAM file system, that can be\n"..
#                 "used to store files, device-files, and pipes. The file system\n"..
#                 "is the best choice if you want to use only one file system for\n"..
#                 "each file operations. The ramfs is not fast as devfs, because\n"..
#                 "provides greater functionality.")
# this:AddExtraWidget("Hyperlink", "RAMFS_CONFIGURE", "")
# this:SetEvent("clicked", "RAMFS_CONFIGURE", function() end)
#--*/
#define __ENABLE_RAMFS__ _YES_
#/*
__ENABLE_RAMFS__=_YES_
#*/

#/*--
# this:AddWidget("Checkbox", "Enable devfs")
# this:SetToolTip("The devfs is a small file system, that can be used to store\n"..
#                 "device and pipe files only. This file system is minimalistic\n"..
#                 "and thus is ideal for devices-files, because is fast. A data\n"..
#                 "of this file system is stored in the RAM.")
# this:AddExtraWidget("Hyperlink", "DEVFS_CONFIGURE", "")
# this:SetEvent("clicked", "DEVFS_CONFIGURE", function() end)
#--*/
#define __ENABLE_DEVFS__ _YES_
#/*
__ENABLE_DEVFS__=_YES_
#*/


#/*--
# this:AddExtraWidget("Label", "LabelSpecial", "\nSpecial file systems", -1, "bold")
# this:AddExtraWidget("Void", "VoidSpecial")
#++*/

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
#define __ENABLE_PROCFS__ _YES_
#/*
__ENABLE_PROCFS__=_YES_
#*/


#/*--
# this:AddExtraWidget("Label", "LabelStorage", "\nStorage file systems", -1, "bold")
# this:AddExtraWidget("Void", "VoidStorage")
#++*/

#/*--
# this:AddWidget("Checkbox", "Enable fatfs")
# this:SetToolTip("If you want to use FAT12, FAT16, and FAT32 in your system\n"..
#                 "enable this file system. The fatfs read/write data from/to\n"..
#                 "device-file e.g. SD cards and other bigger volumes. This file\n"..
#                 "system does not support special files.")
# this:AddExtraWidget("Hyperlink", "FATFS_CONFIGURE", "Configure")
# this:SetEvent("clicked", "FATFS_CONFIGURE", function() this:LoadFile("filesystems/fatfs_flags.h") end)
#--*/
#define __ENABLE_FATFS__ _YES_
#/*
__ENABLE_FATFS__=_YES_
#*/

#/*--
# this:AddWidget("Checkbox", "Enable ext2fs")
# this:SetToolTip("The ext2fs is standard Linux file system. This file system\n"..
#                 "requires more RAM and stack than fatfs, but provides better\n"..
#                 "performance because of caching system. Higher values of cache\n"..
#                 "size makes file system faster. Cache size is a number of blocks\n"..
#                 "that will be load to memory.")
# this:AddExtraWidget("Hyperlink", "EXT2FS_CONFIGURE", "Configure")
# this:SetEvent("clicked", "EXT2FS_CONFIGURE", function() this:LoadFile("filesystems/ext2fs_flags.h") end)
#--*/
#define __ENABLE_EXT2FS__ _NO_
#/*
__ENABLE_EXT2FS__=_NO_
#*/

#endif /* _FILE_SYSTEMS_FLAGS_H_ */
#/*=============================================================================
#  End of file
#=============================================================================*/
