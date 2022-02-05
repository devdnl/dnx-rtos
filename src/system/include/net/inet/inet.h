/*=============================================================================
File     inet.h

Author   Daniel Zorychta

Brief    Internet protocol port.

         Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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
@defgroup INET_H_ INET_H_

Detailed Doxygen description.
*/
/**@{*/

#ifndef _INET_H_
#define _INET_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "lib/sys/types.h"
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
extern int   INET_ifinit(void**, const char*);
extern int   INET_ifdeinit(void*);
extern int   INET_ifup(void*, const NET_INET_config_t*, size_t);
extern int   INET_ifdown(void*);
extern int   INET_ifstatus(void*, NET_INET_status_t*, size_t);
extern int   INET_socket_create(void*, NET_protocol_t, INET_socket_t*);
extern int   INET_socket_destroy(void*, INET_socket_t*);
extern int   INET_socket_connect(void*, INET_socket_t*, const NET_INET_sockaddr_t*, size_t);
extern int   INET_socket_disconnect(void*, INET_socket_t*);
extern int   INET_socket_shutdown(void*, INET_socket_t*, NET_shut_t);
extern int   INET_socket_bind(void*, INET_socket_t*, const NET_INET_sockaddr_t*, size_t);
extern int   INET_socket_listen(void*, INET_socket_t*);
extern int   INET_socket_accept(void*, INET_socket_t*, INET_socket_t*);
extern int   INET_socket_recv(void*, INET_socket_t*, void*, size_t, NET_flags_t, size_t*);
extern int   INET_socket_recvfrom(void*, INET_socket_t*, void*, size_t, NET_flags_t, NET_INET_sockaddr_t*, size_t, size_t*);
extern int   INET_socket_send(void*, INET_socket_t*, const void*, size_t, NET_flags_t, size_t*);
extern int   INET_socket_sendto(void*, INET_socket_t*, const void*, size_t, NET_flags_t, const NET_INET_sockaddr_t*, size_t, size_t*);
extern int   INET_gethostbyname(void*, const char*, NET_INET_sockaddr_t*, size_t);
extern int   INET_socket_set_recv_timeout(void*, INET_socket_t*, uint32_t);
extern int   INET_socket_set_send_timeout(void*, INET_socket_t*, uint32_t);
extern int   INET_socket_get_recv_timeout(void*, INET_socket_t*, uint32_t*);
extern int   INET_socket_get_send_timeout(void*, INET_socket_t*, uint32_t*);
extern int   INET_socket_getaddress(void*, INET_socket_t*, NET_INET_sockaddr_t*, size_t);
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
