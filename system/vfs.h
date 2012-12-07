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
#include "system.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/
/* USER CFG: memory management, free memory */
#define FREE(ptr)             free(ptr)

/* USER CFG: memory management, memory allocation */
#define CALLOC(nmemb, msize)  calloc(nmemb, msize)

/* USER CFG: memory management, memory allocation */
#define MALLOC(size)          malloc(size)


/* set position equal to offset bytes */
#define VFS_SEEK_SET    0

/* set position to current location plus offset */
#define VFS_SEEK_CUR    1

/* set position to EOF plus offset */
#define VFS_SEEK_END    2

struct vfs_stat {
     u32_t  st_dev;           /* ID of device containing file */
     u32_t  st_rdev;          /* device ID (if special file) */
     u32_t  st_mode;          /* protection */
     u32_t  st_uid;           /* user ID of owner */
     u32_t  st_gid;           /* group ID of owner */
     size_t st_size;          /* total size, in bytes */
     u32_t  st_mtime;         /* time of last modification */
};

struct vfs_statfs {
      u32_t f_type;           /* file system type */
      u32_t f_blocks;         /* total blocks */
      u32_t f_bfree;          /* free blocks */
      u32_t f_files;          /* total file nodes in FS */
      u32_t f_ffree;          /* free file nodes in FS */
      const ch_t *fsname;     /* FS name */
};

struct vfs_drvcfg {
      u32_t    dev;
      u32_t    part;
      stdRet_t (*f_open )(devx_t dev, fd_t part);
      stdRet_t (*f_close)(devx_t dev, fd_t part);
      size_t   (*f_write)(devx_t dev, fd_t part, void *src, size_t size, size_t nitems, size_t seek);
      size_t   (*f_read )(devx_t dev, fd_t part, void *dst, size_t size, size_t nitems, size_t seek);
      stdRet_t (*f_ioctl)(devx_t dev, fd_t part, IORq_t iorq, void *data);
};

struct vfs_fscfg {
      u32_t     dev;
      stdRet_t  (*f_init   )(devx_t dev);
      stdRet_t  (*f_open   )(devx_t dev, fd_t *fd, size_t *seek, const ch_t *path, const ch_t *mode);
      stdRet_t  (*f_close  )(devx_t dev, fd_t fd);
      size_t    (*f_write  )(devx_t dev, fd_t fd, void *src, size_t size, size_t nitems, size_t seek);
      size_t    (*f_read   )(devx_t dev, fd_t fd, void *dst, size_t size, size_t nitems, size_t seek);
      stdRet_t  (*f_ioctl  )(devx_t dev, fd_t fd, IORq_t iroq, void *data);
      stdRet_t  (*f_fstat  )(devx_t dev, fd_t fd, struct vfs_stat *stat);
      stdRet_t  (*f_mkdir  )(devx_t dev, const ch_t *path);
      stdRet_t  (*f_mknod  )(devx_t dev, const ch_t *path, struct vfs_drvcfg *dcfg);
      stdRet_t  (*f_opendir)(devx_t dev, const ch_t *path, DIR_t *dir);
      stdRet_t  (*f_remove )(devx_t dev, const ch_t *path);
      stdRet_t  (*f_rename )(devx_t dev, const ch_t *oldName, const ch_t *newName);
      stdRet_t  (*f_chmod  )(devx_t dev, const ch_t *path, u32_t mode);
      stdRet_t  (*f_chown  )(devx_t dev, const ch_t *path, u16_t owner, u16_t group);
      stdRet_t  (*f_stat   )(devx_t dev, const ch_t *path, struct vfs_stat *stat);
      stdRet_t  (*f_statfs )(devx_t dev, struct vfs_statfs *statfs);
      stdRet_t  (*f_release)(devx_t dev);
};


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern stdRet_t vfs_init(void);
extern stdRet_t vfs_mount(const ch_t *path, struct vfs_fscfg *mountcfg);
extern stdRet_t vfs_umount(const ch_t *path);
extern stdRet_t vfs_mknod(const ch_t *path, struct vfs_drvcfg *drvcfg);
extern stdRet_t vfs_mkdir(const ch_t *path);
extern DIR_t   *vfs_opendir(const ch_t *path);
extern stdRet_t vfs_closedir(DIR_t *dir);
extern dirent_t vfs_readdir(DIR_t *dir);
extern stdRet_t vfs_remove(const ch_t *path);
extern stdRet_t vfs_rename(const ch_t *oldName, const ch_t *newName);
extern stdRet_t vfs_chmod(const ch_t *path, u16_t mode);
extern stdRet_t vfs_chown(const ch_t *path, u16_t owner, u16_t group);
extern stdRet_t vfs_stat(const ch_t *path, struct vfs_stat *stat);
extern stdRet_t vfs_statfs(const ch_t *path, struct vfs_statfs *statfs);
extern FILE_t  *vfs_fopen(const ch_t *path, const ch_t *mode);
extern stdRet_t vfs_fclose(FILE_t *file);
extern size_t   vfs_fwrite(void *ptr, size_t size, size_t nitems, FILE_t *file);
extern size_t   vfs_fread(void *ptr, size_t size, size_t nitems, FILE_t *file);
extern stdRet_t vfs_fseek(FILE_t *file, i32_t offset, i32_t mode);
extern i32_t    vfs_ftell(FILE_t *file);
extern stdRet_t vfs_ioctl(FILE_t *file, IORq_t rq, void *data);
extern stdRet_t vfs_fstat(FILE_t *file, struct vfs_stat *stat);


#ifdef __cplusplus
}
#endif

#endif /* VFS_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
