/*=========================================================================*//**
@file    ioctl.h

@author  Daniel Zorychta

@brief   Header contain all device control commands. Depend on existing drivers.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _IOCTL_H_
#define _IOCTL_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <stdarg.h>
#include <drivers/ioctl_macros.h>
#include <drivers/ioctl_requests.h>
#include <kernel/syscall.h>

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief int ioctl(FILE *stream, int request, ...)
 * The <b>ioctl</b>() function manipulates the file parameters.  In particular, many
 * operating characteristics of character special files (e.g., drivers) may
 * be controlled with <b>ioctl</b>() requests.
 *
 * The second argument is a device-dependent request code. The third
 * argument is an untyped pointer to memory.
 *
 * @param seconds   number of seconds to sleep
 *
 * @errors EINVAL, ENOENT, EBADRQC, ...
 *
 * @return On success zero is returned. On error, -1 is returned, and
 * <b>errno</b> is set appropriately.
 *
 * @example
 * // ...
 * FILE *file = fopen("/dev/tty0", "r");
 * if (file) {
 *         ioctl(file, TTY_IORQ_CLEAR_SCR);
 *
 *         // ...
 * }
 *
 * // ...
 */
//==============================================================================
static inline int ioctl(FILE *stream, int request, ...)
{
        va_list arg;
        va_start(arg, request);
        int r = -1;
        syscall(SYSCALL_IOCTL, &r, stream, &request, &arg);
        va_end(arg);
        return r;
}

#ifdef __cplusplus
}
#endif

#endif /* _IOCTL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
