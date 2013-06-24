/*=========================================================================*//**
@file    cp.c

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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cp.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define BUFFER_MAX_SIZE                 32768
#define INFO_REFRESH_TIME_MS            1000
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
GLOBAL_VARIABLES {
};

/*==============================================================================
  Exported object definitions
==============================================================================*/
PROGRAM_PARAMS(cp, STACK_DEPTH_LOW);

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Cat main function
 */
//==============================================================================
int PROGRAM_MAIN(cp, int argc, char *argv[])
{
        if (argc != 3) {
                printf("Usage: %s <source file> <destination file>\n", argv[0]);
                return EXIT_FAILURE;
        }

        char *src_path = NULL;
        char *dst_path = NULL;
        char *buffer   = NULL;
        FILE *src_file = NULL;
        FILE *dst_file = NULL;

        src_path = calloc(PATH_MAX_SIZE, sizeof(char));
        if (!src_path) {
                printf("Not enough free memory\n");
                goto exit_error;
        }

        if (argv[1][0] != '/') {
                getcwd(src_path, PATH_MAX_SIZE / 2);
                strcat(src_path, "/");
        }
        strcat(src_path, argv[1]);


        dst_path = calloc(PATH_MAX_SIZE, sizeof(char));
        if (!dst_path) {
                printf("Not enough free memory\n");
                goto exit_error;
        }

        if (argv[2][0] != '/') {
                getcwd(dst_path, PATH_MAX_SIZE / 2);
                strcat(dst_path, "/");
        }
        strcat(dst_path, argv[2]);

        int buffer_size = BUFFER_MAX_SIZE;
        while ((buffer = malloc(buffer_size * sizeof(char))) == NULL) {
                buffer_size /= 2;

                if (buffer_size < 512) {
                        printf("Not enough free memory\n");
                        goto exit_error;
                }
        }

        src_file = fopen(src_path, "r");
        if (!src_file) {
                printf("Cannot open file %s\n", src_path);
                goto exit_error;
        }

        dst_file = fopen(dst_path, "w");
        if (!dst_file) {
                printf("Cannot create file %s\n", dst_path);
                goto exit_error;
        }

        fseek(src_file, 0, SEEK_END);
        uint file_size = ftell(src_file);
        fseek(src_file, 0, SEEK_SET);

        uint start_time   = get_tick_counter();
        uint refresh_time = start_time;
        uint copy_size    = 0;
        int  n;

        while ((n = fread(buffer, sizeof(char), buffer_size, src_file))) {
                copy_size += n;

                if (get_tick_counter() - refresh_time >= INFO_REFRESH_TIME_MS) {
                        refresh_time = get_tick_counter();
                        printf("\r%d.%2d%% copied...",
                               ((copy_size*100)/file_size),
                               ((copy_size*10000)/file_size) % 100);
                }

                if (fwrite(buffer, sizeof(char), n, dst_file) == 0) {
                        printf("\rCoping error\n");
                        break;
                }
        }

        uint stop_time = get_tick_counter() - start_time;
        printf("\rCopied %d bytes in %d.%3d seconds (%d.%3d KiB/s)\n",
               copy_size,
               stop_time / 1000,
               stop_time % 1000,
               ((copy_size / stop_time) * 1000) / 1024,
               ((copy_size / stop_time) * 1000) % 1024);

        return EXIT_SUCCESS;

exit_error:
        if (src_path) {
                free(src_path);
        }

        if (dst_path) {
                free(dst_path);
        }

        if (buffer) {
                free(buffer);
        }

        if (src_file) {
                fclose(src_file);
        }

        if (dst_file) {
                fclose(dst_file);
        }

        return EXIT_FAILURE;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
