/*=========================================================================*//**
@file    vfprintf.c

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
