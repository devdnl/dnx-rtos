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


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local ui = {}
local ID = {}
local modified       = ct:new_modify_indicator()
local default_dirs   = {"/dev", "/home", "/mnt", "/proc", "/srv", "/tmp", "/usr"}
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
        local INITD_TEMPLATE_STACK_INFO        = config.project.path.initd_template_stack_info_file:GetValue()
        local INITD_TEMPLATE_APP_PREPARE       = config.project.path.initd_template_app_prepare_file:GetValue()
        local INITD_TEMPLATE_APP_START         = config.project.path.initd_template_app_start_file:GetValue()
        local INITD_TEMPLATE_APP_FINISH        = config.project.path.initd_template_app_finish_file:GetValue()
        local INITD_TEMPLATE_APP_STREAM_OPEN   = config.project.path.initd_template_app_stream_open_file:GetValue()
        local INITD_SRC_FILE                   = config.project.path.initd_src_file:GetValue()

        local answer = ct:show_question_msg(ct.MAIN_WINDOW_NAME,
                                            "Do you want to generate initd code based on current configuration?\n\n"..
                                            "If Yes will be selected then current initd code will be overwritten (custom initd code will be lost), otherwise no changes are done.",
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
                                n = n + ct:apply_template(INITD_TEMPLATE_STACK_INFO, INITD_SRC_FILE, {}, n)
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
        local initd = ct:load_table(INITD_CFG_FILE)

        -------------------
        -- RUNLEVEL BOOT --
        -------------------
        -- load base file system
        ui.Choice_RLB_root_FS:SetSelection(FS_list:get_index_of(initd.runlevel_boot.base_FS) - 1)

        -- load folders to create
        ui.ListBox_RLB_folders:Clear()
        ui.ComboBox_RLB_other_FS_mntpt:Clear()
        ui.ComboBox_RL1_FS_mount_mntpt:Clear()
        ui.ComboBox_RL2_app_start_CWD:Clear()
        for i = 1, #initd.runlevel_boot.folders do
                local dirname = initd.runlevel_boot.folders[i]
                ui.ListBox_RLB_folders:Append(dirname)
                ui.ComboBox_RLB_other_FS_mntpt:Append(dirname)
                ui.ComboBox_RL1_FS_mount_mntpt:Append(dirname)
                ui.ComboBox_RL2_app_start_CWD:Append(dirname)
        end

        -- load additional file systems to mount
        ui.ListView_RLB_other_FS:DeleteAllItems()
        for i = 1, #initd.runlevel_boot.additional_FS do
                local item = initd.runlevel_boot.additional_FS[i]
                ui.ListView_RLB_other_FS:AppendItem(item.file_system, item.source_file, item.mount_point)
        end

        ----------------
        -- RUNLEVEL 0 --
        ----------------
        -- load list of drivers to initialize
        ui.ListView_RL0_drv_list:DeleteAllItems()
        ui.ComboBox_RL0_sys_msg_file:Clear()
        ui.ComboBox_RL1_storage_file:Clear()
        ui.ComboBox_RL1_FS_mount_src:Clear()
        ui.ComboBox_RL2_sys_msg_file:Clear()
        ui.ComboBox_RL2_app_start_stdin:Clear()
        ui.ComboBox_RL2_app_start_stdout:Clear()
        ui.ComboBox_RL2_app_start_stderr:Clear()

        for i = 1, #initd.runlevel_0.driver_init do
                local item = initd.runlevel_0.driver_init[i]
                ui.ListView_RL0_drv_list:AppendItem(item.name, item.node)
                ui.ComboBox_RL0_sys_msg_file:Append(item.node)
                ui.ComboBox_RL1_storage_file:Append(item.node)
                ui.ComboBox_RL1_FS_mount_src:Append(item.node)
                ui.ComboBox_RL2_sys_msg_file:Append(item.node)
                ui.ComboBox_RL2_app_start_stdin:Append(item.node)
                ui.ComboBox_RL2_app_start_stdout:Append(item.node)
                ui.ComboBox_RL2_app_start_stderr:Append(item.node)
        end

        -- load system messages configuration
        ui.CheckBox_RL0_sys_msg_en:SetValue(initd.runlevel_0.system_messages.show)
        ui.Panel_RL0_sys_msg_sub:Enable(initd.runlevel_0.system_messages.show)
        ui.CheckBox_RL0_sys_msg_invitation:SetValue(initd.runlevel_0.system_messages.invitation)
        ui.ComboBox_RL0_sys_msg_file:SetValue(initd.runlevel_0.system_messages.file)
        ui.Choice_RL0_sys_msg_init_after:SetSelection(drv_list:get_index_of(initd.runlevel_0.system_messages.init_after) - 1)

        ----------------
        -- RUNLEVEL 1 --
        ----------------
        -- load daemon list
        ui.ListView_RL1_daemons:DeleteAllItems()
        for i = 1, #initd.runlevel_1.daemons do
                local item = initd.runlevel_1.daemons[i]
                ui.ListView_RL1_daemons:AppendItem(item.name, item.CWD)
        end

        -- storage initialization
        ui.ListBox_RL1_storage:Clear()
        for i = 1, #initd.runlevel_1.storage_init do
                ui.ListBox_RL1_storage:Append(initd.runlevel_1.storage_init[i])
        end

        -- mount table
        ui.ListView_RL1_FS_mount:DeleteAllItems()
        for i = 1, #initd.runlevel_1.mount_table do
                local item = initd.runlevel_1.mount_table[i]
                ui.ListView_RL1_FS_mount:AppendItem(item.file_system, item.source_file, item.mount_point)
        end

        -- network start
        ui.CheckBox_RL1_network_DHCP:SetValue(initd.runlevel_1.network.DHCP)
        ui.CheckBox_RL1_network_static:SetValue(initd.runlevel_1.network.static)
        ui.CheckBox_RL1_network_summary:SetValue(initd.runlevel_1.network.summary)

        ----------------
        -- RUNLEVEL 2 --
        ----------------
        -- system messages
        ui.CheckBox_RL2_sys_msg_en:SetValue(initd.runlevel_2.system_messages.show)
        ui.ComboBox_RL2_sys_msg_file:SetValue(initd.runlevel_2.system_messages.file)

        -- load applications list
        ui.ListView_RL2_app_start:DeleteAllItems()
        for i = 1, #initd.runlevel_2.applications do
                local item = initd.runlevel_2.applications[i]
                ui.ListView_RL2_app_start:AppendItem(item.name, item.CWD, item.stdin, item.stdout, item.stderr)
        end

        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function save_configuration()
        local initd = {}

        -- runlevel boot
        initd.runlevel_boot = {}

                -- root FS
                local sel = ui.Choice_RLB_root_FS:GetSelection()
                initd.runlevel_boot.base_FS = ui.Choice_RLB_root_FS:GetString(ifs(sel > -1, sel, 0))

                -- new folder list
                initd.runlevel_boot.folders = {}
                for i = 0, ui.ListBox_RLB_folders:GetCount() - 1 do
                        table.insert(initd.runlevel_boot.folders, ui.ListBox_RLB_folders:GetString(i))
                end

                -- additional file system mount
                initd.runlevel_boot.additional_FS = {}
                for i = 0, ui.ListView_RLB_other_FS:GetItemCount() - 1 do
                        local cols = ui.ListView_RLB_other_FS:GetItemTexts(i, 3)

                        local item = {}
                        item.file_system = cols[0]
                        item.source_file = cols[1]
                        item.mount_point = cols[2]

                        table.insert(initd.runlevel_boot.additional_FS, item)
                end


        -- runlevel 0
        initd.runlevel_0 = {}

                -- list of drivers to initialize
                initd.runlevel_0.driver_init = {}
                for i = 0, ui.ListView_RL0_drv_list:GetItemCount() - 1 do
                        local cols = ui.ListView_RL0_drv_list:GetItemTexts(i, 2)

                        local item = {}
                        item.name = cols[0]
                        item.node = cols[1]

                        table.insert(initd.runlevel_0.driver_init, item)
                end

                -- system messages
                initd.runlevel_0.system_messages = {}
                initd.runlevel_0.system_messages.show = ui.CheckBox_RL0_sys_msg_en:GetValue()
                initd.runlevel_0.system_messages.invitation = ui.CheckBox_RL0_sys_msg_invitation:GetValue()
                initd.runlevel_0.system_messages.file = ui.ComboBox_RL0_sys_msg_file:GetValue()
                local sel = ui.Choice_RL0_sys_msg_init_after:GetSelection()
                initd.runlevel_0.system_messages.init_after = ui.Choice_RL0_sys_msg_init_after:GetString(ifs(sel > -1, sel, 0))


        -- runlevel 1
        initd.runlevel_1 = {}

                -- early started daemons
                initd.runlevel_1.daemons = {}
                for i = 0, ui.ListView_RL1_daemons:GetItemCount() - 1 do
                        local cols = ui.ListView_RL1_daemons:GetItemTexts(i, 2)

                        local item = {}
                        item.name = cols[0]
                        item.CWD  = cols[1]

                        table.insert(initd.runlevel_1.daemons, item)
                end

                -- storage initialization
                initd.runlevel_1.storage_init = {}
                for i = 0, ui.ListBox_RL1_storage:GetCount() - 1 do
                        table.insert(initd.runlevel_1.storage_init, ui.ListBox_RL1_storage:GetString(i))
                end

                -- mount table
                initd.runlevel_1.mount_table = {}
                for i = 0, ui.ListView_RL1_FS_mount:GetItemCount() - 1 do
                        local cols = ui.ListView_RL1_FS_mount:GetItemTexts(i, 3)

                        local item = {}
                        item.file_system = cols[0]
                        item.source_file = cols[1]
                        item.mount_point = cols[2]

                        table.insert(initd.runlevel_1.mount_table, item)
                end

                -- network configuration
                initd.runlevel_1.network = {}
                initd.runlevel_1.network.DHCP    = ui.CheckBox_RL1_network_DHCP:GetValue()
                initd.runlevel_1.network.static  = ui.CheckBox_RL1_network_static:GetValue()
                initd.runlevel_1.network.summary = ui.CheckBox_RL1_network_summary:GetValue()


        -- runlevel 2
        initd.runlevel_2 = {}

                -- system messages
                initd.runlevel_2.system_messages = {}
                initd.runlevel_2.system_messages.show = ui.CheckBox_RL2_sys_msg_en:GetValue()
                initd.runlevel_2.system_messages.file = ui.ComboBox_RL2_sys_msg_file:GetValue()

                -- applications start
                initd.runlevel_2.applications = {}
                for i = 0, ui.ListView_RL2_app_start:GetItemCount() - 1 do
                        local cols = ui.ListView_RL2_app_start:GetItemTexts(i, 5)

                        local item = {}
                        item.name   = cols[0]
                        item.CWD    = cols[1]
                        item.stdin  = cols[2]
                        item.stdout = cols[3]
                        item.stderr = cols[4]

                        table.insert(initd.runlevel_2.applications, item)
                end


        -- save configuration to file
        ct:save_table(initd, INITD_CFG_FILE)

        generate_init_code(initd)

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

            ui.StaticText = wx.wxStaticText(ui.Panel_boot, wx.wxID_ANY, "Select base file system:", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_boot_main_FS_1:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            ui.Choice_RLB_root_FS = wx.wxChoice(ui.Panel_boot, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(100, -1), FS_list:get_list())
            ui.FlexGridSizer_boot_main_FS_1:Add(ui.Choice_RLB_root_FS, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.Choice_RLB_root_FS:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)

            ui.StaticBoxSizer_boot_main_FS_0:Add(ui.FlexGridSizer_boot_main_FS_1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_boot:Add(ui.StaticBoxSizer_boot_main_FS_0, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- folders group
        ui.StaticBoxSizer_boot_folders_0 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_boot, "Create folders")
        ui.FlexGridSizer_boot_folders_1 = wx.wxFlexGridSizer(0, 1, 0, 0)

            -- new folder sizer
            ui.FlexGridSizer_boot_folders_2 = wx.wxFlexGridSizer(0, 5, 0, 0)

                -- folder name combobox
                ui.ComboBox_RLB_folder_name = wx.wxComboBox(ui.Panel_boot, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(100, -1), default_dirs, wx.wxTE_PROCESS_ENTER)
                ui.FlexGridSizer_boot_folders_2:Add(ui.ComboBox_RLB_folder_name, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.ComboBox_RLB_folder_name:Connect(wx.wxEVT_COMMAND_TEXT_ENTER, function() ui.Button_RLB_folder_add:Command(wx.wxCommandEvent(wx.wxEVT_COMMAND_BUTTON_CLICKED)) end)

                -- add button
                ui.Button_RLB_folder_add = wx.wxButton(ui.Panel_boot, wx.wxNewId(), "Add", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_boot_folders_2:Add(ui.Button_RLB_folder_add, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Button_RLB_folder_add:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                        function()
                                local dirname = ui.ComboBox_RLB_folder_name:GetValue()
                                if dirname ~= "" then
                                        if not dirname:match("^/.*") then dirname = "/"..dirname end

                                        ui.ListBox_RLB_folders:InsertItems({dirname}, ui.ListBox_RLB_folders:GetCount())
                                        ui.ComboBox_RLB_other_FS_mntpt:Append(dirname)
                                        ui.ComboBox_RL1_FS_mount_mntpt:Append(dirname)
                                        ui.ComboBox_RL2_app_start_CWD:Append(dirname)
                                        ui.ComboBox_RLB_folder_name:SetValue("")
                                        modified:yes()
                                end
                        end
                )

                -- seperator
                ui.StaticLine = wx.wxStaticLine(ui.Panel_boot, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxLI_VERTICAL)
                ui.FlexGridSizer_boot_folders_2:Add(ui.StaticLine, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- remove button
                ui.Button_RLB_folder_remove = wx.wxButton(ui.Panel_boot, wx.wxID_ANY, "Remove selected", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_boot_folders_2:Add(ui.Button_RLB_folder_remove, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Button_RLB_folder_remove:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                        function()
                                local sel = ui.ListBox_RLB_folders:GetSelection()
                                local t   = {}
                                for i = 0, ui.ListBox_RLB_folders:GetCount() do
                                        if i ~= sel then
                                                local str = ui.ListBox_RLB_folders:GetString(i)
                                                if str ~= "" then
                                                        table.insert(t, str)
                                                end
                                        end
                                end

                                ui.ListBox_RLB_folders:Clear()
                                ui.ListBox_RLB_folders:InsertItems(t, 0)
                                ui.ComboBox_RLB_other_FS_mntpt:Clear()
                                ui.ComboBox_RLB_other_FS_mntpt:Append(t)
                                ui.ComboBox_RL2_app_start_CWD:Clear()
                                ui.ComboBox_RL2_app_start_CWD:Append(t)
                                modified:yes()
                        end
                )

                -- add new folder sizer to folder group
                ui.FlexGridSizer_boot_folders_1:Add(ui.FlexGridSizer_boot_folders_2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

            -- add folder list
            ui.ListBox_RLB_folders = wx.wxListBox(ui.Panel_boot, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, 110), {}, 0)
            ui.FlexGridSizer_boot_folders_1:Add(ui.ListBox_RLB_folders, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

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
            ui.Choice_RLB_other_FS_name = wx.wxChoice(ui.Panel_boot, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(125, -1), FS_list:get_list())
            ui.FlexGridSizer_other_FS_2:Add(ui.Choice_RLB_other_FS_name, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- source file combobox
            ui.ComboBox_RLB_other_FS_src = wx.wxComboBox(ui.Panel_boot, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(125, -1), {"", "none"})
            ui.FlexGridSizer_other_FS_2:Add(ui.ComboBox_RLB_other_FS_src, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- FS mount point
            ui.ComboBox_RLB_other_FS_mntpt = wx.wxComboBox(ui.Panel_boot, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(125, -1), {})
            ui.FlexGridSizer_other_FS_2:Add(ui.ComboBox_RLB_other_FS_mntpt, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- add buttons
            ui.Button_other_FS_add = wx.wxButton(ui.Panel_boot, wx.wxNewId(), "Add", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_other_FS_2:Add(ui.Button_other_FS_add, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.Button_other_FS_add:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                    function()
                            local sel      = ui.Choice_RLB_other_FS_name:GetSelection()
                            local fs_name  = ui.Choice_RLB_other_FS_name:GetString(ifs(sel > -1, sel, 0))
                            local src_file = ui.ComboBox_RLB_other_FS_src:GetValue()
                            local mntpt    = ui.ComboBox_RLB_other_FS_mntpt:GetValue()

                            if fs_name ~= "" and src_file ~= "" and mntpt:match("^/.*") then
                                    ui.ListView_RLB_other_FS:AppendItem(fs_name, src_file, mntpt)
                                    ui.Choice_RLB_other_FS_name:SetSelection(0)
                                    ui.ComboBox_RLB_other_FS_src:SetValue("")
                                    ui.ComboBox_RLB_other_FS_mntpt:SetValue("")
                                    modified:yes()
                            end
                    end
            )

            -- add sizers
            ui.FlexGridSizer_other_FS_1:Add(ui.FlexGridSizer_other_FS_2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

            -- FS list
            ui.ListView_RLB_other_FS = wx.wxListView(ui.Panel_boot, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, 150), wx.wxLC_REPORT)
            ui.ListView_RLB_other_FS.AppendItem   = insert_item
            ui.ListView_RLB_other_FS.GetItemTexts = get_item_texts
            ui.ListView_RLB_other_FS:InsertColumn(0, "File system", wx.wxLIST_FORMAT_LEFT, 150)
            ui.ListView_RLB_other_FS:InsertColumn(1, "Source file", wx.wxLIST_FORMAT_LEFT, 150)
            ui.ListView_RLB_other_FS:InsertColumn(2, "Mount point", wx.wxLIST_FORMAT_LEFT, 150)
            ui.FlexGridSizer_other_FS_1:Add(ui.ListView_RLB_other_FS, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- add remove button
            ui.Button_RLB_other_FS_remove = wx.wxButton(ui.Panel_boot, wx.wxNewId(), "Remove selected", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_other_FS_1:Add(ui.Button_RLB_other_FS_remove, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
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
                ui.Choice_RL0_drv_name = wx.wxChoice(ui.Panel_runlevel_0, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(150, -1), drv_list:get_list())
                ui.FlexGridSizer_drv_init_sel:Add(ui.Choice_RL0_drv_name, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_RL0_drv_name:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED,
                        function()
                                local str = ui.Choice_RL0_drv_name:GetString(ui.Choice_RL0_drv_name:GetSelection())
                                if str ~= "" then
                                        ui.ComboBox_RL0_drv_node:SetValue("/dev/"..str)
                                else
                                        ui.ComboBox_RL0_drv_node:SetValue("")
                                end
                        end
                )

                -- add driver node path
                ui.ComboBox_RL0_drv_node = wx.wxComboBox(ui.Panel_runlevel_0, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(150, -1), {"none"})
                ui.FlexGridSizer_drv_init_sel:Add(ui.ComboBox_RL0_drv_node, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add Add button
                ui.Button_RL0_drv_add = wx.wxButton(ui.Panel_runlevel_0, wx.wxNewId(), "Add", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_drv_init_sel:Add(ui.Button_RL0_drv_add, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Button_RL0_drv_add:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                        function()
                                local drv_name  = ui.Choice_RL0_drv_name:GetString(ui.Choice_RL0_drv_name:GetSelection())
                                local node_path = ui.ComboBox_RL0_drv_node:GetValue()

                                if drv_name ~= "" and (node_path:match("^/.*") or node_path == "none") then
                                        ui.ListView_RL0_drv_list:AppendItem(drv_name, node_path)
                                        ui.Choice_RL0_drv_name:SetSelection(0)
                                        ui.ComboBox_RL0_drv_node:SetValue("")

                                        if node_path ~= "none" then
                                                ui.ComboBox_RL0_sys_msg_file:Append(node_path)
                                                ui.ComboBox_RL1_storage_file:Append(node_path)
                                                ui.ComboBox_RL2_app_start_stdin:Append(node_path)
                                                ui.ComboBox_RL2_app_start_stdout:Append(node_path)
                                                ui.ComboBox_RL2_app_start_stderr:Append(node_path)
                                                ui.ComboBox_RL2_sys_msg_file:Append(node_path)
                                        end
                                        modified:yes()
                                end
                        end
                )

                -- add driver selection, driver node path, and add button to the group
                ui.FlexGridSizer_drv_init:Add(ui.FlexGridSizer_drv_init_sel, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 0)

            -- add list box
            ui.ListView_RL0_drv_list = wx.wxListView(ui.Panel_runlevel_0, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, 300), wx.wxLC_REPORT)
            ui.ListView_RL0_drv_list.AppendItem   = insert_item
            ui.ListView_RL0_drv_list.GetItemTexts = get_item_texts
            ui.ListView_RL0_drv_list:InsertColumn(0, "Driver name", wx.wxLIST_FORMAT_LEFT, 200)
            ui.ListView_RL0_drv_list:InsertColumn(1, "Node path", wx.wxLIST_FORMAT_LEFT, 300)
            ui.FlexGridSizer_drv_init:Add(ui.ListView_RL0_drv_list, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- add remove button
            ui.Button_RL0_drv_init_remove = wx.wxButton(ui.Panel_runlevel_0, wx.wxNewId(), "Remove selected", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_drv_init:Add(ui.Button_RL0_drv_init_remove, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
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

                            if updated then
                                    local t = {}
                                    for i = 0, ui.ListView_RL0_drv_list:GetItemCount() - 1 do
                                            local col = ui.ListView_RL0_drv_list:GetItemTexts(i, 2)

                                            if col[1] ~= "none" then
                                                    table.insert(t, col[1])
                                            end
                                    end

                                    ui.ComboBox_RL0_sys_msg_file:Clear()
                                    ui.ComboBox_RL0_sys_msg_file:Append(t)
                                    ui.ComboBox_RL1_storage_file:Clear()
                                    ui.ComboBox_RL1_storage_file:Append(t)
                                    ui.ComboBox_RL2_app_start_stdin:Clear()
                                    ui.ComboBox_RL2_app_start_stdin:Append(t)
                                    ui.ComboBox_RL2_app_start_stdout:Clear()
                                    ui.ComboBox_RL2_app_start_stdout:Append(t)
                                    ui.ComboBox_RL2_app_start_stderr:Clear()
                                    ui.ComboBox_RL2_app_start_stderr:Append(t)
                                    ui.ComboBox_RL2_sys_msg_file:Clear()
                                    ui.ComboBox_RL2_sys_msg_file:Append(t)
                            end
                    end
            )

            -- add driver init group to the panel's sizer
            ui.StaticBoxSizer_drv_init:Add(ui.FlexGridSizer_drv_init, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_runlevel_0:Add(ui.StaticBoxSizer_drv_init, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

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
        ui.FlexGridSizer_daemons = wx.wxFlexGridSizer(0, 1, 0, 0)

            -- create sizer for buttons
            ui.FlexGridSizer_daemons_buttons = wx.wxFlexGridSizer(0, 5, 0, 0)

                -- add combobox with daemons names (and parameters)
                ui.StaticText = wx.wxStaticText(ui.Panel_runlevel_1, wx.wxID_ANY, "Name:")
                ui.FlexGridSizer_daemons_buttons:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.ComboBox_RL1_daemons_name = wx.wxComboBox(ui.Panel_runlevel_1, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(150,-1), {}, wx.wxTE_PROCESS_ENTER)
                ui.ComboBox_RL1_daemons_name:Connect(wx.wxEVT_COMMAND_TEXT_ENTER, function() ui.Button_RL1_daemons_add:Command(wx.wxCommandEvent(wx.wxEVT_COMMAND_BUTTON_CLICKED)) end)
                ui.FlexGridSizer_daemons_buttons:Add(ui.ComboBox_RL1_daemons_name, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add CWD path
                ui.StaticText = wx.wxStaticText(ui.Panel_runlevel_1, wx.wxID_ANY, "  CWD:")
                ui.FlexGridSizer_daemons_buttons:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.ComboBox_RL1_daemons_CWD = wx.wxComboBox(ui.Panel_runlevel_1, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(150,-1), default_dirs, wx.wxTE_PROCESS_ENTER)
                ui.ComboBox_RL1_daemons_CWD:Connect(wx.wxEVT_COMMAND_TEXT_ENTER, function() ui.Button_RL1_daemons_add:Command(wx.wxCommandEvent(wx.wxEVT_COMMAND_BUTTON_CLICKED)) end)
                ui.FlexGridSizer_daemons_buttons:Add(ui.ComboBox_RL1_daemons_CWD, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add Add button
                ui.Button_RL1_daemons_add = wx.wxButton(ui.Panel_runlevel_1, wx.wxNewId(), "Add")
                ui.FlexGridSizer_daemons_buttons:Add(ui.Button_RL1_daemons_add, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Button_RL1_daemons_add:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                        function()
                                local program = ui.ComboBox_RL1_daemons_name:GetValue()
                                local cwd     = ui.ComboBox_RL1_daemons_CWD:GetValue()

                                if not cwd:match("^/.*") then cwd = "/"..cwd end

                                if program ~= "" then
                                        ui.ListView_RL1_daemons:AppendItem(program, cwd)
                                        ui.ComboBox_RL1_daemons_name:SetValue("")
                                        ui.ComboBox_RL1_daemons_CWD:SetValue("")
                                        modified:yes()
                                end
                        end
                )

                -- add buttons to group
                ui.FlexGridSizer_daemons:Add(ui.FlexGridSizer_daemons_buttons, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 0)

            -- add daemons list
            ui.ListView_RL1_daemons = wx.wxListView(ui.Panel_runlevel_1, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, 125), wx.wxLC_REPORT)
            ui.ListView_RL1_daemons.AppendItem   = insert_item
            ui.ListView_RL1_daemons.GetItemTexts = get_item_texts
            ui.ListView_RL1_daemons:InsertColumn(0, "Daemon", wx.wxLIST_FORMAT_LEFT, 250)
            ui.ListView_RL1_daemons:InsertColumn(1, "Working directory", wx.wxLIST_FORMAT_LEFT, 250)
            ui.FlexGridSizer_daemons:Add(ui.ListView_RL1_daemons, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- add remove button
            ui.Button_RL1_daemons_remove = wx.wxButton(ui.Panel_runlevel_1, wx.wxNewId(), "Remove", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_daemons:Add(ui.Button_RL1_daemons_remove, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
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

            -- add group to runlevel 1 panel
            ui.StaticBoxSizer_daemons:Add(ui.FlexGridSizer_daemons, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_runlevel_1:Add(ui.StaticBoxSizer_daemons, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- create storage initialization group
        ui.StaticBoxSizer_storage_init = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_runlevel_1, "Storage initialization")
        ui.FlexGridSizer_storage_init = wx.wxFlexGridSizer(0, 1, 0, 0)

            -- create button sizer
            ui.FlexGridSizer_storage_init_buttons = wx.wxFlexGridSizer(0, 4, 0, 0)

                -- add combobox with storage file path
                ui.FlexGridSizer_storage_init_buttons:Add(wx.wxStaticText(ui.Panel_runlevel_1, wx.wxID_ANY, "Storage path"), 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.ComboBox_RL1_storage_file = wx.wxComboBox(ui.Panel_runlevel_1, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(125,-1), {})
                ui.FlexGridSizer_storage_init_buttons:Add(ui.ComboBox_RL1_storage_file, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add Add button
                ui.Button_RL1_storage_add = wx.wxButton(ui.Panel_runlevel_1, wx.wxNewId(), "Add", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_storage_init_buttons:Add(ui.Button_RL1_storage_add, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Button_RL1_storage_add:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                        function()
                                local src_file = ui.ComboBox_RL1_storage_file:GetValue()
                                if src_file ~= "" then
                                        ui.ListBox_RL1_storage:Append(src_file)
                                        ui.ComboBox_RL1_storage_file:SetValue("")
                                        modified:yes()
                                end
                        end
                )

                -- add remove button
                ui.Button_RL1_storage_remove = wx.wxButton(ui.Panel_runlevel_1, wx.wxNewId(), "Remove", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_storage_init_buttons:Add(ui.Button_RL1_storage_remove, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Button_RL1_storage_remove:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                        function()
                                local sel = ui.ListBox_RL1_storage:GetSelection()
                                local t   = {}
                                for i = 0, ui.ListBox_RL1_storage:GetCount() do
                                        if i ~= sel then
                                                local str = ui.ListBox_RL1_storage:GetString(i)
                                                if str ~= "" then
                                                        table.insert(t, str)
                                                end
                                        end
                                end

                                ui.ListBox_RL1_storage:Clear()
                                ui.ListBox_RL1_storage:Append(t)
                                modified:yes()
                        end
                )

                -- add button sizer to the storage initialization group
                ui.FlexGridSizer_storage_init:Add(ui.FlexGridSizer_storage_init_buttons, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 0)

            -- create list with added cards
            ui.ListBox_RL1_storage  = wx.wxListBox(ui.Panel_runlevel_1, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, 110), {}, 0)
            ui.FlexGridSizer_storage_init:Add(ui.ListBox_RL1_storage, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- add group to main panel
            ui.StaticBoxSizer_storage_init:Add(ui.FlexGridSizer_storage_init, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_runlevel_1:Add(ui.StaticBoxSizer_storage_init, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)


        -- mount list
        ui.StaticBoxSizer_FS_mount = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_runlevel_1, "Mount table")
        ui.FlexGridSizer_FS_mount_1 = wx.wxFlexGridSizer(0, 1, 0, 0)
        ui.FlexGridSizer_FS_mount_2 = wx.wxFlexGridSizer(0, 4, 0, 0)

            -- colums descriptions
            ui.FlexGridSizer_FS_mount_2:Add(wx.wxStaticText(ui.Panel_runlevel_1, wx.wxID_ANY, "File system"), 1, (wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_FS_mount_2:Add(wx.wxStaticText(ui.Panel_runlevel_1, wx.wxID_ANY, "Source file"), 1, (wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_FS_mount_2:Add(wx.wxStaticText(ui.Panel_runlevel_1, wx.wxID_ANY, "Mount point"), 1, (wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_FS_mount_2:Add(0, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- file system name selection
            ui.Choice_RL1_FS_mount_name = wx.wxChoice(ui.Panel_runlevel_1, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(125, -1), FS_list:get_list())
            ui.FlexGridSizer_FS_mount_2:Add(ui.Choice_RL1_FS_mount_name, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- source file combobox
            ui.ComboBox_RL1_FS_mount_src = wx.wxComboBox(ui.Panel_runlevel_1, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(125, -1), {"", "none"})
            ui.FlexGridSizer_FS_mount_2:Add(ui.ComboBox_RL1_FS_mount_src, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- FS mount point
            ui.ComboBox_RL1_FS_mount_mntpt = wx.wxComboBox(ui.Panel_runlevel_1, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(125, -1), {})
            ui.FlexGridSizer_FS_mount_2:Add(ui.ComboBox_RL1_FS_mount_mntpt, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- add buttons
            ui.Button_RL1_FS_mount_add = wx.wxButton(ui.Panel_runlevel_1, wx.wxNewId(), "Add", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_FS_mount_2:Add(ui.Button_RL1_FS_mount_add, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.Button_RL1_FS_mount_add:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                    function()
                            local sel      = ui.Choice_RL1_FS_mount_name:GetSelection()
                            local fs_name  = ui.Choice_RL1_FS_mount_name:GetString(ifs(sel > -1, sel, 0))
                            local src_file = ui.ComboBox_RL1_FS_mount_src:GetValue()
                            local mntpt    = ui.ComboBox_RL1_FS_mount_mntpt:GetValue()

                            if fs_name ~= "" and src_file ~= "" and mntpt:match("^/.*") then
                                    ui.ListView_RL1_FS_mount:AppendItem(fs_name, src_file, mntpt)
                                    ui.Choice_RL1_FS_mount_name:SetSelection(0)
                                    ui.ComboBox_RL1_FS_mount_src:SetValue("")
                                    ui.ComboBox_RL1_FS_mount_mntpt:SetValue("")
                                    modified:yes()
                            end
                    end
            )

            -- add sizers
            ui.FlexGridSizer_FS_mount_1:Add(ui.FlexGridSizer_FS_mount_2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

            -- FS list
            ui.ListView_RL1_FS_mount = wx.wxListView(ui.Panel_runlevel_1, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, 150), wx.wxLC_REPORT)
            ui.ListView_RL1_FS_mount.AppendItem   = insert_item
            ui.ListView_RL1_FS_mount.GetItemTexts = get_item_texts
            ui.ListView_RL1_FS_mount:InsertColumn(0, "File system", wx.wxLIST_FORMAT_LEFT, 150)
            ui.ListView_RL1_FS_mount:InsertColumn(1, "Source file", wx.wxLIST_FORMAT_LEFT, 150)
            ui.ListView_RL1_FS_mount:InsertColumn(2, "Mount point", wx.wxLIST_FORMAT_LEFT, 150)
            ui.FlexGridSizer_FS_mount_1:Add(ui.ListView_RL1_FS_mount, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- add remove button
            ui.Button_RL1_FS_mount_remove = wx.wxButton(ui.Panel_runlevel_1, wx.wxNewId(), "Remove selected", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_FS_mount_1:Add(ui.Button_RL1_FS_mount_remove, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
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

            -- add group
            ui.StaticBoxSizer_FS_mount:Add(ui.FlexGridSizer_FS_mount_1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.FlexGridSizer_runlevel_1:Add(ui.StaticBoxSizer_FS_mount, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)





        -- create network start group
        ui.StaticBoxSizer_network = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_runlevel_1, "Network start")
        ui.FlexGridSizer_network = wx.wxFlexGridSizer(0, 1, 0, 0)

            -- add DHCP enable checkbox
            ui.CheckBox_RL1_network_DHCP = wx.wxCheckBox(ui.Panel_runlevel_1, wx.wxNewId(), "Start DHCP client to get addresses", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_network:Add(ui.CheckBox_RL1_network_DHCP, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.CheckBox_RL1_network_DHCP:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)

            -- add Static configuration checkbox
            ui.CheckBox_RL1_network_static = wx.wxCheckBox(ui.Panel_runlevel_1, wx.wxNewId(), "Set static IP configuration (see Network configuration)", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_network:Add(ui.CheckBox_RL1_network_static, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.CheckBox_RL1_network_static:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function(event) modified:yes() end)

            -- add connection summary checkbox
            ui.CheckBox_RL1_network_summary = wx.wxCheckBox(ui.Panel_runlevel_1, wx.wxNewId(), "Show connection summary", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_network:Add(ui.CheckBox_RL1_network_summary, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.CheckBox_RL1_network_summary:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function(event) modified:yes() end)

            -- add info text
            ui.StaticText = wx.wxStaticText(ui.Panel_runlevel_1, wx.wxID_ANY, "NOTE: If static and dynamic network configurations are enabled at the same time, then dynamic "..
                                                                              "configuration will  start first. When DHCP client cannot receive addresses, then static "..
                                                                              "configuration will be applied automatically.")
            ui.StaticText:Wrap(ct.CONTROL_X_SIZE)
            ui.FlexGridSizer_network:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

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
        ui.FlexGridSizer_RL2_app_start = wx.wxFlexGridSizer(0, 1, 0, 0)

            -- add information about daemons
            ui.StaticText = wx.wxStaticText(ui.Panel_runlevel_2, wx.wxID_ANY, "NOTE: If you want to start program as daemon set stdin, stdout, and stderr to 'none' or left empty.")
            ui.StaticText:Wrap(ct.CONTROL_X_SIZE)
            ui.FlexGridSizer_RL2_app_start:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- create sizer for fields
            ui.FlexGridSizer_RL2_app_start_fields = wx.wxFlexGridSizer(0, 2, 0, 0)

                -- add program name field
                ui.StaticText = wx.wxStaticText(ui.Panel_runlevel_2, wx.wxID_ANY, "Program name")
                ui.FlexGridSizer_RL2_app_start_fields:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.ComboBox_RL2_app_start_name = wx.wxComboBox(ui.Panel_runlevel_2, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(250,-1), app_list:get_list(), wx.wxTE_PROCESS_ENTER)
                ui.ComboBox_RL2_app_start_name:Connect(wx.wxEVT_COMMAND_TEXT_ENTER, function() ui.Button_RL2_app_start_add:Command(wx.wxCommandEvent(wx.wxEVT_COMMAND_BUTTON_CLICKED)) end)
                ui.ComboBox_RL2_app_start_name:Connect(wx.wxEVT_COMMAND_COMBOBOX_SELECTED, function() modified:yes() end)
                ui.ComboBox_RL2_app_start_name:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED,      function() modified:yes() end)
                ui.FlexGridSizer_RL2_app_start_fields:Add(ui.ComboBox_RL2_app_start_name, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add CWD field
                ui.StaticText = wx.wxStaticText(ui.Panel_runlevel_2, wx.wxID_ANY, "Working directory")
                ui.FlexGridSizer_RL2_app_start_fields:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.ComboBox_RL2_app_start_CWD = wx.wxComboBox(ui.Panel_runlevel_2, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, {}, wx.wxTE_PROCESS_ENTER)
                ui.ComboBox_RL2_app_start_CWD:Connect(wx.wxEVT_COMMAND_TEXT_ENTER, function() ui.Button_RL2_app_start_add:Command(wx.wxCommandEvent(wx.wxEVT_COMMAND_BUTTON_CLICKED)) end)
                ui.ComboBox_RL2_app_start_CWD:Connect(wx.wxEVT_COMMAND_COMBOBOX_SELECTED, function() modified:yes() end)
                ui.ComboBox_RL2_app_start_CWD:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED,      function() modified:yes() end)
                ui.FlexGridSizer_RL2_app_start_fields:Add(ui.ComboBox_RL2_app_start_CWD, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add stdin filed
                ui.StaticText = wx.wxStaticText(ui.Panel_runlevel_2, wx.wxID_ANY, "stdin file")
                ui.FlexGridSizer_RL2_app_start_fields:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.ComboBox_RL2_app_start_stdin = wx.wxComboBox(ui.Panel_runlevel_2, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, {}, wx.wxTE_PROCESS_ENTER)
                ui.ComboBox_RL2_app_start_stdin:Connect(wx.wxEVT_COMMAND_TEXT_ENTER, function() ui.Button_RL2_app_start_add:Command(wx.wxCommandEvent(wx.wxEVT_COMMAND_BUTTON_CLICKED)) end)
                ui.ComboBox_RL2_app_start_stdin:Connect(wx.wxEVT_COMMAND_COMBOBOX_SELECTED, function() modified:yes() end)
                ui.ComboBox_RL2_app_start_stdin:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED,      function() modified:yes() end)
                ui.FlexGridSizer_RL2_app_start_fields:Add(ui.ComboBox_RL2_app_start_stdin, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add stdout filed
                ui.StaticText = wx.wxStaticText(ui.Panel_runlevel_2, wx.wxID_ANY, "stdout file")
                ui.FlexGridSizer_RL2_app_start_fields:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.ComboBox_RL2_app_start_stdout = wx.wxComboBox(ui.Panel_runlevel_2, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, {}, wx.wxTE_PROCESS_ENTER)
                ui.ComboBox_RL2_app_start_stdout:Connect(wx.wxEVT_COMMAND_TEXT_ENTER, function() ui.Button_RL2_app_start_add:Command(wx.wxCommandEvent(wx.wxEVT_COMMAND_BUTTON_CLICKED)) end)
                ui.ComboBox_RL2_app_start_stdout:Connect(wx.wxEVT_COMMAND_COMBOBOX_SELECTED, function() modified:yes() end)
                ui.ComboBox_RL2_app_start_stdout:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED,      function() modified:yes() end)
                ui.FlexGridSizer_RL2_app_start_fields:Add(ui.ComboBox_RL2_app_start_stdout, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add stderr filed
                ui.StaticText = wx.wxStaticText(ui.Panel_runlevel_2, wx.wxID_ANY, "stderr file")
                ui.FlexGridSizer_RL2_app_start_fields:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.ComboBox_RL2_app_start_stderr = wx.wxComboBox(ui.Panel_runlevel_2, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, {}, wx.wxTE_PROCESS_ENTER)
                ui.ComboBox_RL2_app_start_stderr:Connect(wx.wxEVT_COMMAND_TEXT_ENTER, function() ui.Button_RL2_app_start_add:Command(wx.wxCommandEvent(wx.wxEVT_COMMAND_BUTTON_CLICKED)) end)
                ui.ComboBox_RL2_app_start_stderr:Connect(wx.wxEVT_COMMAND_COMBOBOX_SELECTED, function() modified:yes() end)
                ui.ComboBox_RL2_app_start_stderr:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED,      function() modified:yes() end)
                ui.FlexGridSizer_RL2_app_start_fields:Add(ui.ComboBox_RL2_app_start_stderr, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                -- add fields to group
                ui.FlexGridSizer_RL2_app_start:Add(ui.FlexGridSizer_RL2_app_start_fields, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- add Add button
            ui.Button_RL2_app_start_add = wx.wxButton(ui.Panel_runlevel_2, wx.wxNewId(), "Add", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_RL2_app_start:Add(ui.Button_RL2_app_start_add, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.Button_RL2_app_start_add:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                    function()
                            local program = ui.ComboBox_RL2_app_start_name:GetValue()
                            local cwd     = ui.ComboBox_RL2_app_start_CWD:GetValue()
                            local stdin   = ui.ComboBox_RL2_app_start_stdin:GetValue()
                            local stdout  = ui.ComboBox_RL2_app_start_stdout:GetValue()
                            local stderr  = ui.ComboBox_RL2_app_start_stderr:GetValue()

                            if not cwd:match("^/.*") then cwd = "/"..cwd end
                            if stdin  == "" then stdin  = "none" end
                            if stdout == "" then stdout = "none" end
                            if stderr == "" then stderr = "none" end

                            if program ~= "" then
                                    ui.ListView_RL2_app_start:AppendItem(program, cwd, stdin, stdout, stderr)
                                    ui.ComboBox_RL2_app_start_name:SetValue("")
                                    ui.ComboBox_RL2_app_start_CWD:SetValue("")
                                    ui.ComboBox_RL2_app_start_stdin:SetValue("")
                                    ui.ComboBox_RL2_app_start_stdout:SetValue("")
                                    ui.ComboBox_RL2_app_start_stderr:SetValue("")
                                    modified:yes()
                            end
                    end
            )

            -- add list
            ui.ListView_RL2_app_start = wx.wxListView(ui.Panel_runlevel_2, wx.wxNewId(), wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, 250), wx.wxLC_REPORT)
            ui.ListView_RL2_app_start.AppendItem   = insert_item
            ui.ListView_RL2_app_start.GetItemTexts = get_item_texts
            ui.ListView_RL2_app_start:InsertColumn(0, "Command", wx.wxLIST_FORMAT_LEFT, 150)
            ui.ListView_RL2_app_start:InsertColumn(1, "Working directory", wx.wxLIST_FORMAT_LEFT, 150)
            ui.ListView_RL2_app_start:InsertColumn(2, "stdin", wx.wxLIST_FORMAT_LEFT, 80)
            ui.ListView_RL2_app_start:InsertColumn(3, "stdout", wx.wxLIST_FORMAT_LEFT, 80)
            ui.ListView_RL2_app_start:InsertColumn(4, "stderr", wx.wxLIST_FORMAT_LEFT, 80)
            ui.FlexGridSizer_RL2_app_start:Add(ui.ListView_RL2_app_start, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

            -- add Remove button
            ui.Button_RL2_app_start_remove = wx.wxButton(ui.Panel_runlevel_2, wx.wxNewId(), "Remove", wx.wxDefaultPosition, wx.wxDefaultSize)
            ui.FlexGridSizer_RL2_app_start:Add(ui.Button_RL2_app_start_remove, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
            ui.Button_RL2_app_start_remove:Connect(wx.wxEVT_COMMAND_BUTTON_CLICKED,
                    function()
                            local n = ui.ListView_RL2_app_start:GetFirstSelected()
                            if n > -1 then modified:yes() end

                            while n > -1 do
                                    ui.ListView_RL2_app_start:DeleteItem(n)
                                    n = ui.ListView_RL2_app_start:GetNextSelected(-1)
                            end
                    end
            )

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
                ui.Notebook_runlevels:AddPage(create_boot_widgets(ui.Notebook_runlevels), "Runlevel boot", false)
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
        FS_list:reload()
        ui.Choice_RLB_root_FS:Clear()
        ui.Choice_RLB_root_FS:Append(FS_list:get_list())
        ui.Choice_RLB_other_FS_name:Clear()
        ui.Choice_RLB_other_FS_name:Append(FS_list:get_list())

        app_list:reload()
        ui.ComboBox_RL1_daemons_name:Clear()
        ui.ComboBox_RL1_daemons_name:Append(app_list:get_list())

        load_configuration()
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
