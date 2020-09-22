/*=========================================================================*//**
@file    modprobe.c

@author  Daniel Zorychta

@brief   Module control.

@note    Copyright (C) 2018 Daniel Zorychta <daniel.zorychta@gmail.com>

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
PROGRAM_PARAMS(modprobe, STACK_DEPTH_LOW);

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
        printf("Usage: %s <flag> [OPTIONS]\n", name);
        puts("  -i            initialize module <module name> <major> <minor> [node path]");
        puts("  -r            release module <module name> <major> <minor>");
        puts("  -R            release module <module path>");
        puts("  -h, --help    this help");
}

//==============================================================================
/**
 * @brief Program main function
 */
//==============================================================================
int main(int argc, char *argv[])
{
        int err = EXIT_FAILURE;

        const char *action  = NULL;
        const char *modname = NULL;
        const char *path    = NULL;
        int         major   = 0;
        int         minor   = 0;


        if (argc < 2) {
                show_help(argv[0]);

        } else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
                show_help(argv[0]);

        } else if ((strcmp(argv[1], "-i") == 0) && (argc >= 5)) {

                action  = "initialize";
                modname = argv[2];
                major   = atoi(argv[3]);
                minor   = atoi(argv[4]);
                path    = argc >= 6 ? argv[5] : NULL;

                if (driver_init(modname, major, minor, path) >= 0) {
                        err = EXIT_SUCCESS;
                }

        } else if ((strcmp(argv[1], "-r") == 0) && (argc == 5)) {

                action  = "release";
                modname = argv[2];
                major   = atoi(argv[3]);
                minor   = atoi(argv[4]);

                if (driver_release(modname, major, minor) == 0) {
                        err = EXIT_SUCCESS;
                }

        } else if ((strcmp(argv[1], "-R") == 0) && (argc == 3)) {

                action = "release";
                path   = argv[2];

                if (driver_release2(path) == 0) {
                        err = EXIT_SUCCESS;
                }

        } else {
                show_help(argv[0]);
        }


        if (action) {
                if (!err) {
                        if (path) {
                                printf("Module '%s' %sd.\n", path, action);

                        } else {
                                printf("Module '%s%d-%d' %sd.\n",
                                       modname, major, minor, action);
                        }

                } else {
                        if (path) {
                                fprintf(stderr, "Module '%s' error: %s.\n",
                                        path, strerror(errno));

                        } else {
                                fprintf(stderr, "Module '%s%d-%d' error: %s.\n",
                                        modname, major, minor, strerror(errno));
                        }
                }
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
