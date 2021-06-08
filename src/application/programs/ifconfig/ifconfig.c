/*=========================================================================*//**
@file    ifconfig.c

@author  Daniel Zorychta

@brief   Read network configuration

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes dnx RTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


*//*==========================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <string.h>
#include <dnx/net.h>
#include <dnx/misc.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define KiB                             cast(u32_t, 1024)
#define MiB                             cast(u32_t, 1024*1024)
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
        char *network[32];

        union {
                NET_INET_status_t inet;
                NET_SIPC_status_t sipc;
                NET_CANNET_status_t cannet;
        } status;
};

/*==============================================================================
  Exported object definitions
==============================================================================*/
PROGRAM_PARAMS(ifconfig, STACK_DEPTH_LOW);

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief  Print help message
 * @param  prog_name    program name
 * @return None
 */
//==============================================================================
static void print_help_msg(const char *prog_name)
{
        printf("Usage: %s <network> [up=<options>] [down]\n", prog_name);
        printf("General options:\n");
        printf("  network       network name\n");
        printf("  up=<options>  configure network\n");
        printf("  down          disable network\n");
        printf("  -h, --help    this help\n");
        printf("\n");
        printf("Options for INET:\n");
        printf("  Static: %s INET up=address,netmask,gateway\n", prog_name);
        printf("  DHCP  : %s INET up=dhcp\n", prog_name);
        printf("\n");
        printf("Options for SIPC:\n");
        printf("  MTU setup  : %s SIPC up=MTU\n", prog_name);
        printf("  Default MTU: %s SIPC up=auto\n", prog_name);
        printf("\n");
        printf("Options for CANNET:\n");
        printf("  %s CANNET up=address\n", prog_name);
        printf("\n");
}

//==============================================================================
/**
 * @brief  Select unit according to value size
 * @param  val          value
 * @return Selected unit. Value is changed according to selected unit.
 */
//==============================================================================
static const char *convert_unit(u64_t *val)
{
        if (*val >= 10*MiB) {
                *val = CONVERT_TO_MiB(*val);
                return "MiB";
        } else if (*val >= 10*KiB) {
                *val = CONVERT_TO_KiB(*val);
                return "KiB";
        } else {
                return "B";
        }
}

//==============================================================================
/**
 * @brief Function setup INET connection.
 * @param options       string options
 */
//==============================================================================
static void INET_up(const char *netname, const char *options)
{
        if (isstreq(options, "dhcp") || isstreq(options, "DHCP")) {
                static const NET_INET_config_t DHCP = {
                        .mode    = NET_INET_MODE__DHCP_START,
                        .address = NET_INET_IPv4_ANY,
                        .mask    = NET_INET_IPv4_ANY,
                        .gateway = NET_INET_IPv4_ANY
                };

                if (ifup(netname, &DHCP) != 0) {
                        perror(netname);
                }

        } else {
                int aa = INT_MAX, ab = INT_MAX, ac = INT_MAX, ad = INT_MAX;
                int ma = INT_MAX, mb = INT_MAX, mc = INT_MAX, md = INT_MAX;
                int ga = INT_MAX, gb = INT_MAX, gc = INT_MAX, gd = INT_MAX;

                sscanf(options, "%4u.%4u.%4u.%4u,"
                                "%4u.%4u.%4u.%4u,"
                                "%4u.%4u.%4u.%4u",

                       &aa, &ab, &ac, &ad,
                       &ma, &mb, &mc, &md,
                       &ga, &gb, &gc, &gd);

                if ((aa > 255) or (ab > 255) or (ac > 255) or (ad > 255)) {
                        fputs("Incorrect format of IP address.\n", stderr);
                        return;
                }

                if ((ma > 255) or (mb > 255) or (mc > 255) or (md > 255)) {
                        fputs("Incorrect format of network mask.\n", stderr);
                        return;
                }

                if ((ga > 255) or (gb > 255) or (gc > 255) or (gd > 255)) {
                        fputs("Incorrect format of gateway address.\n", stderr);
                        return;
                }

                NET_INET_config_t config = {
                        .mode    = NET_INET_MODE__STATIC,
                        .address = NET_INET_IPv4(aa,ab,ac,ad),
                        .mask    = NET_INET_IPv4(ma,mb,mc,md),
                        .gateway = NET_INET_IPv4(ga,gb,gc,gd)
                };

                if (ifup(netname, &config) != 0) {
                        perror(netname);
                }
        }
}


//==============================================================================
/**
 * @brief Function gets INET connection status.
 */
//==============================================================================
static void INET_print_status(void)
{
        static const char *INET_STATE[] = {
               "NOT CONFIGURED",
               "STATIC IP",
               "DHCP CONFIGURING",
               "DHCP",
               "LINK DISCONNECTED"
        };

        const char *tx_unit = convert_unit(&global->status.inet.tx_bytes);
        const char *rx_unit = convert_unit(&global->status.inet.rx_bytes);

        printf("INET\n"
               "  Status : %s\n"
               "  HWaddr : %02X:%02X:%02X:%02X:%02X:%02X\n"
               "  Address: %u.%u.%u.%u\n"
               "  Gateway: %u.%u.%u.%u\n"
               "  Netmask: %u.%u.%u.%u\n"
               "  RX packets: %llu (%llu %s)\n"
               "  TX packets: %llu (%llu %s)\n",

               INET_STATE[global->status.inet.state],

               global->status.inet.hw_addr[0],
               global->status.inet.hw_addr[1],
               global->status.inet.hw_addr[2],
               global->status.inet.hw_addr[3],
               global->status.inet.hw_addr[4],
               global->status.inet.hw_addr[5],

               NET_INET_IPv4_a(global->status.inet.address),
               NET_INET_IPv4_b(global->status.inet.address),
               NET_INET_IPv4_c(global->status.inet.address),
               NET_INET_IPv4_d(global->status.inet.address),

               NET_INET_IPv4_a(global->status.inet.gateway),
               NET_INET_IPv4_b(global->status.inet.gateway),
               NET_INET_IPv4_c(global->status.inet.gateway),
               NET_INET_IPv4_d(global->status.inet.gateway),

               NET_INET_IPv4_a(global->status.inet.mask),
               NET_INET_IPv4_b(global->status.inet.mask),
               NET_INET_IPv4_c(global->status.inet.mask),
               NET_INET_IPv4_d(global->status.inet.mask),

               global->status.inet.rx_packets, global->status.inet.rx_bytes, rx_unit,
               global->status.inet.tx_packets, global->status.inet.tx_bytes, tx_unit
       );
}


//==============================================================================
/**
 * @brief Function setup SIPC connection.
 * @param options       string options
 */
//==============================================================================
static void SIPC_up(const char *netname, const char *options)
{
        NET_SIPC_config_t conf;
        if (isstreq(options, "auto")) {
                conf.MTU = 1024;
        } else {
                u32_t MTU = 0;
                sscanf(options, "%lu", &MTU);
                conf.MTU = MTU;
        }

        if (ifup(netname, &conf) != 0) {
                perror(netname);
        }
}

//==============================================================================
/**
 * @brief Function gets INET connection status.
 */
//==============================================================================
static void SIPC_print_status(void)
{
        static const char *SIPC_STATE[] = {
               "NOT CONFIGURED",
               "CONFIGURED"
        };

        const char *tx_unit = convert_unit(&global->status.sipc.tx_bytes);
        const char *rx_unit = convert_unit(&global->status.sipc.rx_bytes);

        printf("SIPC\n"
               "  Status: %s\n"
               "  MTU: %u B\n"
               "  RX packets: %llu (%llu %s)\n"
               "  TX packets: %llu (%llu %s)\n",

               SIPC_STATE[global->status.sipc.state],
               global->status.sipc.MTU,
               global->status.sipc.rx_packets, global->status.sipc.rx_bytes, rx_unit,
               global->status.sipc.tx_packets, global->status.sipc.tx_bytes, tx_unit
       );
}


//==============================================================================
/**
 * @brief Function setup SIPC connection.
 * @param options       string options
 */
//==============================================================================
static void CANNET_up(const char *netname, const char *options)
{
        NET_CANNET_config_t conf;
        u32_t addr = 0;
        sscanf(options, "%lu", &addr);
        conf.addr = addr;

        if (ifup(netname, &conf) != 0) {
                perror(netname);
        }
}

//==============================================================================
/**
 * @brief Function gets INET connection status.
 */
//==============================================================================
static void CANNET_print_status(void)
{
        static const char *CANNET_STATE[] = {
               "NOT CONFIGURED",
               "CONFIGURED"
        };

        const char *tx_unit = convert_unit(&global->status.cannet.tx_bytes);
        const char *rx_unit = convert_unit(&global->status.cannet.rx_bytes);

        printf("CANNET\n"
               "  Status: %s\n"
               "  Address: %u\n"
               "  MTU: %u B\n"
               "  RX packets: %llu (%llu %s)\n"
               "  TX packets: %llu (%llu %s)\n",

               CANNET_STATE[global->status.cannet.state],
               global->status.cannet.addr,
               global->status.cannet.MTU,
               global->status.cannet.rx_packets, global->status.cannet.rx_bytes, rx_unit,
               global->status.cannet.tx_packets, global->status.cannet.tx_bytes, tx_unit
       );
}

//==============================================================================
/**
 * @brief Function print network status.
 */
//==============================================================================
static void print_network_status(const char *network, NET_family_t family)
{
        printf("%s, ", network);

        if (family == _NET_FAMILY__COUNT) {
                puts("UNKNOWN");

        } else if (family == NET_FAMILY__INET) {
                INET_print_status();

        } else if (family == NET_FAMILY__SIPC) {
                SIPC_print_status();

        } else if (family == NET_FAMILY__CANNET) {
                CANNET_print_status();
        }

        puts("");
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
int main(int argc, char *argv[])
{
        iflist(global->network, ARRAY_SIZE(global->network));

        if (argc == 1) {
                uint num_of_networks = 0;
                for (; global->network[num_of_networks]; num_of_networks++);
                printf("Number of networks: %u\n", num_of_networks);

                char **network = global->network;
                while (*network) {
                        NET_family_t family;
                        if (ifstatus(*network, &family, &global->status) == 0) {
                                print_network_status(*network, family);
                        }
                        network++;
                }

        } else if (argc >= 4) {
                // print help if too many arguments are given
                print_help_msg(argv[0]);

        } else {
                bool        down    = false;
                const char *up_args = NULL;
                const char *netname = NULL;

                // analyze arguments
                for (int i = 1; i < argc; i++) {
                        if (isstreq(argv[i], "-h") || isstreq(argv[i], "--help")) {
                                print_help_msg(argv[0]);
                                return 0;

                        } else if (isstreq(argv[i], "down")) {
                                down = true;

                        } else if (isstreqn(argv[i], "up=", 3)) {
                                up_args = argv[i] + 3;

                        } else {
                                netname = argv[i];
                        }
                }

                // configure selected network
                if (netname) {
                        NET_family_t family;
                        if (ifstatus(netname, &family, &global->status) == 0) {
                                if (down) {
                                        ifdown(netname);

                                } else if (up_args) {
                                        if (family == _NET_FAMILY__COUNT) {
                                                fputs("Unknown network.\n", stderr);

                                        } else if (family == NET_FAMILY__INET) {
                                                INET_up(netname, up_args);

                                        } else if (family == NET_FAMILY__SIPC) {
                                                SIPC_up(netname, up_args);

                                        } else if (family == NET_FAMILY__CANNET) {
                                                CANNET_up(netname, up_args);
                                        }
                                } else {
                                        print_network_status(netname, family);
                                }
                        }

                } else {
                        fputs("Unknown network.\n", stderr);
                }
        }

        return 0;
}

/*==============================================================================
  End of file
==============================================================================*/
