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
#include <errno.h>
#include <dnx/os.h>
#include <dnx/timer.h>

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
GLOBAL_VARIABLES_SECTION_BEGIN

GLOBAL_VARIABLES_SECTION_END

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
PROGRAM_MAIN(cp, int argc, char *argv[])
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

        uint  start_time = get_time_ms();
        uint  info_timer = timer_set_expired();
        u64_t lcopy_size = 0;
        int   n;

        while ((n = fread(buffer, sizeof(char), buffer_size, src_file)) > 0) {
                if (ferror(src_file)) {
                        perror(argv[1]);
                        break;
                }

                if (timer_is_expired(info_timer, INFO_REFRESH_TIME_MS)) {
                        info_timer = timer_reset();

                        u32_t file_size = lfile_size / 1024;
                        u32_t copy_size = lcopy_size / 1024;

                        printf("\r%d.%2d%% copied...",
                               ((copy_size*100)/file_size),
                               ((copy_size*10000)/file_size) % 100);
                }

                fwrite(buffer, sizeof(char), n, dst_file);
                if (ferror(dst_file)) {
                        perror(argv[2]);
                        break;
                }

                lcopy_size += n;
        }

        uint  stop_time = get_time_ms() - start_time;
        u32_t copy_size = lcopy_size;

        if (ferror(src_file)) {
                perror(argv[1]);
                goto exit_error;
        }

        const char *pre = "";
        if (lcopy_size >= 1024) {
                copy_size = lcopy_size / 1024;
                pre = "Ki";
        }

        printf("\rCopied %d%sB in %d.%3d seconds (%d.%3d KiB/s)\n",
               copy_size,
               pre,
               stop_time / 1000,
               stop_time % 1000,
               (((u32_t)lcopy_size / stop_time) * 1000) / 1024,
               (((u32_t)lcopy_size / stop_time) * 1000) % 1024);

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

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
