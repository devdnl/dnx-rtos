/*==============================================================================
File     cannet.c

Author   Daniel Zorychta

Brief    CANNET Network management.

         Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

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
Connection:
   Client                               Host
1. connect frame + 0    ->
                        <-              connect frame + flags


Disconnect:
   Client                               Host
1. disconnect frame     ->              no answer


Data transfer (stream)
   Client                               Host
1. stat frame           ->
                        <-              stat frame + buffer-available
   first frame          ->
   next frame           ->
   next frame           ->
   ...                  ->
   last frame           ->
                        <-              ack frame + flags

   if more data then go to 1


Data transfer (datagram, up to MTU)
   Client                               Host
1. first frame          ->
   next frame           ->
   next frame           ->
   ...                  ->
   last frame           ->
*/

/*==============================================================================
  Include files
==============================================================================*/
#include <stdbool.h>
#include "net/cannet/cannet.h"
#include "cpuctl.h"
#include "drivers/ioctl_requests.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define zalloc(_size, _pptr) _kzalloc(_MM_NET, _size, NULL, 0, 0, _pptr)
#define kalloc(_size, _pptr) _kmalloc(_MM_NET, _size, NULL, 0, 0, _pptr)
#define zfree(_pptr) _kfree(_MM_NET, _pptr)

#if __NETWORK_CANNET_DEBUG_ON__ > 0
#define DEBUG(...) printk("CANNET: "__VA_ARGS__)
#else
#define DEBUG(...)
#endif

#define CONNECT_FRAME_FLAG__RESPONSE    (1<<0)
#define CONNECT_FRAME_FLAG__CONNECTED   (1<<1)
#define STAT_FRAME_FLAG__RESPONSE       (1<<0)

/*==============================================================================
  Local object types
==============================================================================*/
enum {
        send_state_idle,
        send_state_sending,
        send_state_sent,
};

typedef enum {
        // data transfer frames
        frame_type__first,
        frame_type__next,
        frame_type__last,
        frame_type__single,

        // control frames
        frame_type__connect,            // connect to host
        frame_type__disconnect,         // inform about disconnection
        frame_type__stat,               // transfer statistics (available space, etc)
        frame_type__ack                 // transfer acknowledge
} frame_type_t;

/*
 * HEADER BITS: [TTT PPP SS]
 * 7-5: type
 * 4-2: port
 * 1-0: seq
 */
typedef uint8_t header_t;

typedef struct PACKED {
        header_t header;
        uint16_t transfer_size;
        uint16_t transfer_crc16;
        uint8_t  data[3];
} first_frame_t;

typedef struct PACKED {
        header_t header;
        uint8_t  data[7];
} next_frame_t;

typedef struct PACKED {
        header_t header;
        uint8_t  data[7];       // up to 7 bytes, can be less
} last_frame_t;

typedef struct PACKED {
        header_t header;
        uint8_t  data[7];       // up to 7 bytes, can be less
} single_frame_t;

typedef struct PACKED {
        header_t header;
        uint8_t  flags;
} connect_frame_t;

typedef struct PACKED {
        header_t header;
} disconnect_frame_t;

typedef struct PACKED {
        header_t header;
        uint8_t  flags;
        uint16_t available;
} stat_frame_t;

typedef struct PACKED {
        header_t header;
        uint8_t  flags;
} ack_frame_t;

typedef struct PACKED {
        header_t header;
} generic_frame_t;

typedef struct {
        u16_t addr_src;
        union {
                generic_frame_t generic;
                connect_frame_t connect;
                disconnect_frame_t disconnect;
                stat_frame_t stat;
                ack_frame_t ack;
        };
} answer_t;

typedef struct {
        bool  thread_run;
        FILE *if_file;
        mutex_t *mutex;
        tid_t if_thread;
        NET_CANNET_state_t state;
        u16_t addr;
        CANNET_socket_t *sockets[__NETWORK_CANNET_MAX_SOCKETS__];

        struct {
                u64_t rx_packets;
                u64_t tx_packets;
                u64_t rx_bytes;
                u64_t tx_bytes;
        } stats;
} cannet_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int  input_thread(void *sem);
static void clear_transfer_counters(cannet_t *cannet);

/*==============================================================================
  External function prototypes
==============================================================================*/
static int register_socket(cannet_t *cannet, CANNET_socket_t *socket);
static int unregister_socket(cannet_t *cannet, CANNET_socket_t *socket);
static bool is_socket_registered(cannet_t *cannet, CANNET_socket_t *socket);
static int send_payload(cannet_t *cannet, CANNET_socket_t *socket, const void *payload, uint16_t payload_len);
static frame_type_t get_frame_type(const header_t *header);
static void set_frame_type(header_t *header, frame_type_t type);
static inline uint8_t get_frame_sequence(const header_t *header);
static inline void set_frame_sequence(header_t *header, uint8_t sequence);
static inline void set_frame_port(header_t *header, uint8_t port);
static inline uint8_t get_frame_port(const header_t *header);
static inline void set_header(header_t *header, frame_type_t type, uint8_t port, uint8_t sequence);
static uint16_t calculate_crc16(const void *buf, uint16_t len, uint16_t crc_in);
static inline uint32_t set_can_id(uint16_t destination, uint16_t source);
static inline uint16_t get_destination_address(uint32_t can_id);
static inline uint16_t get_source_address(uint32_t can_id);
static int send_first_frame(cannet_t *cannet, CANNET_socket_t *socket, const void **payload, u16_t *payload_len);
static int send_next_frame(cannet_t *cannet, CANNET_socket_t *socket, const void **payload, u16_t *payload_len);
static int send_last_frame(cannet_t *cannet, CANNET_socket_t *socket, const void **payload, u16_t *payload_len);
static int send_single_frame(cannet_t *cannet, CANNET_socket_t *socket, const void **payload, u16_t *payload_len);
static int send_connect_frame(cannet_t *cannet, CANNET_socket_t *socket, u8_t flags);
static int send_disconnect_frame(cannet_t *cannet, CANNET_socket_t *socket);
static int send_stat_frame(cannet_t *cannet, CANNET_socket_t *socket, u8_t flags);
static int send_ack_frame(cannet_t *cannet, CANNET_socket_t *socket, u8_t flags);
static int wait_for_peer_answer(cannet_t *cannet, CANNET_socket_t *socket, answer_t *answer, u32_t timeout);
static void handle_incoming_frame(cannet_t *cannet, CAN_msg_t *can_frame);
static int received_first_frame(cannet_t *cannet, CANNET_socket_t *socket, const first_frame_t *frame);
static int received_next_frame(cannet_t *cannet, CANNET_socket_t *socket, const next_frame_t *frame);
static int received_last_frame(cannet_t *cannet, CANNET_socket_t *socket, const last_frame_t *frame, u8_t len);
static int received_single_frame(cannet_t *cannet, CANNET_socket_t *socket, const single_frame_t *frame, u8_t len);
static int received_connect_frame(cannet_t *cannet, CANNET_socket_t *socket, u16_t src_addr, const connect_frame_t *frame);
static int received_disconnect_frame(cannet_t *cannet, CANNET_socket_t *socket, const disconnect_frame_t *frame);
static int received_stat_frame(cannet_t *cannet, CANNET_socket_t *socket, const stat_frame_t *frame);
static int received_ack_frame(cannet_t *cannet, CANNET_socket_t *socket, const ack_frame_t *frame);

/*==============================================================================
  Local objects
==============================================================================*/
static const uint16_t crc16_ccitt_table[256] = {
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
        0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
        0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
        0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
        0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
        0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
        0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
        0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
        0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
        0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
        0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
        0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
        0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
        0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
        0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
        0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
        0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
        0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
        0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
        0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
        0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
        0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
        0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
        0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
        0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
        0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
        0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
        0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
        0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
        0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
        0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
        0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

static const uint16_t crc16_ccitt_start = 0xFFFF;

static const u32_t CAN_SEND_TIMEOUT = 2000;
static const u32_t THREAD_CAN_RECV_TIMEOUT_ms = 1000;
static const u32_t MUTEX_TIMEOUT_ms = 10000;
static const u32_t CONNECTION_TIMEOUT_ms = 250;
static const u32_t ACK_TIMEOUT_ms = 500;
static const u32_t STAT_TIMEOUT_ms = 500;

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
static void clear_transfer_counters(cannet_t *cannet)
{
        cannet->stats.rx_bytes   = 0;
        cannet->stats.tx_bytes   = 0;
        cannet->stats.rx_packets = 0;
        cannet->stats.tx_packets = 0;
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
static int input_thread(void *arg)
{
        cannet_t *cannet = arg;

        if (cannet->thread_run) {
                ioctl(fileno(cannet->if_file), IOCTL_CAN__SET_RECV_TIMEOUT, &THREAD_CAN_RECV_TIMEOUT_ms);
        }

        while (cannet->thread_run) {
                CAN_msg_t can_frame;
                int err = ioctl(fileno(cannet->if_file), IOCTL_CAN__RECV_MSG, &can_frame);
                if (!err) {
                        if (cannet->state == NET_CANNET_STATE__UP) {

                                if (can_frame.extended_ID and (can_frame.data_length >= 1)
                                   and not can_frame.remote_transmission) {

                                        handle_incoming_frame(cannet, &can_frame);
                                }
                        }
                }
        }

        return -1;
}

//==============================================================================
/**
 * @brief  Function initialize network.
 * @param  ctx          context
 * @param  if_path      interface path
 * @return One of @ref errno value.
 */
//==============================================================================
int CANNET_ifinit(void **ctx, const char *if_path)
{
        int err = zalloc(sizeof(cannet_t), ctx);
        if (!err) {
                cannet_t *cannet = *ctx;

                cannet->thread_run = true;

                static const thread_attr_t attr = {
                        .priority    = PRIORITY_NORMAL,
                        .stack_depth = STACK_DEPTH_LOW,
                        .detached    = true
                };

                int merr  = sys_mutex_create(MUTEX_TYPE_RECURSIVE, &cannet->mutex);
                int ferr  = sys_fopen(if_path, "r+", &cannet->if_file);
                int tierr = sys_thread_create(input_thread, &attr, cannet, &cannet->if_thread);

                if (merr or tierr or ferr) {
                        CANNET_ifdeinit(cannet);
                        printk("CANNET: stack initialization error");
                        err = ENOMEM;

                } else {
                        sys_ioctl(cannet->if_file, IOCTL_CAN__SET_SEND_TIMEOUT, &CAN_SEND_TIMEOUT);

                        printk("CANNET (%s): thread ID: %u", if_path, cannet->if_thread);
                        cannet->state = NET_CANNET_STATE__DOWN;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function deinitialize network.
 * @param  ctx   context
 * @return One of @ref errno value.
 */
//==============================================================================
int CANNET_ifdeinit(void *ctx)
{
        cannet_t *cannet = ctx;

        int err = EINVAL;

        cannet->thread_run = false;

        sys_sleep_ms(THREAD_CAN_RECV_TIMEOUT_ms * 2);
        sys_fclose(cannet->if_file);
        sys_mutex_destroy(cannet->mutex);

        err = zfree(&ctx);

        return err;
}

//==============================================================================
/**
 * @brief  Function up the network.
 * @param  ctx   context
 * @param  cfg   configuration structure
 * @return One of @ref errno value.
 */
//==============================================================================
int CANNET_ifup(void *ctx, const NET_CANNET_config_t *cfg)
{
        cannet_t *cannet = ctx;

        int err = EINVAL;

        if (cannet and cfg) {

                if (cannet->state == NET_CANNET_STATE__DOWN) {

                        cannet->addr = cfg->addr;
                        clear_transfer_counters(cannet);

                        CAN_mode_t mode = CAN_MODE__INIT;
                        err = sys_ioctl(cannet->if_file, IOCTL_CAN__SET_MODE, &mode);

                        if (!err) {
                                CAN_filter_t filter;
                                filter.number = 0;
                                filter.ID = cfg->addr;
                                filter.mask = NET_CANNET_CAN_ID_MASK;
                                filter.extended_ID = true;
                                err = sys_ioctl(cannet->if_file, IOCTL_CAN__SET_FILTER, &filter);
                        }

                        if (!err) {
                                CAN_filter_t filter;
                                filter.number = 1;
                                filter.ID = NET_CANNET_ADDR_BROADCAST;
                                filter.mask = NET_CANNET_CAN_ID_MASK;
                                filter.extended_ID = true;
                                err = sys_ioctl(cannet->if_file, IOCTL_CAN__SET_FILTER, &filter);
                        }

                        if (!err) {
                                CAN_mode_t mode = CAN_MODE__NORMAL;
                                err = sys_ioctl(cannet->if_file, IOCTL_CAN__SET_MODE, &mode);
                        }

                        if (!err) {
                                cannet->state = NET_CANNET_STATE__UP;
                        }
                } else {
                        err = EALREADY;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function turn down interface with static configuration.
 * @return One of @ref errno value.
 */
//==============================================================================
int CANNET_ifdown(void *ctx)
{
        cannet_t *cannet = ctx;

        int err = EINVAL;

        if (cannet) {
                clear_transfer_counters(cannet);
                cannet->state = NET_CANNET_STATE__DOWN;
                err = 0;
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
int CANNET_ifstatus(void *ctx, NET_CANNET_status_t *status)
{
        cannet_t *cannet = ctx;

        if (cannet) {
                status->MTU        = __NETWORK_CANNET_MAX_SOCKET_BUFFER_LENGTH__;
                status->addr       = cannet->addr;
                status->rx_packets = cannet->stats.rx_packets;
                status->rx_bytes   = cannet->stats.rx_bytes;
                status->tx_packets = cannet->stats.tx_packets;
                status->tx_bytes   = cannet->stats.tx_bytes;
                status->state      = cannet->state;

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
int CANNET_socket_create(void *ctx, NET_protocol_t prot, CANNET_socket_t *socket)
{
        UNUSED_ARG1(ctx);

        int qerr  = sys_queue_create(1, sizeof(answer_t), &socket->ansq);
        int rberr = cannetbuf__create(&socket->rx_buf, __NETWORK_CANNET_MAX_SOCKET_BUFFER_LENGTH__);
        int aberr = cannetbuf__create(&socket->assembly_buf, __NETWORK_CANNET_MAX_SOCKET_BUFFER_LENGTH__);
        int err   = ENOMEM;

        if (!qerr and !rberr and !aberr) {
                socket->protocol     = prot;
                socket->addr_remote  = UINT16_MAX;
                socket->port         = UINT8_MAX;
                socket->recv_timeout = MAX_DELAY_MS;
                socket->send_timeout = MAX_DELAY_MS;
                socket->shutdown     = 0;
                socket->connected    = (prot == NET_PROTOCOL__DATAGRAM);
                err = 0;
        } else {
                sys_queue_destroy(socket->ansq);
                cannetbuf__destroy(socket->rx_buf);
                cannetbuf__destroy(socket->assembly_buf);
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
int CANNET_socket_destroy(void *ctx, CANNET_socket_t *socket)
{
        cannet_t *cannet = ctx;

        unregister_socket(cannet, socket);
        sys_queue_destroy(socket->ansq);
        cannetbuf__destroy(socket->rx_buf);
        cannetbuf__destroy(socket->assembly_buf);

        memset(socket, 0, sizeof(*socket));

        return 0;
}

//==============================================================================
/**
 * @brief  Function connect socket to selected address. Client side function.
 * @param  socket    socket
 * @param  addr      sipc address
 * @return One of @ref errno value.
 */
//==============================================================================
int CANNET_socket_connect(void *ctx, CANNET_socket_t *socket, const NET_CANNET_sockaddr_t *addr)
{
        cannet_t *cannet = ctx;

        socket->addr_remote = addr->addr;
        socket->port = addr->port;

        int err = register_socket(cannet, socket);
        if (!err) {

                for (int try = 1; try <= 3; try++) {

                        DEBUG("[%u] socket_connect: try %i, socket [%u:%u]", try,
                              cannet->addr, socket->addr_remote, socket->port);

                        err = send_connect_frame(cannet, socket, 0);
                        if (!err) {
                                answer_t answer;
                                err = wait_for_peer_answer(cannet, socket, &answer, CONNECTION_TIMEOUT_ms);
                                if (!err) {
                                        if (get_frame_type(&answer.generic.header) == frame_type__connect) {

                                                if (    (answer.connect.flags & CONNECT_FRAME_FLAG__RESPONSE)
                                                    and (answer.connect.flags & CONNECT_FRAME_FLAG__CONNECTED) ) {

                                                        socket->connected = true;

                                                        DEBUG("[%u] socket_connect: connected, socket [%u:%u]",
                                                              cannet->addr, socket->addr_remote, socket->port);

                                                        err = 0;

                                                } else {
                                                        DEBUG("[%u] socket_connect: refused, socket [%u:%u]",
                                                              cannet->addr, socket->addr_remote, socket->port);
                                                        err = ECONNREFUSED;
                                                }

                                                break;

                                        } else {
                                                DEBUG("[%u] socket_connect: invalid reply frame, socket [%u:%u]",
                                                      cannet->addr, socket->addr_remote, socket->port);

                                                err = ECONNABORTED;
                                        }
                                } else {
                                        DEBUG("[%u] socket_connect: timeout, socket [%u:%u]",
                                              cannet->addr, socket->addr_remote, socket->port);
                                }
                        }
                }

                if (err) {
                        unregister_socket(cannet, socket);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function disconnect socket from selected address. Client side function.
 * @param  socket     socket
 * @return One of @ref errno value.
 */
//==============================================================================
int CANNET_socket_disconnect(void *ctx, CANNET_socket_t *socket)
{
        cannet_t *cannet = ctx;

        int err = EINVAL;

        if (is_socket_registered(cannet, socket)) {
                err = send_disconnect_frame(cannet, socket);
                if (!err) {
                        socket->connected = false;

                        DEBUG("[%u] socket [%u:%u] disconnected",
                              cannet->addr, socket->addr_remote, socket->port);
                }

                unregister_socket(cannet, socket);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function shutdown selected socket direction. Client/host side function.
 * @param  socket    socket
 * @param  how          shutdown direction
 * @return One of @ref errno value.
 */
//==============================================================================
int CANNET_socket_shutdown(void *ctx, CANNET_socket_t *socket, NET_shut_t how)
{
        UNUSED_ARG1(ctx);

        socket->shutdown = how;

        return ENOTSUP;
}

//==============================================================================
/**
 * @brief  Function bind selected address with socket. Host side function.
 * @param  socket     socket
 * @param  addr          inet address
 * @return One of @ref errno value.
 */
//==============================================================================
int CANNET_socket_bind(void *ctx, CANNET_socket_t *socket, const NET_CANNET_sockaddr_t *addr)
{
        cannet_t *cannet = ctx;

        socket->addr_remote = addr->addr;
        socket->port = addr->port;

        int err = ESUCC;

        if (socket->protocol == NET_PROTOCOL__DATAGRAM) {
                err = register_socket(cannet, socket);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function listen connection. Host side function.
 * @param  socket        socket
 * @return One of @ref errno value.
 */
//==============================================================================
int CANNET_socket_listen(void *ctx, CANNET_socket_t *socket)
{
        cannet_t *cannet = ctx;

        int err = ESUCC;

        if (socket->protocol == NET_PROTOCOL__STREAM) {
                err = register_socket(cannet, socket);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function accept incoming connection. Host side function.
 * @param  socket       socket
 * @param  new_socket   new socket of accepted connection
 * @return One of @ref errno value.
 */
//==============================================================================
int CANNET_socket_accept(void *ctx, CANNET_socket_t *socket, CANNET_socket_t *new_socket)
{
        cannet_t *cannet = ctx;

        int err = ECANCELED;

        if (socket->protocol == NET_PROTOCOL__DATAGRAM) {
                return err;
        }

        while (true) {
                answer_t answer;
                err = wait_for_peer_answer(cannet, socket, &answer, CONNECTION_TIMEOUT_ms);

                if (not err
                    and (get_frame_type(&answer.generic.header) == frame_type__connect)
                    and (answer.connect.flags == 0) ) {

                        err = CANNET_socket_create(cannet, socket->protocol, new_socket);
                        if (!err) {
                                new_socket->addr_remote = answer.addr_src;
                                new_socket->port = socket->port;
                                new_socket->connected = true;

                                err = register_socket(cannet, new_socket);
                                if (!err) {
                                        u8_t flags = CONNECT_FRAME_FLAG__RESPONSE
                                                   | CONNECT_FRAME_FLAG__CONNECTED;

                                        err = send_connect_frame(cannet, new_socket, flags);
                                        if (!err) {
                                                break;

                                        } else {
                                                unregister_socket(cannet, new_socket);
                                        }
                                }

                                if (err) {
                                        CANNET_socket_destroy(cannet, new_socket);
                                }
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function receive incoming data. Client/Host side function.
 * @param  socket       socket
 * @param  buf          buffer for data
 * @param  len          number of data to receive
 * @param  flags        flags
 * @param  recved       number of received bytes
 * @return One of @ref errno value.
 */
//==============================================================================
int CANNET_socket_recv(void            *ctx,
                       CANNET_socket_t *socket,
                       void            *buf,
                       size_t           len,
                       NET_flags_t      flags,
                       size_t          *recved)
{
        NET_CANNET_sockaddr_t sockaddr;
        return CANNET_socket_recvfrom(ctx, socket, buf, len, flags, &sockaddr, recved);
}

//==============================================================================
/**
 * @brief  Function receive data from selected address. Client/Host side function.
 * @param  socket       socket
 * @param  buf          buffer for data
 * @param  len          number of bytes to receive
 * @param  flags        flags
 * @param  sockaddr     socket address
 * @param  recved       number of received bytes
 * @return One of @ref errno value.
 */
//==============================================================================
int CANNET_socket_recvfrom(void                  *ctx,
                           CANNET_socket_t       *socket,
                           void                  *buf,
                           size_t                 len,
                           NET_flags_t            flags,
                           NET_CANNET_sockaddr_t *sockaddr,
                           size_t                *recved)
{
        cannet_t *cannet = ctx;

        if (not is_socket_registered(cannet, socket)) {
                return ENOENT;
        }

        if (socket->shutdown & NET_SHUT__RD) {
                return EPERM;
        }

        if (not socket->connected) {
                return ECONNREFUSED;
        }

        *recved = 0;
        u32_t tref = sys_time_get_reference();

        sockaddr->addr = 0;
        sockaddr->port = socket->port;

        int err = ETIME;

        while (!sys_time_is_expired(tref, socket->recv_timeout) and (len > 0)) {

                err = cannetbuf__read(socket->rx_buf, buf, len, recved);
                if (!err) {
                        if (*recved > 0) {
                                sockaddr->addr = socket->datagram_source;
                                break;

                        } else {
                                if (not socket->connected) {
                                        err = ECONNREFUSED;
                                        break;
                                } else {
                                        err = ETIME;
                                        sys_sleep_ms(5);
                                }
                        }
                }
        }

        if (flags & NET_FLAGS__FREEBUF) {
                cannetbuf__clear(socket->rx_buf);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function send data to connected socket. Client/Host side function.
 * @param  socket    socket
 * @param  buf          buffer to send
 * @param  len          number of bytes to send
 * @param  flags        flags
 * @param  sent         number of sent bytes
 * @return One of @ref errno value.
 */
//==============================================================================
int CANNET_socket_send(void            *ctx,
                       CANNET_socket_t *socket,
                       const void      *buf,
                       size_t           len,
                       NET_flags_t      flags,
                       size_t          *sent)
{
        UNUSED_ARG1(flags);

        cannet_t *cannet = ctx;

        if (not is_socket_registered(cannet, socket)) {
                return ENOENT;
        }

        if (socket->shutdown & NET_SHUT__WR) {
                return EPERM;
        }

        if (socket->protocol != NET_PROTOCOL__STREAM) {
                return ECANCELED;
        }

        if (not socket->connected) {
                return ECONNREFUSED;
        }

        int err = 0;

        while (!err and (len > 0)) {
                size_t chunk_len = 0;

                for (int try = 0; try < 3 and !err; try++) {
                        err = send_stat_frame(cannet, socket, 0);
                        if (!err) {
                                answer_t answer;
                                err = wait_for_peer_answer(cannet, socket, &answer, STAT_TIMEOUT_ms);
                                if (!err) {
                                        if (get_frame_type(&answer.generic.header) == frame_type__stat) {
                                                chunk_len = min(answer.stat.available, len);
                                                err = 0;
                                                break;

                                        } else {
                                                err = EILSEQ;
                                        }
                                } else {
                                        DEBUG("[%u] stat error %i", cannet->addr, err);
                                }
                        }
                }

                for (int try = 0; try < 3 and !err; try++) {
                        err = send_payload(cannet, socket, buf, chunk_len);
                        if (!err) {
                                answer_t answer;
                                err = wait_for_peer_answer(cannet, socket, &answer, ACK_TIMEOUT_ms);
                                if (!err) {
                                        if (get_frame_type(&answer.generic.header) == frame_type__ack) {

                                                if (answer.ack.flags == 0) {
                                                        buf   += chunk_len;
                                                        len   -= chunk_len;
                                                        *sent += chunk_len;
                                                        err = 0;
                                                        break;
                                                }
                                        } else {
                                                err = EILSEQ;
                                        }
                                } else {
                                        // try again sending whole chunk
                                        err = 0;
                                }
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function send buffer to selected address. Client/Host side function.
 * @param  socket    socket
 * @param  buf          buffer to send
 * @param  len          number of bytes to send
 * @param  flags        flags
 * @param  to_sockaddr  socket address
 * @param  sent         number of sent bytes
 * @return One of @ref errno value.
 */
//==============================================================================
int CANNET_socket_sendto(void                        *ctx,
                         CANNET_socket_t             *socket,
                         const void                  *buf,
                         size_t                       len,
                         NET_flags_t                  flags,
                         const NET_CANNET_sockaddr_t *to_sockaddr,
                         size_t                      *sent)
{
        UNUSED_ARG1(flags);

        cannet_t *cannet = ctx;

        if (socket->shutdown & NET_SHUT__WR) {
                return EPERM;
        }

        if (socket->protocol != NET_PROTOCOL__DATAGRAM) {
                return ECANCELED;
        }

        int err = ESUCC;

        if (not is_socket_registered(cannet, socket)) {
                socket->addr_remote = to_sockaddr->addr;
                socket->port = to_sockaddr->port;
                err = register_socket(cannet, socket);
        }

        if (!err) {
                CANNET_socket_t tmpsoc = *socket;
                tmpsoc.addr_remote = to_sockaddr->addr;

                err = send_payload(cannet, &tmpsoc, buf, len);
                if (!err) {
                        *sent = len;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function gets host address by name. Client side function.
 * @param  name         address name
 * @param  addr         received address
 * @return One of @ref errno value.
 */
//==============================================================================
int CANNET_gethostbyname(void *ctx, const char *name, NET_CANNET_sockaddr_t *sock_addr)
{
        UNUSED_ARG3(ctx, name, sock_addr);
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
int CANNET_socket_set_recv_timeout(void *ctx, CANNET_socket_t *socket, uint32_t timeout)
{
        UNUSED_ARG1(ctx);
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
int CANNET_socket_set_send_timeout(void *ctx, CANNET_socket_t *socket, uint32_t timeout)
{
        UNUSED_ARG1(ctx);
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
int CANNET_socket_get_recv_timeout(void *ctx, CANNET_socket_t *socket, uint32_t *timeout)
{
        UNUSED_ARG1(ctx);
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
int CANNET_socket_get_send_timeout(void *ctx, CANNET_socket_t *socket, uint32_t *timeout)
{
        UNUSED_ARG1(ctx);
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
int CANNET_socket_getaddress(void *ctx, CANNET_socket_t *socket, NET_CANNET_sockaddr_t *sockaddr)
{
        UNUSED_ARG1(ctx);
        sockaddr->addr = socket->addr_remote;
        sockaddr->port = socket->port;
        return EINVAL;
}

//==============================================================================
/**
 * @brief  Function convert value for host/network purpose.
 * @param  value        value to convert
 * @return Converted value.
 */
//==============================================================================
u16_t CANNET_hton_u16(u16_t value)
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
u32_t CANNET_hton_u32(u32_t value)
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
u64_t CANNET_hton_u64(u64_t value)
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

//==============================================================================
/**
 * @brief  Function register socket.
 *
 * @param  cannet       stack instance
 * @param  socket       socket to register
 *
 * @return One of errno value.
 */
//==============================================================================
static int register_socket(cannet_t *cannet, CANNET_socket_t *socket)
{
        int err = EINVAL;

        if (socket->port > NET_CANNET_MAX_PORT) {
                return err;
        }

        err = sys_mutex_lock(cannet->mutex, MUTEX_TIMEOUT_ms);
        if (!err) {
                // check if socket exist
                for (size_t i = 0; i < ARRAY_SIZE(cannet->sockets); i++) {
                        CANNET_socket_t *s = cannet->sockets[i];
                        if (s and (s->addr_remote == socket->addr_remote)
                              and (s->port == socket->port)) {

                                DEBUG("[%u] unable to register socket [%u:%u] twice",
                                      cannet->addr, socket->addr_remote, socket->port);

                                err = EADDRINUSE;

                                break;
                        }
                }

                // if not exist then register in first empty slot
                if (!err) {
                        for (size_t i = 0; i < ARRAY_SIZE(cannet->sockets); i++) {
                                if (cannet->sockets[i] == NULL) {
                                        cannet->sockets[i] = socket;

                                        DEBUG("[%u] registered socket [%u:%u]",
                                              cannet->addr, socket->addr_remote, socket->port);

                                        break;
                                }
                        }
                }

                sys_mutex_unlock(cannet->mutex);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function unregister socket.
 *
 * @param  cannet       stack instance
 * @param  socket       socket to unregister
 *
 * @return One of errno value.
 */
//==============================================================================
static int unregister_socket(cannet_t *cannet, CANNET_socket_t *socket)
{
        int err = EINVAL;

        err = sys_mutex_lock(cannet->mutex, MUTEX_TIMEOUT_ms);
        if (!err) {
                for (size_t i = 0; i < ARRAY_SIZE(cannet->sockets); i++) {
                        if (cannet->sockets[i] == socket) {
                                cannet->sockets[i] = NULL;

                                DEBUG("[%u] unregistered socket [%u:%u]",
                                      cannet->addr, socket->addr_remote, socket->port);

                                break;
                        }
                }

                sys_mutex_unlock(cannet->mutex);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function unregister socket.
 *
 * @param  cannet       stack instance
 * @param  socket       socket to check
 *
 * @return True/False.
 */
//==============================================================================
static bool is_socket_registered(cannet_t *cannet, CANNET_socket_t *socket)
{
        bool registered = false;

        if (sys_mutex_lock(cannet->mutex, MUTEX_TIMEOUT_ms) == 0) {
                for (size_t i = 0; i < ARRAY_SIZE(cannet->sockets); i++) {
                        if (cannet->sockets[i] == socket) {
                                registered = true;
                                break;
                        }
                }

                sys_mutex_unlock(cannet->mutex);
        }

        return registered;
}

//==============================================================================
/**
 * @brief  Function send payload to destination address.
 *
 * @param  cannet               stack object
 * @param  socket               send socket
 * @param  dest_address         destination address
 * @param  port                 port
 * @param  payload              payload to send
 * @param  payload_len          payload length in bytes
 *
 * @return One of errno value.
 */
//==============================================================================
static int send_payload(cannet_t *cannet, CANNET_socket_t *socket,
                        const void *payload, uint16_t payload_len)
{
        int err = EINVAL;

        DEBUG("[%u] sending payload %u bytes by socket [%u:%u]", cannet->addr,
              payload_len, socket->addr_remote, socket->port);

        payload_len = payload ? payload_len : 0;

        if (payload_len <= sizeof(((single_frame_t*)NULL)->data)) {
                err = send_single_frame(cannet, socket, &payload, &payload_len);

        } else {
                err = send_first_frame(cannet, socket, &payload, &payload_len);

                while (!err and (payload_len > sizeof((last_frame_t*)NULL)->data)) {
                        err = send_next_frame(cannet, socket, &payload, &payload_len);
                }

                if (!err) {
                        send_last_frame(cannet, socket, &payload, &payload_len);
                }
        }

        DEBUG("[%u] sent payload socket [%u:%u] finished: %i",
              cannet->addr, socket->addr_remote, socket->port, err);

        return err;
}

//==============================================================================
/**
 * @brief  Return frame type.
 *
 * @param  header       header
 *
 * @return Frame type.
 */
//==============================================================================
static inline frame_type_t get_frame_type(const header_t *header)
{
        return ((*header) >> 5) & 7;
}

//==============================================================================
/**
 * @brief  Set frame type.
 *
 * @param  header       header to set
 * @param  type         frame type
 */
//==============================================================================
static inline void set_frame_type(header_t *header, frame_type_t type)
{
        *header &= 0x1F;
        *header |= ((type & 7) << 5);
}

//==============================================================================
/**
 * @brief  Get frame sequence number.
 *
 * @param  header       header
 *
 * @return Sequence number.
 */
//==============================================================================
static inline uint8_t get_frame_sequence(const header_t *header)
{
        return *header & 3;
}

//==============================================================================
/**
 * @brief  Set frame sequence number.
 *
 * @param  header       header to set
 * @param  sequence     sequence number
 */
//==============================================================================
static inline void set_frame_sequence(header_t *header, uint8_t sequence)
{
        *header &= ~3;
        *header |= (sequence & 3);
}

//==============================================================================
/**
 * @brief  Set frame port number.
 *
 * @param  header       header to set
 * @param  port         port number
 */
//==============================================================================
static inline void set_frame_port(header_t *header, uint8_t port)
{
        *header &= ~(7 << 2);
        *header |= ((port & 7) << 2);
}

//==============================================================================
/**
 * @brief  Get frame port number.
 *
 * @param  header       header to set
 */
//==============================================================================
static inline uint8_t get_frame_port(const header_t *header)
{
        return (*header >> 2) & 7;
}

//==============================================================================
/**
 * @brief  Set header.
 *
 * @param  header       header to set
 * @param  type         frame type
 * @param  port         port
 * @param  sequence     sequence
 */
//==============================================================================
static inline void set_header(header_t *header, frame_type_t type, uint8_t port, uint8_t sequence)
{
        *header = 0;
        set_frame_type(header, type);
        set_frame_port(header, port);
        set_frame_sequence(header, sequence);
}

//==============================================================================
/**
 * @brief  Calculate CCITT CRC16.
 *
 * @param  buf          buffer to calculate
 * @param  len          buffer length
 * @param  crc_in       init value or last calculated CRC
 *
 * @return CRC16.
 */
//==============================================================================
static uint16_t calculate_crc16(const void *buf, uint16_t len, uint16_t crc_in)
{
        const uint8_t *b = buf;
        uint16_t   crc16 = crc_in;

        while (len-- != 0) {
                crc16 = crc16_ccitt_table[((crc16 >> 8) ^ *b++) & 0xff] ^ (crc16 << 8);
        }

        return crc16;
}

//==============================================================================
/**
 * @brief  Set can ID according to destination and source addresses.
 *
 * @param  destination          device destination address
 * @param  source               device source address
 *
 * @return CAN ID.
 */
//==============================================================================
static inline uint32_t set_can_id(uint16_t destination, uint16_t source)
{
        return (((uint32_t)source & 0x3FFF) << 14) | (((uint32_t)destination & 0x3FFF) << 0);
}

//==============================================================================
/**
 * @brief  Return destination address extracted from CAN ID.
 *
 * @param  can_id               CAN ID
 *
 * @return Device destination address.
 */
//==============================================================================
static inline uint16_t get_destination_address(uint32_t can_id)
{
        return can_id & 0x3FFF;
}

//==============================================================================
/**
 * @brief  Return source address extracted from CAN ID.
 *
 * @param  can_id               CAN ID
 *
 * @return Device source address.
 */
//==============================================================================
static inline uint16_t get_source_address(uint32_t can_id)
{
        return ((can_id >> 14) & 0x3FFF);
}

//==============================================================================
/**
 * @brief  Function send RAW can frame.
 *
 * @param  cannet               stack object
 * @param  can_frame            raw can frame
 *
 * @return One of errno value.
 */
//==============================================================================
static int send_can_frame(cannet_t *cannet, CAN_msg_t *can_frame)
{
        int err = EFAULT;

        uint seed = (int)cannet->stats.tx_packets ^ (int)cannet->stats.tx_bytes;

        for (int i = 0; i < __NETWORK_CANNET_SEND_REPETITIONS__; i++) {

                err = sys_ioctl(cannet->if_file, IOCTL_CAN__SEND_MSG, can_frame);
                if (!err) {
                        cannet->stats.tx_packets++;
                        cannet->stats.tx_bytes += can_frame->data_length;
                        break;

                } else {
                        u32_t min = __NETWORK_CANNET_SEND_REPETITIONS_INTERVAL_MIN__;
                        u32_t max = __NETWORK_CANNET_SEND_REPETITIONS_INTERVAL_MAX__;

                        u32_t delay_ms = min + (sys_rand_r(&seed) % max);
                        sys_sleep_ms(delay_ms);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Send first frame.
 *
 * @param  cannet               stack object
 * @param  socket               send socket
 * @param  payload              payload buffer
 * @param  payload_len          payload length
 *
 * @return One of errno value.
 */
//==============================================================================
static int send_first_frame(cannet_t *cannet, CANNET_socket_t *socket,
                            const void **payload, u16_t *payload_len)
{
        CAN_msg_t can_frame;
        can_frame.extended_ID = true;
        can_frame.remote_transmission = false;
        can_frame.ID = set_can_id(socket->addr_remote, cannet->addr);
        can_frame.data_length = sizeof(first_frame_t);

        first_frame_t *frame = cast(first_frame_t *, can_frame.data);

        socket->seq_tx = 0;

        DEBUG("[%u]->[%u] first-frame: port:%u seq:%u len:%u", cannet->addr,
              socket->addr_remote, socket->port, socket->seq_tx, *payload_len);

        set_header(&frame->header, frame_type__first, socket->port, socket->seq_tx++);

        frame->transfer_size = *payload_len;
        frame->transfer_crc16 = calculate_crc16(*payload, *payload_len, crc16_ccitt_start);

        size_t len = sizeof(frame->data);
        memcpy(frame->data, *payload, len);
        *payload += len;
        *payload_len -= len;

        return send_can_frame(cannet, &can_frame);
}

//==============================================================================
/**
 * @brief  Send next frame.
 *
 * @param  cannet               stack object
 * @param  socket               send socket
 * @param  payload              payload buffer
 * @param  payload_len          payload length
 *
 * @return One of errno value.
 */
//==============================================================================
static int send_next_frame(cannet_t *cannet, CANNET_socket_t *socket,
                           const void **payload, u16_t *payload_len)
{
        CAN_msg_t can_frame;
        can_frame.extended_ID = true;
        can_frame.remote_transmission = false;
        can_frame.ID = set_can_id(socket->addr_remote, cannet->addr);
        can_frame.data_length = sizeof(first_frame_t);

        next_frame_t *frame = cast(next_frame_t *, can_frame.data);

        DEBUG("[%u]->[%u] next-frame: port:%u seq:%u len:%u", cannet->addr,
              socket->addr_remote, socket->port, socket->seq_tx, *payload_len);

        set_header(&frame->header, frame_type__next, socket->port, socket->seq_tx++);

        size_t len = sizeof(frame->data);
        memcpy(frame->data, *payload, len);
        *payload += len;
        *payload_len -= len;

        return send_can_frame(cannet, &can_frame);
}

//==============================================================================
/**
 * @brief  Send last frame.
 *
 * @param  cannet               stack object
 * @param  socket               send socket
 * @param  payload              payload buffer
 * @param  payload_len          payload length
 *
 * @return One of errno value.
 */
//==============================================================================
static int send_last_frame(cannet_t *cannet, CANNET_socket_t *socket,
                           const void **payload, u16_t *payload_len)
{
        CAN_msg_t can_frame;
        can_frame.extended_ID = true;
        can_frame.remote_transmission = false;
        can_frame.ID = set_can_id(socket->addr_remote, cannet->addr);
        can_frame.data_length = *payload_len + 1;

        last_frame_t *frame = cast(last_frame_t *, can_frame.data);

        DEBUG("[%u]->[%u] last-frame: port:%u seq:%u len:%u", cannet->addr,
              socket->addr_remote, socket->port, socket->seq_tx, *payload_len);

        set_header(&frame->header, frame_type__last, socket->port, socket->seq_tx++);

        size_t len = min(sizeof(frame->data), *payload_len);
        memcpy(frame->data, *payload, len);
        *payload += len;
        *payload_len -= len;

        return send_can_frame(cannet, &can_frame);

}

//==============================================================================
/**
 * @brief  Send single frame.
 *
 * @param  cannet               stack object
 * @param  socket               send socket
 * @param  payload              payload buffer
 * @param  payload_len          payload length
 *
 * @return One of errno value.
 */
//==============================================================================
static int send_single_frame(cannet_t *cannet, CANNET_socket_t *socket,
                             const void **payload, u16_t *payload_len)
{
        CAN_msg_t can_frame;
        can_frame.extended_ID = true;
        can_frame.remote_transmission = false;
        can_frame.ID = set_can_id(socket->addr_remote, cannet->addr);
        can_frame.data_length = *payload_len + 1;

        single_frame_t *frame = cast(single_frame_t *, can_frame.data);

        socket->seq_tx = 0;

        DEBUG("[%u]->[%u] single-frame: port:%u seq:%u len:%u", cannet->addr,
              socket->addr_remote, socket->port, socket->seq_tx, *payload_len);

        set_header(&frame->header, frame_type__single, socket->port, socket->seq_tx);

        size_t len = min(sizeof(frame->data), *payload_len);
        memcpy(frame->data, *payload, len);
        *payload += len;
        *payload_len -= len;

        return send_can_frame(cannet, &can_frame);
}

//==============================================================================
/**
 * @brief  Send connect frame.
 *
 * @param  cannet               stack context
 * @param  socket               send socket
 * @param  flags                flags
 *
 * @return One of errno value.
 */
//==============================================================================
static int send_connect_frame(cannet_t *cannet, CANNET_socket_t *socket, u8_t flags)
{
        DEBUG("[%u]->[%u] connect-frame: port:%u flags:%02Xh", cannet->addr,
              socket->addr_remote, socket->port, flags);

        CAN_msg_t can_frame;
        can_frame.extended_ID = true;
        can_frame.remote_transmission = false;
        can_frame.ID = set_can_id(socket->addr_remote, cannet->addr);
        can_frame.data_length = sizeof(connect_frame_t);

        connect_frame_t *frame = cast(connect_frame_t *, can_frame.data);
        frame->flags = flags;

        socket->seq_tx = 0;
        set_header(&frame->header, frame_type__connect, socket->port, socket->seq_tx++);

        sys_queue_reset(socket->ansq);

        return send_can_frame(cannet, &can_frame);
}

//==============================================================================
/**
 * @brief  Send connect frame.
 *
 * @param  cannet               stack object
 * @param  socket               send socket
 *
 * @return One of errno value.
 */
//==============================================================================
static int send_disconnect_frame(cannet_t *cannet, CANNET_socket_t *socket)
{
        DEBUG("[%u]->[%u] disconnect-frame: port:%u", cannet->addr,
              socket->addr_remote, socket->port);

        CAN_msg_t can_frame;
        can_frame.extended_ID = true;
        can_frame.remote_transmission = false;
        can_frame.ID = set_can_id(socket->addr_remote, cannet->addr);
        can_frame.data_length = sizeof(disconnect_frame_t);

        disconnect_frame_t *frame = cast(disconnect_frame_t *, can_frame.data);

        socket->seq_tx = 0;
        set_header(&frame->header, frame_type__disconnect, socket->port, socket->seq_tx++);

        return send_can_frame(cannet, &can_frame);
}

//==============================================================================
/**
 * @brief  Send stat frame.
 *
 * @param  cannet               stack object
 * @param  socket               send socket
 * @param  flags                flags
 *
 * @return One of errno value.
 */
//==============================================================================
static int send_stat_frame(cannet_t *cannet, CANNET_socket_t *socket, u8_t flags)
{
        size_t available = 0;
        cannetbuf__get_available(socket->rx_buf, &available);

        DEBUG("[%u]->[%u] stat-frame: port:%u flags:%02Xh avail:%u", cannet->addr,
              socket->addr_remote, socket->port, flags, available);

        CAN_msg_t can_frame;
        can_frame.extended_ID = true;
        can_frame.remote_transmission = false;
        can_frame.ID = set_can_id(socket->addr_remote, cannet->addr);
        can_frame.data_length = sizeof(stat_frame_t);

        stat_frame_t *frame = cast(stat_frame_t *, can_frame.data);
        frame->flags = flags;
        frame->available = available;

        socket->seq_tx = 0;
        set_header(&frame->header, frame_type__stat, socket->port, socket->seq_tx++);

        sys_queue_reset(socket->ansq);

        return send_can_frame(cannet, &can_frame);
}

//==============================================================================
/**
 * @brief  Send ack frame.
 *
 * @param  cannet               stack context
 * @param  socket               send socket
 * @param  flags                flags
 *
 * @return One of errno value.
 */
//==============================================================================
static int send_ack_frame(cannet_t *cannet, CANNET_socket_t *socket, u8_t flags)
{
        DEBUG("[%u]->[%u] ack-frame: port:%u flags:%02Xh", cannet->addr,
              socket->addr_remote, socket->port, flags);

        CAN_msg_t can_frame;
        can_frame.extended_ID = true;
        can_frame.remote_transmission = false;
        can_frame.ID = set_can_id(socket->addr_remote, cannet->addr);
        can_frame.data_length = sizeof(ack_frame_t);

        ack_frame_t *frame = cast(ack_frame_t *, can_frame.data);
        frame->flags = flags;

        socket->seq_tx = 0;
        set_header(&frame->header, frame_type__ack, socket->port, socket->seq_tx++);

        return send_can_frame(cannet, &can_frame);
}

//==============================================================================
/**
 * @brief  Function receive answer from network.
 *
 * @param  cannet       cannet instance
 * @param  socket       socket
 * @param  answer       answer
 * @param  timeout      timeout
 *
 * @return One of errno.
 */
//==============================================================================
static int wait_for_peer_answer(cannet_t *cannet, CANNET_socket_t *socket, answer_t *answer, u32_t timeout)
{
        UNUSED_ARG1(cannet);

        int err = sys_queue_receive(socket->ansq, answer, timeout);

        DEBUG("[%u] answer receive for socket [%u:%u] %s (%d)",
              cannet->addr, socket->addr_remote, socket->port, err ? "fail" : "success", err);

        return err;
}

//==============================================================================
/**
 * @brief  Function handle incoming frame.
 *
 * @param  cannet       cannet instance
 * @param  can_frame    RAW CAN frame
 */
//==============================================================================
static void handle_incoming_frame(cannet_t *cannet, CAN_msg_t *can_frame)
{
        if (sys_mutex_lock(cannet->mutex, MUTEX_TIMEOUT_ms) != 0) {
                return;
        }

        cannet->stats.rx_packets++;
        cannet->stats.rx_bytes += can_frame->data_length;

        u16_t dst_addr  = get_destination_address(can_frame->ID);
        u16_t src_addr  = get_source_address(can_frame->ID);
        bool  broadcast = (dst_addr == NET_CANNET_ADDR_BROADCAST);

        if ((dst_addr == cannet->addr) or broadcast) {

                generic_frame_t *generic = cast(generic_frame_t*, can_frame->data);

                frame_type_t type = get_frame_type(&generic->header);
                u8_t  port        = get_frame_port(&generic->header);

                #if __NETWORK_CANNET_DEBUG_ON__
                static const char *frame[] = {
                        "first-frame",
                        "next-frame",
                        "last-frame",
                        "single-frame",
                        "connect-frame",
                        "disconnect-frame",
                        "stat-frame",
                        "ack-frame",
                };
                #endif

                DEBUG("[%u]<-[%u] port:%u seq:%u type:%s",
                      cannet->addr, src_addr, get_frame_port(&generic->header),
                      get_frame_sequence(&generic->header), frame[type]);


                CANNET_socket_t *socket = NULL;

                for (size_t i = 0; i < ARRAY_SIZE(cannet->sockets); i++) {
                        CANNET_socket_t *s = cannet->sockets[i];
                        if (s and (s->addr_remote == src_addr) and (s->port == port)) {
                                socket = s;
                                break;
                        }
                }

                if (socket == NULL) {
                        for (size_t i = 0; i < ARRAY_SIZE(cannet->sockets); i++) {
                                CANNET_socket_t *s = cannet->sockets[i];
                                if (s and (s->addr_remote == NET_CANNET_ADDR_ANY) and (s->port == port)) {
                                        socket = s;
                                        break;
                                }
                        }
                }


                if (socket) {
                        if (broadcast and (socket->protocol == NET_PROTOCOL__STREAM)) {
                                DEBUG("[%u] broadcast is not allowed in stream mode, socket [%u:%u]",
                                      cannet->addr, src_addr, port);

                        } else {
                                socket->datagram_source = src_addr;

                                if (socket->connected) {
                                        if ((can_frame->data_length == sizeof(first_frame_t)) && (type == frame_type__first)) {
                                                received_first_frame(cannet, socket, (first_frame_t*)generic);

                                        } else if ((can_frame->data_length == sizeof(next_frame_t)) && (type == frame_type__next)) {
                                                received_next_frame(cannet, socket, (next_frame_t*)generic);

                                        } else if ((can_frame->data_length >= 2) && (type == frame_type__last)) {
                                                received_last_frame(cannet, socket, (last_frame_t*)generic, can_frame->data_length - 1);

                                        } else if ((can_frame->data_length >= 1) && (type == frame_type__single)) {
                                                received_single_frame(cannet, socket, (single_frame_t*)generic, can_frame->data_length - 1);

                                        } else if ((can_frame->data_length == sizeof(disconnect_frame_t)) && (type == frame_type__disconnect) ) {
                                                received_disconnect_frame(cannet, socket, (disconnect_frame_t*)generic);

                                        } else if ((can_frame->data_length == sizeof(ack_frame_t)) && (type == frame_type__ack) ) {
                                                received_ack_frame(cannet, socket, (ack_frame_t*)generic);

                                        } else if ((can_frame->data_length == sizeof(stat_frame_t)) && (type == frame_type__stat) ) {
                                                received_stat_frame(cannet, socket, (stat_frame_t*)generic);

                                        } else if ((can_frame->data_length == sizeof(connect_frame_t)) && (type == frame_type__connect) ) {
                                                DEBUG("[%u] reconnect request", cannet->addr);
                                                received_connect_frame(cannet, socket, src_addr, (connect_frame_t*)generic);

                                        } else {
                                                DEBUG("[%u] received incorrect frame", cannet->addr);
                                        }
                                } else {
                                        if ((can_frame->data_length == sizeof(connect_frame_t)) && (type == frame_type__connect) ) {
                                                received_connect_frame(cannet, socket, src_addr, (connect_frame_t*)generic);
                                        }
                                }
                        }
                } else {
                        DEBUG("[%u] socket [%u:%u] not found", cannet->addr, src_addr, port);
                }
        }

        sys_mutex_unlock(cannet->mutex);
}

//==============================================================================
/**
 * @brief  Handle first frame. This frame setup reception.
 *
 * @param  cannet               stack object
 * @param  src_addr             source address
 * @param  frame                frame
 *
 * @return One of errno.
 */
//==============================================================================
static int received_first_frame(cannet_t *cannet, CANNET_socket_t *socket, const first_frame_t *frame)
{
        UNUSED_ARG1(cannet);

        if (get_frame_sequence(&frame->header) != 0) {
                DEBUG("[%u] incorrect frame sequence %u expected %u",
                      cannet->addr, get_frame_sequence(&frame->header), 0);
                return EILSEQ;
        }

        socket->seq_rx = 1;
        socket->expected_crc = frame->transfer_crc16;
        socket->crc = calculate_crc16(frame->data, sizeof(frame->data), crc16_ccitt_start);

#if __NETWORK_CANNET_DEBUG_ON__
        char buf[32];
        for (size_t i = 0; i < sizeof(frame->data); i++) {
                sys_snprintf(buf + (i * 3), sizeof(buf) - (i * 3), "%02X ", frame->data[i]);
        }
        DEBUG("Payload [ %s]", buf);
#endif

        cannetbuf__clear(socket->assembly_buf);
        return cannetbuf__write(socket->assembly_buf, frame->data, sizeof(frame->data));
}

//==============================================================================
/**
 * @brief  Handle next frame.
 *
 * @param  self                 stack object
 * @param  source               source address
 * @param  frame                frame
 *
 * @return One of errno.
 */
//==============================================================================
static int received_next_frame(cannet_t *cannet, CANNET_socket_t *socket, const next_frame_t *frame)
{
        UNUSED_ARG1(cannet);

        if (get_frame_sequence(&frame->header) != socket->seq_rx) {
                DEBUG("[%u] incorrect frame sequence %u expected %u",
                      cannet->addr, get_frame_sequence(&frame->header), socket->seq_rx);
                return EILSEQ;
        }

        socket->seq_rx++;
        socket->seq_rx &= 3;
        socket->crc = calculate_crc16(frame->data, sizeof(frame->data), socket->crc);

#if __NETWORK_CANNET_DEBUG_ON__
        char buf[32];
        for (size_t i = 0; i < sizeof(frame->data); i++) {
                sys_snprintf(buf + (i * 3), sizeof(buf) - (i * 3), "%02X ", frame->data[i]);
        }
        DEBUG("Payload [ %s]", buf);
#endif

        return cannetbuf__write(socket->assembly_buf, frame->data, sizeof(frame->data));
}

//==============================================================================
/**
 * @brief  Handle last frame.
 *
 * @param  self                 stack object
 * @param  source               source address
 * @param  frame                frame
 *
 * @return One of errno.
 */
//==============================================================================
static int received_last_frame(cannet_t *cannet, CANNET_socket_t *socket, const last_frame_t *frame, u8_t len)
{
        if (get_frame_sequence(&frame->header) != socket->seq_rx) {
                DEBUG("[%u] incorrect frame sequence %u expected %u",
                      cannet->addr, get_frame_sequence(&frame->header), socket->seq_rx);
                return EILSEQ;
        }

        socket->seq_rx++;
        socket->seq_rx &= 3;
        socket->crc = calculate_crc16(frame->data, len, socket->crc);

#if __NETWORK_CANNET_DEBUG_ON__
        char buf[32];
        for (size_t i = 0; i < len; i++) {
                sys_snprintf(buf + (i * 3), sizeof(buf) - (i * 3), "%02X ", frame->data[i]);
        }
        DEBUG("Payload [ %s]", buf);
#endif

        int err = cannetbuf__write(socket->assembly_buf, frame->data, len);
        if (!err) {
                if (socket->crc == socket->expected_crc) {
                        err = cannetbuf__move(socket->rx_buf, socket->assembly_buf);
                } else {
                        DEBUG("  incorrect payload CRC");
                        err = EAGAIN;
                }

                if (socket->protocol == NET_PROTOCOL__STREAM) {
                        send_ack_frame(cannet, socket, err);
                }

                cannetbuf__clear(socket->assembly_buf);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Handle single frame.
 *
 * @param  self                 stack object
 * @param  source               source address
 * @param  frame                frame
 *
 * @return One of errno value.
 */
//==============================================================================
static int received_single_frame(cannet_t *cannet, CANNET_socket_t *socket, const single_frame_t *frame, u8_t len)
{
        if (get_frame_sequence(&frame->header) != 0) {
                DEBUG("[%u] incorrect frame sequence %u expected %u",
                      cannet->addr, get_frame_sequence(&frame->header), 0);
                return EILSEQ;
        }

#if __NETWORK_CANNET_DEBUG_ON__
        char buf[32];
        for (size_t i = 0; i < len; i++) {
                sys_snprintf(buf + (i * 3), sizeof(buf) - (i * 3), "%02X ", frame->data[i]);
        }
        DEBUG("Payload [ %s]", buf);
#endif

        int err = cannetbuf__write(socket->rx_buf, frame->data, len);

        if (socket->protocol == NET_PROTOCOL__STREAM) {
                send_ack_frame(cannet, socket, err);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Handle connect frame.
 *
 * @param  self                 stack object
 * @param  source               source address
 * @param  frame                frame
 *
 * @return One of errno value.
 */
//==============================================================================
static int received_connect_frame(cannet_t *cannet, CANNET_socket_t *socket, u16_t src_addr, const connect_frame_t *frame)
{
        if (get_frame_sequence(&frame->header) != 0) {
                DEBUG("[%u] incorrect frame sequence %u expected %u",
                      cannet->addr, get_frame_sequence(&frame->header), 0);
                return EILSEQ;
        }

        int err = ENODEV;

        if (frame->flags == 0) {

                if (not socket->connected) {

                        answer_t answer;
                        answer.addr_src = src_addr;
                        answer.connect = *frame;
                        err = sys_queue_send(socket->ansq, &answer, 0);

                } else {
                        DEBUG("[%u] socket already in use", cannet->addr);

                        u8_t flags = CONNECT_FRAME_FLAG__RESPONSE;
                        CANNET_socket_t tmpsock;
                        memcpy(&tmpsock, socket, sizeof(tmpsock));
                        tmpsock.addr_remote = src_addr;
                        err = send_connect_frame(cannet, &tmpsock, flags);
                }

        } else {
                answer_t answer;
                answer.addr_src = socket->addr_remote;
                answer.connect = *frame;
                err = sys_queue_send(socket->ansq, &answer, 0);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Handle disconnect frame.
 *
 * @param  self                 stack object
 * @param  source               source address
 * @param  frame                frame
 *
 * @return One of errno value.
 */
//==============================================================================
static int received_disconnect_frame(cannet_t *cannet, CANNET_socket_t *socket, const disconnect_frame_t *frame)
{
        UNUSED_ARG1(cannet);

        if (get_frame_sequence(&frame->header) != 0) {
                DEBUG("[%u] incorrect frame sequence %u expected %u",
                      cannet->addr, get_frame_sequence(&frame->header), 0);
                return EILSEQ;
        }

        cannetbuf__clear(socket->rx_buf);
        cannetbuf__clear(socket->assembly_buf);

        int err = unregister_socket(cannet, socket);
        if (!err) {
                socket->connected = false;
                socket->addr_remote = UINT16_MAX;
        }

        return err;
}

//==============================================================================
/**
 * @brief  Handle stat frame.
 *
 * @param  self                 stack object
 * @param  source               source address
 * @param  frame                frame
 *
 * @return One of errno value.
 */
//==============================================================================
static int received_stat_frame(cannet_t *cannet, CANNET_socket_t *socket, const stat_frame_t *frame)
{
        if (get_frame_sequence(&frame->header) != 0) {
                DEBUG("[%u] incorrect frame sequence %u expected %u",
                      cannet->addr, get_frame_sequence(&frame->header), 0);
                return EILSEQ;
        }

        int err = ENODEV;

        if (frame->flags == 0) {
                size_t available = 0;
                cannetbuf__get_available(socket->rx_buf, &available);
                err = send_stat_frame(cannet, socket, STAT_FRAME_FLAG__RESPONSE);

        } else {
                answer_t answer;
                answer.addr_src = socket->addr_remote;
                answer.stat = *frame;
                err = sys_queue_send(socket->ansq, &answer, 0);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Handle ack frame.
 *
 * @param  self                 stack object
 * @param  source               source address
 * @param  frame                frame
 *
 * @return One of errno value.
 */
//==============================================================================
static int received_ack_frame(cannet_t *cannet, CANNET_socket_t *socket, const ack_frame_t *frame)
{
        UNUSED_ARG1(cannet);

        if (get_frame_sequence(&frame->header) != 0) {
                DEBUG("[%u] incorrect frame sequence %u expected %u",
                      cannet->addr, get_frame_sequence(&frame->header), 0);
                return EILSEQ;
        }

        answer_t answer;
        answer.addr_src = socket->addr_remote;
        answer.ack = *frame;
        return sys_queue_send(socket->ansq, &answer, 0);
}

/*==============================================================================
  End of file
==============================================================================*/
