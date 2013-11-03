/*=========================================================================*//**
@file    netapi.h

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

#ifndef _NETAPI_H_
#define _NETAPI_H_

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
typedef ip_addr_t                               netapi_ip_t;
typedef struct netconn                          netapi_conn_t;
typedef struct netbuf                           netapi_buf_t;

typedef enum netapi_conn_type {
        NETAPI_CONN_TYPE_INVALID                = NETCONN_INVALID,
        NETAPI_CONN_TYPE_TCP                    = NETCONN_TCP,
        NETAPI_CONN_TYPE_UDP                    = NETCONN_UDP,
        NETAPI_CONN_TYPE_UDPLITE                = NETCONN_UDPLITE,
        NETAPI_CONN_TYPE_UDPNOCHKSUM            = NETCONN_UDPNOCHKSUM,
        NETAPI_CONN_TYPE_RAW                    = NETCONN_RAW
} netapi_conn_type_t;

typedef enum netapi_conn_state {
        NETAPI_CONN_STATE_NONE                  = NETCONN_NONE,
        NETAPI_CONN_STATE_WRITE                 = NETCONN_WRITE,
        NETAPI_CONN_STATE_LISTEN                = NETCONN_LISTEN,
        NETAPI_CONN_STATE_CONNECT               = NETCONN_CONNECT,
        NETAPI_CONN_STATE_CLOSE                 = NETCONN_CLOSE
} netapi_conn_state_t;

typedef enum netapi_err {
        NETAPI_ERR_OK                           = ERR_OK,
        NETAPI_ERR_OUT_OF_MEMORY                = ERR_MEM,
        NETAPI_ERR_BUFFER                       = ERR_BUF,
        NETAPI_ERR_TIMEOUT                      = ERR_TIMEOUT,
        NETAPI_ERR_ROUTING_PROBLEM              = ERR_RTE,
        NETAPI_ERR_OPERATION_IN_PROGRESS        = ERR_INPROGRESS,
        NETAPI_ERR_ILLEGAL_VALUE                = ERR_VAL,
        NETAPI_ERR_OPERATION_WOULD_BLOCK        = ERR_WOULDBLOCK,
        NETAPI_ERR_ADDRESS_IN_USE               = ERR_USE,
        NETAPI_ERR_IS_CONNECTED                 = ERR_ISCONN,
        NETAPI_ERR_CONNECTION_ABORTED           = ERR_ABRT,
        NETAPI_ERR_CONNECTION_RESET             = ERR_RST,
        NETAPI_ERR_CONNECTION_CLOSED            = ERR_CLSD,
        NETAPI_ERR_NOT_CONNECTED                = ERR_CONN,
        NETAPI_ERR_ILLEGAL_ARGUMENT             = ERR_ARG,
        NETAPI_ERR_INTERFACE_ERROR              = ERR_IF
} netapi_err_t;

typedef enum netapi_flags {
        NETAPI_CONN_FLAG_COPY                   = NETCONN_COPY,
        NETAPI_CONN_FLAG_MORE                   = NETCONN_MORE,
        NETAPI_CONN_FLAG_DONTBLOCK              = NETCONN_DONTBLOCK
} netapi_flags_t;

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
 * @brief Function start DHCP client to gets IP addresses
 *
 * @return 0 if success, otherwise -1
 */
//==============================================================================
static inline int netapi_start_DHCP_client(void)
{
        return _ethif_start_DHCP_client();
}

//==============================================================================
/**
 * @brief Function stop DHCP client
 *
 * @return 0 if success, otherwise -1
 */
//==============================================================================
static inline int netapi_stop_DHCP_client(void)
{
        return _ethif_stop_DHCP_client();
}

//==============================================================================
/**
 * @brief Function renew DHCP connection
 *
 * @return 0 if success, otherwise -1
 */
//==============================================================================
static inline int netapi_renew_DHCP_connection(void)
{
        return _ethif_renew_DHCP_connection();
}

//==============================================================================
/**
 * @brief Function inform DHCP server about static IP configuration
 *
 * @return 0 if success, otherwise -1
 */
//==============================================================================
static inline int netapi_inform_DHCP_server(void)
{
        return _ethif_inform_DHCP_server();
}

//==============================================================================
/**
 * @brief Function configure network on static IP addresses
 *
 * @param ip            a IP address
 * @param netmask       a net mask
 * @param gateway       a gateway IP address
 *
 * @return 0 if success, otherwise -1
 */
//==============================================================================
static inline int netapi_ifup(const netapi_ip_t *ip, const netapi_ip_t *netmask, const netapi_ip_t *gateway)
{
        return _ethif_if_up(ip, netmask, gateway);
}

//==============================================================================
/**
 * @brief Function close static configured network
 *
 * @return 0 if success, otherwise -1
 */
//==============================================================================
static inline int netapi_ifdown(void)
{
        return _ethif_if_down();
}

//==============================================================================
/**
 * @brief Function gets network connection informations
 *
 * @param ifcfg         a pointer to information object
 *
 * @return 0 if success, otherwise -1
 */
//==============================================================================
static inline int netapi_get_ifconfig(ifconfig *ifcfg)
{
        return _ethif_get_ifconfig(ifcfg);
}

//==============================================================================
/**
 * @brief Function set ip address
 *
 * @param ip            a IP address object
 * @param a             IP part a
 * @param b             IP part b
 * @param c             IP part c
 * @param d             IP part d
 */
//==============================================================================
static inline void netapi_set_ip(netapi_ip_t *ip, const u8_t a, const u8_t b, const u8_t c, const u8_t d)
{
        IP4_ADDR(ip, a, b ,c ,d);
}

//==============================================================================
/**
 * @brief Function returns 1st part of IP address
 *
 * @param ip            a IP address
 *
 * @return IP value
 */
//==============================================================================
static inline u8_t netapi_get_ip_part_a(netapi_ip_t *ip)
{
        return ip4_addr1(ip);
}

//==============================================================================
/**
 * @brief Function returns 2nd part of IP address
 *
 * @param ip            a IP address
 *
 * @return IP value
 */
//==============================================================================
static inline u8_t netapi_get_ip_part_b(netapi_ip_t *ip)
{
        return ip4_addr2(ip);
}

//==============================================================================
/**
 * @brief Function returns 3rd part of IP address
 *
 * @param ip            a IP address
 *
 * @return IP value
 */
//==============================================================================
static inline u8_t netapi_get_ip_part_c(netapi_ip_t *ip)
{
        return ip4_addr3(ip);
}

//==============================================================================
/**
 * @brief Function returns 4th part of IP address
 *
 * @param ip            a IP address
 *
 * @return IP value
 */
//==============================================================================
static inline u8_t netapi_get_ip_part_d(netapi_ip_t *ip)
{
        return ip4_addr4(ip);
}

//==============================================================================
/**
 * @brief Function set IP to 0.0.0.0
 *
 * @param ip            a IP address
 */
//==============================================================================
static inline void netapi_set_ip_to_any(netapi_ip_t *ip)
{
        *ip = ip_addr_any;
}

//==============================================================================
/**
 * @brief Function set IP to 127.0.0.1
 *
 * @param ip            a IP address
 */
//==============================================================================
static inline void netapi_set_ip_to_loopback(netapi_ip_t *ip)
{
        ip_addr_set_loopback(ip);
}

//==============================================================================
/**
 * @brief Function set IP to 127.0.0.1
 *
 * @param ip            a IP address
 */
//==============================================================================
static inline void netapi_set_ip_to_broadcast(netapi_ip_t *ip)
{
        *ip = ip_addr_broadcast;
}

//==============================================================================
/**
 * @brief Create a new netconn (of a specific type) that has a callback function.
 * The corresponding pcb is also created.
 *
 * @param type          the type of 'connection' to create (@see enum netapi_conn_type)
 *
 * @return a newly allocated object or NULL on memory error
 */
//==============================================================================
static inline netapi_conn_t *netapi_new_conn(netapi_conn_type_t type)
{
        return netconn_new(type);
}

//==============================================================================
/**
 * @brief Close a netconn 'connection' and free its resources.
 * UDP and RAW connection are completely closed, TCP pcbs might still be in a waitstate
 * after this returns.
 *
 * @param conn          the connection to delete
 *
 * @return NETAPI_ERR_OK if the connection was deleted
 */
//==============================================================================
static inline netapi_err_t netapi_delete_conn(netapi_conn_t *conn)
{
        return netconn_delete(conn);
}

//==============================================================================
/**
 * @brief Function returns connection type
 *
 * @param conn          the connection
 *
 * @return connection type
 */
//==============================================================================
static inline netapi_conn_type_t netapi_get_conn_type(netapi_conn_t *conn)
{
        return netconn_type(conn);
}

//==============================================================================
/**
 * @brief Function check if in connection fatal error occurred
 *
 * @param error         the error to examine
 *
 * @return true if error is fatal, otherwise false
 */
//==============================================================================
static inline bool netapi_is_fatal_error(netapi_err_t error)
{
        return ERR_IS_FATAL(error) ? true : false;
}

//==============================================================================
/**
 * @brief Get the local or remote IP address and port of a netconn.
 * For RAW netconns, this returns the protocol instead of a port!
 *
 * @param conn          the netconn to query
 * @param addr          a pointer to which to save the IP address
 * @param port          a pointer to which to save the port (or protocol for RAW)
 * @param local         true to get the local IP address, false to get the remote one
 *
 * @retval NETAPI_ERR_NOT_CONNECTED     for invalid connections
 * @retval NETAPI_ERR_OK                if the information was retrieved
 */
//==============================================================================
static inline netapi_err_t netapi_get_conn_address(netapi_conn_t *conn, netapi_ip_t *addr, u16_t *port, bool local)
{
        return netconn_getaddr(conn, addr, port, local);
}

//==============================================================================
/**
 * @brief Bind a netconn to a specific local IP address and port.
 * Binding one netconn twice might not always be checked correctly!
 *
 * @param conn          the netconn to bind
 * @param addr          the local IP address to bind the netconn to (use IP_ADDR_ANY
 *                      to bind to all addresses)
 * @param port          the local port to bind the netconn to (not used for RAW)
 *
 * @return NETAPI_ERR_OK if bound, any other on failure
 */
//==============================================================================
static inline netapi_err_t netapi_bind(netapi_conn_t *conn, netapi_ip_t *addr, u16_t port)
{
        return netconn_bind(conn, addr, port);
}

//==============================================================================
/**
 * @brief Connect a netconn to a specific remote IP address and port.
 *
 * @param conn          the netconn to connect
 * @param addr          the remote IP address to connect to
 * @param port          the remote port to connect to (no used for RAW)
 *
 * @return NETAPI_ERR_OK if connected, return value of tcp_/udp_/raw_connect otherwise
 */
//==============================================================================
static inline netapi_err_t netapi_connect(netapi_conn_t *conn, netapi_ip_t *addr, u16_t port)
{
        return netconn_connect(conn, addr, port);
}

//==============================================================================
/**
 * @brief Disconnect a netconn from its current peer (only valid for UDP netconns).
 *
 * @param conn          the netconn to disconnect
 *
 * @return NETAPI_ERR_OK if disconnected
 */
//==============================================================================
static inline netapi_err_t netapi_disconnect(netapi_conn_t *conn)
{
        return netconn_disconnect(conn);
}

//==============================================================================
/**
 * @brief Set a TCP netconn into listen mode
 *
 * @param conn          the tcp netconn to set to listen mode
 *
 * @return NETAPI_ERR_OK if the netconn was set to listen (UDP and RAW netconns)
 */
//==============================================================================
static inline netapi_err_t netapi_listen(netapi_conn_t *conn)
{
        return netconn_listen(conn);
}

//==============================================================================
/**
 * @brief Accept a new connection on a TCP listening netconn.
 *
 * @param conn          the TCP listen netconn
 * @param new_conn      pointer where the new connection is stored
 *
 * @return NETAPI_ERR_OK if a new connection has been received or an error code otherwise
 */
//==============================================================================
static inline netapi_err_t netapi_accept(netapi_conn_t *conn, netapi_conn_t ** new_conn)
{
        return netconn_accept(conn, new_conn);
}

//==============================================================================
/**
 * @brief Receive data (in form of a netbuf containing a packet buffer) from a netconn
 *
 * @param conn          the netconn from which to receive data
 * @param new_buf       pointer where a new netbuf is stored when received data
 *
 * @return NETAPI_ERR_OK if data has been received, an error code otherwise
 */
//==============================================================================
static inline netapi_err_t netapi_recv(netapi_conn_t *conn, netapi_buf_t **new_buf)
{
        return netconn_recv(conn, new_buf);
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
 * @return NETAPI_ERR_OK if data was sent, any other on error
 */
//==============================================================================
static inline netapi_err_t netapi_sendto(netapi_conn_t *conn, netapi_buf_t *buf, netapi_ip_t *addr, u16_t port)
{
        return netconn_sendto(conn, buf, addr, port);
}

//==============================================================================
/**
 * @brief Send data over a UDP or RAW netconn (that is already connected).
 *
 * @param conn          the UDP or RAW netconn over which to send data
 * @param buf           a netbuf containing the data to send
 *
 * @return NETAPI_ERR_OK if data was sent, any other on error
 */
//==============================================================================
static inline netapi_err_t netapi_send(netapi_conn_t *conn, netapi_buf_t *buf)
{
        return netconn_send(conn, buf);
}

//==============================================================================
/**
 * @brief Send data over a TCP netconn.
 *
 * @param conn          the TCP netconn over which to send data
 * @param data          pointer to the application buffer that contains the data to send
 * @param size          size of the application data to send
 * @param flags         combination of following flags :
 * - NETAPI_CONN_FALG_COPY: data will be copied into memory belonging to the stack
 * - NETAPI_CONN_FALG_MORE: for TCP connection, PSH flag will be set on last segment sent
 * - NETAPI_CONN_FALG_DONTBLOCK: only write the data if all dat can be written at once
 * @param bytes_written pointer to a location that receives the number of written bytes
 *
 * @return NETAPI_ERR_OK if data was sent, any other on error
 */
//==============================================================================
static inline netapi_err_t netapi_write_partly(netapi_conn_t *conn, const void *data,
                                               size_t size, netapi_flags_t flags,
                                               size_t *bytes_written)
{
        return netconn_write_partly(conn, data, size, flags, bytes_written);
}

//==============================================================================
/**
 * @brief Send data over a TCP netconn.
 *
 * @param conn          the TCP netconn over which to send data
 * @param data          pointer to the application buffer that contains the data to send
 * @param size          size of the application data to send
 * @param flags         combination of following flags :
 * - NETAPI_CONN_FALG_COPY: data will be copied into memory belonging to the stack
 * - NETAPI_CONN_FALG_MORE: for TCP connection, PSH flag will be set on last segment sent
 * - NETAPI_CONN_FALG_DONTBLOCK: only write the data if all dat can be written at once
 *
 * @return NETAPI_ERR_OK if data was sent, any other on error
 */
//==============================================================================
static inline netapi_err_t netapi_write(netapi_conn_t *conn, const void *data,
                                        size_t size, netapi_flags_t flags)
{
        return netconn_write(conn, data, size, flags);
}

//==============================================================================
/**
 * @brief Close a TCP netconn (doesn't delete it).
 *
 * @param conn          the TCP netconn to close
 *
 * @return NETAPI_ERR_OK if the netconn was closed, any other on error
 */
//==============================================================================
static inline netapi_err_t netapi_close(netapi_conn_t *conn)
{
        return netconn_close(conn);
}

//==============================================================================
/**
 * @brief Shutdown one or both sides of a TCP netconn (doesn't delete it).
 *
 * @param conn          the TCP netconn to shut down
 * @param shut_rx       shutdown rx connection
 * @param shut_tx       shutdown tx connection
 *
 * @return NETAPI_ERR_OK if the netconn was closed, any other on error
 */
//==============================================================================
static inline netapi_err_t netapi_shutdown(netapi_conn_t *conn, bool shut_rx, bool shut_tx)
{
        return netconn_shutdown(conn, shut_rx, shut_tx);
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
static inline netapi_err_t netapi_get_last_conn_error(netapi_conn_t *conn)
{
        return netconn_err(conn);
}

//==============================================================================
/**
 * @brief Create (allocate) and initialize a new netbuf.
 * The netbuf doesn't yet contain a packet buffer!
 *
 * @return a pointer to a new buffer, NULL on lack of memory
 */
//==============================================================================
static inline netapi_buf_t *netapi_new_buf(void)
{
        return netbuf_new();
}

//==============================================================================
/**
 * @brief Deallocate a buffer allocated by netapi_new_buf().
 *
 * @param buf   pointer to a buffer allocated by netapi_new_buf()
 */
//==============================================================================
static inline void netapi_delete_buf(netapi_buf_t *buf)
{
        netbuf_delete(buf);
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
static inline void *netapi_alloc_buf(netapi_buf_t *buf, u16_t size)
{
        return netbuf_alloc(buf, size);
}

//==============================================================================
/**
 * @brief Free the packet buffer included in a netbuf
 *
 * @param buf   pointer to the netbuf which contains the packet buffer to free
 */
//==============================================================================
static inline void netapi_free_buf(netapi_buf_t *buf)
{
        netbuf_free(buf);
}

//==============================================================================
/**
 * @brief Let a netbuf reference existing (non-volatile) data.
 *
 * @param buf           netbuf which should reference the data
 * @param data          pointer to the data to reference
 * @param size          size of the data
 *
 * @return NETAPI_ERR_OK        if data is referenced
 *         NETAPI_ERR_MEM       if data couldn't be referenced due to lack of memory
 */
//==============================================================================
static inline netapi_err_t netapi_ref_buf(netapi_buf_t *buf, const void *data, u16_t size)
{
        return netbuf_ref(buf, data, size);
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
static inline void netapi_buf_chain(netapi_buf_t *head, netapi_buf_t *tail)
{
        netbuf_chain(head, tail);
}

//==============================================================================
/**
 * @brief Get the data pointer and length of the data inside a netbuf.
 *
 * @param buf           netbuf to get the data from
 * @param data          pointer to a void pointer where to store the data pointer
 * @param len           pointer to an u16_t where the length of the data is stored
 *
 * @return NETAPI_ERR_OK        if the information was retreived,
 *         NETAPI_ERR_BUFFER    on error.
 */
//==============================================================================
static inline netapi_err_t netapi_buf_data(netapi_buf_t *buf, void **data, u16_t *len)
{
        return netbuf_data(buf, data, len);
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
static inline int netapi_next_buf(netapi_buf_t *buf)
{
        return netbuf_next(buf);
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
static inline void netapi_first_buf(netapi_buf_t *buf)
{
        netbuf_first(buf);
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
static inline int netapi_copy_buf_partial(netapi_buf_t *buf, void *data, u16_t len, u16_t offset)
{
        return netbuf_copy_partial(buf, data, len, offset);
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
static inline int netapi_copy_buf(netapi_buf_t *buf, void *data, u16_t len)
{
        return netbuf_copy(buf, data, len);
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
 * @return NETAPI_ERR_OK                if successful,
 *         NETAPI_ERR_OUT_OF_MEMORY     if the pbuf is not big enough
 */
//==============================================================================
static inline netapi_err_t netapi_take_buf(netapi_buf_t *buf, void *data, u16_t len)
{
        return netbuf_take(buf, data, len);
}

//==============================================================================
/**
 * @brief Gets size of buffer
 *
 * @param buf           the netbuf for which length is getting
 *
 * return netbuf length
 */
//==============================================================================
static inline u16_t netapi_get_buf_length(netapi_buf_t *buf)
{
        return netbuf_len(buf);
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
static inline netapi_ip_t netapi_get_buf_origin_address(netapi_buf_t *buf)
{
        return *netbuf_fromaddr(buf);
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
static inline u16_t netapi_get_buf_port(netapi_buf_t *buf)
{
        return netbuf_fromport(buf);
}

#ifdef __cplusplus
}
#endif

#endif /* _NETAPI_H_ */
/*==============================================================================
  End of file
==============================================================================*/
