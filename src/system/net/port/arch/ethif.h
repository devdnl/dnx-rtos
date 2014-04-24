/*=========================================================================*//**
@file    ethif.h

@author  Daniel Zorychta

@brief   lwIP Ethernet interface definition.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _ETHIF_H_
#define _ETHIF_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <sys/types.h>
#include "lwip/api.h"

/*==============================================================================
  Exported macros
==============================================================================*/
#define _ETHIF_INTERFACE_FILE           __NETWORK_ETHIF_FILE__

/*==============================================================================
  Exported object types
==============================================================================*/
typedef enum ifstatus {
        IFSTATUS_NOT_CONFIGURED,
        IFSTATUS_STATIC_IP,
        IFSTATUS_DHCP_CONFIGURING,
        IFSTATUS_DHCP_CONFIGURED,
} ifstatus_t;

typedef struct ifconfig {
        ifstatus_t      status;
        ip_addr_t       IP_address;
        ip_addr_t       net_mask;
        ip_addr_t       gateway;
        u8_t            hw_address[6];
        uint            rx_packets;
        uint            rx_bytes;
        uint            tx_packets;
        uint            tx_bytes;
} ifconfig_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern void _ethif_start_lwIP_daemon();
extern int  _ethif_start_DHCP_client();
extern int  _ethif_stop_DHCP_client();
extern int  _ethif_inform_DHCP_server();
extern int  _ethif_renew_DHCP_connection();
extern int  _ethif_if_up(const ip_addr_t*, const ip_addr_t*, const ip_addr_t*);
extern int  _ethif_if_down();
extern int  _ethif_get_ifconfig(ifconfig_t*);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _ETHIF_H_ */
/*==============================================================================
  End of file
==============================================================================*/
