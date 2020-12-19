/*=========================================================================*//**
@file    cp.c

@author  Daniel Zorychta

@brief   Program to copy files

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
#include <time.h>
#include <errno.h>
#include <dnx/os.h>
#include <sys/stat.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define BUFFER_MAX_SIZE                 8192
#define INFO_REFRESH_TIME_MS            (CLOCKS_PER_SEC * 1)
#define PATH_MAX_SIZE                   128

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
};

/*==============================================================================
  Exported object definitions
==============================================================================*/
PROGRAM_PARAMS(cp, STACK_DEPTH_MEDIUM);

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Cat main function
 */
//==============================================================================
int main(int argc, char *argv[])
{
        if (argc != 3) {
                printf("Usage: %s <source file> <destination file>\n", argv[0]);
                return EXIT_FAILURE;
        }

        errno = 0;

        int   err      = EXIT_SUCCESS;
        char *buffer   = NULL;
        FILE *src_file = NULL;
        FILE *dst_file = NULL;

        src_file = fopen(argv[1], "r");
        if (!src_file) {
                perror(argv[1]);
                err = EXIT_FAILURE;
                goto exit;
        }

        dst_file = fopen(argv[2], "w");
        if (!dst_file) {
                perror(argv[2]);
                err = EXIT_FAILURE;
                goto exit;
        }

        int buffer_size = BUFFER_MAX_SIZE;
        while ((buffer = malloc(buffer_size * sizeof(char))) == NULL) {
                buffer_size /= 2;

                if (buffer_size < 512) {
                        perror(NULL);
                        err = EXIT_FAILURE;
                        goto exit;
                }
        }

        int n;
        while (buffer && (n = fread(buffer, sizeof(char), buffer_size, src_file)) > 0) {
                if (ferror(src_file)) {
                        perror(argv[1]);
                        break;
                }

                fwrite(buffer, sizeof(char), n, dst_file);
                if (ferror(dst_file)) {
                        perror(argv[2]);
                        break;
                }
        }

exit:
        if (buffer) {
                free(buffer);
        }

        if (src_file) {
                fclose(src_file);
        }

        if (dst_file) {
                fclose(dst_file);
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
