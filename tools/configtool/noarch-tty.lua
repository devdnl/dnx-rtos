module(..., package.seeall)
require("wx")
require("wizcore")

-- module's main object
tty = {}

-- local objects
local ui = {}
local ID = {}


local function load_controls()

end


local function on_button_save_click()

        ui.Button_save:Enable(false)
end


--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function tty:create_window(parent)
        ui = {}

        ID = {}

        ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
        local this = ui.window


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
        return false
end


--------------------------------------------------------------------------------
-- @brief  Function returns module handler
-- @return Module handler
--------------------------------------------------------------------------------
function get_handler()
        return tty
end
