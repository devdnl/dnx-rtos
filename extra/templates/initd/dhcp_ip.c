
        printk("Configuring DHCP client... ");
        if (net_start_DHCP_client() == 0) {
                printk("OK\n");
        } else {
                printk(FONT_COLOR_RED"fail"RESET_ATTRIBUTES"\n");
        }
