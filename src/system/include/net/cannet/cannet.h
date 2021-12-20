/*=============================================================================
File     cannet.h

Author   Daniel Zorychta

Brief    CAN Network communication.

         Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

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
@defgroup CANNET_H_ CANNET_H_

Detailed Doxygen description.
*/
/**@{*/

#ifndef _CANNET_H_
#define _CANNET_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "lib/sys/types.h"
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
typedef struct cannetbuf cannetbuf_t;

typedef struct CANNET_socket {
        kqueue_t *ansq;
        cannetbuf_t *rx_buf;
        cannetbuf_t *assembly_buf;
        u32_t recv_timeout;
        u32_t send_timeout;
        u16_t addr_remote;
        u16_t datagram_source;          // datagram source address, only NET_PROTOCOL__DATAGRAM
        u16_t expected_crc;
        u16_t crc;
        NET_protocol_t protocol;
        NET_shut_t shutdown;
        u8_t port;
        u8_t seq_tx;
        u8_t seq_rx;
        bool connected;
} CANNET_socket_t;


/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
// stack interface functions
extern int   CANNET_ifinit(void**, const char*);
extern int   CANNET_ifdeinit(void*);
extern int   CANNET_ifup(void*, const NET_CANNET_config_t*);
extern int   CANNET_ifdown(void*);
extern int   CANNET_ifstatus(void*, NET_CANNET_status_t*);
extern int   CANNET_socket_create(void*, NET_protocol_t, CANNET_socket_t*);
extern int   CANNET_socket_destroy(void*, CANNET_socket_t*);
extern int   CANNET_socket_connect(void*, CANNET_socket_t*, const NET_CANNET_sockaddr_t*);
extern int   CANNET_socket_disconnect(void*, CANNET_socket_t*);
extern int   CANNET_socket_shutdown(void*, CANNET_socket_t*, NET_shut_t);
extern int   CANNET_socket_bind(void*, CANNET_socket_t*, const NET_CANNET_sockaddr_t*);
extern int   CANNET_socket_listen(void*, CANNET_socket_t*);
extern int   CANNET_socket_accept(void*, CANNET_socket_t*, CANNET_socket_t*);
extern int   CANNET_socket_recv(void*, CANNET_socket_t*, void*, size_t, NET_flags_t, size_t*);
extern int   CANNET_socket_recvfrom(void*, CANNET_socket_t*, void*, size_t, NET_flags_t, NET_CANNET_sockaddr_t*, size_t*);
extern int   CANNET_socket_send(void*, CANNET_socket_t*, const void*, size_t, NET_flags_t, size_t*);
extern int   CANNET_socket_sendto(void*, CANNET_socket_t*, const void*, size_t, NET_flags_t, const NET_CANNET_sockaddr_t*, size_t*);
extern int   CANNET_gethostbyname(void*, const char*, NET_CANNET_sockaddr_t*);
extern int   CANNET_socket_set_recv_timeout(void*, CANNET_socket_t*, uint32_t);
extern int   CANNET_socket_set_send_timeout(void*, CANNET_socket_t*, uint32_t);
extern int   CANNET_socket_get_recv_timeout(void*, CANNET_socket_t*, uint32_t*);
extern int   CANNET_socket_get_send_timeout(void*, CANNET_socket_t*, uint32_t*);
extern int   CANNET_socket_getaddress(void*, CANNET_socket_t*, NET_CANNET_sockaddr_t*);
extern u16_t CANNET_hton_u16(u16_t);
extern u32_t CANNET_hton_u32(u32_t);
extern u64_t CANNET_hton_u64(u64_t);

// buffer management
extern int  cannetbuf__create(cannetbuf_t **cannetbuf, size_t max_capacity);
extern void cannetbuf__destroy(cannetbuf_t *cannetbuf);
extern int  cannetbuf__write(cannetbuf_t *cannetbuf, const u8_t *data, size_t size);
extern int  cannetbuf__move(cannetbuf_t *cannetbuf_dst, cannetbuf_t *cannetbuf_src);
extern int  cannetbuf__read(cannetbuf_t *cannetbuf, u8_t *data, size_t size, size_t *rdctr);
extern void cannetbuf__clear(cannetbuf_t *cannetbuf);
extern bool cannetbuf__is_full(cannetbuf_t *cannetbuf);
extern int  cannetbuf__get_available(cannetbuf_t *cannetbuf, size_t *available);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _CANNET_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
