/*=========================================================================*//**
File     netm.c

Author   Daniel Zorychta

Brief    Network management.

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


*//*==========================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include "net/netm.h"
#include "net/inet/inet.h"
#include "net/sipc/sipc.h"
#include "cpuctl.h"
#include "kernel/sysfunc.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define MAXIMUM_SAFE_UDP_PAYLOAD                508

#define PROXY_TABLE                             static const proxy_func_t proxy[_NET_FAMILY__COUNT]
#define PROXY_TABLE_U16                         static const proxy_func_u16_t proxy[_NET_FAMILY__COUNT]
#define PROXY_TABLE_U32                         static const proxy_func_u32_t proxy[_NET_FAMILY__COUNT]
#define PROXY_TABLE_U64                         static const proxy_func_u64_t proxy[_NET_FAMILY__COUNT]
#define PROXY_FUNCTION(_family, _proxy_func)    [NET_FAMILY__##_family] = (proxy_func_t)_family##_##_proxy_func
#define PROXY_FUNCTION_U16(_family, _proxy_func)[NET_FAMILY__##_family] = (proxy_func_u16_t)_family##_##_proxy_func
#define PROXY_FUNCTION_U32(_family, _proxy_func)[NET_FAMILY__##_family] = (proxy_func_u32_t)_family##_##_proxy_func
#define PROXY_FUNCTION_U64(_family, _proxy_func)[NET_FAMILY__##_family] = (proxy_func_u64_t)_family##_##_proxy_func
#define PROXY_ifup(_family)                     PROXY_FUNCTION(_family, ifup)
#define PROXY_ifdown(_family)                   PROXY_FUNCTION(_family, ifdown)
#define PROXY_ifstatus(_family)                 PROXY_FUNCTION(_family, ifstatus)
#define PROXY_gethostbyname(_family)            PROXY_FUNCTION(_family, gethostbyname)
#define PROXY_socket_create(_family)            PROXY_FUNCTION(_family, socket_create)
#define PROXY_socket_destroy(_family)           PROXY_FUNCTION(_family, socket_destroy)
#define PROXY_socket_bind(_family)              PROXY_FUNCTION(_family, socket_bind)
#define PROXY_socket_listen(_family)            PROXY_FUNCTION(_family, socket_listen)
#define PROXY_socket_accept(_family)            PROXY_FUNCTION(_family, socket_accept)
#define PROXY_socket_recv(_family)              PROXY_FUNCTION(_family, socket_recv)
#define PROXY_socket_recvfrom(_family)          PROXY_FUNCTION(_family, socket_recvfrom)
#define PROXY_socket_send(_family)              PROXY_FUNCTION(_family, socket_send)
#define PROXY_socket_sendto(_family)            PROXY_FUNCTION(_family, socket_sendto)
#define PROXY_socket_set_recv_timeout(_family)  PROXY_FUNCTION(_family, socket_set_recv_timeout)
#define PROXY_socket_set_send_timeout(_family)  PROXY_FUNCTION(_family, socket_set_send_timeout)
#define PROXY_socket_get_recv_timeout(_family)  PROXY_FUNCTION(_family, socket_get_recv_timeout)
#define PROXY_socket_get_send_timeout(_family)  PROXY_FUNCTION(_family, socket_get_send_timeout)
#define PROXY_socket_connect(_family)           PROXY_FUNCTION(_family, socket_connect)
#define PROXY_socket_disconnect(_family)        PROXY_FUNCTION(_family, socket_disconnect)
#define PROXY_socket_shutdown(_family)          PROXY_FUNCTION(_family, socket_shutdown)
#define PROXY_socket_getaddress(_family)        PROXY_FUNCTION(_family, socket_getaddress)
#define PROXY_hton_u16(_family)                 PROXY_FUNCTION_U16(_family, hton_u16)
#define PROXY_hton_u32(_family)                 PROXY_FUNCTION_U32(_family, hton_u32)
#define PROXY_hton_u64(_family)                 PROXY_FUNCTION_U64(_family, hton_u64)
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
typedef u16_t (*proxy_func_u16_t)();
typedef u32_t (*proxy_func_u32_t)();
typedef u64_t (*proxy_func_u64_t)();

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
 * @brief Function allocate socket.
 * @param socket        socket pointer
 * @param family        network family
 * @return One of @ref errno value.
 */
//==============================================================================
static int socket_alloc(SOCKET **socket, NET_family_t family)
{
        static const uint8_t net_socket_size[_NET_FAMILY__COUNT] = {
                #if __ENABLE_TCPIP_STACK__ > 0
                [NET_FAMILY__INET] = _mm_align(sizeof(INET_socket_t)),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                [NET_FAMILY__SIPC] = _mm_align(sizeof(SIPC_socket_t)),
                #endif
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
 * @brief Function free allocated socket object.
 * @param socket        socket to free
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
 * @brief Function check if socket object is valid
 * @param socket        socket to validate
 * @return One of @ref errno value.
 */
//==============================================================================
static bool is_socket_valid(SOCKET *socket)
{
        return _mm_is_object_in_heap(socket)
            && (socket->header.type == RES_TYPE_SOCKET)
            && (socket->family < _NET_FAMILY__COUNT)
            && (socket->ctx == cast(void *, cast(size_t, socket)
                                          + _mm_align(sizeof(SOCKET))));
}

//==============================================================================
/**
 * @brief Function setup network interface.
 * @param family        network family
 * @param config        configuration object (generic)
 * @return One of @ref errno value.
 */
//==============================================================================
int _net_ifup(NET_family_t family, const NET_generic_config_t *config)
{
        PROXY_TABLE = {
                #if __ENABLE_TCPIP_STACK__ > 0
                PROXY_ifup(INET),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                PROXY_ifup(SIPC),
                #endif
        };

        if (family < _NET_FAMILY__COUNT && config) {
                return call_proxy_function(family, config);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function shutdown network interface.
 * @param family        network family
 * @return One of @ref errno value.
 */
//==============================================================================
int _net_ifdown(NET_family_t family)
{
        PROXY_TABLE = {
                #if __ENABLE_TCPIP_STACK__ > 0
                PROXY_ifdown(INET),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                PROXY_ifdown(SIPC),
                #endif
        };

        if (family < _NET_FAMILY__COUNT) {
                return call_proxy_function(family);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief  Function return status of network interface.
 * @param  family       network family
 * @param  status       network status
 * @return One of @ref errno value.
 */
//==============================================================================
int _net_ifstatus(NET_family_t family,  NET_generic_status_t *status)
{
        PROXY_TABLE = {
                #if __ENABLE_TCPIP_STACK__ > 0
                PROXY_ifstatus(INET),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                PROXY_ifstatus(SIPC),
                #endif
        };

        if (family < _NET_FAMILY__COUNT && status) {
                return call_proxy_function(family, status);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function create socket for specified network interface.
 * @param family        network family
 * @param protocol      network protocol
 * @param socket        created socket
 * @return One of @ref errno value.
 */
//==============================================================================
int _net_socket_create(NET_family_t family, NET_protocol_t protocol, SOCKET **socket)
{
        PROXY_TABLE = {
                #if __ENABLE_TCPIP_STACK__ > 0
                PROXY_socket_create(INET),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                PROXY_socket_create(SIPC),
                #endif
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
 * @brief Function destroy socket of specified network.
 * @param socket        socket to destroy
 * @return One of @ref errno value.
 */
//==============================================================================
int _net_socket_destroy(SOCKET *socket)
{
        PROXY_TABLE = {
                #if __ENABLE_TCPIP_STACK__ > 0
                PROXY_socket_destroy(INET),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                PROXY_socket_destroy(SIPC),
                #endif
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
 * @brief Function bind socket with address.
 * @param socket        socket to bind
 * @param addr          addres to bind
 * @return One of @ref errno value.
 */
//==============================================================================
int _net_socket_bind(SOCKET *socket, const NET_generic_sockaddr_t *addr)
{
        PROXY_TABLE = {
                #if __ENABLE_TCPIP_STACK__ > 0
                PROXY_socket_bind(INET),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                PROXY_socket_bind(SIPC),
                #endif
        };

        if (is_socket_valid(socket) && addr) {
                return call_proxy_function(socket->family, socket->ctx, addr);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function listen connection on socket.
 * @param socket        socket to listen
 * @return One of @ref errno value.
 */
//==============================================================================
int _net_socket_listen(SOCKET *socket)
{
        PROXY_TABLE = {
                #if __ENABLE_TCPIP_STACK__ > 0
                PROXY_socket_listen(INET),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                PROXY_socket_listen(SIPC),
                #endif
        };

        if (is_socket_valid(socket)) {
                return call_proxy_function(socket->family, socket->ctx);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function accept incoming connection.
 * @param socket        socket accepting connection
 * @param new_socket    socket of new connection
 * @return One of @ref errno value.
 */
//==============================================================================
int _net_socket_accept(SOCKET *socket, SOCKET **new_socket)
{
        PROXY_TABLE = {
                #if __ENABLE_TCPIP_STACK__ > 0
                PROXY_socket_accept(INET),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                PROXY_socket_accept(SIPC),
                #endif
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
 * @brief Function receive bytes from selected socket.
 * @param socket        socket to receive
 * @param buf           destination buffer
 * @param len           bytes to receive
 * @param flags         control flags
 * @param recved        number of received bytes
 * @return One of @ref errno value.
 */
//==============================================================================
int _net_socket_recv(SOCKET *socket, void *buf, size_t len, NET_flags_t flags, size_t *recved)
{
        PROXY_TABLE = {
                #if __ENABLE_TCPIP_STACK__ > 0
                PROXY_socket_recv(INET),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                PROXY_socket_recv(SIPC),
                #endif
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
 * @brief Function receive bytes from selected socket and obtain sender address.
 * @param socket        socket to receive
 * @param buf           destination buffer
 * @param len           bytes to receive
 * @param flags         control flags
 * @param sockaddr      obtained address of received bytes
 * @param recved        number of received bytes
 * @return One of @ref errno value.
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
                #if __ENABLE_TCPIP_STACK__ > 0
                PROXY_socket_recvfrom(INET),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                PROXY_socket_recvfrom(SIPC),
                #endif
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
 * @brief Function send bytes to selected socket.
 * @param socket        socket that send bytes
 * @param buf           source buffer
 * @param len           number of bytes to send
 * @param flags         control flags
 * @param sent          number of sent bytes
 * @return One of @ref errno value.
 */
//==============================================================================
int _net_socket_send(SOCKET *socket, const void *buf, size_t len, NET_flags_t flags, size_t *sent)
{
        PROXY_TABLE = {
                #if __ENABLE_TCPIP_STACK__ > 0
                PROXY_socket_send(INET),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                PROXY_socket_send(SIPC),
                #endif
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
 * @brief Function send bytes by socket to selected address.
 * @param socket        socket that send bytes
 * @param buf           source buffer
 * @param len           number of bytes to send
 * @param flags         control flags
 * @param to_addr       destination address
 * @param sent          number of sent bytes
 * @return One of @ref errno value.
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
                #if __ENABLE_TCPIP_STACK__ > 0
                PROXY_socket_sendto(INET),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                PROXY_socket_sendto(SIPC),
                #endif
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
 * @brief Function set socket receive timeout.
 * @param socket        socket
 * @param timeout       timeout value
 * @return One of @ref errno value.
 */
//==============================================================================
int _net_socket_set_recv_timeout(SOCKET *socket, uint32_t timeout)
{
        PROXY_TABLE = {
                #if __ENABLE_TCPIP_STACK__ > 0
                PROXY_socket_set_recv_timeout(INET),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                PROXY_socket_set_recv_timeout(SIPC),
                #endif
        };

        if (is_socket_valid(socket)) {
                return call_proxy_function(socket->family, socket->ctx, timeout);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function set socket send timeout.
 * @param socket        socket
 * @param timeout       timeout value
 * @return One of @ref errno value.
 */
//==============================================================================
int _net_socket_set_send_timeout(SOCKET *socket, uint32_t timeout)
{
        PROXY_TABLE = {
                #if __ENABLE_TCPIP_STACK__ > 0
                PROXY_socket_set_send_timeout(INET),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                PROXY_socket_set_send_timeout(SIPC),
                #endif
        };

        if (is_socket_valid(socket)) {
                return call_proxy_function(socket->family, socket->ctx, timeout);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function get socket receive timeout.
 * @param socket        socket
 * @param timeout       timeout value
 * @return One of @ref errno value.
 */
//==============================================================================
int _net_socket_get_recv_timeout(SOCKET *socket, uint32_t *timeout)
{
        PROXY_TABLE = {
                #if __ENABLE_TCPIP_STACK__ > 0
                PROXY_socket_get_recv_timeout(INET),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                PROXY_socket_get_recv_timeout(SIPC),
                #endif
        };

        if (is_socket_valid(socket) && timeout) {
                return call_proxy_function(socket->family, socket->ctx, timeout);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function get socket send timeout.
 * @param socket        socket
 * @param timeout       timeout value
 * @return One of @ref errno value.
 */
//==============================================================================
int _net_socket_get_send_timeout(SOCKET *socket, uint32_t *timeout)
{
        PROXY_TABLE = {
                #if __ENABLE_TCPIP_STACK__ > 0
                PROXY_socket_get_send_timeout(INET),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                PROXY_socket_get_send_timeout(SIPC),
                #endif
        };

        if (is_socket_valid(socket) && timeout) {
                return call_proxy_function(socket->family, socket->ctx, timeout);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function connect socket to selected address.
 * @param socket        socket used for connection
 * @param addr          address where socket is connecting
 * @return One of @ref errno value.
 */
//==============================================================================
int _net_socket_connect(SOCKET *socket, const NET_generic_sockaddr_t *addr)
{
        PROXY_TABLE = {
                #if __ENABLE_TCPIP_STACK__ > 0
                PROXY_socket_connect(INET),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                PROXY_socket_connect(SIPC),
                #endif
        };

        if (is_socket_valid(socket) && addr) {
                return call_proxy_function(socket->family, socket->ctx, addr);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function disconnect socket.
 * @param socket        socket to disconnect
 * @return One of @ref errno value.
 */
//==============================================================================
int _net_socket_disconnect(SOCKET *socket)
{
        PROXY_TABLE = {
                #if __ENABLE_TCPIP_STACK__ > 0
                PROXY_socket_disconnect(INET),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                PROXY_socket_disconnect(SIPC),
                #endif
        };

        if (is_socket_valid(socket)) {
                return call_proxy_function(socket->family, socket->ctx);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function shutdown selected connection direction.
 * @param socket        socket to shutdown
 * @param how           direction to shutdown
 * @return One of @ref errno value.
 */
//==============================================================================
int _net_socket_shutdown(SOCKET *socket, NET_shut_t how)
{
        PROXY_TABLE = {
                #if __ENABLE_TCPIP_STACK__ > 0
                PROXY_socket_shutdown(INET),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                PROXY_socket_shutdown(SIPC),
                #endif
        };

        if (is_socket_valid(socket)) {
                return call_proxy_function(socket->family, socket->ctx, how);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function return address to which socket is connected.
 * @param socket        socket
 * @param sockaddr      address of connection
 * @return One of @ref errno value.
 */
//==============================================================================
int _net_socket_getaddress(SOCKET *socket, NET_generic_sockaddr_t *sockaddr)
{
        PROXY_TABLE = {
                #if __ENABLE_TCPIP_STACK__ > 0
                PROXY_socket_getaddress(INET),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                PROXY_socket_getaddress(SIPC),
                #endif
        };

        if (is_socket_valid(socket) && sockaddr) {
                return call_proxy_function(socket->family, socket->ctx, sockaddr);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function return address of host by name.
 * @param family        network family
 * @param name          host name
 * @param addr          obtained address
 * @return One of @ref errno value.
 */
//==============================================================================
int _net_gethostbyname(NET_family_t family, const char *name, NET_generic_sockaddr_t *addr)
{
        PROXY_TABLE = {
                #if __ENABLE_TCPIP_STACK__ > 0
                PROXY_gethostbyname(INET),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                PROXY_gethostbyname(SIPC),
                #endif
        };

        if (family < _NET_FAMILY__COUNT && name && addr) {
                return call_proxy_function(family, name, addr);
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief Function convert host byte order to network.
 * @param family        network family
 * @param value         value to convert
 * @return Converted value.
 */
//==============================================================================
u16_t _net_hton_u16(NET_family_t family, u16_t value)
{
        PROXY_TABLE_U16 = {
                #if __ENABLE_TCPIP_STACK__ > 0
                PROXY_hton_u16(INET),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                PROXY_hton_u16(SIPC),
                #endif
        };

        if (family < _NET_FAMILY__COUNT) {
                return call_proxy_function(family, value);
        } else {
                return value;
        }
}

//==============================================================================
/**
 * @brief Function convert host byte order to network.
 * @param family        network family
 * @param value         value to convert
 * @return Converted value.
 */
//==============================================================================
u32_t _net_hton_u32(NET_family_t family, u32_t value)
{
        PROXY_TABLE_U32 = {
                #if __ENABLE_TCPIP_STACK__ > 0
                PROXY_hton_u32(INET),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                PROXY_hton_u32(SIPC),
                #endif
        };

        if (family < _NET_FAMILY__COUNT) {
                return call_proxy_function(family, value);
        } else {
                return value;
        }
}

//==============================================================================
/**
 * @brief Function convert host byte order to network.
 * @param family        network family
 * @param value         value to convert
 * @return Converted value.
 */
//==============================================================================
u64_t _net_hton_u64(NET_family_t family, u64_t value)
{
        PROXY_TABLE_U64 = {
                #if __ENABLE_TCPIP_STACK__ > 0
                PROXY_hton_u64(INET),
                #endif
                #if __ENABLE_SIPC_STACK__ > 0
                PROXY_hton_u64(SIPC),
                #endif
        };

        if (family < _NET_FAMILY__COUNT) {
                return call_proxy_function(family, value);
        } else {
                return value;
        }
}

/*==============================================================================
  End of file
==============================================================================*/
