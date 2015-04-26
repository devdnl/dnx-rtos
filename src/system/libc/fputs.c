/*=========================================================================*//**
@file    fputs.c

@author  Daniel Zorychta

@brief

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
#include <config.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

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
#if (CONFIG_PRINTF_ENABLE > 0)
        if (file) {
                int n = fwrite(s, sizeof(char), strlen(s), file);

                if (puts) {
                        n += fwrite("\n", sizeof(char), 1, file);
                }

                if (n != 0) {
                        return n;
                }
        }
#else
        UNUSED_ARG(s);
        UNUSED_ARG(file);
        UNUSED_ARG(puts);
#endif
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
