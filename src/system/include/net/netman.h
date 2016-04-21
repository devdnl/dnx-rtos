/*=========================================================================*//**
@file    netman.h

@author  Daniel Zorychta

@brief   Network manager.

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

#ifndef _NETMAN_H_
#define _NETMAN_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <sys/types.h>
#include "lwip/api.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
typedef enum ifstatus {
        NET_STATUS_NOT_CONFIGURED,
        NET_STATUS_STATIC_IP,
        NET_STATUS_DHCP_CONFIGURING,
        NET_STATUS_DHCP_CONFIGURED,
        NET_STATUS_LINK_DISCONNECTED
} net_status_t;

typedef struct ifconfig {
        net_status_t    status;
        ip_addr_t       IP_address;
        ip_addr_t       net_mask;
        ip_addr_t       gateway;
        u8_t            hw_address[6];
        u64_t           tx_bytes;
        u64_t           rx_bytes;
        uint            rx_packets;
        uint            tx_packets;
} _ifconfig_t;

typedef struct {
        mutex_t        *access;
        FILE           *if_file;
        void           *if_mem;
        thread_t        if_thread;
        struct netif    netif;
        uint            rx_packets;
        uint            tx_packets;
        uint            rx_bytes;
        uint            tx_bytes;
        bool            ready:1;
        bool            disconnected:1;
        bool            configured:1;
} _netman_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern void _netman_init();
extern int  _netman_start_DHCP_client();
extern int  _netman_stop_DHCP_client();
extern int  _netman_inform_DHCP_server();
extern int  _netman_renew_DHCP_connection();
extern int  _netman_if_up(const ip_addr_t*, const ip_addr_t*, const ip_addr_t*);
extern int  _netman_if_down();
extern int  _netman_get_ifconfig(_ifconfig_t*);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _NETMAN_H_ */
/*==============================================================================
  End of file
==============================================================================*/
