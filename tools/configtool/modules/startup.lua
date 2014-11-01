--[[============================================================================
@file    startup.lua

@author  Daniel Zorychta

@brief   The startup configuration script.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the  Free Software  Foundation;  either version 2 of the License, or
         any later version.

         This  program  is  distributed  in the hope that  it will be useful,
         but  WITHOUT  ANY  WARRANTY;  without  even  the implied warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         You  should  have received a copy  of the GNU General Public License
         along  with  this  program;  if not,  write  to  the  Free  Software
         Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


==============================================================================]]

--==============================================================================
-- EXTERNAL MODULES
--==============================================================================
require("wx")
require("modules/ctcore")


--==============================================================================
-- GLOBAL OBJECTS
--==============================================================================
startup  = {}


--==============================================================================
-- LOCAL CLASSES
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Create a new File System list
-- @param  None
-- @return New object
--------------------------------------------------------------------------------
local function new_FS_list()
        local self = {}
        self._list = {}

        -- method load list
        self.reload = function(self)
                self._list = {}
                table.insert(self._list, "")
                for i = 1, config.project.filesystems:NumChildren() do
                        table.insert(self._list, config.project.filesystems:Children()[i].name:GetValue():lower())
                end
        end

        -- method return list
        self.get_list = function(self)
                return self._list
        end

        -- method return item by index
        self.get_FS_by_index = function(self, idx)
                return self._list[idx]
        end

        self:reload()

        return self
end


--------------------------------------------------------------------------------
-- @brief  Create a new driver list
-- @param  None
-- @return New object
--------------------------------------------------------------------------------
local function new_driver_list()
        local FILE_DRIVER_REGISTARTION  = config.project.path.drivers_reg_file:GetValue()
        local self = {}
        self._list = {}

        -- method load list
        self.reload = function(self)
                local cpu_arch   = ct:key_read(config.project.key.PROJECT_CPU_ARCH)
                local cpu_name   = ct:key_read(config.arch[cpu_arch].key.CPU_NAME)
                local cpu_idx    = ct:get_cpu_index(cpu_arch, cpu_name)
                local periph_num = config.arch[cpu_arch].cpulist:Children()[cpu_idx].peripherals:NumChildren()

                self._list = {}
                table.insert(self._list, "")
                for i = 1, periph_num do
                        local module_name = config.arch[cpu_arch].cpulist:Children()[cpu_idx].peripherals:Children()[i]:GetName():upper()

                        if ct:get_module_state(module_name) == true then
                                local regex = "%s*_DRIVER_INTERFACE%(%s*"..module_name.."%s*,%s*\"(.*)\"%s*,%s*.*,.*%)%s*,"
                                local n = ct:find_line(FILE_DRIVER_REGISTARTION, 1, regex)
                                while n > 0 do
                                        local line = ct:get_line(FILE_DRIVER_REGISTARTION, n)
                                        if line ~= nil then
                                                table.insert(self._list, line:match(regex))
                                        end

                                        n = ct:find_line(FILE_DRIVER_REGISTARTION, n + 1, regex)
                                end

                        end
                end

                table.sort(self._list)
        end

        -- method return entire list
        self.get_list = function(self)
                return self._list
        end

        self:reload()

        return self
end


--------------------------------------------------------------------------------
-- @brief  Create a new application list
-- @param  None
-- @return New object
--------------------------------------------------------------------------------
local function new_app_list()
        local FILE_PROGRAM_REGISTRATION = config.project.path.programs_reg_file:GetValue()
        local self = {}
        self._list = {}

        -- method load list
        self.reload = function(self)
                self._list = {}
                table.insert(self._list, "")

                if ct.fs:exists(FILE_PROGRAM_REGISTRATION) then
                        local regex = "%s*_IMPORT_PROGRAM%((.+)%)%s*;"
                        local n = ct:find_line(FILE_PROGRAM_REGISTRATION, 1, regex)
                        while n > 0 do
                                local line = ct:get_line(FILE_PROGRAM_REGISTRATION, n)
                                if line ~= nil then
                                        table.insert(self._list, line:match(regex))
                                end

                                n = ct:find_line(FILE_PROGRAM_REGISTRATION, n + 1, regex)
                        end

                        table.sort(self._list)
                end
        end

        -- method return entire list
        self.get_list = function(self)
                return self._list
        end

        self:reload()

        return self
end


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local ui = {}
local ID = {}
local modified     = ct:new_modify_indicator()
local default_dirs = {"/dev", "/mnt", "/tmp", "/proc", "/srv", "/home", "/usr"}
local FS_list      = new_FS_list()
local drv_list     = new_driver_list()
local app_list     = new_app_list()

--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function loads all controls from configuration files
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_controls()

end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function save_configuration()
        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Insert item to the wxListView
-- @param  self     wxListView
-- @param  ...      Variable number of arguments that are inserted to the columns
-- @return None
--------------------------------------------------------------------------------
local function insert_item(self, ...)
        local arg   = {...}
        local count = self:GetItemCount()
        self:InsertItem(count, "")

        for i, v in pairs(arg) do
                self:SetItem(count, i - 1, v)
        end
end


--------------------------------------------------------------------------------
-- @brief  Get item's texts
-- @param  self     wxListView
-- @param  row      row read
-- @param  n        number of columns to read
-- @return Next columns values as table
--------------------------------------------------------------------------------
local function get_item_texts(self, row, no_of_cols)
        local item = wx.wxListItem()
        item:SetId(row)

        local t = {}
        for i = 0, no_of_cols - 1 do
                item:SetColumn(i)
                self:GetItem(item)
                t[i] = item:GetText()
        end

        return t
end


--------------------------------------------------------------------------------
-- @brief  Create panel with boot runlevel
-- @param  parent       parent window
-- @return Panel object
--------------------------------------------------------------------------------
local function create_boot_widgets(parent)
        -- create boot panel
        ui.Panel_boot = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_boot = wx.wxFlexGridSizer(0, 1, 0, 0)

        -- boot information
        ui.StaticText = wx.wxStaticText(ui.Panel_boot, wx.wxID_ANY, "The purpose of this runlevel is to create a basic file system environment.", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_boot:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        ui.StaticText = wx.wxStaticText(ui.Panel_boot, wx.wxID_ANY,
                                        "NOTE: Some file systems requires a source files, that in this level may not exist. "..
                                        "In this case, these file systems will not be mounted. File system like that can be mounted in the runlevel 1 or later.",
                                        wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, -1))
        ui.StaticText:Wrap(ct.CONTROL_X_SIZE)
        ui.FlexGridSizer_boot:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        ui.StaticLine = wx.wxStaticLine(ui.Panel_boot, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxLI_HORIZONTAL, "wx.wxID_ANY")
        ui.FlexGridSizer_boot:Add(ui.StaticLine, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- Root file system selection group
        ui.StaticBoxSizer_boot_main_FS_0 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_boot, "Root file system (\"/\")")
        ui.FlexGridSizer_boot_main_FS_1 = wx.wxFlexGridSizer(0, 2, 0, 0)

            ui.StaticText1 = wx.wxStaticText(ui.Panel_boot, wx.wxID_ANY, "Select base file system:", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_boot_main_FS_1:Add(ui.StaticText1, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            ui.Choice_boot_root_FS = wx.wxChoice(ui.Panel_boot, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(100, -1), FS_list:get_list())
            ui.FlexGridSizer_boot_main_FS_1:Add(ui.Choice_boot_root_FS, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.Choice_boot_root_FS:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)

            ui.StaticBoxSizer_boot_main_FS_0:Add(ui.FlexGridSizer_boot_main_FS_1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_boot:Add(ui.StaticBoxSizer_boot_main_FS_0, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- folders group
        ui.StaticBoxSizer_boot_folders_0 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_boot, "Create folders")
        ui.FlexGridSizer_boot_folders_1 = wx.wxFlexGridSizer(0, 1, 0, 0)

            -- new folder sizer
            ui.FlexGridSizer_boot_folders_2 = wx.wxFlexGridSizer(0, 5, 0, 0)

                -- folder name combobox
                ui.ComboBox_folder_name = wx.wxComboBox(ui.Panel_boot, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(100, -1), default_dirs, wx.wxTE_PROCESS_ENTER)
                ui.FlexGridSizer_boot_folders_2:Add(ui.ComboBox_folder_name, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.ComboBox_folder_name:Connect(wx.wxEVT_COMMAND_TEXT_ENTER, function() ui.Button_folder_add:Command(wx.wxCommandEvent(wx.wxEVT_COMMAND_BUTTON_CLICKED)) end)

                -- add button
                ui.Button_folder_add = wx.wxButton(ui.Panel_boot, wx.wxNewId(), "Add", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_boot_folders_2:Add(ui.Button_folder_add, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Button_folder_add:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                        function()
                                local dirname = ui.ComboBox_folder_name:GetValue()
                                if dirname ~= "" then
                                        if not dirname:match("^/.*") then dirname = "/"..dirname end

                                        ui.ListBox_folders:InsertItems({dirname}, ui.ListBox_folders:GetCount())
                                        ui.ComboBox_other_FS_mntpt:Append(dirname)
                                        ui.ComboBox_sd_cards_mntp:Append(dirname)
                                        ui.ComboBox_folder_name:SetValue("")
                                        modified:yes()
                                end
                        end
                )

                -- seperator
                ui.StaticLine = wx.wxStaticLine(ui.Panel_boot, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxLI_VERTICAL)
                ui.FlexGridSizer_boot_folders_2:Add(ui.StaticLine, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- remove button
                ui.Button_folder_remove = wx.wxButton(ui.Panel_boot, wx.wxID_ANY, "Remove selected", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_boot_folders_2:Add(ui.Button_folder_remove, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Button_folder_remove:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                        function()
                                local sel = ui.ListBox_folders:GetSelection()
                                local t   = {}
                                for i = 0, ui.ListBox_folders:GetCount() do
                                        if i ~= sel then
                                                local str = ui.ListBox_folders:GetString(i)
                                                if str ~= "" then
                                                        table.insert(t, str)
                                                end
                                        end
                                end

                                ui.ListBox_folders:Clear()
                                ui.ListBox_folders:InsertItems(t, 0)
                                ui.ComboBox_other_FS_mntpt:Clear()
                                ui.ComboBox_other_FS_mntpt:Append(t)
                                ui.ComboBox_sd_cards_mntp:Clear()
                                ui.ComboBox_sd_cards_mntp:Append(t)
                                modified:yes()
                        end
                )

                -- add new folder sizer to folder group
                ui.FlexGridSizer_boot_folders_1:Add(ui.FlexGridSizer_boot_folders_2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

            -- add folder list
            ui.ListBox_folders = wx.wxListBox(ui.Panel_boot, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, 110), {}, 0)
            ui.FlexGridSizer_boot_folders_1:Add(ui.ListBox_folders, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- add sizers
            ui.StaticBoxSizer_boot_folders_0:Add(ui.FlexGridSizer_boot_folders_1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_boot:Add(ui.StaticBoxSizer_boot_folders_0, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- Additional file systems group
        ui.StaticBoxSizer_other_FS_0 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_boot, "Additional file systems")
        ui.FlexGridSizer_other_FS_1 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.FlexGridSizer_other_FS_2 = wx.wxFlexGridSizer(0, 4, 0, 0)

            -- colums descriptions
            ui.StaticText = wx.wxStaticText(ui.Panel_boot, wx.wxID_ANY, "File system", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_other_FS_2:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.StaticText = wx.wxStaticText(ui.Panel_boot, wx.wxID_ANY, "Source file", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_other_FS_2:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.StaticText = wx.wxStaticText(ui.Panel_boot, wx.wxID_ANY, "Mount point", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_other_FS_2:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_other_FS_2:Add(0, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- file system name selection
            ui.Choice_other_FS_name = wx.wxChoice(ui.Panel_boot, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(125, -1), FS_list:get_list())
            ui.FlexGridSizer_other_FS_2:Add(ui.Choice_other_FS_name, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- source file combobox
            ui.ComboBox_other_FS_src = wx.wxComboBox(ui.Panel_boot, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(125, -1), {"", "none"})
            ui.FlexGridSizer_other_FS_2:Add(ui.ComboBox_other_FS_src, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- FS mount point
            ui.ComboBox_other_FS_mntpt = wx.wxComboBox(ui.Panel_boot, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(125, -1), {})
            ui.FlexGridSizer_other_FS_2:Add(ui.ComboBox_other_FS_mntpt, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- add buttons
            ui.Button_other_FS_add = wx.wxButton(ui.Panel_boot, wx.wxNewId(), "Add", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_other_FS_2:Add(ui.Button_other_FS_add, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.Button_other_FS_add:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                    function()
                            local sel      = ui.Choice_other_FS_name:GetSelection()
                            local fs_name  = ui.Choice_other_FS_name:GetString(ifs(sel > -1, sel, 0))
                            local src_file = ui.ComboBox_other_FS_src:GetValue()
                            local mntpt    = ui.ComboBox_other_FS_mntpt:GetValue()

                            if fs_name ~= "" and src_file ~= "" and mntpt:match("^/.*") then
                                    ui.ListView_other_FS:AppendItem(fs_name, src_file, mntpt)
                                    ui.Choice_other_FS_name:SetSelection(0)
                                    ui.ComboBox_other_FS_src:SetValue("")
                                    ui.ComboBox_other_FS_mntpt:SetValue("")
                                    modified:yes()
                            end
                    end
            )

                -- add sizers
                ui.FlexGridSizer_other_FS_1:Add(ui.FlexGridSizer_other_FS_2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

            -- folder list
            ui.ListView_other_FS = wx.wxListView(ui.Panel_boot, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, 150), wx.wxLC_REPORT)
            ui.ListView_other_FS.AppendItem   = insert_item
            ui.ListView_other_FS.GetItemTexts = get_item_texts
            ui.ListView_other_FS:InsertColumn(0, "File system", wx.wxLIST_FORMAT_LEFT, 150)
            ui.ListView_other_FS:InsertColumn(1, "Source file", wx.wxLIST_FORMAT_LEFT, 150)
            ui.ListView_other_FS:InsertColumn(2, "Mount point", wx.wxLIST_FORMAT_LEFT, 150)
            ui.FlexGridSizer_other_FS_1:Add(ui.ListView_other_FS, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- add remove button
            ui.Button_other_FS_remove = wx.wxButton(ui.Panel_boot, wx.wxNewId(), "Remove selected", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_other_FS_1:Add(ui.Button_other_FS_remove, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.Button_other_FS_remove:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                    function()
                            local n = ui.ListView_other_FS:GetFirstSelected()
                            if n > -1 then modified:yes() end

                            while n > -1 do
                                    ui.ListView_other_FS:DeleteItem(n)
                                    n = ui.ListView_other_FS:GetNextSelected(-1)
                            end
                    end
            )

            -- add group
            ui.StaticBoxSizer_other_FS_0:Add(ui.FlexGridSizer_other_FS_1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_boot:Add(ui.StaticBoxSizer_other_FS_0, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- set panel sizer
        ui.Panel_boot:SetSizer(ui.FlexGridSizer_boot)

        return ui.Panel_boot
end


--------------------------------------------------------------------------------
-- @brief  Create widgets for runlevel 0
-- @param  parent       parent window
-- @return Panel object
--------------------------------------------------------------------------------
local function create_runlevel_0_widgets(parent)
        -- create runlevel 0 panel
        ui.Panel_runlevel_0 = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_runlevel_0 = wx.wxFlexGridSizer(0, 1, 0, 0)

        -- runlevel info
        ui.StaticText = wx.wxStaticText(ui.Panel_runlevel_0, wx.wxID_ANY, "The purpose of this runlevel is initialization of driver modules required by application.")
        ui.StaticText:Wrap(ct.CONTROL_X_SIZE)
        ui.FlexGridSizer_runlevel_0:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticLine = wx.wxStaticLine(ui.Panel_runlevel_0, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxSize(10,-1), wx.wxLI_HORIZONTAL, "wx.wxID_ANY")
        ui.FlexGridSizer_runlevel_0:Add(ui.StaticLine, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- driver initialization group
        ui.StaticBoxSizer_drv_init = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_runlevel_0, "Drivers initialization")
        ui.FlexGridSizer_drv_init = wx.wxFlexGridSizer(0, 1, 0, 0)

            -- driver initialization choices sizer
            ui.FlexGridSizer_drv_init_sel = wx.wxFlexGridSizer(0, 3, 0, 0)

                -- driver initialization header
                ui.StaticText = wx.wxStaticText(ui.Panel_runlevel_0, wx.wxID_ANY, "Driver name", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_drv_init_sel:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText = wx.wxStaticText(ui.Panel_runlevel_0, wx.wxID_ANY, "Node path", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_drv_init_sel:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer_drv_init_sel:Add(0,0,1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add driver selection choice
                ui.Choice_drv_name = wx.wxChoice(ui.Panel_runlevel_0, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(150, -1), drv_list:get_list())
                ui.FlexGridSizer_drv_init_sel:Add(ui.Choice_drv_name, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_drv_name:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED,
                        function()
                                local str = ui.Choice_drv_name:GetString(ui.Choice_drv_name:GetSelection())
                                if str ~= "" then
                                        ui.ComboBox_drv_node:SetValue("/dev/"..str)
                                else
                                        ui.ComboBox_drv_node:SetValue("")
                                end
                        end
                )

                -- add driver node path
                ui.ComboBox_drv_node = wx.wxComboBox(ui.Panel_runlevel_0, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(150, -1), {"none"})
                ui.FlexGridSizer_drv_init_sel:Add(ui.ComboBox_drv_node, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add Add button
                ui.Button_drv_add = wx.wxButton(ui.Panel_runlevel_0, wx.wxNewId(), "Add", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_drv_init_sel:Add(ui.Button_drv_add, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Button_drv_add:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                        function()
                                local drv_name  = ui.Choice_drv_name:GetString(ui.Choice_drv_name:GetSelection())
                                local node_path = ui.ComboBox_drv_node:GetValue()

                                if drv_name ~= "" and (node_path:match("^/.*") or node_path == "none") then
                                        ui.ListView_drv_list:AppendItem(drv_name, node_path)
                                        ui.Choice_drv_name:SetSelection(0)
                                        ui.ComboBox_drv_node:SetValue("")

                                        if node_path ~= "none" then
                                                ui.ComboBox_sys_msg_file:Append(node_path)
                                                ui.ComboBox_sd_cards_file:Append(node_path)
                                        end
                                        modified:yes()
                                end
                        end
                )

                -- add driver selection, driver node path, and add button to the group
                ui.FlexGridSizer_drv_init:Add(ui.FlexGridSizer_drv_init_sel, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 0)

            -- add list box
            ui.ListView_drv_list = wx.wxListView(ui.Panel_runlevel_0, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, 300), wx.wxLC_REPORT)
            ui.ListView_drv_list.AppendItem   = insert_item
            ui.ListView_drv_list.GetItemTexts = get_item_texts
            ui.ListView_drv_list:InsertColumn(0, "Driver name", wx.wxLIST_FORMAT_LEFT, 200)
            ui.ListView_drv_list:InsertColumn(1, "Node path", wx.wxLIST_FORMAT_LEFT, 300)
            ui.FlexGridSizer_drv_init:Add(ui.ListView_drv_list, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- add remove button
            ui.Button_drv_init_remove = wx.wxButton(ui.Panel_runlevel_0, wx.wxNewId(), "Remove selected", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_drv_init:Add(ui.Button_drv_init_remove, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.Button_drv_init_remove:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                    function()
                            local n = ui.ListView_drv_list:GetFirstSelected()
                            if n > -1 then modified:yes() end

                            local updated = false
                            while n > -1 do
                                    ui.ListView_drv_list:DeleteItem(n)
                                    n = ui.ListView_drv_list:GetNextSelected(-1)
                                    updated = true
                            end

                            if updated then
                                    local t = {}
                                    for i = 0, ui.ListView_drv_list:GetItemCount() - 1 do
                                            local col = ui.ListView_drv_list:GetItemTexts(i, 2)

                                            if col[1] ~= "none" then
                                                    table.insert(t, col[1])
                                            end
                                    end

                                    ui.ComboBox_sys_msg_file:Clear()
                                    ui.ComboBox_sys_msg_file:Append(t)
                                    ui.ComboBox_sd_cards_file:Clear()
                                    ui.ComboBox_sd_cards_file:Append(t)
                            end
                    end
            )

            -- add driver init group to the panel's sizer
            ui.StaticBoxSizer_drv_init:Add(ui.FlexGridSizer_drv_init, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_runlevel_0:Add(ui.StaticBoxSizer_drv_init, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- printk configuration for runlevel 0 - group
        ui.StaticBoxSizer_sys_msg = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_runlevel_0, "System messages")
        ui.FlexGridSizer_sys_msg = wx.wxFlexGridSizer(0, 2, 0, 0)

            -- add system messages enable checkbox
            ui.CheckBox_sys_msg_en = wx.wxCheckBox(ui.Panel_runlevel_0, wx.wxNewId(), "Show system messages", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_sys_msg:Add(ui.CheckBox_sys_msg_en, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.CheckBox_sys_msg_en:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)

            -- add system messages invitation checkbox
            ui.CheckBox_sys_msg_invitation = wx.wxCheckBox(ui.Panel_runlevel_0, wx.wxNewId(), "Show system welcome message", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_sys_msg:Add(ui.CheckBox_sys_msg_invitation, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.CheckBox_sys_msg_invitation:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)

            -- add selector after which module printk must be enabled
            ui.StaticText = wx.wxStaticText(ui.Panel_runlevel_0, wx.wxID_ANY, "Enable messages after initialization of driver", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_sys_msg:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)

            ui.Choice_sys_msg_init_after = wx.wxChoice(ui.Panel_runlevel_0, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, drv_list:get_list())
            ui.FlexGridSizer_sys_msg:Add(ui.Choice_sys_msg_init_after, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.Choice_sys_msg_init_after:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)

            -- add selection of file used by printk in this runlevel
            ui.StaticText = wx.wxStaticText(ui.Panel_runlevel_0, wx.wxID_ANY, "To show system messages use file", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_sys_msg:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)

            ui.ComboBox_sys_msg_file = wx.wxComboBox(ui.Panel_runlevel_0, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, {})
            ui.FlexGridSizer_sys_msg:Add(ui.ComboBox_sys_msg_file, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.ComboBox_sys_msg_file:Connect(wx.wxEVT_COMMAND_COMBOBOX_SELECTED, function() modified:yes() end)
            ui.ComboBox_sys_msg_file:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)

            -- add group to the panel's main sizer
            ui.StaticBoxSizer_sys_msg:Add(ui.FlexGridSizer_sys_msg, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_runlevel_0:Add(ui.StaticBoxSizer_sys_msg, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- set panel's sizer
        ui.Panel_runlevel_0:SetSizer(ui.FlexGridSizer_runlevel_0)

        return ui.Panel_runlevel_0
end


--------------------------------------------------------------------------------
-- @brief  Create widgets for runlevel 1
-- @param  parent       parent window
-- @return Panel object
--------------------------------------------------------------------------------
local function create_runlevel_1_widgets(parent)
        -- create runlevel 1 panel
        ui.Panel_runlevel_1 = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_runlevel_1 = wx.wxFlexGridSizer(0, 1, 0, 0)

        -- add runlevel 1 description
        ui.StaticText = wx.wxStaticText(ui.Panel_runlevel_1, wx.wxID_ANY, "The purpose of this runlevel is starting daemons, network layer, and mounting file systems from external devices.")
        ui.StaticText:Wrap(ct.CONTROL_X_SIZE)
        ui.FlexGridSizer_runlevel_1:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

        ui.StaticLine4 = wx.wxStaticLine(ui.Panel_runlevel_1, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxLI_HORIZONTAL)
        ui.FlexGridSizer_runlevel_1:Add(ui.StaticLine4, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- create SD cards initialization group
        ui.StaticBoxSizer_sd_cards = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_runlevel_1, "SD cards initialization")
        ui.FlexGridSizer_sd_cards = wx.wxFlexGridSizer(0, 1, 0, 0)

            -- create button sizer
            ui.FlexGridSizer_sd_cards_buttons = wx.wxFlexGridSizer(0, 4, 0, 0)

                -- add header
                ui.StaticText = wx.wxStaticText(ui.Panel_runlevel_1, wx.wxID_ANY, "Card file")
                ui.FlexGridSizer_sd_cards_buttons:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText = wx.wxStaticText(ui.Panel_runlevel_1, wx.wxID_ANY, "File system")
                ui.FlexGridSizer_sd_cards_buttons:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.StaticText = wx.wxStaticText(ui.Panel_runlevel_1, wx.wxID_ANY, "Mount point")
                ui.FlexGridSizer_sd_cards_buttons:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer_sd_cards_buttons:Add(0,0,1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add combobox with SD file path
                ui.ComboBox_sd_cards_file = wx.wxComboBox(ui.Panel_runlevel_1, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(125,-1), {})
                ui.FlexGridSizer_sd_cards_buttons:Add(ui.ComboBox_sd_cards_file, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- file system name selection
                ui.Choice_sd_cards_FS_name = wx.wxChoice(ui.Panel_runlevel_1, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(125, -1), FS_list:get_list())
                ui.FlexGridSizer_sd_cards_buttons:Add(ui.Choice_sd_cards_FS_name, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add combobox with mount point
                ui.ComboBox_sd_cards_mntp = wx.wxComboBox(ui.Panel_runlevel_1, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(125,-1), {})
                ui.FlexGridSizer_sd_cards_buttons:Add(ui.ComboBox_sd_cards_mntp, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add Add button
                ui.Button_sd_cards_add = wx.wxButton(ui.Panel_runlevel_1, wx.wxNewId(), "Add", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_sd_cards_buttons:Add(ui.Button_sd_cards_add, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Button_sd_cards_add:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                        function()
                                local sel      = ui.Choice_sd_cards_FS_name:GetSelection()
                                local fs_name  = ui.Choice_sd_cards_FS_name:GetString(ifs(sel > -1, sel, 0))
                                local src_file = ui.ComboBox_sd_cards_file:GetValue()
                                local mntpt    = ui.ComboBox_sd_cards_mntp:GetValue()

                                if src_file ~= "" then
                                        ui.ListView_sd_cards:AppendItem(src_file, fs_name, mntpt)
                                        ui.Choice_sd_cards_FS_name:SetSelection(0)
                                        ui.ComboBox_sd_cards_file:SetValue("")
                                        ui.ComboBox_sd_cards_mntp:SetValue("")
                                        modified:yes()
                                end
                        end
                )

                -- add button sizer to the SD cards initialization group
                ui.FlexGridSizer_sd_cards:Add(ui.FlexGridSizer_sd_cards_buttons, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 0)

            -- create list with added cards
            ui.ListView_sd_cards = wx.wxListView(ui.Panel_runlevel_1, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, 100), wx.wxLC_REPORT)
            ui.ListView_sd_cards.AppendItem   = insert_item
            ui.ListView_sd_cards.GetItemTexts = get_item_texts
            ui.ListView_sd_cards:InsertColumn(0, "Card file", wx.wxLIST_FORMAT_LEFT, 125)
            ui.ListView_sd_cards:InsertColumn(1, "File system", wx.wxLIST_FORMAT_LEFT, 125)
            ui.ListView_sd_cards:InsertColumn(2, "Mount point", wx.wxLIST_FORMAT_LEFT, 125)
            ui.FlexGridSizer_sd_cards:Add(ui.ListView_sd_cards, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- add remove button
            ui.Button_sd_cards_remove = wx.wxButton(ui.Panel_runlevel_1, wx.wxNewId(), "Remove", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_sd_cards:Add(ui.Button_sd_cards_remove, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.Button_sd_cards_remove:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                    function()
                            local n = ui.ListView_sd_cards:GetFirstSelected()
                            if n > -1 then modified:yes() end

                            while n > -1 do
                                    ui.ListView_sd_cards:DeleteItem(n)
                                    n = ui.ListView_sd_cards:GetNextSelected(-1)
                            end
                    end
            )

            -- add group to main panel
            ui.StaticBoxSizer_sd_cards:Add(ui.FlexGridSizer_sd_cards, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_runlevel_1:Add(ui.StaticBoxSizer_sd_cards, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- create network start group
        ui.StaticBoxSizer_network = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_runlevel_1, "Network start")
        ui.FlexGridSizer_network = wx.wxFlexGridSizer(0, 1, 0, 0)

            -- add DHCP enable checkbox
            ui.CheckBox_network_DHCP = wx.wxCheckBox(ui.Panel_runlevel_1, wx.wxNewId(), "Start DHCP client to get addresses", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_network:Add(ui.CheckBox_network_DHCP, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.CheckBox_network_DHCP:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)

            -- add Static configuration checkbox
            ui.CheckBox_network_static = wx.wxCheckBox(ui.Panel_runlevel_1, wx.wxNewId(), "Set static IP configuration (see Network configuration)", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_network:Add(ui.CheckBox_network_static, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.CheckBox_network_static:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function(event) modified:yes() end)

            -- add info text
            ui.StaticText = wx.wxStaticText(ui.Panel_runlevel_1, wx.wxID_ANY, "NOTE: If static and dynamic network configurations are enabled at the same time, then dynamic "..
                                                                              "configuration will  start first. When DHCP client cannot receive addresses, then static "..
                                                                              "configuration will be applied automatically.")
            ui.StaticText:Wrap(ct.CONTROL_X_SIZE)
            ui.FlexGridSizer_network:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- add group to panel's sizer
            ui.StaticBoxSizer_network:Add(ui.FlexGridSizer_network, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_runlevel_1:Add(ui.StaticBoxSizer_network, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- create daemons group
        ui.StaticBoxSizer_daemons = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_runlevel_1, "Daemons")
        ui.FlexGridSizer_daemons = wx.wxFlexGridSizer(0, 1, 0, 0)

            -- create sizer for buttons
            ui.FlexGridSizer_daemons_buttons = wx.wxFlexGridSizer(0, 5, 0, 0)

                -- add combobox with daemons names (and parameters)
                ui.StaticText = wx.wxStaticText(ui.Panel_runlevel_1, wx.wxID_ANY, "Daemon:")
                ui.FlexGridSizer_daemons_buttons:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.ComboBox_daemons_name = wx.wxComboBox(ui.Panel_runlevel_1, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(150,-1), {}, wx.wxTE_PROCESS_ENTER)
                ui.ComboBox_daemons_name:Connect(wx.wxEVT_COMMAND_TEXT_ENTER, function() ui.Button_daemons_add:Command(wx.wxCommandEvent(wx.wxEVT_COMMAND_BUTTON_CLICKED)) end)
                ui.FlexGridSizer_daemons_buttons:Add(ui.ComboBox_daemons_name, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add CWD path
                ui.StaticText = wx.wxStaticText(ui.Panel_runlevel_1, wx.wxID_ANY, "  CWD:")
                ui.FlexGridSizer_daemons_buttons:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.ComboBox_daemons_CWD = wx.wxComboBox(ui.Panel_runlevel_1, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(150,-1), default_dirs, wx.wxTE_PROCESS_ENTER)
                ui.ComboBox_daemons_CWD:Connect(wx.wxEVT_COMMAND_TEXT_ENTER, function() ui.Button_daemons_add:Command(wx.wxCommandEvent(wx.wxEVT_COMMAND_BUTTON_CLICKED)) end)
                ui.FlexGridSizer_daemons_buttons:Add(ui.ComboBox_daemons_CWD, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add Add button
                ui.Button_daemons_add = wx.wxButton(ui.Panel_runlevel_1, wx.wxNewId(), "Add")
                ui.FlexGridSizer_daemons_buttons:Add(ui.Button_daemons_add, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Button_daemons_add:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                        function()
                                local program = ui.ComboBox_daemons_name:GetValue()
                                local cwd     = ui.ComboBox_daemons_CWD:GetValue()

                                if not cwd:match("^/.*") then cwd = "/"..cwd end

                                if program ~= "" then
                                        ui.ListView_daemons:AppendItem(program, cwd)
                                        ui.ComboBox_daemons_name:SetValue("")
                                        ui.ComboBox_daemons_CWD:SetValue("")
                                        modified:yes()
                                end
                        end
                )

                -- add buttons to group
                ui.FlexGridSizer_daemons:Add(ui.FlexGridSizer_daemons_buttons, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 0)

            -- add daemons list
            ui.ListView_daemons = wx.wxListView(ui.Panel_runlevel_1, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, 150), wx.wxLC_REPORT)
            ui.ListView_daemons.AppendItem   = insert_item
            ui.ListView_daemons.GetItemTexts = get_item_texts
            ui.ListView_daemons:InsertColumn(0, "Daemon", wx.wxLIST_FORMAT_LEFT, 250)
            ui.ListView_daemons:InsertColumn(1, "Working directory", wx.wxLIST_FORMAT_LEFT, 250)
            ui.FlexGridSizer_daemons:Add(ui.ListView_daemons, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- add remove button
            ui.Button_daemons_remove = wx.wxButton(ui.Panel_runlevel_1, wx.wxNewId(), "Remove", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_daemons:Add(ui.Button_daemons_remove, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.Button_daemons_remove:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                    function()
                            local n = ui.ListView_daemons:GetFirstSelected()
                            if n > -1 then modified:yes() end

                            while n > -1 do
                                    ui.ListView_daemons:DeleteItem(n)
                                    n = ui.ListView_daemons:GetNextSelected(-1)
                            end
                    end
            )

            -- add group to runlevel 1 panel
            ui.StaticBoxSizer_daemons:Add(ui.FlexGridSizer_daemons, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_runlevel_1:Add(ui.StaticBoxSizer_daemons, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- set runlevel 1's panel
        ui.Panel_runlevel_1:SetSizer(ui.FlexGridSizer_runlevel_1)

        return ui.Panel_runlevel_1
end


--==============================================================================
-- GLOBAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function startup:create_window(parent)
        if ui.window == nil then
                -- create main window
                ui.window = wx.wxScrolledWindow(parent, wx.wxID_ANY)
                ui.FlexGridSizer_main = wx.wxFlexGridSizer(0, 1, 0, 0)

                -- create main notebook
                ui.Notebook_runlevels = wx.wxNotebook(ui.window, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.Notebook_runlevels:AddPage(create_boot_widgets(ui.Notebook_runlevels), "Runlevel boot", false)
                ui.Notebook_runlevels:AddPage(create_runlevel_0_widgets(ui.Notebook_runlevels), "Runlevel 0", false)
                ui.Notebook_runlevels:AddPage(create_runlevel_1_widgets(ui.Notebook_runlevels), "Runlevel 1", false)
                ui.FlexGridSizer_main:Add(ui.Notebook_runlevels, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- set main sizers
                ui.window:SetSizer(ui.FlexGridSizer_main)
                ui.window:SetScrollRate(5, 5)
        end

        return ui.window
end


--------------------------------------------------------------------------------
-- @brief  Function returns module name
-- @param  None
-- @return Module name
--------------------------------------------------------------------------------
function startup:get_window_name()
        return "System startup"
end


--------------------------------------------------------------------------------
-- @brief  Function is called when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function startup:refresh()
        FS_list:reload()
        ui.Choice_boot_root_FS:Clear()
        ui.Choice_boot_root_FS:Append(FS_list:get_list())
        ui.Choice_other_FS_name:Clear()
        ui.Choice_other_FS_name:Append(FS_list:get_list())

        app_list:reload()
        ui.ComboBox_daemons_name:Clear()
        ui.ComboBox_daemons_name:Append(app_list:get_list())

        load_controls()
end


--------------------------------------------------------------------------------
-- @brief  Function check if options are modified
-- @param  None
-- @return true if options are modified, otherwise false
--------------------------------------------------------------------------------
function startup:is_modified()
        return modified:get_value()
end


--------------------------------------------------------------------------------
-- @brief  Function save configuration
-- @return None
--------------------------------------------------------------------------------
function startup:save()
        save_configuration()
end
