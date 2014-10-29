--[[============================================================================
@file    startup.lua

@author  Daniel Zorychta

@brief   The startup configuration script.

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
startup  = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local ui = {}
local ID = {}
local modified = ct:new_modify_indicator()

--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function loads all controls from configuration files
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_controls()

end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function save_configuration()
        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Create panel with boot runlevel
-- @param  parent       parent window
-- @return Panel object
--------------------------------------------------------------------------------
local function create_boot_widgets(parent)
        -- create boot panel
        ui.Panel_boot = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_boot = wx.wxFlexGridSizer(0, 1, 0, 0)

        -- boot information
        ui.StaticText = wx.wxStaticText(ui.Panel_boot, wx.wxID_ANY, "The purpose of this runlevel is to create a basic file system environment.", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_boot:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        ui.StaticText = wx.wxStaticText(ui.Panel_boot, wx.wxID_ANY, "NOTE: some file systems require a source files. In this level may not exist.", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_boot:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        ui.StaticLine = wx.wxStaticLine(ui.Panel_boot, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxSize(10,-1), wx.wxLI_HORIZONTAL, "wx.wxID_ANY")
        ui.FlexGridSizer_boot:Add(ui.StaticLine, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- Root file system selection group
        ui.StaticBoxSizer_boot_main_FS_0 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_boot, "Root file system (\"/\")")
        ui.FlexGridSizer_boot_main_FS_1 = wx.wxFlexGridSizer(0, 2, 0, 0)

            ui.StaticText1 = wx.wxStaticText(ui.Panel_boot, wx.wxID_ANY, "Select base file system", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_boot_main_FS_1:Add(ui.StaticText1, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            ui.Choice_boot_root_FS = wx.wxChoice(ui.Panel_boot, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
            ui.FlexGridSizer_boot_main_FS_1:Add(ui.Choice_boot_root_FS, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            ui.StaticBoxSizer_boot_main_FS_0:Add(ui.FlexGridSizer_boot_main_FS_1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_boot:Add(ui.StaticBoxSizer_boot_main_FS_0, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- folders group
        ui.StaticBoxSizer_boot_folders_0 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_boot, "Folders")
        ui.FlexGridSizer_boot_folders_1 = wx.wxFlexGridSizer(0, 1, 0, 0)

            -- new folder sizer
            ui.FlexGridSizer_boot_folders_2 = wx.wxFlexGridSizer(0, 5, 0, 0)

                -- folder name combobox
                ui.ComboBox_folder_name = wx.wxComboBox(ui.Panel_boot, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, {})
                ui.FlexGridSizer_boot_folders_2:Add(ui.ComboBox_folder_name, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add button
                ui.Button_folder_add = wx.wxButton(ui.Panel_boot, wx.wxNewId(), "Add", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_boot_folders_2:Add(ui.Button_folder_add, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- seperator
                ui.StaticLine = wx.wxStaticLine(ui.Panel_boot, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxLI_VERTICAL)
                ui.FlexGridSizer_boot_folders_2:Add(ui.StaticLine, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- remove button
                ui.Button_folder_remove = wx.wxButton(ui.Panel_boot, wx.wxID_ANY, "Remove selected", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_boot_folders_2:Add(ui.Button_folder_remove, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add new folder sizer to folder group
                ui.FlexGridSizer_boot_folders_1:Add(ui.FlexGridSizer_boot_folders_2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

            -- add folder list
            ui.ListBox_folders = wx.wxListBox(ui.Panel_boot, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(-1,110), {}, 0)
            ui.FlexGridSizer_boot_folders_1:Add(ui.ListBox_folders, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- add sizers
            ui.StaticBoxSizer_boot_folders_0:Add(ui.FlexGridSizer_boot_folders_1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_boot:Add(ui.StaticBoxSizer_boot_folders_0, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- Additional file systems group
        ui.StaticBoxSizer_other_FS_0 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_boot, "Additional file systems")
        ui.FlexGridSizer_other_FS_1 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.FlexGridSizer_other_FS_2 = wx.wxFlexGridSizer(0, 3, 0, 0)

            -- colums descriptions
            ui.StaticText = wx.wxStaticText(ui.Panel_boot, wx.wxID_ANY, "File system", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_other_FS_2:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.StaticText = wx.wxStaticText(ui.Panel_boot, wx.wxID_ANY, "Source file", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_other_FS_2:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.StaticText = wx.wxStaticText(ui.Panel_boot, wx.wxID_ANY, "Mount point", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_other_FS_2:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- file system name selection
            ui.Choice_other_FS_name = wx.wxChoice(ui.Panel_boot, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
            ui.FlexGridSizer_other_FS_2:Add(ui.Choice_other_FS_name, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- source file combobox
            ui.ComboBox_other_FS_src = wx.wxComboBox(ui.Panel_boot, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, {})
            ui.FlexGridSizer_other_FS_2:Add(ui.ComboBox_other_FS_src, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- FS mount point
            ui.ComboBox_other_FS_mntpt = wx.wxComboBox(ui.Panel_boot, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, {})
            ui.FlexGridSizer_other_FS_2:Add(ui.ComboBox_other_FS_mntpt, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add sizers
                ui.FlexGridSizer_other_FS_1:Add(ui.FlexGridSizer_other_FS_2, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

            -- add buttons
            ui.Button_other_FS_add = wx.wxButton(ui.Panel_boot, wx.wxNewId(), "Add", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_other_FS_1:Add(ui.Button_other_FS_add, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- folder list
            ui.ListView_other_FS = wx.wxListView(ui.Panel_boot, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(-1, 150), wx.wxLC_REPORT)
            ui.ListView_other_FS:InsertColumn(0, "File system", wx.wxLIST_FORMAT_LEFT, 150)
            ui.ListView_other_FS:InsertColumn(1, "Source file", wx.wxLIST_FORMAT_LEFT, 150)
            ui.ListView_other_FS:InsertColumn(2, "Mount point", wx.wxLIST_FORMAT_LEFT, 150)

            ui.ListView_other_FS:InsertItem(wx.wxListItem())
            ui.ListView_other_FS:SetItem(0, 0, "lfs")
            ui.ListView_other_FS:SetItem(0, 1, "none")
            ui.ListView_other_FS:SetItem(0, 2, "/")


            print(ui.ListView_other_FS:GetColumnCount())

            ui.FlexGridSizer_other_FS_1:Add(ui.ListView_other_FS, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- add remove button
            ui.Button_other_FS_remove = wx.wxButton(ui.Panel_boot, wx.wxNewId(), "Remove selected", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_other_FS_1:Add(ui.Button_other_FS_remove, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- add group
            ui.StaticBoxSizer_other_FS_0:Add(ui.FlexGridSizer_other_FS_1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_boot:Add(ui.StaticBoxSizer_other_FS_0, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- set panel sizer
        ui.Panel_boot:SetSizer(ui.FlexGridSizer_boot)

        return ui.Panel_boot
end

--==============================================================================
-- GLOBAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function startup:create_window(parent)
        if ui.window == nil then
                -- create main window
                ui.window = wx.wxScrolledWindow(parent, wx.wxID_ANY)
                ui.FlexGridSizer_main = wx.wxFlexGridSizer(0, 1, 0, 0)

                -- create main notebook
                ui.Notebook_runlevels = wx.wxNotebook(ui.window, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.Notebook_runlevels:AddPage(create_boot_widgets(ui.Notebook_runlevels), "Runlevel boot", false)
                ui.FlexGridSizer_main:Add(ui.Notebook_runlevels, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- set main sizers
                ui.window:SetSizer(ui.FlexGridSizer_main)
                ui.window:SetScrollRate(5, 5)
        end

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @param  None
-- @return Module name
--------------------------------------------------------------------------------
function startup:get_window_name()
        return "System startup"
end


--------------------------------------------------------------------------------
-- @brief  Function is called when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function startup:refresh()
        -- load configuration
        load_controls()
end


--------------------------------------------------------------------------------
-- @brief  Function check if options are modified
-- @param  None
-- @return true if options are modified, otherwise false
--------------------------------------------------------------------------------
function startup:is_modified()
        return modified:get_value()
end


--------------------------------------------------------------------------------
-- @brief  Function save configuration
-- @return None
--------------------------------------------------------------------------------
function startup:save()
        save_configuration()
end
