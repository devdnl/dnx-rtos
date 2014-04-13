--[[============================================================================
@file    memory.lua

@author  Daniel Zorychta

@brief   Dynamic Memory Management configuration file.

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
mem = {}

--------------------------------------------------------------------------------
-- FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Calculate total steps of this configuration
--------------------------------------------------------------------------------
local function calculate_total_steps()
        set_total_steps(2)
end

--------------------------------------------------------------------------------
-- @brief Configure heap size
--------------------------------------------------------------------------------
local function configure_heap_size()
        local value    = key_read(db.path.project.flags, "__HEAP_SIZE__")
        local ram_size = db:get_mcu_ram_size(db:get_cpu_name())

        msg(progress() .. "Configure heap size. Heap must be smaller than ".. ram_size .." bytes.")
        msg("Current heap size is: " .. value .. " bytes.")
        value = get_number("dec", 1024, ram_size)
        if (can_be_saved(value)) then
                key_save(db.path.project.flags, "__HEAP_SIZE__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Configre heap block size
--------------------------------------------------------------------------------
local function configure_heap_block_size()
        local value = key_read(db.path.project.flags, "__HEAP_BLOCK_SIZE__")
        msg(progress() .. "Configure the smallest block size that can be allocated in the heap.")
        msg("Current block size is: " .. value .. " bytes.")
        value = get_number("dec", 1, 4096)
        if (can_be_saved(value)) then
                key_save(db.path.project.flags, "__HEAP_BLOCK_SIZE__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Function execute configuration
--------------------------------------------------------------------------------
function mem:configure()
        calculate_total_steps()

        title("Dynamic Memory Management Configuration")
        navigation("Home/Memory")

        ::heap_size::  if configure_heap_size()       == back then return back    end
        ::block_size:: if configure_heap_block_size() == back then goto heap_size end

        return next
end

-- started without master file
if (master ~= true) then
        show_no_master_info()
end

--------------------------------------------------------------------------------
-- END OF FILE
--------------------------------------------------------------------------------
