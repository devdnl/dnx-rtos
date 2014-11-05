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
require("modules/operating_system")


--==============================================================================
-- GLOBAL OBJECTS
--==============================================================================
network  = {}


--==============================================================================
-- LOCAL OBJECTS
--==============================================================================
local ui = {}
local ID = {}
local modified = ct:new_modify_indicator()
local priority_min = 0
local priority_max = 0
local function _get_value(self, idx) for i, v in pairs(self) do if v == idx then return i end end end
local function _count(self) local n = 0 for i, v in pairs(self) do if type(v) == "number" then n = n + 1 end end return n end

local TCP_OVERSIZE = {}
TCP_OVERSIZE["0"]         = 0
TCP_OVERSIZE["1"]         = 1
TCP_OVERSIZE["128"]       = 2
TCP_OVERSIZE["TCP_MSS"]   = 3
TCP_OVERSIZE["TCP_MSS/4"] = 4
TCP_OVERSIZE.get_value    = _get_value

local stack_size = {};
stack_size["STACK_DEPTH_MINIMAL"]    = 0
stack_size["STACK_DEPTH_VERY_LOW"]   = 1
stack_size["STACK_DEPTH_LOW"]        = 2
stack_size["STACK_DEPTH_MEDIUM"]     = 3
stack_size["STACK_DEPTH_LARGE"]      = 4
stack_size["STACK_DEPTH_VERY_LARGE"] = 5
stack_size["STACK_DEPTH_HUGE"]       = 6
stack_size["STACK_DEPTH_VERY_HUGE"]  = 7
stack_size.count                     = _count
stack_size.get_value                 = _get_value


local debug_level = {}
debug_level["LWIP_DBG_LEVEL_ALL"]     = 0
debug_level["LWIP_DBG_LEVEL_WARNING"] = 1
debug_level["LWIP_DBG_LEVEL_SERIOUS"] = 2
debug_level["LWIP_DBG_LEVEL_SEVERE"]  = 3
debug_level.count                     = _count
debug_level.get_value                 = _get_value

local debug_switch = {}
debug_switch["LWIP_DBG_OFF"] = false
debug_switch["LWIP_DBG_ON"]  = true
debug_switch.get_value       = _get_value

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
        for i = 1, 4 do ui.SpinCtrl_static_IP_addr[i]:SetValue(ct:key_read(config.project.key["NETWORK_IP_ADDR"..i])) end
        for i = 1, 4 do ui.SpinCtrl_static_IP_mask[i]:SetValue(ct:key_read(config.project.key["NETWORK_IP_MASK"..i])) end
        for i = 1, 4 do ui.SpinCtrl_static_IP_gw[i]:SetValue(ct:key_read(config.project.key["NETWORK_IP_GW"..i])) end
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

        -- load adv SNMP options
        ui.Choice_adv_LWIP_SNMP:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_LWIP_SNMP)))
        ui.SpinCtrl_adv_SNMP_CONCURRENT_REQUESTS:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_SNMP_CONCURRENT_REQUESTS)))
        ui.SpinCtrl_adv_SNMP_TRAP_DESTINATIONS:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_SNMP_TRAP_DESTINATIONS)))
        ui.SpinCtrl_adv_SNMP_MAX_OCTET_STRING_LEN:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_SNMP_MAX_OCTET_STRING_LEN)))
        ui.SpinCtrl_adv_SNMP_MAX_TREE_DEPTH:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_SNMP_MAX_TREE_DEPTH)))

        -- load adv IGMP options
        ui.Choice_adv_LWIP_IGMP:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_LWIP_IGMP)))

        -- load adv DNS options
        ui.Choice_adv_LWIP_DNS:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_LWIP_DNS)))
        ui.SpinCtrl_adv_DNS_TABLE_SIZE:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_DNS_TABLE_SIZE)))
        ui.SpinCtrl_adv_DNS_MAX_NAME_LENGTH:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_DNS_MAX_NAME_LENGTH)))
        ui.SpinCtrl_adv_DNS_MAX_SERVERS:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_DNS_MAX_SERVERS)))
        ui.Choice_adv_DNS_DOES_NAME_CHECK:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_DNS_DOES_NAME_CHECK)))
        ui.Choice_adv_DNS_LOCAL_HOSTLIST_IS_DYNAMIC:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_DNS_LOCAL_HOSTLIST_IS_DYNAMIC)))

        -- load adv UDP options
        ui.Choice_adv_LWIP_UDP:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_LWIP_UDP)))
        ui.SpinCtrl_adv_UDP_TTL:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_UDP_TTL)))
        ui.Choice_adv_LWIP_NETBUF_RECVINFO:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_LWIP_NETBUF_RECVINFO)))

        -- load adv TCP options
        ui.Choice_adv_LWIP_TCP:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_LWIP_TCP)))
        ui.SpinCtrl_adv_TCP_TTL:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_TCP_TTL)))
        ui.SpinCtrl_adv_TCP_MAXRTX:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_TCP_MAXRTX)))
        ui.SpinCtrl_adv_TCP_SYNMAXRTX:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_TCP_SYNMAXRTX)))
        ui.Choice_adv_TCP_QUEUE_OOSEQ:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_TCP_QUEUE_OOSEQ)))
        ui.SpinCtrl_adv_TCP_MSS:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_TCP_MSS)))
        ui.Choice_adv_TCP_CALCULATE_EFF_SEND_MSS:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_TCP_CALCULATE_EFF_SEND_MSS)))
        ui.SpinCtrl_adv_TCP_OOSEQ_MAX_BYTES:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_TCP_OOSEQ_MAX_BYTES)))
        ui.SpinCtrl_adv_TCP_OOSEQ_MAX_PBUFS:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_TCP_OOSEQ_MAX_PBUFS)))
        ui.Choice_adv_TCP_LISTEN_BACKLOG:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_TCP_LISTEN_BACKLOG)))
        ui.SpinCtrl_adv_TCP_DEFAULT_LISTEN_BACKLOG:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_TCP_DEFAULT_LISTEN_BACKLOG)))
        ui.Choice_adv_TCP_OVERSIZE:SetSelection(TCP_OVERSIZE[ct:key_read(config.project.key.NETWORK_TCP_OVERSIZE)])
        ui.Choice_adv_LWIP_TCP_TIMESTAMPS:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_LWIP_TCP_TIMESTAMPS)))

        -- load adv NETIF options
        ui.Choice_adv_LWIP_NETIF_HOSTNAME:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_LWIP_NETIF_HOSTNAME)))
        ui.Choice_adv_LWIP_NETIF_HWADDRHINT:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_LWIP_NETIF_HWADDRHINT)))
        ui.Choice_adv_LWIP_NETIF_LOOPBACK:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_LWIP_NETIF_LOOPBACK)))
        ui.SpinCtrl_adv_LWIP_LOOPBACK_MAX_PBUFS:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_LWIP_LOOPBACK_MAX_PBUFS)))

        -- load adv LOOPIF options
        ui.Choice_adv_LWIP_HAVE_LOOPIF:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_LWIP_HAVE_LOOPIF)))

        -- load adv LOOPIF options
        ui.Choice_adv_LWIP_HAVE_SLIPIF:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_LWIP_HAVE_SLIPIF)))

        -- load adv thread options
        ui.TextCtrl_adv_TCPIP_THREAD_NAME:SetValue(ct:key_read(config.project.key.NETWORK_TCPIP_THREAD_NAME):gsub('"', ""))
        ui.Choice_adv_TCPIP_THREAD_STACKSIZE:SetSelection(stack_size[ct:key_read(config.project.key.NETWORK_TCPIP_THREAD_STACKSIZE)])
        ui.SpinCtrl_adv_TCPIP_THREAD_PRIO:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_TCPIP_THREAD_PRIO)))
        ui.SpinCtrl_adv_TCPIP_MBOX_SIZE:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_TCPIP_MBOX_SIZE)))
        ui.TextCtrl_adv_SLIPIF_THREAD_NAME:SetValue(ct:key_read(config.project.key.NETWORK_SLIPIF_THREAD_NAME):gsub('"', ""))
        ui.Choice_adv_SLIPIF_THREAD_STACKSIZE:SetSelection(stack_size[ct:key_read(config.project.key.NETWORK_SLIPIF_THREAD_STACKSIZE)])
        ui.SpinCtrl_adv_SLIPIF_THREAD_PRIO:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_SLIPIF_THREAD_PRIO)))
        ui.TextCtrl_adv_PPP_THREAD_NAME:SetValue(ct:key_read(config.project.key.NETWORK_PPP_THREAD_NAME):gsub('"', ""))
        ui.Choice_adv_PPP_THREAD_STACKSIZE:SetSelection(stack_size[ct:key_read(config.project.key.NETWORK_PPP_THREAD_STACKSIZE)])
        ui.SpinCtrl_adv_PPP_THREAD_PRIO:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_PPP_THREAD_PRIO)))
        ui.TextCtrl_adv_DEFAULT_THREAD_NAME:SetValue(ct:key_read(config.project.key.NETWORK_DEFAULT_THREAD_NAME):gsub('"', ""))
        ui.Choice_adv_DEFAULT_THREAD_STACKSIZE:SetSelection(stack_size[ct:key_read(config.project.key.NETWORK_DEFAULT_THREAD_STACKSIZE)])
        ui.SpinCtrl_adv_DEFAULT_THREAD_PRIO:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_DEFAULT_THREAD_PRIO)))
        ui.SpinCtrl_adv_DEFAULT_RAW_RECVMBOX_SIZE:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_DEFAULT_RAW_RECVMBOX_SIZE)))
        ui.SpinCtrl_adv_DEFAULT_UDP_RECVMBOX_SIZE:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_DEFAULT_UDP_RECVMBOX_SIZE)))
        ui.SpinCtrl_adv_DEFAULT_TCP_RECVMBOX_SIZE:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_DEFAULT_TCP_RECVMBOX_SIZE)))
        ui.SpinCtrl_adv_DEFAULT_ACCEPTMBOX_SIZE:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_DEFAULT_ACCEPTMBOX_SIZE)))

        -- load adv Sequential layer options
        ui.Choice_adv_LWIP_TCPIP_TIMEOUT:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_LWIP_TCPIP_TIMEOUT)))

        -- load adv PPP options
        ui.Choice_adv_PPP_SUPPORT:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_PPP_SUPPORT)))
        ui.Choice_adv_PPPOE_SUPPORT:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_PPPOE_SUPPORT)))
        ui.SpinCtrl_adv_NUM_PPP:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_NUM_PPP)))
        ui.Choice_adv_PAP_SUPPORT:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_PAP_SUPPORT)))
        ui.Choice_adv_CHAP_SUPPORT:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_CHAP_SUPPORT)))
        ui.Choice_adv_MD5_SUPPORT:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_MD5_SUPPORT)))
        ui.SpinCtrl_adv_FSM_DEFTIMEOUT:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_FSM_DEFTIMEOUT)))
        ui.SpinCtrl_adv_FSM_DEFMAXTERMREQS:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_FSM_DEFMAXTERMREQS)))
        ui.SpinCtrl_adv_FSM_DEFMAXCONFREQS:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_FSM_DEFMAXCONFREQS)))
        ui.SpinCtrl_adv_FSM_DEFMAXNAKLOOPS:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_FSM_DEFMAXNAKLOOPS)))
        ui.SpinCtrl_adv_UPAP_DEFTIMEOUT:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_UPAP_DEFTIMEOUT)))
        ui.SpinCtrl_adv_UPAP_DEFREQTIME:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_UPAP_DEFREQTIME)))
        ui.SpinCtrl_adv_CHAP_DEFTIMEOUT:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_CHAP_DEFTIMEOUT)))
        ui.SpinCtrl_adv_CHAP_DEFTRANSMITS:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_CHAP_DEFTRANSMITS)))
        ui.SpinCtrl_adv_LCP_ECHOINTERVAL:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_LCP_ECHOINTERVAL)))
        ui.SpinCtrl_adv_LCP_MAXECHOFAILS:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_LCP_MAXECHOFAILS)))
        ui.SpinCtrl_adv_PPP_MAXIDLEFLAG:SetValue(tonumber(ct:key_read(config.project.key.NETWORK_PPP_MAXIDLEFLAG)))

        -- load adv checksum options
        ui.Choice_adv_LWIP_CHECKSUM_ON_COPY:SetSelection(tonumber(ct:key_read(config.project.key.NETWORK_LWIP_CHECKSUM_ON_COPY)))

        -- load adv debug options
        ui.Choice_adv_LWIP_DBG_MIN_LEVEL:SetSelection(debug_level[ct:key_read(config.project.key.NETWORK_LWIP_DBG_MIN_LEVEL)])
        ui.CheckBox_adv_LWIP_DBG_TYPES_ON:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_LWIP_DBG_TYPES_ON)])
        ui.CheckBox_adv_LOW_LEVEL_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_LOW_LEVEL_DEBUG)])
        ui.CheckBox_adv_ETHARP_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_ETHARP_DEBUG)])
        ui.CheckBox_adv_NETIF_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_NETIF_DEBUG)])
        ui.CheckBox_adv_PBUF_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_PBUF_DEBUG)])
        ui.CheckBox_adv_API_LIB_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_API_LIB_DEBUG)])
        ui.CheckBox_adv_API_MSG_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_API_MSG_DEBUG)])
        ui.CheckBox_adv_SOCKETS_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_SOCKETS_DEBUG)])
        ui.CheckBox_adv_ICMP_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_ICMP_DEBUG)])
        ui.CheckBox_adv_IGMP_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_IGMP_DEBUG)])
        ui.CheckBox_adv_INET_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_INET_DEBUG)])
        ui.CheckBox_adv_IP_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_IP_DEBUG)])
        ui.CheckBox_adv_IP_REASS_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_IP_REASS_DEBUG)])
        ui.CheckBox_adv_RAW_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_RAW_DEBUG)])
        ui.CheckBox_adv_MEM_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_MEM_DEBUG)])
        ui.CheckBox_adv_MEMP_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_MEMP_DEBUG)])
        ui.CheckBox_adv_SYS_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_SYS_DEBUG)])
        ui.CheckBox_adv_TIMERS_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_TIMERS_DEBUG)])
        ui.CheckBox_adv_TCP_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_TCP_DEBUG)])
        ui.CheckBox_adv_TCP_INPUT_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_TCP_INPUT_DEBUG)])
        ui.CheckBox_adv_TCP_FR_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_TCP_FR_DEBUG)])
        ui.CheckBox_adv_TCP_RTO_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_TCP_RTO_DEBUG)])
        ui.CheckBox_adv_TCP_CWND_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_TCP_CWND_DEBUG)])
        ui.CheckBox_adv_TCP_WND_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_TCP_WND_DEBUG)])
        ui.CheckBox_adv_TCP_OUTPUT_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_TCP_OUTPUT_DEBUG)])
        ui.CheckBox_adv_TCP_RST_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_TCP_RST_DEBUG)])
        ui.CheckBox_adv_TCP_QLEN_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_TCP_QLEN_DEBUG)])
        ui.CheckBox_adv_UDP_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_UDP_DEBUG)])
        ui.CheckBox_adv_TCPIP_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_TCPIP_DEBUG)])
        ui.CheckBox_adv_PPP_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_PPP_DEBUG)])
        ui.CheckBox_adv_SLIP_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_SLIP_DEBUG)])
        ui.CheckBox_adv_DHCP_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_DHCP_DEBUG)])
        ui.CheckBox_adv_AUTOIP_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_AUTOIP_DEBUG)])
        ui.CheckBox_adv_SNMP_MSG_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_SNMP_MSG_DEBUG)])
        ui.CheckBox_adv_SNMP_MIB_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_SNMP_MIB_DEBUG)])
        ui.CheckBox_adv_DNS_DEBUG:SetValue(debug_switch[ct:key_read(config.project.key.NETWORK_DNS_DEBUG)])

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
        for i = 1, 4 do ct:key_write(config.project.key["NETWORK_IP_ADDR"..i], tostring(ui.SpinCtrl_static_IP_addr[i]:GetValue())) end
        for i = 1, 4 do ct:key_write(config.project.key["NETWORK_IP_MASK"..i], tostring(ui.SpinCtrl_static_IP_mask[i]:GetValue())) end
        for i = 1, 4 do ct:key_write(config.project.key["NETWORK_IP_GW"..i], tostring(ui.SpinCtrl_static_IP_gw[i]:GetValue())) end
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

        -- save adv AUTOIP options
        ct:key_write(config.project.key.NETWORK_LWIP_AUTOIP, tostring(ui.Choice_adv_LWIP_AUTOIP:GetSelection()))
        ct:key_write(config.project.key.NETWORK_LWIP_DHCP_AUTOIP_COOP, tostring(ui.Choice_adv_LWIP_DHCP_AUTOIP_COOP:GetSelection()))
        ct:key_write(config.project.key.NETWORK_LWIP_DHCP_AUTOIP_COOP_TRIES, tostring(ui.SpinCtrl_adv_LWIP_DHCP_AUTOIP_COOP_TRIES:GetValue()))

        -- save adv SNMP options
        ct:key_write(config.project.key.NETWORK_LWIP_SNMP, tostring(ui.Choice_adv_LWIP_SNMP:GetSelection()))
        ct:key_write(config.project.key.NETWORK_SNMP_CONCURRENT_REQUESTS, tostring(ui.SpinCtrl_adv_SNMP_CONCURRENT_REQUESTS:GetValue()))
        ct:key_write(config.project.key.NETWORK_SNMP_TRAP_DESTINATIONS, tostring(ui.SpinCtrl_adv_SNMP_TRAP_DESTINATIONS:GetValue()))
        ct:key_write(config.project.key.NETWORK_SNMP_MAX_OCTET_STRING_LEN, tostring(ui.SpinCtrl_adv_SNMP_MAX_OCTET_STRING_LEN:GetValue()))
        ct:key_write(config.project.key.NETWORK_SNMP_MAX_TREE_DEPTH, tostring(ui.SpinCtrl_adv_SNMP_MAX_TREE_DEPTH:GetValue()))

        -- save adv DNS options
        ct:key_write(config.project.key.NETWORK_LWIP_DNS, tostring(ui.Choice_adv_LWIP_DNS:GetSelection()))
        ct:key_write(config.project.key.NETWORK_DNS_TABLE_SIZE, tostring(ui.SpinCtrl_adv_DNS_TABLE_SIZE:GetValue()))
        ct:key_write(config.project.key.NETWORK_DNS_MAX_NAME_LENGTH, tostring(ui.SpinCtrl_adv_DNS_MAX_NAME_LENGTH:GetValue()))
        ct:key_write(config.project.key.NETWORK_DNS_MAX_SERVERS, tostring(ui.SpinCtrl_adv_DNS_MAX_SERVERS:GetValue()))
        ct:key_write(config.project.key.NETWORK_DNS_DOES_NAME_CHECK, tostring(ui.Choice_adv_DNS_DOES_NAME_CHECK:GetSelection()))
        ct:key_write(config.project.key.NETWORK_DNS_LOCAL_HOSTLIST_IS_DYNAMIC, tostring(ui.Choice_adv_DNS_LOCAL_HOSTLIST_IS_DYNAMIC:GetSelection()))

        -- save adv IGMP options
        ct:key_write(config.project.key.NETWORK_LWIP_IGMP, tostring(ui.Choice_adv_LWIP_IGMP:GetSelection()))

        -- save adv UDP options
        ct:key_write(config.project.key.NETWORK_LWIP_UDP, tostring(ui.Choice_adv_LWIP_UDP:GetSelection()))
        ct:key_write(config.project.key.NETWORK_UDP_TTL, tostring(ui.SpinCtrl_adv_UDP_TTL:GetValue()))
        ct:key_write(config.project.key.NETWORK_LWIP_NETBUF_RECVINFO, tostring(ui.Choice_adv_LWIP_NETBUF_RECVINFO:GetSelection()))

        -- save adv TCP options
        ct:key_write(config.project.key.NETWORK_LWIP_TCP, tostring(ui.Choice_adv_LWIP_TCP:GetSelection()))
        ct:key_write(config.project.key.NETWORK_TCP_TTL, tostring(ui.SpinCtrl_adv_TCP_TTL:GetValue()))
        ct:key_write(config.project.key.NETWORK_TCP_MAXRTX, tostring(ui.SpinCtrl_adv_TCP_MAXRTX:GetValue()))
        ct:key_write(config.project.key.NETWORK_TCP_SYNMAXRTX, tostring(ui.SpinCtrl_adv_TCP_SYNMAXRTX:GetValue()))
        ct:key_write(config.project.key.NETWORK_TCP_QUEUE_OOSEQ, tostring(ui.Choice_adv_TCP_QUEUE_OOSEQ:GetSelection()))
        ct:key_write(config.project.key.NETWORK_TCP_MSS, tostring(ui.SpinCtrl_adv_TCP_MSS:GetValue()))
        ct:key_write(config.project.key.NETWORK_TCP_CALCULATE_EFF_SEND_MSS, tostring(ui.Choice_adv_TCP_CALCULATE_EFF_SEND_MSS:GetSelection()))
        ct:key_write(config.project.key.NETWORK_TCP_OOSEQ_MAX_BYTES, tostring(ui.SpinCtrl_adv_TCP_OOSEQ_MAX_BYTES:GetValue()))
        ct:key_write(config.project.key.NETWORK_TCP_OOSEQ_MAX_PBUFS, tostring(ui.SpinCtrl_adv_TCP_OOSEQ_MAX_PBUFS:GetValue()))
        ct:key_write(config.project.key.NETWORK_TCP_LISTEN_BACKLOG, tostring(ui.Choice_adv_TCP_LISTEN_BACKLOG:GetSelection()))
        ct:key_write(config.project.key.NETWORK_TCP_DEFAULT_LISTEN_BACKLOG, tostring(ui.SpinCtrl_adv_TCP_DEFAULT_LISTEN_BACKLOG:GetValue()))
        ct:key_write(config.project.key.NETWORK_TCP_OVERSIZE, TCP_OVERSIZE:get_value(ui.Choice_adv_TCP_OVERSIZE:GetSelection()))
        ct:key_write(config.project.key.NETWORK_LWIP_TCP_TIMESTAMPS, tostring(ui.Choice_adv_LWIP_TCP_TIMESTAMPS:GetSelection()))

        -- save adv NETIF options
        ct:key_write(config.project.key.NETWORK_LWIP_NETIF_HOSTNAME, tostring(ui.Choice_adv_LWIP_NETIF_HOSTNAME:GetSelection()))
        ct:key_write(config.project.key.NETWORK_LWIP_NETIF_HWADDRHINT, tostring(ui.Choice_adv_LWIP_NETIF_HWADDRHINT:GetSelection()))
        ct:key_write(config.project.key.NETWORK_LWIP_NETIF_LOOPBACK, tostring(ui.Choice_adv_LWIP_NETIF_LOOPBACK:GetSelection()))
        ct:key_write(config.project.key.NETWORK_LWIP_LOOPBACK_MAX_PBUFS, tostring(ui.SpinCtrl_adv_LWIP_LOOPBACK_MAX_PBUFS:GetValue()))

        -- save adv LOOPIF options
        ct:key_write(config.project.key.NETWORK_LWIP_HAVE_LOOPIF, tostring(ui.Choice_adv_LWIP_HAVE_LOOPIF:GetSelection()))

        -- save adv SLIPIF options
        ct:key_write(config.project.key.NETWORK_LWIP_HAVE_SLIPIF, tostring(ui.Choice_adv_LWIP_HAVE_SLIPIF:GetSelection()))

        -- save adv thread options
        ct:key_write(config.project.key.NETWORK_TCPIP_THREAD_NAME, '"'..ui.TextCtrl_adv_TCPIP_THREAD_NAME:GetValue()..'"')
        ct:key_write(config.project.key.NETWORK_TCPIP_THREAD_STACKSIZE, stack_size:get_value(ui.Choice_adv_TCPIP_THREAD_STACKSIZE:GetSelection()))
        ct:key_write(config.project.key.NETWORK_TCPIP_THREAD_PRIO, tostring(ui.SpinCtrl_adv_TCPIP_THREAD_PRIO:GetValue()))
        ct:key_write(config.project.key.NETWORK_TCPIP_MBOX_SIZE, tostring(ui.SpinCtrl_adv_TCPIP_MBOX_SIZE:GetValue()))
        ct:key_write(config.project.key.NETWORK_SLIPIF_THREAD_NAME, '"'..ui.TextCtrl_adv_SLIPIF_THREAD_NAME:GetValue()..'"')
        ct:key_write(config.project.key.NETWORK_SLIPIF_THREAD_STACKSIZE, stack_size:get_value(ui.Choice_adv_SLIPIF_THREAD_STACKSIZE:GetSelection()))
        ct:key_write(config.project.key.NETWORK_SLIPIF_THREAD_PRIO, tostring(ui.SpinCtrl_adv_SLIPIF_THREAD_PRIO:GetValue()))
        ct:key_write(config.project.key.NETWORK_PPP_THREAD_NAME, '"'..ui.TextCtrl_adv_PPP_THREAD_NAME:GetValue()..'"')
        ct:key_write(config.project.key.NETWORK_PPP_THREAD_STACKSIZE, stack_size:get_value(ui.Choice_adv_PPP_THREAD_STACKSIZE:GetSelection()))
        ct:key_write(config.project.key.NETWORK_PPP_THREAD_PRIO, tostring(ui.SpinCtrl_adv_PPP_THREAD_PRIO:GetValue()))
        ct:key_write(config.project.key.NETWORK_DEFAULT_THREAD_NAME, '"'..ui.TextCtrl_adv_DEFAULT_THREAD_NAME:GetValue()..'"')
        ct:key_write(config.project.key.NETWORK_DEFAULT_THREAD_STACKSIZE, stack_size:get_value(ui.Choice_adv_DEFAULT_THREAD_STACKSIZE:GetSelection()))
        ct:key_write(config.project.key.NETWORK_DEFAULT_THREAD_PRIO, tostring(ui.SpinCtrl_adv_DEFAULT_THREAD_PRIO:GetValue()))
        ct:key_write(config.project.key.NETWORK_DEFAULT_RAW_RECVMBOX_SIZE, tostring(ui.SpinCtrl_adv_DEFAULT_RAW_RECVMBOX_SIZE:GetValue()))
        ct:key_write(config.project.key.NETWORK_DEFAULT_UDP_RECVMBOX_SIZE, tostring(ui.SpinCtrl_adv_DEFAULT_UDP_RECVMBOX_SIZE:GetValue()))
        ct:key_write(config.project.key.NETWORK_DEFAULT_TCP_RECVMBOX_SIZE, tostring(ui.SpinCtrl_adv_DEFAULT_TCP_RECVMBOX_SIZE:GetValue()))
        ct:key_write(config.project.key.NETWORK_DEFAULT_ACCEPTMBOX_SIZE, tostring(ui.SpinCtrl_adv_DEFAULT_ACCEPTMBOX_SIZE:GetValue()))

        -- save adv Sequential layer options
        ct:key_write(config.project.key.NETWORK_LWIP_TCPIP_TIMEOUT, tostring(ui.Choice_adv_LWIP_TCPIP_TIMEOUT:GetSelection()))

        -- save adv PPP options
        ct:key_write(config.project.key.NETWORK_PPP_SUPPORT, tostring(ui.Choice_adv_PPP_SUPPORT:GetSelection()))
        ct:key_write(config.project.key.NETWORK_PPPOE_SUPPORT, tostring(ui.Choice_adv_PPPOE_SUPPORT:GetSelection()))
        ct:key_write(config.project.key.NETWORK_NUM_PPP, tostring(ui.SpinCtrl_adv_NUM_PPP:GetValue()))
        ct:key_write(config.project.key.NETWORK_PAP_SUPPORT, tostring(ui.Choice_adv_PAP_SUPPORT:GetSelection()))
        ct:key_write(config.project.key.NETWORK_CHAP_SUPPORT, tostring(ui.Choice_adv_CHAP_SUPPORT:GetSelection()))
        ct:key_write(config.project.key.NETWORK_MD5_SUPPORT, tostring(ui.Choice_adv_MD5_SUPPORT:GetSelection()))
        ct:key_write(config.project.key.NETWORK_FSM_DEFTIMEOUT, tostring(ui.SpinCtrl_adv_FSM_DEFTIMEOUT:GetValue()))
        ct:key_write(config.project.key.NETWORK_FSM_DEFMAXTERMREQS, tostring(ui.SpinCtrl_adv_FSM_DEFMAXTERMREQS:GetValue()))
        ct:key_write(config.project.key.NETWORK_FSM_DEFMAXCONFREQS, tostring(ui.SpinCtrl_adv_FSM_DEFMAXCONFREQS:GetValue()))
        ct:key_write(config.project.key.NETWORK_FSM_DEFMAXNAKLOOPS, tostring(ui.SpinCtrl_adv_FSM_DEFMAXNAKLOOPS:GetValue()))
        ct:key_write(config.project.key.NETWORK_UPAP_DEFTIMEOUT, tostring(ui.SpinCtrl_adv_UPAP_DEFTIMEOUT:GetValue()))
        ct:key_write(config.project.key.NETWORK_UPAP_DEFREQTIME, tostring(ui.SpinCtrl_adv_UPAP_DEFREQTIME:GetValue()))
        ct:key_write(config.project.key.NETWORK_CHAP_DEFTIMEOUT, tostring(ui.SpinCtrl_adv_CHAP_DEFTIMEOUT:GetValue()))
        ct:key_write(config.project.key.NETWORK_CHAP_DEFTRANSMITS, tostring(ui.SpinCtrl_adv_CHAP_DEFTRANSMITS:GetValue()))
        ct:key_write(config.project.key.NETWORK_LCP_ECHOINTERVAL, tostring(ui.SpinCtrl_adv_LCP_ECHOINTERVAL:GetValue()))
        ct:key_write(config.project.key.NETWORK_LCP_MAXECHOFAILS, tostring(ui.SpinCtrl_adv_LCP_MAXECHOFAILS:GetValue()))
        ct:key_write(config.project.key.NETWORK_PPP_MAXIDLEFLAG, tostring(ui.SpinCtrl_adv_PPP_MAXIDLEFLAG:GetValue()))

        -- save adv checksum options
        ct:key_write(config.project.key.NETWORK_LWIP_CHECKSUM_ON_COPY, tostring(ui.Choice_adv_LWIP_CHECKSUM_ON_COPY:GetSelection()))

        -- save adv debug options
        ct:key_write(config.project.key.NETWORK_LWIP_DBG_MIN_LEVEL, debug_level:get_value(ui.Choice_adv_LWIP_DBG_MIN_LEVEL:GetSelection()))
        ct:key_write(config.project.key.NETWORK_LWIP_DBG_TYPES_ON, debug_switch:get_value(ui.CheckBox_adv_LWIP_DBG_TYPES_ON:GetValue()))
        ct:key_write(config.project.key.NETWORK_LOW_LEVEL_DEBUG, debug_switch:get_value(ui.CheckBox_adv_LOW_LEVEL_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_ETHARP_DEBUG, debug_switch:get_value(ui.CheckBox_adv_ETHARP_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_NETIF_DEBUG, debug_switch:get_value(ui.CheckBox_adv_NETIF_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_PBUF_DEBUG, debug_switch:get_value(ui.CheckBox_adv_PBUF_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_API_LIB_DEBUG, debug_switch:get_value(ui.CheckBox_adv_API_LIB_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_API_MSG_DEBUG, debug_switch:get_value(ui.CheckBox_adv_API_MSG_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_SOCKETS_DEBUG, debug_switch:get_value(ui.CheckBox_adv_SOCKETS_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_ICMP_DEBUG, debug_switch:get_value(ui.CheckBox_adv_ICMP_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_IGMP_DEBUG, debug_switch:get_value(ui.CheckBox_adv_IGMP_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_INET_DEBUG, debug_switch:get_value(ui.CheckBox_adv_INET_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_IP_DEBUG, debug_switch:get_value(ui.CheckBox_adv_IP_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_IP_REASS_DEBUG, debug_switch:get_value(ui.CheckBox_adv_IP_REASS_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_RAW_DEBUG, debug_switch:get_value(ui.CheckBox_adv_RAW_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_MEM_DEBUG, debug_switch:get_value(ui.CheckBox_adv_MEM_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_MEMP_DEBUG, debug_switch:get_value(ui.CheckBox_adv_MEMP_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_SYS_DEBUG, debug_switch:get_value(ui.CheckBox_adv_SYS_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_TIMERS_DEBUG, debug_switch:get_value(ui.CheckBox_adv_TIMERS_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_TCP_DEBUG, debug_switch:get_value(ui.CheckBox_adv_TCP_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_TCP_INPUT_DEBUG, debug_switch:get_value(ui.CheckBox_adv_TCP_INPUT_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_TCP_FR_DEBUG, debug_switch:get_value(ui.CheckBox_adv_TCP_FR_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_TCP_RTO_DEBUG, debug_switch:get_value(ui.CheckBox_adv_TCP_RTO_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_TCP_CWND_DEBUG, debug_switch:get_value(ui.CheckBox_adv_TCP_CWND_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_TCP_WND_DEBUG, debug_switch:get_value(ui.CheckBox_adv_TCP_WND_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_TCP_OUTPUT_DEBUG, debug_switch:get_value(ui.CheckBox_adv_TCP_OUTPUT_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_TCP_RST_DEBUG, debug_switch:get_value(ui.CheckBox_adv_TCP_RST_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_TCP_QLEN_DEBUG, debug_switch:get_value(ui.CheckBox_adv_TCP_QLEN_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_UDP_DEBUG, debug_switch:get_value(ui.CheckBox_adv_UDP_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_TCPIP_DEBUG, debug_switch:get_value(ui.CheckBox_adv_TCPIP_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_PPP_DEBUG, debug_switch:get_value(ui.CheckBox_adv_PPP_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_SLIP_DEBUG, debug_switch:get_value(ui.CheckBox_adv_SLIP_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_DHCP_DEBUG, debug_switch:get_value(ui.CheckBox_adv_DHCP_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_AUTOIP_DEBUG, debug_switch:get_value(ui.CheckBox_adv_AUTOIP_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_SNMP_MSG_DEBUG, debug_switch:get_value(ui.CheckBox_adv_SNMP_MSG_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_SNMP_MIB_DEBUG, debug_switch:get_value(ui.CheckBox_adv_SNMP_MIB_DEBUG:GetValue()))
        ct:key_write(config.project.key.NETWORK_DNS_DEBUG, debug_switch:get_value(ui.CheckBox_adv_DNS_DEBUG:GetValue()))

        -- set that nothing is modified
        modified:no()
end


--------------------------------------------------------------------------------
-- @brief  Create basic options controls
-- @param  parent
-- @return Created panel with widgets
--------------------------------------------------------------------------------
local function create_basic_options_widgets(parent)
        -- create panel for basic options
        ui.Panel_basic = wx.wxPanel(parent, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTAB_TRAVERSAL)
        ui.FlexGridSizer_basic = wx.wxFlexGridSizer(0, 1, 0, 0)

        -- create staticbox sizer for MAC address
        ui.StaticBoxSizer_MAC = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_basic, "MAC address")
        ui.FlexGridSizer_MAC  = wx.wxFlexGridSizer(0, 11, 0, 0)

                -- add MAC address widgets
                ui.TextCtrl_MAC = {}
                for i = 0, 5 do
                        ui.TextCtrl_MAC[i] = wx.wxTextCtrl(ui.Panel_basic, wx.wxNewId(), "FF", wx.wxDefaultPosition, wx.wxSize(30,-1), 0, ct.hexvalidator)
                        ui.TextCtrl_MAC[i]:SetMaxLength(2)
                        ui.FlexGridSizer_MAC:Add(ui.TextCtrl_MAC[i], 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

                        if i < 5 then
                                ui.StaticText = wx.wxStaticText(ui.Panel_basic, wx.wxID_ANY, ":", wx.wxDefaultPosition, wx.wxDefaultSize)
                                ui.FlexGridSizer_MAC:Add(ui.StaticText, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                        end

                        ui.TextCtrl_MAC[i]:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
                end

                -- add MAC group to the sizer
                ui.StaticBoxSizer_MAC:Add(ui.FlexGridSizer_MAC, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)
                ui.FlexGridSizer_basic:Add(ui.StaticBoxSizer_MAC, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

        -- create group for Ethernet device path
        ui.StaticBoxSizer_path = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_basic, "Ethernet device path")

                -- create path selection combobox
                ui.ComboBox_path = wx.wxComboBox(ui.Panel_basic, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, {})
                ui.ComboBox_path:Append({"/dev/eth0", "/dev/ethmac"})
                ui.ComboBox_path:Connect(wx.wxEVT_COMMAND_COMBOBOX_SELECTED, function() modified:yes() end)
                ui.ComboBox_path:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
                ui.StaticBoxSizer_path:Add(ui.ComboBox_path, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer_basic:Add(ui.StaticBoxSizer_path, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)


        -- create IP configuration group
        ui.StaticBoxSizer_static_IP = wx.wxStaticBoxSizer(wx.wxHORIZONTAL, ui.Panel_basic, "Static IP configuration")
        ui.FlexGridSizer_static_IP = wx.wxFlexGridSizer(0, 8, 0, 0)

                -- add IP address configuration fields
                ui.FlexGridSizer_static_IP:Add(wx.wxStaticText(ui.Panel_basic, wx.wxID_ANY, "IP Address:"), 1, (wx.wxALL+wx.wxALIGN_RIGHT+wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.SpinCtrl_static_IP_addr = {}
                for i = 1, 4 do
                        ui.SpinCtrl_static_IP_addr[i] = wx.wxSpinCtrl(ui.Panel_basic, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(65,-1), 0, 0, 255)
                        ui.SpinCtrl_static_IP_addr[i]:Connect(wx.wxEVT_COMMAND_SPINCTRL_UPDATED, function() modified:yes() end)
                        ui.FlexGridSizer_static_IP:Add(ui.SpinCtrl_static_IP_addr[i], 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

                        if i < 4 then
                                ui.FlexGridSizer_static_IP:Add(wx.wxStaticText(ui.Panel_basic, wx.wxID_ANY, "."), 1, (wx.wxALL+wx.wxALIGN_CENTER_HORIZONTAL+wx.wxALIGN_CENTER_VERTICAL), 0)
                        end
                end

                -- add IP mask address configuration fields
                ui.FlexGridSizer_static_IP:Add(wx.wxStaticText(ui.Panel_basic, wx.wxID_ANY, "IP Mask:"), 1, (wx.wxALL+wx.wxALIGN_RIGHT+wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.SpinCtrl_static_IP_mask = {}
                for i = 1, 4 do
                        ui.SpinCtrl_static_IP_mask[i] = wx.wxSpinCtrl(ui.Panel_basic, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(65,-1), 0, 0, 255)
                        ui.SpinCtrl_static_IP_mask[i]:Connect(wx.wxEVT_COMMAND_SPINCTRL_UPDATED, function() modified:yes() end)
                        ui.FlexGridSizer_static_IP:Add(ui.SpinCtrl_static_IP_mask[i], 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

                        if i < 4 then
                                ui.FlexGridSizer_static_IP:Add(wx.wxStaticText(ui.Panel_basic, wx.wxID_ANY, "."), 1, (wx.wxALL+wx.wxALIGN_CENTER_HORIZONTAL+wx.wxALIGN_CENTER_VERTICAL), 0)
                        end
                end

                -- add IP gateway address configuration fields
                ui.FlexGridSizer_static_IP:Add(wx.wxStaticText(ui.Panel_basic, wx.wxID_ANY, "Gateway IP:"), 1, (wx.wxALL+wx.wxALIGN_RIGHT+wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.SpinCtrl_static_IP_gw = {}
                for i = 1, 4 do
                        ui.SpinCtrl_static_IP_gw[i] = wx.wxSpinCtrl(ui.Panel_basic, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxSize(65,-1), 0, 0, 255)
                        ui.SpinCtrl_static_IP_gw[i]:Connect(wx.wxEVT_COMMAND_SPINCTRL_UPDATED, function() modified:yes() end)
                        ui.FlexGridSizer_static_IP:Add(ui.SpinCtrl_static_IP_gw[i], 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 0)

                        if i < 4 then
                                ui.FlexGridSizer_static_IP:Add(wx.wxStaticText(ui.Panel_basic, wx.wxID_ANY, "."), 1, (wx.wxALL+wx.wxALIGN_CENTER_HORIZONTAL+wx.wxALIGN_CENTER_VERTICAL), 0)
                        end
                end

                -- add group to panel's sizer
                ui.StaticBoxSizer_static_IP:Add(ui.FlexGridSizer_static_IP, 1, bit.bor(wx.wxALL,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)
                ui.FlexGridSizer_basic:Add(ui.StaticBoxSizer_static_IP, 1, bit.bor(wx.wxALL,wx.wxEXPAND,wx.wxALIGN_CENTER_HORIZONTAL,wx.wxALIGN_CENTER_VERTICAL), 5)

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
        ui.Choice_adv_MEMP_SEPARATE_POOLS:SetToolTip("MEMP_SEPARATE_POOLS: If selected yes, each pool is placed\n"..
                                                     "in its own array. This can be used to individually change\n"..
                                                     "the location of each pool.\n"..
                                                     "Default is one big array for all pools.")
        ui.Choice_adv_MEMP_SEPARATE_POOLS:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_MEM:Add(ui.Choice_adv_MEMP_SEPARATE_POOLS, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_OVERFLOW_CHECK
        ui.FlexGridSizer_adv_MEM:AddStaticText("MEMP_OVERFLOW_CHECK")
        ui.Choice_adv_MEMP_OVERFLOW_CHECK = wx.wxChoice(ui.Panel_adv_MEM, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_MEMP_OVERFLOW_CHECK:Append({"Disable (0)", "Each element when it is freed (1)", "Each element in every pool every time (2)"})
        ui.Choice_adv_MEMP_OVERFLOW_CHECK:SetToolTip("MEMP_OVERFLOW_CHECK: memp overflow protection\n"..
                                                     "reserves a configurable amount of bytes before\n"..
                                                     "and after each memp element in every pool and\n"..
                                                     "fills it with a prominent default value.\n"..
                                                     "- 0: no checking\n"..
                                                     "- 1: checks each element when it is freed\n"..
                                                     "- 2: checks each element in every pool every time memp_malloc() or memp_free() is called (useful but slow!).")
        ui.Choice_adv_MEMP_OVERFLOW_CHECK:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_MEM:Add(ui.Choice_adv_MEMP_OVERFLOW_CHECK, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_SANITY_CHECK
        ui.FlexGridSizer_adv_MEM:AddStaticText("MEMP_SANITY_CHECK")
        ui.Choice_adv_MEMP_SANITY_CHECK = wx.wxChoice(ui.Panel_adv_MEM, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_MEMP_SANITY_CHECK:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_MEMP_SANITY_CHECK:SetToolTip("MEMP_SANITY_CHECK = 1: run a sanity check after each memp_free()\n"..
                                                   "to make sure that there are no cycles in the linked lists.")
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
        ui.SpinCtrl_adv_MEMP_NUM_PBUF:SetToolTip("MEMP_NUM_PBUF: the number of memp struct pbufs. If the application\n"..
                                                 "sends a lot of data out of ROM (or other static memory), this\n"..
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
        ui.SpinCtrl_adv_MEMP_NUM_UDP_PCB:SetToolTip("MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks.\nOne per active UDP \"connection\".")
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
        ui.SpinCtrl_adv_MEMP_NUM_REASSDATA:SetToolTip("MEMP_NUM_REASSDATA: the number of IP packets simultaneously\nqueued for reassembly (whole packets, not fragments!)")
        ui.SpinCtrl_adv_MEMP_NUM_REASSDATA:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_MEMP_NUM_REASSDATA, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_NUM_FRAG_PBUF
        ui.FlexGridSizer_adv_IMPS:AddStaticText("MEMP_NUM_FRAG_PBUF")
        ui.SpinCtrl_adv_MEMP_NUM_FRAG_PBUF = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_MEMP_NUM_FRAG_PBUF:SetToolTip("MEMP_NUM_FRAG_PBUF: the number of IP fragments\nsimultaneously sent (fragments, not whole packets!).\n"..
                                                      "This is only used with IP_FRAG_USES_STATIC_BUF==0\nand LWIP_NETIF_TX_SINGLE_PBUF==0 and only has\nto be > 1 with DMA-enabled MACs "..
                                                      "where the packet is not yet sent when\nnetif->output returns.")
        ui.SpinCtrl_adv_MEMP_NUM_FRAG_PBUF:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_MEMP_NUM_FRAG_PBUF, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_NUM_ARP_QUEUE
        ui.FlexGridSizer_adv_IMPS:AddStaticText("MEMP_NUM_ARP_QUEUE")
        ui.SpinCtrl_adv_MEMP_NUM_ARP_QUEUE = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_MEMP_NUM_ARP_QUEUE:SetToolTip("MEMP_NUM_ARP_QUEUE: the number of simulateously\nqueued outgoing packets (pbufs) that are waiting\n"..
                                                      "for an ARP request (to resolve their destination\naddress) to finish. (requires the ARP_QUEUEING option)")
        ui.SpinCtrl_adv_MEMP_NUM_ARP_QUEUE:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_MEMP_NUM_ARP_QUEUE, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_NUM_IGMP_GROUP
        ui.FlexGridSizer_adv_IMPS:AddStaticText("MEMP_NUM_IGMP_GROUP")
        ui.SpinCtrl_adv_MEMP_NUM_IGMP_GROUP = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_MEMP_NUM_IGMP_GROUP:SetToolTip("MEMP_NUM_IGMP_GROUP: The number of multicast\ngroups whose network interfaces\n"..
                                                       "can be members at the same time (one per\nnetif - allsystems group -, plus one\n"..
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
        ui.SpinCtrl_adv_MEMP_NUM_TCPIP_MSG_API:SetToolTip("MEMP_NUM_TCPIP_MSG_API: the number of struct\ntcpip_msg, which are used for callback/timeout API communication.")
        ui.SpinCtrl_adv_MEMP_NUM_TCPIP_MSG_API:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_MEMP_NUM_TCPIP_MSG_API, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_NUM_TCPIP_MSG_INPKT
        ui.FlexGridSizer_adv_IMPS:AddStaticText("MEMP_NUM_TCPIP_MSG_INPKT")
        ui.SpinCtrl_adv_MEMP_NUM_TCPIP_MSG_INPKT = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_MEMP_NUM_TCPIP_MSG_INPKT:SetToolTip("MEMP_NUM_TCPIP_MSG_INPKT: the number of struct\ntcpip_msg, which are used for incoming packets.")
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
        ui.SpinCtrl_adv_MEMP_NUM_SNMP_ROOTNODE:SetToolTip("MEMP_NUM_SNMP_ROOTNODE: the number of branches\nin the SNMP tree. "..
                                                          "Every branch has one leaf (MEMP_NUM_SNMP_NODE) at least!")
        ui.SpinCtrl_adv_MEMP_NUM_SNMP_ROOTNODE:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_MEMP_NUM_SNMP_ROOTNODE, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_NUM_LOCALHOSTLIST
        ui.FlexGridSizer_adv_IMPS:AddStaticText("MEMP_NUM_LOCALHOSTLIST")
        ui.SpinCtrl_adv_MEMP_NUM_LOCALHOSTLIST = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_MEMP_NUM_LOCALHOSTLIST:SetToolTip("MEMP_NUM_LOCALHOSTLIST: the number of host\nentries in the local host list if DNS_LOCAL_HOSTLIST_IS_DYNAMIC==1.")
        ui.SpinCtrl_adv_MEMP_NUM_LOCALHOSTLIST:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IMPS:Add(ui.SpinCtrl_adv_MEMP_NUM_LOCALHOSTLIST, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_NUM_PPPOE_INTERFACES
        ui.FlexGridSizer_adv_IMPS:AddStaticText("MEMP_NUM_PPPOE_INTERFACES")
        ui.SpinCtrl_adv_MEMP_NUM_PPPOE_INTERFACES = wx.wxSpinCtrl(ui.Panel_adv_IMPS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_MEMP_NUM_PPPOE_INTERFACES:SetToolTip("MEMP_NUM_PPPOE_INTERFACES: the number of concurrently\nactive PPPoE interfaces (only used with PPPOE_SUPPORT==1)")
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
        ui.Choice_adv_ARP_QUEUEING:SetToolTip("ARP_QUEUEING==1: Multiple outgoing packets are\nqueued during hardware address\n"..
                                              "resolution. By default, only the most recent\npacket is queued per IP address.\n"..
                                              "This is sufficient for most protocols and\nmainly reduces TCP connection\n"..
                                              "startup time. Set this to 1 if you know\nyour application sends more than one\n"..
                                              "packet in a row to an IP address that\nis not in the ARP cache.")
        ui.Choice_adv_ARP_QUEUEING:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_ARP:Add(ui.Choice_adv_ARP_QUEUEING, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- ETHARP_TRUST_IP_MAC
        ui.FlexGridSizer_adv_ARP:AddStaticText("ETHARP_TRUST_IP_MAC")
        ui.Choice_adv_ETHARP_TRUST_IP_MAC = wx.wxChoice(ui.Panel_adv_ARP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_ETHARP_TRUST_IP_MAC:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_ETHARP_TRUST_IP_MAC:SetToolTip("ETHARP_TRUST_IP_MAC==1: Incoming IP\npackets cause the ARP table to be\n"..
                                                     "updated with the source MAC and IP\naddresses supplied in the packet.\n"..
                                                     "You may want to disable this if you\ndo not trust LAN peers to have the\n"..
                                                     "correct addresses, or as a limited\napproach to attempt to handle\n"..
                                                     "spoofing. If disabled, lwIP will\nneed to make a new ARP request if\n"..
                                                     "the peer is not already in the ARP\ntable, adding a little latency.\n"..
                                                     "The peer *is* in the ARP table if\nit requested our address before.\n"..
                                                     "Also notice that this slows down\ninput processing of every IP packet!")
        ui.Choice_adv_ETHARP_TRUST_IP_MAC:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_ARP:Add(ui.Choice_adv_ETHARP_TRUST_IP_MAC, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- ETHARP_SUPPORT_VLAN
        ui.FlexGridSizer_adv_ARP:AddStaticText("ETHARP_SUPPORT_VLAN")
        ui.Choice_adv_ETHARP_SUPPORT_VLAN = wx.wxChoice(ui.Panel_adv_ARP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_ETHARP_SUPPORT_VLAN:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_ETHARP_SUPPORT_VLAN:SetToolTip("ETHARP_SUPPORT_VLAN==1: support receiving\nethernet packets with VLAN header.\n"..
                                                     "Additionally, you can define ETHARP_VLAN_CHECK\nto an u16_t VLAN ID to check.\n"..
                                                     "If ETHARP_VLAN_CHECK is defined, only\nVLAN-traffic for this VLAN is accepted.\n"..
                                                     "If ETHARP_VLAN_CHECK is not defined,\nall traffic is accepted.\n"..
                                                     "Alternatively, define a function/define\nETHARP_VLAN_CHECK_FN(eth_hdr, vlan)\n"..
                                                     "that returns 1 to accept a packet or\n0 to drop a packet.")
        ui.Choice_adv_ETHARP_SUPPORT_VLAN:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_ARP:Add(ui.Choice_adv_ETHARP_SUPPORT_VLAN, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- ETH_PAD_SIZE
        ui.FlexGridSizer_adv_ARP:AddStaticText("ETH_PAD_SIZE")
        ui.SpinCtrl_adv_ETH_PAD_SIZE = wx.wxSpinCtrl(ui.Panel_adv_ARP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 4)
        ui.SpinCtrl_adv_ETH_PAD_SIZE:SetToolTip("ETH_PAD_SIZE: number of bytes added before\nthe ethernet header to ensure\n"..
                                                "alignment of payload after that header.\nSince the header is 14 bytes long,\n"..
                                                "without this padding e.g. addresses in\nthe IP header will not be aligned\n"..
                                                "on a 32-bit boundary, so setting this to\n2 can speed up 32-bit-platforms.")
        ui.SpinCtrl_adv_ETH_PAD_SIZE:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_ARP:Add(ui.SpinCtrl_adv_ETH_PAD_SIZE, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- ETHARP_SUPPORT_STATIC_ENTRIES
        ui.FlexGridSizer_adv_ARP:AddStaticText("ETHARP_SUPPORT_STATIC_ENTRIES")
        ui.Choice_adv_ETHARP_SUPPORT_STATIC_ENTRIES = wx.wxChoice(ui.Panel_adv_ARP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_ETHARP_SUPPORT_STATIC_ENTRIES:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_ETHARP_SUPPORT_STATIC_ENTRIES:SetToolTip("ETHARP_SUPPORT_STATIC_ENTRIES==1: enable\ncode to support static ARP table\n"..
                                                               "entries (using etharp_add_static_entry/\netharp_remove_static_entry).")
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
        ui.Choice_adv_IP_FORWARD:SetToolTip("IP_FORWARD==1: Enables the ability to\nforward IP packets across network\n"..
                                            "interfaces. If you are going to run\nlwIP on a device with only one network nterface,\ndefine this to 0.")
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
        ui.Choice_adv_IP_REASSEMBLY:SetToolTip("IP_REASSEMBLY==1: Reassemble incoming\nfragmented IP packets. Note that\n"..
                                               "this option does not affect outgoing\npacket sizes, which can be controlled via IP_FRAG.")
        ui.Choice_adv_IP_REASSEMBLY:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IP:Add(ui.Choice_adv_IP_REASSEMBLY, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- IP_FRAG
        ui.FlexGridSizer_adv_IP:AddStaticText("IP_FRAG")
        ui.Choice_adv_IP_FRAG = wx.wxChoice(ui.Panel_adv_IP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_IP_FRAG:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_IP_FRAG:SetToolTip("IP_FRAG==1: Fragment outgoing IP packets\nif their size exceeds MTU. Note\n"..
                                         "that this option does not affect incoming\npacket sizes, which can be controlled via IP_REASSEMBLY.")
        ui.Choice_adv_IP_FRAG:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IP:Add(ui.Choice_adv_IP_FRAG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- IP_REASS_MAXAGE
        ui.FlexGridSizer_adv_IP:AddStaticText("IP_REASS_MAXAGE")
        ui.SpinCtrl_adv_IP_REASS_MAXAGE = wx.wxSpinCtrl(ui.Panel_adv_IP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 60)
        ui.SpinCtrl_adv_IP_REASS_MAXAGE:SetToolTip("IP_REASS_MAXAGE: Maximum time (in\nmultiples of IP_TMR_INTERVAL - so seconds, normally)\n"..
                                                   "a fragmented IP packet waits for all\nfragments to arrive. If not all fragments arrived\n"..
                                                   "in this time, the whole packet is discarded.")
        ui.SpinCtrl_adv_IP_REASS_MAXAGE:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IP:Add(ui.SpinCtrl_adv_IP_REASS_MAXAGE, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- IP_REASS_MAX_PBUFS
        ui.FlexGridSizer_adv_IP:AddStaticText("IP_REASS_MAX_PBUFS")
        ui.SpinCtrl_adv_IP_REASS_MAX_PBUFS = wx.wxSpinCtrl(ui.Panel_adv_IP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_IP_REASS_MAX_PBUFS:SetToolTip("IP_REASS_MAX_PBUFS: Total maximum\namount of pbufs waiting to be reassembled.\n"..
                                                      "Since the received pbufs are enqueued,\nbe sure to configure\n"..
                                                      "PBUF_POOL_SIZE > IP_REASS_MAX_PBUFS\nso that the stack is still able to receive\n"..
                                                      "packets even if the maximum amount\nof fragments is enqueued for reassembly!")
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
        ui.Choice_adv_IP_SOF_BROADCAST:SetToolTip("IP_SOF_BROADCAST=1: Use the SOF_BROADCAST\nfield to enable broadcast\n"..
                                                  "filter per pcb on udp and raw send\noperations. To enable broadcast filter\n"..
                                                  "on recv operations, you also have\nto set IP_SOF_BROADCAST_RECV=1.")
        ui.Choice_adv_IP_SOF_BROADCAST:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IP:Add(ui.Choice_adv_IP_SOF_BROADCAST, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- IP_SOF_BROADCAST_RECV
        ui.FlexGridSizer_adv_IP:AddStaticText("IP_SOF_BROADCAST_RECV")
        ui.Choice_adv_IP_SOF_BROADCAST_RECV = wx.wxChoice(ui.Panel_adv_IP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_IP_SOF_BROADCAST_RECV:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_IP_SOF_BROADCAST_RECV:SetToolTip("IP_SOF_BROADCAST_RECV (requires IP_SOF_BROADCAST=1)\nenable the broadcast filter on recv operations.")
        ui.Choice_adv_IP_SOF_BROADCAST_RECV:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IP:Add(ui.Choice_adv_IP_SOF_BROADCAST_RECV, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- IP_FORWARD_ALLOW_TX_ON_RX_NETIF
        ui.FlexGridSizer_adv_IP:AddStaticText("IP_FORWARD_ALLOW_TX_ON_RX_NETIF")
        ui.Choice_adv_IP_FORWARD_ALLOW_TX_ON_RX_NETIF = wx.wxChoice(ui.Panel_adv_IP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_IP_FORWARD_ALLOW_TX_ON_RX_NETIF:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_IP_FORWARD_ALLOW_TX_ON_RX_NETIF:SetToolTip("IP_FORWARD_ALLOW_TX_ON_RX_NETIF==1: allow\nip_forward() to send packets back\n"..
                                                                 "out on the netif where it was received.\nThis should only be used for wireless networks.\n"..
                                                                 "ATTENTION: When this is 1, make sure your\nnetif driver correctly marks incoming\n"..
                                                                 "link-layer-broadcast/multicast packets\nas such using the corresponding pbuf flags!")
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
        ui.Choice_adv_LWIP_ICMP:SetToolTip("LWIP_ICMP==1: Enable ICMP module inside the IP stack.\nBe careful, disable that make your product non-compliant to RFC1122.")
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
        ui.Choice_adv_LWIP_DHCP_AUTOIP_COOP:SetToolTip("LWIP_DHCP_AUTOIP_COOP==1: Allow DHCP and AUTOIP\nto be both enabled on the same interface at the same time.")
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
       ui.FlexGridSizer_adv_SNMP.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_SNMP, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

        -- LWIP_SNMP
        ui.FlexGridSizer_adv_SNMP:AddStaticText("LWIP_SNMP")
        ui.Choice_adv_LWIP_SNMP = wx.wxChoice(ui.Panel_adv_SNMP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_LWIP_SNMP:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_LWIP_SNMP:SetToolTip("LWIP_SNMP==1: Turn on SNMP module. UDP must be available for SNMP transport.")
        ui.Choice_adv_LWIP_SNMP:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_SNMP:Add(ui.Choice_adv_LWIP_SNMP, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- SNMP_CONCURRENT_REQUESTS
        ui.FlexGridSizer_adv_SNMP:AddStaticText("SNMP_CONCURRENT_REQUESTS")
        ui.SpinCtrl_adv_SNMP_CONCURRENT_REQUESTS = wx.wxSpinCtrl(ui.Panel_adv_SNMP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 4)
        ui.SpinCtrl_adv_SNMP_CONCURRENT_REQUESTS:SetToolTip("SNMP_CONCURRENT_REQUESTS: Number of concurrent\nrequests the module will allow.\nAt least one request buffer is required.\n"..
                                                            "Does not have to be changed unless external\nMIBs answer request asynchronously.")
        ui.SpinCtrl_adv_SNMP_CONCURRENT_REQUESTS:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_SNMP:Add(ui.SpinCtrl_adv_SNMP_CONCURRENT_REQUESTS, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- SNMP_TRAP_DESTINATIONS
        ui.FlexGridSizer_adv_SNMP:AddStaticText("SNMP_TRAP_DESTINATIONS")
        ui.SpinCtrl_adv_SNMP_TRAP_DESTINATIONS = wx.wxSpinCtrl(ui.Panel_adv_SNMP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_SNMP_TRAP_DESTINATIONS:SetToolTip("SNMP_TRAP_DESTINATIONS: Number of trap destinations.\nAt least one trap destination is required.")
        ui.SpinCtrl_adv_SNMP_TRAP_DESTINATIONS:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_SNMP:Add(ui.SpinCtrl_adv_SNMP_TRAP_DESTINATIONS, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- SNMP_MAX_OCTET_STRING_LEN
        ui.FlexGridSizer_adv_SNMP:AddStaticText("SNMP_MAX_OCTET_STRING_LEN")
        ui.SpinCtrl_adv_SNMP_MAX_OCTET_STRING_LEN = wx.wxSpinCtrl(ui.Panel_adv_SNMP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 16, 256)
        ui.SpinCtrl_adv_SNMP_MAX_OCTET_STRING_LEN:SetToolTip("The maximum length of strings used.\nThis affects the size of MEMP_SNMP_VALUE elements.")
        ui.SpinCtrl_adv_SNMP_MAX_OCTET_STRING_LEN:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_SNMP:Add(ui.SpinCtrl_adv_SNMP_MAX_OCTET_STRING_LEN, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- SNMP_MAX_TREE_DEPTH
        ui.FlexGridSizer_adv_SNMP:AddStaticText("SNMP_MAX_TREE_DEPTH")
        ui.SpinCtrl_adv_SNMP_MAX_TREE_DEPTH = wx.wxSpinCtrl(ui.Panel_adv_SNMP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_SNMP_MAX_TREE_DEPTH:SetToolTip("The maximum depth of the SNMP tree.\nWith private MIBs enabled, this depends on your MIB!\n"..
                                                       "This affects the size of MEMP_SNMP_VALUE elements.")
        ui.SpinCtrl_adv_SNMP_MAX_TREE_DEPTH:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_SNMP:Add(ui.SpinCtrl_adv_SNMP_MAX_TREE_DEPTH, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
        ui.FlexGridSizer_adv_IGMP.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_IGMP, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

        -- LWIP_IGMP
        ui.FlexGridSizer_adv_IGMP:AddStaticText("LWIP_IGMP")
        ui.Choice_adv_LWIP_IGMP = wx.wxChoice(ui.Panel_adv_IGMP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_LWIP_IGMP:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_LWIP_IGMP:SetToolTip("LWIP_IGMP==1: Turn on IGMP module.")
        ui.Choice_adv_LWIP_IGMP:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_IGMP:Add(ui.Choice_adv_LWIP_IGMP, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
        ui.FlexGridSizer_adv_DNS.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_DNS, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

        -- LWIP_DNS
        ui.FlexGridSizer_adv_DNS:AddStaticText("LWIP_DNS")
        ui.Choice_adv_LWIP_DNS = wx.wxChoice(ui.Panel_adv_DNS, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_LWIP_DNS:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_LWIP_DNS:SetToolTip("LWIP_DNS==1: Turn on DNS module.\nUDP must be available for DNS transport.")
        ui.Choice_adv_LWIP_DNS:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DNS:Add(ui.Choice_adv_LWIP_DNS, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- DNS_TABLE_SIZE
        ui.FlexGridSizer_adv_DNS:AddStaticText("DNS_TABLE_SIZE")
        ui.SpinCtrl_adv_DNS_TABLE_SIZE = wx.wxSpinCtrl(ui.Panel_adv_DNS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 16)
        ui.SpinCtrl_adv_DNS_TABLE_SIZE:SetToolTip("DNS maximum number of entries to maintain locally.")
        ui.SpinCtrl_adv_DNS_TABLE_SIZE:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DNS:Add(ui.SpinCtrl_adv_DNS_TABLE_SIZE, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- DNS_MAX_NAME_LENGTH
        ui.FlexGridSizer_adv_DNS:AddStaticText("DNS_MAX_NAME_LENGTH")
        ui.SpinCtrl_adv_DNS_MAX_NAME_LENGTH = wx.wxSpinCtrl(ui.Panel_adv_DNS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 16, 256)
        ui.SpinCtrl_adv_DNS_MAX_NAME_LENGTH:SetToolTip("DNS maximum host name length supported in the name table.")
        ui.SpinCtrl_adv_DNS_MAX_NAME_LENGTH:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DNS:Add(ui.SpinCtrl_adv_DNS_MAX_NAME_LENGTH, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- DNS_MAX_SERVERS
        ui.FlexGridSizer_adv_DNS:AddStaticText("DNS_MAX_SERVERS")
        ui.SpinCtrl_adv_DNS_MAX_SERVERS = wx.wxSpinCtrl(ui.Panel_adv_DNS, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 8)
        ui.SpinCtrl_adv_DNS_MAX_SERVERS:SetToolTip("The maximum of DNS servers.")
        ui.SpinCtrl_adv_DNS_MAX_SERVERS:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DNS:Add(ui.SpinCtrl_adv_DNS_MAX_SERVERS, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- DNS_DOES_NAME_CHECK
        ui.FlexGridSizer_adv_DNS:AddStaticText("DNS_DOES_NAME_CHECK")
        ui.Choice_adv_DNS_DOES_NAME_CHECK = wx.wxChoice(ui.Panel_adv_DNS, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_DNS_DOES_NAME_CHECK:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_DNS_DOES_NAME_CHECK:SetToolTip("DNS do a name checking between the query and the response.")
        ui.Choice_adv_DNS_DOES_NAME_CHECK:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DNS:Add(ui.Choice_adv_DNS_DOES_NAME_CHECK, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- DNS_LOCAL_HOSTLIST_IS_DYNAMIC
        ui.FlexGridSizer_adv_DNS:AddStaticText("DNS_LOCAL_HOSTLIST_IS_DYNAMIC")
        ui.Choice_adv_DNS_LOCAL_HOSTLIST_IS_DYNAMIC = wx.wxChoice(ui.Panel_adv_DNS, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_DNS_LOCAL_HOSTLIST_IS_DYNAMIC:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_DNS_LOCAL_HOSTLIST_IS_DYNAMIC:SetToolTip("If this is turned on, the local host-list\ncan be dynamically changed at runtime.")
        ui.Choice_adv_DNS_LOCAL_HOSTLIST_IS_DYNAMIC:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DNS:Add(ui.Choice_adv_DNS_LOCAL_HOSTLIST_IS_DYNAMIC, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
        ui.FlexGridSizer_adv_UDP.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_UDP, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

        -- LWIP_UDP
        ui.FlexGridSizer_adv_UDP:AddStaticText("LWIP_UDP")
        ui.Choice_adv_LWIP_UDP = wx.wxChoice(ui.Panel_adv_UDP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_LWIP_UDP:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_LWIP_UDP:SetToolTip("LWIP_UDP==1: Turn on UDP.")
        ui.Choice_adv_LWIP_UDP:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_UDP:Add(ui.Choice_adv_LWIP_UDP, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- UDP_TTL
        ui.FlexGridSizer_adv_UDP:AddStaticText("UDP_TTL")
        ui.SpinCtrl_adv_UDP_TTL = wx.wxSpinCtrl(ui.Panel_adv_UDP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 255)
        ui.SpinCtrl_adv_UDP_TTL:SetToolTip("UDP_TTL: Default Time-To-Live value.")
        ui.SpinCtrl_adv_UDP_TTL:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_UDP:Add(ui.SpinCtrl_adv_UDP_TTL, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- LWIP_NETBUF_RECVINFO
        ui.FlexGridSizer_adv_UDP:AddStaticText("LWIP_NETBUF_RECVINFO")
        ui.Choice_adv_LWIP_NETBUF_RECVINFO = wx.wxChoice(ui.Panel_adv_UDP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_LWIP_NETBUF_RECVINFO:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_LWIP_NETBUF_RECVINFO:SetToolTip("LWIP_NETBUF_RECVINFO==1: append destination\naddr and port to every netbuf.")
        ui.Choice_adv_LWIP_NETBUF_RECVINFO:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_UDP:Add(ui.Choice_adv_LWIP_NETBUF_RECVINFO, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
        ui.FlexGridSizer_adv_TCP.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_TCP, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

        -- LWIP_TCP
        ui.FlexGridSizer_adv_TCP:AddStaticText("LWIP_TCP")
        ui.Choice_adv_LWIP_TCP = wx.wxChoice(ui.Panel_adv_TCP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_LWIP_TCP:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_LWIP_TCP:SetToolTip("LWIP_TCP==1: Turn on TCP.")
        ui.Choice_adv_LWIP_TCP:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_TCP:Add(ui.Choice_adv_LWIP_TCP, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TCP_TTL
        ui.FlexGridSizer_adv_TCP:AddStaticText("TCP_TTL")
        ui.SpinCtrl_adv_TCP_TTL = wx.wxSpinCtrl(ui.Panel_adv_TCP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 255)
        ui.SpinCtrl_adv_TCP_TTL:SetToolTip("TCP_TTL: Default Time-To-Live value.")
        ui.SpinCtrl_adv_TCP_TTL:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_TCP:Add(ui.SpinCtrl_adv_TCP_TTL, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TCP_MAXRTX
        ui.FlexGridSizer_adv_TCP:AddStaticText("TCP_MAXRTX")
        ui.SpinCtrl_adv_TCP_MAXRTX = wx.wxSpinCtrl(ui.Panel_adv_TCP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 50)
        ui.SpinCtrl_adv_TCP_MAXRTX:SetToolTip("TCP_MAXRTX: Maximum number of retransmissions of data segments.")
        ui.SpinCtrl_adv_TCP_MAXRTX:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_TCP:Add(ui.SpinCtrl_adv_TCP_MAXRTX, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TCP_SYNMAXRTX
        ui.FlexGridSizer_adv_TCP:AddStaticText("TCP_SYNMAXRTX")
        ui.SpinCtrl_adv_TCP_SYNMAXRTX = wx.wxSpinCtrl(ui.Panel_adv_TCP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 50)
        ui.SpinCtrl_adv_TCP_SYNMAXRTX:SetToolTip("TCP_SYNMAXRTX: Maximum number of retransmissions of SYN segments.")
        ui.SpinCtrl_adv_TCP_SYNMAXRTX:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_TCP:Add(ui.SpinCtrl_adv_TCP_SYNMAXRTX, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TCP_QUEUE_OOSEQ
        ui.FlexGridSizer_adv_TCP:AddStaticText("TCP_QUEUE_OOSEQ")
        ui.Choice_adv_TCP_QUEUE_OOSEQ = wx.wxChoice(ui.Panel_adv_TCP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_TCP_QUEUE_OOSEQ:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_TCP_QUEUE_OOSEQ:SetToolTip("Controls if TCP should queue segments that\narrive out of order. Define to 0 if your device is low on memory.")
        ui.Choice_adv_TCP_QUEUE_OOSEQ:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_TCP:Add(ui.Choice_adv_TCP_QUEUE_OOSEQ, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TCP_MSS
        ui.FlexGridSizer_adv_TCP:AddStaticText("TCP_MSS")
        ui.SpinCtrl_adv_TCP_MSS = wx.wxSpinCtrl(ui.Panel_adv_TCP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 16, 65536)
        ui.SpinCtrl_adv_TCP_MSS:SetToolTip("TCP Maximum segment size.")
        ui.SpinCtrl_adv_TCP_MSS:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_TCP:Add(ui.SpinCtrl_adv_TCP_MSS, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TCP_CALCULATE_EFF_SEND_MSS
        ui.FlexGridSizer_adv_TCP:AddStaticText("TCP_CALCULATE_EFF_SEND_MSS")
        ui.Choice_adv_TCP_CALCULATE_EFF_SEND_MSS = wx.wxChoice(ui.Panel_adv_TCP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_TCP_CALCULATE_EFF_SEND_MSS:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_TCP_CALCULATE_EFF_SEND_MSS:SetToolTip("TCP_CALCULATE_EFF_SEND_MSS: \"The maximum size of a segment that TCP really\n"..
                                                            "sends, the 'effective send MSS,' MUST be the smaller of the send MSS (which\n"..
                                                            "reflects the available reassembly buffer size at the remote host) and the\n"..
                                                            "largest size permitted by the IP layer\" (RFC 1122)\n"..
                                                            "Setting this to 1 enables code that checks TCP_MSS against the MTU of the\n"..
                                                            "netif used for a connection and limits the MSS if it would be too big otherwise.")
        ui.Choice_adv_TCP_CALCULATE_EFF_SEND_MSS:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_TCP:Add(ui.Choice_adv_TCP_CALCULATE_EFF_SEND_MSS, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TCP_OOSEQ_MAX_BYTES
        ui.FlexGridSizer_adv_TCP:AddStaticText("TCP_OOSEQ_MAX_BYTES")
        ui.SpinCtrl_adv_TCP_OOSEQ_MAX_BYTES = wx.wxSpinCtrl(ui.Panel_adv_TCP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 65536)
        ui.SpinCtrl_adv_TCP_OOSEQ_MAX_BYTES:SetToolTip("TCP_OOSEQ_MAX_BYTES: The maximum number of bytes queued on ooseq per pcb.\n"..
                                                       "Default is 0 (no limit). Only valid for TCP_QUEUE_OOSEQ==0.")
        ui.SpinCtrl_adv_TCP_OOSEQ_MAX_BYTES:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_TCP:Add(ui.SpinCtrl_adv_TCP_OOSEQ_MAX_BYTES, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TCP_OOSEQ_MAX_PBUFS
        ui.FlexGridSizer_adv_TCP:AddStaticText("TCP_OOSEQ_MAX_PBUFS")
        ui.SpinCtrl_adv_TCP_OOSEQ_MAX_PBUFS = wx.wxSpinCtrl(ui.Panel_adv_TCP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 65536)
        ui.SpinCtrl_adv_TCP_OOSEQ_MAX_PBUFS:SetToolTip("TCP_OOSEQ_MAX_PBUFS: The maximum number of pbufs queued on ooseq per pcb.\n"..
                                                       "Default is 0 (no limit). Only valid for TCP_QUEUE_OOSEQ==0.")
        ui.SpinCtrl_adv_TCP_OOSEQ_MAX_PBUFS:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_TCP:Add(ui.SpinCtrl_adv_TCP_OOSEQ_MAX_PBUFS, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TCP_LISTEN_BACKLOG
        ui.FlexGridSizer_adv_TCP:AddStaticText("TCP_LISTEN_BACKLOG")
        ui.Choice_adv_TCP_LISTEN_BACKLOG = wx.wxChoice(ui.Panel_adv_TCP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_TCP_LISTEN_BACKLOG:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_TCP_LISTEN_BACKLOG:SetToolTip("TCP_LISTEN_BACKLOG==1: Enable the backlog option for tcp listen pcb.")
        ui.Choice_adv_TCP_LISTEN_BACKLOG:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_TCP:Add(ui.Choice_adv_TCP_LISTEN_BACKLOG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TCP_DEFAULT_LISTEN_BACKLOG
        ui.FlexGridSizer_adv_TCP:AddStaticText("TCP_DEFAULT_LISTEN_BACKLOG")
        ui.SpinCtrl_adv_TCP_DEFAULT_LISTEN_BACKLOG = wx.wxSpinCtrl(ui.Panel_adv_TCP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 255)
        ui.SpinCtrl_adv_TCP_DEFAULT_LISTEN_BACKLOG:SetToolTip("The maximum allowed backlog for TCP listen netconns.\n"..
                                                              "This backlog is used unless another is explicitly specified. 255 is the maximum (u8_t).")
        ui.SpinCtrl_adv_TCP_DEFAULT_LISTEN_BACKLOG:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_TCP:Add(ui.SpinCtrl_adv_TCP_DEFAULT_LISTEN_BACKLOG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TCP_OVERSIZE
        ui.FlexGridSizer_adv_TCP:AddStaticText("TCP_OVERSIZE")
        ui.Choice_adv_TCP_OVERSIZE = wx.wxChoice(ui.Panel_adv_TCP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_TCP_OVERSIZE:Append({"0", "1", "128", "TCP_MSS", "TCP_MSS/4"})
        ui.Choice_adv_TCP_OVERSIZE:SetToolTip("TCP_OVERSIZE: The maximum number of bytes that tcp_write may\n"..
                                              "allocate ahead of time in an attempt to create shorter pbuf chains\n"..
                                              "for transmission. The meaningful range is 0 to TCP_MSS. Some\n"..
                                              "suggested values are:\n"..
                                              "0:         Disable oversized allocation. Each tcp_write() allocates a new pbuf (old behaviour).\n"..
                                              "1:         Allocate size-aligned pbufs with minimal excess. Use this if your scatter-gather DMA requires aligned fragments.\n"..
                                              "128:       Limit the pbuf/memory overhead to 20%.\n"..
                                              "TCP_MSS:   Try to create unfragmented TCP packets.\n"..
                                              "TCP_MSS/4: Try to create 4 fragments or less per TCP packet.")
        ui.Choice_adv_TCP_OVERSIZE:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_TCP:Add(ui.Choice_adv_TCP_OVERSIZE, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- LWIP_TCP_TIMESTAMPS
        ui.FlexGridSizer_adv_TCP:AddStaticText("LWIP_TCP_TIMESTAMPS")
        ui.Choice_adv_LWIP_TCP_TIMESTAMPS = wx.wxChoice(ui.Panel_adv_TCP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_LWIP_TCP_TIMESTAMPS:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_LWIP_TCP_TIMESTAMPS:SetToolTip("LWIP_TCP_TIMESTAMPS==1: support the TCP timestamp option.")
        ui.Choice_adv_LWIP_TCP_TIMESTAMPS:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_TCP:Add(ui.Choice_adv_LWIP_TCP_TIMESTAMPS, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
        ui.FlexGridSizer_adv_NETIF.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_NETIF, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

        -- LWIP_NETIF_HOSTNAME
        ui.FlexGridSizer_adv_NETIF:AddStaticText("LWIP_NETIF_HOSTNAME")
        ui.Choice_adv_LWIP_NETIF_HOSTNAME = wx.wxChoice(ui.Panel_adv_NETIF, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_LWIP_NETIF_HOSTNAME:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_LWIP_NETIF_HOSTNAME:SetToolTip("LWIP_NETIF_HOSTNAME==1: use DHCP_OPTION_HOSTNAME with netif's hostname field.")
        ui.Choice_adv_LWIP_NETIF_HOSTNAME:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_NETIF:Add(ui.Choice_adv_LWIP_NETIF_HOSTNAME, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- LWIP_NETIF_HWADDRHINT
        ui.FlexGridSizer_adv_NETIF:AddStaticText("LWIP_NETIF_HWADDRHINT")
        ui.Choice_adv_LWIP_NETIF_HWADDRHINT = wx.wxChoice(ui.Panel_adv_NETIF, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_LWIP_NETIF_HWADDRHINT:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_LWIP_NETIF_HWADDRHINT:SetToolTip("LWIP_NETIF_HWADDRHINT==1: Cache link-layer-address hints (e.g. table\n"..
                                                       "indices) in struct netif. TCP and UDP can make use of this to prevent\n"..
                                                       "scanning the ARP table for every sent packet. While this is faster for big\n"..
                                                       "ARP tables or many concurrent connections, it might be counterproductive\n"..
                                                       "if you have a tiny ARP table or if there never are concurrent connections.")
        ui.Choice_adv_LWIP_NETIF_HWADDRHINT:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_NETIF:Add(ui.Choice_adv_LWIP_NETIF_HWADDRHINT, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- LWIP_NETIF_LOOPBACK
        ui.FlexGridSizer_adv_NETIF:AddStaticText("LWIP_NETIF_LOOPBACK")
        ui.Choice_adv_LWIP_NETIF_LOOPBACK = wx.wxChoice(ui.Panel_adv_NETIF, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_LWIP_NETIF_LOOPBACK:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_LWIP_NETIF_LOOPBACK:SetToolTip("LWIP_NETIF_LOOPBACK==1: Support sending packets with a destination IP\n"..
                                                     "address equal to the netif IP address, looping them back up the stack.")
        ui.Choice_adv_LWIP_NETIF_LOOPBACK:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_NETIF:Add(ui.Choice_adv_LWIP_NETIF_LOOPBACK, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- LWIP_LOOPBACK_MAX_PBUFS
        ui.FlexGridSizer_adv_NETIF:AddStaticText("LWIP_LOOPBACK_MAX_PBUFS")
        ui.SpinCtrl_adv_LWIP_LOOPBACK_MAX_PBUFS = wx.wxSpinCtrl(ui.Panel_adv_NETIF, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 100)
        ui.SpinCtrl_adv_LWIP_LOOPBACK_MAX_PBUFS:SetToolTip("LWIP_LOOPBACK_MAX_PBUFS: Maximum number of pbufs\non queue for loopback sending for each netif (0 = disabled).")
        ui.SpinCtrl_adv_LWIP_LOOPBACK_MAX_PBUFS:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_NETIF:Add(ui.SpinCtrl_adv_LWIP_LOOPBACK_MAX_PBUFS, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
        ui.FlexGridSizer_adv_LOOPIF.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_LOOPIF, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

        -- LWIP_HAVE_LOOPIF
        ui.FlexGridSizer_adv_LOOPIF:AddStaticText("LWIP_HAVE_LOOPIF")
        ui.Choice_adv_LWIP_HAVE_LOOPIF = wx.wxChoice(ui.Panel_adv_LOOPIF, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_LWIP_HAVE_LOOPIF:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_LWIP_HAVE_LOOPIF:SetToolTip("LWIP_HAVE_LOOPIF==1: Support loop interface (127.0.0.1).")
        ui.Choice_adv_LWIP_HAVE_LOOPIF:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_LOOPIF:Add(ui.Choice_adv_LWIP_HAVE_LOOPIF, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
        ui.FlexGridSizer_adv_SLIPIF.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_SLIPIF, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

        -- LWIP_HAVE_SLIPIF
        ui.FlexGridSizer_adv_SLIPIF:AddStaticText("LWIP_HAVE_SLIPIF")
        ui.Choice_adv_LWIP_HAVE_SLIPIF = wx.wxChoice(ui.Panel_adv_SLIPIF, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_LWIP_HAVE_SLIPIF:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_LWIP_HAVE_SLIPIF:SetToolTip("LWIP_HAVE_SLIPIF==1: Support slip interface")
        ui.Choice_adv_LWIP_HAVE_SLIPIF:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_SLIPIF:Add(ui.Choice_adv_LWIP_HAVE_SLIPIF, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
        ui.FlexGridSizer_adv_THREAD.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_THREAD, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

        -- TCPIP_THREAD_NAME
        ui.FlexGridSizer_adv_THREAD:AddStaticText("TCPIP_THREAD_NAME")
        ui.TextCtrl_adv_TCPIP_THREAD_NAME = wx.wxTextCtrl(ui.Panel_adv_THREAD, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.TextCtrl_adv_TCPIP_THREAD_NAME:SetToolTip("TCPIP_THREAD_NAME: The name assigned to the main tcpip thread.")
        ui.TextCtrl_adv_TCPIP_THREAD_NAME:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_THREAD:Add(ui.TextCtrl_adv_TCPIP_THREAD_NAME, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TCPIP_THREAD_STACKSIZE
        ui.FlexGridSizer_adv_THREAD:AddStaticText("TCPIP_THREAD_STACKSIZE")
        ui.Choice_adv_TCPIP_THREAD_STACKSIZE = wx.wxChoice(ui.Panel_adv_THREAD, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        for i = 0, stack_size:count() - 1 do ui.Choice_adv_TCPIP_THREAD_STACKSIZE:Append(stack_size:get_value(i):gsub("STACK_DEPTH_", "")) end
        ui.Choice_adv_TCPIP_THREAD_STACKSIZE:SetToolTip("TCPIP_THREAD_STACKSIZE: The stack size used by the main tcpip thread.\n"..
                                                        "The stack size value itself is platform-dependent, but is passed to\n"..
                                                        "sys_thread_new() when the thread is created.")
        ui.Choice_adv_TCPIP_THREAD_STACKSIZE:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_THREAD:Add(ui.Choice_adv_TCPIP_THREAD_STACKSIZE, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TCPIP_THREAD_PRIO
        ui.FlexGridSizer_adv_THREAD:AddStaticText("TCPIP_THREAD_PRIO")
        ui.SpinCtrl_adv_TCPIP_THREAD_PRIO = wx.wxSpinCtrl(ui.Panel_adv_THREAD, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, priority_min, priority_max)
        ui.SpinCtrl_adv_TCPIP_THREAD_PRIO:SetToolTip("TCPIP_THREAD_PRIO: The priority assigned to the main tcpip thread.\n"..
                                                     "The priority value itself is platform-dependent, but is passed to\n"..
                                                     "sys_thread_new() when the thread is created.")
        ui.SpinCtrl_adv_TCPIP_THREAD_PRIO:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_THREAD:Add(ui.SpinCtrl_adv_TCPIP_THREAD_PRIO, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TCPIP_MBOX_SIZE
        ui.FlexGridSizer_adv_THREAD:AddStaticText("TCPIP_MBOX_SIZE")
        ui.SpinCtrl_adv_TCPIP_MBOX_SIZE = wx.wxSpinCtrl(ui.Panel_adv_THREAD, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 128)
        ui.SpinCtrl_adv_TCPIP_MBOX_SIZE:SetToolTip("TCPIP_MBOX_SIZE: The mailbox size for the tcpip thread messages\n"..
                                                   "The queue size value itself is platform-dependent, but is passed\nto sys_mbox_new() when tcpip_init is called.")
        ui.SpinCtrl_adv_TCPIP_MBOX_SIZE:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_THREAD:Add(ui.SpinCtrl_adv_TCPIP_MBOX_SIZE, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- SLIPIF_THREAD_NAME
        ui.FlexGridSizer_adv_THREAD:AddStaticText("SLIPIF_THREAD_NAME")
        ui.TextCtrl_adv_SLIPIF_THREAD_NAME = wx.wxTextCtrl(ui.Panel_adv_THREAD, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.TextCtrl_adv_SLIPIF_THREAD_NAME:SetToolTip("SLIPIF_THREAD_NAME: The name assigned to the slipif_loop thread.")
        ui.TextCtrl_adv_SLIPIF_THREAD_NAME:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_THREAD:Add(ui.TextCtrl_adv_SLIPIF_THREAD_NAME, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- SLIPIF_THREAD_STACKSIZE
        ui.FlexGridSizer_adv_THREAD:AddStaticText("SLIPIF_THREAD_STACKSIZE")
        ui.Choice_adv_SLIPIF_THREAD_STACKSIZE = wx.wxChoice(ui.Panel_adv_THREAD, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        for i = 0, stack_size:count() - 1 do ui.Choice_adv_SLIPIF_THREAD_STACKSIZE:Append(stack_size:get_value(i):gsub("STACK_DEPTH_", "")) end
        ui.Choice_adv_SLIPIF_THREAD_STACKSIZE:SetToolTip("SLIPIF_THREAD_STACKSIZE: The stack size used by the main tcpip thread.\n"..
                                                         "The stack size value itself is platform-dependent, but is passed to\n"..
                                                         "sys_thread_new() when the thread is created.")
        ui.Choice_adv_SLIPIF_THREAD_STACKSIZE:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_THREAD:Add(ui.Choice_adv_SLIPIF_THREAD_STACKSIZE, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- SLIPIF_THREAD_PRIO
        ui.FlexGridSizer_adv_THREAD:AddStaticText("SLIPIF_THREAD_PRIO")
        ui.SpinCtrl_adv_SLIPIF_THREAD_PRIO = wx.wxSpinCtrl(ui.Panel_adv_THREAD, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, priority_min, priority_max)
        ui.SpinCtrl_adv_SLIPIF_THREAD_PRIO:SetToolTip("SLIPIF_THREAD_PRIO: The priority assigned to the main tcpip thread.\n"..
                                                      "The priority value itself is platform-dependent, but is passed to\n"..
                                                      "sys_thread_new() when the thread is created.")
        ui.SpinCtrl_adv_SLIPIF_THREAD_PRIO:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_THREAD:Add(ui.SpinCtrl_adv_SLIPIF_THREAD_PRIO, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- PPP_THREAD_NAME
        ui.FlexGridSizer_adv_THREAD:AddStaticText("PPP_THREAD_NAME")
        ui.TextCtrl_adv_PPP_THREAD_NAME = wx.wxTextCtrl(ui.Panel_adv_THREAD, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.TextCtrl_adv_PPP_THREAD_NAME:SetToolTip("PPP_THREAD_NAME: The name assigned to the pppInputThread.")
        ui.TextCtrl_adv_PPP_THREAD_NAME:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_THREAD:Add(ui.TextCtrl_adv_PPP_THREAD_NAME, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- PPP_THREAD_STACKSIZE
        ui.FlexGridSizer_adv_THREAD:AddStaticText("PPP_THREAD_STACKSIZE")
        ui.Choice_adv_PPP_THREAD_STACKSIZE = wx.wxChoice(ui.Panel_adv_THREAD, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        for i = 0, stack_size:count() - 1 do ui.Choice_adv_PPP_THREAD_STACKSIZE:Append(stack_size:get_value(i):gsub("STACK_DEPTH_", "")) end
        ui.Choice_adv_PPP_THREAD_STACKSIZE:SetToolTip("PPP_THREAD_STACKSIZE: The stack size used by the main tcpip thread.\n"..
                                                      "The stack size value itself is platform-dependent, but is passed to\n"..
                                                      "sys_thread_new() when the thread is created.")
        ui.Choice_adv_PPP_THREAD_STACKSIZE:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_THREAD:Add(ui.Choice_adv_PPP_THREAD_STACKSIZE, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- PPP_THREAD_PRIO
        ui.FlexGridSizer_adv_THREAD:AddStaticText("PPP_THREAD_PRIO")
        ui.SpinCtrl_adv_PPP_THREAD_PRIO = wx.wxSpinCtrl(ui.Panel_adv_THREAD, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, priority_min, priority_max)
        ui.SpinCtrl_adv_PPP_THREAD_PRIO:SetToolTip("PPP_THREAD_PRIO: The priority assigned to the main tcpip thread.\n"..
                                                   "The priority value itself is platform-dependent, but is passed to\n"..
                                                   "sys_thread_new() when the thread is created.")
        ui.SpinCtrl_adv_PPP_THREAD_PRIO:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_THREAD:Add(ui.SpinCtrl_adv_PPP_THREAD_PRIO, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- DEFAULT_THREAD_NAME
        ui.FlexGridSizer_adv_THREAD:AddStaticText("DEFAULT_THREAD_NAME")
        ui.TextCtrl_adv_DEFAULT_THREAD_NAME = wx.wxTextCtrl(ui.Panel_adv_THREAD, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.TextCtrl_adv_DEFAULT_THREAD_NAME:SetToolTip("DEFAULT_THREAD_NAME: The name assigned to any other lwIP thread.")
        ui.TextCtrl_adv_DEFAULT_THREAD_NAME:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_THREAD:Add(ui.TextCtrl_adv_DEFAULT_THREAD_NAME, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- DEFAULT_THREAD_STACKSIZE
        ui.FlexGridSizer_adv_THREAD:AddStaticText("DEFAULT_THREAD_STACKSIZE")
        ui.Choice_adv_DEFAULT_THREAD_STACKSIZE = wx.wxChoice(ui.Panel_adv_THREAD, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        for i = 0, stack_size:count() - 1 do ui.Choice_adv_DEFAULT_THREAD_STACKSIZE:Append(stack_size:get_value(i):gsub("STACK_DEPTH_", "")) end
        ui.Choice_adv_DEFAULT_THREAD_STACKSIZE:SetToolTip("DEFAULT_THREAD_STACKSIZE: The stack size used by the main tcpip thread.\n"..
                                                          "The stack size value itself is platform-dependent, but is passed to\n"..
                                                          "sys_thread_new() when the thread is created.")
        ui.Choice_adv_DEFAULT_THREAD_STACKSIZE:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_THREAD:Add(ui.Choice_adv_DEFAULT_THREAD_STACKSIZE, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- DEFAULT_THREAD_PRIO
        ui.FlexGridSizer_adv_THREAD:AddStaticText("DEFAULT_THREAD_PRIO")
        ui.SpinCtrl_adv_DEFAULT_THREAD_PRIO = wx.wxSpinCtrl(ui.Panel_adv_THREAD, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, priority_min, priority_max)
        ui.SpinCtrl_adv_DEFAULT_THREAD_PRIO:SetToolTip("DEFAULT_THREAD_PRIO: The priority assigned to the main tcpip thread.\n"..
                                                       "The priority value itself is platform-dependent, but is passed to\n"..
                                                       "sys_thread_new() when the thread is created.")
        ui.SpinCtrl_adv_DEFAULT_THREAD_PRIO:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_THREAD:Add(ui.SpinCtrl_adv_DEFAULT_THREAD_PRIO, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- DEFAULT_RAW_RECVMBOX_SIZE
        ui.FlexGridSizer_adv_THREAD:AddStaticText("DEFAULT_RAW_RECVMBOX_SIZE")
        ui.SpinCtrl_adv_DEFAULT_RAW_RECVMBOX_SIZE = wx.wxSpinCtrl(ui.Panel_adv_THREAD, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 128)
        ui.SpinCtrl_adv_DEFAULT_RAW_RECVMBOX_SIZE:SetToolTip("DEFAULT_RAW_RECVMBOX_SIZE: The mailbox size for the incoming packets on a\n"..
                                                             "NETCONN_RAW. The queue size value itself is platform-dependent, but is passed\n"..
                                                             "to sys_mbox_new() when the recvmbox is created.")
        ui.SpinCtrl_adv_DEFAULT_RAW_RECVMBOX_SIZE:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_THREAD:Add(ui.SpinCtrl_adv_DEFAULT_RAW_RECVMBOX_SIZE, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- DEFAULT_UDP_RECVMBOX_SIZE
        ui.FlexGridSizer_adv_THREAD:AddStaticText("DEFAULT_UDP_RECVMBOX_SIZE")
        ui.SpinCtrl_adv_DEFAULT_UDP_RECVMBOX_SIZE = wx.wxSpinCtrl(ui.Panel_adv_THREAD, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 128)
        ui.SpinCtrl_adv_DEFAULT_UDP_RECVMBOX_SIZE:SetToolTip("DEFAULT_UDP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a\n"..
                                                             "NETCONN_UDP. The queue size value itself is platform-dependent, but is passed\n"..
                                                             "to sys_mbox_new() when the recvmbox is created.")
        ui.SpinCtrl_adv_DEFAULT_UDP_RECVMBOX_SIZE:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_THREAD:Add(ui.SpinCtrl_adv_DEFAULT_UDP_RECVMBOX_SIZE, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- DEFAULT_TCP_RECVMBOX_SIZE
        ui.FlexGridSizer_adv_THREAD:AddStaticText("DEFAULT_TCP_RECVMBOX_SIZE")
        ui.SpinCtrl_adv_DEFAULT_TCP_RECVMBOX_SIZE = wx.wxSpinCtrl(ui.Panel_adv_THREAD, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 128)
        ui.SpinCtrl_adv_DEFAULT_TCP_RECVMBOX_SIZE:SetToolTip("DEFAULT_TCP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a\n"..
                                                             "NETCONN_TCP. The queue size value itself is platform-dependent, but is passed\n"..
                                                             "to sys_mbox_new() when the recvmbox is created.")
        ui.SpinCtrl_adv_DEFAULT_TCP_RECVMBOX_SIZE:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_THREAD:Add(ui.SpinCtrl_adv_DEFAULT_TCP_RECVMBOX_SIZE, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- DEFAULT_ACCEPTMBOX_SIZE
        ui.FlexGridSizer_adv_THREAD:AddStaticText("DEFAULT_ACCEPTMBOX_SIZE")
        ui.SpinCtrl_adv_DEFAULT_ACCEPTMBOX_SIZE = wx.wxSpinCtrl(ui.Panel_adv_THREAD, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 128)
        ui.SpinCtrl_adv_DEFAULT_ACCEPTMBOX_SIZE:SetToolTip("DEFAULT_ACCEPTMBOX_SIZE: The mailbox size for the incoming connections.\n"..
                                                           "The queue size value itself is platform-dependent, but is passed to\n"..
                                                           "sys_mbox_new() when the acceptmbox is created.")
        ui.SpinCtrl_adv_DEFAULT_ACCEPTMBOX_SIZE:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_THREAD:Add(ui.SpinCtrl_adv_DEFAULT_ACCEPTMBOX_SIZE, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
        ui.FlexGridSizer_adv_SEQL.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_SEQL, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

        -- LWIP_TCPIP_TIMEOUT
        ui.FlexGridSizer_adv_SEQL:AddStaticText("LWIP_TCPIP_TIMEOUT")
        ui.Choice_adv_LWIP_TCPIP_TIMEOUT = wx.wxChoice(ui.Panel_adv_SEQL, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_LWIP_TCPIP_TIMEOUT:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_LWIP_TCPIP_TIMEOUT:SetToolTip("LWIP_TCPIP_TIMEOUT==1: Enable tcpip_timeout/tcpip_untimeout\nto create timers running in tcpip_thread from another thread.")
        ui.Choice_adv_LWIP_TCPIP_TIMEOUT:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_SEQL:Add(ui.Choice_adv_LWIP_TCPIP_TIMEOUT, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
        ui.FlexGridSizer_adv_PPP.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_PPP, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

        -- PPP_SUPPORT
        ui.FlexGridSizer_adv_PPP:AddStaticText("PPP_SUPPORT")
        ui.Choice_adv_PPP_SUPPORT = wx.wxChoice(ui.Panel_adv_PPP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_PPP_SUPPORT:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_PPP_SUPPORT:SetToolTip("PPP_SUPPORT==1: Enable PPP.")
        ui.Choice_adv_PPP_SUPPORT:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_PPP:Add(ui.Choice_adv_PPP_SUPPORT, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- PPPOE_SUPPORT
        ui.FlexGridSizer_adv_PPP:AddStaticText("PPPOE_SUPPORT")
        ui.Choice_adv_PPPOE_SUPPORT = wx.wxChoice(ui.Panel_adv_PPP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_PPPOE_SUPPORT:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_PPPOE_SUPPORT:SetToolTip("PPPOE_SUPPORT==1: Enable PPP Over Ethernet.")
        ui.Choice_adv_PPPOE_SUPPORT:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_PPP:Add(ui.Choice_adv_PPPOE_SUPPORT, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- NUM_PPP
        ui.FlexGridSizer_adv_PPP:AddStaticText("NUM_PPP")
        ui.SpinCtrl_adv_NUM_PPP = wx.wxSpinCtrl(ui.Panel_adv_PPP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 8)
        ui.SpinCtrl_adv_NUM_PPP:SetToolTip("NUM_PPP: Max PPP sessions.")
        ui.SpinCtrl_adv_NUM_PPP:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_PPP:Add(ui.SpinCtrl_adv_NUM_PPP, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- PAP_SUPPORT
        ui.FlexGridSizer_adv_PPP:AddStaticText("PAP_SUPPORT")
        ui.Choice_adv_PAP_SUPPORT = wx.wxChoice(ui.Panel_adv_PPP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_PAP_SUPPORT:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_PAP_SUPPORT:SetToolTip("PAP_SUPPORT==1: Support PAP.")
        ui.Choice_adv_PAP_SUPPORT:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_PPP:Add(ui.Choice_adv_PAP_SUPPORT, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- CHAP_SUPPORT
        ui.FlexGridSizer_adv_PPP:AddStaticText("CHAP_SUPPORT")
        ui.Choice_adv_CHAP_SUPPORT = wx.wxChoice(ui.Panel_adv_PPP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_CHAP_SUPPORT:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_CHAP_SUPPORT:SetToolTip("CHAP_SUPPORT==1: Support CHAP.")
        ui.Choice_adv_CHAP_SUPPORT:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_PPP:Add(ui.Choice_adv_CHAP_SUPPORT, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MD5_SUPPORT
        ui.FlexGridSizer_adv_PPP:AddStaticText("MD5_SUPPORT")
        ui.Choice_adv_MD5_SUPPORT = wx.wxChoice(ui.Panel_adv_PPP, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_MD5_SUPPORT:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_MD5_SUPPORT:SetToolTip("MD5_SUPPORT==1: Support MD5 (see also CHAP).")
        ui.Choice_adv_MD5_SUPPORT:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_PPP:Add(ui.Choice_adv_MD5_SUPPORT, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- FSM_DEFTIMEOUT
        ui.FlexGridSizer_adv_PPP:AddStaticText("FSM_DEFTIMEOUT")
        ui.SpinCtrl_adv_FSM_DEFTIMEOUT = wx.wxSpinCtrl(ui.Panel_adv_PPP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 300)
        ui.SpinCtrl_adv_FSM_DEFTIMEOUT:SetToolTip("Default timeout value. Timeout time in seconds.")
        ui.SpinCtrl_adv_FSM_DEFTIMEOUT:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_PPP:Add(ui.SpinCtrl_adv_FSM_DEFTIMEOUT, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- FSM_DEFMAXTERMREQS
        ui.FlexGridSizer_adv_PPP:AddStaticText("FSM_DEFMAXTERMREQS")
        ui.SpinCtrl_adv_FSM_DEFMAXTERMREQS = wx.wxSpinCtrl(ui.Panel_adv_PPP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_FSM_DEFMAXTERMREQS:SetToolTip("Maximum Terminate-Request transmissions.")
        ui.SpinCtrl_adv_FSM_DEFMAXTERMREQS:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_PPP:Add(ui.SpinCtrl_adv_FSM_DEFMAXTERMREQS, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- FSM_DEFMAXCONFREQS
        ui.FlexGridSizer_adv_PPP:AddStaticText("FSM_DEFMAXCONFREQS")
        ui.SpinCtrl_adv_FSM_DEFMAXCONFREQS = wx.wxSpinCtrl(ui.Panel_adv_PPP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_FSM_DEFMAXCONFREQS:SetToolTip("Maximum Configure-Request transmissions.")
        ui.SpinCtrl_adv_FSM_DEFMAXCONFREQS:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_PPP:Add(ui.SpinCtrl_adv_FSM_DEFMAXCONFREQS, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- FSM_DEFMAXNAKLOOPS
        ui.FlexGridSizer_adv_PPP:AddStaticText("FSM_DEFMAXNAKLOOPS")
        ui.SpinCtrl_adv_FSM_DEFMAXNAKLOOPS = wx.wxSpinCtrl(ui.Panel_adv_PPP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 100)
        ui.SpinCtrl_adv_FSM_DEFMAXNAKLOOPS:SetToolTip("Maximum number of NAK loops.")
        ui.SpinCtrl_adv_FSM_DEFMAXNAKLOOPS:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_PPP:Add(ui.SpinCtrl_adv_FSM_DEFMAXNAKLOOPS, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- UPAP_DEFTIMEOUT
        ui.FlexGridSizer_adv_PPP:AddStaticText("UPAP_DEFTIMEOUT")
        ui.SpinCtrl_adv_UPAP_DEFTIMEOUT = wx.wxSpinCtrl(ui.Panel_adv_PPP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 300)
        ui.SpinCtrl_adv_UPAP_DEFTIMEOUT:SetToolTip("Timeout (seconds) for retransmitting request.")
        ui.SpinCtrl_adv_UPAP_DEFTIMEOUT:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_PPP:Add(ui.SpinCtrl_adv_UPAP_DEFTIMEOUT, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- UPAP_DEFREQTIME
        ui.FlexGridSizer_adv_PPP:AddStaticText("UPAP_DEFREQTIME")
        ui.SpinCtrl_adv_UPAP_DEFREQTIME = wx.wxSpinCtrl(ui.Panel_adv_PPP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 300)
        ui.SpinCtrl_adv_UPAP_DEFREQTIME:SetToolTip("Time to wait for authenticate-request from peer.")
        ui.SpinCtrl_adv_UPAP_DEFREQTIME:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_PPP:Add(ui.SpinCtrl_adv_UPAP_DEFREQTIME, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- CHAP_DEFTIMEOUT
        ui.FlexGridSizer_adv_PPP:AddStaticText("CHAP_DEFTIMEOUT")
        ui.SpinCtrl_adv_CHAP_DEFTIMEOUT = wx.wxSpinCtrl(ui.Panel_adv_PPP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 300)
        ui.SpinCtrl_adv_CHAP_DEFTIMEOUT:SetToolTip("CHAP Timeout time in seconds.")
        ui.SpinCtrl_adv_CHAP_DEFTIMEOUT:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_PPP:Add(ui.SpinCtrl_adv_CHAP_DEFTIMEOUT, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- CHAP_DEFTRANSMITS
        ui.FlexGridSizer_adv_PPP:AddStaticText("CHAP_DEFTRANSMITS")
        ui.SpinCtrl_adv_CHAP_DEFTRANSMITS = wx.wxSpinCtrl(ui.Panel_adv_PPP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 300)
        ui.SpinCtrl_adv_CHAP_DEFTRANSMITS:SetToolTip("Max # times to send challenge.")
        ui.SpinCtrl_adv_CHAP_DEFTRANSMITS:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_PPP:Add(ui.SpinCtrl_adv_CHAP_DEFTRANSMITS, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- LCP_ECHOINTERVAL
        ui.FlexGridSizer_adv_PPP:AddStaticText("LCP_ECHOINTERVAL")
        ui.SpinCtrl_adv_LCP_ECHOINTERVAL = wx.wxSpinCtrl(ui.Panel_adv_PPP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 300)
        ui.SpinCtrl_adv_LCP_ECHOINTERVAL:SetToolTip("Interval in seconds between keepalive echo requests, 0 to disable.")
        ui.SpinCtrl_adv_LCP_ECHOINTERVAL:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_PPP:Add(ui.SpinCtrl_adv_LCP_ECHOINTERVAL, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- LCP_MAXECHOFAILS
        ui.FlexGridSizer_adv_PPP:AddStaticText("LCP_MAXECHOFAILS")
        ui.SpinCtrl_adv_LCP_MAXECHOFAILS = wx.wxSpinCtrl(ui.Panel_adv_PPP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 0, 100)
        ui.SpinCtrl_adv_LCP_MAXECHOFAILS:SetToolTip("Number of unanswered echo requests before failure.")
        ui.SpinCtrl_adv_LCP_MAXECHOFAILS:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_PPP:Add(ui.SpinCtrl_adv_LCP_MAXECHOFAILS, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- PPP_MAXIDLEFLAG
        ui.FlexGridSizer_adv_PPP:AddStaticText("PPP_MAXIDLEFLAG")
        ui.SpinCtrl_adv_PPP_MAXIDLEFLAG = wx.wxSpinCtrl(ui.Panel_adv_PPP, wx.wxNewId(), "", wx.wxDefaultPosition, wx.wxDefaultSize, 0, 1, 300)
        ui.SpinCtrl_adv_PPP_MAXIDLEFLAG:SetToolTip("Max Xmit idle time (in jiffies) before resend flag char.")
        ui.SpinCtrl_adv_PPP_MAXIDLEFLAG:Connect(wx.wxEVT_COMMAND_TEXT_UPDATED, function() modified:yes() end)
        ui.FlexGridSizer_adv_PPP:Add(ui.SpinCtrl_adv_PPP_MAXIDLEFLAG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
        ui.FlexGridSizer_adv_CHSUM.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_CHSUM, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

        -- LWIP_CHECKSUM_ON_COPY
        ui.FlexGridSizer_adv_CHSUM:AddStaticText("LWIP_CHECKSUM_ON_COPY")
        ui.Choice_adv_LWIP_CHECKSUM_ON_COPY = wx.wxChoice(ui.Panel_adv_CHSUM, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        ui.Choice_adv_LWIP_CHECKSUM_ON_COPY:Append({"Disable (0)", "Enable (1)"})
        ui.Choice_adv_LWIP_CHECKSUM_ON_COPY:SetToolTip("LWIP_CHECKSUM_ON_COPY==1: Calculate checksum when\ncopying data from application buffers to pbufs.")
        ui.Choice_adv_LWIP_CHECKSUM_ON_COPY:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_CHSUM:Add(ui.Choice_adv_LWIP_CHECKSUM_ON_COPY, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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
        ui.FlexGridSizer_adv_DEBUG.AddStaticText = function(self, s) self:Add(wx.wxStaticText(ui.Panel_adv_DEBUG, wx.wxID_ANY, s), 1, wx.wxALL+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5) end

        -- LWIP_DBG_MIN_LEVEL
        ui.FlexGridSizer_adv_DEBUG:AddStaticText("LWIP_DBG_MIN_LEVEL")
        ui.Choice_adv_LWIP_DBG_MIN_LEVEL = wx.wxChoice(ui.Panel_adv_DEBUG, wx.wxNewId(), wx.wxDefaultPosition, wx.wxDefaultSize, {})
        for i = 0, debug_level:count() -1 do ui.Choice_adv_LWIP_DBG_MIN_LEVEL:Append(debug_level:get_value(i):gsub("LWIP_DBG_LEVEL_", "")) end
        ui.Choice_adv_LWIP_DBG_MIN_LEVEL:SetToolTip("LWIP_DBG_MIN_LEVEL: After masking, the value of the debug is\n"..
                                                    "compared against this value. If it is smaller, then debugging messages are written.")
        ui.Choice_adv_LWIP_DBG_MIN_LEVEL:Connect(wx.wxEVT_COMMAND_CHOICE_SELECTED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.Choice_adv_LWIP_DBG_MIN_LEVEL, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- LWIP_DBG_TYPES_ON
        ui.CheckBox_adv_LWIP_DBG_TYPES_ON = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "LWIP_DBG_TYPES_ON", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_LWIP_DBG_TYPES_ON:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_LWIP_DBG_TYPES_ON, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- LOW_LEVEL_DEBUG
        ui.CheckBox_adv_LOW_LEVEL_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "LOW_LEVEL_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_LOW_LEVEL_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_LOW_LEVEL_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- ETHARP_DEBUG
        ui.CheckBox_adv_ETHARP_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "ETHARP_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_ETHARP_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_ETHARP_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- NETIF_DEBUG
        ui.CheckBox_adv_NETIF_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "NETIF_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_NETIF_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_NETIF_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- PBUF_DEBUG
        ui.CheckBox_adv_PBUF_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "PBUF_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_PBUF_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_PBUF_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- API_LIB_DEBUG
        ui.CheckBox_adv_API_LIB_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "API_LIB_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_API_LIB_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_API_LIB_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- API_MSG_DEBUG
        ui.CheckBox_adv_API_MSG_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "API_MSG_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_API_MSG_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_API_MSG_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- SOCKETS_DEBUG
        ui.CheckBox_adv_SOCKETS_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "SOCKETS_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_SOCKETS_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_SOCKETS_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- ICMP_DEBUG
        ui.CheckBox_adv_ICMP_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "ICMP_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_ICMP_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_ICMP_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- IGMP_DEBUG
        ui.CheckBox_adv_IGMP_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "IGMP_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_IGMP_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_IGMP_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- INET_DEBUG
        ui.CheckBox_adv_INET_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "INET_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_INET_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_INET_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- IP_DEBUG
        ui.CheckBox_adv_IP_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "IP_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_IP_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_IP_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- IP_REASS_DEBUG
        ui.CheckBox_adv_IP_REASS_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "IP_REASS_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_IP_REASS_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_IP_REASS_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- RAW_DEBUG
        ui.CheckBox_adv_RAW_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "RAW_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_RAW_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_RAW_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEM_DEBUG
        ui.CheckBox_adv_MEM_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "MEM_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_MEM_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_MEM_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- MEMP_DEBUG
        ui.CheckBox_adv_MEMP_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "MEMP_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_MEMP_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_MEMP_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- SYS_DEBUG
        ui.CheckBox_adv_SYS_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "SYS_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_SYS_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_SYS_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TIMERS_DEBUG
        ui.CheckBox_adv_TIMERS_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "TIMERS_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_TIMERS_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_TIMERS_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TCP_DEBUG
        ui.CheckBox_adv_TCP_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "TCP_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_TCP_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_TCP_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TCP_INPUT_DEBUG
        ui.CheckBox_adv_TCP_INPUT_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "TCP_INPUT_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_TCP_INPUT_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_TCP_INPUT_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TCP_FR_DEBUG
        ui.CheckBox_adv_TCP_FR_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "TCP_FR_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_TCP_FR_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_TCP_FR_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TCP_RTO_DEBUG
        ui.CheckBox_adv_TCP_RTO_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "TCP_RTO_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_TCP_RTO_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_TCP_RTO_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TCP_CWND_DEBUG
        ui.CheckBox_adv_TCP_CWND_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "TCP_CWND_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_TCP_CWND_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_TCP_CWND_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TCP_WND_DEBUG
        ui.CheckBox_adv_TCP_WND_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "TCP_WND_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_TCP_WND_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_TCP_WND_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TCP_OUTPUT_DEBUG
        ui.CheckBox_adv_TCP_OUTPUT_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "TCP_OUTPUT_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_TCP_OUTPUT_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_TCP_OUTPUT_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TCP_RST_DEBUG
        ui.CheckBox_adv_TCP_RST_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "TCP_RST_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_TCP_RST_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_TCP_RST_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TCP_QLEN_DEBUG
        ui.CheckBox_adv_TCP_QLEN_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "TCP_QLEN_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_TCP_QLEN_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_TCP_QLEN_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- UDP_DEBUG
        ui.CheckBox_adv_UDP_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "UDP_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_UDP_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_UDP_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- TCPIP_DEBUG
        ui.CheckBox_adv_TCPIP_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "TCPIP_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_TCPIP_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_TCPIP_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- PPP_DEBUG
        ui.CheckBox_adv_PPP_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "PPP_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_PPP_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_PPP_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- SLIP_DEBUG
        ui.CheckBox_adv_SLIP_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "SLIP_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_SLIP_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_SLIP_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- DHCP_DEBUG
        ui.CheckBox_adv_DHCP_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "DHCP_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_DHCP_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_DHCP_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- AUTOIP_DEBUG
        ui.CheckBox_adv_AUTOIP_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "AUTOIP_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_AUTOIP_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_AUTOIP_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- SNMP_MSG_DEBUG
        ui.CheckBox_adv_SNMP_MSG_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "SNMP_MSG_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_SNMP_MSG_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_SNMP_MSG_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- SNMP_MIB_DEBUG
        ui.CheckBox_adv_SNMP_MIB_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "SNMP_MIB_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_SNMP_MIB_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_SNMP_MIB_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

        -- DNS_DEBUG
        ui.CheckBox_adv_DNS_DEBUG = wx.wxCheckBox(ui.Panel_adv_DEBUG, wx.wxNewId(), "DNS_DEBUG", wx.wxDefaultPosition, wx.wxDefaultSize)
        ui.CheckBox_adv_DNS_DEBUG:Connect(wx.wxEVT_COMMAND_CHECKBOX_CLICKED, function() modified:yes() end)
        ui.FlexGridSizer_adv_DEBUG:Add(ui.CheckBox_adv_DNS_DEBUG, 1, wx.wxALL+wx.wxEXPAND+wx.wxALIGN_LEFT+wx.wxALIGN_CENTER_VERTICAL, 5)

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

                priority_min, priority_max = operating_system:get_priority_range()

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
        ui.window:Hide()

        -- refresh priority range
        priority_min, priority_max = operating_system:get_priority_range()

        -- update range of priority controls
        ui.SpinCtrl_adv_TCPIP_THREAD_PRIO:SetRange(priority_min, priority_max)
        ui.SpinCtrl_adv_SLIPIF_THREAD_PRIO:SetRange(priority_min, priority_max)
        ui.SpinCtrl_adv_PPP_THREAD_PRIO:SetRange(priority_min, priority_max)
        ui.SpinCtrl_adv_DEFAULT_THREAD_PRIO:SetRange(priority_min, priority_max)

        -- load configuration
        modified:no()
        modified:enable(false)
        load_controls()
        modified:enable(true)

        ui.window:Show()
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
