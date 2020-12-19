/*=========================================================================*//**
File     dmesg.c

Author   Daniel Zorychta

Brief    System log reader

         Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <dnx/misc.h>
#include <dnx/os.h>
#include <sys/ioctl.h>

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
};

/*==============================================================================
  Exported objects
==============================================================================*/
PROGRAM_PARAMS(dmesg, STACK_DEPTH_LOW);

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
        bool clear = false;
        bool loop  = false;

        for (int i = 1; i < argc; i++) {
            if (isstreq(argv[i], "-h") || isstreq(argv[i], "--help")) {
                    printf("Usage: %s [options]\n\n", argv[0]);
                    puts("Options:");
                    puts("  -c, --clear     log clear");
                    puts("  -h, --help      this help");
                    puts("  -l,             loop");
                    return EXIT_FAILURE;
            }

            if (isstreq(argv[i], "-c") || isstreq(argv[i], "--help")) {
                    clear = true;
            }

            if (isstreq(argv[i], "-l")) {
                    loop = true;
            }
        }

        if (clear) {
                syslog_clear();

        } else {
                struct timeval t = {0, 0};

                ioctl(fileno(stdin), IOCTL_VFS__NON_BLOCKING_RD_MODE);

                do {
                        char str[128];

                        while (syslog_read(str, sizeof(str), &t, &t)) {
                                printf("[%u.%06u] %s\n", t.tv_sec, t.tv_usec, str);
                        }

                        if (loop) {
                                int c = getchar();
                                if (c == ETX) {
                                        loop = false;
                                } else {
                                        msleep(100);
                                }
                        }
                } while (loop);

                ioctl(fileno(stdin), IOCTL_VFS__DEFAULT_RD_MODE);
        }

        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/

