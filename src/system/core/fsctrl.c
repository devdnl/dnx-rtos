/*=========================================================================*//**
@file    fsctrl.c

@author  Daniel Zorychta

@brief   File system control support.

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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "core/systypes.h"
#include "core/fsctrl.h"
#include "core/vfs.h"

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
extern const struct _FS_entry _FS_table[];
extern const uint             _FS_table_size;

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Function mount file system
 *
 * @param *FS_name       file system name
 * @param *src_path      path to file with source data
 * @param *mount_point   mount point of file system
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t _mount(const char *FS_name, const char *src_path, const char *mount_point)
{
        if (!FS_name || !mount_point || !src_path) {
                return STD_RET_ERROR;
        }

        for (uint i = 0; i < _FS_table_size; i++) {
                if (strcmp(_FS_table[i].FS_name, FS_name) == 0) {
                        return vfs_mount(src_path, mount_point,
                                         (struct vfs_FS_interface *)&_FS_table[i].FS_if);
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function unmount file system
 *
 * @param *mount_point   path to file system
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t _umount(const char *mount_point)
{
        if (mount_point)
                return vfs_umount(mount_point);
        else
                return STD_RET_ERROR;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
