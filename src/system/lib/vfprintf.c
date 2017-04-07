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
#include "config.h"
#include "dnx/misc.h"
#include "lib/vfprintf.h"
#include "lib/vsnprintf.h"
#include "lib/cast.h"
#include "mm/mm.h"

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
int _vfprintf(FILE *file, const char *format, va_list arg)
{
        int n = 0;

#if (__OS_PRINTF_ENABLE__ > 0)

        if (file && format) {
                va_list carg;
                va_copy(carg, arg);
                u32_t size = _vsnprintf(NULL, 0, format, carg) + 1;
                va_end(carg);

                char *str = NULL;
                int err = _kzalloc(_MM_KRN, size, cast(void*, &str));
                if (!err && str) {
                        n = _vsnprintf(str, size, format, arg);

                        size_t wrcnt;
                        _vfs_fwrite(str, n, &wrcnt, file);

                        _kfree(_MM_KRN, cast(void*, &str));
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
