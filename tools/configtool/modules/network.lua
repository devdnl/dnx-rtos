--[[============================================================================
@file    network.lua

@author  Daniel Zorychta

@brief   Network configuration script.

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
network  = {}
local ui = {}
local ID = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local modified = ct:new_modify_indicator()


--==============================================================================
-- LOCAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function loads all controls from configuration files
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function load_controls()
        -- load module state
        local module_enabled = ct:get_module_state("NETWORK")
        ui.CheckBox_enable:SetValue(module_enabled)

        -- load basic options
        for i = 0, 5 do ui.TextCtrl_MAC[i]:SetValue(ct:key_read(config.project.key["NETWORK_MAC_ADDR_"..i]):gsub("0x", "")) end
        ui.ComboBox_path:SetValue(ct:key_read(config.project.key.NETWORK_ETHIF_FILE):gsub('"', ''))

        -- load advanced options

        -- set notebook enable status
        ui.Notebook_options:Enable(module_enabled)
end


--------------------------------------------------------------------------------
-- @brief  Event is called when Save button is clicked
-- @param  None
-- @return None
--------------------------------------------------------------------------------
local function save_configuration()
        -- save module state
        ct:enable_module("NETWORK", ui.CheckBox_enable:GetValue())

        -- save basic options
        for i = 0, 5 do ct:key_write(config.project.key["NETWORK_MAC_ADDR_"..i], "0x"..ui.TextCtrl_MAC[i]:GetValue()) end
        ct:key_write(config.project.key.NETWORK_ETHIF_FILE, '"'..ui.ComboBox_path:GetValue()..'"')

        -- save advanced options

        -- set that nothing is modified
        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Create basic options controls
-- @param  parent
-- @return Created panel with widgets
--------------------------------------------------------------------------------
local function create_basic_options_widgets(parent)
        ID.PANEL_BASIC   = wx.wxNewId()
        ID.COMBOBOX_PATH = wx.wxNewId()

        -- create panel for basic options
        ui.Panel_basic = wx.wxPanel(parent, ID.PANEL_BASIC, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_basic = wx.wxFlexGridSizer(0, 1, 0, 0)

        -- create staticbox sizer for MAC address
        ui.StaticBoxSizer_MAC = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_basic, "MAC address")
        ui.FlexGridSizer_MAC  = wx.wxFlexGridSizer(0, 11, 0, 0)

                -- add MAC address widgets
                ui.TextCtrl_MAC = {}
                ID.TEXTCTRL_MAC = {}
                for i = 0, 5 do
                        ID.TEXTCTRL_MAC[i] = wx.wxNewId()

                        ui.TextCtrl_MAC[i] = wx.wxTextCtrl(ui.Panel_basic, ID.TEXTCTRL_MAC[i], "FF", wx.wxDefaultPosition, wx.wxSize(30,-1), 0, ct.hexvalidator)
                        ui.TextCtrl_MAC[i]:SetMaxLength(2)
                        ui.FlexGridSizer_MAC:Add(ui.TextCtrl_MAC[i], 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                        if i < 5 then
                                ui.StaticText = wx.wxStaticText(ui.Panel_basic, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize)
                                ui.FlexGridSizer_MAC:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                        end

                        ui.window:Connect(ID.TEXTCTRL_MAC[i], wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
                end

                -- add MAC group to the sizer
                ui.StaticBoxSizer_MAC:Add(ui.FlexGridSizer_MAC, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.FlexGridSizer_basic:Add(ui.StaticBoxSizer_MAC, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- create group for Ethernet device path
        ui.StaticBoxSizer_path = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_basic, "Ethernet device path")

                -- create path selection combobox
                ui.ComboBox_path = wx.wxComboBox(ui.Panel_basic, ID.COMBOBOX_PATH, "", wx.wxDefaultPosition, wx.wxDefaultSize, {})
                ui.ComboBox_path:Append("/dev/eth0")
                ui.ComboBox_path:Connect(wx.wxEVT_COMMAND_COMBOBOX_SELECTED, function() modified:yes() end)
                ui.ComboBox_path:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
                ui.StaticBoxSizer_path:Add(ui.ComboBox_path, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer_basic:Add(ui.StaticBoxSizer_path, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- set panel's sizer
        ui.Panel_basic:SetSizer(ui.FlexGridSizer_basic)

        return ui.Panel_basic
end


--------------------------------------------------------------------------------
-- @brief  Memory options widgets
-- @param  parent
-- @return Created panel
--------------------------------------------------------------------------------
local function create_memory_options_widgets(parent)
        -- create panel
        ui.Panel_adv_MEM = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_adv_MEM = wx.wxFlexGridSizer(0, 2, 0, 0)

        ui.StaticText = wx.wxStaticText(ui.Panel_adv_MEM, wx.wxID_ANY, "Memory options", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_adv_MEM:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

        -- set panel's sizer
        ui.Panel_adv_MEM:SetSizer(ui.FlexGridSizer_adv_MEM)

        return ui.Panel_adv_MEM
end


--------------------------------------------------------------------------------
-- @brief  Internal memory pool sizes widgets
-- @param  parent
-- @return Created panel
--------------------------------------------------------------------------------
local function create_internal_memory_pool_sizes_options_widgets(parent)
        -- create panel
        ui.Panel_adv_IMPS = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_adv_IMPS = wx.wxFlexGridSizer(0, 2, 0, 0)

        ui.StaticText = wx.wxStaticText(ui.Panel_adv_IMPS, wx.wxID_ANY, "internal memory pool sizes", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_adv_IMPS:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

        -- set panel's sizer
        ui.Panel_adv_IMPS:SetSizer(ui.FlexGridSizer_adv_IMPS)

        return ui.Panel_adv_IMPS
end


--------------------------------------------------------------------------------
-- @brief  ARP options widgets
-- @param  parent
-- @return Created panel
--------------------------------------------------------------------------------
local function create_ARP_options_widgets(parent)
        -- create panel
        ui.Panel_adv_ARP = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_adv_ARP = wx.wxFlexGridSizer(0, 2, 0, 0)

        ui.StaticText = wx.wxStaticText(ui.Panel_adv_ARP, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_adv_ARP:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

        -- set panel's sizer
        ui.Panel_adv_ARP:SetSizer(ui.FlexGridSizer_adv_ARP)

        return ui.Panel_adv_ARP
end


--------------------------------------------------------------------------------
-- @brief  IP options widgets
-- @param  parent
-- @return Created panel
--------------------------------------------------------------------------------
local function create_IP_options_widgets(parent)
        -- create panel
        ui.Panel_adv_IP = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_adv_IP = wx.wxFlexGridSizer(0, 2, 0, 0)

        ui.StaticText = wx.wxStaticText(ui.Panel_adv_IP, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_adv_IP:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

        -- set panel's sizer
        ui.Panel_adv_IP:SetSizer(ui.FlexGridSizer_adv_IP)

        return ui.Panel_adv_IP
end


--------------------------------------------------------------------------------
-- @brief  ICMP options widgets
-- @param  parent
-- @return Created panel
--------------------------------------------------------------------------------
local function create_ICMP_options_widgets(parent)
        -- create panel
        ui.Panel_adv_ICMP = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_adv_ICMP = wx.wxFlexGridSizer(0, 2, 0, 0)

        ui.StaticText = wx.wxStaticText(ui.Panel_adv_ICMP, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_adv_ICMP:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

        -- set panel's sizer
        ui.Panel_adv_ICMP:SetSizer(ui.FlexGridSizer_adv_ICMP)

        return ui.Panel_adv_ICMP
end


--------------------------------------------------------------------------------
-- @brief  DHCP options widgets
-- @param  parent
-- @return Created panel
--------------------------------------------------------------------------------
local function create_DHCP_options_widgets(parent)
        -- create panel
        ui.Panel_adv_DHCP = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_adv_DHCP = wx.wxFlexGridSizer(0, 2, 0, 0)

        ui.StaticText = wx.wxStaticText(ui.Panel_adv_DHCP, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_adv_DHCP:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

        -- set panel's sizer
        ui.Panel_adv_DHCP:SetSizer(ui.FlexGridSizer_adv_DHCP)

        return ui.Panel_adv_DHCP
end


--------------------------------------------------------------------------------
-- @brief  AUTOIP options widgets
-- @param  parent
-- @return Created panel
--------------------------------------------------------------------------------
local function create_AUTOIP_options_widgets(parent)
        -- create panel
        ui.Panel_adv_AUTOIP = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_adv_AUTOIP = wx.wxFlexGridSizer(0, 2, 0, 0)

        ui.StaticText = wx.wxStaticText(ui.Panel_adv_AUTOIP, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_adv_AUTOIP:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

        -- set panel's sizer
        ui.Panel_adv_AUTOIP:SetSizer(ui.FlexGridSizer_adv_AUTOIP)

        return ui.Panel_adv_AUTOIP
end


--------------------------------------------------------------------------------
-- @brief  SNMP options widgets
-- @param  parent
-- @return Created panel
--------------------------------------------------------------------------------
local function create_SNMP_options_widgets(parent)
        -- create panel
        ui.Panel_adv_SNMP = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_adv_SNMP = wx.wxFlexGridSizer(0, 2, 0, 0)

        ui.StaticText = wx.wxStaticText(ui.Panel_adv_SNMP, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_adv_SNMP:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

        -- set panel's sizer
        ui.Panel_adv_SNMP:SetSizer(ui.FlexGridSizer_adv_SNMP)

        return ui.Panel_adv_SNMP
end


--------------------------------------------------------------------------------
-- @brief  IGMP options widgets
-- @param  parent
-- @return Created panel
--------------------------------------------------------------------------------
local function create_IGMP_options_widgets(parent)
        -- create panel
        ui.Panel_adv_IGMP = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_adv_IGMP = wx.wxFlexGridSizer(0, 2, 0, 0)

        ui.StaticText = wx.wxStaticText(ui.Panel_adv_IGMP, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_adv_IGMP:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

        -- set panel's sizer
        ui.Panel_adv_IGMP:SetSizer(ui.FlexGridSizer_adv_IGMP)

        return ui.Panel_adv_IGMP
end


--------------------------------------------------------------------------------
-- @brief  DNS options widgets
-- @param  parent
-- @return Created panel
--------------------------------------------------------------------------------
local function create_DNS_options_widgets(parent)
        -- create panel
        ui.Panel_adv_DNS = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_adv_DNS = wx.wxFlexGridSizer(0, 2, 0, 0)

        ui.StaticText = wx.wxStaticText(ui.Panel_adv_DNS, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_adv_DNS:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

        -- set panel's sizer
        ui.Panel_adv_DNS:SetSizer(ui.FlexGridSizer_adv_DNS)

        return ui.Panel_adv_DNS
end


--------------------------------------------------------------------------------
-- @brief  UDP options widgets
-- @param  parent
-- @return Created panel
--------------------------------------------------------------------------------
local function create_UDP_options_widgets(parent)
        -- create panel
        ui.Panel_adv_UDP = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_adv_UDP = wx.wxFlexGridSizer(0, 2, 0, 0)

        ui.StaticText = wx.wxStaticText(ui.Panel_adv_UDP, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_adv_UDP:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

        -- set panel's sizer
        ui.Panel_adv_UDP:SetSizer(ui.FlexGridSizer_adv_UDP)

        return ui.Panel_adv_UDP
end


--------------------------------------------------------------------------------
-- @brief  TCP options widgets
-- @param  parent
-- @return Created panel
--------------------------------------------------------------------------------
local function create_TCP_options_widgets(parent)
        -- create panel
        ui.Panel_adv_TCP = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_adv_TCP = wx.wxFlexGridSizer(0, 2, 0, 0)

        ui.StaticText = wx.wxStaticText(ui.Panel_adv_TCP, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_adv_TCP:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

        -- set panel's sizer
        ui.Panel_adv_TCP:SetSizer(ui.FlexGridSizer_adv_TCP)

        return ui.Panel_adv_TCP
end


--------------------------------------------------------------------------------
-- @brief  NETIF options widgets
-- @param  parent
-- @return Created panel
--------------------------------------------------------------------------------
local function create_NETIF_options_widgets(parent)
        -- create panel
        ui.Panel_adv_NETIF = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_adv_NETIF = wx.wxFlexGridSizer(0, 2, 0, 0)

        ui.StaticText = wx.wxStaticText(ui.Panel_adv_NETIF, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_adv_NETIF:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

        -- set panel's sizer
        ui.Panel_adv_NETIF:SetSizer(ui.FlexGridSizer_adv_NETIF)

        return ui.Panel_adv_NETIF
end


--------------------------------------------------------------------------------
-- @brief  LOOPIF options widgets
-- @param  parent
-- @return Created panel
--------------------------------------------------------------------------------
local function create_LOOPIF_options_widgets(parent)
        -- create panel
        ui.Panel_adv_LOOPIF = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_adv_LOOPIF = wx.wxFlexGridSizer(0, 2, 0, 0)

        ui.StaticText = wx.wxStaticText(ui.Panel_adv_LOOPIF, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_adv_LOOPIF:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

        -- set panel's sizer
        ui.Panel_adv_LOOPIF:SetSizer(ui.FlexGridSizer_adv_LOOPIF)

        return ui.Panel_adv_LOOPIF
end


--------------------------------------------------------------------------------
-- @brief  SLIPIF options widgets
-- @param  parent
-- @return Created panel
--------------------------------------------------------------------------------
local function create_SLIPIF_options_widgets(parent)
        -- create panel
        ui.Panel_adv_SLIPIF = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_adv_SLIPIF = wx.wxFlexGridSizer(0, 2, 0, 0)

        ui.StaticText = wx.wxStaticText(ui.Panel_adv_SLIPIF, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_adv_SLIPIF:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

        -- set panel's sizer
        ui.Panel_adv_SLIPIF:SetSizer(ui.FlexGridSizer_adv_SLIPIF)

        return ui.Panel_adv_SLIPIF
end


--------------------------------------------------------------------------------
-- @brief  THREAD options widgets
-- @param  parent
-- @return Created panel
--------------------------------------------------------------------------------
local function create_THREAD_options_widgets(parent)
        -- create panel
        ui.Panel_adv_THREAD = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_adv_THREAD = wx.wxFlexGridSizer(0, 2, 0, 0)

        ui.StaticText = wx.wxStaticText(ui.Panel_adv_THREAD, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_adv_THREAD:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

        -- set panel's sizer
        ui.Panel_adv_THREAD:SetSizer(ui.FlexGridSizer_adv_THREAD)

        return ui.Panel_adv_THREAD
end


--------------------------------------------------------------------------------
-- @brief  SEQL options widgets
-- @param  parent
-- @return Created panel
--------------------------------------------------------------------------------
local function create_SEQL_options_widgets(parent)
        -- create panel
        ui.Panel_adv_SEQL = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_adv_SEQL = wx.wxFlexGridSizer(0, 2, 0, 0)

        ui.StaticText = wx.wxStaticText(ui.Panel_adv_SEQL, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_adv_SEQL:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

        -- set panel's sizer
        ui.Panel_adv_SEQL:SetSizer(ui.FlexGridSizer_adv_SEQL)

        return ui.Panel_adv_SEQL
end


--------------------------------------------------------------------------------
-- @brief  PPP options widgets
-- @param  parent
-- @return Created panel
--------------------------------------------------------------------------------
local function create_PPP_options_widgets(parent)
        -- create panel
        ui.Panel_adv_PPP = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_adv_PPP = wx.wxFlexGridSizer(0, 2, 0, 0)

        ui.StaticText = wx.wxStaticText(ui.Panel_adv_PPP, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_adv_PPP:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

        -- set panel's sizer
        ui.Panel_adv_PPP:SetSizer(ui.FlexGridSizer_adv_PPP)

        return ui.Panel_adv_PPP
end


--------------------------------------------------------------------------------
-- @brief  CHSUM options widgets
-- @param  parent
-- @return Created panel
--------------------------------------------------------------------------------
local function create_CHSUM_options_widgets(parent)
        -- create panel
        ui.Panel_adv_CHSUM = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_adv_CHSUM = wx.wxFlexGridSizer(0, 2, 0, 0)

        ui.StaticText = wx.wxStaticText(ui.Panel_adv_CHSUM, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_adv_CHSUM:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

        -- set panel's sizer
        ui.Panel_adv_CHSUM:SetSizer(ui.FlexGridSizer_adv_CHSUM)

        return ui.Panel_adv_CHSUM
end


--------------------------------------------------------------------------------
-- @brief  DEBUG options widgets
-- @param  parent
-- @return Created panel
--------------------------------------------------------------------------------
local function create_DEBUG_options_widgets(parent)
        -- create panel
        ui.Panel_adv_DEBUG = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_adv_DEBUG = wx.wxFlexGridSizer(0, 2, 0, 0)

        ui.StaticText = wx.wxStaticText(ui.Panel_adv_DEBUG, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

        -- set panel's sizer
        ui.Panel_adv_DEBUG:SetSizer(ui.FlexGridSizer_adv_DEBUG)

        return ui.Panel_adv_DEBUG
end


--------------------------------------------------------------------------------
-- @brief  Create advanced options controls
-- @param  parent
-- @return Created panel with widgets
--------------------------------------------------------------------------------
local function create_advanced_options_widgets(parent)
        ID.PANEL_ADVANCED    = wx.wxNewId()
        ID.CHOICEBOOK_OPTION = wx.wxNewId()

        -- create panel
        ui.Panel_advanced = wx.wxPanel(parent, ID.PANEL_ADVANCED, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_advanced = wx.wxFlexGridSizer(0, 1, 0, 0)

        -- create choicebook to more advanced options
        ui.Choicebook_option = wx.wxChoicebook(ui.Panel_advanced, ID.CHOICEBOOK_OPTION, wx.wxDefaultPosition, wx.wxSize(ct.CONTROL_X_SIZE, -1))
        ui.FlexGridSizer_advanced:Add(ui.Choicebook_option, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- add advanced options
        ui.Choicebook_option:AddPage(create_memory_options_widgets(ui.Choicebook_option), "Memory options", false)
        ui.Choicebook_option:AddPage(create_internal_memory_pool_sizes_options_widgets(ui.Choicebook_option), "Internal memory pool sizes", false)
        ui.Choicebook_option:AddPage(create_ARP_options_widgets(ui.Choicebook_option), "ARP options", false)
        ui.Choicebook_option:AddPage(create_IP_options_widgets(ui.Choicebook_option), "IP options", false)
        ui.Choicebook_option:AddPage(create_ICMP_options_widgets(ui.Choicebook_option), "ICMP options", false)
        ui.Choicebook_option:AddPage(create_DHCP_options_widgets(ui.Choicebook_option), "DHCP options", false)
        ui.Choicebook_option:AddPage(create_AUTOIP_options_widgets(ui.Choicebook_option), "AUTOIP options", false)
        ui.Choicebook_option:AddPage(create_SNMP_options_widgets(ui.Choicebook_option), "SNMP options", false)
        ui.Choicebook_option:AddPage(create_IGMP_options_widgets(ui.Choicebook_option), "IGMP options", false)
        ui.Choicebook_option:AddPage(create_DNS_options_widgets(ui.Choicebook_option), "DNS options", false)
        ui.Choicebook_option:AddPage(create_UDP_options_widgets(ui.Choicebook_option), "UDP options", false)
        ui.Choicebook_option:AddPage(create_TCP_options_widgets(ui.Choicebook_option), "TCP options", false)
        ui.Choicebook_option:AddPage(create_NETIF_options_widgets(ui.Choicebook_option), "Network Interfaces options", false)
        ui.Choicebook_option:AddPage(create_LOOPIF_options_widgets(ui.Choicebook_option), "LOOPIF options", false)
        ui.Choicebook_option:AddPage(create_SLIPIF_options_widgets(ui.Choicebook_option), "SLIPIF options", false)
        ui.Choicebook_option:AddPage(create_THREAD_options_widgets(ui.Choicebook_option), "Thread options", false)
        ui.Choicebook_option:AddPage(create_SEQL_options_widgets(ui.Choicebook_option), "Sequential Layer options", false)
        ui.Choicebook_option:AddPage(create_PPP_options_widgets(ui.Choicebook_option), "PPP options", false)
        ui.Choicebook_option:AddPage(create_CHSUM_options_widgets(ui.Choicebook_option), "Checksum options", false)
        ui.Choicebook_option:AddPage(create_DEBUG_options_widgets(ui.Choicebook_option), "Debug options", false)

        -- set panel's sizer
        ui.Panel_advanced:SetSizer(ui.FlexGridSizer_advanced)

        return ui.Panel_advanced
end

--==============================================================================
-- GLOBAL FUNCTIONS
--==============================================================================
--------------------------------------------------------------------------------
-- @brief  Function creates a new window
-- @param  parent       parent window
-- @return New window handle
--------------------------------------------------------------------------------
function network:create_window(parent)
        if ui.window == nil then
                ID.CHECKBOX_ENABLE  = wx.wxNewId()
                ID.NOTEBOOK_OPTIONS = wx.wxNewId()

                -- create main window
                ui.window = wx.wxScrolledWindow(parent, wx.wxID_ANY)
                ui.FlexGridSizer_main = wx.wxFlexGridSizer(0, 1, 0, 0)

                -- create module enable checkbox
                ui.CheckBox_enable = wx.wxCheckBox(ui.window, ID.CHECKBOX_ENABLE, "Enable", wx.wxDefaultPosition, wx.wxDefaultSize)
                ui.FlexGridSizer_main:Add(ui.CheckBox_enable, 1, bit.bor(wx.wxALL,wx.wxALIGN_LEFT,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.CheckBox_enable:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED,
                        function(event)
                                ui.Notebook_options:Enable(event:IsChecked())
                                modified:yes()
                        end
                )

                -- create notebook with options (used as panel)
                ui.Notebook_options = wx.wxNotebook(ui.window, ID.NOTEBOOK_OPTIONS, wx.wxDefaultPosition, wx.wxDefaultSize)

                -- add options panels
                ui.Notebook_options:AddPage(create_basic_options_widgets(ui.Notebook_options), "Basic", false)
                ui.Notebook_options:AddPage(create_advanced_options_widgets(ui.Notebook_options), "Advanced", false)

                -- add notebook to the main sizer
                ui.FlexGridSizer_main:Add(ui.Notebook_options, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
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
function network:get_window_name()
        return "Network"
end


--------------------------------------------------------------------------------
-- @brief  Function is called when window is selected
-- @param  None
-- @return None
--------------------------------------------------------------------------------
function network:refresh()
        load_controls()
        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Function check if options are modified
-- @param  None
-- @return true if options are modified, otherwise false
--------------------------------------------------------------------------------
function network:is_modified()
        return modified:get_value()
end


--------------------------------------------------------------------------------
-- @brief  Function save configuration
-- @return None
--------------------------------------------------------------------------------
function network:save()
        save_configuration()
end
