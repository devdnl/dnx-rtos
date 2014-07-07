require("wx")
require("wizcore")

file_systems = {}

local ui = {}

local ID = {}
ID.BUTTON_SAVE = wx.wxNewId()
ID.CHECKBOX_DEVFS = wx.wxNewId()
ID.CHECKBOX_FATFS = wx.wxNewId()
ID.CHECKBOX_FATFS_LFN = wx.wxNewId()
ID.CHECKBOX_LFS = wx.wxNewId()
ID.CHECKBOX_PROCFS = wx.wxNewId()
ID.CHOICE_FATFS_LFN_CODEPAGE = wx.wxNewId()


local codepage = {"437 - U.S.",
                  "720 - Arabic",
                  "737 - Greek",
                  "775 - Baltic",
                  "850 - Multilingual Latin 1",
                  "852 - Latin 2",
                  "855 - Cyrillic",
                  "857 - Turkish",
                  "858 - Multilingual Latin 1 + Euro",
                  "862 - Hebrew",
                  "866 - Russian",
                  "874 - Thai",
                  "932 - Japanese Shift-JIS",
                  "936 - Simplified Chinese GBK",
                  "949 - Korean",
                  "950 - Traditional Chinese Big5",
                  "1250 - Central Europe",
                  "1251 - Cyrillic",
                  "1252 - Latin 1",
                  "1253 - Greek",
                  "1254 - Turkish",
                  "1255 - Hebrew",
                  "1256 - Arabic",
                  "1257 - Baltic",
                  "1258 - Vietnam"}


local function load_controls()
        ui.CheckBox_devfs:SetValue(wizcore:get_module_state("DEVFS"))
        ui.CheckBox_lfs:SetValue(wizcore:get_module_state("LFS"))
        ui.CheckBox_fatfs:SetValue(wizcore:get_module_state("FATFS"))
        ui.CheckBox_fatfs_lfn:SetValue(wizcore:yes_no_to_bool(wizcore:key_read(wizcore.PROJECT.KEY.FATFS_LFN_ENABLE)))
        ui.Choice_fatfs_lfn_codepage:SetSelection(wizcore:get_string_index(codepage, wizcore:key_read(wizcore.PROJECT.KEY.FATFS_LFN_CODEPAGE)) - 1)
        ui.CheckBox_procfs:SetValue(wizcore:get_module_state("PROCFS"))
end


local function on_button_save_click()
        wizcore:enable_module("DEVFS", ui.CheckBox_devfs:GetValue())
        wizcore:enable_module("LFS", ui.CheckBox_lfs:GetValue())
        wizcore:enable_module("FATFS", ui.CheckBox_devfs:GetValue())
        wizcore:key_write(wizcore.PROJECT.KEY.FATFS_LFN_ENABLE, wizcore:bool_to_yes_no(ui.CheckBox_fatfs_lfn:GetValue()))
        wizcore:key_write(wizcore.PROJECT.KEY.FATFS_LFN_CODEPAGE, codepage[ui.Choice_fatfs_lfn_codepage:GetSelection() + 1]:match("%d*"))
        wizcore:enable_module("PROCFS", ui.CheckBox_procfs:GetValue())

        ui.Button_save:Enable(false)
end


local function value_changed()
        ui.Button_save:Enable(true)
end


local function FATFS_state_changed(this)
    ui.Choice_fatfs_lfn_codepage:Enable(this:IsChecked())
    ui.CheckBox_fatfs_lfn:Enable(this:IsChecked())
end


local function LFN_enable_changed(this)
        ui.Choice_fatfs_lfn_codepage:Enable(this:IsChecked())
end


function file_systems:create_window(parent)
        if ui.window == nil then
                ui.window  = wx.wxScrolledWindow(parent, wx.wxID_ANY)
                local this = ui.window

                ui.FlexGridSizer1 = wx.wxFlexGridSizer(0, 1, 0, 0)

                --
                ui.StaticBoxSizer1 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "devfs")
                ui.FlexGridSizer2 = wx.wxFlexGridSizer(2, 1, 0, 0)

                ui.StaticText1 = wx.wxStaticText(this, wx.wxID_ANY, "The devfs is a small file system, that can store device and pipe files only. File system is minimalistic and thus is ideal for devices files, because it is fast. A data of this file system is stored in the RAM.", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT1")
                ui.StaticText1:Wrap(wizcore.CONTROL_X_SIZE)
                ui.FlexGridSizer2:Add(ui.StaticText1, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_devfs = wx.wxCheckBox(this, ID.CHECKBOX_DEVFS, "Enable", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_DEVFS")
                ui.FlexGridSizer2:Add(ui.CheckBox_devfs, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticBoxSizer1:Add(ui.FlexGridSizer2, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer1, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                --
                ui.StaticBoxSizer2 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "lfs")

                ui.FlexGridSizer3 = wx.wxFlexGridSizer(2, 1, 0, 0)

                ui.StaticText2 = wx.wxStaticText(this, wx.wxID_ANY, "The lfs is a general purpose RAM file system, that can be used to store files, device-files, and pipes. The file system is the best choice if you want to use only one file system for each file operations. The lfs is not fast as devfs, because provide greater functionality.", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT2")
                ui.StaticText2:Wrap(wizcore.CONTROL_X_SIZE)
                ui.FlexGridSizer3:Add(ui.StaticText2, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_lfs = wx.wxCheckBox(this, ID.CHECKBOX_LFS, "Enable", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_LFS")
                ui.FlexGridSizer3:Add(ui.CheckBox_lfs, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticBoxSizer2:Add(ui.FlexGridSizer3, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer2, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                --
                ui.StaticBoxSizer3 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "fatfs (FAT12, FAT16, FAT32,")

                ui.FlexGridSizer4 = wx.wxFlexGridSizer(2, 1, 0, 0)

                ui.StaticText3 = wx.wxStaticText(this, wx.wxID_ANY, "If you want to use FAT12, FAT16, and FAT32 in your system then enable this file system. The fatfs store data in device-file e.g. SD cards and other bigger volumes. This file system does not support special files.", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT3")
                ui.StaticText3:Wrap(wizcore.CONTROL_X_SIZE)
                ui.FlexGridSizer4:Add(ui.StaticText3, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.CheckBox_fatfs = wx.wxCheckBox(this, ID.CHECKBOX_FATFS, "Enable", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_FATFS")
                ui.FlexGridSizer4:Add(ui.CheckBox_fatfs, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.FlexGridSizer6 = wx.wxFlexGridSizer(1, 2, 0, 0)
                ui.CheckBox_fatfs_lfn = wx.wxCheckBox(this, ID.CHECKBOX_FATFS_LFN, "Enable long file names", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX1")
                ui.FlexGridSizer6:Add(ui.CheckBox_fatfs_lfn, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_fatfs_lfn_codepage = wx.wxChoice(this, ID.CHOICE_FATFS_LFN_CODEPAGE, wx.wxDefaultPosition, wx.wxDefaultSize, codepage, 0, wx.wxDefaultValidator, "ID.CHOICE1")

                ui.FlexGridSizer6:Add(ui.Choice_fatfs_lfn_codepage, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer4:Add(ui.FlexGridSizer6, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 0)

                ui.StaticBoxSizer3:Add(ui.FlexGridSizer4, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer3, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                --
                ui.StaticBoxSizer4 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "procfs")
                ui.FlexGridSizer5 = wx.wxFlexGridSizer(2, 1, 0, 0)

                ui.StaticText4 = wx.wxStaticText(this, wx.wxID_ANY, "The procfs is a special file system, that provide special functionality; from this file system you can see all tasks and their names and so on. In this file system are stored special system files, that can be read to obtain system specified settings, or microcontroller information. If you do not need to read special information, then probably you do not need this file system.", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT4")
                ui.StaticText4:Wrap(wizcore.CONTROL_X_SIZE)
                ui.FlexGridSizer5:Add(ui.StaticText4, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.CheckBox_procfs = wx.wxCheckBox(this, ID.CHECKBOX_PROCFS, "Enable", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_PROCFS")
                ui.FlexGridSizer5:Add(ui.CheckBox_procfs, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)

                ui.StaticBoxSizer4:Add(ui.FlexGridSizer5, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.FlexGridSizer1:Add(ui.StaticBoxSizer4, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                --
                ui.StaticLine2 = wx.wxStaticLine(this, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxSize(10,-1), wx.wxLI_HORIZONTAL)
                ui.FlexGridSizer1:Add(ui.StaticLine2, 1, bit.bor(wx.wxALL, wx.wxEXPAND, wx.wxALIGN_CENTER_HORIZONTAL, wx.wxALIGN_CENTER_VERTICAL), 0)

                ui.Button_save = wx.wxButton(this, ID.BUTTON_SAVE, "&Save", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer1:Add(ui.Button_save, 1, bit.bor(wx.wxALL, wx.wxALIGN_RIGHT, wx.wxALIGN_CENTER_VERTICAL), 5)

                --
                this:SetSizer(ui.FlexGridSizer1)
                this:SetScrollRate(50, 50)

                --
                this:Connect(ID.CHECKBOX_DEVFS,            wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_changed       )
                this:Connect(ID.CHECKBOX_LFS,              wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_changed       )
                this:Connect(ID.CHECKBOX_FATFS,            wx.wxEVT_COMMAND_CHECKBOX_CLICKED, FATFS_state_changed )
                this:Connect(ID.CHECKBOX_FATFS_LFN,        wx.wxEVT_COMMAND_CHECKBOX_CLICKED, LFN_enable_changed  )
                this:Connect(ID.CHOICE_FATFS_LFN_CODEPAGE, wx.wxEVT_COMMAND_CHOICE_SELECTED,  value_changed       )
                this:Connect(ID.CHECKBOX_PROCFS,           wx.wxEVT_COMMAND_CHECKBOX_CLICKED, value_changed       )
                this:Connect(ID.BUTTON_SAVE,               wx.wxEVT_COMMAND_BUTTON_CLICKED,   on_button_save_click)
        end

        return ui.window
end


function file_systems:get_window_name()
        return "File Systems"
end


function file_systems:refresh()
        load_controls()
        ui.Button_save:Enable(false)
end

function file_systems:is_modified()
        return ui.Button_save:IsEnabled()
end
