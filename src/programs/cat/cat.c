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
GLOBAL_VARIABLES_SECTION_BEGIN
bool rm_ctrl_chars;
GLOBAL_VARIABLES_SECTION_END

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Function remove control characters
 *
 * @param str           string to modify
 */
//==============================================================================
static void remove_control_characters(char *str)
{
        for (uint i = 0; i < strlen(str); i++) {
                if (str[i] < ' ' && str[i] != '\n') {
                        str[i] = 0xFF;
                }
        }
}

//==============================================================================
/**
 * @brief Read file
 *
 * @param file          file to read
 * @param str           line buffer
 * @param len           line buffer legth
 *
 */
//==============================================================================
static void print_file(FILE *file, char *str, int len)
{
        int eof = 0;
        while (!eof && fgets(str, len, file)) {
                eof = feof(file);

                if (global->rm_ctrl_chars) {
                        remove_control_characters(str);
                }

                if (LAST_CHARACTER(str) != '\n') {
                        strcat(str, "\n");
                }

                fputs(str, stdout);
        }
}

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
        puts("  -n              remove control characters");
        puts("  -h, --help      show this help");
}

//==============================================================================
/**
 * @brief Cat main function
 */
//==============================================================================
PROGRAM_MAIN(cat, int argc, char *argv[])
{
        int status = EXIT_SUCCESS;

        int i = 1;
        for (; i < argc; i++) {
                if (strcmp(argv[i], "-n") == 0) {
                        global->rm_ctrl_chars = true;
                        continue;
                }

                if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                        print_help(argv[0]);
                        return 0;
                }

                break;
        }

        errno = 0;

        u32_t col = 80;
        ioctl(stdout, TTY_IORQ_GET_COL, &col);

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

                        print_file(file, str, col);

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
