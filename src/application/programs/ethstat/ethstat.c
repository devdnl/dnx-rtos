/*==============================================================================
File    ethstat.c

Author  Daniel Zorychta

Brief   Ethernet info

        Copyright (C) 2020 Daniel Zorychta <daniel.zorychta@gmail.com>

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

==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <sys/unistd.h>

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
GLOBAL_VARIABLES_SECTION {
        ETH_status_t status;
};

static const char *ETH_STATE[]  = {"RESET", "READY", "ERROR"};
static const char *ETH_LINK[]   = {"DISCONNECTED", "CONNECTED", "PHY ERROR"};
static const char *ETH_SPEED[]  = {"10", "100"};
static const char *ETH_DUPLEX[] = {"HALF", "FULL"};

/*==============================================================================
  Exported objects
==============================================================================*/
PROGRAM_PARAMS(ethstat, STACK_DEPTH_LOW);

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * Main program function.
 *
 * Note: Please adjust stack size according to programs needs.
 *
 * @param argc      argument count
 * @param argv      arguments
 */
//==============================================================================
int main(int argc, char *argv[])
{
        int err = EXIT_FAILURE;

        if (argc == 1) {
                printf("Usage: %s <ethernet-interface-path>\n", argv[0]);

        } else {
                fd_t fd = open(argv[1], O_RDONLY);
                if (fd != -1) {
                        err = ioctl(fd, IOCTL_ETH__GET_STATUS, &global->status);
                        if (!err) {
                                printf("Configured: %s\n", global->status.configured ? "YES" : "NO");
                                printf("State     : %16s\n", ETH_STATE[global->status.state]);
                                printf("Link      : %16s\n", ETH_LINK[global->status.link_status]);
                                printf("Duplex    : %16s\n", ETH_DUPLEX[global->status.duplex]);
                                printf("Speed     : %16s Mbps\n", ETH_SPEED[global->status.speed]);
                                printf("MAC       : %02x:%02x:%02x:%02x:%02x:%02x\n",
                                       global->status.MAC[0], global->status.MAC[1],
                                       global->status.MAC[2], global->status.MAC[3],
                                       global->status.MAC[4], global->status.MAC[5]);
                                printf("RX packets: %llu, %u dropped\n",
                                       global->status.rx_packets, global->status.rx_dropped_frames);
                                printf("RX bytes  : %llu\n", global->status.rx_bytes);
                                printf("TX packets: %llu\n", global->status.tx_packets);
                                printf("TX bytes  : %llu\n", global->status.tx_bytes);

                                err = EXIT_SUCCESS;

                        } else {
                                perror(argv[1]);
                        }

                        close(fd);

                } else {
                        perror(argv[1]);
                }
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/

