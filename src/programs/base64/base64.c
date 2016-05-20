/*=========================================================================*//**
File     base64.c

Author   Daniel Zorychta

Brief    Base64 code and encode program.

         Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <stdbool.h>
#include <errno.h>
#include <dnx/misc.h>
#include <lib/base64/base64.h>

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
        uint8_t buf[6];
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
        FILE *file = stdin;
        if (infile != NULL) {
                file = fopen(infile, "r");
                if (!file) {
                        perror(infile);
                        return EXIT_FAILURE;
                }
        }

        // do job
        int    n   = 0;
        size_t col = 0;
        while (feof(file) == 0) {
                if (decode) {
                        n = 0;
                        do {
                                int c = fgetc(file);
                                if (c != EOF) {
                                        if ((c != '\r') && (c != '\n')) {
                                                global->buf[n++] = c;
                                        }
                                } else {
                                        break;
                                }
                        } while (n < 4);

                } else {
                        n = fread(global->buf, 1, 6, file);
                }

                if (n > 0) {
                        size_t  len    = 0;
                        void   *result = NULL;

                        if (decode) {
                                result = base64_decode((char *)global->buf, n, &len);
                        } else {
                                result = base64_encode(global->buf, n, &len);
                        }

                        if (result) {
                                fwrite(result, 1, len, stdout);
                                free(result);

                                if (!decode) {
                                        if (++col >= 72/8) {
                                                col = 0;
                                                fputs("\n", stdout);
                                        }
                                }
                        } else {
                                break;
                        }
                } else {
                        break;
                }
        }

        if (!decode && col != 0) {
                puts("");
        }

        // free resources
        if (file && file != stdin) {
                fclose(file);
        }

        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/
