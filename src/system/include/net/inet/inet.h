/*=========================================================================*//**
File     inet.h

Author   Daniel Zorychta

Brief

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
@defgroup INET_H_ INET_H_

Detailed Doxygen description.
*/
/**@{*/

#ifndef _INET_H_
#define _INET_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <sys/types.h>
#include "net/netm.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct {
        struct netconn *netconn;
        struct netbuf  *netbuf;
        uint16_t        seek;
} INET_socket_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern int   INET_ifup(const NET_INET_config_t*);
extern int   INET_ifdown(void);
extern int   INET_ifstatus(NET_INET_status_t*);
extern int   INET_socket_create(NET_protocol_t, INET_socket_t*);
extern int   INET_socket_destroy(INET_socket_t*);
extern int   INET_socket_connect(INET_socket_t*, const NET_INET_sockaddr_t*);
extern int   INET_socket_disconnect(INET_socket_t*);
extern int   INET_socket_shutdown(INET_socket_t*, NET_shut_t);
extern int   INET_socket_bind(INET_socket_t*, const NET_INET_sockaddr_t*);
extern int   INET_socket_listen(INET_socket_t*);
extern int   INET_socket_accept(INET_socket_t*, INET_socket_t*);
extern int   INET_socket_recv(INET_socket_t*, void*, size_t, NET_flags_t, size_t*);
extern int   INET_socket_recvfrom(INET_socket_t*, void*, size_t, NET_flags_t, NET_INET_sockaddr_t*, size_t*);
extern int   INET_socket_send(INET_socket_t*, const void*, size_t, NET_flags_t, size_t*);
extern int   INET_socket_sendto(INET_socket_t*, const void*, size_t, NET_flags_t, const NET_INET_sockaddr_t*, size_t*);
extern int   INET_gethostbyname(const char*, NET_INET_sockaddr_t*);
extern int   INET_socket_set_recv_timeout(INET_socket_t*, uint32_t);
extern int   INET_socket_set_send_timeout(INET_socket_t*, uint32_t);
extern int   INET_socket_getaddress(INET_socket_t*, NET_INET_sockaddr_t*);
extern u16_t INET_hton_u16(u16_t);
extern u32_t INET_hton_u32(u32_t);
extern u64_t INET_hton_u64(u64_t);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _INET_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
