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

--------------------------------------------------------------------------------
-- FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Calculate total steps of this configuration
--------------------------------------------------------------------------------
local function calculate_total_steps()
        total_cfg = 2
end

--------------------------------------------------------------------------------
-- @brief Configure heap size
--------------------------------------------------------------------------------
function configure_heap_size()
        local value = key_read("../project/flags.h", "__HEAP_SIZE__")
        msg(progress() .. "Configure heap size. Heap must be smaller than RAM size.")
        msg("Current heap size is: " .. value .. " bytes.")
        value = get_number("dec", 1024, 1*1024*1024*1024)
        if (value ~= "") then
                key_save("../project/flags.h", "__HEAP_SIZE__", value)
        end
end

--------------------------------------------------------------------------------
-- @brief Configre heap block size
--------------------------------------------------------------------------------
function configure_heap_block_size()
        local value = key_read("../project/flags.h", "__HEAP_BLOCK_SIZE__")
        msg(progress() .. "Configure the smallest block size that can be allocated in the heap.")
        msg("Current block size is: " .. value .. " bytes.")
        value = get_number("dec", 1, 4096)
        if (value ~= "") then
                key_save("../project/flags.h", "__HEAP_BLOCK_SIZE__", value)
        end
end

--------------------------------------------------------------------------------
-- @brief Function execute memory configuration
--------------------------------------------------------------------------------
function mem_configure()
        current_cfg = 1
        title("Dynamic Memory Management Configuration")
        calculate_total_steps()
        configure_heap_size()
        configure_heap_block_size()
end

--------------------------------------------------------------------------------
-- Enable configuration if master wizard is not defined
--------------------------------------------------------------------------------
if (master ~= true) then
        mem_configure()
        configuration_finished()
end

--------------------------------------------------------------------------------
-- END OF FILE
--------------------------------------------------------------------------------
