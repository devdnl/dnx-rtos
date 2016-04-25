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
#define NET_INET_IP(a, b, c, d)                 {a, b, c, d}
#define NET_INET_IP_ANY(a, b, c, d)             {0, 0, 0, 0}
#define NET_INET_IP_LOOPBACK(a, b, c, d)        {127, 0, 0, 1}
#define NET_INET_IP_BROADCAST(a, b, c, d)       {255, 255, 255, 255}


/*==============================================================================
  Exported object types
==============================================================================*/
/** @addtogroup dnx-net-h
 * @{ */

typedef struct socket SOCKET;

typedef enum {
        NET_FAMILY__INET,
        NET_FAMILY__CAN,
        NET_FAMILY__RFM,
        NET_FAMILY__MICROLAN
} NET_family_t;

typedef enum {
        NET_PROTOCOL__UDP,
        NET_PROTOCOL__TCP,
        NET_PROTOCOL__ISO_TP
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

typedef struct {
        u8_t  addr[4];
        u16_t port;
} NET_INET_addr_t;

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
        u8_t            address[4];
        u8_t            mask[4];
        u8_t            gateway[4];
} NET_INET_cfg_t;

typedef struct {
        NET_INET_state_t state;
        u8_t             address[4];
        u8_t             mask[4];
        u8_t             gateway[4];
        u8_t             hw_addr[6];
        u64_t            tx_bytes;
        u64_t            rx_bytes;
        u64_t            tx_packets;
        u64_t            rx_packets;
} NET_INET_status_t;

/**@}*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern int _net_ifup(NET_family_t, const void*, size_t);
extern int _net_ifdown(NET_family_t);
extern int _net_ifstatus(NET_family_t, void*, size_t);
extern int _net_socketcreate(NET_family_t, NET_protocol_t, SOCKET**);
extern int _net_socketdestroy(SOCKET*);
extern int _net_socketbind(SOCKET*, const void*, size_t);
extern int _net_socketlisten(SOCKET*);
extern int _net_socketaccept(SOCKET*, SOCKET**);
extern int _net_socketrecv(SOCKET*, void*, uint16_t, NET_flags_t, u16_t*);
extern int _net_socketsend(SOCKET*, const void*, uint16_t, NET_flags_t, u16_t*);

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
