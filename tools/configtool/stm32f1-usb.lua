module(..., package.seeall)
require("wx")
require("wizcore")

usb = {}

local ui = {}
local ID = {}

function usb:create_window(parent)
        ui = {}
        ID = {}

        ui.window  = wx.wxPanel(parent, wx.wxID_ANY)
        local this = ui.window

        choice = wx.wxChoice(this, -1, wx.wxPoint(10, 100), wx.wxSize(wizcore.CONTROL_X_SIZE, -1), {}, 0, wx.wxDefaultValidator, "ID.CHOICE_TIMEOUT")
        gpio = require("stm32f1-gpio").get_handler()
        choice:Append(gpio:get_pin_list(true))

        button = wx.wxButton(this, wx.wxID_ANY, "USB")


--         ui.window:Enable(false)

        return ui.window
end

function usb:get_window_name()
        return "USB"
end


function usb:selected()
        print("usb:selected()")
end


function usb:is_modified()
        return false
end


function get_handler()
        return usb
end
