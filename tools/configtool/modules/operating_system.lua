--[[============================================================================
@file    operating_system.lua

@author  Daniel Zorychta

@brief   Operating System configuration script.

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
operating_system = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local modified = ct:new_modify_indicator()
local ui = {}
local ID = {}
ID.CHECKBOX1_SYS_MEMMON = wx.wxNewId()
ID.CHECKBOX_COLOR_TERM = wx.wxNewId()
ID.CHECKBOX_CPU_LOADMON = wx.wxNewId()
ID.CHECKBOX_KRN_MEMMON = wx.wxNewId()
ID.CHECKBOX_MOD_MEMMON = wx.wxNewId()
ID.CHECKBOX_NET_MEMMON = wx.wxNewId()
ID.CHECKBOX_PRINTF = wx.wxNewId()
ID.CHECKBOX_PRINTK = wx.wxNewId()
ID.CHECKBOX_SCANF = wx.wxNewId()
ID.CHECKBOX_SLEEP_ON_IDLE = wx.wxNewId()
ID.CHECKBOX_STOP_MACRO = wx.wxNewId()
ID.CHECKBOX_TASK_FILEMON = wx.wxNewId()
ID.CHECKBOX_TASK_MEMMON = wx.wxNewId()
ID.CHOICE_ERRNO_SIZE = wx.wxNewId()
ID.SPINCTRL_IRQ_STACK_SIZE = wx.wxNewId()
ID.SPINCTRL_FS_STACK_SIZE = wx.wxNewId()
ID.SPINCTRL_INITD_STACK_SIZE = wx.wxNewId()
ID.SPINCTRL_MEM_BLOCK = wx.wxNewId()
ID.SPINCTRL_NET_MEM_LIMIT = wx.wxNewId()
ID.SPINCTRL_NUMBER_OF_PRIORITIES = wx.wxNewId()
ID.SPINCTRL_PIPE_LEN = wx.wxNewId()
ID.SPINCTRL_STREAM_LEN = wx.wxNewId()
ID.SPINCTRL_SWITCH_FREQ = wx.wxNewId()
ID.SPINCTRL_TASK_NAME_LEN = wx.wxNewId()
ID.SPINCTRL_TASK_STACK_SIZE = wx.wxNewId()
ID.STATICTEXT2 = wx.wxNewId()
ID.STATICTEXT3 = wx.wxNewId()
ID.STATICTEXT4 = wx.wxNewId()
ID.STATICTEXT5 = wx.wxNewId()
ID.STATICTEXT8 = wx.wxNewId()
ID.STATICTEXT9 = wx.wxNewId()
ID.STATICTEXT10 = wx.wxNewId()
ID.STATICTEXT11 = wx.wxNewId()
ID.STATICTEXT12 = wx.wxNewId()
ID.STATICTEXT13 = wx.wxNewId()
ID.STATICTEXT14 = wx.wxNewId()
ID.STATICTEXT_TOTAL_STACK_SIZE = wx.wxNewId()
ID.TEXTCTRL_HOSTNAME = wx.wxNewId()
ID.TEXTCTRL_RTCPATH = wx.wxNewId()

--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function calculate total stack size string using selected configuration
-- @param  None
-- @return Calculated value of total size of stack (string)
--------------------------------------------------------------------------------
local function get_total_stack_size_string()
        return tostring((ui.SpinCtrl_task_stack_size:GetValue() + ui.SpinCtrl_fs_stack_size:GetValue() + ui.SpinCtrl_irq_stack_size:GetValue()).." levels")
end


--------------------------------------------------------------------------------
-- @brief  Function loads all controls from configuration files
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_configuration()
        ui.SpinCtrl_task_stack_size:SetValue(tonumber(ct:key_read(config.project.key.OS_TASK_MIN_STACK_DEPTH)))
        ui.SpinCtrl_fs_stack_size:SetValue(tonumber(ct:key_read(config.project.key.OS_FILE_SYSTEM_STACK_DEPTH)))
        ui.SpinCtrl_irq_stack_size:SetValue(tonumber(ct:key_read(config.project.key.OS_IRQ_STACK_DEPTH)))
        ui.SpinCtrl_initd_stack_size:SetValue(tonumber(ct:key_read(config.project.key.OS_INITD_STACK_DEPTH)))
        ui.StaticText_total_stack_size:SetLabel(get_total_stack_size_string())
        ui.SpinCtrl_number_of_priorities:SetValue(tonumber(ct:key_read(config.project.key.OS_TASK_MAX_PRIORITIES)))
        ui.SpinCtrl_task_name_len:SetValue(tonumber(ct:key_read(config.project.key.OS_TASK_NAME_LEN)))
        ui.SpinCtrl_switch_freq:SetValue(tonumber(ct:key_read(config.project.key.OS_TASK_SCHED_FREQ)))
        ui.CheckBox_sleep_on_idle:SetValue(ct:yes_no_to_bool(ct:key_read(config.project.key.OS_SLEEP_ON_IDLE)))
        ui.CheckBox_printk:SetValue(ct:yes_no_to_bool(ct:key_read(config.project.key.OS_SYSTEM_MSG_ENABLE)))
        ui.CheckBox_printf:SetValue(ct:yes_no_to_bool(ct:key_read(config.project.key.OS_PRINTF_ENABLE)))
        ui.CheckBox_scanf:SetValue(ct:yes_no_to_bool(ct:key_read(config.project.key.OS_SCANF_ENABLE)))
        ui.CheckBox_color_term:SetValue(ct:yes_no_to_bool(ct:key_read(config.project.key.OS_COLOR_TERMINAL_ENABLE)))
        ui.CheckBox_stop_macro:SetValue(ct:yes_no_to_bool(ct:key_read(config.project.key.OS_SYSTEM_STOP_MACRO)))
        ui.CheckBox_task_memmon:SetValue(ct:yes_no_to_bool(ct:key_read(config.project.key.OS_MONITOR_TASK_MEMORY_USAGE)))
        ui.CheckBox_task_filemon:SetValue(ct:yes_no_to_bool(ct:key_read(config.project.key.OS_MONITOR_TASK_FILE_USAGE)))
        ui.CheckBox_krn_memmon:SetValue(ct:yes_no_to_bool(ct:key_read(config.project.key.OS_MONITOR_KERNEL_MEMORY_USAGE)))
        ui.CheckBox_mod_memmon:SetValue(ct:yes_no_to_bool(ct:key_read(config.project.key.OS_MONITOR_MODULE_MEMORY_USAGE)))
        ui.CheckBox_sys_memmon:SetValue(ct:yes_no_to_bool(ct:key_read(config.project.key.OS_MONITOR_SYSTEM_MEMORY_USAGE)))
        ui.CheckBox_CPU_loadmon:SetValue(ct:yes_no_to_bool(ct:key_read(config.project.key.OS_MONITOR_CPU_LOAD)))
        ui.CheckBox_net_memmon:SetValue(ct:yes_no_to_bool(ct:key_read(config.project.key.OS_MONITOR_NETWORK_MEMORY_USAGE)))
        ui.SpinCtrl_net_mem_limit:SetValue(ct:key_read(config.project.key.OS_MONITOR_NETWORK_MEMORY_USAGE_LIMIT))
        ui.SpinCtrl_net_mem_limit:Enable(ui.CheckBox_net_memmon:GetValue())
        ui.SpinCtrl_stream_len:SetValue(tonumber(ct:key_read(config.project.key.OS_STREAM_BUFFER_LENGTH)))
        ui.SpinCtrl_pipe_len:SetValue(tonumber(ct:key_read(config.project.key.OS_PIPE_LENGTH)))
        ui.SpinCtrl_mem_block:SetValue(tonumber(ct:key_read(config.project.key.HEAP_BLOCK_SIZE)))
        ui.Choice_errno_size:SetSelection(tonumber(ct:key_read(config.project.key.OS_ERRNO_STRING_LEN)))
        ui.TextCtrl_hostname:SetValue(ct:key_read(config.project.key.OS_HOSTNAME):gsub('"', ''))
        ui.TextCtrl_RTC_path:SetValue(ct:key_read(config.project.key.OS_RTC_FILE_PATH):gsub('"', ''))
end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function save_configuration()
        ct:key_write(config.project.key.OS_TASK_MIN_STACK_DEPTH, tostring(ui.SpinCtrl_task_stack_size:GetValue()))
        ct:key_write(config.project.key.OS_FILE_SYSTEM_STACK_DEPTH, tostring(ui.SpinCtrl_fs_stack_size:GetValue()))
        ct:key_write(config.project.key.OS_IRQ_STACK_DEPTH, tostring(ui.SpinCtrl_irq_stack_size:GetValue()))
        ct:key_write(config.project.key.OS_INITD_STACK_DEPTH, tostring(ui.SpinCtrl_initd_stack_size:GetValue()))
        ct:key_write(config.project.key.OS_TASK_MAX_PRIORITIES, tostring(ui.SpinCtrl_number_of_priorities:GetValue()))
        ct:key_write(config.project.key.OS_TASK_NAME_LEN, tostring(ui.SpinCtrl_task_name_len:GetValue()))
        ct:key_write(config.project.key.OS_TASK_SCHED_FREQ, tostring(ui.SpinCtrl_switch_freq:GetValue()))
        ct:key_write(config.project.key.OS_SLEEP_ON_IDLE, ct:bool_to_yes_no(ui.CheckBox_sleep_on_idle:GetValue()))
        ct:key_write(config.project.key.OS_SYSTEM_MSG_ENABLE, ct:bool_to_yes_no(ui.CheckBox_printk:GetValue()))
        ct:key_write(config.project.key.OS_PRINTF_ENABLE, ct:bool_to_yes_no(ui.CheckBox_printf:GetValue()))
        ct:key_write(config.project.key.OS_SCANF_ENABLE, ct:bool_to_yes_no(ui.CheckBox_scanf:GetValue()))
        ct:key_write(config.project.key.OS_COLOR_TERMINAL_ENABLE, ct:bool_to_yes_no(ui.CheckBox_color_term:GetValue()))
        ct:key_write(config.project.key.OS_SYSTEM_STOP_MACRO, ct:bool_to_yes_no(ui.CheckBox_stop_macro:GetValue()))
        ct:key_write(config.project.key.OS_MONITOR_TASK_MEMORY_USAGE, ct:bool_to_yes_no(ui.CheckBox_task_memmon:GetValue()))
        ct:key_write(config.project.key.OS_MONITOR_TASK_FILE_USAGE, ct:bool_to_yes_no(ui.CheckBox_task_filemon:GetValue()))
        ct:key_write(config.project.key.OS_MONITOR_KERNEL_MEMORY_USAGE, ct:bool_to_yes_no(ui.CheckBox_krn_memmon:GetValue()))
        ct:key_write(config.project.key.OS_MONITOR_MODULE_MEMORY_USAGE, ct:bool_to_yes_no(ui.CheckBox_mod_memmon:GetValue()))
        ct:key_write(config.project.key.OS_MONITOR_SYSTEM_MEMORY_USAGE, ct:bool_to_yes_no(ui.CheckBox_sys_memmon:GetValue()))
        ct:key_write(config.project.key.OS_MONITOR_CPU_LOAD, ct:bool_to_yes_no(ui.CheckBox_CPU_loadmon:GetValue()))
        ct:key_write(config.project.key.OS_MONITOR_NETWORK_MEMORY_USAGE, ct:bool_to_yes_no(ui.CheckBox_net_memmon:GetValue()))
        ct:key_write(config.project.key.OS_MONITOR_NETWORK_MEMORY_USAGE_LIMIT, tostring(ui.SpinCtrl_net_mem_limit:GetValue()))
        ct:key_write(config.project.key.OS_STREAM_BUFFER_LENGTH, tostring(ui.SpinCtrl_stream_len:GetValue()))
        ct:key_write(config.project.key.OS_PIPE_LENGTH, tostring(ui.SpinCtrl_pipe_len:GetValue()))
        ct:key_write(config.project.key.HEAP_BLOCK_SIZE, tostring(ui.SpinCtrl_mem_block:GetValue()))
        ct:key_write(config.project.key.OS_ERRNO_STRING_LEN, tostring(ui.Choice_errno_size:GetSelection()))
        ct:key_write(config.project.key.OS_HOSTNAME, '"'..ui.TextCtrl_hostname:GetValue()..'"')
        ct:key_write(config.project.key.OS_RTC_FILE_PATH, '"'..ui.TextCtrl_RTC_path:GetValue()..'"')

        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Event is called when stack value is changed
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function stack_value_changed()
        ui.StaticText_total_stack_size:SetLabel(get_total_stack_size_string())
        modified:yes()
end


--------------------------------------------------------------------------------
-- @brief  Event is called when value is changed (general)
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function value_changed()
        modified:yes()
end


--------------------------------------------------------------------------------
-- @brief  Event is called when network memory monitor checkbox is changed
-- @param  this         event object
-- @return None
--------------------------------------------------------------------------------
local function net_memmon_changed(this)
        ui.SpinCtrl_net_mem_limit:Enable(this:IsChecked())
        modified:yes()
end


--==============================================================================
-- GLOBAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function operating_system:create_window(parent)
        if ui.window == nil then
                ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
                local this = ui.window

                ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)

                -- stack group box
                ui.StaticBoxSizer1 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Stacks")
                ui.FlexGridSizer3 = wx.wxFlexGridSizer(0, 2, 0, 0)
                ui.StaticText2 = wx.wxStaticText(this, ID.STATICTEXT2, "Minimal size of task stack [levels]", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer3:Add(ui.StaticText2, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.SpinCtrl_task_stack_size = wx.wxSpinCtrl(this, ID.SPINCTRL_TASK_STACK_SIZE, "0", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 48, 8192, 0)
                ui.SpinCtrl_task_stack_size:SetToolTip("This value determines the lowest possible stack size that can be assigned for the task. "..
                                                       "The value is a part of entire stack size calculated by using few components.")
                ui.FlexGridSizer3:Add(ui.SpinCtrl_task_stack_size, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText3 = wx.wxStaticText(this, ID.STATICTEXT3, "Size of file systems stack [levels]", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer3:Add(ui.StaticText3, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.SpinCtrl_fs_stack_size = wx.wxSpinCtrl(this, ID.SPINCTRL_FS_STACK_SIZE, "0", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 16, 2048, 0)
                ui.SpinCtrl_fs_stack_size:SetToolTip("This value determines the size of stack that is used by the file systems. This value depends on used file systems. "..
                                                     "The value is a part of entire stack size calculated by using few components.")
                ui.FlexGridSizer3:Add(ui.SpinCtrl_fs_stack_size, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText4 = wx.wxStaticText(this, ID.STATICTEXT4, "Size of interrupt stack [levels]", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT4")
                ui.FlexGridSizer3:Add(ui.StaticText4, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.SpinCtrl_irq_stack_size = wx.wxSpinCtrl(this, ID.SPINCTRL_IRQ_STACK_SIZE, "0", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 16, 2048, 0, "ID.SPINCTRL_IRQ_STACK_SIZE")
                ui.SpinCtrl_irq_stack_size:SetToolTip("This value determines the size of stack that is used by the interrupts. "..
                                                      "The value is a part of entire stack size calculated by using few components.")
                ui.FlexGridSizer3:Add(ui.SpinCtrl_irq_stack_size, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText5 = wx.wxStaticText(this, ID.STATICTEXT5, "Total minimal stack size:", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT5")
                ui.FlexGridSizer3:Add(ui.StaticText5, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText_total_stack_size = wx.wxStaticText(this, ID.STATICTEXT_TOTAL_STACK_SIZE, "0 levels", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT_TOTAL_STACK_SIZE")
                ui.FlexGridSizer3:Add(ui.StaticText_total_stack_size, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticBoxSizer1:Add(ui.FlexGridSizer3, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText = wx.wxStaticText(this, wx.wxID_ANY, "Size of initd stack [levels]", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer3:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.SpinCtrl_initd_stack_size = wx.wxSpinCtrl(this, ID.SPINCTRL_INITD_STACK_SIZE, "0", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 32, 4096)
                ui.SpinCtrl_initd_stack_size:SetToolTip("This value determines the size of the initd stack.")
                ui.FlexGridSizer3:Add(ui.SpinCtrl_initd_stack_size, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- task management group box
                ui.StaticBoxSizer2 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Task management")
                ui.FlexGridSizer4 = wx.wxFlexGridSizer(0, 2, 0, 0)

                ui.StaticText11 = wx.wxStaticText(this, ID.STATICTEXT11, "Number of task priorities", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT11")
                ui.FlexGridSizer4:Add(ui.StaticText11, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.SpinCtrl_number_of_priorities = wx.wxSpinCtrl(this, ID.SPINCTRL_NUMBER_OF_PRIORITIES, "0", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 3, 255, 0, "ID.SPINCTRL_NUMBER_OF_PRIORITIES")
                ui.SpinCtrl_number_of_priorities:SetToolTip("This value determines total number of task priorities. By using this value task priorities are calculated. "..
                                                            "If this value is set to e.g. 7, then system priorities are in range from -3 to 3.")
                ui.FlexGridSizer4:Add(ui.SpinCtrl_number_of_priorities, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText12 = wx.wxStaticText(this, ID.STATICTEXT12, "Length of task name [bytes]", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT12")
                ui.FlexGridSizer4:Add(ui.StaticText12, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.SpinCtrl_task_name_len = wx.wxSpinCtrl(this, ID.SPINCTRL_TASK_NAME_LEN, "0", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 8, 256, 0, "ID.SPINCTRL_TASK_NAME_LEN")
                ui.SpinCtrl_task_name_len:SetToolTip("This value determines the maximum size of task name. If task name is longer than this value, then name will be cutted.")
                ui.FlexGridSizer4:Add(ui.SpinCtrl_task_name_len, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText13 = wx.wxStaticText(this, ID.STATICTEXT13, "Context switch frequency [Hz]", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT13")
                ui.FlexGridSizer4:Add(ui.StaticText13, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.SpinCtrl_switch_freq = wx.wxSpinCtrl(this, ID.SPINCTRL_SWITCH_FREQ, "0", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 1000, 0, "ID.SPINCTRL_SWITCH_FREQ")
                ui.SpinCtrl_switch_freq:SetToolTip("Context switch frequency has influence to system response for events. If your system should response faster for events set this value to higher ones. "..
                                                   "If your microcontroller works on slower frequency this value should be smaller to avoid wasting of CPU time for context switch.")
                ui.FlexGridSizer4:Add(ui.SpinCtrl_switch_freq, 10, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticBoxSizer2:Add(ui.FlexGridSizer4, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- Miscellaneous group box
                ui.StaticBoxSizer5 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Features")
                ui.GridSizer2 = wx.wxGridSizer(0, 2, 0, 0)
                ui.CheckBox_sleep_on_idle = wx.wxCheckBox(this, ID.CHECKBOX_SLEEP_ON_IDLE, "Sleep on idle", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_SLEEP_ON_IDLE")
                 ui.CheckBox_sleep_on_idle:SetToolTip("If this option is selected then system is halted in the idle task (CPU goes to sleep mode). This option can prevent debugging.")
                ui.GridSizer2:Add(ui.CheckBox_sleep_on_idle, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_printk = wx.wxCheckBox(this, ID.CHECKBOX_PRINTK, "pirntk() function", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_PRINTK")
                ui.CheckBox_printk:SetToolTip("If this option is selected then system printing function can be used. Function can be used by modules to print system messages on the terminal.")
                ui.GridSizer2:Add(ui.CheckBox_printk, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_printf = wx.wxCheckBox(this, ID.CHECKBOX_PRINTF, "printf() family functions", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_PRINTF")
                ui.CheckBox_printf:SetToolTip("If this function is selected then printf() family function can be used by the application.")
                ui.GridSizer2:Add(ui.CheckBox_printf, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_scanf = wx.wxCheckBox(this, ID.CHECKBOX_SCANF, "scanf() family functions", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_SCANF")
                ui.CheckBox_scanf:SetToolTip("If this function is selected then scanf() family function can be used by the application.")
                ui.GridSizer2:Add(ui.CheckBox_scanf, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_color_term = wx.wxCheckBox(this, ID.CHECKBOX_COLOR_TERM, "Color terminal", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_COLOR_TERM")
                ui.CheckBox_color_term:SetToolTip("If this function is selected then terminal output can be colorized by using VT100 commands.")
                ui.GridSizer2:Add(ui.CheckBox_color_term, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_stop_macro = wx.wxCheckBox(this, ID.CHECKBOX_STOP_MACRO, "Stop macro (development)", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_STOP_MACRO")
                ui.CheckBox_stop_macro:SetToolTip("If this function is selected then system can be halted due to specific error conditions (e.g. wrong pointer value). "..
                                                  "Use this option only for development purposes.")
                ui.GridSizer2:Add(ui.CheckBox_stop_macro, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_task_memmon = wx.wxCheckBox(this, ID.CHECKBOX_TASK_MEMMON, "Task memory monitoring", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_TASK_MEMMON")
                ui.CheckBox_task_memmon:SetToolTip("If this function is selected then system monitors memory usage of all tasks. This selection is recommended.")
                ui.GridSizer2:Add(ui.CheckBox_task_memmon, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_task_filemon = wx.wxCheckBox(this, ID.CHECKBOX_TASK_FILEMON, "Task file monitoring", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_TASK_FILEMON")
                ui.CheckBox_task_filemon:SetToolTip("If this function is selected then system monitors file usage of all tasks. This selection is recommended.")
                ui.GridSizer2:Add(ui.CheckBox_task_filemon, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_krn_memmon = wx.wxCheckBox(this, ID.CHECKBOX_KRN_MEMMON, "Kernel memory monitoring", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_KRN_MEMMON")
                ui.CheckBox_krn_memmon:SetToolTip("If this function is selected then system monitors kernel memory usage.")
                ui.GridSizer2:Add(ui.CheckBox_krn_memmon, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_mod_memmon = wx.wxCheckBox(this, ID.CHECKBOX_MOD_MEMMON, "Module memory monitoring", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_MOD_MEMMON")
                ui.CheckBox_mod_memmon:SetToolTip("If this function is selected then system monitors memory usage of all modules (drivers).")
                ui.GridSizer2:Add(ui.CheckBox_mod_memmon, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_sys_memmon = wx.wxCheckBox(this, ID.CHECKBOX1_SYS_MEMMON, "System memory monitoring", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX1_SYS_MEMMON")
                ui.CheckBox_sys_memmon:SetToolTip("If this function is selected then memory usage of system is monitored.")
                ui.GridSizer2:Add(ui.CheckBox_sys_memmon, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_CPU_loadmon = wx.wxCheckBox(this, ID.CHECKBOX_CPU_LOADMON, "CPU load monitoring", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_CPU_LOADMON")
                ui.CheckBox_CPU_loadmon:SetToolTip("If this function is selected then system monitors CPU load.")
                ui.GridSizer2:Add(ui.CheckBox_CPU_loadmon, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_net_memmon = wx.wxCheckBox(this, ID.CHECKBOX_NET_MEMMON, "Network memory monitoring", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_NET_MEMMON")
                ui.CheckBox_net_memmon:SetToolTip("If this function is selected then system monitors memory usage of network layer.")
                ui.GridSizer2:Add(ui.CheckBox_net_memmon, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.SpinCtrl_net_mem_limit = wx.wxSpinCtrl(this, ID.SPINCTRL_NET_MEM_LIMIT, "0", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 16777216, 0, "ID.SPINCTRL_NET_MEM_LIMIT")
                ui.SpinCtrl_net_mem_limit:SetToolTip("This value represents a maximum amount of memory in bytes that can be used by the network layer. Set this value to 0 to disable limit.")
                ui.GridSizer2:Add(ui.SpinCtrl_net_mem_limit, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticBoxSizer5:Add(ui.GridSizer2, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer5, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- Sizes of buffers group box
                ui.StaticBoxSizer6 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Blocks and buffers")
                ui.FlexGridSizer2 = wx.wxFlexGridSizer(0, 2, 0, 0)

                ui.StaticText8 = wx.wxStaticText(this, ID.STATICTEXT8, "Length of stream buffer [bytes]", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT8")
                ui.FlexGridSizer2:Add(ui.StaticText8, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.SpinCtrl_stream_len = wx.wxSpinCtrl(this, ID.SPINCTRL_STREAM_LEN, "0", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 8, 2048, 0, "ID.SPINCTRL_STREAM_LEN")
                ui.SpinCtrl_stream_len:SetToolTip("This value determines a size of buffer used in the paricular streams.")
                ui.FlexGridSizer2:Add(ui.SpinCtrl_stream_len, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText9 = wx.wxStaticText(this, ID.STATICTEXT9, "Length of pipe buffer [bytes]", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT9")
                ui.FlexGridSizer2:Add(ui.StaticText9, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.SpinCtrl_pipe_len = wx.wxSpinCtrl(this, ID.SPINCTRL_PIPE_LEN, "0", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 8, 2048, 0, "ID.SPINCTRL_PIPE_LEN")
                ui.SpinCtrl_pipe_len:SetToolTip("This value determines a size of buffer used in the each pipe (fifo file).")
                ui.FlexGridSizer2:Add(ui.SpinCtrl_pipe_len, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText14 = wx.wxStaticText(this, ID.STATICTEXT14, "Allocation block size [bytes]", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT14")
                ui.FlexGridSizer2:Add(ui.StaticText14, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.SpinCtrl_mem_block = wx.wxSpinCtrl(this, ID.SPINCTRL_MEM_BLOCK, "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 4, 4096, 0, "ID.SPINCTRL_MEM_BLOCK")
                ui.SpinCtrl_mem_block:SetToolTip("The allocation block size is a minimal memory block that can be allocated by the Dynamic Memory Management (e.g. malloc function).")
                ui.FlexGridSizer2:Add(ui.SpinCtrl_mem_block, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText10 = wx.wxStaticText(this, ID.STATICTEXT10, "Length of error messages", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT10")
                ui.FlexGridSizer2:Add(ui.StaticText10, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_errno_size = wx.wxChoice(this, ID.CHOICE_ERRNO_SIZE, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_ERRNO_SIZE")
                ui.Choice_errno_size:SetToolTip("The error messages are used to deliver users an information about error that occured by using specified functions. "..
                                                "This option is used to translate errno error number to user friedly strings.")
                ui.Choice_errno_size:Append("Disabled (low memory usage)")
                ui.Choice_errno_size:Append("Only numbers (small memory usage)")
                ui.Choice_errno_size:Append("Abbreviations (medium memory usage)")
                ui.Choice_errno_size:Append("Full names (high memory usage)")
                ui.FlexGridSizer2:Add(ui.Choice_errno_size, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticBoxSizer6:Add(ui.FlexGridSizer2, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer6, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- Hostname group box
                ui.StaticBoxSizer7 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Host name")
                ui.TextCtrl_hostname = wx.wxTextCtrl(this, ID.TEXTCTRL_HOSTNAME, "", wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, -1), 0, wx.wxDefaultValidator, "ID.TEXTCTRL_HOSTNAME")
                ui.TextCtrl_hostname:SetToolTip("This string configures system name in the e.g. network.")
                ui.StaticBoxSizer7:Add(ui.TextCtrl_hostname, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer7, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- RTC group box
                ui.StaticBoxSizer8 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "RTC device path")
                ui.TextCtrl_RTC_path = wx.wxTextCtrl(this, ID.TEXTCTRL_RTCPATH, "", wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, -1))
                ui.TextCtrl_RTC_path:SetToolTip("This is RTC device path. By using this file (device) the system will read the time.")
                ui.StaticBoxSizer8:Add(ui.TextCtrl_RTC_path, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer8, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- layout configuration
                this:SetSizer(ui.FlexGridSizer1)
                this:SetScrollRate(50, 50)

                -- signals
                this:Connect(ID.SPINCTRL_TASK_STACK_SIZE,      wx.wxEVT_COMMAND_SPINCTRL_UPDATED, stack_value_changed )
                this:Connect(ID.SPINCTRL_FS_STACK_SIZE,        wx.wxEVT_COMMAND_SPINCTRL_UPDATED, stack_value_changed )
                this:Connect(ID.SPINCTRL_INITD_STACK_SIZE,     wx.wxEVT_COMMAND_SPINCTRL_UPDATED, value_changed       )
                this:Connect(ID.SPINCTRL_IRQ_STACK_SIZE,       wx.wxEVT_COMMAND_SPINCTRL_UPDATED, stack_value_changed )
                this:Connect(ID.SPINCTRL_NUMBER_OF_PRIORITIES, wx.wxEVT_COMMAND_SPINCTRL_UPDATED, value_changed       )
                this:Connect(ID.SPINCTRL_TASK_NAME_LEN,        wx.wxEVT_COMMAND_SPINCTRL_UPDATED, value_changed       )
                this:Connect(ID.SPINCTRL_SWITCH_FREQ,          wx.wxEVT_COMMAND_SPINCTRL_UPDATED, value_changed       )
                this:Connect(ID.CHECKBOX_SLEEP_ON_IDLE,        wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_changed       )
                this:Connect(ID.CHECKBOX_PRINTK,               wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_changed       )
                this:Connect(ID.CHECKBOX_PRINTF,               wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_changed       )
                this:Connect(ID.CHECKBOX_SCANF,                wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_changed       )
                this:Connect(ID.CHECKBOX_COLOR_TERM,           wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_changed       )
                this:Connect(ID.CHECKBOX_STOP_MACRO,           wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_changed       )
                this:Connect(ID.CHECKBOX_TASK_MEMMON,          wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_changed       )
                this:Connect(ID.CHECKBOX_TASK_FILEMON,         wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_changed       )
                this:Connect(ID.CHECKBOX_KRN_MEMMON,           wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_changed       )
                this:Connect(ID.CHECKBOX_MOD_MEMMON,           wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_changed       )
                this:Connect(ID.CHECKBOX1_SYS_MEMMON,          wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_changed       )
                this:Connect(ID.CHECKBOX_CPU_LOADMON,          wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_changed       )
                this:Connect(ID.CHECKBOX_NET_MEMMON,           wx.wxEVT_COMMAND_CHECKBOX_CLICKED, net_memmon_changed  )
                this:Connect(ID.SPINCTRL_NET_MEM_LIMIT,        wx.wxEVT_COMMAND_SPINCTRL_UPDATED, value_changed       )
                this:Connect(ID.SPINCTRL_STREAM_LEN,           wx.wxEVT_COMMAND_SPINCTRL_UPDATED, value_changed       )
                this:Connect(ID.SPINCTRL_PIPE_LEN,             wx.wxEVT_COMMAND_SPINCTRL_UPDATED, value_changed       )
                this:Connect(ID.SPINCTRL_MEM_BLOCK,            wx.wxEVT_COMMAND_SPINCTRL_UPDATED, value_changed       )
                this:Connect(ID.CHOICE_ERRNO_SIZE,             wx.wxEVT_COMMAND_CHOICE_SELECTED,  value_changed       )
                this:Connect(ID.TEXTCTRL_HOSTNAME,             wx.wxEVT_COMMAND_TEXT_UPDATED,     value_changed       )
                this:Connect(ID.TEXTCTRL_RTCPATH,              wx.wxEVT_COMMAND_TEXT_UPDATED,     value_changed       )
        end

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @param  None
-- @return Module name
--------------------------------------------------------------------------------
function operating_system:get_window_name()
        return "Operating System"
end


--------------------------------------------------------------------------------
-- @brief  Function is called when window is selected
-- @return None
--------------------------------------------------------------------------------
function operating_system:refresh()
        load_configuration()
        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Function check if options are modified
-- @return true if options are modified, otherwise false
--------------------------------------------------------------------------------
function operating_system:is_modified()
        return modified:get_value()
end


--------------------------------------------------------------------------------
-- @brief  Function save configuration
-- @return None
--------------------------------------------------------------------------------
function operating_system:save()
        save_configuration()
end

--------------------------------------------------------------------------------
-- @brief  Function save configuration
-- @return Priority range: min, max
--------------------------------------------------------------------------------
function operating_system:get_priority_range()
        local number_of_priorities = tonumber(ct:key_read(config.project.key.OS_TASK_MAX_PRIORITIES))
        return -math.floor(number_of_priorities / 2), math.floor(number_of_priorities / 2)
end
