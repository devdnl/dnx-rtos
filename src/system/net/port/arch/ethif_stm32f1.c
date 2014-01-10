/*=========================================================================*//**
@file    ethif_stm32f1.c

@author  Daniel Zorychta

@brief   lwIP Ethernet interface definition.

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

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <dnx/os.h>
#include <dnx/thread.h>
#include <dnx/timer.h>
#include "ethif.h"
#include "lwipopts.h"
#include "lwip/tcp_impl.h"
#include "lwip/dhcp.h"
#include "lwip/init.h"
#include "lwip/tcpip.h"
#include "netif/etharp.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define REQUEST_TIMEOUT_MS              2000

/* ETH_HEADER + ETH_EXTRA + MAX_ETH_PAYLOAD + ETH_CRC */
#define ETH_MAX_PACKET_SIZE             1520

/* number of Rx buffers */
#define ETH_NUMBER_OF_RX_BUFFERS        3

/* number of Tx buffers */
#define ETH_NUMBER_OF_TX_BUFFERS        2

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct request_msg {
        enum request {
                RQ_NOP,
                RQ_START_DHCP,
                RQ_STOP_DHCP,
                RQ_RENEW_DHCP,
                RQ_INFORM_DHCP,
                RQ_UP_STATIC,
                RQ_DOWN_STATIC
        } cmd;

        ip_addr_t       ip_address;
        ip_addr_t       net_mask;
        ip_addr_t       gateway;
} request;

typedef struct {
        FILE           *eth_file;
        u8_t           *rx_buffer;
        u8_t           *tx_buffer;
        mutex_t        *protect_request_mtx;
        request        *request;
        queue_t        *response_queue;
        struct netif    netif;
        int             manager_step;
        uint            rx_packets;
        uint            tx_packets;
        uint            rx_bytes;
        uint            tx_bytes;
} ethif_data;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
static ethif_data *ethif_mem;

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
 * @brief Should allocate a pbuf and transfer the bytes of the incoming packet
 * from the interface into the pbuf.
 *
 * @return a pbuf filled with the received packet (including MAC header) NULL on memory error
 */
//==============================================================================
static struct pbuf *low_level_input()
{
        struct ethmac_frame frame = {.buffer = NULL, .length = 0};
        if (ioctl(ethif_mem->eth_file, ETHMAC_IORQ_GET_RX_PACKET_CHAIN_MODE, &frame) != 0) {
                LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("low_level_input: ioctl() fail\n"));
                return NULL;
        }

        if (frame.length == 0) {
                LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("low_level_input: frame length = 0\n"));
                return NULL;
        }

        ethif_mem->rx_packets++;

        /* We allocate a pbuf chain of pbufs from the pool */
        struct pbuf *p = pbuf_alloc(PBUF_RAW, frame.length, PBUF_POOL);
        if (p != NULL) {
                uint len = 0;
                for (struct pbuf *q = p; q != NULL; q = q->next) {
                        memcpy((u8_t *)q->payload, &frame.buffer[len], q->len);
                        len                 += q->len;
                        ethif_mem->rx_bytes += q->len;
                }
        }

        bool flag = false;
        if (ioctl(ethif_mem->eth_file, ETHMAC_IORQ_GET_RX_BUFFER_UNAVAILABLE_STATUS, &flag) != 0) {
                LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("low_level_input: ioctl() fail\n"));
        }

        if (flag) {
              if (ioctl(ethif_mem->eth_file, ETHMAC_IORQ_CLEAR_RX_BUFFER_UNAVAILABLE_STATUS) != 0) {
                      LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("low_level_input: ioctl() fail\n"));
              }

              if (ioctl(ethif_mem->eth_file, ETHMAC_IORQ_RESUME_DMA_RECEPTION) != 0) {
                      LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("low_level_input: ioctl() fail\n"));
              }
        }

        return p;
}

//==============================================================================
/**
 * @brief This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf might be chained.
 *
 * @param netif         the lwip network interface structure for this ethernetif
 * @param p             the MAC packet to send (e.g. IP packet including MAC addresses and type)
 *
 * @return ERR_OK if the packet could be sent an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become available since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */
//==============================================================================
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
      (void)netif;

      u8_t *buffer = NULL;
      if (ioctl(ethif_mem->eth_file, ETHMAC_IORQ_GET_CURRENT_TX_BUFFER, &buffer) != 0) {
              LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("low_level_output: ioctl() fail\n"));
              return ERR_MEM;
      }

      if (!buffer) {
              LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("low_level_output: buffer NULL\n"));
              return ERR_MEM;
      }

      ethif_mem->tx_packets++;

      int len = 0;
      for (struct pbuf *q = p; q != NULL; q = q->next) {
            memcpy((u8_t*)&buffer[len], q->payload, q->len);
            len                 += q->len;
            ethif_mem->tx_bytes += q->len;
      }

      if (ioctl(ethif_mem->eth_file, ETHMAC_IORQ_SET_TX_FRAME_LENGTH_CHAIN_MODE, &len) != 0) {
              LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("low_level_output: ioctl() fail\n"));
              return ERR_MEM;
      }

      return ERR_OK;
}

//==============================================================================
/**
 * @brief Low level initialization function
 *
 * @param netif         the netif object
 *
 * @return ERR_OK if success, otherwise other value
 */
//==============================================================================
static err_t low_level_init(struct netif *netif)
{
        netif->hwaddr[0] = ETHIF_MAC_ADDR_0;
        netif->hwaddr[1] = ETHIF_MAC_ADDR_1;
        netif->hwaddr[2] = ETHIF_MAC_ADDR_2;
        netif->hwaddr[3] = ETHIF_MAC_ADDR_3;
        netif->hwaddr[4] = ETHIF_MAC_ADDR_4;
        netif->hwaddr[5] = ETHIF_MAC_ADDR_5;

        if (ioctl(ethif_mem->eth_file, ETHMAC_IORQ_SET_MAC_ADR, netif->hwaddr) != 0) {
                LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("low_level_init: ioctl() fail\n"));
                goto exit_error;
        }

        /* allocate Rx and Tx buffers */
        ethif_mem->rx_buffer = malloc(sizeof(u8_t) * ETH_NUMBER_OF_RX_BUFFERS * ETH_MAX_PACKET_SIZE);
        ethif_mem->tx_buffer = malloc(sizeof(u8_t) * ETH_NUMBER_OF_TX_BUFFERS * ETH_MAX_PACKET_SIZE);

        if (!ethif_mem->rx_buffer || !ethif_mem->tx_buffer) {
                LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("low_level_init: rx or tx buffer NULL\n"));
                goto exit_error;
        }

        /* initialize Tx Descriptors list: Chain Mode */
        struct ethmac_DMA_description DMA_description;
        DMA_description.buffer       = ethif_mem->tx_buffer;
        DMA_description.buffer_count = ETH_NUMBER_OF_TX_BUFFERS;

        if (ioctl(ethif_mem->eth_file, ETHMAC_IORQ_INIT_DMA_TX_DESC_LIST_CHAIN_MODE, &DMA_description) != 0) {
                LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("low_level_init: ioctl() fail\n"));
                goto exit_error;
        }

        /* initialize Rx Descriptors list: Chain Mode */
        DMA_description.buffer       = ethif_mem->rx_buffer;
        DMA_description.buffer_count = ETH_NUMBER_OF_RX_BUFFERS;

        if (ioctl(ethif_mem->eth_file, ETHMAC_IORQ_INIT_DMA_RX_DESC_LIST_CHAIN_MODE, &DMA_description) != 0) {
                LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("low_level_init: ioctl() fail\n"));
                goto exit_error;
        }

        /* enable Ethernet Rx interrrupt */
        if (ioctl(ethif_mem->eth_file, ETHMAC_IORQ_ENABLE_RX_IRQ) != 0) {
                LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("low_level_init: ioctl() fail\n"));
                goto exit_error;
        }

        if (ETHMAC_CHECKSUM_BY_HARDWARE != 0) {
                if (ioctl(ethif_mem->eth_file, ETHMAC_IORQ_ENABLE_TX_HARDWARE_CHECKSUM) != 0) {
                        LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("low_level_init: ioctl() fail\n"));
                        goto exit_error;
                }
        }

        /* start Ethernet interface */
        if (ioctl(ethif_mem->eth_file, ETHMAC_IORQ_ETHERNET_START) != 0) {
                LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("low_level_init: ioctl() fail\n"));
                goto exit_error;
        }

        return ERR_OK;

exit_error:
        if (ethif_mem->rx_buffer) {
                free(ethif_mem->rx_buffer);
                ethif_mem->rx_buffer = NULL;
        }

        if (ethif_mem->tx_buffer) {
                free(ethif_mem->tx_buffer);
                ethif_mem->tx_buffer = NULL;
        }

        return ERR_MEM;
}

//==============================================================================
/**
 * @brief Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif         the lwip network interface structure for this ethernetif
 *
 * @return ERR_OK       if the loopif is initialized
 *         ERR_MEM      if private data couldn't be allocated any other err_t on error
 */
//==============================================================================
static err_t ethif_init(struct netif *netif)
{
        /* initialize Ethernet */
        if (ioctl(ethif_mem->eth_file, ETHMAC_IORQ_ETHERNET_INIT) != 0) {
                LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("ethif_init: ioctl() fail\n"));
                return ERR_IF;
        }

        netif->state      = ethif_mem;
        netif->hostname   = (char *)get_host_name();
        netif->name[0]    = 'E';
        netif->name[1]    = 'T';
        netif->output     = etharp_output;
        netif->linkoutput = low_level_output;
        netif->mtu        = 1500;
        netif->hwaddr_len = ETHARP_HWADDR_LEN;
        netif->flags      = NETIF_FLAG_BROADCAST
                          | NETIF_FLAG_ETHARP
                          | /*NETIF_FLAG_LINK_UP*/ NETIF_FLAG_IGMP;

        return low_level_init(&ethif_mem->netif);
}

//==============================================================================
/**
 * @brief Function receive packet from MAC
 *
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface.
 */
//==============================================================================
static void ethif_input(struct netif *netif)
{
        bool rx_flag = false;
        if (ioctl(ethif_mem->eth_file, ETHMAC_IORQ_GET_RX_FLAG, &rx_flag) != 0) {
                LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("ethif_input: ioctl() fail\n"));
                return;
        }

        if (!rx_flag)
                return;

        u32_t rx_packet_size = 0;
        if (ioctl(ethif_mem->eth_file, ETHMAC_IORQ_GET_RX_PACKET_SIZE, &rx_packet_size) != 0) {
                LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("ethif_input: ioctl() fail\n"));
                return;
        }

        while (rx_packet_size) {
                /*
                 * read a received packet from the Ethernet buffers and send it to the
                 * lwIP for handling
                 */

                /* move received packet into a new pbuf */
                struct pbuf *p = low_level_input();
                if (p) {
                        if (ethif_mem->netif.input(p, netif) != ERR_OK) {
                                pbuf_free(p);
                        }
                }

                if (ioctl(ethif_mem->eth_file, ETHMAC_IORQ_GET_RX_PACKET_SIZE, &rx_packet_size) != 0) {
                        LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("ethif_input: ioctl() fail\n"));
                        return;
                }
        }

        if (ioctl(ethif_mem->eth_file, ETHMAC_IORQ_CLEAR_RX_FLAG) != 0) {
                LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("ethif_input: ioctl() fail\n"));
                return;
        }
}

//==============================================================================
/**
 * @brief Function send success response
 */
//==============================================================================
static void send_response_success()
{
        static const int response = 0;
        queue_send(ethif_mem->response_queue, &response, MAX_DELAY);
}

//==============================================================================
/**
 * @brief Function send fail response
 */
//==============================================================================
static void send_response_fail()
{
        static const int response = -1;
        queue_send(ethif_mem->response_queue, &response, MAX_DELAY);
}

//==============================================================================
/**
 * @brief Set request as finished
 */
//==============================================================================
static inline void request_finished()
{
        ethif_mem->manager_step = 0;
        ethif_mem->request      = NULL;
}

//==============================================================================
/**
 * @brief Clear received and transmitted byte counters
 */
//==============================================================================
static void clear_rx_tx_counters()
{
        ethif_mem->rx_bytes   = 0;
        ethif_mem->tx_bytes   = 0;
        ethif_mem->rx_packets = 0;
        ethif_mem->tx_packets = 0;
}

//==============================================================================
/**
 * @brief Function manage interface requests
 */
//==============================================================================
static void manage_interface()
{
        if (ethif_mem->request) {
                switch (ethif_mem->request->cmd) {
                case RQ_START_DHCP:
                        if (ethif_mem->manager_step == 0) {
                                clear_rx_tx_counters();
                                netif_set_down(&ethif_mem->netif);
                                netif_set_addr(&ethif_mem->netif,
                                               (ip_addr_t *)&ip_addr_any,
                                               (ip_addr_t *)&ip_addr_any,
                                               (ip_addr_t *)&ip_addr_any);

                                if (dhcp_start(&ethif_mem->netif) == ERR_OK) {
                                        netif_set_up(&ethif_mem->netif);
                                        ethif_mem->manager_step++;
                                } else {
                                        request_finished();
                                        send_response_fail();
                                }
                        } else {
                                if (ethif_mem->netif.dhcp->state == DHCP_BOUND) {
                                        request_finished();
                                        send_response_success();
                                }
                        }
                        break;

                case RQ_RENEW_DHCP:
                        if (ethif_mem->manager_step == 0) {
                                if (dhcp_renew(&ethif_mem->netif) == ERR_OK) {
                                        ethif_mem->manager_step++;
                                } else {
                                        request_finished();
                                        send_response_fail();
                                }
                        } else {
                                if (ethif_mem->netif.dhcp->state == DHCP_BOUND) {
                                        request_finished();
                                        send_response_success();
                                }
                        }
                        break;

                case RQ_INFORM_DHCP:
                        dhcp_inform(&ethif_mem->netif);
                        request_finished();
                        send_response_success();
                        break;

                case RQ_STOP_DHCP:
                        dhcp_release(&ethif_mem->netif);
                        dhcp_stop(&ethif_mem->netif);
                        netif_set_down(&ethif_mem->netif);
                        request_finished();
                        send_response_success();
                        break;

                case RQ_UP_STATIC:
                        clear_rx_tx_counters();
                        netif_set_down(&ethif_mem->netif);
                        netif_set_addr(&ethif_mem->netif,
                                       &ethif_mem->request->ip_address,
                                       &ethif_mem->request->net_mask,
                                       &ethif_mem->request->gateway);
                        netif_set_up(&ethif_mem->netif);
                        request_finished();
                        send_response_success();
                        break;

                case RQ_DOWN_STATIC:
                        netif_set_down(&ethif_mem->netif);
                        request_finished();
                        send_response_success();
                        break;

                default:
                        request_finished();
                        break;
                }
        }
}

//==============================================================================
/**
 * @brief Function is called from tcpip thread when task was successfully started
 */
//==============================================================================
static void tcpip_init_done(void *arg)
{
        (void)arg;

        ethif_mem = calloc(1, sizeof(ethif_data));
        if (!ethif_mem) {
                LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("tcpip_init_done: out of memory\n"));
                goto release_resources;
        }

        ethif_mem->protect_request_mtx = mutex_new(MUTEX_NORMAL);
        if (!ethif_mem->protect_request_mtx) {
                LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("tcpip_init_done: new mutex fail\n"));
                goto release_resources;
        }

        ethif_mem->response_queue = queue_new(1, sizeof(int));
        if (!ethif_mem->response_queue) {
                LWIP_DEBUGF(LOW_LEVEL_DEBUG, ("tcpip_init_done: new queue fail\n"));
                goto release_resources;
        }

        while (!(ethif_mem->eth_file = fopen(_ETHIF_INTERFACE_FILE, "r+"))) {
                sleep_ms(100);
        }

        netif_set_default(netif_add(&ethif_mem->netif,
                                    (ip_addr_t *)&ip_addr_any,
                                    (ip_addr_t *)&ip_addr_any,
                                    (ip_addr_t *)&ip_addr_any,
                                    NULL,
                                    ethif_init,
                                    tcpip_input));

        return;

release_resources:
        if (ethif_mem->protect_request_mtx)
                mutex_delete(ethif_mem->protect_request_mtx);

        if (ethif_mem)
                free(ethif_mem);

        task_exit();
}

//==============================================================================
/**
 * @brief Function send request to daemon
 */
//==============================================================================
static inline void send_request(request *request)
{
        ethif_mem->request = request;
}

//==============================================================================
/**
 * @brief Function send request and wait for response
 */
//==============================================================================
static int send_request_and_wait_for_response(request *request)
{
        if (!request)
                return -1;

        timer_t timer = timer_reset();
        while (!ethif_mem->eth_file) {
                if (timer_is_expired(timer, REQUEST_TIMEOUT_MS))
                        return -1;
                sleep_ms(250);
        }

        int response = -1;
        if (mutex_lock(ethif_mem->protect_request_mtx, REQUEST_TIMEOUT_MS)) {

                send_request(request);
                queue_receive(ethif_mem->response_queue, &response, MAX_DELAY);

                mutex_unlock(ethif_mem->protect_request_mtx);
        }

        return response;
}

//==============================================================================
/**
 * @brief Function manage packets and read user requests
 *
 * Function shall be called from tcpip_thread() using LWIP_TCPIP_THREAD_ALIVE()
 * macro. Macro shall be defined in sys_arch.h file.
 */
//==============================================================================
void _ethif_manager(void)
{
        ethif_input(&ethif_mem->netif);
        manage_interface();
}

//==============================================================================
/**
 * @brief Function starts lwIP daemon
 */
//==============================================================================
void _ethif_start_lwIP_daemon()
{
        tcpip_init(tcpip_init_done, NULL);
}

//==============================================================================
/**
 * @brief Function start DHCP client
 *
 * Function controls lwIP daemon via queue, because this function is called
 * from other task.
 *
 * @return 0 if success, otherwise -1
 */
//==============================================================================
int _ethif_start_DHCP_client()
{
        if (netif_is_up(&ethif_mem->netif))
                return -1;

        request rq;
        rq.cmd        = RQ_START_DHCP;
        rq.ip_address = ip_addr_any;
        rq.net_mask   = ip_addr_any;
        rq.gateway    = ip_addr_any;

        return send_request_and_wait_for_response(&rq);
}

//==============================================================================
/**
 * @brief Function stops DHCP client
 *
 * Function controls lwIP daemon via queue, because this function is called
 * from other task.
 *
 * @return 0 if success, otherwise -1
 */
//==============================================================================
int _ethif_stop_DHCP_client()
{
        if (netif_is_up(&ethif_mem->netif) && (ethif_mem->netif.flags & NETIF_FLAG_DHCP)) {

                request rq;
                rq.cmd = RQ_STOP_DHCP;

                return send_request_and_wait_for_response(&rq);
        }

        return -1;
}

//==============================================================================
/**
 * @brief Function renew DHCP connection
 *
 * Function controls lwIP daemon via queue, because this function is called
 * from other task.
 *
 * @return 0 if success, otherwise -1
 */
//==============================================================================
int _ethif_renew_DHCP_connection()
{
        if (netif_is_up(&ethif_mem->netif) && (ethif_mem->netif.flags & NETIF_FLAG_DHCP)) {

                request rq;
                rq.cmd = RQ_RENEW_DHCP;

                return send_request_and_wait_for_response(&rq);
        }

        return -1;
}

//==============================================================================
/**
 * @brief Function inform DHCP about current static configuration
 *
 * Function controls lwIP daemon via queue, because this function is called
 * from other task.
 *
 * @return 0 if success, otherwise -1
 */
//==============================================================================
int _ethif_inform_DHCP_server()
{
        if (netif_is_up(&ethif_mem->netif) && !(ethif_mem->netif.flags & NETIF_FLAG_DHCP)) {

                request rq;
                rq.cmd = RQ_INFORM_DHCP;

                return send_request_and_wait_for_response(&rq);
        }

        return -1;
}

//==============================================================================
/**
 * @brief Function configure interface as static
 *
 * Function controls lwIP daemon via queue, because this function is called
 * from other task.
 *
 * @param[in] ip_address        a IP address
 * @param[in] net_mask          a net mask value
 * @param[in] gateway           a gateway address
 *
 * @return 0 if success, otherwise -1
 */
//==============================================================================
int _ethif_if_up(const ip_addr_t *ip_address, const ip_addr_t *net_mask, const ip_addr_t *gateway)
{
        if (!ip_address || !net_mask || !gateway || netif_is_up(&ethif_mem->netif))
                return -1;

        request rq;
        rq.cmd        = RQ_UP_STATIC;
        rq.ip_address = *ip_address;
        rq.net_mask   = *net_mask;
        rq.gateway    = *gateway;

        return send_request_and_wait_for_response(&rq);
}

//==============================================================================
/**
 * @brief Function turn down interface with static configuration
 *
 * Function controls lwIP daemon via queue, because this function is called
 * from other task.
 *
 * @return 0 if success, otherwise -1
 */
//==============================================================================
int _ethif_if_down()
{
        if (netif_is_up(&ethif_mem->netif) && !(ethif_mem->netif.flags & NETIF_FLAG_DHCP)) {

                request rq;
                rq.cmd = RQ_DOWN_STATIC;

                return send_request_and_wait_for_response(&rq);
        }

        return -1;
}

//==============================================================================
/**
 * @brief Function gets interface configuration
 *
 * @param[out] ifcfg            a pointer to status object
 *
 * @return 0 if success, otherwise -1
 */
//==============================================================================
int _ethif_get_ifconfig(ifconfig *ifcfg)
{
        if (!ifcfg)
                return -1;

        if (netif_is_up(&ethif_mem->netif)) {
                if (ethif_mem->netif.flags & NETIF_FLAG_DHCP) {
                        if (ethif_mem->netif.dhcp->state != DHCP_BOUND) {
                                ifcfg->status = IFSTATUS_DHCP_CONFIGURING;
                        } else {
                                ifcfg->status = IFSTATUS_DHCP_CONFIGURED;
                        }
                } else {
                        ifcfg->status = IFSTATUS_STATIC_IP;
                }

                ifcfg->IP_address = ethif_mem->netif.ip_addr;
                ifcfg->net_mask   = ethif_mem->netif.netmask;
                ifcfg->gateway    = ethif_mem->netif.gw;
        } else {
                ifcfg->status     = IFSTATUS_NOT_CONFIGURED;
                ifcfg->IP_address = ip_addr_any;
                ifcfg->net_mask   = ip_addr_any;
                ifcfg->gateway    = ip_addr_any;
        }

        ifcfg->hw_address[0] = ETHIF_MAC_ADDR_0;
        ifcfg->hw_address[1] = ETHIF_MAC_ADDR_1;
        ifcfg->hw_address[2] = ETHIF_MAC_ADDR_2;
        ifcfg->hw_address[3] = ETHIF_MAC_ADDR_3;
        ifcfg->hw_address[4] = ETHIF_MAC_ADDR_4;
        ifcfg->hw_address[5] = ETHIF_MAC_ADDR_5;
        ifcfg->rx_packets    = ethif_mem->rx_packets;
        ifcfg->rx_bytes      = ethif_mem->rx_bytes;
        ifcfg->tx_packets    = ethif_mem->tx_packets;
        ifcfg->tx_bytes      = ethif_mem->tx_bytes;

        return 0;
}

/*==============================================================================
  End of file
==============================================================================*/
