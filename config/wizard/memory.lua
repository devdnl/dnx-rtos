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
require "cpu"

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
        local value = key_read("../project/flags.h", "__HEAP_SIZE__")
        msg(progress() .. "Configure heap size. Heap must be smaller than ".. cpu:get_RAM_size() .." bytes.")
        msg("Current heap size is: " .. value .. " bytes.")
        value = get_number("dec", 1024, cpu:get_RAM_size())
        if (can_be_saved(value)) then
                key_save("../project/flags.h", "__HEAP_SIZE__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Configre heap block size
--------------------------------------------------------------------------------
local function configure_heap_block_size()
        local value = key_read("../project/flags.h", "__HEAP_BLOCK_SIZE__")
        msg(progress() .. "Configure the smallest block size that can be allocated in the heap.")
        msg("Current block size is: " .. value .. " bytes.")
        value = get_number("dec", 1, 4096)
        if (can_be_saved(value)) then
                key_save("../project/flags.h", "__HEAP_BLOCK_SIZE__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Function execute configuration
--------------------------------------------------------------------------------
function mem:configure()
        calculate_total_steps()

        title("Dynamic Memory Management Configuration")

        ::_01_::
        if configure_heap_size() == back then
                return back
        end

        ::_02_::
        if configure_heap_block_size() == back then
                goto _01_
        end

        return next
end

--------------------------------------------------------------------------------
-- Enable configuration if master wizard is not defined
--------------------------------------------------------------------------------
if (master ~= true) then
        while mem:configure() == back do
        end
        configuration_finished()
end

--------------------------------------------------------------------------------
-- END OF FILE
--------------------------------------------------------------------------------
