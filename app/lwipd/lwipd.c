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

#include "ether_def.h"

/* Begin of application section declaration */
PROGRAM(lwipd, 6)
PROG_SEC_BEGIN

/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
/* Ethernet interface file */
#define ETH_FILE                    "/dev/eth0"

/* daemon status file */
#define STATUS_FILE                 "/etc/netinf"

/* wait time to connect to DHCP server [ms] */
#define DHCP_CLIENT_WAIT_TIME       5000

/* base handling time [ms] */
#define BASE_TIME                   250

/* MAC address */
#define MACADDR0                    0x00
#define MACADDR1                    0x00
#define MACADDR2                    0x00
#define MACADDR3                    0x00
#define MACADDR4                    0x00
#define MACADDR5                    0x01

/* ETH_HEADER + ETH_EXTRA + MAX_ETH_PAYLOAD + ETH_CRC */
#define ETH_MAX_PACKET_SIZE         1520

/* number of Rx buffers */
#define ETH_RXBUFNB                 3

/* number of Tx buffers */
#define ETH_TXBUFNB                 2


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
struct ethernetif {
      struct eth_addr *ethaddr;
};


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
/** TCP Timer */
u32_t TCPTimer = 0;

/** ARP timer */
u32_t ARPTimer = 0;

#if LWIP_DHCP
/** DHCP timers */
u32_t DHCPfineTimer   = 0;
u32_t DHCPcoarseTimer = 0;
#endif

/** network interface status */
bool_t netif_ready = FALSE;

/** Ethernet device file */
FILE_t *ethf = NULL;

/** Rx buffer */
u8_t *Rx_Buff = NULL;

/** Tx buffer */
u8_t *Tx_Buff = NULL;

/** IP addresses */
struct ip_addr ipaddr;
struct ip_addr netmask;
struct ip_addr gateway;

/** DHCP connection timeout */
#if LWIP_DHCP
u8_t dhcp_timeout_counter = DHCP_CLIENT_WAIT_TIME / BASE_TIME;
#endif

/** daemon status file */
FILE_t *fnetinf = NULL;


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
      uint         l = 0;

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
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
//================================================================================================//
err_t ethernetif_init(struct netif *netif)
{
      struct ethernetif *ethernetif = malloc(sizeof(struct ethernetif));

      if (ethernetif == NULL) {
            goto ethernetif_init_error;
      }

#if LWIP_NETIF_HOSTNAME
      netif->hostname = SystemGetHostname();
#endif

      /*
       * Initialize the snmp variables and counters inside the struct netif.  The last argument
       * should be replaced with your link speed, in units of bits per second.
       */
      NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 100000000);

      netif->state   = ethernetif;
      netif->name[0] = 'e';
      netif->name[1] = '0';

      /*
       * we directly use etharp_output() here to save a function call. You can instead declare your
       * own function an call etharp_output() from it if you have to do some checks before sending
       * (e.g. if linkis available...)
       */
      netif->output     = etharp_output;
      netif->linkoutput = low_level_output;

      /* maximum transfer unit */
      netif->mtu = 1500;

      /* device capabilities */
      /* don't set NETIF_FLAG_ETHARP if this device is not an Ethernet one */
      netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

      /* set MAC hardware address length */
      netif->hwaddr_len = ETHARP_HWADDR_LEN;

      /* set MAC hardware address */
      netif->hwaddr[0] = MACADDR0;
      netif->hwaddr[1] = MACADDR1;
      netif->hwaddr[2] = MACADDR2;
      netif->hwaddr[3] = MACADDR3;
      netif->hwaddr[4] = MACADDR4;
      netif->hwaddr[5] = MACADDR5;

      ethernetif->ethaddr = (struct eth_addr*)&(netif->hwaddr[0]);

      u8_t macaddress[6] = {MACADDR0, MACADDR1, MACADDR2, MACADDR3, MACADDR4, MACADDR5};

      if (ioctl(ethf, ETHER_IORQ_SET_MAC_ADR, macaddress) != STD_RET_OK) {
            goto ethernetif_init_error;
      }

      /* allocate Rx and Tx buffers */
      Rx_Buff = malloc(sizeof(u8_t) * ETH_RXBUFNB * ETH_MAX_PACKET_SIZE);
      Tx_Buff = malloc(sizeof(u8_t) * ETH_TXBUFNB * ETH_MAX_PACKET_SIZE);

      if (!Rx_Buff || !Tx_Buff) {
            goto ethernetif_init_error;
      }

      /* initialize Tx Descriptors list: Chain Mode */
      struct DMADesc DMADesc;

      DMADesc.buffer       = Tx_Buff;
      DMADesc.buffer_count = ETH_TXBUFNB;

      if (ioctl(ethf, ETHER_IORQ_INIT_DMA_TX_DESC_LIST_CHAIN_MODE, &DMADesc) != STD_RET_OK) {
            goto ethernetif_init_error;
      }

      /* initialize Rx Descriptors list: Chain Mode */
      DMADesc.buffer       = Rx_Buff;
      DMADesc.buffer_count = ETH_RXBUFNB;

      if (ioctl(ethf, ETHER_IORQ_INIT_DMA_RX_DESC_LIST_CHAIN_MODE, &DMADesc) != STD_RET_OK) {
            goto ethernetif_init_error;
      }

      /* enable Ethernet Rx interrrupt */
      if (ioctl(ethf, ETHER_IORQ_ENABLE_RX_IRQ, NULL) != STD_RET_OK) {
            goto ethernetif_init_error;
      }

#ifdef CHECKSUM_BY_HARDWARE
      if (ioctl(ethf, ETHER_IORQ_ENABLE_TX_HARDWARE_CHECKSUM, NULL) != STD_RET_OK) {
            goto ethernetif_init_error;
      }
#endif

      /* start Ethernet interface */
      if (ioctl(ethf, ETHER_IORQ_ETHERNET_START, NULL) != STD_RET_OK) {
            goto ethernetif_init_error;
      }

      /* initialization OK */
      return ERR_OK;

      /* initialization error */
      ethernetif_init_error:
      if (ethernetif) {
            free(ethernetif);
      }

      if (Rx_Buff) {
            free(Rx_Buff);
      }

      if (Tx_Buff) {
            free(Tx_Buff);
      }

      return ERR_MEM;
}


//================================================================================================//
/**
 * @brief Function write status to file
 */
//================================================================================================//
void print_daemon_status(const ch_t *statusStr)
{
      u8_t retry = 5;

      /* print daemon information to file */
      while ((fnetinf = fopen(STATUS_FILE, "w")) == NULL && retry > 0) {
            retry--;
            milisleep(250);
      }

      if (fnetinf) {
            fprintf(fnetinf, "%s\n"
                             "  Hostname  : %s\n"
                             "  MAC       : %x2:%x2:%x2:%x2:%x2:%x2\n"
                             "  IP Address: %d.%d.%d.%d\n"
                             "  Net Mask  : %d.%d.%d.%d\n"
                             "  Gateway   : %d.%d.%d.%d\n",
                    statusStr,
                    SystemGetHostname(),
                    MACADDR0, MACADDR1, MACADDR2, MACADDR3, MACADDR4, MACADDR5,
                    ip4_addr1(&ipaddr),  ip4_addr2(&ipaddr),
                    ip4_addr3(&ipaddr),  ip4_addr4(&ipaddr),
                    ip4_addr1(&netmask), ip4_addr2(&netmask),
                    ip4_addr3(&netmask), ip4_addr4(&netmask),
                    ip4_addr1(&gateway), ip4_addr2(&gateway),
                    ip4_addr3(&gateway), ip4_addr4(&gateway)
            );

            fclose(fnetinf);
      } else {
            kprint("lwip: unable to open "STATUS_FILE"!\n");
      }
}


//================================================================================================//
/**
 * @brief Function wait to DHCP connection
 */
//================================================================================================//
void dhcp_conn_wait(bool_t dhcp_en, struct netif *netif)
{
      const ch_t *msg;

      if (netif_ready == FALSE) {
            if (dhcp_en == TRUE) {
#if LWIP_DHCP
                  if (netif->dhcp->state != DHCP_BOUND && dhcp_timeout_counter > 0) {
                        dhcp_timeout_counter--;
                        return;
                  }

                  if (dhcp_timeout_counter != 0) {
                        ip_addr_set(&ipaddr,  &netif->ip_addr);
                        ip_addr_set(&netmask, &netif->netmask);
                        ip_addr_set(&gateway, &netif->gw);

                        msg = FONT_COLOR_GREEN "DHCP Client connected to server." RESET_ATTRIBUTES;
                  } else {
                        dhcp_release(netif);
                        dhcp_stop(netif);

                        IP4_ADDR(&ipaddr , 192, 168, 0  , 20 );
                        IP4_ADDR(&netmask, 255, 255, 255, 0  );
                        IP4_ADDR(&gateway, 192, 168, 0  , 1  );

                        netif_set_addr(netif, &ipaddr, &netmask, &gateway);

                        msg = FONT_COLOR_RED "DHCP Client not connected to server!" RESET_ATTRIBUTES "\n"
                              " Setting static IP...";
                  }
#else
                  msg = "Setting static IP...";
#endif

            } else {
                  msg = "Setting static IP...";
            }

            netif_set_up(netif);
            netif_ready = TRUE;

            print_daemon_status(msg);
      }

      return;
}


//================================================================================================//
/**
 * @brief Function receive data from MAC
 */
//================================================================================================//
void receive_packet(struct netif *netif)
{
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
}


//================================================================================================//
/**
 * @brief Functions propel timers
 */
//================================================================================================//
void propel_timers(u32_t localtime)
{
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
}


//================================================================================================//
/**
 * @brief clear main function
 */
//================================================================================================//
stdRet_t appmain(ch_t *argv[], int argc)
{
      stdRet_t      status = STD_RET_ERROR;
      struct netif *netif  = malloc(sizeof(struct netif));
      bool_t        dhcp   = FALSE;

      if (ParseArg(argv, "dhcp", PARSE_AS_EXIST, NULL) == STD_RET_OK) {
#if LWIP_DHCP
            dhcp = TRUE;
#endif
      }

      /* check if net interface was allocated */
      if (netif == NULL) {
            if ((fnetinf = fopen(STATUS_FILE, "w")) != NULL) {
                  fprintf(fnetinf, "\x1"FONT_COLOR_RED "Unable to allocate netif interface!"RESET_ATTRIBUTES"\n");
            }
            goto lwipd_end;
      }

      /* check if Ethernet interface exist */
      ethf = fopen(ETH_FILE, "r");

      if (ethf == NULL) {
            if ((fnetinf = fopen(STATUS_FILE, "w")) != NULL) {
                  fprintf(fnetinf, "\x1"FONT_COLOR_RED "Ethernet interface does not exist!"RESET_ATTRIBUTES"\n");
            }
            goto lwipd_end;
      }

      /* initialize lwIP stack */
      lwip_init();

      /* DHCP or static initialization */
      if (dhcp == TRUE) {
#if LWIP_DHCP
            ipaddr.addr  = 0;
            netmask.addr = 0;
            gateway.addr = 0;
#endif
      } else {
            IP4_ADDR(&ipaddr , 192, 168, 0  , 20 );
            IP4_ADDR(&netmask, 255, 255, 255, 0  );
            IP4_ADDR(&gateway, 192, 168, 0  , 1  );
      }

      /*
       * the init function pointer must point to a initialization function for your ethernet netif
       * interface. The following code illustrates it's use.
       */
      if (netif_add(netif, &ipaddr, &netmask, &gateway, NULL, &ethernetif_init, &ethernet_input) == NULL) {
            if ((fnetinf = fopen(STATUS_FILE, "w")) != NULL) {
                  fprintf(fnetinf, "\x1"FONT_COLOR_RED "Cannot register network interface!"RESET_ATTRIBUTES"\n");
            }
            goto lwipd_end;
      }

      /* registers the default network interface */
      netif_set_default(netif);

#if LWIP_DHCP
      if (dhcp == TRUE) {
            /*
             * creates a new DHCP client for this interface on the first call.
             * Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at the predefined regular
             * intervals after starting the client. You can peek in the netif->dhcp struct for the actual
             * DHCP status.
             */
            if (ERR_MEM == dhcp_start(netif)) {
                  if ((fnetinf = fopen(STATUS_FILE, "w")) != NULL) {
                        fprintf(fnetinf, "\x1"FONT_COLOR_RED "DHCP start failed: out of memory!"RESET_ATTRIBUTES"\n");
                  }
                  goto lwipd_end;
            }
      }
#endif

      while (TRUE) {
            u32_t localtime = SystemGetOSTickCnt();

            dhcp_conn_wait(dhcp, netif);
            receive_packet(netif);
            propel_timers(localtime);

            milisleep(BASE_TIME);
      }


      lwipd_end:
      if (netif) {
            free(netif);
      }

      fclose(ethf);
      fclose(fnetinf);

      return status;
}

/* End of application section declaration */
PROG_SEC_END

#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
