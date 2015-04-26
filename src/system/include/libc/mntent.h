/*=========================================================================*//**
@file    mntent.h

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

#ifndef _MNTENT_H_
#define _MNTENT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "kernel/syscall.h"

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
 * @brief int getmntentry(int item, struct mntent *mntent)
 * Function return file system describe object. After operation object must be
 * freed using free() function.<p>
 *
 * <b>mntent</b> structure:
 * <pre>
 * struct mntent {
 *         const char *mnt_fsname;    // device or server for file system
 *         const char *mnt_dir;       // directory mounted on
 *         u64_t       total;         // device total size
 *         u64_t       free;          // device free
 * };
 * </pre>
 *
 * @param item          n-item to read
 * @param mntent        pointer to mntent object
 *
 * @errors EINVAL
 *
 * @return Returns 0 on success. Returns 1 if all items was read. On error -1 is
 * returned and <b>errno</b> is set appropriately.
 *
 * @example
 * // ...
 *
 * struct mntent entry;
 * int n = 0;
 * while (getmntentry(i++, &entry) == 0) {
 *        // ...
 * }
 */
//==============================================================================
static inline int getmntentry(int item, struct mntent *mntent)
{
        int r = -1;
        syscall(SYSCALL_OPENDIR, &r, &item, mntent);
        return r;
}

#ifdef __cplusplus
}
#endif

#endif /* _MNTENT_H_ */
/*==============================================================================
  End of file
==============================================================================*/
