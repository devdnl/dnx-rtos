/*=========================================================================*//**
@file    vfprintf.c

@author  Daniel Zorychta

@brief   Print functions.

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes FreeRTOS without
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
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <lib/vsnprintf.h>
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
 * @brief Function write to file formatted string
 *
 * @param file                file
 * @param format              formated text
 * @param arg                 arguments
 *
 * @retval number of written characters
 */
//==============================================================================
int vfprintf(FILE *file, const char *format, va_list arg)
{
        int n = 0;

#if (__OS_PRINTF_ENABLE__ > 0)
        if (file && format) {
                va_list carg;
                va_copy(carg, arg);
                u32_t size = vsnprintf(NULL, 0, format, carg) + 1;
                va_end(carg);

                char *str = calloc(1, size);
                if (str) {
                        n = vsnprintf(str, size, format, arg);
                        fwrite(str, sizeof(char), n, file);
                        int err = _errno;
                        free(str);
                        _errno = err;
                }
        }
#else
        UNUSED_ARG3(file, format, arg);
#endif

        return n;
}

/*==============================================================================
  End of file
==============================================================================*/
