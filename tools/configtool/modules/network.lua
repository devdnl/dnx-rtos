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

        -- load adv MEM options
        ui.Choice_adv_MEMP_SEPARATE_POOLS:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_MEMP_SEPARATE_POOLS)))
        ui.Choice_adv_MEMP_OVERFLOW_CHECK:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_MEMP_OVERFLOW_CHECK)))
        ui.Choice_adv_MEMP_SANITY_CHECK:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_MEMP_SANITY_CHECK)))

        -- load adv internal memory pool sizes
        ui.SpinCtrl_adv_MEMP_NUM_PBUF:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_MEMP_NUM_PBUF)))
        ui.SpinCtrl_adv_MEMP_NUM_RAW_PCB:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_MEMP_NUM_RAW_PCB)))
        ui.SpinCtrl_adv_MEMP_NUM_UDP_PCB:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_MEMP_NUM_UDP_PCB)))
        ui.SpinCtrl_adv_MEMP_NUM_TCP_PCB:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_MEMP_NUM_TCP_PCB)))
        ui.SpinCtrl_adv_MEMP_NUM_TCP_PCB_LISTEN:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_MEMP_NUM_TCP_PCB_LISTEN)))
        ui.SpinCtrl_adv_MEMP_NUM_TCP_SEG:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_MEMP_NUM_TCP_SEG)))
        ui.SpinCtrl_adv_MEMP_NUM_REASSDATA:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_MEMP_NUM_REASSDATA)))
        ui.SpinCtrl_adv_MEMP_NUM_FRAG_PBUF:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_MEMP_NUM_FRAG_PBUF)))
        ui.SpinCtrl_adv_MEMP_NUM_ARP_QUEUE:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_MEMP_NUM_ARP_QUEUE)))
        ui.SpinCtrl_adv_MEMP_NUM_IGMP_GROUP:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_MEMP_NUM_IGMP_GROUP)))
        ui.SpinCtrl_adv_MEMP_NUM_NETBUF:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_MEMP_NUM_NETBUF)))
        ui.SpinCtrl_adv_MEMP_NUM_NETCONN:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_MEMP_NUM_NETCONN)))
        ui.SpinCtrl_adv_MEMP_NUM_TCPIP_MSG_API:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_MEMP_NUM_TCPIP_MSG_API)))
        ui.SpinCtrl_adv_MEMP_NUM_TCPIP_MSG_INPKT:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_MEMP_NUM_TCPIP_MSG_INPKT)))
        ui.SpinCtrl_adv_MEMP_NUM_SNMP_NODE:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_MEMP_NUM_SNMP_NODE)))
        ui.SpinCtrl_adv_MEMP_NUM_SNMP_ROOTNODE:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_MEMP_NUM_SNMP_ROOTNODE)))
        ui.SpinCtrl_adv_MEMP_NUM_LOCALHOSTLIST:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_MEMP_NUM_LOCALHOSTLIST)))
        ui.SpinCtrl_adv_MEMP_NUM_PPPOE_INTERFACES:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_MEMP_NUM_PPPOE_INTERFACES)))
        ui.SpinCtrl_adv_PBUF_POOL_SIZE:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_PBUF_POOL_SIZE)))

        -- load adv ARP options
        ui.Choice_adv_LWIP_ARP:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_LWIP_ARP)))
        ui.SpinCtrl_adv_ARP_TABLE_SIZE:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_ARP_TABLE_SIZE)))
        ui.Choice_adv_ARP_QUEUEING:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_ARP_QUEUEING)))
        ui.Choice_adv_ETHARP_TRUST_IP_MAC:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_ETHARP_TRUST_IP_MAC)))
        ui.Choice_adv_ETHARP_SUPPORT_VLAN:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_ETHARP_SUPPORT_VLAN)))
        ui.SpinCtrl_adv_ETH_PAD_SIZE:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_ETH_PAD_SIZE)))
        ui.Choice_adv_ETHARP_SUPPORT_STATIC_ENTRIES:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_ETHARP_SUPPORT_STATIC_ENTRIES)))

        -- load adv IP options
        ui.Choice_adv_IP_FORWARD:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_IP_FORWARD)))
        ui.Choice_adv_IP_OPTIONS_ALLOWED:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_IP_OPTIONS_ALLOWED)))
        ui.Choice_adv_IP_REASSEMBLY:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_IP_REASSEMBLY)))
        ui.Choice_adv_IP_FRAG:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_IP_FRAG)))
        ui.SpinCtrl_adv_IP_REASS_MAXAGE:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_IP_REASS_MAXAGE)))
        ui.SpinCtrl_adv_IP_REASS_MAX_PBUFS:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_IP_REASS_MAX_PBUFS)))
        ui.SpinCtrl_adv_IP_DEFAULT_TTL:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_IP_DEFAULT_TTL)))
        ui.Choice_adv_IP_SOF_BROADCAST:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_IP_SOF_BROADCAST)))
        ui.Choice_adv_IP_SOF_BROADCAST_RECV:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_IP_SOF_BROADCAST_RECV)))
        ui.Choice_adv_IP_FORWARD_ALLOW_TX_ON_RX_NETIF:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_IP_FORWARD_ALLOW_TX_ON_RX_NETIF)))

        -- load adv ICMP options
        ui.Choice_adv_LWIP_ICMP:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_LWIP_ICMP)))
        ui.SpinCtrl_adv_ICMP_TTL:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_ICMP_TTL)))
        ui.Choice_adv_LWIP_BROADCAST_PING:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_LWIP_BROADCAST_PING)))
        ui.Choice_adv_LWIP_MULTICAST_PING:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_LWIP_MULTICAST_PING)))

        -- load adv DHCP options
        ui.Choice_adv_LWIP_DHCP:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_LWIP_DHCP)))

        -- load adv AUTOIP options
        ui.Choice_adv_LWIP_AUTOIP:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_LWIP_AUTOIP)))
        ui.Choice_adv_LWIP_DHCP_AUTOIP_COOP:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_LWIP_DHCP_AUTOIP_COOP)))
        ui.SpinCtrl_adv_LWIP_DHCP_AUTOIP_COOP_TRIES:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_LWIP_DHCP_AUTOIP_COOP_TRIES)))

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

        -- save adv MEM options
        ct:key_write(config.project.key.NETWORK_MEMP_SEPARATE_POOLS, tostring(ui.Choice_adv_MEMP_SEPARATE_POOLS:GetSelection()))
        ct:key_write(config.project.key.NETWORK_MEMP_OVERFLOW_CHECK, tostring(ui.Choice_adv_MEMP_OVERFLOW_CHECK:GetSelection()))
        ct:key_write(config.project.key.NETWORK_MEMP_SANITY_CHECK, tostring(ui.Choice_adv_MEMP_SANITY_CHECK:GetSelection()))

        -- save adv internal memory pool sizes
        ct:key_write(config.project.key.NETWORK_MEMP_NUM_PBUF, tostring(ui.SpinCtrl_adv_MEMP_NUM_PBUF:GetValue()))
        ct:key_write(config.project.key.NETWORK_MEMP_NUM_RAW_PCB, tostring(ui.SpinCtrl_adv_MEMP_NUM_RAW_PCB:GetValue()))
        ct:key_write(config.project.key.NETWORK_MEMP_NUM_UDP_PCB, tostring(ui.SpinCtrl_adv_MEMP_NUM_UDP_PCB:GetValue()))
        ct:key_write(config.project.key.NETWORK_MEMP_NUM_TCP_PCB, tostring(ui.SpinCtrl_adv_MEMP_NUM_TCP_PCB:GetValue()))
        ct:key_write(config.project.key.NETWORK_MEMP_NUM_TCP_PCB_LISTEN, tostring(ui.SpinCtrl_adv_MEMP_NUM_TCP_PCB_LISTEN:GetValue()))
        ct:key_write(config.project.key.NETWORK_MEMP_NUM_TCP_SEG, tostring(ui.SpinCtrl_adv_MEMP_NUM_TCP_SEG:GetValue()))
        ct:key_write(config.project.key.NETWORK_MEMP_NUM_REASSDATA, tostring(ui.SpinCtrl_adv_MEMP_NUM_REASSDATA:GetValue()))
        ct:key_write(config.project.key.NETWORK_MEMP_NUM_FRAG_PBUF, tostring(ui.SpinCtrl_adv_MEMP_NUM_FRAG_PBUF:GetValue()))
        ct:key_write(config.project.key.NETWORK_MEMP_NUM_ARP_QUEUE, tostring(ui.SpinCtrl_adv_MEMP_NUM_ARP_QUEUE:GetValue()))
        ct:key_write(config.project.key.NETWORK_MEMP_NUM_IGMP_GROUP, tostring(ui.SpinCtrl_adv_MEMP_NUM_IGMP_GROUP:GetValue()))
        ct:key_write(config.project.key.NETWORK_MEMP_NUM_NETBUF, tostring(ui.SpinCtrl_adv_MEMP_NUM_NETBUF:GetValue()))
        ct:key_write(config.project.key.NETWORK_MEMP_NUM_NETCONN, tostring(ui.SpinCtrl_adv_MEMP_NUM_NETCONN:GetValue()))
        ct:key_write(config.project.key.NETWORK_MEMP_NUM_TCPIP_MSG_API, tostring(ui.SpinCtrl_adv_MEMP_NUM_TCPIP_MSG_API:GetValue()))
        ct:key_write(config.project.key.NETWORK_MEMP_NUM_TCPIP_MSG_INPKT, tostring(ui.SpinCtrl_adv_MEMP_NUM_TCPIP_MSG_INPKT:GetValue()))
        ct:key_write(config.project.key.NETWORK_MEMP_NUM_SNMP_NODE, tostring(ui.SpinCtrl_adv_MEMP_NUM_SNMP_NODE:GetValue()))
        ct:key_write(config.project.key.NETWORK_MEMP_NUM_SNMP_ROOTNODE, tostring(ui.SpinCtrl_adv_MEMP_NUM_SNMP_ROOTNODE:GetValue()))
        ct:key_write(config.project.key.NETWORK_MEMP_NUM_LOCALHOSTLIST, tostring(ui.SpinCtrl_adv_MEMP_NUM_LOCALHOSTLIST:GetValue()))
        ct:key_write(config.project.key.NETWORK_MEMP_NUM_PPPOE_INTERFACES, tostring(ui.SpinCtrl_adv_MEMP_NUM_PPPOE_INTERFACES:GetValue()))
        ct:key_write(config.project.key.NETWORK_PBUF_POOL_SIZE, tostring(ui.SpinCtrl_adv_PBUF_POOL_SIZE:GetValue()))

        -- save adv ARP options
        ct:key_write(config.project.key.NETWORK_LWIP_ARP, tostring(ui.Choice_adv_LWIP_ARP:GetSelection()))
        ct:key_write(config.project.key.NETWORK_ARP_TABLE_SIZE, tostring(ui.SpinCtrl_adv_ARP_TABLE_SIZE:GetValue()))
        ct:key_write(config.project.key.NETWORK_ARP_QUEUEING, tostring(ui.Choice_adv_ARP_QUEUEING:GetSelection()))
        ct:key_write(config.project.key.NETWORK_ETHARP_TRUST_IP_MAC, tostring(ui.Choice_adv_ETHARP_TRUST_IP_MAC:GetSelection()))
        ct:key_write(config.project.key.NETWORK_ETHARP_SUPPORT_VLAN, tostring(ui.Choice_adv_ETHARP_SUPPORT_VLAN:GetSelection()))
        ct:key_write(config.project.key.NETWORK_ETH_PAD_SIZE, tostring(ui.SpinCtrl_adv_ETH_PAD_SIZE:GetValue()))
        ct:key_write(config.project.key.NETWORK_ETHARP_SUPPORT_STATIC_ENTRIES, tostring(ui.Choice_adv_ETHARP_SUPPORT_STATIC_ENTRIES:GetSelection()))

        -- save adv IP options
        ct:key_write(config.project.key.NETWORK_IP_FORWARD, tostring(ui.Choice_adv_IP_FORWARD:GetSelection()))
        ct:key_write(config.project.key.NETWORK_IP_OPTIONS_ALLOWED, tostring(ui.Choice_adv_IP_OPTIONS_ALLOWED:GetSelection()))
        ct:key_write(config.project.key.NETWORK_IP_REASSEMBLY, tostring(ui.Choice_adv_IP_REASSEMBLY:GetSelection()))
        ct:key_write(config.project.key.NETWORK_IP_FRAG, tostring(ui.Choice_adv_IP_FRAG:GetSelection()))
        ct:key_write(config.project.key.NETWORK_IP_REASS_MAXAGE, tostring(ui.SpinCtrl_adv_IP_REASS_MAXAGE:GetValue()))
        ct:key_write(config.project.key.NETWORK_IP_REASS_MAX_PBUFS, tostring(ui.SpinCtrl_adv_IP_REASS_MAX_PBUFS:GetValue()))
        ct:key_write(config.project.key.NETWORK_IP_DEFAULT_TTL, tostring(ui.SpinCtrl_adv_IP_DEFAULT_TTL:GetValue()))
        ct:key_write(config.project.key.NETWORK_IP_SOF_BROADCAST, tostring(ui.Choice_adv_IP_SOF_BROADCAST:GetSelection()))
        ct:key_write(config.project.key.NETWORK_IP_SOF_BROADCAST_RECV, tostring(ui.Choice_adv_IP_SOF_BROADCAST_RECV:GetSelection()))
        ct:key_write(config.project.key.NETWORK_IP_FORWARD_ALLOW_TX_ON_RX_NETIF, tostring(ui.Choice_adv_IP_FORWARD_ALLOW_TX_ON_RX_NETIF:GetSelection()))

        -- save adv ICMP options
        ct:key_write(config.project.key.NETWORK_LWIP_ICMP, tostring(ui.Choice_adv_LWIP_ICMP:GetSelection()))
        ct:key_write(config.project.key.NETWORK_ICMP_TTL, tostring(ui.SpinCtrl_adv_ICMP_TTL:GetValue()))
        ct:key_write(config.project.key.NETWORK_LWIP_BROADCAST_PING, tostring(ui.Choice_adv_LWIP_BROADCAST_PING:GetSelection()))
        ct:key_write(config.project.key.NETWORK_LWIP_MULTICAST_PING, tostring(ui.Choice_adv_LWIP_MULTICAST_PING:GetSelection()))

        -- save adv DHCP options
        ct:key_write(config.project.key.NETWORK_LWIP_DHCP, tostring(ui.Choice_adv_LWIP_DHCP:GetSelection()))

        -- load adv AUTOIP options
        ct:key_write(config.project.key.NETWORK_LWIP_AUTOIP, tostring(ui.Choice_adv_LWIP_AUTOIP:GetSelection()))
        ct:key_write(config.project.key.NETWORK_LWIP_DHCP_AUTOIP_COOP, tostring(ui.Choice_adv_LWIP_DHCP_AUTOIP_COOP:GetSelection()))
        ct:key_write(config.project.key.NETWORK_LWIP_DHCP_AUTOIP_COOP_TRIES, tostring(ui.SpinCtrl_adv_LWIP_DHCP_AUTOIP_COOP_TRIES:GetValue()))


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
        ui.FlexGridSizer_adv_MEM.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_MEM, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

        -- MEMP_SEPARATE_POOLS
        ui.FlexGridSizer_adv_MEM:AddStaticText("MEMP_SEPARATE_POOLS")
        ui.Choice_adv_MEMP_SEPARATE_POOLS = wx.wxChoice(ui.Panel_adv_MEM, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_MEMP_SEPARATE_POOLS:Append({"No (0)", "Yes (1)"})
        ui.Choice_adv_MEMP_SEPARATE_POOLS:SetToolTip("MEMP_SEPARATE_POOLS: If selected yes, each pool is placed in its own array. "..
                                                "This can be used to individually change the location of each pool. "..
                                                "Default is one big array for all pools.")
        ui.Choice_adv_MEMP_SEPARATE_POOLS:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_MEM:Add(ui.Choice_adv_MEMP_SEPARATE_POOLS, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_OVERFLOW_CHECK
        ui.FlexGridSizer_adv_MEM:AddStaticText("MEMP_OVERFLOW_CHECK")
        ui.Choice_adv_MEMP_OVERFLOW_CHECK = wx.wxChoice(ui.Panel_adv_MEM, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_MEMP_OVERFLOW_CHECK:Append({"Disable (0)", "Each element when it is freed (1)", "Each element in every pool every time (2)"})
        ui.Choice_adv_MEMP_OVERFLOW_CHECK:SetToolTip("MEMP_OVERFLOW_CHECK: memp overflow protection reserves a configurable amount of bytes before and after each "..
                                                "memp element in every pool and fills it with a prominent default value.\n"..
                                                "- 0: no checking\n"..
                                                "- 1: checks each element when it is freed\n"..
                                                "- 2: checks each element in every pool every time memp_malloc() or memp_free() is called (useful but slow!).")
        ui.Choice_adv_MEMP_OVERFLOW_CHECK:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_MEM:Add(ui.Choice_adv_MEMP_OVERFLOW_CHECK, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_SANITY_CHECK
        ui.FlexGridSizer_adv_MEM:AddStaticText("MEMP_SANITY_CHECK")
        ui.Choice_adv_MEMP_SANITY_CHECK = wx.wxChoice(ui.Panel_adv_MEM, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_MEMP_SANITY_CHECK:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_MEMP_SANITY_CHECK:SetToolTip("MEMP_SANITY_CHECK = 1: run a sanity check after each memp_free() to make sure that there are no cycles in the linked lists.")
        ui.Choice_adv_MEMP_SANITY_CHECK:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_MEM:Add(ui.Choice_adv_MEMP_SANITY_CHECK, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
        ui.FlexGridSizer_adv_IMPS.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_IMPS, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

        -- MEMP_NUM_PBUF
        ui.FlexGridSizer_adv_IMPS:AddStaticText("MEMP_NUM_PBUF")
        ui.SpinCtrl_adv_MEMP_NUM_PBUF = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_MEMP_NUM_PBUF:SetToolTip("MEMP_NUM_PBUF: the number of memp struct pbufs. If the application "..
                                                 "sends a lot of data out of ROM (or other static memory), this "..
                                                  "should be set high.")
        ui.SpinCtrl_adv_MEMP_NUM_PBUF:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_MEMP_NUM_PBUF, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_NUM_RAW_PCB
        ui.FlexGridSizer_adv_IMPS:AddStaticText("MEMP_NUM_RAW_PCB")
        ui.SpinCtrl_adv_MEMP_NUM_RAW_PCB = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_MEMP_NUM_RAW_PCB:SetToolTip("MEMP_NUM_RAW_PCB: Number of raw connection PCBs")
        ui.SpinCtrl_adv_MEMP_NUM_RAW_PCB:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_MEMP_NUM_RAW_PCB, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_NUM_UDP_PCB
        ui.FlexGridSizer_adv_IMPS:AddStaticText("MEMP_NUM_UDP_PCB")
        ui.SpinCtrl_adv_MEMP_NUM_UDP_PCB = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_MEMP_NUM_UDP_PCB:SetToolTip("MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One per active UDP \"connection\".")
        ui.SpinCtrl_adv_MEMP_NUM_UDP_PCB:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_MEMP_NUM_UDP_PCB, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_NUM_TCP_PCB
        ui.FlexGridSizer_adv_IMPS:AddStaticText("MEMP_NUM_TCP_PCB")
        ui.SpinCtrl_adv_MEMP_NUM_TCP_PCB = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_MEMP_NUM_TCP_PCB:SetToolTip("MEMP_NUM_TCP_PCB: the number of simulatenously active TCP connections.")
        ui.SpinCtrl_adv_MEMP_NUM_TCP_PCB:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_MEMP_NUM_TCP_PCB, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_NUM_TCP_PCB_LISTEN
        ui.FlexGridSizer_adv_IMPS:AddStaticText("MEMP_NUM_TCP_PCB_LISTEN")
        ui.SpinCtrl_adv_MEMP_NUM_TCP_PCB_LISTEN = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_MEMP_NUM_TCP_PCB_LISTEN:SetToolTip("MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP connections.")
        ui.SpinCtrl_adv_MEMP_NUM_TCP_PCB_LISTEN:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_MEMP_NUM_TCP_PCB_LISTEN, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_NUM_TCP_SEG
        ui.FlexGridSizer_adv_IMPS:AddStaticText("MEMP_NUM_TCP_SEG")
        ui.SpinCtrl_adv_MEMP_NUM_TCP_SEG = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_MEMP_NUM_TCP_SEG:SetToolTip("MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP segments.")
        ui.SpinCtrl_adv_MEMP_NUM_TCP_SEG:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_MEMP_NUM_TCP_SEG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_NUM_REASSDATA
        ui.FlexGridSizer_adv_IMPS:AddStaticText("MEMP_NUM_REASSDATA")
        ui.SpinCtrl_adv_MEMP_NUM_REASSDATA = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_MEMP_NUM_REASSDATA:SetToolTip("MEMP_NUM_REASSDATA: the number of IP packets simultaneously queued for reassembly (whole packets, not fragments!)")
        ui.SpinCtrl_adv_MEMP_NUM_REASSDATA:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_MEMP_NUM_REASSDATA, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_NUM_FRAG_PBUF
        ui.FlexGridSizer_adv_IMPS:AddStaticText("MEMP_NUM_FRAG_PBUF")
        ui.SpinCtrl_adv_MEMP_NUM_FRAG_PBUF = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_MEMP_NUM_FRAG_PBUF:SetToolTip("MEMP_NUM_FRAG_PBUF: the number of IP fragments simultaneously sent (fragments, not whole packets!). "..
                                            "This is only used with IP_FRAG_USES_STATIC_BUF==0 and LWIP_NETIF_TX_SINGLE_PBUF==0 and only has to be > 1 with DMA-enabled MACs "..
                                            "where the packet is not yet sent when netif->output returns.")
        ui.SpinCtrl_adv_MEMP_NUM_FRAG_PBUF:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_MEMP_NUM_FRAG_PBUF, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_NUM_ARP_QUEUE
        ui.FlexGridSizer_adv_IMPS:AddStaticText("MEMP_NUM_ARP_QUEUE")
        ui.SpinCtrl_adv_MEMP_NUM_ARP_QUEUE = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_MEMP_NUM_ARP_QUEUE:SetToolTip("MEMP_NUM_ARP_QUEUE: the number of simulateously queued outgoing packets (pbufs) that are waiting "..
                                                           "for an ARP request (to resolve their destination address) to finish. (requires the ARP_QUEUEING option)")
        ui.SpinCtrl_adv_MEMP_NUM_ARP_QUEUE:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_MEMP_NUM_ARP_QUEUE, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_NUM_IGMP_GROUP
        ui.FlexGridSizer_adv_IMPS:AddStaticText("MEMP_NUM_IGMP_GROUP")
        ui.SpinCtrl_adv_MEMP_NUM_IGMP_GROUP = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_MEMP_NUM_IGMP_GROUP:SetToolTip("MEMP_NUM_IGMP_GROUP: The number of multicast groups whose network interfaces "..
                                                            "can be members at the same time (one per netif - allsystems group -, plus one "..
                                                            "per netif membership). (requires the LWIP_IGMP option)")
        ui.SpinCtrl_adv_MEMP_NUM_IGMP_GROUP:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_MEMP_NUM_IGMP_GROUP, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_NUM_NETBUF
        ui.FlexGridSizer_adv_IMPS:AddStaticText("MEMP_NUM_NETBUF")
        ui.SpinCtrl_adv_MEMP_NUM_NETBUF = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_MEMP_NUM_NETBUF:SetToolTip("MEMP_NUM_NETBUF: the number of struct netbufs.")
        ui.SpinCtrl_adv_MEMP_NUM_NETBUF:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_MEMP_NUM_NETBUF, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_NUM_NETCONN
        ui.FlexGridSizer_adv_IMPS:AddStaticText("MEMP_NUM_NETCONN")
        ui.SpinCtrl_adv_MEMP_NUM_NETCONN = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_MEMP_NUM_NETCONN:SetToolTip("MEMP_NUM_NETCONN: the number of struct netconns.")
        ui.SpinCtrl_adv_MEMP_NUM_NETCONN:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_MEMP_NUM_NETCONN, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_NUM_TCPIP_MSG_API
        ui.FlexGridSizer_adv_IMPS:AddStaticText("MEMP_NUM_TCPIP_MSG_API")
        ui.SpinCtrl_adv_MEMP_NUM_TCPIP_MSG_API = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_MEMP_NUM_TCPIP_MSG_API:SetToolTip("MEMP_NUM_TCPIP_MSG_API: the number of struct tcpip_msg, which are used for callback/timeout API communication.")
        ui.SpinCtrl_adv_MEMP_NUM_TCPIP_MSG_API:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_MEMP_NUM_TCPIP_MSG_API, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_NUM_TCPIP_MSG_INPKT
        ui.FlexGridSizer_adv_IMPS:AddStaticText("MEMP_NUM_TCPIP_MSG_INPKT")
        ui.SpinCtrl_adv_MEMP_NUM_TCPIP_MSG_INPKT = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_MEMP_NUM_TCPIP_MSG_INPKT:SetToolTip("MEMP_NUM_TCPIP_MSG_INPKT: the number of struct tcpip_msg, which are used for incoming packets.")
        ui.SpinCtrl_adv_MEMP_NUM_TCPIP_MSG_INPKT:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_MEMP_NUM_TCPIP_MSG_INPKT, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_NUM_SNMP_NODE
        ui.FlexGridSizer_adv_IMPS:AddStaticText("MEMP_NUM_SNMP_NODE")
        ui.SpinCtrl_adv_MEMP_NUM_SNMP_NODE = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_MEMP_NUM_SNMP_NODE:SetToolTip("MEMP_NUM_SNMP_NODE: the number of leafs in the SNMP tree.")
        ui.SpinCtrl_adv_MEMP_NUM_SNMP_NODE:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_MEMP_NUM_SNMP_NODE, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_NUM_SNMP_ROOTNODE
        ui.FlexGridSizer_adv_IMPS:AddStaticText("MEMP_NUM_SNMP_ROOTNODE")
        ui.SpinCtrl_adv_MEMP_NUM_SNMP_ROOTNODE = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_MEMP_NUM_SNMP_ROOTNODE:SetToolTip("MEMP_NUM_SNMP_ROOTNODE: the number of branches in the SNMP tree. "..
                                                               "Every branch has one leaf (MEMP_NUM_SNMP_NODE) at least!")
        ui.SpinCtrl_adv_MEMP_NUM_SNMP_ROOTNODE:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_MEMP_NUM_SNMP_ROOTNODE, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_NUM_LOCALHOSTLIST
        ui.FlexGridSizer_adv_IMPS:AddStaticText("MEMP_NUM_LOCALHOSTLIST")
        ui.SpinCtrl_adv_MEMP_NUM_LOCALHOSTLIST = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_MEMP_NUM_LOCALHOSTLIST:SetToolTip("MEMP_NUM_LOCALHOSTLIST: the number of host entries in the local host list if DNS_LOCAL_HOSTLIST_IS_DYNAMIC==1.")
        ui.SpinCtrl_adv_MEMP_NUM_LOCALHOSTLIST:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_MEMP_NUM_LOCALHOSTLIST, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_NUM_PPPOE_INTERFACES
        ui.FlexGridSizer_adv_IMPS:AddStaticText("MEMP_NUM_PPPOE_INTERFACES")
        ui.SpinCtrl_adv_MEMP_NUM_PPPOE_INTERFACES = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_MEMP_NUM_PPPOE_INTERFACES:SetToolTip("MEMP_NUM_PPPOE_INTERFACES: the number of concurrently active PPPoE interfaces (only used with PPPOE_SUPPORT==1)")
        ui.SpinCtrl_adv_MEMP_NUM_PPPOE_INTERFACES:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_MEMP_NUM_PPPOE_INTERFACES, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- PBUF_POOL_SIZE
        ui.FlexGridSizer_adv_IMPS:AddStaticText("PBUF_POOL_SIZE")
        ui.SpinCtrl_adv_PBUF_POOL_SIZE = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_PBUF_POOL_SIZE:SetToolTip("PBUF_POOL_SIZE: the number of buffers in the pbuf pool.")
        ui.SpinCtrl_adv_PBUF_POOL_SIZE:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_PBUF_POOL_SIZE, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
        ui.FlexGridSizer_adv_ARP.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_ARP, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

        -- LWIP_ARP
        ui.FlexGridSizer_adv_ARP:AddStaticText("LWIP_ARP")
        ui.Choice_adv_LWIP_ARP = wx.wxChoice(ui.Panel_adv_ARP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_LWIP_ARP:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_LWIP_ARP:SetToolTip("LWIP_ARP==1: Enable ARP functionality.")
        ui.Choice_adv_LWIP_ARP:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_ARP:Add(ui.Choice_adv_LWIP_ARP, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- ARP_TABLE_SIZE
        ui.FlexGridSizer_adv_ARP:AddStaticText("ARP_TABLE_SIZE")
        ui.SpinCtrl_adv_ARP_TABLE_SIZE = wx.wxSpinCtrl(ui.Panel_adv_ARP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_ARP_TABLE_SIZE:SetToolTip("ARP_TABLE_SIZE: Number of active MAC-IP address pairs cached.")
        ui.SpinCtrl_adv_ARP_TABLE_SIZE:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_ARP:Add(ui.SpinCtrl_adv_ARP_TABLE_SIZE, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- ARP_QUEUEING
        ui.FlexGridSizer_adv_ARP:AddStaticText("ARP_QUEUEING")
        ui.Choice_adv_ARP_QUEUEING = wx.wxChoice(ui.Panel_adv_ARP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_ARP_QUEUEING:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_ARP_QUEUEING:SetToolTip("ARP_QUEUEING==1: Multiple outgoing packets are queued during hardware address "..
                                              "resolution. By default, only the most recent packet is queued per IP address. "..
                                              "This is sufficient for most protocols and mainly reduces TCP connection "..
                                              "startup time. Set this to 1 if you know your application sends more than one "..
                                              "packet in a row to an IP address that is not in the ARP cache.")
        ui.Choice_adv_ARP_QUEUEING:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_ARP:Add(ui.Choice_adv_ARP_QUEUEING, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- ETHARP_TRUST_IP_MAC
        ui.FlexGridSizer_adv_ARP:AddStaticText("ETHARP_TRUST_IP_MAC")
        ui.Choice_adv_ETHARP_TRUST_IP_MAC = wx.wxChoice(ui.Panel_adv_ARP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_ETHARP_TRUST_IP_MAC:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_ETHARP_TRUST_IP_MAC:SetToolTip("ETHARP_TRUST_IP_MAC==1: Incoming IP packets cause the ARP table to be "..
                                                       "updated with the source MAC and IP addresses supplied in the packet. "..
                                                       "You may want to disable this if you do not trust LAN peers to have the "..
                                                       "correct addresses, or as a limited approach to attempt to handle "..
                                                       "spoofing. If disabled, lwIP will need to make a new ARP request if "..
                                                       "the peer is not already in the ARP table, adding a little latency. "..
                                                       "The peer *is* in the ARP table if it requested our address before. "..
                                                       "Also notice that this slows down input processing of every IP packet!")
        ui.Choice_adv_ETHARP_TRUST_IP_MAC:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_ARP:Add(ui.Choice_adv_ETHARP_TRUST_IP_MAC, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- ETHARP_SUPPORT_VLAN
        ui.FlexGridSizer_adv_ARP:AddStaticText("ETHARP_SUPPORT_VLAN")
        ui.Choice_adv_ETHARP_SUPPORT_VLAN = wx.wxChoice(ui.Panel_adv_ARP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_ETHARP_SUPPORT_VLAN:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_ETHARP_SUPPORT_VLAN:SetToolTip("ETHARP_SUPPORT_VLAN==1: support receiving ethernet packets with VLAN header. "..
                                                     "Additionally, you can define ETHARP_VLAN_CHECK to an u16_t VLAN ID to check. "..
                                                     "If ETHARP_VLAN_CHECK is defined, only VLAN-traffic for this VLAN is accepted. "..
                                                     "If ETHARP_VLAN_CHECK is not defined, all traffic is accepted. "..
                                                     "Alternatively, define a function/define ETHARP_VLAN_CHECK_FN(eth_hdr, vlan) "..
                                                     "that returns 1 to accept a packet or 0 to drop a packet.")
        ui.Choice_adv_ETHARP_SUPPORT_VLAN:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_ARP:Add(ui.Choice_adv_ETHARP_SUPPORT_VLAN, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- ETH_PAD_SIZE
        ui.FlexGridSizer_adv_ARP:AddStaticText("ETH_PAD_SIZE")
        ui.SpinCtrl_adv_ETH_PAD_SIZE = wx.wxSpinCtrl(ui.Panel_adv_ARP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 4)
        ui.SpinCtrl_adv_ETH_PAD_SIZE:SetToolTip("ETH_PAD_SIZE: number of bytes added before the ethernet header to ensure "..
                                                "alignment of payload after that header. Since the header is 14 bytes long, "..
                                                "without this padding e.g. addresses in the IP header will not be aligned "..
                                                "on a 32-bit boundary, so setting this to 2 can speed up 32-bit-platforms.")
        ui.SpinCtrl_adv_ETH_PAD_SIZE:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_ARP:Add(ui.SpinCtrl_adv_ETH_PAD_SIZE, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- ETHARP_SUPPORT_STATIC_ENTRIES
        ui.FlexGridSizer_adv_ARP:AddStaticText("ETHARP_SUPPORT_STATIC_ENTRIES")
        ui.Choice_adv_ETHARP_SUPPORT_STATIC_ENTRIES = wx.wxChoice(ui.Panel_adv_ARP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_ETHARP_SUPPORT_STATIC_ENTRIES:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_ETHARP_SUPPORT_STATIC_ENTRIES:SetToolTip("ETHARP_SUPPORT_STATIC_ENTRIES==1: enable code to support static ARP table "..
                                                               "entries (using etharp_add_static_entry/etharp_remove_static_entry).")
        ui.Choice_adv_ETHARP_SUPPORT_STATIC_ENTRIES:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_ARP:Add(ui.Choice_adv_ETHARP_SUPPORT_STATIC_ENTRIES, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
        ui.FlexGridSizer_adv_IP.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_IP, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

        -- IP_FORWARD
        ui.FlexGridSizer_adv_IP:AddStaticText("IP_FORWARD")
        ui.Choice_adv_IP_FORWARD = wx.wxChoice(ui.Panel_adv_IP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_IP_FORWARD:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_IP_FORWARD:SetToolTip("IP_FORWARD==1: Enables the ability to forward IP packets across network "..
                                            "interfaces. If you are going to run lwIP on a device with only one network nterface, define this to 0.")
        ui.Choice_adv_IP_FORWARD:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IP:Add(ui.Choice_adv_IP_FORWARD, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- IP_OPTIONS_ALLOWED
        ui.FlexGridSizer_adv_IP:AddStaticText("IP_OPTIONS_ALLOWED")
        ui.Choice_adv_IP_OPTIONS_ALLOWED = wx.wxChoice(ui.Panel_adv_IP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_IP_OPTIONS_ALLOWED:Append({"No (0)", "Yes (1)"})
        ui.Choice_adv_IP_OPTIONS_ALLOWED:SetToolTip("IP_OPTIONS_ALLOWED: Defines the behavior for IP options.\n"..
                                                    "IP_OPTIONS_ALLOWED==0: All packets with IP options are dropped.\n"..
                                                    "IP_OPTIONS_ALLOWED==1: IP options are allowed (but not parsed).")
        ui.Choice_adv_IP_OPTIONS_ALLOWED:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IP:Add(ui.Choice_adv_IP_OPTIONS_ALLOWED, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- IP_REASSEMBLY
        ui.FlexGridSizer_adv_IP:AddStaticText("IP_REASSEMBLY")
        ui.Choice_adv_IP_REASSEMBLY = wx.wxChoice(ui.Panel_adv_IP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_IP_REASSEMBLY:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_IP_REASSEMBLY:SetToolTip("IP_REASSEMBLY==1: Reassemble incoming fragmented IP packets. Note that "..
                                               "this option does not affect outgoing packet sizes, which can be controlled via IP_FRAG.")
        ui.Choice_adv_IP_REASSEMBLY:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IP:Add(ui.Choice_adv_IP_REASSEMBLY, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- IP_FRAG
        ui.FlexGridSizer_adv_IP:AddStaticText("IP_FRAG")
        ui.Choice_adv_IP_FRAG = wx.wxChoice(ui.Panel_adv_IP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_IP_FRAG:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_IP_FRAG:SetToolTip("IP_FRAG==1: Fragment outgoing IP packets if their size exceeds MTU. Note "..
                                         "that this option does not affect incoming packet sizes, which can be controlled via IP_REASSEMBLY.")
        ui.Choice_adv_IP_FRAG:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IP:Add(ui.Choice_adv_IP_FRAG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- IP_REASS_MAXAGE
        ui.FlexGridSizer_adv_IP:AddStaticText("IP_REASS_MAXAGE")
        ui.SpinCtrl_adv_IP_REASS_MAXAGE = wx.wxSpinCtrl(ui.Panel_adv_IP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 60)
        ui.SpinCtrl_adv_IP_REASS_MAXAGE:SetToolTip("IP_REASS_MAXAGE: Maximum time (in multiples of IP_TMR_INTERVAL - so seconds, normally) "..
                                                   "a fragmented IP packet waits for all fragments to arrive. If not all fragments arrived "..
                                                   "in this time, the whole packet is discarded.")
        ui.SpinCtrl_adv_IP_REASS_MAXAGE:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IP:Add(ui.SpinCtrl_adv_IP_REASS_MAXAGE, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- IP_REASS_MAX_PBUFS
        ui.FlexGridSizer_adv_IP:AddStaticText("IP_REASS_MAX_PBUFS")
        ui.SpinCtrl_adv_IP_REASS_MAX_PBUFS = wx.wxSpinCtrl(ui.Panel_adv_IP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_IP_REASS_MAX_PBUFS:SetToolTip("IP_REASS_MAX_PBUFS: Total maximum amount of pbufs waiting to be reassembled. "..
                                                      "Since the received pbufs are enqueued, be sure to configure "..
                                                      "PBUF_POOL_SIZE > IP_REASS_MAX_PBUFS so that the stack is still able to receive "..
                                                      "packets even if the maximum amount of fragments is enqueued for reassembly!")
        ui.SpinCtrl_adv_IP_REASS_MAX_PBUFS:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IP:Add(ui.SpinCtrl_adv_IP_REASS_MAX_PBUFS, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- IP_DEFAULT_TTL
        ui.FlexGridSizer_adv_IP:AddStaticText("IP_DEFAULT_TTL")
        ui.SpinCtrl_adv_IP_DEFAULT_TTL = wx.wxSpinCtrl(ui.Panel_adv_IP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 255)
        ui.SpinCtrl_adv_IP_DEFAULT_TTL:SetToolTip("IP_DEFAULT_TTL: Default value for Time-To-Live used by transport layers.")
        ui.SpinCtrl_adv_IP_DEFAULT_TTL:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IP:Add(ui.SpinCtrl_adv_IP_DEFAULT_TTL, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- IP_SOF_BROADCAST
        ui.FlexGridSizer_adv_IP:AddStaticText("IP_SOF_BROADCAST")
        ui.Choice_adv_IP_SOF_BROADCAST = wx.wxChoice(ui.Panel_adv_IP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_IP_SOF_BROADCAST:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_IP_SOF_BROADCAST:SetToolTip("IP_SOF_BROADCAST=1: Use the SOF_BROADCAST field to enable broadcast "..
                                                  "filter per pcb on udp and raw send operations. To enable broadcast filter "..
                                                  "on recv operations, you also have to set IP_SOF_BROADCAST_RECV=1.")
        ui.Choice_adv_IP_SOF_BROADCAST:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IP:Add(ui.Choice_adv_IP_SOF_BROADCAST, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- IP_SOF_BROADCAST_RECV
        ui.FlexGridSizer_adv_IP:AddStaticText("IP_SOF_BROADCAST_RECV")
        ui.Choice_adv_IP_SOF_BROADCAST_RECV = wx.wxChoice(ui.Panel_adv_IP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_IP_SOF_BROADCAST_RECV:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_IP_SOF_BROADCAST_RECV:SetToolTip("IP_SOF_BROADCAST_RECV (requires IP_SOF_BROADCAST=1) enable the broadcast filter on recv operations.")
        ui.Choice_adv_IP_SOF_BROADCAST_RECV:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IP:Add(ui.Choice_adv_IP_SOF_BROADCAST_RECV, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- IP_FORWARD_ALLOW_TX_ON_RX_NETIF
        ui.FlexGridSizer_adv_IP:AddStaticText("IP_FORWARD_ALLOW_TX_ON_RX_NETIF")
        ui.Choice_adv_IP_FORWARD_ALLOW_TX_ON_RX_NETIF = wx.wxChoice(ui.Panel_adv_IP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_IP_FORWARD_ALLOW_TX_ON_RX_NETIF:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_IP_FORWARD_ALLOW_TX_ON_RX_NETIF:SetToolTip("IP_FORWARD_ALLOW_TX_ON_RX_NETIF==1: allow ip_forward() to send packets back "..
                                                                 "out on the netif where it was received. This should only be used for wireless networks.\n"..
                                                                 "ATTENTION: When this is 1, make sure your netif driver correctly marks incoming "..
                                                                 "link-layer-broadcast/multicast packets as such using the corresponding pbuf flags!")
        ui.Choice_adv_IP_FORWARD_ALLOW_TX_ON_RX_NETIF:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IP:Add(ui.Choice_adv_IP_FORWARD_ALLOW_TX_ON_RX_NETIF, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
        ui.FlexGridSizer_adv_ICMP.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_ICMP, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

        -- LWIP_ICMP
        ui.FlexGridSizer_adv_ICMP:AddStaticText("LWIP_ICMP")
        ui.Choice_adv_LWIP_ICMP = wx.wxChoice(ui.Panel_adv_ICMP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_LWIP_ICMP:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_LWIP_ICMP:SetToolTip("LWIP_ICMP==1: Enable ICMP module inside the IP stack. Be careful, disable that make your product non-compliant to RFC1122.")
        ui.Choice_adv_LWIP_ICMP:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_ICMP:Add(ui.Choice_adv_LWIP_ICMP, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- ICMP_TTL
        ui.FlexGridSizer_adv_ICMP:AddStaticText("ICMP_TTL")
        ui.SpinCtrl_adv_ICMP_TTL = wx.wxSpinCtrl(ui.Panel_adv_ICMP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 255)
        ui.SpinCtrl_adv_ICMP_TTL:SetToolTip("ICMP_TTL: Default value for Time-To-Live used by ICMP packets.")
        ui.SpinCtrl_adv_ICMP_TTL:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_ICMP:Add(ui.SpinCtrl_adv_ICMP_TTL, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- LWIP_BROADCAST_PING
        ui.FlexGridSizer_adv_ICMP:AddStaticText("LWIP_BROADCAST_PING")
        ui.Choice_adv_LWIP_BROADCAST_PING = wx.wxChoice(ui.Panel_adv_ICMP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_LWIP_BROADCAST_PING:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_LWIP_BROADCAST_PING:SetToolTip("LWIP_BROADCAST_PING==1: respond to broadcast pings (default is unicast only).")
        ui.Choice_adv_LWIP_BROADCAST_PING:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_ICMP:Add(ui.Choice_adv_LWIP_BROADCAST_PING, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- LWIP_MULTICAST_PING
        ui.FlexGridSizer_adv_ICMP:AddStaticText("LWIP_MULTICAST_PING")
        ui.Choice_adv_LWIP_MULTICAST_PING = wx.wxChoice(ui.Panel_adv_ICMP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_LWIP_MULTICAST_PING:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_LWIP_MULTICAST_PING:SetToolTip("LWIP_MULTICAST_PING==1: respond to multicast pings (default is unicast only)")
        ui.Choice_adv_LWIP_MULTICAST_PING:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_ICMP:Add(ui.Choice_adv_LWIP_MULTICAST_PING, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
        ui.FlexGridSizer_adv_DHCP.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_DHCP, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

        -- LWIP_DHCP
        ui.FlexGridSizer_adv_DHCP:AddStaticText("LWIP_DHCP")
        ui.Choice_adv_LWIP_DHCP = wx.wxChoice(ui.Panel_adv_DHCP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_LWIP_DHCP:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_LWIP_DHCP:SetToolTip("LWIP_DHCP==1: Enable DHCP module.")
        ui.Choice_adv_LWIP_DHCP:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DHCP:Add(ui.Choice_adv_LWIP_DHCP, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
       ui.FlexGridSizer_adv_AUTOIP.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_AUTOIP, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

        -- LWIP_AUTOIP
        ui.FlexGridSizer_adv_AUTOIP:AddStaticText("LWIP_AUTOIP")
        ui.Choice_adv_LWIP_AUTOIP = wx.wxChoice(ui.Panel_adv_AUTOIP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_LWIP_AUTOIP:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_LWIP_AUTOIP:SetToolTip("LWIP_AUTOIP==1: Enable AUTOIP module.")
        ui.Choice_adv_LWIP_AUTOIP:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_AUTOIP:Add(ui.Choice_adv_LWIP_AUTOIP, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- LWIP_DHCP_AUTOIP_COOP
        ui.FlexGridSizer_adv_AUTOIP:AddStaticText("LWIP_DHCP_AUTOIP_COOP")
        ui.Choice_adv_LWIP_DHCP_AUTOIP_COOP = wx.wxChoice(ui.Panel_adv_AUTOIP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_LWIP_DHCP_AUTOIP_COOP:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_LWIP_DHCP_AUTOIP_COOP:SetToolTip("LWIP_DHCP_AUTOIP_COOP==1: Allow DHCP and AUTOIP to be both enabled on the same interface at the same time.")
        ui.Choice_adv_LWIP_DHCP_AUTOIP_COOP:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_AUTOIP:Add(ui.Choice_adv_LWIP_DHCP_AUTOIP_COOP, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- LWIP_DHCP_AUTOIP_COOP_TRIES
        ui.FlexGridSizer_adv_AUTOIP:AddStaticText("LWIP_DHCP_AUTOIP_COOP_TRIES")
        ui.SpinCtrl_adv_LWIP_DHCP_AUTOIP_COOP_TRIES = wx.wxSpinCtrl(ui.Panel_adv_AUTOIP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 25)
        ui.SpinCtrl_adv_LWIP_DHCP_AUTOIP_COOP_TRIES:SetToolTip("")
        ui.SpinCtrl_adv_LWIP_DHCP_AUTOIP_COOP_TRIES:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_AUTOIP:Add(ui.SpinCtrl_adv_LWIP_DHCP_AUTOIP_COOP_TRIES, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
--        ui.FlexGridSizer_adv_.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

--         -- !CH!
--         ui.FlexGridSizer_adv_:AddStaticText("!CH!")
--         ui.Choice_adv_!CH! = wx.wxChoice(ui.Panel_adv_, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
--         ui.Choice_adv_!CH!:Append({"Disable (0)", "Enable (1)"})
--         ui.Choice_adv_!CH!:SetToolTip("")
--         ui.Choice_adv_!CH!:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.Choice_adv_!CH!, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)
--
--         -- SPINCTRL
--         ui.FlexGridSizer_adv_:AddStaticText("SPINCTRL")
--         ui.SpinCtrl_adv_SPINCTRL = wx.wxSpinCtrl(ui.Panel_adv_, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 4)
--         ui.SpinCtrl_adv_SPINCTRL:SetToolTip("")
--         ui.SpinCtrl_adv_SPINCTRL:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.SpinCtrl_adv_SPINCTRL, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
--        ui.FlexGridSizer_adv_.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

--         -- !CH!
--         ui.FlexGridSizer_adv_:AddStaticText("!CH!")
--         ui.Choice_adv_!CH! = wx.wxChoice(ui.Panel_adv_, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
--         ui.Choice_adv_!CH!:Append({"Disable (0)", "Enable (1)"})
--         ui.Choice_adv_!CH!:SetToolTip("")
--         ui.Choice_adv_!CH!:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.Choice_adv_!CH!, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)
--
--         -- SPINCTRL
--         ui.FlexGridSizer_adv_:AddStaticText("SPINCTRL")
--         ui.SpinCtrl_adv_SPINCTRL = wx.wxSpinCtrl(ui.Panel_adv_, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 4)
--         ui.SpinCtrl_adv_SPINCTRL:SetToolTip("")
--         ui.SpinCtrl_adv_SPINCTRL:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.SpinCtrl_adv_SPINCTRL, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
--        ui.FlexGridSizer_adv_.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

--         -- !CH!
--         ui.FlexGridSizer_adv_:AddStaticText("!CH!")
--         ui.Choice_adv_!CH! = wx.wxChoice(ui.Panel_adv_, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
--         ui.Choice_adv_!CH!:Append({"Disable (0)", "Enable (1)"})
--         ui.Choice_adv_!CH!:SetToolTip("")
--         ui.Choice_adv_!CH!:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.Choice_adv_!CH!, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)
--
--         -- SPINCTRL
--         ui.FlexGridSizer_adv_:AddStaticText("SPINCTRL")
--         ui.SpinCtrl_adv_SPINCTRL = wx.wxSpinCtrl(ui.Panel_adv_, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 4)
--         ui.SpinCtrl_adv_SPINCTRL:SetToolTip("")
--         ui.SpinCtrl_adv_SPINCTRL:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.SpinCtrl_adv_SPINCTRL, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
--        ui.FlexGridSizer_adv_.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

--         -- !CH!
--         ui.FlexGridSizer_adv_:AddStaticText("!CH!")
--         ui.Choice_adv_!CH! = wx.wxChoice(ui.Panel_adv_, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
--         ui.Choice_adv_!CH!:Append({"Disable (0)", "Enable (1)"})
--         ui.Choice_adv_!CH!:SetToolTip("")
--         ui.Choice_adv_!CH!:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.Choice_adv_!CH!, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)
--
--         -- SPINCTRL
--         ui.FlexGridSizer_adv_:AddStaticText("SPINCTRL")
--         ui.SpinCtrl_adv_SPINCTRL = wx.wxSpinCtrl(ui.Panel_adv_, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 4)
--         ui.SpinCtrl_adv_SPINCTRL:SetToolTip("")
--         ui.SpinCtrl_adv_SPINCTRL:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.SpinCtrl_adv_SPINCTRL, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
--        ui.FlexGridSizer_adv_.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

--         -- !CH!
--         ui.FlexGridSizer_adv_:AddStaticText("!CH!")
--         ui.Choice_adv_!CH! = wx.wxChoice(ui.Panel_adv_, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
--         ui.Choice_adv_!CH!:Append({"Disable (0)", "Enable (1)"})
--         ui.Choice_adv_!CH!:SetToolTip("")
--         ui.Choice_adv_!CH!:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.Choice_adv_!CH!, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)
--
--         -- SPINCTRL
--         ui.FlexGridSizer_adv_:AddStaticText("SPINCTRL")
--         ui.SpinCtrl_adv_SPINCTRL = wx.wxSpinCtrl(ui.Panel_adv_, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 4)
--         ui.SpinCtrl_adv_SPINCTRL:SetToolTip("")
--         ui.SpinCtrl_adv_SPINCTRL:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.SpinCtrl_adv_SPINCTRL, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
--        ui.FlexGridSizer_adv_.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

--         -- !CH!
--         ui.FlexGridSizer_adv_:AddStaticText("!CH!")
--         ui.Choice_adv_!CH! = wx.wxChoice(ui.Panel_adv_, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
--         ui.Choice_adv_!CH!:Append({"Disable (0)", "Enable (1)"})
--         ui.Choice_adv_!CH!:SetToolTip("")
--         ui.Choice_adv_!CH!:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.Choice_adv_!CH!, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)
--
--         -- SPINCTRL
--         ui.FlexGridSizer_adv_:AddStaticText("SPINCTRL")
--         ui.SpinCtrl_adv_SPINCTRL = wx.wxSpinCtrl(ui.Panel_adv_, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 4)
--         ui.SpinCtrl_adv_SPINCTRL:SetToolTip("")
--         ui.SpinCtrl_adv_SPINCTRL:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.SpinCtrl_adv_SPINCTRL, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
--        ui.FlexGridSizer_adv_.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

--         -- !CH!
--         ui.FlexGridSizer_adv_:AddStaticText("!CH!")
--         ui.Choice_adv_!CH! = wx.wxChoice(ui.Panel_adv_, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
--         ui.Choice_adv_!CH!:Append({"Disable (0)", "Enable (1)"})
--         ui.Choice_adv_!CH!:SetToolTip("")
--         ui.Choice_adv_!CH!:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.Choice_adv_!CH!, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)
--
--         -- SPINCTRL
--         ui.FlexGridSizer_adv_:AddStaticText("SPINCTRL")
--         ui.SpinCtrl_adv_SPINCTRL = wx.wxSpinCtrl(ui.Panel_adv_, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 4)
--         ui.SpinCtrl_adv_SPINCTRL:SetToolTip("")
--         ui.SpinCtrl_adv_SPINCTRL:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.SpinCtrl_adv_SPINCTRL, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
--        ui.FlexGridSizer_adv_.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

--         -- !CH!
--         ui.FlexGridSizer_adv_:AddStaticText("!CH!")
--         ui.Choice_adv_!CH! = wx.wxChoice(ui.Panel_adv_, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
--         ui.Choice_adv_!CH!:Append({"Disable (0)", "Enable (1)"})
--         ui.Choice_adv_!CH!:SetToolTip("")
--         ui.Choice_adv_!CH!:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.Choice_adv_!CH!, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)
--
--         -- SPINCTRL
--         ui.FlexGridSizer_adv_:AddStaticText("SPINCTRL")
--         ui.SpinCtrl_adv_SPINCTRL = wx.wxSpinCtrl(ui.Panel_adv_, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 4)
--         ui.SpinCtrl_adv_SPINCTRL:SetToolTip("")
--         ui.SpinCtrl_adv_SPINCTRL:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.SpinCtrl_adv_SPINCTRL, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
--        ui.FlexGridSizer_adv_.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

--         -- !CH!
--         ui.FlexGridSizer_adv_:AddStaticText("!CH!")
--         ui.Choice_adv_!CH! = wx.wxChoice(ui.Panel_adv_, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
--         ui.Choice_adv_!CH!:Append({"Disable (0)", "Enable (1)"})
--         ui.Choice_adv_!CH!:SetToolTip("")
--         ui.Choice_adv_!CH!:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.Choice_adv_!CH!, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)
--
--         -- SPINCTRL
--         ui.FlexGridSizer_adv_:AddStaticText("SPINCTRL")
--         ui.SpinCtrl_adv_SPINCTRL = wx.wxSpinCtrl(ui.Panel_adv_, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 4)
--         ui.SpinCtrl_adv_SPINCTRL:SetToolTip("")
--         ui.SpinCtrl_adv_SPINCTRL:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.SpinCtrl_adv_SPINCTRL, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
--        ui.FlexGridSizer_adv_.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

--         -- !CH!
--         ui.FlexGridSizer_adv_:AddStaticText("!CH!")
--         ui.Choice_adv_!CH! = wx.wxChoice(ui.Panel_adv_, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
--         ui.Choice_adv_!CH!:Append({"Disable (0)", "Enable (1)"})
--         ui.Choice_adv_!CH!:SetToolTip("")
--         ui.Choice_adv_!CH!:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.Choice_adv_!CH!, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)
--
--         -- SPINCTRL
--         ui.FlexGridSizer_adv_:AddStaticText("SPINCTRL")
--         ui.SpinCtrl_adv_SPINCTRL = wx.wxSpinCtrl(ui.Panel_adv_, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 4)
--         ui.SpinCtrl_adv_SPINCTRL:SetToolTip("")
--         ui.SpinCtrl_adv_SPINCTRL:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.SpinCtrl_adv_SPINCTRL, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
--        ui.FlexGridSizer_adv_.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

--         -- !CH!
--         ui.FlexGridSizer_adv_:AddStaticText("!CH!")
--         ui.Choice_adv_!CH! = wx.wxChoice(ui.Panel_adv_, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
--         ui.Choice_adv_!CH!:Append({"Disable (0)", "Enable (1)"})
--         ui.Choice_adv_!CH!:SetToolTip("")
--         ui.Choice_adv_!CH!:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.Choice_adv_!CH!, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)
--
--         -- SPINCTRL
--         ui.FlexGridSizer_adv_:AddStaticText("SPINCTRL")
--         ui.SpinCtrl_adv_SPINCTRL = wx.wxSpinCtrl(ui.Panel_adv_, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 4)
--         ui.SpinCtrl_adv_SPINCTRL:SetToolTip("")
--         ui.SpinCtrl_adv_SPINCTRL:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.SpinCtrl_adv_SPINCTRL, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
--        ui.FlexGridSizer_adv_.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

--         -- !CH!
--         ui.FlexGridSizer_adv_:AddStaticText("!CH!")
--         ui.Choice_adv_!CH! = wx.wxChoice(ui.Panel_adv_, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
--         ui.Choice_adv_!CH!:Append({"Disable (0)", "Enable (1)"})
--         ui.Choice_adv_!CH!:SetToolTip("")
--         ui.Choice_adv_!CH!:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.Choice_adv_!CH!, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)
--
--         -- SPINCTRL
--         ui.FlexGridSizer_adv_:AddStaticText("SPINCTRL")
--         ui.SpinCtrl_adv_SPINCTRL = wx.wxSpinCtrl(ui.Panel_adv_, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 4)
--         ui.SpinCtrl_adv_SPINCTRL:SetToolTip("")
--         ui.SpinCtrl_adv_SPINCTRL:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.SpinCtrl_adv_SPINCTRL, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
--        ui.FlexGridSizer_adv_.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

--         -- !CH!
--         ui.FlexGridSizer_adv_:AddStaticText("!CH!")
--         ui.Choice_adv_!CH! = wx.wxChoice(ui.Panel_adv_, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
--         ui.Choice_adv_!CH!:Append({"Disable (0)", "Enable (1)"})
--         ui.Choice_adv_!CH!:SetToolTip("")
--         ui.Choice_adv_!CH!:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.Choice_adv_!CH!, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)
--
--         -- SPINCTRL
--         ui.FlexGridSizer_adv_:AddStaticText("SPINCTRL")
--         ui.SpinCtrl_adv_SPINCTRL = wx.wxSpinCtrl(ui.Panel_adv_, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 4)
--         ui.SpinCtrl_adv_SPINCTRL:SetToolTip("")
--         ui.SpinCtrl_adv_SPINCTRL:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
--         ui.FlexGridSizer_adv_:Add(ui.SpinCtrl_adv_SPINCTRL, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
