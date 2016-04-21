/*=========================================================================*//**
File     netm.c

Author   Daniel Zorychta

Brief    Network management.

	 Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "net/netm.h"

#include "net/netman.h" // TODO

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/

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
 * @brief
 */
//==============================================================================
int _net_ifup(NET_family_t family, const void *config, size_t size)
{
        if (config && size) {
                _netman_init();

                switch (family) {
                case NET_FAMILY__INET:
                        if (sizeof(NET_INET_cfg_t) == size) {
                                const NET_INET_cfg_t *cfg = config;

                                switch (cfg->mode) {
                                case NET_INET_MODE__STATIC: {
                                        ip_addr_t addr, mask, gateway;
                                        IP4_ADDR(&addr,
                                                 cfg->address[0],
                                                 cfg->address[1],
                                                 cfg->address[2],
                                                 cfg->address[3]);

                                        IP4_ADDR(&mask,
                                                 cfg->mask[0],
                                                 cfg->mask[1],
                                                 cfg->mask[2],
                                                 cfg->mask[3]);

                                        IP4_ADDR(&gateway,
                                                 cfg->gateway[0],
                                                 cfg->gateway[1],
                                                 cfg->gateway[2],
                                                 cfg->gateway[3]);

                                        return _netman_if_up(&addr, &mask, &gateway);
                                }
                                break;

                                case NET_INET_MODE__DHCP_START:
                                        return _netman_start_DHCP_client();
                                break;

                                case NET_INET_MODE__DHCP_INFORM:
                                        return _netman_inform_DHCP_server();
                                break;

                                case NET_INET_MODE__DHCP_RENEW:
                                        return _netman_renew_DHCP_connection();
                                default:
                                        return EINVAL;
                                }
                        } else {
                                return EINVAL;
                        }
                        break;

                case NET_FAMILY__CAN:
                        return ENOTSUP;

                case NET_FAMILY__MICROLAN:
                        return ENOTSUP;

                case NET_FAMILY__RFM:
                        return ENOTSUP;

                default:
                        return EINVAL;
                }
        }

        return EINVAL;
}

int _net_ifdown(NET_family_t family)
{
        switch (family) {
        case NET_FAMILY__INET:
                return _netman_if_down();

        case NET_FAMILY__CAN:
                return ENOTSUP;

        case NET_FAMILY__MICROLAN:
                return ENOTSUP;

        case NET_FAMILY__RFM:
                return ENOTSUP;

        default:
                return EINVAL;
        }
}

int _net_ifstatus(NET_family_t family, void *status, size_t size)
{
        if (status && size) {
                switch (family) {
                case NET_FAMILY__INET:
                        if (sizeof(NET_INET_status_t) == size) {
                                NET_INET_status_t *stat = status;

                                _ifconfig_t inetstat;
                                if (_netman_get_ifconfig(&inetstat) == ESUCC) {
                                        memcpy(stat->hw_addr, inetstat.hw_address, sizeof(stat->hw_addr));
                                        stat->rx_bytes   = inetstat.rx_bytes;
                                        stat->tx_bytes   = inetstat.tx_bytes;
                                        stat->rx_packets = inetstat.rx_packets;
                                        stat->tx_packets = inetstat.tx_packets;

                                        stat->address[0] = ip4_addr1(&inetstat.IP_address);
                                        stat->address[1] = ip4_addr2(&inetstat.IP_address);
                                        stat->address[2] = ip4_addr3(&inetstat.IP_address);
                                        stat->address[3] = ip4_addr4(&inetstat.IP_address);

                                        stat->mask[0]    = ip4_addr1(&inetstat.net_mask);
                                        stat->mask[1]    = ip4_addr2(&inetstat.net_mask);
                                        stat->mask[2]    = ip4_addr3(&inetstat.net_mask);
                                        stat->mask[3]    = ip4_addr4(&inetstat.net_mask);

                                        stat->gateway[0] = ip4_addr1(&inetstat.gateway);
                                        stat->gateway[1] = ip4_addr2(&inetstat.gateway);
                                        stat->gateway[2] = ip4_addr3(&inetstat.gateway);
                                        stat->gateway[3] = ip4_addr4(&inetstat.gateway);

                                        stat->state      = inetstat.status;
                                        return ESUCC;
                                }

                                return EINVAL;
                        }

                        return EINVAL;

                case NET_FAMILY__CAN:
                        return ENOTSUP;

                case NET_FAMILY__MICROLAN:
                        return ENOTSUP;

                case NET_FAMILY__RFM:
                        return ENOTSUP;

                default:
                        return EINVAL;
                }
        }

        return EINVAL;
}

/*==============================================================================
  End of file
==============================================================================*/
