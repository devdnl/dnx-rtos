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
};

static const char *INET_STATE[] = {
       "NOT CONFIGURED",
       "STATIC IP",
       "DHCP CONFIGURING",
       "DHCP",
       "LINK DISCONNECTED"
};

static const char *SIPC_STATE[] = {
       "NOT CONFIGURED",
       "CONFIGURED"
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
 * @brief  Print help message
 * @param  prog_name    program name
 * @return None
 */
//==============================================================================
static void print_help_msg(const char *prog_name)
{
        printf("Usage: %s <network> [up=<options>] [down]\n", prog_name);
        printf("General options:\n");
        printf("  INET,...      network name\n");
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
}

//==============================================================================
/**
 * @brief Function setup INET connection.
 * @param options       string options
 */
//==============================================================================
static void INET_up(const char *options)
{
        if (isstreq(options, "dhcp") || isstreq(options, "DHCP")) {
                static const NET_INET_config_t DHCP = {
                        .mode    = NET_INET_MODE__DHCP_START,
                        .address = NET_INET_IPv4_ANY,
                        .mask    = NET_INET_IPv4_ANY,
                        .gateway = NET_INET_IPv4_ANY
                };

                if (ifup(NET_FAMILY__INET, &DHCP) != 0) {
                        perror("INET");
                }

        } else {
                int aa = INT_MAX, ab = INT_MAX, ac = INT_MAX, ad = INT_MAX;
                int ma = INT_MAX, mb = INT_MAX, mc = INT_MAX, md = INT_MAX;
                int ga = INT_MAX, gb = INT_MAX, gc = INT_MAX, gd = INT_MAX;

                sscanf(options, "%4d.%4d.%4d.%4d,"
                                "%4d.%4d.%4d.%4d,"
                                "%4d.%4d.%4d.%4d",

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

                if (ifup(NET_FAMILY__INET, &config) != 0) {
                        perror("INET");
                }
        }
}


//==============================================================================
/**
 * @brief Function gets INET connection status.
 */
//==============================================================================
static void INET_status(void)
{
        NET_INET_status_t ifstat;
        if (ifstatus(NET_FAMILY__INET, &ifstat) == 0) {

                const char *tx_unit = convert_unit(&ifstat.tx_bytes);
                const char *rx_unit = convert_unit(&ifstat.rx_bytes);

                printf("INET\n"
                       "  Status : %s\n"
                       "  HWaddr : %02X:%02X:%02X:%02X:%02X:%02X\n"
                       "  Address: %d.%d.%d.%d\n"
                       "  Gateway: %d.%d.%d.%d\n"
                       "  Netmask: %d.%d.%d.%d\n"
                       "  RX packets: %u (%u %s)\n"
                       "  TX packets: %u (%u %s)\n",

                       INET_STATE[ifstat.state],

                       ifstat.hw_addr[0],
                       ifstat.hw_addr[1],
                       ifstat.hw_addr[2],
                       ifstat.hw_addr[3],
                       ifstat.hw_addr[4],
                       ifstat.hw_addr[5],

                       NET_INET_IPv4_a(ifstat.address),
                       NET_INET_IPv4_b(ifstat.address),
                       NET_INET_IPv4_c(ifstat.address),
                       NET_INET_IPv4_d(ifstat.address),

                       NET_INET_IPv4_a(ifstat.gateway),
                       NET_INET_IPv4_b(ifstat.gateway),
                       NET_INET_IPv4_c(ifstat.gateway),
                       NET_INET_IPv4_d(ifstat.gateway),

                       NET_INET_IPv4_a(ifstat.mask),
                       NET_INET_IPv4_b(ifstat.mask),
                       NET_INET_IPv4_c(ifstat.mask),
                       NET_INET_IPv4_d(ifstat.mask),

                       (uint)ifstat.rx_packets, cast(uint, ifstat.rx_bytes), rx_unit,
                       (uint)ifstat.tx_packets, cast(uint, ifstat.tx_bytes), tx_unit
               );
        }
}


//==============================================================================
/**
 * @brief Function setup SIPC connection.
 * @param options       string options
 */
//==============================================================================
static void SIPC_up(const char *options)
{
        UNUSED_ARG1(options);

        NET_SIPC_config_t conf;
        if (isstreq(options, "auto")) {
                conf.MTU = 1024;
        } else {
                int MTU = 0;
                sscanf(options, "%u", &MTU);
                conf.MTU = MTU;
        }

        if (ifup(NET_FAMILY__SIPC, &conf) != 0) {
                perror("INET");
        }
}

//==============================================================================
/**
 * @brief Function gets INET connection status.
 */
//==============================================================================
static void SIPC_status(void)
{
        NET_SIPC_status_t ifstat;
        if (ifstatus(NET_FAMILY__SIPC, &ifstat) == 0) {

                const char *tx_unit = convert_unit(&ifstat.tx_bytes);
                const char *rx_unit = convert_unit(&ifstat.rx_bytes);

                printf("SIPC\n"
                       "  Status: %s\n"
                       "  MTU: %u B\n"
                       "  RX packets: %u (%u %s)\n"
                       "  TX packets: %u (%u %s)\n",

                       SIPC_STATE[ifstat.state],
                       ifstat.MTU,
                       (uint)ifstat.rx_packets, cast(uint, ifstat.rx_bytes), rx_unit,
                       (uint)ifstat.tx_packets, cast(uint, ifstat.tx_bytes), tx_unit
               );
        } else {
                perror("SIPC");
        }
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
        if (argc == 1) {
                // print statuses of all networks
                INET_status();
                SIPC_status();

        } else if (argc >= 4) {
                // print help if too many arguments are given
                print_help_msg(argv[0]);

        } else {
                bool        down    = false;
                const char *up_args = NULL;
                const char *net     = NULL;

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
                                net = argv[i];
                        }
                }

                // configure selected network
                if (net) {
                        if (isstreq(net, "INET")) {
                                if (down) {
                                        ifdown(NET_FAMILY__INET);
                                } else if (up_args) {
                                        INET_up(up_args);
                                } else {
                                        INET_status();
                                }

                        } else if (isstreq(net, "SIPC")) {
                                if (down) {
                                        ifdown(NET_FAMILY__SIPC);
                                } else if (up_args) {
                                        SIPC_up(up_args);
                                } else {
                                        SIPC_status();
                                }

                        } else {
                                fputs("Unknown network family.\n", stderr);
                        }
                } else {
                        fputs("Unknown network family.\n", stderr);
                }
        }

        return 0;
}

/*==============================================================================
  End of file
==============================================================================*/
