/*=========================================================================*//**
@file    cat.c

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

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <dnx/misc.h>

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
GLOBAL_VARIABLES {
        u8_t buffer[512];
};

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Function show help screen
 *
 * @param name          program name
 */
//==============================================================================
static void print_help(char *name)
{
        printf("Usage: %s [OPTION] [FILE]\n", name);
        puts("  -h, --help      show this help");
}

//==============================================================================
/**
 * @brief Cat main function
 */
//==============================================================================
int_main(cat, STACK_DEPTH_LOW, int argc, char *argv[])
{
        int status = EXIT_SUCCESS;

        int i = 1;
        for (; i < argc; i++) {
                if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                        print_help(argv[0]);
                        return 0;
                }

                break;
        }

        errno = 0;

        u32_t col = 80;
        ioctl(stdout, IOCTL_TTY__GET_COL, &col);

        char *str = calloc(col + 1, sizeof(char));
        if (str) {
                FILE *file;

                do {
                        if (argc == i) {
                                file = stdin;
                        } else {
                                file = fopen(argv[i], "r");
                                if (!file) {
                                        perror(argv[i]);
                                        break;
                                }
                        }

                        int n;
                        do {
                                n = fread(global->buffer, 1, sizeof(global->buffer), file);
                                fwrite(global->buffer, 1, n, stdout);
                        } while (n == sizeof(global->buffer));

                        if (file != stdin) {
                                fclose(file);
                        }

                } while (++i < argc);

                free(str);
        } else {
                perror(NULL);
                status = EXIT_FAILURE;
        }

        return status;
}

/*==============================================================================
  End of file
==============================================================================*/
