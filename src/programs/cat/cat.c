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
PROGRAM_PARAMS(cat, STACK_DEPTH_LOW,);

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
                printf("Usage: %s <file>\n", argv[0]);
                return EXIT_FAILURE;
        }

        u32_t col = 80;
        ioctl(stdin, TTY_IORQ_GET_COL, &col);

        char *data = calloc(col + 1, sizeof(char));

        if (data) {
                FILE *file = fopen(argv[1], "r");
                if (file) {
                        while (fgets(data, col, file)) {
                                for (uint i = 0; i < strlen(data); i++) {
                                        if (data[i] < ' ' && data[i] != '\n') {
                                                data[i] = ' ';
                                        }
                                }

                                fputs(data, stdout);
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
