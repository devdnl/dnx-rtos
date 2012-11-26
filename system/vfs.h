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
struct vfsstat {
     u32_t  st_dev;           /* ID of device containing file */
     u32_t  st_mode;          /* protection */
     u32_t  st_uid;           /* user ID of owner */
     u32_t  st_gid;           /* group ID of owner */
     size_t st_size;          /* total size, in bytes */
     u32_t  st_mtime;         /* time of last modification */
};

struct vfsmcfg {
      stdRet_t  (*open   )(const ch_t *name, const ch_t *mode);
      stdRet_t  (*close  )(void *fd);
      size_t    (*write  )(void *fd, void *src, size_t size, size_t nitems, size_t seek);
      size_t    (*read   )(void *fd, void *dst, size_t size, size_t nitems, size_t seek);
      stdRet_t  (*ioctl  )(void *fd, IORq_t iroq, void *data);
      stdRet_t  (*mkdir  )(const ch_t *path);
      DIR_t    *(*opendir)(const ch_t *path);
      stdRet_t  (*remove )(const ch_t *path);
      stdRet_t  (*rename )(const ch_t *oldName, const ch_t *newName);
      stdRet_t  (*stat   )(const ch_t *path, struct vfsstat *stat);
};

typedef struct vfsmcfg vfsmcfg_t;

struct vfsdcfg {
      nod_t    device;
      stdRet_t (*open )(nod_t dev);
      stdRet_t (*close)(nod_t dev);
      size_t   (*write)(nod_t dev, void *src, size_t size, size_t nitems, size_t seek);
      size_t   (*read )(nod_t dev, void *dst, size_t size, size_t nitems, size_t seek);
      stdRet_t (*ioctl)(nod_t dev, IORq_t iroq, void *data);
};

typedef struct vfsdcfg vfsdcfg_t;




/*
struct fuse_operations {
    int (*getattr) (const char *, struct stat *);
    int (*readlink) (const char *, char *, size_t);
    int (*getdir) (const char *, fuse_dirh_t, fuse_dirfil_t);
    int (*mknod) (const char *, mode_t, dev_t);
    int (*mkdir) (const char *, mode_t);
    int (*unlink) (const char *);
    int (*rmdir) (const char *);
    int (*symlink) (const char *, const char *);
    int (*rename) (const char *, const char *);
    int (*link) (const char *, const char *);
    int (*chmod) (const char *, mode_t);
    int (*chown) (const char *, uid_t, gid_t);
    int (*truncate) (const char *, off_t);
    int (*utime) (const char *, struct utimbuf *);
    int (*open) (const char *, struct fuse_file_info *);
    int (*read) (const char *, char *, size_t, off_t, struct fuse_file_info *);
    int (*write) (const char *, const char *, size_t, off_t,struct fuse_file_info *);
    int (*statfs) (const char *, struct statfs *);
    int (*flush) (const char *, struct fuse_file_info *);
    int (*release) (const char *, struct fuse_file_info *);
    int (*fsync) (const char *, int, struct fuse_file_info *);
    int (*setxattr) (const char *, const char *, const char *, size_t, int);
    int (*getxattr) (const char *, const char *, char *, size_t);
    int (*listxattr) (const char *, char *, size_t);
    int (*removexattr) (const char *, const char *);
};*/





/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern stdRet_t vfs_init(void);
extern stdRet_t vfs_mount(const ch_t *path, vfsmcfg_t *mountcfg);
extern stdRet_t vfs_umount(const ch_t *path);
extern stdRet_t vfs_mknod(const ch_t *path, vfsdcfg_t *drvcfg);
extern stdRet_t vfs_mkdir(const ch_t *path);
extern DIR_t   *vfs_opendir(const ch_t *path);
extern stdRet_t vfs_closedir(DIR_t *dir);
extern dirent_t vfs_readdir(DIR_t *dir);
extern stdRet_t vfs_remove(const ch_t *path);
extern stdRet_t vfs_rename(const ch_t *oldName, const ch_t *newName);
extern stdRet_t vfs_stat(const ch_t *path, struct vfsstat *stat);

extern FILE_t  *vfs_fopen(const ch_t *name, const ch_t *mode);
extern stdRet_t vfs_fclose(FILE_t *file);
extern size_t   vfs_fwrite(void *ptr, size_t size, size_t nitems, FILE_t *file);
extern size_t   vfs_fread(void *ptr, size_t size, size_t nitems, FILE_t *file);
extern stdRet_t vfs_fseek(FILE_t *file, i32_t offset, i32_t mode);
extern stdRet_t vfs_ioctl(FILE_t *file, IORq_t rq, void *data);


#ifdef __cplusplus
}
#endif

#endif /* VFS_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
