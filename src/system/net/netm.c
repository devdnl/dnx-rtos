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
#define MAXIMUM_SAFE_UDP_PAYLOAD        508

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
        static const uint8_t net_socket_size[] = {
                [NET_FAMILY__INET    ] = sizeof(INET_socket_t),
                [NET_FAMILY__CAN     ] = sizeof(int),
                [NET_FAMILY__RFM     ] = sizeof(int),
                [NET_FAMILY__MICROLAN] = sizeof(int),
        };

        int err = _kzalloc(_MM_NET, sizeof(SOCKET), cast(void**, socket));
        if (!err) {
                err = _kzalloc(_MM_NET, net_socket_size[family], cast(void**, &(*socket)->ctx));
                if (!err) {
                        (*socket)->header.type = RES_TYPE_SOCKET;
                        (*socket)->family      = family;
                } else {
                        _kfree(_MM_NET, cast(void**, socket));
                }
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
        _kfree(_MM_NET, &(*socket)->ctx);
        _kfree(_MM_NET, cast(void**, socket));
        *socket = NULL;
}

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
 * @param prot
 * @param socket
 * @return
 */
//==============================================================================
static int INET_socket_create(NET_protocol_t prot, SOCKET **socket)
{
        int err = socket_alloc(socket, NET_FAMILY__INET);
        if (!err) {
                INET_socket_t *inet = (*socket)->ctx;

                inet->netconn = netconn_new(prot == NET_PROTOCOL__TCP
                                                  ? NETCONN_TCP
                                                  : NETCONN_UDP);
                if (!inet->netconn) {
                        err = ENOMEM;
                        socket_free(socket);
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
static int INET_socket_destroy(SOCKET *socket)
{
        INET_socket_t *inet = socket->ctx;

        if (inet->netbuf) {
                netbuf_delete(inet->netbuf);
        }

        if (inet->netconn) {
                netconn_close(inet->netconn);
                netconn_delete(inet->netconn);
        }

        socket_free(&socket);

        return ESUCC;
}

//==============================================================================
/**
 *
 * @param socket
 * @param addr
 * @return
 */
//==============================================================================
static int INET_socket_bind(SOCKET *socket, const NET_INET_addr_t *addr)
{
        INET_socket_t *inet = socket->ctx;

        ip_addr_t IP;
        IP4_ADDR(&IP, addr->addr[0], addr->addr[1], addr->addr[2], addr->addr[3]);

        return INET_lwIP_status_to_errno(netconn_bind(inet->netconn, &IP, addr->port));
}

//==============================================================================
/**
 *
 * @param socket
 * @return
 */
//==============================================================================
static int INET_socket_listen(SOCKET *socket)
{
        INET_socket_t *inet = socket->ctx;

        return INET_lwIP_status_to_errno(netconn_listen(inet->netconn));
}

//==============================================================================
/**
 *
 * @param socket
 * @param new_socket
 * @return
 */
//==============================================================================
static int INET_socket_accept(SOCKET *socket, SOCKET **new_socket)
{
        int err = socket_alloc(new_socket, NET_FAMILY__INET);
        if (!err) {
                INET_socket_t *inet     = socket->ctx;
                INET_socket_t *new_inet = (*new_socket)->ctx;

                err = INET_lwIP_status_to_errno(netconn_accept(inet->netconn,
                                                               &new_inet->netconn));
                if (err) {
                        socket_free(new_socket);
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
static int INET_socket_recv(SOCKET     *socket,
                            void       *buf,
                            uint16_t    len,
                            NET_flags_t flags,
                            uint16_t   *recved)
{
        INET_socket_t *inet = socket->ctx;

        if (flags & NET_FLAGS__REWIND) {
                inet->seek = 0;
        }

        int err = ESUCC;
        if (inet->netbuf == NULL) {
                err = INET_lwIP_status_to_errno(netconn_recv(inet->netconn,
                                                             &inet->netbuf));
        }

        if (!err) {
                u16_t sz = netbuf_copy_partial(inet->netbuf, buf, len, inet->seek);
                inet->seek += sz;
                *recved     = sz;

                if ((flags & NET_FLAGS__FREEBUF) || inet->seek >= netbuf_len(inet->netbuf)) {
                        netbuf_delete(inet->netbuf);
                        inet->netbuf = NULL;
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
static int INET_socket_send(SOCKET     *socket,
                            const void *buf,
                            uint16_t    len,
                            NET_flags_t flags,
                            uint16_t   *sent)
{
        INET_socket_t *inet = socket->ctx;

        int err = EINVAL;

        enum netconn_type type = netconn_type(inet->netconn);

        if (type & NETCONN_TCP) {
                u8_t lwip_flags = 0;
                if (flags & NET_FLAGS__NOCOPY)
                        lwip_flags |= NETCONN_NOCOPY;
                else
                        lwip_flags |= NETCONN_COPY;

                err = INET_lwIP_status_to_errno(netconn_write(inet->netconn,
                                                              buf,
                                                              len,
                                                              lwip_flags));
                *sent = len;

        } else if (type & NETCONN_UDP) {

                if (len <= MAXIMUM_SAFE_UDP_PAYLOAD) {

                        inet->netbuf = netbuf_new();
                        if (inet->netbuf) {
                                if (flags & NET_FLAGS__NOCOPY) {
                                        err = INET_lwIP_status_to_errno(
                                                        netbuf_ref(inet->netbuf,
                                                                   buf,
                                                                   len));
                                } else {
                                        char *data = netbuf_alloc(inet->netbuf, len);
                                        if (data) {
                                                memcpy(data, buf, len);
                                                err = INET_lwIP_status_to_errno(
                                                         netconn_send(inet->netconn,
                                                                      inet->netbuf));
                                        } else {
                                                err = ENOMEM;
                                        }
                                }

                                if (!err) {
                                        err = INET_lwIP_status_to_errno(
                                                 netconn_send(inet->netconn,
                                                              inet->netbuf));
                                }

                                netbuf_delete(inet->netbuf);
                                inet->netbuf = NULL;

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
 * @param name
 * @param addr
 * @param addr_size
 * @return
 */
//==============================================================================
int INET_gethostbyname(const char *name, void *addr, size_t addr_size)
{
        int err = EINVAL;

        if (addr_size == sizeof(NET_INET_addr_t)) {

                ip_addr_t ip_addr;
                err = INET_lwIP_status_to_errno(netconn_gethostbyname(name, &ip_addr));
                if (!err) {
                        NET_INET_addr_t *IP = addr;

                        IP->addr[0] = ip4_addr1(&ip_addr);
                        IP->addr[1] = ip4_addr2(&ip_addr);
                        IP->addr[2] = ip4_addr3(&ip_addr);
                        IP->addr[3] = ip4_addr4(&ip_addr);
                        IP->port    = 0;
                }
        }

        return err;
}

//==============================================================================
/**
 *
 * @param socket
 * @param timeout
 * @return
 */
//==============================================================================
static int INET_socket_set_recv_timeout(SOCKET *socket, uint32_t timeout)
{
        INET_socket_t *inet = socket->ctx;

        return INET_lwIP_status_to_errno(netconn_set_sendtimeout(inet->netconn,
                                                                 timeout));
}

//==============================================================================
/**
 *
 * @param socket
 * @param timeout
 * @return
 */
//==============================================================================
static int INET_socket_set_send_timeout(SOCKET *socket, uint32_t timeout)
{
        INET_socket_t *inet = socket->ctx;

        return INET_lwIP_status_to_errno(netconn_set_recvtimeout(inet->netconn,
                                                                 timeout));
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
static int INET_socket_connect(SOCKET *socket, const NET_INET_addr_t *addr)
{
        INET_socket_t *inet = socket->ctx;

        ip_addr_t IP;
        IP4_ADDR(&IP, addr->addr[0], addr->addr[1], addr->addr[2], addr->addr[3]);

        return INET_lwIP_status_to_errno(netconn_connect(inet->netconn,
                                                         &IP, addr->port));
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
int _net_ifup(NET_family_t family, const void *config, size_t size)
{
        if (config && size) {
                switch (family) {
                case NET_FAMILY__INET:
                        if (sizeof(NET_INET_cfg_t) == size) {
                                _netman_init();

                                const NET_INET_cfg_t *cfg = config;

                                switch (cfg->mode) {
                                case NET_INET_MODE__STATIC: {
                                        ip_addr_t addr, mask, gateway;
                                        IP4_ADDR(&addr,
                                                 cfg->address[0],
                                                 cfg->address[1],
                                                 cfg->address[2],
                                                 cfg->address[3]);

                                        IP4_ADDR(&mask,
                                                 cfg->mask[0],
                                                 cfg->mask[1],
                                                 cfg->mask[2],
                                                 cfg->mask[3]);

                                        IP4_ADDR(&gateway,
                                                 cfg->gateway[0],
                                                 cfg->gateway[1],
                                                 cfg->gateway[2],
                                                 cfg->gateway[3]);

                                        return _netman_if_up(&addr, &mask, &gateway);
                                }
                                break;

                                case NET_INET_MODE__DHCP_START:
                                        return _netman_start_DHCP_client();
                                break;

                                case NET_INET_MODE__DHCP_INFORM:
                                        return _netman_inform_DHCP_server();
                                break;

                                case NET_INET_MODE__DHCP_RENEW:
                                        return _netman_renew_DHCP_connection();
                                default:
                                        return EINVAL;
                                }
                        } else {
                                return EINVAL;
                        }
                        break;

                case NET_FAMILY__CAN:
                        return ENOTSUP;

                case NET_FAMILY__MICROLAN:
                        return ENOTSUP;

                case NET_FAMILY__RFM:
                        return ENOTSUP;

                default:
                        return EINVAL;
                }
        }

        return EINVAL;
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
        switch (family) {
        case NET_FAMILY__INET:
                return _netman_if_down();

        case NET_FAMILY__CAN:
                return ENOTSUP;

        case NET_FAMILY__MICROLAN:
                return ENOTSUP;

        case NET_FAMILY__RFM:
                return ENOTSUP;

        default:
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
int _net_ifstatus(NET_family_t family, void *status, size_t size)
{
        if (status && size) {
                switch (family) {
                case NET_FAMILY__INET:
                        if (sizeof(NET_INET_status_t) == size) {
                                NET_INET_status_t *stat = status;

                                _ifconfig_t inetstat;
                                if (_netman_get_ifconfig(&inetstat) == ESUCC) {
                                        memcpy(stat->hw_addr, inetstat.hw_address, sizeof(stat->hw_addr));
                                        stat->rx_bytes   = inetstat.rx_bytes;
                                        stat->tx_bytes   = inetstat.tx_bytes;
                                        stat->rx_packets = inetstat.rx_packets;
                                        stat->tx_packets = inetstat.tx_packets;

                                        stat->address[0] = ip4_addr1(&inetstat.IP_address);
                                        stat->address[1] = ip4_addr2(&inetstat.IP_address);
                                        stat->address[2] = ip4_addr3(&inetstat.IP_address);
                                        stat->address[3] = ip4_addr4(&inetstat.IP_address);

                                        stat->mask[0]    = ip4_addr1(&inetstat.net_mask);
                                        stat->mask[1]    = ip4_addr2(&inetstat.net_mask);
                                        stat->mask[2]    = ip4_addr3(&inetstat.net_mask);
                                        stat->mask[3]    = ip4_addr4(&inetstat.net_mask);

                                        stat->gateway[0] = ip4_addr1(&inetstat.gateway);
                                        stat->gateway[1] = ip4_addr2(&inetstat.gateway);
                                        stat->gateway[2] = ip4_addr3(&inetstat.gateway);
                                        stat->gateway[3] = ip4_addr4(&inetstat.gateway);

                                        stat->state      = inetstat.status;
                                        return ESUCC;
                                }

                                return EINVAL;
                        }

                        return EINVAL;

                case NET_FAMILY__CAN:
                        return ENOTSUP;

                case NET_FAMILY__MICROLAN:
                        return ENOTSUP;

                case NET_FAMILY__RFM:
                        return ENOTSUP;

                default:
                        return EINVAL;
                }
        }

        return EINVAL;
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
int _net_socketcreate(NET_family_t family, NET_protocol_t protocol, SOCKET **socket)
{
        int err = EINVAL;

        if (socket) {
                switch (family) {
                case NET_FAMILY__INET:
                        if (  protocol == NET_PROTOCOL__UDP
                           || protocol == NET_PROTOCOL__TCP) {

                                err = INET_socket_create(protocol, socket);
                        }
                        break;

                case NET_FAMILY__CAN:
                        err = ENOTSUP;

                case NET_FAMILY__MICROLAN:
                        err = ENOTSUP;

                case NET_FAMILY__RFM:
                        err = ENOTSUP;

                default:
                        err = EINVAL;
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
int _net_socketdestroy(SOCKET *socket)
{
        int err = EINVAL;

        if (socket && socket->header.type == RES_TYPE_SOCKET) {
                switch (socket->family) {
                case NET_FAMILY__INET:
                        err = INET_socket_destroy(socket);
                        break;

                case NET_FAMILY__CAN:
                        err = ENOTSUP;

                case NET_FAMILY__MICROLAN:
                        err = ENOTSUP;

                case NET_FAMILY__RFM:
                        err = ENOTSUP;

                default:
                        err = EINVAL;
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
int _net_socketbind(SOCKET *socket, const void *addr, size_t addr_size)
{
        int err = EINVAL;

        if (socket && socket->header.type == RES_TYPE_SOCKET) {
                switch (socket->family) {
                case NET_FAMILY__INET:
                        if (addr_size == sizeof(NET_INET_addr_t)) {
                                err = INET_socket_bind(socket, addr);
                        }
                        break;

                case NET_FAMILY__CAN:
                        err = ENOTSUP;

                case NET_FAMILY__MICROLAN:
                        err = ENOTSUP;

                case NET_FAMILY__RFM:
                        err = ENOTSUP;

                default:
                        err = EINVAL;
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
int _net_socketlisten(SOCKET *socket)
{
        int err = EINVAL;

        if (socket && socket->header.type == RES_TYPE_SOCKET) {
                switch (socket->family) {
                case NET_FAMILY__INET:
                        err = INET_socket_listen(socket);
                        break;

                case NET_FAMILY__CAN:
                        err = ENOTSUP;

                case NET_FAMILY__MICROLAN:
                        err = ENOTSUP;

                case NET_FAMILY__RFM:
                        err = ENOTSUP;

                default:
                        err = EINVAL;
                }
        }

        return err;
}

//==============================================================================
/**
 *
 * @param socket
 * @param new_socket
 * @return
 */
//==============================================================================
int _net_socketaccept(SOCKET *socket, SOCKET **new_socket)
{
        int err = EINVAL;

        if (socket && socket->header.type == RES_TYPE_SOCKET) {
                switch (socket->family) {
                case NET_FAMILY__INET:
                        err = INET_socket_accept(socket, new_socket);
                        break;

                case NET_FAMILY__CAN:
                        err = ENOTSUP;

                case NET_FAMILY__MICROLAN:
                        err = ENOTSUP;

                case NET_FAMILY__RFM:
                        err = ENOTSUP;

                default:
                        err = EINVAL;
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
int _net_socketrecv(SOCKET *socket, void *buf, uint16_t len, NET_flags_t flags, u16_t *recved)
{
        int err = EINVAL;

        if (socket && socket->header.type == RES_TYPE_SOCKET && buf && len && recved) {
                switch (socket->family) {
                case NET_FAMILY__INET:
                        err = INET_socket_recv(socket, buf, len, flags, recved);
                        break;

                case NET_FAMILY__CAN:
                        err = ENOTSUP;

                case NET_FAMILY__MICROLAN:
                        err = ENOTSUP;

                case NET_FAMILY__RFM:
                        err = ENOTSUP;

                default:
                        err = EINVAL;
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
int _net_socketsend(SOCKET *socket, const void *buf, uint16_t len, NET_flags_t flags, u16_t *sent)
{
        int err = EINVAL;

        if (socket && socket->header.type == RES_TYPE_SOCKET && buf && len && sent) {
                switch (socket->family) {
                case NET_FAMILY__INET:
                        err = INET_socket_send(socket, buf, len, flags, sent);
                        break;

                case NET_FAMILY__CAN:
                        err = ENOTSUP;

                case NET_FAMILY__MICROLAN:
                        err = ENOTSUP;

                case NET_FAMILY__RFM:
                        err = ENOTSUP;

                default:
                        err = EINVAL;
                }
        }

        return err;
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
int _net_gethostbyname(NET_family_t family, const char *name, void *addr, size_t addr_size)
{
        int err = EINVAL;

        if (name && addr && addr_size) {
                switch (family) {
                case NET_FAMILY__INET:
                        err = INET_gethostbyname(name, addr, addr_size);
                        break;

                case NET_FAMILY__CAN:
                        err = ENOTSUP;

                case NET_FAMILY__MICROLAN:
                        err = ENOTSUP;

                case NET_FAMILY__RFM:
                        err = ENOTSUP;

                default:
                        err = EINVAL;
                }
        }

        return err;
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
        int err = EINVAL;

        if (socket && socket->header.type == RES_TYPE_SOCKET) {
                switch (socket->family) {
                case NET_FAMILY__INET:
                        err = INET_socket_set_recv_timeout(socket, timeout);
                        break;

                case NET_FAMILY__CAN:
                        err = ENOTSUP;

                case NET_FAMILY__MICROLAN:
                        err = ENOTSUP;

                case NET_FAMILY__RFM:
                        err = ENOTSUP;

                default:
                        err = EINVAL;
                }
        }

        return err;
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
        int err = EINVAL;

        if (socket && socket->header.type == RES_TYPE_SOCKET) {
                switch (socket->family) {
                case NET_FAMILY__INET:
                        err = INET_socket_set_send_timeout(socket, timeout);
                        break;

                case NET_FAMILY__CAN:
                        err = ENOTSUP;

                case NET_FAMILY__MICROLAN:
                        err = ENOTSUP;

                case NET_FAMILY__RFM:
                        err = ENOTSUP;

                default:
                        err = EINVAL;
                }
        }

        return err;
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
int _net_socket_connect(SOCKET *socket, const void *addr, size_t addr_size)
{
        int err = EINVAL;

        if (socket && socket->header.type == RES_TYPE_SOCKET && addr && addr_size) {
                switch (socket->family) {
                case NET_FAMILY__INET:
                        if (addr_size == sizeof(NET_INET_addr_t)) {
                                err = INET_socket_connect(socket, addr);
                        }
                        break;

                case NET_FAMILY__CAN:
                        err = ENOTSUP;

                case NET_FAMILY__MICROLAN:
                        err = ENOTSUP;

                case NET_FAMILY__RFM:
                        err = ENOTSUP;

                default:
                        err = EINVAL;
                }
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
