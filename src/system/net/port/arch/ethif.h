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

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern void _ethif_start_lwIP_daemon();
/* extern status _ethif_set_MAC(); */
/* extern status _ethif_get_MAC(); */
/* extern status _ethif_start_DHCP_client(); */
/* extern status _ethif_stop_DHCP_client(); */
/* extern status _ethif_get_DHCP_client_status(*); */
/* extern status _ethif_if_up(ip, ip, ip); */
/* extern status _ethif_if_down(); */
/* extern status _ethif_get_if_status(); */
/* ... */

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
