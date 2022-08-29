/*==============================================================================
File     netman.h

Author   Daniel Zorychta

Brief    Network Manager.

         Copyright (C) 2022 Daniel Zorychta <daniel.zorychta@gmail.com>

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

==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include <dnx/net.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <conf_parser.h>
#include <sys/shm.h>
#include "shared.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define debug(...)                      printf("NM: "__VA_ARGS__);

#define ETHERNET_FILE                   "/dev/eth"
#define ETHERNET_CONF_FILE              "/mmc/conf/ethernet.conf"
#define NETWORK_CONF_FILE               "/mmc/conf/network.conf"

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void handle_interface(void);
static int configure_ethernet(void);
static void ifup_by_config(void);

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES_SECTION {
        struct {
                conf_parser_t *ethernet;
                conf_parser_t *network;
        } conf;

        netman_shared_t *shared;
};

/*==============================================================================
  Exported object definitions
==============================================================================*/
PROGRAM_PARAMS(netman, STACK_DEPTH_LOW);

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Program main function
 */
//==============================================================================
int main(int argc, char *argv[])
{
        if (argc < 2) {
                printf("Usage: %s [option]\n", argv[0]);
                puts("  --daemon        daemon mode");
                puts("  --conf-reload   reload configuration");
                return EXIT_FAILURE;
        }

        char *option = argv[1];

        if (isstreq(option, "--daemon")) {

                void *mem;
                size_t mem_size;
                if (shmat(NETMAN_SHM, &mem, &mem_size) == 0) {
                        debug("daemon is already running!\n");
                        shmdt(NETMAN_SHM);
                        return EXIT_FAILURE;
                }

                if (shmget(NETMAN_SHM, sizeof(netman_shared_t)) != 0) {
                        perror(argv[0]);
                        return EXIT_FAILURE;
                }

                if (shmat(NETMAN_SHM, cast(void **, &global->shared), &mem_size) != 0) {
                        perror(argv[0]);
                        shmrm(NETMAN_SHM);
                        return EXIT_FAILURE;
                }

                global->shared->reload_conf = true;

                handle_interface();

                shmdt(NETMAN_SHM);
                shmrm(NETMAN_SHM);

        } else if (isstreq(option, "--conf-reload")) {

                size_t mem_size;
                if (shmat(NETMAN_SHM, cast(void **, &global->shared), &mem_size) != 0) {
                        perror(argv[0]);
                        debug("deamon is not running.\n");
                        return EXIT_FAILURE;
                }

                global->shared->reload_conf = true;

                shmdt(NETMAN_SHM);

        } else {
                debug("Unkown option: '%s'\n", option);
                return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
}

//==============================================================================
/**
 * @brief  Handle network interface.
 */
//==============================================================================
static void handle_interface(void)
{
        if (configure_ethernet() != 0) {
                return;
        }

        global->conf.network = conf_parser__new(NETWORK_CONF_FILE);
        if (!global->conf.network) {
                fclose(fopen(NETWORK_CONF_FILE, "w"));
                conf_parser__set_file_value(NETWORK_CONF_FILE, "mode", "DHCP");
                conf_parser__set_file_value(NETWORK_CONF_FILE, "address", "0.0.0.0");
                conf_parser__set_file_value(NETWORK_CONF_FILE, "netmask", "0.0.0.0");
                conf_parser__set_file_value(NETWORK_CONF_FILE, "gateway", "0.0.0.0");

                global->conf.network = conf_parser__new(NULL);
        }

        NET_INET_state_t last_state = NET_INET_STATE__LINK_DISCONNECTED;

        while (true) {

                if (global->shared->reload_conf) {
                        ifdown("inet");
                        if (conf_parser__load_file(global->conf.network, NETWORK_CONF_FILE) > 0) {
                                global->shared->reload_conf = false;
                        } else {
                                sleep(5);
                                continue;
                        }
                }

                NET_INET_status_t status;
                NET_family_t family;
                if (ifstatus("inet", &family, &status, sizeof(status)) == 0) {

                        switch (status.state) {
                        case NET_INET_STATE__NOT_CONFIGURED:
                                ifup_by_config();
                                break;

                        case NET_INET_STATE__LINK_DISCONNECTED:
                                if (last_state != status.state) {
                                        debug("link disconnected\n");
                                }
                                ifdown("inet");
                                break;

                        case NET_INET_STATE__DHCP_CONFIGURING:
                                break;

                        case NET_INET_STATE__STATIC_IP:
                                if (last_state != status.state) {
                                        debug("estabilished static IP connection.\n");
                                }
                                break;

                        case NET_INET_STATE__DHCP_CONFIGURED:
                                if (last_state != status.state) {
                                        debug("estabilished DHCP IP connection.\n");
                                }
                                break;

                        default:
                                ifdown("inet");
                                break;
                        }

                        last_state = status.state;
                } else {
                        ifdown("inet");
                        ifup_by_config();
                }

                sleep(2);
        }
}

//==============================================================================
/**
 * @brief  Configure ethernet.
 */
//==============================================================================
static int configure_ethernet(void)
{
        int err = -1;

        global->conf.ethernet = conf_parser__new(ETHERNET_CONF_FILE);
        if (!global->conf.ethernet) {

                fclose(fopen(ETHERNET_CONF_FILE, "w"));
                conf_parser__set_file_value(ETHERNET_CONF_FILE, "MAC", "C2:70:50:FF:FF:78");
                conf_parser__set_file_value(ETHERNET_CONF_FILE, "auto-negotiation", "true");
                conf_parser__set_file_value(ETHERNET_CONF_FILE, "speed", "100M");
                conf_parser__set_file_value(ETHERNET_CONF_FILE, "duplex", "full");

                global->conf.ethernet = conf_parser__new(ETHERNET_CONF_FILE);
                if (!global->conf.ethernet) {
                        debug("%s: no configuration file.\n", ETHERNET_CONF_FILE);
                        return err;
                }
        }

        conf_parser_t *cp = global->conf.ethernet;
        const char *conf_mac     = conf_parser__get_value(cp, "MAC");
        const char *conf_autoneg = conf_parser__get_value(cp, "auto-negotiation");
        const char *conf_speed   = conf_parser__get_value(cp, "speed");
        const char *conf_duplex  = conf_parser__get_value(cp, "duplex");

        ETH_config_t conf = {0};

        if (sscanf(conf_mac, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                &conf.MAC[0], &conf.MAC[1], &conf.MAC[2],
                &conf.MAC[3], &conf.MAC[4], &conf.MAC[5] ) != ARRAY_SIZE(conf.MAC)) {

                debug("%s: invalid MAC address: '%s'\n", ETHERNET_CONF_FILE, conf_mac);
                err = -1;
                goto finish;
        }

        conf.auto_negotiation = isstreq(conf_autoneg, "true") ? true : false;
        conf.speed = isstreq(conf_speed, "100M") ? ETH_SPEED__100Mbps : ETH_SPEED__10Mbps;
        conf.duplex = isstreq(conf_duplex, "full") ? ETH_DUPLEX__FULL : ETH_DUPLEX__HALF;


        int fd = open(ETHERNET_FILE, O_RDWR);
        if (fd >= 0) {
                err = ioctl(fd, IOCTL_ETH__CONFIGURE, &conf);

                if (not err) {
                        err = ioctl(fd, IOCTL_ETH__START);
                }

                if (not err) {
                        err = ifadd("inet", NET_FAMILY__INET, "/dev/eth");
                }

                close(fd);
        } else {
                debug("%s: %s\n", ETHERNET_FILE, strerror(errno));
                err = -1;
                goto finish;
        }

        finish:
        conf_parser__delete(cp);
        return err;
}

//==============================================================================
/**
 * @brief  Function configure network interface according to current configuration.
 */
//==============================================================================
static void ifup_by_config(void)
{
        conf_parser_t *cp = global->conf.network;

        const char *conf_ip_address = conf_parser__get_value(cp, "address");
        const char *conf_ip_gateway = conf_parser__get_value(cp, "gateway");
        const char *conf_ip_netmask = conf_parser__get_value(cp, "netmask");
        const char *conf_ip_mode    = conf_parser__get_value(cp, "mode");

        bool dhcp_enable = true;
        if (isstreq(conf_ip_mode, "DHCP")) dhcp_enable = true;
        else if (isstreq(conf_ip_mode, "static")) dhcp_enable = false;
        else debug("%s: unknown mode value: '%s', used DHCP\n", NETWORK_CONF_FILE, conf_ip_mode);

        u8_t ip_address[4];
        if (sscanf(conf_ip_address, "%hhu.%hhu.%hhu.%hhu",
               &ip_address[0], &ip_address[1], &ip_address[2], &ip_address[3]) != ARRAY_SIZE(ip_address)) {
                debug("%s: invalid ip address: '%s', used DHCP\n", NETWORK_CONF_FILE, conf_ip_address);
        }

        u8_t ip_gateway[4];
        if (sscanf(conf_ip_gateway, "%hhu.%hhu.%hhu.%hhu",
               &ip_gateway[0], &ip_gateway[1], &ip_gateway[2], &ip_gateway[3]) != ARRAY_SIZE(ip_gateway)) {
                debug("%s: invalid ip address: '%s', used DHCP\n", NETWORK_CONF_FILE, conf_ip_gateway);
        }

        u8_t ip_netmask[4];
        if (sscanf(conf_ip_netmask, "%hhu.%hhu.%hhu.%hhu",
               &ip_netmask[0], &ip_netmask[1], &ip_netmask[2], &ip_netmask[3]) != ARRAY_SIZE(ip_netmask)) {
                debug("%s: invalid ip address: '%s', used DHCP\n", NETWORK_CONF_FILE, conf_ip_netmask);
        }

        NET_INET_config_t netcfg;

        if (dhcp_enable) {
                debug("applying DHCP configuration...\n");
                netcfg.mode    = NET_INET_MODE__DHCP_START;
                netcfg.address = NET_INET_IPv4_ANY;
                netcfg.mask    = NET_INET_IPv4_ANY;
                netcfg.gateway = NET_INET_IPv4_ANY;

        } else {
                debug("applying static configuration...\n");
                netcfg.mode    = NET_INET_MODE__STATIC;
                netcfg.address = NET_INET_IPv4(ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
                netcfg.mask    = NET_INET_IPv4(ip_netmask[0], ip_netmask[1], ip_netmask[2], ip_netmask[3]);
                netcfg.gateway = NET_INET_IPv4(ip_gateway[0], ip_gateway[1], ip_gateway[2], ip_gateway[3]);
        }

        if (ifup("inet", &netcfg, sizeof(netcfg)) != 0) {
                ifdown("inet");
        }
}

/*==============================================================================
  End of file
==============================================================================*/
