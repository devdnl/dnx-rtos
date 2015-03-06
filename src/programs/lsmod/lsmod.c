/*=========================================================================*//**
@file    lsmod.c

@author  Daniel Zorychta

@brief   Module listing

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
 * @brief Program main function
 */
//==============================================================================
int_main(lsmod, STACK_DEPTH_LOW, int argc, char *argv[])
{
        (void)argc;
        (void)argv;

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

        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/
