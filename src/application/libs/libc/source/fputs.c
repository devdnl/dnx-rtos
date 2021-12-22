/*=========================================================================*//**
@file    fputs.c

@author  Daniel Zorychta

@brief   String put functions.

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
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Function puts string to selected file (fputs & puts)
 *
 * @param[in] *s        string
 * @param[in] *file     file
 * @param[in]  puts     puts functionality (true: add \n at the end of string)
 *
 * @return number of characters written to the stream
 */
//==============================================================================
static int f_puts(const char *s, FILE *file, bool puts)
{
        if (file) {
                int n = EOF;

                if (puts) {
                        char *buf = malloc(strlen(s) + 2);
                        if (buf) {
                                strcpy(buf, s);
                                strcat(buf, "\n");
                                n = fwrite(buf, sizeof(char), strlen(buf), file);
                                free(buf);
                        }
                } else {
                        n = fwrite(s, sizeof(char), strlen(s), file);
                }

                if (n != 0) {
                        return n;
                }
        }
        return EOF;
}

//==============================================================================
/**
 * @brief Function puts string to selected file
 *
 * @param[in] *s        string
 * @param[in] *file     file
 *
 * @return number of characters written to the stream
 */
//==============================================================================
int fputs(const char *s, FILE *file)
{
        return f_puts(s, file, false);
}

//==============================================================================
/**
 * @brief Function puts string to stdout
 *
 * @param[in] *s        string
 *
 * @return number of characters written to the stream
 */
//==============================================================================
int puts(const char *s)
{
        return f_puts(s, stdout, true);
}

/*==============================================================================
  End of file
==============================================================================*/
