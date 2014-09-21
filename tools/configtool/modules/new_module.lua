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

local FILE_TEMPLATE_MODULE_CFG       = config.project.path.module_template_cfg_file:GetValue()
local FILE_TEMPLATE_MODULE_DEF       = config.project.path.module_template_def_file:GetValue()
local FILE_TEMPLATE_MODULE_FLAGS     = config.project.path.module_template_flags_file:GetValue()
local FILE_TEMPLATE_MODULE_IOCTL     = config.project.path.module_template_ioctl_file:GetValue()
local FILE_TEMPLATE_MODULE_SRC       = config.project.path.module_template_src_file:GetValue()
local FILE_TEMPLATE_MODULE_MK_ARCH   = config.project.path.module_template_makefile_arch_file:GetValue()
local FILE_TEMPLATE_MODULE_MK_NOARCH = config.project.path.module_template_makefile_noarch_file:GetValue()
local FILE_TEMPLATE_CONFIGTOOL_FORM  = config.project.path.configtool_template_form_file:GetValue()
local FILE_PROJECT_FLAGS             = config.project.path.project_flags_file:GetValue()
local FILE_PROJECT_MAKEFILE          = config.project.path.project_makefile:GetValue()
local FILE_DRIVERS_MAIN_MAKEFILE     = config.project.path.drivers_main_makefile:GetValue()
local FILE_SYS_IOCTL                 = config.project.path.sys_ioctl_file:GetValue()

local DIR_CONFIG          = config.project.path.config_dir:GetValue()
local DIR_DRIVERS         = config.project.path.drivers_dir:GetValue()
local DIR_CONFIGTOOL_ARCH = config.project.path.configtool_arch_dir:GetValue()

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
        local selected_arch      = get_checked_items(ui.CheckListBox_arch_list)
        local selected_cpu       = get_checked_items(ui.CheckListBox_module_assign)
        local module_name        = ui.TextCtrl_module_name:GetValue():lower()
        local module_description = ui.TextCtrl_module_description:GetValue()
        local module_author      = ui.TextCtrl_module_author:GetValue()
        local author_email       = ui.TextCtrl_author_email:GetValue()
        local n

        if ui.CheckBox_noarch:IsChecked() then
                selected_arch = {"noarch"}
        end

        local tags = {
                {tag = "<!cpu_arch!>", to = ""},
                {tag = "<!author!>", to = module_author},
                {tag = "<!module_description!>", to = module_description},
                {tag = "<!year!>", to = os.date("%Y")},
                {tag = "<!email!>", to = author_email},
                {tag = "<!MODULE_NAME!>", to = module_name:upper()},
                {tag = "<!module_name!>", to = module_name:lower()},
        }


        -- check if all fields are filled
        if module_name:len() == 0 or module_description:len() == 0 or module_author:len() == 0 or author_email:len() == 0 then
                ct:show_info_msg(ct.MAIN_WINDOW_NAME, "Fill all fields in the 'Module details'.", ui.window)
                return
        end

        if #selected_arch == 0 then
                ct:show_info_msg(ct.MAIN_WINDOW_NAME, "Select architecture.", ui.window)
                return
        end

        if #selected_cpu == 0 then
                ct:show_info_msg(ct.MAIN_WINDOW_NAME, "Select microcontrollers.", ui.window)
                return
        end

        -- checks if module exist
        if ct:exists(DIR_DRIVERS.."/"..module_name) then
                ct:show_info_msg(ct.MAIN_WINDOW_NAME, "Module already exists in the system.", ui.window)
                return
        end

        -- checks if project is not read only
        if not ct:mkdir("_test_") then
                ct:show_error_msg(ct.MAIN_WINDOW_NAME, "Project seems to be a read only.", ui.window)
                return
        else
                os.remove("_test_")
        end

        -- add module to the system for selected architectures
        for _, arch in pairs(selected_arch) do
                tags[1] = {tag = "<!cpu_arch!>", to = arch}


                -- create new folders
                ct:mkdir(DIR_DRIVERS.."/"..module_name.."/"..arch)
                ct:mkdir(DIR_CONFIG.."/"..arch)
                ct:mkdir(DIR_CONFIGTOOL_ARCH.."/"..arch)

                ct:apply_template(FILE_TEMPLATE_MODULE_FLAGS, DIR_CONFIG.."/"..arch.."/"..module_name.."_flags.h", tags)
                ct:apply_template(FILE_TEMPLATE_MODULE_CFG,   DIR_DRIVERS.."/"..module_name.."/"..arch.."/"..module_name.."_cfg.h", tags)
                ct:apply_template(FILE_TEMPLATE_MODULE_DEF,   DIR_DRIVERS.."/"..module_name.."/"..arch.."/"..module_name.."_def.h", tags)
                ct:apply_template(FILE_TEMPLATE_MODULE_IOCTL, DIR_DRIVERS.."/"..module_name.."/"..arch.."/"..module_name.."_ioctl.h", tags)
                ct:apply_template(FILE_TEMPLATE_MODULE_SRC,   DIR_DRIVERS.."/"..module_name.."/"..arch.."/"..module_name..".c", tags)


                -- add module form to configtool
                ct:apply_template(FILE_TEMPLATE_CONFIGTOOL_FORM, DIR_CONFIGTOOL_ARCH.."/"..arch.."/"..module_name..".lua", tags)


                -- include module in the system
                n = ct:find_line(FILE_PROJECT_FLAGS, 1, "#.?.?if %(__CPU_ARCH__%s*==%s*"..arch.."%)")
                if n then
                        ct:insert_line(FILE_PROJECT_FLAGS, n + 1, "#       include \"../"..arch.."/"..module_name.."_flags.h\"")
                else
                        ct:show_error_msg(ct.MAIN_WINDOW_NAME, "Corrupted '"..FILE_PROJECT_FLAGS.."' file.", ui.window)
                        return
                end


                -- add module to ioctl requests
                n = ct:find_line(FILE_SYS_IOCTL, 1, "#ifdef%s+ARCH_"..arch)
                if n then
                        ct:insert_line(FILE_SYS_IOCTL, n + 1, "#       include \""..arch.."/"..module_name.."_ioctl.h\"")
                else
                        ct:show_error_msg(ct.MAIN_WINDOW_NAME, "Corrupted '"..FILE_SYS_IOCTL.."' file.", ui.window)
                        return
                end

                -- add module to the configtool's xml configuration
        end


        -- adds enable flag in the flags.h file
        n = ct:find_line(FILE_PROJECT_FLAGS, 1, "/%* modules %*/")
        if n then
                ct:insert_line(FILE_PROJECT_FLAGS, n + 1, "#define __ENABLE_"..module_name:upper().."__ __NO__")
        else
                ct:show_error_msg(ct.MAIN_WINDOW_NAME, "Corrupted '"..FILE_PROJECT_FLAGS.."' file.", ui.window)
                return
        end


        -- adds enable flag in the Makefile
        n = ct:find_line(FILE_PROJECT_MAKEFILE, 1, "# modules enable flags")
        if n then
                ct:insert_line(FILE_PROJECT_MAKEFILE, n + 1, "ENABLE_"..module_name:upper().."__=__NO__")
        else
                ct:show_error_msg(ct.MAIN_WINDOW_NAME, "Corrupted '"..FILE_PROJECT_MAKEFILE.."' file.", ui.window)
                return
        end


        -- add module makefile
        if arch == "noarch" then
                ct:apply_template(FILE_TEMPLATE_MODULE_MK_NOARCH, DIR_DRIVERS.."/"..module_name.."/Makefile", tags)
        else
                ct:apply_template(FILE_TEMPLATE_MODULE_MK_ARCH, DIR_DRIVERS.."/"..module_name.."/Makefile", tags)
        end


        -- add module to drivers' main makefile
        ct:insert_line(FILE_DRIVERS_MAIN_MAKEFILE, 2, "include $(DRV_LOC)/"..module_name.."/Makefile")


        -- register module in the system


        -- assign modules to microcontrollers in the xml file


        -- finished
        ct:show_info_msg(ct.MAIN_WINDOW_NAME, "Module added successfully.", ui.window)
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
                ID.TEXTCTRL_MODULE_DESCRIPTION = wx.wxNewId()
                ID.TEXTCTRL_MODULE_AUTHOR = wx.wxNewId()
                ID.TEXTCTRL_AUTHOR_EMAIL = wx.wxNewId()
                ID.CHECKBOX_NOARCH = wx.wxNewId()
                ID.CHECKLISTBOX_ARCH_LIST = wx.wxNewId()
                ID.CHECKLISTBOX_MODULE_ASSIGN = wx.wxNewId()
                ID.BUTTON_CREATE = wx.wxNewId()
                ID.BUTTON_MODULE_ASSING_SELECT_ALL = wx.wxNewId()
                ID.BUTTON_MODULE_ASSIGN_DESELECT_ALL = wx.wxNewId()

                ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
                local this = ui.window

                ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)

                ui.StaticBoxSizer_module_name = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Module details")
                ui.FlexGridSizer4 = wx.wxFlexGridSizer(0, 2, 0, 0)
                ui.StaticText1 = wx.wxStaticText(this, wx.wxID_ANY, "Module name", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer4:Add(ui.StaticText1, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.TextCtrl_module_name = wx.wxTextCtrl(this, ID.TEXTCTRL_MODULE_NAME, "", wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE * 0.8, -1))
                ui.FlexGridSizer4:Add(ui.TextCtrl_module_name, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText2 = wx.wxStaticText(this, wx.wxID_ANY, "Description", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer4:Add(ui.StaticText2, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.TextCtrl_module_description = wx.wxTextCtrl(this, ID.TEXTCTRL_MODULE_DESCRIPTION, "", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer4:Add(ui.TextCtrl_module_description, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText3 = wx.wxStaticText(this, wx.wxID_ANY, "Author name", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer4:Add(ui.StaticText3, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.TextCtrl_module_author = wx.wxTextCtrl(this, ID.TEXTCTRL_MODULE_AUTHOR, "", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer4:Add(ui.TextCtrl_module_author, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText4 = wx.wxStaticText(this, wx.wxID_ANY, "Email", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer4:Add(ui.StaticText4, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.TextCtrl_author_email = wx.wxTextCtrl(this, ID.TEXTCTRL_AUTHOR_EMAIL, "", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer4:Add(ui.TextCtrl_author_email, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticBoxSizer_module_name:Add(ui.FlexGridSizer4, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
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
