/*=============================================================================================*//**
@file    cat.c

@author  Daniel Zorychta

@brief

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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


*//*==============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "cat.h"
#include "tty_def.h"
#include <string.h>

/* Begin of application section declaration */
PROGRAM(cat, 3)
PROG_SEC_BEGIN

/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/


//================================================================================================//
/**
 * @brief clear main function
 */
//================================================================================================//
stdRet_t appmain(ch_t *argv[], int_t argc)
{
        printf("Argument count: %d\n", argc);

        for (int_t i = 0; i < argc; i++) {
                printf("%s ", argv[i]);
        }
        putchar('\n');

        return 0;


      stdRet_t status = STD_RET_OK;

      u32_t col = 80;
      ioctl(stdin, TTY_IORQ_GETCOL, &col);

      ch_t *data     = calloc(col + 1, sizeof(ch_t));
      ch_t *filepath = calloc(128, sizeof(ch_t));

      if (data && filepath) {
            if (argv[0] == '/') {
                  strcpy(filepath, argv);
            } else {
                  getcwd(filepath, 128);

                  if (filepath[strlen(filepath) - 1] != '/') {
                        strcat(filepath, "/");
                  }

                  strcat(filepath, argv);
            }

            FILE_t *file = fopen(filepath, "r");

            if (file) {
                  fseek(file, 0, SEEK_END);
                  i32_t filesize = ftell(file);
                  fseek(file, 0, SEEK_SET);

                  while (filesize > 0) {
                        i32_t n = fread(data, sizeof(ch_t), col, file);

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
                  printf("No such file\n");

                  status = STD_RET_ERROR;
            }

      } else {
            printf("Enough free memory\n");

            status = STD_RET_ERROR;
      }

      if (data) {
            free(data);
      }

      if (filepath) {
            free(filepath);
      }

      return status;
}

/* End of application section declaration */
PROG_SEC_END

#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
