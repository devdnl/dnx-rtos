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
#define LWIPD_STACK_SIZE                STACK_DEPTH_HUGE
#define ETH_FILE                        "/dev/eth0"

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
typedef struct {
        FILE           *ethif;
        u32_t           TCP_timer;
        u32_t           ARP_timer;
        u32_t           DHCP_fine_timer;
        u32_t           DHCP_coarse_timer;
        struct netif    netif;
} ethif_mem;

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
 * @brief Should allocate a pbuf and transfer the bytes of the incoming packet
 * from the interface into the pbuf.
 *
 * @return a pbuf filled with the received packet (including MAC header) NULL on memory error
 */
//==============================================================================
static struct pbuf *low_level_input()
{
      struct ethmac_frame frame = {.buffer = NULL, .length = 0};
      if (ioctl(mem->ethif, ETHMAC_IORQ_GET_RX_PACKET_CHAIN_MODE, &frame) != STD_RET_OK) {
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
      ioctl(mem->ethif, ETHMAC_IORQ_GET_RX_BUFFER_UNAVAILABLE_STATUS, &flag);

      if (flag) {
            ioctl(mem->ethif, ETHMAC_IORQ_CLEAR_RX_BUFFER_UNAVAILABLE_STATUS);
            ioctl(mem->ethif, ETHMAC_IORQ_RESUME_DMA_RECEPTION);
      }

      return p;
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
        ioctl(mem->ethif, ETHMAC_IORQ_GET_RX_FLAG, &rx_flag);
        if (!rx_flag)
                return;

        u32_t rx_packet_size = 0;
        ioctl(mem->ethif, ETHMAC_IORQ_GET_RX_PACKET_SIZE, &rx_packet_size);
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

                ioctl(mem->ethif, ETHMAC_IORQ_GET_RX_PACKET_SIZE, &rx_packet_size);
        }

//                u32_t rx_packet_size;
//                do {
//                        rx_packet_size = 0;
//                        ioctl(mem->ethif, ETHMAC_IORQ_GET_RX_PACKET_SIZE, &rx_packet_size);
//
//                        if (rx_packet_size != 0) {
//                                /*
//                                 * read a received packet from the Ethernet buffers and send it to the
//                                 * lwIP for handling
//                                 */
//
//                                /* move received packet into a new pbuf */
//                                struct pbuf *p = low_level_input();
//                                if (p) {
//                                        if (mem->netif.input(p, &mem->netif) != ERR_OK) {
//                                                pbuf_free(p);
//                                        }
//                                }
//                        }
//
//                } while (rx_packet_size != 0);

        ioctl(mem->ethif, ETHMAC_IORQ_CLEAR_RX_FLAG);
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

        while (!(mem->ethif = fopen(ETH_FILE, "r+"))) {
                sleep_ms(500);
        }

        while (true) {
                handle_timers();
                receive_packet();
                sleep_ms(TCP_TMR_INTERVAL);
        }
}

//==============================================================================
/**
 * @brief Function starts lwIP deamon
 */
//==============================================================================
void ethif_start_lwIP_daemon()
{
        new_task(lwIP_daemon, "lwipd", LWIPD_STACK_SIZE, NULL);
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
