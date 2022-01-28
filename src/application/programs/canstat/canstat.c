/*==============================================================================
File    canstat.c

Author  Daniel Zorychta

Brief   CAN info

        Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <errno.h>

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
        CAN_status_t status;
};

static const char *CAN_MODE[] = {"INIT", "NORMAL", "SLEEP"};
static const char *BUS_STATUS[] = {"OK", "WARNING", "PASSIVE", "OFF"};

/*==============================================================================
  Exported objects
==============================================================================*/
PROGRAM_PARAMS(canstat, STACK_DEPTH_LOW);

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
                printf("Usage: %s <can-interface-path>\n", argv[0]);

        } else {
                int fd = open(argv[1], O_RDONLY);
                if (fd != -1) {
                        errno = 0;
                        err = ioctl(fd, IOCTL_CAN__GET_STATUS, &global->status);
                        if (!err) {
                                printf("Speed      : %lu bps\n", global->status.baud_bps);
                                printf("Mode       : %s\n", CAN_MODE[global->status.mode]);
                                printf("Bus status : %s\n", BUS_STATUS[global->status.can_bus_status]);
                                printf("RX errors  : %u\n", global->status.rx_error_ctr);
                                printf("TX errors  : %u\n", global->status.tx_error_ctr);
                                printf("RX overruns: %lu\n", global->status.rx_overrun_ctr);
                                printf("TX frames  : %llu (%llu bytes)\n", global->status.tx_frames, global->status.tx_bytes);
                                printf("RX frames  : %llu (%llu bytes)\n", global->status.rx_frames, global->status.rx_bytes);
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

