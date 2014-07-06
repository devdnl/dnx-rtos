require("wx")
require("wizcore")

network = {}

local ui = {}

local ID = {}
ID.BUTTON_SAVE = wx.wxNewId()
ID.CHECKBOX_ENABLE = wx.wxNewId()
ID.COMBOBOX_IF_PATH = wx.wxNewId()
ID.TEXTCTRL_MAC0 = wx.wxNewId()
ID.TEXTCTRL_MAC1 = wx.wxNewId()
ID.TEXTCTRL_MAC2 = wx.wxNewId()
ID.TEXTCTRL_MAC3 = wx.wxNewId()
ID.TEXTCTRL_MAC4 = wx.wxNewId()
ID.TEXTCTRL_MAC5 = wx.wxNewId()


function network:create_window(parent)
        if ui.window == nil then
                ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
                local this = ui.window

                ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)
                ui.CheckBox_enable = wx.wxCheckBox(this, ID.CHECKBOX_ENABLE, "Enable", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_ENABLE")
                ui.FlexGridSizer1:Add(ui.CheckBox_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticBoxSizer1 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "MAC address")
                ui.FlexGridSizer2 = wx.wxFlexGridSizer(0, 11, 0, 0)
                ui.TextCtrl_MAC0 = wx.wxTextCtrl(this, ID.TEXTCTRL_MAC0, "FF", wx.wxDefaultPosition, wx.wxSize(30,-1), 0, wx.wxDefaultValidator, "ID.TEXTCTRL_MAC0")
                ui.FlexGridSizer2:Add(ui.TextCtrl_MAC0, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText1 = wx.wxStaticText(this, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT1")
                ui.FlexGridSizer2:Add(ui.StaticText1, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.TextCtrl_MAC1 = wx.wxTextCtrl(this, ID.TEXTCTRL_MAC1, "FF", wx.wxDefaultPosition, wx.wxSize(30,-1), 0, wx.wxDefaultValidator, "ID.TEXTCTRL_MAC1")
                ui.FlexGridSizer2:Add(ui.TextCtrl_MAC1, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText2 = wx.wxStaticText(this, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT2")
                ui.FlexGridSizer2:Add(ui.StaticText2, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.TextCtrl_MAC2 = wx.wxTextCtrl(this, ID.TEXTCTRL_MAC2, "FF", wx.wxDefaultPosition, wx.wxSize(30,-1), 0, wx.wxDefaultValidator, "ID.TEXTCTRL_MAC3")
                ui.FlexGridSizer2:Add(ui.TextCtrl_MAC2, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText3 = wx.wxStaticText(this, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT3")
                ui.FlexGridSizer2:Add(ui.StaticText3, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.TextCtrl_MAC3 = wx.wxTextCtrl(this, ID.TEXTCTRL_MAC3, "FF", wx.wxDefaultPosition, wx.wxSize(30,-1), 0, wx.wxDefaultValidator, "ID.TEXTCTRL1")
                ui.FlexGridSizer2:Add(ui.TextCtrl_MAC3, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText4 = wx.wxStaticText(this, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT4")
                ui.FlexGridSizer2:Add(ui.StaticText4, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.TextCtrl_MAC4 = wx.wxTextCtrl(this, ID.TEXTCTRL_MAC4, "FF", wx.wxDefaultPosition, wx.wxSize(30,-1), 0, wx.wxDefaultValidator, "ID.TEXTCTRL_MAC4")
                ui.FlexGridSizer2:Add(ui.TextCtrl_MAC4, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText5 = wx.wxStaticText(this, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT5")
                ui.FlexGridSizer2:Add(ui.StaticText5, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.TextCtrl_MAC5 = wx.wxTextCtrl(this, ID.TEXTCTRL_MAC5, "FF", wx.wxDefaultPosition, wx.wxSize(30,-1), 0, wx.wxDefaultValidator, "ID.TEXTCTRL_MAC5")
                ui.FlexGridSizer2:Add(ui.TextCtrl_MAC5, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticBoxSizer1:Add(ui.FlexGridSizer2, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticBoxSizer2 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "Ethernet device path")
                ui.ComboBox_if_path = wx.wxComboBox(this, ID.COMBOBOX_IF_PATH, "", wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.COMBOBOX_IF_PATH")
                ui.ComboBox_if_path:Append("/dev/eth0")
                ui.StaticBoxSizer2:Add(ui.ComboBox_if_path, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                --
                ui.StaticLine2 = wx.wxStaticLine(this, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxSize(10,-1), wx.wxLI_HORIZONTAL)
                ui.FlexGridSizer1:Add(ui.StaticLine2, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 0)

                ui.Button_save = wx.wxButton(this, ID.BUTTON_SAVE, "&Save", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer1:Add(ui.Button_save, 1, bit.bor(wx.wxALL, wx.wxALIGN_RIGHT, wx.wxALIGN_CENTER_VERTICAL), 5)

                --
                this:SetSizer(ui.FlexGridSizer1)
                this:SetScrollRate(5, 5)
        end

        return ui.window
end


function network:get_window_name()
        return "Network"
end


function network:refresh()
end


function network:is_modified()
        return false
end
