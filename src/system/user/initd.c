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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <unistd.h>
#include "user/initd.h"
#include "system/dnx.h"
#include "system/ioctl.h"
#include "system/netapi.h"
#include "system/mount.h"
#include "system/thread.h"

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
        mkdir("/bin", 0666);
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

        int pll_init = driver_init("pll", NULL);

        driver_init("uart1", "/dev/ttyS0");

        if (pll_init != 0) {
                FILE *ttyS0 = fopen("/dev/ttyS0", "r+");
                if (ttyS0) {
                        ioctl(ttyS0, UART_IORQ_SET_BAUDRATE, 115200 * (CONFIG_CPU_TARGET_FREQ / PLL_CPU_BASE_FREQ));
                        fclose(ttyS0);
                }
        }

        driver_init("tty0", "/dev/tty0");

        printk_enable("/dev/tty0");

        printk(FONT_COLOR_GREEN FONT_BOLD "%s/%s" FONT_NORMAL " by "
               FONT_COLOR_CYAN "%s " FONT_COLOR_YELLOW "%s" RESET_ATTRIBUTES "\n\n",
               get_OS_name(), get_kernel_name(), get_author_name(), get_author_email());

        if (pll_init != 0) {
                printk(FONT_COLOR_RED"PLL not started, running no base frequency!"RESET_ATTRIBUTES"\n");
        }

        driver_init("tty1", "/dev/tty1");
        driver_init("tty2", "/dev/tty2");
        driver_init("tty3", "/dev/tty3");
        driver_init("sdspi", "/dev/sda");
        driver_init("ethmac", "/dev/eth0");

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
        /* initializing SD card and detecting partitions */
        printk("Detecting SD card... ");

        FILE *sd = fopen("/dev/sda", "r+");
        if (sd) {
                bool status;
                ioctl(sd, SDSPI_IORQ_INITIALIZE_CARD, &status);

                if (status == true) {
                        printk("initialized\n");
                        mount("fatfs", "/dev/sda1", "/mnt");
                } else {
                        printk(FONT_COLOR_RED"fail"RESET_ATTRIBUTES"\n");
                }

                fclose(sd);
        } else {
                printk(FONT_COLOR_RED"Cannot open file!"RESET_ATTRIBUTES"\n");
        }

        /* network up */
        printk("Configuring DHCP client... ");

        if (netapi_start_DHCP_client() == 0) {
                printk("OK\n");
        } else {
                printk(FONT_COLOR_RED"fail"RESET_ATTRIBUTES"\n");

                printk("Configuring static IP... ");

                netapi_ip_t ip, mask, gateway;
                netapi_set_ip(&ip, 192,168,0,120);
                netapi_set_ip(&mask, 255,255,255,0);
                netapi_set_ip(&gateway, 192,168,0,1);

                if (netapi_ifup(&ip, &mask, &gateway) == 0) {
                        printk("OK\n");
                } else {
                        printk(FONT_COLOR_RED"fail"RESET_ATTRIBUTES"\n");
                }
        }

        ifconfig ifcfg;
        netapi_get_ifconfig(&ifcfg);
        if (ifcfg.status != IFSTATUS_NOT_CONFIGURED) {
                printk("  Hostname  : %s\n"
                       "  MAC       : %2x:%2x:%2x:%2x:%2x:%2x\n"
                       "  IP Address: %d.%d.%d.%d\n"
                       "  Net Mask  : %d.%d.%d.%d\n"
                       "  Gateway   : %d.%d.%d.%d\n",
                       get_host_name(),
                       ifcfg.hw_address[0], ifcfg.hw_address[1], ifcfg.hw_address[2],
                       ifcfg.hw_address[3], ifcfg.hw_address[4], ifcfg.hw_address[5],
                       netapi_get_ip_part_a(&ifcfg.IP_address),  netapi_get_ip_part_b(&ifcfg.IP_address),
                       netapi_get_ip_part_c(&ifcfg.IP_address),  netapi_get_ip_part_d(&ifcfg.IP_address),
                       netapi_get_ip_part_a(&ifcfg.net_mask), netapi_get_ip_part_b(&ifcfg.net_mask),
                       netapi_get_ip_part_c(&ifcfg.net_mask), netapi_get_ip_part_d(&ifcfg.net_mask),
                       netapi_get_ip_part_a(&ifcfg.gateway), netapi_get_ip_part_b(&ifcfg.gateway),
                       netapi_get_ip_part_c(&ifcfg.gateway), netapi_get_ip_part_d(&ifcfg.gateway));
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
        /* stdio program control */
        FILE   *tty[TTY_DEV_COUNT]         = {NULL};
        FILE   *tty0                       =  NULL;
        prog_t *program[TTY_DEV_COUNT - 1] = {NULL};
        int     current_tty                = -1;

        while (!(tty0 = fopen("/dev/tty0", "r+"))) {
                sleep_ms(200);
        }

        /* initd info about stack usage */
        printk("[%d] initd: free stack: %d levels\n\n", get_time_ms(), task_get_free_stack());

        /* change TTY for printk */
        printk_enable("/dev/tty3");

        for (;;) {
                ioctl(tty0, TTY_IORQ_GET_CURRENT_TTY, &current_tty);

                if (current_tty >= 0 && current_tty < TTY_DEV_COUNT - 1) {
                        if (!program[current_tty]) {
                                if (tty[current_tty] == NULL) {
                                        char path[16];
                                        snprintf(path, sizeof(path), "/dev/tty%c", '0' + current_tty);
                                        tty[current_tty] = fopen(path, "r+");
                                }

                                fprintf(tty[current_tty], "Welcome to %s/%s (tty%d)\n",
                                        get_OS_name(), get_kernel_name(), current_tty);


                                program[current_tty] = program_new("terminal", "/",
                                                                   tty[current_tty],
                                                                   tty[current_tty],
                                                                   tty[current_tty]);
                                if (!program[current_tty]) {
                                        perror("initd");
                                } else {
                                        printk("initd: terminal started\n");
                                }
                        }
                }

                for (int i = 0; i < TTY_DEV_COUNT - 1; i++) {
                        if (program[i]) {
                                if (program_is_closed(program[i])) {
                                        printk("initd: terminal closed\n");
                                        program_delete(program[i]);
                                        program[i] = NULL;

                                        ioctl(tty[i], TTY_IORQ_CLEAR_SCR);
                                        fclose(tty[i]);
                                        tty[i] = NULL;

                                        if (current_tty == i) {
                                                ioctl(tty0, TTY_IORQ_SWITCH_TTY_TO, TTY_DEV_0);
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
                sleep_ms(MAX_DELAY);
        }

        return STD_RET_OK;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
