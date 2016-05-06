/*=========================================================================*//**
File     inet.c

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
#include "net/inet/inet.h"
#include "inet_types.h"
#include "cpuctl.h"
#include "lwip/api.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/tcpip.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define MAXIMUM_SAFE_UDP_PAYLOAD                508

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void network_interface_thread(void *sem);
static void clear_rx_tx_counters();
static bool wait_for_init_done();
static bool DHCP_is_started();
static void restore_configuration();
static int DHCP_start_client();
static int IF_up(const ip_addr_t *ip_address, const ip_addr_t *net_mask, const ip_addr_t *gateway);

/*==============================================================================
  External function prototypes
==============================================================================*/
extern bool  _netman_ifmem_alloc        (netman_t *netman);
extern void  _netman_ifmem_free         (netman_t *netman);
extern bool  _netman_HW_init            (netman_t *netman);
extern void  _netman_HW_deinit          (netman_t *netman);
extern err_t _netman_netif_init         (struct netif *netif);
extern void  _netman_handle_input       (netman_t *netman, uint input_timeout);
extern bool  _netman_is_link_connected  (netman_t *netman);

/*==============================================================================
  Local objects
==============================================================================*/
static netman_t  *netman;
static const uint access_timeout = 10000;
static const uint DHCP_timeout   = 5000;
static const uint init_timeout   = 5000;
static const uint input_timeout  = 5000;
static const uint link_poll_time = 250;

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
 * @brief  Clear received and transmitted byte counters
 * @param  None
 * @return None
 */
//==============================================================================
static void clear_rx_tx_counters()
{
        netman->rx_bytes   = 0;
        netman->tx_bytes   = 0;
        netman->rx_packets = 0;
        netman->tx_packets = 0;
}

//==============================================================================
/**
 * @brief  Function wait for interface to be available
 * @param  None
 * @return If interface is re
 */
//==============================================================================
static bool wait_for_init_done()
{
        u32_t timer = sys_get_time_ms();
        while (not netman->ready && not sys_time_is_expired(timer, init_timeout)) {
                sys_msleep(1);
        }

        return not sys_time_is_expired(timer, init_timeout);
}

//==============================================================================
/**
 * @brief  Function restores last configuration after link connection
 * @param  None
 * @return None
 */
//==============================================================================
static void restore_configuration()
{
        bool was_DHCP;
        ip_addr_t ip_addr, gw, netmask;

        netman->disconnected = true;

        if (DHCP_is_started()) {
                dhcp_release(&netman->netif);
                dhcp_stop(&netman->netif);
                was_DHCP = true;
        } else {
                ip_addr = netman->netif.ip_addr;
                gw      = netman->netif.gw;
                netmask = netman->netif.netmask;
                was_DHCP = false;
        }

        netif_set_down(&netman->netif);

        if (sys_mutex_lock(netman->access, MAX_DELAY_MS) == ESUCC) {

                while (!_netman_is_link_connected(netman)) {
                        sys_msleep(link_poll_time);
                }

                netman->disconnected = false;

                if (netman->configured) {
                        if (was_DHCP) {
                                DHCP_start_client();
                        } else {
                                IF_up(&ip_addr, &netmask, &gw);
                        }
                }

                sys_mutex_unlock(netman->access);
        }
}

//==============================================================================
/**
 * @brief Network interface thread
 *
 * This task is used to open the interface file (e.g. Ethernet interface).
 * Task should wait for file until to be available. Task starts at system
 * startup, parallel to initd and tcpipd tasks. The task allocates a memory
 * needed by interface (depends on implementation). After memory initialization,
 * task initialize hardware (set MAC address, start interface) and indicate that
 * all operations finished successfully. After this operation the task try read
 * incoming packets from the interface. If packet is not received in the
 * specified time, then task check if connection is available. If connection is
 * not available then TCPIP stack is turned off. If connection start again then
 * task initialize TCPIP stack to the last configuration.
 *
 * @param  arg          task's argument
 * @return None
 */
//==============================================================================
static void network_interface_thread(void *arg)
{
        UNUSED_ARG1(arg);

        /* open interface file */
        while (netman->if_file == NULL) {
                sys_fopen(__NETWORK_ETHIF_FILE__, "r+", &netman->if_file);
                sys_msleep(100);
        }

        /* initialize interface */
        if (_netman_ifmem_alloc(netman)) {

                if (_netman_HW_init(netman)) {

                        netman->ready = true;

                        while (true) {
                                _netman_handle_input(netman, input_timeout);

                                if (!_netman_is_link_connected(netman)) {
                                        restore_configuration();
                                }
                        }

                        _netman_HW_deinit(netman);
                }

                _netman_ifmem_free(netman);
        }

        // error occurred
        return;
}

//==============================================================================
/**
 *
 * @param err
 * @return
 */
//==============================================================================
static int lwIP_status_to_errno(err_t err)
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
static void create_addr(NET_INET_IPv4_t *addr, const ip_addr_t *lwip_addr)
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
static void create_lwIP_addr(ip_addr_t *lwip_addr, const NET_INET_IPv4_t *addr)
{
        IP4_ADDR(lwip_addr,
                 NET_INET_IPv4_a(*addr),
                 NET_INET_IPv4_b(*addr),
                 NET_INET_IPv4_c(*addr),
                 NET_INET_IPv4_d(*addr));
}

//==============================================================================
/**
 * @brief  Function starts network manager, TCP/IP stack, and set network interface.
 *         Function is called before kernel start.
 * @param  None
 * @return One of @ref errno value.
 */
//==============================================================================
static int stack_init() // FIXME
{
        if (netman)
                return ESUCC;

        netman = _netcalloc(1, sizeof(netman_t));
        if (netman) {
                static const thread_attr_t attr = {
                        .priority    = PRIORITY_NORMAL,
                        .stack_depth = STACK_DEPTH_LOW
                };

                int ts = sys_thread_create(network_interface_thread, &attr, NULL, &netman->if_thread);
                int ms = sys_mutex_create(MUTEX_TYPE_RECURSIVE, &netman->access);

                if (ms == ESUCC && ts == ESUCC) {

                        tcpip_init(NULL, NULL);

                        netif_set_default(netif_add(&netman->netif,
                                                    const_cast(ip_addr_t*, &ip_addr_any),
                                                    const_cast(ip_addr_t*, &ip_addr_any),
                                                    const_cast(ip_addr_t*, &ip_addr_any),
                                                    netman,
                                                    _netman_netif_init,
                                                    tcpip_input));
                        return ESUCC;

                } else {
                        if (ms)
                                sys_mutex_destroy(netman->access);

                        if (ts)
                                sys_thread_destroy(&netman->if_thread);

                        _netfree(netman);
                        netman = NULL;
                }
        }

        return ENOMEM;
}

//==============================================================================
/**
 * @brief  Function configure interface as static
 * @param  ip_address        a IP address
 * @param  net_mask          a net mask value
 * @param  gateway           a gateway address
 * @return One of @ref errno value.
 */
//==============================================================================
static int IF_up(const ip_addr_t *ip_address, const ip_addr_t *net_mask, const ip_addr_t *gateway)
{ // TODO move to INET_ifup
        int status = EINVAL;

        if (  netman && ip_address && net_mask && gateway
           && !netif_is_up(&netman->netif)
           && wait_for_init_done()
           && sys_mutex_lock(netman->access, access_timeout) == ESUCC ) {

                clear_rx_tx_counters();
                netif_set_down(&netman->netif);
                netif_set_addr(&netman->netif,
                               const_cast(ip_addr_t*, ip_address),
                               const_cast(ip_addr_t*, net_mask),
                               const_cast(ip_addr_t*, gateway) );
                netif_set_up(&netman->netif);

                netman->configured = true;

                status = ESUCC;

                sys_mutex_unlock(netman->access);
        }

        return status;
}

//==============================================================================
/**
 * @brief  Function starts DHCP client
 * @param  None
 * @return One of @ref errno value.
 */
//==============================================================================
static int DHCP_start_client()
{ // TODO move to INET_ifup
        int status = ENONET;

        if (  netman
           && !netif_is_up(&netman->netif)
           && wait_for_init_done()
           && sys_mutex_lock(netman->access, access_timeout) == ESUCC ) {

                clear_rx_tx_counters();
                netif_set_down(&netman->netif);
                netif_set_addr(&netman->netif,
                               const_cast(ip_addr_t*, &ip_addr_any),
                               const_cast(ip_addr_t*, &ip_addr_any),
                               const_cast(ip_addr_t*, &ip_addr_any) );

                if (dhcp_start(&netman->netif) == ERR_OK) {
                        netif_set_up(&netman->netif);

                        u32_t timer = sys_get_time_ms();
                        while (not sys_time_is_expired(timer, DHCP_timeout)) {

                                if (netman->netif.dhcp->state == DHCP_BOUND) {
                                        netman->configured = true;
                                        status = ESUCC;
                                        break;
                                } else {
                                        sys_msleep(100);
                                }
                        }
                }

                sys_mutex_unlock(netman->access);
        }

        return status;
}

//==============================================================================
/**
 * @brief  Function inform DHCP about current static configuration
 * @param  None
 * @return One of @ref errno value.
 */
//==============================================================================
int DHCP_inform_server()
{ // TODO move to INET_ifup
        int status = ENONET;

        if (  netman
           && netif_is_up(&netman->netif)
           && !DHCP_is_started()
           && sys_mutex_lock(netman->access, access_timeout) == ESUCC ) {

                dhcp_inform(&netman->netif);
                status = ESUCC;

                sys_mutex_unlock(netman->access);
        }

        return status;
}

//==============================================================================
/**
 * @brief  Function renew DHCP connection
 * @param  None
 * @return One of @ref errno value.
 */
//==============================================================================
int DHCP_renew_connection()
{ // TODO move to INET_ifup
        int status = ENONET;

        if (  netman
           && netif_is_up(&netman->netif)
           && DHCP_is_started()
           && sys_mutex_lock(netman->access, access_timeout) == ESUCC ) {

                if (dhcp_renew(&netman->netif) == ERR_OK) {

                        u32_t timeout = sys_get_time_ms();
                        while (not sys_time_is_expired(timeout, DHCP_timeout)) {

                                if (netman->netif.dhcp->state == DHCP_BOUND) {
                                        status = ESUCC;
                                        break;
                                } else {
                                        sys_msleep(500);
                                }
                        }
                }

                sys_mutex_unlock(netman->access);
        }

        return status;
}

//==============================================================================
/**
 * @brief  Function check if DHCP client is started
 * @param  None
 * @return If DHCP Client is started then true is returned, otherwise false.
 */
//==============================================================================
static bool DHCP_is_started()
{
        return (netman->netif.flags & NETIF_FLAG_DHCP);
}

//==============================================================================
/**
 *
 * @param config
 */
//==============================================================================
int INET_ifup(const NET_INET_config_t *cfg)
{
        int err = EINVAL;

        stack_init();

        switch (cfg->mode) {
        case NET_INET_MODE__STATIC: {
                ip_addr_t addr, mask, gateway;
                create_lwIP_addr(&addr, &cfg->address);
                create_lwIP_addr(&mask, &cfg->mask);
                create_lwIP_addr(&gateway, &cfg->gateway);

                err = IF_up(&addr, &mask, &gateway);

                break;
        }

        case NET_INET_MODE__DHCP_START:
                err = DHCP_start_client();
                break;

        case NET_INET_MODE__DHCP_INFORM:
                err = DHCP_inform_server();
                break;

        case NET_INET_MODE__DHCP_RENEW:
                err = DHCP_renew_connection();
                break;

        default:
                err = EINVAL;
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function turn down interface with static configuration.
 * @return One of @ref errno value.
 */
//==============================================================================
int INET_ifdown(void)
{
        int status = ENONET;

        if (  netman
           && netif_is_up(&netman->netif)
           && sys_mutex_lock(netman->access, access_timeout) == ESUCC ) {

                if (DHCP_is_started()) {
                        if (dhcp_release(&netman->netif) == ERR_OK) {
                                dhcp_stop(&netman->netif);
                        }
                }

                netif_set_down(&netman->netif);
                netman->configured = false;
                status = ESUCC;

                sys_mutex_unlock(netman->access);
        }

        return status;
}

//==============================================================================
/**
 *
 * @param status
 * @return
 */
//==============================================================================
int INET_ifstatus(NET_INET_status_t *status)
{
        int err = EINVAL;

        if (netman) {
                status->hw_addr[0] = 0x50; // TODO
                status->hw_addr[1] = 0x51;
                status->hw_addr[2] = 0x52;
                status->hw_addr[3] = 0x53;
                status->hw_addr[4] = 0x54;
                status->hw_addr[5] = 0x55;

                status->address    = NET_INET_IPv4_ANY;
                status->mask       = NET_INET_IPv4_ANY;
                status->gateway    = NET_INET_IPv4_ANY;

                status->rx_packets = netman->rx_packets;
                status->rx_bytes   = netman->rx_bytes;
                status->tx_packets = netman->tx_packets;
                status->tx_bytes   = netman->tx_bytes;

                status->state      = NET_INET_STATE__NOT_CONFIGURED;

                if (netman->configured) {
                        if (netman->disconnected) {
                                status->state = NET_INET_STATE__LINK_DISCONNECTED;

                        } else if (netif_is_up(&netman->netif)) {
                                if (DHCP_is_started()) {
                                        if (netman->netif.dhcp->state != DHCP_BOUND) {
                                                status->state = NET_INET_STATE__DHCP_CONFIGURING;
                                        } else {
                                                status->state = NET_INET_STATE__DHCP_CONFIGURED;
                                        }
                                } else {
                                        status->state = NET_INET_STATE__STATIC_IP;
                                }

                                create_addr(&status->address, &netman->netif.ip_addr);
                                create_addr(&status->mask, &netman->netif.netmask);
                                create_addr(&status->gateway, &netman->netif.gw);
                        }
                }

                err = ESUCC;
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
int INET_socket_create(NET_protocol_t prot, INET_socket_t *inet_sock)
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
int INET_socket_destroy(INET_socket_t *inet_sock)
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
int INET_socket_connect(INET_socket_t *inet_sock, const NET_INET_sockaddr_t *addr)
{
        ip_addr_t IP;
        create_lwIP_addr(&IP, &addr->addr);

        return lwIP_status_to_errno(netconn_connect(inet_sock->netconn,
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
int INET_socket_disconnect(INET_socket_t *inet_sock)
{
        return lwIP_status_to_errno(netconn_disconnect(inet_sock->netconn));
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
int INET_socket_shutdown(INET_socket_t *inet_sock, NET_shut_t how)
{
        return lwIP_status_to_errno(netconn_shutdown(inet_sock->netconn,
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
int INET_socket_bind(INET_socket_t *inet_sock, const NET_INET_sockaddr_t *addr)
{
        ip_addr_t IP;
        create_lwIP_addr(&IP, &addr->addr);

        return lwIP_status_to_errno(netconn_bind(inet_sock->netconn,
                                                      &IP, addr->port));
}

//==============================================================================
/**
 *
 * @param socket
 * @return
 */
//==============================================================================
int INET_socket_listen(INET_socket_t *inet_sock)
{
        return lwIP_status_to_errno(netconn_listen(inet_sock->netconn));
}

//==============================================================================
/**
 *
 * @param socket
 * @param new_socket
 * @return
 */
//==============================================================================
int INET_socket_accept(INET_socket_t *inet_sock, INET_socket_t *new_inet_sock)
{
        return lwIP_status_to_errno(netconn_accept(inet_sock->netconn,
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
int INET_socket_recv(INET_socket_t *inet_sock,
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
                err = lwIP_status_to_errno(netconn_recv(inet_sock->netconn,
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
                        inet_sock->seek   = 0;
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
int INET_socket_recvfrom(INET_socket_t       *inet_sock,
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
                err = lwIP_status_to_errno(netconn_recv(inet_sock->netconn,
                                                             &netbuf));
                if (!err) {
                        *recved = netbuf_copy(netbuf, buf, len);

                        ip_addr_t *fromaddr = netbuf_fromaddr(netbuf);
                        sockaddr->port      = netbuf_fromport(netbuf);
                        create_addr(&sockaddr->addr, fromaddr);

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
int INET_socket_send(INET_socket_t *inet_sock,
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

                err = lwIP_status_to_errno(netconn_write(inet_sock->netconn,
                                                              buf,
                                                              len,
                                                              lwip_flags));
                *sent = len;

        } else if (type & NETCONN_UDP) {

                if (len <= MAXIMUM_SAFE_UDP_PAYLOAD) {

                        inet_sock->netbuf = netbuf_new();
                        if (inet_sock->netbuf) {
                                if (flags & NET_FLAGS__NOCOPY) {
                                        err = lwIP_status_to_errno(
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
                                        err = lwIP_status_to_errno(
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
int INET_socket_sendto(INET_socket_t             *inet_sock,
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
                err = lwIP_status_to_errno(netconn_peer(inet_sock->netconn,
                                                             &lwip_addr,
                                                             &sockaddr.port));

                if (!err) {
                        create_addr(&sockaddr.addr, &lwip_addr);

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
        err = lwIP_status_to_errno(netconn_gethostbyname(name, &lwip_addr));
        if (!err) {
                create_addr(&sock_addr->addr, &lwip_addr);
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
int INET_socket_set_recv_timeout(INET_socket_t *inet_sock, uint32_t timeout)
{
        netconn_set_recvtimeout(inet_sock->netconn,timeout);    // function returns nothing
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
int INET_socket_set_send_timeout(INET_socket_t *inet_sock, uint32_t timeout)
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
int INET_socket_getaddress(INET_socket_t *inet_sock, NET_INET_sockaddr_t *sockaddr)
{
        ip_addr_t lwip_addr;
        int err = lwIP_status_to_errno(
                netconn_getaddr(inet_sock->netconn, &lwip_addr, &sockaddr->port, 0)
        );

        if (!err) {
                create_addr(&sockaddr->addr, &lwip_addr);
        }

        return err;
}

//==============================================================================
/**
 *
 * @param value
 * @return
 */
//==============================================================================
u16_t INET_hton_u16(u16_t value)
{
#if _CPUCTL_BYTE_ORDER == _BYTE_ORDER_LITTLE_ENDIAN
        return ((value & 0x00FF) << 8)
             | ((value & 0xFF00) >> 8);
#elif _CPUCTL_BYTE_ORDER == _BYTE_ORDER_BIG_ENDIAN
        return value;
#else
#error "Not supported endianness!"
#endif
}

//==============================================================================
/**
 *
 * @param value
 * @return
 */
//==============================================================================
u32_t INET_hton_u32(u32_t value)
{
#if _CPUCTL_BYTE_ORDER == _BYTE_ORDER_LITTLE_ENDIAN
        return ((value & 0x000000FFUL) << 24)
             | ((value & 0x0000FF00UL) << 8)
             | ((value & 0x00FF0000UL) >> 8)
             | ((value & 0xFF000000UL) >> 24);
#elif _CPUCTL_BYTE_ORDER == _BYTE_ORDER_BIG_ENDIAN
        return value;
#else
#error "Not supported endianness!"
#endif
}

//==============================================================================
/**
 *
 * @param value
 * @return
 */
//==============================================================================
u64_t INET_hton_u64(u64_t value)
{
#if _CPUCTL_BYTE_ORDER == _BYTE_ORDER_LITTLE_ENDIAN
        return ((value & 0x00000000000000FFULL) << 56)
             | ((value & 0x000000000000FF00ULL) << 40)
             | ((value & 0x0000000000FF0000ULL) << 24)
             | ((value & 0x00000000FF000000ULL) << 8)
             | ((value & 0x000000FF00000000ULL) >> 8)
             | ((value & 0x0000FF0000000000ULL) >> 24)
             | ((value & 0x00FF000000000000ULL) >> 40)
             | ((value & 0xFF00000000000000ULL) >> 56);
#elif _CPUCTL_BYTE_ORDER == _BYTE_ORDER_BIG_ENDIAN
        return value;
#else
#error "Not supported endianness!"
#endif
}

/*==============================================================================
  End of file
==============================================================================*/
