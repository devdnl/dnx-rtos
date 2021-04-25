/*==============================================================================
File    grep.c

Author  Daniel Zorychta

Brief   grep.

        Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

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

==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <string.h>
#include <dnx/vt100.h>
#include <stdlib.h>

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
GLOBAL_VARIABLES_SECTION {
        char str[512];
};

/*==============================================================================
  Exported objects
==============================================================================*/
PROGRAM_PARAMS(grep, STACK_DEPTH_LOW);

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * Main program function.
 *
 * Note: Please adjust stack size according to programs needs.
 *
 * @param argc      argument count
 * @param argv      arguments
 */
//==============================================================================
int main(int argc, char *argv[])
{
        if (argc == 1) {
                printf("Usage: %s PATTERN [FILE]\n", argv[0]);
                // no return, input stream need to be empty
        }

        FILE *input = (argc == 3) ? fopen(argv[2], "r") : stdin;

        while (fgets(global->str, sizeof(global->str), input)) {

                const char *point = strstr(global->str, argv[1]);
                if (point) {
                        size_t begin_len  = point - global->str;
                        size_t middle_len = strlen(argv[1]);

                        printf("%.*s"VT100_FONT_BOLD VT100_FONT_COLOR_RED
                               "%.*s"VT100_RESET_ATTRIBUTES"%s",
                               begin_len, global->str, middle_len,
                               point, point + middle_len);
                }
        }

        if (input != stdin) fclose(input);

        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/

