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

/*==============================================================================
  Exported object types
==============================================================================*/
#ifdef DOXYGEN
typedef struct socket SOCKET;

typedef enum {
        NET_FAMILY__INET,
        NET_FAMILY__CAN,
        NET_FAMILY__RFM,
        NET_FAMILY__MICROLAN
} NET_family_t;

typedef enum {
        NET_PROTOCOL__UDP,
        NET_PROTOCOL__TCP,
        NET_PROTOCOL__ISO_TP
} NET_protocol_t;

typedef enum {
        NET_FLAGS__NO_COPY   = (1 << 0),
        NET_FLAGS__COPY      = (1 << 1),
        NET_FLAGS__MORE      = (1 << 2),
        NET_FLAGS__DONTBLOCK = (1 << 3),
} NET_flags_t;

typedef enum {
        NET_SHUT__RD   = (1 << 0),
        NET_SHUT__WR   = (1 << 1),
        NET_SHUT__RDWR = (NET_SHUT__RD | NET_SHUT__WR)
} NET_shut_t;

typedef struct {
        uint32_t    port;
        const char *addr;
} NET_addr_t;
#endif


/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/
static inline int ifup(NET_family_t family, const void *config, size_t size)
{
        int result = -1;
        syscall(SYSCALL_NETIFUP, &result, &family, config, &size);
        return result;
}

static inline int ifdown(NET_family_t family)
{
        int result = -1;
        syscall(SYSCALL_NETIFDOWN, &result, &family);
        return result;
}

static inline int ifstatus(NET_family_t family, const void *status, size_t size)
{
        int result = -1;
        syscall(SYSCALL_NETIFSTATUS, &result, &family, status, &size);
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
static inline SOCKET *socket(NET_family_t family, NET_protocol_t protocol)
{
        return NULL;
}

//==============================================================================
/**
 * @brief  The function closes socket.
 *
 * @param  socket       Socket
 */
//==============================================================================
static inline void close_socket(SOCKET *socket)
{

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
 * @param localAddress          The pointer to the structure containing
 *                              the address to assign.
 *
 * @return On success 0 is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 */
//==============================================================================
static inline int bind(SOCKET *socket, const NET_addr_t *localAddress)
{
        return -1;
}

//==============================================================================
/**
 * @brief The function connect the socket to a specific remote IP address.
 *
 * @param socket        The socket
 * @param address       The pointer to the structure containing address
 *
 * @return On success 0 is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 */
//==============================================================================
static inline int connect(SOCKET *socket, const NET_addr_t *address)
{
        return -1;
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
static inline int disconnect(SOCKET *socket)
{
        return -1;
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
static inline int listen(SOCKET *socket)
{
        return -1;
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
static inline int accept(SOCKET *socket, SOCKET **new_socket)
{
        return -1;
}

//==============================================================================
/**
 * @brief  The function is used to receive messages from socket. recv()
 *         may be used only on a connected socket (see connect(), accept()).
 *
 * @param  socket       The socket from which to receive the data.
 * @param  buf          The buffer into which the received data is put.
 * @param  len          The buffer length.
 * @param  flags        Flags parameters that can be OR'ed together.
 *
 * @return Number of bytes actually received from the socket, or -1 on error and
 *         @ref errno value is set appropriately.
 */
//==============================================================================
static inline int recv(SOCKET *socket, void *buf, size_t len, NET_flags_t flags)
{
        return -1;
}

//==============================================================================
/**
 * @brief  The function is used to receive messages from another socket.
 *         recvfrom() may be used to receive data on a socket whether it is in
 *         a connected state or not but not on a TCP socket.
 *
 * @param  socket       The socket descriptor from which to receive the data.
 * @param  buf          The buffer into which the received data is put.
 * @param  len          The buffer length.
 * @param  flags        Flags parameters that can be OR'ed together.
 * @param  from_addr    The socket from which the data is (or to be) received.
 *
 * @return Number of bytes actually received from the socket, or -1 on error and
 *         @ref errno value is set appropriately.
 */
//==============================================================================
static inline int recvfrom(SOCKET           *socket,
                           char             *buf,
                           size_t            len,
                           NET_flags_t       flags,
                           const NET_addr_t *from_addr)
{
        return -1;
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
 * @param  to_addr      The address to send the data to.
 *
 * @return Number of bytes actually sent on the socket, or -1 on error and
 *         @ref errno value is set appropriately.
 */
//==============================================================================
static inline int sendto(SOCKET           *socket,
                         const void       *buf,
                         size_t            len,
                         NET_flags_t       flags,
                         const NET_addr_t *to_addr)
{
        return -1;
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
 *
 * @return Number of bytes actually sent on the socket, or -1 on error and
 *         @ref errno value is set appropriately.
 */
//==============================================================================
static inline int send(SOCKET *socket, const void *buf, size_t len, NET_flags_t flags)
{
        return -1;
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static inline int shutdown(SOCKET *socket, NET_shut_t how)
{
        return -1;
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static inline int get_host_by_name(const char *name, NET_addr_t *addr)
{
        return -1;
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static inline int set_receive_timeout(SOCKET *socket, uint32_t timeout)
{
        return -1;
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static inline int set_send_timeout(SOCKET *socket, uint32_t timeout)
{
        return -1;
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static inline int get_address(SOCKET *socket, NET_addr_t *addr)
{
        return -1;
}

#ifdef __cplusplus
}
#endif

#endif /* _DNX_NET_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
