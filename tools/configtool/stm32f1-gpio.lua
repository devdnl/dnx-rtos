module(..., package.seeall)
require("wx")
require("wizcore")

-- module's main object
gpio = {}

-- local objects
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
                          "Input pulled"}

local port_mode_index = {["_GPIO_OUT_PUSH_PULL_2MHZ"]       = 0,
                         ["_GPIO_OUT_PUSH_PULL_10MHZ"]      = 1,
                         ["_GPIO_OUT_PUSH_PULL_50MHZ"]      = 2,
                         ["_GPIO_OUT_OPEN_DRAIN_2MHZ"]      = 3,
                         ["_GPIO_OUT_OPEN_DRAIN_10MHZ"]     = 4,
                         ["_GPIO_OUT_OPEN_DRAIN_50MHZ"]     = 5,
                         ["_GPIO_ALT_OUT_PUSH_PULL_2MHZ"]   = 6,
                         ["_GPIO_ALT_OUT_PUSH_PULL_10MHZ"]  = 7,
                         ["_GPIO_ALT_OUT_PUSH_PULL_50MHZ"]  = 8,
                         ["_GPIO_ALT_OUT_OPEN_DRAIN_2MHZ"]  = 9,
                         ["_GPIO_ALT_OUT_OPEN_DRAIN_10MHZ"] = 10,
                         ["_GPIO_ALT_OUT_OPEN_DRAIN_50MHZ"] = 11,
                         ["_GPIO_IN_ANALOG"]                = 12,
                         ["_GPIO_IN_FLOAT"]                 = 13,
                         ["_GPIO_IN_PULLED"]                = 14}

local port_state_in_pulled     = {"Down", "Up"}
local port_state_out_push_pull = {"Low (0)", "High (1)"}
local port_state_float         = {"Hi-Z"}
local port_names               = {"A", "B", "C", "D", "E", "F", "G"}
local number_of_pins           = 16

local function enable_controls(state)
        ui.Choice_port:Enable(state)
        ui.StaticText1:Enable(state)
        ui.StaticText2:Enable(state)
        ui.StaticText3:Enable(state)
        ui.StaticText4:Enable(state)

        for pin = 1, number_of_pins do
                ui.StaticText_pin[pin]:Enable(state)
                ui.TextCtrl_pin_name[pin]:Enable(state)
                ui.Choice_mode[pin]:Enable(state)

                if not ui.Choice_state[pin].Inactive then
                        ui.Choice_state[pin]:Enable(state)
                end
        end
end


function port_mode_index:get_mode(idx)
        for key, value in pairs(port_mode_index) do
                if idx == value then
                        return key
                end
        end
        
        return ""
end


local function set_pin_state_by_pin_mode(pin, pin_state)
        local pin_mode = port_mode_index:get_mode(ui.Choice_mode[pin + 1]:GetSelection())

        ui.Choice_state[pin + 1]:Clear()
        ui.Choice_state[pin + 1]:Enable(true)
        ui.Choice_state[pin + 1].Inactive = false
        if pin_mode:match("_GPIO_IN_PULLED") then
                ui.Choice_state[pin + 1]:Append(port_state_in_pulled)
        elseif pin_mode:match("_OUT_OPEN_DRAIN_") then
                ui.Choice_state[pin + 1]:Append(port_state_out_push_pull)
        elseif pin_mode:match("_OUT_PUSH_PULL_") then
                ui.Choice_state[pin + 1]:Append(port_state_out_push_pull)
        else
                ui.Choice_state[pin + 1]:Append(port_state_float)
                ui.Choice_state[pin + 1]:Enable(false)
                ui.Choice_state[pin + 1].Inactive = true
        end

        if pin_state == "_HIGH" then
                ui.Choice_state[pin + 1]:SetSelection(1)
        else
                ui.Choice_state[pin + 1]:SetSelection(0)
        end
end


local function load_controls()
        local port     = ui.Choice_port:GetSelection() + 1
        local pin_mask = periph:Children()[port].pinmask:GetValue()

        for pin = 0, number_of_pins - 1 do
                if bit.band(pin_mask, 1) == 1 then

                        local pin_key = config.arch.stm32f1.key.GPIO

                        ui.StaticText_pin[pin + 1]:Show()

                        pin_key.key:SetValue("__GPIO_P"..periph:Children()[port].name:GetValue().."_PIN_"..pin.."_NAME__")
                        local pin_name = wizcore:key_read(pin_key)
                        ui.TextCtrl_pin_name[pin + 1]:SetValue(pin_name)
                        ui.TextCtrl_pin_name[pin + 1]:Show(true)

                        pin_key.key:SetValue("__GPIO_P"..periph:Children()[port].name:GetValue().."_PIN_"..pin.."_MODE__")
                        local pin_mode = wizcore:key_read(pin_key)
                        ui.Choice_mode[pin + 1]:SetSelection(port_mode_index[pin_mode])
                        ui.Choice_mode[pin + 1]:Show(true)

                        pin_key.key:SetValue("__GPIO_P"..periph:Children()[port].name:GetValue().."_PIN_"..pin.."_STATE__")
                        local pin_state = wizcore:key_read(pin_key)
                        ui.Choice_state[pin + 1]:Show()
                        set_pin_state_by_pin_mode(pin, pin_state)
                else
                        ui.StaticText_pin[pin + 1]:Hide()
                        ui.TextCtrl_pin_name[pin + 1]:Hide()
                        ui.Choice_mode[pin + 1]:Hide()
                        ui.Choice_state[pin + 1]:Hide()
                end

                pin_mask = bit.rshift(pin_mask, 1)
        end

        ui.window:FitInside()

        local gpio_enabled = wizcore:get_module_state("GPIO")
        ui.CheckBox_enable:SetValue(gpio_enabled)
        enable_controls(gpio_enabled)
end


local function on_button_save_click()
        local port_to_save = periph:Children()[ui.Choice_port:GetSelection() + 1].name:GetValue()
        local pin_mask     = periph:Children()[ui.Choice_port:GetSelection() + 1].pinmask:GetValue()
        local key          = config.arch.stm32f1.key.GPIO
        
        -- save pins configuration for selected port
        for pin = 0, number_of_pins - 1 do
                local pin_name
                local pin_mode
                local pin_state
        
                if bit.band(pin_mask, 1) == 1 then
                        pin_name = ui.TextCtrl_pin_name[pin + 1]:GetValue()
                        
                        pin_mode = port_mode_index:get_mode(ui.Choice_mode[pin + 1]:GetSelection())
                        
                        if pin_mode == "_GPIO_IN_ANALOG" or pin_mode == "_GPIO_IN_FLOAT" then
                                pin_state = "_FLOAT"
                        else
                                pin_state = ifs(ui.Choice_state[pin + 1]:GetSelection() == 0, "_LOW", "_HIGH")
                        end
                else
                        pin_name  = "NC_GPIO"..port_to_save.."_"..pin
                        pin_mode  = port_mode_index:get_mode(13)
                        pin_state = "_FLOAT"
                end
                
                -- save pin settings
                key.key:SetValue("__GPIO_P"..port_to_save.."_PIN_"..pin.."_NAME__")
                wizcore:key_write(key, pin_name)
                
                key.key:SetValue("__GPIO_P"..port_to_save.."_PIN_"..pin.."_MODE__")
                wizcore:key_write(key, pin_mode)
                
                key.key:SetValue("__GPIO_P"..port_to_save.."_PIN_"..pin.."_STATE__")
                wizcore:key_write(key, pin_state)
                
                pin_mask = bit.rshift(pin_mask, 1)
        end
                        
        -- enable used ports by specified microcontroller
        for i, port_name in ipairs(port_names) do
                local port_found = false
                for i = 1, #port_names do
                        if periph:Children()[i] then
                                if periph:Children()[i].name:GetValue() == port_name then
                                        port_found = true
                                        break
                                end
                        end
                end
        
                if port_found then
                        key.key:SetValue("__GPIO_P"..port_name.."_ENABLE__")
                        wizcore:key_write(key, config.project.def.YES:GetValue())
                else
                        key.key:SetValue("__GPIO_P"..port_name.."_ENABLE__")
                        wizcore:key_write(key, config.project.def.NO:GetValue())
                end
        end
        
        -- module enable
        wizcore:enable_module("GPIO", ui.CheckBox_enable:IsChecked())
        
        ui.Button_save:Enable(false)
end


local function checkbox_changed(this)
        ui.Choice_port:Enable(this:IsChecked())
        enable_controls(this:IsChecked())
        ui.Button_save:Enable(true)
end


local function port_number_changed(this)
        if ui.Choice_port:GetSelection() == ui.Choice_port.OldSelection then
                return
        end

        local answer = wx.wxID_NO
        if ui.Button_save:IsEnabled() then
                answer = wizcore:show_question_msg(wizcore.MAIN_WINDOW_NAME, "Do you want to save changes?", bit.bor(wx.wxYES_NO, wx.wxCANCEL))
        end

        if answer == wx.wxID_YES then
                on_button_save_click()
                ui.Choice_port.OldSelection = ui.Choice_port:GetSelection()
        elseif answer == wx.wxID_NO then
                ui.Choice_port.OldSelection = ui.Choice_port:GetSelection()
        elseif answer == wx.wxID_CANCEL then
                ui.Choice_port:SetSelection(ui.Choice_port.OldSelection)
                return
        end

        ui.window:Freeze()
        load_controls()
        ui.window:Thaw()

        if answer == wx.wxID_YES or answer == wx.wxID_NO then
                ui.Button_save:Enable(false)
        end
end


local function pin_name_updated(this)
        ui.Button_save:Enable(true)
end


local function pin_state_updated()
        ui.Button_save:Enable(true)
end


local port_mode_changed = {}
for i = 1, number_of_pins do port_mode_changed[i] = function() set_pin_state_by_pin_mode(i - 1) ui.Button_save:Enable(true) end end


--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
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
        ui.Choice_port.OldSelection = 0

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

        for i = 1, number_of_pins do
                ID.TEXTCTRL_PIN_NAME[i] = wx.wxNewId()
                ID.CHOICE_MODE[i]       = wx.wxNewId()
                ID.CHOICE_STATE[i]      = wx.wxNewId()

                ui.StaticText_pin[i] = wx.wxStaticText(this, wx.wxID_ANY, i-1 ..":")
                ui.FlexGridSizer2:Add(ui.StaticText_pin[i], 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.TextCtrl_pin_name[i] = wx.wxTextCtrl(this, ID.TEXTCTRL_PIN_NAME[i], "", wx.wxDefaultPosition, wx.wxSize(125,-1))
                ui.FlexGridSizer2:Add(ui.TextCtrl_pin_name[i], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 1)

                ui.Choice_mode[i] = wx.wxChoice(this, ID.CHOICE_MODE[i], wx.wxDefaultPosition, wx.wxDefaultSize, port_mode_string)
                ui.FlexGridSizer2:Add(ui.Choice_mode[i], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 1)

                ui.Choice_state[i] = wx.wxChoice(this, ID.CHOICE_STATE[i], wx.wxDefaultPosition, wx.wxSize(100, -1), {})
                ui.Choice_state[i].Inactive = false
                ui.FlexGridSizer2:Add(ui.Choice_state[i], 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 1)

                this:Connect(ID.TEXTCTRL_PIN_NAME[i], wx.wxEVT_COMMAND_TEXT_UPDATED,    pin_name_updated    )
                this:Connect(ID.CHOICE_MODE[i],       wx.wxEVT_COMMAND_CHOICE_SELECTED, port_mode_changed[i])
                this:Connect(ID.CHOICE_STATE[i],      wx.wxEVT_COMMAND_CHOICE_SELECTED, pin_state_updated   )
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
        this:Connect(ID.CHECKBOX_ENABLE, wx.wxEVT_COMMAND_CHECKBOX_CLICKED, checkbox_changed    )
        this:Connect(ID.CHOICE_PORT,     wx.wxEVT_COMMAND_CHOICE_SELECTED,  port_number_changed )
        this:Connect(ID.BUTTON_SAVE,     wx.wxEVT_COMMAND_BUTTON_CLICKED,   on_button_save_click)

        --
        load_controls()
        ui.Button_save:Enable(false)

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @return Module name
--------------------------------------------------------------------------------
function gpio:get_window_name()
        return "GPIO"
end


--------------------------------------------------------------------------------
-- @brief  Function is called by parent when window is selected
--------------------------------------------------------------------------------
function gpio:selected()
end


--------------------------------------------------------------------------------
-- @brief  Function returns modify status
-- @return If data is modified true is returned, otherwise false
--------------------------------------------------------------------------------
function gpio:is_modified()
        return ui.Button_save:IsEnabled()
end


--------------------------------------------------------------------------------
-- @brief  Function returns module handler
-- @return Module handler
--------------------------------------------------------------------------------
function get_handler()
        return gpio
end
