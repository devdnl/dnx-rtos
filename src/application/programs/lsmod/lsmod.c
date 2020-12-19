/*=========================================================================*//**
@file    lsmod.c

@author  Daniel Zorychta

@brief   Module listing

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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
PROGRAM_PARAMS(lsmod, STACK_DEPTH_LOW);

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Program main function
 */
//==============================================================================
int main(int argc, char *argv[])
{
        (void)argc;
        (void)argv;

        puts(VT100_FONT_BOLD"ID"VT100_CURSOR_BACKWARD(99)
             VT100_CURSOR_FORWARD(4)"Name"VT100_CURSOR_BACKWARD(99)
             VT100_CURSOR_FORWARD(20)"Instances"VT100_RESET_ATTRIBUTES);

        ssize_t drv_number = get_number_of_modules();
        for (ssize_t i = 0; i > -1 && i < drv_number; i++) {

                printf("%d"VT100_CURSOR_BACKWARD(99)
                       VT100_CURSOR_FORWARD(4)"%s"VT100_CURSOR_BACKWARD(99)
                       VT100_CURSOR_FORWARD(20)"%d\n",
                       i, get_module_name(i), get_number_of_module_instances(i));
        }

        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/
