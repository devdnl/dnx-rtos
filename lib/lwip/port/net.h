#ifndef NET_H_
#define NET_H_
/*=============================================================================================*//**
@file    net.h

@author  Daniel Zorychta

@brief   This file support upper layer of Ethernet interface and LwIP stack

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
#include "system.h"
#include "lwip/tcp.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/
typedef u32_t netSoc_t;

typedef union
{
      struct
      {
            bool_t BindAccepted:1;
            bool_t DataReceived:1;
            bool_t DataPosted:1;
            bool_t ConnectionError:1;
            bool_t Poll:1;
      } flag;
      u8_t integer;
} netStatus_t;


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern netSoc_t    NET_NewTCPSocket(ip_addr_t *ipaddr, u16_t port);
extern stdStatus_t NET_CloseTCPSocket(netSoc_t socket);
extern stdStatus_t NET_GetTCPStatus(netSoc_t socket, netStatus_t *status);
extern stdStatus_t NET_TCPAcceptReceived(netSoc_t socket);
extern void        *NET_GetReceivedDataBuffer(netSoc_t socket);
extern void        NET_FreeReceivedBuffer(netSoc_t socket);
extern stdStatus_t NET_TCPWrite(netSoc_t socket, void *src, u32_t *len);
extern stdStatus_t NET_TCPClose(netSoc_t socket);
extern stdStatus_t NET_TCPWrite(netSoc_t socket, void *src, u32_t *len);
extern stdStatus_t NET_TCPAcceptError(netSoc_t socket);
extern stdStatus_t NET_TCPAcceptPoll(netSoc_t socket);


#ifdef __cplusplus
}
#endif

#endif /* NET_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
