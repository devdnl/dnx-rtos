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

        -- method return index of selected value
        self.get_index_of = function(self, value)
                for i, v in pairs(self._list) do
                        if value == v then
                                return i
                        end
                end

                return 0
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

        -- method return index of selected value
        self.get_index_of = function(self, value)
                for i, v in pairs(self._list) do
                        if value == v then
                                return i
                        end
                end

                return 0
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


--------------------------------------------------------------------------------
-- @brief  Create universal data dialog
-- @param  parent       parent object
-- @param  dialog_name  dialog name
-- @param  grid_size    the size of flex grid sizer used to store user's objects
-- @return New object
--------------------------------------------------------------------------------
local function new_data_dialog(parent, dialog_name, grid_size)
        local self = {}

        self.ShowModal = function(self)
                -- add sizers to main sizer
                self._FlexGridSizer_main:Fit(self._dialog)
                self._FlexGridSizer_main:SetSizeHints(self._dialog)
                self._dialog:ShowModal()
        end

        self.Add = function(self, object)
                self._FlexGridSizer_fields:Add(object, 1, (wx.wxALL+wx.wxEXPAND+wx.wxALIGN_CENTER_HORIZONTAL+wx.wxALIGN_CENTER_VERTICAL), 5)
        end

        self.GetHandle = function(self)
                return self._dialog
        end

        self.SetSaveFunction = function(self, func)
                self._Button_OK:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED, func)
        end

        self.Close = function(self)
                self._dialog:Destroy()
        end

        -- create basic objects
        self._dialog               = wx.wxDialog(parent, wx.wxID_ANY, dialog_name)
        self._FlexGridSizer_main   = wx.wxFlexGridSizer(0, 1, 0, 0)
        self._FlexGridSizer_fields = wx.wxFlexGridSizer(0, grid_size, 0, 0)
        self._BoxSizer_buttons     = wx.wxBoxSizer(wx.wxHORIZONTAL)
        self._Button_Cancel        = wx.wxButton(self._dialog, wx.wxNewId(), "Cancel")
        self._Button_OK            = wx.wxButton(self._dialog, wx.wxNewId(), "OK")
        self._BoxSizer_buttons:Add(self._Button_Cancel, 1, (wx.wxALL+wx.wxALIGN_RIGHT+wx.wxALIGN_CENTER_VERTICAL), 5)
        self._BoxSizer_buttons:Add(self._Button_OK, 1, (wx.wxALL+wx.wxALIGN_RIGHT+wx.wxALIGN_CENTER_VERTICAL), 5)
        self._Button_Cancel:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED, function() self._dialog:Destroy() end)
        self._FlexGridSizer_main:Add(self._FlexGridSizer_fields, 1, (wx.wxALL+wx.wxEXPAND+wx.wxALIGN_CENTER_HORIZONTAL+wx.wxALIGN_CENTER_VERTICAL), 0)
        self._FlexGridSizer_main:Add(self._BoxSizer_buttons, 1, (wx.wxALL+wx.wxALIGN_RIGHT+wx.wxALIGN_CENTER_VERTICAL), 0)
        self._dialog:SetSizer(self._FlexGridSizer_main)

        return self
end


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local ui = {}
local ID = {}
local modified       = ct:new_modify_indicator()
local default_dirs   = {"/", "/dev", "/home", "/mnt", "/proc", "/srv", "/tmp", "/usr"}
local FS_list        = new_FS_list()
local drv_list       = new_driver_list()
local app_list       = new_app_list()
local INITD_CFG_FILE = config.project.path.initd_cfg_file:GetValue()

--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function generate code of init daemon according to configuration.
--         Function ask user to overwrite current initd code.
-- @param  cfg      initd configuration
-- @return None
--------------------------------------------------------------------------------
local function generate_init_code(cfg)
        local INITD_TEMPLATE_FILE              = config.project.path.initd_template_entire_file:GetValue()
        local INITD_TEMPLATE_MOUNT             = config.project.path.initd_template_mount_file:GetValue()
        local INITD_TEMPLATE_MKDIR             = config.project.path.initd_template_mkdir_file:GetValue()
        local INITD_TEMPLATE_DRVINIT           = config.project.path.initd_template_driverinit_file:GetValue()
        local INITD_TEMPLATE_PRINTKEN          = config.project.path.initd_template_printken_file:GetValue()
        local INITD_TEMPLATE_PRINTKDIS         = config.project.path.initd_template_printkdis_file:GetValue()
        local INITD_TEMPLATE_INVMSG            = config.project.path.initd_template_ivitationmsg_file:GetValue()
        local INITD_TEMPLATE_DAEMON_START      = config.project.path.initd_template_daemonstart_file:GetValue()
        local INITD_TEMPLATE_STORAGE_INIT      = config.project.path.initd_template_storage_init_file:GetValue()
        local INITD_TEMPLATE_DHCP_OR_STATIC_IP = config.project.path.initd_template_dhcp_or_static_ip_file:GetValue()
        local INITD_TEMPLATE_DHCP_IP           = config.project.path.initd_template_dhcp_ip_file:GetValue()
        local INITD_TEMPLATE_STATIC_IP         = config.project.path.initd_template_static_ip_file:GetValue()
        local INITD_TEMPLATE_IP_SUMMARY        = config.project.path.initd_template_ip_summary_file:GetValue()
        local INITD_TEMPLATE_SUMMARY           = config.project.path.initd_template_summary_file:GetValue()
        local INITD_TEMPLATE_APP_PREPARE       = config.project.path.initd_template_app_prepare_file:GetValue()
        local INITD_TEMPLATE_APP_START         = config.project.path.initd_template_app_start_file:GetValue()
        local INITD_TEMPLATE_APP_FINISH        = config.project.path.initd_template_app_finish_file:GetValue()
        local INITD_TEMPLATE_APP_STREAM_OPEN   = config.project.path.initd_template_app_stream_open_file:GetValue()
        local INITD_SRC_FILE                   = config.project.path.initd_src_file:GetValue()

        local answer = ct:show_question_msg(ct.MAIN_WINDOW_NAME,
                                            "Do you want to generate initd code based on current configuration?\n\n"..
                                            "Click 'Yes' to apply initd configuration and generate new code (current initd code will be OVERWRITTEN).\n\n"..
                                            "Click 'No' to discard generation of initd code and leave current initd implementation with no changes.",
                                            wx.wxYES_NO,
                                            ui.window)
        if answer == wx.wxID_YES then

                -- clear initd.c file
                f = io.open(INITD_SRC_FILE, "wb")
                if f then f:close() end

                -- apply initd template
                local tags = {}
                table.insert(tags, {tag = "<!year!>", to = os.date("%Y")})
                ct:apply_template(INITD_TEMPLATE_FILE, INITD_SRC_FILE, tags, 1)

                -- create runlevel boot code
                local regex = "^%s*static%s+int%s+run_level_boot%(.*%)$"
                local n     = ct:find_line(INITD_SRC_FILE, 1, regex)
                if n then
                        n = n + 2

                        -- root file system
                        local tags = {}
                        table.insert(tags, {tag = "<!msg!>",         to = ""})
                        table.insert(tags, {tag = "<!file_system!>", to = cfg.runlevel_boot.base_FS})
                        table.insert(tags, {tag = "<!source_file!>", to = ""})
                        table.insert(tags, {tag = "<!mount_point!>", to = "/"})
                        n = n + ct:apply_template(INITD_TEMPLATE_MOUNT, INITD_SRC_FILE, tags, n)

                        -- new folders
                        for i = 1, #cfg.runlevel_boot.folders do
                                local tags = {}
                                table.insert(tags, {tag = "<!dir!>", to = cfg.runlevel_boot.folders[i]})
                                n = n + ct:apply_template(INITD_TEMPLATE_MKDIR, INITD_SRC_FILE, tags, n)
                        end

                        -- additional file systems
                        for i = 1, #cfg.runlevel_boot.additional_FS do
                                local item = cfg.runlevel_boot.additional_FS[i]
                                local tags = {}
                                table.insert(tags, {tag = "<!msg!>",         to = ""})
                                table.insert(tags, {tag = "<!file_system!>", to = item.file_system})
                                table.insert(tags, {tag = "<!source_file!>", to = ifs(item.source_file == "none", "", item.source_file)})
                                table.insert(tags, {tag = "<!mount_point!>", to = item.mount_point})
                                n = n + ct:apply_template(INITD_TEMPLATE_MOUNT, INITD_SRC_FILE, tags, n)
                        end
                end

                -- create runlevel 0 code
                local regex = "^%s*static%s+int%s+run_level_0%(.*%)$"
                local n     = ct:find_line(INITD_SRC_FILE, n, regex)
                if n then
                        n = n + 2

                        -- add driver to initialize
                        for i = 1, #cfg.runlevel_0.driver_init do
                                local drv = cfg.runlevel_0.driver_init[i]
                                local tags = {}
                                table.insert(tags, {tag = "<!driver!>", to = drv.name})
                                table.insert(tags, {tag = "<!node!>", to = ifs(drv.node == "none", "NULL", '"'..drv.node..'"')})
                                n = n + ct:apply_template(INITD_TEMPLATE_DRVINIT, INITD_SRC_FILE, tags, n)

                                if cfg.runlevel_0.system_messages.show == true then
                                        if drv.name == cfg.runlevel_0.system_messages.init_after then
                                                local tags = {}
                                                table.insert(tags, {tag = "<!node!>", to = cfg.runlevel_0.system_messages.file})
                                                n = n + ct:apply_template(INITD_TEMPLATE_PRINTKEN, INITD_SRC_FILE, tags, n)

                                                if cfg.runlevel_0.system_messages.invitation == true then
                                                        local tags = {}
                                                        n = n + ct:apply_template(INITD_TEMPLATE_INVMSG, INITD_SRC_FILE, tags, n)
                                                end
                                        end
                                end
                        end
                end

                -- create runlevel 1 code
                local regex = "^%s*static%s+int%s+run_level_1%(.*%)$"
                local n     = ct:find_line(INITD_SRC_FILE, n, regex)
                if n then
                        n = n + 2

                        -- add daemons
                        for i = 1, #cfg.runlevel_1.daemons do
                                local daemon = cfg.runlevel_1.daemons[i]
                                local tags = {}
                                table.insert(tags, {tag = "<!name!>", to = daemon.name})
                                table.insert(tags, {tag = "<!CWD!>", to = daemon.CWD})
                                n = n + ct:apply_template(INITD_TEMPLATE_DAEMON_START, INITD_SRC_FILE, tags, n)
                        end

                        -- initialize SD cards
                        for i = 1, #cfg.runlevel_1.storage_init do
                                local tags = {}
                                table.insert(tags, {tag = "<!storage_path!>", to = cfg.runlevel_1.storage_init[i]})
                                n = n + ct:apply_template(INITD_TEMPLATE_STORAGE_INIT, INITD_SRC_FILE, tags, n)
                        end

                        -- mount table
                        for i = 1, #cfg.runlevel_1.mount_table do
                                local item = cfg.runlevel_1.mount_table[i]
                                local tags = {}
                                table.insert(tags, {tag = "<!msg!>",         to = "msg_"})
                                table.insert(tags, {tag = "<!file_system!>", to = item.file_system})
                                table.insert(tags, {tag = "<!source_file!>", to = ifs(item.source_file == "none", "", item.source_file)})
                                table.insert(tags, {tag = "<!mount_point!>", to = item.mount_point})
                                n = n + ct:apply_template(INITD_TEMPLATE_MOUNT, INITD_SRC_FILE, tags, n)
                        end

                        -- network start
                        if cfg.runlevel_1.network.DHCP == true and cfg.runlevel_1.network.static == true then
                                n = n + ct:apply_template(INITD_TEMPLATE_DHCP_OR_STATIC_IP, INITD_SRC_FILE, {}, n)
                        elseif cfg.runlevel_1.network.DHCP == true then
                                n = n + ct:apply_template(INITD_TEMPLATE_DHCP_IP, INITD_SRC_FILE, {}, n)
                        elseif cfg.runlevel_1.network.static == true then
                                n = n + ct:apply_template(INITD_TEMPLATE_STATIC_IP, INITD_SRC_FILE, {}, n)
                        end

                        if cfg.runlevel_1.network.DHCP == true or cfg.runlevel_1.network.static == true then
                                n = n + ct:apply_template(INITD_TEMPLATE_IP_SUMMARY, INITD_SRC_FILE, {}, n)
                        end
                end

                -- create runlevel 2 code
                local regex = "^%s*static%s+int%s+run_level_2%(.*%)$"
                local n     = ct:find_line(INITD_SRC_FILE, n, regex)
                if n then
                        n = n + 2

                        -- add daemons
                        for i = 1, #cfg.runlevel_2.applications do
                                local app = cfg.runlevel_2.applications[i]

                                if app.stdin == "none" and app.stdout == "none" and app.stderr == "none" then
                                        local tags = {}
                                        table.insert(tags, {tag = "<!name!>", to = app.name})
                                        table.insert(tags, {tag = "<!CWD!>", to = app.CWD})
                                        n = n + ct:apply_template(INITD_TEMPLATE_DAEMON_START, INITD_SRC_FILE, tags, n)
                                end
                        end

                        -- add initd stack info
                        if cfg.runlevel_0.system_messages.show == true then
                                n = n + ct:apply_template(INITD_TEMPLATE_SUMMARY, INITD_SRC_FILE, {}, n)
                        end

                        -- enable/disable printk
                        if cfg.runlevel_2.system_messages.show == true then
                                local tags = {}
                                table.insert(tags, {tag = "<!node!>", to = cfg.runlevel_2.system_messages.file})
                                n = n + ct:apply_template(INITD_TEMPLATE_PRINTKEN, INITD_SRC_FILE, tags, n)
                        else
                                n = n + ct:apply_template(INITD_TEMPLATE_PRINTKDIS, INITD_SRC_FILE, {}, n)
                        end

                        -- calculate number of applications and streams
                        local number_of_applications = 0
                        local unique_streams         = {}
                        for i = 1, #cfg.runlevel_2.applications do
                                local app = cfg.runlevel_2.applications[i]
                                if app.stdin ~= "none" or app.stdout ~= "none" or app.stderr ~= "none" then
                                        number_of_applications = number_of_applications + 1
                                end

                                unique_streams[app.stdin]  = true
                                unique_streams[app.stdout] = true
                                unique_streams[app.stderr] = true
                        end

                        if unique_streams["none"] ~= nil then
                                unique_streams["none"] = nil
                        end

                        local number_of_streams = 0;
                        for k, v in pairs(unique_streams) do
                                unique_streams[k] = number_of_streams
                                number_of_streams = number_of_streams + 1
                        end

                        -- add applications
                        if number_of_applications > 0 then
                                -- create application handles and streams
                                local tags = {}
                                table.insert(tags, {tag = "<!number_of_applications!>", to = number_of_applications})
                                table.insert(tags, {tag = "<!number_of_streams!>", to = number_of_streams})
                                n = n + ct:apply_template(INITD_TEMPLATE_APP_PREPARE, INITD_SRC_FILE, tags, n)

                                -- open streams
                                for stream_path in pairs(unique_streams) do
                                        local tags = {}
                                        table.insert(tags, {tag = "<!stream_number!>", to = unique_streams[stream_path]})
                                        table.insert(tags, {tag = "<!stream_path!>", to = stream_path})
                                        n = n + ct:apply_template(INITD_TEMPLATE_APP_STREAM_OPEN, INITD_SRC_FILE, tags, n)
                                end

                                -- start applications
                                local application_number = 0
                                for i = 1, #cfg.runlevel_2.applications do
                                        local app = cfg.runlevel_2.applications[i]

                                        if app.stdin ~= "none" or app.stdout ~= "none" or app.stderr ~= "none" then
                                                local tags = {}
                                                table.insert(tags, {tag = "<!application_number!>", to = application_number})
                                                table.insert(tags, {tag = "<!name!>",   to = app.name})
                                                table.insert(tags, {tag = "<!CWD!>",    to = app.CWD})

                                                if app.stdin  == "none" then app.stdin  = "NULL" else app.stdin  = "f["..unique_streams[app.stdin].."]" end
                                                if app.stdout == "none" then app.stdout = "NULL" else app.stdout = "f["..unique_streams[app.stdout].."]" end
                                                if app.stderr == "none" then app.stderr = "NULL" else app.stderr = "f["..unique_streams[app.stderr].."]" end

                                                table.insert(tags, {tag = "<!stdin!>",  to = app.stdin})
                                                table.insert(tags, {tag = "<!stdout!>", to = app.stdout})
                                                table.insert(tags, {tag = "<!stderr!>", to = app.stderr})
                                                n = n + ct:apply_template(INITD_TEMPLATE_APP_START, INITD_SRC_FILE, tags, n)

                                                application_number = application_number + 1
                                        end
                                end

                                -- add code that wait for finishing all running programs before goes to runlevel exit
                                n = n + ct:apply_template(INITD_TEMPLATE_APP_FINISH, INITD_SRC_FILE, {}, n)
                        end
                end
        end
end

--------------------------------------------------------------------------------
-- @brief  Function loads all controls from configuration files
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_configuration()
        local cfg = ct:load_table(INITD_CFG_FILE)

        -------------------
        -- RUNLEVEL BOOT --
        -------------------
        -- load base file system
        ui.Choice_RLB_root_FS:SetSelection(FS_list:get_index_of(cfg.runlevel_boot.base_FS) - 1)

        -- load folders to create
        ui.ListBox_RLB_folders:Clear()
--         ui.ComboBox_RLB_other_FS_mntpt:Clear() FIXME
--         ui.ComboBox_RL1_FS_mount_mntpt:Clear() FIXME
--         ui.ComboBox_RL2_app_start_CWD:Clear() FIXME
        for i = 1, #cfg.runlevel_boot.folders do
                local dirname = cfg.runlevel_boot.folders[i]
                ui.ListBox_RLB_folders:Append(dirname)
--                 ui.ComboBox_RLB_other_FS_mntpt:Append(dirname) FIXME
--                 ui.ComboBox_RL1_FS_mount_mntpt:Append(dirname) FIXME
--                 ui.ComboBox_RL2_app_start_CWD:Append(dirname) FIXME
        end

        -- load additional file systems to mount
        ui.ListView_RLB_other_FS:DeleteAllItems()
        for i = 1, #cfg.runlevel_boot.additional_FS do
                local item = cfg.runlevel_boot.additional_FS[i]
                ui.ListView_RLB_other_FS:AppendItem({item.file_system, item.source_file, item.mount_point})
        end

        ----------------
        -- RUNLEVEL 0 --
        ----------------
        -- load list of drivers to initialize
        ui.ListView_RL0_drv_list:DeleteAllItems()
        ui.ComboBox_RL0_sys_msg_file:Clear()
--         ui.ComboBox_RL1_storage_file:Clear() FIXME
--         ui.ComboBox_RL1_FS_mount_src:Clear() FIXME
        ui.ComboBox_RL2_sys_msg_file:Clear()
--         ui.ComboBox_RL2_app_start_stdin:Clear() FIXME
--         ui.ComboBox_RL2_app_start_stdout:Clear() FIXME
--         ui.ComboBox_RL2_app_start_stderr:Clear() FIXME

        for i = 1, #cfg.runlevel_0.driver_init do
                local item = cfg.runlevel_0.driver_init[i]
                ui.ListView_RL0_drv_list:AppendItem({item.name, item.node})
                ui.ComboBox_RL0_sys_msg_file:Append(item.node)
--                 ui.ComboBox_RL1_storage_file:Append(item.node) FIXME
--                 ui.ComboBox_RL1_FS_mount_src:Append(item.node) FIXME
                ui.ComboBox_RL2_sys_msg_file:Append(item.node)
--                 ui.ComboBox_RL2_app_start_stdin:Append(item.node) FIXME
--                 ui.ComboBox_RL2_app_start_stdout:Append(item.node) FIXME
--                 ui.ComboBox_RL2_app_start_stderr:Append(item.node) FIXME
        end

        -- load system messages configuration
        ui.CheckBox_RL0_sys_msg_en:SetValue(cfg.runlevel_0.system_messages.show)
        ui.Panel_RL0_sys_msg_sub:Enable(cfg.runlevel_0.system_messages.show)
        ui.CheckBox_RL0_sys_msg_invitation:SetValue(cfg.runlevel_0.system_messages.invitation)
        ui.ComboBox_RL0_sys_msg_file:SetValue(cfg.runlevel_0.system_messages.file)
        ui.Choice_RL0_sys_msg_init_after:SetSelection(drv_list:get_index_of(cfg.runlevel_0.system_messages.init_after) - 1)

        ----------------
        -- RUNLEVEL 1 --
        ----------------
        -- load daemon list
        ui.ListView_RL1_daemons:DeleteAllItems()
        for i = 1, #cfg.runlevel_1.daemons do
                local item = cfg.runlevel_1.daemons[i]
                ui.ListView_RL1_daemons:AppendItem({item.name, item.CWD})
        end

        -- storage initialization
        ui.ListBox_RL1_storage:Clear()
        for i = 1, #cfg.runlevel_1.storage_init do
                ui.ListBox_RL1_storage:Append(cfg.runlevel_1.storage_init[i])
        end

        -- mount table
        ui.ListView_RL1_FS_mount:DeleteAllItems()
        for i = 1, #cfg.runlevel_1.mount_table do
                local item = cfg.runlevel_1.mount_table[i]
                ui.ListView_RL1_FS_mount:AppendItem({item.file_system, item.source_file, item.mount_point})
        end

        -- network start
        ui.CheckBox_RL1_network_DHCP:SetValue(cfg.runlevel_1.network.DHCP)
        ui.CheckBox_RL1_network_static:SetValue(cfg.runlevel_1.network.static)
        ui.CheckBox_RL1_network_summary:SetValue(cfg.runlevel_1.network.summary)

        ----------------
        -- RUNLEVEL 2 --
        ----------------
        -- system messages
        ui.CheckBox_RL2_sys_msg_en:SetValue(cfg.runlevel_2.system_messages.show)
        ui.ComboBox_RL2_sys_msg_file:SetValue(cfg.runlevel_2.system_messages.file)

        -- load applications list
        ui.ListView_RL2_app_start:DeleteAllItems()
        for i = 1, #cfg.runlevel_2.applications do
                local item = cfg.runlevel_2.applications[i]
                ui.ListView_RL2_app_start:AppendItem({item.name, item.CWD, item.stdin, item.stdout, item.stderr})
        end

        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function save_configuration()
        local cfg = {}

        -------------------
        -- RUNLEVEL BOOT --
        -------------------
        cfg.runlevel_boot = {}

        -- root FS
        local sel = ui.Choice_RLB_root_FS:GetSelection()
        cfg.runlevel_boot.base_FS = ui.Choice_RLB_root_FS:GetString(ifs(sel > -1, sel, 0))

        -- new folder list
        cfg.runlevel_boot.folders = {}
        for i = 0, ui.ListBox_RLB_folders:GetCount() - 1 do
                table.insert(cfg.runlevel_boot.folders, ui.ListBox_RLB_folders:GetString(i))
        end

        -- additional file system mount
        cfg.runlevel_boot.additional_FS = {}
        for i = 0, ui.ListView_RLB_other_FS:GetItemCount() - 1 do
                local cols = ui.ListView_RLB_other_FS:GetItemTexts(i, 3)

                local item = {}
                item.file_system = cols[1]
                item.source_file = cols[2]
                item.mount_point = cols[3]

                table.insert(cfg.runlevel_boot.additional_FS, item)
        end


        ----------------
        -- RUNLEVEL 0 --
        ----------------
        cfg.runlevel_0 = {}

        -- list of drivers to initialize
        cfg.runlevel_0.driver_init = {}
        for i = 0, ui.ListView_RL0_drv_list:GetItemCount() - 1 do
                local cols = ui.ListView_RL0_drv_list:GetItemTexts(i, 2)

                local item = {}
                item.name = cols[1]
                item.node = cols[2]

                table.insert(cfg.runlevel_0.driver_init, item)
        end

        -- system messages
        cfg.runlevel_0.system_messages = {}
        cfg.runlevel_0.system_messages.show = ui.CheckBox_RL0_sys_msg_en:GetValue()
        cfg.runlevel_0.system_messages.invitation = ui.CheckBox_RL0_sys_msg_invitation:GetValue()
        cfg.runlevel_0.system_messages.file = ui.ComboBox_RL0_sys_msg_file:GetValue()
        local sel = ui.Choice_RL0_sys_msg_init_after:GetSelection()
        cfg.runlevel_0.system_messages.init_after = ui.Choice_RL0_sys_msg_init_after:GetString(ifs(sel > -1, sel, 0))


        ----------------
        -- RUNLEVEL 1 --
        ----------------
        cfg.runlevel_1 = {}

        -- early started daemons
        cfg.runlevel_1.daemons = {}
        for i = 0, ui.ListView_RL1_daemons:GetItemCount() - 1 do
                local cols = ui.ListView_RL1_daemons:GetItemTexts(i, 2)

                local item = {}
                item.name = cols[1]
                item.CWD  = cols[2]

                table.insert(cfg.runlevel_1.daemons, item)
        end

        -- storage initialization
        cfg.runlevel_1.storage_init = {}
        for i = 0, ui.ListBox_RL1_storage:GetCount() - 1 do
                table.insert(cfg.runlevel_1.storage_init, ui.ListBox_RL1_storage:GetString(i))
        end

        -- mount table
        cfg.runlevel_1.mount_table = {}
        for i = 0, ui.ListView_RL1_FS_mount:GetItemCount() - 1 do
                local cols = ui.ListView_RL1_FS_mount:GetItemTexts(i, 3)

                local item = {}
                item.file_system = cols[1]
                item.source_file = cols[2]
                item.mount_point = cols[3]

                table.insert(cfg.runlevel_1.mount_table, item)
        end

        -- network configuration
        cfg.runlevel_1.network = {}
        cfg.runlevel_1.network.DHCP    = ui.CheckBox_RL1_network_DHCP:GetValue()
        cfg.runlevel_1.network.static  = ui.CheckBox_RL1_network_static:GetValue()
        cfg.runlevel_1.network.summary = ui.CheckBox_RL1_network_summary:GetValue()


        ----------------
        -- RUNLEVEL 2 --
        ----------------
        cfg.runlevel_2 = {}

        -- system messages
        cfg.runlevel_2.system_messages = {}
        cfg.runlevel_2.system_messages.show = ui.CheckBox_RL2_sys_msg_en:GetValue()
        cfg.runlevel_2.system_messages.file = ui.ComboBox_RL2_sys_msg_file:GetValue()

        -- applications start
        cfg.runlevel_2.applications = {}
        for i = 0, ui.ListView_RL2_app_start:GetItemCount() - 1 do
                local cols = ui.ListView_RL2_app_start:GetItemTexts(i, 5)

                local item = {}
                item.name   = cols[1]
                item.CWD    = cols[2]
                item.stdin  = cols[3]
                item.stdout = cols[4]
                item.stderr = cols[5]

                table.insert(cfg.runlevel_2.applications, item)
        end


        -- save configuration to file
        ct:save_table(cfg, INITD_CFG_FILE)

        generate_init_code(cfg)

        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Insert item to the wxListView
-- @param  self     wxListView
-- @param  col      table with data that will be insterted to next columns
-- @return None
--------------------------------------------------------------------------------
local function wxListView_insert_item(self, col)
        local count = self:GetItemCount()
        self:InsertItem(count, "")

        for i, v in pairs(col) do
                self:SetItem(count, i - 1, v)
        end

        self:Focus(count)
end


--------------------------------------------------------------------------------
-- @brief  Remove selected items form the wxListView
-- @param  self     wxListView
-- @return true if removed, otherwise false
--------------------------------------------------------------------------------
local function wxListView_remove_selected_items(self)
        local n = self:GetFirstSelected()
        local r = false

        while n > -1 do
                self:DeleteItem(n)
                n = self:GetNextSelected(-1)
                r = true
        end

        return r
end


--------------------------------------------------------------------------------
-- @brief  Update item of the wxListView
-- @param  self     wxListView
-- @param  row      row to update
-- @param  col      table with data that will be insterted to next columns
-- @return None
--------------------------------------------------------------------------------
local function wxListView_update_item(self, row, col)
        local count = self:GetItemCount()

        assert(row < count, "wxListView_update_item(): row out of bounds")

        for i, v in pairs(col) do
                self:SetItem(row, i - 1, v)
        end
end


--------------------------------------------------------------------------------
-- @brief  Get item's texts
-- @param  self     wxListView
-- @param  row      row read
-- @param  n        number of columns to read
-- @return Next columns values as table
--------------------------------------------------------------------------------
local function wxListView_get_item_texts(self, row, no_of_cols)
        local item = wx.wxListItem()
        item:SetId(row)
        item:SetMask(wx.wxLIST_MASK_TEXT)

        local t = {}
        for i = 0, no_of_cols - 1 do
                item:SetColumn(i)
                self:GetItem(item)
                table.insert(t, item:GetText())
        end

        return t
end


--------------------------------------------------------------------------------
-- @brief  Move selected item in the wxListView
-- @param  self     wxListView
-- @param  row      row read
-- @param  move     movement (-1 or 1)
-- @param  n        number of colums in the item
-- @return None
--------------------------------------------------------------------------------
local function wxListView_move_item(self, move, n)
        local sel   = self:GetFirstSelected()
        local count = self:GetItemCount()

        if ((move == -1 and sel > 0) or (move == 1 and sel < count - 1)) and n > 0 then
                self:Freeze()
                local newp = wxListView_get_item_texts(self, sel + move, n)
                local curr = wxListView_get_item_texts(self, sel, n)
                wxListView_update_item(self, sel + move, curr)
                wxListView_update_item(self, sel, newp)
                self:Select(sel, false)
                self:Select(sel + move, true)
                self:Focus(sel + move)
                self:Thaw()
        end
end


--------------------------------------------------------------------------------
-- @brief  Create panel with boot runlevel
-- @param  parent       parent window
-- @return Panel object
--------------------------------------------------------------------------------
local function create_runlevel_boot_widgets(parent)
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

            ui.StaticText = wx.wxStaticText(ui.Panel_boot, wx.wxID_ANY, "Select base file system:", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_boot_main_FS_1:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            ui.Choice_RLB_root_FS = wx.wxChoice(ui.Panel_boot, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(100, -1), FS_list:get_list())
            ui.FlexGridSizer_boot_main_FS_1:Add(ui.Choice_RLB_root_FS, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.Choice_RLB_root_FS:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)

            ui.StaticBoxSizer_boot_main_FS_0:Add(ui.FlexGridSizer_boot_main_FS_1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_boot:Add(ui.StaticBoxSizer_boot_main_FS_0, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- folders group
        ui.StaticBoxSizer_boot_folders_0 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_boot, "Create folders")
        ui.FlexGridSizer_boot_folders_1 = wx.wxFlexGridSizer(0, 2, 0, 0)

            -- add folder list
            ui.ListBox_RLB_folders = wx.wxListBox(ui.Panel_boot, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, 150), {}, 0)
            ui.FlexGridSizer_boot_folders_1:Add(ui.ListBox_RLB_folders, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
            ui.ListBox_RLB_folders.GetList = function(self) local t = {} for i = 0, self:GetCount() - 1 do table.insert(t, self:GetString(i)) end return t end

                -- new folder sizer
                ui.FlexGridSizer_boot_folders_2 = wx.wxFlexGridSizer(0, 1, 0, 0)

                -- add button
                ui.Button_RLB_folder_add = wx.wxBitmapButton(ui.Panel_boot, wx.wxNewId(), ct.icon.list_add_16x16)
                ui.Button_RLB_folder_add:SetToolTip("Add")
                ui.FlexGridSizer_boot_folders_2:Add(ui.Button_RLB_folder_add, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
                ui.Button_RLB_folder_add:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                        function()
                                -- show window with parameters to fill
                                local dialog = new_data_dialog(ui.window, "Add new folder", 1)
                                local ComboBox_folder_name = wx.wxComboBox(dialog:GetHandle(), wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(300, -1), default_dirs, wx.wxTE_PROCESS_ENTER)

                                local function save()
                                        local dirname = ComboBox_folder_name:GetValue()

                                        if dirname ~= "" then
                                                if not dirname:match("^/.*") then dirname = "/"..dirname end

                                                ui.ListBox_RLB_folders:InsertItems({dirname}, ui.ListBox_RLB_folders:GetCount())
                                                ui.ListBox_RLB_folders:SetSelection(ui.ListBox_RLB_folders:GetCount() - 1)
                                                modified:yes()
                                                dialog:Close()
                                        end
                                end

                                ComboBox_folder_name:Connect(wx.wxEVT_COMMAND_TEXT_ENTER, function() save() end)
                                ComboBox_folder_name:SetFocus()
                                dialog:Add(ComboBox_folder_name)
                                dialog:SetSaveFunction(save)
                                dialog:ShowModal()
                        end
                )

                -- remove button
                ui.Button_RLB_folder_remove = wx.wxBitmapButton(ui.Panel_boot, wx.wxNewId(), ct.icon.edit_delete_16x16)
                ui.Button_RLB_folder_remove:SetToolTip("Remove")
                ui.FlexGridSizer_boot_folders_2:Add(ui.Button_RLB_folder_remove, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
                ui.Button_RLB_folder_remove:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                        function()
                                local sel   = ui.ListBox_RLB_folders:GetSelection()
                                local t     = {}
                                local count = ui.ListBox_RLB_folders:GetCount()
                                for i = 0, count - 1 do
                                        if i ~= sel then
                                                local str = ui.ListBox_RLB_folders:GetString(i)
                                                if str ~= "" then
                                                        table.insert(t, str)
                                                end
                                        end
                                end

                                ui.ListBox_RLB_folders:Clear()
                                ui.ListBox_RLB_folders:InsertItems(t, 0)
                                ui.ListBox_RLB_folders:SetSelection(ifs(sel >= count - 1, count - 2, sel))
                                modified:yes()
                        end
                )

                -- add new folder sizer to folder group
                ui.FlexGridSizer_boot_folders_1:Add(ui.FlexGridSizer_boot_folders_2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_TOP), 0)

            -- add sizers
            ui.StaticBoxSizer_boot_folders_0:Add(ui.FlexGridSizer_boot_folders_1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
            ui.FlexGridSizer_boot:Add(ui.StaticBoxSizer_boot_folders_0, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- Additional file systems group
        ui.StaticBoxSizer_other_FS_0 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_boot, "Additional file systems")
        ui.FlexGridSizer_other_FS_1 = wx.wxFlexGridSizer(0, 2, 0, 0)
        ui.FlexGridSizer_other_FS_2 = wx.wxFlexGridSizer(0, 1, 0, 0)

            -- FS list
            ui.ListView_RLB_other_FS = wx.wxListView(ui.Panel_boot, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, 150), wx.wxLC_REPORT)
            ui.ListView_RLB_other_FS.AppendItem   = wxListView_insert_item
            ui.ListView_RLB_other_FS.GetItemTexts = wxListView_get_item_texts
            ui.ListView_RLB_other_FS.UpdateItem   = wxListView_update_item
            ui.ListView_RLB_other_FS.MoveItem     = wxListView_move_item
            ui.ListView_RLB_other_FS:InsertColumn(0, "File system", wx.wxLIST_FORMAT_LEFT, 150)
            ui.ListView_RLB_other_FS:InsertColumn(1, "Source file", wx.wxLIST_FORMAT_LEFT, 150)
            ui.ListView_RLB_other_FS:InsertColumn(2, "Mount point", wx.wxLIST_FORMAT_LEFT, 150)
            ui.FlexGridSizer_other_FS_1:Add(ui.ListView_RLB_other_FS, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)

            -- add Add buttons
            ui.Button_other_FS_add = wx.wxBitmapButton(ui.Panel_boot, wx.wxNewId(), ct.icon.list_add_16x16)
            ui.Button_other_FS_add:SetToolTip("Add")
            ui.FlexGridSizer_other_FS_2:Add(ui.Button_other_FS_add, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
            ui.Button_other_FS_add:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                    function()
                            -- show window with parameters to fill
                            local dialog = new_data_dialog(ui.window, "Add mount parameters", 3)
                            dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "File system"))
                            dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "Source file"))
                            dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "Mount point"))
                            local Choice_RLB_other_FS_name = wx.wxChoice(dialog:GetHandle(), wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(125, -1), FS_list:get_list())
                            local ComboBox_RLB_other_FS_src = wx.wxComboBox(dialog:GetHandle(), wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(125, -1), {"none"})
                            local ComboBox_RLB_other_FS_mntpt = wx.wxComboBox(dialog:GetHandle(), wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(125, -1), ui.ListBox_RLB_folders:GetList())

                            local function save()
                                    local sel         = Choice_RLB_other_FS_name:GetSelection()
                                    local fs_name     = Choice_RLB_other_FS_name:GetString(ifs(sel > -1, sel, 0))
                                    local src_file    = ComboBox_RLB_other_FS_src:GetValue()
                                    local mount_point = ComboBox_RLB_other_FS_mntpt:GetValue()

                                    if not mount_point:match("^/.*") then mount_point = "/"..mount_point end
                                    if src_file == "" then src_file = "none" end

                                    if fs_name ~= "" then
                                            ui.ListView_RLB_other_FS:AppendItem({fs_name, src_file, mount_point})
                                            modified:yes()
                                            dialog:Close()
                                    end
                            end

                            Choice_RLB_other_FS_name:SetFocus()
                            dialog:Add(Choice_RLB_other_FS_name)
                            dialog:Add(ComboBox_RLB_other_FS_src)
                            dialog:Add(ComboBox_RLB_other_FS_mntpt)
                            dialog:SetSaveFunction(save)
                            dialog:ShowModal()
                    end
            )

            -- add remove button
            ui.Button_RLB_other_FS_remove = wx.wxBitmapButton(ui.Panel_boot, wx.wxNewId(), ct.icon.edit_delete_16x16)
            ui.Button_RLB_other_FS_remove:SetToolTip("Remove")
            ui.FlexGridSizer_other_FS_2:Add(ui.Button_RLB_other_FS_remove, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
            ui.Button_RLB_other_FS_remove:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                    function()
                            local n = ui.ListView_RLB_other_FS:GetFirstSelected()
                            if n > -1 then modified:yes() end

                            while n > -1 do
                                    ui.ListView_RLB_other_FS:DeleteItem(n)
                                    n = ui.ListView_RLB_other_FS:GetNextSelected(-1)
                            end
                    end
            )

            -- add edit button
            ui.Button_RLB_other_FS_edit = wx.wxBitmapButton(ui.Panel_boot, wx.wxNewId(), ct.icon.document_edit_16x16)
            ui.Button_RLB_other_FS_edit:SetToolTip("Edit")
            ui.FlexGridSizer_other_FS_2:Add(ui.Button_RLB_other_FS_edit, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
            ui.Button_RLB_other_FS_edit:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                    function()
                            local selected_item = ui.ListView_RLB_other_FS:GetFirstSelected()
                            if selected_item > -1 then
                                    -- show window with parameters to fill
                                    local dialog = new_data_dialog(ui.window, "Entry modification", 3)
                                    dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "File system"))
                                    dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "Source file"))
                                    dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "Mount point"))
                                    local Choice_RLB_other_FS_name = wx.wxChoice(dialog:GetHandle(), wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(125, -1), FS_list:get_list())
                                    local ComboBox_RLB_other_FS_src = wx.wxComboBox(dialog:GetHandle(), wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(125, -1), {"none"})
                                    local dirlist = {} for i = 0, ui.ListBox_RLB_folders:GetCount() - 1 do table.insert(dirlist, ui.ListBox_RLB_folders:GetString(i)) end
                                    local ComboBox_RLB_other_FS_mntpt = wx.wxComboBox(dialog:GetHandle(), wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(125, -1), dirlist)

                                    local col = ui.ListView_RLB_other_FS:GetItemTexts(selected_item, 3)
                                    Choice_RLB_other_FS_name:SetSelection(FS_list:get_index_of(col[1]) - 1)
                                    ComboBox_RLB_other_FS_src:SetValue(col[2])
                                    ComboBox_RLB_other_FS_mntpt:SetValue(col[3])

                                    local function save()
                                            local sel         = Choice_RLB_other_FS_name:GetSelection()
                                            local fs_name     = Choice_RLB_other_FS_name:GetString(ifs(sel > -1, sel, 0))
                                            local src_file    = ComboBox_RLB_other_FS_src:GetValue()
                                            local mount_point = ComboBox_RLB_other_FS_mntpt:GetValue()

                                            if not mount_point:match("^/.*") then mount_point = "/"..mount_point end
                                            if src_file == "" then src_file = "none" end

                                            if fs_name ~= "" then
                                                    ui.ListView_RLB_other_FS:UpdateItem(selected_item, {fs_name, src_file, mount_point})
                                                    modified:yes()
                                                    dialog:Close()
                                            end
                                    end

                                    Choice_RLB_other_FS_name:SetFocus()
                                    dialog:Add(Choice_RLB_other_FS_name)
                                    dialog:Add(ComboBox_RLB_other_FS_src)
                                    dialog:Add(ComboBox_RLB_other_FS_mntpt)
                                    dialog:SetSaveFunction(save)
                                    dialog:ShowModal()
                            end
                    end
            )

            -- add: arrow up button
            ui.Button_RLB_other_FS_up = wx.wxBitmapButton(ui.Panel_boot, wx.wxNewId(), ct.icon.arrow_up_16x16)
            ui.Button_RLB_other_FS_up:SetToolTip("Move up")
            ui.FlexGridSizer_other_FS_2:Add(ui.Button_RLB_other_FS_up, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
            ui.Button_RLB_other_FS_up:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED, function() ui.ListView_RLB_other_FS:MoveItem(-1, 3) end)

            -- add: arrow down button
            ui.Button_RLB_other_FS_down = wx.wxBitmapButton(ui.Panel_boot, wx.wxNewId(), ct.icon.arrow_down_16x16)
            ui.Button_RLB_other_FS_down:SetToolTip("Move down")
            ui.FlexGridSizer_other_FS_2:Add(ui.Button_RLB_other_FS_down, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
            ui.Button_RLB_other_FS_down:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED, function() ui.ListView_RLB_other_FS:MoveItem(1, 3) end)

            -- add sizers
            ui.FlexGridSizer_other_FS_1:Add(ui.FlexGridSizer_other_FS_2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

            -- add group
            ui.StaticBoxSizer_other_FS_0:Add(ui.FlexGridSizer_other_FS_1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
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
        ui.FlexGridSizer_drv_init = wx.wxFlexGridSizer(0, 2, 0, 0)

            -- add list box
            ui.ListView_RL0_drv_list = wx.wxListView(ui.Panel_runlevel_0, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, 350), wx.wxLC_REPORT)
            ui.ListView_RL0_drv_list.AppendItem   = wxListView_insert_item
            ui.ListView_RL0_drv_list.GetItemTexts = wxListView_get_item_texts
            ui.ListView_RL0_drv_list.UpdateItem   = wxListView_update_item
            ui.ListView_RL0_drv_list.MoveItem     = wxListView_move_item
            ui.ListView_RL0_drv_list.GetNodeList  = function(self) local t = {} for i = 0, self:GetItemCount() - 1 do local n = self:GetItemTexts(i, 2)[2] if n ~= "none" then table.insert(t, n) end end return t end
            ui.ListView_RL0_drv_list:InsertColumn(0, "Driver name", wx.wxLIST_FORMAT_LEFT, 200)
            ui.ListView_RL0_drv_list:InsertColumn(1, "Node path", wx.wxLIST_FORMAT_LEFT, 300)
            ui.FlexGridSizer_drv_init:Add(ui.ListView_RL0_drv_list, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)

            -- driver initialization choices sizer
            ui.FlexGridSizer_drv_init_buttons = wx.wxFlexGridSizer(0, 1, 0, 0)

                -- add: Add button
                ui.Button_RL0_drv_init_add = wx.wxBitmapButton(ui.Panel_runlevel_0, wx.wxNewId(), ct.icon.list_add_16x16)
                ui.Button_RL0_drv_init_add:SetToolTip("Add")
                ui.FlexGridSizer_drv_init_buttons:Add(ui.Button_RL0_drv_init_add, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
                ui.Button_RL0_drv_init_add:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                        function()
                                -- show window with parameters to fill
                                local dialog = new_data_dialog(ui.window, "Add driver", 2)
                                dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "Driver name"))
                                dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "Node path"))
                                local Choice_RL0_drv_name   = wx.wxChoice(dialog:GetHandle(), wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(150, -1), drv_list:get_list())
                                local ComboBox_RL0_drv_node = wx.wxComboBox(dialog:GetHandle(), wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(150, -1), {"none"})

                                Choice_RL0_drv_name:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED,
                                        function()
                                                local str = Choice_RL0_drv_name:GetString(Choice_RL0_drv_name:GetSelection())
                                                if str ~= "" then
                                                        ComboBox_RL0_drv_node:SetValue("/dev/"..str)
                                                else
                                                        ComboBox_RL0_drv_node:SetValue("")
                                                end
                                        end
                                )

                                local function save()
                                        local drv_name  = Choice_RL0_drv_name:GetString(Choice_RL0_drv_name:GetSelection())
                                        local node_path = ComboBox_RL0_drv_node:GetValue()

                                        if drv_name ~= "" and (node_path:match("^/.*") or node_path == "none") then
                                                ui.ListView_RL0_drv_list:AppendItem({drv_name, node_path})
                                                modified:yes()
                                                dialog:Close()
                                        end
                                end

                                Choice_RL0_drv_name:SetFocus()
                                dialog:Add(Choice_RL0_drv_name)
                                dialog:Add(ComboBox_RL0_drv_node)
                                dialog:SetSaveFunction(save)
                                dialog:ShowModal()
                        end
                )

                -- add: remove button
                ui.Button_RL0_drv_init_remove = wx.wxBitmapButton(ui.Panel_runlevel_0, wx.wxNewId(), ct.icon.edit_delete_16x16)
                ui.Button_RL0_drv_init_remove:SetToolTip("Remove")
                ui.FlexGridSizer_drv_init_buttons:Add(ui.Button_RL0_drv_init_remove, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
                ui.Button_RL0_drv_init_remove:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                        function()
                                local n = ui.ListView_RL0_drv_list:GetFirstSelected()
                                if n > -1 then modified:yes() end

                                local updated = false
                                while n > -1 do
                                        ui.ListView_RL0_drv_list:DeleteItem(n)
                                        n = ui.ListView_RL0_drv_list:GetNextSelected(-1)
                                        updated = true
                                end
                        end
                )

                -- add: edit button
                ui.Button_RL0_drv_init_edit = wx.wxBitmapButton(ui.Panel_runlevel_0, wx.wxNewId(), ct.icon.document_edit_16x16)
                ui.Button_RL0_drv_init_edit:SetToolTip("Edit")
                ui.FlexGridSizer_drv_init_buttons:Add(ui.Button_RL0_drv_init_edit, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
                ui.Button_RL0_drv_init_edit:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                        function()
                                local selected_item = ui.ListView_RL0_drv_list:GetFirstSelected()
                                if selected_item > -1 then
                                        -- show window with parameters to fill
                                        local dialog = new_data_dialog(ui.window, "Entry modification", 2)
                                        dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "Driver name"))
                                        dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "Node path"))
                                        local Choice_RL0_drv_name   = wx.wxChoice(dialog:GetHandle(), wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(150, -1), drv_list:get_list())
                                        local ComboBox_RL0_drv_node = wx.wxComboBox(dialog:GetHandle(), wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(150, -1), {"none"})

                                        local col = ui.ListView_RL0_drv_list:GetItemTexts(selected_item, 2)
                                        Choice_RL0_drv_name:SetSelection(drv_list:get_index_of(col[1]) - 1)
                                        ComboBox_RL0_drv_node:SetValue(col[2])

                                        Choice_RL0_drv_name:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED,
                                                function()
                                                        local str = Choice_RL0_drv_name:GetString(Choice_RL0_drv_name:GetSelection())
                                                        if str ~= "" then
                                                                ComboBox_RL0_drv_node:Append("/dev/"..str)
                                                        end
                                                end
                                        )

                                        local function save()
                                                local drv_name  = Choice_RL0_drv_name:GetString(Choice_RL0_drv_name:GetSelection())
                                                local node_path = ComboBox_RL0_drv_node:GetValue()

                                                if drv_name ~= "" and (node_path:match("^/.*") or node_path == "none") then
                                                        ui.ListView_RL0_drv_list:UpdateItem(selected_item, {drv_name, node_path})
                                                        modified:yes()
                                                        dialog:Close()
                                                end
                                        end

                                        Choice_RL0_drv_name:SetFocus()
                                        dialog:Add(Choice_RL0_drv_name)
                                        dialog:Add(ComboBox_RL0_drv_node)
                                        dialog:SetSaveFunction(save)
                                        dialog:ShowModal()
                                end
                        end
                )

                -- add: arrow up button
                ui.Button_RL0_drv_init_up = wx.wxBitmapButton(ui.Panel_runlevel_0, wx.wxNewId(), ct.icon.arrow_up_16x16)
                ui.Button_RL0_drv_init_up:SetToolTip("Move up")
                ui.FlexGridSizer_drv_init_buttons:Add(ui.Button_RL0_drv_init_up, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
                ui.Button_RL0_drv_init_up:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED, function() ui.ListView_RL0_drv_list:MoveItem(-1, 2) end)

                -- add: arrow down button
                ui.Button_RL0_drv_init_down = wx.wxBitmapButton(ui.Panel_runlevel_0, wx.wxNewId(), ct.icon.arrow_down_16x16)
                ui.Button_RL0_drv_init_down:SetToolTip("Move down")
                ui.FlexGridSizer_drv_init_buttons:Add(ui.Button_RL0_drv_init_down, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
                ui.Button_RL0_drv_init_down:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED, function() ui.ListView_RL0_drv_list:MoveItem(1, 2) end)

                -- add: driver selection, driver node path, and add button to the group
                ui.FlexGridSizer_drv_init:Add(ui.FlexGridSizer_drv_init_buttons, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_TOP), 0)

            -- add driver init group to the panel's sizer
            ui.StaticBoxSizer_drv_init:Add(ui.FlexGridSizer_drv_init, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_TOP), 5)
            ui.FlexGridSizer_runlevel_0:Add(ui.StaticBoxSizer_drv_init, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)

        -- printk configuration for runlevel 0 - group
        ui.StaticBoxSizer_sys_msg = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_runlevel_0, "System messages")
        ui.FlexGridSizer_sys_msg = wx.wxFlexGridSizer(0, 1, 0, 0)

            -- add system messages enable checkbox
            ui.CheckBox_RL0_sys_msg_en = wx.wxCheckBox(ui.Panel_runlevel_0, wx.wxNewId(), "Show system messages")
            ui.FlexGridSizer_sys_msg:Add(ui.CheckBox_RL0_sys_msg_en, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.CheckBox_RL0_sys_msg_en:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED,
                    function(event)
                            ui.Panel_RL0_sys_msg_sub:Enable(event:IsChecked())
                            modified:yes()
                    end
            )

            -- create panel for sub-options
            ui.Panel_RL0_sys_msg_sub = wx.wxPanel(ui.Panel_runlevel_0, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
            ui.FlexGridSizer_RL0_sys_msg_sub = wx.wxFlexGridSizer(0, 2, 0, 0)

                -- add system messages invitation checkbox
                ui.CheckBox_RL0_sys_msg_invitation = wx.wxCheckBox(ui.Panel_RL0_sys_msg_sub, wx.wxNewId(), "Show system invitation", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.CheckBox_RL0_sys_msg_invitation:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
                ui.FlexGridSizer_RL0_sys_msg_sub:Add(ui.CheckBox_RL0_sys_msg_invitation, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer_RL0_sys_msg_sub:Add(0,0,1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add selector after which module printk must be enabled
                ui.StaticText = wx.wxStaticText(ui.Panel_RL0_sys_msg_sub, wx.wxID_ANY, "Enable messages after initialization of driver")
                ui.FlexGridSizer_RL0_sys_msg_sub:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.Choice_RL0_sys_msg_init_after = wx.wxChoice(ui.Panel_RL0_sys_msg_sub, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, drv_list:get_list())
                ui.Choice_RL0_sys_msg_init_after:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
                ui.FlexGridSizer_RL0_sys_msg_sub:Add(ui.Choice_RL0_sys_msg_init_after, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add selection of file used by the printk in this runlevel
                ui.StaticText = wx.wxStaticText(ui.Panel_RL0_sys_msg_sub, wx.wxID_ANY, "To show system messages use file")
                ui.FlexGridSizer_RL0_sys_msg_sub:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_RIGHT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.ComboBox_RL0_sys_msg_file = wx.wxComboBox(ui.Panel_RL0_sys_msg_sub, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, {})
                ui.ComboBox_RL0_sys_msg_file:Connect(wx.wxEVT_COMMAND_COMBOBOX_SELECTED, function() modified:yes() end)
                ui.ComboBox_RL0_sys_msg_file:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
                ui.FlexGridSizer_RL0_sys_msg_sub:Add(ui.ComboBox_RL0_sys_msg_file, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- set panel sizer
                ui.Panel_RL0_sys_msg_sub:SetSizer(ui.FlexGridSizer_RL0_sys_msg_sub)
                ui.FlexGridSizer_sys_msg:Add(ui.Panel_RL0_sys_msg_sub, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

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

        -- create daemons group
        ui.StaticBoxSizer_daemons = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_runlevel_1, "Early started daemons")
        ui.FlexGridSizer_daemons = wx.wxFlexGridSizer(0, 2, 0, 0)

            -- add daemons list
            ui.ListView_RL1_daemons = wx.wxListView(ui.Panel_runlevel_1, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, 125), wx.wxLC_REPORT)
            ui.ListView_RL1_daemons.AppendItem   = wxListView_insert_item
            ui.ListView_RL1_daemons.UpdateItem   = wxListView_update_item
            ui.ListView_RL1_daemons.GetItemTexts = wxListView_get_item_texts
            ui.ListView_RL1_daemons.MoveItem     = wxListView_move_item
            ui.ListView_RL1_daemons:InsertColumn(0, "Daemon", wx.wxLIST_FORMAT_LEFT, 250)
            ui.ListView_RL1_daemons:InsertColumn(1, "Working directory", wx.wxLIST_FORMAT_LEFT, 250)
            ui.FlexGridSizer_daemons:Add(ui.ListView_RL1_daemons, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)

            -- create sizer for buttons
            ui.FlexGridSizer_daemons_buttons = wx.wxFlexGridSizer(0, 1, 0, 0)

                -- add Add button
                ui.Button_RL1_daemons_add = wx.wxBitmapButton(ui.Panel_runlevel_1, wx.wxNewId(), ct.icon.list_add_16x16)
                ui.Button_RL1_daemons_add:SetToolTip("Add")
                ui.FlexGridSizer_daemons_buttons:Add(ui.Button_RL1_daemons_add, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
                ui.Button_RL1_daemons_add:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                        function()
                                -- show window with parameters to fill
                                local dialog = new_data_dialog(ui.window, "Add program", 2)
                                dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "Command"))
                                dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "Working directory"))
                                local ComboBox_RL1_daemons_name = wx.wxComboBox(dialog:GetHandle(), wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(150,-1), app_list:get_list(), wx.wxTE_PROCESS_ENTER)
                                local ComboBox_RL1_daemons_CWD = wx.wxComboBox(dialog:GetHandle(), wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(150,-1), default_dirs, wx.wxTE_PROCESS_ENTER)

                                local function save()
                                        local program = ComboBox_RL1_daemons_name:GetValue()
                                        local cwd     = ComboBox_RL1_daemons_CWD:GetValue()

                                        if not cwd:match("^/.*") then cwd = "/"..cwd end

                                        if program ~= "" then
                                                ui.ListView_RL1_daemons:AppendItem({program, cwd})
                                                modified:yes()
                                                dialog:Close()
                                        end
                                end

                                ComboBox_RL1_daemons_name:Connect(wx.wxEVT_COMMAND_TEXT_ENTER, save)
                                ComboBox_RL1_daemons_CWD:Connect(wx.wxEVT_COMMAND_TEXT_ENTER, save)

                                ComboBox_RL1_daemons_name:SetFocus()
                                dialog:Add(ComboBox_RL1_daemons_name)
                                dialog:Add(ComboBox_RL1_daemons_CWD)
                                dialog:SetSaveFunction(save)
                                dialog:ShowModal()
                        end
                )

                -- add remove button
                ui.Button_RL1_daemons_remove = wx.wxBitmapButton(ui.Panel_runlevel_1, wx.wxNewId(), ct.icon.edit_delete_16x16)
                ui.Button_RL1_daemons_remove:SetToolTip("Remove")
                ui.FlexGridSizer_daemons_buttons:Add(ui.Button_RL1_daemons_remove, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
                ui.Button_RL1_daemons_remove:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                        function()
                                local n = ui.ListView_RL1_daemons:GetFirstSelected()
                                if n > -1 then modified:yes() end

                                while n > -1 do
                                        ui.ListView_RL1_daemons:DeleteItem(n)
                                        n = ui.ListView_RL1_daemons:GetNextSelected(-1)
                                end
                        end
                )

                -- add: edit button
                ui.Button_RL1_daemons_edit = wx.wxBitmapButton(ui.Panel_runlevel_1, wx.wxNewId(), ct.icon.document_edit_16x16)
                ui.Button_RL1_daemons_edit:SetToolTip("Edit")
                ui.FlexGridSizer_daemons_buttons:Add(ui.Button_RL1_daemons_edit, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
                ui.Button_RL1_daemons_edit:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                        function()
                                local selected_item = ui.ListView_RL1_daemons:GetFirstSelected()
                                if selected_item > -1 then
                                        -- show window with parameters to fill
                                        local dialog = new_data_dialog(ui.window, "Entry modification", 2)
                                        dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "Command"))
                                        dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "Working directory"))
                                        local ComboBox_RL1_daemons_name = wx.wxComboBox(dialog:GetHandle(), wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(150,-1), app_list:get_list(), wx.wxTE_PROCESS_ENTER)
                                        local ComboBox_RL1_daemons_CWD = wx.wxComboBox(dialog:GetHandle(), wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(150,-1), default_dirs, wx.wxTE_PROCESS_ENTER)

                                        local col = ui.ListView_RL1_daemons:GetItemTexts(selected_item, 2)
                                        ComboBox_RL1_daemons_name:SetValue(col[1])
                                        ComboBox_RL1_daemons_CWD:SetValue(col[2])

                                        local function save()
                                                local program = ComboBox_RL1_daemons_name:GetValue()
                                                local cwd     = ComboBox_RL1_daemons_CWD:GetValue()

                                                if not cwd:match("^/.*") then cwd = "/"..cwd end

                                                if program ~= "" then
                                                        ui.ListView_RL1_daemons:UpdateItem(selected_item, {program, cwd})
                                                        modified:yes()
                                                end

                                                dialog:Close()
                                        end

                                        ComboBox_RL1_daemons_name:Connect(wx.wxEVT_COMMAND_TEXT_ENTER, save)
                                        ComboBox_RL1_daemons_CWD:Connect(wx.wxEVT_COMMAND_TEXT_ENTER, save)

                                        ComboBox_RL1_daemons_name:SetFocus()
                                        dialog:Add(ComboBox_RL1_daemons_name)
                                        dialog:Add(ComboBox_RL1_daemons_CWD)
                                        dialog:SetSaveFunction(save)
                                        dialog:ShowModal()
                                end
                        end
                )

                -- add: arrow up button
                ui.Button_RL1_daemons_up = wx.wxBitmapButton(ui.Panel_runlevel_1, wx.wxNewId(), ct.icon.arrow_up_16x16)
                ui.Button_RL1_daemons_up:SetToolTip("Move up")
                ui.FlexGridSizer_daemons_buttons:Add(ui.Button_RL1_daemons_up, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
                ui.Button_RL1_daemons_up:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED, function() ui.ListView_RL1_daemons:MoveItem(-1, 2) end)

                -- add: arrow down button
                ui.Button_RL1_daemons_down = wx.wxBitmapButton(ui.Panel_runlevel_1, wx.wxNewId(), ct.icon.arrow_down_16x16)
                ui.Button_RL1_daemons_down:SetToolTip("Move down")
                ui.FlexGridSizer_daemons_buttons:Add(ui.Button_RL1_daemons_down, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
                ui.Button_RL1_daemons_down:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED, function() ui.ListView_RL1_daemons:MoveItem(1, 2) end)

                -- add buttons to group
                ui.FlexGridSizer_daemons:Add(ui.FlexGridSizer_daemons_buttons, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_TOP), 0)

            -- add group to runlevel 1 panel
            ui.StaticBoxSizer_daemons:Add(ui.FlexGridSizer_daemons, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_runlevel_1:Add(ui.StaticBoxSizer_daemons, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- create storage initialization group
        ui.StaticBoxSizer_storage_init = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_runlevel_1, "Storage initialization")
        ui.FlexGridSizer_storage_init = wx.wxFlexGridSizer(0, 2, 0, 0)

            -- create list with added cards
            ui.ListBox_RL1_storage  = wx.wxListBox(ui.Panel_runlevel_1, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, 110), {}, 0)
            ui.FlexGridSizer_storage_init:Add(ui.ListBox_RL1_storage, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)

            -- create button sizer
            ui.FlexGridSizer_storage_init_buttons = wx.wxFlexGridSizer(0, 1, 0, 0)

                -- add Add button
                ui.Button_RL1_storage_add = wx.wxBitmapButton(ui.Panel_runlevel_1, wx.wxNewId(), ct.icon.list_add_16x16)
                ui.Button_RL1_storage_add:SetToolTip("Add")
                ui.FlexGridSizer_storage_init_buttons:Add(ui.Button_RL1_storage_add, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
                ui.Button_RL1_storage_add:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                        function()
                                -- show window with parameters to fill
                                local dialog = new_data_dialog(ui.window, "Add storage device", 1)
                                local ComboBox_RL1_storage_file = wx.wxComboBox(dialog:GetHandle(), wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(300,-1), ui.ListView_RL0_drv_list:GetNodeList(), wx.wxTE_PROCESS_ENTER)

                                local function save()
                                        local src_file = ComboBox_RL1_storage_file:GetValue()
                                        if src_file ~= "" then
                                                ui.ListBox_RL1_storage:Append(src_file)
                                                modified:yes()
                                                dialog:Close()
                                        end
                                end

                                ComboBox_RL1_storage_file:Connect(wx.wxEVT_COMMAND_TEXT_ENTER, save)
                                ComboBox_RL1_storage_file:SetFocus()
                                dialog:Add(ComboBox_RL1_storage_file)
                                dialog:SetSaveFunction(save)
                                dialog:ShowModal()
                        end
                )

                -- add remove button
                ui.Button_RL1_storage_remove = wx.wxBitmapButton(ui.Panel_runlevel_1, wx.wxNewId(), ct.icon.edit_delete_16x16)
                ui.Button_RL1_daemons_remove:SetToolTip("Remove")
                ui.FlexGridSizer_storage_init_buttons:Add(ui.Button_RL1_storage_remove, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
                ui.Button_RL1_storage_remove:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                        function()
                                local sel   = ui.ListBox_RL1_storage:GetSelection()
                                local t     = {}
                                local count = ui.ListBox_RL1_storage:GetCount()
                                for i = 0, count - 1 do
                                        if i ~= sel then
                                                local str = ui.ListBox_RL1_storage:GetString(i)
                                                if str ~= "" then
                                                        table.insert(t, str)
                                                end
                                        end
                                end

                                ui.ListBox_RL1_storage:Clear()
                                ui.ListBox_RL1_storage:Append(t)
                                ui.ListBox_RL1_storage:SetSelection(ifs(sel >= count - 1, count - 2, sel))
                                modified:yes()
                        end
                )

                -- add button sizer to the storage initialization group
                ui.FlexGridSizer_storage_init:Add(ui.FlexGridSizer_storage_init_buttons, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_TOP), 0)

            -- add group to main panel
            ui.StaticBoxSizer_storage_init:Add(ui.FlexGridSizer_storage_init, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_runlevel_1:Add(ui.StaticBoxSizer_storage_init, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)


        -- mount list
        ui.StaticBoxSizer_FS_mount = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_runlevel_1, "Mount table")
        ui.FlexGridSizer_FS_mount = wx.wxFlexGridSizer(0, 2, 0, 0)

            -- FS list
            ui.ListView_RL1_FS_mount = wx.wxListView(ui.Panel_runlevel_1, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, 150), wx.wxLC_REPORT)
            ui.ListView_RL1_FS_mount.AppendItem   = wxListView_insert_item
            ui.ListView_RL1_FS_mount.UpdateItem   = wxListView_update_item
            ui.ListView_RL1_FS_mount.GetItemTexts = wxListView_get_item_texts
            ui.ListView_RL1_FS_mount.MoveItem     = wxListView_move_item
            ui.ListView_RL1_FS_mount:InsertColumn(0, "File system", wx.wxLIST_FORMAT_LEFT, 150)
            ui.ListView_RL1_FS_mount:InsertColumn(1, "Source file", wx.wxLIST_FORMAT_LEFT, 150)
            ui.ListView_RL1_FS_mount:InsertColumn(2, "Mount point", wx.wxLIST_FORMAT_LEFT, 150)
            ui.FlexGridSizer_FS_mount:Add(ui.ListView_RL1_FS_mount, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)

            -- create sizer for buttons
            ui.FlexGridSizer_FS_mount_buttons = wx.wxFlexGridSizer(0, 1, 0, 0)

                -- add buttons
                ui.Button_RL1_FS_mount_add = wx.wxBitmapButton(ui.Panel_runlevel_1, wx.wxNewId(), ct.icon.list_add_16x16)
                ui.Button_RL1_FS_mount_add:SetToolTip("Add")
                ui.FlexGridSizer_FS_mount_buttons:Add(ui.Button_RL1_FS_mount_add, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
                ui.Button_RL1_FS_mount_add:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                        function()
                                -- show window with parameters to fill
                                local dialog = new_data_dialog(ui.window, "Add mount parameters", 3)
                                dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "File system"))
                                dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "Source file"))
                                dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "Mount point"))
                                local Choice_RL1_FS_mount_name    = wx.wxChoice(dialog:GetHandle(), wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(125, -1), FS_list:get_list())
                                local ComboBox_RL1_FS_mount_src   = wx.wxComboBox(dialog:GetHandle(), wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(125, -1), ui.ListView_RL0_drv_list:GetNodeList())
                                local ComboBox_RL1_FS_mount_mntpt = wx.wxComboBox(dialog:GetHandle(), wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(125, -1), ui.ListBox_RLB_folders:GetList())

                                local function save()
                                        local sel         = Choice_RL1_FS_mount_name:GetSelection()
                                        local file_system = Choice_RL1_FS_mount_name:GetString(ifs(sel > -1, sel, 0))
                                        local src_file    = ComboBox_RL1_FS_mount_src:GetValue()
                                        local mount_point = ComboBox_RL1_FS_mount_mntpt:GetValue()

                                        if not mount_point:match("^/.*") then mount_point = "/"..mount_point end
                                        if src_file == "" then src_file = "none" end

                                        if file_system ~= "" then
                                                ui.ListView_RL1_FS_mount:AppendItem({file_system, src_file, mount_point})
                                                modified:yes()
                                                dialog:Close()
                                        end
                                end

                                Choice_RL1_FS_mount_name:SetFocus()
                                dialog:Add(Choice_RL1_FS_mount_name)
                                dialog:Add(ComboBox_RL1_FS_mount_src)
                                dialog:Add(ComboBox_RL1_FS_mount_mntpt)
                                dialog:SetSaveFunction(save)
                                dialog:ShowModal()
                        end
                )

                -- add remove button
                ui.Button_RL1_FS_mount_remove = wx.wxBitmapButton(ui.Panel_runlevel_1, wx.wxNewId(), ct.icon.edit_delete_16x16)
                ui.Button_RL1_FS_mount_remove:SetToolTip("Remove")
                ui.FlexGridSizer_FS_mount_buttons:Add(ui.Button_RL1_FS_mount_remove, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
                ui.Button_RL1_FS_mount_remove:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                        function()
                                local n = ui.ListView_RL1_FS_mount:GetFirstSelected()
                                if n > -1 then modified:yes() end

                                while n > -1 do
                                        ui.ListView_RL1_FS_mount:DeleteItem(n)
                                        n = ui.ListView_RL1_FS_mount:GetNextSelected(-1)
                                end
                        end
                )

                -- add: edit button
                ui.Button_RL1_FS_mount_edit = wx.wxBitmapButton(ui.Panel_runlevel_1, wx.wxNewId(), ct.icon.document_edit_16x16)
                ui.Button_RL1_FS_mount_edit:SetToolTip("Edit")
                ui.FlexGridSizer_FS_mount_buttons:Add(ui.Button_RL1_FS_mount_edit, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
                ui.Button_RL1_FS_mount_edit:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                        function()
                                local selected_item = ui.ListView_RL1_FS_mount:GetFirstSelected()
                                if selected_item > -1 then
                                        -- show window with parameters to fill
                                        local dialog = new_data_dialog(ui.window, "Entry modification", 3)
                                        dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "File system"))
                                        dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "Source file"))
                                        dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "Mount point"))
                                        local Choice_RL1_FS_mount_name    = wx.wxChoice(dialog:GetHandle(), wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(125, -1), FS_list:get_list())
                                        local ComboBox_RL1_FS_mount_src   = wx.wxComboBox(dialog:GetHandle(), wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(125, -1), ui.ListView_RL0_drv_list:GetNodeList())
                                        local ComboBox_RL1_FS_mount_mntpt = wx.wxComboBox(dialog:GetHandle(), wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(125, -1), ui.ListBox_RLB_folders:GetList())

                                        local col = ui.ListView_RL1_FS_mount:GetItemTexts(selected_item, 3)
                                        Choice_RL1_FS_mount_name:SetSelection(FS_list:get_index_of(col[1]) - 1)
                                        ComboBox_RL1_FS_mount_src:SetValue(col[2])
                                        ComboBox_RL1_FS_mount_mntpt:SetValue(col[3])

                                        local function save()
                                                local sel         = Choice_RL1_FS_mount_name:GetSelection()
                                                local file_system = Choice_RL1_FS_mount_name:GetString(ifs(sel > -1, sel, 0))
                                                local src_file    = ComboBox_RL1_FS_mount_src:GetValue()
                                                local mount_point = ComboBox_RL1_FS_mount_mntpt:GetValue()

                                                if not mount_point:match("^/.*") then mount_point = "/"..mount_point end
                                                if src_file == "" then src_file = "none" end

                                                if file_system ~= "" then
                                                        ui.ListView_RL1_FS_mount:UpdateItem(selected_item, {file_system, src_file, mount_point})
                                                        modified:yes()
                                                        dialog:Close()
                                                end
                                        end

                                        Choice_RL1_FS_mount_name:SetFocus()
                                        dialog:Add(Choice_RL1_FS_mount_name)
                                        dialog:Add(ComboBox_RL1_FS_mount_src)
                                        dialog:Add(ComboBox_RL1_FS_mount_mntpt)
                                        dialog:SetSaveFunction(save)
                                        dialog:ShowModal()
                                end
                        end
                )

                -- add: arrow up button
                ui.Button_RL1_FS_mount_up = wx.wxBitmapButton(ui.Panel_runlevel_1, wx.wxNewId(), ct.icon.arrow_up_16x16)
                ui.Button_RL1_FS_mount_up:SetToolTip("Move up")
                ui.FlexGridSizer_FS_mount_buttons:Add(ui.Button_RL1_FS_mount_up, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
                ui.Button_RL1_FS_mount_up:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED, function() ui.ListView_RL1_FS_mount:MoveItem(-1, 2) end)

                -- add: arrow down button
                ui.Button_RL1_FS_mount_down = wx.wxBitmapButton(ui.Panel_runlevel_1, wx.wxNewId(), ct.icon.arrow_down_16x16)
                ui.Button_RL1_FS_mount_down:SetToolTip("Move down")
                ui.FlexGridSizer_FS_mount_buttons:Add(ui.Button_RL1_FS_mount_down, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
                ui.Button_RL1_FS_mount_down:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED, function() ui.ListView_RL1_FS_mount:MoveItem(1, 2) end)

                -- add buttons to group
                ui.FlexGridSizer_FS_mount:Add(ui.FlexGridSizer_FS_mount_buttons, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_TOP), 0)

            -- add group to runlevel 1 panel
            ui.StaticBoxSizer_FS_mount:Add(ui.FlexGridSizer_FS_mount, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_runlevel_1:Add(ui.StaticBoxSizer_FS_mount, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)


        -- create network start group
        ui.StaticBoxSizer_network = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_runlevel_1, "Network start")
        ui.FlexGridSizer_network = wx.wxFlexGridSizer(0, 1, 0, 0)

            -- add DHCP enable checkbox
            ui.CheckBox_RL1_network_DHCP = wx.wxCheckBox(ui.Panel_runlevel_1, wx.wxNewId(), "Start DHCP client to get addresses", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_network:Add(ui.CheckBox_RL1_network_DHCP, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 2)
            ui.CheckBox_RL1_network_DHCP:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)

            -- add Static configuration checkbox
            ui.CheckBox_RL1_network_static = wx.wxCheckBox(ui.Panel_runlevel_1, wx.wxNewId(), "Set static IP configuration (see Network configuration)", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_network:Add(ui.CheckBox_RL1_network_static, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 2)
            ui.CheckBox_RL1_network_static:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function(event) modified:yes() end)

            -- add connection summary checkbox
            ui.CheckBox_RL1_network_summary = wx.wxCheckBox(ui.Panel_runlevel_1, wx.wxNewId(), "Show connection summary", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_network:Add(ui.CheckBox_RL1_network_summary, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 2)
            ui.CheckBox_RL1_network_summary:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function(event) modified:yes() end)

            -- add info text
            ui.StaticText = wx.wxStaticText(ui.Panel_runlevel_1, wx.wxID_ANY, "NOTE: If static and dynamic network configurations are enabled at the same time, then dynamic "..
                                                                              "configuration will  start first. When DHCP client cannot receive addresses, then static "..
                                                                              "configuration will be applied automatically.")
            ui.StaticText:Wrap(ct.CONTROL_X_SIZE)
            ui.FlexGridSizer_network:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 2)

            -- add group to panel's sizer
            ui.StaticBoxSizer_network:Add(ui.FlexGridSizer_network, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_runlevel_1:Add(ui.StaticBoxSizer_network, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- set runlevel 1's panel
        ui.Panel_runlevel_1:SetSizer(ui.FlexGridSizer_runlevel_1)

        return ui.Panel_runlevel_1
end


--------------------------------------------------------------------------------
-- @brief  Create widgets for runlevel 2
-- @param  parent       parent window
-- @return Panel object
--------------------------------------------------------------------------------
local function create_runlevel_2_widgets(parent)
        -- create Runlevel 2 panel
        ui.Panel_runlevel_2 = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_runlevel_2 = wx.wxFlexGridSizer(0, 1, 0, 0)

        -- add runlevel description
        ui.StaticText = wx.wxStaticText(ui.Panel_runlevel_2, wx.wxID_ANY, "The purpose of this runlevel is starting user's applications.")
        ui.StaticText:Wrap(ct.CONTROL_X_SIZE)
        ui.FlexGridSizer_runlevel_2:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
        ui.StaticLine = wx.wxStaticLine(ui.Panel_runlevel_2, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxLI_HORIZONTAL)
        ui.FlexGridSizer_runlevel_2:Add(ui.StaticLine, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- create system messages group
        ui.StaticBoxSizer_RL2_sys_msg = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_runlevel_2, "System messages")
        ui.FlexGridSizer_RL2_sys_msg = wx.wxFlexGridSizer(0, 2, 0, 0)

            -- add system message enable checkbox
            ui.CheckBox_RL2_sys_msg_en = wx.wxCheckBox(ui.Panel_runlevel_2, wx.wxNewId(), "Enable in this runlevel by using file:", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_RL2_sys_msg:Add(ui.CheckBox_RL2_sys_msg_en, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.CheckBox_RL2_sys_msg_en:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED,
                    function(event)
                            ui.ComboBox_RL2_sys_msg_file:Enable(event:IsChecked())
                            modified:yes()
                    end
            )

            -- add filed to set printk()'s file
            ui.ComboBox_RL2_sys_msg_file = wx.wxComboBox(ui.Panel_runlevel_2, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(200, -1), {})
            ui.FlexGridSizer_RL2_sys_msg:Add(ui.ComboBox_RL2_sys_msg_file, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.ComboBox_RL2_sys_msg_file:Connect(wx.wxEVT_COMMAND_COMBOBOX_SELECTED, function() modified:yes() end)
            ui.ComboBox_RL2_sys_msg_file:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED,      function() modified:yes() end)

            -- add group to runlevel 2 panel
            ui.StaticBoxSizer_RL2_sys_msg:Add(ui.FlexGridSizer_RL2_sys_msg, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_runlevel_2:Add(ui.StaticBoxSizer_RL2_sys_msg, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- create applications start group
        ui.StaticBoxSizer_RL2_app_start = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_runlevel_2, "Applications start")
        ui.FlexGridSizer_RL2_app_start = wx.wxFlexGridSizer(0, 2, 0, 0)

            -- add list
            ui.ListView_RL2_app_start = wx.wxListView(ui.Panel_runlevel_2, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, 300), wx.wxLC_REPORT)
            ui.ListView_RL2_app_start.AppendItem          = wxListView_insert_item
            ui.ListView_RL2_app_start.RemoveSelectedItems = wxListView_remove_selected_items
            ui.ListView_RL2_app_start.UpdateItem          = wxListView_update_item
            ui.ListView_RL2_app_start.GetItemTexts        = wxListView_get_item_texts
            ui.ListView_RL2_app_start.MoveItem            = wxListView_move_item
            ui.ListView_RL2_app_start:InsertColumn(0, "Command", wx.wxLIST_FORMAT_LEFT, 150)
            ui.ListView_RL2_app_start:InsertColumn(1, "Working directory", wx.wxLIST_FORMAT_LEFT, 150)
            ui.ListView_RL2_app_start:InsertColumn(2, "stdin", wx.wxLIST_FORMAT_LEFT, 80)
            ui.ListView_RL2_app_start:InsertColumn(3, "stdout", wx.wxLIST_FORMAT_LEFT, 80)
            ui.ListView_RL2_app_start:InsertColumn(4, "stderr", wx.wxLIST_FORMAT_LEFT, 80)
            ui.FlexGridSizer_RL2_app_start:Add(ui.ListView_RL2_app_start, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)

            -- create sizer for buttons
            ui.FlexGridSizer_RL2_app_start_buttons = wx.wxFlexGridSizer(0, 1, 0, 0)

                -- function used to show dialog do enter parameters
                local function RL2_program_add_edit_dialog(edit)
                        local selected_item = ui.ListView_RL2_app_start:GetFirstSelected()
                        if selected_item > -1 or edit == false then
                                -- show window with parameters to fill
                                local dialog = new_data_dialog(ui.window, ifs(edit == true, "Entry modification", "Add program"), 2)
                                local ComboBox_program_name   = wx.wxComboBox(dialog:GetHandle(), wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(250,-1), app_list:get_list(), wx.wxTE_PROCESS_ENTER)
                                local ComboBox_program_CWD    = wx.wxComboBox(dialog:GetHandle(), wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, default_dirs, wx.wxTE_PROCESS_ENTER)
                                local ComboBox_program_stdin  = wx.wxComboBox(dialog:GetHandle(), wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, ui.ListView_RL0_drv_list:GetNodeList(), wx.wxTE_PROCESS_ENTER)
                                local ComboBox_program_stdout = wx.wxComboBox(dialog:GetHandle(), wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, ui.ListView_RL0_drv_list:GetNodeList(), wx.wxTE_PROCESS_ENTER)
                                local ComboBox_program_stderr = wx.wxComboBox(dialog:GetHandle(), wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, ui.ListView_RL0_drv_list:GetNodeList(), wx.wxTE_PROCESS_ENTER)
                                local ToolTip_as_daemon       = "Set stdin, stdout, and stderr to 'none' or left empty to start program as daemon."

                                if edit == true then
                                        local col = ui.ListView_RL2_app_start:GetItemTexts(selected_item, 5)
                                        ComboBox_program_name:SetValue(col[1])
                                        ComboBox_program_CWD:SetValue(col[2])
                                        ComboBox_program_stdin:SetValue(col[3])
                                        ComboBox_program_stdout:SetValue(col[4])
                                        ComboBox_program_stderr:SetValue(col[5])
                                end

                                local function save()
                                        local program = ComboBox_program_name:GetValue()
                                        local cwd     = ComboBox_program_CWD:GetValue()
                                        local stdin   = ComboBox_program_stdin:GetValue()
                                        local stdout  = ComboBox_program_stdout:GetValue()
                                        local stderr  = ComboBox_program_stderr:GetValue()

                                        if not cwd:match("^/.*") then cwd = "/"..cwd end
                                        if stdin  == "" then stdin  = "none" end
                                        if stdout == "" then stdout = "none" end
                                        if stderr == "" then stderr = "none" end

                                        if program ~= "" then
                                                if edit == true then
                                                        ui.ListView_RL2_app_start:UpdateItem(selected_item, {program, cwd, stdin, stdout, stderr})
                                                else
                                                        ui.ListView_RL2_app_start:AppendItem({program, cwd, stdin, stdout, stderr})
                                                end

                                                modified:yes()
                                                dialog:Close()
                                        end
                                end

                                ComboBox_program_name:SetFocus()
                                ComboBox_program_name:Connect(wx.wxEVT_COMMAND_TEXT_ENTER, save)
                                ComboBox_program_CWD:Connect(wx.wxEVT_COMMAND_TEXT_ENTER, save)
                                ComboBox_program_stdin:Connect(wx.wxEVT_COMMAND_TEXT_ENTER, save)
                                ComboBox_program_stdout:Connect(wx.wxEVT_COMMAND_TEXT_ENTER, save)
                                ComboBox_program_stderr:Connect(wx.wxEVT_COMMAND_TEXT_ENTER, save)

                                ComboBox_program_name:SetToolTip("Type program name and parameters.")
                                ComboBox_program_stdin:SetToolTip(ToolTip_as_daemon)
                                ComboBox_program_stdout:SetToolTip(ToolTip_as_daemon)
                                ComboBox_program_stderr:SetToolTip(ToolTip_as_daemon)

                                dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "Program name"))
                                dialog:Add(ComboBox_program_name)
                                dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "Working directory"))
                                dialog:Add(ComboBox_program_CWD)
                                dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "stdin stream"))
                                dialog:Add(ComboBox_program_stdin)
                                dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "stdout stream"))
                                dialog:Add(ComboBox_program_stdout)
                                dialog:Add(wx.wxStaticText(dialog:GetHandle(), wx.wxID_ANY, "stderr stream"))
                                dialog:Add(ComboBox_program_stderr)

                                dialog:SetSaveFunction(save)
                                dialog:ShowModal()
                        end
                end

                -- add Add button
                ui.Button_RL2_app_start_add = wx.wxBitmapButton(ui.Panel_runlevel_2, wx.wxNewId(), ct.icon.list_add_16x16)
                ui.Button_RL2_app_start_add:SetToolTip("Add")
                ui.FlexGridSizer_RL2_app_start_buttons:Add(ui.Button_RL2_app_start_add, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
                ui.Button_RL2_app_start_add:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED, function() RL2_program_add_edit_dialog(false) end)

                -- add Remove button
                ui.Button_RL2_app_start_remove = wx.wxBitmapButton(ui.Panel_runlevel_2, wx.wxNewId(), ct.icon.edit_delete_16x16)
                ui.Button_RL2_app_start_remove:SetToolTip("Remove")
                ui.FlexGridSizer_RL2_app_start_buttons:Add(ui.Button_RL2_app_start_remove, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
                ui.Button_RL2_app_start_remove:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED, function() if ui.ListView_RL2_app_start:RemoveSelectedItems() == true then modified:yes() end end)

                -- add: edit button
                ui.Button_RL2_app_start_edit = wx.wxBitmapButton(ui.Panel_runlevel_2, wx.wxNewId(), ct.icon.document_edit_16x16)
                ui.Button_RL2_app_start_edit:SetToolTip("Edit")
                ui.FlexGridSizer_RL2_app_start_buttons:Add(ui.Button_RL2_app_start_edit, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
                ui.Button_RL2_app_start_edit:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED, function() RL2_program_add_edit_dialog(true) end)

                -- add: arrow up button
                ui.Button_RL2_app_start_up = wx.wxBitmapButton(ui.Panel_runlevel_2, wx.wxNewId(), ct.icon.arrow_up_16x16)
                ui.Button_RL2_app_start_up:SetToolTip("Move up")
                ui.FlexGridSizer_RL2_app_start_buttons:Add(ui.Button_RL2_app_start_up, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
                ui.Button_RL2_app_start_up:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED, function() ui.ListView_RL2_app_start:MoveItem(-1, 2) end)

                -- add: arrow down button
                ui.Button_RL2_app_start_down = wx.wxBitmapButton(ui.Panel_runlevel_2, wx.wxNewId(), ct.icon.arrow_down_16x16)
                ui.Button_RL2_app_start_down:SetToolTip("Move down")
                ui.FlexGridSizer_RL2_app_start_buttons:Add(ui.Button_RL2_app_start_down, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 2)
                ui.Button_RL2_app_start_down:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED, function() ui.ListView_RL2_app_start:MoveItem(1, 2) end)

                -- add fields to group
                ui.FlexGridSizer_RL2_app_start:Add(ui.FlexGridSizer_RL2_app_start_buttons, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_TOP), 0)

            -- add entire group to the panel
            ui.StaticBoxSizer_RL2_app_start:Add(ui.FlexGridSizer_RL2_app_start, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_runlevel_2:Add(ui.StaticBoxSizer_RL2_app_start, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- set panel's sizer
        ui.Panel_runlevel_2:SetSizer(ui.FlexGridSizer_runlevel_2)

        return ui.Panel_runlevel_2
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
                ui.Notebook_runlevels:AddPage(create_runlevel_boot_widgets(ui.Notebook_runlevels), "Runlevel boot", false)
                ui.Notebook_runlevels:AddPage(create_runlevel_0_widgets(ui.Notebook_runlevels), "Runlevel 0", false)
                ui.Notebook_runlevels:AddPage(create_runlevel_1_widgets(ui.Notebook_runlevels), "Runlevel 1", false)
                ui.Notebook_runlevels:AddPage(create_runlevel_2_widgets(ui.Notebook_runlevels), "Runlevel 2", false)
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
        ui.window:Hide()

        app_list:reload()
        FS_list:reload()

        ui.Choice_RLB_root_FS:Clear()
        ui.Choice_RLB_root_FS:Append(FS_list:get_list())

        load_configuration()

        ui.window:Show()
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

--------------------------------------------------------------------------------
-- @brief  Function generate initd code
-- @return None
--------------------------------------------------------------------------------
function startup:generate()
        local cfg = ct:load_table(INITD_CFG_FILE)
        generate_init_code(cfg)
end
