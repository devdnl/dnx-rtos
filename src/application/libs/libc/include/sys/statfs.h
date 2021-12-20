/*=========================================================================*//**
@file    statfs.h

@author  Daniel Zorychta

@brief   File systems information.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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
\defgroup sys-statfs-h <sys/statfs.h>

The library is used to get file systems information.

*/
/**@{*/

#ifndef _STATFS_H_
#define _STATFS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <sys/types.h>
#include <libc/source/syscall.h>

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
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief Function gets file system information.
 *
 * The function statfs() returns information about a mounted file system.
 * A <i>path</i> is directory of the mount point of file system.
 *
 * @param path          node name
 * @param statfs        file system information container
 *
 * @exception | @ref EINVAL
 * @exception | @ref ENOMEM
 * @exception | @ref EACCES
 * @exception | @ref EEXIST
 * @exception | @ref ENOENT
 * @exception | @ref ENOSPC
 * @exception | @ref ENOTDIR
 * @exception | ...
 *
 * @return On success, \b 0 is returned. On error, \b -1 is returned, and \b errno
 * is set appropriately.
 *
 * @b Example
 * @code
        // ...

        struct statfs info;
        if (statfs("/proc", &info) == 0) {
                // ...
        } else {
                perror("/proc");
                // ...
        }

        // ...
   @endcode
 *
 * @see mount(), umount()
 */
//==============================================================================
static inline int statfs(const char *path, struct statfs *statfs)
{
        int r = -1;
        libc_syscall(_LIBC_SYS_STATFS, &r, path, statfs);
        return r;
}

#ifdef __cplusplus
}
#endif

#endif /* _STATFS_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
