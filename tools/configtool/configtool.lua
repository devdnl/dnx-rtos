--[[============================================================================
@file    configtool.lua

@author  Daniel Zorychta

@brief   This file is the main file of the configuration tool

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

os.setlocale('C')

--==============================================================================
-- EXTERNAL MODULES
--==============================================================================
require("wx")
require("modules/ctcore")
require("modules/welcome")
require("modules/configinfo")
require("modules/project")
require("modules/operating_system")
require("modules/file_systems")
require("modules/network")
require("modules/modules")
require("modules/about")
require("modules/creators")
require("modules/new_module")


--==============================================================================
-- PUBLIC OBJECTS
--==============================================================================


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
-- configuration pages
local page = {
    {form = welcome,            subpage = false},
    {form = configinfo,         subpage = false};
    {form = project,            subpage = true },
    {form = operating_system,   subpage = true },
    {form = file_systems,       subpage = true },
    {form = network,            subpage = true },
    {form = modules,            subpage = true },
    {form = creators,           subpage = false},
    {form = new_module,         subpage = true },
    {form = about,              subpage = false},
}

-- container for UI controls
local ui = {}
local ID = {}
ID.MENU_SAVE      = wx.wxID_SAVE
ID.MENU_IMPORT    = wx.wxID_OPEN
ID.MENU_EXPORT    = wx.wxID_SAVEAS
ID.TOOLBAR_SAVE   = wx.wxNewId()
ID.TOOLBAR_IMPORT = wx.wxNewId()
ID.TOOLBAR_EXPORT = wx.wxNewId()

--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Signal is called when page was changed
-- @param  this          treebook object
-- @return None
--------------------------------------------------------------------------------
local function treebook_page_changed(this)
        local card = this:GetSelection() + 1
        page[card].form:refresh()
        this:Skip()
end


--------------------------------------------------------------------------------
-- @brief  Signal is called when page is changing
-- @param  event          treebook event
-- @return None
--------------------------------------------------------------------------------
local function treebook_page_changing(event)
        local card = event:GetOldSelection() + 1

        if page[card].form:is_modified() then
                local answer = ct:show_question_msg(ct.MAIN_WINDOW_NAME, "There are modified not saved settings.\n\nDo you want to save the changes?", bit.bor(wx.wxYES_NO,wx.wxCANCEL), ui.frame)
                if answer == wx.wxID_CANCEL then
                        event:Veto()
                elseif answer == wx.wxID_YES then
                        page[card].form:save()
                end
        end
end


--------------------------------------------------------------------------------
-- @brief  Signal is called when main window is closing
-- @param  event
-- @return None
--------------------------------------------------------------------------------
local function window_close(event)
        local card = ui.treebook:GetSelection() + 1

        if page[card].form:is_modified() then
                local answer = ct:show_question_msg(ct.MAIN_WINDOW_NAME, "The configuration has changed.\n\nDo you want to save the changes?", bit.bor(wx.wxYES_NO,wx.wxCANCEL), ui.frame)
                if answer == wx.wxID_CANCEL then
                        event:Veto()
                        return
                elseif answer == wx.wxID_YES then
                        page[card].form:save()
                end
        end

        ui.frame:Destroy()
end


--------------------------------------------------------------------------------
-- @brief  Signal is called when menu's save item is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function event_save_configuration()
        local card = ui.treebook:GetSelection() + 1

        if page[card].form:is_modified() then
                page[card].form:save()
                toolBar:EnableTool(ID.TOOLBAR_SAVE, false)
                ui.frame:SetStatusText("Configuration saved")
        end
end


--------------------------------------------------------------------------------
-- @brief  Signal is called when menu's import item is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function event_import_configuration()
        ui.treebook:SetSelection(0)
        if ui.treebook:GetSelection() == 0 then

                dialog = wx.wxFileDialog(ui.frame,
                                         "Import configuration file",
                                         config.project.path.bsp_dir:GetValue(),
                                         "",
                                         "dnx RTOS configuration files (*.dnxc)|*.dnxc",
                                         bit.bor(wx.wxFD_OPEN, wx.wxFD_FILE_MUST_EXIST))

                if (dialog:ShowModal() == wx.wxID_OK) then
                        ct:apply_project_configuration(dialog:GetPath(), ui.frame)
                end
        end
end


--------------------------------------------------------------------------------
-- @brief  Signal is called when menu's export item is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function event_export_configuration()
        dialog = wx.wxFileDialog(ui.frame, "Export configuration file", config.project.path.bsp_dir:GetValue(), "", "dnx RTOS configuration files (*.dnxc)|*.dnxc", bit.bor(wx.wxFD_SAVE, wx.wxFD_OVERWRITE_PROMPT))
        if (dialog:ShowModal() == wx.wxID_OK) then
                ct:save_project_configuration(dialog:GetPath(), ui.frame)
        end
end


--------------------------------------------------------------------------------
-- @brief  Signal is called when menu's export item is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function event_configuration_modified(modified)
        toolBar:EnableTool(ID.TOOLBAR_SAVE, modified)

        if modified then
                ui.frame:SetStatusText("Configuration is modified")
        else
                ui.frame:SetStatusText("")
        end
end

--------------------------------------------------------------------------------
-- @brief  Function create widgets
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function main()
        -- create main frame
        ui.frame = wx.wxFrame(wx.NULL, wx.wxID_ANY, ct.MAIN_WINDOW_NAME, wx.wxDefaultPosition, wx.wxSize(ct:get_window_size()))
        ui.frame:Connect(wx.wxEVT_CLOSE_WINDOW, window_close)

        -- create Configuration menu
        cfg_menu = wx.wxMenu()
        cfg_menu:Append(ID.MENU_SAVE, "&Save", "Save currently selected configuration")
        cfg_menu:Append(ID.MENU_IMPORT, "&Import", "Import configuration from file")
        cfg_menu:Append(ID.MENU_EXPORT, "&Export", "Export configuration to file")

        -- create menubar and add menus
        menubar = wx.wxMenuBar()
        menubar:Append(cfg_menu, "&Configuration")
        ui.frame:SetMenuBar(menubar)

        -- create toolbar with options
        toolBar = ui.frame:CreateToolBar(wx.wxTB_TEXT)
        toolBar:SetToolBitmapSize(wx.wxSize(22, 22))
        local toolBmpSize = toolBar:GetToolBitmapSize()
        toolBar:AddTool(ID.TOOLBAR_SAVE, "Save", wx.wxArtProvider.GetBitmap(wx.wxART_FILE_SAVE, wx.wxART_MENU, toolBmpSize), "Save configuration")
        toolBar:AddSeparator()
        toolBar:AddTool(ID.TOOLBAR_IMPORT, "Import", wx.wxArtProvider.GetBitmap(wx.wxART_FILE_OPEN, wx.wxART_MENU, toolBmpSize), "Import configuration from the file")
        toolBar:AddTool(ID.TOOLBAR_EXPORT, "Export", wx.wxArtProvider.GetBitmap(wx.wxART_FILE_SAVE_AS, wx.wxART_MENU, toolBmpSize), "Export configuration to the file")
        toolBar:Realize()
        toolBar:EnableTool(ID.TOOLBAR_SAVE, false)

        statusBar = ui.frame:CreateStatusBar(1)
        ui.frame:SetStatusText("Welcome to dnx RTOS Configtool")


        -- create treebook view
        ui.treebook = wx.wxTreebook(ui.frame, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxLB_LEFT)

        -- add configuration pages
        for i, page in ipairs(page) do
                if page.subpage then
                        ui.treebook:AddSubPage(page.form:create_window(ui.treebook), page.form:get_window_name())
                else
                        ui.treebook:AddPage(page.form:create_window(ui.treebook), page.form:get_window_name())
                end
        end

        -- expand all pages
        for i = 0, ui.treebook:GetPageCount() do
                ui.treebook:ExpandNode(i)
        end

        -- connect signals
        ui.treebook:Connect(wx.wxEVT_COMMAND_TREEBOOK_PAGE_CHANGED, treebook_page_changed)
        ui.treebook:Connect(wx.wxEVT_COMMAND_TREEBOOK_PAGE_CHANGING, treebook_page_changing)

        ui.frame:Connect(ID.MENU_SAVE,   wx.wxEVT_COMMAND_MENU_SELECTED, event_save_configuration)
        ui.frame:Connect(ID.MENU_IMPORT, wx.wxEVT_COMMAND_MENU_SELECTED, event_import_configuration)
        ui.frame:Connect(ID.MENU_EXPORT, wx.wxEVT_COMMAND_MENU_SELECTED, event_export_configuration)
        ui.frame:Connect(ID.TOOLBAR_SAVE, wx.wxEVT_COMMAND_MENU_SELECTED, event_save_configuration)
        ui.frame:Connect(ID.TOOLBAR_IMPORT, wx.wxEVT_COMMAND_MENU_SELECTED, event_import_configuration)
        ui.frame:Connect(ID.TOOLBAR_EXPORT, wx.wxEVT_COMMAND_MENU_SELECTED, event_export_configuration)

        -- set modification function event
        ct:set_modify_event_function(event_configuration_modified)

        -- show created window
        ui.frame:Show(true)
        wx.wxGetApp():MainLoop()
end

main()
