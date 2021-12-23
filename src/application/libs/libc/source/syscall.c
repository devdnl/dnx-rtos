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
typedef void (*syscall_func)(_libc_syscall_t, void*, va_list);

/**
 * @brief dnx RTOS application context.
 */
typedef struct {
        void **stdin_ref;
        void **stdout_ref;
        void **stderr_ref;
        void **global_ref;
        int   *errno_ref;
} dnxrtctx_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/
extern void **_libc_stdin;
extern void **_libc_stdout;
extern void **_libc_stderr;
extern int   *_libc_errno;
extern void **_libc_global;

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief  Get dnx RTOS application context.
 *
 * @param  retptr       return pointer
 * @param  ...          arguments
 */
//==============================================================================
static void get_context(void *retptr, ...)
{
        va_list args;
        va_start(args, retptr);
        dnx_syscall(_LIBC_SYS_GETRUNTIMECTX, retptr, args);
        va_end(args);
}

//==============================================================================
/**
 * @brief  Syscall function.
 *
 * @param  syscall      syscall number
 * @param  retptr       return variable pointer
 * @param  ...          additional arguments
 */
//==============================================================================
void _libc_syscall(_libc_syscall_t syscall, void *retptr, ...)
{
        /*
         * This part of code is called if dnx RTOS context is not referenced.
         */
        if (_libc_stdin == NULL) {
                dnxrtctx_t dnxctx;
                dnxctx.stdin_ref  = NULL;
                dnxctx.stdout_ref = NULL;
                dnxctx.stderr_ref = NULL;
                dnxctx.global_ref = NULL;
                dnxctx.errno_ref  = NULL;

                int err = -1;
                get_context(&err, &dnxctx);

                _libc_stdin  = dnxctx.stdin_ref;
                _libc_stdout = dnxctx.stdout_ref;
                _libc_stderr = dnxctx.stderr_ref;
                _libc_global = dnxctx.global_ref;
                _libc_errno  = dnxctx.errno_ref;
        }

        /*
         * dnx RTOS syscall execution.
         */
        va_list args;
        va_start(args, retptr);
        dnx_syscall(syscall, retptr, args);
        va_end(args);
}

/*==============================================================================
  End of file
==============================================================================*/
