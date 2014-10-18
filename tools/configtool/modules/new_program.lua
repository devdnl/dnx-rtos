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

local FILE_TEMPLATE_SRC      = config.project.path.program_template_src_file:GetValue()
local FILE_TEMPLATE_HDR      = config.project.path.program_template_hdr_file:GetValue()
local FILE_TEMPLATE_MAKEFILE = config.project.path.program_template_makefile:GetValue()
local DIR_PROGRAMS           = config.project.path.programs_dir:GetValue()

local stack_size = {};
stack_size[0] = "STACK_DEPTH_MINIMAL"
stack_size[1] = "STACK_DEPTH_VERY_LOW"
stack_size[2] = "STACK_DEPTH_LOW"
stack_size[3] = "STACK_DEPTH_MEDIUM"
stack_size[4] = "STACK_DEPTH_LARGE"
stack_size[5] = "STACK_DEPTH_VERY_LARGE"
stack_size[6] = "STACK_DEPTH_HUGE"
stack_size[7] = "STACK_DEPTH_VERY_HUGE"

local name_validator = wx.wxTextValidator(wx.wxFILTER_EXCLUDE_CHAR_LIST)
name_validator:SetExcludes({',', '.', '/', '\\', ';', '~', '`', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '-', '=', '+', '[', ']', '{', '}', ':', '\'', '"', '?', '<', '>'})


--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function create a new program
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function create_program()
        local name        = ui.TextCtrl_name:GetValue()
        local description = ui.TextCtrl_description:GetValue()
        local author      = ui.TextCtrl_author:GetValue()
        local email       = ui.TextCtrl_email:GetValue()
        local stack       = stack_size[ui.Choice_stack:GetSelection()]
        local C_lang      = ui.RadioButton_lang_C:GetValue()
        local header      = ui.CheckBox_create_header:GetValue()

        -- check if program's name is set
        if name == "" then
                ct:show_info_msg(ct.MAIN_WINDOW_NAME, "The program name field is empty. Please, put program's name and try again.", ui.window)
                return
        end

        -- check if program exists
        if ct.fs:exists(DIR_PROGRAMS.."/"..name) then
                ct:show_info_msg(ct.MAIN_WINDOW_NAME, "The program named '"..name.."' already exists. Please, change the program name.", ui.window)
                return
        end

        -- checks if project is not read only
        if not ct.fs:mkdir("_test_") then
                ct:show_info_msg(ct.MAIN_WINDOW_NAME, "Project seems to be a read only.", ui.window)
                return
        else
                ct.fs:remove("_test_")
        end

        -- create tags
        local tags = {
                {tag = "<!path_program_name_c!>", to = ifs(C_lang, name.."/"..name..".c", "")},
                {tag = "<!path_program_name_cpp!>", to = ifs(not C_lang, name.."/"..name..".cpp", "")},
                {tag = "<!program_name!>", to = name},
                {tag = "<!PROGRAM_NAME!>", to = name:upper()},
                {tag = "<!file_extension!>", to = ifs(C_lang, "c", "cpp")},
                {tag = "<!author!>", to = author},
                {tag = "<!program_description!>", to = description},
                {tag = "<!year!>", to = os.date("%Y")},
                {tag = "<!email!>", to = email},
                {tag = "<!program_header!>", to = ifs(header, "#include \""..name..".h\"", "")},
                {tag = "<!global_variables_in_src!>", to = ifs(not header, "GLOBAL_VARIABLES {\n};", "")},
                {tag = "<!stack_size!>", to = stack},
        }

        -- create program's directory
        ct.fs:mkdir(DIR_PROGRAMS.."/"..name)

        -- create program's Makefile
        ct:apply_template(FILE_TEMPLATE_MAKEFILE, DIR_PROGRAMS.."/"..name.."/Makefile", tags)

        -- create program's source file
        local filename = DIR_PROGRAMS.."/"..name.."/"..name.."."..ifs(C_lang, "c", "cpp")
        ct:apply_template(FILE_TEMPLATE_SRC, filename, tags)

        -- create program's header file
        if header then
                local filename = DIR_PROGRAMS.."/"..name.."/"..name..".h"
                ct:apply_template(FILE_TEMPLATE_HDR, filename, tags)
        end

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
function new_program:create_window(parent)
        if ui.window == nil then
                ui = {}
                ID = {}
                ID.PANEL_CREATOR = wx.wxNewId()
                ID.TEXTCTRL_NAME = wx.wxNewId()
                ID.TEXTCTRL_DESCRIPTION = wx.wxNewId()
                ID.TEXTCTRL_AUTHOR = wx.wxNewId()
                ID.TEXTCTRL_EMAIL = wx.wxNewId()
                ID.CHOICE_STACK = wx.wxNewId()
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
                    ui.TextCtrl_name = wx.wxTextCtrl(ui.Panel_creator, ID.TEXTCTRL_NAME, "", wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE * 0.8, -1), 0, name_validator)
                    ui.TextCtrl_name:SetToolTip("This is the name of a new program. The name of program must be unique.")
                    ui.FlexGridSizer_basis_opt:Add(ui.TextCtrl_name, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                    -- create program description controls
                    ui.StaticText = wx.wxStaticText(ui.Panel_creator, wx.wxID_ANY, "Description", wx.wxDefaultPosition, wx.wxDefaultSize)
                    ui.FlexGridSizer_basis_opt:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                    ui.TextCtrl_description = wx.wxTextCtrl(ui.Panel_creator, ID.TEXTCTRL_DESCRIPTION, "", wx.wxDefaultPosition, wx.wxDefaultSize)
                    ui.TextCtrl_description:SetToolTip("Enter here a short description of program's purpose.")
                    ui.FlexGridSizer_basis_opt:Add(ui.TextCtrl_description, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                    -- create author name controls
                    ui.StaticText = wx.wxStaticText(ui.Panel_creator, wx.wxID_ANY, "Author name", wx.wxDefaultPosition, wx.wxDefaultSize)
                    ui.FlexGridSizer_basis_opt:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                    ui.TextCtrl_author = wx.wxTextCtrl(ui.Panel_creator, ID.TEXTCTRL_AUTHOR, "", wx.wxDefaultPosition, wx.wxDefaultSize)
                    ui.TextCtrl_author:SetToolTip("Enter here your name.")
                    ui.FlexGridSizer_basis_opt:Add(ui.TextCtrl_author, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                    -- create author's email controls
                    ui.StaticText = wx.wxStaticText(ui.Panel_creator, wx.wxID_ANY, "E-mail", wx.wxDefaultPosition, wx.wxDefaultSize)
                    ui.FlexGridSizer_basis_opt:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                    ui.TextCtrl_email = wx.wxTextCtrl(ui.Panel_creator, ID.TEXTCTRL_EMAIL, "", wx.wxDefaultPosition, wx.wxDefaultSize)
                    ui.TextCtrl_email:SetToolTip("Enter here your e-mail address.")
                    ui.FlexGridSizer_basis_opt:Add(ui.TextCtrl_email, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                    -- add basis options to the basis sizer
                    ui.StaticBoxSizer_basis:Add(ui.FlexGridSizer_basis_opt, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

                    -- add basis group to the panel's sizer
                    ui.FlexGridSizer_panel:Add(ui.StaticBoxSizer_basis, 0, wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_TOP, 0)


                -- create details group
                ui.StaticBoxSizer_details = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_creator, "Details")
                ui.FlexGridSizer_details_opt = wx.wxFlexGridSizer(0, 1, 0, 0)

                    -- create stack size sizer
                    ui.FlexGridSizer_stack = wx.wxFlexGridSizer(0, 2, 0, 0)
                    ui.StaticText = wx.wxStaticText(ui.Panel_creator, wx.wxID_ANY, "Stack size:", wx.wxDefaultPosition, wx.wxDefaultSize)
                    ui.FlexGridSizer_stack:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                        -- add stack size choice
                        ui.Choice_stack = wx.wxChoice(ui.Panel_creator, ID.CHOICE_STACK, wx.wxDefaultPosition, wx.wxDefaultSize, {})
                        ui.Choice_stack:Append("Minimal")
                        ui.Choice_stack:Append("Very low")
                        ui.Choice_stack:Append("Low")
                        ui.Choice_stack:Append("Medium")
                        ui.Choice_stack:Append("Large")
                        ui.Choice_stack:Append("Very large")
                        ui.Choice_stack:Append("Huge")
                        ui.Choice_stack:Append("Very huge")
                        ui.Choice_stack:SetSelection(2)
                        ui.Choice_stack:SetToolTip("Select the stack size of your program. This value can be changed later in the program's main function.")
                        ui.FlexGridSizer_stack:Add(ui.Choice_stack, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                        -- add stack size sizer to the details sizer
                        ui.FlexGridSizer_details_opt:Add(ui.FlexGridSizer_stack, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)


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
                    ui.CheckBox_create_header:SetToolTip("If option is selected the program's header is created. "..
                                                         "In the header there is the structure that describes program's global variables. "..
                                                         "The main purpose of this header is to provides program's global variables to all source files.")
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
                ui.window:Connect(ID.BUTTON_CREATE, wx.wxEVT_COMMAND_BUTTON_CLICKED, create_program)

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
