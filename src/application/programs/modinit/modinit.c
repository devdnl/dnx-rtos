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

        int err = EXIT_SUCCESS;
        errno   = 0;

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

        const char *modname = NULL;
        const char *action  = "";
        int         major   = 0;
        int         minor   = 0;

        if (release) {
                action     = "release";
                major      = atoi(argv[3]);
                minor      = atoi(argv[4]);
                modname    = argv[2];

                if (driver_release(modname, major, minor) != 0) {
                        err = EXIT_FAILURE;
                }

        } else {
                action     = "initialize";
                major      = atoi(argv[2]);
                minor      = atoi(argv[3]);
                modname    = argv[1];

                if (driver_init(modname, major, minor, argv[4]) < 0) {
                        err = EXIT_FAILURE;
                }
        }

        if (!err) {
                printf("Module '%s%d-%d' %sd.\n", modname, major, minor, action);
        } else {
                fprintf(stderr, "Module '%s%d-%d' error: %s.\n",
                        modname, major, minor, strerror(errno));
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
