/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#include "config.h"
#include "system/dnx.h"

/*
   -----------------------------------------------
   ---------- Platform specific locking ----------
   -----------------------------------------------
*/
/*
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT                    0

/*
 * NO_SYS==1: Provides VERY minimal functionality. Otherwise,
 * use lwIP facilities.
 */
#define NO_SYS                                  0

/*
 * LWIP_NOASSERT: comment to enable asserts
 */
#define LWIP_NOASSERT                           0

/*
 * NO_SYS_NO_TIMERS==1: Drop support for sys_timeout when NO_SYS==1
 * Mainly for compatibility to old versions.
 */
#define NO_SYS_NO_TIMERS                        0


/*
   ------------------------------------------------
   ---------- Network Interfaces options ----------
   ------------------------------------------------
*/
/*
 * LWIP_NETIF_HOSTNAME==1: use DHCP_OPTION_HOSTNAME with netif's hostname
 * field.
 */
#define LWIP_NETIF_HOSTNAME                     1

/*
 * LWIP_NETIF_API==1: Support netif api (in netifapi.c)
 */
#define LWIP_NETIF_API                          1


/*
   ------------------------------------
   ---------- Memory options ----------
   ------------------------------------
*/
/*
 * MEM_ALIGNMENT: should be set to the alignment of the CPU for which
 * lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
 * byte alignment -> define MEM_ALIGNMENT to 2.
 */
#define MEM_ALIGNMENT                           CONFIG_HEAP_ALIGN

/*
 * MEM_LIBC_MALLOC==1: Use malloc/free/realloc provided by your C-library
 * instead of the lwip internal allocator. Can save code size if you
 * already use it.
 */
#define MEM_LIBC_MALLOC                         1

/*
 * MEMP_MEM_MALLOC==1: Use mem_malloc/mem_free instead of the lwip pool allocator.
 * Especially useful with MEM_LIBC_MALLOC but handle with care regarding execution
 * speed and usage from interrupts!
 */
#define MEMP_MEM_MALLOC                         1

/*
 * Memory allocation functions
 */
#define mem_free                                sysm_netfree
#define mem_malloc                              sysm_netmalloc
#define mem_calloc                              sysm_netcalloc

/*
   ------------------------------------------------
   ---------- Internal Memory Pool Sizes ----------
   ------------------------------------------------
*/
/*
 * MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
 * sends a lot of data out of ROM (or other static memory), this
 * should be set high.
 */
#define MEMP_NUM_PBUF                           10

/*
 * MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
 * per active UDP "connection".
 */
#define MEMP_NUM_UDP_PCB                        4

/*
 * MEMP_NUM_TCP_PCB: the number of simulatenously active TCP
 * connections.
 */
#define MEMP_NUM_TCP_PCB                        10

/*
 * MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP
 * connections.
 */
#define MEMP_NUM_TCP_PCB_LISTEN                 6

/*
 * MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP
 * segments.
 */
#define MEMP_NUM_TCP_SEG                        12

/*
   ----------------------------------
   ---------- Pbuf options ----------
   ----------------------------------
*/
/*
 * PBUF_POOL_SIZE: the number of buffers in the pbuf pool.
 */
#define PBUF_POOL_SIZE                          10

/*
 * PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool.
 */
#define PBUF_POOL_BUFSIZE                       1500


/*
   ---------------------------------
   ---------- TCP options ----------
   ---------------------------------
*/
/*
 * LWIP_TCP==1: Turn on TCP.
 */
#define LWIP_TCP                                1

/*
 * TCP_TTL: Default Time-To-Live value.
 */
#define TCP_TTL                                 255

/*
 * Controls if TCP should queue segments that arrive out of
 * order. Define to 0 if your device is low on memory.
 */
#define TCP_QUEUE_OOSEQ                         0

/*
 * TCP Maximum segment size.
 */
#define TCP_MSS                                 (1500 - 40)  /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */

/*
 * TCP maximum segment lifetime in milliseconds
 */
#define TCP_MSL                                 5000

/*
 * TCP sender buffer space (bytes).
 */
#define TCP_SND_BUF                             (2*TCP_MSS)

/*
 * TCP sender buffer space (pbufs).
 * This must be at least = 2 * TCP_SND_BUF/TCP_MSS for things to work.
 */
#define TCP_SND_QUEUELEN                        (6 * TCP_SND_BUF)/TCP_MSS

/*
 * TCP receive window.
 */
#define TCP_WND                                 (2*TCP_MSS)


/*
   ----------------------------------
   ---------- ICMP options ----------
   ----------------------------------
*/
/*
 * LWIP_ICMP==1: Enable ICMP module inside the IP stack.
 * Be careful, disable that make your product non-compliant to RFC1122
 */
#define LWIP_ICMP                               1


/*
   ----------------------------------
   ---------- DHCP options ----------
   ----------------------------------
*/
/*
 * LWIP_DHCP==1: Enable DHCP module.
 */
#define LWIP_DHCP                               1


/*
   ---------------------------------
   ---------- UDP options ----------
   ---------------------------------
*/
/*
 * LWIP_UDP==1: Turn on UDP.
 */
#define LWIP_UDP                                1

/*
 * UDP_TTL: Default Time-To-Live value.
 */
#define UDP_TTL                                 255


/*
   ----------------------------------------
   ---------- Statistics options ----------
   ----------------------------------------
*/
/*
 * LWIP_STATS==1: Enable statistics collection in lwip_stats.
 */
#define LWIP_STATS                              0

/*
   --------------------------------------
   ---------- Checksum options ----------
   --------------------------------------
*/
/*
The STM32F107 allows computing and verifying the IP, UDP, TCP and ICMP checksums by hardware:
 - To use this feature set following define to 1.
 - To disable it and process by CPU set to 0.
 - Make sure that ETH driver support hardware checksum calculation.
*/
#define LWIP_CHECKSUM_BY_HARDWARE               1

#if (LWIP_CHECKSUM_BY_HARDWARE != 0)
  /* CHECKSUM_GEN_IP==0: Generate checksums by hardware for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                       0
  /* CHECKSUM_GEN_UDP==0: Generate checksums by hardware for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                      0
  /* CHECKSUM_GEN_TCP==0: Generate checksums by hardware for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                      0
  /* CHECKSUM_CHECK_IP==0: Check checksums by hardware for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP                     0
  /* CHECKSUM_CHECK_UDP==0: Check checksums by hardware for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP                    0
  /* CHECKSUM_CHECK_TCP==0: Check checksums by hardware for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP                    0
#else
  /* CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                       1
  /* CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                      1
  /* CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                      1
  /* CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP                     1
  /* CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP                    1
  /* CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP                    1
#endif


/*
   ----------------------------------------------
   ---------- Sequential layer options ----------
   ----------------------------------------------
*/
/*
 * LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)
 */
#define LWIP_NETCONN                            1

/*
   ------------------------------------
   ---------- Socket options ----------
   ------------------------------------
*/
/*
 * LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
 */
#define LWIP_SOCKET                             0


/*
   ------------------------------------
   ---------- Thread options ----------
   ------------------------------------
*/
/*
 * TCPIP_THREAD_NAME: The name assigned to the main tcpip thread.
 */
#define TCPIP_THREAD_NAME                      "tcpipd"

/*
 * TCPIP_THREAD_STACKSIZE: The stack size used by the main tcpip thread.
 * The stack size value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#define TCPIP_THREAD_STACKSIZE                  STACK_DEPTH_LOW

/*
 * TCPIP_THREAD_PRIO: The priority assigned to the main tcpip thread.
 * The priority value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#define TCPIP_THREAD_PRIO                       0

/*
 * TCPIP_MBOX_SIZE: The mailbox size for the tcpip thread messages
 * The queue size value itself is platform-dependent, but is passed to
 * sys_mbox_new() when tcpip_init is called.
 */
#define TCPIP_MBOX_SIZE                         8

/*
 * SLIPIF_THREAD_NAME: The name assigned to the slipif_loop thread.
 */
#define SLIPIF_THREAD_NAME                      "slipif_loop"

/*
 * SLIP_THREAD_STACKSIZE: The stack size used by the slipif_loop thread.
 * The stack size value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#define SLIPIF_THREAD_STACKSIZE                 STACK_DEPTH_LOW

/*
 * SLIPIF_THREAD_PRIO: The priority assigned to the slipif_loop thread.
 * The priority value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#define SLIPIF_THREAD_PRIO                      0

/*
 * PPP_THREAD_NAME: The name assigned to the pppInputThread.
 */
#define PPP_THREAD_NAME                         "pppInput"

/*
 * PPP_THREAD_STACKSIZE: The stack size used by the pppInputThread.
 * The stack size value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#define PPP_THREAD_STACKSIZE                    STACK_DEPTH_LOW

/*
 * PPP_THREAD_PRIO: The priority assigned to the pppInputThread.
 * The priority value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#define PPP_THREAD_PRIO                         0

/**
 * DEFAULT_THREAD_NAME: The name assigned to any other lwIP thread.
 */
#define DEFAULT_THREAD_NAME                     "lwIP"

/*
 * DEFAULT_THREAD_STACKSIZE: The stack size used by any other lwIP thread.
 * The stack size value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#define DEFAULT_THREAD_STACKSIZE                STACK_DEPTH_LOW

/*
 * DEFAULT_THREAD_PRIO: The priority assigned to any other lwIP thread.
 * The priority value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#define DEFAULT_THREAD_PRIO                     0

/*
 * DEFAULT_RAW_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_RAW. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#define DEFAULT_RAW_RECVMBOX_SIZE               8

/*
 * DEFAULT_UDP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_UDP. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#define DEFAULT_UDP_RECVMBOX_SIZE               8

/*
 * DEFAULT_TCP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_TCP. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#define DEFAULT_TCP_RECVMBOX_SIZE               8

/*
 * DEFAULT_ACCEPTMBOX_SIZE: The mailbox size for the incoming connections.
 * The queue size value itself is platform-dependent, but is passed to
 * sys_mbox_new() when the acceptmbox is created.
 */
#define DEFAULT_ACCEPTMBOX_SIZE                 8


/*
   ---------------------------------------
   ---------- Debugging options ----------
   ---------------------------------------
*/
/*
 * LWIP_DBG_MIN_LEVEL: After masking, the value of the debug is
 * compared against this value. If it is smaller, then debugging
 * messages are written.
 */
#define LWIP_DBG_MIN_LEVEL                      LWIP_DBG_LEVEL_ALL

/*
 * LWIP_DBG_TYPES_ON: A mask that can be used to globally enable/disable
 * debug messages of certain types.
 */
#define LWIP_DBG_TYPES_ON                       LWIP_DBG_OFF

/*
 * LOW_LEVEL_DEBUG: Enable debugging in low level driver (ethif, etc)
 */
#define LOW_LEVEL_DEBUG                         LWIP_DBG_OFF


/*
 * ETHARP_DEBUG: Enable debugging in etharp.c.
 */
#define ETHARP_DEBUG                            LWIP_DBG_OFF

/*
 * NETIF_DEBUG: Enable debugging in netif.c.
 */
#define NETIF_DEBUG                             LWIP_DBG_OFF

/*
 * PBUF_DEBUG: Enable debugging in pbuf.c.
 */
#define PBUF_DEBUG                              LWIP_DBG_OFF

/*
 * API_LIB_DEBUG: Enable debugging in api_lib.c.
 */
#define API_LIB_DEBUG                           LWIP_DBG_OFF

/*
 *  * API_MSG_DEBUG: Enable debugging in api_msg.c.
 */
#define API_MSG_DEBUG                           LWIP_DBG_OFF

/*
 * SOCKETS_DEBUG: Enable debugging in sockets.c.
 */
#define SOCKETS_DEBUG                           LWIP_DBG_OFF

/*
 * ICMP_DEBUG: Enable debugging in icmp.c.
 */
#define ICMP_DEBUG                              LWIP_DBG_OFF

/*
 * IGMP_DEBUG: Enable debugging in igmp.c.
 */
#define IGMP_DEBUG                              LWIP_DBG_OFF

/*
 * INET_DEBUG: Enable debugging in inet.c.
 */
#define INET_DEBUG                              LWIP_DBG_OFF

/*
 * IP_DEBUG: Enable debugging for IP.
 */
#define IP_DEBUG                                LWIP_DBG_OFF

/*
 * IP_REASS_DEBUG: Enable debugging in ip_frag.c for both frag & reass.
 */
#define IP_REASS_DEBUG                          LWIP_DBG_OFF

/*
 * RAW_DEBUG: Enable debugging in raw.c.
 */
#define RAW_DEBUG                               LWIP_DBG_OFF

/*
 * MEM_DEBUG: Enable debugging in mem.c.
 */
#define MEM_DEBUG                               LWIP_DBG_OFF

/*
 * MEMP_DEBUG: Enable debugging in memp.c.
 */
#define MEMP_DEBUG                              LWIP_DBG_OFF

/*
 * SYS_DEBUG: Enable debugging in sys.c.
 */
#define SYS_DEBUG                               LWIP_DBG_OFF

/*
 * TIMERS_DEBUG: Enable debugging in timers.c.
 */
#define TIMERS_DEBUG                            LWIP_DBG_OFF

/*
 * TCP_DEBUG: Enable debugging for TCP.
 */
#define TCP_DEBUG                               LWIP_DBG_OFF

/*
 * TCP_INPUT_DEBUG: Enable debugging in tcp_in.c for incoming debug.
 */
#define TCP_INPUT_DEBUG                         LWIP_DBG_OFF

/*
 * TCP_FR_DEBUG: Enable debugging in tcp_in.c for fast retransmit.
 */
#define TCP_FR_DEBUG                            LWIP_DBG_OFF

/*
 * TCP_RTO_DEBUG: Enable debugging in TCP for retransmit
 * timeout.
 */
#define TCP_RTO_DEBUG                           LWIP_DBG_OFF

/*
 * TCP_CWND_DEBUG: Enable debugging for TCP congestion window.
 */
#define TCP_CWND_DEBUG                          LWIP_DBG_OFF

/*
 * TCP_WND_DEBUG: Enable debugging in tcp_in.c for window updating.
 */
#define TCP_WND_DEBUG                           LWIP_DBG_OFF

/*
 * TCP_OUTPUT_DEBUG: Enable debugging in tcp_out.c output functions.
 */
#define TCP_OUTPUT_DEBUG                        LWIP_DBG_OFF

/*
 * TCP_RST_DEBUG: Enable debugging for TCP with the RST message.
 */
#define TCP_RST_DEBUG                           LWIP_DBG_OFF

/*
 * TCP_QLEN_DEBUG: Enable debugging for TCP queue lengths.
 */
#define TCP_QLEN_DEBUG                          LWIP_DBG_OFF

/*
 * UDP_DEBUG: Enable debugging in UDP.
 */
#define UDP_DEBUG                               LWIP_DBG_OFF

/*
 * TCPIP_DEBUG: Enable debugging in tcpip.c.
 */
#define TCPIP_DEBUG                             LWIP_DBG_OFF

/*
 * PPP_DEBUG: Enable debugging for PPP.
 */
#define PPP_DEBUG                               LWIP_DBG_OFF

/*
 * SLIP_DEBUG: Enable debugging in slipif.c.
 */
#define SLIP_DEBUG                              LWIP_DBG_OFF

/*
 * DHCP_DEBUG: Enable debugging in dhcp.c.
 */
#define DHCP_DEBUG                              LWIP_DBG_OFF

/*
 * AUTOIP_DEBUG: Enable debugging in autoip.c.
 */
#define AUTOIP_DEBUG                            LWIP_DBG_OFF

/*
 * SNMP_MSG_DEBUG: Enable debugging for SNMP messages.
 */
#define SNMP_MSG_DEBUG                          LWIP_DBG_OFF

/*
 * SNMP_MIB_DEBUG: Enable debugging for SNMP MIBs.
 */
#define SNMP_MIB_DEBUG                          LWIP_DBG_OFF

/*
 * DNS_DEBUG: Enable debugging for DNS.
 */
#define DNS_DEBUG                               LWIP_DBG_OFF

#endif /* __LWIPOPTS_H__ */

