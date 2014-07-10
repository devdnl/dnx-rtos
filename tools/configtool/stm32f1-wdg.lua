module(..., package.seeall)
require("wx")
require("wizcore")

-- module's main object
wdg = {}

-- local objects
local ui = {}
local ID = {}


-- watchdog timeout table
local timeout2reg = {{0.004, 4,   40  },
                     {0.008, 4,   80  },
                     {0.016, 4,   160 },
                     {0.032, 4,   320 },
                     {0.064, 8,   320 },
                     {0.125, 16,  313 },
                     {0.25,  32,  313 },
                     {0.5,   64,  313 },
                     {1,     256, 157 },
                     {5,     256, 782 },
                     {10,    256, 1563},
                     {15,    256, 2344},
                     {20,    256, 3125},
                     {25,    256, 3907}}


local function load_controls()
        local enable = wizcore:get_module_state("WDG")
        local lock   = wizcore:yes_no_to_bool(wizcore:key_read(config.arch.stm32f1.key.WDG_DEVICE_LOCK_AT_OPEN))
        local debug  = wizcore:yes_no_to_bool(wizcore:key_read(config.arch.stm32f1.key.WDG_DISABLE_ON_DEBUG))
        local clkdiv = tonumber(wizcore:key_read(config.arch.stm32f1.key.WDG_CLK_DIVIDER))
        local reload = tonumber(wizcore:key_read(config.arch.stm32f1.key.WDG_RELOAD_VALUE))

        for i = 1, #timeout2reg do
                if timeout2reg[i][2] == clkdiv and timeout2reg[i][3] == reload then
                        ui.Choice_timeout:SetSelection(i - 1)
                        break
                end
        end

        ui.CheckBox_lock:SetValue(lock)
        ui.CheckBox_debug:SetValue(debug)
        ui.CheckBox_enable:SetValue(enable)
        ui.Panel1:Enable(enable)
end


local function on_button_save_click()
        local enable = ui.CheckBox_enable:GetValue()
        local lock   = wizcore:bool_to_yes_no(ui.CheckBox_lock:GetValue())
        local debug  = wizcore:bool_to_yes_no(ui.CheckBox_debug:GetValue())
        local clkdiv = tostring(timeout2reg[ui.Choice_timeout:GetSelection() + 1][2])
        local reload = tostring(timeout2reg[ui.Choice_timeout:GetSelection() + 1][3])

        wizcore:enable_module("WDG", enable)
        wizcore:key_write(config.arch.stm32f1.key.WDG_DEVICE_LOCK_AT_OPEN, lock)
        wizcore:key_write(config.arch.stm32f1.key.WDG_DISABLE_ON_DEBUG, debug)
        wizcore:key_write(config.arch.stm32f1.key.WDG_CLK_DIVIDER, clkdiv)
        wizcore:key_write(config.arch.stm32f1.key.WDG_RELOAD_VALUE, reload)

        ui.Button_save:Enable(false)
end


local function checkbox_enable_updated(this)
        ui.Button_save:Enable(true)
        ui.Panel1:Enable(this:IsChecked())
end


local function value_updated()
        ui.Button_save:Enable(true)
end

--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function wdg:create_window(parent)
        ui = {}

        ID = {}
        ID.CHECKBOX_ENABLE = wx.wxNewId()
        ID.PANEL1 = wx.wxNewId()
        ID.CHECKBOX_LOCK = wx.wxNewId()
        ID.CHECKBOX_DEBUG = wx.wxNewId()
        ID.CHOICE_TIMEOUT = wx.wxNewId()
        ID.STATICLINE1 = wx.wxNewId()
        ID.BUTTON_SAVE = wx.wxNewId()


        ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
        local this = ui.window

        ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.CheckBox_enable = wx.wxCheckBox(this, ID.CHECKBOX_ENABLE, "Enable", wx.wxDefaultPosition, wx.wxSize(wizcore.CONTROL_X_SIZE, -1), 0, wx.wxDefaultValidator, "ID.CHECKBOX_ENABLE")
        ui.FlexGridSizer1:Add(ui.CheckBox_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Panel1 = wx.wxPanel(this, ID.PANEL1, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL, "ID.PANEL1")
        ui.FlexGridSizer2 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.CheckBox_lock = wx.wxCheckBox(ui.Panel1, ID.CHECKBOX_LOCK, "Lock device at first open", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_LOCK")
        ui.CheckBox_lock:SetToolTip("When this option is selected then is not possible to open device from another task. In this option, only one task can control watchdog.")
        ui.FlexGridSizer2:Add(ui.CheckBox_lock, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.CheckBox_debug = wx.wxCheckBox(ui.Panel1, ID.CHECKBOX_DEBUG, "Disable watchdog on debug", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_DEBUG")
        ui.FlexGridSizer2:Add(ui.CheckBox_debug, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticBoxSizer1 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel1, "Watchdog timeout")
        ui.Choice_timeout = wx.wxChoice(ui.Panel1, ID.CHOICE_TIMEOUT, wx.wxDefaultPosition, wx.wxSize(wizcore.CONTROL_X_SIZE, -1), {}, 0, wx.wxDefaultValidator, "ID.CHOICE_TIMEOUT")
        ui.Choice_timeout:Append("4 ms")
        ui.Choice_timeout:Append("8 ms")
        ui.Choice_timeout:Append("16 ms")
        ui.Choice_timeout:Append("32 ms")
        ui.Choice_timeout:Append("64 ms")
        ui.Choice_timeout:Append("125 ms")
        ui.Choice_timeout:Append("250 ms")
        ui.Choice_timeout:Append("500 ms")
        ui.Choice_timeout:Append("1 s")
        ui.Choice_timeout:Append("5 s")
        ui.Choice_timeout:Append("10 s")
        ui.Choice_timeout:Append("15 s")
        ui.Choice_timeout:Append("20 s")
        ui.Choice_timeout:Append("25 s")
        ui.StaticBoxSizer1:Add(ui.Choice_timeout, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.FlexGridSizer2:Add(ui.StaticBoxSizer1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Panel1:SetSizer(ui.FlexGridSizer2)
        ui.FlexGridSizer2:Fit(ui.Panel1)
        ui.FlexGridSizer2:SetSizeHints(ui.Panel1)
        ui.FlexGridSizer1:Add(ui.Panel1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
        ui.StaticLine1 = wx.wxStaticLine(this, ID.STATICLINE1, wx.wxDefaultPosition, wx.wxSize(10,-1), wx.wxLI_HORIZONTAL, "ID.STATICLINE1")
        ui.FlexGridSizer1:Add(ui.StaticLine1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.Button_save = wx.wxButton(this, ID.BUTTON_SAVE, "Save", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.BUTTON_SAVE")
        ui.FlexGridSizer1:Add(ui.Button_save, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)

        --
        this:SetSizer(ui.FlexGridSizer1)
        this:SetScrollRate(50, 50)

        --
        this:Connect(ID.CHECKBOX_ENABLE, wx.wxEVT_COMMAND_CHECKBOX_CLICKED, checkbox_enable_updated)
        this:Connect(ID.CHECKBOX_LOCK,   wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_updated          )
        this:Connect(ID.CHECKBOX_DEBUG,  wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_updated          )
        this:Connect(ID.CHOICE_TIMEOUT,  wx.wxEVT_COMMAND_CHOICE_SELECTED,  value_updated          )
        this:Connect(ID.BUTTON_SAVE,     wx.wxEVT_COMMAND_BUTTON_CLICKED,   on_button_save_click   )

        --
        load_controls()
        ui.Button_save:Enable(false)

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @return Module name
--------------------------------------------------------------------------------
function wdg:get_window_name()
        return "WDG"
end


--------------------------------------------------------------------------------
-- @brief  Function is called by parent when window is selected
--------------------------------------------------------------------------------
function wdg:selected()
end


--------------------------------------------------------------------------------
-- @brief  Function returns modify status
-- @return If data is modified true is returned, otherwise false
--------------------------------------------------------------------------------
function wdg:is_modified()
        return ui.Button_save:IsEnabled()
end


--------------------------------------------------------------------------------
-- @brief  Function returns module handler
-- @return Module handler
--------------------------------------------------------------------------------
function get_handler()
        return wdg
end
