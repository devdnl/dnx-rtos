/*=========================================================================*//**
@file    ifconfig.c

@author  Daniel Zorychta

@brief   Read network configuration

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
#include "system/dnx.h"
#include "system/netapi.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES_SECTION_BEGIN
/* put here global variables */
GLOBAL_VARIABLES_SECTION_END

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Convert string to IP
 */
//==============================================================================
static netapi_ip_t strtoip(const char *str)
{
        netapi_ip_t ip;
        int a = 0, b = 0, c = 0, d = 0;

        sscanf(str, "%d.%d.%d.%d", &a, &b, &c, &d);

        netapi_set_ip(&ip, a, b, c, d);

        return ip;
}

//==============================================================================
/**
 * @brief Shows connection details
 */
//==============================================================================
static void show_details()
{
        static const char *if_status[] = {
                "NOT CONFIGURED",
                "STATIC IP",
                "DHCP CONFIGURING",
                "DHCP CONFIGURED"
        };

        ifconfig ifcfg;
        memset(&ifcfg, 0, sizeof(ifconfig));
        netapi_get_ifconfig(&ifcfg);

        printf("Network status: %s  HWaddr %2x:%2x:%2x:%2x:%2x:%2x\n"
               "inet addr:%d.%d.%d.%d  gateway:%d.%d.%d.%d  Mask:%d.%d.%d.%d\n"
               "RX packets:%d  RX bytes:%d\n"
               "TX packets:%d  TX bytes:%d\n",
               if_status[ifcfg.status],
               ifcfg.hw_address[0], ifcfg.hw_address[1], ifcfg.hw_address[2],
               ifcfg.hw_address[3], ifcfg.hw_address[4], ifcfg.hw_address[5],
               netapi_get_ip_part_a(&ifcfg.IP_address),
               netapi_get_ip_part_b(&ifcfg.IP_address),
               netapi_get_ip_part_c(&ifcfg.IP_address),
               netapi_get_ip_part_d(&ifcfg.IP_address),
               netapi_get_ip_part_a(&ifcfg.gateway),
               netapi_get_ip_part_b(&ifcfg.gateway),
               netapi_get_ip_part_c(&ifcfg.gateway),
               netapi_get_ip_part_d(&ifcfg.gateway),
               netapi_get_ip_part_a(&ifcfg.net_mask),
               netapi_get_ip_part_b(&ifcfg.net_mask),
               netapi_get_ip_part_c(&ifcfg.net_mask),
               netapi_get_ip_part_d(&ifcfg.net_mask),
               ifcfg.rx_packets, ifcfg.rx_bytes,
               ifcfg.tx_packets, ifcfg.tx_bytes);
}

//==============================================================================
/**
 * @brief Program main function
 *
 * @param  argc         count of arguments
 * @param *argv[]       argument table
 *
 * @return program status
 */
//==============================================================================
PROGRAM_MAIN(ifconfig, int argc, char *argv[])
{
        if (argc == 1) {
                show_details();
        } else {
                bool up   = false;
                bool down = false;
                bool dhcp = false;
                netapi_ip_t ip;
                netapi_ip_t netmask;
                netapi_ip_t gateway;

                netapi_set_ip(&ip     , 0,0,0,0);
                netapi_set_ip(&netmask, 0,0,0,0);
                netapi_set_ip(&gateway, 0,0,0,0);

                for (int i = 1; i < argc; i++) {
                        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
                                printf("Usage: %s <options>\n"
                                       "  addr <address>\n"
                                       "  mask <netmask>\n"
                                       "  gw <gateway>\n"
                                       "  dhcp\n"
                                       "  up\n"
                                       "  down\n",
                                       argv[0]);

                                return 0;
                        } else if (strcmp(argv[i], "addr") == 0) {
                                if (argv[i + 1]) {
                                        ip = strtoip(argv[i + 1]);
                                }
                        } else if (strcmp(argv[i], "mask") == 0) {
                                if (argv[i + 1]) {
                                        netmask = strtoip(argv[i + 1]);
                                }
                        } else if (strcmp(argv[i], "gw") == 0) {
                                if (argv[i + 1]) {
                                        gateway = strtoip(argv[i + 1]);
                                }
                        } else if (strcmp(argv[i], "dhcp") == 0) {
                                dhcp = true;
                        } else if (strcmp(argv[i], "up") == 0) {
                                up = true;
                        } else if (strcmp(argv[i], "down") == 0) {
                                down = true;
                        }
                }

                if (down && up) {
                        puts("Wrong flags!");
                        return 0;
                }

                ifconfig ifcfg;
                memset(&ifcfg, 0, sizeof(ifconfig));
                netapi_get_ifconfig(&ifcfg);

                if (down) {
                        if (  ifcfg.status == IFSTATUS_DHCP_CONFIGURED
                           || ifcfg.status == IFSTATUS_DHCP_CONFIGURING) {

                                if (netapi_stop_DHCP_client() != 0 && ifcfg.status != IFSTATUS_NOT_CONFIGURED) {
                                        puts("DHCP stop error!");
                                }
                        } else {
                                if (netapi_ifdown() != 0 && ifcfg.status != IFSTATUS_NOT_CONFIGURED) {
                                        puts("Interface down fail!");
                                }
                        }
                }

                if (up) {
                        if (dhcp) {
                                if (netapi_start_DHCP_client() == 0) {
                                        show_details();
                                } else {
                                        puts("DHCP start error!");
                                }
                        } else {
                                if (ip.addr == 0) {
                                        puts("Bad IP address!");
                                        return 0;
                                }

                                if (netmask.addr == 0) {
                                        puts("Bad netmask!");
                                        return 0;
                                }

                                if (gateway.addr == 0) {
                                        puts("Bad gateway address!");
                                        return 0;
                                }

                                if (netapi_ifup(&ip, &netmask, &gateway) == 0) {
                                        show_details();
                                } else {
                                        puts("Configuration fail!");
                                }
                        }
                }
        }

        return 0;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
