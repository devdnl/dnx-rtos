/*=========================================================================*//**
@file    net.h

@author  Daniel Zorychta

@brief   This file provide network API.

@note    Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/**
\defgroup dnx-net-h <dnx/net.h>

Network handling library.

@section net_code_example Code examples
@subsection net_code_example_if Network configuration
@code
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <dnx/net.h>
#include <dnx/misc.h>

GLOBAL_VARIABLES_SECTION {
};

static const NET_INET_config_t DHCP_UP = {
        .mode    = NET_INET_MODE__DHCP_START,
        .address = NET_INET_IPv4_ANY,   // ignored
        .mask    = NET_INET_IPv4_ANY,   // ignored
        .gateway = NET_INET_IPv4_ANY    // ignored
};

int_main(init, STACK_DEPTH_LOW, int argc, char *argv[])
{
        // create a new network interface
        if (ifadd("inet", NET_FAMILY__INET, "/dev/eth") != 0) {
                perror("ifadd");
                return EXIT_FAILURE;
        }

        // start DHCP client
        if (ifup("inet", &DHCP_UP) != 0) {
                perror("ifup");
                return EXIT_FAILURE;
        }

        // get connection status
        NET_INET_status_t stat;
        NET_family_t family;
        if (ifstatus("inet", &family, &stat) != 0) {
                perror("ifstatus");
                return EXIT_FAILURE;
        } else {
                // print connection status
                printf("");
        }


        // shutdown network
        ifdown("inet");

        return EXIT_SUCCESS;
}
@endcode

@subsection net_code_example_server Simple server implementation
@code
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <dnx/net.h>
#include <dnx/misc.h>

GLOBAL_VARIABLES_SECTION {
        char buf[128];
};

static const char HELLO[] = "I'm server!";

static const NET_INET_sockaddr_t ADDR_ANY = {
        .addr = NET_INET_IPv4_ANY,
        .port = 8080
};

int_main(server, STACK_DEPTH_LOW, int argc, char *argv[])
{
        UNUSED_ARG2(argc, argv);

        SOCKET *socket = socket_open("inet", NET_PROTOCOL__TCP);
        if (socket) {
                if (socket_bind(socket, &ADDR_ANY) == 0) {
                        if (socket_listen(socket) == 0) {
                                SOCKET *new_socket;
                                while (socket_accept(socket, &new_socket) == 0) {
                                        // receive string from client
                                        int sz = socket_recv(new_socket,
                                                             global->buf,
                                                             sizeof(global->buf),
                                                             NET_FLAGS__NONE);
                                        if (sz < 0)
                                                break;

                                        // print received buffer
                                        printf("%.*s\n", sz, global->buf);

                                        // send response
                                        socket_send(new_socket,
                                                    HELLO,
                                                    strlen(HELLO),
                                                    NET_FLAGS__NOCOPY);

                                        socket_close(new_socket);
                                }
                        }
                }

                socket_close(socket);
        }

        if (errno != 0) {
                perror("Server");
        }

        return EXIT_SUCCESS;
}
@endcode

@subsection net_code_example_client Simple client implementation
@code
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <dnx/net.h>
#include <dnx/misc.h>

GLOBAL_VARIABLES_SECTION {
        char buf[128];
};

static const char HELLO[] = "I'm client!";

static const NET_INET_sockaddr_t SERVER_ADDR = {
        .addr = NET_INET_IPv4(192.168.0.100),
        .port = 8080
};

int_main(client, STACK_DEPTH_LOW, int argc, char *argv[])
{
        SOCKET *socket = socket_open("inet", NET_PROTOCOL__TCP);
        if (socket) {
                socket_set_send_timeout(socket, 2000);
                socket_set_recv_timeout(socket, 2000);

                do {
                        if (socket_connect(socket, &SERVER_ADDR) == 0) {

                                // send string to server
                                int sz = socket_write(socket, HELLO, strlen(HELLO));
                                if (sz <= 0)
                                        break;

                                // read string from server
                                sz = socket_read(socket, global->buf, sizeof(global->buf));
                                if (sz <= 0)
                                        break;

                                // print message from server
                                printf("%s.*s\n", sz, global->buf);
                        }
                } while (0);

                socket_close(socket);
        }

        if (errno != 0) {
                perror("Client");
        }

        return EXIT_SUCCESS;
}
@endcode

*/
/**@{*/

#ifndef _LIBC_NET_H_
#define _LIBC_NET_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <stdint.h>
#include <libc/source/syscall.h>
#include <stddef.h>
#include <net/netm.h>   // to remove in future
#include <dnx/misc.h>
#include <errno.h>

/*==============================================================================
  Exported macros
==============================================================================*/

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
 * @brief  Function add new network interface.
 *
 * @param  netname      network name
 * @param  family       interface family
 * @param  if_path      interface path
 *
 * @return On success 0 is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 *
 * @see ifrm(), ifstatus(), iflist()
 */
//==============================================================================
static inline int ifadd(const char *netname, NET_family_t family, const char *if_path)
{
        int result = -1;
        _libc_syscall(_LIBC_SYS_NETADD, &result, netname, &family, if_path);
        return result;
}

//==============================================================================
/**
 * @brief  Function remove network interface.
 *
 * @param  netname      network name
 *
 * @return On success 0 is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 *
 * @see ifrm(), ifstatus(), iflist()
 */
//==============================================================================
static inline int ifrm(const char *netname)
{
        int result = -1;
        _libc_syscall(_LIBC_SYS_NETRM, &result, netname);
        return result;
}

//==============================================================================
/**
 * @brief  Function list all networks.
 *
 * @param  netname      network name
 * @param  netname_len  network name buffer length
 *
 * @return On success 0 is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 *
 * @see ifadd(), ifrm()
 */
//==============================================================================
static inline int iflist(char *netname[], size_t netname_len)
{
        int result = -1;
        _libc_syscall(_LIBC_SYS_NETIFLIST, &result, netname, &netname_len);
        return result;
}

//==============================================================================
/**
 * @brief  Function set up selected network interface.
 *
 * @param  netname      network name
 * @param  config       configuration for specified interface
 *
 * @return On success 0 is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 *
 * @see ifdown(), ifstatus()
 */
//==============================================================================
static inline int ifup(const char *netname, const NET_generic_config_t *config)
{
        int result = -1;
        _libc_syscall(_LIBC_SYS_NETIFUP, &result, netname, config);
        return result;
}

//==============================================================================
/**
 * @brief  Function set down selected network interface.
 *
 * @param  netname      network name
 *
 * @return On success 0 is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 *
 * @see ifup(), ifstatus()
 */
//==============================================================================
static inline int ifdown(const char *netname)
{
        int result = -1;
        _libc_syscall(_LIBC_SYS_NETIFDOWN, &result, netname);
        return result;
}

//==============================================================================
/**
 * @brief  Function get status of selected interface.
 *
 * @param  netname      network name
 * @param  family       interface family
 * @param  status       status of selected interface
 *
 * @return On success 0 is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 *
 * @see ifup(), ifdown()
 */
//==============================================================================
static inline int ifstatus(const char *netname, NET_family_t *family, NET_generic_status_t *status)
{
        int result = -1;
        _libc_syscall(_LIBC_SYS_NETIFSTATUS, &result, netname, family, status);
        return result;
}


//==============================================================================
/**
 * @brief Creates an endpoint for communication and returns a socket.
 *
 * @param netname       Network name.
 * @param protocol      The protocol name.
 *
 * @return On error @b NULL is returned and @ref errno value is set appropriately,
 *         otherwise new socket pointer.
 *
 * @see socket_close()
 */
//==============================================================================
static inline SOCKET *socket_open(const char *netname, NET_protocol_t protocol)
{
        SOCKET *socket = NULL;
        _libc_syscall(_LIBC_SYS_NETSOCKETCREATE, &socket, netname, &protocol);
        return socket;
}

//==============================================================================
/**
 * @brief  The function close socket in both directions.
 *
 * @param  socket       The socket to close.
 *
 * @return On success 0 is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 *
 * @see socket_shutdown(), socket_open()
 */
//==============================================================================
static inline int socket_close(SOCKET *socket)
{
        _libc_syscall(_LIBC_SYS_NETSOCKETDESTROY, NULL, socket);
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
static inline int socket_bind(SOCKET *socket, const NET_generic_sockaddr_t *sockAddr)
{
        int result = -1;
        _libc_syscall(_LIBC_SYS_NETBIND, &result, socket, sockAddr);
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
 *
 * @see socket_disconnect()
 */
//==============================================================================
static inline int socket_connect(SOCKET *socket, const NET_generic_sockaddr_t *sockAddr)
{
        int result = -1;
        _libc_syscall(_LIBC_SYS_NETCONNECT, &result, socket, sockAddr);
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
 *
 * @see socket_connect()
 */
//==============================================================================
static inline int socket_disconnect(SOCKET *socket)
{
        int result = -1;
        _libc_syscall(_LIBC_SYS_NETDISCONNECT, &result, socket);
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
 *
 * @see socket_accept(), socket_bind()
 */
//==============================================================================
static inline int socket_listen(SOCKET *socket)
{
        int result = -1;
        _libc_syscall(_LIBC_SYS_NETLISTEN, &result, socket);
        return result;
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
static inline int socket_accept(SOCKET *socket, SOCKET **new_socket)
{
        int result = -1;
        _libc_syscall(_LIBC_SYS_NETACCEPT, &result, socket, new_socket);
        return result;
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
static inline int socket_recv(SOCKET *socket, void *buf, size_t len, NET_flags_t flags)
{
        int result = -1;
        _libc_syscall(_LIBC_SYS_NETRECV, &result, socket, buf, &len, &flags);
        return result;
}

//==============================================================================
/**
 * @brief  The function is used to receive messages from socket. socket_read()
 *         may be used only on a connected socket.
 *         The socket_read() is equivalent to socket_recv() with flags
 *         set to @ref NET_FLAGS__NONE.
 *
 * @param  socket       The socket from which to receive the data.
 * @param  buf          The buffer into which the received data is put.
 * @param  len          The buffer length.
 *
 * @return Number of bytes actually received from the socket, or -1 on error and
 *         @ref errno value is set appropriately.
 *
 * @see socket_connect(), socket_accept(), socket_recv(), socket_recvfrom()
 */
//==============================================================================
static inline int socket_read(SOCKET *socket, void *buf, size_t len)
{
        return socket_recv(socket, buf, len, NET_FLAGS__NONE);
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
static inline int socket_recvfrom(SOCKET                 *socket,
                                  char                   *buf,
                                  size_t                  len,
                                  NET_flags_t             flags,
                                  NET_generic_sockaddr_t *from_sockaddr)
{
        int result = -1;
        _libc_syscall(_LIBC_SYS_NETRECVFROM, &result, socket, buf, &len, &flags, from_sockaddr);
        return result;
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
static inline int socket_send(SOCKET *socket, const void *buf, size_t len, NET_flags_t flags)
{
        int result = -1;
        _libc_syscall(_LIBC_SYS_NETSEND, &result, socket, buf, &len, &flags);
        return result;
}

//==============================================================================
/**
 * @brief  The function is used to transmit a message to another transport
 *         end-point. socket_write() may be used only when the socket is in a connected
 *         state. The socket_write() is equivalent to socket_send() with flags
 *         set to @ref NET_FLAGS__NONE.
 *
 * @param  socket       The socket to use to send the data.
 * @param  buf          A pointer to the buffer to send.
 * @param  len          The length of the buffer to send.
 *
 * @return Number of bytes actually sent on the socket, or -1 on error and
 *         @ref errno value is set appropriately.
 *
 * @see socket_connect(), socket_send(), socket_write(), socket_sendto()
 */
//==============================================================================
static inline int socket_write(SOCKET *socket, const void *buf, size_t len)
{
        return socket_send(socket, buf, len, NET_FLAGS__NONE);
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
static inline int socket_sendto(SOCKET                       *socket,
                                const void                   *buf,
                                size_t                        len,
                                NET_flags_t                   flags,
                                const NET_generic_sockaddr_t *to_sockaddr)
{
        int result = -1;
        _libc_syscall(_LIBC_SYS_NETSENDTO, &result, socket, buf, &len, &flags, to_sockaddr);
        return result;
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
static inline int socket_shutdown(SOCKET *socket, NET_shut_t how)
{
        int result = -1;
        _libc_syscall(_LIBC_SYS_NETSHUTDOWN, &result, socket, &how);
        return result;
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
static inline int socket_set_recv_timeout(SOCKET *socket, uint32_t timeout)
{
        int result = -1;
        _libc_syscall(_LIBC_SYS_NETSETRECVTIMEOUT, &result, socket, &timeout);
        return result;
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
static inline int socket_set_send_timeout(SOCKET *socket, uint32_t timeout)
{
        int result = -1;
        _libc_syscall(_LIBC_SYS_NETSETSENDTIMEOUT, &result, socket, &timeout);
        return result;
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
static inline int socket_get_address(SOCKET *socket, NET_generic_sockaddr_t *addr)
{
        int result = -1;
        _libc_syscall(_LIBC_SYS_NETGETADDRESS, &result, socket, addr);
        return result;
}

//==============================================================================
/**
 * @brief  The function gets host address by its name.
 *
 * @param  netname      Network name.
 * @param  name         Host name.
 * @param  sock_addr    Obtained host address.
 *
 * @return On success 0 is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 */
//==============================================================================
static inline int get_host_by_name(const char             *netname,
                                   const char             *name,
                                   NET_generic_sockaddr_t *sock_addr)
{
        int result = -1;
        _libc_syscall(_LIBC_SYS_NETGETHOSTBYNAME, &result, netname, name, sock_addr);
        return result;
}

//==============================================================================
/**
 * @brief  The function converts host byte order to network.
 *
 * @param  family       Network family.
 * @param  value        Value to convert.
 *
 * @return Converted value.
 *
 * @see hton_u32(), hton_u64()
 */
//==============================================================================
static inline uint16_t hton_u16(NET_family_t family, uint16_t value)
{
        uint16_t r;
        _libc_syscall(_LIBC_SYS_NETHTON16, &r, &family, &value);
        return r;
}

//==============================================================================
/**
 * @brief  The function converts host byte order to network.
 *
 * @param  family       Network family.
 * @param  value        Value to convert.
 *
 * @return Converted value.
 *
 * @see hton_u16(), hton_u64()
 */
//==============================================================================
static inline uint32_t hton_u32(NET_family_t family, uint32_t value)
{
        uint32_t r;
        _libc_syscall(_LIBC_SYS_NETHTON32, &r, &family, &value);
        return r;
}

//==============================================================================
/**
 * @brief  The function converts host byte order to network.
 *
 * @param  family       Network family.
 * @param  value        Value to convert.
 *
 * @return Converted value.
 *
 * @see hton_u16(), hton_u32()
 */
//==============================================================================
static inline uint64_t hton_u64(NET_family_t family, uint64_t value)
{
        uint64_t r;
        _libc_syscall(_LIBC_SYS_NETHTON64, &r, &family, &value);
        return r;
}

//==============================================================================
/**
 * @brief  The function converts network byte order to host.
 *
 * @param  family       Network family.
 * @param  value        Value to convert.
 *
 * @return Converted value.
 *
 * @see ntoh_u32(), ntoh_u64()
 */
//==============================================================================
static inline uint16_t ntoh_u16(NET_family_t family, uint16_t value)
{
        return hton_u16(family, value);
}

//==============================================================================
/**
 * @brief  The function converts network byte order to host.
 *
 * @param  family       Network family.
 * @param  value        Value to convert.
 *
 * @return Converted value.
 *
 * @see ntoh_u16(), ntoh_u64()
 */
//==============================================================================
static inline uint32_t ntoh_u32(NET_family_t family, uint32_t value)
{
        return hton_u32(family, value);
}

//==============================================================================
/**
 * @brief  The function converts network byte order to host.
 *
 * @param  family       Network family.
 * @param  value        Value to convert.
 *
 * @return Converted value.
 *
 * @see ntoh_u16(), ntoh_u32()
 */
//==============================================================================
static inline uint64_t ntoh_u64(NET_family_t family, uint64_t value)
{
        return hton_u64(family, value);
}

#ifdef __cplusplus
}
#endif

#endif /* _DNX_NET_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
