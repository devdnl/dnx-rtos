--[[============================================================================
@file    new_program.lua

@author  Daniel Zorychta

@brief   Script create a new program by using template file

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
new_program = {}


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
local FILE_IOCTL_MACROS              = config.project.path.ioctl_macros_file:GetValue()
local FILE_XML_CONFIG                = config.project.path.xml_config_file:GetValue()
local FILE_DRIVER_REGISTARTION       = config.project.path.drivers_reg_file:GetValue()

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
function a(event)
end


--==============================================================================
-- GLOBAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function new_program:create_window(parent)
        if ui.window == nil then
                ui = {}
                ID = {}
                ID.PANEL_CREATOR = wx.wxNewId()
                ID.TEXTCTRL_NAME = wx.wxNewId()
                ID.TEXTCTRL_DESCRIPTION = wx.wxNewId()
                ID.TEXTCTRL_AUTHOR = wx.wxNewId()
                ID.TEXTCTRL_EMAIL = wx.wxNewId()
                ID.RADIOBUTTON_LANG_C = wx.wxNewId()
                ID.RADIOBUTTON_LANG_CPP = wx.wxNewId()
                ID.CHECKBOX_CREATE_HEADER = wx.wxNewId()
                ID.BUTTON_CREATE = wx.wxNewId()

                -- create main window
                ui.window = wx.wxScrolledWindow(parent, wx.wxID_ANY)
                ui.FlexGridSizer_creator = wx.wxFlexGridSizer(0, 1, 0, 0)

                -- create main panel
                ui.Panel_creator = wx.wxPanel(ui.window, ID.PANEL_CREATOR, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
                ui.FlexGridSizer_panel = wx.wxFlexGridSizer(0, 1, 0, 0)

                -- create basis group
                ui.StaticBoxSizer_basis = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_creator, "Basis")
                ui.FlexGridSizer_basis_opt = wx.wxFlexGridSizer(0, 2, 0, 0)

                    -- create program name controls
                    ui.StaticText = wx.wxStaticText(ui.Panel_creator, wx.wxID_ANY, "Program name", wx.wxDefaultPosition, wx.wxDefaultSize)
                    ui.FlexGridSizer_basis_opt:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                    ui.TextCtrl_name = wx.wxTextCtrl(ui.Panel_creator, ID.TEXTCTRL_NAME, "", wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE * 0.8, -1), 0, wx.wxDefaultValidator)
                    ui.FlexGridSizer_basis_opt:Add(ui.TextCtrl_name, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                    -- create program description controls
                    ui.StaticText = wx.wxStaticText(ui.Panel_creator, wx.wxID_ANY, "Description", wx.wxDefaultPosition, wx.wxDefaultSize)
                    ui.FlexGridSizer_basis_opt:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                    ui.TextCtrl_description = wx.wxTextCtrl(ui.Panel_creator, ID.TEXTCTRL_DESCRIPTION, "", wx.wxDefaultPosition, wx.wxDefaultSize)
                    ui.FlexGridSizer_basis_opt:Add(ui.TextCtrl_description, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                    -- create author name controls
                    ui.StaticText = wx.wxStaticText(ui.Panel_creator, wx.wxID_ANY, "Author name", wx.wxDefaultPosition, wx.wxDefaultSize)
                    ui.FlexGridSizer_basis_opt:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                    ui.TextCtrl_author = wx.wxTextCtrl(ui.Panel_creator, ID.TEXTCTRL_AUTHOR, "", wx.wxDefaultPosition, wx.wxDefaultSize)
                    ui.FlexGridSizer_basis_opt:Add(ui.TextCtrl_author, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                    -- create author's email controls
                    ui.StaticText = wx.wxStaticText(ui.Panel_creator, wx.wxID_ANY, "Email", wx.wxDefaultPosition, wx.wxDefaultSize)
                    ui.FlexGridSizer_basis_opt:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                    ui.TextCtrl_email = wx.wxTextCtrl(ui.Panel_creator, ID.TEXTCTRL_EMAIL, "", wx.wxDefaultPosition, wx.wxDefaultSize)
                    ui.FlexGridSizer_basis_opt:Add(ui.TextCtrl_email, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                    -- add basis options to the basis sizer
                    ui.StaticBoxSizer_basis:Add(ui.FlexGridSizer_basis_opt, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

                    -- add basis group to the panel's sizer
                    ui.FlexGridSizer_panel:Add(ui.StaticBoxSizer_basis, 0, wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_TOP, 0)

                -- create details group
                ui.StaticBoxSizer_details = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_creator, "Details")
                ui.FlexGridSizer_details_opt = wx.wxFlexGridSizer(0, 1, 0, 0)

                    -- create language sizer
                    ui.FlexGridSizer_lang = wx.wxFlexGridSizer(0, 3, 0, 0)
                    ui.StaticText = wx.wxStaticText(ui.Panel_creator, wx.wxID_ANY, "Language:", wx.wxDefaultPosition, wx.wxDefaultSize)
                    ui.FlexGridSizer_lang:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                    -- add radio button C
                    ui.RadioButton_lang_C = wx.wxRadioButton(ui.Panel_creator, ID.RADIOBUTTON_LANG_C, "C", wx.wxDefaultPosition, wx.wxDefaultSize)
                    ui.FlexGridSizer_lang:Add(ui.RadioButton_lang_C, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                    -- add radio button C++
                    ui.RadioButton_lang_CPP = wx.wxRadioButton(ui.Panel_creator, ID.RADIOBUTTON_LANG_CPP, "C++", wx.wxDefaultPosition, wx.wxDefaultSize)
                    ui.FlexGridSizer_lang:Add(ui.RadioButton_lang_CPP, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                    -- add lang sizer to the details sizer
                    ui.FlexGridSizer_details_opt:Add(ui.FlexGridSizer_lang, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

                    -- add create header checkbox
                    ui.CheckBox_create_header = wx.wxCheckBox(ui.Panel_creator, ID.CHECKBOX_CREATE_HEADER, "Create program's header", wx.wxDefaultPosition, wx.wxDefaultSize)
                    ui.FlexGridSizer_details_opt:Add(ui.CheckBox_create_header, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                    -- add the details options sizer to the details sizer
                    ui.StaticBoxSizer_details:Add(ui.FlexGridSizer_details_opt, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

                    -- add details group to the panel's sizer
                    ui.FlexGridSizer_panel:Add(ui.StaticBoxSizer_details, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- create horizontal line
                ui.StaticLine = wx.wxStaticLine(ui.Panel_creator, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxSize(10,-1), wx.wxLI_HORIZONTAL)
                ui.FlexGridSizer_panel:Add(ui.StaticLine, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add button
                ui.Button_create = wx.wxButton(ui.Panel_creator, ID.BUTTON_CREATE, "Create program", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_panel:Add(ui.Button_create, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- set panel's sizer
                ui.Panel_creator:SetSizer(ui.FlexGridSizer_panel)

                -- fit window
                ui.FlexGridSizer_panel:Fit(ui.Panel_creator)
                ui.FlexGridSizer_panel:SetSizeHints(ui.Panel_creator)

                -- add panel to the creator's sizer
                ui.FlexGridSizer_creator:Add(ui.Panel_creator, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.window:SetSizer(ui.FlexGridSizer_creator)
                ui.FlexGridSizer_creator:Fit(ui.window)
                ui.FlexGridSizer_creator:SetSizeHints(ui.window)

                -- set creator's sizer
                ui.window:SetSizer(ui.FlexGridSizer_creator)
                ui.window:SetScrollRate(5, 5)
        end

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @param  None
-- @return Module name
--------------------------------------------------------------------------------
function new_program:get_window_name()
        return "New program"
end


--------------------------------------------------------------------------------
-- @brief  Function is called when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function new_program:refresh()
end


--------------------------------------------------------------------------------
-- @brief  Function check if options are modified
-- @param  None
-- @return true if options are modified, otherwise false
--------------------------------------------------------------------------------
function new_program:is_modified()
        return false
end


--------------------------------------------------------------------------------
-- @brief  Function save configuration
-- @return None
--------------------------------------------------------------------------------
function new_program:save()
end
