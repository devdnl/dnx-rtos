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
#include "net/netman.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dnx/timer.h>
#include <dnx/misc.h>
#include <dnx/thread.h>
#include <unistd.h>
#include "kernel/kwrapper.h"
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
        timer_t timer = timer_reset();
        while (!netman->ready && timer_is_not_expired(timer, init_timeout)) {
                sleep_ms(1);
        }

        return timer_is_not_expired(timer, init_timeout);
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

        if (mutex_lock(netman->access, MAX_DELAY_MS)) {

                while (!_netman_is_link_connected(netman)) {
                        sleep_ms(link_poll_time);
                }

                netman->disconnected = false;

                if (netman->configured) {
                        if (was_DHCP) {
                                _netman_start_DHCP_client();
                        } else {
                                _netman_if_up(&ip_addr, &netmask, &gw);
                        }
                }

                mutex_unlock(netman->access);
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
        UNUSED_ARG(arg);

        /* open interface file */
        while (netman->if_file == NULL) {
                netman->if_file = fopen(__NETWORK_ETHIF_FILE__, "r+");
                sleep_ms(100);
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

        _task_exit();
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
                netman->access    = _mutex_new(MUTEX_TYPE_RECURSIVE);
                netman->if_thread = _task_new(network_interface_thread, "netifd", STACK_DEPTH_LOW, NULL, NULL);

                if (netman->access && netman->if_thread) {

                        tcpip_init(NULL, NULL);

                        netif_set_default(netif_add(&netman->netif,
                                                    const_cast(ip_addr_t*, &ip_addr_any),
                                                    const_cast(ip_addr_t*, &ip_addr_any),
                                                    const_cast(ip_addr_t*, &ip_addr_any),
                                                    netman,
                                                    _netman_netif_init,
                                                    tcpip_input));
                } else {
                        if (netman->access)
                                _mutex_delete(netman->access);

                        if (netman->if_thread)
                                _task_delete(netman->if_thread);

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
        int status = -1;

        if (  netman
           && !netif_is_up(&netman->netif)
           && wait_for_init_done()
           && _mutex_lock(netman->access, access_timeout) ) {

                clear_rx_tx_counters();
                netif_set_down(&netman->netif);
                netif_set_addr(&netman->netif,
                               const_cast(ip_addr_t*, &ip_addr_any),
                               const_cast(ip_addr_t*, &ip_addr_any),
                               const_cast(ip_addr_t*, &ip_addr_any) );

                if (dhcp_start(&netman->netif) == ERR_OK) {
                        netif_set_up(&netman->netif);

                        timer_t timer = timer_reset();
                        while (timer_is_not_expired(timer, DHCP_timeout)) {

                                if (netman->netif.dhcp->state == DHCP_BOUND) {
                                        netman->configured = true;
                                        status = 0;
                                        break;
                                } else {
                                        _sleep_ms(100);
                                }
                        }
                }

                _mutex_unlock(netman->access);
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
int _netman_stop_DHCP_client()
{
        int status = -1;

        if (  netman
           && netif_is_up(&netman->netif)
           && is_DHCP_started()
           && _mutex_lock(netman->access, access_timeout) ) {

                if (dhcp_release(&netman->netif) == ERR_OK) {
                        dhcp_stop(&netman->netif);
                        netif_set_down(&netman->netif);
                        netman->configured = false;
                        status = 0;
                }

                _mutex_unlock(netman->access);
        }

        return status;
}

//==============================================================================
/**
 * @brief  Function renew DHCP connection
 * @param  None
 * @return On success 0 is returned, otherwise -1
 */
//==============================================================================
int _netman_renew_DHCP_connection()
{
        int status = -1;

        if (  netman
           && netif_is_up(&netman->netif)
           && is_DHCP_started()
           && _mutex_lock(netman->access, access_timeout) ) {

                if (dhcp_renew(&netman->netif) == ERR_OK) {

                        timer_t timeout = timer_reset();
                        while (timer_is_not_expired(timeout, DHCP_timeout)) {

                                if (netman->netif.dhcp->state == DHCP_BOUND) {
                                        status = 0;
                                        break;
                                } else {
                                        _sleep_ms(500);
                                }
                        }
                }

                _mutex_unlock(netman->access);
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
        int status = -1;

        if (  netman
           && netif_is_up(&netman->netif)
           && !is_DHCP_started()
           && _mutex_lock(netman->access, access_timeout) ) {

                dhcp_inform(&netman->netif);
                status = 0;

                _mutex_unlock(netman->access);
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
        int status = -1;

        if (  netman && ip_address && net_mask && gateway
           && !netif_is_up(&netman->netif)
           && wait_for_init_done()
           && _mutex_lock(netman->access, access_timeout) ) {

                clear_rx_tx_counters();
                netif_set_down(&netman->netif);
                netif_set_addr(&netman->netif,
                               const_cast(ip_addr_t*, ip_address),
                               const_cast(ip_addr_t*, net_mask),
                               const_cast(ip_addr_t*, gateway) );
                netif_set_up(&netman->netif);

                netman->configured = true;

                status = 0;

                _mutex_unlock(netman->access);
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
        int status = -1;

        if (  netman
           && netif_is_up(&netman->netif)
           && !is_DHCP_started()
           && _mutex_lock(netman->access, access_timeout) ) {

                netif_set_down(&netman->netif);

                netman->configured = false;

                status = 0;

                _mutex_unlock(netman->access);
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
        int status = -1;

        if (netman && ifcfg) {
                ifcfg->hw_address[0] = __NETWORK_MAC_ADDR_0__;
                ifcfg->hw_address[1] = __NETWORK_MAC_ADDR_1__;
                ifcfg->hw_address[2] = __NETWORK_MAC_ADDR_2__;
                ifcfg->hw_address[3] = __NETWORK_MAC_ADDR_3__;
                ifcfg->hw_address[4] = __NETWORK_MAC_ADDR_4__;
                ifcfg->hw_address[5] = __NETWORK_MAC_ADDR_5__;
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

                status = 0;
        }

        return status;
}

/*==============================================================================
  End of file
==============================================================================*/
