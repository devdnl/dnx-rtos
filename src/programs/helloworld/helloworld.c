/*=========================================================================*//**
@file    helloworld.c

@author  Daniel Zorychta

@brief   The simple example program

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
#include <string.h>
#include <dnx/os.h>
#include <dnx/thread.h>

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
        /* put here global variables */
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
 *
 * @param  argc         count of arguments
 * @param *argv[]       argument table
 *
 * @return program status
 */
//==============================================================================
int_main(helloworld, STACK_DEPTH_VERY_LOW, int argc, char *argv[])
{
        puts("Hello world!");
        printf("Free stack: %d\n", task_get_free_stack());
        printf("Static memory usage: %d\n", get_used_static_memory());
        printf("Memory size: %d\n", get_memory_size());
        printf("Free memory: %d\n", get_free_memory());

        printf("Program arguments:\n");
        for (int i = 0; i < argc; i++) {
                printf("%d: %s\n", i + 1, argv[i]);
        }

        int c;
        do {
                puts("Do you really want exit? [y/n]");
                while (c = getchar(), !(c == 'y' || c == 'n'));
        } while (c != 'y');

        return 0;
}

/*==============================================================================
  End of file
==============================================================================*/
