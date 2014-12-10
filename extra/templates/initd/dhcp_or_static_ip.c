
        printk("Configuring DHCP client... ");
        if (net_DHCP_start() == 0) {
                printk("OK\n");
        } else {
                printk(FONT_COLOR_RED"fail"RESET_ATTRIBUTES"\n");

                printk("Configuring static IP... ");
                net_ip_t ip      = net_IP_set(__NETWORK_IP_ADDR1__,__NETWORK_IP_ADDR2__,__NETWORK_IP_ADDR3__,__NETWORK_IP_ADDR4__);
                net_ip_t netmask = net_IP_set(__NETWORK_IP_MASK1__,__NETWORK_IP_MASK2__,__NETWORK_IP_MASK3__,__NETWORK_IP_MASK4__);
                net_ip_t gateway = net_IP_set(__NETWORK_IP_GW1__,__NETWORK_IP_GW2__,__NETWORK_IP_GW3__,__NETWORK_IP_GW4__);
                if (net_ifup(&ip, &netmask, &gateway) == 0) {
                        printk("OK\n");
                } else {
                        printk(FONT_COLOR_RED"fail"RESET_ATTRIBUTES"\n");
                }
        }
