/*=========================================================================*//**
File     base64.c

Author   Daniel Zorychta

Brief    Base64 code and encode program.

         Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <stdbool.h>
#include <errno.h>
#include <dnx/misc.h>
#include <base64.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define WRAP 76

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
        FILE   *file;

        union {
                char    decode[240];    // must be multiple of 4
                uint8_t encode[240];    // must be multiple of 6
        } buf;
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
        puts("  -d, --decode    decode data");
        puts("  -h, --help      show this help\n");
        puts("If file is not set then stdin is read.");
}

//==============================================================================
/**
 * @brief Function converts string coded in base64 to binary.
 */
//==============================================================================
static void do_decode(void)
{
        while (feof(global->file) == 0) {
                memset(global->buf.decode, 0, sizeof(global->buf.decode));
                if (fgets(global->buf.decode, sizeof(global->buf.decode) - 3, global->file)) {
                        size_t buf_len = 0;
                        uint8_t *buf = base64_decode(global->buf.decode,
                                                     strlen(global->buf.decode),
                                                     &buf_len);
                        if (!buf) {
                                return;
                        }

                        if (fwrite(buf, 1, buf_len, stdout) != buf_len) {
                                free(buf);
                                perror(NULL);
                                return;
                        }

                        free(buf);
                }
        }
}

//==============================================================================
/**
 * @brief Function converts binary buffer to string coded in base64.
 */
//==============================================================================
static void do_encode(void)
{
        size_t col = 0;

        while (feof(global->file) == 0) {
                int n = fread(global->buf.encode, 1, sizeof(global->buf.encode),
                              global->file);

                if (  (global->file == stdin)
                   && (global->buf.encode[n - 1] == 0)
                   && (global->buf.encode[n - 2] == '\n') ) {

                        n--;
                }

                if (n) {
                        size_t buf_len = 0;
                        char *buf = base64_encode(global->buf.encode, n, &buf_len);
                        if (!buf) {
                                return;
                        }

                        char *ptr = buf;
                        while (buf_len > 0) {
                                size_t sz = min(WRAP - col, buf_len);

                                if ((col + sz) < WRAP) {
                                        printf(ptr);
                                        col += sz;
                                } else {
                                        printf("%.*s\n", sz, ptr);
                                        col = 0;
                                }

                                ptr     += sz;
                                buf_len -= sz;
                        }

                        free(buf);
                }
        }

        if (col != 0) {
                puts("");
        }
}

//==============================================================================
/**
 * @brief Program main function
 */
//==============================================================================
int_main(base64, STACK_DEPTH_LOW, int argc, char *argv[])
{
        bool        decode = false;
        const char *infile = NULL;

        // check arguments
        for (int i = 1; i < argc; i++) {
                if (  isstreq("-h", argv[i])
                   || isstreq("--help", argv[i]) ) {

                        print_help(argv[0]);
                        return EXIT_SUCCESS;

                } else if (  isstreq("-d", argv[i])
                          || isstreq("--decode", argv[i]) ) {

                        decode = true;

                } else {
                        infile = argv[i];
                }
        }

        // open input file
        global->file = stdin;
        if (infile != NULL) {
                global->file = fopen(infile, "r");
                if (!global->file) {
                        perror(infile);
                        return EXIT_FAILURE;
                }
        }

        // do job
        if (decode) {
                do_decode();
        } else {
                do_encode();
        }

        // free resources
        if (global->file && (global->file != stdin)) {
                fclose(global->file);
        }

        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/
