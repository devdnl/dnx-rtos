--[[============================================================================
@file    wizard.lua

@author  Daniel Zorychta

@brief   This file is the main file of wizard

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
require("welcome")
require("project")
require("operating_system")
require("file_systems")
require("network")
require("modules")


--==============================================================================
-- PUBLIC OBJECTS
--==============================================================================


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
-- configuration pages
local page = {welcome, project, operating_system, file_systems, network, modules}

-- container for UI controls
local ui = {}


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
        page[card]:refresh()
        this:Skip()
end


--------------------------------------------------------------------------------
-- @brief  Signal is called when page is changing
-- @param  this          treebook object
-- @return None
--------------------------------------------------------------------------------
local function treebook_page_changing(this)
        local card = this:GetOldSelection() + 1

        if page[card]:is_modified() then
                local answer = wizcore:show_question_msg(wizcore.MAIN_WINDOW_NAME, "Do you want to discard changes?", wx.wxYES_NO)
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

        if page[card]:is_modified() then
                local answer = wizcore:show_question_msg(wizcore.MAIN_WINDOW_NAME, "Do you want to quit and discard changes?", wx.wxYES_NO)
                if answer == wx.wxID_YES then
                        ui.frame:Destroy()
                end
        else
                ui.frame:Destroy()
        end
end


--------------------------------------------------------------------------------
-- @brief  Function create widgets
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function main()
        -- creating controls
        ui.frame = wx.wxFrame(wx.NULL, wx.wxID_ANY, wizcore.MAIN_WINDOW_NAME, wx.wxDefaultPosition, wx.wxSize(wizcore:get_window_size()), bit.bor(wx.wxMINIMIZE_BOX,wx.wxSYSTEM_MENU,wx.wxCAPTION,wx.wxCLOSE_BOX))
        ui.frame:SetMaxSize(wx.wxSize(wizcore:get_window_size()))
        ui.frame:Connect(wx.wxEVT_CLOSE_WINDOW, window_close)
        ui.frame:SetMinSize(wx.wxSize(wizcore:get_window_size()))

        ui.treebook = wx.wxTreebook(ui.frame, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxLB_LEFT)
        for i, page in ipairs(page) do ui.treebook:AddPage(page:create_window(ui.treebook), page:get_window_name())end
        ui.treebook:Connect(wx.wxEVT_COMMAND_TREEBOOK_PAGE_CHANGED, treebook_page_changed)
        ui.treebook:Connect(wx.wxEVT_COMMAND_TREEBOOK_PAGE_CHANGING, treebook_page_changing)

        ui.frame:Show(true)
        wx.wxGetApp():MainLoop()
end

main()
