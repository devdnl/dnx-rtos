--[[============================================================================
@file    config_manager.lua

@author  Daniel Zorychta

@brief   Configuration manager. Is used to import and export configuration

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


==============================================================================]]

--==============================================================================
-- EXTERNAL MODULES
--==============================================================================
require("modules/ctcore")

--==============================================================================
-- PUBLIC OBJECTS
--==============================================================================
cm = {}

--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local DIR_CONFIG   = config.project.path.config_dir:GetValue()
local FILE_ID      = "87f472ea728616a4127b47dc08e5f2d2"
local FILE_VERSION = "1"

--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief
-- @param
-- @return
--------------------------------------------------------------------------------

--==============================================================================
-- PUBLIC FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief
-- @param
-- @return
--------------------------------------------------------------------------------
function cm:save_project_configuration(file)
        local cfg_table = {}
        local cpu_arch = ct:key_read(config.project.key.PROJECT_CPU_ARCH)

        cfg_table.ID      = FILE_ID
        cfg_table.version = FILE_VERSION

        -- load configuration of modules of selected architecture
        cfg_table.file = {}

        for i, module in pairs(config.project.modules:Children()) do
                local module_name = module.name:GetValue():lower()
                local noarch      = module["@noarch"]
                local cfgfile

                if noarch == "true" then
                        cfgfile = DIR_CONFIG.."/noarch/"..module_name.."_flags.h"
                else
                        cfgfile = DIR_CONFIG.."/"..cpu_arch.."/"..module_name.."_flags.h"
                end

                cfg_table.file[cfgfile] = {}

                local f = io.open(cfgfile, "r")
                if f then
                        for line in f:lines() do
                                if line:match("%s*#define%s+__.*__%s+.*") then
                                        local _, _, k, v = line:find("%s*#define%s+(__.*__)%s+(.*)")
                                        table.insert(cfg_table.file[cfgfile], {key = k, value = v})
                                end
                        end

                        f:close()
                end
        end

        -- load project configuration

        -- load CPU specific configuration

        return ct:save_table(cfg_table, file)
end


--------------------------------------------------------------------------------
-- @brief
-- @param
-- @return
--------------------------------------------------------------------------------
function cm:apply_project_configuration(file)
        local cfg_table = ct:load_table(file)

        if cfg_table then
                for name, path in pairs(cfg_table.file) do
                        print(name)

                        for i, cfg in pairs(path) do
                                print(i, cfg.key, cfg.value)
                        end
                end
        end

        return false
end
