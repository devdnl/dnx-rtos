module(..., package.seeall)
require("wx")
require("wizcore")

gpio = {}

local ui = {}
local ID = {}
local periph

local port_mode_string = {"Output Push-Pull 2MHz",
                          "Output Push-Pull 10MHz",
                          "Output Push-Pull 50MHz",
                          "Output Open drain 2MHz",
                          "Output Open drain 10MHz",
                          "Output Open drain 50MHz",
                          "Alternative output Push-Pull 2MHz",
                          "Alternative output Push-Pull 10MHz",
                          "Alternative output Push-Pull 50MHz",
                          "Alternative output Open drain 2MHz",
                          "Alternative output Open drain 10MHz",
                          "Alternative output Open drain 50MHz",
                          "Analog input",
                          "Float input",
                          "Pulled input"}

local port_mode_config = {"_GPIO_OUT_PUSH_PULL_2MHZ",
                          "_GPIO_OUT_PUSH_PULL_10MHZ",
                          "_GPIO_OUT_PUSH_PULL_50MHZ",
                          "_GPIO_OUT_OPEN_DRAIN_2MHZ",
                          "_GPIO_OUT_OPEN_DRAIN_10MHZ",
                          "_GPIO_OUT_OPEN_DRAIN_50MHZ",
                          "_GPIO_ALT_OUT_PUSH_PULL_2MHZ",
                          "_GPIO_ALT_OUT_PUSH_PULL_10MHZ",
                          "_GPIO_ALT_OUT_PUSH_PULL_50MHZ",
                          "_GPIO_ALT_OUT_OPEN_DRAIN_2MHZ",
                          "_GPIO_ALT_OUT_OPEN_DRAIN_10MHZ",
                          "_GPIO_ALT_OUT_OPEN_DRAIN_50MHZ",
                          "_GPIO_IN_ANALOG",
                          "_GPIO_IN_FLOAT",
                          "_GPIO_IN_PULLED"}


local function load_controls()
        local port = ui.Choice_port:GetSelection() + 1

        for pin = 0, 15 do
                local pin_key = config.arch.stm32f1.key.GPIO

                pin_key.key:SetValue("__GPIO_P"..periph:Children()[port].name:GetValue().."_PIN_"..pin.."_NAME__")
                local pin_name = wizcore:key_read(pin_key)

                pin_key.key:SetValue("__GPIO_P"..periph:Children()[port].name:GetValue().."_PIN_"..pin.."_MODE__")
                local pin_mode = wizcore:key_read(pin_key)

                pin_key.key:SetValue("__GPIO_P"..periph:Children()[port].name:GetValue().."_PIN_"..pin.."_STATE__")
                local pin_state = wizcore:key_read(pin_key)

                ui.TextCtrl_pin_name[pin + 1]:SetValue(pin_name)
        end


        local gpio_enabled = wizcore:get_module_state("GPIO")

end


local function checkbox_changed(this)
        ui.Choice_port:Enable(this:IsChecked())

        for pin = 1, 16 do
                ui.StaticText_pin[pin]:Enable(this:IsChecked() and cpu.STM32F103C6xx.A[pin])
                ui.TextCtrl_pin_name[pin]:Enable(this:IsChecked() and cpu.STM32F103C6xx.A[pin])
                ui.Choice_mode[pin]:Enable(this:IsChecked() and cpu.STM32F103C6xx.A[pin])
                ui.Choice_state[pin]:Enable(this:IsChecked() and cpu.STM32F103C6xx.A[pin])
                ui.StaticText1:Enable(this:IsChecked())
                ui.StaticText2:Enable(this:IsChecked())
                ui.StaticText3:Enable(this:IsChecked())
                ui.StaticText4:Enable(this:IsChecked())
        end
end


local function port_number_changed(this)
        local port_name = {"A", "B", "C", "D"}

        local port = this:GetSelection() + 1

        for pin = 1, 16 do
                if cpu.STM32F103C6xx[port_name[port]][pin] then
                        ui.TextCtrl_pin_name[pin]:SetValue("PIN"..port_name[port]..pin-1)
                        ui.Choice_mode[pin]:SetSelection(5)
                        ui.Choice_state[pin]:SetSelection(1)
                else
                        ui.TextCtrl_pin_name[pin]:SetValue("")
                        ui.Choice_mode[pin]:SetSelection(-1)
                        ui.Choice_state[pin]:SetSelection(-1)
                end

                ui.StaticText_pin[pin]:Enable(cpu.STM32F103C6xx[port_name[port]][pin])
                ui.TextCtrl_pin_name[pin]:Enable(cpu.STM32F103C6xx[port_name[port]][pin])
                ui.Choice_mode[pin]:Enable(cpu.STM32F103C6xx[port_name[port]][pin])
                ui.Choice_state[pin]:Enable(cpu.STM32F103C6xx[port_name[port]][pin])
        end
end


local function port_mode_changed_hdl(pin, selection)
        ui.Choice_state[pin]:Clear()

        if selection >= 0 and selection <= 11 then
                ui.Choice_state[pin]:Append({"Low (0)", "High (1)"})
                ui.Choice_state[pin]:Enable(true)

        elseif selection >= 12 and selection <= 13  then
                ui.Choice_state[pin]:Enable(false)

        elseif selection == 14 then
                ui.Choice_state[pin]:Append({"Down", "Up"})
                ui.Choice_state[pin]:Enable(true)
        end

        ui.Choice_state[pin]:SetSelection(0)
end

local port_mode_changed = {}
for i = 1, 16 do port_mode_changed[i] = function(this) port_mode_changed_hdl(i, this:GetSelection()) end end


function gpio:create_window(parent)
        ui = {}
        ui.StaticText_pin    = {}
        ui.TextCtrl_pin_name = {}
        ui.Choice_mode       = {}
        ui.Choice_state      = {}

        ID = {}
        ID.CHECKBOX_ENABLE   = wx.wxNewId()
        ID.CHOICE_PORT       = wx.wxNewId()
        ID.BUTTON_SAVE       = wx.wxNewId()
        ID.TEXTCTRL_PIN_NAME = {}
        ID.CHOICE_MODE       = {}
        ID.CHOICE_STATE      = {}


        ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
        local this = ui.window

        ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)

        ui.CheckBox_enable = wx.wxCheckBox(this, ID.CHECKBOX_ENABLE, "Enable")
        ui.FlexGridSizer1:Add(ui.CheckBox_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- port selection choice
        ui.StaticBoxSizer1 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Port selection")
        ui.Choice_port = wx.wxChoice(this, ID.CHOICE_PORT, wx.wxDefaultPosition, wx.wxSize(wizcore.CONTROL_X_SIZE, -1), {}, 0)

        local cpu_name = wizcore:key_read(config.arch.stm32f1.key.CPU_NAME)
        local cpu_idx  = wizcore:get_cpu_index("stm32f1", cpu_name)
        periph         = config.arch.stm32f1.cpulist:Children()[cpu_idx].peripherals.GPIO

        for i = 1, periph:NumChildren() do ui.Choice_port:Append("Port "..periph:Children()[i].name:GetValue()) end
        ui.Choice_port:SetSelection(0)

        ui.StaticBoxSizer1:Add(ui.Choice_port, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.FlexGridSizer1:Add(ui.StaticBoxSizer1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        --
        ui.StaticBoxSizer2 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Port configuration")
        ui.FlexGridSizer2  = wx.wxFlexGridSizer(0, 4, 0, 0)

        ui.StaticText1     = wx.wxStaticText(this, wx.wxID_ANY, "Pin")
        ui.StaticText2     = wx.wxStaticText(this, wx.wxID_ANY, "Name")
        ui.StaticText3     = wx.wxStaticText(this, wx.wxID_ANY, "Mode")
        ui.StaticText4     = wx.wxStaticText(this, wx.wxID_ANY, "State")
        ui.FlexGridSizer2:Add(ui.StaticText1, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 1)
        ui.FlexGridSizer2:Add(ui.StaticText2, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 1)
        ui.FlexGridSizer2:Add(ui.StaticText3, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 1)
        ui.FlexGridSizer2:Add(ui.StaticText4, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 1)

        for i = 1, 16 do
                ID.TEXTCTRL_PIN_NAME[i] = wx.wxNewId()
                ID.CHOICE_MODE[i]       = wx.wxNewId()
                ID.CHOICE_STATE[i]      = wx.wxNewId()

                ui.StaticText_pin[i] = wx.wxStaticText(this, wx.wxID_ANY, i-1 ..":")
                ui.FlexGridSizer2:Add(ui.StaticText_pin[i], 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.TextCtrl_pin_name[i] = wx.wxTextCtrl(this, ID.TEXTCTRL_PIN_NAME[i], "", wx.wxDefaultPosition, wx.wxSize(125,-1))
                ui.FlexGridSizer2:Add(ui.TextCtrl_pin_name[i], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 1)

                ui.Choice_mode[i] = wx.wxChoice(this, ID.CHOICE_MODE[i], wx.wxDefaultPosition, wx.wxDefaultSize, port_mode_string)
                ui.FlexGridSizer2:Add(ui.Choice_mode[i], 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 1)

                ui.Choice_state[i] = wx.wxChoice(this, ID.CHOICE_STATE[i], wx.wxDefaultPosition, wx.wxSize(100, -1), {})
                ui.FlexGridSizer2:Add(ui.Choice_state[i], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 1)

                this:Connect(ID.CHOICE_MODE[i], wx.wxEVT_COMMAND_CHOICE_SELECTED, port_mode_changed[i])
        end

        ui.StaticBoxSizer2:Add(ui.FlexGridSizer2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.FlexGridSizer1:Add(ui.StaticBoxSizer2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        --
        ui.StaticLine2 = wx.wxStaticLine(this, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxSize(10,-1), wx.wxLI_HORIZONTAL)
        ui.FlexGridSizer1:Add(ui.StaticLine2, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 0)

        ui.Button_save = wx.wxButton(this, ID.BUTTON_SAVE, "&Save", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer1:Add(ui.Button_save, 1, bit.bor(wx.wxALL, wx.wxALIGN_RIGHT, wx.wxALIGN_CENTER_VERTICAL), 5)

        --
        this:SetSizer(ui.FlexGridSizer1)
        this:SetScrollRate(50, 50)

        --
        this:Connect(ID.CHECKBOX_ENABLE, wx.wxEVT_COMMAND_CHECKBOX_CLICKED, checkbox_changed)
        this:Connect(ID.CHOICE_PORT, wx.wxEVT_COMMAND_CHOICE_SELECTED, port_number_changed)

        --
        load_controls()

        return ui.window
end

function gpio:get_window_name()
        return "GPIO"
end


function gpio:selected()
        print("gpio:selected()")
end


function gpio:is_modified()
        return ui.Button_save:IsEnabled()
end


function get_handler()
        return gpio
end
