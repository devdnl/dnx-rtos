/*=========================================================================*//**
@file    net.h

@author  Daniel Zorychta

@brief   This file provide network API.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _DNXNET_H_
#define _DNXNET_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "arch/ethif.h"
#include "lwip/api.h"
#include "lwip/netbuf.h"

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
typedef ip_addr_t                               net_ip_t;
typedef struct netconn                          net_conn_t;
typedef struct netbuf                           net_buf_t;

typedef enum net_conn_type {
        NET_CONN_TYPE_INVALID                   = NETCONN_INVALID,
        NET_CONN_TYPE_TCP                       = NETCONN_TCP,
        NET_CONN_TYPE_UDP                       = NETCONN_UDP,
        NET_CONN_TYPE_UDPLITE                   = NETCONN_UDPLITE,
        NET_CONN_TYPE_UDPNOCHKSUM               = NETCONN_UDPNOCHKSUM,
        NET_CONN_TYPE_RAW                       = NETCONN_RAW
} net_conn_type_t;

typedef enum net_conn_state {
        NET_CONN_STATE_NONE                     = NETCONN_NONE,
        NET_CONN_STATE_WRITE                    = NETCONN_WRITE,
        NET_CONN_STATE_LISTEN                   = NETCONN_LISTEN,
        NET_CONN_STATE_CONNECT                  = NETCONN_CONNECT,
        NET_CONN_STATE_CLOSE                    = NETCONN_CLOSE
} net_conn_state_t;

typedef enum net_err {
        NET_ERR_OK                              = ERR_OK,
        NET_ERR_OUT_OF_MEMORY                   = ERR_MEM,
        NET_ERR_BUFFER                          = ERR_BUF,
        NET_ERR_TIMEOUT                         = ERR_TIMEOUT,
        NET_ERR_ROUTING_PROBLEM                 = ERR_RTE,
        NET_ERR_OPERATION_IN_PROGRESS           = ERR_INPROGRESS,
        NET_ERR_ILLEGAL_VALUE                   = ERR_VAL,
        NET_ERR_OPERATION_WOULD_BLOCK           = ERR_WOULDBLOCK,
        NET_ERR_ADDRESS_IN_USE                  = ERR_USE,
        NET_ERR_IS_CONNECTED                    = ERR_ISCONN,
        NET_ERR_CONNECTION_ABORTED              = ERR_ABRT,
        NET_ERR_CONNECTION_RESET                = ERR_RST,
        NET_ERR_CONNECTION_CLOSED               = ERR_CLSD,
        NET_ERR_NOT_CONNECTED                   = ERR_CONN,
        NET_ERR_ILLEGAL_ARGUMENT                = ERR_ARG,
        NET_ERR_INTERFACE_ERROR                 = ERR_IF
} net_err_t;

typedef enum net_flags {
        NET_CONN_FLAG_NOCOPY                    = NETCONN_NOCOPY,
        NET_CONN_FLAG_COPY                      = NETCONN_COPY,
        NET_CONN_FLAG_MORE                      = NETCONN_MORE,
        NET_CONN_FLAG_DONTBLOCK                 = NETCONN_DONTBLOCK
} net_flags_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief int net_start_DHCP_client(void)
 * The function <b>net_start_DHCP_client</b>() starts DHCP client. DHCP
 * client gets IP address received from DHCP server. If DHCP
 * client was successfully started, then 0 is returned. On error -1.
 *
 * @param None
 *
 * @errors None
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 *
 * @example
 * #include <dnx/net.h>
 *
 * // ...
 *
 * if (net_start_DHCP_client() == 0) {
 *         ifconfig dhcp_cfg;
 *
 *         net_get_ifconfig(&dhcp_cfg);
 *
 *         // configuration print ...
 *
 * } else {
 *         puts(strerror(ENONET));
 * }
 *
 * // ...
 */
//==============================================================================
static inline int net_start_DHCP_client(void)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return _ethif_start_DHCP_client();
#else
        return -1;
#endif
}

//==============================================================================
/**
 * @brief int net_stop_DHCP_client(void)
 * The function <b>net_stop_DHCP_client</b>() stops DHCP client. DHCP
 * client gets IP address received from DHCP server. If DHCP
 * client was successfully stopped, then 0 is returned. On error -1.
 *
 * @param None
 *
 * @errors None
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 *
 * @example
 * #include <dnx/net.h>
 *
 * // ...
 *
 * net_stop_DHCP_client();
 *
 * // ...
 */
//==============================================================================
static inline int net_stop_DHCP_client(void)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return _ethif_stop_DHCP_client();
#else
        return -1;
#endif
}

//==============================================================================
/**
 * @brief int net_renew_DHCP_connection(void)
 * The function <b>net_renew_DHCP_connection</b>() renegotiates connection
 * with DHCP server to get new IP or refresh connection.
 * If connection was successfully renegotiates, then 0 is returned. On error -1.
 *
 * @param None
 *
 * @errors None
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 *
 * @example
 * #include <dnx/net.h>
 *
 * // ...
 *
 * if (net_renew_DHCP_connection() == 0) {
 *         // actions on success ...
 * } else {
 *         // actions on error ...
 * }
 *
 * // ...
 */
//==============================================================================
static inline int net_renew_DHCP_connection(void)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return _ethif_renew_DHCP_connection();
#else
        return -1;
#endif
}

//==============================================================================
/**
 * @brief int net_inform_DHCP_server(void)
 * The function <b>net_inform_DHCP_server</b>() inform DHCP server about
 * static IP configuration. On success, 0 is returned. On error, -1 is returned.
 *
 * @param None
 *
 * @errors None
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 *
 * @example
 * #include <dnx/net.h>
 *
 * // ...
 *
 * net_ip_t ip, netmask, gateway;
 *
 * net_set_ip(&ip, 192,168,0,1);
 * net_set_ip(&netmask, 255,255,255,0);
 * net_set_ip(&gateway, 192,168,0,0);
 *
 * if (net_ifup(&ip, &netmask, &gateway) == 0) {
 *
 *         // if in the nerwork exist DHCP server then inform it about this configuration
 *         if (net_inform_DHCP_server() == 0) {
 *                 // actions on success ...
 *         } else {
 *                 // actions on error ...
 *         }
 * } else {
 *         puts(strerror(ENONET));
 * }
 *
 * // ...
 */
//==============================================================================
static inline int net_inform_DHCP_server(void)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return _ethif_inform_DHCP_server();
#else
        return -1;
#endif
}

//==============================================================================
/**
 * @brief int net_ifup(const net_ip_t *ip, const net_ip_t *netmask, const net_ip_t *gateway)
 * The function <b>net_ifup</b>() establish static IP connection.
 * On success, 0 is returned. On error, -1 is returned.
 *
 * @param ip            a IP address
 * @param netmask       a net mask
 * @param gateway       a gateway IP address
 *
 * @errors None
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 *
 * @example
 * #include <dnx/net.h>
 *
 * // ...
 *
 * net_ip_t ip, netmask, gateway;
 *
 * net_set_ip(&ip, 192,168,0,1);
 * net_set_ip(&netmask, 255,255,255,0);
 * net_set_ip(&gateway, 192,168,0,0);
 *
 * if (net_ifup(&ip, &netmask, &gateway) == 0) {
 *
 *         // if in the nerwork exist DHCP server then inform it about this configuration
 *         if (net_inform_DHCP_server() == 0) {
 *                 // actions on success ...
 *         } else {
 *                 // actions on error ...
 *         }
 * } else {
 *         puts(strerror(ENONET));
 * }
 *
 * // ...
 */
//==============================================================================
static inline int net_ifup(const net_ip_t *ip, const net_ip_t *netmask, const net_ip_t *gateway)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return _ethif_if_up(ip, netmask, gateway);
#else
        (void) ip;
        (void) netmask;
        (void) gateway;
        return -1;
#endif
}

//==============================================================================
/**
 * @brief int net_ifdown(void)
 * The function <b>net_ifdown</b>() close static configured network.
 * On success, 0 is returned. On error or if DHCP is used, -1 is returned.
 *
 * @param None
 *
 * @errors None
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 *
 * @example
 * #include <dnx/net.h>
 *
 * // ...
 *
 * net_ip_t ip, netmask, gateway;
 *
 * net_set_ip(&ip, 192,168,0,1);
 * net_set_ip(&netmask, 255,255,255,0);
 * net_set_ip(&gateway, 192,168,0,0);
 *
 * if (net_ifup(&ip, &netmask, &gateway) == 0) {
 *
 *         // ...
 *
 *         net_ifdown();
 *
 *         // ...
 * } else {
 *         puts(strerror(ENONET));
 * }
 *
 * // ...
 */
//==============================================================================
static inline int net_ifdown(void)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return _ethif_if_down();
#else
        return -1;
#endif
}

//==============================================================================
/**
 * @brief int net_get_ifconfig(ifconfig *ifcfg)
 * The function <b>net_get_ifconfig</b>() return network configuration pointed
 * by <i>ifcfg</i>.
 * On success, 0 is returned. On error, -1 is returned.
 *
 * @param ifcfg         a pointer to information object
 *
 * @errors None
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 *
 * @example
 * #include <dnx/net.h>
 *
 * // ...
 *
 * if (net_start_DHCP_client() == 0) {
 *         ifconfig ifcfg;
 *
 *         net_get_ifconfig(&ifcfg);
 *
 *         printk("IP Address: %d.%d.%d.%d\n",
 *                net_get_ip_part_a(&ifcfg.IP_address),
 *                net_get_ip_part_b(&ifcfg.IP_address),
 *                net_get_ip_part_c(&ifcfg.IP_address),
 *                net_get_ip_part_d(&ifcfg.IP_address));
 *
 * } else {
 *         puts(strerror(ENONET));
 * }
 *
 * // ...
 */
//==============================================================================
static inline int net_get_ifconfig(ifconfig *ifcfg)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return _ethif_get_ifconfig(ifcfg);
#else
        (void) ifcfg;
        return -1;
#endif
}

//==============================================================================
/**
 * @brief void net_set_ip(net_ip_t *ip, const u8_t a, const u8_t b, const u8_t c, const u8_t d)
 * The function <b>net_set_ip</b>() set specified fields of IP object. IP object
 * is pointed by <i>ip</i>. Specific parts of IP address are passed by <i>a</i>,
 * <i>b</i>, <i>c</i>, and <i>d</i> values.
 *
 * @param ip            IP object
 * @param a             IP part a
 * @param b             IP part b
 * @param c             IP part c
 * @param d             IP part d
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/net.h>
 *
 * // ...
 *
 * net_ip_t ip, netmask, gateway;
 *
 * net_set_ip(&ip, 192,168,0,1);
 * net_set_ip(&netmask, 255,255,255,0);
 * net_set_ip(&gateway, 192,168,0,0);
 *
 * if (net_ifup(&ip, &netmask, &gateway) == 0) {
 *
 *         // ...
 * } else {
 *         puts(strerror(ENONET));
 * }
 *
 * // ...
 */
//==============================================================================
static inline void net_set_ip(net_ip_t *ip, const u8_t a, const u8_t b, const u8_t c, const u8_t d)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        IP4_ADDR(ip, a, b, c, d);
#else
        (void) ip;
        (void) a;
        (void) b;
        (void) c;
        (void) d;
#endif
}

//==============================================================================
/**
 * @brief net_ip_t net_load_ip(const u8_t a, const u8_t b, const u8_t c, const u8_t d)
 * The function <b>net_load_ip</b>() set specified fields of IP. Specific parts
 * of IP address are passed by <i>a</i>, <i>b</i>, <i>c</i>, and <i>d</i> values.
 * Function return assembled IP address.
 *
 * @param a             IP part a
 * @param b             IP part b
 * @param c             IP part c
 * @param d             IP part d
 *
 * @errors None
 *
 * @return Return assembled IP address.
 *
 * @example
 * #include <dnx/net.h>
 *
 * // ...
 *
 * net_ip_t ip      = net_load_ip(192,168,0,1);
 * net_ip_t netmask = net_load_ip(255,255,255,0);
 * net_ip_t gateway = net_load_ip(192,168,0,0);
 *
 * if (net_ifup(&ip, &netmask, &gateway) == 0) {
 *
 *         // ...
 * } else {
 *         puts(strerror(ENONET));
 * }
 *
 * // ...
 */
//==============================================================================
static inline net_ip_t net_load_ip(const u8_t a, const u8_t b, const u8_t c, const u8_t d)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        net_ip_t ip;
        IP4_ADDR(&ip, a, b, c, d);
        return ip;
#else
        (void) a;
        (void) b;
        (void) c;
        (void) d;

        net_ip_t ip;
        ip.addr = 0;
        return ip;
#endif
}

//==============================================================================
/**
 * @brief u8_t net_get_ip_part_a(net_ip_t *ip)
 * The function <b>net_get_ip_part_a</b>() return part <i>a</i> of IP address
 * pointed by <i>ip</i>.
 *
 * @param ip            IP address
 *
 * @errors None
 *
 * @return Part <i>a</i> of selected IP address.
 *
 * @example
 * #include <dnx/net.h>
 *
 * // ...
 *
 * if (net_start_DHCP_client() == 0) {
 *         ifconfig ifcfg;
 *
 *         net_get_ifconfig(&ifcfg);
 *
 *         printk("IP Address: %d.%d.%d.%d\n",
 *                net_get_ip_part_a(&ifcfg.IP_address),
 *                net_get_ip_part_b(&ifcfg.IP_address),
 *                net_get_ip_part_c(&ifcfg.IP_address),
 *                net_get_ip_part_d(&ifcfg.IP_address));
 *
 * } else {
 *         puts(strerror(ENONET));
 * }
 *
 * // ...
 */
//==============================================================================
static inline u8_t net_get_ip_part_a(net_ip_t *ip)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return ip4_addr1(ip);
#else
        (void) ip;
        return 0;
#endif
}

//==============================================================================
/**
 * @brief u8_t net_get_ip_part_b(net_ip_t *ip)
 * The function <b>net_get_ip_part_b</b>() return part <i>b</i> of IP address
 * pointed by <i>ip</i>.
 *
 * @param ip            IP address
 *
 * @errors None
 *
 * @return Part <i>b</i> of selected IP address.
 *
 * @example
 * #include <dnx/net.h>
 *
 * // ...
 *
 * if (net_start_DHCP_client() == 0) {
 *         ifconfig ifcfg;
 *
 *         net_get_ifconfig(&ifcfg);
 *
 *         printk("IP Address: %d.%d.%d.%d\n",
 *                net_get_ip_part_a(&ifcfg.IP_address),
 *                net_get_ip_part_b(&ifcfg.IP_address),
 *                net_get_ip_part_c(&ifcfg.IP_address),
 *                net_get_ip_part_d(&ifcfg.IP_address));
 *
 * } else {
 *         puts(strerror(ENONET));
 * }
 *
 * // ...
 */
//==============================================================================
static inline u8_t net_get_ip_part_b(net_ip_t *ip)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return ip4_addr2(ip);
#else
        (void) ip;
        return 0;
#endif
}

//==============================================================================
/**
 * @brief u8_t net_get_ip_part_c(net_ip_t *ip)
 * The function <b>net_get_ip_part_c</b>() return part <i>c</i> of IP address
 * pointed by <i>ip</i>.
 *
 * @param ip            IP address
 *
 * @errors None
 *
 * @return Part <i>c</i> of selected IP address.
 *
 * @example
 * #include <dnx/net.h>
 *
 * // ...
 *
 * if (net_start_DHCP_client() == 0) {
 *         ifconfig ifcfg;
 *
 *         net_get_ifconfig(&ifcfg);
 *
 *         printk("IP Address: %d.%d.%d.%d\n",
 *                net_get_ip_part_a(&ifcfg.IP_address),
 *                net_get_ip_part_b(&ifcfg.IP_address),
 *                net_get_ip_part_c(&ifcfg.IP_address),
 *                net_get_ip_part_d(&ifcfg.IP_address));
 *
 * } else {
 *         puts(strerror(ENONET));
 * }
 *
 * // ...
 */
//==============================================================================
static inline u8_t net_get_ip_part_c(net_ip_t *ip)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return ip4_addr3(ip);
#else
        (void) ip;
        return 0;
#endif
}

//==============================================================================
/**
 * @brief u8_t net_get_ip_part_d(net_ip_t *ip)
 * The function <b>net_get_ip_part_d</b>() return part <i>d</i> of IP address
 * pointed by <i>ip</i>.
 *
 * @param ip            IP address
 *
 * @errors None
 *
 * @return Part <i>d</i> of selected IP address.
 *
 * @example
 * #include <dnx/net.h>
 *
 * // ...
 *
 * if (net_start_DHCP_client() == 0) {
 *         ifconfig ifcfg;
 *
 *         net_get_ifconfig(&ifcfg);
 *
 *         printk("IP Address: %d.%d.%d.%d\n",
 *                net_get_ip_part_a(&ifcfg.IP_address),
 *                net_get_ip_part_b(&ifcfg.IP_address),
 *                net_get_ip_part_c(&ifcfg.IP_address),
 *                net_get_ip_part_d(&ifcfg.IP_address));
 *
 * } else {
 *         puts(strerror(ENONET));
 * }
 *
 * // ...
 */
//==============================================================================
static inline u8_t net_get_ip_part_d(net_ip_t *ip)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return ip4_addr4(ip);
#else
        (void) ip;
        return 0;
#endif
}

//==============================================================================
/**
 * @brief void net_set_ip_to_any(net_ip_t *ip)
 * The function <b>net_set_ip_to_any</b>() set IP address pointed by <i>ip</i>
 * to any address value (0.0.0.0).
 *
 * @param ip            IP address
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/net.h>
 *
 * // ...
 *
 * net_ip_t ip;
 * net_set_ip_to_any(&ip);
 *
 * // ...
 */
//==============================================================================
static inline void net_set_ip_to_any(net_ip_t *ip)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        *ip = ip_addr_any;
#else
        (void) ip;
#endif
}

//==============================================================================
/**
 * @brief void net_set_ip_to_loopback(net_ip_t *ip)
 * The function <b>net_set_ip_to_loopback</b>() set IP address pointed by <i>ip</i>
 * to loopback address value (127.0.0.1).
 *
 * @param ip            IP address
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/net.h>
 *
 * // ...
 *
 * net_ip_t ip;
 * net_set_ip_to_loopback(&ip);
 *
 * // ...
 */
//==============================================================================
static inline void net_set_ip_to_loopback(net_ip_t *ip)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        ip_addr_set_loopback(ip);
#else
        (void) ip;
#endif
}

//==============================================================================
/**
 * @brief void net_set_ip_to_broadcast(net_ip_t *ip)
 * The function <b>net_set_ip_to_broadcast</b>() set IP address pointed by <i>ip</i>
 * to broadcast address value (255.255.255.255).
 *
 * @param ip            IP address
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/net.h>
 *
 * // ...
 *
 * net_ip_t ip;
 * net_set_ip_to_broadcast(&ip);
 *
 * // ...
 */
//==============================================================================
static inline void net_set_ip_to_broadcast(net_ip_t *ip)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        *ip = ip_addr_broadcast;
#else
        (void) ip;
#endif
}

//==============================================================================
/**
 * @brief net_conn_t *net_new_conn(net_conn_type_t type)
 * The function <b>net_new_conn</b>() creates a new connection of specific
 * type pointed by <i>type</i>. Return connection object address or <b>NULL</i>.
 *
 * @param type          connection type
 *
 * @errors ENOMEM
 *
 * @return On success, object address is returned. On error, <b>NULL</b> is
 * returned, and <b>errno</b> is set appropriately.
 *
 * @example
 * #include <dnx/net.h>
 *
 * // ...
 *
 * net_conn_t *conn = net_new_conn(NET_CONN_TYPE_TCP);
 * if (conn) {
 *         net_ip_t ip;
 *         net_set_ip_to_any(&ip);
 *
 *         if (net_bind(conn, &ip, 80) == NET_ERR_OK) {
 *                 if (net_listen(conn) == NET_ERR_OK) {
 *                         puts("Listen connection");
 *
 *                         net_err_t err;
 *                         do {
 *                                 net_conn_t *new_conn;
 *                                 err = net_accept(conn, &new_conn);
 *                                 if (err == NET_ERR_OK) {
 *                                         puts("Accept connection");
 *
 *                                         // connection handle ...
 *
 *                                         net_delete_conn(new_conn);
 *                                 }
 *
 *                         } while (err == NET_ERR_OK);
 *                 }
 *         }
 *
 *         net_delete_conn(conn);
 * }
 *
 * // ...
 */
//==============================================================================
static inline net_conn_t *net_new_conn(net_conn_type_t type)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netconn_new(type);
#else
        (void) type;
        return NULL;
#endif
}

//==============================================================================
/**
 * @brief net_err_t net_delete_conn(net_conn_t *conn)
 * The function <b>net_delete_conn</b>() close connection pointed by <i>conn</i>
 * and free its resources. UDP and RAW connection are completely closed, TCP pcbs
 * might still be in a waitstate after this returns. Function returns operation
 * status described by <b>net_err_t</b>.
 *
 * @param conn          connection
 *
 * @errors None
 *
 * @return One of statuses defined in the <b>net_err_t</b> type.
 *
 * @example
 * #include <dnx/net.h>
 *
 * // ...
 *
 * net_conn_t *conn = net_new_conn(NET_CONN_TYPE_TCP);
 * if (conn) {
 *         net_ip_t ip;
 *         net_set_ip_to_any(&ip);
 *
 *         if (net_bind(conn, &ip, 80) == NET_ERR_OK) {
 *                 if (net_listen(conn) == NET_ERR_OK) {
 *                         puts("Listen connection");
 *
 *                         net_err_t err;
 *                         do {
 *                                 net_conn_t *new_conn;
 *                                 err = net_accept(conn, &new_conn);
 *                                 if (err == NET_ERR_OK) {
 *                                         puts("Accept connection");
 *
 *                                         // connection handle ...
 *
 *                                         net_delete_conn(new_conn);
 *                                 }
 *
 *                         } while (err == NET_ERR_OK);
 *                 }
 *         }
 *
 *         net_delete_conn(conn);
 * }
 *
 * // ...
 */
//==============================================================================
static inline net_err_t net_delete_conn(net_conn_t *conn)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netconn_delete(conn);
#else
        (void) conn;
        return NET_ERR_INTERFACE_ERROR;
#endif
}

//==============================================================================
/**
 * @brief net_conn_type_t net_get_conn_type(net_conn_t *conn)
 * The function <b>net_get_conn_type</b>() returns connection type pointed
 * by <i>conn</i>.
 *
 * @param conn          connection
 *
 * @errors None
 *
 * @return Connection type defined in the <b>net_conn_type_t</b> type.
 *
 * @example
 * #include <dnx/net.h>
 *
 * // ...
 *
 * net_conn_t *conn = net_new_conn(NET_CONN_TYPE_TCP);
 * if (conn) {
 *         // ...
 *
 *         net_conn_type_t type = net_get_conn_type(conn);
 *
 *         // ...
 *
 *         net_delete_conn(conn);
 * }
 *
 * // ...
 */
//==============================================================================
static inline net_conn_type_t net_get_conn_type(net_conn_t *conn)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netconn_type(conn);
#else
        (void) conn;
        return NET_CONN_TYPE_INVALID;
#endif
}

//==============================================================================
/**
 * @brief bool net_is_fatal_error(net_err_t error)
 * The function <b>net_is_fatal_error</b>() check if <i>error</i> is a
 * fatal error.
 *
 * @param error         error number
 *
 * @errors None
 *
 * @return Return <b>true</b> if <i>error</i> is a fatal error, otherwise <b>false</b>.
 *
 * @example
 * #include <dnx/net.h>
 *
 * // ...
 *
 * net_conn_t *conn = net_new_conn(NET_CONN_TYPE_TCP);
 * if (conn) {
 *         net_ip_t ip;
 *         net_set_ip_to_any(&ip);
 *
 *         net_err_t error = net_bind(conn, &ip, 80);
 *         if (error == NET_ERR_OK) {
 *                 // ...
 *         } else {
 *                 if (net_is_fatal_error(error)) {
 *                         // ...
 *                 }
 *         }
 *
 *         net_delete_conn(conn);
 * }
 *
 * // ...
 */
//==============================================================================
static inline bool net_is_fatal_error(net_err_t error)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return ERR_IS_FATAL(error) ? true : false;
#else
        (void) error;
        return true;
#endif
}

//==============================================================================
/**
 * @brief net_err_t net_get_conn_address(net_conn_t *conn, net_ip_t *addr, u16_t *port, bool local)
 * The function <b>net_get_conn_address</b>() return local or remote IP address
 * and port of connection pointed by <i>conn</i>. For RAW connections, this
 * returns the protocol instead of a port!
 *
 * @param conn          the connection to query
 * @param addr          a pointer to which to save the IP address
 * @param port          a pointer to which to save the port (or protocol for RAW)
 * @param local         true to get the local IP address, false to get the remote one
 *
 * @errors None
 *
 * @return One of statuses defined in the <b>net_err_t</b> type.
 *
 * @example
 * #include <dnx/net.h>
 *
 * // ...
 *
 * net_conn_t *conn = net_new_conn(NET_CONN_TYPE_TCP);
 * if (conn) {
 *         net_ip_t ip;
 *         net_set_ip_to_any(&ip);
 *
 *         if (net_bind(conn, &ip, 80) == NET_ERR_OK) {
 *                 if (net_listen(conn) == NET_ERR_OK) {
 *                         puts("Listen connection");
 *
 *                         net_err_t err;
 *                         do {
 *                                 net_conn_t *new_conn;
 *                                 err = net_accept(conn, &new_conn);
 *                                 if (err == NET_ERR_OK) {
 *                                         puts("Accept connection");
 *
 *                                         net_ip_t ip;
 *                                         u16_t       port;
 *                                         net_get_conn_address(new_conn, &ip, &port, false);
 *
 *                                         printf("Remote connection from: %d.%d.%d.%d:%d\n",
 *                                                net_get_ip_part_a(&ip),
 *                                                net_get_ip_part_b(&ip),
 *                                                net_get_ip_part_c(&ip),
 *                                                net_get_ip_part_d(&ip),
 *                                                port);
 *
 *                                         // connection handle ...
 *
 *                                         net_delete_conn(new_conn);
 *                                 }
 *
 *                         } while (err == NET_ERR_OK);
 *                 }
 *         }
 *
 *         net_delete_conn(conn);
 * }
 *
 * // ...
 */
//==============================================================================
static inline net_err_t net_get_conn_address(net_conn_t *conn, net_ip_t *addr, u16_t *port, bool local)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netconn_getaddr(conn, addr, port, local);
#else
        (void) conn;
        (void) addr;
        (void) port;
        (void) local;
        return NET_ERR_INTERFACE_ERROR;
#endif
}

//==============================================================================
/**
 * @brief net_err_t net_bind(net_conn_t *conn, net_ip_t *addr, u16_t port)
 * The function <b>net_bind</b>() bind a connection pointed by <i>conn</i>
 * to a specific local IP <i>addr</i> and port </i>port</i>.
 * Binding one netconn twice might not always be checked correctly!
 *
 * @param conn          the netconn to bind
 * @param addr          the local IP address to bind the netconn to (use IP_ADDR_ANY to bind to all addresses)
 * @param port          the local port to bind the netconn to (not used for RAW)
 *
 * @errors None
 *
 * @return One of statuses defined in the <b>net_err_t</b> type.
 *
 * @example
 * #include <dnx/net.h>
 *
 * // ...
 *
 * net_conn_t *conn = net_new_conn(NET_CONN_TYPE_TCP);
 * if (conn) {
 *         net_ip_t ip;
 *         net_set_ip_to_any(&ip);
 *
 *         if (net_bind(conn, &ip, 80) == NET_ERR_OK) {
 *                 if (net_listen(conn) == NET_ERR_OK) {
 *                         puts("Listen connection");
 *
 *                         net_err_t err;
 *                         do {
 *                                 net_conn_t *new_conn;
 *                                 err = net_accept(conn, &new_conn);
 *                                 if (err == NET_ERR_OK) {
 *                                         puts("Accept connection");
 *
 *                                         net_ip_t ip;
 *                                         u16_t       port;
 *                                         net_get_conn_address(new_conn, &ip, &port, false);
 *
 *                                         printf("Remote connection from: %d.%d.%d.%d:%d\n",
 *                                                net_get_ip_part_a(&ip),
 *                                                net_get_ip_part_b(&ip),
 *                                                net_get_ip_part_c(&ip),
 *                                                net_get_ip_part_d(&ip),
 *                                                port);
 *
 *                                         // connection handle ...
 *
 *                                         net_delete_conn(new_conn);
 *                                 }
 *
 *                         } while (err == NET_ERR_OK);
 *                 }
 *         }
 *
 *         net_delete_conn(conn);
 * }
 *
 * // ...
 */
//==============================================================================
static inline net_err_t net_bind(net_conn_t *conn, net_ip_t *addr, u16_t port)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netconn_bind(conn, addr, port);
#else
        (void) conn;
        (void) addr;
        (void) port;
        return NET_ERR_INTERFACE_ERROR;
#endif
}

//==============================================================================
/**
 * @brief net_err_t net_connect(net_conn_t *conn, net_ip_t *addr, u16_t port)
 * The function <b>net_connect</b>() connect a connection pointed by <i>conn</i>
 * to a specific remote IP address <i>addr</i> and port <i>port</i>.
 *
 * @param conn          the connection
 * @param addr          the remote IP address to connect to
 * @param port          the remote port to connect to (no used for RAW)
 *
 * @errors None
 *
 * @return One of statuses defined in the <b>net_err_t</b> type.
 *
 * @example
 * #include <dnx/net.h>
 *
 * // ...
 *
 * net_conn_t *conn = net_new_conn(NET_CONN_TYPE_TCP);
 * if (conn) {
 *         ifconfig ifcfg;
 *         net_get_ifconfig(&ifcfg);
 *         net_ip_t local_ip = ifcfg.IP_address;
 *
 *         if (net_bind(conn, &local_ip, 0) == NET_ERR_OK) {
 *                         net_ip_t remote_ip = net_load_ip(123,165,14,56);
 *
 *                         if (net_connect(conn, &remote_ip, 80) == NET_ERR_OK) {
 *                                 // ...
 *                         }
 *                 }
 *         }
 *
 *         net_delete_conn(conn);
 * }
 *
 * // ...
 */
//==============================================================================
static inline net_err_t net_connect(net_conn_t *conn, net_ip_t *addr, u16_t port)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netconn_connect(conn, addr, port);
#else
        (void) conn;
        (void) addr;
        (void) port;
        return NET_ERR_INTERFACE_ERROR;
#endif
}

//==============================================================================
/**
 * @brief Disconnect a netconn from its current peer (only valid for UDP netconns).
 *
 * @param conn          the netconn to disconnect
 *
 * @return NET_ERR_OK if disconnected
 */
//==============================================================================
static inline net_err_t net_disconnect(net_conn_t *conn)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netconn_disconnect(conn);
#else
        (void) conn;
        return NET_ERR_INTERFACE_ERROR;
#endif
}

//==============================================================================
/**
 * @brief Set a TCP netconn into listen mode
 *
 * @param conn          the tcp netconn to set to listen mode
 *
 * @return NET_ERR_OK if the netconn was set to listen (UDP and RAW netconns)
 */
//==============================================================================
static inline net_err_t net_listen(net_conn_t *conn)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netconn_listen(conn);
#else
        (void) conn;
        return NET_ERR_INTERFACE_ERROR;
#endif
}

//==============================================================================
/**
 * @brief Accept a new connection on a TCP listening netconn.
 *
 * @param conn          the TCP listen netconn
 * @param new_conn      pointer where the new connection is stored
 *
 * @return NET_ERR_OK if a new connection has been received or an error code otherwise
 */
//==============================================================================
static inline net_err_t net_accept(net_conn_t *conn, net_conn_t ** new_conn)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netconn_accept(conn, new_conn);
#else
        (void) conn;
        (void) new_conn;
        return NET_ERR_INTERFACE_ERROR;
#endif
}

//==============================================================================
/**
 * @brief Receive data (in form of a netbuf containing a packet buffer) from a netconn
 *
 * @param conn          the netconn from which to receive data
 * @param new_buf       pointer where a new netbuf is stored when received data
 *
 * @return NET_ERR_OK if data has been received, an error code otherwise
 */
//==============================================================================
static inline net_err_t net_recv(net_conn_t *conn, net_buf_t **new_buf)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netconn_recv(conn, new_buf);
#else
        (void) conn;
        (void) new_buf;
        return NET_ERR_INTERFACE_ERROR;
#endif
}

//==============================================================================
/**
 * @brief Send data (in form of a netbuf) to a specific remote IP address and port.
 * Only to be used for UDP and RAW netconns (not TCP).
 *
 * @param conn          the netconn over which to send data
 * @param buf           a netbuf containing the data to send
 * @param addr          the remote IP address to which to send the data
 * @param port          the remote port to which to send the data
 *
 * @return NET_ERR_OK if data was sent, any other on error
 */
//==============================================================================
static inline net_err_t net_sendto(net_conn_t *conn, net_buf_t *buf, net_ip_t *addr, u16_t port)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netconn_sendto(conn, buf, addr, port);
#else
        (void) conn;
        (void) buf;
        (void) addr;
        (void) port;
        return NET_ERR_INTERFACE_ERROR;
#endif
}

//==============================================================================
/**
 * @brief Send data over a UDP or RAW netconn (that is already connected).
 *
 * @param conn          the UDP or RAW netconn over which to send data
 * @param buf           a netbuf containing the data to send
 *
 * @return NET_ERR_OK if data was sent, any other on error
 */
//==============================================================================
static inline net_err_t net_send(net_conn_t *conn, net_buf_t *buf)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netconn_send(conn, buf);
#else
        (void) conn;
        (void) buf;
        return NET_ERR_INTERFACE_ERROR;
#endif
}

//==============================================================================
/**
 * @brief Send data over a TCP netconn.
 *
 * @param conn          the TCP netconn over which to send data
 * @param data          pointer to the application buffer that contains the data to send
 * @param size          size of the application data to send
 * @param flags         combination of following flags :
 * - NET_CONN_FLAG_NOCOPY: data will not be copied into stack memory (ROM source)
 * - NET_CONN_FALG_COPY: data will be copied into memory belonging to the stack
 * - NET_CONN_FALG_MORE: for TCP connection, PSH flag will be set on last segment sent
 * - NET_CONN_FALG_DONTBLOCK: only write the data if all dat can be written at once
 * @param bytes_written pointer to a location that receives the number of written bytes
 *
 * @return NET_ERR_OK if data was sent, any other on error
 */
//==============================================================================
static inline net_err_t net_write_partly(net_conn_t *conn, const void *data,
                                               size_t size, net_flags_t flags,
                                               size_t *bytes_written)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netconn_write_partly(conn, data, size, flags, bytes_written);
#else
        (void) conn;
        (void) data;
        (void) size;
        (void) flags;
        (void) bytes_written;
        return NET_ERR_INTERFACE_ERROR;
#endif
}

//==============================================================================
/**
 * @brief Send data over a TCP netconn.
 *
 * @param conn          the TCP netconn over which to send data
 * @param data          pointer to the application buffer that contains the data to send
 * @param size          size of the application data to send
 * @param flags         combination of following flags :
 * - NET_CONN_FLAG_NOCOPY: data will not be copied into stack memory (ROM source)
 * - NET_CONN_FLAG_COPY: data will be copied into memory belonging to the stack
 * - NET_CONN_FLAG_MORE: for TCP connection, PSH flag will be set on last segment sent
 * - NET_CONN_FLAG_DONTBLOCK: only write the data if all data can be written at once
 *
 * @return NET_ERR_OK if data was sent, any other on error
 */
//==============================================================================
static inline net_err_t net_write(net_conn_t *conn, const void *data,
                                        size_t size, net_flags_t flags)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netconn_write(conn, data, size, flags);
#else
        (void) conn;
        (void) data;
        (void) size;
        (void) flags;
        return NET_ERR_INTERFACE_ERROR;
#endif
}

//==============================================================================
/**
 * @brief Close a TCP netconn (doesn't delete it).
 *
 * @param conn          the TCP netconn to close
 *
 * @return NET_ERR_OK if the netconn was closed, any other on error
 */
//==============================================================================
static inline net_err_t net_close(net_conn_t *conn)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netconn_close(conn);
#else
        (void) conn;
        return NET_ERR_INTERFACE_ERROR;
#endif
}

//==============================================================================
/**
 * @brief Shutdown one or both sides of a TCP netconn (doesn't delete it).
 *
 * @param conn          the TCP netconn to shut down
 * @param shut_rx       shutdown rx connection
 * @param shut_tx       shutdown tx connection
 *
 * @return NET_ERR_OK if the netconn was closed, any other on error
 */
//==============================================================================
static inline net_err_t net_shutdown(net_conn_t *conn, bool shut_rx, bool shut_tx)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netconn_shutdown(conn, shut_rx, shut_tx);
#else
        (void) conn;
        (void) shut_rx;
        (void) shut_tx;
        return NET_ERR_INTERFACE_ERROR;
#endif
}

//==============================================================================
/**
 * @brief Return last connection error
 *
 * @param conn          the netconn
 *
 * @return error value
 */
//==============================================================================
static inline net_err_t net_get_last_conn_error(net_conn_t *conn)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netconn_err(conn);
#else
        (void) conn;
        return NET_ERR_INTERFACE_ERROR;
#endif
}

//==============================================================================
/**
 * @brief Create (allocate) and initialize a new netbuf.
 * The netbuf doesn't yet contain a packet buffer!
 *
 * @return a pointer to a new buffer, NULL on lack of memory
 */
//==============================================================================
static inline net_buf_t *net_new_buf(void)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netbuf_new();
#else
        return NULL;
#endif
}

//==============================================================================
/**
 * @brief Deallocate a buffer allocated by net_new_buf().
 *
 * @param buf   pointer to a buffer allocated by net_new_buf()
 */
//==============================================================================
static inline void net_delete_buf(net_buf_t *buf)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        netbuf_delete(buf);
#else
        (void) buf;
#endif
}

//==============================================================================
/**
 * @brief Allocate memory for a packet buffer for a given buffer.
 *
 * @param buf           the netbuf for which to allocate a packet buffer
 * @param size          the size of the packet buffer to allocate
 *
 * @return pointer to the allocated memory, NULL if no memory could be allocated
 */
//==============================================================================
static inline void *net_alloc_buf(net_buf_t *buf, u16_t size)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netbuf_alloc(buf, size);
#else
        (void) buf;
        (void) size;
        return NULL;
#endif
}

//==============================================================================
/**
 * @brief Free the packet buffer included in a netbuf
 *
 * @param buf   pointer to the netbuf which contains the packet buffer to free
 */
//==============================================================================
static inline void net_free_buf(net_buf_t *buf)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        netbuf_free(buf);
#else
        (void) buf;
#endif
}

//==============================================================================
/**
 * @brief Let a netbuf reference existing (non-volatile) data.
 *
 * @param buf           netbuf which should reference the data
 * @param data          pointer to the data to reference
 * @param size          size of the data
 *
 * @return NET_ERR_OK        if data is referenced
 *         NET_ERR_MEM       if data couldn't be referenced due to lack of memory
 */
//==============================================================================
static inline net_err_t net_ref_buf(net_buf_t *buf, const void *data, u16_t size)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netbuf_ref(buf, data, size);
#else
        (void) buf;
        (void) data;
        (void) size;
        return NET_ERR_INTERFACE_ERROR;
#endif
}

//==============================================================================
/**
 * @brief Chain one netbuf to another (@see pbuf_chain)
 *
 * @param head          the first netbuf
 * @param tail          netbuf to chain after head, freed by this function,
 *                      may not be reference after returning
 */
//==============================================================================
static inline void net_buf_chain(net_buf_t *head, net_buf_t *tail)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        netbuf_chain(head, tail);
#else
        (void) head;
        (void) tail;
#endif
}

//==============================================================================
/**
 * @brief Get the data pointer and length of the data inside a netbuf.
 *
 * @param buf           netbuf to get the data from
 * @param data          pointer to a void pointer where to store the data pointer
 * @param len           pointer to an u16_t where the length of the data is stored
 *
 * @return NET_ERR_OK        if the information was retreived,
 *         NET_ERR_BUFFER    on error.
 */
//==============================================================================
static inline net_err_t net_buf_data(net_buf_t *buf, void **data, u16_t *len)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netbuf_data(buf, data, len);
#else
        (void) buf;
        (void) data;
        (void) len;
        return NET_ERR_INTERFACE_ERROR;
#endif
}

//==============================================================================
/**
 * @brief Move the current data pointer of a packet buffer contained in a netbuf
 * to the next part.
 * The packet buffer itself is not modified.
 *
 * @param buf   the netbuf to modify
 *
 * @return -1  if there is no next part
 *          1  if moved to the next part but now there is no next part
 *          0  if moved to the next part and there are still more parts
 */
//==============================================================================
static inline int net_next_buf(net_buf_t *buf)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netbuf_next(buf);
#else
        (void) buf;
        return -1;
#endif
}

//==============================================================================
/**
 * @brief Move the current data pointer of a packet buffer contained in a netbuf
 * to the beginning of the packet.
 * The packet buffer itself is not modified.
 *
 * @param buf   the netbuf to modify
 */
//==============================================================================
static inline void net_first_buf(net_buf_t *buf)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        netbuf_first(buf);
#else
        (void) buf;
#endif
}

//==============================================================================
/**
 * @brief Copy (part of) the contents of a packet buffer to an application supplied buffer.
 *
 * @param buf           the pbuf from which to copy data
 * @param data          the application supplied buffer
 * @param len           length of data to copy (dataptr must be big enough). No more
 *                      than buf->tot_len will be copied, irrespective of len
 * @param offset        offset into the packet buffer from where to begin copying len bytes
 *
 * @return the number of bytes copied, or 0 on failure
 */
//==============================================================================
static inline int net_copy_buf_partial(net_buf_t *buf, void *data, u16_t len, u16_t offset)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netbuf_copy_partial(buf, data, len, offset);
#else
        (void) buf;
        (void) data;
        (void) len;
        (void) offset;
        return 0;
#endif
}

//==============================================================================
/**
 * @brief Copy the contents of a packet buffer to an application supplied buffer.
 *
 * @param buf           the pbuf from which to copy data
 * @param data          the application supplied buffer
 * @param len           length of data to copy (dataptr must be big enough). No more
 *                      than buf->tot_len will be copied, irrespective of len
 *
 * @return the number of bytes copied, or 0 on failure
 */
//==============================================================================
static inline int net_copy_buf(net_buf_t *buf, void *data, u16_t len)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netbuf_copy(buf, data, len);
#else
        (void) buf;
        (void) data;
        (void) len;
        return 0;
#endif
}

//==============================================================================
/**
 * @brief Copy application supplied data into a buf.
 * This function can only be used to copy the equivalent of buf->tot_len data.
 *
 * @param buf           pbuf to fill with data
 * @param data          application supplied data buffer
 * @param len           length of the application supplied data buffer
 *
 * @return NET_ERR_OK                if successful,
 *         NET_ERR_OUT_OF_MEMORY     if the pbuf is not big enough
 */
//==============================================================================
static inline net_err_t net_take_buf(net_buf_t *buf, void *data, u16_t len)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netbuf_take(buf, data, len);
#else
        (void) buf;
        (void) data;
        (void) len;
        return NET_ERR_INTERFACE_ERROR;
#endif
}

//==============================================================================
/**
 * @brief Gets size of buffer
 *
 * @param buf           the netbuf for which length is getting
 *
 * @return netbuf length
 */
//==============================================================================
static inline u16_t net_get_buf_length(net_buf_t *buf)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netbuf_len(buf);
#else
        (void) buf;
        return 0;
#endif
}

//==============================================================================
/**
 * @brief Return address which buffer is from
 *
 * @param buf           the netbuf
 *
 * @return buffer address source
 */
//==============================================================================
static inline net_ip_t net_get_buf_origin_address(net_buf_t *buf)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return *netbuf_fromaddr(buf);
#else
        (void) buf;
        net_ip_t ip;
        ip.addr = 0;
        return ip;
#endif
}

//==============================================================================
/**
 * @brief Return port of buffer
 *
 * @param buf           the netbuf
 *
 * @return port number
 */
//==============================================================================
static inline u16_t net_get_buf_port(net_buf_t *buf)
{
#if (CONFIG_NETWORK_ENABLE > 0)
        return netbuf_fromport(buf);
#else
        (void) buf;
        return 0;
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* _DNXNET_H_ */
/*==============================================================================
  End of file
==============================================================================*/
