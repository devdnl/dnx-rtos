#ifndef VFS_H_
#define VFS_H_
/*=========================================================================*//**
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


*//*==========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "systypes.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/* set position equal to offset bytes */
#define VFS_SEEK_SET            0

/* set position to current location plus offset */
#define VFS_SEEK_CUR            1

/* set position to EOF plus offset */
#define VFS_SEEK_END            2

/* translate functions to STDC */
#ifndef SEEK_SET
#define SEEK_SET                VFS_SEEK_SET
#endif

#ifndef SEEK_CUR
#define SEEK_CUR                VFS_SEEK_CUR
#endif

#ifndef SEEK_END
#define SEEK_END                VFS_SEEK_END
#endif

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
/** device number type */
typedef uint devx_t;

/** IO request type */
typedef uint iorq_t;

/** file descriptor */
typedef uint fd_t;

/** file system descriptor */
typedef uint fsd_t;

/** file type */
typedef enum
{
        FILE_TYPE_REGULAR,
        FILE_TYPE_DIR,
        FILE_TYPE_DRV,
        FILE_TYPE_LINK
} tfile_t;

/** directory entry */
typedef struct
{
        char   *name;
        size_t  size;
        tfile_t filetype;
} dirent_t;

/** directory type */
struct vfs_dir
{
        dirent_t (*rddir)(fsd_t fsd, struct vfs_dir *dir);
        stdret_t (*cldir)(fsd_t fsd, struct vfs_dir *dir);
        size_t     items;
        size_t     seek;
        void      *dd;
        fsd_t      fsd;
};

/** file statistics */
struct vfs_stat {
        u32_t  st_dev;           /* ID of device containing file */
        u32_t  st_rdev;          /* device ID (if special file) */
        u32_t  st_mode;          /* protection */
        u32_t  st_uid;           /* user ID of owner */
        u32_t  st_gid;           /* group ID of owner */
        size_t st_size;          /* total size, in bytes */
        u32_t  st_mtime;         /* time of last modification */
};

/** file system statistic */
struct vfs_statfs {
        u32_t f_type;           /* file system type */
        u32_t f_blocks;         /* total blocks */
        u32_t f_bfree;          /* free blocks */
        u32_t f_files;          /* total file nodes in FS */
        u32_t f_ffree;          /* free file nodes in FS */
        const char *fsname;     /* FS name */
};

/** structure describing a mount table entry */
struct vfs_mntent {
        char *mnt_fsname;       /* device or server for filesystem.  */
        char *mnt_dir;          /* directory mounted on.  */
        u32_t total;            /* device total size */
        u32_t free;             /* device free */
};

/** driver configuration */
struct vfs_drvcfg {
        devx_t   dev;
        fd_t     part;
        stdret_t (*f_open )(devx_t dev, fd_t part);
        stdret_t (*f_close)(devx_t dev, fd_t part);
        size_t   (*f_write)(devx_t dev, fd_t part, void *src, size_t size, size_t nitems, size_t seek);
        size_t   (*f_read )(devx_t dev, fd_t part, void *dst, size_t size, size_t nitems, size_t seek);
        stdret_t (*f_ioctl)(devx_t dev, fd_t part, iorq_t iorq, void *data);
};

/** file system configuration */
struct vfs_fscfg {
        fsd_t    f_fsd;
        stdret_t (*f_init   )(const char *path, fsd_t *fsd);
        stdret_t (*f_open   )(fsd_t fsd, fd_t *fd, size_t *seek, const char *path, const char *mode);
        stdret_t (*f_close  )(fsd_t fsd, fd_t fd);
        size_t   (*f_write  )(fsd_t fsd, fd_t fd, void *src, size_t size, size_t nitems, size_t seek);
        size_t   (*f_read   )(fsd_t fsd, fd_t fd, void *dst, size_t size, size_t nitems, size_t seek);
        stdret_t (*f_ioctl  )(fsd_t fsd, fd_t fd, iorq_t iroq, void *data);
        stdret_t (*f_fstat  )(fsd_t fsd, fd_t fd, struct vfs_stat *stat);
        stdret_t (*f_mkdir  )(fsd_t fsd, const char *path);
        stdret_t (*f_mknod  )(fsd_t fsd, const char *path, struct vfs_drvcfg *dcfg);
        stdret_t (*f_opendir)(fsd_t fsd, const char *path, dir_t *dir);
        stdret_t (*f_remove )(fsd_t fsd, const char *path);
        stdret_t (*f_rename )(fsd_t fsd, const char *oldName, const char *newName);
        stdret_t (*f_chmod  )(fsd_t fsd, const char *path, u32_t mode);
        stdret_t (*f_chown  )(fsd_t fsd, const char *path, u16_t owner, u16_t group);
        stdret_t (*f_stat   )(fsd_t fsd, const char *path, struct vfs_stat *stat);
        stdret_t (*f_statfs )(fsd_t fsd, struct vfs_statfs *statfs);
        stdret_t (*f_release)(fsd_t fsd);
};

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern stdret_t vfs_init(void);
extern stdret_t vfs_mount(const char *srcPath, const char *mntPoint, struct vfs_fscfg *mountcfg);
extern stdret_t vfs_umount(const char *path);
extern stdret_t vfs_getmntentry(size_t item, struct vfs_mntent *mntent);
extern stdret_t vfs_mknod(const char *path, struct vfs_drvcfg *drvcfg);
extern stdret_t vfs_mkdir(const char *path);
extern dir_t   *vfs_opendir(const char *path);
extern stdret_t vfs_closedir(dir_t *dir);
extern dirent_t vfs_readdir(dir_t *dir);
extern stdret_t vfs_remove(const char *path);
extern stdret_t vfs_rename(const char *oldName, const char *newName);
extern stdret_t vfs_chmod(const char *path, u16_t mode);
extern stdret_t vfs_chown(const char *path, u16_t owner, u16_t group);
extern stdret_t vfs_stat(const char *path, struct vfs_stat *stat);
extern stdret_t vfs_statfs(const char *path, struct vfs_statfs *statfs);
extern file_t  *vfs_fopen(const char *path, const char *mode);
extern stdret_t vfs_fclose(file_t *file);
extern size_t   vfs_fwrite(void *ptr, size_t size, size_t nitems, file_t *file);
extern size_t   vfs_fread(void *ptr, size_t size, size_t nitems, file_t *file);
extern stdret_t vfs_fseek(file_t *file, i32_t offset, int mode);
extern i32_t    vfs_ftell(file_t *file);
extern stdret_t vfs_ioctl(file_t *file, iorq_t rq, void *data);
extern stdret_t vfs_fstat(file_t *file, struct vfs_stat *stat);

#ifdef __cplusplus
}
#endif

#endif /* VFS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
