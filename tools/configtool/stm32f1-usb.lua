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

        button = wx.wxButton(this, wx.wxID_ANY, "USB")

        ui.window:Enable(false)

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
