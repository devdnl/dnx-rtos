/*=========================================================================*//**
@file    initd.c

@author  Daniel Zorychta

@brief   This file contain initialize and runtime daemon

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <dnx/os.h>
#include <dnx/net.h>
#include <dnx/thread.h>
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
        driver_init("afio", NULL);
        driver_init("pll", "/dev/pll");
        driver_init("uart1", "/dev/ttyS0");
        driver_init("tty0", "/dev/tty0");

        printk_enable("/dev/tty0");
        printk(FONT_COLOR_GREEN FONT_BOLD "%s/%s" FONT_NORMAL " by "
               FONT_COLOR_CYAN "%s " FONT_COLOR_YELLOW "%s" RESET_ATTRIBUTES "\n\n",
               get_OS_name(), get_kernel_name(), get_author_name(), get_author_email());

        driver_init("tty1", "/dev/tty1");
        driver_init("tty2", "/dev/tty2");
        driver_init("tty3", "/dev/tty3");
        driver_init("spi3-0", "/dev/spi_sda");
        driver_init("ethmac", "/dev/eth0");
        driver_init("crc", "/dev/crc");
        driver_init("irq", "/dev/irq");
        driver_init("sdspia", "/dev/sda");
        driver_init("sdspia1", "/dev/sda1");
        driver_init("sdspia2", "/dev/sda2");
        driver_init("sdspia3", "/dev/sda3");
        driver_init("sdspia4", "/dev/sda4");
        driver_init("i2c1-0", "/dev/ds1307");
        driver_init("loop0", "/dev/loop0");

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
        /* mbus-daemon start */
        printk("Starting mbus daemon... ");
        if (program_new("mbus_daemon", "/", NULL, NULL, NULL)) {
                printk("OK\n");
        } else {
                printk(FONT_COLOR_RED"fail"RESET_ATTRIBUTES"\n");
        }

        /* initializing SD card and detecting partitions */
        printk("Starting SD card... ");
        FILE *sd = fopen("/dev/sda", "r+");
        if (sd) {
                switch (ioctl(sd, IOCTL_SDSPI__INITIALIZE_CARD)) {
                case 1:
                        switch (ioctl(sd, IOCTL_SDSPI__READ_MBR)) {
                        case 1:
                                mount("fatfs", "/dev/sda1", "/mnt");
                                printk("initialized\n");
                                break;

                        case 0:
                                printk(FONT_COLOR_YELLOW"no partitions"RESET_ATTRIBUTES"\n");
                                break;

                        case -1:
                                printk(FONT_COLOR_RED"fail"RESET_ATTRIBUTES"\n");
                                break;
                        }
                        break;

                default:
                        printk(FONT_COLOR_RED"fail"RESET_ATTRIBUTES"\n");
                        break;
                }

                fclose(sd);
        } else {
                printk(FONT_COLOR_RED"fail"RESET_ATTRIBUTES"\n");
        }

        /* network up */
        printk("Configuring DHCP client... ");

        if (net_start_DHCP_client() == 0) {
                printk("OK\n");
        } else {
                printk(FONT_COLOR_RED"fail"RESET_ATTRIBUTES"\n");

                printk("Configuring static IP... ");

                net_ip_t ip, mask, gateway;
                net_set_ip(&ip, 192,168,0,120);
                net_set_ip(&mask, 255,255,255,0);
                net_set_ip(&gateway, 192,168,0,1);

                if (net_ifup(&ip, &mask, &gateway) == 0) {
                        printk("OK\n");
                } else {
                        printk(FONT_COLOR_RED"fail"RESET_ATTRIBUTES"\n");
                }
        }

        ifconfig_t ifcfg;
        int stat = net_get_ifconfig(&ifcfg);
        if (stat == 0 && ifcfg.status != IFSTATUS_NOT_CONFIGURED) {
                printk("  Hostname  : %s\n"
                       "  MAC       : %2x:%2x:%2x:%2x:%2x:%2x\n"
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
        FILE  *tty0 = NULL;
        while (!(tty0 = fopen("/dev/tty0", "r+"))) {
                sleep_ms(200);
        }

        int number_of_ttys = 0;
        ioctl(tty0, IOCTL_TTY__GET_NUMBER_OF_TTYS, &number_of_ttys);

        /* stdio program control */
        FILE   *tty[number_of_ttys];
        prog_t *program[number_of_ttys - 1];
        int     current_tty = -1;

        memset(tty, 0, sizeof(tty));
        memset(program, 0, sizeof(program));

        /* terminal size info */
        int col = 0;
        int row = 0;
        ioctl(tty0, IOCTL_TTY__GET_COL, &col);
        ioctl(tty0, IOCTL_TTY__GET_ROW, &row);
        printk("Terminal size: %d columns x %d rows\n", col, row);

        /* initd info about stack usage */
        printk("[%d] initd: free stack: %d levels\n\n", get_time_ms(), task_get_free_stack());

        /* change TTY for printk */
        printk_enable("/dev/tty3");

        for (;;) {
                ioctl(tty0, IOCTL_TTY__GET_CURRENT_TTY, &current_tty);

                if (current_tty >= 0 && current_tty < number_of_ttys - 1) {
                        if (!program[current_tty]) {
                                if (tty[current_tty] == NULL) {
                                        char path[16];
                                        snprintf(path, sizeof(path), "/dev/tty%c", '0' + current_tty);
                                        tty[current_tty] = fopen(path, "r+");
                                }

                                fprintf(tty[current_tty], "Welcome to %s (tty%d)\n",
                                        get_OS_name(), current_tty);


                                program[current_tty] = program_new("dsh", "/",
                                                                   tty[current_tty],
                                                                   tty[current_tty],
                                                                   tty[current_tty]);
                                if (!program[current_tty]) {
                                        perror("initd");
                                } else {
                                        printk("initd: shell started\n");
                                }
                        }
                }

                for (int i = 0; i < number_of_ttys - 1; i++) {
                        if (program[i]) {
                                if (program_is_closed(program[i])) {
                                        printk("initd: shell closed\n");
                                        program_delete(program[i]);
                                        program[i] = NULL;

                                        ioctl(tty[i], IOCTL_TTY__CLEAR_SCR);
                                        fclose(tty[i]);
                                        tty[i] = NULL;

                                        if (current_tty == i) {
                                                ioctl(tty0, IOCTL_TTY__SWITCH_TTY_TO, 0);
                                        }
                                }
                        }
                }

                sleep_ms(500);
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
