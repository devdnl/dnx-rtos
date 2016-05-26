/*=========================================================================*//**
@file    initd.c

@author  Daniel Zorychta

@brief   Initialization daemon

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <dnx/misc.h>
#include <dnx/os.h>
#include <dnx/thread.h>
#include <dnx/net.h>
#include <unistd.h>

#include "stm32f1/gpio_cfg.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

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
        const char *str;
};

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
static void thread2(void *arg)
{
        UNUSED_ARG1(arg);

        puts("I'm a thread in thread!");
        puts("Thread exit;");
}

static void thread(void *arg)
{
//        puts("This text is from thread function!");
//        printf("Thread arg: %d\n", (int)arg);
//
//
//        tid_t tid = thread_create(thread2, NULL, NULL);
//        thread_join(tid);

        int i = 0;
        while (i++ < 10) {
                for (int i = 0; i < 1000000; i++) {
                        __asm("nop");
                }
        }

//        global->str = "Text from thread!";
//
//        puts("Thread exit.");
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
int_main(initd, STACK_DEPTH_CUSTOM(240), int argc, char *argv[])
{
        UNUSED_ARG2(argc, argv);

        int result = 0;

        if (argc == 2 && strcmp(argv[1], "--child") == 0) {

                process_stat_t stat;
                if (process_stat(getpid(), &stat) == 0) {
                        printf("Name: %s\n", stat.name);
                        printf("PID: %d\n", stat.pid);
                        printf("Files: %d\n", stat.files_count);
                        printf("Dirs: %d\n", stat.dir_count);
                        printf("Mutexes: %d\n", stat.mutexes_count);
                        printf("Semaphores: %d\n", stat.semaphores_count);
                        printf("Queues: %d\n", stat.queue_count);
                        printf("Threads: %d\n", stat.threads_count);
                        printf("Mem blocks: %d\n", stat.memory_block_count);
                        printf("Memory usage: %d\n", stat.memory_usage);
                        printf("Stack size: %d\n", stat.stack_size);
                        printf("Stack max usage: %d\n", stat.stack_max_usage);
                } else {
                        perror(NULL);
                }

                sleep(2);
                puts("Hello! I'm child of initd parent!");

                printf("I have PID: %d\n", getpid());

                global->str = "Works!";

                char *cwd = calloc(100, 1);
                if (cwd) {
                        if (/*strcpy(cwd, "Test")*/getcwd(cwd, 100) == cwd) {
                                printf("CWD is: %s\n", cwd);
                        } else {
                                printf("CWD return error\n");
                        }

                        free(cwd);
                }

                int i = 0;
                while (true) {
//                        GPIO_SET_PIN(PB14);

//                        printf("=== Sec: %d\n", i++);
//
//                        int t = _kernel_get_number_of_tasks();
//                        printf("Number of tasks %d\n", t);

//                        disable_CPU_load_measurement();
//                        process_stat_t stat;
//                        if (process_stat(getpid(), &stat) == 0) {
//                                printf("Name: %s\n", stat.name);
//                                printf("PID: %d\n", stat.pid);
//                                printf("Files: %d\n", stat.files_count);
//                                printf("Dirs: %d\n", stat.dir_count);
//                                printf("Mutexes: %d\n", stat.mutexes_count);
//                                printf("Semaphores: %d\n", stat.semaphores_count);
//                                printf("Queues: %d\n", stat.queue_count);
//                                printf("Threads: %d\n", stat.threads_count);
//                                printf("Mem blocks: %d\n", stat.memory_block_count);
//                                printf("Memory usage: %d\n", stat.memory_usage);
//                                printf("Stack size: %d\n", stat.stack_size);
//                                printf("Stack max usage: %d\n", stat.stack_max_usage);
//                                printf("CPU load cnt: %d\n", stat.cpu_load_cnt);
//
//                                u32_t tct = stat.cpu_load_total_cnt;
//                                printf("CPU total cnt: %d\n", tct);
//
//                                printf("CPU load: %d.%02d%%\n",
//                                       stat.cpu_load_cnt * 100 / tct,
//                                       (stat.cpu_load_cnt * 1000 / tct) % 10
//                                       );
//                        } else {
//                                perror(NULL);
//                        }
//                        enable_CPU_load_measurement();


//                          process_stat_t stat;
//                          size_t         seek = 0;
//                          while (process_stat_seek(seek++, &stat) == 0) {
//                                  u32_t tct = stat.cpu_load_total_cnt;
//                                  printf("%d %s: %d.%02d%% %s TH:%d\n",
//                                         stat.pid,
//                                         stat.name,
//                                         stat.cpu_load_cnt * 100 / tct,
//                                         (stat.cpu_load_cnt * 1000 / tct) % 10,
//                                         stat.zombie ? "Z" : "R",
//                                         stat.threads_count
//                                        );
//                          }
//                          enable_CPU_load_measurement();


//                        for (int i = 0; i < 10000000; i++) {
//                                __asm("nop");
//                        }

//                        if (i == 3) {
//                                int status = -1;
//                                int err = process_destroy(2, &status);
//
//                                printf("Killed zombie: %d : %d\n", err, status);
//                        }
//
//                        if (i == 4) {
//                                perror("perror(): test");
//                                fputs("stdout test\n", stdout);
//                                fputs("stderr test\n", stderr);
//                        }
//
//
//                        if (i == 5 || i == 100 || i == 200) {
//
//                                tid_t tid1 = thread_create(thread, NULL, (void*)0);
//                                tid_t tid2 = thread_create(thread, NULL, (void*)1);
//
//                                printf("Threads: tid1: %d; tid2: %d\n", tid1, tid2);
//
//                                thread_join(tid1);
//                                puts("Thread 1 joined");
//
//                                thread_join(tid2);
//                                puts("Thread 2 joined");
//
//                                puts(global->str);
//                        }

//                        GPIO_CLEAR_PIN(PB14);
//                        sleep(1);
                }

        } if (argc == 2 && strcmp(argv[1], "--wait") == 0) {
                puts("I'm process that wait 2 seconds");
                sleep(2);
                puts("Bye!");

        } else {
                result = mount("ramfs", "", "/");

                result = mkdir("/dev", 0777);
                mkdir("/tmp", 0777);
//                result = mkdir("/proc", 0777);

//                result = mount("devfs", "", "/dev");
//                result = mount("procfs", "", "/proc");

                driver_init("GPIO", 0, 0, "/dev/GPIOA");
                driver_init("GPIO", 1, 0, "/dev/GPIOB");
                driver_init("GPIO", 2, 0, "/dev/GPIOC");
                driver_init("GPIO", 3, 0, "/dev/GPIOD");
                driver_init("GPIO", 4, 0, "/dev/GPIOE");

                driver_init("AFIO", 0, 0, NULL);
                driver_init("PLL", 0, 0, "/dev/pll");
                driver_init("UART", 1, 0, "/dev/ttyS0");
                driver_init("TTY", 0, 0, "/dev/tty0");
                result = syslog_enable("/dev/tty0");
                detect_kernel_panic(true);
                driver_init("TTY", 1, 0, "/dev/tty1");
                driver_init("TTY", 2, 0, "/dev/tty2");
                driver_init("TTY", 3, 0, "/dev/tty3");
                driver_init("CRC", 0, 0, "/dev/crc");

                driver_init("SPI", 0, 0, "/dev/spi_sda");
                driver_init("SPI", 2, 1, "/dev/SPI3-1");
                driver_init("SPI", 2, 2, "/dev/SPI3-2");
                driver_init("SPI", 2, 3, "/dev/SPI3-3");

                FILE *f = fopen("/dev/spi_sda", "r+");
                if (f) {
                        static const SPI_config_t cfg = {
                                .flush_byte  = 0xFF,
                                .clk_divider = SPI_CLK_DIV__8,
                                .mode        = SPI_MODE__0,
                                .msb_first   = true,
                                .CS_port_idx = IOCTL_GPIO_PORT_IDX__SD_CS,
                                .CS_pin_idx  = IOCTL_GPIO_PIN_IDX__SD_CS
                        };
                        ioctl(f, IOCTL_SPI__SET_CONFIGURATION, &cfg);
                        fclose(f);
                }

                driver_init("SDSPI", 0, 0, "/dev/sda");
                driver_init("SDSPI", 0, 1, "/dev/sda1");

                driver_init("LOOP", 0, 0, "/dev/l0");

                driver_init("RTC", 0, 0, "/dev/rtc");

                driver_init("ETHMAC", 0, 0, "/dev/ethmac");


//                driver_release("SPI", 2, 0);
//                driver_release("SPI", 2, 1);
//                driver_release("SPI", 2, 2);
//                driver_release("SPI", 2, 3);




                stdout = fopen("/dev/tty0", "r+");
                stderr = stdout;


                sleep(2);
                puts("Starting DHCP client...\n");

                static const NET_INET_config_t cfg_static = {
                        .mode    = NET_INET_MODE__STATIC,
                        .address = NET_INET_IPv4(192,168,0,150),
                        .mask    = NET_INET_IPv4(255,255,255,0),
                        .gateway = NET_INET_IPv4(192,168,0,1)
                };

                static const NET_INET_config_t cfg_dhcp = {
                        .mode    = NET_INET_MODE__DHCP_START,
                        .address = NET_INET_IPv4_ANY,
                        .mask    = NET_INET_IPv4_ANY,
                        .gateway = NET_INET_IPv4_ANY
                };

                errno = 0;
                if (ifup(NET_FAMILY__INET, &cfg_dhcp) != 0) {
                        perror("ifup");
                }

              /*  if (ifup(NET_FAMILY__INET, &cfg2, sizeof(NET_INET_cfg_t)) != 0) {
                        perror("DHCP inform");
                }*/

                sleep(1);

                NET_INET_status_t status;
                if (ifstatus(NET_FAMILY__INET, &status) != 0) {
                        perror(NULL);
                } else {
                        printf("Status: %d\n", status.state);
                        printf("Address: %d.%d.%d.%d\n", NET_INET_IPv4_a(status.address), NET_INET_IPv4_b(status.address), NET_INET_IPv4_c(status.address), NET_INET_IPv4_d(status.address));
                        printf("Mask: %d.%d.%d.%d\n", NET_INET_IPv4_a(status.mask), NET_INET_IPv4_b(status.mask), NET_INET_IPv4_c(status.mask), NET_INET_IPv4_d(status.mask));
                        printf("Gateway: %d.%d.%d.%d\n", NET_INET_IPv4_a(status.gateway), NET_INET_IPv4_b(status.gateway), NET_INET_IPv4_c(status.gateway), NET_INET_IPv4_d(status.gateway));
                        printf("MAC0: %02X\n", status.hw_addr[0]);
                        printf("MAC1: %02X\n", status.hw_addr[1]);
                        printf("MAC2: %02X\n", status.hw_addr[2]);
                        printf("MAC3: %02X\n", status.hw_addr[3]);
                        printf("MAC4: %02X\n", status.hw_addr[4]);
                        printf("MAC5: %02X\n", status.hw_addr[5]);
                        printf("tx bytes: %d\n", status.tx_bytes);
                        printf("rx bytes: %d\n", status.rx_bytes);
                }


                sleep(2);

                syslog_enable("/dev/tty3");

                static const process_attr_t attr0 = {
                       .cwd = "/",
                       .f_stderr   = NULL,
                       .f_stdin    = NULL,
                       .f_stdout   = NULL,
                       .detached   = true,
                       .p_stderr   = "/dev/tty0",
                       .p_stdin    = "/dev/tty0",
                       .p_stdout   = "/dev/tty0"
                };
                process_create("top", &attr0);

                static const process_attr_t attr1 = {
                       .cwd = "/",
                       .f_stderr   = NULL,
                       .f_stdin    = NULL,
                       .f_stdout   = NULL,
                       .detached   = true,
                       .p_stderr   = "/dev/tty1",
                       .p_stdin    = "/dev/tty1",
                       .p_stdout   = "/dev/tty1"
                };
                process_create("dsh", &attr1);

                static const process_attr_t attr2 = {
                       .cwd = "/",
                       .f_stderr   = NULL,
                       .f_stdin    = NULL,
                       .f_stdout   = NULL,
                       .detached   = true,
                       .p_stderr   = "/dev/tty2",
                       .p_stdin    = "/dev/tty2",
                       .p_stdout   = "/dev/tty2"
                };
                process_create("dsh", &attr2);

//                _netman_start_DHCP_client();

//                FILE *f = fopen("/dev/tty0", "w");
//                stdout = f;
//
//                for (;;) {
//                        u32_t used = get_used_memory();
//                        printf("Used memory: %d\n", (int)used);
//
//                        pid_t pid = process_create("top", &attr0);
//                        if (pid) {
//                                process_wait(pid, NULL, MAX_DELAY_MS);
//                        }
//
//                        puts("Process closed");
//
//                        sleep(4);
//                }
        }

        return result;
}

/*==============================================================================
  End of file
==============================================================================*/
