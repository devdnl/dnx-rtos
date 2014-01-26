/*=========================================================================*//**
@file    modinit.c

@author  Daniel Zorychta

@brief   Module control.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <sys/mount.h>

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
GLOBAL_VARIABLES_SECTION_BEGIN

GLOBAL_VARIABLES_SECTION_END

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
        printf("Usage: %s [OPTIONS] <module name> [module node]\n", name);
        puts("  -r            release module");
        puts("  -h, --help    this help");
}

//==============================================================================
/**
 * @brief Program main function
 */
//==============================================================================
PROGRAM_MAIN(modinit, int argc, char *argv[])
{
        if (argc < 2) {
                show_help(argv[0]);
                return 0;
        }

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
                status = driver_release(argv[2]);
        } else {
                if (argc == 2) {
                        status = driver_init(argv[1], NULL);
                } else {
                        status = driver_init(argv[1], argv[2]);
                }
        }

        if (status == 0) {
                puts("Success.");
        } else {
                puts("Failure.");
        }

        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/
