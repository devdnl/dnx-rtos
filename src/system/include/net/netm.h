/*=========================================================================*//**
File     netm.h

Author   Daniel Zorychta

Brief    Network management.

         Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/**
defgroup NETM_H_ NETM_H_

Detailed Doxygen description.
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
#define NET_INET_IPv4(a, b, c, d)               (((a & 0xFF) << 24) | ((b & 0xFF) << 16) | ((c & 0xFF) << 8) | ((d & 0xFF)))
#define NET_INET_IPv4_ANY                       NET_INET_IPv4(0,0,0,0)
#define NET_INET_IPv4_LOOPBACK                  NET_INET_IPv4(127,0,0,1)
#define NET_INET_IPv4_BROADCAST                 NET_INET_IPv4(255,255,255,255)
#define NET_INET_IPv4_a(ip)                     ((ip >> 24) & 0xFF)
#define NET_INET_IPv4_b(ip)                     ((ip >> 16) & 0xFF)
#define NET_INET_IPv4_c(ip)                     ((ip >> 8)  & 0xFF)
#define NET_INET_IPv4_d(ip)                     ((ip >> 0)  & 0xFF)

/*==============================================================================
  Exported object types
==============================================================================*/
/** @addtogroup dnx-net-h
 * @{ */
typedef enum {
        NET_FAMILY__INET,
        _NET_FAMILY__COUNT
} NET_family_t;

typedef enum {
        NET_PROTOCOL__UDP,
        NET_PROTOCOL__TCP,
} NET_protocol_t;

typedef enum {
        NET_FLAGS__NONE      = 0,
        NET_FLAGS__NOCOPY    = (1 << 0),
        NET_FLAGS__COPY      = (1 << 1),
        NET_FLAGS__MORE      = (1 << 2),
        NET_FLAGS__DONTBLOCK = (1 << 3),
        NET_FLAGS__REWIND    = (1 << 4),
        NET_FLAGS__FREEBUF   = (1 << 5),
} NET_flags_t;

typedef enum {
        NET_SHUT__RD   = (1 << 0),
        NET_SHUT__WR   = (1 << 1),
        NET_SHUT__RDWR = (NET_SHUT__RD | NET_SHUT__WR)
} NET_shut_t;




typedef uint32_t NET_INET_IPv4_t;

typedef struct {
        NET_INET_IPv4_t addr;
        u16_t           port;
} NET_INET_sockaddr_t;

typedef enum {
        NET_INET_STATE__NOT_CONFIGURED,
        NET_INET_STATE__STATIC_IP,
        NET_INET_STATE__DHCP_CONFIGURING,
        NET_INET_STATE__DHCP_CONFIGURED,
        NET_INET_STATE__LINK_DISCONNECTED
} NET_INET_state_t;

typedef enum {
        NET_INET_MODE__STATIC,
        NET_INET_MODE__DHCP_START,
        NET_INET_MODE__DHCP_INFORM,
        NET_INET_MODE__DHCP_RENEW,
} NET_INET_mode_t;

typedef struct {
        NET_INET_mode_t mode;
        NET_INET_IPv4_t address;
        NET_INET_IPv4_t mask;
        NET_INET_IPv4_t gateway;
} NET_INET_config_t;

typedef struct {
        NET_INET_state_t state;
        NET_INET_IPv4_t  address;
        NET_INET_IPv4_t  mask;
        NET_INET_IPv4_t  gateway;
        u8_t             hw_addr[6];
        u64_t            tx_bytes;
        u64_t            rx_bytes;
        u64_t            tx_packets;
        u64_t            rx_packets;
} NET_INET_status_t;





typedef void NET_generic_sockaddr_t;

typedef void NET_generic_config_t;

typedef void NET_generic_status_t;

typedef struct socket SOCKET;

/**@}*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern int _net_ifup(NET_family_t, const NET_generic_config_t*);
extern int _net_ifdown(NET_family_t);
extern int _net_ifstatus(NET_family_t, NET_generic_status_t*);
extern int _net_gethostbyname(NET_family_t, const char*, NET_generic_sockaddr_t*);
extern int _net_socket_create(NET_family_t, NET_protocol_t, SOCKET**);
extern int _net_socket_destroy(SOCKET*);
extern int _net_socket_bind(SOCKET*, const NET_generic_sockaddr_t*);
extern int _net_socket_listen(SOCKET*);
extern int _net_socket_accept(SOCKET*, SOCKET**);
extern int _net_socket_recv(SOCKET*, void*, size_t, NET_flags_t, size_t*);
extern int _net_socket_recvfrom(SOCKET*, void*, size_t, NET_flags_t, NET_generic_sockaddr_t*, size_t*);
extern int _net_socket_send(SOCKET*, const void*, size_t, NET_flags_t, size_t*);
extern int _net_socket_sendto(SOCKET*, const void*, size_t, NET_flags_t, const NET_generic_sockaddr_t*, size_t*);
extern int _net_socket_set_recv_timeout(SOCKET*, uint32_t);
extern int _net_socket_set_send_timeout(SOCKET*, uint32_t);
extern int _net_socket_connect(SOCKET*, const NET_generic_sockaddr_t*);
extern int _net_socket_disconnect(SOCKET*);
extern int _net_socket_shutdown(SOCKET*, NET_shut_t);

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
