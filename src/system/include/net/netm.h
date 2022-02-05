/*=============================================================================
File     netm.h

Author   Daniel Zorychta

Brief    Network management.

         Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "lib/sys/types.h"

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
#define NETM_INET_IPv4(a, b, c, d)              (((a & 0xFF) << 24) | ((b & 0xFF) << 16) | ((c & 0xFF) << 8) | ((d & 0xFF)))

/** Macro creates ANY address for INET family network. */
#define NETM_INET_IPv4_ANY                      NETM_INET_IPv4(0,0,0,0)

/** Macro creates LOOPBACK address for INET family network. */
#define NETM_INET_IPv4_LOOPBACK                 NETM_INET_IPv4(127,0,0,1)

/** Macro creates BROADCAST address for INET family network. */
#define NETM_INET_IPv4_BROADCAST                NETM_INET_IPv4(255,255,255,255)

/** Macro gets part <i>a</i> of INET family network address. */
#define NETM_INET_IPv4_a(ip)                    ((ip >> 24) & 0xFF)

/** Macro gets part <i>b</i> of INET family network address. */
#define NETM_INET_IPv4_b(ip)                    ((ip >> 16) & 0xFF)

/** Macro gets part <i>c</i> of INET family network address. */
#define NETM_INET_IPv4_c(ip)                    ((ip >> 8)  & 0xFF)

/** Macro gets part <i>d</i> of INET family network address. */
#define NETM_INET_IPv4_d(ip)                    ((ip >> 0)  & 0xFF)

/*------------------------------------------------------------------------------
  SIPC NETWORK FAMILY
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  CANNET NETWORK FAMILY
------------------------------------------------------------------------------*/
#define NETM_CANNET_MAX_PORT            7
#define NETM_CANNET_CAN_ID_MASK         0x10003FFF
#define NETM_CANNET_ADDR_BROADCAST      0x3FFF
#define NETM_CANNET_ADDR_ANY            0x0000

/*==============================================================================
  Exported object types
==============================================================================*/
/*------------------------------------------------------------------------------
  GENERIC NETWORK
------------------------------------------------------------------------------*/
/** Network family. */
typedef enum {
        NETM_FAMILY__INET,                      //!< Internet network.
        NETM_FAMILY__SIPC,                      //!< Serial Inter-Processor Communication
        NETM_FAMILY__CANNET,                    //!< CAN network Communication
        _NETM_FAMILY__COUNT
} NETM_family_t;

/** Protocol selection. Not all protocols are available for all family networks. */
typedef enum {
        NETM_PROTOCOL__DATAGRAM,                        //!< DATAGRAM type protocol
        NETM_PROTOCOL__STREAM,                          //!< STREAM type protocol
        NETM_PROTOCOL__UDP = NETM_PROTOCOL__DATAGRAM,   //!< UDP protocol.
        NETM_PROTOCOL__TCP = NETM_PROTOCOL__STREAM,     //!< TCP protocol.
        _NETM_PROTOCOL__COUNT,                          //!< number of protocols
} NETM_protocol_t;

/** Control flags. */
typedef enum {
        NETM_FLAGS__NONE      = 0,              //!< Flags not set.
        NETM_FLAGS__NOCOPY    = (1 << 0),       //!< Buffer is not internally copy.
        NETM_FLAGS__COPY      = (1 << 1),       //!< Buffer is internally copy.
        NETM_FLAGS__MORE      = (1 << 2),       //!< More transfers.
        NETM_FLAGS__REWIND    = (1 << 3),       //!< Read stream index is rewind.
        NETM_FLAGS__FREEBUF   = (1 << 4),       //!< Skip unread bytes after read and free buffer.
} NETM_flags_t;

/** Socket shutdown direction. */
typedef enum {
        NETM_SHUT__RD   = (1 << 0),             //!< Shutdown read direction.
        NETM_SHUT__WR   = (1 << 1),             //!< Shutdown write direction.
        NETM_SHUT__RDWR = (NETM_SHUT__RD | NETM_SHUT__WR)  //!< Shutdown both directions.
} NETM_shut_t;

/** Generic socket address. This type accept all network family addresses. */
typedef void NETM_generic_sockaddr_t;

/** Generic interface configuration. This type accept all network family configurations. */
typedef void NETM_generic_config_t;

/** Generic network status. This type accept all network family statuses. */
typedef void NETM_generic_status_t;

/** Socket object definition. Protected object fields. */
typedef struct ksocket ksocket_t;

/*------------------------------------------------------------------------------
  INET NETWORK FAMILY
------------------------------------------------------------------------------*/
/** INET network address: IPv4. */
typedef uint32_t NETM_INET_IPv4_t;

/** INET socket address. This type contains IPv4 address and port. */
typedef struct {
        NETM_INET_IPv4_t addr;                  /*!< IPv4 address.*/
        u16_t           port;                   /*!< Port.*/
} NETM_INET_sockaddr_t;

/** INET network state. */
typedef enum {
        NETM_INET_STATE__NOT_CONFIGURED,        //!< Network not configured.
        NETM_INET_STATE__STATIC_IP,             //!< Static IP configuration.
        NETM_INET_STATE__DHCP_CONFIGURING,      //!< DHCP address is configuring.
        NETM_INET_STATE__DHCP_CONFIGURED,       //!< DHCP address configured.
        NETM_INET_STATE__LINK_DISCONNECTED      //!< Network interface not connected.
} NETM_INET_state_t;

/** INET configuration mode. */
typedef enum {
        NETM_INET_MODE__STATIC,                 //!< Configure static IP.
        NETM_INET_MODE__DHCP_START,             //!< Start DHCP client.
        NETM_INET_MODE__DHCP_INFORM,            //!< Inform DHCP server.
        NETM_INET_MODE__DHCP_RENEW,             //!< Renew DHCP connection.
} NETM_INET_mode_t;

/** INET configuration. */
typedef struct {
        NETM_INET_mode_t mode;                  /*!< Configuration mode.*/
        NETM_INET_IPv4_t address;               /*!< Address if static mode selected.*/
        NETM_INET_IPv4_t mask;                  /*!< Network mask if static mode selected.*/
        NETM_INET_IPv4_t gateway;               /*!< Gateway address if static mode selected.*/
} NETM_INET_config_t;

/** INET status. */
typedef struct {
        NETM_INET_state_t state;                /*!< Connection state.*/
        NETM_INET_IPv4_t  address;              /*!< Connection address.*/
        NETM_INET_IPv4_t  mask;                 /*!< Connection network mask.*/
        NETM_INET_IPv4_t  gateway;              /*!< Connection gateway.*/
        u8_t             hw_addr[6];            /*!< MAC address.*/
        u64_t            tx_bytes;              /*!< Number of transmitted bytes.*/
        u64_t            rx_bytes;              /*!< Number of received bytes.*/
        u64_t            tx_packets;            /*!< Number of transmitted packets.*/
        u64_t            rx_packets;            /*!< Number of received packets.*/
} NETM_INET_status_t;

/*------------------------------------------------------------------------------
  SIPC NETWORK FAMILY
------------------------------------------------------------------------------*/
/** SIPC network state. */
typedef enum {
        NETM_SIPC_STATE__DOWN,                  //!< Network not configured.
        NETM_SIPC_STATE__UP,                    //!< Network configured.
} NETM_SIPC_state_t;

/** SIPC configuration. */
typedef struct {
        u16_t MTU;                              //!< Maximal Transfer Unit
} NETM_SIPC_config_t;

/** SIPC status. */
typedef struct {
        NETM_SIPC_state_t state;                /*!< Connection state.*/
        u16_t            MTU;                   /*!< Transfer size. */
        u64_t            tx_bytes;              /*!< Number of transmitted bytes.*/
        u64_t            rx_bytes;              /*!< Number of received bytes.*/
        u64_t            tx_packets;            /*!< Number of transmitted packets.*/
        u64_t            rx_packets;            /*!< Number of received packets.*/
} NETM_SIPC_status_t;

/** SIPC socket address. */
typedef struct {
        u8_t             port;                   /*!< Port.*/
} NETM_SIPC_sockaddr_t;


/*------------------------------------------------------------------------------
  CANNET NETWORK FAMILY
------------------------------------------------------------------------------*/
/** CANNET network state. */
typedef enum {
        NETM_CANNET_STATE__DOWN,                //!< Network not configured.
        NETM_CANNET_STATE__UP,                  //!< Network configured.
} NETM_CANNET_state_t;

/** CANNET configuration. */
typedef struct {
        u16_t             addr;                 //!< device address.
} NETM_CANNET_config_t;

/** CANNET status. */
typedef struct {
        NETM_CANNET_state_t state;              /*!< Connection state.*/
        u16_t              addr;                /*!< Address.*/
        u16_t              MTU;                 /*!< Transfer size. */
        u64_t              tx_bytes;            /*!< Number of transmitted bytes.*/
        u64_t              rx_bytes;            /*!< Number of received bytes.*/
        u64_t              tx_packets;          /*!< Number of transmitted packets.*/
        u64_t              rx_packets;          /*!< Number of received packets.*/
} NETM_CANNET_status_t;

/** SIPC socket address. */
typedef struct {
        u16_t addr;                             /*!< Address.*/
        u8_t  port;                             /*!< Port.*/
} NETM_CANNET_sockaddr_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
#ifndef DOXYGEN
extern int   _net_ifadd(const char*, NETM_family_t, const char*);
extern int   _net_ifrm(const char*);
extern int   _net_ifdelete(const char*);
extern int   _net_iflist(char**, size_t, size_t*);
extern int   _net_ifup(const char*, const NETM_generic_config_t*, size_t);
extern int   _net_ifdown(const char*);
extern int   _net_ifstatus(const char*, NETM_family_t*, NETM_generic_status_t*, size_t);
extern int   _net_gethostbyname(const char*, const char*, NETM_generic_sockaddr_t*, size_t);
extern int   _net_socket_create(const char*, NETM_protocol_t, ksocket_t**);
extern int   _net_socket_destroy(ksocket_t*);
extern int   _net_socket_bind(ksocket_t*, const NETM_generic_sockaddr_t*, size_t);
extern int   _net_socket_listen(ksocket_t*);
extern int   _net_socket_accept(ksocket_t*, ksocket_t**);
extern int   _net_socket_recv(ksocket_t*, void*, size_t, NETM_flags_t, size_t*);
extern int   _net_socket_recvfrom(ksocket_t*, void*, size_t, NETM_flags_t, NETM_generic_sockaddr_t*, size_t, size_t*);
extern int   _net_socket_send(ksocket_t*, const void*, size_t, NETM_flags_t, size_t*);
extern int   _net_socket_sendto(ksocket_t*, const void*, size_t, NETM_flags_t, const NETM_generic_sockaddr_t*, size_t, size_t*);
extern int   _net_socket_set_recv_timeout(ksocket_t*, uint32_t);
extern int   _net_socket_set_send_timeout(ksocket_t*, uint32_t);
extern int   _net_socket_get_recv_timeout(ksocket_t*, uint32_t*);
extern int   _net_socket_get_send_timeout(ksocket_t*, uint32_t*);
extern int   _net_socket_connect(ksocket_t*, const NETM_generic_sockaddr_t*, size_t);
extern int   _net_socket_disconnect(ksocket_t*);
extern int   _net_socket_shutdown(ksocket_t*, NETM_shut_t);
extern int   _net_socket_getaddress(ksocket_t*, NETM_generic_sockaddr_t*, size_t);
extern u16_t _net_hton_u16(NETM_family_t, u16_t);
extern u32_t _net_hton_u32(NETM_family_t, u32_t);
extern u64_t _net_hton_u64(NETM_family_t, u64_t);
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
