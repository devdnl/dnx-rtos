/*=========================================================================*//**
@file    lwip_flags.h

@author  Daniel Zorychta

@brief   Global project flags which are included from compiler command line

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes dnx RTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


*//*==========================================================================*/

/*
 * NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as suffix
 *       and prefix) are exported to the single configuration file
 *       (by using Configtool) when entire project configuration is exported.
 *       All other flag definitions and statements are ignored.
 */

#ifndef _LWIP_FLAGS_H
#define _LWIP_FLAGS_H

/*--
this.backFile = "network/tcpip_flags.h"
this:SetLayout("TitledGridBack", 2, "Home > Network > TCP/IP Stack > LwIP",
               function() this:LoadFile(this.backFile) end)

this.GoBackWidget = function(this, name)
    this:AddExtraWidget("Hyperlink", "Hl"..name, "Back")
    this:SetEvent("clicked", "Hl"..name,
        function() this:LoadFile(this.backFile) end)
end

this.AddDebugItems = function(this)
    this:AddItem("Off", "LWIP_DBG_OFF")
    this:AddItem("On", "LWIP_DBG_ON")
end
++*/

/*------------------------------------------------------------------------------
this:AddExtraWidget("Label", "LabelMEMP", "Memory options", -1, "bold")
this:GoBackWidget("MEMP")
++*/
/*--
this:AddWidget("Combobox", "MEMP_OVERFLOW_CHECK")
this:AddItem("Disable (0)", "0")
this:AddItem("Each element when it is freed (1)", "1")
this:AddItem("Each element in every pool every time (2)", "2")
this:SetToolTip("MEMP_OVERFLOW_CHECK: memp overflow protection\n"..
                "reserves a configurable amount of bytes before\n"..
                "and after each memp element in every pool and\n"..
                "fills it with a prominent default value.\n"..
                "- 0: no checking\n"..
                "- 1: checks each element when it is freed\n"..
                "- 2: checks each element in every pool every time memp_malloc() or memp_free() is called (useful but slow!).")
--*/
#define __NETWORK_MEMP_OVERFLOW_CHECK__ 0
/*--
this:AddWidget("Combobox", "MEMP_SANITY_CHECK")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("MEMP_SANITY_CHECK = 1: run a sanity check after each memp_free()\n"..
                "to make sure that the linked list of heap elements is not corrupted.")
--*/
#define __NETWORK_MEMP_SANITY_CHECK__ 0
/*--
this:AddWidget("Combobox", "MEM_OVERFLOW_CHECK")
this:AddItem("No checking (0)", "0")
this:AddItem("Check each element (1)", "1")
this:AddItem("Check all heap elements (2)", "2")
this:SetToolTip("MEM_OVERFLOW_CHECK: mem overflow protection reserves a configurable\n"..
                "amount of bytes before and after each heap allocation chunk and fills\n"..
                "it with a prominent default value.\n"..
                "- 0: no checking\n"..
                "- 1: each element\n"..
                "- 2: all heap elements")
--*/
#define __NETWORK_MEM_OVERFLOW_CHECK__ 0
/*--
this:AddWidget("Combobox", "MEM_SANITY_CHECK")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("MEM_SANITY_CHECK==1: run a sanity check after each mem_free() to make\n"..
                "sure that the linked list of heap elements is not corrupted.")
--*/
#define __NETWORK_MEM_SANITY_CHECK__ 0
/*--
this:AddWidget("Combobox", "MEM_USE_POOLS")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("MEM_USE_POOLS==1: Use an alternative to malloc() by allocating from a set\n"..
                "of memory pools of various sizes. When mem_malloc is called, an element of\n"..
                "the smallest pool that can provide the length needed is returned.\n"..
                "To use this, MEMP_USE_CUSTOM_POOLS also has to be enabled.\n")
--*/
#define __NETWORK_MEM_USE_POOLS__ 0
/*--
this:AddWidget("Combobox", "MEM_USE_POOLS_TRY_BIGGER_POOL")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("MEM_USE_POOLS_TRY_BIGGER_POOL==1: if one malloc-pool is empty, try the next\n"..
                "bigger pool - WARNING: THIS MIGHT WASTE MEMORY but it can make a system more\n"..
                "reliable.")
--*/
#define __NETWORK_MEM_USE_POOLS_TRY_BIGGER_POOL__ 0
/*--
this:AddWidget("Combobox", "MEMP_USE_CUSTOM_POOLS")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("MEMP_USE_CUSTOM_POOLS==1: whether to include a user file lwippools.h\n"..
                "that defines additional pools beyond the \"standard\" ones required\n"..
                "by lwIP. If you set this to 1, you must have lwippools.h in your\n"..
                "include path somewhere.")
--*/
#define __NETWORK_MEMP_USE_CUSTOM_POOLS__ 0


/*------------------------------------------------------------------------------
this:AddExtraWidget("Label", "LabelMEMPNUM", "\nInternal memory pool sizes", -1, "bold")
this:GoBackWidget("MEMPNUM")
++*/
/*--
this:AddWidget("Spinbox", 0, 100, "MEMP_NUM_PBUF")
this:SetToolTip("MEMP_NUM_PBUF: the number of memp struct pbufs. If the application\n"..
                "sends a lot of data out of ROM (or other static memory), this\n"..
                "should be set high.")
--*/
#define __NETWORK_MEMP_NUM_PBUF__ 48
/*--
this:AddWidget("Spinbox", 0, 100, "MEMP_NUM_RAW_PCB")
this:SetToolTip("MEMP_NUM_RAW_PCB: Number of raw connection PCBs")
--*/
#define __NETWORK_MEMP_NUM_RAW_PCB__ 32
/*--
this:AddWidget("Spinbox", 0, 100, "MEMP_NUM_UDP_PCB")
this:SetToolTip("MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks.\nOne per active UDP \"connection\".")
--*/
#define __NETWORK_MEMP_NUM_UDP_PCB__ 16
/*--
this:AddWidget("Spinbox", 0, 100, "MEMP_NUM_TCP_PCB")
this:SetToolTip("MEMP_NUM_TCP_PCB: the number of simulatenously active TCP connections.")
--*/
#define __NETWORK_MEMP_NUM_TCP_PCB__ 16
/*--
this:AddWidget("Spinbox", 0, 100, "MEMP_NUM_TCP_PCB_LISTEN")
this:SetToolTip("MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP connections.")
--*/
#define __NETWORK_MEMP_NUM_TCP_PCB_LISTEN__ 8
/*--
this:AddWidget("Spinbox", 0, 100, "MEMP_NUM_TCP_SEG")
this:SetToolTip("MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP segments.")
--*/
#define __NETWORK_MEMP_NUM_TCP_SEG__ 48
/*--
this:AddWidget("Spinbox", 0, 100, "MEMP_NUM_REASSDATA")
this:SetToolTip("MEMP_NUM_REASSDATA: the number of IP packets simultaneously\nqueued for reassembly (whole packets, not fragments!)")
--*/
#define __NETWORK_MEMP_NUM_REASSDATA__ 32
/*--
this:AddWidget("Spinbox", 0, 100, "MEMP_NUM_FRAG_PBUF")
this:SetToolTip("MEMP_NUM_FRAG_PBUF: the number of IP fragments\nsimultaneously sent (fragments, not whole packets!).\n"..
                "This is only used with IP_FRAG_USES_STATIC_BUF==0\nand LWIP_NETIF_TX_SINGLE_PBUF==0 and only has\nto be > 1 with DMA-enabled MACs "..
                "where the packet is not yet sent when\nnetif->output returns.")
--*/
#define __NETWORK_MEMP_NUM_FRAG_PBUF__ 32
/*--
this:AddWidget("Spinbox", 0, 100, "MEMP_NUM_ARP_QUEUE")
this:SetToolTip("MEMP_NUM_ARP_QUEUE: the number of simulateously\nqueued outgoing packets (pbufs) that are waiting\n"..
                "for an ARP request (to resolve their destination\naddress) to finish. (requires the ARP_QUEUEING option)")
--*/
#define __NETWORK_MEMP_NUM_ARP_QUEUE__ 30
/*--
this:AddWidget("Spinbox", 0, 100, "MEMP_NUM_IGMP_GROUP")
this:SetToolTip("MEMP_NUM_IGMP_GROUP: The number of multicast\ngroups whose network interfaces\n"..
                "can be members at the same time (one per\nnetif - allsystems group -, plus one\n"..
                "per netif membership). (requires the LWIP_IGMP option)")
--*/
#define __NETWORK_MEMP_NUM_IGMP_GROUP__ 8
/*--
this:AddWidget("Spinbox", 0, 100, "MEMP_NUM_NETBUF")
this:SetToolTip("MEMP_NUM_NETBUF: the number of struct netbufs.")
--*/
#define __NETWORK_MEMP_NUM_NETBUF__ 32
/*--
this:AddWidget("Spinbox", 0, 100, "MEMP_NUM_NETCONN")
this:SetToolTip("MEMP_NUM_NETCONN: the number of struct netconns.")
--*/
#define __NETWORK_MEMP_NUM_NETCONN__ 16
/*--
this:AddWidget("Spinbox", 0, 100, "MEMP_NUM_SELECT_CB")
this:SetToolTip("MEMP_NUM_SELECT_CB: the number of struct lwip_select_cb.")
--*/
#define __NETWORK_MEMP_NUM_SELECT_CB__ 4
/*--
this:AddWidget("Spinbox", 1, 100, "MEMP_NUM_TCPIP_MSG_API")
this:SetToolTip("MEMP_NUM_TCPIP_MSG_API: the number of struct\ntcpip_msg, which are used for callback/timeout API communication.")
--*/
#define __NETWORK_MEMP_NUM_TCPIP_MSG_API__ 32
/*--
this:AddWidget("Spinbox", 1, 100, "MEMP_NUM_TCPIP_MSG_INPKT")
this:SetToolTip("MEMP_NUM_TCPIP_MSG_INPKT: the number of struct\ntcpip_msg, which are used for incoming packets.")
--*/
#define __NETWORK_MEMP_NUM_TCPIP_MSG_INPKT__ 32
/*--
this:AddWidget("Spinbox", 1, 100, "MEMP_NUM_NETDB")
this:SetToolTip("MEMP_NUM_NETDB: the number of concurrently running lwip_addrinfo() calls")
--*/
#define __NETWORK_MEMP_NUM_NETDB__ 1
/*--
this:AddWidget("Spinbox", 1, 100, "MEMP_NUM_LOCALHOSTLIST")
this:SetToolTip("MEMP_NUM_LOCALHOSTLIST: the number of host entries in the local host list if DNS_LOCAL_HOSTLIST_IS_DYNAMIC==1.")
--*/
#define __NETWORK_MEMP_NUM_LOCALHOSTLIST__ 1
/*--
this:AddWidget("Spinbox", 1, 100, "PBUF_POOL_SIZE")
this:SetToolTip("PBUF_POOL_SIZE: the number of buffers in the pbuf pool.")
--*/
#define __NETWORK_PBUF_POOL_SIZE__ 64


/*------------------------------------------------------------------------------
this:AddExtraWidget("Label", "LabelARP", "\nARP options", -1, "bold")
this:GoBackWidget("ARP")
++*/
/*--
this:AddWidget("Combobox", "LWIP_ARP")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_ARP==1: Enable ARP functionality.")
--*/
#define __NETWORK_LWIP_ARP__ 1
/*--
this:AddWidget("Spinbox", 0, 100, "ARP_TABLE_SIZE")
this:SetToolTip("ARP_TABLE_SIZE: Number of active MAC-IP address pairs cached.")
--*/
#define __NETWORK_ARP_TABLE_SIZE__ 10
/*--
this:AddWidget("Spinbox", 1, 1000, "ARP_MAXAGE")
this:SetToolTip("The time an ARP entry stays valid after its last update (seconds).")
--*/
#define __NETWORK_ARP_MAXAGE__ 300
/*--
this:AddWidget("Combobox", "ARP_QUEUEING")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("ARP_QUEUEING==1: Multiple outgoing packets are\nqueued during hardware address\n"..
                "resolution. By default, only the most recent\npacket is queued per IP address.\n"..
                "This is sufficient for most protocols and\nmainly reduces TCP connection\n"..
                "startup time. Set this to 1 if you know\nyour application sends more than one\n"..
                "packet in a row to an IP address that\nis not in the ARP cache.")
--*/
#define __NETWORK_ARP_QUEUEING__ 0
/*--
this:AddWidget("Spinbox", 0, 10, "ARP_QUEUE_LEN")
this:SetToolTip("The maximum number of packets which may be queued.")
--*/
#define __NETWORK_ARP_QUEUE_LEN__ 3
/*--
this:AddWidget("Combobox", "ETHARP_SUPPORT_VLAN")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("ETHARP_SUPPORT_VLAN==1: support receiving\nethernet packets with VLAN header.\n"..
                "Additionally, you can define ETHARP_VLAN_CHECK\nto an u16_t VLAN ID to check.\n"..
                "If ETHARP_VLAN_CHECK is defined, only\nVLAN-traffic for this VLAN is accepted.\n"..
                "If ETHARP_VLAN_CHECK is not defined,\nall traffic is accepted.\n"..
                "Alternatively, define a function/define\nETHARP_VLAN_CHECK_FN(eth_hdr, vlan)\n"..
                "that returns 1 to accept a packet or\n0 to drop a packet.")
--*/
#define __NETWORK_ETHARP_SUPPORT_VLAN__ 0
/*--
this:AddWidget("Spinbox", 0, 4, "ETH_PAD_SIZE")
this:SetToolTip("ETH_PAD_SIZE: number of bytes added before\nthe ethernet header to ensure\n"..
                "alignment of payload after that header.\nSince the header is 14 bytes long,\n"..
                "without this padding e.g. addresses in\nthe IP header will not be aligned\n"..
                "on a 32-bit boundary, so setting this to\n2 can speed up 32-bit-platforms.")
--*/
#define __NETWORK_ETH_PAD_SIZE__ 0
/*--
this:AddWidget("Combobox", "ETHARP_SUPPORT_STATIC_ENTRIES")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("ETHARP_SUPPORT_STATIC_ENTRIES==1: enable\ncode to support static ARP table\n"..
                "entries (using etharp_add_static_entry/\netharp_remove_static_entry).")
--*/
#define __NETWORK_ETHARP_SUPPORT_STATIC_ENTRIES__ 0


/*------------------------------------------------------------------------------
this:AddExtraWidget("Label", "LabelIP", "\nIP options", -1, "bold")
this:GoBackWidget("IP")
++*/
/*--
this:AddWidget("Combobox", "LWIP_IPV4")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_IPV4==1: Enable IPv4.")
--*/
#define __NETWORK_LWIP_IPV4__ 1
/*--
this:AddWidget("Combobox", "IP_FORWARD")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("IP_FORWARD==1: Enables the ability to\nforward IP packets across network\n"..
                "interfaces. If you are going to run\nlwIP on a device with only one network interface,\ndefine this to 0.")
--*/
#define __NETWORK_IP_FORWARD__ 0
/*--
this:AddWidget("Combobox", "IP_REASSEMBLY")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("IP_REASSEMBLY==1: Reassemble incoming\nfragmented IP packets. Note that\n"..
                "this option does not affect outgoing\npacket sizes, which can be controlled via IP_FRAG.")
--*/
#define __NETWORK_IP_REASSEMBLY__ 1
/*--
this:AddWidget("Combobox", "IP_FRAG")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("IP_FRAG==1: Fragment outgoing IP packets\nif their size exceeds MTU. Note\n"..
                "that this option does not affect incoming\npacket sizes, which can be controlled via IP_REASSEMBLY.")
--*/
#define __NETWORK_IP_FRAG__ 1
/*--
this:AddWidget("Combobox", "IP_OPTIONS_ALLOWED")
this:AddItem("No (0)", "0")
this:AddItem("Yes (1)", "1")
this:SetToolTip("IP_OPTIONS_ALLOWED: Defines the behavior for IP options.\n"..
                "IP_OPTIONS_ALLOWED==0: All packets with IP options are dropped.\n"..
                "IP_OPTIONS_ALLOWED==1: IP options are allowed (but not parsed).")
--*/
#define __NETWORK_IP_OPTIONS_ALLOWED__ 1
/*--
this:AddWidget("Spinbox", 1, 60, "IP_REASS_MAXAGE")
this:SetToolTip("IP_REASS_MAXAGE: Maximum time (in\nmultiples of IP_TMR_INTERVAL - so seconds, normally)\n"..
                "a fragmented IP packet waits for all\nfragments to arrive. If not all fragments arrived\n"..
                "in this time, the whole packet is discarded.")
--*/
#define __NETWORK_IP_REASS_MAXAGE__ 15
/*--
this:AddWidget("Spinbox", 1, 100, "IP_REASS_MAX_PBUFS")
this:SetToolTip("IP_REASS_MAX_PBUFS: Total maximum\namount of pbufs waiting to be reassembled.\n"..
                "Since the received pbufs are enqueued,\nbe sure to configure\n"..
                "PBUF_POOL_SIZE > IP_REASS_MAX_PBUFS\nso that the stack is still able to receive\n"..
                "packets even if the maximum amount\nof fragments is enqueued for reassembly!")
--*/
#define __NETWORK_IP_REASS_MAX_PBUFS__ 48
/*--
this:AddWidget("Spinbox", 1, 255, "IP_DEFAULT_TTL")
this:SetToolTip("IP_DEFAULT_TTL: Default value for Time-To-Live used by transport layers.")
--*/
#define __NETWORK_IP_DEFAULT_TTL__ 255
/*--
this:AddWidget("Combobox", "IP_SOF_BROADCAST")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("IP_SOF_BROADCAST=1: Use the SOF_BROADCAST\nfield to enable broadcast\n"..
                "filter per pcb on udp and raw send\noperations. To enable broadcast filter\n"..
                "on recv operations, you also have\nto set IP_SOF_BROADCAST_RECV=1.")
--*/
#define __NETWORK_IP_SOF_BROADCAST__ 0
/*--
this:AddWidget("Combobox", "IP_SOF_BROADCAST_RECV")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("IP_SOF_BROADCAST_RECV (requires IP_SOF_BROADCAST=1)\nenable the broadcast filter on recv operations.")
--*/
#define __NETWORK_IP_SOF_BROADCAST_RECV__ 0
/*--
this:AddWidget("Combobox", "IP_FORWARD_ALLOW_TX_ON_RX_NETIF")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("IP_FORWARD_ALLOW_TX_ON_RX_NETIF==1: allow\nip_forward() to send packets back\n"..
                "out on the netif where it was received.\nThis should only be used for wireless networks.\n"..
                "ATTENTION: When this is 1, make sure your\nnetif driver correctly marks incoming\n"..
                "link-layer-broadcast/multicast packets\nas such using the corresponding pbuf flags!")
--*/
#define __NETWORK_IP_FORWARD_ALLOW_TX_ON_RX_NETIF__ 0


/*------------------------------------------------------------------------------
this:AddExtraWidget("Label", "LabelICMP", "\nICMP options", -1, "bold")
this:GoBackWidget("ICMP")
++*/
/*--
this:AddWidget("Combobox", "LWIP_ICMP")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_ICMP==1: Enable ICMP module inside the IP stack.\nBe careful, disable that make your product non-compliant to RFC1122.")
--*/
#define __NETWORK_LWIP_ICMP__ 1
/*--
this:AddWidget("Spinbox", 1, 255, "ICMP_TTL")
this:SetToolTip("ICMP_TTL: Default value for Time-To-Live used by ICMP packets.")
--*/
#define __NETWORK_ICMP_TTL__ 255
/*--
this:AddWidget("Combobox", "LWIP_BROADCAST_PING")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_BROADCAST_PING==1: respond to broadcast pings (default is unicast only).")
--*/
#define __NETWORK_LWIP_BROADCAST_PING__ 0
/*--
this:AddWidget("Combobox", "LWIP_MULTICAST_PING")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_MULTICAST_PING==1: respond to multicast pings (default is unicast only)")
--*/
#define __NETWORK_LWIP_MULTICAST_PING__ 0


/*------------------------------------------------------------------------------
this:AddExtraWidget("Label", "LabelDHCP", "\nDHCP options", -1, "bold")
this:GoBackWidget("DHCP")
++*/
/*--
this:AddWidget("Combobox", "LWIP_DHCP")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_DHCP==1: Enable DHCP module.")
--*/
#define __NETWORK_LWIP_DHCP__ 1
/*--
this:AddWidget("Combobox", "LWIP_DHCP_BOOTP_FILE")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_DHCP_BOOTP_FILE==1: Store offered_si_addr and boot_file_name.")
--*/
#define __NETWORK_LWIP_DHCP_BOOTP_FILE__ 0
/*--
this:AddWidget("Combobox", "LWIP_DHCP_GET_NTP_SRV")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_DHCP_GETS_NTP==1: Request NTP servers with discover/select.")
--*/
#define __NETWORK_LWIP_DHCP_GET_NTP_SRV__ 0
/*--
this:AddWidget("Spinbox", 1, 255, "LWIP_DHCP_MAX_NTP_SERVERS")
this:SetToolTip("The maximum of NTP servers requested.")
--*/
#define __NETWORK_LWIP_DHCP_MAX_NTP_SERVERS__ 1


/*------------------------------------------------------------------------------
this:AddExtraWidget("Label", "LabelAUTOIP", "\nAUTOIP options", -1, "bold")
this:GoBackWidget("AUTOIP")
++*/
/*--
this:AddWidget("Combobox", "LWIP_AUTOIP")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_AUTOIP==1: Enable AUTOIP module.")
--*/
#define __NETWORK_LWIP_AUTOIP__ 0
/*--
this:AddWidget("Combobox", "LWIP_DHCP_AUTOIP_COOP")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_DHCP_AUTOIP_COOP==1: Allow DHCP and AUTOIP\nto be both enabled on the same interface at the same time.")
--*/
#define __NETWORK_LWIP_DHCP_AUTOIP_COOP__ 0
/*--
this:AddWidget("Spinbox", 1, 25, "LWIP_DHCP_AUTOIP_COOP_TRIES")
--*/
#define __NETWORK_LWIP_DHCP_AUTOIP_COOP_TRIES__ 9



/*------------------------------------------------------------------------------
this:AddExtraWidget("Label", "LabelIGMP", "\nIGMP options", -1, "bold")
this:GoBackWidget("IGMP")
++*/
/*--
this:AddWidget("Combobox", "LWIP_IGMP")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_IGMP==1: Turn on IGMP module.")
--*/
#define __NETWORK_LWIP_IGMP__ 1



/*------------------------------------------------------------------------------
this:AddExtraWidget("Label", "LabelDNS", "\nDNS options", -1, "bold")
this:GoBackWidget("DNS")
++*/
/*--
this:AddWidget("Combobox", "LWIP_DNS")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_DNS==1: Turn on DNS module.\nUDP must be available for DNS transport.")
--*/
#define __NETWORK_LWIP_DNS__ 1
/*--
this:AddWidget("Spinbox", 1, 16, "DNS_TABLE_SIZE")
this:SetToolTip("DNS maximum number of entries to maintain locally.")
--*/
#define __NETWORK_DNS_TABLE_SIZE__ 4
/*--
this:AddWidget("Spinbox", 16, 256, "DNS_MAX_NAME_LENGTH")
this:SetToolTip("DNS maximum host name length supported in the name table.")
--*/
#define __NETWORK_DNS_MAX_NAME_LENGTH__ 128
/*--
this:AddWidget("Spinbox", 1, 8, "DNS_MAX_SERVERS")
this:SetToolTip("The maximum of DNS servers.")
--*/
#define __NETWORK_DNS_MAX_SERVERS__ 2
/*--
this:AddWidget("Spinbox", 16, 256, "DNS_MAX_RETRIES")
this:SetToolTip("DNS maximum number of retries when asking for a name, before \"timeout\".")
--*/
#define __NETWORK_DNS_MAX_RETRIES__ 4
/*--
this:AddWidget("Combobox", "DNS_DOES_NAME_CHECK")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("DNS do a name checking between the query and the response.")
--*/
#define __NETWORK_DNS_DOES_NAME_CHECK__ 1
/*--
this:AddWidget("Combobox", "DNS_LOCAL_HOSTLIST_IS_DYNAMIC")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("If this is turned on, the local host-list\ncan be dynamically changed at runtime.")
--*/
#define __NETWORK_DNS_LOCAL_HOSTLIST_IS_DYNAMIC__ 0
/*--
this:AddWidget("Combobox", "LWIP_DNS_SUPPORT_MDNS_QUERIES")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("Set this to 1 to enable querying \".local\" names via mDNS using a One-Shot Multicast DNS Query.")
--*/
#define __NETWORK_LWIP_DNS_SUPPORT_MDNS_QUERIES__ 0



/*------------------------------------------------------------------------------
this:AddExtraWidget("Label", "LabelUDP", "\nUDP options", -1, "bold")
this:GoBackWidget("UDP")
++*/
/*--
this:AddWidget("Combobox", "LWIP_UDP")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_UDP==1: Turn on UDP.")
--*/
#define __NETWORK_LWIP_UDP__ 1
/*--
this:AddWidget("Spinbox", 1, 255, "UDP_TTL")
this:SetToolTip("UDP_TTL: Default Time-To-Live value.")
--*/
#define __NETWORK_UDP_TTL__ 255
/*--
this:AddWidget("Combobox", "LWIP_NETBUF_RECVINFO")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_NETBUF_RECVINFO==1: append destination\naddr and port to every netbuf.")
--*/
#define __NETWORK_LWIP_NETBUF_RECVINFO__ 0


/*------------------------------------------------------------------------------
this:AddExtraWidget("Label", "LabelTCP", "\nTCP options", -1, "bold")
this:GoBackWidget("TCP")
++*/
/*--
this:AddWidget("Combobox", "LWIP_TCP")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_TCP==1: Turn on TCP.")
--*/
#define __NETWORK_LWIP_TCP__ 1
/*--
this:AddWidget("Spinbox", 1, 255, "TCP_TTL")
this:SetToolTip("TCP_TTL: Default Time-To-Live value.")
--*/
#define __NETWORK_TCP_TTL__ 255
/*--
this:AddWidget("Spinbox", 1, 255, "TCP_WND")
this:SetToolTip("TCP_WND: The size of a TCP window (n * TCP_MSS).")
--*/
#define __NETWORK_TCP_WND__ 20
/*--
this:AddWidget("Spinbox", 0, 50, "TCP_MAXRTX")
this:SetToolTip("TCP_MAXRTX: Maximum number of retransmissions of data segments.")
--*/
#define __NETWORK_TCP_MAXRTX__ 12
/*--
this:AddWidget("Spinbox", 0, 50, "TCP_SYNMAXRTX")
this:SetToolTip("TCP_SYNMAXRTX: Maximum number of retransmissions of SYN segments.")
--*/
#define __NETWORK_TCP_SYNMAXRTX__ 6
/*--
this:AddWidget("Combobox", "LWIP_TCP_SACK_OUT")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_TCP_SACK_OUT==1: TCP will support sending selective acknowledgements (SACKs).")
--*/
#define __NETWORK_LWIP_TCP_SACK_OUT__ 0
/*--
this:AddWidget("Spinbox", 1, 16, "LWIP_TCP_MAX_SACK_NUM")
this:SetToolTip("LWIP_TCP_MAX_SACK_NUM: The maximum number of SACK values to include in TCP segments.")
--*/
#define __NETWORK_LWIP_TCP_MAX_SACK_NUM__ 4
/*--
this:AddWidget("Spinbox", 16, 65535, "TCP_MSS")
this:SetToolTip("TCP_MSS: TCP Maximum segment size. (default is 536, a conservative default, you might want to increase this.)")
--*/
#define __NETWORK_TCP_MSS__ 1460
/*--
this:AddWidget("Combobox", "TCP_CALCULATE_EFF_SEND_MSS")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("TCP_CALCULATE_EFF_SEND_MSS: \"The maximum size of a segment that TCP really\n"..
                "sends, the 'effective send MSS,' MUST be the smaller of the send MSS (which\n"..
                "reflects the available reassembly buffer size at the remote host) and the\n"..
                "largest size permitted by the IP layer\" (RFC 1122)\n"..
                "Setting this to 1 enables code that checks TCP_MSS against the MTU of the\n"..
                "netif used for a connection and limits the MSS if it would be too big otherwise.")
--*/
#define __NETWORK_TCP_CALCULATE_EFF_SEND_MSS__ 1
/*--
this:AddWidget("Spinbox", 2, 65535, "TCP_SND_BUF")
this:SetToolTip("TCP_SND_BUF: TCP sender buffer space (n * TCP_MSS).)")
--*/
#define __NETWORK_TCP_SND_BUF__ 4
/*--
this:AddWidget("Spinbox", 0, 65536, "TCP_OOSEQ_MAX_BYTES")
this:SetToolTip("TCP_OOSEQ_MAX_BYTES: The maximum number of bytes queued on ooseq per pcb.\n"..
                "Default is 0 (no limit). Only valid for TCP_QUEUE_OOSEQ==1.")
--*/
#define __NETWORK_TCP_OOSEQ_MAX_BYTES__ 0
/*--
this:AddWidget("Spinbox", 0, 65536, "TCP_OOSEQ_MAX_PBUFS")
this:SetToolTip("TCP_OOSEQ_MAX_PBUFS: The maximum number of pbufs queued on ooseq per pcb.\n"..
                "Default is 0 (no limit). Only valid for TCP_QUEUE_OOSEQ==1.")
--*/
#define __NETWORK_TCP_OOSEQ_MAX_PBUFS__ 0
/*--
this:AddWidget("Combobox", "TCP_LISTEN_BACKLOG")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("TCP_LISTEN_BACKLOG==1: Enable the backlog option for tcp listen pcb.")
--*/
#define __NETWORK_TCP_LISTEN_BACKLOG__ 0
/*--
this:AddWidget("Spinbox", 1, 255, "TCP_DEFAULT_LISTEN_BACKLOG")
this:SetToolTip("The maximum allowed backlog for TCP listen netconns.\n"..
                "This backlog is used unless another is explicitly specified. 255 is the maximum (u8_t).")
--*/
#define __NETWORK_TCP_DEFAULT_LISTEN_BACKLOG__ 255
/*--
this:AddWidget("Combobox", "TCP_OVERSIZE")
this:AddItem("0", "0")
this:AddItem("1", "1")
this:AddItem("128", "128")
this:AddItem("TCP_MSS", "TCP_MSS")
this:AddItem("TCP_MSS/4", "TCP_MSS/4")
this:SetToolTip("TCP_OVERSIZE: The maximum number of bytes that tcp_write may\n"..
                "allocate ahead of time in an attempt to create shorter pbuf chains\n"..
                "for transmission. The meaningful range is 0 to TCP_MSS. Some\n"..
                "suggested values are:\n"..
                "0:         Disable oversized allocation. Each tcp_write() allocates a new pbuf (old behaviour).\n"..
                "1:         Allocate size-aligned pbufs with minimal excess. Use this if your scatter-gather DMA requires aligned fragments.\n"..
                "128:       Limit the pbuf/memory overhead to 20%.\n"..
                "TCP_MSS:   Try to create unfragmented TCP packets.\n"..
                "TCP_MSS/4: Try to create 4 fragments or less per TCP packet.")
--*/
#define __NETWORK_TCP_OVERSIZE__ TCP_MSS
/*--
this:AddWidget("Combobox", "LWIP_TCP_TIMESTAMPS")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_TCP_TIMESTAMPS==1: support the TCP timestamp option.")
--*/
#define __NETWORK_LWIP_TCP_TIMESTAMPS__ 0
/*--
this:AddWidget("Combobox", "LWIP_WND_SCALE")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("Set LWIP_WND_SCALE to 1 to enable window scaling.")
--*/
#define __NETWORK_LWIP_WND_SCALE__ 0
/*--
this:AddWidget("Spinbox", 0, 14, "TCP_RCV_SCALE")
this:SetToolTip("TSet TCP_RCV_SCALE to the desired scaling factor (shift count in the range of [0..14]).")
--*/
#define __NETWORK_TCP_RCV_SCALE__ 0




/*------------------------------------------------------------------------------
this:AddExtraWidget("Label", "LabelPBUF", "\nPbuf options", -1, "bold")
this:GoBackWidget("PBUF")
++*/
/*--
this:AddWidget("Spinbox", 0, 65536, "PBUF_LINK_ENCAPSULATION_HLEN")
this:SetToolTip("PBUF_LINK_ENCAPSULATION_HLEN: the number of bytes that should be allocated\n"..
                "for an additional encapsulation header before ethernet headers (e.g. 802.11).")
--*/
#define __NETWORK_PBUF_LINK_ENCAPSULATION_HLEN__ 0



/*------------------------------------------------------------------------------
this:AddExtraWidget("Label", "LabelNETIF", "\nNETIF options", -1, "bold")
this:GoBackWidget("NETIF")
++*/
/*--
this:AddWidget("Combobox", "LWIP_SINGLE_NETIF")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_SINGLE_NETIF==1: use a single netif only.")
--*/
#define __NETWORK_LWIP_SINGLE_NETIF__ 1
/*--
this:AddWidget("Combobox", "LWIP_NETIF_HOSTNAME")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_NETIF_HOSTNAME==1: use DHCP_OPTION_HOSTNAME with netif's hostname field.")
--*/
#define __NETWORK_LWIP_NETIF_HOSTNAME__ 1
/*--
this:AddWidget("Combobox", "LWIP_NETIF_HWADDRHINT")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_NETIF_HWADDRHINT==1: Cache link-layer-address hints (e.g. table\n"..
                "indices) in struct netif. TCP and UDP can make use of this to prevent\n"..
                "scanning the ARP table for every sent packet. While this is faster for big\n"..
                "ARP tables or many concurrent connections, it might be counterproductive\n"..
                "if you have a tiny ARP table or if there never are concurrent connections.")
--*/
#define __NETWORK_LWIP_NETIF_HWADDRHINT__ 0



/*------------------------------------------------------------------------------
this:AddExtraWidget("Label", "LabelLOOPIF", "\nLOOPIF options", -1, "bold")
this:GoBackWidget("LOOPIF")
++*/
/*--
this:AddWidget("Combobox", "LWIP_LOOPIF_MULTICAST")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_LOOPIF_MULTICAST==1: Support multicast/IGMP on loop interface (127.0.0.1).")
--*/
#define __NETWORK_LWIP_LOOPIF_MULTICAST__ 0
/*--
this:AddWidget("Combobox", "LWIP_NETIF_LOOPBACK")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_NETIF_LOOPBACK==1: Support sending packets with a destination IP\n"..
                "address equal to the netif IP address, looping them back up the stack.")
--*/
#define __NETWORK_LWIP_NETIF_LOOPBACK__ 0
/*--
this:AddWidget("Spinbox", 0, 100, "LWIP_LOOPBACK_MAX_PBUFS")
this:SetToolTip("LWIP_LOOPBACK_MAX_PBUFS: Maximum number of pbufs\non queue for loopback sending for each netif (0 = disabled).")
--*/
#define __NETWORK_LWIP_LOOPBACK_MAX_PBUFS__ 0



/*------------------------------------------------------------------------------
this:AddExtraWidget("Label", "LabelThr", "\nThread options", -1, "bold")
this:GoBackWidget("THR")
++*/
/*--
this:AddWidget("Editline", true, "TCPIP_THREAD_NAME")
this:SetToolTip("TCPIP_THREAD_NAME: The name assigned to the main tcpip thread.")
--*/
#define __NETWORK_TCPIP_THREAD_NAME__ "tcpipd"
/*--
this:AddWidget("Combobox", "TCPIP_THREAD_STACKSIZE")
this:AddItem("Low stack size", "STACK_DEPTH_LOW")
this:AddItem("Medium stack size", "STACK_DEPTH_MEDIUM")
this:AddItem("Large stack size", "STACK_DEPTH_LARGE")
this:AddItem("Very large stack size", "STACK_DEPTH_VERY_LARGE")
this:AddItem("Huge stack size", "STACK_DEPTH_HUGE")
this:AddItem("Very huge stack size", "STACK_DEPTH_VERY_HUGE")
this:SetToolTip("TCPIP_THREAD_STACKSIZE: The stack size used by the main tcpip thread.\n"..
                "The stack size value itself is platform-dependent, but is passed to\n"..
                "sys_thread_new() when the thread is created.")
--*/
#define __NETWORK_TCPIP_THREAD_STACKSIZE__ STACK_DEPTH_LOW
/*--
this:AddWidget("Spinbox", -128, 127, "TCPIP_THREAD_PRIO")
this:SetToolTip("TCPIP_THREAD_PRIO: The priority assigned to the main tcpip thread.\n"..
                "The priority value itself is platform-dependent, but is passed to\n"..
                "sys_thread_new() when the thread is created.")
--*/
#define __NETWORK_TCPIP_THREAD_PRIO__ 0
/*--
this:AddWidget("Spinbox", 1, 128, "TCPIP_MBOX_SIZE")
this:SetToolTip("TCPIP_MBOX_SIZE: The mailbox size for the tcpip thread messages\n"..
                "The queue size value itself is platform-dependent, but is passed\n"..
                "to sys_mbox_new() when tcpip_init is called.")
--*/
#define __NETWORK_TCPIP_MBOX_SIZE__ 8
/*--
this:AddWidget("Editline", true, "SLIPIF_THREAD_NAME")
this:SetToolTip("SLIPIF_THREAD_NAME: The name assigned to the slipif_loop thread.")
--*/
#define __NETWORK_SLIPIF_THREAD_NAME__ "slipifd"
/*--
this:AddWidget("Combobox", "SLIPIF_THREAD_STACKSIZE")
this:AddItem("Low stack size", "STACK_DEPTH_LOW")
this:AddItem("Medium stack size", "STACK_DEPTH_MEDIUM")
this:AddItem("Large stack size", "STACK_DEPTH_LARGE")
this:AddItem("Very large stack size", "STACK_DEPTH_VERY_LARGE")
this:AddItem("Huge stack size", "STACK_DEPTH_HUGE")
this:AddItem("Very huge stack size", "STACK_DEPTH_VERY_HUGE")
this:SetToolTip("SLIPIF_THREAD_STACKSIZE: The stack size used by the main tcpip thread.\n"..
                "The stack size value itself is platform-dependent, but is passed to\n"..
                "sys_thread_new() when the thread is created.")
--*/
#define __NETWORK_SLIPIF_THREAD_STACKSIZE__ STACK_DEPTH_LOW
/*--
this:AddWidget("Spinbox", -128, 127, "SLIPIF_THREAD_PRIO")
this:SetToolTip("SLIPIF_THREAD_PRIO: The priority assigned to the main tcpip thread.\n"..
                "The priority value itself is platform-dependent, but is passed to\n"..
                "sys_thread_new() when the thread is created.")
--*/
#define __NETWORK_SLIPIF_THREAD_PRIO__ 0
/*--
this:AddWidget("Editline", true, "DEFAULT_THREAD_NAME")
this:SetToolTip("DEFAULT_THREAD_NAME: The name assigned to any other lwIP thread.")
--*/
#define __NETWORK_DEFAULT_THREAD_NAME__ "lwipd"
/*--
this:AddWidget("Combobox", "DEFAULT_THREAD_STACKSIZE")
this:AddItem("Low stack size", "STACK_DEPTH_LOW")
this:AddItem("Medium stack size", "STACK_DEPTH_MEDIUM")
this:AddItem("Large stack size", "STACK_DEPTH_LARGE")
this:AddItem("Very large stack size", "STACK_DEPTH_VERY_LARGE")
this:AddItem("Huge stack size", "STACK_DEPTH_HUGE")
this:AddItem("Very huge stack size", "STACK_DEPTH_VERY_HUGE")
this:SetToolTip("DEFAULT_THREAD_STACKSIZE: The stack size used by the main tcpip thread.\n"..
                "The stack size value itself is platform-dependent, but is passed to\n"..
                "sys_thread_new() when the thread is created.")
--*/
#define __NETWORK_DEFAULT_THREAD_STACKSIZE__ STACK_DEPTH_LOW
/*--
this:AddWidget("Spinbox", -128, 127, "DEFAULT_THREAD_PRIO")
this:SetToolTip("DEFAULT_THREAD_PRIO: The priority assigned to the main tcpip thread.\n"..
                "The priority value itself is platform-dependent, but is passed to\n"..
                "sys_thread_new() when the thread is created.")
--*/
#define __NETWORK_DEFAULT_THREAD_PRIO__ 0
/*--
this:AddWidget("Spinbox", 1, 128, "RAW_RECVMBOX_SIZE")
this:SetToolTip("DEFAULT_RAW_RECVMBOX_SIZE: The mailbox size for the incoming packets on a\n"..
                "NETCONN_RAW. The queue size value itself is platform-dependent, but is passed\n"..
                "to sys_mbox_new() when the recvmbox is created.")
--*/
#define __NETWORK_DEFAULT_RAW_RECVMBOX_SIZE__ 8
/*--
this:AddWidget("Spinbox", 1, 128, "UDP_RECVMBOX_SIZE")
this:SetToolTip("DEFAULT_UDP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a\n"..
                "NETCONN_UDP. The queue size value itself is platform-dependent, but is passed\n"..
                "to sys_mbox_new() when the recvmbox is created.")
--*/
#define __NETWORK_DEFAULT_UDP_RECVMBOX_SIZE__ 8
/*--
this:AddWidget("Spinbox", 1, 128, "TCP_RECVMBOX_SIZE")
this:SetToolTip("DEFAULT_TCP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a\n"..
                "NETCONN_TCP. The queue size value itself is platform-dependent, but is passed\n"..
                "to sys_mbox_new() when the recvmbox is created.")
--*/
#define __NETWORK_DEFAULT_TCP_RECVMBOX_SIZE__ 8
/*--
this:AddWidget("Spinbox", 1, 128, "ACCEPTMBOX_SIZE")
this:SetToolTip("DEFAULT_ACCEPTMBOX_SIZE: The mailbox size for the incoming connections.\n"..
                "The queue size value itself is platform-dependent, but is passed to\n"..
                "sys_mbox_new() when the acceptmbox is created.")
--*/
#define __NETWORK_DEFAULT_ACCEPTMBOX_SIZE__ 8


/*------------------------------------------------------------------------------
this:AddExtraWidget("Label", "LabelSEQL", "\nSequential Layer options", -1, "bold")
this:GoBackWidget("SEQL")
++*/
/*--
this:AddWidget("Combobox", "LWIP_TCPIP_TIMEOUT")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_TCPIP_TIMEOUT==1: Enable tcpip_timeout/tcpip_untimeout\nto create timers running in tcpip_thread from another thread.")
--*/
#define __NETWORK_LWIP_TCPIP_TIMEOUT__ 0



/*------------------------------------------------------------------------------
this:AddExtraWidget("Label", "LabelSocket", "\nSocket options", -1, "bold")
this:GoBackWidget("Socket")
++*/
/*--
this:AddWidget("Combobox", "LWIP_TCP_KEEPALIVE")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_TCP_KEEPALIVE==1: Enable TCP_KEEPIDLE, TCP_KEEPINTVL and TCP_KEEPCNT options processing (seconds).")
--*/
#define __NETWORK_LWIP_TCP_KEEPALIVE__ 0



/*------------------------------------------------------------------------------
this:AddExtraWidget("Label", "LabelStats", "\nStatistics options", -1, "bold")
this:GoBackWidget("Stats")
++*/
/*--
this:AddWidget("Combobox", "LWIP_STATS")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_STATS==1: Enable statistics collection in lwip_stats.")
--*/
#define __NETWORK_LWIP_STATS__ 0



/*------------------------------------------------------------------------------
this:AddExtraWidget("Label", "LabelChS", "\nChecksum options", -1, "bold")
this:GoBackWidget("ChS")
++*/
/*--
this:AddWidget("Combobox", "LWIP_CHECKSUM_CTRL_PER_NETIF")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_CHECKSUM_CTRL_PER_NETIF==1: Checksum generation/check can be enabled/disabled per netif.\n"..
                "ATTENTION: if enabled, the CHECKSUM_GEN_* and CHECKSUM_CHECK_* defines must be enabled!")
--*/
#define __NETWORK_LWIP_CHECKSUM_CTRL_PER_NETIF__ 0
/*--
this:AddWidget("Combobox", "CHECKSUM_GEN_IP")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.")
--*/
#define __NETWORK_CHECKSUM_GEN_IP__ 0
/*--
this:AddWidget("Combobox", "CHECKSUM_GEN_UDP")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.")
--*/
#define __NETWORK_CHECKSUM_GEN_UDP__ 0
/*--
this:AddWidget("Combobox", "CHECKSUM_GEN_TCP")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.")
--*/
#define __NETWORK_CHECKSUM_GEN_TCP__ 0
/*--
this:AddWidget("Combobox", "CHECKSUM_GEN_ICMP")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("CHECKSUM_GEN_ICMP==1: Generate checksums in software for outgoing ICMP packets.")
--*/
#define __NETWORK_CHECKSUM_GEN_ICMP__ 0
/*--
this:AddWidget("Combobox", "CHECKSUM_GEN_ICMP6")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("CHECKSUM_GEN_ICMP6==1: Generate checksums in software for outgoing ICMP6 packets.")
--*/
#define __NETWORK_CHECKSUM_GEN_ICMP6__ 0
/*--
this:AddWidget("Combobox", "CHECKSUM_CHECK_IP")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.")
--*/
#define __NETWORK_CHECKSUM_CHECK_IP__ 1
/*--
this:AddWidget("Combobox", "CHECKSUM_CHECK_UDP")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.")
--*/
#define __NETWORK_CHECKSUM_CHECK_UDP__ 1
/*--
this:AddWidget("Combobox", "CHECKSUM_CHECK_TCP")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.")
--*/
#define __NETWORK_CHECKSUM_CHECK_TCP__ 1
/*--
this:AddWidget("Combobox", "CHECKSUM_CHECK_ICMP")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("CHECKSUM_CHECK_ICMP==1: Check checksums in software for incoming ICMP packets.")
--*/
#define __NETWORK_CHECKSUM_CHECK_ICMP__ 1
/*--
this:AddWidget("Combobox", "CHECKSUM_CHECK_ICMP6")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("CHECKSUM_CHECK_ICMP6==1: Check checksums in software for incoming ICMPv6 packets")
--*/
#define __NETWORK_CHECKSUM_CHECK_ICMP6__ 1
/*--
this:AddWidget("Combobox", "LWIP_CHECKSUM_ON_COPY")
this:AddItem("Disable (0)", "0")
this:AddItem("Enable (1)", "1")
this:SetToolTip("LWIP_CHECKSUM_ON_COPY==1: Calculate checksum when\ncopying data from application buffers to pbufs.")
--*/
#define __NETWORK_LWIP_CHECKSUM_ON_COPY__ 0


/*------------------------------------------------------------------------------
this:AddExtraWidget("Label", "LabelDebug", "\nDebug options", -1, "bold")
this:GoBackWidget("Debug")
++*/
/*--
this:AddWidget("Combobox", "LWIP_DBG_MIN_LEVEL")
this:AddItem("All", "LWIP_DBG_LEVEL_ALL")
this:AddItem("Warnings", "LWIP_DBG_LEVEL_WARNING")
this:AddItem("Serious", "LWIP_DBG_LEVEL_SERIOUS")
this:AddItem("Severe", "LWIP_DBG_LEVEL_SEVERE")
this:SetToolTip("LWIP_DBG_MIN_LEVEL: After masking, the value of the debug is\n"..
                "compared against this value. If it is smaller, then debugging messages are written.")
--*/
#define __NETWORK_LWIP_DBG_MIN_LEVEL__ LWIP_DBG_LEVEL_ALL
/*--
this:AddWidget("Combobox", "LWIP_DBG_TYPES_ON")
this:AddDebugItems()
--*/
#define __NETWORK_LWIP_DBG_TYPES_ON__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "ETHARP_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_ETHARP_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "NETIF_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_NETIF_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "PBUF_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_PBUF_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "API_LIB_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_API_LIB_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "API_MSG_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_API_MSG_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "SOCKETS_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_SOCKETS_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "ICMP_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_ICMP_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "IGMP_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_IGMP_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "INET_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_INET_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "IP_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_IP_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "IP_REASS_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_IP_REASS_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "RAW_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_RAW_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "MEM_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_MEM_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "MEMP_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_MEMP_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "SYS_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_SYS_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "TIMERS_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_TIMERS_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "TCP_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_TCP_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "TCP_INPUT_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_TCP_INPUT_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "TCP_FR_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_TCP_FR_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "TCP_RTO_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_TCP_RTO_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "TCP_CWND_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_TCP_CWND_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "TCP_WND_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_TCP_WND_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "TCP_OUTPUT_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_TCP_OUTPUT_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "TCP_RST_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_TCP_RST_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "TCP_QLEN_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_TCP_QLEN_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "UDP_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_UDP_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "TCPIP_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_TCPIP_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "SLIP_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_SLIP_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "DHCP_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_DHCP_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "AUTOIP_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_AUTOIP_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "DNS_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_DNS_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "IP6_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_IP6_DEBUG__ LWIP_DBG_OFF
/*--
this:AddWidget("Combobox", "DHCP6_DEBUG")
this:AddDebugItems()
--*/
#define __NETWORK_DHCP6_DEBUG__ LWIP_DBG_OFF

#endif /* _LWIP_FLAGS_H */
/*==============================================================================
  End of file
==============================================================================*/
