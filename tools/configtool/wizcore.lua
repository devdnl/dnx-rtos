--[[============================================================================
@file    wizcore.lua

@author  Daniel Zorychta

@brief   The core functions used by wizard

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
require("wx")
xml = require("xmlSimple").newParser()


--==============================================================================
-- PUBLIC OBJECTS
--==============================================================================
-- entire xml configuration mapped in the lua's table
config = xml:loadFile("config.xml").config

-- shared module variables
wizcore = {}
wizcore.MAIN_WINDOW_NAME = config.tool.window.name:GetValue()
wizcore.WINDOW_X_SIZE    = tonumber(config.tool.window.xsize:GetValue())
wizcore.WINDOW_Y_SIZE    = tonumber(config.tool.window.ysize:GetValue())
wizcore.CONTROL_X_SIZE   = tonumber(config.tool.window.csize:GetValue())


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local FILETYPE_HEADER   = 0
local FILETYPE_MAKEFILE = 1


--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function try found specified key in the selected line
-- @param  line         line where key is finding
-- @param  filetype     type of file from line comes
-- @param  key          key to find
-- @return true if key was found, otherwise false
--------------------------------------------------------------------------------
local function line_found(line, filetype, key)
        if filetype == FILETYPE_HEADER then
                return line:match('^%s*#define%s+'..key..'%s*.*')

        elseif filetype == FILETYPE_MAKEFILE then
                return line:match('^%s*'..key..'%s*=%s*.*')
        end

        return false
end


--------------------------------------------------------------------------------
-- @brief  Function modify selected line by adding new value to the specified key
-- @param  line         line to modify
-- @param  filetype     type of file from line comes
-- @param  key          key to find
-- @param  value        value to modify
-- @return Modified line
--------------------------------------------------------------------------------
local function modify_line(line, filetype, key, value)
        if filetype == FILETYPE_HEADER then
                return "#define "..key.." "..value
        elseif filetype == FILETYPE_MAKEFILE then
                return key.."="..value
        end

        return line
end


--------------------------------------------------------------------------------
-- @brief  Function returns a value of selected key
-- @param  line         line from value is getting
-- @param  filetype     type of file from line comes
-- @param  key          key from value is getting
-- @return Value of selected key
--------------------------------------------------------------------------------
local function get_line_value(line, filetype, key)
        if filetype == FILETYPE_HEADER then
                local _, _, value = line:find("%s*#define%s+"..key.."%s+(.*)")
                return value
        elseif filetype == FILETYPE_MAKEFILE then
                local _, _, value = line:find("%s*"..key.."%s*=%s*(.*)")
                return value
        end

        return ""
end


--==============================================================================
-- PUBLIC FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function shows error dialog. Function kills entire wizard.
-- @param  title        window title
-- @param  caption      window caption
-- @return None
--------------------------------------------------------------------------------
function wizcore:show_error_msg(title, caption)
        dialog = wx.wxMessageDialog(wx.NULL, caption, title, bit.bor(wx.wxOK, wx.wxICON_ERROR))
        dialog:ShowModal()
        wx.wxGetApp():ExitMainLoop()
end


--------------------------------------------------------------------------------
-- @brief  Function shows info dialog
-- @param  title        window title
-- @param  caption      window caption
-- @return None
--------------------------------------------------------------------------------
function wizcore:show_info_msg(title, caption)
        dialog = wx.wxMessageDialog(wx.NULL, caption, title, bit.bor(wx.wxOK, wx.wxICON_INFORMATION))
        dialog:ShowModal()
end


--------------------------------------------------------------------------------
-- @brief  Function shows question dialog
-- @param  title        window title
-- @param  caption      window caption
-- @param  buttons      wxWidgets button definitions to show
-- @return Selected button
--------------------------------------------------------------------------------
function wizcore:show_question_msg(title, caption, buttons)
        dialog = wx.wxMessageDialog(wx.NULL, caption, title, bit.bor(buttons, wx.wxICON_QUESTION))
        return dialog:ShowModal()
end


--------------------------------------------------------------------------------
-- @brief  Function returns size of main window in form of table {xs, ys}
-- @param  None
-- @return Main window size if form of table {xs, ys}
--------------------------------------------------------------------------------
function wizcore:get_window_size()
        return wizcore.WINDOW_X_SIZE, wizcore.WINDOW_Y_SIZE
end


--------------------------------------------------------------------------------
-- @brief  Writes value of selected key
-- @param  keypath      key and path from xml configuration (table)
-- @param  value        value to write
-- @return On success true is returned. On error false is returned.
--------------------------------------------------------------------------------
function wizcore:key_write(keypath, value)
        local filename = keypath.path:GetValue()
        local key      = keypath.key:GetValue()

        -- type check
        if type(filename) ~= "string" then
                wizcore:show_error_msg(wizcore.MAIN_WINDOW_NAME, "key_write(): Invalid type of 'filename <"..type(filename)..">'\n"..debug.traceback())
                return false
        end

        if type(key) ~= "string" then
                wizcore:show_error_msg(wizcore.MAIN_WINDOW_NAME, "key_write(): Invalid type of 'key <"..type(key)..">'\n"..debug.traceback())
                return false
        end

        if type(value) ~= "string" then
                wizcore:show_error_msg(wizcore.MAIN_WINDOW_NAME, "key_write(): Invalid type of 'value <"..type(value)..">'\n"..debug.traceback())
                return false
        end

        -- read file
        local file = io.open(filename, "r")
        if file == nil then
                wizcore:show_error_msg(wizcore.MAIN_WINDOW_NAME, "key_write(): "..filename..": Cannot open file specified\n"..debug.traceback())
                return false
        end

        -- file type check
        local filetype
        if filename:find(".h") or filename:find(".hpp") or filename:find(".hh") or filename:find(".hxx") then
                filetype = FILETYPE_HEADER
        elseif filename:find(".mk") or filename:find(".mak") or filename:find(".makefile") or filename:find("Makefile") or filename:find("makefile") then
                filetype = FILETYPE_MAKEFILE
        else
                wizcore:show_error_msg(wizcore.MAIN_WINDOW_NAME, "key_read(): Unknown file type\n"..debug.traceback())
                return false
        end

        -- key modifing
        local lines = {}
        local line_cnt = 1
        for line in file:lines() do

                if line_found(line, filetype, key) then
                        lines[#lines + 1] = modify_line(line, filetype, key, value)
                else
                        lines[#lines + 1] = line
                end

                line_cnt = line_cnt + 1
        end
        file:close()

        -- write the file.
        file = io.open(filename, "w")
        if file == nil then
                wizcore:show_error_msg(wizcore.MAIN_WINDOW_NAME, "key_write(): File write protected\n"..debug.traceback())
                return false
        end

        for i, line in ipairs(lines) do
                file:write(line, "\n")
        end
        file:close()

        return true
end


--------------------------------------------------------------------------------
-- @brief  Reads value of selected key
-- @param  keypath      key and path from xml configuration (table)
-- @return On success a value with form of string, otherwise nil.
--------------------------------------------------------------------------------
function wizcore:key_read(keypath)
        local filename = keypath.path:GetValue()
        local key      = keypath.key:GetValue()

        -- type check
        if type(filename) ~= "string" then
                wizcore:show_error_msg(wizcore.MAIN_WINDOW_NAME, "key_read(): Invalid type of 'filename'\n"..debug.traceback())
                return nil
        end

        if type(key) ~= "string" then
                wizcore:show_error_msg(wizcore.MAIN_WINDOW_NAME, "key_read(): Invalid type of 'key'\n"..debug.traceback())
                return nil
        end

        -- read file
        local file = io.open(filename, "r")
        if file == nil then
                wizcore:show_error_msg(wizcore.MAIN_WINDOW_NAME, "key_read(): "..filename..": Cannot open specified file\n"..debug.traceback())
                return nil
        end

        -- file type check
        local filetype
        if filename:find(".h") or filename:find(".hpp") or filename:find(".hh") or filename:find(".hxx") then
                filetype = FILETYPE_HEADER
        elseif filename:find(".mk") or filename:find(".mak") or filename:find(".makefile") or filename:find("Makefile") or filename:find("makefile") then
                filetype = FILETYPE_MAKEFILE
        else
                wizcore:show_error_msg(wizcore.MAIN_WINDOW_NAME, "key_read(): Unknown file type\n"..debug.traceback())
                return nil
        end

        -- key finding
        local value = nil
        for line in file:lines() do
                if line_found(line, filetype, key) then
                        value = get_line_value(line, filetype, key)
                        if type(value) == "string" then
                                value = value:gsub("\r", "")
                        end

                        break
                end
        end
        file:close()

        if value == nil then
                wizcore:show_error_msg(wizcore.MAIN_WINDOW_NAME, "key_read(): '"..key.."': key not found\n"..debug.traceback())
        end

        return value
end


--------------------------------------------------------------------------------
-- @brief  Returns an index of the string in the selected table
-- @param  tab      table of strings
-- @param  str      string to find
-- @return On success string index is returned (1..+). On error 0 is returned.
--------------------------------------------------------------------------------
function wizcore:get_string_index(tab, str)
        for i, s in ipairs(tab) do
                if s:match(str) then
                        return i
                end
        end

        return 0
end


--------------------------------------------------------------------------------
-- @brief  Function enables/disables selected module
-- @param  name     name of module (string)
-- @param  state    new state of module (bool)
-- @return None
--------------------------------------------------------------------------------
function wizcore:enable_module(name, state)
        local key1 = config.project.key["ENABLE_"..name:upper().."_H"]
        local key2 = config.project.key["ENABLE_"..name:upper().."_MK"]

        wizcore:key_write(key1, wizcore:bool_to_yes_no(state))
        wizcore:key_write(key2, wizcore:bool_to_yes_no(state))
end


--------------------------------------------------------------------------------
-- @brief  Gets module state (check configuration consistency)
-- @param  name     name of module (string)
-- @return Module state in form of bool
--------------------------------------------------------------------------------
function wizcore:get_module_state(name)
        local key1 = config.project.key["ENABLE_"..name:upper().."_H"]
        local key2 = config.project.key["ENABLE_"..name:upper().."_MK"]

        local key1_value = wizcore:key_read(key1)
        local key2_value = wizcore:key_read(key2)

        if key1_value ~= key2_value then
                wizcore:show_error_msg(wizcore.MAIN_WINDOW_NAME, "Configuration inconsistency detected!\nValues of keys: ENABLE_"..name:upper().."_H and ENABLE_"..name:upper().."_MK are different!")
        end

        return wizcore:yes_no_to_bool(key1_value)
end


--------------------------------------------------------------------------------
-- @brief  Function converts yes/no strings to bool
-- @param  yes_no       yes/no string
-- @return yes_no converted to bool
--------------------------------------------------------------------------------
function wizcore:yes_no_to_bool(yes_no)
        if yes_no:match(config.project.def.YES:GetValue()) then
                return true
        else
                return false
        end
end


--------------------------------------------------------------------------------
-- @brief  Function converts bool to strings yes/no
-- @param  bool        value to convert
-- @return yes/no string
--------------------------------------------------------------------------------
function wizcore:bool_to_yes_no(bool)
        if bool then
                return config.project.def.YES:GetValue()
        else
                return config.project.def.NO:GetValue()
        end
end


--------------------------------------------------------------------------------
-- @brief  Function returns index of selected CPU
-- @param  cpu_arch     CPU architecture
-- @param  cpu_name     name of CPU
-- @return On success return CPU index from xml configuration. On error 0.
--------------------------------------------------------------------------------
function wizcore:get_cpu_index(cpu_arch, cpu_name)
        for i = 1, config.arch[cpu_arch].cpulist:NumChildren() do
                if config.arch[cpu_arch].cpulist:Children()[i].name:GetValue() == cpu_name then
                        return i
                end
        end

        return 0
end


--------------------------------------------------------------------------------
-- @brief Short verion of if statement
-- @param expr      test expression
-- @param ontrue    return value if expr is true
-- @param onfalse   return value if expr is false
-- @return Return ontrue if expr is true, otherwise onfalse is returned.
--------------------------------------------------------------------------------
function ifs(expr, ontrue, onfalse)
        if expr then
                return ontrue
        else
                return onfalse
        end
end


--------------------------------------------------------------------------------
-- @brief  Function return list of priorities for selected architecture
-- @param  cpu_arch     selected CPU architecture
-- @return List of priorities. Item format: {["name"] = name, ["value"] = value}
--------------------------------------------------------------------------------
function wizcore:get_priority_list(cpu_arch)
        local list = {}

        for i = 1, config.arch[cpu_arch].priorities:NumChildren() do
                local item = {}
                item.name  = config.arch[cpu_arch].priorities.priority[i].name:GetValue()
                item.value = config.arch[cpu_arch].priorities.priority[i].value:GetValue()
                list[#list + 1] = item
        end

        return list
end
