--[[============================================================================
@file    modules.lua

@author  Daniel Zorychta

@brief   Module is used to load CPU-specified peripherals configurations.

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
require("modules/ctcore")


--==============================================================================
-- GLOBAL OBJECTS
--==============================================================================
modules = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local ui   = {}
local page = {}


--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Event is called when notebook page is changed
-- @param  this         event object
-- @return None
--------------------------------------------------------------------------------
local function notebook_page_changed(this)
        local card = this:GetSelection() + 1
        if page[card] then page[card]:selected() end
        this:Skip()
end


--==============================================================================
-- GLOBAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function modules:create_window(parent)
        if ui.window == nil then
                ui.window    = wx.wxPanel(parent, wx.wxNewId())
                ui.BoxSizer1 = wx.wxBoxSizer(wx.wxHORIZONTAL)
                ui.notebook  = wx.wxNotebook(ui.window, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxLB_TOP)
                ui.BoxSizer1:Add(ui.notebook, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL))
                ui.window:SetSizer(ui.BoxSizer1)
                ui.window:Layout()

                ui.notebook:Connect(wx.wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, notebook_page_changed)
        end

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @param  None
-- @return Module name
--------------------------------------------------------------------------------
function modules:get_window_name()
        return "Modules"
end


--------------------------------------------------------------------------------
-- @brief  Function is called when window is selected
-- @return None
--------------------------------------------------------------------------------
function modules:refresh()
        local cpu_arch   = ct:key_read(config.project.key.PROJECT_CPU_ARCH)
        local cpu_name   = ct:key_read(config.arch[cpu_arch].key.CPU_NAME)
        local cpu_idx    = ct:get_cpu_index(cpu_arch, cpu_name)
        local periph_num = config.arch[cpu_arch].cpulist:Children()[cpu_idx].peripherals:NumChildren()
        page             = {}

        -- check if CPU exist
        if cpu_idx == 0 then
                ct:show_error_msg(ct.MAIN_WINDOW_NAME, cpu_name..": no such microcontroller.", ui.window)
                return
        end

        -- check if CPU has peripherals
        if periph_num == 0 then
                ct:show_info_msg(ct.MAIN_WINDOW_NAME, cpu_name..": no peripherals!", ui.window)
                return
        end

        -- create progress dialog
        local dialog = wx.wxProgressDialog("Loading configuration", "Clearing old data...", periph_num + 1, ui.window, bit.bor(wx.wxPD_APP_MODAL,wx.wxPD_AUTO_HIDE))
        dialog:SetMinSize(wx.wxSize(300, 100))
        dialog:Centre()

        -- clear all pages
        dialog:Update(0)
        ui.notebook:Hide()
        ui.notebook:DeleteAllPages()
        dialog:Update(1)

        -- load specified modules required by configuration for specified microcontroller
        for i = 1, periph_num do
                local peripheral  = config.arch[cpu_arch].cpulist:Children()[cpu_idx].peripherals:Children()[i]
                local module_name = peripheral:GetName():lower()
                local no_arch     = false
                local module_file

                -- find if module is a 'noarch' module
                local module_list = config.project.modules
                for m = 1, module_list:NumChildren() do
                        if module_name == module_list:Children()[m].name:GetValue():lower() then
                                if module_list:Children()[m]:NumProperties() ~= 0 then
                                        if module_list:Children()[m]["@noarch"] == "true" then
                                                no_arch = true
                                        end
                                end

                                break
                        end
                end

                -- create name of module file to load according to architecture
                if no_arch then
                        module_file = "arch/noarch/"..module_name
                else
                        module_file = "arch/"..cpu_arch.."/"..module_name
                end

                -- check that specified module file exist
                local f = io.open(module_file..".lua")
                if f then
                        f:close()
                else
                        dialog:Update(i, "Loading data of "..module_name:upper().."...")
                        ct:show_error_msg(ct.MAIN_WINDOW_NAME, "Configuration file for specified architecture and microcontroller does not exist!\n"..
                                                                         "Architecture: "..ifs(no_arch, "noarch", cpu_arch).."\n"..
                                                                         "Microcontroller: "..cpu_name.."\n"..
                                                                         "Module: "..module_name:upper().."\n"..
                                                                         "Expected file: "..module_file..".lua", ui.window)
                        return
                end

                -- load module and gets handler
                local module = require(module_file).get_handler()
                dialog:Update(i, "Loading data of "..module:get_window_name().."...")
                ui.notebook:AddPage(module:create_window(ui.notebook), module:get_window_name())
                page[i] = module
                dialog:Update(i+1)
        end

        ui.notebook:Show()
end


--------------------------------------------------------------------------------
-- @brief  Function check if options are modified
-- @return true if options are modified, otherwise false
--------------------------------------------------------------------------------
function modules:is_modified()
        for i, module in ipairs(page) do
                if module:is_modified() then
                        return true
                end
        end

        return false
end


--------------------------------------------------------------------------------
-- @brief  Function save configuration
-- @return None
--------------------------------------------------------------------------------
function modules:save()
        for i, module in ipairs(page) do
                if module:is_modified() then
                        module:save()
                end
        end
end
