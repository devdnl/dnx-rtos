/*==============================================================================
File     socket.c

Author   Daniel Zorychta

Brief    Socket implementation.

	 Copyright (C) 2022 Daniel Zorychta <daniel.zorychta@gmail.com>

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


==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include <stddef.h>
#include <stdarg.h>
#include <dnx/net.h>
#include "syscall.h"
#include "common.h"

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/

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
 * @brief Creates an endpoint for communication and returns a socket.
 *
 * @param netname       Network name.
 * @param protocol      The protocol name.
 *
 * @return On error @b NULL is returned and @ref errno value is set appropriately,
 *         otherwise new socket pointer.
 */
//==============================================================================
SOCKET *socket_open(const char *netname, NET_protocol_t protocol)
{
        SOCKET *socket = _libc_malloc(sizeof(*socket));
        if (socket) {
                int err = _libc_syscall(_LIBC_SYS_NETSOCKETCREATE, &socket->fd, netname, &protocol);
                if (!err) {
                        return socket;
                }

                _libc_free(socket);
        }

        return NULL;
}

//==============================================================================
/**
 * @brief Creates an endpoint for communication and returns a socket according
 *        to given descriptor.
 *
 * @param fd    socket descriptor
 *
 * @return On error @b NULL is returned and @ref errno value is set appropriately,
 *         otherwise new socket pointer.
 *
 * @see socket_close(), socket_open(), socket()
 */
//==============================================================================
SOCKET *socket_fdopen(int fd)
{
        if (fd < 0) {
                errno = EINVAL;
                return NULL;
        }

        SOCKET *socket = _libc_malloc(sizeof(*socket));
        if (socket) {
                socket->fd = fd;
        }

        return socket;
}

//==============================================================================
/**
 * @brief Return socket descriptor from SOCKET object.
 *
 * @param socket        socket object
 *
 * @return On error @b -1 is returned and @ref errno value is set appropriately,
 *         otherwise socket descriptor.
 *
 * @see socket_close(), socket_open(), socket()
 */
//==============================================================================
int socket_fileno(SOCKET *socket)
{
        if (!socket) {
                errno = EINVAL;
                return -1;
        }

        return socket->fd;
}

//==============================================================================
/**
 * @brief  The function close socket in both directions.
 *
 * @param  socket       The socket to close.
 *
 * @return On success 0 is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 */
//==============================================================================
int socket_close(SOCKET *socket)
{
        if (!socket) {
                errno = EINVAL;
                return -1;
        }

        if (close(socket->fd) != 0) {
                return -1;
        }

        _libc_free(socket);
        return 0;
}

//==============================================================================
/**
 * @brief Assigns an address to the socket. When a socket is created with
 *        socket_open(), it exists in an address family space but has no address
 *        assigned. socket_bind() requests that the address pointed to by sockAddr be
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
int socket_bind(SOCKET *socket, const NET_generic_sockaddr_t *sockaddr, size_t sockaddr_size)
{
        if (!socket or !sockaddr or !sockaddr_size) {
                errno = EINVAL;
                return -1;
        }

        int err = _libc_syscall(_LIBC_SYS_NETBIND, &socket->fd, sockaddr, &sockaddr_size);
        return err ? -1 : 0;
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
 *
 * @see socket_disconnect()
 */
//==============================================================================
int socket_connect(SOCKET *socket, const NET_generic_sockaddr_t *sockaddr, size_t sockaddr_size)
{
        if (!socket or !sockaddr or !sockaddr_size) {
                errno = EINVAL;
                return -1;
        }

        int err = _libc_syscall(_LIBC_SYS_NETCONNECT, &socket->fd, sockaddr, &sockaddr_size);
        return err ? -1 : 0;
}

//==============================================================================
/**
 * @brief The function disconnect the socket from current connection.
 *
 * @param socket        The socket
 *
 * @return On success 0 is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 *
 * @see socket_connect()
 */
//==============================================================================
int socket_disconnect(SOCKET *socket)
{
        if (!socket) {
                errno = EINVAL;
                return -1;
        }

        int err = _libc_syscall(_LIBC_SYS_NETDISCONNECT, &socket->fd);
        return err ? -1 : 0;
}

//==============================================================================
/**
 * @brief The function set a connection into listen mode.
 *
 * @param socket        The socket
 *
 * @return On success 0 is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 *
 * @see socket_accept(), socket_bind()
 */
//==============================================================================
int socket_listen(SOCKET *socket)
{
        if (!socket) {
                errno = EINVAL;
                return -1;
        }

        int err = _libc_syscall(_LIBC_SYS_NETLISTEN, &socket->fd);
        return err ? -1 : 0;
}

//==============================================================================
/**
 * @brief The function accept a new connection and create new socket.
 *
 * @param socket        The listen connection
 * @param new_socket    The new connection
 *
 * @return On error -1 is returned end @ref errno value is set appropriately,
 *         otherwise 0 and new socket is set.
 *
 * @see socket_listen(), socket_bind()
 */
//==============================================================================
int socket_accept(SOCKET *socket, SOCKET **new_socket)
{
        if (!socket or !new_socket) {
                errno = EINVAL;
                return -1;
        }

        int err = -1;
        SOCKET *nsocket = _libc_malloc(sizeof(*nsocket));
        if (nsocket) {
                err = _libc_syscall(_LIBC_SYS_NETACCEPT, &socket->fd, &nsocket->fd);
                if (!err) {
                        *new_socket = nsocket;
                } else {
                        _libc_free(nsocket);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  The function is used to receive messages from socket. socket_recv()
 *         may be used only on a connected socket.
 *
 * @param  socket       The socket from which to receive the data.
 * @param  buf          The buffer into which the received data is put.
 * @param  len          The buffer length.
 * @param  flags        Flags parameters that can be OR'ed together.
 *
 * @return Number of bytes actually received from the socket, or -1 on error and
 *         @ref errno value is set appropriately.
 *
 * @see socket_connect(), socket_accept(), socket_recvfrom(), socket_read()
 */
//==============================================================================
int socket_recv(SOCKET *socket, void *buf, size_t len, NET_flags_t flags)
{
        if (!socket or !buf or !len) {
                errno = EINVAL;
                return -1;
        }

        size_t rcved;
        int err = _libc_syscall(_LIBC_SYS_NETRECV, &socket->fd, buf, &len, &flags, &rcved);
        return err ? -1 : (int)rcved;
}

//==============================================================================
/**
 * @brief  The function is used to receive messages from another socket.
 *         socket_recvfrom() may be used to receive data on a socket whether it is in
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
 *
 * @see socket_connect(), socket_accept(), socket_recv(), socket_read()
 */
//==============================================================================
int socket_recvfrom(SOCKET                 *socket,
                    char                   *buf,
                    size_t                  len,
                    NET_flags_t             flags,
                    NET_generic_sockaddr_t *from_sockaddr,
                    size_t                  from_sockaddr_size)
{
        if (!socket or !buf or !len or !from_sockaddr or !from_sockaddr_size) {
                errno = EINVAL;
                return -1;
        }

        size_t rcved;
        int err = _libc_syscall(_LIBC_SYS_NETRECVFROM, &socket->fd, buf, &len, &flags,
                                from_sockaddr, &from_sockaddr_size, &rcved);
        return err ? -1 : (int)rcved;
}

//==============================================================================
/**
 * @brief  The function is used to transmit a message to another transport
 *         end-point. socket_send() may be used only when the socket is in
 *         a connected state.
 *
 * @param  socket       The socket to use to send the data.
 * @param  buf          A pointer to the buffer to send.
 * @param  len          The length of the buffer to send.
 * @param  flags        Flags parameters that can be OR'ed together.
 *
 * @return Number of bytes actually sent on the socket, or -1 on error and
 *         @ref errno value is set appropriately.
 *
 * @see socket_connect(), socket_sendto(), socket_write()
 */
//==============================================================================
int socket_send(SOCKET *socket, const void *buf, size_t len, NET_flags_t flags)
{
        if (!socket or !buf or !len) {
                errno = EINVAL;
                return -1;
        }

        size_t sent;
        int err = _libc_syscall(_LIBC_SYS_NETSEND, &socket->fd, buf, &len, &flags, &sent);
        return err ? -1 : (int)sent;
}

//==============================================================================
/**
 * @brief  The function is used to transmit a message to another transport
 *         end-point. socket_sendto() may be used any time the socket is in the
 *         unconnected state, and never for TCP sockets.
 *
 * @param  socket       The socket to use to send the data.
 * @param  buf          A pointer to the buffer to send.
 * @param  len          The length of the buffer to send.
 * @param  flags        Flags parameters that can be OR'ed together.
 * @param  to_sockaddr  The address to send the data to.
 *
 * @return Number of bytes actually sent on the socket, or -1 on error and
 *         @ref errno value is set appropriately.
 *
 * @see socket_connect(), socket_send(), socket_write()
 */
//==============================================================================
int socket_sendto(SOCKET                       *socket,
                  const void                   *buf,
                  size_t                        len,
                  NET_flags_t                   flags,
                  const NET_generic_sockaddr_t *to_sockaddr,
                  size_t                        to_sockaddr_size)
{
        if (!socket or !buf or !len or !to_sockaddr or !to_sockaddr_size) {
                errno = EINVAL;
                return -1;
        }

        size_t sent;
        int err = _libc_syscall(_LIBC_SYS_NETSENDTO, &socket->fd, buf, &len, &flags,
                                to_sockaddr, &to_sockaddr_size, &sent);
        return err ? -1 : (int)sent;
}

//==============================================================================
/**
 * @brief  The function shutdown selected communication direction.
 *
 * @param  socket       The socket to shutdown.
 * @param  how          The flags of direction to shutdown.
 *
 * @return On success 0 is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 *
 * @see socket_close()
 */
//==============================================================================
int socket_shutdown(SOCKET *socket, NET_shut_t how)
{
        if (!socket) {
                errno = EINVAL;
                return -1;
        }

        int err = _libc_syscall(_LIBC_SYS_NETSHUTDOWN, &socket->fd, &how);
        return err ? -1 : 0;
}

//==============================================================================
/**
 * @brief  The function set receive timeout value.
 *
 * @param  socket       The socket to set timeout.
 * @param  timeout      The timeout value.
 *
 * @return On success 0 is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 *
 * @see socket_set_send_timeout()
 */
//==============================================================================
int socket_set_recv_timeout(SOCKET *socket, uint32_t timeout)
{
        if (!socket) {
                errno = EINVAL;
                return -1;
        }

        int err = _libc_syscall(_LIBC_SYS_NETSETRECVTIMEOUT, &socket->fd, &timeout);
        return err ? -1 : 0;
}

//==============================================================================
/**
 * @brief  The function set send timeout value.
 *
 * @param  socket       The socket to set timeout.
 * @param  timeout      The timeout value.
 *
 * @return On success 0 is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 *
 * @see socket_set_recv_timeout()
 */
//==============================================================================
int socket_set_send_timeout(SOCKET *socket, uint32_t timeout)
{
        if (!socket) {
                errno = EINVAL;
                return -1;
        }

        int err = _libc_syscall(_LIBC_SYS_NETSETSENDTIMEOUT, &socket->fd, &timeout);
        return err ? -1 : 0;
}

//==============================================================================
/**
 * @brief  The function gets connection address of socket.
 *
 * @param  socket       The socket.
 * @param  addr         Obtained remote address.
 *
 * @return On success 0 is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 *
 * @see socket_connect(), socket_bind()
 */
//==============================================================================
int socket_get_address(SOCKET *socket, NET_generic_sockaddr_t *addr, size_t addr_size)
{
        if (!socket or !addr or !addr_size) {
                errno = EINVAL;
                return -1;
        }

        int err = _libc_syscall(_LIBC_SYS_NETGETADDRESS, &socket->fd, addr, &addr_size);
        return err ? -1 : 0;
}

/*==============================================================================
  End of file
==============================================================================*/
