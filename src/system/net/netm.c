/*=========================================================================*//**
File     netm.c

Author   Daniel Zorychta

Brief    Network management.

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

/*==============================================================================
  Include files
==============================================================================*/
#include "net/netm.h"
#include "net/inet/inet.h"
#include "cpuctl.h"
#include "kernel/sysfunc.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define MAXIMUM_SAFE_UDP_PAYLOAD                508

#define PROXY_TABLE                             static const proxy_func_t proxy[_NET_FAMILY__COUNT]
#define PROXY_ADD_FAMILY(_family, _proxy_func)  [NET_FAMILY__##_family] = (proxy_func_t)_proxy_func
#define call_proxy_function(family, ...)        proxy[family](__VA_ARGS__)

/*==============================================================================
  Local object types
==============================================================================*/
struct socket {
        res_header_t header;
        NET_family_t family;
        void        *ctx;
};

typedef int (*proxy_func_t)();

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 *
 * @param socket
 * @param family
 * @return
 */
//==============================================================================
static int socket_alloc(SOCKET **socket, NET_family_t family)
{
        static const uint8_t net_socket_size[_NET_FAMILY__COUNT] = {
                [NET_FAMILY__INET] = _mm_align(sizeof(INET_socket_t)),
        };

        int err = _kzalloc(_MM_NET,
                           _mm_align(sizeof(SOCKET)) + net_socket_size[family],
                           cast(void**, socket));
        if (!err) {
                (*socket)->header.type = RES_TYPE_SOCKET;
                (*socket)->family      = family;
                (*socket)->ctx         = cast(void *,
                                              cast(size_t, *socket)
                                              + _mm_align(sizeof(SOCKET)));
        }

        return err;
}

//==============================================================================
/**
 *
 * @param socket
 */
//==============================================================================
static void socket_free(SOCKET **socket)
{
        (*socket)->header.type = RES_TYPE_UNKNOWN;
        _kfree(_MM_NET, cast(void**, socket));
        *socket = NULL;
}

//==============================================================================
/**
 *
 * @param socket
 * @return
 */
//==============================================================================
static bool is_socket_valid(SOCKET *socket)
{
        return (socket != NULL)
            && (socket->header.type == RES_TYPE_SOCKET)
            && (socket->family < _NET_FAMILY__COUNT)
            && (socket->ctx == cast(void *, cast(size_t, socket)
                                          + _mm_align(sizeof(SOCKET))));
}

//==============================================================================
/**
 *
 * @param family
 * @param config
 * @param size
 * @return
 */
//==============================================================================
int _net_ifup(NET_family_t family, const NET_generic_config_t *config)
{
        PROXY_TABLE = {
                PROXY_ADD_FAMILY(INET, INET_ifup),
        };

        if (family < _NET_FAMILY__COUNT && config) {
                return call_proxy_function(family, config);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
int _net_ifdown(NET_family_t family)
{
        PROXY_TABLE = {
                PROXY_ADD_FAMILY(INET, INET_ifdown),
        };

        if (family < _NET_FAMILY__COUNT) {
                return call_proxy_function(family);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
int _net_ifstatus(NET_family_t family,  NET_generic_status_t *status)
{
        PROXY_TABLE = {
                PROXY_ADD_FAMILY(INET, INET_ifstatus),
        };

        if (family < _NET_FAMILY__COUNT && status) {
                return call_proxy_function(family, status);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 *
 * @param family
 * @param protocol
 * @param socket
 * @return
 */
//==============================================================================
int _net_socket_create(NET_family_t family, NET_protocol_t protocol, SOCKET **socket)
{
        PROXY_TABLE = {
                PROXY_ADD_FAMILY(INET, INET_socket_create),
        };

        int err = EINVAL;

        if (family < _NET_FAMILY__COUNT) {
                err = socket_alloc(socket, family);

                if (!err) {
                        err = call_proxy_function(family, protocol, (*socket)->ctx);

                        if (err) {
                                socket_free(socket);
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 *
 * @param socket
 * @return
 */
//==============================================================================
int _net_socket_destroy(SOCKET *socket)
{
        PROXY_TABLE = {
                PROXY_ADD_FAMILY(INET, INET_socket_destroy),
        };

        int err = EINVAL;

        if (is_socket_valid(socket)) {
                err = call_proxy_function(socket->family, socket->ctx);
                if (!err) {
                        socket_free(&socket);
                }
        }

        return err;
}

//==============================================================================
/**
 *
 * @param socket
 * @param addr
 * @param addr_size
 * @return
 */
//==============================================================================
int _net_socket_bind(SOCKET *socket, const NET_generic_sockaddr_t *addr)
{
        PROXY_TABLE = {
                PROXY_ADD_FAMILY(INET, INET_socket_bind),
        };

        if (is_socket_valid(socket) && addr) {
                return call_proxy_function(socket->family, socket->ctx, addr);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 *
 * @param socket
 * @return
 */
//==============================================================================
int _net_socket_listen(SOCKET *socket)
{
        PROXY_TABLE = {
                PROXY_ADD_FAMILY(INET, INET_socket_listen),
        };

        if (is_socket_valid(socket)) {
                return call_proxy_function(socket->family, socket->ctx);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 *
 * @param socket
 * @param new_socket
 * @return
 */
//==============================================================================
int _net_socket_accept(SOCKET *socket, SOCKET **new_socket)
{
        PROXY_TABLE = {
                PROXY_ADD_FAMILY(INET, INET_socket_accept),
        };

        int err = EINVAL;

        if (is_socket_valid(socket) && new_socket) {

                err = socket_alloc(new_socket, socket->family);

                if (!err) {
                        err = call_proxy_function(socket->family,
                                                  socket->ctx,
                                                  (*new_socket)->ctx);

                        if (err) {
                                socket_free(new_socket);
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 *
 * @param socket
 * @param buf
 * @param len
 * @param flags
 * @return
 */
//==============================================================================
int _net_socket_recv(SOCKET *socket, void *buf, size_t len, NET_flags_t flags, size_t *recved)
{
        PROXY_TABLE = {
                PROXY_ADD_FAMILY(INET, INET_socket_recv),
        };

        if (is_socket_valid(socket) && buf && len && recved) {
                return call_proxy_function(socket->family, socket->ctx, buf,
                                           len, flags, recved);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 *
 * @param socket
 * @param buf
 * @param len
 * @param flags
 * @param sockaddr
 * @param recved
 * @return
 */
//==============================================================================
int _net_socket_recvfrom(SOCKET                 *socket,
                         void                   *buf,
                         size_t                  len,
                         NET_flags_t             flags,
                         NET_generic_sockaddr_t *sockaddr,
                         size_t                 *recved)
{
        PROXY_TABLE = {
                PROXY_ADD_FAMILY(INET, INET_socket_recvfrom),
        };

        if (is_socket_valid(socket) && buf && len && sockaddr && recved) {
                return call_proxy_function(socket->family, socket->ctx, buf,
                                           len, flags, sockaddr, recved);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 *
 * @param socket
 * @param buf
 * @param len
 * @param flags
 * @return
 */
//==============================================================================
int _net_socket_send(SOCKET *socket, const void *buf, size_t len, NET_flags_t flags, size_t *sent)
{
        PROXY_TABLE = {
                PROXY_ADD_FAMILY(INET, INET_socket_send),
        };

        if (is_socket_valid(socket) && buf && len && sent) {
                return call_proxy_function(socket->family, socket->ctx, buf,
                                           len, flags, sent);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 *
 * @param socket
 * @param buf
 * @param len
 * @param flags
 * @return
 */
//==============================================================================
int _net_socket_sendto(SOCKET                       *socket,
                       const void                   *buf,
                       size_t                        len,
                       NET_flags_t                   flags,
                       const NET_generic_sockaddr_t *to_addr,
                       size_t                       *sent)
{
        PROXY_TABLE = {
                PROXY_ADD_FAMILY(INET, INET_socket_sendto),
        };

        if (is_socket_valid(socket) && buf && len && to_addr && sent) {
                return call_proxy_function(socket->family, socket->ctx, buf,
                                           len, flags, to_addr, sent);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 *
 * @param
 * @param
 * @param
 * @param size_t
 * @return
 */
//==============================================================================
int _net_socket_set_recv_timeout(SOCKET *socket, uint32_t timeout)
{
        PROXY_TABLE = {
                PROXY_ADD_FAMILY(INET, INET_socket_set_recv_timeout),
        };

        if (is_socket_valid(socket)) {
                return call_proxy_function(socket->family, socket->ctx, timeout);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 *
 * @param
 * @param
 * @param
 * @param size_t
 * @return
 */
//==============================================================================
int _net_socket_set_send_timeout(SOCKET *socket, uint32_t timeout)
{
        PROXY_TABLE = {
                PROXY_ADD_FAMILY(INET, INET_socket_set_send_timeout),
        };

        if (is_socket_valid(socket)) {
                return call_proxy_function(socket->family, socket->ctx, timeout);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 *
 * @param
 * @param
 * @param size_t
 * @return
 */
//==============================================================================
int _net_socket_connect(SOCKET *socket, const NET_generic_sockaddr_t *addr)
{
        PROXY_TABLE = {
                PROXY_ADD_FAMILY(INET, INET_socket_connect),
        };

        if (is_socket_valid(socket) && addr) {
                return call_proxy_function(socket->family, socket->ctx, addr);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 *
 * @param
 * @param
 * @param size_t
 * @return
 */
//==============================================================================
int _net_socket_disconnect(SOCKET *socket)
{
        PROXY_TABLE = {
                PROXY_ADD_FAMILY(INET, INET_socket_disconnect),
        };

        if (is_socket_valid(socket)) {
                return call_proxy_function(socket->family, socket->ctx);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 *
 * @param
 * @param
 * @param size_t
 * @return
 */
//==============================================================================
int _net_socket_shutdown(SOCKET *socket, NET_shut_t how)
{
        PROXY_TABLE = {
                PROXY_ADD_FAMILY(INET, INET_socket_shutdown),
        };

        if (is_socket_valid(socket)) {
                return call_proxy_function(socket->family, socket->ctx, how);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 *
 * @param socket
 * @param sockaddr
 * @return
 */
//==============================================================================
int _net_socket_getaddress(SOCKET *socket, NET_generic_sockaddr_t *sockaddr)
{
        PROXY_TABLE = {
                PROXY_ADD_FAMILY(INET, INET_socket_getaddress),
        };

        if (is_socket_valid(socket) && sockaddr) {
                return call_proxy_function(socket->family, socket->ctx, sockaddr);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 *
 * @param
 * @param
 * @param
 * @param size_t
 * @return
 */
//==============================================================================
int _net_gethostbyname(NET_family_t family, const char *name, NET_generic_sockaddr_t *addr)
{
        PROXY_TABLE = {
                PROXY_ADD_FAMILY(INET, INET_gethostbyname),
        };

        if (family < _NET_FAMILY__COUNT && name && addr) {
                return call_proxy_function(family, name, addr);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 *
 * @param family
 * @param value
 * @return
 */
//==============================================================================
u16_t _net_hton_u16(NET_family_t family, u16_t value)
{
        PROXY_TABLE = {
                PROXY_ADD_FAMILY(INET, INET_hton_u16),
        };

        if (family < _NET_FAMILY__COUNT) {
                return call_proxy_function(family, value);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 *
 * @param family
 * @param value
 * @return
 */
//==============================================================================
u32_t _net_hton_u32(NET_family_t family, u32_t value)
{
        PROXY_TABLE = {
                PROXY_ADD_FAMILY(INET, INET_hton_u32),
        };

        if (family < _NET_FAMILY__COUNT) {
                return call_proxy_function(family, value);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 *
 * @param family
 * @param value
 * @return
 */
//==============================================================================
u64_t _net_hton_u64(NET_family_t family, u64_t value)
{
        PROXY_TABLE = {
                PROXY_ADD_FAMILY(INET, INET_hton_u64),
        };

        if (family < _NET_FAMILY__COUNT) {
                return call_proxy_function(family, value);
        } else {
                return EINVAL;
        }
}

/*==============================================================================
  End of file
==============================================================================*/
