/*=========================================================================*//**
@file    cp.c

@author  Daniel Zorychta

@brief   Program to copy files

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <time.h>
#include <errno.h>
#include <dnx/os.h>
#include <sys/stat.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define BUFFER_MAX_SIZE                 16384
#define INFO_REFRESH_TIME_MS            (1 * CLOCKS_PER_SEC)
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

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Cat main function
 */
//==============================================================================
int_main(cp, STACK_DEPTH_MEDIUM, int argc, char *argv[])
{
        if (argc != 3) {
                printf("Usage: %s <source file> <destination file>\n", argv[0]);
                return EXIT_FAILURE;
        }

        errno = 0;

        char *buffer   = NULL;
        FILE *src_file = NULL;
        FILE *dst_file = NULL;

        src_file = fopen(argv[1], "r");
        if (!src_file) {
                perror(argv[1]);
                goto exit_error;
        }

        dst_file = fopen(argv[2], "w");
        if (!dst_file) {
                perror(argv[2]);
                goto exit_error;
        }

        int buffer_size = BUFFER_MAX_SIZE;
        while ((buffer = malloc(buffer_size * sizeof(char))) == NULL) {
                buffer_size /= 2;

                if (buffer_size < 512) {
                        perror(NULL);
                        goto exit_error;
                }
        }

        fseek(src_file, 0, SEEK_END);
        u64_t lfile_size = ftell(src_file);
        fseek(src_file, 0, SEEK_SET);

        time_t start_time = time(NULL);
        time_t info_timer = time(NULL) + INFO_REFRESH_TIME_MS + 1;
        u64_t lcopy_size = 0;
        int   n;

        while ((n = fread(buffer, sizeof(char), buffer_size, src_file)) > 0) {
                if (ferror(src_file)) {
                        perror(argv[1]);
                        break;
                }

                if (difftime(time(NULL), info_timer) >= INFO_REFRESH_TIME_MS) {
                        info_timer = time(NULL);

                        u32_t file_size = lfile_size / 1024;
                        u32_t copy_size = lcopy_size / 1024;

                        printf("\r%d.%2d%% copied...",
                               ((copy_size*100)/file_size),
                               ((copy_size*10000)/file_size) % 100);

                        fflush(stdout);
                }

                fwrite(buffer, sizeof(char), n, dst_file);
                if (ferror(dst_file)) {
                        puts("");
                        perror(argv[2]);
                        break;
                }

                lcopy_size += n;
        }

        time_t stop_time = time(NULL) - start_time;
        u32_t  copy_size = lcopy_size;
        struct stat stat;
        stat.st_size = 0;
        fstat(src_file, &stat);

        if (ferror(src_file) && stat.st_size > 0) {
                perror(argv[1]);
                goto exit_error;
        }

        const char *pre = "";
        if (lcopy_size >= 1024) {
                copy_size = lcopy_size / 1024;
                pre = "Ki";
        }

        printf("\rCopied %d%sB in %d.%03d seconds (%d.%03d KiB/s)\n",
               copy_size,
               pre,
               stop_time / CLOCKS_PER_SEC,
               stop_time % CLOCKS_PER_SEC,
               (((u32_t)lcopy_size / stop_time) * CLOCKS_PER_SEC) / 1024,
               (((u32_t)lcopy_size / stop_time) * CLOCKS_PER_SEC) % 1024);

        fclose(src_file);
        fclose(dst_file);
        free(buffer);

        return EXIT_SUCCESS;

exit_error:
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

/*==============================================================================
  End of file
==============================================================================*/
