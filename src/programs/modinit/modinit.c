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
        puts("  -l            list of modules");
        puts("  -h, --help    this help");
}

//==============================================================================
/**
 * @brief Print list of modules
 */
//==============================================================================
static void show_list_of_modules()
{
        puts(FONT_BOLD"Driver"CURSOR_BACKWARD(99)CURSOR_FORWARD(16)"Module"
             CURSOR_BACKWARD(99)CURSOR_FORWARD(32)"MID"
             CURSOR_BACKWARD(99)CURSOR_FORWARD(37)"DID"
             CURSOR_BACKWARD(99)CURSOR_FORWARD(42)"Active"
             RESET_ATTRIBUTES);

        int drv_number = get_number_of_drivers();
        for (int i = 0; i < drv_number; i++) {
                const char *mod_name  = get_driver_module_name(i);
                const char *drv_name  = get_driver_name(i);
                int         mod_id    = get_module_number(mod_name);
                bool        is_active = is_driver_active(i);

                printf("%s"CURSOR_BACKWARD(99)CURSOR_FORWARD(16)"%s"
                           CURSOR_BACKWARD(99)CURSOR_FORWARD(32)"%i"
                           CURSOR_BACKWARD(99)CURSOR_FORWARD(37)"%u"
                           CURSOR_BACKWARD(99)CURSOR_FORWARD(42)"%c\n",
                       drv_name, mod_name, mod_id, i, is_active ? '*': ' ');
        }
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

                if (strcmp(argv[i], "-l") == 0) {
                        show_list_of_modules();
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
