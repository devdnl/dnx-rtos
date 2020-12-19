/*==============================================================================
File    more.c

Author  Daniel Zorychta

Brief   Show more data.

        Copyright (C) 2018 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <unistd.h>

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
        char line[256];
};

/*==============================================================================
  Exported objects
==============================================================================*/
PROGRAM_PARAMS(more, STACK_DEPTH_LOW);

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
 * @param argc      argument count
 * @param argv      arguments
 */
//==============================================================================
int main(int argc, char *argv[])
{
        FILE *file;

        if ((argc == 2) && argv[1]) {
                file = fopen(argv[1], "r+");

                if (!file) {
                        perror(argv[1]);
                        return EXIT_FAILURE;
                }

        } else {
                file = stdin;
        }

        ioctl(fileno(stdout), IOCTL_VFS__NON_BLOCKING_RD_MODE);
        ioctl(fileno(stdout), IOCTL_TTY__ECHO_OFF);

        int ROWS = 23;
        ioctl(fileno(stdout), IOCTL_TTY__GET_ROW, &ROWS);
        ROWS -= 1;

        int show = ROWS;

        while (true) {
                for (int i = 0; i < show; i++) {
                        if (fgets(global->line, sizeof(global->line), file)) {
                                printf("%s", global->line);
                        } else {
                                goto exit;
                        }
                }

                while (true) {
                        int c = getc(stdout);

                        if (c == '\n') {
                                show = 1;
                                break;

                        } else if (c == ' ') {
                                show = ROWS;
                                break;
                        }

                        msleep(20);
                }
        }

        exit:
        ioctl(fileno(stdout), IOCTL_TTY__ECHO_ON);
        ioctl(fileno(stdout), IOCTL_VFS__DEFAULT_RD_MODE);

        if (file != stdin) {
                fclose(file);
        }

        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/

