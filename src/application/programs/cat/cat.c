/*=========================================================================*//**
@file    cat.c

@author  Daniel Zorychta

@brief   Concatenate files or show file content in terminal

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
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <dnx/misc.h>
#include <ctype.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void read_stdin(void);
static void read_file(const char *filename);
static char *strlf(char *str, size_t slen, size_t *len);
static void convert_to_printable(char *buf, size_t buflen);

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES_SECTION {
        int    columns;
        char   buffer[512];
        bool   printable_only;
        bool   number_lines;
        int    args;
        size_t line;
};

/*==============================================================================
  Exported object definitions
==============================================================================*/
PROGRAM_PARAMS(cat, STACK_DEPTH_LOW);

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
        puts("  -p,             show only printable characters");
        puts("  -n,             show line numbers");
        puts("  -h, --help      show this help");
}

//==============================================================================
/**
 * @brief Cat main function
 *
 * @param  argc         argument count
 * @param  argv         argument values
 *
 * @return 0 on success.
 */
//==============================================================================
int main(int argc, char *argv[])
{
        int status = EXIT_SUCCESS;

        global->args = 1;
        global->columns = 80;
        global->line = 1;

        for (int i = 0; i < argc - 1; i++) {
                if (strcmp(argv[i], "-p") == 0) {
                        global->printable_only = true;
                        global->args++;

                } else if (strcmp(argv[i], "-n") == 0) {
                        global->number_lines = true;
                        global->args++;

                } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                        print_help(argv[0]);
                        return EXIT_FAILURE;
                }
        }

        errno = 0;

        ioctl(fileno(stdout), IOCTL_TTY__GET_COL, &global->columns);

        if (argc == global->args) {
                read_stdin();

        } else {
                for (int i = global->args; i < argc; i++) {
                        read_file(argv[i]);
                }
        }

        return status;
}

//==============================================================================
/**
 * @brief  Function read stdin.
 */
//==============================================================================
static void read_stdin(void)
{
        char   *str = cast(char*, global->buffer);
        int     len = sizeof(global->buffer);
        int     eof = 0;
        size_t line = 1;

        while (!eof && fgets(str, len, stdin)) {
                eof = feof(stdin);

                if (global->number_lines) {
                        fprintf(stdout, "%6u  %s", line, str);
                        line++;
                } else {
                        fputs(str, stdout);
                }
        }
}

//==============================================================================
/**
 * @brief  Function read regular file.
 *
 * @param  filename     file name
 */
//==============================================================================
static void read_file(const char *filename)
{
        errno = 0;
        FILE *file = fopen(filename, "r");

        while (file) {
                char *str = global->buffer;

                size_t n = fread(global->buffer, 1, sizeof(global->buffer), file);
                if (n == 0) {
                        break;

                } else {
                        if (global->printable_only) {
                                convert_to_printable(global->buffer, sizeof(global->buffer));
                        }

                        while (n > 0) {
                                size_t len = n;
                                char *lf = strlf(str, n, &len);
                                if (lf) {
                                        if (global->number_lines) {
                                                fprintf(stdout, "%6u  %.*s", global->line, len, str);
                                                global->line++;
                                        } else {
                                                fprintf(stdout, "%.*s", len, str);
                                        }

                                        str = lf + 1;
                                } else {
                                        fwrite(str, 1, len, stdout);
                                }

                                n -= len;
                        }
                }
        }

        if (file) {
                fclose(file);
        } else {
                perror(filename);
        }
}

//==============================================================================
/**
 * @brief  Function localize \n in buffer and return substring size.
 *
 * @param  str          source string
 * @param  slen         string length
 * @param  len          substring length (number of characters from str to \n)
 *
 * @return Pointer to \n or NULL if not found.
 */
//==============================================================================
static char *strlf(char *str, size_t slen, size_t *len)
{
        size_t n = 0;

        while (slen--) {
                n++;

                if (*str == '\n') {
                        *len = n;
                        return str;
                } else {
                        str++;
                }
        }

        return NULL;
}

//==============================================================================
/**
 * @brief  Function convert non printing values to '.'
 *
 * @param  buf          buffer
 * @param  buflen       buffer length
 */
//==============================================================================
static void convert_to_printable(char *buf, size_t buflen)
{
        while (buflen--) {
                if (not ((*buf == '\n') or isprint(*buf))) {
                        *buf = '.';
                }
                buf++;
        }
}

/*==============================================================================
  End of file
==============================================================================*/
