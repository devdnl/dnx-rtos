/*=========================================================================*//**
@file    ifconfig.c

@author  Daniel Zorychta

@brief   Read network configuration

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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
//#include <dnx/net.h>
#include <dnx/misc.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define KiB                             static_cast(u32_t, 1024)
#define MiB                             static_cast(u32_t, 1024*1024)
#define CONVERT_TO_KiB(_val)            (_val >> 10)
#define CONVERT_TO_MiB(_val)            (_val >> 20)

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES_SECTION {
};

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief  Convert string to IP
 * @param  str          string IP
 * @return String converted to IP address
 */
//==============================================================================
//static net_ip_t strtoip(const char *str)
//{
//        int a = 0, b = 0, c = 0, d = 0;
//
//        sscanf(str, "%3d.%3d.%3d.%3d", &a, &b, &c, &d);
//
//        return net_IP_set(a, b, c, d);
//}
//
////==============================================================================
///**
// * @brief  Select unit according to value size
// * @param  val          value
// * @return Selected unit. Value is changed according to selected unit.
// */
////==============================================================================
//static const char *convert_unit(u64_t *val)
//{
//        if (*val >= 10*MiB) {
//                *val = CONVERT_TO_MiB(*val);
//                return "MiB";
//        } else if (*val >= 10*KiB) {
//                *val = CONVERT_TO_KiB(*val);
//                return "KiB";
//        } else {
//                return "B";
//        }
//}
//
////==============================================================================
///**
// * @brief  Shows connection details
// * @param  None
// * @return None
// */
////==============================================================================
//static void show_details()
//{
//        static const char *if_status[] = {
//                "NOT CONFIGURED",
//                "STATIC IP",
//                "DHCP CONFIGURING",
//                "DHCP CONFIGURED",
//                "DISCONNECTED"
//        };
//
//        net_config_t ifcfg;
//        memset(&ifcfg, 0, sizeof(net_config_t));
//        net_get_ifconfig(&ifcfg);
//
//        const char *tx_unit = convert_unit(&ifcfg.tx_bytes);
//        const char *rx_unit = convert_unit(&ifcfg.rx_bytes);
//
//        printf("Network status: %s\n"
//               "HWaddr %2X:%2X:%2X:%2X:%2X:%2X\n"
//               "Addr:%d.%d.%d.%d\n"
//               "Gateway:%d.%d.%d.%d\n"
//               "Mask:%d.%d.%d.%d\n"
//               "RX packets:%d (%d %s)\n"
//               "TX packets:%d (%d %s)\n",
//               if_status[ifcfg.status],
//               ifcfg.hw_address[0], ifcfg.hw_address[1], ifcfg.hw_address[2],
//               ifcfg.hw_address[3], ifcfg.hw_address[4], ifcfg.hw_address[5],
//               net_IP_get_part_a(&ifcfg.IP_address),
//               net_IP_get_part_b(&ifcfg.IP_address),
//               net_IP_get_part_c(&ifcfg.IP_address),
//               net_IP_get_part_d(&ifcfg.IP_address),
//               net_IP_get_part_a(&ifcfg.gateway),
//               net_IP_get_part_b(&ifcfg.gateway),
//               net_IP_get_part_c(&ifcfg.gateway),
//               net_IP_get_part_d(&ifcfg.gateway),
//               net_IP_get_part_a(&ifcfg.net_mask),
//               net_IP_get_part_b(&ifcfg.net_mask),
//               net_IP_get_part_c(&ifcfg.net_mask),
//               net_IP_get_part_d(&ifcfg.net_mask),
//               ifcfg.rx_packets, static_cast(int, ifcfg.rx_bytes), rx_unit,
//               ifcfg.tx_packets, static_cast(int, ifcfg.tx_bytes), tx_unit);
//}
//
////==============================================================================
///**
// * @brief  Print help message
// * @param  prog_name    program name
// * @return None
// */
////==============================================================================
//static void print_help_msg(const char *prog_name)
//{
//        printf("Usage: %s <options>\n"
//               "  addr=<address>\n"
//               "  mask=<netmask>\n"
//               "  gw=<gateway>\n"
//               "  dhcp\n"
//               "  up\n"
//               "  down\n",
//               prog_name);
//}

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
int_main(ifconfig, STACK_DEPTH_LOW, int argc, char *argv[])
{
#warning ifconfig: TODO network sockets

        if (argc == 1) {
//                show_details();
        } else {
//                bool up   = false;
//                bool down = false;
//                bool dhcp = false;
//
//                net_ip_t ip      = net_IP_set(0,0,0,0);
//                net_ip_t netmask = net_IP_set(0,0,0,0);
//                net_ip_t gateway = net_IP_set(0,0,0,0);
//
//                for (int i = 1; i < argc; i++) {
//                        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
//                                print_help_msg(argv[0]);
//                                return 0;
//                        } else if (strncmp(argv[i], "addr=", 5) == 0) {
//                                if (argv[i]) {
//                                        ip = strtoip(argv[i] + 5);
//                                }
//                        } else if (strncmp(argv[i], "mask=", 5) == 0) {
//                                if (argv[i]) {
//                                        netmask = strtoip(argv[i] + 5);
//                                }
//                        } else if (strncmp(argv[i], "gw=", 3) == 0) {
//                                if (argv[i]) {
//                                        gateway = strtoip(argv[i] + 3);
//                                }
//                        } else if (strcmp(argv[i], "dhcp") == 0) {
//                                dhcp = true;
//                        } else if (strcmp(argv[i], "up") == 0) {
//                                up = true;
//                        } else if (strcmp(argv[i], "down") == 0) {
//                                down = true;
//                        } else {
//                                print_help_msg(argv[0]);
//                                return 0;
//                        }
//                }
//
//                if (down && up) {
//                        puts("Wrong flags!");
//                        return 0;
//                }
//
//                net_config_t ifcfg;
//                memset(&ifcfg, 0, sizeof(net_config_t));
//                net_get_ifconfig(&ifcfg);
//
//                if (down) {
//                        if (  ifcfg.status == NET_STATUS_DHCP_CONFIGURED
//                           || ifcfg.status == NET_STATUS_DHCP_CONFIGURING) {
//
//                                if (net_DHCP_stop() != 0 && ifcfg.status != NET_STATUS_NOT_CONFIGURED) {
//                                        puts("DHCP stop error!");
//                                }
//                        } else {
//                                if (net_ifdown() != 0 && ifcfg.status != NET_STATUS_NOT_CONFIGURED) {
//                                        puts("Interface down fail!");
//                                }
//                        }
//                }
//
//                if (up) {
//                        if (dhcp) {
//                                if (net_DHCP_start() == 0) {
//                                        show_details();
//                                } else {
//                                        puts("DHCP start error!");
//                                }
//                        } else {
//                                if (ip.addr == 0) {
//                                        puts("Bad IP address!");
//                                        return 0;
//                                }
//
//                                if (netmask.addr == 0) {
//                                        puts("Bad netmask!");
//                                        return 0;
//                                }
//
//                                if (gateway.addr == 0) {
//                                        puts("Bad gateway address!");
//                                        return 0;
//                                }
//
//                                if (net_ifup(&ip, &netmask, &gateway) == 0) {
//                                        show_details();
//                                } else {
//                                        puts("Configuration fail!");
//                                }
//                        }
//                }
        }

        return 0;
}

/*==============================================================================
  End of file
==============================================================================*/
