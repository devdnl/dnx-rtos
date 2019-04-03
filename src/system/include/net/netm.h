/*=============================================================================
File     netm.h

Author   Daniel Zorychta

Brief    Network management.

         Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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


==============================================================================*/

/**
@addtogroup dnx-net-h
*/
/**@{*/

#ifndef _NETM_H_
#define _NETM_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/*------------------------------------------------------------------------------
  INET NETWORK FAMILY
------------------------------------------------------------------------------*/
/** Macro creates IPv4 address for INET family network. */
#define NET_INET_IPv4(a, b, c, d)               (((a & 0xFF) << 24) | ((b & 0xFF) << 16) | ((c & 0xFF) << 8) | ((d & 0xFF)))

/** Macro creates ANY address for INET family network. */
#define NET_INET_IPv4_ANY                       NET_INET_IPv4(0,0,0,0)

/** Macro creates LOOPBACK address for INET family network. */
#define NET_INET_IPv4_LOOPBACK                  NET_INET_IPv4(127,0,0,1)

/** Macro creates BROADCAST address for INET family network. */
#define NET_INET_IPv4_BROADCAST                 NET_INET_IPv4(255,255,255,255)

/** Macro gets part <i>a</i> of INET family network address. */
#define NET_INET_IPv4_a(ip)                     ((ip >> 24) & 0xFF)

/** Macro gets part <i>b</i> of INET family network address. */
#define NET_INET_IPv4_b(ip)                     ((ip >> 16) & 0xFF)

/** Macro gets part <i>c</i> of INET family network address. */
#define NET_INET_IPv4_c(ip)                     ((ip >> 8)  & 0xFF)

/** Macro gets part <i>d</i> of INET family network address. */
#define NET_INET_IPv4_d(ip)                     ((ip >> 0)  & 0xFF)

/*------------------------------------------------------------------------------
  SIPC NETWORK FAMILY
------------------------------------------------------------------------------*/


/*==============================================================================
  Exported object types
==============================================================================*/
/*------------------------------------------------------------------------------
  GENERIC NETWORK
------------------------------------------------------------------------------*/
/** Network family. */
typedef enum {
#if __ENABLE_TCPIP_STACK__ > 0
        NET_FAMILY__INET,                       //!< Internet network.
#else
#define NET_FAMILY__INET _NET_FAMILY__COUNT
#endif
#if __ENABLE_SIPC_STACK__ > 0
        NET_FAMILY__SIPC,                       //!< Serial Inter-Processor Communication
#else
#define NET_FAMILY__SIPC _NET_FAMILY__COUNT
#endif
    #ifndef DOXYGEN
        _NET_FAMILY__COUNT
    #endif
} NET_family_t;

/** Protocol selection. Not all protocols are available for all family networks. */
typedef enum {
        NET_PROTOCOL__UDP,                      //!< UDP protocol (INET).
        NET_PROTOCOL__TCP,                      //!< TCP protocol (INET).
        NET_PROTOCOL__STREAM,                   //!< STREAM protocol (SIPC)
} NET_protocol_t;

/** Control flags. */
typedef enum {
        NET_FLAGS__NONE      = 0,               //!< Flags not set.
        NET_FLAGS__NOCOPY    = (1 << 0),        //!< Buffer is not internally copy.
        NET_FLAGS__COPY      = (1 << 1),        //!< Buffer is internally copy.
        NET_FLAGS__MORE      = (1 << 2),        //!< More transfers.
        NET_FLAGS__REWIND    = (1 << 3),        //!< Read stream index is rewind.
        NET_FLAGS__FREEBUF   = (1 << 4),        //!< Skip unread bytes after read and free buffer.
} NET_flags_t;

/** Socket shutdown direction. */
typedef enum {
        NET_SHUT__RD   = (1 << 0),              //!< Shutdown read direction.
        NET_SHUT__WR   = (1 << 1),              //!< Shutdown write direction.
        NET_SHUT__RDWR = (NET_SHUT__RD | NET_SHUT__WR)  //!< Shutdown both directions.
} NET_shut_t;

/** Generic socket address. This type accept all network family addresses. */
typedef void NET_generic_sockaddr_t;

/** Generic interface configuration. This type accept all network family configurations. */
typedef void NET_generic_config_t;

/** Generic network status. This type accept all network family statuses. */
typedef void NET_generic_status_t;

/** Socket object definition. Protected object fields. */
typedef struct socket SOCKET;

/*------------------------------------------------------------------------------
  INET NETWORK FAMILY
------------------------------------------------------------------------------*/
/** INET network address: IPv4. */
typedef uint32_t NET_INET_IPv4_t;

/** INET socket address. This type contains IPv4 address and port. */
typedef struct {
        NET_INET_IPv4_t addr;                   /*!< IPv4 address.*/
        u16_t           port;                   /*!< Port.*/
} NET_INET_sockaddr_t;

/** INET network state. */
typedef enum {
        NET_INET_STATE__NOT_CONFIGURED,         //!< Network not configured.
        NET_INET_STATE__STATIC_IP,              //!< Static IP configuration.
        NET_INET_STATE__DHCP_CONFIGURING,       //!< DHCP address is configuring.
        NET_INET_STATE__DHCP_CONFIGURED,        //!< DHCP address configured.
        NET_INET_STATE__LINK_DISCONNECTED       //!< Network interface not connected.
} NET_INET_state_t;

/** INET configuration mode. */
typedef enum {
        NET_INET_MODE__STATIC,                  //!< Configure static IP.
        NET_INET_MODE__DHCP_START,              //!< Start DHCP client.
        NET_INET_MODE__DHCP_INFORM,             //!< Inform DHCP server.
        NET_INET_MODE__DHCP_RENEW,              //!< Renew DHCP connection.
} NET_INET_mode_t;

/** INET configuration. */
typedef struct {
        NET_INET_mode_t mode;                   /*!< Configuration mode.*/
        NET_INET_IPv4_t address;                /*!< Address if static mode selected.*/
        NET_INET_IPv4_t mask;                   /*!< Network mask if static mode selected.*/
        NET_INET_IPv4_t gateway;                /*!< Gateway address if static mode selected.*/
} NET_INET_config_t;

/** INET status. */
typedef struct {
        NET_INET_state_t state;                 /*!< Connection state.*/
        NET_INET_IPv4_t  address;               /*!< Connection address.*/
        NET_INET_IPv4_t  mask;                  /*!< Connection network mask.*/
        NET_INET_IPv4_t  gateway;               /*!< Connection gateway.*/
        u8_t             hw_addr[6];            /*!< MAC address.*/
        u64_t            tx_bytes;              /*!< Number of transmitted bytes.*/
        u64_t            rx_bytes;              /*!< Number of received bytes.*/
        u64_t            tx_packets;            /*!< Number of transmitted packets.*/
        u64_t            rx_packets;            /*!< Number of received packets.*/
} NET_INET_status_t;

/*------------------------------------------------------------------------------
  SIPC NETWORK FAMILY
------------------------------------------------------------------------------*/
/** SIPC network state. */
typedef enum {
        NET_SIPC_STATE__DOWN,                   //!< Network not configured.
        NET_SIPC_STATE__UP,                     //!< Network configured.
} NET_SIPC_state_t;

/** SIPC configuration. */
typedef struct {
        u16_t MTU;
} NET_SIPC_config_t;

/** SIPC status. */
typedef struct {
        NET_SIPC_state_t state;                 /*!< Connection state.*/
        u16_t            MTU;                   /*!< Transfer size. */
        u64_t            tx_bytes;              /*!< Number of transmitted bytes.*/
        u64_t            rx_bytes;              /*!< Number of received bytes.*/
        u64_t            tx_packets;            /*!< Number of transmitted packets.*/
        u64_t            rx_packets;            /*!< Number of received packets.*/
} NET_SIPC_status_t;

/** SIPC socket address. */
typedef struct {
        u8_t             port;                   /*!< Port.*/
} NET_SIPC_sockaddr_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
#ifndef DOXYGEN
extern int   _net_ifup(NET_family_t, const NET_generic_config_t*);
extern int   _net_ifdown(NET_family_t);
extern int   _net_ifstatus(NET_family_t, NET_generic_status_t*);
extern int   _net_gethostbyname(NET_family_t, const char*, NET_generic_sockaddr_t*);
extern int   _net_socket_create(NET_family_t, NET_protocol_t, SOCKET**);
extern int   _net_socket_destroy(SOCKET*);
extern int   _net_socket_bind(SOCKET*, const NET_generic_sockaddr_t*);
extern int   _net_socket_listen(SOCKET*);
extern int   _net_socket_accept(SOCKET*, SOCKET**);
extern int   _net_socket_recv(SOCKET*, void*, size_t, NET_flags_t, size_t*);
extern int   _net_socket_recvfrom(SOCKET*, void*, size_t, NET_flags_t, NET_generic_sockaddr_t*, size_t*);
extern int   _net_socket_send(SOCKET*, const void*, size_t, NET_flags_t, size_t*);
extern int   _net_socket_sendto(SOCKET*, const void*, size_t, NET_flags_t, const NET_generic_sockaddr_t*, size_t*);
extern int   _net_socket_set_recv_timeout(SOCKET*, uint32_t);
extern int   _net_socket_set_send_timeout(SOCKET*, uint32_t);
extern int   _net_socket_get_recv_timeout(SOCKET*, uint32_t*);
extern int   _net_socket_get_send_timeout(SOCKET*, uint32_t*);
extern int   _net_socket_connect(SOCKET*, const NET_generic_sockaddr_t*);
extern int   _net_socket_disconnect(SOCKET*);
extern int   _net_socket_shutdown(SOCKET*, NET_shut_t);
extern int   _net_socket_getaddress(SOCKET*, NET_generic_sockaddr_t*);
extern u16_t _net_hton_u16(NET_family_t, u16_t);
extern u32_t _net_hton_u32(NET_family_t, u32_t);
extern u64_t _net_hton_u64(NET_family_t, u64_t);
#endif

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _NETM_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
