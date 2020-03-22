/*==============================================================================
File     sipc.c

Author   Daniel Zorychta

Brief    SIPC Network management.

         Copyright (C) 2019 Daniel Zorychta <daniel.zorychta@gmail.com>

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


==============================================================================*/

/*
HOST                   CLIENT

Handshake communication:
HANDSHAKE -----------> (port opened)
(ok)      <----------- ACK

HANDSHAKE -----------> (port not opened)
(error)   <----------- NACK

BIND      -----------> (no action)

Data send (ACK):
DATA      -----------> buffer (empty) -> store
(ok)      <----------- ACK

Data send (NACK) - port not exists:
DATA      -----------> (port not opened)
(error)   <----------- NACK

Data send (BUSY) - buffer full:
DATA      -----------> (port opened) buffer (full capacity) -> store
(wait)    <----------- BUSY (buffer exceeds capacity)
(ok)      <----------- ACK  (buffer ready for data receive)

Data send (REPEAT) - checksum incorrect:
DATA      -----------> checksum incorrect
(ok)      <----------- REPEAT
DATA      -----------> store data
(ok)      <----------- ACK

*/

/*==============================================================================
  Include files
==============================================================================*/
#include <stdbool.h>
#include "net/sipc/sipc.h"
#include "sipcbuf.h"
#include "cpuctl.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define PACKET_TYPE_DATA                0
#define PACKET_TYPE_HANDSHAKE           1
#define PACKET_TYPE_ACK                 2
#define PACKET_TYPE_NACK                3
#define PACKET_TYPE_BUSY                4
#define PACKET_TYPE_REPEAT              5
#define PACKET_TYPE_BIND                6
#define _PACKET_TYPE_COUNT              7

#define zalloc(_size, _pptr) _kzalloc(_MM_NET, _size, _pptr)
#define kalloc(_size, _pptr) _kmalloc(_MM_NET, _size, _pptr)
#define zfree(_pptr) _kfree(_MM_NET, _pptr)

#if __NETWORK_SIPC_DEBUG_ON__ > 0
#define DEBUG(...) printk("SIPC: "__VA_ARGS__)
#else
#define DEBUG(...)
#endif

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        u8_t  preamble[2];      /*!< Preamble */
        u8_t  checksum[2];      /*!< Fletcher16 Checksum */
        u16_t plen;             /*!< Payload size */
        u16_t seq;              /*!< Sequence */
        u8_t  port;             /*!< Port */
        u8_t  type;             /*!< Packet type */
        u8_t  payload[];        /*!< Payload */
} sipc_packet_t;

typedef struct {
        struct {
                u16_t MTU;
        } conf;

        struct {
                u64_t rx_packets;
                u64_t tx_packets;
                u64_t rx_bytes;
                u64_t tx_bytes;
        } stats;

        FILE *if_file;
        tid_t if_thread;
        NET_SIPC_state_t state;
        mutex_t *packet_send_mtx;
        mutex_t *socket_list_mtx;
        llist_t *socket_list;
        uint16_t seq_ctr;
} sipc_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void input_thread(void *sem);
static void clear_transfer_counters();
static void unregister_socket(SIPC_socket_t *socket);
static int  register_socket(SIPC_socket_t *socket);
static bool is_socket_registered(SIPC_socket_t *socket);
static int  send_packet(u16_t seq, u8_t port, u8_t type, const u8_t *payload, u16_t plen);
static SIPC_socket_t *get_socket_by_port(u8_t port);

/*==============================================================================
  External function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
static sipc_t *sipc;
static const u8_t PACKET_PREABLE[2] = {0xAA, 0x55};
static u32_t CONNECTION_TIMEOUT = 4000;

#if __NETWORK_SIPC_DEBUG_ON__ > 0
static const char *TYPE_STR[] = {"DATA", "HSK", "ACK", "NACK", "BUSY", "REPT", "BIND"};
#endif

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
static void clear_transfer_counters(void)
{
        sipc->stats.rx_bytes   = 0;
        sipc->stats.tx_bytes   = 0;
        sipc->stats.rx_packets = 0;
        sipc->stats.tx_packets = 0;
}

//==============================================================================
/**
 * @brief  Function compare sockets. Used in linked list.
 *
 * @param  a    socket a
 * @param  b    spclet b
 *
 * @return 0 if equal, otherwise -1 or 1.
 */
//==============================================================================
static int list_cmp_functor(const void *a, const void *b)
{
        const SIPC_socket_t *socka = a;
        const SIPC_socket_t *sockb = b;

        if (socka->port == sockb->port) {
                return 0;

        } else if (socka->port > sockb->port) {
                return 1;

        } else {
                return -1;
        }
}

//==============================================================================
/**
 * @brief  Function calculate fletcher 16 checksum.
 *
 * @param  data         buffer
 * @param  bytes        buffer size
 *
 * @return Checksum.
 */
//==============================================================================
static uint16_t fletcher16(uint8_t const *data, size_t bytes)
{
        uint16_t sum1 = 0xff, sum2 = 0xff;
        size_t tlen;

        while (bytes) {
                tlen = ((bytes >= 20) ? 20 : bytes);
                bytes -= tlen;
                do {
                        sum2 += sum1 += *data++;
                        tlen--;
                } while (tlen);

                sum1 = (sum1 & 0xff) + (sum1 >> 8);
                sum2 = (sum2 & 0xff) + (sum2 >> 8);
        }

        /* Second reduction step to reduce sums to 8 bits */
        sum1 = (sum1 & 0xff) + (sum1 >> 8);
        sum2 = (sum2 & 0xff) + (sum2 >> 8);

        return (sum2 << 8) | sum1;
}

//==============================================================================
/**
 * @brief  Function receive incoming packet.
 *
 * @param  packet       packet container
 *
 * @return On success return allocated data.
 */
//==============================================================================
static void receive_packet(sipc_packet_t *phdr, u8_t **payload)
{
        while (true) {
                size_t rdcnt;

                if (sys_fread(&phdr->preamble[0], 1, &rdcnt, sipc->if_file) != 0) continue;
                if (phdr->preamble[0] != PACKET_PREABLE[0]) continue;

                if (sys_fread(&phdr->preamble[1], 1, &rdcnt, sipc->if_file) != 0) continue;
                if (phdr->preamble[1] != PACKET_PREABLE[1]) continue;

                // check sum + rest of header
                if (sys_fread(&phdr->checksum, 8, &rdcnt, sipc->if_file) != 0) continue;

                if (phdr->plen > 0) {
                        u8_t *buf = NULL;
                        int err = zalloc(phdr->plen, (void*)&buf);
                        if (!err) {
                                if (  (sys_fread(buf, phdr->plen, &rdcnt, sipc->if_file) == 0)
                                   && (rdcnt == phdr->plen)) {

                                        *payload = buf;
                                        return;
                                } else {
                                        zfree((void*)&buf);
                                }
                        }
                } else {
                        *payload = NULL;
                        return;
                }
        }
}

//==============================================================================
/**
 * @brief  Function send packet.
 *
 * @param  port         packet port
 * @param  type         packet type
 * @param  payload      payload to send
 * @param  plen         payload length
 *
 * @return One of errno value.
 */
//==============================================================================
static int send_packet(u16_t seq, u8_t port, u8_t type, const u8_t *payload, u16_t plen)
{
        DEBUG("sending packet : seq:%u, port:%d, type:%s, plen:%d", seq, port, TYPE_STR[type], plen);

        if (!sipc) {
                return ENONET;
        }

        if (sipc->state == NET_SIPC_STATE__DOWN) {
                return ENONET;
        }

        if (!payload) {
                plen = 0;
        }

        sipc_packet_t packet;

        packet.preamble[0] = PACKET_PREABLE[0];
        packet.preamble[1] = PACKET_PREABLE[1];
        packet.seq         = seq;
        packet.port        = port;
        packet.type        = type;
        packet.plen        = plen;


        u16_t pktchks = fletcher16(cast(u8_t*, &packet.plen), sizeof(sipc_packet_t)
                                   - sizeof(packet.preamble)
                                   - sizeof(packet.checksum));

        u16_t datachks = payload ? fletcher16(payload, plen) : 0;

        u16_t checksum = pktchks ^ datachks;

        packet.checksum[0] = checksum & 0xFF;
        packet.checksum[1] = (checksum >> 8) & 0xFF;

        int err = sys_mutex_lock(sipc->packet_send_mtx, MAX_DELAY_MS);
        if (!err) {

                size_t wrcnt;
                int err = sys_fwrite(&packet, sizeof(sipc_packet_t), &wrcnt, sipc->if_file);

                if (!err) {
                        if (payload) {
                                err = sys_fwrite(payload, plen, &wrcnt, sipc->if_file);
                        }
                }

                sipc->seq_ctr++;
                sipc->stats.tx_packets++;
                sipc->stats.tx_bytes += plen;

                sys_mutex_unlock(sipc->packet_send_mtx);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function check if packet is valid (checksum valid)
 *
 * @param  packet       packet to examine (packet header)
 * @param  data         packet data
 *
 * @return If packet consistency is OK then true is returned, otherwise false.
 */
//==============================================================================
static bool is_packet_valid(sipc_packet_t *packet, u8_t *payload)
{
        u16_t pktchks = fletcher16(cast(u8_t*, &packet->plen), sizeof(sipc_packet_t)
                                   - sizeof(packet->preamble)
                                   - sizeof(packet->checksum));

        u16_t datachks = payload ? fletcher16(payload, packet->plen) : 0;

        u16_t checksum = pktchks ^ datachks;

        bool is_valid = checksum == cast(u16_t, (packet->checksum[0] | (packet->checksum[1] << 8)));

        if (not is_valid) {
                if (not __NETWORK_SIPC_DEBUG_ON__) {
                        printk("SIPC: invalid checksum");
                }

                DEBUG("packet checksum: %04Xh:%04Xh->%s", checksum,
                       cast(u16_t, (packet->checksum[0] | (packet->checksum[1] << 8))),
                       is_valid ? "VALID" : "INVALID");
        }

        return is_valid;
}

//==============================================================================
/**
 * @brief  Function return registered socket.
 *
 * @param  port         finding socket with port
 *
 * @return Socket object.
 */
//==============================================================================
static SIPC_socket_t *get_socket_by_port(u8_t port)
{
        SIPC_socket_t *socket = NULL;

        if (sys_mutex_lock(sipc->socket_list_mtx, MAX_DELAY_MS) == 0) {

                SIPC_socket_t find_socket;
                find_socket.port = port;

                int pos = sys_llist_find_begin(sipc->socket_list, &find_socket);

                if (pos >= 0) {
                        socket = sys_llist_at(sipc->socket_list, pos);
                }

                sys_mutex_unlock(sipc->socket_list_mtx);
        }

        return socket;
}

//==============================================================================
/**
 * @brief Network interface thread
 *
 * Thread handle receiving of incoming packets.
 *
 * @param arg   thread argument
 */
//==============================================================================
static void input_thread(void *arg)
{
        UNUSED_ARG1(arg);

        /* open interface file */
        bool msg = false;

        while (sipc->if_file == NULL) {
                int err = sys_fopen(__NETWORK_SIPC_INTERFACE_PATH__, "r+", &sipc->if_file);

                if (err && !msg) {
                        printk("SIPC: interface file error (%s)", strerror(err));
                        msg = true;
                }

                sys_sleep_ms(100);
        }

        while (true) {
                sipc_packet_t packet;
                u8_t *payload;
                receive_packet(&packet, &payload);

                if (sipc->state == NET_SIPC_STATE__DOWN) {

                        clear_transfer_counters();

                        if (sys_mutex_lock(sipc->socket_list_mtx, MAX_DELAY_MS) == 0) {

                                sys_llist_foreach(SIPC_socket_t*, socket, sipc->socket_list) {
                                        u8_t type = PACKET_TYPE_NACK;
                                        sys_queue_send(socket->ansq, &type, 0);
                                }

                                sys_mutex_unlock(sipc->socket_list_mtx);
                        }

                        continue;
                }

                DEBUG("received packet: seq:%u, port:%d, type:%s, plen:%d",
                      packet.seq, packet.port, packet.type < _PACKET_TYPE_COUNT
                                                           ? TYPE_STR[packet.type]
                                                           : "UNKNOWN", packet.plen);

                if (is_packet_valid(&packet, payload)) {

                        sipc->stats.rx_packets++;
                        sipc->stats.rx_bytes += sizeof(sipc_packet_t) + packet.plen;

                        SIPC_socket_t *socket = get_socket_by_port(packet.port);

                        if (socket) {
                                socket->seq = packet.seq;

                                if (  (packet.type == PACKET_TYPE_ACK )
                                   || (packet.type == PACKET_TYPE_NACK) ) {

                                        if (payload) {
                                                DEBUG("ACK/NACK with data - freeing");
                                                zfree((void*)&payload);
                                        }

                                        sys_queue_send(socket->ansq, &packet.type, 0);

                                } else if ((packet.type == PACKET_TYPE_BUSY)) {
                                        if (payload) {
                                                DEBUG("BUSY with data - freeing");
                                                zfree((void*)&payload);
                                        }

                                } else if ((packet.type) == PACKET_TYPE_REPEAT) {
                                        if (payload) {
                                                DEBUG("REPEAT with data - freeing");
                                                zfree((void*)&payload);
                                        }

                                        sys_queue_send(socket->ansq, &packet.type, 0);

                                } else if (packet.type == PACKET_TYPE_DATA) {

                                        int err = sipcbuf__write(socket->rxbuf, payload,
                                                                 packet.plen, false);

                                        u8_t ptype = PACKET_TYPE_ACK;

                                        if (err && payload) {
                                                DEBUG("buffer error: %d", err);
                                                zfree((void*)&payload);
                                                ptype = PACKET_TYPE_NACK;

                                        } else if (sipcbuf__is_full(socket->rxbuf)) {
                                                socket->busy = true;
                                                ptype = PACKET_TYPE_BUSY;
                                        }

                                        send_packet(packet.seq, packet.port, ptype, NULL, 0);

                                } else if (packet.type == PACKET_TYPE_BIND) {

                                        if (payload) {
                                                DEBUG("BIND with data - freeing");
                                                zfree((void*)&payload);
                                        }

                                        if (socket->waiting_for_data_ack) {
                                                sys_queue_send(socket->ansq, &packet.type, 0);
                                        }

                                } else if (packet.type == PACKET_TYPE_HANDSHAKE) {

                                        if (payload) {
                                                DEBUG("HANDSHAKE with data - freeing");
                                                zfree((void*)&payload);
                                        }

                                        send_packet(packet.seq, packet.port, PACKET_TYPE_ACK, NULL, 0);

                                        if (socket->waiting_for_data_ack) {
                                                sys_queue_send(socket->ansq, &packet.type, 0);
                                        }

                                } else {
                                        if (payload) {
                                                zfree((void*)&payload);
                                        }

                                        DEBUG("ignoring answer for unknown packet");
                                }
                        } else {
                                DEBUG("socket for incoming port not registered");

                                if (payload) {
                                        zfree((void*)&payload);
                                }

                                send_packet(packet.seq, packet.port, PACKET_TYPE_NACK, NULL, 0);
                        }

                } else {
                        DEBUG("received inconsistent packet");

                        if (payload) {
                                zfree((void*)&payload);
                        }

                        send_packet(packet.seq, packet.port, PACKET_TYPE_REPEAT, NULL, 0);
                }
        }
}

//==============================================================================
/**
 * @brief  Function register selected socket in stack.
 *
 * @param  socket       socket to register
 *
 * @return One of errno value.
 */
//==============================================================================
static int register_socket(SIPC_socket_t *socket)
{
        int err = sys_mutex_lock(sipc->socket_list_mtx, MAX_DELAY_MS);
        if (!err) {

                if (sys_llist_contains(sipc->socket_list, socket) > 0) {
                        err = EADDRINUSE;
                        goto finish;
                }

                if (sys_llist_push_back(sipc->socket_list, socket) == NULL) {
                        err = ENOMEM;
                }

                finish:
                sys_mutex_unlock(sipc->socket_list_mtx);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function check if socket is already registered.
 *
 * @param  socket       socket to examine
 *
 * @return If socket is registered then true is returned, otherwise false.
 */
//==============================================================================
static bool is_socket_registered(SIPC_socket_t *socket)
{
        bool is_registered = false;

        if (sys_mutex_lock(sipc->socket_list_mtx, MAX_DELAY_MS) == 0) {
                is_registered = sys_llist_contains(sipc->socket_list, socket) > 0;
                sys_mutex_unlock(sipc->socket_list_mtx);
        }

        return is_registered;
}

//==============================================================================
/**
 * @brief  Function unregister selected socket from stack.
 *
 * @param  socket       socket to unregister
 */
//==============================================================================
static void unregister_socket(SIPC_socket_t *socket)
{
        int err = sys_mutex_lock(sipc->socket_list_mtx, MAX_DELAY_MS);
        if (!err) {
                int pos = sys_llist_find_begin(sipc->socket_list, socket);

                if (pos >= 0) {
                        sys_llist_take(sipc->socket_list, pos);
                }

                sys_mutex_unlock(sipc->socket_list_mtx);
        }
}

//==============================================================================
/**
 * @brief  Function starts network manager and set network interface.
 *         Function is called before kernel start.
 * @param  None
 * @return One of @ref errno value.
 */
//==============================================================================
static int stack_init(void)
{
        if (sipc) {
                return ESUCC;
        }

        int err = zalloc(sizeof(sipc_t), (void*)&sipc);
        if (!err) {
                static const thread_attr_t attr = {
                        .priority    = PRIORITY_NORMAL,
                        .stack_depth = STACK_DEPTH_LOW,
                        .detached    = true
                };

                int tierr = sys_thread_create(input_thread, &attr, NULL, &sipc->if_thread);
                int merr  = sys_mutex_create(MUTEX_TYPE_NORMAL, &sipc->socket_list_mtx);
                int serr  = sys_mutex_create(MUTEX_TYPE_NORMAL, &sipc->packet_send_mtx);
                int lerr  = _llist_create_krn(_MM_NET, list_cmp_functor, NULL, &sipc->socket_list);

                if (tierr || merr || lerr || serr) {
                        if (!tierr) {
                                sys_thread_destroy(sipc->if_thread);
                        }

                        if (!merr) {
                                sys_mutex_destroy(sipc->socket_list_mtx);
                        }

                        if (!serr) {
                                sys_mutex_destroy(sipc->packet_send_mtx);
                        }

                        if (!lerr) {
                                sys_llist_destroy(sipc->socket_list);
                        }

                        zfree((void*)&sipc);

                        printk("SIPC: stack initialization error");

                        err = ENOMEM;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function up the network.
 * @param  cfg   configuration structure
 * @return One of @ref errno value.
 */
//==============================================================================
int SIPC_ifup(const NET_SIPC_config_t *cfg)
{
        UNUSED_ARG1(cfg);

        int err = stack_init();
        if (!err) {
                sipc->conf.MTU = max(64, cfg->MTU);
                sipc->state = NET_SIPC_STATE__UP;
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function turn down interface with static configuration.
 * @return One of @ref errno value.
 */
//==============================================================================
int SIPC_ifdown(void)
{
        int err = ENONET;

        if ((sipc != NULL) && (sipc->state == NET_SIPC_STATE__UP)) {

                sipc->state = NET_SIPC_STATE__DOWN;

                clear_transfer_counters();

                err = ESUCC;
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function returns interface status.
 * @param  status       status container
 * @return One of @ref errno value.
 */
//==============================================================================
int SIPC_ifstatus(NET_SIPC_status_t *status)
{
        if (sipc) {
                status->MTU        = sipc->conf.MTU;
                status->rx_packets = sipc->stats.rx_packets;
                status->rx_bytes   = sipc->stats.rx_bytes;
                status->tx_packets = sipc->stats.tx_packets;
                status->tx_bytes   = sipc->stats.tx_bytes;
                status->state      = sipc->state;

                return ESUCC;
        } else {
                return ENONET;
        }
}

//==============================================================================
/**
 * @brief  Function create socket container.
 * @param  prot         protocol
 * @param  socket       sipc socket (object is already created)
 * @return One of @ref errno value.
 */
//==============================================================================
int SIPC_socket_create(NET_protocol_t prot, SIPC_socket_t *socket)
{
        int err = EINVAL;

        if (prot == NET_PROTOCOL__STREAM) {

                socket->port         = 0;
                socket->recv_timeout = MAX_DELAY_MS;
                socket->send_timeout = MAX_DELAY_MS;

                err = sipcbuf__create((void*)&socket->rxbuf, __NETWORK_SIPC_RECV_BUF_SIZE__);
                if (err) {
                        goto finish;
                }

                err = sys_queue_create(1, sizeof(((sipc_packet_t*)NULL)->type), &socket->ansq);
                if (err) {
                        goto finish;
                }

                finish:
                if (err) {
                        if (socket->rxbuf) {
                                sipcbuf__destroy(socket->rxbuf);
                        }

                        if (socket->ansq) {
                                sys_queue_destroy(socket->ansq);
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function destroy created socket.
 * @param  socket    sipc socket to destroy
 * @return One of @ref errno value.
 */
//==============================================================================
int SIPC_socket_destroy(SIPC_socket_t *socket)
{
        unregister_socket(socket);

        socket->port         = 0;
        socket->recv_timeout = 0;
        socket->send_timeout = 0;

        sipcbuf__destroy(socket->rxbuf);

        return sys_queue_destroy(socket->ansq);
}

//==============================================================================
/**
 * @brief  Function connect socket to selected address.
 * @param  socket    socket
 * @param  addr      sipc address
 * @return One of @ref errno value.
 */
//==============================================================================
int SIPC_socket_connect(SIPC_socket_t *socket, const NET_SIPC_sockaddr_t *addr)
{
        if (addr->port == 0) {
                return EINVAL;
        }

        socket->port = addr->port;

        int err = register_socket(socket);
        if (err) {
                socket->port = 0;
                return err;
        }

        sys_queue_reset(socket->ansq);

        socket->seq = sipc->seq_ctr;

        err = send_packet(socket->seq, socket->port, PACKET_TYPE_HANDSHAKE, NULL, 0);
        if (!err) {
                u8_t type;
                err = sys_queue_receive(socket->ansq, &type, CONNECTION_TIMEOUT);
                if (!err) {
                        switch (type) {
                        case PACKET_TYPE_ACK:
                                break;

                        case PACKET_TYPE_NACK:
                                err = ECONNREFUSED;
                                break;

                        default:
                                err = EFAULT;
                                break;
                        }
                } else {
                        DEBUG("connection timeout");
                }
        }

        if (err) {
                unregister_socket(socket);
                socket->port = 0;
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function disconnect socket from selected address.
 * @param  socket     socket
 * @return One of @ref errno value.
 */
//==============================================================================
int SIPC_socket_disconnect(SIPC_socket_t *socket)
{
        unregister_socket(socket);
        sipcbuf__clear(socket->rxbuf);
        sys_queue_reset(socket->ansq);
        socket->port = 0;

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function shutdown selected socket direction.
 * @param  socket    socket
 * @param  how          shutdown direction
 * @return One of @ref errno value.
 */
//==============================================================================
int SIPC_socket_shutdown(SIPC_socket_t *socket, NET_shut_t how)
{
        UNUSED_ARG2(socket, how);
        return ENOTSUP;
}

//==============================================================================
/**
 * @brief  Function bind selected address with socket.
 * @param  socket     socket
 * @param  addr          inet address
 * @return One of @ref errno value.
 */
//==============================================================================
int SIPC_socket_bind(SIPC_socket_t *socket, const NET_SIPC_sockaddr_t *addr)
{
        if (addr->port == 0) {
                return EINVAL;
        }

        socket->port = addr->port;

        int err = register_socket(socket);
        if (!err) {

                sys_queue_reset(socket->ansq);
                socket->seq = sipc->seq_ctr;
                err = send_packet(socket->seq, socket->port, PACKET_TYPE_BIND, NULL, 0);

        } else {
                socket->port = 0;
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function listen connection.
 * @param  socket        socket
 * @return One of @ref errno value.
 */
//==============================================================================
int SIPC_socket_listen(SIPC_socket_t *socket)
{
        UNUSED_ARG1(socket);
        return ENOTSUP;
}

//==============================================================================
/**
 * @brief  Function accept incoming connection.
 * @param  socket       socket
 * @param  new_socket   new socket of accepted connection
 * @return One of @ref errno value.
 */
//==============================================================================
int SIPC_socket_accept(SIPC_socket_t *socket, SIPC_socket_t *new_socket)
{
        UNUSED_ARG2(socket, new_socket);
        return ENOTSUP;
}

//==============================================================================
/**
 * @brief  Function receive incoming data.
 * @param  socket    socket
 * @param  buf          buffer for data
 * @param  len          number of data to receive
 * @param  flags        flags
 * @param  recved       number of received bytes
 * @return One of @ref errno value.
 */
//==============================================================================
int SIPC_socket_recv(SIPC_socket_t *socket,
                     void          *buf,
                     size_t         len,
                     NET_flags_t    flags,
                     size_t        *recved)
{
        if (!is_socket_registered(socket)) {
                return ECONNREFUSED;
        }

        int   err  = ETIME;
        u32_t tref = sys_time_get_reference();

        *recved = 0;

        while (!sys_time_is_expired(tref, socket->recv_timeout)) {

                err = sipcbuf__read(socket->rxbuf, buf, len, recved);
                if (!err) {
                        if (*recved == 0) {
                                err = ETIME;
                                sys_sleep_ms(5);
                                continue;
                        } else {
                                if (socket->busy && !sipcbuf__is_full(socket->rxbuf)) {
                                        socket->busy = false;
                                        send_packet(socket->seq, socket->port, PACKET_TYPE_ACK, NULL, 0);
                                }
                        }
                }

                break;
        }

        if (flags & NET_FLAGS__FREEBUF) {
                sipcbuf__clear(socket->rxbuf);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function receive data from selected address.
 * @param  socket    socket
 * @param  buf          buffer for data
 * @param  len          number of bytes to receive
 * @param  flags        flags
 * @param  sockaddr     socket address
 * @param  recved       number of received bytes
 * @return One of @ref errno value.
 */
//==============================================================================
int SIPC_socket_recvfrom(SIPC_socket_t       *socket,
                         void                *buf,
                         size_t               len,
                         NET_flags_t          flags,
                         NET_SIPC_sockaddr_t *sockaddr,
                         size_t              *recved)
{
        UNUSED_ARG6(socket, buf, len, flags, sockaddr, recved);
        return ENOTSUP;
}

//==============================================================================
/**
 * @brief  Function send data to connected socket.
 * @param  socket    socket
 * @param  buf          buffer to send
 * @param  len          number of bytes to send
 * @param  flags        flags
 * @param  sent         number of sent bytes
 * @return One of @ref errno value.
 */
//==============================================================================
int SIPC_socket_send(SIPC_socket_t *socket,
                     const void    *buf,
                     size_t         len,
                     NET_flags_t    flags,
                     size_t        *sent)
{
        UNUSED_ARG1(flags);

        if (!is_socket_registered(socket)) {
                return ECONNREFUSED;
        }

        int err = EINVAL;

        *sent = 0;

        void *bufcopy = NULL;

        if (flags & NET_FLAGS__COPY) {
                err = kalloc(len, &bufcopy);
                if (!err && bufcopy) {
                        memcpy(bufcopy, buf, len);
                        buf = bufcopy;
                } else {
                        return err;
                }
        }

        sys_queue_reset(socket->ansq);

        while (len) {

                u16_t plen = min(len, sipc->conf.MTU);
                socket->seq = sipc->seq_ctr;
                socket->waiting_for_data_ack = true;

                err = send_packet(socket->seq, socket->port, PACKET_TYPE_DATA, buf, plen);
                if (!err) {
                        u8_t type;
                        err = sys_queue_receive(socket->ansq, &type, socket->send_timeout);
                        if (!err) {
                                switch (type) {
                                case PACKET_TYPE_ACK:
                                        buf   += plen;
                                        *sent += plen;
                                        len   -= plen;
                                        break;

                                case PACKET_TYPE_NACK:
                                        err = ECONNREFUSED;
                                        len = 0;
                                        break;

                                case PACKET_TYPE_REPEAT:
                                        break;

                                case PACKET_TYPE_BIND:
                                case PACKET_TYPE_HANDSHAKE:
                                        err = ECONNRESET;
                                        len = 0;
                                        break;

                                default:
                                        err = EFAULT;
                                        len = 0;
                                        break;
                                }
                        } else {
                                break;
                        }
                } else {
                        break;
                }
        }

        socket->waiting_for_data_ack = false;

        if (bufcopy) {
                zfree(&bufcopy);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function send buffer to selected address.
 * @param  socket    socket
 * @param  buf          buffer to send
 * @param  len          number of bytes to send
 * @param  flags        flags
 * @param  to_sockaddr  socket address
 * @param  sent         number of sent bytes
 * @return One of @ref errno value.
 */
//==============================================================================
int SIPC_socket_sendto(SIPC_socket_t             *socket,
                       const void                *buf,
                       size_t                     len,
                       NET_flags_t                flags,
                       const NET_SIPC_sockaddr_t *to_sockaddr,
                       size_t                    *sent)
{
        UNUSED_ARG6(socket, buf, len, flags, to_sockaddr, sent);
        return ENOTSUP;
}

//==============================================================================
/**
 * @brief  Function gets host address by name.
 * @param  name         address name
 * @param  addr         received address
 * @return One of @ref errno value.
 */
//==============================================================================
int SIPC_gethostbyname(const char *name, NET_SIPC_sockaddr_t *sock_addr)
{
        UNUSED_ARG2(name, sock_addr);
        return ENOTSUP;
}

//==============================================================================
/**
 * @brief  Function set receive timeout.
 * @param  socket    socket
 * @param  timeout      timeout value in milliseconds
 * @return One of @ref errno value.
 */
//==============================================================================
int SIPC_socket_set_recv_timeout(SIPC_socket_t *socket, uint32_t timeout)
{
        socket->recv_timeout = timeout;

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function set send timeout.
 * @param  socket    socket
 * @param  timeout      timeout value in milliseconds
 * @return One of @ref errno value.
 */
//==============================================================================
int SIPC_socket_set_send_timeout(SIPC_socket_t *socket, uint32_t timeout)
{
        socket->send_timeout = timeout;

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function get receive timeout.
 * @param  socket    socket
 * @param  timeout      timeout value in milliseconds
 * @return One of @ref errno value.
 */
//==============================================================================
int SIPC_socket_get_recv_timeout(SIPC_socket_t *socket, uint32_t *timeout)
{
        *timeout = socket->recv_timeout;
        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function get send timeout.
 * @param  socket    socket
 * @param  timeout      timeout value in milliseconds
 * @return One of @ref errno value.
 */
//==============================================================================
int SIPC_socket_get_send_timeout(SIPC_socket_t *socket, uint32_t *timeout)
{
        *timeout = socket->send_timeout;
        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function returns address of socket (remote connection address).
 * @param  socket    socket
 * @param  sockaddr     socket address (address and port)
 * @return One of @ref errno value.
 */
//==============================================================================
int SIPC_socket_getaddress(SIPC_socket_t *socket, NET_SIPC_sockaddr_t *sockaddr)
{
        return sockaddr->port = socket->port;
}

//==============================================================================
/**
 * @brief  Function convert value for host/network purpose.
 * @param  value        value to convert
 * @return Converted value.
 */
//==============================================================================
u16_t SIPC_hton_u16(u16_t value)
{
#if _CPUCTL_BYTE_ORDER == _BYTE_ORDER_LITTLE_ENDIAN
        return value;
#elif _CPUCTL_BYTE_ORDER == _BYTE_ORDER_BIG_ENDIAN
        return ((value & 0x00FF) << 8)
             | ((value & 0xFF00) >> 8);
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
u32_t SIPC_hton_u32(u32_t value)
{
#if _CPUCTL_BYTE_ORDER == _BYTE_ORDER_LITTLE_ENDIAN
        return value;
#elif _CPUCTL_BYTE_ORDER == _BYTE_ORDER_BIG_ENDIAN
        return ((value & 0x000000FFUL) << 24)
             | ((value & 0x0000FF00UL) << 8)
             | ((value & 0x00FF0000UL) >> 8)
             | ((value & 0xFF000000UL) >> 24);
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
u64_t SIPC_hton_u64(u64_t value)
{
#if _CPUCTL_BYTE_ORDER == _BYTE_ORDER_LITTLE_ENDIAN
        return value;
#elif _CPUCTL_BYTE_ORDER == _BYTE_ORDER_BIG_ENDIAN
        return ((value & 0x00000000000000FFULL) << 56)
             | ((value & 0x000000000000FF00ULL) << 40)
             | ((value & 0x0000000000FF0000ULL) << 24)
             | ((value & 0x00000000FF000000ULL) << 8)
             | ((value & 0x000000FF00000000ULL) >> 8)
             | ((value & 0x0000FF0000000000ULL) >> 24)
             | ((value & 0x00FF000000000000ULL) >> 40)
             | ((value & 0xFF00000000000000ULL) >> 56);
#else
#error "Not supported endianness!"
#endif
}

/*==============================================================================
  End of file
==============================================================================*/
