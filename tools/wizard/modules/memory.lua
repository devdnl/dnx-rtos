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
-- @brief Configre heap block size
--------------------------------------------------------------------------------
local function configure_heap_block_size()
        local value = key_read(db.path.project.flags, "__HEAP_BLOCK_SIZE__")
        msg(progress(1, 1) .. "Configure the smallest block size that can be allocated in the heap.")
        msg("Current block size is: " .. value .. " bytes.")
        value = get_number("dec", 4, 4096)
        if (can_be_saved(value)) then
                key_save(db.path.project.flags, "__HEAP_BLOCK_SIZE__", value)
        end

        return value
end

--------------------------------------------------------------------------------
-- @brief Configuration summary
--------------------------------------------------------------------------------
local function print_summary()
        local block_size = key_read(db.path.project.flags, "__HEAP_BLOCK_SIZE__")

        msg("Dynamic memory management configuration summary:")
        msg("Heap block size: "..block_size.." bytes")

        pause()
end

--------------------------------------------------------------------------------
-- @brief Function execute configuration
--------------------------------------------------------------------------------
function mem:configure()
        title("Dynamic Memory Management Configuration")
        navigation("Home/Memory")

        local pages = {configure_heap_block_size, print_summary}
        return show_pages(pages)
end

-- started without master file
if (master ~= true) then
        show_no_master_info()
end

--------------------------------------------------------------------------------
-- END OF FILE
--------------------------------------------------------------------------------
