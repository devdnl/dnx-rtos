/*==============================================================================
File     chmod.c

Author   Daniel Zorychta

Brief    Modify file mode.

         Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <ctype.h>

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
};

/*==============================================================================
  Exported objects
==============================================================================*/
PROGRAM_PARAMS(chmod, STACK_DEPTH_VERY_LOW);

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief  Print help message.
 *
 * @param  name         this program name
 */
//==============================================================================
static void print_help(const char *name)
{
        printf("Usage: %s MODE FILE\n\n", name);
        printf("Mode:\n");
        printf("   [+|-][x|r|w]  flag selection (only USR)\n");
        printf("   value         mode value\n");
}

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
        int err = EXIT_FAILURE;

        if (argc < 3) {
                print_help(argv[0]);
        } else {
                struct stat st;
                if (stat(argv[2], &st) == 0) {

                        bool set   = argv[1][0] == '+';
                        bool unset = argv[1][0] == '-';

                        if (set || unset) {
                                mode_t flag = 0;

                                switch (argv[1][1]) {
                                case 'r': flag = S_IRUSR; break;
                                case 'w': flag = S_IWUSR; break;
                                case 'x': flag = S_IXUSR; break;
                                }

                                st.st_mode |=  (set   ? flag : 0);
                                st.st_mode &= ~(unset ? flag : 0);
                        } else {
                                if (isdigit(argv[1][0])) {
                                        char *end;
                                        st.st_mode = strtol(argv[1], &end, 0);
                                } else {
                                        print_help(argv[0]);
                                        return EXIT_FAILURE;
                                }
                        }

                        if (chmod(argv[2], st.st_mode) == 0) {
                                err = EXIT_SUCCESS;
                        } else {
                                perror(argv[2]);
                        }

                } else {
                        perror(argv[2]);
                }
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
