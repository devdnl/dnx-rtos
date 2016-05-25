/*=========================================================================*//**
@file    printk.c

@author  Daniel Zorychta

@brief   Kernel print support

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
#include <stddef.h>
#include "kernel/printk.h"
#include "config.h"
#include "fs/vfs.h"
#include "mm/mm.h"
#include "dnx/misc.h"
#include "lib/vsnprintf.h"
#include "libc/errno.h"

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
#if ((__OS_SYSTEM_MSG_ENABLE__ > 0) && (__OS_PRINTF_ENABLE__ > 0))
static FILE *printk_file;
#endif

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
 * @brief Enable printk functionality
 *
 * @param filename      path to file used to write kernel log
 *
 * @return One of errno value.
 */
//==============================================================================
int _printk_enable(const char *filename)
{
#if ((__OS_SYSTEM_MSG_ENABLE__ > 0) && (__OS_PRINTF_ENABLE__ > 0))
        if (printk_file) {
                _vfs_fclose(printk_file, false);
                printk_file = NULL;
        }

        return _vfs_fopen(filename, "w+", &printk_file);
#else
        UNUSED_ARG1(filename);
        return ENOTSUP;
#endif
}

//==============================================================================
/**
 * @brief Disable printk functionality
 *
 * @param None
 *
 * @return One of errno value
 */
//==============================================================================
int _printk_disable(void)
{
#if ((__OS_SYSTEM_MSG_ENABLE__ > 0) && (__OS_PRINTF_ENABLE__ > 0))
        if (printk_file) {
                int result  = _vfs_fclose(printk_file, false);
                printk_file = NULL;
                return result;
        } else {
                return ESUCC;
        }
#else
        return ENOTSUP;
#endif
}

//==============================================================================
/**
 * @brief Function send kernel message on terminal
 *
 * @param *format             formated text
 * @param ...                 format arguments
 */
//==============================================================================
void _printk(const char *format, ...)
{
#if ((__OS_SYSTEM_MSG_ENABLE__ > 0) && (__OS_PRINTF_ENABLE__ > 0))
        va_list args;

        if (printk_file) {
                va_start(args, format);
                int size = _vsnprintf(NULL, 0, format, args) + 1;
                va_end(args);

                char *buffer = NULL;
                _kzalloc(_MM_KRN, size, cast(void**, &buffer));

                if (buffer) {
                        va_start(args, format);
                        int n = _vsnprintf(buffer, size, format, args);
                        va_end(args);

                        size_t wrcnt;
                        _vfs_fwrite(buffer, n, &wrcnt, printk_file);

                        if (LAST_CHARACTER(buffer) != '\n') {
                                _vfs_fflush(printk_file);
                        }

                        _kfree(_MM_KRN, cast(void**, &buffer));
                }
        }
#else
        UNUSED_ARG1(format);
#endif
}

/*==============================================================================
  End of file
==============================================================================*/
