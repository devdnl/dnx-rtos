require("wx")
require("wizcore")
xml = require("xmlSimple").newParser()

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




        local parsedXml = xml:loadFile("stm32f1-gpio.xml")


        print(parsedXml.note.to:GetValue())
        print(parsedXml.note.from:GetValue())
        print(parsedXml.note.heading:GetValue())
        print(parsedXml.note.body:GetValue())

        print(parsedXml.microcontroller.STM32F107RCXX.GPIO:NumChildren(),
               parsedXml.microcontroller.STM32F107RCXX.GPIO.PORT[1].name:GetValue(),
               parsedXml.microcontroller.STM32F107RCXX.GPIO.PORT[2].NAME:GetValue(),
               parsedXml.microcontroller.STM32F107RCXX.GPIO.PORT[2].NAME:SetValue("dupa"),
               parsedXml.microcontroller.STM32F107RCXX.GPIO.PORT[2].NAME:GetValue(),
               parsedXml.microcontroller.STM32F107RCXX.GPIO.PORT[3].NAME:GetValue())


        print("Number of microcontrollers: "..parsedXml.stm32f1:NumChildren())
        for i = 1, parsedXml.stm32f1:NumChildren() do
                print(parsedXml.stm32f1.mcu[i].name:GetValue())
        end


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
