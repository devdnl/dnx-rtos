/*=============================================================================
File     sipc.h

Author   Daniel Zorychta

Brief    Serial Inter-Processor communication.

         Copyright (C) 2019 Daniel Zorychta <daniel.zorychta@gmail.com>

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
@defgroup SIPC_H_ SIPC_H_

Detailed Doxygen description.
*/
/**@{*/

#ifndef _SIPC_H_
#define _SIPC_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <sys/types.h>
#include "net/netm.h"
#include "kernel/sysfunc.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct SIPC_socket {
        queue_t *ansq;
        void    *rxbuf;
        u32_t    recv_timeout;
        u32_t    send_timeout;
        u16_t    seq;
        u8_t     port;
        bool     busy;
        bool     waiting_for_data_ack;
} SIPC_socket_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern int   SIPC_ifup(const NET_SIPC_config_t*);
extern int   SIPC_ifdown(void);
extern int   SIPC_ifstatus(NET_SIPC_status_t*);
extern int   SIPC_socket_create(NET_protocol_t, SIPC_socket_t*);
extern int   SIPC_socket_destroy(SIPC_socket_t*);
extern int   SIPC_socket_connect(SIPC_socket_t*, const NET_SIPC_sockaddr_t*);
extern int   SIPC_socket_disconnect(SIPC_socket_t*);
extern int   SIPC_socket_shutdown(SIPC_socket_t*, NET_shut_t);
extern int   SIPC_socket_bind(SIPC_socket_t*, const NET_SIPC_sockaddr_t*);
extern int   SIPC_socket_listen(SIPC_socket_t*);
extern int   SIPC_socket_accept(SIPC_socket_t*, SIPC_socket_t*);
extern int   SIPC_socket_recv(SIPC_socket_t*, void*, size_t, NET_flags_t, size_t*);
extern int   SIPC_socket_recvfrom(SIPC_socket_t*, void*, size_t, NET_flags_t, NET_SIPC_sockaddr_t*, size_t*);
extern int   SIPC_socket_send(SIPC_socket_t*, const void*, size_t, NET_flags_t, size_t*);
extern int   SIPC_socket_sendto(SIPC_socket_t*, const void*, size_t, NET_flags_t, const NET_SIPC_sockaddr_t*, size_t*);
extern int   SIPC_gethostbyname(const char*, NET_SIPC_sockaddr_t*);
extern int   SIPC_socket_set_recv_timeout(SIPC_socket_t*, uint32_t);
extern int   SIPC_socket_set_send_timeout(SIPC_socket_t*, uint32_t);
extern int   SIPC_socket_get_recv_timeout(SIPC_socket_t*, uint32_t*);
extern int   SIPC_socket_get_send_timeout(SIPC_socket_t*, uint32_t*);
extern int   SIPC_socket_getaddress(SIPC_socket_t*, NET_SIPC_sockaddr_t*);
extern u16_t SIPC_hton_u16(u16_t);
extern u32_t SIPC_hton_u32(u32_t);
extern u64_t SIPC_hton_u64(u64_t);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _SIPC_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
