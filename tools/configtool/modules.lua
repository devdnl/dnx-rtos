require("wx")
require("wizcore")

modules = {}


local ui   = {}
local page = {}


local function notebook_page_changed(this)
        local card = this:GetSelection() + 1
        if page[card] then page[card]:selected() end
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
        local cpu_arch   = wizcore:key_read(config.project.key.PROJECT_CPU_ARCH)
        local cpu_name   = wizcore:key_read(config.arch[cpu_arch].key.CPU_NAME)
        local cpu_idx    = wizcore:get_cpu_index(cpu_arch, cpu_name)
        local periph_num = config.arch[cpu_arch].cpulist:Children()[cpu_idx].peripherals:NumChildren()

        -- check if CPU exist
        if cpu_idx == 0 then
                wizcore:show_error_msg(wizcore.MAIN_WINDOW_NAME, cpu_name..": no such microcontroller.")
                return
        end

        -- check if CPU has peripherals
        if periph_num == 0 then
                wizcore:show_info_msg(wizcore.MAIN_WINDOW_NAME, cpu_name..": no peripherals!")
                return
        end

        -- create progress dialog
        local dialog = wx.wxProgressDialog("Loading configuration", "Clearing old data...", periph_num + 1, ui.notebook, bit.bor(wx.wxPD_APP_MODAL,wx.wxPD_AUTO_HIDE,wx.wxPD_SMOOTH))
        dialog:SetMinSize(wx.wxSize(300, 100))
        dialog:Centre()

        -- clear all pages
        dialog:Update(0)
        ui.notebook:Hide()
        ui.notebook:DeleteAllPages()
        dialog:Update(1)

        -- load specified modules required by configuration for specified microcontroller
        for i = 1, periph_num do
                local peripheral  = config.arch[cpu_arch].cpulist:Children()[cpu_idx].peripherals:Children()[i]
                local module_name = peripheral:GetName():lower()
                local module_file = cpu_arch.."-"..module_name

                local module = require(module_file).get_handler()

                dialog:Update(i, "Loading data of "..module:get_window_name().."...")

                ui.notebook:AddPage(module:create_window(ui.notebook), module:get_window_name())
                page[i] = module

                dialog:Update(i+1)
        end

        ui.notebook:Show()
end


function modules:is_modified()
        for i, module in ipairs(page) do
                if module:is_modified() then
                        return true
                end
        end

        return false
end
