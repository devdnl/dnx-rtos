/*=========================================================================*//**
@file    statfs.h

@author  Daniel Zorychta

@brief   File systems information.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief Function gets file system information.
 *
 * The function <b>statfs</b>() returns information about a mounted file system.
 * A <i>path</i> is directory of the mount point of file system.
 *
 * @param path          node name
 * @param statfs        file system information container
 *
 * @exception EINVAL    invalid argument
 * @exception ENOMEM    not enough free memory to create directory
 * @exception EACCES    access denied
 * @exception EEXIST    directory already exists
 * @exception ENOENT    parent directory does not exists
 * @exception ENOSPC    not enough free space on disc
 * @exception ENOTDIR   path not point to the directory
 * @exception ...       other errors returned by file system
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
 * @see mount()
 * @see umount()
 */
//==============================================================================
static inline int statfs(const char *path, struct statfs *statfs)
{
        int r = -1;
        syscall(SYSCALL_STATFS, &r, path, statfs);
        return r;
}

#ifdef __cplusplus
}
#endif

#endif /* _STATFS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
