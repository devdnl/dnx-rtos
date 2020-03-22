/*=========================================================================*//**
File     inet.c

Author   Daniel Zorychta

Brief    Network management.

         Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "net/inet/inet.h"
#include "inet_types.h"
#include "cpuctl.h"
#include "lwip/api.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"
#include "lwip/prot/dhcp.h"
#include "netif/etharp.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define MAXIMUM_SAFE_UDP_PAYLOAD        1024

#define zalloc(_size, _pptr)            _kzalloc(_MM_NET, _size, _pptr)
#define zfree(_pptr)                    _kfree(_MM_NET, _pptr)

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void  network_interface_thread(void *sem);
static void  clear_rx_tx_counters();
static bool  is_init_done();
static dhcp_state_enum_t DHCP_get_state(void);
static void  restore_configuration();
static int   DHCP_start_client();
static err_t netif_configure(struct netif *netif);
static int   apply_static_IP_configuration(const ip_addr_t *ip_address, const ip_addr_t *net_mask, const ip_addr_t *gateway);

/*==============================================================================
  External function prototypes
==============================================================================*/
extern int   _inetdrv_hardware_init    (inet_t *inet);
extern int   _inetdrv_hardware_deinit  (inet_t *inet);
extern err_t _inetdrv_handle_output    (struct netif *netif, struct pbuf *p);
extern void  _inetdrv_handle_input     (inet_t *inet, u32_t timeout);
extern bool  _inetdrv_is_link_connected(inet_t *inet);

/*==============================================================================
  Local objects
==============================================================================*/
static inet_t     *inet;
static const u32_t ACCESS_TIMEOUT = 10000;
static const u32_t DHCP_TIMEOUT   = 5000;
static const u32_t INIT_TIMEOUT   = 5000;
static const u32_t INPUT_TIMEOUT  = 5000;
static const u32_t LINK_POLL_TIME = 250;

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
static void clear_rx_tx_counters(void)
{
        inet->rx_bytes   = 0;
        inet->tx_bytes   = 0;
        inet->rx_packets = 0;
        inet->tx_packets = 0;
}

//==============================================================================
/**
 * @brief  Function wait for interface to be available
 * @return Returns true if initialization done.
 */
//==============================================================================
static bool is_init_done(void)
{
        u32_t timer = sys_get_uptime_ms();
        while (not inet->ready && not sys_time_is_expired(timer, INIT_TIMEOUT)) {
                sys_sleep_ms(1);
        }

        return not sys_time_is_expired(timer, INIT_TIMEOUT);
}

//==============================================================================
/**
 * @brief  Function restores last configuration after link connection
 */
//==============================================================================
static void restore_configuration(void)
{
        bool was_DHCP;
        ip_addr_t ip_addr, gw, netmask;

        inet->disconnected = true;

        if ((DHCP_get_state() != DHCP_STATE_OFF)) {
                dhcp_release(&inet->netif);
                dhcp_stop(&inet->netif);
                was_DHCP = true;
        } else {
                ip_addr = inet->netif.ip_addr;
                gw      = inet->netif.gw;
                netmask = inet->netif.netmask;
                was_DHCP = false;
        }

        netif_set_down(&inet->netif);

        if (sys_mutex_lock(inet->access, MAX_DELAY_MS) == ESUCC) {

                while (!_inetdrv_is_link_connected(inet)) {
                        sys_msleep(LINK_POLL_TIME);
                }

                inet->disconnected = false;

                if (inet->configured) {
                        if (was_DHCP) {
                                DHCP_start_client();
                        } else {
                                apply_static_IP_configuration(&ip_addr, &netmask, &gw);
                        }
                }

                sys_mutex_unlock(inet->access);
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
        bool msg = false;

        while (inet->if_file == NULL) {
                int err = sys_fopen(__NETWORK_TCPIP_DEVICE_PATH__, "r+", &inet->if_file);

                if (err && !msg) {
                        printk("INET: interface file error (%s)", strerror(err));
                        msg = true;
                }

                sys_msleep(100);
        }

        /* initialize interface */
        if (_inetdrv_hardware_init(inet) == ESUCC) {

                inet->ready = true;

                while (inet->disconnected) {
                        sys_msleep(100);
                        inet->disconnected = not _inetdrv_is_link_connected(inet);
                }

                while (true) {
                        _inetdrv_handle_input(inet, INPUT_TIMEOUT);

                        if (!_inetdrv_is_link_connected(inet)) {
                                restore_configuration();
                        }
                }

                _inetdrv_hardware_deinit(inet);
        }

        // error occurred
        return;
}

//==============================================================================
/**
 * @brief Function createNET_INET_IPv4_t address from lwIP address.
 * @param addr          INET address
 * @param lwip_addr     lwIP address
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
 * @brief Function create lwIP address by using INET address.
 * @param lwip_addr     lwIP address
 * @param addr          INET address
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
static int stack_init(void)
{
        if (inet)
                return ESUCC;

        int err = zalloc(sizeof(inet_t), (void*)&inet);
        if (err) {
                return err;
        }

        if (inet) {
                static const thread_attr_t attr = {
                        .priority    = PRIORITY_NORMAL,
                        .stack_depth = STACK_DEPTH_LOW,
                        .detached    = true
                };

                int terr = sys_thread_create(network_interface_thread, &attr, NULL, &inet->if_thread);
                int merr = sys_mutex_create(MUTEX_TYPE_RECURSIVE, &inet->access);

                if (merr == ESUCC && terr == ESUCC) {

                        tcpip_init(NULL, NULL);

                        netif_add(&inet->netif,
                                  const_cast(ip_addr_t*, &ip_addr_any),
                                  const_cast(ip_addr_t*, &ip_addr_any),
                                  const_cast(ip_addr_t*, &ip_addr_any),
                                  inet,
                                  netif_configure,
                                  tcpip_input);

                        netif_set_default(&inet->netif);

                        return ESUCC;

                } else {
                        if (!merr)
                                sys_mutex_destroy(inet->access);

                        if (!terr)
                                sys_thread_destroy(inet->if_thread);

                        zfree((void*)&inet);
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
static int apply_static_IP_configuration(const ip_addr_t *ip_address, const ip_addr_t *net_mask, const ip_addr_t *gateway)
{
        int status = EINVAL;

        if (  inet && ip_address && net_mask && gateway
           && !netif_is_up(&inet->netif)
           && is_init_done()
           && sys_mutex_lock(inet->access, ACCESS_TIMEOUT) == ESUCC ) {

                clear_rx_tx_counters();
                netif_set_down(&inet->netif);
                netif_set_addr(&inet->netif,
                               const_cast(ip_addr_t*, ip_address),
                               const_cast(ip_addr_t*, net_mask),
                               const_cast(ip_addr_t*, gateway) );
                netif_set_up(&inet->netif);
                netif_set_link_up(&inet->netif);

                inet->configured = true;

                status = ESUCC;

                sys_mutex_unlock(inet->access);
        }

        return status;
}

//==============================================================================
/**
 * @brief  Function return DHCP state.
 * @return DHCP state.
 */
//==============================================================================
static dhcp_state_enum_t DHCP_get_state(void)
{
        struct dhcp *dhcp = inet->netif.client_data[LWIP_NETIF_CLIENT_DATA_INDEX_DHCP];
        if (dhcp) {
                return dhcp->state;

        } else {
                return DHCP_STATE_OFF;
        }
}

//==============================================================================
/**
 * @brief  Function starts DHCP client
 * @return One of @ref errno value.
 */
//==============================================================================
static int DHCP_start_client(void)
{
        int err = ENONET;

        if (inet) {
                if (netif_is_up(&inet->netif)) {
                        err = EADDRINUSE;
                        goto finish;
                }

                if (not is_init_done()) {
                        err = EAGAIN;
                        goto finish;
                }

                if (sys_mutex_lock(inet->access, ACCESS_TIMEOUT) == ESUCC) {

                        clear_rx_tx_counters();
                        netif_set_down(&inet->netif);
                        netif_set_addr(&inet->netif,
                                       const_cast(ip_addr_t*, &ip_addr_any),
                                       const_cast(ip_addr_t*, &ip_addr_any),
                                       const_cast(ip_addr_t*, &ip_addr_any) );
                        netif_set_up(&inet->netif);
                        netif_set_link_up(&inet->netif);

                        if (dhcp_start(&inet->netif) == ERR_OK) {
//                                netif_set_up(&inet->netif);

                                u32_t timer = sys_get_uptime_ms();

                                while (not sys_time_is_expired(timer, DHCP_TIMEOUT)) {

                                        if (dhcp_supplied_address(&inet->netif)) {
                                                inet->configured = true;
                                                err = ESUCC;
                                                break;
                                        } else {
                                                sys_msleep(100);
                                        }
                                }
                        } else {
                                netif_set_down(&inet->netif);
                        }

                        sys_mutex_unlock(inet->access);
                }
        }

        finish:
        return err;
}

//==============================================================================
/**
 * @brief  Function inform DHCP about current static configuration
 * @return One of @ref errno value.
 */
//==============================================================================
static int DHCP_inform_server(void)
{
        int status = ENONET;

        if (  inet
           && netif_is_up(&inet->netif)
           && (DHCP_get_state() == DHCP_STATE_OFF)
           && sys_mutex_lock(inet->access, ACCESS_TIMEOUT) == ESUCC ) {

                dhcp_inform(&inet->netif);
                status = ESUCC;

                sys_mutex_unlock(inet->access);
        }

        return status;
}

//==============================================================================
/**
 * @brief  Function renew DHCP connection
 * @return One of @ref errno value.
 */
//==============================================================================
static int DHCP_renew_connection(void)
{
        int status = ENONET;

        if (  inet
           && netif_is_up(&inet->netif)
           && (DHCP_get_state() != DHCP_STATE_OFF)
           && sys_mutex_lock(inet->access, ACCESS_TIMEOUT) == ESUCC ) {

                if (dhcp_renew(&inet->netif) == ERR_OK) {

                        u32_t timeout = sys_get_uptime_ms();
                        while (not sys_time_is_expired(timeout, DHCP_TIMEOUT)) {

                                if (DHCP_get_state() == DHCP_STATE_BOUND) {
                                        status = ESUCC;
                                        break;
                                } else {
                                        sys_msleep(500);
                                }
                        }
                }

                sys_mutex_unlock(inet->access);
        }

        return status;
}

//==============================================================================
/**
 * @brief  Function configures network interface (TCPIP stack configuration).
 *         The function must not allocate any memory and block program flow.
 *
 * @param  netif        the lwip network interface structure for this interface
 *
 * @return ERR_OK       if the loopif is initialized
 *         ERR_MEM      if private data couldn't be allocated any other err_t on error
 *
 * @note   Called at system startup.
 */
//==============================================================================
static err_t netif_configure(struct netif *netif)
{
        netif->hostname   = const_cast(char*, __OS_HOSTNAME__);
        netif->name[0]    = 'E';
        netif->name[1]    = 'T';
        netif->output     = etharp_output;
        netif->linkoutput = _inetdrv_handle_output;
        netif->mtu        = 1500;
        netif->hwaddr_len = ETHARP_HWADDR_LEN;
        netif->flags      = NETIF_FLAG_ETHERNET
                          | NETIF_FLAG_BROADCAST
                          | NETIF_FLAG_ETHARP
                          | NETIF_FLAG_IGMP;

        return ERR_OK;
}

//==============================================================================
/**
 * @brief  Function up the network.
 * @param  cfg   configuration structure
 * @return One of @ref errno value.
 */
//==============================================================================
int INET_ifup(const NET_INET_config_t *cfg)
{
        int err = stack_init();
        if (err) {
                return err;
        }

        inet->disconnected = not _inetdrv_is_link_connected(inet);

        switch (cfg->mode) {
        case NET_INET_MODE__STATIC: {
                ip_addr_t addr, mask, gateway;
                create_lwIP_addr(&addr, &cfg->address);
                create_lwIP_addr(&mask, &cfg->mask);
                create_lwIP_addr(&gateway, &cfg->gateway);

                err = apply_static_IP_configuration(&addr, &mask, &gateway);

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

        if (  inet
           && netif_is_up(&inet->netif)
           && sys_mutex_lock(inet->access, ACCESS_TIMEOUT) == ESUCC ) {

                if ((DHCP_get_state() != DHCP_STATE_OFF)) {
                        if (dhcp_release(&inet->netif) == ERR_OK) {
                                dhcp_stop(&inet->netif);
                        }
                }

                netif_set_down(&inet->netif);
                inet->configured = false;
                status = ESUCC;

                sys_mutex_unlock(inet->access);
        }

        return status;
}

//==============================================================================
/**
 * @brief  Function returns interface status.
 * @param  status       status container
 * @return One of @ref errno value.
 */
//==============================================================================
int INET_ifstatus(NET_INET_status_t *status)
{
        int err = ENONET;

        if (inet) {
                memcpy(status->hw_addr, inet->netif.hwaddr, sizeof(status->hw_addr));

                status->address    = NET_INET_IPv4_ANY;
                status->mask       = NET_INET_IPv4_ANY;
                status->gateway    = NET_INET_IPv4_ANY;

                status->rx_packets = inet->rx_packets;
                status->rx_bytes   = inet->rx_bytes;
                status->tx_packets = inet->tx_packets;
                status->tx_bytes   = inet->tx_bytes;

                status->state      = NET_INET_STATE__NOT_CONFIGURED;

                if (inet->configured) {
                        if (inet->disconnected) {
                                status->state = NET_INET_STATE__LINK_DISCONNECTED;

                        } else if (netif_is_up(&inet->netif)) {
                                if ((DHCP_get_state() != DHCP_STATE_OFF)) {
                                        if (DHCP_get_state() != DHCP_STATE_BOUND) {
                                                status->state = NET_INET_STATE__DHCP_CONFIGURING;
                                        } else {
                                                status->state = NET_INET_STATE__DHCP_CONFIGURED;
                                        }
                                } else {
                                        status->state = NET_INET_STATE__STATIC_IP;
                                }

                                create_addr(&status->address, &inet->netif.ip_addr);
                                create_addr(&status->mask, &inet->netif.netmask);
                                create_addr(&status->gateway, &inet->netif.gw);
                        }
                } else {
                        if (inet->disconnected) {
                                status->state = NET_INET_STATE__LINK_DISCONNECTED;
                        }
                }

                err = ESUCC;
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function create socket container.
 * @param  prot         protocol
 * @param  inet_sock    inet socket
 * @return One of @ref errno value.
 */
//==============================================================================
int INET_socket_create(NET_protocol_t prot, INET_socket_t *inet_sock)
{
        int err = EINVAL;

        if (prot == NET_PROTOCOL__TCP || prot == NET_PROTOCOL__UDP) {

                _errno = 0;

                inet_sock->netconn = netconn_new(prot == NET_PROTOCOL__TCP
                                                       ? NETCONN_TCP
                                                       : NETCONN_UDP);

                if (inet_sock->netconn) {
                        err = ESUCC;
                } else {
                        if (_errno == ENOMEM) {
                                err = _errno;
                        } else {
                                err = ENONET;
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function destroy created socket.
 * @param  inet_sock    inet socket to destroy
 * @return One of @ref errno value.
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
 * @brief  Function connect socket to selected address.
 * @param  inet_sock    socket
 * @param  addr         inet address
 * @return One of @ref errno value.
 */
//==============================================================================
int INET_socket_connect(INET_socket_t *inet_sock, const NET_INET_sockaddr_t *addr)
{
        ip_addr_t IP;
        create_lwIP_addr(&IP, &addr->addr);

        return err_to_errno(netconn_connect(inet_sock->netconn, &IP, addr->port));
}

//==============================================================================
/**
 * @brief  Function disconnect socket from selected address.
 * @param  inet_sock     socket
 * @param  addr          inet addres
 * @return One of @ref errno value.
 */
//==============================================================================
int INET_socket_disconnect(INET_socket_t *inet_sock)
{
        return err_to_errno(netconn_disconnect(inet_sock->netconn));
}

//==============================================================================
/**
 * @brief  Function shutdown selected socket direction.
 * @param  inet_sock    socket
 * @param  how          shutdown direction
 * @return One of @ref errno value.
 */
//==============================================================================
int INET_socket_shutdown(INET_socket_t *inet_sock, NET_shut_t how)
{
        return err_to_errno(netconn_shutdown(inet_sock->netconn,
                                             how & NET_SHUT__RD,
                                             how & NET_SHUT__WR));
}

//==============================================================================
/**
 * @brief  Function bind selected address with socket.
 * @param  inet_sock     socket
 * @param  addr          inet address
 * @return One of @ref errno value.
 */
//==============================================================================
int INET_socket_bind(INET_socket_t *inet_sock, const NET_INET_sockaddr_t *addr)
{
        ip_addr_t IP;
        create_lwIP_addr(&IP, &addr->addr);

        return err_to_errno(netconn_bind(inet_sock->netconn, &IP, addr->port));
}

//==============================================================================
/**
 * @brief  Function listen connection.
 * @param  socket        socket
 * @return One of @ref errno value.
 */
//==============================================================================
int INET_socket_listen(INET_socket_t *inet_sock)
{
        return err_to_errno(netconn_listen(inet_sock->netconn));
}

//==============================================================================
/**
 * @brief  Function accept incoming connection.
 * @param  socket       socket
 * @param  new_socket   new socket of accepted connection
 * @return One of @ref errno value.
 */
//==============================================================================
int INET_socket_accept(INET_socket_t *inet_sock, INET_socket_t *new_inet_sock)
{
        return err_to_errno(netconn_accept(inet_sock->netconn,
                                           &new_inet_sock->netconn));
}

//==============================================================================
/**
 * @brief  Function receive incoming data.
 * @param  inet_sock    socket
 * @param  buf          buffer for data
 * @param  len          number of data to receive
 * @param  flags        flags
 * @param  recved       number of received bytes
 * @return One of @ref errno value.
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
                err = err_to_errno(netconn_recv(inet_sock->netconn,
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
 * @brief  Function receive data from selected address.
 * @param  inet_sock    socket
 * @param  buf          buffer for data
 * @param  len          number of bytes to receive
 * @param  flags        flags
 * @param  sockaddr     socket address
 * @param  recved       number of received bytes
 * @return One of @ref errno value.
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
                err = err_to_errno(netconn_recv(inet_sock->netconn, &netbuf));
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
 * @brief  Function send data to connected socket.
 * @param  inet_sock    socket
 * @param  buf          buffer to send
 * @param  len          number of bytes to send
 * @param  flags        flags
 * @param  sent         number of sent bytes
 * @return One of @ref errno value.
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

                if (flags & NET_FLAGS__MORE)
                        lwip_flags |= NETCONN_MORE;


                err = err_to_errno(netconn_write_partly(inet_sock->netconn, buf,
                                                        len, lwip_flags, sent));

        } else if (type & NETCONN_UDP) {

                if (len <= MAXIMUM_SAFE_UDP_PAYLOAD) {

                        inet_sock->netbuf = netbuf_new();
                        if (inet_sock->netbuf) {
                                if (flags & NET_FLAGS__NOCOPY) {
                                        err = err_to_errno(
                                                netbuf_ref(inet_sock->netbuf,
                                                           buf, len)
                                        );
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
                                        err = err_to_errno(
                                                netconn_send(inet_sock->netconn,
                                                             inet_sock->netbuf)
                                        );

                                }

                                if (!err) {
                                        *sent = len;
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
 * @brief  Function send buffer to selected address.
 * @param  inet_sock    socket
 * @param  buf          buffer to send
 * @param  len          number of bytes to send
 * @param  flags        flags
 * @param  to_sockaddr  socket address
 * @param  sent         number of sent bytes
 * @return One of @ref errno value.
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
                err = err_to_errno(netconn_peer(inet_sock->netconn, &lwip_addr,
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
 * @brief  Function gets host address by name.
 * @param  name         address name
 * @param  addr         received address
 * @return One of @ref errno value.
 */
//==============================================================================
int INET_gethostbyname(const char *name, NET_INET_sockaddr_t *sock_addr)
{
        int err = EINVAL;

        ip_addr_t lwip_addr;
        err = err_to_errno(netconn_gethostbyname(name, &lwip_addr));
        if (!err) {
                create_addr(&sock_addr->addr, &lwip_addr);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function set receive timeout.
 * @param  inet_sock    socket
 * @param  timeout      timeout value in milliseconds
 * @return One of @ref errno value.
 */
//==============================================================================
int INET_socket_set_recv_timeout(INET_socket_t *inet_sock, uint32_t timeout)
{
        netconn_set_recvtimeout(inet_sock->netconn, timeout);
        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function set send timeout.
 * @param  inet_sock    socket
 * @param  timeout      timeout value in milliseconds
 * @return One of @ref errno value.
 */
//==============================================================================
int INET_socket_set_send_timeout(INET_socket_t *inet_sock, uint32_t timeout)
{
        netconn_set_sendtimeout(inet_sock->netconn, timeout);
        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function get receive timeout.
 * @param  inet_sock    socket
 * @param  timeout      timeout value in milliseconds
 * @return One of @ref errno value.
 */
//==============================================================================
int INET_socket_get_recv_timeout(INET_socket_t *inet_sock, uint32_t *timeout)
{
        *timeout = netconn_get_recvtimeout(inet_sock->netconn);
        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function get send timeout.
 * @param  inet_sock    socket
 * @param  timeout      timeout value in milliseconds
 * @return One of @ref errno value.
 */
//==============================================================================
int INET_socket_get_send_timeout(INET_socket_t *inet_sock, uint32_t *timeout)
{
        *timeout = netconn_get_sendtimeout(inet_sock->netconn);
        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function returns address of socket (remote connection address).
 * @param  inet_sock    socket
 * @param  sockaddr     socket address (address and port)
 * @return One of @ref errno value.
 */
//==============================================================================
int INET_socket_getaddress(INET_socket_t *inet_sock, NET_INET_sockaddr_t *sockaddr)
{
        ip_addr_t lwip_addr;
        int err = err_to_errno(
                netconn_getaddr(inet_sock->netconn, &lwip_addr, &sockaddr->port, 0)
        );

        if (!err) {
                create_addr(&sockaddr->addr, &lwip_addr);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function convert value for host/network purpose.
 * @param  value        value to convert
 * @return Converted value.
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
 * @brief  Function convert value for host/network purpose.
 * @param  value        value to convert
 * @return Converted value.
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
 * @brief  Function convert value for host/network purpose.
 * @param  value        value to convert
 * @return Converted value.
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
