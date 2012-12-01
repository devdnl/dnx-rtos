#ifndef LFS_H_
#define LFS_H_
/*=============================================================================================*//**
@file    lfs.h

@author  Daniel Zorychta

@brief   This file support lfs - list file system

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


*//*==============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "basic_types.h"
#include "vfs.h"
#include "systypes.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern stdRet_t  lfs_init   (u32_t dev);
extern stdRet_t  lfs_open   (u32_t dev, fd_t *fd, const ch_t *path, const ch_t *mode);
extern stdRet_t  lfs_close  (u32_t dev, fd_t fd);
extern size_t    lfs_write  (u32_t dev, fd_t fd, void *src, size_t size, size_t nitems, size_t seek);
extern size_t    lfs_read   (u32_t dev, fd_t fd, void *dst, size_t size, size_t nitems, size_t seek);
extern stdRet_t  lfs_ioctl  (u32_t dev, fd_t fd, IORq_t iroq, void *data);
extern stdRet_t  lfs_mkdir  (u32_t dev, const ch_t *path);
extern stdRet_t  lfs_mknod  (u32_t dev, const ch_t *path, struct vfs_drvcfg *drvcfg);
extern stdRet_t  lfs_opendir(u32_t dev, const ch_t *path, DIR_t *dir);
extern stdRet_t  lfs_remove (u32_t dev, const ch_t *path);
extern stdRet_t  lfs_rename (u32_t dev, const ch_t *oldName, const ch_t *newName);
extern stdRet_t  lfs_chmod  (u32_t dev, u32_t mode);
extern stdRet_t  lfs_chown  (u32_t dev, u16_t owner, u16_t group);
extern stdRet_t  lfs_stat   (u32_t dev, const ch_t *path, struct vfs_stat *stat);
extern stdRet_t  lfs_statfs (u32_t dev, struct vfs_statfs *statfs);
extern stdRet_t  lfs_release(u32_t dev);


#ifdef __cplusplus
}
#endif

#endif /* LFS_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
