#ifndef VFS_H_
#define VFS_H_
/*=============================================================================================*//**
@file    vfs.h

@author  Daniel Zorychta

@brief   This file support virtual file system

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


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/
struct vfsnode
{
      fd_t    *(*getFD)(const ch_t *path);
      stdRet_t (*open)(fd_t fd);
      stdRet_t (*close)(fd_t fd);
      size_t   (*write)(fd_t fd, void *src, size_t size, size_t nitems, size_t seek);
      size_t   (*read)(fd_t fd, void *dst, size_t size, size_t nitems, size_t seek);
      stdRet_t (*ioctl)(fd_t fd, IORq_t iroq, void *data);
      stdRet_t (*mkdir)(const ch_t *path);
      DIR_t   *(*opendir)(const ch_t *path);
      dirent_t (*readdir)(DIR_t *dir);
      stdRet_t (*closedir)(DIR_t *dir);
      size_t   (*remove)(dirent_t *direntry);
      size_t   (*rename)(const ch_t *oldName, const ch_t *newName);
};

typedef struct vfsnode vfsnode_t;


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern stdRet_t vfs_init(void);
extern FILE_t  *vfs_fopen(const ch_t *name, const ch_t *mode);

extern stdRet_t vfs_mount(struct vfsnode node, const ch_t *path);
extern stdRet_t vfs_umount(const ch_t *path);

extern stdRet_t vfs_fclose(FILE_t *file);
extern size_t   vfs_fwrite(void *ptr, size_t size, size_t nitems, FILE_t *file);
extern size_t   vfs_fread(void *ptr, size_t size, size_t nitems, FILE_t *file);
extern stdRet_t vfs_fseek(FILE_t *file, i32_t offset, i32_t mode);
extern stdRet_t vfs_ioctl(FILE_t *file, IORq_t rq, void *data);
extern stdRet_t vfs_mkdir(const ch_t *path);
extern DIR_t   *vfs_opendir(const ch_t *path);
extern dirent_t vfs_readdir(DIR_t *dir);
extern stdRet_t vfs_closedir(DIR_t *dir);
extern size_t   vfs_remove(const ch_t *path);
extern size_t   vfs_rename(const ch_t *oldName, const ch_t *newName);

#ifdef __cplusplus
}
#endif

#endif /* VFS_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
