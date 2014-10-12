--[[============================================================================
@file    tty.lua

@author  Daniel Zorychta

@brief   Configuration script for TTY module.

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
module(..., package.seeall)


--==============================================================================
-- EXTERNAL MODULES
--==============================================================================
require("wx")
require("modules/ctcore")


--==============================================================================
-- GLOBAL OBJECTS
--==============================================================================
-- module's main object
tty = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
-- local objects
local ui = {}
local ID = {}


--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function loads all controls from configuration files
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_controls()
        local enable          = ct:get_module_state("TTY")
        local term_cols       = tonumber(ct:key_read(config.noarch.key.TTY_TERM_COLS))
        local term_rows       = tonumber(ct:key_read(config.noarch.key.TTY_TERM_ROWS))
        local out_stream_size = tonumber(ct:key_read(config.noarch.key.TTY_OUT_STREAM_LEN))
        local number_of_term  = tonumber(ct:key_read(config.noarch.key.TTY_NUMBER_OF_TERM)) - 1
        local autosize_enable = ct:yes_no_to_bool(ct:key_read(config.noarch.key.TTY_ENABLE_TERM_SIZE_CHECK))
        local term_in_file    = ct:key_read(config.noarch.key.TTY_TERM_IN_FILE):gsub('"', '')
        local term_out_file   = ct:key_read(config.noarch.key.TTY_TERM_OUT_FILE):gsub('"', '')

        ui.SpinCtrl_columns:SetValue(term_cols)
        ui.SpinCtrl_rows:SetValue(term_rows)
        ui.CheckBox_autosize:SetValue(autosize_enable)
        ui.SpinCtrl_outbufsize:SetValue(out_stream_size)
        ui.Choice_termcount:SetSelection(number_of_term)
        ui.ComboBox_instreampath:SetValue(term_in_file)
        ui.ComboBox_outstreampath:SetValue(term_out_file)
        ui.CheckBox_enable:SetValue(enable)
        ui.Panel1:Enable(enable)
end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function on_button_save_click()
        local enable          = ui.CheckBox_enable:GetValue()
        local term_cols       = tostring(ui.SpinCtrl_columns:GetValue())
        local term_rows       = tostring(ui.SpinCtrl_rows:GetValue())
        local out_stream_size = tostring(ui.SpinCtrl_outbufsize:GetValue())
        local number_of_term  = tostring(ui.Choice_termcount:GetSelection() + 1)
        local autosize_enable = ct:bool_to_yes_no(ui.CheckBox_autosize:GetValue())
        local term_in_file    = '"'..ui.ComboBox_instreampath:GetValue()..'"'
        local term_out_file   = '"'..ui.ComboBox_outstreampath:GetValue()..'"'

        ct:enable_module("TTY", enable)
        ct:key_write(config.noarch.key.TTY_TERM_COLS, term_cols)
        ct:key_write(config.noarch.key.TTY_TERM_ROWS, term_rows)
        ct:key_write(config.noarch.key.TTY_OUT_STREAM_LEN, out_stream_size)
        ct:key_write(config.noarch.key.TTY_NUMBER_OF_TERM, number_of_term)
        ct:key_write(config.noarch.key.TTY_ENABLE_TERM_SIZE_CHECK, autosize_enable)
        ct:key_write(config.noarch.key.TTY_TERM_IN_FILE, term_in_file)
        ct:key_write(config.noarch.key.TTY_TERM_OUT_FILE, term_out_file)

        ui.Button_save:Enable(false)
end


--------------------------------------------------------------------------------
-- @brief  Event is called when module enable checkbox is changed
-- @param  this         event object
-- @return None
--------------------------------------------------------------------------------
local function checkbox_enable_updated(this)
        ui.Panel1:Enable(this:IsChecked())
        ui.Button_save:Enable(true)
end


--------------------------------------------------------------------------------
-- @brief  Evemt is called when value is updated
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function value_updated()
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
function tty:create_window(parent)
        ui = {}

        ID = {}
        ID.CHECKBOX_ENABLE = wx.wxNewId()
        ID.PANEL1 = wx.wxNewId()
        ID.STATICTEXT1 = wx.wxNewId()
        ID.SPINCTRL_COLUMNS = wx.wxNewId()
        ID.STATICTEXT2 = wx.wxNewId()
        ID.SPINCTRL_ROWS = wx.wxNewId()
        ID.CHECKBOX_AUTOSIZE = wx.wxNewId()
        ID.SPINCTRL_OUTBUFSIZE = wx.wxNewId()
        ID.STATICTEXT3 = wx.wxNewId()
        ID.CHOICE_TERMCOUNT = wx.wxNewId()
        ID.STATICTEXT4 = wx.wxNewId()
        ID.COMBOBOX_INSTREAMPATH = wx.wxNewId()
        ID.STATICTEXT5 = wx.wxNewId()
        ID.COMBOBOX_OUTSTREAMPATH = wx.wxNewId()
        ID.STATICLINE1 = wx.wxNewId()
        ID.BUTTON_SAVE = wx.wxNewId()

        ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
        local this = ui.window

        ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.CheckBox_enable = wx.wxCheckBox(this, ID.CHECKBOX_ENABLE, "Enable module", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_ENABLE")
        ui.FlexGridSizer1:Add(ui.CheckBox_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Panel1 = wx.wxPanel(this, ID.PANEL1, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL, "ID.PANEL1")
        ui.FlexGridSizer2 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.StaticBoxSizer1 = wx.wxStaticBoxSizer(wx.wxVERTICAL, ui.Panel1, "Terminal size")
        ui.FlexGridSizer3 = wx.wxFlexGridSizer(0, 5, 0, 0)
        ui.StaticText1 = wx.wxStaticText(ui.Panel1, ID.STATICTEXT1, "Columns", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT1")
        ui.FlexGridSizer3:Add(ui.StaticText1, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.SpinCtrl_columns = wx.wxSpinCtrl(ui.Panel1, ID.SPINCTRL_COLUMNS, "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 20, 128, 0, "ID.SPINCTRL_COLUMNS")
        ui.SpinCtrl_columns:SetToolTip("Default terminal size.")
        ui.FlexGridSizer3:Add(ui.SpinCtrl_columns, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.FlexGridSizer3:Add(-1,-1,1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText2 = wx.wxStaticText(ui.Panel1, ID.STATICTEXT2, "Rows", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT2")
        ui.FlexGridSizer3:Add(ui.StaticText2, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.SpinCtrl_rows = wx.wxSpinCtrl(ui.Panel1, ID.SPINCTRL_ROWS, "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 4, 100, 0, "ID.SPINCTRL_ROWS")
        ui.SpinCtrl_rows:SetToolTip("Default terminal size.")
        ui.FlexGridSizer3:Add(ui.SpinCtrl_rows, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticBoxSizer1:Add(ui.FlexGridSizer3, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.BoxSizer1 = wx.wxBoxSizer(wx.wxHORIZONTAL)
        ui.CheckBox_autosize = wx.wxCheckBox(ui.Panel1, ID.CHECKBOX_AUTOSIZE, "Get real terminal size", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_AUTOSIZE")
        ui.CheckBox_autosize:SetToolTip("This option enables a read of a real terminal size. The default terminal size will be overwritten.")
        ui.BoxSizer1:Add(ui.CheckBox_autosize, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticBoxSizer1:Add(ui.BoxSizer1, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.FlexGridSizer2:Add(ui.StaticBoxSizer1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticBoxSizer2 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel1, "Output stream size")
        ui.FlexGridSizer4 = wx.wxFlexGridSizer(0, 2, 0, 0)
        ui.SpinCtrl_outbufsize = wx.wxSpinCtrl(ui.Panel1, ID.SPINCTRL_OUTBUFSIZE, "0", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 20, 128, 0, "ID.SPINCTRL_OUTBUFSIZE")
        ui.SpinCtrl_outbufsize:SetToolTip("This value determine a size of terminal output buffer.")
        ui.FlexGridSizer4:Add(ui.SpinCtrl_outbufsize, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText3 = wx.wxStaticText(ui.Panel1, ID.STATICTEXT3, "bytes", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT3")
        ui.FlexGridSizer4:Add(ui.StaticText3, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticBoxSizer2:Add(ui.FlexGridSizer4, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.FlexGridSizer2:Add(ui.StaticBoxSizer2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticBoxSizer3 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel1, "Number of virtual terminals")
        ui.Choice_termcount = wx.wxChoice(ui.Panel1, ID.CHOICE_TERMCOUNT, wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, -1), {}, 0, wx.wxDefaultValidator, "ID.CHOICE_TERMCOUNT")
        ui.Choice_termcount:Append("1 terminal")
        ui.Choice_termcount:Append("2 terminals")
        ui.Choice_termcount:Append("3 terminals")
        ui.Choice_termcount:Append("4 terminals")
        ui.StaticBoxSizer3:Add(ui.Choice_termcount, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.FlexGridSizer2:Add(ui.StaticBoxSizer3, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticBoxSizer4 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel1, "Streams")
        ui.FlexGridSizer5 = wx.wxFlexGridSizer(0, 2, 0, 0)
        ui.StaticText4 = wx.wxStaticText(ui.Panel1, ID.STATICTEXT4, "Input stream", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT4")
        ui.FlexGridSizer5:Add(ui.StaticText4, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.ComboBox_instreampath = wx.wxComboBox(ui.Panel1, ID.COMBOBOX_INSTREAMPATH, "", wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.COMBOBOX_INSTREAMPATH")
        ui.ComboBox_instreampath:Append("/dev/ttyS0")
        ui.ComboBox_instreampath:SetToolTip("This is a path to the input stream of the module. From this file module reads keys. If input and output streams are the same, make sure that selected file can be accessed from many threads (one thread for read and one for write).")
        ui.FlexGridSizer5:Add(ui.ComboBox_instreampath, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticText5 = wx.wxStaticText(ui.Panel1, ID.STATICTEXT5, "Output stream", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT5")
        ui.FlexGridSizer5:Add(ui.StaticText5, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.ComboBox_outstreampath = wx.wxComboBox(ui.Panel1, ID.COMBOBOX_OUTSTREAMPATH, "", wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.COMBOBOX_OUTSTREAMPATH")
        ui.ComboBox_outstreampath:Append("/dev/ttyS0")
        ui.ComboBox_outstreampath:SetToolTip("This is a path to the output stream of the module. Module by using this file sends messages to terminal. If input and output streams are the same, make sure that selected file can be accessed from many threads (one thread for read and one for write).")
        ui.FlexGridSizer5:Add(ui.ComboBox_outstreampath, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticBoxSizer4:Add(ui.FlexGridSizer5, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.FlexGridSizer2:Add(ui.StaticBoxSizer4, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticLine1 = wx.wxStaticLine(ui.Panel1, ID.STATICLINE1, wx.wxDefaultPosition, wx.wxSize(10,-1), wx.wxLI_HORIZONTAL, "ID.STATICLINE1")
        ui.FlexGridSizer2:Add(ui.StaticLine1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Panel1:SetSizer(ui.FlexGridSizer2)
        ui.FlexGridSizer1:Add(ui.Panel1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.Button_save = wx.wxButton(this, ID.BUTTON_SAVE, "Save", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.BUTTON_SAVE")
        ui.FlexGridSizer1:Add(ui.Button_save, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)

        --
        this:SetSizer(ui.FlexGridSizer1)
        this:SetScrollRate(50, 50)

        --
        this:Connect(ID.CHECKBOX_ENABLE,        wx.wxEVT_COMMAND_CHECKBOX_CLICKED,  checkbox_enable_updated)
        this:Connect(ID.SPINCTRL_COLUMNS,       wx.wxEVT_COMMAND_SPINCTRL_UPDATED,  value_updated          )
        this:Connect(ID.SPINCTRL_ROWS,          wx.wxEVT_COMMAND_SPINCTRL_UPDATED,  value_updated          )
        this:Connect(ID.CHECKBOX_AUTOSIZE,      wx.wxEVT_COMMAND_CHECKBOX_CLICKED,  value_updated          )
        this:Connect(ID.SPINCTRL_OUTBUFSIZE,    wx.wxEVT_COMMAND_SPINCTRL_UPDATED,  value_updated          )
        this:Connect(ID.CHOICE_TERMCOUNT,       wx.wxEVT_COMMAND_CHOICE_SELECTED,   value_updated          )
        this:Connect(ID.COMBOBOX_INSTREAMPATH,  wx.wxEVT_COMMAND_COMBOBOX_SELECTED, value_updated          )
        this:Connect(ID.COMBOBOX_INSTREAMPATH,  wx.wxEVT_COMMAND_TEXT_UPDATED,      value_updated          )
        this:Connect(ID.COMBOBOX_OUTSTREAMPATH, wx.wxEVT_COMMAND_COMBOBOX_SELECTED, value_updated          )
        this:Connect(ID.COMBOBOX_OUTSTREAMPATH, wx.wxEVT_COMMAND_TEXT_UPDATED,      value_updated          )
        this:Connect(ID.BUTTON_SAVE,            wx.wxEVT_COMMAND_BUTTON_CLICKED,    on_button_save_click   )

        --
        load_controls()
        ui.Button_save:Enable(false)

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @return Module name
--------------------------------------------------------------------------------
function tty:get_window_name()
        return "TTY"
end


--------------------------------------------------------------------------------
-- @brief  Function is called by parent when window is selected
--------------------------------------------------------------------------------
function tty:selected()
end


--------------------------------------------------------------------------------
-- @brief  Function returns modify status
-- @return If data is modified true is returned, otherwise false
--------------------------------------------------------------------------------
function tty:is_modified()
        return ui.Button_save:IsEnabled()
end


--------------------------------------------------------------------------------
-- @brief  Function save configuration
-- @return None
--------------------------------------------------------------------------------
function tty:save()
        on_button_save_click()
end


--------------------------------------------------------------------------------
-- @brief  Function returns module handler
-- @return Module handler
--------------------------------------------------------------------------------
function get_handler()
        return tty
end
