/*=========================================================================*//**
@file    netapi.h

@author  Daniel Zorychta

@brief   This file provide network API.

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

#ifndef _NETAPI_H_
#define _NETAPI_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "arch/ethif.h"

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
typedef ip_addr_t ip_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief Function start DHCP client to gets IP addresses
 *
 * @return 0 if success, otherwise -1
 */
//==============================================================================
static inline int netapi_start_DHCP_client(void)
{
        return _ethif_start_DHCP_client();
}

//==============================================================================
/**
 * @brief Function stop DHCP client
 *
 * @return 0 if success, otherwise -1
 */
//==============================================================================
static inline int netapi_stop_DHCP_client(void)
{
        return _ethif_stop_DHCP_client();
}

//==============================================================================
/**
 * @brief Function renew DHCP connection
 *
 * @return 0 if success, otherwise -1
 */
//==============================================================================
static inline int netapi_renew_DHCP_connection(void)
{
        return _ethif_renew_DHCP_connection();
}

//==============================================================================
/**
 * @brief Function configure network on static IP addresses
 *
 * @param ip            a IP address
 * @param netmask       a net mask
 * @param gateway       a gateway IP address
 *
 * @return 0 if success, otherwise -1
 */
//==============================================================================
static inline int netapi_ifup(const ip_t *ip, const ip_t *netmask, const ip_t *gateway)
{
        return _ethif_if_up(ip, netmask, gateway);
}

//==============================================================================
/**
 * @brief Function close static configured network
 *
 * @return 0 if success, otherwise -1
 */
//==============================================================================
static inline int netapi_ifdown(void)
{
        return _ethif_if_down();
}

//==============================================================================
/**
 * @brief Function gets network connection informations
 *
 * @param ifcfg         a pointer to information object
 *
 * @return 0 if success, otherwise -1
 */
//==============================================================================
static inline int netapi_get_ifconfig(ifconfig *ifcfg)
{
        return _ethif_get_ifconfig(ifcfg);
}

//==============================================================================
/**
 * @brief Function set ip address
 *
 * @param ip            a IP address object
 * @param a             IP part a
 * @param b             IP part b
 * @param c             IP part c
 * @param d             IP part d
 */
//==============================================================================
static inline void netapi_set_ip(ip_t *ip, const u8_t a, const u8_t b, const u8_t c, const u8_t d)
{
        IP4_ADDR(ip, a, b ,c ,d);
}

#ifdef __cplusplus
}
#endif

#endif /* _NETAPI_H_ */
/*==============================================================================
  End of file
==============================================================================*/
