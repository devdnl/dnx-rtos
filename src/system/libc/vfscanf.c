/*=========================================================================*//**
@file    vfscanf.c

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
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <dnx/misc.h>

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
 * @brief Function scan stream
 *
 * @param[in]  *stream        file
 * @param[in]  *format        message format
 * @param[out]  arg           output arguments
 *
 * @return number of scanned elements
 */
//==============================================================================
int vfscanf(FILE *stream, const char *format, va_list arg)
{
        int n = 0;

#if (CONFIG_SCANF_ENABLE > 0)
        char *str = calloc(BUFSIZ, sizeof(char));
        if (!str)
                return 0;

        if (fgets(str, BUFSIZ, stream) == str) {
                char *lf;
                if ((lf = strchr(str, '\n')) != NULL) {
                        *lf = '\0';
                }

                n = vsscanf(str, format, arg);
        }

        free(str);
#else
        UNUSED_ARG3(stream, format, arg);
#endif

        return n;
}

/*==============================================================================
  End of file
==============================================================================*/
