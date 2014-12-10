/*=========================================================================*//**
@file    initd.c

@author  Daniel Zorychta

@brief   This file contain initialize and runtime daemon

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <dnx/os.h>
#include <dnx/net.h>
#include <dnx/thread.h>
#include <dnx/misc.h>
#include "user/initd.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int run_level_boot(void);
static int run_level_0(void);
static int run_level_1(void);
static int run_level_2(void);
static int run_level_exit(void);

/*==============================================================================
  Local object definitions
==============================================================================*/

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief  Function initialize storage device
 * @param  storage  storage device path
 * @return None
 */
//==============================================================================
static void msg_mount(const char *filesystem, const char *src_file, const char *mount_point)
{
        printk("Mounting ");
        if (src_file != NULL && strlen(src_file) > 0) {
                printk("%s ", src_file);
        } else {
                printk("%s ", filesystem);
        }
        printk("to %s... ", mount_point);

        errno = 0;
        if (mount(filesystem, src_file, mount_point) == STD_RET_OK) {
                printk("OK\n");
        } else {
                printk(FONT_COLOR_RED" fail (%d)"RESET_ATTRIBUTES"\n", errno);
        }
}

//==============================================================================
/**
 * @brief  Function initialize storage device
 * @param  storage  storage device path
 * @return None
 */
//==============================================================================
static void init_storage(const char *storage)
{
        printk("Initializing %s... ", storage);
        FILE *st = fopen(storage, "r+");
        if (st) {
                if (ioctl(st, IOCTL_STORAGE__INITIALIZE)) {
                        switch (ioctl(st, IOCTL_STORAGE__READ_MBR)) {
                                case 1 : printk("OK\n"); break;
                                case 0 : printk("OK (no MBR)\n"); break;
                                default: printk(FONT_COLOR_RED"read error"RESET_ATTRIBUTES"\n");
                        }
                } else {
                        printk(FONT_COLOR_RED"fail"RESET_ATTRIBUTES"\n");
                }

                fclose(st);
        } else {
                printk(FONT_COLOR_RED"no such file"RESET_ATTRIBUTES"\n");
        }
}

//==============================================================================
/**
 * @brief  Function start daemon
 * @param  name     daemon's name
 * @param  cwd      current working directory
 * @return None
 */
//==============================================================================
static void start_daemon(const char *name, const char *cwd)
{
        printk("Starting '%s' daemon... ", name);
        if (program_new(name, cwd, NULL, NULL, NULL)) {
                printk("OK\n");
        } else {
                printk(FONT_COLOR_RED"fail"RESET_ATTRIBUTES"\n");
        }
}

//==============================================================================
/**
 * @brief Initialize devices and programs
 * User program which provide basic system functionality e.g. STDIO handle
 * (joining TTY driver with program), basic program starting and etc. This task
 * is an example to show how this can be implemented.
 */
//==============================================================================
void initd(void *arg)
{
        (void)arg;

        task_set_priority(INITD_PRIORITY);

        if (run_level_boot() != STD_RET_OK)
                goto start_failure;

        if (run_level_0() != STD_RET_OK)
                goto start_failure;

        if (run_level_1() != STD_RET_OK)
                goto start_failure;

        if (run_level_2() != STD_RET_OK)
                goto start_failure;

start_failure:
        run_level_exit();

        task_exit();
}

//==============================================================================
/**
 * @brief Run level at boot time
 *
 * @retval STD_RET_OK           run level finished successfully
 * @retval STD_RET_ERROR        run level error
 */
//==============================================================================
static int run_level_boot(void)
{
        mount("lfs", "", "/");
        mkdir("/dev", 0666);
        mkdir("/mnt", 0666);
        mkdir("/proc", 0666);
        mkdir("/tmp", 0666);
        mount("procfs", "", "/proc");
        mount("devfs", "", "/dev");

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Run level 0
 *
 * @retval STD_RET_OK           run level finished successfully
 * @retval STD_RET_ERROR        run level error
 */
//==============================================================================
static int run_level_0(void)
{
        driver_init("gpio", "/dev/gpio");
        driver_init("afiom", NULL);
        driver_init("pll", "/dev/pll");
        driver_init("uart1", "/dev/ttyS0");
        driver_init("tty0", "/dev/tty0");
        printk_enable("/dev/tty0");
        printk(FONT_COLOR_GREEN FONT_BOLD "%s/%s" FONT_NORMAL " by "
               FONT_COLOR_CYAN "%s " FONT_COLOR_GRAY "%s" RESET_ATTRIBUTES "\n\n",
               get_OS_name(), get_kernel_name(), get_author_name(), get_author_email());
        driver_init("tty1", "/dev/tty1");
        driver_init("tty2", "/dev/tty2");
        driver_init("tty3", "/dev/tty3");
        driver_init("spi3-0", "/dev/spi_sda");
        driver_init("sdspia", "/dev/sda");
        driver_init("sdspia1", "/dev/sda1");
        driver_init("sdspia2", "/dev/sda2");
        driver_init("sdspia3", "/dev/sda3");
        driver_init("sdspia4", "/dev/sda4");
        driver_init("ethmac", "/dev/eth0");
        driver_init("i2c1-0", "/dev/DS1307");
        driver_init("i2c1-1", "/dev/MPL115A2");

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Run level 1
 *
 * @retval STD_RET_OK           run level finished successfully
 * @retval STD_RET_ERROR        run level error
 */
//==============================================================================
static int run_level_1(void)
{
        start_daemon("mbusd", "/");
        init_storage("/dev/sda");
        msg_mount("fatfs", "/dev/sda1", "/mnt");

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

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Run level 2
 *
 * @retval STD_RET_OK           run level finished successfully
 * @retval STD_RET_ERROR        run level error
 */
//==============================================================================
static int run_level_2(void)
{
        printk("[%d] initd: free stack: %d levels\n\n", get_time_ms(), task_get_free_stack());
        printk("Welcome to dnx RTOS \"%s\"!\n", get_OS_codename());
        printk_enable("/dev/tty3");

        /* initialize handles for applications and streams */
        prog_t *p[3];
        memset(p, 0, sizeof(p));

        FILE *f[3];
        memset(f, 0, sizeof(f));

        /* open streams and start applications */
        f[0] = fopen("/dev/tty0", "r+");
        f[1] = fopen("/dev/tty1", "r+");
        f[2] = fopen("/dev/tty2", "r+");
        p[0] = program_new("dsh", "/", f[0], f[0], f[0]);
        p[1] = program_new("dsh", "/", f[1], f[1], f[1]);
        p[2] = program_new("dsh", "/", f[2], f[2], f[2]);

        /* waits until all applications are closed */
        while (true) {
                size_t closed_programs = 0;
                for (size_t i = 0; i < ARRAY_SIZE(p); i++) {
                        if (p[i]) {
                                if (program_is_closed(p[i])) {
                                        program_delete(p[i]);
                                        p[i] = NULL;
                                        closed_programs++;
                                }
                        } else {
                                closed_programs++;
                        }
                }

                if (closed_programs >= ARRAY_SIZE(p)) {
                        for (size_t i = 0; i < ARRAY_SIZE(f); i++) {
                                fclose(f[i]);
                        }

                        break;
                }

                sleep(1);
        }

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Run level exit
 *
 * @retval STD_RET_OK           run level finished successfully
 * @retval STD_RET_ERROR        run level error
 */
//==============================================================================
static int run_level_exit(void)
{
        critical_section_begin();
        ISR_disable();

        while (true) {
                sleep_ms(MAX_DELAY_MS);
        }

        return STD_RET_OK;
}

/*==============================================================================
  End of file
==============================================================================*/
