--[[============================================================================
@file    defs.lua

@author  Daniel Zorychta

@brief   Configuration definitions.

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

--------------------------------------------------------------------------------
-- GLOBAL VARIABLES
--------------------------------------------------------------------------------
arch            = ""
yes             = "__YES__"
no              = "__NO__"
current_cfg     = 1
total_cfg       = 0

--------------------------------------------------------------------------------
-- FUNCTIONS
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
-- @brief Function create progress string
-- @return progress string
--------------------------------------------------------------------------------
function progress()
        local msg = "(" .. current_cfg .. "/" .. total_cfg .. ") "
        current_cfg = current_cfg + 1
        return msg
end

--------------------------------------------------------------------------------
-- @brief The last message after configuration
--------------------------------------------------------------------------------
function configuration_finished()
        title("Configuration done!")
        msg("Your configuration is done. If you want to configure your project again, then run wizard as previously.")
        msg("To compile your project enter specified commands in the command line:")
        msg("        make clean")
        msg("        make")
        msg("")
        msg("Enjoy!")
        pause()
end

--------------------------------------------------------------------------------
-- @brief Function read current architecture from file
-- Function set architecture variable directly, and also return it.
-- @return current architecture
--------------------------------------------------------------------------------
function get_cpu_architecture()
        arch = key_read("../project/Makefile", "PROJECT_CPU_ARCH")
        return arch
end

--------------------------------------------------------------------------------
-- END OF FILE
--------------------------------------------------------------------------------
