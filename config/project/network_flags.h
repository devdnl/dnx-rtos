/*=========================================================================*//**
@file    flags.h

@author  Daniel Zorychta

@brief   Global project flags which are included from compiler command line

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


*//*==========================================================================*/

/*
 * NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as suffix
 *       and prefix) are exported to the single configuration file
 *       (by using Configtool) when entire project configuration is exported.
 *       All other flag definitions and statements are ignored.
 */

#ifndef _NETWORK_FLAGS_H_
#define _NETWORK_FLAGS_H_

/* basic options */
#define __NETWORK_MAC_ADDR_0__ 0x50
#define __NETWORK_MAC_ADDR_1__ 0xE5
#define __NETWORK_MAC_ADDR_2__ 0x49
#define __NETWORK_MAC_ADDR_3__ 0x37
#define __NETWORK_MAC_ADDR_4__ 0xB5
#define __NETWORK_MAC_ADDR_5__ 0xBD
#define __NETWORK_ETHIF_FILE__ "/dev/eth0"

/* memory options */
#define __NETWORK_MEMP_SEPARATE_POOLS__ 0
#define __NETWORK_MEMP_OVERFLOW_CHECK__ 0
#define __NETWORK_MEMP_SANITY_CHECK__ 0

/* internal memory pool sizes */
#define __NETWORK_MEMP_NUM_PBUF__ 10
#define __NETWORK_MEMP_NUM_RAW_PCB__ 4
#define __NETWORK_MEMP_NUM_UDP_PCB__ 4
#define __NETWORK_MEMP_NUM_TCP_PCB__ 10
#define __NETWORK_MEMP_NUM_TCP_PCB_LISTEN__ 8
#define __NETWORK_MEMP_NUM_TCP_SEG__ 16
#define __NETWORK_MEMP_NUM_REASSDATA__ 5
#define __NETWORK_MEMP_NUM_FRAG_PBUF__ 15
#define __NETWORK_MEMP_NUM_ARP_QUEUE__ 30
#define __NETWORK_MEMP_NUM_IGMP_GROUP__ 8
#define __NETWORK_MEMP_NUM_NETBUF__ 2
#define __NETWORK_MEMP_NUM_NETCONN__ 4
#define __NETWORK_MEMP_NUM_TCPIP_MSG_API__ 8
#define __NETWORK_MEMP_NUM_TCPIP_MSG_INPKT__ 8
#define __NETWORK_MEMP_NUM_SNMP_NODE__ 50
#define __NETWORK_MEMP_NUM_SNMP_ROOTNODE__ 30
#define __NETWORK_MEMP_NUM_LOCALHOSTLIST__ 1
#define __NETWORK_MEMP_NUM_PPPOE_INTERFACES__ 1
#define __NETWORK_PBUF_POOL_SIZE__ 10

/* ARP options */
#define __NETWORK_LWIP_ARP__ 1
#define __NETWORK_ARP_TABLE_SIZE__ 10
#define __NETWORK_ARP_QUEUEING__ 0
#define __NETWORK_ETHARP_TRUST_IP_MAC__ 0
#define __NETWORK_ETHARP_SUPPORT_VLAN__ 0
#define __NETWORK_ETH_PAD_SIZE__ 0
#define __NETWORK_ETHARP_SUPPORT_STATIC_ENTRIES__ 0

/* IP options */
#define __NETWORK_IP_FORWARD__ 0
#define __NETWORK_IP_OPTIONS_ALLOWED__ 1
#define __NETWORK_IP_REASSEMBLY__ 1
#define __NETWORK_IP_FRAG__ 1
#define __NETWORK_IP_REASS_MAXAGE__ 3
#define __NETWORK_IP_REASS_MAX_PBUFS__ 10
#define __NETWORK_IP_DEFAULT_TTL__ 255
#define __NETWORK_IP_SOF_BROADCAST__ 0
#define __NETWORK_IP_SOF_BROADCAST_RECV__ 0
#define __NETWORK_IP_FORWARD_ALLOW_TX_ON_RX_NETIF__ 0

/* ICMP options */
#define __NETWORK_LWIP_ICMP__ 1
#define __NETWORK_ICMP_TTL__ 255
#define __NETWORK_LWIP_BROADCAST_PING__ 1
#define __NETWORK_LWIP_MULTICAST_PING__ 1

/* DHCP options */
#define __NETWORK_LWIP_DHCP__ 1

/* AUTOIP options */
#define __NETWORK_LWIP_AUTOIP__ 0
#define __NETWORK_LWIP_DHCP_AUTOIP_COOP__ 0
#define __NETWORK_LWIP_DHCP_AUTOIP_COOP_TRIES__ 9

/* SNMP options */
#define __NETWORK_LWIP_SNMP__ 0
#define __NETWORK_SNMP_CONCURRENT_REQUESTS__ 1
#define __NETWORK_SNMP_TRAP_DESTINATIONS__ 1
#define __NETWORK_SNMP_MAX_OCTET_STRING_LEN__ 127
#define __NETWORK_SNMP_MAX_TREE_DEPTH__ 15

/* IGMP options */
#define __NETWORK_LWIP_IGMP__ 0

/* DNS options */
#define __NETWORK_LWIP_DNS__ 0
#define __NETWORK_DNS_TABLE_SIZE__ 4
#define __NETWORK_DNS_MAX_NAME_LENGTH__ 256
#define __NETWORK_DNS_MAX_SERVERS__ 2
#define __NETWORK_DNS_DOES_NAME_CHECK__ 1
#define __NETWORK_DNS_LOCAL_HOSTLIST_IS_DYNAMIC__ 0

/* UDP options */
#define __NETWORK_LWIP_UDP__ 1
#define __NETWORK_UDP_TTL__ 255
#define __NETWORK_LWIP_NETBUF_RECVINFO__ 0

/* TCP options */
#define __NETWORK_LWIP_TCP__ 1
#define __NETWORK_TCP_TTL__ 255
#define __NETWORK_TCP_MAXRTX__ 12
#define __NETWORK_TCP_SYNMAXRTX__ 6
#define __NETWORK_TCP_QUEUE_OOSEQ__ 0
#define __NETWORK_TCP_MSS__ 1460
#define __NETWORK_TCP_CALCULATE_EFF_SEND_MSS__ 1
#define __NETWORK_TCP_OOSEQ_MAX_BYTES__ 0
#define __NETWORK_TCP_OOSEQ_MAX_PBUFS__ 0
#define __NETWORK_TCP_LISTEN_BACKLOG__ 0
#define __NETWORK_TCP_DEFAULT_LISTEN_BACKLOG__ 255
#define __NETWORK_TCP_OVERSIZE__ TCP_MSS
#define __NETWORK_LWIP_TCP_TIMESTAMPS__ 0

/* Network Interfaces options */
#define __NETWORK_LWIP_NETIF_HOSTNAME__ 1
#define __NETWORK_LWIP_NETIF_HWADDRHINT__ 0
#define __NETWORK_LWIP_NETIF_LOOPBACK__ 0
#define __NETWORK_LWIP_LOOPBACK_MAX_PBUFS__ 0

/* LOOPIF options */
#define __NETWORK_LWIP_HAVE_LOOPIF__ 0

/* SLIPIF options */
#define __NETWORK_LWIP_HAVE_SLIPIF__ 0

/* thread options */
#define __NETWORK_TCPIP_THREAD_NAME__ "tcpipd"
#define __NETWORK_TCPIP_THREAD_STACKSIZE__ STACK_DEPTH_LOW
#define __NETWORK_TCPIP_THREAD_PRIO__ 0
#define __NETWORK_TCPIP_MBOX_SIZE__ 8
#define __NETWORK_SLIPIF_THREAD_NAME__ "slipifd"
#define __NETWORK_SLIPIF_THREAD_STACKSIZE__ STACK_DEPTH_LOW
#define __NETWORK_SLIPIF_THREAD_PRIO__ 0
#define __NETWORK_PPP_THREAD_NAME__ "pppind"
#define __NETWORK_PPP_THREAD_STACKSIZE__ STACK_DEPTH_LOW
#define __NETWORK_PPP_THREAD_PRIO__ 0
#define __NETWORK_DEFAULT_THREAD_NAME__ "lwipd"
#define __NETWORK_DEFAULT_THREAD_STACKSIZE__ STACK_DEPTH_LOW
#define __NETWORK_DEFAULT_THREAD_PRIO__ 0
#define __NETWORK_DEFAULT_RAW_RECVMBOX_SIZE__ 8
#define __NETWORK_DEFAULT_UDP_RECVMBOX_SIZE__ 8
#define __NETWORK_DEFAULT_TCP_RECVMBOX_SIZE__ 8
#define __NETWORK_DEFAULT_ACCEPTMBOX_SIZE__ 8

/* Sequential layer options */
#define __NETWORK_LWIP_TCPIP_TIMEOUT__ 1

/* PPP options */
#define __NETWORK_PPP_SUPPORT__ 0
#define __NETWORK_PPPOE_SUPPORT__ 0
#define __NETWORK_NUM_PPP__ 1
#define __NETWORK_PAP_SUPPORT__ 0
#define __NETWORK_CHAP_SUPPORT__ 0
#define __NETWORK_VJ_SUPPORT__ 0
#define __NETWORK_MD5_SUPPORT__ 0
#define __NETWORK_FSM_DEFTIMEOUT__ 6
#define __NETWORK_FSM_DEFMAXTERMREQS__ 2
#define __NETWORK_FSM_DEFMAXCONFREQS__ 10
#define __NETWORK_FSM_DEFMAXNAKLOOPS__ 5
#define __NETWORK_UPAP_DEFTIMEOUT__ 6
#define __NETWORK_UPAP_DEFREQTIME__ 30
#define __NETWORK_CHAP_DEFTIMEOUT__ 6
#define __NETWORK_CHAP_DEFTRANSMITS__ 10
#define __NETWORK_LCP_ECHOINTERVAL__ 0
#define __NETWORK_LCP_MAXECHOFAILS__ 3
#define __NETWORK_PPP_MAXIDLEFLAG__ 100

/* checksum options */
#define __NETWORK_LWIP_CHECKSUM_ON_COPY__ 0

/* debug options */
#define __NETWORK_LWIP_DBG_MIN_LEVEL__ LWIP_DBG_LEVEL_ALL
#define __NETWORK_LWIP_DBG_TYPES_ON__ LWIP_DBG_OFF
#define __NETWORK_LOW_LEVEL_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_ETHARP_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_NETIF_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_PBUF_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_API_LIB_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_API_MSG_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_SOCKETS_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_ICMP_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_IGMP_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_INET_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_IP_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_IP_REASS_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_RAW_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_MEM_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_MEMP_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_SYS_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_TIMERS_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_TCP_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_TCP_INPUT_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_TCP_FR_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_TCP_RTO_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_TCP_CWND_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_TCP_WND_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_TCP_OUTPUT_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_TCP_RST_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_TCP_QLEN_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_UDP_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_TCPIP_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_PPP_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_SLIP_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_DHCP_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_AUTOIP_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_SNMP_MSG_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_SNMP_MIB_DEBUG__ LWIP_DBG_OFF
#define __NETWORK_DNS_DEBUG__ LWIP_DBG_OFF

#endif /* _NETWORK_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
