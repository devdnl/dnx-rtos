--[[============================================================================
@file    project.lua

@author  Daniel Zorychta

@brief   This file is the configuration script for general project settings

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
-- PUBLIC OBJECTS
--==============================================================================
project  = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local modified = ct:new_modify_indicator()
local ui = {}
local ID = {}
ID.WINDOW                  = wx.wxNewId()
ID.STATICTEXT2             = wx.wxNewId()
ID.TEXTCTRL_PROJECT_NAME   = wx.wxNewId()
ID.TEXTCTRL_TOOLCHAIN_NAME = wx.wxNewId()
ID.CHOICE_CPU_ARCH         = wx.wxNewId()
ID.CHOICE_CPU_NAME         = wx.wxNewId()
ID.CHOICE_DEFAULT_IRQ_PRIO = wx.wxNewId()
ID.SPINCTRL_OSC_FREQ       = wx.wxNewId()

local last_cpu_arch
local last_cpu_name
local last_priority


--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function loads CPU list according to selected CPU architecture
-- @param  cpu_arch
-- @return None
--------------------------------------------------------------------------------
local function load_cpu_list(cpu_arch)
        if cpu_arch ~= last_cpu_arch then
                -- load CPU architecture list if not loaded
                if ui.Choice_CPU_arch:GetCount() == 0 then
                        ui.Choice_CPU_arch:Clear()
                        for i = 1, config.arch:NumChildren() do
                                ui.Choice_CPU_arch:Append(config.arch:Children()[i]:GetName())
                        end
                end

                -- select CPU architecture from existing architectures
                for i = 1, config.arch:NumChildren() do
                        local arch_name = config.arch:Children()[i]:GetName()
                        if arch_name == cpu_arch then
                                ui.Choice_CPU_arch:SetSelection(i - 1)
                                break
                        end
                end
        end
end


--------------------------------------------------------------------------------
-- @brief  Function loads microcontroller list of selected CPU architecture
-- @param  cpu_arch
-- @return None
--------------------------------------------------------------------------------
local function load_cpu_name_list(cpu_name, cpu_arch)
        -- load CPU name list if CPU architecture was changed
        if last_cpu_arch ~= cpu_arch then
                ui.Choice_CPU_name:Clear()
                for i, cpu in pairs(config.arch[cpu_arch].cpulist.cpu) do
                        ui.Choice_CPU_name:Append(cpu.name:GetValue())
                end
        end

        -- select specified CPU
        if last_cpu_name ~= cpu_name or last_cpu_arch ~= cpu_arch then
                for i, cpu in pairs(config.arch[cpu_arch].cpulist.cpu) do
                        local name = cpu.name:GetValue()

                        if name == cpu_name then
                                ui.Choice_CPU_name:SetSelection(i - 1)
                                break
                        end
                end
        end
end


--------------------------------------------------------------------------------
-- @brief  Function loads priority list of selected CPU architecture
-- @param  cpu_arch
-- @return None
--------------------------------------------------------------------------------
local function load_cpu_priorities(cpu_arch)
        -- load priority list
        if last_cpu_arch ~= cpu_arch then
                ui.Choice_default_irq_prio:Clear()
                for i, priority in pairs(ct:get_priority_list(cpu_arch)) do
                        ui.Choice_default_irq_prio:Append(priority.name)
                end
        end

        -- select priority
        local user_priority = ct:key_read(config.project.key.IRQ_USER_PRIORITY)
        for i, priority in pairs(ct:get_priority_list(cpu_arch)) do
                if user_priority == priority.value then
                        ui.Choice_default_irq_prio:SetSelection(i - 1)
                        break
                end
        end
end


--------------------------------------------------------------------------------
-- @brief  Function loads all controls from configuration files
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_configuration()
        local project_name   = ct:key_read(config.project.key.PROJECT_NAME)
        local toolchain_name = ct:key_read(config.project.key.PROJECT_TOOLCHAIN)
        local cpu_arch       = ct:key_read(config.project.key.PROJECT_CPU_ARCH)
        local cpu_osc_freq   = ct:key_read(config.project.key.CPU_OSC_FREQ)
        local cpu_name       = ct:key_read(config.arch[cpu_arch].key.CPU_NAME)

        ui.TextCtrl_project_name:SetValue(project_name)
        ui.TextCtrl_toolchain_name:SetValue(toolchain_name)
        ui.SpinCtrl_osc_freq:SetValue(cpu_osc_freq)

        load_cpu_list(cpu_arch)
        load_cpu_name_list(cpu_name, cpu_arch)
        load_cpu_priorities(cpu_arch)

        last_cpu_arch = cpu_arch
        last_cpu_name = cpu_name

        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Function handle Save button click event (configuration save)
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function save_configuration()
        local cpu_arch     = config.arch:Children()[ui.Choice_CPU_arch:GetSelection() + 1]:GetName()
        local cpu_name     = config.arch[cpu_arch].cpulist:Children()[ui.Choice_CPU_name:GetSelection() + 1].name:GetValue()
        local cpu_family   = config.arch[cpu_arch].cpulist:Children()[ui.Choice_CPU_name:GetSelection() + 1].family:GetValue()
        local cpu_priority = config.arch[cpu_arch].priorities.priority[ui.Choice_default_irq_prio:GetSelection() + 1].value:GetValue()

        ct:key_write(config.project.key.PROJECT_NAME, ui.TextCtrl_project_name:GetValue())
        ct:key_write(config.project.key.PROJECT_TOOLCHAIN, ui.TextCtrl_toolchain_name:GetValue())
        ct:key_write(config.project.key.CPU_OSC_FREQ, tostring(ui.SpinCtrl_osc_freq:GetValue()))
        ct:key_write(config.project.key.PROJECT_CPU_ARCH, cpu_arch)
        ct:key_write(config.project.key.CPU_ARCH, cpu_arch)
        ct:key_write(config.project.key.IRQ_USER_PRIORITY, cpu_priority)
        ct:key_write(config.arch[cpu_arch].key.CPU_NAME, cpu_name)             -- CPU name in the cpu.h file (one for each architecture)
        ct:key_write(config.arch[cpu_arch].key.CPUCONFIG_CPUNAME, cpu_name)    -- CPU name in the Makefile (one for each architecture)
        ct:key_write(config.arch[cpu_arch].key.CPU_FAMILY, cpu_family)

        -- disables all peripherals that are not assigned to selected microcontroller
        local cpu_idx    = ct:get_cpu_index(cpu_arch, cpu_name)
        local cpu        = config.arch[cpu_arch].cpulist:Children()[cpu_idx]
        local cpu_periph = {}

        for i = 1, cpu.peripherals:NumChildren() do
                cpu_periph[i] = cpu.peripherals:Children()[i]:GetName()
        end

        for i = 1, config.project.modules:NumChildren() do
                local module = config.project.modules:Children()[i].name:GetValue()
                local exist  = false

                for j, name in ipairs(cpu_periph) do
                        if name == module then
                                exist = true
                                break
                        end
                end

                if not exist then
                        ct:enable_module(module, exist)
                end
        end

        -- info about changed configuration
        ct:show_info_msg(ct.MAIN_WINDOW_NAME, "The CPU configuration was changed. Make sure that the specific peripherals assigned to the selected microcontroller are correctly configured.", ui.window)

        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Function handle event of all TextCtrls when text was updated
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function textctrl_updated()
        modified:yes()
end


--------------------------------------------------------------------------------
-- @brief  Function handle event of CPU architecture change
-- @param  this     event object
-- @return None
--------------------------------------------------------------------------------
local function choice_cpu_arch_selected(event)
        local cpu_arch = event:GetString(event:GetSelection())
        local cpu_name = ct:key_read(config.arch[cpu_arch].key.CPU_NAME)

        load_cpu_list(cpu_arch)
        load_cpu_name_list(cpu_name, cpu_arch)
        load_cpu_priorities(cpu_arch)

        last_cpu_arch = cpu_arch
        last_cpu_name = cpu_name

        modified:yes()
end


--------------------------------------------------------------------------------
-- @brief  Function handle event of CPU name change
-- @param  this     event object
-- @return None
--------------------------------------------------------------------------------
local function choice_cpu_name_selected(this)
        modified:yes()
end


--------------------------------------------------------------------------------
-- @brief  Function handle event of CPU priority change
-- @param  this     event object
-- @return None
--------------------------------------------------------------------------------
local function choice_cpu_prio_selected(this)
        modified:yes()
end


--------------------------------------------------------------------------------
-- @brief  Function handle event of oscillator frequency update
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function spinctrl_osc_freq_updated()
        modified:yes()
end


--==============================================================================
-- GLOBAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function creates new widgets
-- @param  parent       parent window
-- @return New window object
--------------------------------------------------------------------------------
function project:create_window(parent)
        if ui.window == nil then
                ui.window  = wx.wxScrolledWindow(parent, ID.WINDOW)
                local this = ui.window

                ui.FlexGridSizer1 = wx.wxFlexGridSizer(3, 1, 0, 0)

                -- Project name groupbox
                ui.StaticBoxSizer_project_name = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Project name")
                ui.TextCtrl_project_name = wx.wxTextCtrl(this, ID.TEXTCTRL_PROJECT_NAME, "", wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, -1))
                ui.TextCtrl_project_name:SetToolTip("This is a name of your project. The string is used to generate names of output files.")
                ui.StaticBoxSizer_project_name:Add(ui.TextCtrl_project_name, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer_project_name, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 5)

                -- Toolchain name groupbox
                ui.StaticBoxSizer_toochain_name = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Toolchain name")
                ui.TextCtrl_toolchain_name = wx.wxTextCtrl(this, ID.TEXTCTRL_TOOLCHAIN_NAME, "", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.TextCtrl_toolchain_name:SetToolTip("Enter a name of first part of your toolchain's name, example:\n - Linaro, CodeSourcery: arm-none-eabi-")
                ui.StaticBoxSizer_toochain_name:Add(ui.TextCtrl_toolchain_name, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer_toochain_name, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 5)

                -- CPU architecture groupbox
                ui.StaticBoxSizer1 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "CPU architecture")
                ui.Choice_CPU_arch = wx.wxChoice(this, ID.CHOICE_CPU_ARCH, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0);
                ui.StaticBoxSizer1:Add(ui.Choice_CPU_arch, 1, bit.bor(wx.wxALL, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer1, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 5)

                -- Microcontroller selection groupbox
                ui.StaticBoxSizer3 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Microcontroller selection")
                ui.Choice_CPU_name = wx.wxChoice(this, ID.CHOICE_CPU_NAME, wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.StaticBoxSizer3:Add(ui.Choice_CPU_name, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer3, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 5)

                -- Default priority groupbox
                ui.StaticBoxSizer_default_irq_prio = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Default priority value for user\'s interrupts")
                ui.Choice_default_irq_prio = wx.wxChoice(this, ID.CHOICE_DEFAULT_IRQ_PRIO, wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.Choice_default_irq_prio:SetToolTip("This is a default priority for interrupts that are selected as 'System default' in the particular modules."..
                                                      "This value can be changed individually in the configuration of specified modules.")
                ui.StaticBoxSizer_default_irq_prio:Add(ui.Choice_default_irq_prio, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer_default_irq_prio, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 5)

                -- oscillator frequency selection
                ui.StaticBoxSizer2 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Oscillator frequency")
                ui.SpinCtrl_osc_freq = wx.wxSpinCtrl(this, ID.SPINCTRL_OSC_FREQ, "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1000, 100000000)
                ui.SpinCtrl_osc_freq:SetToolTip("This is a frequency of a connected to the microcontroller external generator or crystal.")
                ui.StaticBoxSizer2:Add(ui.SpinCtrl_osc_freq, 8, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText2 = wx.wxStaticText(this, ID.STATICTEXT2, "Hz", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.StaticBoxSizer2:Add(ui.StaticText2, 1, bit.bor(wx.wxALL, wx.wxALIGN_LEFT, wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer2, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 5)

                --
                this:SetSizer(ui.FlexGridSizer1)
                this:SetScrollRate(5, 5)

                -- event connections
                this:Connect(ID.TEXTCTRL_PROJECT_NAME,   wx.wxEVT_COMMAND_TEXT_UPDATED,     textctrl_updated         )
                this:Connect(ID.TEXTCTRL_TOOLCHAIN_NAME, wx.wxEVT_COMMAND_TEXT_UPDATED,     textctrl_updated         )
                this:Connect(ID.CHOICE_CPU_ARCH,         wx.wxEVT_COMMAND_CHOICE_SELECTED,  choice_cpu_arch_selected )
                this:Connect(ID.CHOICE_CPU_NAME,         wx.wxEVT_COMMAND_CHOICE_SELECTED,  choice_cpu_name_selected )
                this:Connect(ID.CHOICE_DEFAULT_IRQ_PRIO, wx.wxEVT_COMMAND_CHOICE_SELECTED,  choice_cpu_prio_selected )
                this:Connect(ID.SPINCTRL_OSC_FREQ,       wx.wxEVT_COMMAND_SPINCTRL_UPDATED, spinctrl_osc_freq_updated)

                load_configuration()
        end

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns window name
-- @return Window name
--------------------------------------------------------------------------------
function project:get_window_name()
        return "Project"
end


--------------------------------------------------------------------------------
-- @brief  Function is called when window is selected
-- @return None
--------------------------------------------------------------------------------
function project:refresh()
        load_configuration()
end


--------------------------------------------------------------------------------
-- @brief  Function check if options are modified
-- @return true if options are modified, otherwise false
--------------------------------------------------------------------------------
function project:is_modified()
        return modified:get_value()
end


--------------------------------------------------------------------------------
-- @brief  Function save configuration
-- @return None
--------------------------------------------------------------------------------
function project:save()
        save_configuration()
end

