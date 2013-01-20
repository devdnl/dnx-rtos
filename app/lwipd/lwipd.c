/*=============================================================================================*//**
@file    lwipd.c

@author  Daniel Zorychta

@brief

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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


*//*==============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "lwipd.h"
#include <string.h>

#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/tcp_impl.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"

#include "ethernetif.h" /* DNLFIXME to remove */

#include "ether_def.h"

/* Begin of application section declaration */
APPLICATION(lwipd, 6)
APP_SEC_BEGIN

/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
u32_t TCPTimer = 0;
u32_t ARPTimer = 0;

#if LWIP_DHCP
u32_t DHCPfineTimer   = 0;
u32_t DHCPcoarseTimer = 0;
#endif

bool_t netifConfigured = FALSE;
FILE_t *ethf           = NULL;



/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * Should allocate a pbuf and transfer the bytes of the incoming packet from the interface into
 * the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header) NULL on memory error
 */
//================================================================================================//
struct pbuf *low_level_input(struct netif *netif)
{
      (void)netif;

      struct pbuf *p = NULL;
      struct pbuf *q = NULL;
      uint_t       l = 0;

      struct ether_frame frame = {0, 0};

      if (ioctl(ethf, ETHER_IORQ_GET_RX_PACKET_CHAIN_MODE, &frame) != STD_RET_OK) {
            goto low_level_input_end;
      }

      if (frame.length == 0) {
            goto low_level_input_end;
      }

      u8_t *buffer = (u8_t*) frame.buffer;

      /* We allocate a pbuf chain of pbufs from the pool. */
      p = pbuf_alloc(PBUF_RAW, frame.length, PBUF_POOL);

      if (p != NULL) {
            for (q = p; q != NULL; q = q->next) {
                  memcpy((u8_t*) q->payload, (u8_t*) &buffer[l], q->len);
                  l = l + q->len;
            }
      }

      bool_t flag = FALSE;
      ioctl(ethf, ETHER_IORQ_GET_RX_BUFFER_UNAVAILABLE_STATUS, &flag);

      if (flag == TRUE) {
            ioctl(ethf, ETHER_IORQ_CLEAR_RX_BUFFER_UNAVAILABLE_STATUS, NULL);
            ioctl(ethf, ETHER_IORQ_RESUME_DMA_RECEPTION, NULL);
      }

      low_level_input_end:
      return p;
}


//================================================================================================//
/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */
//================================================================================================//
err_t low_level_output(struct netif *netif, struct pbuf *p)
{
      (void)netif;

      struct pbuf *q;
      int l = 0;

      u8_t *buffer = NULL;
      ioctl(ethf, ETHER_IORQ_GET_CURRENT_TX_BUFFER, &buffer);

      for (q = p; q != NULL; q = q->next) {
            memcpy((u8_t*)&buffer[l], q->payload, q->len);
            l = l + q->len;
      }

      ioctl(ethf, ETHER_IORQ_SET_TX_FRAME_LENGTH_CHAIN_MODE, &l);

      return ERR_OK;
}


//================================================================================================//
/**
 * @brief clear main function
 */
//================================================================================================//
stdRet_t appmain(ch_t *argv)
{
      (void)argv;

      stdRet_t status = STD_RET_ERROR;

      struct netif *netif = malloc(sizeof(struct netif));

      if (netif == NULL) {
            kprint(FONT_COLOR_RED "Unable to allocate netif interface!\n" RESET_ATTRIBUTES);
            goto lwipd_end;
      }

      struct ip_addr ipaddr;
      struct ip_addr netmask;
      struct ip_addr gw;
      u8_t macaddress[6] = {0, 0, 0, 0, 0, 1};

      /* check if Ethernet interface exist */
      ethf = fopen("/dev/eth0", "r");

      if (ethf == NULL) {
            kprint("lwipd: Ethernet interface does not exist!");
            goto lwipd_end;
      }

      lwip_init();

      #if LWIP_DHCP
      ipaddr.addr  = 0;
      netmask.addr = 0;
      gw.addr      = 0;
      #else
      IP4_ADDR(&ipaddr , 192, 168, 0  , 20 );
      IP4_ADDR(&netmask, 255, 255, 255, 0  );
      IP4_ADDR(&gw     , 192, 168, 0  , 1  );
      #endif

      ioctl(ethf, ETHER_IORQ_SET_MAC_ADR, macaddress);

      /*
       * the init function pointer must point to a initialization function for your ethernet netif
       * interface. The following code illustrates it's use.
       */
      if (netif_add(netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input) == NULL) {
            goto lwipd_end;
      }

      /* set MAC hardware address */
      netif->hwaddr[0] = macaddress[0];
      netif->hwaddr[1] = macaddress[1];
      netif->hwaddr[2] = macaddress[2];
      netif->hwaddr[3] = macaddress[3];
      netif->hwaddr[4] = macaddress[4];
      netif->hwaddr[5] = macaddress[5];

      netif->linkoutput = low_level_output;

      /* registers the default network interface */
      netif_set_default(netif);

#if LWIP_DHCP
      /*
       * creates a new DHCP client for this interface on the first call.
       * Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at the predefined regular
       * intervals after starting the client. You can peek in the netif->dhcp struct for the actual
       * DHCP status.
       */
      kprint("Starting DHCP Client..");
      if (ERR_MEM == dhcp_start(netif)) {
            kprint(". " FONT_COLOR_RED "Fail" RESET_ATTRIBUTES "\n");
            goto lwipd_end;
      }
#endif

      while (TRUE) {
            if (netifConfigured == FALSE) {
#if LWIP_DHCP
                  /* waiting for DHCP connection */
                  u8_t times = 100;

                  if (netif->dhcp->state != DHCP_BOUND && times > 0) {
//                        kprint(".");
//                        TaskDelay(20);
                        times--;
                  } else {
                        /* checking that DHCP connect */
                        if (times > 0) {
                              kprint(FONT_COLOR_GREEN "OK" RESET_ATTRIBUTES "\n");

                              ip_addr_set(&ipaddr,  &netif->ip_addr);
                              ip_addr_set(&netmask, &netif->netmask);
                              ip_addr_set(&gw,      &netif->gw);
                        } else {
                              dhcp_release(netif);
                              dhcp_stop(netif);

                              kprint(FONT_COLOR_RED "Fail" RESET_ATTRIBUTES "\n");

                              kprint("Setting static IP...\n");
                              IP4_ADDR(&ipaddr , 192, 168, 0  , 20 );
                              IP4_ADDR(&netmask, 255, 255, 255, 0  );
                              IP4_ADDR(&gw     , 192, 168, 0  , 1  );

                              netif_set_addr(netif, &ipaddr, &netmask, &gw);
                        }
#endif

                        /* when the netif is fully configured this function must be called.*/
                        netif_set_up(netif);
                        netifConfigured = TRUE;

                        kprint("Hostname  : %s\n", netif->hostname);
                        kprint("MAC       : %x2:%x2:%x2:%x2:%x2:%x2\n", macaddress[0], macaddress[1], macaddress[2],
                                                                        macaddress[3], macaddress[4], macaddress[5]);
                        kprint("IP Address: %d.%d.%d.%d\n", ip4_addr1(&ipaddr),  ip4_addr2(&ipaddr),
                                                            ip4_addr3(&ipaddr),  ip4_addr4(&ipaddr));
                        kprint("Net Mask  : %d.%d.%d.%d\n", ip4_addr1(&netmask), ip4_addr2(&netmask),
                                                            ip4_addr3(&netmask), ip4_addr4(&netmask));
                        kprint("Gateway   : %d.%d.%d.%d\n", ip4_addr1(&gw),      ip4_addr2(&gw),
                                                            ip4_addr3(&gw),      ip4_addr4(&gw));

                        status = STD_RET_OK;
#if LWIP_DHCP
                  }
#endif
            }


            /* receive packet from MAC */
            bool_t rxflag = FALSE;

            if (ioctl(ethf, ETHER_IORQ_GET_RX_FLAG, &rxflag) == STD_RET_OK) {
                  if (rxflag == TRUE) {
                        u32_t rxPktSize;

                        do {
                              rxPktSize = 0;
                              ioctl(ethf, ETHER_IORQ_GET_RX_PACKET_SIZE, &rxPktSize);

                              if (rxPktSize != 0) {
                                    /*
                                     * read a received packet from the Ethernet buffers and send it to the
                                     * lwIP for handling
                                     */

                                    /* move received packet into a new pbuf */
                                    struct pbuf *p = low_level_input(netif);

                                    if (p) {
                                          if (netif->input(p, netif) != ERR_OK) {
                                                pbuf_free(p);
                                          }
                                    }
                              }

                        } while (rxPktSize != 0);

                        ioctl(ethf, ETHER_IORQ_CLEAR_RX_FLAG, NULL);
                  }
            }

            u32_t localtime = TaskGetTickCount();

            /* TCP periodic process every 250 ms */
            if (localtime - TCPTimer >= TCP_TMR_INTERVAL) {
                  TCPTimer = localtime;
                  tcp_tmr();
            }

            /* ARP periodic process every 5s */
            if (localtime - ARPTimer >= ARP_TMR_INTERVAL) {
                  ARPTimer = localtime;
                  etharp_tmr();
            }

#if LWIP_DHCP
            /* fine DHCP periodic process every 500ms */
            if (localtime - DHCPfineTimer >= DHCP_FINE_TIMER_MSECS) {
                  DHCPfineTimer = localtime;
                  dhcp_fine_tmr();
            }

            /* DHCP Coarse periodic process every 60s */
            if (localtime - DHCPcoarseTimer >= DHCP_COARSE_TIMER_MSECS) {
                  DHCPcoarseTimer = localtime;
                  dhcp_coarse_tmr();
            }
#endif

            Sleep(50);
      }


      lwipd_end:
      if (netif) {
            free(netif);
      }

      fclose(ethf);

      return status;
}

/* End of application section declaration */
APP_SEC_END

#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
