require("wx")
xml = require("xmlSimple").newParser()

config = xml:loadFile("config.xml").config

wizcore = {}
wizcore.MAIN_WINDOW_NAME = config.tool.window.name:GetValue()
wizcore.WINDOW_X_SIZE    = tonumber(config.tool.window.xsize:GetValue())
wizcore.WINDOW_Y_SIZE    = tonumber(config.tool.window.ysize:GetValue())
wizcore.CONTROL_X_SIZE   = tonumber(config.tool.window.csize:GetValue())


local FILETYPE_HEADER   = 0
local FILETYPE_MAKEFILE = 1


local function line_found(line, filetype, key)
        if filetype == FILETYPE_HEADER then
                return line:match('^%s*#define%s+'..key..'%s*.*')

        elseif filetype == FILETYPE_MAKEFILE then
                return line:match('^%s*'..key..'%s*=%s*.*')
        end

        return false
end


local function modify_line(line, filetype, key, value)
        if filetype == FILETYPE_HEADER then
                return "#define "..key.." "..value
        elseif filetype == FILETYPE_MAKEFILE then
                return key.."="..value
        end

        return line
end


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


function wizcore:show_error_msg(title, caption)
        print("["..title.."] "..caption)
        dialog = wx.wxMessageDialog(wx.NULL, caption, title, bit.bor(wx.wxOK, wx.wxICON_ERROR))
        dialog:ShowModal()
        wx.wxGetApp():ExitMainLoop()
end


function wizcore:show_info_msg(title, caption)
        print("["..title.."] "..caption)
        dialog = wx.wxMessageDialog(wx.NULL, caption, title, bit.bor(wx.wxOK, wx.wxICON_INFORMATION))
        dialog:ShowModal()
end


function wizcore:show_question_msg(title, caption, buttons)
        print("["..title.."] "..caption)
        dialog = wx.wxMessageDialog(wx.NULL, caption, title, bit.bor(buttons, wx.wxICON_QUESTION))
        return dialog:ShowModal()
end


function wizcore:get_window_size()
        return wizcore.WINDOW_X_SIZE, wizcore.WINDOW_Y_SIZE
end


function wizcore:key_write(keypath, value)
        local filename = keypath.path:GetValue()
        local key      = keypath.key:GetValue()

        -- type check
        if type(filename) ~= "string" or type(key) ~= "string" or type(value) ~= "string" then
                wizcore:show_error_msg(wizcore.MAIN_WINDOW_NAME, "key_write(): Invalid type of 'filename' or 'key' or 'value'\n"..debug.traceback())
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


function wizcore:key_read(keypath)
        local filename = keypath.path:GetValue()
        local key      = keypath.key:GetValue()

        -- type check
        if type(filename) ~= "string" or type(key) ~= "string" then
                wizcore:show_error_msg(wizcore.MAIN_WINDOW_NAME, "key_read(): Invalid type of 'filename' or 'key'\n"..debug.traceback())
                return false
        end

        -- read file
        local file = io.open(filename, "r")
        if file == nil then
                wizcore:show_error_msg(wizcore.MAIN_WINDOW_NAME, "key_read(): "..filename..": Cannot open specified file\n"..debug.traceback())
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

        -- key finding
        local value = nil
        for line in file:lines() do
                if line_found(line, filetype, key) then
                        value = get_line_value(line, filetype, key):gsub("\r", "")
                        break
                end
        end
        file:close()

        if value == nil then
                wizcore:show_error_msg(wizcore.MAIN_WINDOW_NAME, "key_read(): "..key..": key not found\n"..debug.traceback())
        end

        return value
end


function wizcore:get_string_index(table, string)
        for i, s in ipairs(table) do
                if s:match(string) then
                        return i
                end
        end

        return -1
end


function wizcore:enable_module(name, state)
        local key1 = config.project.key["ENABLE_"..name:upper().."_H"]
        local key2 = config.project.key["ENABLE_"..name:upper().."_MK"]

        wizcore:key_write(key1, wizcore:bool_to_yes_no(state))
        wizcore:key_write(key2, wizcore:bool_to_yes_no(state))
end


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


function wizcore:yes_no_to_bool(yes_no)
        if yes_no:match(config.project.def.YES:GetValue()) then
                return true
        else
                return false
        end
end


function wizcore:bool_to_yes_no(bool)
        if bool then
                return config.project.def.YES:GetValue()
        else
                return config.project.def.NO:GetValue()
        end
end


function wizcore:get_cpu_index(cpu_arch, cpu_name)
        for i = 1, config.arch[cpu_arch].cpulist:NumChildren() do
                if config.arch[cpu_arch].cpulist:Children()[i].name:GetValue() == cpu_name then
                        return i
                end
        end

        return 0
end


function ifs(expr, ontrue, onfalse)
        if expr then
                return ontrue
        else
                return onfalse
        end
end
