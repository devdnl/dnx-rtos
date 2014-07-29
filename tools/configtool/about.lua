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
require("ctcore")


--==============================================================================
-- GLOBAL OBJECTS
--==============================================================================
about = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local ui = {}


--==============================================================================
-- GLOBAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function about:create_window(parent)
        if ui.window == nil then
                ui.window  = wx.wxPanel(parent, wx.wxID_ANY)
                local this = ui.window

                ui.BoxSizer1 = wx.wxBoxSizer(wx.wxVERTICAL)
                ui.BoxSizer2 = wx.wxFlexGridSizer(3, 1, 0, 0)

                ui.BoxSizer2:AddSpacer(10)
                
                ui.StaticText1 = wx.wxStaticText(this, wx.wxID_ANY, "dnx RTOS Configuration Tool")
                ui.StaticText1:SetFont(wx.wxFont(14, wx.wxDEFAULT, wx.wxFONTSTYLE_NORMAL, wx.wxBOLD, false, "", wx.wxFONTENCODING_DEFAULT))
                ui.BoxSizer2:Add(ui.StaticText1, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.BoxSizer2:AddSpacer(20)
                
                ui.StaticText2 = wx.wxStaticText(this, wx.wxID_ANY, "dnx Configuration Tool is a tool that helps users to configure dnx RTOS project. "..
                                                                     "From tool a particular microcontroller's modules can be configured more easily "..
                                                                     "than by editing source file flags. The application takes care of disabling unused modules "..
                                                                     "for the selected microcontroller and resolves some dependencies between configuration flags.",
                                                                     wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxALIGN_CENTER)
                ui.StaticText2:Wrap(400)
                ui.BoxSizer2:Add(ui.StaticText2, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                
                ui.BoxSizer2:AddSpacer(20)

                ui.StaticText3 = wx.wxStaticText(this, wx.wxID_ANY, "© 2014 Daniel Zorychta")
                ui.BoxSizer2:Add(ui.StaticText3, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                
                ui.BoxSizer2:AddSpacer(20)
                
                ui.StaticText4 = wx.wxStaticText(this, wx.wxID_ANY, "http://www.dnx-rtos.org/")
                ui.BoxSizer2:Add(ui.StaticText4, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.BoxSizer2:AddSpacer(20)
                
                ui.StaticText5 = wx.wxStaticText(this, wx.wxID_ANY, "This program is free software; you can redistribute it and/or modify\n"..
                                                                    "it under the terms of the GNU General Public License as published by\n"..
                                                                    "the  Free Software  Foundation;  either version 2 of the License, or\n"..
                                                                    "any later version.                                                  \n"..
                                                                    "                                                                    \n"..
                                                                    "This  program  is  distributed  in the hope that  it will be useful,\n"..
                                                                    "but  WITHOUT  ANY  WARRANTY;  without  even  the implied warranty of\n"..
                                                                    "MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the\n"..
                                                                    "GNU General Public License for more details.                        \n"..
                                                                    "                                                                    \n"..
                                                                    "You  should  have received a copy  of the GNU General Public License\n"..
                                                                    "along  with  this  program;  if not,  write  to  the  Free  Software\n"..
                                                                    "Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.")
                
         
                ui.BoxSizer2:Add(ui.StaticText5, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            
                ui.BoxSizer1:Add(ui.BoxSizer2, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_VERTICAL,wx.wxALIGN_CENTER_HORIZONTAL), 0)

                this:SetSizer(ui.BoxSizer1)
        end

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @param  None
-- @return Module name
--------------------------------------------------------------------------------
function about:get_window_name()
        return "About"
end


--------------------------------------------------------------------------------
-- @brief  Function is called when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function about:refresh()
end


--------------------------------------------------------------------------------
-- @brief  Function check if options are modified
-- @param  None
-- @return true if options are modified, otherwise false
--------------------------------------------------------------------------------
function about:is_modified()
        return false
end
