require("wx")
require("wizcore")

modules = {}


local ui   = {}
local ID   = {}
local page = {}


local function notebook_page_changed(this)
        local card = this:GetSelection() + 1
        page[card]:selected()
        this:Skip()
end


function modules:create_window(parent)
        if ui.window == nil then
                ui.window    = wx.wxPanel(parent, wx.wxNewId())
                ui.BoxSizer1 = wx.wxBoxSizer(wx.wxHORIZONTAL)
                ui.notebook  = wx.wxNotebook(ui.window, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxLB_TOP)
                ui.BoxSizer1:Add(ui.notebook, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL))
                ui.window:SetSizer(ui.BoxSizer1)
                ui.window:Layout()

                ui.notebook:Connect(wx.wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, notebook_page_changed)
        end

        return ui.window
end


function modules:get_window_name()
        return "Modules"
end


function modules:refresh()
        print("modules:refresh()")

        local arch = wizcore:key_read("test.h", "CPU_ARCH")

        if arch:match("stm32f1") then
                require("stm32f1-gpio")
                require("stm32f1-usb")
                page = {gpio, usb}

        elseif arch:match("stm32f2") then
                require("stm32f2-gpio")
                require("stm32f2-gpio")
                page = {gpio, usb}
        end


        local dialog = wx.wxProgressDialog("Loading configuration", "Clearing old data...", #page + 1, ui.notebook, bit.bor(wx.wxPD_APP_MODAL,wx.wxPD_AUTO_HIDE,wx.wxPD_SMOOTH))
        dialog:SetMinSize(wx.wxSize(300, 100))
        dialog:Centre()
        dialog:Update(0)
        ui.notebook:DeleteAllPages()
        dialog:Update(1)

        for i, page in ipairs(page) do
                dialog:Update(i, "Loading data of "..page:get_window_name().."...")
                ui.notebook:AddPage(page:create_window(ui.notebook), page:get_window_name())
                dialog:Update(i+1)
        end
end


function modules:is_modified()
        return false
end
