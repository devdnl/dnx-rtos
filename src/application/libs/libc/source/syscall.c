/*==============================================================================
File     syscall.c

Author   Daniel Zorychta

Brief    Syscall handling.

	 Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

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


==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include <stddef.h>
#include <stdarg.h>
#include "syscall.h"
#include "common.h"

/*==============================================================================
  Local macros
==============================================================================*/
/*
 * Syscall section address is 0x08000400 in normal mode.
 * The syscall section compiled is in the Thumb mode. To indicate this mode jump
 * address is incremented by 1.
 */
#define dnx_syscall ((syscall_func)(0x08000401))

/*==============================================================================
  Local object types
==============================================================================*/
typedef int (*syscall_func)(_libc_syscall_t, va_list);

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
 * @brief  Syscall function.
 *
 * @param  syscall      syscall number
 * @param  ...          additional arguments
 *
 * @return Operation status (errno).
 */
//==============================================================================
int _libc_syscall(_libc_syscall_t syscall, ...)
{
        va_list args;
        va_start(args, syscall);
        int err = dnx_syscall(syscall, args);
        va_end(args);
        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
