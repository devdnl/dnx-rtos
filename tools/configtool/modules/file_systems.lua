--[[============================================================================
@file    file_systems.lua

@author  Daniel Zorychta

@brief   Configuration script for file systems.

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
file_systems = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local ui = {}
local ID = {}
ID.BUTTON_SAVE = wx.wxNewId()
ID.CHECKBOX_DEVFS = wx.wxNewId()
ID.CHECKBOX_FATFS = wx.wxNewId()
ID.CHECKBOX_FATFS_LFN = wx.wxNewId()
ID.CHECKBOX_LFS = wx.wxNewId()
ID.CHECKBOX_PROCFS = wx.wxNewId()
ID.CHOICE_FATFS_LFN_CODEPAGE = wx.wxNewId()

local codepage = {"437 - U.S.",
                  "720 - Arabic",
                  "737 - Greek",
                  "775 - Baltic",
                  "850 - Multilingual Latin 1",
                  "852 - Latin 2",
                  "855 - Cyrillic",
                  "857 - Turkish",
                  "858 - Multilingual Latin 1 + Euro",
                  "862 - Hebrew",
                  "866 - Russian",
                  "874 - Thai",
                  "932 - Japanese Shift-JIS",
                  "936 - Simplified Chinese GBK",
                  "949 - Korean",
                  "950 - Traditional Chinese Big5",
                  "1250 - Central Europe",
                  "1251 - Cyrillic",
                  "1252 - Latin 1",
                  "1253 - Greek",
                  "1254 - Turkish",
                  "1255 - Hebrew",
                  "1256 - Arabic",
                  "1257 - Baltic",
                  "1258 - Vietnam"}


--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function loads all controls from configuration files
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_controls()
        ui.CheckBox_devfs:SetValue(ct:get_module_state("DEVFS"))
        ui.CheckBox_lfs:SetValue(ct:get_module_state("LFS"))
        ui.CheckBox_procfs:SetValue(ct:get_module_state("PROCFS"))

        local fatfs_en = ct:get_module_state("FATFS")
        ui.CheckBox_fatfs:SetValue(fatfs_en)

        local codepage_idx = 1
        local codepage_val = ct:key_read(config.project.key.FATFS_LFN_CODEPAGE)
        for i = 1, #codepage do
                if codepage[i]:match(codepage_val) then
                        codepage_idx = i
                        break
                end
        end

        local fatfs_lfn = ct:yes_no_to_bool(ct:key_read(config.project.key.FATFS_LFN_ENABLE))
        ui.CheckBox_fatfs_lfn:SetValue(fatfs_lfn)
        ui.CheckBox_fatfs_lfn:Enable(fatfs_en)
        ui.Choice_fatfs_lfn_codepage:SetSelection(codepage_idx - 1)
        ui.Choice_fatfs_lfn_codepage:Enable(fatfs_lfn and fatfs_en)
end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function on_button_save_click()
        ct:enable_module("DEVFS", ui.CheckBox_devfs:GetValue())
        ct:enable_module("LFS", ui.CheckBox_lfs:GetValue())
        ct:enable_module("FATFS", ui.CheckBox_fatfs:GetValue())
        ct:key_write(config.project.key.FATFS_LFN_ENABLE, ct:bool_to_yes_no(ui.CheckBox_fatfs_lfn:GetValue()))
        ct:key_write(config.project.key.FATFS_LFN_CODEPAGE, codepage[ui.Choice_fatfs_lfn_codepage:GetSelection() + 1]:match("%d*"))
        ct:enable_module("PROCFS", ui.CheckBox_procfs:GetValue())

        ui.Button_save:Enable(false)
end


--------------------------------------------------------------------------------
-- @brief  Event is called when value is changed (by any control)
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function value_changed()
        ui.Button_save:Enable(true)
end


--------------------------------------------------------------------------------
-- @brief  Event is called when FATFS checkbox is changed
-- @param  this         event object
-- @return None
--------------------------------------------------------------------------------
local function FATFS_state_changed(this)
        ui.Choice_fatfs_lfn_codepage:Enable(this:IsChecked())
        ui.CheckBox_fatfs_lfn:Enable(this:IsChecked())
        ui.Button_save:Enable(true)
end


--------------------------------------------------------------------------------
-- @brief  Event is called when LFN checkbox is changed
-- @param  this         event object
-- @return None
--------------------------------------------------------------------------------
local function LFN_enable_changed(this)
        ui.Choice_fatfs_lfn_codepage:Enable(this:IsChecked())
        ui.Button_save:Enable(true)
end


--==============================================================================
-- GLOBAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function file_systems:create_window(parent)
        if ui.window == nil then
                ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
                local this = ui.window

                ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)

                --
                ui.StaticBoxSizer1 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "devfs")
                ui.FlexGridSizer2 = wx.wxFlexGridSizer(2, 1, 0, 0)

                ui.StaticText1 = wx.wxStaticText(this, wx.wxID_ANY, "The devfs is a small file system, that can be used to store device and pipe files only. This file system is minimalistic and thus is ideal for devices-files, because is fast. A data of this file system is stored in the RAM.", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT1")
                ui.StaticText1:Wrap(ct.CONTROL_X_SIZE)
                ui.FlexGridSizer2:Add(ui.StaticText1, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_devfs = wx.wxCheckBox(this, ID.CHECKBOX_DEVFS, "Enable", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_DEVFS")
                ui.FlexGridSizer2:Add(ui.CheckBox_devfs, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticBoxSizer1:Add(ui.FlexGridSizer2, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                --
                ui.StaticBoxSizer2 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "lfs")

                ui.FlexGridSizer3 = wx.wxFlexGridSizer(2, 1, 0, 0)

                ui.StaticText2 = wx.wxStaticText(this, wx.wxID_ANY, "The lfs is a general purpose RAM file system, that can be used to store files, device-files, and pipes. The file system is the best choice if you want to use only one file system for each file operations. The lfs is not fast as devfs, because provides greater functionality.", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT2")
                ui.StaticText2:Wrap(ct.CONTROL_X_SIZE)
                ui.FlexGridSizer3:Add(ui.StaticText2, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_lfs = wx.wxCheckBox(this, ID.CHECKBOX_LFS, "Enable", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_LFS")
                ui.FlexGridSizer3:Add(ui.CheckBox_lfs, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticBoxSizer2:Add(ui.FlexGridSizer3, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                --
                ui.StaticBoxSizer3 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "fatfs (FAT12, FAT16, FAT32)")

                ui.FlexGridSizer4 = wx.wxFlexGridSizer(2, 1, 0, 0)

                ui.StaticText3 = wx.wxStaticText(this, wx.wxID_ANY, "If you want to use FAT12, FAT16, and FAT32 in your system enable this file system. The fatfs read/write data from/to device-file e.g. SD cards and other bigger volumes. This file system does not support special files.", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT3")
                ui.StaticText3:Wrap(ct.CONTROL_X_SIZE)
                ui.FlexGridSizer4:Add(ui.StaticText3, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.CheckBox_fatfs = wx.wxCheckBox(this, ID.CHECKBOX_FATFS, "Enable", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_FATFS")
                ui.FlexGridSizer4:Add(ui.CheckBox_fatfs, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.FlexGridSizer6 = wx.wxFlexGridSizer(1, 2, 0, 0)
                ui.CheckBox_fatfs_lfn = wx.wxCheckBox(this, ID.CHECKBOX_FATFS_LFN, "Enable long file names", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX1")
                ui.FlexGridSizer6:Add(ui.CheckBox_fatfs_lfn, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_fatfs_lfn_codepage = wx.wxChoice(this, ID.CHOICE_FATFS_LFN_CODEPAGE, wx.wxDefaultPosition, wx.wxDefaultSize, codepage, 0, wx.wxDefaultValidator, "ID.CHOICE1")

                ui.FlexGridSizer6:Add(ui.Choice_fatfs_lfn_codepage, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer4:Add(ui.FlexGridSizer6, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 0)

                ui.StaticBoxSizer3:Add(ui.FlexGridSizer4, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer3, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                --
                ui.StaticBoxSizer4 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "procfs")
                ui.FlexGridSizer5 = wx.wxFlexGridSizer(2, 1, 0, 0)

                ui.StaticText4 = wx.wxStaticText(this, wx.wxID_ANY, "The procfs is a special file system, that provides special functionality; by using this file system you can see all tasks and their names and so on. In this file system are stored special system files, that can be read to obtain system specified settings, or microcontroller information. If you do not need to read special information, then probably you do not need this file system.", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT4")
                ui.StaticText4:Wrap(ct.CONTROL_X_SIZE)
                ui.FlexGridSizer5:Add(ui.StaticText4, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_procfs = wx.wxCheckBox(this, ID.CHECKBOX_PROCFS, "Enable", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_PROCFS")
                ui.FlexGridSizer5:Add(ui.CheckBox_procfs, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticBoxSizer4:Add(ui.FlexGridSizer5, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer4, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                --
                ui.StaticLine2 = wx.wxStaticLine(this, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxSize(10,-1), wx.wxLI_HORIZONTAL)
                ui.FlexGridSizer1:Add(ui.StaticLine2, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 0)

                ui.Button_save = wx.wxButton(this, ID.BUTTON_SAVE, "&Save", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer1:Add(ui.Button_save, 1, bit.bor(wx.wxALL, wx.wxALIGN_RIGHT, wx.wxALIGN_CENTER_VERTICAL), 5)

                --
                this:SetSizer(ui.FlexGridSizer1)
                this:SetScrollRate(50, 50)

                --
                this:Connect(ID.CHECKBOX_DEVFS,            wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_changed       )
                this:Connect(ID.CHECKBOX_LFS,              wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_changed       )
                this:Connect(ID.CHECKBOX_FATFS,            wx.wxEVT_COMMAND_CHECKBOX_CLICKED, FATFS_state_changed )
                this:Connect(ID.CHECKBOX_FATFS_LFN,        wx.wxEVT_COMMAND_CHECKBOX_CLICKED, LFN_enable_changed  )
                this:Connect(ID.CHOICE_FATFS_LFN_CODEPAGE, wx.wxEVT_COMMAND_CHOICE_SELECTED,  value_changed       )
                this:Connect(ID.CHECKBOX_PROCFS,           wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_changed       )
                this:Connect(ID.BUTTON_SAVE,               wx.wxEVT_COMMAND_BUTTON_CLICKED,   on_button_save_click)
        end

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @param  None
-- @return Module name
--------------------------------------------------------------------------------
function file_systems:get_window_name()
        return "File Systems"
end


--------------------------------------------------------------------------------
-- @brief  Function is called when window is selected
-- @return None
--------------------------------------------------------------------------------
function file_systems:refresh()
        load_controls()
        ui.Button_save:Enable(false)
end


--------------------------------------------------------------------------------
-- @brief  Function check if options are modified
-- @return true if options are modified, otherwise false
--------------------------------------------------------------------------------
function file_systems:is_modified()
        return ui.Button_save:IsEnabled()
end


--------------------------------------------------------------------------------
-- @brief  Function save configuration
-- @return None
--------------------------------------------------------------------------------
function file_systems:save()
        on_button_save_click()
end
