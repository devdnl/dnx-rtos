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
#include <dnx/vt100.h>

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

        puts(VT100_FONT_BOLD"ID"VT100_CURSOR_BACKWARD(99)
             VT100_CURSOR_FORWARD(5)"Name"VT100_CURSOR_BACKWARD(99)
             VT100_CURSOR_FORWARD(16)"Instances"VT100_RESET_ATTRIBUTES);

        ssize_t drv_number = get_number_of_modules();
        for (ssize_t i = 0; i > -1 && i < drv_number; i++) {

                printf("%d"VT100_CURSOR_BACKWARD(99)
                       VT100_CURSOR_FORWARD(5)"%s"VT100_CURSOR_BACKWARD(99)
                       VT100_CURSOR_FORWARD(16)"%d",
                       i, get_module_name(i), get_number_of_module_instances(i));
        }

        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/
