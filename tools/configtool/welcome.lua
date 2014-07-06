require("wx")
require("wizcore")

welcome = {}

local window
local FlexGridSizer1
local BoxSizer1
local StaticText1
local StaticText2
local StaticText3
local StaticText4
local StaticText5
local StaticText6
local StaticText7
local StaticText8


function welcome:create_window(parent)
        if window == nil then
                window     = wx.wxScrolledWindow(parent, wx.wxID_ANY)
                local this = window

                BoxSizer1 = wx.wxBoxSizer(wx.wxHORIZONTAL);
                FlexGridSizer1 = wx.wxFlexGridSizer(3, 1, 0, 0)

                StaticText1 = wx.wxStaticText(this, wx.wxID_ANY, "Welcome to the dnx RTOS Configuration Tool!", wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxALIGN_LEFT)
                StaticText1:SetFont(wx.wxFont(wizcore.HEADER_FONT_SIZE, wx.wxDEFAULT, wx.wxFONTSTYLE_NORMAL, wx.wxBOLD, false, "", wx.wxFONTENCODING_DEFAULT))
                FlexGridSizer1:Add(StaticText1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                StaticText2 = wx.wxStaticText(this, wx.wxID_ANY, "\nThe configuration is divided to few parts, each one configures a specified software module.")
                FlexGridSizer1:Add(StaticText2, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                StaticText3 = wx.wxStaticText(this, wx.wxID_ANY, "\nThe specified options configure:")
                FlexGridSizer1:Add(StaticText3, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                StaticText4 = wx.wxStaticText(this, wx.wxID_ANY, "    - Project: general project configuration (project and toolchain name, CPU, etc)")
                FlexGridSizer1:Add(StaticText4, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                StaticText5 = wx.wxStaticText(this, wx.wxID_ANY, "    - Operating System: all settings of the Operating System")
                FlexGridSizer1:Add(StaticText5, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                StaticText6 = wx.wxStaticText(this, wx.wxID_ANY, "    - File Systems: configuration of all file systems existing in the system")
                FlexGridSizer1:Add(StaticText6, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                StaticText7 = wx.wxStaticText(this, wx.wxID_ANY, "    - Network: configuration of newtwork (Ethernet)")
                FlexGridSizer1:Add(StaticText7, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                StaticText8 = wx.wxStaticText(this, wx.wxID_ANY, "    - Modules: configuration of specified modules (drivers) that exist in the selected microcontroller")
                FlexGridSizer1:Add(StaticText8, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                BoxSizer1:Add(FlexGridSizer1, 1, bit.bor(wx.wxALL,wx.wxALIGN_TOP,wx.wxALIGN_CENTER_HORIZONTAL), 0)

                this:SetSizer(BoxSizer1)
                this:SetScrollRate(5, 5)
        end

        return window
end


function welcome:get_window_name()
        return "Welcome"
end


function welcome:refresh()
end


function welcome:is_modified()
        return false
end
