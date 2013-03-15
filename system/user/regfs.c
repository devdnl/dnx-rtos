/*=========================================================================*//**
@file    regfs.c

@author  Daniel Zorychta

@brief   This file is used to registration file systems

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "regfs.h"
#include <string.h>
#include "vfs.h"

/* include here FS headers */
#include "lfs.h"
#include "appfs.h"
#include "procfs.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define IMPORT_FILE_SYSTEM_INTERFACE(classname, fsname)\
{.fsName = fsname,\
 .mntcfg = {.f_fsd     = 0,\
            .f_init    = classname##_init,\
            .f_chmod   = classname##_chmod,\
            .f_chown   = classname##_chown,\
            .f_close   = classname##_close,\
            .f_ioctl   = classname##_ioctl,\
            .f_mkdir   = classname##_mkdir,\
            .f_mknod   = classname##_mknod,\
            .f_open    = classname##_open,\
            .f_opendir = classname##_opendir,\
            .f_read    = classname##_read,\
            .f_release = classname##_release,\
            .f_remove  = classname##_remove,\
            .f_rename  = classname##_rename,\
            .f_stat    = classname##_stat,\
            .f_fstat   = classname##_fstat,\
            .f_statfs  = classname##_statfs,\
            .f_write   = classname##_write}}

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
struct FS_entry {
      char             *fsName;
      struct vfs_FS_interface  mntcfg;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
/* driver registration */
static const struct FS_entry FS_table[] =
{
        IMPORT_FILE_SYSTEM_INTERFACE(lfs   , "lfs"   ),
//        IMPORT_FILE_SYSTEM_INTERFACE(appfs , "appfs" ),
        IMPORT_FILE_SYSTEM_INTERFACE(procfs, "procfs"),
};

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Function mount file system
 *
 * @param *fsname       file system name
 * @param *srcpath      path to file with source data
 * @param *mountpoint   mount point of file system
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t mount(const char *fsname, const char *srcpath, const char *mountpoint)
{
        stdret_t status = STD_RET_ERROR;

        if (fsname && mountpoint) {
                for (uint i = 0; i < ARRAY_SIZE(FS_table); i++) {
                        if (strcmp(FS_table[i].fsName, fsname) == 0) {
                                status = vfs_mount(srcpath, mountpoint,
                                                   (struct vfs_FS_interface *)&FS_table[i].mntcfg);
                                break;
                        }
                }
        }

        return status;
}


//==============================================================================
/**
 * @brief Function unmount file system
 *
 * @param *mountpoint   path to file system
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t umount(const char *mountpoint)
{
        stdret_t status = STD_RET_ERROR;

        if (mountpoint) {
                status = vfs_umount(mountpoint);
        }

        return status;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
