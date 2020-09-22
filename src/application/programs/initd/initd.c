/*=========================================================================*//**
@file    initd.c

@author  Daniel Zorychta

@brief   Initialization daemon - example.

@note    Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes dnx RTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


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
        char str[80];
};

/*==============================================================================
  Exported object definitions
==============================================================================*/
PROGRAM_PARAMS(initd, STACK_DEPTH_LOW);

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief Function initializes basic file system.
 */
//==============================================================================
static void create_base_file_system_structure(void)
{
        /*
         * 1. Mount base file system as root.
         */
        mount("ramfs", "", "/", "");

        /*
         * 2. Create basic folders. This folders can be used to store device
         *    files or to mount next file systems e.g. SD Card etc.
         */
        mkdir("/dev", 0666);    // this folder store devices drivers
        mkdir("/tmp", 0666);    // this folder store temporary files
        mkdir("/run", 0666);    // this folder store runtime files
        mkdir("/mnt", 0666);    // this folder store SD card content

        /*
         * NOTE: Number and naming of folders depends on user needs. There can
         *       be mounted additional file systems that does not requires
         *       drivers to work (e.g. procfs, devfs, etc). Remember that this
         *       stage does not give access to drivers because drivers will be
         *       initialized in the next runlevel.
         */
}

//==============================================================================
/**
 * @brief Function initialize basic drivers needed by CPU configuration or board
 *        specification.
 */
//==============================================================================
static void initialize_basic_drivers(void)
{
        /*
         * 1. Initialize GPIO drivers. Number of GPIO drivers depends on microcontroller
         *    pinout. If microcontroller has only 2 GPIO ports then only those
         *    two should be initialized. Not all GPIOs should have file representation.
         *    If file is not needed (because port is not used from application)
         *    then node is not necessary (use NULL or empty string as file name).
         */
        driver_init("GPIO", 0, 0, "/dev/GPIOA");
        driver_init("GPIO", 1, 0, "/dev/GPIOB");
        driver_init("GPIO", 2, 0, "/dev/GPIOC");
        driver_init("GPIO", 3, 0, "/dev/GPIOD");
        driver_init("GPIO", 4, 0, "/dev/GPIOE");

        /*
         * 2. Next part of drivers that can be initialized at this early stage.
         */
        driver_init("AFM", 0, 0, NULL);                 // alternative function configuration
        driver_init("CLK", 0, 0, "/dev/clk");           // system clock configuration

        /*
         * NOTE: make sure that UART1 is used as terminal output!
         *       Some BSPs use UART2 as default terminal output.
         */
        driver_init("UART", 0, 0, "/dev/ttyS0");        // UART1 will be used as TTY I/O

        static const TTY_config_t TTY_config = {
                .input_file   = "/dev/ttyS0",
                .output_file  = "/dev/ttyS0",
                .clear_screen = true,
        };
        driver_init2("TTY", 0, 0, "/dev/tty0", &TTY_config);     // first user terminal

        /*
         * NOTE: Drivers that are initialized in this stage can be reduced or
         *       added because this depends on user needs. There are drivers
         *       that cannot be initialized in this runlevel because are using
         *       access to basic drivers e.g. SDSPI (SD Card over SPI).
         *       The configuration of drivers can be done by using project
         *       configuration -- Configtool. Some drivers need additional
         *       runtime configuration. In this case see driver help.
         */
}

//==============================================================================
/**
 * @brief Function open output stream. Stream is should be opened as soon as
 *        possible to present user system messages.
 */
//==============================================================================
static void open_output_stream(void)
{
        /*
         * 1. Open standard output and standard error streams to see system
         *    messages. Device that can be used as output stream was initialized
         *    in the previous stage (tty0 connected to UART2).
         */
        stdout = fopen("/dev/tty0", "r+");
        stderr = stdout;
}

//==============================================================================
/**
 * @brief Show kernel panic message.
 */
//==============================================================================
static void show_kernel_panic_message(void)
{
        /*
         * 1. Show kernel panic message if occurred in the last cycle.
         *    Kernel panic message is redirected to the standard output.
         *    Implementation hold kernel panic message by 3 seconds.
         */
        if (detect_kernel_panic(stdout)) {
                sleep(3);
        }
}

//==============================================================================
/**
 * @brief Function initialize additional drivers.
 */
//==============================================================================
static void initialize_additional_drivers(void)
{
        /*
         * 1. Initialize next terminals that can be used to hold user applications.
         *    Number of terminals can be configured in the TTY module configuration
         *    by using Configtool. Number of terminals depends on user needs.
         *    In special cases TTY terminals does not need to be initialized.
         */
        driver_init("TTY", 0, 1, "/dev/tty1");
        driver_init("TTY", 0, 2, "/dev/tty2");
        driver_init("TTY", 0, 3, "/dev/tty3");

        /*
         * 2. If real time clock is needed then RTC driver should be initialized.
         */
        driver_init("RTC", 0, 0, "/dev/rtc");

        /*
         * 3. If needed the Ethernet driver is initialized.
         */
        driver_init("ETHMAC", 0, 0, "/dev/ethmac");
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
static void mount_SD_card(void)
{
        /*
         * 1. SD Card is connected to the microcontroller by using SPI interface.
         *    In this case (stm32f1xx) SD card is connected to the SPI1. In this
         *    case the SPI driver should be initialized.
         *    The /dev/spi_sda is the SPI interface path that is used by SD card
         *    driver as data source.
         */
        driver_init("SPI", 0, 0, "/dev/spi_sda");

        /*
         * 2. SPI interface should be configured to meet SD card requirements.
         *    In this case SPI interface is opened and configured.
         */
        FILE *f = fopen("/dev/spi_sda", "r+");
        if (f) {
                static const SPI_config_t cfg = {
                        .flush_byte  = 0xFF,
                        .clk_divider = SPI_CLK_DIV__8,
                        .mode        = SPI_MODE__0,
                        .msb_first   = true,
                        .CS_port_idx = IOCTL_GPIO_PORT_IDX__NULL,      // port name configured in GPIO driver
                        .CS_pin_idx  = IOCTL_GPIO_PIN_IDX__NULL        // pin name configured in GPIO driver
                };

                // configuration setup
                ioctl(fileno(f), IOCTL_SPI__SET_CONFIGURATION, &cfg);
                fclose(f);
        }

        /*
         * 3. Initialization of SD card driver - SDSPI. This module handle SD card
         *    by using SPI interface.
         */
        driver_init("SDSPI", 0, 0, "/dev/sda");
        driver_init("SDSPI", 0, 1, "/dev/sda1");
        driver_init("SDSPI", 0, 2, "/dev/sda2");

        /*
         * 4. SD Card initialization and MBR read. After this operation SD card
         *    is ready to use and driver know how many partitions is on the card.
         */
        f = fopen("/dev/sda", "r+");
        if (f) {
                static const SDSPI_config_t cfg = {
                         .filepath = "/dev/spi_sda",    // SPI interface connected to SD card
                         .timeout  = 1000
                };

                ioctl(fileno(f), IOCTL_SDSPI__CONFIGURE, &cfg);
                ioctl(fileno(f), IOCTL_STORAGE__INITIALIZE);
                ioctl(fileno(f), IOCTL_STORAGE__READ_MBR);
                fclose(f);
        }

        /*
         * 5. Partition mount. The partition contains e.g. FAT32 file system.
         *    The file system will be mounted in the /mnt folder created in the
         *    previous stage. The EXT2,3,4 can be used alternatively (ext4fs).
         */
        mount("fatfs", "/dev/sda1", "/mnt", "");
}

//==============================================================================
/**
 * @brief Function print system log messages.
 */
//==============================================================================
static void print_system_log_messages(void)
{
        /*
         * 1. This function prints all system log messages that were stored
         *    at system startup. It can be used in debug purposes. It can be
         *    also disabled if not needed. Function run in thread.
         */
        struct timeval t = {0, 0};

        while (true) {
                if (syslog_read(global->str, sizeof(global->str), &t, &t)) {
                        printf("[%u.%06u] %s\n", t.tv_sec, t.tv_usec, global->str);
                } else {
                        break;
                }
        }
}

//==============================================================================
/**
 * @brief Function start DHCP client to get network address.
 */
//==============================================================================
static void start_DHCP_client(void)
{
        /*
         * 1. The DHCP configuration should be prepared to get network address.
         *    The static IP configuration can be used as well.
         */
        puts("Starting DHCP client...\n");

        static const NET_INET_config_t cfg_dhcp = {
                .mode    = NET_INET_MODE__DHCP_START,
                .address = NET_INET_IPv4_ANY,
                .mask    = NET_INET_IPv4_ANY,
                .gateway = NET_INET_IPv4_ANY
        };

        /*
         * 2. Command setup network interface.
         */
        errno = 0;
        if (ifup(NET_FAMILY__INET, &cfg_dhcp) != 0) {
                perror("ifup");
        }

        // 1 second delay to network setup.
        sleep(1);
}

//==============================================================================
/**
 * @brief Function start user programs.
 */
//==============================================================================
static void start_user_programs(void)
{
        /*
         * 1. Start the first terminal emulator that can be accessed by clicking F1
         *    key. Terminal is connected to the tty0.
         */
        static const process_attr_t attr0 = {
               .cwd        = "/",
               .f_stderr   = NULL,
               .f_stdin    = NULL,
               .f_stdout   = NULL,
               .detached   = true,
               .p_stderr   = "/dev/tty0",
               .p_stdin    = "/dev/tty0",
               .p_stdout   = "/dev/tty0"
        };

        process_create("dsh", &attr0);


        /*
         * 2. Start second terminal emulator that can be accessed by clicking F2
         *    key. Terminal is connected to the tty1.
         */
        static const process_attr_t attr1 = {
               .cwd        = "/",
               .f_stderr   = NULL,
               .f_stdin    = NULL,
               .f_stdout   = NULL,
               .detached   = true,
               .p_stderr   = "/dev/tty1",
               .p_stdin    = "/dev/tty1",
               .p_stdout   = "/dev/tty1"
        };

        process_create("dsh", &attr1);


        /*
         * 3. Start user application that can be accessed by clicking F3
         *    key. Application is connected to the tty2.
         */
        static const process_attr_t attr2 = {
               .cwd        = "/",
               .f_stderr   = NULL,
               .f_stdin    = NULL,
               .f_stdout   = NULL,
               .detached   = true,
               .p_stderr   = "/dev/tty2",
               .p_stdin    = "/dev/tty2",
               .p_stdout   = "/dev/tty2"
        };

        process_create("user-program-name", &attr2);

        /*
         * NOTE: User should decide which application should be started. In most
         *       cases only one terminal emulator is needed. In some cases,
         *       terminal emulator does not need to be run. It depends on
         *       user-specific project.
         */
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
int main(int argc, char *argv[])
{
        UNUSED_ARG2(argc, argv);

        /*
         * NOTE: This procedure can be used as example. The order of functions
         *       should be preserved, because this order allows all drivers to
         *       be initialized correctly because of hierarchy.
         */

        // 1. Mount base file system and create first folders.
        create_base_file_system_structure();

        // 2. Initialize basic drivers.
        initialize_basic_drivers();

        // 3. Create first output stream.
        open_output_stream();

        // 4. Show kernel panic message if occurred.
        show_kernel_panic_message();

        // 5. Initialize additional drivers
        initialize_additional_drivers();

        // 6. Initialize and mount of SD Card (optional)
        mount_SD_card();

        // 7. Print system log messages
        print_system_log_messages();

        // 8. Start DHCP client (optional)
        start_DHCP_client();

        // 9. Start user programs
        start_user_programs();

        // 10. If needed, the initd can continue work. It can be used as daemon.
        // while (true) {...}

        // 11. Or can be closed if not needed anymore.
        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/
