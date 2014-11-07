
        ifconfig_t ifcfg;
        if (net_get_ifconfig(&ifcfg) == 0 && ifcfg.status != IFSTATUS_NOT_CONFIGURED) {
                printk("  Hostname  : %s\n"
                       "  MAC       : %02X:%02X:%02X:%02X:%02X:%02X\n"
                       "  IP Address: %d.%d.%d.%d\n"
                       "  Net Mask  : %d.%d.%d.%d\n"
                       "  Gateway   : %d.%d.%d.%d\n",
                       get_host_name(),
                       ifcfg.hw_address[0], ifcfg.hw_address[1], ifcfg.hw_address[2],
                       ifcfg.hw_address[3], ifcfg.hw_address[4], ifcfg.hw_address[5],
                       net_get_ip_part_a(&ifcfg.IP_address), net_get_ip_part_b(&ifcfg.IP_address),
                       net_get_ip_part_c(&ifcfg.IP_address), net_get_ip_part_d(&ifcfg.IP_address),
                       net_get_ip_part_a(&ifcfg.net_mask), net_get_ip_part_b(&ifcfg.net_mask),
                       net_get_ip_part_c(&ifcfg.net_mask), net_get_ip_part_d(&ifcfg.net_mask),
                       net_get_ip_part_a(&ifcfg.gateway), net_get_ip_part_b(&ifcfg.gateway),
                       net_get_ip_part_c(&ifcfg.gateway), net_get_ip_part_d(&ifcfg.gateway));
        } else {
                printk("Network not configured\n");
        }
