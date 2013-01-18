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

/* Begin of application section declaration */
APPLICATION(lwipd, 3)
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
//static struct   netif netif;
//static volatile u32_t TCPTimer = 0;
//static volatile u32_t ARPTimer = 0;
//
//#if LWIP_DHCP
//static volatile u32_t DHCPfineTimer   = 0;
//static volatile u32_t DHCPcoarseTimer = 0;
//#endif
//
//static bool_t packetReceived  = FALSE;
//static bool_t netifInit       = FALSE;
//
//static ch_t *defaultHostname = "localhost";


/*==================================================================================================
                                        Function definitions
==================================================================================================*/


//================================================================================================//
/**
 * @brief clear main function
 */
//================================================================================================//
stdRet_t appmain(ch_t *argv)
{
//      (void) argv;
//
//      while (TRUE)
//      {
//            /* receive packet from MAC */
//            if (packetReceived)
//            {
//                  /* Handles all the received frames */
//                  while(ETH_GetRxPktSize() != 0)
//                  {
//                        /*
//                         * read a received packet from the Ethernet buffers and send it to the lwIP
//                         * for handling
//                         */
//                        ethernetif_input(&netif);
//                  }
//
//                  packetReceived = FALSE;
//            }
//
//            u32_t localtime = TaskGetTickCount();
//
//            /* TCP periodic process every 250 ms */
//            if (localtime - TCPTimer >= TCP_TMR_INTERVAL)
//            {
//                  TCPTimer = localtime;
//                  tcp_tmr();
//            }
//
//            /* ARP periodic process every 5s */
//            if (localtime - ARPTimer >= ARP_TMR_INTERVAL)
//            {
//                  ARPTimer = localtime;
//                  etharp_tmr();
//            }
//
//            #if LWIP_DHCP
//            /* fine DHCP periodic process every 500ms */
//            if (localtime - DHCPfineTimer >= DHCP_FINE_TIMER_MSECS)
//            {
//                  DHCPfineTimer = localtime;
//                  dhcp_fine_tmr();
//            }
//
//            /* DHCP Coarse periodic process every 60s */
//            if (localtime - DHCPcoarseTimer >= DHCP_COARSE_TIMER_MSECS)
//            {
//                  DHCPcoarseTimer = localtime;
//                  dhcp_coarse_tmr();
//            }
//            #endif
//
//            TaskDelay(50);
//      }
//
//      TaskTerminate();
}

/* End of application section declaration */
APP_SEC_END

#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
