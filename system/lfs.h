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
#include "systypes.h"
#include "vfs.h"
#include "system.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/
/* USER CFG: memory management, free memory */
#define FREE(ptr)             free(ptr)

/* USER CFG: memory management, memory allocation */
#define CALLOC(nmemb, msize)  calloc(nmemb, msize)

/* USER CFG: memory management, memory allocation */
#define MALLOC(size)          malloc(size)


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern stdRet_t  lfs_init   (devx_t dev);
extern stdRet_t  lfs_open   (devx_t dev, fd_t *fd, size_t *seek, const ch_t *path, const ch_t *mode);
extern stdRet_t  lfs_close  (devx_t dev, fd_t fd);
extern size_t    lfs_write  (devx_t dev, fd_t fd, void *src, size_t size, size_t nitems, size_t seek);
extern size_t    lfs_read   (devx_t dev, fd_t fd, void *dst, size_t size, size_t nitems, size_t seek);
extern stdRet_t  lfs_ioctl  (devx_t dev, fd_t fd, IORq_t iorq, void *data);
extern stdRet_t  lfs_mkdir  (devx_t dev, const ch_t *path);
extern stdRet_t  lfs_mknod  (devx_t dev, const ch_t *path, struct vfs_drvcfg *drvcfg);
extern stdRet_t  lfs_opendir(devx_t dev, const ch_t *path, DIR_t *dir);
extern stdRet_t  lfs_remove (devx_t dev, const ch_t *path);
extern stdRet_t  lfs_rename (devx_t dev, const ch_t *oldName, const ch_t *newName);
extern stdRet_t  lfs_chmod  (devx_t dev, const ch_t *path, u32_t mode);
extern stdRet_t  lfs_chown  (devx_t dev, const ch_t *path, u16_t owner, u16_t group);
extern stdRet_t  lfs_stat   (devx_t dev, const ch_t *path, struct vfs_stat *stat);
extern stdRet_t  lfs_fstat  (devx_t dev, fd_t fd, struct vfs_stat *stat);
extern stdRet_t  lfs_statfs (devx_t dev, struct vfs_statfs *statfs);
extern stdRet_t  lfs_release(devx_t dev);


#ifdef __cplusplus
}
#endif

#endif /* LFS_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
