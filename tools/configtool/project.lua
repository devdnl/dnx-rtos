require("wx")
require("wizcore")

project  = {}
local ui = {}
local ID = {}
ID.WINDOW                  = wx.wxNewId()
ID.STATICLINE1             = wx.wxNewId()
ID.STATICTEXT2             = wx.wxNewId()
ID.TEXTCTRL_PROJECT_NAME   = wx.wxNewId()
ID.TEXTCTRL_TOOLCHAIN_NAME = wx.wxNewId()
ID.STATICLINE2             = wx.wxNewId()
ID.BUTTON_SAVE             = wx.wxNewId()
ID.CHOICE_CPU_ARCH         = wx.wxNewId()
ID.CHOICE_CPU_NAME         = wx.wxNewId()
ID.CHOICE_DEFAULT_IRQ_PRIO = wx.wxNewId()
ID.SPINCTRL_OSC_FREQ       = wx.wxNewId()


local function set_cpu_specific_controls(cpu_arch)
        if cpu_arch:match(wizcore.PROJECT.DEF.STM32F1) then
                ui.Choice_CPU_arch:SetSelection(wizcore:get_string_index(wizcore.PROJECT.ARCH_LIST, wizcore.PROJECT.DEF.STM32F1) - 1)
                ui.Choice_CPU_arch.OldSelection = ui.Choice_CPU_arch:GetSelection()

                local micro = wizcore:key_read(wizcore.ARCH.STM32F1.KEY.CPU_NAME)
                local midx  = wizcore:get_string_index(wizcore.ARCH.STM32F1.CPU_LIST, micro)

                if midx == -1 then
                        wizcore:show_error_msg("Unknown CPU name", micro..": Unknown CPU name!")
                else
                        if ui.Choice_CPU_name:IsEmpty() then
                                ui.Choice_CPU_name:Append(wizcore.ARCH.STM32F1.CPU_LIST)
                        end
                        ui.Choice_CPU_name:SetSelection(midx - 1)
                end

                local prio = math.floor(tonumber(wizcore:key_read(wizcore.PROJECT.KEY.IRQ_USER_PRIORITY))/16)
                if ui.Choice_default_irq_prio:IsEmpty() then
                        ui.Choice_default_irq_prio:Append(wizcore.ARCH.STM32F1.CPU_PRIORITY_LIST)
                end
                ui.Choice_default_irq_prio:SetSelection(prio)

        else
                wizcore:show_error_msg("Unknown CPU architecture", wizcore.PROJECT.FILE.FLAGS_H..": Unknown CPU architecture")
        end
end


local function load_controls()
        print("load_controls()")

        local project_name   = wizcore:key_read(wizcore.PROJECT.KEY.PROJECT_NAME)
        local toolchain_name = wizcore:key_read(wizcore.PROJECT.KEY.PROJECT_TOOLCHAIN)
        local cpu_arch       = wizcore:key_read(wizcore.PROJECT.KEY.PROJECT_CPU_ARCH)
        local cpu_osc_freq   = wizcore:key_read(wizcore.PROJECT.KEY.CPU_OSC_FREQ)

        ui.TextCtrl_project_name:SetValue(project_name)
        ui.TextCtrl_toolchain_name:SetValue(toolchain_name)

        set_cpu_specific_controls(cpu_arch)

        ui.SpinCtrl_osc_freq:SetValue(cpu_osc_freq)
        ui.Button_save:Enable(true)
end


local function on_button_save_click()
        print("on_button_save_click()")

        wizcore:key_write(wizcore.PROJECT.KEY.PROJECT_NAME, ui.TextCtrl_project_name:GetValue())
        wizcore:key_write(wizcore.PROJECT.KEY.PROJECT_TOOLCHAIN, ui.TextCtrl_toolchain_name:GetValue())
        wizcore:key_write(wizcore.PROJECT.KEY.CPU_OSC_FREQ, tostring(ui.SpinCtrl_osc_freq:GetValue()))

        local arch = wizcore.PROJECT.ARCH_LIST[ui.Choice_CPU_arch:GetSelection() + 1]
        wizcore:key_write(wizcore.PROJECT.KEY.PROJECT_CPU_ARCH, arch)
        wizcore:key_write(wizcore.PROJECT.KEY.CPU_ARCH, arch)

        if arch:match(wizcore.PROJECT.DEF.STM32F1) then
                wizcore:key_write(wizcore.ARCH.STM32F1.KEY.CPU_NAME, wizcore.ARCH.STM32F1.CPU_LIST[ui.Choice_CPU_name:GetSelection() + 1])
                wizcore:key_write(wizcore.PROJECT.KEY.IRQ_USER_PRIORITY, wizcore.ARCH.STM32F1.CPU_LIST[ui.Choice_CPU_name:GetSelection() + 1])
                wizcore:key_write(wizcore.PROJECT.KEY.IRQ_USER_PRIORITY, string.format('0x%X', bit.bor(ui.Choice_default_irq_prio:GetSelection() * 16, 0xF)))
        end

        ui.Button_save:Enable(false)
end


local function textctrl_updated()
        print("textctrl_updated()")
        ui.Button_save:Enable(true)
end


local function choice_cpu_arch_selected(this)
        print("choice_cpu_arch_selected()")
        
        if ui.Choice_CPU_arch.OldSelection ~= this:GetSelection() then
                ui.Choice_CPU_arch.OldSelection = this:GetSelection()
                ui.Choice_CPU_name:Clear()
                ui.Choice_default_irq_prio:Clear()
                set_cpu_specific_controls(wizcore.PROJECT.ARCH_LIST[this:GetSelection() + 1])
                ui.Button_save:Enable(true)
        end
end


local function choice_cpu_name_selected(this)
        print("choice_cpu_name_selected()")
        ui.Button_save:Enable(true)
end


local function choice_cpu_prio_selected(this)
        print("choice_cpu_prio_selected()")
        ui.Button_save:Enable(true)
end


local function spinctrl_osc_freq_updated(this)
        print("spinctrl_osc_freq_updated()")
        ui.Button_save:Enable(true)
end


function project:create_window(parent)
        if ui.window == nil then
                ui.window  = wx.wxScrolledWindow(parent, ID.WINDOW)
                local this = ui.window

                ui.FlexGridSizer1 = wx.wxFlexGridSizer(3, 1, 0, 0)

                -- Project name groupbox
                ui.StaticBoxSizer_project_name = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Project name")
                ui.TextCtrl_project_name = wx.wxTextCtrl(this, ID.TEXTCTRL_PROJECT_NAME, "", wx.wxDefaultPosition, wx.wxSize(500, -1))
                ui.TextCtrl_project_name:SetToolTip("This is a name of your project.")
                ui.StaticBoxSizer_project_name:Add(ui.TextCtrl_project_name, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer_project_name, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticBoxSizer_toochain_name = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Toolchain name")
                ui.TextCtrl_toolchain_name = wx.wxTextCtrl(this, ID.TEXTCTRL_TOOLCHAIN_NAME, "", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.TextCtrl_toolchain_name:SetToolTip("Enter a name of first part of your compiler name, example:\n - Linaro, CodeSourcery: arm-none-eabi-")
                ui.StaticBoxSizer_toochain_name:Add(ui.TextCtrl_toolchain_name, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer_toochain_name, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticBoxSizer1 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "CPU architecture and family")
                ui.Choice_CPU_arch = wx.wxChoice(this, ID.CHOICE_CPU_ARCH, wx.wxDefaultPosition, wx.wxDefaultSize, wizcore.PROJECT.ARCH_LIST, 0);
                ui.StaticBoxSizer1:Add(ui.Choice_CPU_arch, 1, bit.bor(wx.wxALL, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer1, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticBoxSizer3 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Microcontroller selection")
                ui.Choice_CPU_name = wx.wxChoice(this, ID.CHOICE_CPU_NAME, wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.StaticBoxSizer3:Add(ui.Choice_CPU_name, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer3, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticBoxSizer_default_irq_prio = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Default priority value for user\'s interrupts")
                ui.Choice_default_irq_prio = wx.wxChoice(this, ID.CHOICE_DEFAULT_IRQ_PRIO, wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.StaticBoxSizer_default_irq_prio:Add(ui.Choice_default_irq_prio, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer_default_irq_prio, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticBoxSizer2 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Oscillator frequency")
                ui.SpinCtrl_osc_freq = wx.wxSpinCtrl(this, ID.SPINCTRL_OSC_FREQ, "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1000, 100000000)
                ui.SpinCtrl_osc_freq:SetToolTip("This is a frequency of a connected to the microcontroller external generator or crystal.")
                ui.StaticBoxSizer2:Add(ui.SpinCtrl_osc_freq, 8, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText2 = wx.wxStaticText(this, ID.STATICTEXT2, "Hz", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.StaticBoxSizer2:Add(ui.StaticText2, 1, bit.bor(wx.wxALL, wx.wxALIGN_LEFT, wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer2, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticLine2 = wx.wxStaticLine(this, ID.STATICLINE2, wx.wxDefaultPosition, wx.wxSize(10,-1), wx.wxLI_HORIZONTAL)
                ui.FlexGridSizer1:Add(ui.StaticLine2, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 0)

                ui.Button_save = wx.wxButton(this, ID.BUTTON_SAVE, "&Save", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer1:Add(ui.Button_save, 1, bit.bor(wx.wxALL, wx.wxALIGN_RIGHT, wx.wxALIGN_CENTER_VERTICAL), 5)

                this:SetSizer(ui.FlexGridSizer1)
                this:SetScrollRate(5, 5)

                this:Connect(ID.BUTTON_SAVE,             wx.wxEVT_COMMAND_BUTTON_CLICKED,   on_button_save_click     )
                this:Connect(ID.TEXTCTRL_PROJECT_NAME,   wx.wxEVT_COMMAND_TEXT_UPDATED,     textctrl_updated         )
                this:Connect(ID.TEXTCTRL_TOOLCHAIN_NAME, wx.wxEVT_COMMAND_TEXT_UPDATED,     textctrl_updated         )
                this:Connect(ID.CHOICE_CPU_ARCH,         wx.wxEVT_COMMAND_CHOICE_SELECTED,  choice_cpu_arch_selected )
                this:Connect(ID.CHOICE_CPU_NAME,         wx.wxEVT_COMMAND_CHOICE_SELECTED,  choice_cpu_name_selected )
                this:Connect(ID.CHOICE_DEFAULT_IRQ_PRIO, wx.wxEVT_COMMAND_CHOICE_SELECTED,  choice_cpu_prio_selected )
                this:Connect(ID.SPINCTRL_OSC_FREQ,       wx.wxEVT_COMMAND_SPINCTRL_UPDATED, spinctrl_osc_freq_updated)
                
                load_controls()
        end

        return ui.window
end


function project:get_window_name()
        return "Project"
end


function project:refresh()
        print("project:refresh()")
        load_controls()
        ui.Button_save:Enable(false)
end


function project:is_modified()
        return ui.Button_save:IsEnabled()
end
