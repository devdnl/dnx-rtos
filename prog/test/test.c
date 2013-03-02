/*=========================================================================*//**
@file    test.c

@author  Daniel Zorychta

@brief

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "test.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void f1(void);
static void f2(void);

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES {
        int test;
};

/*==============================================================================
  Exported object definitions
==============================================================================*/
PROGRAM_PARAMS(test, 3*MINIMAL_STACK_SIZE);

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief
 */
//==============================================================================
int PROGRAM_MAIN(test)(int argc, ch_t *argv[])
{
        printf("\n---------------------\n");
        printf("Free stack: %d\n", get_free_stack());
        printf("Static memory usage: %d\n", get_used_static_memory());
        printf("Memory size: %d\n", get_memory_size());
        printf("Free memory: %d\n", get_free_memory());

        printf("Program arguments:\n");
        for (int i = 0; i < argc; i++) {
                printf("%d: %s\n", i + 1, argv[i]);
        }

        global->test = 0;
        printf("main\n");
        printf("global->test = %d\n", global->test);

        printf("Free stack: %d\n", get_free_stack());

        f1();
        printf("global->test = %d\n", global->test);
        if (global->test != 1) {
                printf(FONT_COLOR_RED"global->test != 1!"RESET_ATTRIBUTES"\n");
                sleep(5);
        }

        f2();
        printf("global->test = %d\n", global->test);
        if (global->test != -1) {
                printf(FONT_COLOR_RED"global->test != -1!"RESET_ATTRIBUTES"\n");
                sleep(5);
        }

        printf("exiting...\n");

        return 0;
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
static void f1(void)
{
//        sleep(1);
        printf("f1\n");
        global->test = 1;
}

//==============================================================================
/**
 * @brief
 */
//==============================================================================
static void f2(void)
{
//        sleep(1);
        printf("f2: global->test = %d\n", global->test);
        global->test = -global->test;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
