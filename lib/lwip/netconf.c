/*=============================================================================================*//**
@file    netconf.c

@author  Daniel Zorychta

@brief   This file support low level configuration for Ethernet interface and TCP/IP stack

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
#include "lwip/memp.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwipopts.h"
#include "ethernetif.h"
#include "netconf.h"
#include "lwip/init.h"
#include "lwip/tcp_impl.h"
#include "stm32_eth.h"
#include "oswrap.h"
#include "vfs.h"
#include "appmoni.h"


/*==================================================================================================
                                   Local symbolic constants/macros
==================================================================================================*/
#define fopen(path, mode)                 vfs_fopen(path, mode)
#define fclose(file)                      vfs_fclose(file)


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static void LwIP_Daemon(void *argv);


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
static struct   netif netif;
static volatile u32_t TCPTimer = 0;
static volatile u32_t ARPTimer = 0;

#if LWIP_DHCP
static volatile u32_t DHCPfineTimer   = 0;
static volatile u32_t DHCPcoarseTimer = 0;
#endif

static bool_t packetReceived  = FALSE;
static bool_t netifInit       = FALSE;

static ch_t *defaultHostname = "localhost";


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                         Function definitions
==================================================================================================*/
//================================================================================================//
/**
 * @brief Initializes the lwIP stack
 *
 * @retval STD_STATUS_OK      configuration finished successfully
 * @retval STD_STATUS_ERROR   configuration error
 */
//================================================================================================//
stdRet_t LwIP_Init(void)
{
      struct ip_addr ipaddr;
      struct ip_addr netmask;
      struct ip_addr gw;
      uint8_t macaddress[6] = {0, 0, 0, 0, 0, 1};

      /* check if Ethernet interface exist */
      FILE_t *eth = fopen("/dev/eth0", "r");

      if (eth == NULL)
      {
            kprint("lwIP: Ethernet interface does not exist!");
            goto LwIP_Init_exit_Failure;
      }
      else
      {
            fclose(eth);
      }


      lwip_init();

      /* Initializes the dynamic memory heap defined by MEM_SIZE.*/
      mem_init();

      /* Initializes the memory pools defined by MEMP_NUM_x.*/
      memp_init();

      #if LWIP_DHCP
      ipaddr.addr  = 0;
      netmask.addr = 0;
      gw.addr      = 0;
      #else
      IP4_ADDR(&ipaddr , 192, 168, 0  , 20 );
      IP4_ADDR(&netmask, 255, 255, 255, 0  );
      IP4_ADDR(&gw     , 192, 168, 0  , 1  );
      #endif

      Set_MAC_Address(macaddress);

      /*
       * the init function pointer must point to a initialization function for your ethernet netif
       * interface. The following code illustrates it's use.
       */
      if (netif_add(&netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input) == NULL)
      {
            goto LwIP_Init_exit_Failure;
      }

      /* registers the default network interface */
      netif_set_default(&netif);

      /* start task which periodically perform LwIP */
      xTaskHandle LwIPDeamonHdl;
      if (TaskCreate(LwIP_Daemon, "lwipd", 5*MINIMAL_STACK_SIZE, NULL, 3, &LwIPDeamonHdl) != pdPASS)
      {
            goto LwIP_Init_exit_Failure;
      }

      moni_AddTask(LwIPDeamonHdl);

      #if LWIP_DHCP
      /*
       * creates a new DHCP client for this interface on the first call.
       * Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at the predefined regular
       * intervals after starting the client. You can peek in the netif->dhcp struct for the actual
       * DHCP status.
       */
      kprint("Starting DHCP Client..");
      if (ERR_MEM == dhcp_start(&netif))
      {
            moni_DelTask(LwIPDeamonHdl);
            TaskDelete(LwIPDeamonHdl);
            goto LwIP_Init_exit_Failure;
      }

      /* waiting for DHCP connection */
      u8_t times = 5;
      while (netif.dhcp->state != DHCP_BOUND && times > 0)
      {
            kprint(".");
            TaskDelay(1000);
            times--;
      }

      /* checking that DHCP connect */
      if (times > 0)
      {
            kprintOK();

            ip_addr_set(&ipaddr,  &netif.ip_addr);
            ip_addr_set(&netmask, &netif.netmask);
            ip_addr_set(&gw,      &netif.gw);
      }
      else
      {
            dhcp_release(&netif);
            dhcp_stop(&netif);

            kprintFail();

            kprint("Setting static IP...\n");
            IP4_ADDR(&ipaddr , 192, 168, 0  , 20 );
            IP4_ADDR(&netmask, 255, 255, 255, 0  );
            IP4_ADDR(&gw     , 192, 168, 0  , 1  );

            netif_set_addr(&netif, &ipaddr, &netmask, &gw);
      }
      #endif

      /* when the netif is fully configured this function must be called.*/
      netif_set_up(&netif);
      netifInit = TRUE;

      kprint("Hostname  : %s\n", netif.hostname);
      kprint("MAC       : %x2:%x2:%x2:%x2:%x2:%x2\n", macaddress[0], macaddress[1], macaddress[2],
                                                      macaddress[3], macaddress[4], macaddress[5]);
      kprint("IP Address: %d.%d.%d.%d\n", ip4_addr1(&ipaddr),  ip4_addr2(&ipaddr),
                                          ip4_addr3(&ipaddr),  ip4_addr4(&ipaddr));
      kprint("Net Mask  : %d.%d.%d.%d\n", ip4_addr1(&netmask), ip4_addr2(&netmask),
                                          ip4_addr3(&netmask), ip4_addr4(&netmask));
      kprint("Gateway   : %d.%d.%d.%d\n", ip4_addr1(&gw),      ip4_addr2(&gw),
                                          ip4_addr3(&gw),      ip4_addr4(&gw));
      kprint("lwIP configuration: ");

      /* configuration finished successfully */
      kprintOK();
      return STD_RET_OK;

      /* error occur */
      LwIP_Init_exit_Failure:
      kprintFail();
      return STD_RET_ERROR;
}


//================================================================================================//
/**
 * @brief Set flag when packet was received
 */
//================================================================================================//
void LwIP_SetReceiveFlag(void)
{
      packetReceived = TRUE;
}


//================================================================================================//
/**
 * @brief Returns pointer to the hostname
 *
 * @return pointer to the Hostname string
 */
//================================================================================================//
ch_t *LwIP_GetHostname(void)
{
      if (netifInit)
      {
            return netif.hostname;
      }
      else
      {
            return defaultHostname;
      }
}


//================================================================================================//
/**
 * @brief LwIP periodic task (lwip daemon)
 *
 * @param argument list
 */
//================================================================================================//
static void LwIP_Daemon(void *argv)
{
      (void) argv;

      while (TRUE)
      {
            /* receive packet from MAC */
            if (packetReceived)
            {
                  /* Handles all the received frames */
                  while(ETH_GetRxPktSize() != 0)
                  {
                        /*
                         * read a received packet from the Ethernet buffers and send it to the lwIP
                         * for handling
                         */
                        ethernetif_input(&netif);
                  }

                  packetReceived = FALSE;
            }

            u32_t localtime = TaskGetTickCount();

            /* TCP periodic process every 250 ms */
            if (localtime - TCPTimer >= TCP_TMR_INTERVAL)
            {
                  TCPTimer = localtime;
                  tcp_tmr();
            }

            /* ARP periodic process every 5s */
            if (localtime - ARPTimer >= ARP_TMR_INTERVAL)
            {
                  ARPTimer = localtime;
                  etharp_tmr();
            }

            #if LWIP_DHCP
            /* fine DHCP periodic process every 500ms */
            if (localtime - DHCPfineTimer >= DHCP_FINE_TIMER_MSECS)
            {
                  DHCPfineTimer = localtime;
                  dhcp_fine_tmr();
            }

            /* DHCP Coarse periodic process every 60s */
            if (localtime - DHCPcoarseTimer >= DHCP_COARSE_TIMER_MSECS)
            {
                  DHCPcoarseTimer = localtime;
                  dhcp_coarse_tmr();
            }
            #endif

            TaskDelay(50);
      }

      TaskTerminate();
}

#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                             End of file
==================================================================================================*/
