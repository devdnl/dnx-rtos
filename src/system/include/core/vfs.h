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
#include <stdarg.h>
#include "systypes.h"

/*==============================================================================
  Exported macros
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
  Exported object types
==============================================================================*/
/** file object */
typedef struct vfs_file FILE;

/** file descriptor */
typedef uint fd_t;

/** file type */
typedef enum tfile
{
        FILE_TYPE_REGULAR,
        FILE_TYPE_DIR,
        FILE_TYPE_DRV,
        FILE_TYPE_LINK
} tfile_t;

/** directory entry */
typedef struct dirent
{
        char   *name;
        size_t  size;
        tfile_t filetype;
} dirent_t;

/** directory type */
struct vfs_dir
{
        dirent_t (*rddir)(void *fshdl, struct vfs_dir *dir);
        stdret_t (*cldir)(void *fshdl, struct vfs_dir *dir);
        void      *dd;
        void      *handle;
        size_t    items;
        size_t    seek;
};

/** file statistics */
struct vfs_stat {
        u32_t  st_dev;           /* ID of device containing file */
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
struct vfs_drv_interface {
        void     *handle;
        stdret_t (*drv_open )(void *drvhdl);
        stdret_t (*drv_close)(void *drvhdl);
        size_t   (*drv_write)(void *drvhdl, const void *src, size_t size, size_t nitems, size_t seek);
        size_t   (*drv_read )(void *drvhdl, void *dst, size_t size, size_t nitems, size_t seek);
        stdret_t (*drv_ioctl)(void *drvhdl, int iorq, va_list args);
        stdret_t (*drv_flush)(void *drvhdl);
};

/** file system configuration */
struct vfs_FS_interface {
        stdret_t (*fs_init   )(void **fshdl, const char *path);
        stdret_t (*fs_release)(void *fshdl);
        stdret_t (*fs_open   )(void *fshdl, fd_t *fd, size_t *seek, const char *path, const char *mode);
        stdret_t (*fs_close  )(void *fshdl, fd_t fd);
        size_t   (*fs_write  )(void *fshdl, fd_t fd, const void *src, size_t size, size_t nitems, size_t seek);
        size_t   (*fs_read   )(void *fshdl, fd_t fd, void *dst, size_t size, size_t nitems, size_t seek);
        stdret_t (*fs_ioctl  )(void *fshdl, fd_t fd, int iroq, va_list args);
        stdret_t (*fs_fstat  )(void *fshdl, fd_t fd, struct vfs_stat *stat);
        stdret_t (*fs_flush  )(void *fshdl, fd_t fd);
        stdret_t (*fs_mkdir  )(void *fshdl, const char *path);
        stdret_t (*fs_mknod  )(void *fshdl, const char *path, struct vfs_drv_interface *drv_if);
        stdret_t (*fs_opendir)(void *fshdl, const char *path, dir_t *dir);
        stdret_t (*fs_remove )(void *fshdl, const char *path);
        stdret_t (*fs_rename )(void *fshdl, const char *old_name, const char *new_name);
        stdret_t (*fs_chmod  )(void *fshdl, const char *path, int mode);
        stdret_t (*fs_chown  )(void *fshdl, const char *path, u16_t owner, u16_t group);
        stdret_t (*fs_stat   )(void *fshdl, const char *path, struct vfs_stat *stat);
        stdret_t (*fs_statfs )(void *fshdl, struct vfs_statfs *statfs);
};

/*==============================================================================
  Exported API functions
==============================================================================*/
extern stdret_t vfs_init(void);
extern stdret_t vfs_mount(const char*, const char*, struct vfs_FS_interface*);
extern stdret_t vfs_umount(const char*);
extern stdret_t vfs_getmntentry(size_t, struct vfs_mntent*);
extern int      vfs_mknod(const char*, struct vfs_drv_interface*);
extern int      vfs_mkdir(const char*);
extern dir_t   *vfs_opendir(const char*);
extern int      vfs_closedir(dir_t*);
extern dirent_t vfs_readdir(dir_t*);
extern int      vfs_remove(const char*);
extern int      vfs_rename(const char*, const char*);
extern int      vfs_chmod(const char*, int);
extern int      vfs_chown(const char*, u16_t, u16_t);
extern int      vfs_stat(const char*, struct vfs_stat*);
extern int      vfs_statfs(const char*, struct vfs_statfs*);
extern FILE    *vfs_fopen(const char*, const char*);
extern FILE    *vfs_freopen(const char*, const char*, FILE*);
extern int      vfs_fclose(FILE*);
extern size_t   vfs_fwrite(const void*, size_t, size_t, FILE*);
extern size_t   vfs_fread(void*, size_t, size_t, FILE*);
extern int      vfs_fseek(FILE*, i32_t, int);
extern i32_t    vfs_ftell(FILE*);
extern int      vfs_ioctl(FILE*, int, ...);
extern int      vfs_fstat(FILE*, struct vfs_stat*);
extern int      vfs_fflush(FILE*);
extern int      vfs_feof(FILE*);
#define         vfs_rewind(file) vfs_fseek(file, 0, 0)

#ifdef __cplusplus
}
#endif

#endif /* VFS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
