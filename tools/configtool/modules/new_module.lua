--[[============================================================================
@file    new_module.lua

@author  Daniel Zorychta

@brief   Script that helps create a new module

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
new_module = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local ui = {}
local ID = {}


--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function load_cpu_list(arch_name)
        for _, cpu in pairs(config.arch[arch_name].cpulist:Children()) do
                local cpu_name = cpu.name:GetValue()

                ui.CheckListBox_module_assign:Append(cpu_name)
        end
end


--------------------------------------------------------------------------------
-- @brief
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function get_checked_items(checklistbox)
        local checked_items = {}

        for i = 0, checklistbox:GetCount() do
                if checklistbox:IsChecked(i) then
                        table.insert(checked_items, checklistbox:GetString(i))
                end
        end

        return checked_items
end


--------------------------------------------------------------------------------
-- @brief
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function event_checkbox_noarch_clicked(event)
        local checkbox = event:GetEventObject():DynamicCast("wxCheckBox")

        ui.CheckListBox_arch_list:Enable(not checkbox:IsChecked())

        -- clear list
        ui.CheckListBox_module_assign:Clear()

        -- add CPUs of architectures
        if checkbox:IsChecked() then
                for _, arch_name in pairs(config.arch:Children()) do
                        load_cpu_list(arch_name:GetName())
                end
        else
                for _, arch_name in pairs(get_checked_items(ui.CheckListBox_arch_list)) do
                        load_cpu_list(arch_name)
                end
        end
end


--------------------------------------------------------------------------------
-- @brief
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function event_button_module_assign_select_all_clicked(event)
        for i = 0, ui.CheckListBox_module_assign:GetCount() do
                ui.CheckListBox_module_assign:Check(i, true)
        end
end


--------------------------------------------------------------------------------
-- @brief
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function event_button_module_assign_deselect_all_clicked(event)
        for i = 0, ui.CheckListBox_module_assign:GetCount() do
                ui.CheckListBox_module_assign:Check(i, false)
        end
end


--------------------------------------------------------------------------------
-- @brief
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function event_checklistbox_clicked(event)
        local selected_arch = get_checked_items(ui.CheckListBox_arch_list)

        ui.CheckListBox_module_assign:Clear()

        for _, arch_name in pairs(selected_arch) do
                load_cpu_list(arch_name)
        end
end


--------------------------------------------------------------------------------
-- @brief
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function event_button_create_clicked(event)
        local selected_arch = get_checked_items(ui.CheckListBox_arch_list)
        local selected_cpu  = get_checked_items(ui.CheckListBox_module_assign)
        local noarch        = ui.CheckBox_noarch:IsChecked()
        local module_name   = ui.TextCtrl_module_name:GetValue()

        -- check if all fields are filled
        if module_name:len() == 0 then
                ct:show_info_msg(ct.MAIN_WINDOW_NAME, "Enter module name.", ui.window)
                return
        end

        if #selected_arch == 0 and not noarch then
                ct:show_info_msg(ct.MAIN_WINDOW_NAME, "Select architectures.", ui.window)
                return
        end

        if #selected_cpu == 0 then
                ct:show_info_msg(ct.MAIN_WINDOW_NAME, "Select microcontrollers.", ui.window)
                return
        end


        --
end


--==============================================================================
-- GLOBAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function new_module:create_window(parent)
        if ui.window == nil then
                ID.TEXTCTRL_MODULE_NAME = wx.wxNewId()
                ID.CHECKBOX_NOARCH = wx.wxNewId()
                ID.CHECKLISTBOX_ARCH_LIST = wx.wxNewId()
                ID.CHECKLISTBOX_MODULE_ASSIGN = wx.wxNewId()
                ID.BUTTON_CREATE = wx.wxNewId()
                ID.BUTTON_MODULE_ASSING_SELECT_ALL = wx.wxNewId()
                ID.BUTTON_MODULE_ASSIGN_DESELECT_ALL = wx.wxNewId()

                ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
                local this = ui.window

                ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)


                ui.StaticBoxSizer_module_name = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Module\'s name")
                ui.TextCtrl_module_name = wx.wxTextCtrl(this, ID.TEXTCTRL_MODULE_NAME, "", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.StaticBoxSizer_module_name:Add(ui.TextCtrl_module_name, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer_module_name, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)


                ui.StaticBoxSizer_module_arch = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Module\'s architecture")
                ui.FlexGridSizer2 = wx.wxFlexGridSizer(0, 1, 0, 0)
                ui.CheckBox_noarch = wx.wxCheckBox(this, ID.CHECKBOX_NOARCH, "No architecture depended (noarch)", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer2:Add(ui.CheckBox_noarch, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.CheckListBox_arch_list = wx.wxCheckListBox(this, ID.CHECKLISTBOX_ARCH_LIST, wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, -1), {}, 0)
                for i = 1, config.arch:NumChildren() do
                        ui.CheckListBox_arch_list:Append(config.arch:Children()[i]:GetName())
                end
                ui.FlexGridSizer2:Add(ui.CheckListBox_arch_list, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticBoxSizer_module_arch:Add(ui.FlexGridSizer2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer_module_arch, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)



                ui.StaticBoxSizer_module_assign = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Module assignment to microcontrollers")
                ui.FlexGridSizer3 = wx.wxFlexGridSizer(0, 1, 0, 0)
                ui.CheckListBox_module_assign = wx.wxCheckListBox(this, ID.CHECKLISTBOX_MODULE_ASSIGN, wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, 200), {}, 0)
                ui.FlexGridSizer3:Add(ui.CheckListBox_module_assign, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.BoxSizer1 = wx.wxBoxSizer(wx.wxHORIZONTAL)
                ui.Button_module_assign_select_all = wx.wxButton(this, ID.BUTTON_MODULE_ASSING_SELECT_ALL, "Select all", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.BoxSizer1:Add(ui.Button_module_assign_select_all, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Button_module_assign_deselect_all = wx.wxButton(this, ID.BUTTON_MODULE_ASSIGN_DESELECT_ALL, "Deselect all", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.BoxSizer1:Add(ui.Button_module_assign_deselect_all, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer3:Add(ui.BoxSizer1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticBoxSizer_module_assign:Add(ui.FlexGridSizer3, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer_module_assign, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)


                ui.StaticLine1 = wx.wxStaticLine(this, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, -1), wx.wxLI_HORIZONTAL)
                ui.FlexGridSizer1:Add(ui.StaticLine1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)


                ui.Button_create = wx.wxButton(this, ID.BUTTON_CREATE, "Create module", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer1:Add(ui.Button_create, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)


                this:SetSizer(ui.FlexGridSizer1)
                this:SetScrollRate(5, 5)


                this:Connect(ID.CHECKBOX_NOARCH, wx.wxEVT_COMMAND_CHECKBOX_CLICKED, event_checkbox_noarch_clicked)
                this:Connect(ID.CHECKLISTBOX_ARCH_LIST, wx.wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, event_checklistbox_clicked)
                this:Connect(ID.BUTTON_MODULE_ASSING_SELECT_ALL, wx.wxEVT_COMMAND_BUTTON_CLICKED, event_button_module_assign_select_all_clicked)
                this:Connect(ID.BUTTON_MODULE_ASSIGN_DESELECT_ALL, wx.wxEVT_COMMAND_BUTTON_CLICKED, event_button_module_assign_deselect_all_clicked)
                this:Connect(ID.BUTTON_CREATE, wx.wxEVT_COMMAND_BUTTON_CLICKED, event_button_create_clicked)
        end

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @param  None
-- @return Module name
--------------------------------------------------------------------------------
function new_module:get_window_name()
        return "New module"
end


--------------------------------------------------------------------------------
-- @brief  Function is called when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function new_module:refresh()
end


--------------------------------------------------------------------------------
-- @brief  Function check if options are modified
-- @param  None
-- @return true if options are modified, otherwise false
--------------------------------------------------------------------------------
function new_module:is_modified()
        return false
end
