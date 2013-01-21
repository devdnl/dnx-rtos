#ifndef ETHERNETIF_H_
#define ETHERNETIF_H_
/*=============================================================================================*//**
@file    ethernetif.h

@author  Daniel Zorychta

@brief   This file support low level Ethernet interface

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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


*//*==============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "lwip/err.h"
#include "lwip/netif.h"


/*==================================================================================================
                                  Exported symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
//extern struct pbuf *low_level_input(struct netif *netif);
//extern err_t ethernetif_init(struct netif *netif);
//extern err_t ethernetif_input(struct netif *netif);
//extern void  Set_MAC_Address(unsigned char *macadd);
//extern void low_level_init(struct netif *netif);


#ifdef __cplusplus
}
#endif

#endif /* ETHERNETIF_H_ */
/*==================================================================================================
                                             End of file
==================================================================================================*/

