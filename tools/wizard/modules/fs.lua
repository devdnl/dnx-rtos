--[[============================================================================
@file    fs.lua

@author  Daniel Zorychta

@brief   File systems configuration file.

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
require "db"

--------------------------------------------------------------------------------
-- GLOBAL OBJECTS
--------------------------------------------------------------------------------
-- public calls objects
fs = {}

--------------------------------------------------------------------------------
-- FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Calculate total steps of this configuration
--------------------------------------------------------------------------------
local function calculate_total_steps()
        set_total_steps(4)
end

--------------------------------------------------------------------------------
-- @brief Configure devfs enable
--------------------------------------------------------------------------------
local function configure_devfs_enable()
        local choice = key_read(db.path.project.flags, "__ENABLE_DEVFS__")
        msg(progress() .. "The devfs is a small file system, that can store device and pipe files only. "..
                          "File system is minimalistic and thus is ideal for devices files, because it is fast. "..
                          "A data of this file system is stored in the RAM.")
        msg("Do you want to enable devfs?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save(db.path.project.flags, "__ENABLE_DEVFS__", choice)
                key_save(db.path.project.mk,    "ENABLE_DEVFS",     choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Configure lfs enable
--------------------------------------------------------------------------------
local function configure_lfs_enable()
        local choice = key_read(db.path.project.flags, "__ENABLE_LFS__")
        msg(progress() .. "The lfs is a general purpose RAM file system, that can be used to store files, device-files, and pipes. "..
                          "The file system is the best choice if you want to use only one file system for each file operations. "..
                          "The lfs is not fast as devfs, because provide greater functionality.")
        msg("Do you want to enable lfs?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save(db.path.project.flags, "__ENABLE_LFS__", choice)
                key_save(db.path.project.mk,    "ENABLE_LFS",     choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Configure fatfs enable
--------------------------------------------------------------------------------
local function configure_fatfs_enable()
        local choice = key_read(db.path.project.flags, "__ENABLE_FATFS__")
        msg(progress() .. "If you want to use FAT12, FAT16, and FAT32 in your system then enable this file system. "..
                          "The fatfs store data in device-file e.g. SD cards and other bigger volumes. "..
                          "This file system does not support special files.")
        msg("Do you want to enable fatfs?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save(db.path.project.flags, "__ENABLE_FATFS__", choice)
                key_save(db.path.project.mk,    "ENABLE_FATFS",     choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Configure procfs enable
--------------------------------------------------------------------------------
local function configure_procfs_enable()
        local choice = key_read(db.path.project.flags, "__ENABLE_PROCFS__")
        msg(progress() .. "The procfs is a special file system, that provide special functionality; "..
                          "from this file system you can see all tasks and their names and so on. "..
                          "In this file system are stored special system files, that can be read to obtain system specified settings, or microcontroller information. "..
                          "If you do not need to read special information, then probably you do not need this file system.")
        msg("Do you want to enable procfs?")
        msg("Current choice is: " .. filter_yes_no(choice) .. ".")
        add_item(yes, "YES")
        add_item(no, "NO")
        choice = get_selection()
        if (can_be_saved(choice)) then
                key_save(db.path.project.flags, "__ENABLE_PROCFS__", choice)
                key_save(db.path.project.mk,    "ENABLE_PROCFS",     choice)
        end

        return choice
end

--------------------------------------------------------------------------------
-- @brief Function execute configuration
--------------------------------------------------------------------------------
function fs:configure()
        calculate_total_steps()

        title("File Systems Configuration")
        navigation("Home/File systems")

        ::devfs_enable::  if configure_devfs_enable()  == back then return back       end
        ::lfs_enable::    if configure_lfs_enable()    == back then goto devfs_enable end
        ::fatfs_enable::  if configure_fatfs_enable()  == back then goto lfs_enable   end
        ::procfs_enable:: if configure_procfs_enable() == back then goto fatfs_enable end

        return next
end

--------------------------------------------------------------------------------
-- Enable configuration if master wizard is not defined
--------------------------------------------------------------------------------
-- started without master file
if (master ~= true) then
        show_no_master_info()
end

--------------------------------------------------------------------------------
-- END OF FILE
--------------------------------------------------------------------------------
