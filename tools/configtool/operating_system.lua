require("wx")
require("wizcore")

operating_system = {}

local ui = {}

local ID = {}
ID.BUTTON_SAVE = wx.wxNewId()
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
ID.SPINCTRLIRQ_STACK_SIZE = wx.wxNewId()
ID.SPINCTRL_FS_STACK_SIZE = wx.wxNewId()
ID.SPINCTRL_MEM_BLOCK = wx.wxNewId()
ID.SPINCTRL_NET_MEM_LIMIT = wx.wxNewId()
ID.SPINCTRL_NUMBER_OF_PRIORITIES = wx.wxNewId()
ID.SPINCTRL_PIPE_LEN = wx.wxNewId()
ID.SPINCTRL_STREAM_LEN = wx.wxNewId()
ID.SPINCTRL_SWITCH_FREQ = wx.wxNewId()
ID.SPINCTRL_TASK_NAME_LEN = wx.wxNewId()
ID.SPINCTRL_TASL_STACK_SIZE = wx.wxNewId()
ID.STATICLINE1 = wx.wxNewId()
ID.STATICLINE2 = wx.wxNewId()
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
ID.STATICTEXT_HEADER = wx.wxNewId()
ID.STATICTEXT_TOTAL_STACK_SIZE = wx.wxNewId()
ID.TEXTCTRL_HOSTNAME = wx.wxNewId()


local function load_controls()

end


function operating_system:create_window(parent)
        if ui.window == nil then
                ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
                local this = ui.window

                ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)

                -- stack group box
                ui.StaticBoxSizer1 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Stacks")
                ui.FlexGridSizer3 = wx.wxFlexGridSizer(0, 2, 0, 0)
                ui.StaticText2 = wx.wxStaticText(this, ID.STATICTEXT2, "Minimal task stack size", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer3:Add(ui.StaticText2, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.SpinCtrl_task_stack_size = wx.wxSpinCtrl(this, ID.SPINCTRL_TASL_STACK_SIZE, "0", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 48, 8192, 0)
                ui.FlexGridSizer3:Add(ui.SpinCtrl_task_stack_size, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText3 = wx.wxStaticText(this, ID.STATICTEXT3, "File system stack size", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer3:Add(ui.StaticText3, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.SpinCtrl_fs_stack_size = wx.wxSpinCtrl(this, ID.SPINCTRL_FS_STACK_SIZE, "0", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 16, 2048, 0)
                ui.FlexGridSizer3:Add(ui.SpinCtrl_fs_stack_size, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText4 = wx.wxStaticText(this, ID.STATICTEXT4, "Interrupt stack size", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT4")
                ui.FlexGridSizer3:Add(ui.StaticText4, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.SpinCtrl_irq_stack_size = wx.wxSpinCtrl(this, ID.SPINCTRLIRQ_STACK_SIZE, "0", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 16, 2048, 0, "ID.SPINCTRLIRQ_STACK_SIZE")
                ui.FlexGridSizer3:Add(ui.SpinCtrl_irq_stack_size, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText5 = wx.wxStaticText(this, ID.STATICTEXT5, "Total minimal stack size:", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT5")
                ui.FlexGridSizer3:Add(ui.StaticText5, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText_total_stack_size = wx.wxStaticText(this, ID.STATICTEXT_TOTAL_STACK_SIZE, "0 levels", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT_TOTAL_STACK_SIZE")
                ui.FlexGridSizer3:Add(ui.StaticText_total_stack_size, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticBoxSizer1:Add(ui.FlexGridSizer3, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- task management group box
                ui.StaticBoxSizer2 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Task management")
                ui.FlexGridSizer4 = wx.wxFlexGridSizer(0, 2, 0, 0)

                ui.StaticText11 = wx.wxStaticText(this, ID.STATICTEXT11, "Number of task priorities", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT11")
                ui.FlexGridSizer4:Add(ui.StaticText11, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.SpinCtrl_number_of_priorities = wx.wxSpinCtrl(this, ID.SPINCTRL_NUMBER_OF_PRIORITIES, "0", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 3, 255, 0, "ID.SPINCTRL_NUMBER_OF_PRIORITIES")
                ui.FlexGridSizer4:Add(ui.SpinCtrl_number_of_priorities, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText12 = wx.wxStaticText(this, ID.STATICTEXT12, "Length of task name [bytes]", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT12")
                ui.FlexGridSizer4:Add(ui.StaticText12, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.SpinCtrl_task_name_len = wx.wxSpinCtrl(this, ID.SPINCTRL_TASK_NAME_LEN, "0", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 8, 256, 0, "ID.SPINCTRL_TASK_NAME_LEN")
                ui.FlexGridSizer4:Add(ui.SpinCtrl_task_name_len, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText13 = wx.wxStaticText(this, ID.STATICTEXT13, "Context switch frequency [Hz]", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT13")
                ui.FlexGridSizer4:Add(ui.StaticText13, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.SpinCtrl_switch_freq = wx.wxSpinCtrl(this, ID.SPINCTRL_SWITCH_FREQ, "0", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 1000, 0, "ID.SPINCTRL_SWITCH_FREQ")
                ui.FlexGridSizer4:Add(ui.SpinCtrl_switch_freq, 10, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticBoxSizer2:Add(ui.FlexGridSizer4, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- Miscellaneous group box
                ui.StaticBoxSizer5 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Features")
                ui.GridSizer2 = wx.wxGridSizer(0, 2, 0, 0)
                ui.CheckBox_sleep_on_idle = wx.wxCheckBox(this, ID.CHECKBOX_SLEEP_ON_IDLE, "Sleep on idle", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_SLEEP_ON_IDLE")
                ui.GridSizer2:Add(ui.CheckBox_sleep_on_idle, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_printk = wx.wxCheckBox(this, ID.CHECKBOX_PRINTK, "pirntk() function", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_PRINTK")
                ui.GridSizer2:Add(ui.CheckBox_printk, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_printf = wx.wxCheckBox(this, ID.CHECKBOX_PRINTF, "printf() family functions", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_PRINTF")
                ui.GridSizer2:Add(ui.CheckBox_printf, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_scanf = wx.wxCheckBox(this, ID.CHECKBOX_SCANF, "scanf() family functions", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_SCANF")
                ui.GridSizer2:Add(ui.CheckBox_scanf, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_color_term = wx.wxCheckBox(this, ID.CHECKBOX_COLOR_TERM, "Color terminal", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_COLOR_TERM")
                ui.GridSizer2:Add(ui.CheckBox_color_term, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_stop_macro = wx.wxCheckBox(this, ID.CHECKBOX_STOP_MACRO, "Stop macro (development)", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_STOP_MACRO")
                ui.GridSizer2:Add(ui.CheckBox_stop_macro, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_task_memmon = wx.wxCheckBox(this, ID.CHECKBOX_TASK_MEMMON, "Task memory monitoring", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_TASK_MEMMON")
                ui.GridSizer2:Add(ui.CheckBox_task_memmon, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_task_filemon = wx.wxCheckBox(this, ID.CHECKBOX_TASK_FILEMON, "Task file monitoring", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_TASK_FILEMON")
                ui.GridSizer2:Add(ui.CheckBox_task_filemon, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_krn_memmon = wx.wxCheckBox(this, ID.CHECKBOX_KRN_MEMMON, "Kernel memory monitoring", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_KRN_MEMMON")
                ui.GridSizer2:Add(ui.CheckBox_krn_memmon, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_mod_memmon = wx.wxCheckBox(this, ID.CHECKBOX_MOD_MEMMON, "Module memory monitoring", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_MOD_MEMMON")
                ui.GridSizer2:Add(ui.CheckBox_mod_memmon, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_sys_memmon = wx.wxCheckBox(this, ID.CHECKBOX1_SYS_MEMMON, "System memory monitoring", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX1_SYS_MEMMON")
                ui.GridSizer2:Add(ui.CheckBox_sys_memmon, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_CPU_loadmon = wx.wxCheckBox(this, ID.CHECKBOX_CPU_LOADMON, "CPU load monitoring", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_CPU_LOADMON")
                ui.GridSizer2:Add(ui.CheckBox_CPU_loadmon, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_net_memmon = wx.wxCheckBox(this, ID.CHECKBOX_NET_MEMMON, "Network memory monitoring", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_NET_MEMMON")
                ui.GridSizer2:Add(ui.CheckBox_net_memmon, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.SpinCtrl_net_mem_limit = wx.wxSpinCtrl(this, ID.SPINCTRL_NET_MEM_LIMIT, "0", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 16777216, 0, "ID.SPINCTRL_NET_MEM_LIMIT")
                ui.GridSizer2:Add(ui.SpinCtrl_net_mem_limit, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticBoxSizer5:Add(ui.GridSizer2, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer5, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- Sizes of buffers group box
                ui.StaticBoxSizer6 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Blocks and buffers")
                ui.FlexGridSizer2 = wx.wxFlexGridSizer(0, 2, 0, 0)

                ui.StaticText8 = wx.wxStaticText(this, ID.STATICTEXT8, "Stream buffer size [bytes]", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT8")
                ui.FlexGridSizer2:Add(ui.StaticText8, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.SpinCtrl_stream_len = wx.wxSpinCtrl(this, ID.SPINCTRL_STREAM_LEN, "0", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 8, 2048, 0, "ID.SPINCTRL_STREAM_LEN")
                ui.FlexGridSizer2:Add(ui.SpinCtrl_stream_len, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText9 = wx.wxStaticText(this, ID.STATICTEXT9, "Length of pipe buffer [bytes]", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT9")
                ui.FlexGridSizer2:Add(ui.StaticText9, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.SpinCtrl_pipe_len = wx.wxSpinCtrl(this, ID.SPINCTRL_PIPE_LEN, "0", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 8, 2048, 0, "ID.SPINCTRL_PIPE_LEN")
                ui.FlexGridSizer2:Add(ui.SpinCtrl_pipe_len, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText14 = wx.wxStaticText(this, ID.STATICTEXT14, "Allocation block size [bytes]", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT14")
                ui.FlexGridSizer2:Add(ui.StaticText14, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.SpinCtrl_mem_block = wx.wxSpinCtrl(this, ID.SPINCTRL_MEM_BLOCK, "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 4, 4096, 0, "ID.SPINCTRL_MEM_BLOCK")
                ui.FlexGridSizer2:Add(ui.SpinCtrl_mem_block, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticText10 = wx.wxStaticText(this, ID.STATICTEXT10, "Length of error messages", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT10")
                ui.FlexGridSizer2:Add(ui.StaticText10, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_errno_size = wx.wxChoice(this, ID.CHOICE_ERRNO_SIZE, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE_ERRNO_SIZE")
                ui.Choice_errno_size:Append("Disabled (low memory usage)")
                ui.Choice_errno_size:Append("Only numbers (small memory usage)")
                ui.Choice_errno_size:Append("Abbreviations (medium memory usage)")
                ui.Choice_errno_size:Append("Full names (high memory usage)")
                ui.FlexGridSizer2:Add(ui.Choice_errno_size, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticBoxSizer6:Add(ui.FlexGridSizer2, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer6, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- Hostname group box
                ui.StaticBoxSizer7 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Hostname")
                ui.TextCtrl_hostname = wx.wxTextCtrl(this, ID.TEXTCTRL_HOSTNAME, "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.TEXTCTRL_HOSTNAME")
                ui.StaticBoxSizer7:Add(ui.TextCtrl_hostname, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer7, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- line
                ui.StaticLine2 = wx.wxStaticLine(this, ID.STATICLINE2, wx.wxDefaultPosition, wx.wxSize(10,-1), wx.wxLI_HORIZONTAL, "ID.STATICLINE2")
                ui.FlexGridSizer1:Add(ui.StaticLine2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- Save button
                ui.Button_save = wx.wxButton(this, ID.BUTTON_SAVE, "Save", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.BUTTON_SAVE")
                ui.FlexGridSizer1:Add(ui.Button_save, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- layout configuration
                this:SetSizer(ui.FlexGridSizer1)
                this:SetScrollRate(25, 25)
        end

        return ui.window
end


function operating_system:get_window_name()
        return "Operating System"
end


function operating_system:refresh()
        load_controls()
end


function operating_system:is_modified()
        return false
end
