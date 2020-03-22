/*=========================================================================*//**
@file    fsctrl.c

@author  Daniel Zorychta

@brief   File system control support.

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

/*==============================================================================
  Include files
==============================================================================*/
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include "fs/fsctrl.h"
#include "fs/vfs.h"
#include "kernel/sysfunc.h"

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
 * @brief Function mount file system.
 *
 * @param *FS_name       file system name.
 * @param *src_path      path to file with source data.
 * @param *mount_point   mount point of file system.
 *
 * @return One of errno values.
 */
//==============================================================================
int _mount(const char            *FS_name,
           const struct vfs_path *src_path,
           const struct vfs_path *mount_point,
           const char            *opts)
{
        int err = EINVAL;

        if (FS_name && mount_point && mount_point->PATH && src_path && src_path->PATH) {
                err = ENOENT;

                for (uint i = 0; i < _FS_table_size; i++) {
                        if (strcmp(_FS_table[i].FS_name, FS_name) == 0) {
                                err = _vfs_mount(src_path, mount_point,
                                                 &_FS_table[i].FS_if, opts);

                                if (!err) {
                                        printk("Filesystem '%s' mounted in %s",
                                               FS_name, mount_point->PATH);
                                } else {
                                        printk("Filesystem '%s' on '%s' mount error (%d)",
                                               FS_name, src_path->PATH, err);
                                }

                                break;
                        }
                }

                if (err == ENOENT) {
                        printk("Filesystem '%s' does not exist", FS_name);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Function unmount file system.
 *
 * @param *mount_point   path to file system.
 *
 * @return One of errno values.
 */
//==============================================================================
int _umount(const struct vfs_path *mount_point)
{
        if (mount_point) {
                int err = _vfs_umount(mount_point);

                if (err) {
                        printk("Filesystem at '%s' unmount fail (%d)", mount_point->PATH, err);
                } else {
                        printk("Filesystem at '%s' unmounted", mount_point->PATH);
                }

                return err;

        } else {
                return EINVAL;
        }
}

/*==============================================================================
  End of file
==============================================================================*/
