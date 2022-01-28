/*=========================================================================*//**
@file    ioctl.h

@author  Daniel Zorychta

@brief   Header contain all device control commands. Depend on existing drivers.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/**
\defgroup sys-ioctl-h <sys/ioctl.h>

The library is used to control IO and files in non-standard way.

*/
/**@{*/

#ifndef _LIBC_IOCTL_H_
#define _LIBC_IOCTL_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <stdarg.h>
#include <drivers/ioctl_requests.h> // to remove in future
#include <libc/source/syscall.h>
#include <stdbool.h>

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 * @brief Request closes opened \b pipe device.
 *
 * Request closes opened \b pipe device.
 *
 * @see   ioctl()
 */
#define IOCTL_PIPE__CLOSE                       0xF0000

/**
 * @brief Request clears \b pipe device.
 *
 * Request clears \b pipe device.
 *
 * @see   ioctl()
 */
#define IOCTL_PIPE__CLEAR                       0xF0001

/**
 * @brief Request set stream to non-blocking read mode.
 *
 * Request set stream to non-blocking read mode.
 *
 * @see   ioctl()
 */
#define IOCTL_VFS__NON_BLOCKING_RD_MODE         0xF0003

/**
 * @brief Request set stream to default read mode.
 *
 * Request set stream to default read mode.
 *
 * @see   ioctl()
 */
#define IOCTL_VFS__DEFAULT_RD_MODE              0xF0004

/**
 * @brief Request return read mode status.
 *
 * @see   ioctl()
 */
#define IOCTL_VFS__IS_NON_BLOCKING_RD_MODE      0xF0005

/**
 * @brief Request set stream to non-blocking write mode.
 *
 * Request set stream to non-blocking write mode.
 *
 * @see   ioctl()
 */
#define IOCTL_VFS__NON_BLOCKING_WR_MODE         0xF0006

/**
 * @brief Request set stream to default write mode.
 *
 * Request set stream to default write mode.
 *
 * @see   ioctl()
 */
#define IOCTL_VFS__DEFAULT_WR_MODE              0xF0007

/**
 * @brief Request return write mode status.
 *
 * @see   ioctl()
 */
#define IOCTL_VFS__IS_NON_BLOCKING_WR_MODE      0xF0008

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
 * @brief Function sends request to selected file to do non-standard operation.
 *
 * The ioctl() function manipulates the file parameters. In particular, many
 * operating characteristics of character special files (e.g., drivers) may
 * be controlled with ioctl() requests.
 *
 * The second argument is a device-dependent request code. The third
 * argument is an untyped pointer to memory.
 *
 * @param fd            file descriptor
 * @param request       request number (each driver has own requests)
 * @param ...           untyped pointer to memory (optional in some requests)
 *
 * @exception | ...
 *
 * @return On success zero is returned. On error, -1 is returned, and
 * <b>errno</b> is set appropriately.
 *
 * @b Example @b 1
 * @code
        // ...

        FILE *file = fopen("/dev/tty0", "r");
        if (file) {
                ioctl(fileno(file), IOCTL_TTY__CLEAR_SCR);

                // ...
        } else {
                perror(NULL);
        }

        fclose(file);

        // ...

   @endcode

 * @b Example @b 2
 * @code
        // ...

        FILE *file = fopen("/dev/tty0", "r");
        if (file) {
                int row = -1;
                ioctl(fileno(file), IOCTL_TTY__GET_ROW, &row);

                // ...
        } else {
                perror(NULL);
        }

        fclose(file);

        // ...

   @endcode
 *
 * @note
 * The names of all requests are constructed in the same way: @b IOCTL_<MODULE_NAME>__<REQUEST_NAME>.
 */
//==============================================================================
static inline int ioctl(int fd, int request, ...)
{
        va_list arg;
        va_start(arg, request);
        int err = _libc_syscall(_LIBC_SYS_IOCTL, &fd, &request, &arg);
        va_end(arg);
        return err ? -1 : 0;
}

#ifdef __cplusplus
}
#endif

#endif /* _LIBC_IOCTL_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
