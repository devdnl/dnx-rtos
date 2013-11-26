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
#include <errno.h>
#include "system/ioctl.h"

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
 */
//==============================================================================
static void remove_control_characters(char *str)
{
        for (uint i = 0; i < strlen(str); i++) {
                if (str[i] < ' ' && str[i] != '\n') {
                        str[i] = ' ';
                }
        }
}

//==============================================================================
/**
 * @brief Cat main function
 */
//==============================================================================
PROGRAM_MAIN(cat, int argc, char *argv[])
{
        int status = EXIT_SUCCESS;

        if (argc == 1) {
                printf("Usage: %s <file>\n", argv[0]);
                return EXIT_FAILURE;
        }

        errno = 0;

        u32_t col = 80;
        ioctl(stdin, TTY_IORQ_GET_COL, &col);

        char *str = calloc(col + 1, sizeof(char));
        if (str) {
                FILE *file = fopen(argv[1], "r");
                if (file) {
                        while (fgets(str, col, file)) {
                                remove_control_characters(str);

                                if (LAST_CHARACTER(str) != '\n') {
                                        strcat(str, "\n");
                                }

                                fputs(str, stdout);
                        }
                        fclose(file);
                } else {
                        perror(argv[1]);
                        status = EXIT_FAILURE;
                }
        } else {
                perror(NULL);
                status = EXIT_FAILURE;
        }

        if (str) {
                free(str);
        }

        return status;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
