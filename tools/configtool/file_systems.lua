require("wx")
require("wizcore")

file_systems = {}

local ui = {}

local ID = {}
ID.BUTTON_SAVE = wx.wxNewId()
ID.CHECKBOX_DEVFS = wx.wxNewId()
ID.CHECKBOX_FATFS = wx.wxNewId()
ID.CHECKBOX_LFN = wx.wxNewId()
ID.CHECKBOX_LFS = wx.wxNewId()
ID.CHECKBOX_PROCFS = wx.wxNewId()
ID.CHOICE_LFN_CODEPAGE = wx.wxNewId()


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
                ui.StaticBoxSizer3 = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, this, "fatfs (FAT12, FAT16, FAT32)")

                ui.FlexGridSizer4 = wx.wxFlexGridSizer(2, 1, 0, 0)

                ui.StaticText3 = wx.wxStaticText(this, wx.wxID_ANY, "If you want to use FAT12, FAT16, and FAT32 in your system then enable this file system. The fatfs store data in device-file e.g. SD cards and other bigger volumes. This file system does not support special files.", wx.wxDefaultPosition, wx.wxDefaultSize, 0, "ID.STATICTEXT3")
                ui.StaticText3:Wrap(wizcore.CONTROL_X_SIZE)
                ui.FlexGridSizer4:Add(ui.StaticText3, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.CheckBox_fatfs = wx.wxCheckBox(this, ID.CHECKBOX_FATFS, "Enable", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX_FATFS")
                ui.FlexGridSizer4:Add(ui.CheckBox_fatfs, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                
                ui.FlexGridSizer6 = wx.wxFlexGridSizer(1, 2, 0, 0)
                ui.CheckBox_lfn = wx.wxCheckBox(this, ID.CHECKBOX_LFN, "Enable long file names", wx.wxDefaultPosition, wx.wxDefaultSize, 0, wx.wxDefaultValidator, "ID.CHECKBOX1")
                ui.FlexGridSizer6:Add(ui.CheckBox_lfn, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.Choice_lfn_codepage = wx.wxChoice(this, ID.CHOICE_LFN_CODEPAGE, wx.wxDefaultPosition, wx.wxDefaultSize, {}, 0, wx.wxDefaultValidator, "ID.CHOICE1")
                ui.Choice_lfn_codepage:Append("437 - U.S.")
                ui.Choice_lfn_codepage:Append("720 - Arabic")
                ui.Choice_lfn_codepage:Append("737 - Greek")
                ui.Choice_lfn_codepage:Append("775 - Baltic")
                ui.Choice_lfn_codepage:Append("850 - Multilingual Latin 1")
                ui.Choice_lfn_codepage:Append("852 - Latin 2")
                ui.Choice_lfn_codepage:Append("855 - Cyrillic")
                ui.Choice_lfn_codepage:Append("857 - Turkish")
                ui.Choice_lfn_codepage:Append("858 - Multilingual Latin 1 + Euro")
                ui.Choice_lfn_codepage:Append("862 - Hebrew")
                ui.Choice_lfn_codepage:Append("866 - Russian")
                ui.Choice_lfn_codepage:Append("874 - Thai")
                ui.Choice_lfn_codepage:Append("932 - Japanese Shift-JIS")
                ui.Choice_lfn_codepage:Append("936 - Simplified Chinese GBK")
                ui.Choice_lfn_codepage:Append("949 - Korean")
                ui.Choice_lfn_codepage:Append("950 - Traditional Chinese Big5")
                ui.Choice_lfn_codepage:Append("1250 - Central Europe")
                ui.Choice_lfn_codepage:Append("1251 - Cyrillic")
                ui.Choice_lfn_codepage:Append("1252 - Latin 1")
                ui.Choice_lfn_codepage:Append("1253 - Greek")
                ui.Choice_lfn_codepage:Append("1254 - Turkish")
                ui.Choice_lfn_codepage:Append("1255 - Hebrew")
                ui.Choice_lfn_codepage:Append("1256 - Arabic")
                ui.Choice_lfn_codepage:Append("1257 - Baltic")
                ui.Choice_lfn_codepage:Append("1258 - Vietnam")
                ui.FlexGridSizer6:Add(ui.Choice_lfn_codepage, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
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
                this:SetScrollRate(5, 5)
        end

        return ui.window
end


function file_systems:get_window_name()
        return "File Systems"
end


function file_systems:refresh()
end

function file_systems:is_modified()
        return false
end
