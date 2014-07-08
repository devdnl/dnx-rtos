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


        
        print("Number of CPUs: "..config.arch.stm32f1.cpulist:NumChildren())
        for i = 1, config.arch.stm32f1.cpulist:NumChildren() do
            if config.arch.stm32f1.cpulist.cpu[i].name:GetValue() == "STM32F103RBxx" then
                print("Number of ports: "..config.arch.stm32f1.cpulist.cpu[i].GPIO:NumChildren())
            end
        end
        
        print(config.project.keys.CPU_ARCH.path:GetValue(), config.project.keys.CPU_ARCH.key:GetValue())
        
        
        print(config.arch:NumChildren(), config.arch:Children()[1]:GetName())

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
