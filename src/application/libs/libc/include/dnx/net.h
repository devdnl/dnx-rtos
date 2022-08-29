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
        if (ifup("inet", &DHCP_UP, sizeof(DHCP_UP) != 0) {
                perror("ifup");
                return EXIT_FAILURE;
        }

        // get connection status
        NET_INET_status_t stat;
        NET_family_t family;
        if (ifstatus("inet", &family, &stat, sizeof(stat) != 0) {
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
                if (socket_bind(socket, &ADDR_ANY, sizeof(ADDR_ANY)) == 0) {
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
                        if (socket_connect(socket, &SERVER_ADDR, sizeof(SERVER_ADDR) == 0) {

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
#include <dnx/misc.h>
#include <errno.h>

/*==============================================================================
  Exported macros
==============================================================================*/
/*------------------------------------------------------------------------------
  INET NETWORK FAMILY
------------------------------------------------------------------------------*/
/** Macro creates IPv4 address for INET family network. */
#define NET_INET_IPv4(a, b, c, d)               (((a & 0xFF) << 24) | ((b & 0xFF) << 16) | ((c & 0xFF) << 8) | ((d & 0xFF)))

/** Macro creates ANY address for INET family network. */
#define NET_INET_IPv4_ANY                       NET_INET_IPv4(0,0,0,0)

/** Macro creates LOOPBACK address for INET family network. */
#define NET_INET_IPv4_LOOPBACK                  NET_INET_IPv4(127,0,0,1)

/** Macro creates BROADCAST address for INET family network. */
#define NET_INET_IPv4_BROADCAST                 NET_INET_IPv4(255,255,255,255)

/** Macro gets part <i>a</i> of INET family network address. */
#define NET_INET_IPv4_a(ip)                     ((uint8_t)((ip >> 24) & 0xFF))

/** Macro gets part <i>b</i> of INET family network address. */
#define NET_INET_IPv4_b(ip)                     ((uint8_t)((ip >> 16) & 0xFF))

/** Macro gets part <i>c</i> of INET family network address. */
#define NET_INET_IPv4_c(ip)                     ((uint8_t)((ip >> 8)  & 0xFF))

/** Macro gets part <i>d</i> of INET family network address. */
#define NET_INET_IPv4_d(ip)                     ((uint8_t)((ip >> 0)  & 0xFF))

/*------------------------------------------------------------------------------
  SIPC NETWORK FAMILY
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  CANNET NETWORK FAMILY
------------------------------------------------------------------------------*/
#define NET_CANNET_MAX_PORT             7
#define NET_CANNET_CAN_ID_MASK          0x10003FFF
#define NET_CANNET_ADDR_BROADCAST       0x3FFF
#define NET_CANNET_ADDR_ANY             0x0000

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct _libc_socket SOCKET;

/*------------------------------------------------------------------------------
  GENERIC NETWORK
------------------------------------------------------------------------------*/
/** Network family. */
typedef enum {
        NET_FAMILY__INET,                       //!< Internet network.
        NET_FAMILY__SIPC,                       //!< Serial Inter-Processor Communication
        NET_FAMILY__CANNET,                     //!< CAN network Communication
        _NET_FAMILY__COUNT
} NET_family_t;

/** Protocol selection. Not all protocols are available for all family networks. */
typedef enum {
        NET_PROTOCOL__DATAGRAM,                         //!< DATAGRAM type protocol
        NET_PROTOCOL__STREAM,                           //!< STREAM type protocol
        NET_PROTOCOL__UDP = NET_PROTOCOL__DATAGRAM,     //!< UDP protocol.
        NET_PROTOCOL__TCP = NET_PROTOCOL__STREAM,       //!< TCP protocol.
        _NET_PROTOCOL__COUNT,                           //!< number of protocols
} NET_protocol_t;

/** Control flags. */
typedef enum {
        NET_FLAGS__NONE      = 0,               //!< Flags not set.
        NET_FLAGS__NOCOPY    = (1 << 0),        //!< Buffer is not internally copy.
        NET_FLAGS__COPY      = (1 << 1),        //!< Buffer is internally copy.
        NET_FLAGS__MORE      = (1 << 2),        //!< More transfers.
        NET_FLAGS__REWIND    = (1 << 3),        //!< Read stream index is rewind.
        NET_FLAGS__FREEBUF   = (1 << 4),        //!< Skip unread bytes after read and free buffer.
} NET_flags_t;

/** Socket shutdown direction. */
typedef enum {
        NET_SHUT__RD   = (1 << 0),              //!< Shutdown read direction.
        NET_SHUT__WR   = (1 << 1),              //!< Shutdown write direction.
        NET_SHUT__RDWR = (NET_SHUT__RD | NET_SHUT__WR)  //!< Shutdown both directions.
} NET_shut_t;

/** Generic socket address. This type accept all network family addresses. */
typedef void NET_generic_sockaddr_t;

/** Generic interface configuration. This type accept all network family configurations. */
typedef void NET_generic_config_t;

/** Generic network status. This type accept all network family statuses. */
typedef void NET_generic_status_t;

/*------------------------------------------------------------------------------
  INET NETWORK FAMILY
------------------------------------------------------------------------------*/
/** INET network address: IPv4. */
typedef uint32_t NET_INET_IPv4_t;

/** INET socket address. This type contains IPv4 address and port. */
typedef struct {
        NET_INET_IPv4_t addr;                   /*!< IPv4 address.*/
        u16_t           port;                   /*!< Port.*/
} NET_INET_sockaddr_t;

/** INET network state. */
typedef enum {
        NET_INET_STATE__NOT_CONFIGURED,         //!< Network not configured.
        NET_INET_STATE__STATIC_IP,              //!< Static IP configuration.
        NET_INET_STATE__DHCP_CONFIGURING,       //!< DHCP address is configuring.
        NET_INET_STATE__DHCP_CONFIGURED,        //!< DHCP address configured.
        NET_INET_STATE__LINK_DISCONNECTED       //!< Network interface not connected.
} NET_INET_state_t;

/** INET configuration mode. */
typedef enum {
        NET_INET_MODE__STATIC,                  //!< Configure static IP.
        NET_INET_MODE__DHCP_START,              //!< Start DHCP client.
        NET_INET_MODE__DHCP_INFORM,             //!< Inform DHCP server.
        NET_INET_MODE__DHCP_RENEW,              //!< Renew DHCP connection.
} NET_INET_mode_t;

/** INET configuration. */
typedef struct {
        NET_INET_mode_t mode;                   /*!< Configuration mode.*/
        NET_INET_IPv4_t address;                /*!< Address if static mode selected.*/
        NET_INET_IPv4_t mask;                   /*!< Network mask if static mode selected.*/
        NET_INET_IPv4_t gateway;                /*!< Gateway address if static mode selected.*/
} NET_INET_config_t;

/** INET status. */
typedef struct {
        NET_INET_state_t state;                 /*!< Connection state.*/
        NET_INET_IPv4_t  address;               /*!< Connection address.*/
        NET_INET_IPv4_t  mask;                  /*!< Connection network mask.*/
        NET_INET_IPv4_t  gateway;               /*!< Connection gateway.*/
        u8_t             hw_addr[6];            /*!< MAC address.*/
        u64_t            tx_bytes;              /*!< Number of transmitted bytes.*/
        u64_t            rx_bytes;              /*!< Number of received bytes.*/
        u64_t            tx_packets;            /*!< Number of transmitted packets.*/
        u64_t            rx_packets;            /*!< Number of received packets.*/
} NET_INET_status_t;

/*------------------------------------------------------------------------------
  SIPC NETWORK FAMILY
------------------------------------------------------------------------------*/
/** SIPC network state. */
typedef enum {
        NET_SIPC_STATE__DOWN,                   //!< Network not configured.
        NET_SIPC_STATE__UP,                     //!< Network configured.
} NET_SIPC_state_t;

/** SIPC configuration. */
typedef struct {
        u16_t MTU;                              //!< Maximal Transfer Unit
} NET_SIPC_config_t;

/** SIPC status. */
typedef struct {
        NET_SIPC_state_t state;                 /*!< Connection state.*/
        u16_t            MTU;                   /*!< Transfer size. */
        u64_t            tx_bytes;              /*!< Number of transmitted bytes.*/
        u64_t            rx_bytes;              /*!< Number of received bytes.*/
        u64_t            tx_packets;            /*!< Number of transmitted packets.*/
        u64_t            rx_packets;            /*!< Number of received packets.*/
} NET_SIPC_status_t;

/** SIPC socket address. */
typedef struct {
        u8_t             port;                   /*!< Port.*/
} NET_SIPC_sockaddr_t;


/*------------------------------------------------------------------------------
  CANNET NETWORK FAMILY
------------------------------------------------------------------------------*/
/** CANNET network state. */
typedef enum {
        NET_CANNET_STATE__DOWN,                 //!< Network not configured.
        NET_CANNET_STATE__UP,                   //!< Network configured.
} NET_CANNET_state_t;

/** CANNET configuration. */
typedef struct {
        u16_t             addr;                 //!< device address.
} NET_CANNET_config_t;

/** CANNET status. */
typedef struct {
        NET_CANNET_state_t state;               /*!< Connection state.*/
        u16_t              addr;                /*!< Address.*/
        u16_t              MTU;                 /*!< Transfer size. */
        u64_t              tx_bytes;            /*!< Number of transmitted bytes.*/
        u64_t              rx_bytes;            /*!< Number of received bytes.*/
        u64_t              tx_packets;          /*!< Number of transmitted packets.*/
        u64_t              rx_packets;          /*!< Number of received packets.*/
} NET_CANNET_status_t;

/** SIPC socket address. */
typedef struct {
        u16_t addr;                             /*!< Address.*/
        u8_t  port;                             /*!< Port.*/
} NET_CANNET_sockaddr_t;

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
        int err = _libc_syscall(_LIBC_SYS_NETADD, netname, &family, if_path);
        return err ? -1 : 0;
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
        int err = _libc_syscall(_LIBC_SYS_NETRM, netname);
        return err ? -1 : 0;
}

//==============================================================================
/**
 * @brief  Function list all networks.
 *
 * @param  netname      network name
 * @param  netname_len  network name buffer length
 *
 * @return On success number of network is returned, otherwise -1 and @ref errno
 *         value is set appropriately.
 *
 * @see ifadd(), ifrm()
 */
//==============================================================================
static inline int iflist(char *netname[], size_t netname_len)
{
        size_t count;
        int err = _libc_syscall(_LIBC_SYS_NETIFLIST, netname, &netname_len, &count);
        return err ? -1 : (int)count;
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
static inline int ifup(const char *netname, const NET_generic_config_t *config, size_t config_size)
{
        int err = _libc_syscall(_LIBC_SYS_NETIFUP, netname, config, &config_size);
        return err ? -1 : 0;
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
        int err = _libc_syscall(_LIBC_SYS_NETIFDOWN, netname);
        return err ? -1 : 0;
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
static inline int ifstatus(const char *netname, NET_family_t *family, NET_generic_status_t *status, size_t status_size)
{
        int err = _libc_syscall(_LIBC_SYS_NETIFSTATUS, netname, family, status, &status_size);
        return err ? -1 : 0;
}

//==============================================================================
/**
 * @brief  Create new socket descriptor.
 *
 * @param  netname      network name
 * @param  protocol     protocol
 *
 * @return On success descriptor is returned, otherwise -1 and @ref errno value is set
 *         appropriately.
 *
 * @see socket_open()
 */
//==============================================================================
static inline int socket(const char *netname, NET_protocol_t protocol)
{
        int fd;
        int err = _libc_syscall(_LIBC_SYS_NETSOCKETCREATE, &fd, netname, protocol);
        return err ? errno : fd;
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
extern SOCKET *socket_open(const char *netname, NET_protocol_t protocol);

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
extern SOCKET *socket_fdopen(int fd);

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
extern int socket_fileno(SOCKET *socket);

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
extern int socket_close(SOCKET *socket);

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
extern int socket_bind(SOCKET *socket, const NET_generic_sockaddr_t *sockaddr, size_t sockaddr_size);

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
extern int socket_connect(SOCKET *socket, const NET_generic_sockaddr_t *sockaddr, size_t sockaddr_size);

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
extern int socket_disconnect(SOCKET *socket);

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
extern int socket_listen(SOCKET *socket);

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
extern int socket_accept(SOCKET *socket, SOCKET **new_socket);

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
extern int socket_recv(SOCKET *socket, void *buf, size_t len, NET_flags_t flags);

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
extern int socket_recvfrom(SOCKET                 *socket,
                           char                   *buf,
                           size_t                  len,
                           NET_flags_t             flags,
                           NET_generic_sockaddr_t *from_sockaddr,
                           size_t                  from_sockaddr_size);

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
extern int socket_send(SOCKET *socket, const void *buf, size_t len, NET_flags_t flags);

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
        return socket_send(socket, buf, len, NET_FLAGS__COPY);
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
extern int socket_sendto(SOCKET                       *socket,
                         const void                   *buf,
                         size_t                        len,
                         NET_flags_t                   flags,
                         const NET_generic_sockaddr_t *to_sockaddr,
                         size_t                        to_sockaddr_size);

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
extern int socket_shutdown(SOCKET *socket, NET_shut_t how);

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
extern int socket_set_recv_timeout(SOCKET *socket, uint32_t timeout);

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
extern int socket_set_send_timeout(SOCKET *socket, uint32_t timeout);

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
extern int socket_get_address(SOCKET *socket, NET_generic_sockaddr_t *addr, size_t addr_size);

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
                                   NET_generic_sockaddr_t *sock_addr,
                                   size_t                  sock_addr_size)
{
        int err = _libc_syscall(_LIBC_SYS_NETGETHOSTBYNAME, netname, name, sock_addr, &sock_addr_size);
        return err ? -1 : 0;
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
        uint16_t out;
        int err = _libc_syscall(_LIBC_SYS_NETHTON16, &family, &value, &out);
        return err ? value : out;
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
        uint32_t out;
        int err = _libc_syscall(_LIBC_SYS_NETHTON32, &family, &value, &out);
        return err ? value : out;
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
        uint64_t out;
        int err = _libc_syscall(_LIBC_SYS_NETHTON64, &family, &value, &out);
        return err ? value : out;
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
