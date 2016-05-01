/*=========================================================================*//**
@file    net.h

@author  Daniel Zorychta

@brief   This file provide network API.

@note    Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/**
\defgroup dnx-net-h <dnx/net.h>

Network handling library.

*/
/**@{*/

#ifndef _DNX_NET_H_
#define _DNX_NET_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <stdint.h>
#include <kernel/syscall.h>
#include <stddef.h>
#include "net/netm.h"

/*==============================================================================
  Exported macros
==============================================================================*/
#include "lwip/def.h" // TODO must be defined in the network layer
#define htons(x) lwip_htons(x)
#define ntohs(x) lwip_ntohs(x)
#define htonl(x) lwip_htonl(x)
#define ntohl(x) lwip_ntohl(x)

//#define hton_u16(NET_family_t family, u16_t value)

/*==============================================================================
  Exported object types
==============================================================================*/


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
 * @brief  Function set up selected network interface.
 *
 * @param  family       interface family
 * @param  config       configuration for specified interface
 * @param  config_size  size of configuration object
 *
 * @return On success 0 is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 */
//==============================================================================
static inline int ifup(NET_family_t family, const NET_generic_config_t *config)
{
        int result = -1;
        syscall(SYSCALL_NETIFUP, &result, &family, config);
        return result;
}

//==============================================================================
/**
 * @brief  Function set down selected network interface.
 *
 * @param  family       interface family
 *
 * @return On success 0 is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 */
//==============================================================================
static inline int ifdown(NET_family_t family)
{
        int result = -1;
        syscall(SYSCALL_NETIFDOWN, &result, &family);
        return result;
}

//==============================================================================
/**
 * @brief  Function get status of selected interface.
 *
 * @param  family       interface family
 * @param  status       status of selected interface
 * @param  status_size  size of status object
 *
 * @return On success 0 is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 */
//==============================================================================
static inline int ifstatus(NET_family_t family, NET_generic_status_t *status)
{
        int result = -1;
        syscall(SYSCALL_NETIFSTATUS, &result, &family, status);
        return result;
}


//==============================================================================
/**
 * @brief Creates an endpoint for communication and returns a socket.
 *
 * @param family        The protocol family to use for this socket.
 * @param protocol      The protocol name.
 *
 * @return On error @b NULL is returned and @ref errno value is set appropriately,
 *         otherwise new socket pointer.
 */
//==============================================================================
static inline SOCKET *socket_new(NET_family_t family, NET_protocol_t protocol)
{
        SOCKET *socket = NULL;
        syscall(SYSCALL_NETSOCKETCREATE, &socket, &family, &protocol);
        return socket;
}

//==============================================================================
/**
 * @brief  The function closes socket.
 *
 * @param  socket       Socket
 */
//==============================================================================
static inline void socket_delete(SOCKET *socket)
{
        syscall(SYSCALL_NETSOCKETDESTROY, NULL, socket);
}

//==============================================================================
/**
 * @brief Assigns an address to the socket. When a socket is created with
 *        socket(), it exists in an address family space but has no address
 *        assigned. bind() requests that the address pointed to by addressPtr be
 *        assigned to the socket. Clients do not normally require that an address
 *        be assigned to a socket. However, servers usually require that the
 *        socket be bound to a well known address.
 *
 * @param socket                The socket
 * @param sockAddr              The pointer to the structure containing
 *                              the address to assign.
 *
 * @return On success 0 is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 */
//==============================================================================
static inline int socket_bind(SOCKET *socket, const NET_generic_sockaddr_t *sockAddr)
{
        int result = -1;
        syscall(SYSCALL_NETBIND, &result, socket, sockAddr);
        return result;
}

//==============================================================================
/**
 * @brief The function connect the socket to a specific remote IP address.
 *
 * @param socket        The socket
 * @param sockAddr      The pointer to the structure containing address
 *
 * @return On success 0 is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 */
//==============================================================================
static inline int socket_connect(SOCKET *socket, const NET_generic_sockaddr_t *sockAddr)
{
        int result = -1;
        syscall(SYSCALL_NETCONNECT, &result, socket, sockAddr);
        return result;
}

//==============================================================================
/**
 * @brief The function disconnect the socket from current connection.
 *
 * @param socket        The socket
 *
 * @return On success 0 is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 */
//==============================================================================
static inline int socket_disconnect(SOCKET *socket)
{
        int result = -1;
        syscall(SYSCALL_NETDISCONNECT, &result, socket);
        return result;
}

//==============================================================================
/**
 * @brief The function set a connection into listen mode.
 *
 * @param socket        The socket
 *
 * @return On success 0 is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 */
//==============================================================================
static inline int socket_listen(SOCKET *socket)
{
        int result = -1;
        syscall(SYSCALL_NETLISTEN, &result, socket);
        return result;
}

//==============================================================================
/**
 * @brief The function accept a new connection and create new socket.
 *
 * @param socket        The listen connection
 * @param new_socket    The new connection
 *
 * @return On error 0 is returned end @ref errno value is set appropriately,
 *         otherwise new socket descriptor.
 */
//==============================================================================
static inline int socket_accept(SOCKET *socket, SOCKET **new_socket)
{
        int result = -1;
        syscall(SYSCALL_NETACCEPT, &result, socket, new_socket);
        return result;
}

//==============================================================================
/**
 * @brief  The function is used to receive messages from socket. recv()
 *         may be used only on a connected socket (see connect(), accept()).
 *
 * @param  socket       The socket from which to receive the data.
 * @param  buf          The buffer into which the received data is put.
 * @param  len          The buffer length.
 * @param  flags        Flags parameters that can be OR'ed together.@n
 *                      NET_FLAGS__REWIND  : start copy payload from the beginning.@n
 *                      NET_FLAGS__RECVDONE: this flag remove received buffer even
 *                                           if entire payload is not read.@n
 *
 * @return Number of bytes actually received from the socket, or -1 on error and
 *         @ref errno value is set appropriately.
 */
//==============================================================================
static inline int socket_recv(SOCKET *socket, void *buf, size_t len, NET_flags_t flags)
{
        int result = -1;
        syscall(SYSCALL_NETRECV, &result, socket, buf, &len, &flags);
        return result;
}

//==============================================================================
/**
 * @brief  The function is used to receive messages from socket. read()
 *         may be used only on a connected socket (see connect(), accept()).
 *
 * @param  socket       The socket from which to receive the data.
 * @param  buf          The buffer into which the received data is put.
 * @param  len          The buffer length.
 *
 * @return Number of bytes actually received from the socket, or -1 on error and
 *         @ref errno value is set appropriately.
 */
//==============================================================================
static inline int socket_read(SOCKET *socket, void *buf, size_t len)
{
        return socket_recv(socket, buf, len, NET_FLAGS__NONE);
}

//==============================================================================
/**
 * @brief  The function is used to receive messages from another socket.
 *         recvfrom() may be used to receive data on a socket whether it is in
 *         a connected state or not but not on a TCP socket.
 *
 * @param  socket           The socket descriptor from which to receive the data.
 * @param  buf              The buffer into which the received data is put.
 * @param  len              The buffer length.
 * @param  flags            Flags parameters that can be OR'ed together.
 * @param  from_sockaddr    The socket address from which to receive the data.
 *
 * @return Number of bytes actually received from the socket, or -1 on error and
 *         @ref errno value is set appropriately.
 */
//==============================================================================
static inline int socket_recvfrom(SOCKET                 *socket,
                                  char                   *buf,
                                  size_t                  len,
                                  NET_flags_t             flags,
                                  NET_generic_sockaddr_t *from_sockaddr)
{
        int result = -1;
        syscall(SYSCALL_NETRECVFROM, &result, socket, buf, &len, &flags, from_sockaddr);
        return result;
}

//==============================================================================
/**
 * @brief  The function is used to transmit a message to another transport
 *         end-point. sendto() may be used any time the socket is in the
 *         unconnected state, and never for TCP sockets.
 *
 * @param  socket       The socket to use to send the data.
 * @param  buf          A pointer to the buffer to send.
 * @param  len          The length of the buffer to send.
 * @param  flags        Flags parameters that can be OR'ed together.
 * @param  to_sockaddr  The address to send the data to.
 * @param  to_addr_sz   The size of address.
 *
 * @return Number of bytes actually sent on the socket, or -1 on error and
 *         @ref errno value is set appropriately.
 */
//==============================================================================
static inline int socket_sendto(SOCKET                       *socket,
                                const void                   *buf,
                                size_t                        len,
                                NET_flags_t                   flags,
                                const NET_generic_sockaddr_t *to_sockaddr)
{
        int result = -1;
        syscall(SYSCALL_NETSENDTO, &result, socket, buf, &len, &flags, to_sockaddr);
        return result;
}

//==============================================================================
/**
 * @brief  The function is used to transmit a message to another transport
 *         end-point. send() may be used only when the socket is in a connected
 *         state.
 *
 * @param  socket       The socket to use to send the data.
 * @param  buf          A pointer to the buffer to send.
 * @param  len          The length of the buffer to send.
 * @param  flags        Flags parameters that can be OR'ed together.
 *                      NET_FLAGS__COPY  : create buffer for output data [default].@n
 *                      NET_FLAGS__NOCOPY: does not create extra buffer for
 *                                         output data.@n
 *
 * @return Number of bytes actually sent on the socket, or -1 on error and
 *         @ref errno value is set appropriately.
 */
//==============================================================================
static inline int socket_send(SOCKET *socket, const void *buf, size_t len, NET_flags_t flags)
{
        int result = -1;
        syscall(SYSCALL_NETSEND, &result, socket, buf, &len, &flags);
        return result;
}

//==============================================================================
/**
 * @brief  The function is used to transmit a message to another transport
 *         end-point. write() may be used only when the socket is in a connected
 *         state.
 *
 * @param  socket       The socket to use to send the data.
 * @param  buf          A pointer to the buffer to send.
 * @param  len          The length of the buffer to send.
 *
 * @return Number of bytes actually sent on the socket, or -1 on error and
 *         @ref errno value is set appropriately.
 */
//==============================================================================
static inline int socket_write(SOCKET *socket, const void *buf, size_t len)
{
        return socket_send(socket, buf, len, NET_FLAGS__NONE);
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static inline int socket_shutdown(SOCKET *socket, NET_shut_t how)
{
        int result = -1;
        syscall(SYSCALL_NETSHUTDOWN, &result, socket, &how);
        return result;
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static inline int socket_close(SOCKET *socket)
{
        return socket_shutdown(socket, NET_SHUT__RDWR);
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static inline int socket_set_recv_timeout(SOCKET *socket, uint32_t timeout)
{
        int result = -1;
        syscall(SYSCALL_NETSETRECVTIMEOUT, &result, socket, &timeout);
        return result;
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static inline int socket_set_send_timeout(SOCKET *socket, uint32_t timeout)
{
        int result = -1;
        syscall(SYSCALL_NETSETSENDTIMEOUT, &result, socket, &timeout);
        return result;
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static inline int get_host_by_name(NET_family_t            family,
                                   const char             *name,
                                   NET_generic_sockaddr_t *sock_addr)
{
        int result = -1;
        syscall(SYSCALL_NETGETHOSTBYNAME, &result, &family, name, sock_addr);
        return result;
}

#ifdef __cplusplus
}
#endif

#endif /* _DNX_NET_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
