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
#include "netif/etharp.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define LWIPD_STACK_SIZE                STACK_DEPTH_VERY_LOW
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
} request;

typedef struct request_msg {
        request   request;
        ip_addr_t ip_address;
        ip_addr_t net_mask;
        ip_addr_t gateway;
} request_msg;

typedef struct {
        FILE           *eth_file;
        u8_t           *rx_buffer;
        u8_t           *tx_buffer;
        mutex_t        *protect_request_mtx;
        request_msg    *request;
        queue_t        *response_queue;
        u32_t           TCP_timer;
        u32_t           ARP_timer;
#if LWIP_DHCP
        u32_t           DHCP_fine_timer;
        u32_t           DHCP_coarse_timer;
#endif
        struct netif    netif;
        bool            DHCP_enabled;
        int             DHCP_step;
} ethif_mem;


struct ethif {
      struct eth_addr *ethaddr;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
static ethif_mem *mem;

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
static inline void send_request(request_msg *request)
{
        mem->request = request;
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
      if (ioctl(mem->eth_file, ETHMAC_IORQ_GET_RX_PACKET_CHAIN_MODE, &frame) != STD_RET_OK) {
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
      ioctl(mem->eth_file, ETHMAC_IORQ_GET_RX_BUFFER_UNAVAILABLE_STATUS, &flag);

      if (flag) {
            ioctl(mem->eth_file, ETHMAC_IORQ_CLEAR_RX_BUFFER_UNAVAILABLE_STATUS);
            ioctl(mem->eth_file, ETHMAC_IORQ_RESUME_DMA_RECEPTION);
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
      ioctl(mem->eth_file, ETHMAC_IORQ_GET_CURRENT_TX_BUFFER, &buffer);

      int len = 0;
      for (struct pbuf *q = p; q != NULL; q = q->next) {
            memcpy((u8_t*)&buffer[len], q->payload, q->len);
            len += q->len;
      }

      ioctl(mem->eth_file, ETHMAC_IORQ_SET_TX_FRAME_LENGTH_CHAIN_MODE, &len);

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
        struct ethif *ethif = malloc(sizeof(struct ethif));
        if (!ethif) {
                return ERR_MEM;
        }

#if LWIP_NETIF_HOSTNAME
        netif->hostname = (char *)get_host_name();
#endif

        /*
         * Initialize the snmp variables and counters inside the struct netif.  The last argument
         * should be replaced with your link speed, in units of bits per second.
         */
        NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 100000000);

        netif->state   = ethif;
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

        ethif->ethaddr = (struct eth_addr*)&(netif->hwaddr[0]);

        if (ioctl(mem->eth_file, ETHMAC_IORQ_SET_MAC_ADR, netif->hwaddr) != STD_RET_OK) {
                goto exit_error;
        }

        /* allocate Rx and Tx buffers */
        mem->rx_buffer = malloc(sizeof(u8_t) * ETH_NUMBER_OF_RX_BUFFERS * ETH_MAX_PACKET_SIZE);
        mem->tx_buffer = malloc(sizeof(u8_t) * ETH_NUMBER_OF_TX_BUFFERS * ETH_MAX_PACKET_SIZE);

        if (!mem->rx_buffer || !mem->tx_buffer) {
                goto exit_error;
        }

        /* initialize Tx Descriptors list: Chain Mode */
        struct ethmac_DMA_description DMA_description;
        DMA_description.buffer       = mem->tx_buffer;
        DMA_description.buffer_count = ETH_NUMBER_OF_TX_BUFFERS;

        if (ioctl(mem->eth_file, ETHMAC_IORQ_INIT_DMA_TX_DESC_LIST_CHAIN_MODE, &DMA_description) != STD_RET_OK) {
                goto exit_error;
        }

        /* initialize Rx Descriptors list: Chain Mode */
        DMA_description.buffer       = mem->rx_buffer;
        DMA_description.buffer_count = ETH_NUMBER_OF_RX_BUFFERS;

        if (ioctl(mem->eth_file, ETHMAC_IORQ_INIT_DMA_RX_DESC_LIST_CHAIN_MODE, &DMA_description) != STD_RET_OK) {
                goto exit_error;
        }

        /* enable Ethernet Rx interrrupt */
        if (ioctl(mem->eth_file, ETHMAC_IORQ_ENABLE_RX_IRQ) != STD_RET_OK) {
                goto exit_error;
        }

        if (ETHMAC_CHECKSUM_BY_HARDWARE != 0) {
                if (ioctl(mem->eth_file, ETHMAC_IORQ_ENABLE_TX_HARDWARE_CHECKSUM) != STD_RET_OK) {
                        goto exit_error;
                }
        }

        /* start Ethernet interface */
        if (ioctl(mem->eth_file, ETHMAC_IORQ_ETHERNET_START) != STD_RET_OK) {
                goto exit_error;
        }

        return ERR_OK;

      /* initialization error */
exit_error:
        if (ethif)
                free(ethif);

        if (mem->rx_buffer) {
                free(mem->rx_buffer);
                mem->rx_buffer = NULL;
        }

        if (mem->tx_buffer) {
                free(mem->tx_buffer);
                mem->tx_buffer = NULL;
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
        netif_remove(&mem->netif);

        if (mem->netif.state) {
                free(mem->netif.state);
                mem->netif.state = NULL;
        }

        if (mem->rx_buffer) {
                free(mem->rx_buffer);
                mem->rx_buffer = NULL;
        }

        if (mem->tx_buffer) {
                free(mem->tx_buffer);
                mem->tx_buffer = NULL;
        }

        /* DNLTODO ethernet stop */
}

//==============================================================================
/**
 * @brief Function handle lwIP timers
 */
//==============================================================================
static void handle_timers()
{
        u32_t time = get_OS_time_ms();

        /* TCP periodic process every 250 ms */
        if (time - mem->TCP_timer >= TCP_TMR_INTERVAL) {
                mem->TCP_timer = time;
                tcp_tmr();
        }

        /* ARP periodic process every 5s */
        if (time - mem->ARP_timer >= ARP_TMR_INTERVAL) {
                mem->ARP_timer = time;
                etharp_tmr();
        }

#if LWIP_DHCP
        /* fine DHCP periodic process every 500ms */
        if (time - mem->DHCP_fine_timer >= DHCP_FINE_TIMER_MSECS) {
                mem->DHCP_fine_timer = time;
                dhcp_fine_tmr();
        }

        /* DHCP Coarse periodic process every 60s */
        if (time - mem->DHCP_coarse_timer >= DHCP_COARSE_TIMER_MSECS) {
                mem->DHCP_coarse_timer = time;
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
        if (!mem->netif.state)
                return;

        bool rx_flag = false;
        ioctl(mem->eth_file, ETHMAC_IORQ_GET_RX_FLAG, &rx_flag);
        if (!rx_flag)
                return;

        u32_t rx_packet_size = 0;
        ioctl(mem->eth_file, ETHMAC_IORQ_GET_RX_PACKET_SIZE, &rx_packet_size);
        while (rx_packet_size) {
                /*
                 * read a received packet from the Ethernet buffers and send it to the
                 * lwIP for handling
                 */

                /* move received packet into a new pbuf */
                struct pbuf *p = low_level_input();
                if (p) {
                        if (mem->netif.input(p, &mem->netif) != ERR_OK) {
                                pbuf_free(p);
                        }
                }

                ioctl(mem->eth_file, ETHMAC_IORQ_GET_RX_PACKET_SIZE, &rx_packet_size);
        }

        ioctl(mem->eth_file, ETHMAC_IORQ_CLEAR_RX_FLAG);
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
        if (netif_add(&mem->netif,
                      &mem->request->ip_address,
                      &mem->request->net_mask,
                      &mem->request->gateway,
                      NULL,
                      ethif_init,
                      ethernet_input) == NULL) {

                return false;
        } else {
                netif_set_default(&mem->netif);
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
        static const int response_success = 0;
        static const int response_fail    = -1;

        if (mem->request) {

                switch (mem->request->request) {
                case RQ_START_DHCP:
                        switch (mem->DHCP_step) {
                        case 0:
                                if (configure_netif()) {
                                        if (dhcp_start(&mem->netif) == ERR_OK) {
                                                mem->DHCP_enabled = true;
                                                mem->DHCP_step++;
                                        } else {
                                                ethif_deinit();
                                                send_queue(mem->response_queue, &response_fail, MAX_DELAY);
                                        }
                                } else {
                                        send_queue(mem->response_queue, &response_fail, MAX_DELAY);
                                }
                                break;

                        case 1:
                                if (mem->netif.dhcp->state == DHCP_BOUND) {
                                        mem->DHCP_step = 0;
                                        mem->request   = NULL;

                                        netif_set_up(&mem->netif);
                                        send_queue(mem->response_queue, &response_success, MAX_DELAY);

                                        printk("DHCP_BOUND\n");
                                }
                                send_queue(mem->response_queue, &response_success, MAX_DELAY);
                                break;
                        }

                        break;

                case RQ_RENEW_DHCP:
                        mem->request = NULL;
                        break;

                case RQ_STOP_DHCP:
                        mem->request = NULL;
                        break;

                case RQ_UP_STATIC:
                        mem->request = NULL;
                        if (configure_netif()) {
                                netif_set_up(&mem->netif);
                                send_queue(mem->response_queue, &response_success, MAX_DELAY);
                        } else {
                                send_queue(mem->response_queue, &response_fail, MAX_DELAY);
                        }
                        break;

                case RQ_DOWN_STATIC:
                        mem->request = NULL;
                        ethif_deinit();
                        send_queue(mem->response_queue, &response_success, MAX_DELAY);
                        break;

                default:
                        mem->request = NULL;
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

        mem = calloc(1, sizeof(ethif_mem));
        if (!mem) {
                task_exit();
        }

        mem->protect_request_mtx = new_mutex();
        if (!mem->protect_request_mtx) {
                free(mem);
                task_exit();
        }

        mem->response_queue = new_queue(1, sizeof(int));
        if (!mem->response_queue) {
                delete_mutex(mem->protect_request_mtx);
                free(mem);
                task_exit();
        }

        while (!(mem->eth_file = fopen(_ETHIF_FILE, "r+"))) {
                sleep_ms(500);
        }

        while (true) {
                handle_timers();
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
        if (!mem) {
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
        u32_t time = get_OS_time_ms();
        while (!mem->eth_file) {
                if (get_OS_time_ms() - time >= REQUEST_TIMEOUT_MS)
                        return -1;
                sleep_ms(100);
        }

        int response = -1;
        if (lock_mutex(mem->protect_request_mtx, REQUEST_TIMEOUT_MS)) {

                request_msg rq;
                rq.request         = RQ_START_DHCP;
                rq.ip_address.addr = IPADDR_ANY;
                rq.net_mask.addr   = IPADDR_ANY;
                rq.gateway.addr    = IPADDR_ANY;

                send_request(&rq);

                receive_queue(mem->response_queue, &response, MAX_DELAY);

                unlock_mutex(mem->protect_request_mtx);
        }

        return response;
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
        if (!ip_address || !net_mask || !gateway || mem->netif.state)
                return -1;

        u32_t time = get_OS_time_ms();
        while (!mem->eth_file) {
                if (get_OS_time_ms() - time >= REQUEST_TIMEOUT_MS)
                        return -1;
                sleep_ms(100);
        }

        int response = -1;
        if (lock_mutex(mem->protect_request_mtx, REQUEST_TIMEOUT_MS)) {

                request_msg rq;
                rq.request    = RQ_UP_STATIC;
                rq.ip_address = *ip_address;
                rq.net_mask   = *net_mask;
                rq.gateway    = *gateway;

                send_request(&rq);

                receive_queue(mem->response_queue, &response, MAX_DELAY);

                unlock_mutex(mem->protect_request_mtx);
        }

        return response;
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

        if (mem->netif.state) {
                if (mem->DHCP_enabled) {
                        if (mem->netif.dhcp->state != DHCP_BOUND) {
                                ifcfg->mode = IFMODE_DHCP_CONFIGURING;
                        } else {
                                ifcfg->mode = IFMODE_DHCP_CONFIGURED;
                        }
                } else {
                        ifcfg->mode = IFMODE_STATIC_IP;
                }

                ifcfg->IP_address = mem->netif.ip_addr;
                ifcfg->net_mask   = mem->netif.netmask;
                ifcfg->gateway    = mem->netif.gw;
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
