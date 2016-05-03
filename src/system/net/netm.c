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

#include "net/netman.h" // TODO

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
typedef struct {
        struct netconn *netconn;
        struct netbuf  *netbuf;
        uint16_t        seek;
} INET_socket_t;

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
 * @param err
 * @return
 */
//==============================================================================
static int INET_lwIP_status_to_errno(err_t err)
{
        switch (err) {
        case ERR_OK        : return ESUCC;
        case ERR_MEM       : return ENOMEM;
        case ERR_BUF       : return EIO;
        case ERR_TIMEOUT   : return ETIME;
        case ERR_RTE       : return EFAULT;
        case ERR_INPROGRESS: return EBUSY;
        case ERR_VAL       : return EINVAL;
        case ERR_ARG       : return EINVAL;
        case ERR_USE       : return EADDRINUSE;
        case ERR_ISCONN    : return EADDRINUSE;
        case ERR_WOULDBLOCK: return EFAULT;
        case ERR_ABRT      : return EFAULT;
        case ERR_RST       : return EFAULT;
        case ERR_CLSD      : return EFAULT;
        case ERR_CONN      : return EFAULT;
        case ERR_IF        : return EFAULT;
        default            : return EFAULT;
        }
}

//==============================================================================
/**
 *
 * @param addr
 * @param lwip_addr
 */
//==============================================================================
static void INET_addr_from_lwIP(NET_INET_IPv4_t *addr, const ip_addr_t *lwip_addr)
{
        *addr = NET_INET_IPv4(ip4_addr1(lwip_addr),
                              ip4_addr2(lwip_addr),
                              ip4_addr3(lwip_addr),
                              ip4_addr4(lwip_addr));
}

//==============================================================================
/**
 *
 * @param lwip_addr
 * @param addr
 */
//==============================================================================
static void INET_lwIP_addr(ip_addr_t *lwip_addr, const NET_INET_IPv4_t *addr)
{
        IP4_ADDR(lwip_addr,
                 NET_INET_IPv4_a(*addr),
                 NET_INET_IPv4_b(*addr),
                 NET_INET_IPv4_c(*addr),
                 NET_INET_IPv4_d(*addr));
}

//==============================================================================
/**
 *
 * @param config
 */
//==============================================================================
static int INET_ifup(const NET_INET_config_t *cfg)
{
        int err = EINVAL;

        _netman_init();

        switch (cfg->mode) {
        case NET_INET_MODE__STATIC: {
                ip_addr_t addr, mask, gateway;
                INET_lwIP_addr(&addr, &cfg->address);
                INET_lwIP_addr(&mask, &cfg->mask);
                INET_lwIP_addr(&gateway, &cfg->gateway);

                err = _netman_if_up(&addr, &mask, &gateway);

                break;
        }

        case NET_INET_MODE__DHCP_START:
                err = _netman_start_DHCP_client();
                break;

        case NET_INET_MODE__DHCP_INFORM:
                err = _netman_inform_DHCP_server();
                break;

        case NET_INET_MODE__DHCP_RENEW:
                err = _netman_renew_DHCP_connection();
                break;

        default:
                err = EINVAL;
        }

        return err;
}

//==============================================================================
/**
 *
 */
//==============================================================================
static int INET_ifdown(void)
{
        return _netman_if_down();
}

//==============================================================================
/**
 *
 * @param status
 * @return
 */
//==============================================================================
static int INET_ifstatus(NET_INET_status_t *status)
{
        _ifconfig_t inetstat;
        int err = _netman_get_ifconfig(&inetstat);

        if (!err) {
                memcpy(status->hw_addr, inetstat.hw_address, sizeof(status->hw_addr));
                status->rx_bytes   = inetstat.rx_bytes;
                status->tx_bytes   = inetstat.tx_bytes;
                status->rx_packets = inetstat.rx_packets;
                status->tx_packets = inetstat.tx_packets;

                INET_addr_from_lwIP(&status->address, &inetstat.IP_address);
                INET_addr_from_lwIP(&status->mask, &inetstat.net_mask);
                INET_addr_from_lwIP(&status->gateway, &inetstat.gateway);

                status->state = inetstat.status;
        }

        return err;
}

//==============================================================================
/**
 *
 * @param prot
 * @param inet_sock
 * @return
 */
//==============================================================================
static int INET_socket_create(NET_protocol_t prot, INET_socket_t *inet_sock)
{
        int err = EINVAL;

        if (prot == NET_PROTOCOL__TCP || prot == NET_PROTOCOL__UDP) {

                inet_sock->netconn = netconn_new(prot == NET_PROTOCOL__TCP
                                                       ? NETCONN_TCP
                                                       : NETCONN_UDP);
                if (inet_sock->netconn) {
                        err = ESUCC;
                } else {
                        err = ENOMEM;
                }
        }

        return err;
}

//==============================================================================
/**
 *
 * @param inet_sock
 * @return
 */
//==============================================================================
static int INET_socket_destroy(INET_socket_t *inet_sock)
{
        if (inet_sock->netbuf) {
                netbuf_delete(inet_sock->netbuf);
        }

        if (inet_sock->netconn) {
                netconn_close(inet_sock->netconn);
                netconn_delete(inet_sock->netconn);
        }

        return ESUCC;
}

//==============================================================================
/**
 *
 * @param inet_sock
 * @param addr
 * @param addr_size
 * @return
 */
//==============================================================================
static int INET_socket_connect(INET_socket_t *inet_sock, const NET_INET_sockaddr_t *addr)
{
        ip_addr_t IP;
        INET_lwIP_addr(&IP, &addr->addr);

        return INET_lwIP_status_to_errno(netconn_connect(inet_sock->netconn,
                                                         &IP, addr->port));
}

//==============================================================================
/**
 *
 * @param inet_sock
 * @param addr
 * @param addr_size
 * @return
 */
//==============================================================================
static int INET_socket_disconnect(INET_socket_t *inet_sock)
{
        return INET_lwIP_status_to_errno(netconn_disconnect(inet_sock->netconn));
}

//==============================================================================
/**
 *
 * @param inet_sock
 * @param addr
 * @param addr_size
 * @return
 */
//==============================================================================
static int INET_socket_shutdown(INET_socket_t *inet_sock, NET_shut_t how)
{
        return INET_lwIP_status_to_errno(netconn_shutdown(inet_sock->netconn,
                                                          how & NET_SHUT__RD,
                                                          how & NET_SHUT__WR));
}

//==============================================================================
/**
 *
 * @param inet_sock
 * @param addr
 * @return
 */
//==============================================================================
static int INET_socket_bind(INET_socket_t *inet_sock, const NET_INET_sockaddr_t *addr)
{
        ip_addr_t IP;
        INET_lwIP_addr(&IP, &addr->addr);

        return INET_lwIP_status_to_errno(netconn_bind(inet_sock->netconn,
                                                      &IP, addr->port));
}

//==============================================================================
/**
 *
 * @param socket
 * @return
 */
//==============================================================================
static int INET_socket_listen(INET_socket_t *inet_sock)
{
        return INET_lwIP_status_to_errno(netconn_listen(inet_sock->netconn));
}

//==============================================================================
/**
 *
 * @param socket
 * @param new_socket
 * @return
 */
//==============================================================================
static int INET_socket_accept(INET_socket_t *inet_sock, INET_socket_t *new_inet_sock)
{
        return INET_lwIP_status_to_errno(netconn_accept(inet_sock->netconn,
                                                        &new_inet_sock->netconn));
}

//==============================================================================
/**
 *
 * @param inet_sock
 * @param buf
 * @param len
 * @param flags
 * @return
 */
//==============================================================================
static int INET_socket_recv(INET_socket_t *inet_sock,
                            void          *buf,
                            size_t         len,
                            NET_flags_t    flags,
                            size_t        *recved)
{
        if (flags & NET_FLAGS__REWIND) {
                inet_sock->seek = 0;
        }

        int err = ESUCC;
        if (inet_sock->netbuf == NULL) {
                err = INET_lwIP_status_to_errno(netconn_recv(inet_sock->netconn,
                                                             &inet_sock->netbuf));
        }

        if (!err) {
                u16_t sz = netbuf_copy_partial(inet_sock->netbuf, buf, len, inet_sock->seek);
                inet_sock->seek += sz;
                *recved          = sz;

                if (  (flags & NET_FLAGS__FREEBUF)
                   || (inet_sock->seek >= netbuf_len(inet_sock->netbuf)) ) {

                        netbuf_delete(inet_sock->netbuf);
                        inet_sock->netbuf = NULL;
                }
        }

        return err;
}

//==============================================================================
/**
 *
 * @param inet_sock
 * @param buf
 * @param len
 * @param flags
 * @param sockaddr
 * @param recved
 * @return
 */
//==============================================================================
static int INET_socket_recvfrom(INET_socket_t       *inet_sock,
                                void                *buf,
                                size_t               len,
                                NET_flags_t          flags,
                                NET_INET_sockaddr_t *sockaddr,
                                size_t              *recved)
{
        UNUSED_ARG1(flags);

        int err = EPERM;

        enum netconn_type type = netconn_type(inet_sock->netconn);

        if (type & NETCONN_UDP) {
                struct netbuf *netbuf;
                err = INET_lwIP_status_to_errno(netconn_recv(inet_sock->netconn,
                                                             &netbuf));
                if (!err) {
                        *recved = netbuf_copy(netbuf, buf, len);

                        ip_addr_t *fromaddr = netbuf_fromaddr(netbuf);
                        sockaddr->port      = netbuf_fromport(netbuf);
                        INET_addr_from_lwIP(&sockaddr->addr, fromaddr);

                        netbuf_delete(netbuf);
                }
        }

        return err;
}

//==============================================================================
/**
 *
 * @param inet_sock
 * @param buf
 * @param len
 * @param flags
 * @param sent
 * @return
 */
//==============================================================================
static int INET_socket_send(INET_socket_t *inet_sock,
                            const void    *buf,
                            size_t         len,
                            NET_flags_t    flags,
                            size_t        *sent)
{
        int err = EINVAL;

        enum netconn_type type = netconn_type(inet_sock->netconn);

        if (type & NETCONN_TCP) {
                u8_t lwip_flags = 0;
                if (flags & NET_FLAGS__NOCOPY)
                        lwip_flags |= NETCONN_NOCOPY;
                else
                        lwip_flags |= NETCONN_COPY;

                err = INET_lwIP_status_to_errno(netconn_write(inet_sock->netconn,
                                                              buf,
                                                              len,
                                                              lwip_flags));
                *sent = len;

        } else if (type & NETCONN_UDP) {

                if (len <= MAXIMUM_SAFE_UDP_PAYLOAD) {

                        inet_sock->netbuf = netbuf_new();
                        if (inet_sock->netbuf) {
                                if (flags & NET_FLAGS__NOCOPY) {
                                        err = INET_lwIP_status_to_errno(
                                                        netbuf_ref(inet_sock->netbuf,
                                                                   buf,
                                                                   len));
                                } else {
                                        char *data = netbuf_alloc(inet_sock->netbuf, len);
                                        if (data) {
                                                memcpy(data, buf, len);
                                                err = ESUCC;
                                        } else {
                                                err = ENOMEM;
                                        }
                                }

                                if (!err) {
                                        err = INET_lwIP_status_to_errno(
                                                 netconn_send(inet_sock->netconn,
                                                              inet_sock->netbuf));
                                }

                                netbuf_delete(inet_sock->netbuf);
                                inet_sock->netbuf = NULL;

                        } else {
                                err = ENOMEM;
                        }
                } else {
                        err = EFBIG;
                }

        } else {
                err = EFAULT;
        }

        return err;
}

//==============================================================================
/**
 *
 * @param inet_sock
 * @param buf
 * @param len
 * @param flags
 * @param to_addr
 * @param to_addr_sz
 * @param sent
 * @return
 */
//==============================================================================
static int INET_socket_sendto(INET_socket_t             *inet_sock,
                              const void                *buf,
                              size_t                     len,
                              NET_flags_t                flags,
                              const NET_INET_sockaddr_t *to_sockaddr,
                              size_t                    *sent)
{
        int err = EPERM;

        enum netconn_type type = netconn_type(inet_sock->netconn);

        if (type & NETCONN_UDP) {

                ip_addr_t           lwip_addr;
                NET_INET_sockaddr_t sockaddr;

                // get the peer if currently connected
                err = INET_lwIP_status_to_errno(netconn_peer(inet_sock->netconn,
                                                             &lwip_addr,
                                                             &sockaddr.port));

                if (!err) {
                        INET_addr_from_lwIP(&sockaddr.addr, &lwip_addr);

                        // connect to new address
                        err = INET_socket_connect(inet_sock, to_sockaddr);

                        if (!err) {
                                err = INET_socket_send(inet_sock, buf, len, flags, sent);
                        }

                        // reset the remote address and port number of the conn.
                        INET_socket_connect(inet_sock, &sockaddr);
                }
        }

        return err;
}

//==============================================================================
/**
 *
 * @param name
 * @param addr
 * @param addr_size
 * @return
 */
//==============================================================================
int INET_gethostbyname(const char *name, NET_INET_sockaddr_t *sock_addr)
{
        int err = EINVAL;

        ip_addr_t lwip_addr;
        err = INET_lwIP_status_to_errno(netconn_gethostbyname(name, &lwip_addr));
        if (!err) {
                INET_addr_from_lwIP(&sock_addr->addr, &lwip_addr);
        }

        return err;
}

//==============================================================================
/**
 *
 * @param inet_sock
 * @param timeout
 * @return
 */
//==============================================================================
static int INET_socket_set_recv_timeout(INET_socket_t *inet_sock, uint32_t timeout)
{
        netconn_set_sendtimeout(inet_sock->netconn,timeout);    // function returns nothing
        return ESUCC;
}

//==============================================================================
/**
 *
 * @param inet_sock
 * @param timeout
 * @return
 */
//==============================================================================
static int INET_socket_set_send_timeout(INET_socket_t *inet_sock, uint32_t timeout)
{
        netconn_set_recvtimeout(inet_sock->netconn,timeout);    // function returns nothing
        return ESUCC;
}

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

/*==============================================================================
  End of file
==============================================================================*/
