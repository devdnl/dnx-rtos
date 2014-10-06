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
ID.IMPORT_CFG = wx.wxNewId()
ID.EXPORT_CFG = wx.wxNewId()

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
-- @param  this          treebook object
-- @return None
--------------------------------------------------------------------------------
local function treebook_page_changing(this)
        local card = this:GetOldSelection() + 1

        if page[card].form:is_modified() then
                local answer = ct:show_question_msg(ct.MAIN_WINDOW_NAME, "There are modified not saved settings.\nDo you want to discard changes?", wx.wxYES_NO, ui.frame)
                if answer == wx.wxID_NO then
                        this:Veto()
                end
        end
end


--------------------------------------------------------------------------------
-- @brief  Signal is called when main window is closing
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function window_close()
        local card = ui.treebook:GetSelection() + 1

        if page[card].form:is_modified() then
                local answer = ct:show_question_msg(ct.MAIN_WINDOW_NAME, "Do you want to quit and discard changes?", wx.wxYES_NO, ui.frame)
                if answer == wx.wxID_YES then
                        ui.frame:Destroy()
                end
        else
                ui.frame:Destroy()
        end
end


--------------------------------------------------------------------------------
-- @brief  Signal is called when menu's import item is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function event_import_configuration()
        dialog = wx.wxFileDialog(ui.frame, "Import configuration file", "", "", "dnx RTOS configuration files (*.dnxc)|*.dnxc", bit.bor(wx.wxFD_OPEN, wx.wxFD_FILE_MUST_EXIST))
        if (dialog:ShowModal() == wx.wxID_CANCEL) then
                return
        else
                print("import: "..dialog:GetPath())
        end
        
        ui.treebook:SetSelection(0)
end


--------------------------------------------------------------------------------
-- @brief  Signal is called when menu's export item is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function event_export_configuration()
        dialog = wx.wxFileDialog(ui.frame, "Export configuration file", "", "", "dnx RTOS configuration files (*.dnxc)|*.dnxc", bit.bor(wx.wxFD_SAVE))
        if (dialog:ShowModal() == wx.wxID_CANCEL) then
                return
        else
                print("export: "..dialog:GetPath())
        end
end

--------------------------------------------------------------------------------
-- @brief  Function create widgets
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function main()
        ui.frame = wx.wxFrame(wx.NULL, wx.wxID_ANY, ct.MAIN_WINDOW_NAME, wx.wxDefaultPosition, wx.wxSize(ct:get_window_size()), bit.bor(wx.wxMINIMIZE_BOX,wx.wxSYSTEM_MENU,wx.wxCAPTION,wx.wxCLOSE_BOX))
        ui.frame:SetMaxSize(wx.wxSize(ct:get_window_size()))
        ui.frame:Connect(wx.wxEVT_CLOSE_WINDOW, window_close)
        ui.frame:SetMinSize(wx.wxSize(ct:get_window_size()))
        
        menubar  = wx.wxMenuBar()
        cfg_menu = wx.wxMenu()
        cfg_menu:Append(ID.IMPORT_CFG, "&Import", "Import configuration from file")
        cfg_menu:Append(ID.EXPORT_CFG, "&Export", "Export configuration to file")
        menubar:Append(cfg_menu, "&Configuration")
        
        ui.frame:SetMenuBar(menubar)       
        
        ui.treebook = wx.wxTreebook(ui.frame, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxLB_LEFT)

        for i, page in ipairs(page) do
                if page.subpage then
                        ui.treebook:AddSubPage(page.form:create_window(ui.treebook), page.form:get_window_name())
                else
                        ui.treebook:AddPage(page.form:create_window(ui.treebook), page.form:get_window_name())
                end
        end

        for i = 0, ui.treebook:GetPageCount() do
                ui.treebook:ExpandNode(i)
        end

        ui.treebook:Connect(wx.wxEVT_COMMAND_TREEBOOK_PAGE_CHANGED, treebook_page_changed)
        ui.treebook:Connect(wx.wxEVT_COMMAND_TREEBOOK_PAGE_CHANGING, treebook_page_changing)
        
        ui.frame:Connect(ID.IMPORT_CFG, wx.wxEVT_COMMAND_MENU_SELECTED, event_import_configuration)
        ui.frame:Connect(ID.EXPORT_CFG, wx.wxEVT_COMMAND_MENU_SELECTED, event_export_configuration)

        ui.frame:Show(true)
        wx.wxGetApp():MainLoop()
end

main()
