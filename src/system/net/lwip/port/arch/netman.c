/*=========================================================================*//**
@file    netman.h

@author  Daniel Zorychta

@brief   Network manager. Core functionality.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "errno.h"
#include "net/netman.h"
#include "kernel/sysfunc.h"
#include "lwip/tcpip.h"

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/
typedef _netman_t netman_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void network_interface_thread(void *sem);
static void clear_rx_tx_counters();
static bool wait_for_init_done();
static bool is_DHCP_started();
static void restore_configuration();

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
 * @brief  Function check if DHCP client is started
 * @param  None
 * @return If DHCP Client is started then true is returned, otherwise false.
 */
//==============================================================================
static bool is_DHCP_started()
{
        return (netman->netif.flags & NETIF_FLAG_DHCP);
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

        if (is_DHCP_started()) {
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
                                _netman_start_DHCP_client();
                        } else {
                                _netman_if_up(&ip_addr, &netmask, &gw);
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
 * @brief  Function starts network manager, TCP/IP stack, and set network interface.
 *         Function is called before kernel start.
 * @param  None
 * @return None
 */
//==============================================================================
void _netman_init()
{
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
                } else {
                        if (ms)
                                sys_mutex_destroy(netman->access);

                        if (ts)
                                sys_thread_destroy(&netman->if_thread);

                        _netfree(netman);
                        netman = NULL;
                }
        }
}

//==============================================================================
/**
 * @brief  Function starts DHCP client
 * @param  None
 * @return On success 0 is returned, otherwise -1
 */
//==============================================================================
int _netman_start_DHCP_client()
{
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
 * @brief  Function stops DHCP client
 * @param  None
 * @return On success 0 is returned, otherwise -1
 */
//==============================================================================
//int _netman_stop_DHCP_client()
//{
//        int status = -1;
//
//        if (  netman
//           && netif_is_up(&netman->netif)
//           && is_DHCP_started()
//           && sys_mutex_lock(netman->access, access_timeout) == ESUCC ) {
//
//                if (dhcp_release(&netman->netif) == ERR_OK) {
//                        dhcp_stop(&netman->netif);
//                        netif_set_down(&netman->netif);
//                        netman->configured = false;
//                        status = 0;
//                }
//
//                sys_mutex_unlock(netman->access);
//        }
//
//        return status;
//}

//==============================================================================
/**
 * @brief  Function renew DHCP connection
 * @param  None
 * @return On success 0 is returned, otherwise -1
 */
//==============================================================================
int _netman_renew_DHCP_connection()
{
        int status = ENONET;

        if (  netman
           && netif_is_up(&netman->netif)
           && is_DHCP_started()
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
 * @brief  Function inform DHCP about current static configuration
 * @param  None
 * @return On success 0 is returned, otherwise -1
 */
//==============================================================================
int _netman_inform_DHCP_server()
{
        int status = ENONET;

        if (  netman
           && netif_is_up(&netman->netif)
           && !is_DHCP_started()
           && sys_mutex_lock(netman->access, access_timeout) == ESUCC ) {

                dhcp_inform(&netman->netif);
                status = ESUCC;

                sys_mutex_unlock(netman->access);
        }

        return status;
}

//==============================================================================
/**
 * @brief  Function configure interface as static
 * @param  ip_address        a IP address
 * @param  net_mask          a net mask value
 * @param  gateway           a gateway address
 * @return On success 0 is returned, otherwise -1
 */
//==============================================================================
int _netman_if_up(const ip_addr_t *ip_address, const ip_addr_t *net_mask, const ip_addr_t *gateway)
{
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
 * @brief  Function turn down interface with static configuration
 * @param  None
 * @return On success 0 is returned, otherwise -1
 */
//==============================================================================
int _netman_if_down()
{
        int status = ENONET;

        if (  netman
           && netif_is_up(&netman->netif)
           && sys_mutex_lock(netman->access, access_timeout) == ESUCC ) {

                if (is_DHCP_started()) {
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
 * @brief  Function gets interface configuration
 * @param  ifcfg            a pointer to status object
 * @return On success 0 is returned, otherwise -1
 */
//==============================================================================
int _netman_get_ifconfig(_ifconfig_t *ifcfg)
{
        int status = EINVAL;

        if (netman && ifcfg) {


                ifcfg->hw_address[0] = 0x50;
                ifcfg->hw_address[1] = 0x51;
                ifcfg->hw_address[2] = 0x52;
                ifcfg->hw_address[3] = 0x53;
                ifcfg->hw_address[4] = 0x54;
                ifcfg->hw_address[5] = 0x55;
                ifcfg->IP_address    = ip_addr_any;
                ifcfg->net_mask      = ip_addr_any;
                ifcfg->gateway       = ip_addr_any;
                ifcfg->rx_packets    = netman->rx_packets;
                ifcfg->rx_bytes      = netman->rx_bytes;
                ifcfg->tx_packets    = netman->tx_packets;
                ifcfg->tx_bytes      = netman->tx_bytes;
                ifcfg->status        = NET_STATUS_NOT_CONFIGURED;

                if (netman->configured) {
                        if (netman->disconnected) {
                                ifcfg->status = NET_STATUS_LINK_DISCONNECTED;

                        } else if (netif_is_up(&netman->netif)) {
                                if (is_DHCP_started()) {
                                        if (netman->netif.dhcp->state != DHCP_BOUND) {
                                                ifcfg->status = NET_STATUS_DHCP_CONFIGURING;
                                        } else {
                                                ifcfg->status = NET_STATUS_DHCP_CONFIGURED;
                                        }
                                } else {
                                        ifcfg->status = NET_STATUS_STATIC_IP;
                                }

                                ifcfg->IP_address = netman->netif.ip_addr;
                                ifcfg->net_mask   = netman->netif.netmask;
                                ifcfg->gateway    = netman->netif.gw;
                        }
                }

                status = ESUCC;
        }

        return status;
}

/*==============================================================================
  End of file
==============================================================================*/
