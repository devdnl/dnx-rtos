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
#include "core/syscall.h"

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
        // TEST Helloworld variables
        int initial_stack;
        int after_syscall;
        int after_standard;
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
        // TEST Helloworld
//        puts("Hello world!");
//        printf("Free stack: %d\n", task_get_free_stack());
//        printf("Static memory usage: %d\n", get_used_static_memory());
//        printf("Memory size: %d\n", get_memory_size());
//        printf("Free memory: %d\n", get_free_memory());
//
//        printf("Program arguments:\n");
//        for (int i = 0; i < argc; i++) {
//                printf("%d: %s\n", i + 1, argv[i]);
//        }
//
//        int c;
//        do {
//                puts("Do you really want exit? [y/n]");
//                while (c = getchar(), !(c == 'y' || c == 'n'));
//        } while (c != 'y');
//
//        return 0;


        (void)argc;
        (void)argv;

        global->initial_stack = task_get_free_stack();

        FILE *f;
        _syscall(SYSCALL_FOPEN, &f, "/tmp/sca", "w");

        size_t n;
        _syscall(SYSCALL_FWRITE, &n, "syscall\n", 1, 8, f);
//        fwrite("syscall\n", 1, 8, f);

        global->after_syscall = task_get_free_stack();

        f = fopen("/tmp/std", "w");
        fwrite("stdcall\n", 1, 8, f);

        global->after_standard = task_get_free_stack();

        printf("Start stack:   %d\n"
               "After syscall: %d\n"
               "After oldcall: %d\n",
               global->initial_stack, global->after_syscall, global->after_standard);

        return 0;
}

/*==============================================================================
  End of file
==============================================================================*/
