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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cat.h"
#include "drivers/tty_def.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define FILE_PATH_LEN           128

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
        char filepath[FILE_PATH_LEN];
};

/*==============================================================================
  Exported object definitions
==============================================================================*/
PROGRAM_PARAMS(cat, STACK_DEPTH_VERY_LOW);

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Cat main function
 */
//==============================================================================
int PROGRAM_MAIN(cat, int argc, char *argv[])
{
        int status = EXIT_SUCCESS;

        if (argc == 1) {
                printf("Usage: cat [file]\n");
                return STD_RET_ERROR;
        }

        u32_t col = 80;
        ioctl(stdin, TTY_IORQ_GET_COL, &col);

        char *data = calloc(col + 1, sizeof(char));

        if (data) {
                if (argv[1][0] == '/') {
                        strcpy(global->filepath, argv[1]);
                } else {
                        getcwd(global->filepath, 128);

                        if (global->filepath[strlen(global->filepath) - 1] != '/') {
                                strcat(global->filepath, "/");
                        }

                        strcat(global->filepath, argv[1]);
                }

                FILE *file = fopen(global->filepath, "r");

                if (file) {
                        fseek(file, 0, SEEK_END);
                        i32_t filesize = ftell(file);
                        fseek(file, 0, SEEK_SET);

                        while (filesize > 0) {
                                i32_t n = fread(data, sizeof(char), col, file);

                                if (n == 0) {
                                        break;
                                }

                                if (strchr(data, '\n') != NULL) {
                                        printf("%s", data);
                                } else {
                                        printf("%s\n", data);
                                }

                                memset(data, 0, col + 1);

                                filesize -= n;
                        }

                        fclose(file);
                } else {
                        printf("No such file or file is protected\n");

                        status = EXIT_FAILURE;
                }

        } else {
                printf("Enough free memory\n");

                status = EXIT_FAILURE;
        }

        if (data) {
                free(data);
        }

        return status;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
