/*=========================================================================*//**
@file    ethif.c

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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <string.h>
#include "ethif.h"
#include "system/dnx.h"
#include "system/ioctl.h"
#include "lwipopts.h"
#include "lwip/tcp_impl.h"
#include "lwip/dhcp.h"
#include "lwip/init.h"
#include "netif/etharp.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define LWIPD_STACK_SIZE                STACK_DEPTH_LOW
#define MTU                             1500
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
typedef enum request {
        RQ_NOP,
        RQ_START_DHCP,
        RQ_STOP_DHCP,
        RQ_RENEW_DHCP,
        RQ_UP_STATIC,
        RQ_DOWN_STATIC
} request_cmd;

typedef struct request_msg {
        request_cmd     cmd;
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
        u32_t           TCP_timer;
        u32_t           ARP_timer;
#if LWIP_DHCP
        u32_t           DHCP_fine_timer;
        u32_t           DHCP_coarse_timer;
#endif
        struct netif    netif;
        int             ifman_step;
} ethif_mem;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
static ethif_mem *if_mem;

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
 * @brief Function send request to daemon
 */
//==============================================================================
static inline void send_request(request *request)
{
        if_mem->request = request;
}

//==============================================================================
/**
 * @brief Function send success response
 */
//==============================================================================
static void send_response_success()
{
        static const int response = 0;
        send_queue(if_mem->response_queue, &response, MAX_DELAY);
}

//==============================================================================
/**
 * @brief Function send fail response
 */
//==============================================================================
static void send_response_fail()
{
        static const int response = -1;
        send_queue(if_mem->response_queue, &response, MAX_DELAY);
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

        u32_t time = get_OS_time_ms();
        while (!if_mem->eth_file) {
                if (get_OS_time_ms() - time >= REQUEST_TIMEOUT_MS)
                        return -1;
                sleep_ms(250);
        }

        int response = -1;
        if (lock_mutex(if_mem->protect_request_mtx, REQUEST_TIMEOUT_MS)) {

                send_request(request);
                receive_queue(if_mem->response_queue, &response, MAX_DELAY);

                unlock_mutex(if_mem->protect_request_mtx);
        }

        return response;
}

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
      if (ioctl(if_mem->eth_file, ETHMAC_IORQ_GET_RX_PACKET_CHAIN_MODE, &frame) != STD_RET_OK) {
              return NULL;
      }

      if (frame.length == 0) {
              return NULL;
      }

      /* We allocate a pbuf chain of pbufs from the pool */
      struct pbuf *p = pbuf_alloc(PBUF_RAW, frame.length, PBUF_POOL);
      if (p != NULL) {
              uint len = 0;
              for (struct pbuf *q = p; q != NULL; q = q->next) {
                      memcpy((u8_t *)q->payload, &frame.buffer[len], q->len);
                      len += q->len;
              }
      }

      bool flag = false;
      ioctl(if_mem->eth_file, ETHMAC_IORQ_GET_RX_BUFFER_UNAVAILABLE_STATUS, &flag);

      if (flag) {
            ioctl(if_mem->eth_file, ETHMAC_IORQ_CLEAR_RX_BUFFER_UNAVAILABLE_STATUS);
            ioctl(if_mem->eth_file, ETHMAC_IORQ_RESUME_DMA_RECEPTION);
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
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */
//==============================================================================
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
      (void)netif;

      u8_t *buffer = NULL;
      ioctl(if_mem->eth_file, ETHMAC_IORQ_GET_CURRENT_TX_BUFFER, &buffer);

      int len = 0;
      for (struct pbuf *q = p; q != NULL; q = q->next) {
            memcpy((u8_t*)&buffer[len], q->payload, q->len);
            len += q->len;
      }

      ioctl(if_mem->eth_file, ETHMAC_IORQ_SET_TX_FRAME_LENGTH_CHAIN_MODE, &len);

      return ERR_OK;
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
#if LWIP_NETIF_HOSTNAME
        netif->hostname = (char *)get_host_name();
#endif

        /*
         * Initialize the snmp variables and counters inside the struct netif.  The last argument
         * should be replaced with your link speed, in units of bits per second.
         */
        NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 100000000);

        netif->name[0] = 'E';
        netif->name[1] = 'T';

        /*
         * we directly use etharp_output() here to save a function call. You can instead declare your
         * own function an call etharp_output() from it if you have to do some checks before sending
         * (e.g. if linkis available...)
         */
        netif->output     = etharp_output;
        netif->linkoutput = low_level_output;

        /* maximum transfer unit */
        netif->mtu = MTU;

        /* device capabilities */
        /* don't set NETIF_FLAG_ETHARP if this device is not an Ethernet one */
        netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

        /* set MAC hardware address length */
        netif->hwaddr_len = ETHARP_HWADDR_LEN;

        /* set MAC hardware address */
        netif->hwaddr[0] = _ETHIF_MAC_ADDR_0;
        netif->hwaddr[1] = _ETHIF_MAC_ADDR_1;
        netif->hwaddr[2] = _ETHIF_MAC_ADDR_2;
        netif->hwaddr[3] = _ETHIF_MAC_ADDR_3;
        netif->hwaddr[4] = _ETHIF_MAC_ADDR_4;
        netif->hwaddr[5] = _ETHIF_MAC_ADDR_5;

        if (ioctl(if_mem->eth_file, ETHMAC_IORQ_SET_MAC_ADR, netif->hwaddr) != STD_RET_OK) {
                goto exit_error;
        }

        /* allocate Rx and Tx buffers */
        if_mem->rx_buffer = malloc(sizeof(u8_t) * ETH_NUMBER_OF_RX_BUFFERS * ETH_MAX_PACKET_SIZE);
        if_mem->tx_buffer = malloc(sizeof(u8_t) * ETH_NUMBER_OF_TX_BUFFERS * ETH_MAX_PACKET_SIZE);

        if (!if_mem->rx_buffer || !if_mem->tx_buffer) {
                goto exit_error;
        }

        /* initialize Tx Descriptors list: Chain Mode */
        struct ethmac_DMA_description DMA_description;
        DMA_description.buffer       = if_mem->tx_buffer;
        DMA_description.buffer_count = ETH_NUMBER_OF_TX_BUFFERS;

        if (ioctl(if_mem->eth_file, ETHMAC_IORQ_INIT_DMA_TX_DESC_LIST_CHAIN_MODE, &DMA_description) != STD_RET_OK) {
                goto exit_error;
        }

        /* initialize Rx Descriptors list: Chain Mode */
        DMA_description.buffer       = if_mem->rx_buffer;
        DMA_description.buffer_count = ETH_NUMBER_OF_RX_BUFFERS;

        if (ioctl(if_mem->eth_file, ETHMAC_IORQ_INIT_DMA_RX_DESC_LIST_CHAIN_MODE, &DMA_description) != STD_RET_OK) {
                goto exit_error;
        }

        /* enable Ethernet Rx interrrupt */
        if (ioctl(if_mem->eth_file, ETHMAC_IORQ_ENABLE_RX_IRQ) != STD_RET_OK) {
                goto exit_error;
        }

        if (ETHMAC_CHECKSUM_BY_HARDWARE != 0) {
                if (ioctl(if_mem->eth_file, ETHMAC_IORQ_ENABLE_TX_HARDWARE_CHECKSUM) != STD_RET_OK) {
                        goto exit_error;
                }
        }

        /* start Ethernet interface */
        if (ioctl(if_mem->eth_file, ETHMAC_IORQ_ETHERNET_START) != STD_RET_OK) {
                goto exit_error;
        }

        return ERR_OK;

exit_error:
        if (if_mem->rx_buffer) {
                free(if_mem->rx_buffer);
                if_mem->rx_buffer = NULL;
        }

        if (if_mem->tx_buffer) {
                free(if_mem->tx_buffer);
                if_mem->tx_buffer = NULL;
        }

        return ERR_MEM;
}

//==============================================================================
/**
 * @brief Function deinitalize ethif
 */
//==============================================================================
static void ethif_deinit()
{
        netif_remove(&if_mem->netif);

        if (if_mem->rx_buffer) {
                free(if_mem->rx_buffer);
                if_mem->rx_buffer = NULL;
        }

        if (if_mem->tx_buffer) {
                free(if_mem->tx_buffer);
                if_mem->tx_buffer = NULL;
        }
}

//==============================================================================
/**
 * @brief Function handle lwIP
 */
//==============================================================================
static void lwIP_handle()
{
        u32_t time = get_OS_time_ms();

        /* TCP periodic process every 250 ms */
        if (time - if_mem->TCP_timer >= TCP_TMR_INTERVAL) {
                if_mem->TCP_timer = time;
                tcp_tmr();
        }

        /* ARP periodic process every 5s */
        if (time - if_mem->ARP_timer >= ARP_TMR_INTERVAL) {
                if_mem->ARP_timer = time;
                etharp_tmr();
        }

#if LWIP_DHCP
        /* fine DHCP periodic process every 500ms */
        if (time - if_mem->DHCP_fine_timer >= DHCP_FINE_TIMER_MSECS) {
                if_mem->DHCP_fine_timer = time;
                dhcp_fine_tmr();
        }

        /* DHCP Coarse periodic process every 60s */
        if (time - if_mem->DHCP_coarse_timer >= DHCP_COARSE_TIMER_MSECS) {
                if_mem->DHCP_coarse_timer = time;
                dhcp_coarse_tmr();
        }
#endif
}

//==============================================================================
/**
 * @brief Function receive packet from MAC
 */
//==============================================================================
static void receive_packet()
{
        if (!netif_is_up(&if_mem->netif))
                return;

        bool rx_flag = false;
        ioctl(if_mem->eth_file, ETHMAC_IORQ_GET_RX_FLAG, &rx_flag);
        if (!rx_flag)
                return;

        u32_t rx_packet_size = 0;
        ioctl(if_mem->eth_file, ETHMAC_IORQ_GET_RX_PACKET_SIZE, &rx_packet_size);
        while (rx_packet_size) {
                /*
                 * read a received packet from the Ethernet buffers and send it to the
                 * lwIP for handling
                 */

                /* move received packet into a new pbuf */
                struct pbuf *p = low_level_input();
                if (p) {
                        if (if_mem->netif.input(p, &if_mem->netif) != ERR_OK) {
                                pbuf_free(p);
                        }
                }

                ioctl(if_mem->eth_file, ETHMAC_IORQ_GET_RX_PACKET_SIZE, &rx_packet_size);
        }

        ioctl(if_mem->eth_file, ETHMAC_IORQ_CLEAR_RX_FLAG);
}

//==============================================================================
/**
 * @brief Function create netif and add to stack
 *
 * @return true if success, otherwise false
 */
//==============================================================================
static bool configure_netif()
{
        if (netif_add(&if_mem->netif,
                      &if_mem->request->ip_address,
                      &if_mem->request->net_mask,
                      &if_mem->request->gateway,
                      NULL,
                      ethif_init,
                      ethernet_input) == NULL) {

                return false;
        } else {
                netif_set_default(&if_mem->netif);
                return true;
        }
}

//==============================================================================
/**
 * @brief Function manage interface requests
 */
//==============================================================================
static void manage_interface()
{
        if (if_mem->request) {
                switch (if_mem->request->cmd) {
                case RQ_START_DHCP:
                        if (if_mem->ifman_step == 0) {
                                if (configure_netif()) {
                                        if (dhcp_start(&if_mem->netif) == ERR_OK) {
                                                netif_set_up(&if_mem->netif);
                                                if_mem->ifman_step++;
                                        } else {
                                                ethif_deinit();
                                                send_response_fail();
                                        }
                                } else {
                                        send_response_fail();
                                }
                        } else {
                                if (if_mem->netif.dhcp->state == DHCP_BOUND) {
                                        if_mem->ifman_step = 0;
                                        if_mem->request    = NULL;
                                        send_response_success();
                                }
                        }
                        break;

                case RQ_RENEW_DHCP:
                        if (if_mem->ifman_step == 0) {
                                if (dhcp_renew(&if_mem->netif) == ERR_OK) {
                                        if_mem->ifman_step++;
                                } else {
                                        send_response_fail();
                                }
                        } else {
                                if (if_mem->netif.dhcp->state == DHCP_BOUND) {
                                        if_mem->ifman_step = 0;
                                        if_mem->request    = NULL;
                                        send_response_success();
                                }
                        }
                        break;

                case RQ_STOP_DHCP:
                        if_mem->request = NULL;
                        break;

                case RQ_UP_STATIC:
                        if (configure_netif()) {
                                netif_set_up(&if_mem->netif);
                                send_response_success();
                        } else {
                                send_response_fail();
                        }
                        if_mem->request = NULL;
                        break;

                case RQ_DOWN_STATIC:
                        ethif_deinit();
                        if_mem->request = NULL;
                        send_response_success();
                        break;

                default:
                        if_mem->request = NULL;
                        break;
                }
        }
}

//==============================================================================
/**
 * @brief lwIP daemon
 */
//==============================================================================
static void lwIP_daemon(void *arg)
{
        (void)arg;

        lwip_init();

        if_mem = calloc(1, sizeof(ethif_mem));
        if (!if_mem) {
                task_exit();
        }

        if_mem->protect_request_mtx = new_mutex();
        if (!if_mem->protect_request_mtx) {
                free(if_mem);
                task_exit();
        }

        if_mem->response_queue = new_queue(1, sizeof(int));
        if (!if_mem->response_queue) {
                delete_mutex(if_mem->protect_request_mtx);
                free(if_mem);
                task_exit();
        }

        while (!(if_mem->eth_file = fopen(_ETHIF_FILE, "r+"))) {
                sleep_ms(500);
        }

        while (true) {
                lwIP_handle();
                receive_packet();
                manage_interface();
                sleep_ms(TCP_TMR_INTERVAL);
        }
}

//==============================================================================
/**
 * @brief Function starts lwIP daemon
 */
//==============================================================================
void _ethif_start_lwIP_daemon()
{
        if (!if_mem) {
                new_task(lwIP_daemon, "lwipd", LWIPD_STACK_SIZE, NULL);
        }
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
        if (netif_is_up(&if_mem->netif))
                return -1;

        request rq;
        rq.cmd             = RQ_START_DHCP;
        rq.ip_address.addr = IPADDR_ANY;
        rq.net_mask.addr   = IPADDR_ANY;
        rq.gateway.addr    = IPADDR_ANY;

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
int _ethif_renew_DHCP_client()
{
        if (netif_is_up(&if_mem->netif) && (if_mem->netif.flags & NETIF_FLAG_DHCP)) {

                request rq;
                rq.cmd = RQ_RENEW_DHCP;

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
int _ethif_if_up(ip_addr_t *ip_address, ip_addr_t *net_mask, ip_addr_t *gateway)
{
        if (!ip_address || !net_mask || !gateway || netif_is_up(&if_mem->netif))
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

        if (netif_is_up(&if_mem->netif)) {
                if (if_mem->netif.flags & NETIF_FLAG_DHCP) {
                        if (if_mem->netif.dhcp->state != DHCP_BOUND) {
                                ifcfg->mode = IFMODE_DHCP_CONFIGURING;
                        } else {
                                ifcfg->mode = IFMODE_DHCP_CONFIGURED;
                        }
                } else {
                        ifcfg->mode = IFMODE_STATIC_IP;
                }

                ifcfg->IP_address = if_mem->netif.ip_addr;
                ifcfg->net_mask   = if_mem->netif.netmask;
                ifcfg->gateway    = if_mem->netif.gw;
        } else {
                ifcfg->mode            = IFMODE_NOT_CONFIGURED;
                ifcfg->IP_address.addr = IPADDR_ANY;
                ifcfg->net_mask.addr   = IPADDR_ANY;
                ifcfg->gateway.addr    = IPADDR_ANY;
        }

        ifcfg->hw_address[0] = _ETHIF_MAC_ADDR_0;
        ifcfg->hw_address[1] = _ETHIF_MAC_ADDR_1;
        ifcfg->hw_address[2] = _ETHIF_MAC_ADDR_2;
        ifcfg->hw_address[3] = _ETHIF_MAC_ADDR_3;
        ifcfg->hw_address[4] = _ETHIF_MAC_ADDR_4;
        ifcfg->hw_address[5] = _ETHIF_MAC_ADDR_5;

        return 0;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
