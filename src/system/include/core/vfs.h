/*=========================================================================*//**
@file    vfs.h

@author  Daniel Zorychta

@brief   This file support virtual file system

@note    Copyright (C) 2012, 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _VFS_H_
#define _VFS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
#include "kernel/ktypes.h"
#include "core/ioctl_macros.h"

/*==============================================================================
  Exported macros
==============================================================================*/
/* set position equal to offset bytes */
#define VFS_SEEK_SET                            0

/* set position to current location plus offset */
#define VFS_SEEK_CUR                            1

/* set position to EOF plus offset */
#define VFS_SEEK_END                            2

/* translate functions to STDC */
#ifndef SEEK_SET
#define SEEK_SET                                VFS_SEEK_SET
#endif

#ifndef SEEK_CUR
#define SEEK_CUR                                VFS_SEEK_CUR
#endif

#ifndef SEEK_END
#define SEEK_END                                VFS_SEEK_END
#endif

/* modes */
#define S_IRUSR                                 (4 << 0)
#define S_IWUSR                                 (2 << 0)
#define S_IXUSR                                 (1 << 0)
#define S_IRGRO                                 (4 << 3)
#define S_IWGRO                                 (2 << 3)
#define S_IXGRO                                 (1 << 3)
#define S_IROTH                                 (4 << 6)
#define S_IWOTH                                 (2 << 6)
#define S_IXOTH                                 (1 << 6)

/* stream definitions */
#define EOF                                     (-1)
#define ETX                                     0x03
#define EOT                                     0x04

/* IO operations on files */
#define IOCTL_PIPE__CLOSE                       _IO(_IO_GROUP_PIPE, 0x00)
#define IOCTL_VFS__NON_BLOCKING_RD_MODE         _IO(_IO_GROUP_VFS,  0x01)
#define IOCTL_VFS__DEFAULT_RD_MODE              _IO(_IO_GROUP_VFS,  0x02)
#define IOCTL_VFS__NON_BLOCKING_WR_MODE         _IO(_IO_GROUP_VFS,  0x03)
#define IOCTL_VFS__DEFAULT_WR_MODE              _IO(_IO_GROUP_VFS,  0x04)

/*==============================================================================
  Exported object types
==============================================================================*/
/** file object */
typedef struct vfs_file FILE;

/** file type */
typedef enum tfile {
        FILE_TYPE_REGULAR,
        FILE_TYPE_DIR,
        FILE_TYPE_DRV,
        FILE_TYPE_LINK,
        FILE_TYPE_PROGRAM,
        FILE_TYPE_PIPE
} tfile_t;

/** directory entry */
typedef struct dirent {
        char   *name;
        u64_t   size;
        tfile_t filetype;
        dev_t   dev;
} dirent_t;

/** directory type */
struct vfs_dir {
        dirent_t (*f_readdir)(void *fshdl, struct vfs_dir *dir);
        stdret_t (*f_closedir)(void *fshdl, struct vfs_dir *dir);
        void      *f_dd;
        void      *f_handle;
        size_t     f_items;
        size_t     f_seek;
        u32_t      validation;          /**< only for system purposes */
};

typedef struct vfs_dir DIR;

/** file statistics */
struct stat {
        u64_t   st_size;                /**< total size, in bytes         */
        u32_t   st_dev;                 /**< ID of device containing file */
        u32_t   st_mode;                /**< protection                   */
        u32_t   st_uid;                 /**< user ID of owner             */
        u32_t   st_gid;                 /**< group ID of owner            */
        u32_t   st_atime;               /**< time of last access          */
        u32_t   st_mtime;               /**< time of last modification    */
        tfile_t st_type;                /**< type of file                 */
};

/** device info */
struct vfs_dev_stat {
        u64_t st_size;                  /**< total size, in bytes */
        u8_t  st_major;                 /**< device major number  */
        u8_t  st_minor;                 /**< device minor number  */
};

/** file system statistic */
struct statfs {
        u32_t f_type;                   /**< file system type       */
        u32_t f_bsize;                  /**< block size             */
        u32_t f_blocks;                 /**< total blocks           */
        u32_t f_bfree;                  /**< free blocks            */
        u32_t f_files;                  /**< total file nodes in FS */
        u32_t f_ffree;                  /**< free file nodes in FS  */
        const char *f_fsname;           /**< FS name                */
};

/** structure describing a mount table entry */
struct mntent {
        const char *mnt_fsname;         /**< device or server for filesystem */
        const char *mnt_dir;            /**< directory mounted on            */
        u64_t       total;              /**< device total size               */
        u64_t       free;               /**< device free                     */
};

/** file write/read attributtes */
struct vfs_fattr {
        bool non_blocking_rd:1;         /**< non-blocking file read access */
        bool non_blocking_wr:1;         /**< non-blocking file write access */
};

/** file open flags */
typedef enum {
        O_RDONLY = (1 << 0),            /**< read only                    */
        O_WRONLY = (1 << 1),            /**< write only                   */
        O_RDWR   = (1 << 2),            /**< read write                   */
        O_CREATE = (1 << 3),            /**< create file (not for device) */
        O_APPEND = (1 << 4)             /**< append data (not for device) */
} vfs_open_flags_t;

/** file system configuration */
struct vfs_FS_interface {
        stdret_t (*fs_init   )(void **fshdl, const char *path);
        stdret_t (*fs_release)(void *fshdl);
        stdret_t (*fs_open   )(void *fshdl, void **extra_data, fd_t *fd, fpos_t *fpos, const char *path, vfs_open_flags_t flags);
        stdret_t (*fs_close  )(void *fshdl, void  *extra_data, fd_t fd, bool force);
        ssize_t  (*fs_write  )(void *fshdl, void  *extra_data, fd_t fd, const u8_t *src, size_t count, fpos_t *fpos, struct vfs_fattr attr);
        ssize_t  (*fs_read   )(void *fshdl, void  *extra_data, fd_t fd, u8_t *dst, size_t count, fpos_t *fpos, struct vfs_fattr attr);
        stdret_t (*fs_ioctl  )(void *fshdl, void  *extra_data, fd_t fd, int iroq, void *args);
        stdret_t (*fs_fstat  )(void *fshdl, void  *extra_data, fd_t fd, struct stat *stat);
        stdret_t (*fs_flush  )(void *fshdl, void  *extra_data, fd_t fd);
        stdret_t (*fs_mkdir  )(void *fshdl, const char *path, mode_t);
        stdret_t (*fs_mkfifo )(void *fshdl, const char *path, mode_t);
        stdret_t (*fs_mknod  )(void *fshdl, const char *path, const dev_t dev);
        stdret_t (*fs_opendir)(void *fshdl, const char *path, DIR *dir);
        stdret_t (*fs_remove )(void *fshdl, const char *path);
        stdret_t (*fs_rename )(void *fshdl, const char *old_name, const char *new_name);
        stdret_t (*fs_chmod  )(void *fshdl, const char *path, mode_t mode);
        stdret_t (*fs_chown  )(void *fshdl, const char *path, uid_t owner, gid_t group);
        stdret_t (*fs_stat   )(void *fshdl, const char *path, struct stat *stat);
        stdret_t (*fs_statfs )(void *fshdl, struct statfs *statfs);
        void     (*fs_sync   )(void *fshdl);
};

/*==============================================================================
  Exported API functions
==============================================================================*/
extern stdret_t         vfs_init                (void);
extern stdret_t         vfs_mount               (const char*, const char*, struct vfs_FS_interface*);
extern stdret_t         vfs_umount              (const char*);
extern int              vfs_getmntentry         (int, struct mntent*);
extern int              vfs_mknod               (const char*, dev_t);
extern int              vfs_mkdir               (const char*, mode_t);
extern int              vfs_mkfifo              (const char*, mode_t);
extern DIR             *vfs_opendir             (const char*);
extern int              vfs_closedir            (DIR*);
extern dirent_t         vfs_readdir             (DIR*);
extern int              vfs_remove              (const char*);
extern int              vfs_rename              (const char*, const char*);
extern int              vfs_chmod               (const char*, int);
extern int              vfs_chown               (const char*, int, int);
extern int              vfs_stat                (const char*, struct stat*);
extern int              vfs_statfs              (const char*, struct statfs*);
extern FILE            *vfs_fopen               (const char*, const char*);
extern FILE            *vfs_freopen             (const char*, const char*, FILE*);
extern int              vfs_fclose              (FILE*);
extern int              vfs_fclose_force        (FILE*);
extern size_t           vfs_fwrite              (const void*, size_t, size_t, FILE*);
extern size_t           vfs_fread               (void*, size_t, size_t, FILE*);
extern int              vfs_fseek               (FILE*, i64_t, int);
extern i64_t            vfs_ftell               (FILE*);
extern int              vfs_ioctl               (FILE*, int, ...);
extern int              vfs_vioctl              (FILE*, int, va_list arg);
extern int              vfs_fstat               (FILE*, struct stat*);
extern int              vfs_fflush              (FILE*);
extern int              vfs_feof                (FILE*);
extern void             vfs_clearerr            (FILE*);
extern int              vfs_ferror              (FILE*);
extern int              vfs_rewind              (FILE*);
extern void             vfs_sync                (void);

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief vfs_filter_open_flags_for_device
 * Function filter open flags for device.
 *
 * @param flags	flags to filter
 *
 * @errors None
 *
 * @return Filtered flags
 */
//==============================================================================
static inline vfs_open_flags_t vfs_filter_open_flags_for_device(vfs_open_flags_t flags)
{
        return flags & (O_RDONLY | O_WRONLY | O_RDWR);
}

#ifdef __cplusplus
}
#endif

#endif /* _VFS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
