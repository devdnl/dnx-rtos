
        net_config_t ifcfg;
        if (net_get_ifconfig(&ifcfg) == 0 && ifcfg.status != NET_STATUS_NOT_CONFIGURED) {
                printk("  Hostname  : %s\n"
                       "  MAC       : %02X:%02X:%02X:%02X:%02X:%02X\n"
                       "  IP Address: %d.%d.%d.%d\n"
                       "  Net Mask  : %d.%d.%d.%d\n"
                       "  Gateway   : %d.%d.%d.%d\n",
                       get_host_name(),
                       ifcfg.hw_address[0], ifcfg.hw_address[1], ifcfg.hw_address[2],
                       ifcfg.hw_address[3], ifcfg.hw_address[4], ifcfg.hw_address[5],
                       net_IP_get_part_a(&ifcfg.IP_address), net_IP_get_part_b(&ifcfg.IP_address),
                       net_IP_get_part_c(&ifcfg.IP_address), net_IP_get_part_d(&ifcfg.IP_address),
                       net_IP_get_part_a(&ifcfg.net_mask), net_IP_get_part_b(&ifcfg.net_mask),
                       net_IP_get_part_c(&ifcfg.net_mask), net_IP_get_part_d(&ifcfg.net_mask),
                       net_IP_get_part_a(&ifcfg.gateway), net_IP_get_part_b(&ifcfg.gateway),
                       net_IP_get_part_c(&ifcfg.gateway), net_IP_get_part_d(&ifcfg.gateway));
        } else {
                printk("Network not configured\n");
        }
