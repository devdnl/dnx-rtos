/*=========================================================================*//**
@file    modinit.c

@author  Daniel Zorychta

@brief   Module control.

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
#include <stdbool.h>
#include <errno.h>
#include <sys/mount.h>
#include <dnx/os.h>

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
};

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief Print help
 */
//==============================================================================
static void show_help(const char *name)
{
        printf("Usage: %s [OPTIONS] <module name> <major> <minor> [module node]\n", name);
        puts("  -r            release module");
        puts("  -h, --help    this help");
}

//==============================================================================
/**
 * @brief Program main function
 */
//==============================================================================
int_main(modinit, STACK_DEPTH_LOW, int argc, char *argv[])
{
        if (argc < 4) {
                show_help(argv[0]);
                return 0;
        }

        errno = 0;

        bool release = false;
        for (int i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-r") == 0) {
                        release = true;
                }

                if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                        show_help(argv[0]);
                        return 0;
                }
        }

        int status;
        if (release) {
                int major = atoi(argv[3]);
                int minor = atoi(argv[4]);
                status = driver_release(argv[2], major, minor) ? -1 : 0;
        } else {
                int major = atoi(argv[2]);
                int minor = atoi(argv[3]);

                if (argc == 4) {
                        status = driver_init(argv[1], major, minor, NULL);
                } else {
                        status = driver_init(argv[1], major, minor, argv[4]);
                }
        }

        if (status >= 0) {
                puts("Success.");
        } else {
                perror("Error");
        }

        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/
