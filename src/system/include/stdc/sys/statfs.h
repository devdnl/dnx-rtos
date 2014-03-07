/*=========================================================================*//**
@file    statfs.h

@author  Daniel Zorychta

@brief

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

#ifndef _STATFS_H_
#define _STATFS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "core/vfs.h"

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
 * @brief int statfs(const char *path, struct statfs *statfs)
 * The function <b>statfs</b>() returns information about a mounted file system.
 * <i>path</i> is the pathname of any file within the mounted file system.
 * <i>buf</i> is a pointer to a <b>statfs</b> structure defined as follows:
 * <pre>
 * struct statfs {
 *         u32_t f_type;            // file system type
 *         u32_t f_bsize;           // block size
 *         u32_t f_blocks;          // total blocks
 *         u32_t f_bfree;           // free blocks
 *         u32_t f_files;           // total file nodes in FS
 *         u32_t f_ffree;           // free file nodes in FS
 *         const char *f_fsname;    // FS name
 * };
 * </pre>
 *
 * @param pathname      node name
 * @param dev           device number
 *
 * @errors EINVAL, ENOMEM, EIO, EACCES, ENOENT, ENOTDIR, ...
 *
 * @return On success, 0 is returned. On error, -1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @example
 * // ...
 *
 * struct statfs info;
 * if (statfs("/proc", &info) == 0) {
 *         // ...
 * } else {
 *         perror("/proc");
 *         // ...
 * }
 *
 * // ...
 */
//==============================================================================
static inline int statfs(const char *path, struct statfs *statfs)
{
        return vfs_statfs(path, statfs);
}

#ifdef __cplusplus
}
#endif

#endif /* _STATFS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
