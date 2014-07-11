module(..., package.seeall)
require("wx")
require("wizcore")

-- module's main object
crc = {}

-- local objects
local ui = {}
local ID = {}


local function load_controls()
        ui.CheckBox_enable:SetValue(wizcore:get_module_state("CRC"))
end


local function on_button_save_click()
        wizcore:enable_module("CRC", ui.CheckBox_enable:GetValue())
        ui.Button_save:Enable(false)
end


local function checkbox_enable_updated(this)
        ui.Button_save:Enable(true)
end


--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function crc:create_window(parent)
        ui = {}

        ID = {}
        ID.CHECKBOX_ENABLE = wx.wxNewId()
        ID.STATICLINE1 = wx.wxNewId()
        ID.BUTTON_SAVE = wx.wxNewId()


        ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
        local this = ui.window

        ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.CheckBox_enable = wx.wxCheckBox(this, ID.CHECKBOX_ENABLE, "Enable module", wx.wxDefaultPosition, wx.wxSize(wizcore.CONTROL_X_SIZE, -1), 0, wx.wxDefaultValidator, "ID.CHECKBOX_ENABLE")
        ui.FlexGridSizer1:Add(ui.CheckBox_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        ui.StaticLine1 = wx.wxStaticLine(this, ID.STATICLINE1, wx.wxDefaultPosition, wx.wxSize(10,-1), wx.wxLI_HORIZONTAL, "ID.STATICLINE1")
        ui.FlexGridSizer1:Add(ui.StaticLine1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        ui.Button_save = wx.wxButton(this, ID.BUTTON_SAVE, "Save", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.BUTTON_SAVE")
        ui.FlexGridSizer1:Add(ui.Button_save, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)

        --
        this:SetSizer(ui.FlexGridSizer1)
        this:SetScrollRate(50, 50)

        --
        this:Connect(ID.CHECKBOX_ENABLE, wx.wxEVT_COMMAND_CHECKBOX_CLICKED, checkbox_enable_updated)
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
function crc:get_window_name()
        return "CRC"
end


--------------------------------------------------------------------------------
-- @brief  Function is called by parent when window is selected
--------------------------------------------------------------------------------
function crc:selected()
end


--------------------------------------------------------------------------------
-- @brief  Function returns modify status
-- @return If data is modified true is returned, otherwise false
--------------------------------------------------------------------------------
function crc:is_modified()
        return ui.Button_save:IsEnabled()
end


--------------------------------------------------------------------------------
-- @brief  Function returns module handler
-- @return Module handler
--------------------------------------------------------------------------------
function get_handler()
        return crc
end
