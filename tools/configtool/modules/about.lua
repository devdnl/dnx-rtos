--[[============================================================================
@file    about.lua

@author  Daniel Zorychta

@brief   About dialog.

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
about = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local ui = {}

local programmers = "Daniel Zorychta <daniel.zorychta@gmail.com>"

local testers = "Paweł Radecki\n"..
                "Artur Bałut"


--==============================================================================
-- GLOBAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Show credits dialog
-- @param  parent       parent window
-- @return None
--------------------------------------------------------------------------------
local function show_credits_dialog(parent)
        local ui = {}
        local ID = {}
        ID.BUTTON_CLOSE = wx.wxNewId()

        -- create license dialog
        ui.window = wx.wxDialog(parent, wx.wxID_ANY, "Credits", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)

        -- create notebook to pages
        ui.Notebook = wx.wxNotebook(ui.window, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxDefaultSize)

        -- create program page
        ui.Panel_program = wx.wxPanel(ui.Notebook, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer2 = wx.wxFlexGridSizer(0, 3, 0, 0)

            -- insert names of persons which create program
            ui.TextCtrl = wx.wxTextCtrl(ui.Panel_program, wx.wxID_ANY, programmers, wx.wxDefaultPosition, wx.wxSize(400,300), wx.wxTE_MULTILINE+wx.wxTE_READONLY)
            ui.FlexGridSizer2:Add(ui.TextCtrl, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.Panel_program:SetSizer(ui.FlexGridSizer2)
            ui.FlexGridSizer2:Fit(ui.Panel_program)
            ui.FlexGridSizer2:SetSizeHints(ui.Panel_program)

        -- create testers page
        ui.Panel_testers = wx.wxPanel(ui.Notebook, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer3 = wx.wxFlexGridSizer(0, 3, 0, 0)

            -- insert names of testers
            ui.TextCtrl = wx.wxTextCtrl(ui.Panel_testers, wx.wxID_ANY, testers, wx.wxDefaultPosition, wx.wxSize(400,300), wx.wxTE_MULTILINE+wx.wxTE_READONLY)
            ui.FlexGridSizer3:Add(ui.TextCtrl, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.Panel_testers:SetSizer(ui.FlexGridSizer3)
            ui.FlexGridSizer3:Fit(ui.Panel_testers)
            ui.FlexGridSizer3:SetSizeHints(ui.Panel_testers)

        -- add pages to the notebook
        ui.Notebook:AddPage(ui.Panel_program, "Program", false)
        ui.Notebook:AddPage(ui.Panel_testers, "Tests", false)

        -- add notebook to the sizer
        ui.FlexGridSizer1:Add(ui.Notebook, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- add close button
        ui.Button_close = wx.wxButton(ui.window, ID.BUTTON_CLOSE, "Close", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer1:Add(ui.Button_close, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.window:Connect(ID.BUTTON_CLOSE, wx.wxEVT_COMMAND_BUTTON_CLICKED, function() ui.window:Destroy() end)

        -- set sizer
        ui.window:SetSizer(ui.FlexGridSizer1)
        ui.FlexGridSizer1:Fit(ui.window)
        ui.FlexGridSizer1:SetSizeHints(ui.window)

        -- show window
        ui.window:ShowModal()
end

--------------------------------------------------------------------------------
-- @brief  Show license dialog
-- @param  parent       parent window
-- @return None
--------------------------------------------------------------------------------
local function show_licence_dialog(parent)
        local ui = {}
        local ID = {}
        ID.BUTTON_CLOSE = wx.wxNewId()

        -- create license dialog
        ui.window = wx.wxDialog(parent, wx.wxID_ANY, "License", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)

        -- add textctrl with license
        ui.TextCtrl = wx.wxTextCtrl(ui.window, wx.wxID_ANY, "Configtool\nCopyright © 2014 Daniel Zorychta\n\nThis program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the licence, or (at your option) any later version.\n\nThis program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License along with this program.  You may also obtain a copy of the GNU General Public License from the Free Software Foundation by visiting their web site (http://www.fsf.org/) or by writing to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA\n", wx.wxDefaultPosition, wx.wxSize(400, 300), wx.wxTE_MULTILINE+wx.wxTE_AUTO_URL)
        ui.FlexGridSizer1:Add(ui.TextCtrl, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- add close button
        ui.Button_close = wx.wxButton(ui.window, ID.BUTTON_CLOSE, "Close", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer1:Add(ui.Button_close, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.window:Connect(ID.BUTTON_CLOSE, wx.wxEVT_COMMAND_BUTTON_CLICKED, function() ui.window:Destroy() end)

        -- set sizer
        ui.window:SetSizer(ui.FlexGridSizer1)
        ui.FlexGridSizer1:Fit(ui.window)
        ui.FlexGridSizer1:SetSizeHints(ui.window)

        -- show window
        ui.window:ShowModal()
end

--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function about:show(parent)
                local font

                ID = {}
                ID.HYPERLINKCTRL_WEBSITE = wx.wxNewId()
                ID.BUTTON_CREDITS        = wx.wxNewId()
                ID.BUTTON_LICENSE        = wx.wxNewId()
                ID.BUTTON_CLOSE          = wx.wxNewId()

                -- create dialog
                ui.window = wx.wxDialog(parent, wx.wxID_ANY, "About Configtool", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)

                -- add program name string
                ui.StaticText = wx.wxStaticText(ui.window, wx.wxID_ANY, "Configtool", wx.wxDefaultPosition, wx.wxDefaultSize)
                font = ui.StaticText:GetFont()
                font:SetPointSize(font:GetPointSize() + 8)
                font:SetWeight(wx.wxFONTWEIGHT_BOLD)
                ui.StaticText:SetFont(font)
                ui.FlexGridSizer1:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add short program description
                ui.StaticText = wx.wxStaticText(ui.window, wx.wxID_ANY, "Configtool is a graphical tool for configure the dnx RTOS project.", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer1:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add copyright notice
                ui.StaticText = wx.wxStaticText(ui.window, wx.wxID_ANY, "Copyright © 2014 Daniel Zorychta", wx.wxDefaultPosition, wx.wxDefaultSize)
                font = ui.StaticText:GetFont()
                font:SetPointSize(font:GetPointSize() - 1)
                ui.StaticText:SetFont(font)
                ui.FlexGridSizer1:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add project website link
                ui.HyperlinkCtrl_website = wx.wxHyperlinkCtrl(ui.window, ID.HYPERLINKCTRL_WEBSITE, "http://www.dnx-rtos.org/", "http://www.dnx-rtos.org/", wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxHL_CONTEXTMENU+wx.wxHL_ALIGN_CENTRE+wx.wxNO_BORDER)
                ui.FlexGridSizer1:Add(ui.HyperlinkCtrl_website, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add sizer that contains buttons
                ui.BoxSizer1 = wx.wxBoxSizer(wx.wxHORIZONTAL)

                -- add credits button
                ui.Button_credits = wx.wxButton(ui.window, ID.BUTTON_CREDITS, "Credits", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.BoxSizer1:Add(ui.Button_credits, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.window:Connect(ID.BUTTON_CREDITS, wx.wxEVT_COMMAND_BUTTON_CLICKED, function() show_credits_dialog(ui.window) end)

                -- add license button
                ui.Button_license = wx.wxButton(ui.window, ID.BUTTON_LICENSE, "License", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.BoxSizer1:Add(ui.Button_license, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.window:Connect(ID.BUTTON_LICENSE, wx.wxEVT_COMMAND_BUTTON_CLICKED, function() show_licence_dialog(ui.window) end)

                -- add separator
                ui.BoxSizer1:Add(0,0,1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add close button
                ui.Button_close = wx.wxButton(ui.window, ID.BUTTON_CLOSE, "Close", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.BoxSizer1:Add(ui.Button_close, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.window:Connect(ID.BUTTON_CLOSE, wx.wxEVT_COMMAND_BUTTON_CLICKED, function() ui.window:Destroy() end)

                -- add button sizer to main sizer
                ui.FlexGridSizer1:Add(ui.BoxSizer1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- fit window
                ui.FlexGridSizer1:Fit(ui.window)
                ui.FlexGridSizer1:SetSizeHints(ui.window)

                -- set main sizer
                ui.window:SetSizer(ui.FlexGridSizer1)

                -- show window
                ui.window:ShowModal()
end
