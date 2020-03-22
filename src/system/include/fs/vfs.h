/*=========================================================================*//**
@file    vfs.h

@author  Daniel Zorychta

@brief   This file support virtual file system

@note    Copyright (C) 2012, 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "drivers/ioctl_macros.h"

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

/* file access flags. Doxygen documentation in "fs/fs.h" */
#ifndef O_RDONLY
#define O_RDONLY                                00
#endif

#ifndef O_WRONLY
#define O_WRONLY                                01
#endif

#ifndef O_RDWR
#define O_RDWR                                  02
#endif

#ifndef O_CREAT
#define O_CREAT                                 0100
#endif

#ifndef O_EXCL
#define O_EXCL                                  0200
#endif

#ifndef O_TRUNC
#define O_TRUNC                                 01000
#endif

#ifndef O_APPEND
#define O_APPEND                                02000
#endif

/* modes */
#define S_IRWXU                                 0000700    /* RWX mask for owner */
#define S_IRUSR                                 0000400    /* R for owner */
#define S_IWUSR                                 0000200    /* W for owner */
#define S_IXUSR                                 0000100    /* X for owner */

#define S_IRWXG                                 0000070    /* RWX mask for group */
#define S_IRGRP                                 0000040    /* R for group */
#define S_IWGRP                                 0000020    /* W for group */
#define S_IXGRP                                 0000010    /* X for group */

#define S_IRWXO                                 0000007    /* RWX mask for other */
#define S_IROTH                                 0000004    /* R for other */
#define S_IWOTH                                 0000002    /* W for other */
#define S_IXOTH                                 0000001    /* X for other */

#define S_ISUID                                 0004000    /* set user id on execution */
#define S_ISGID                                 0002000    /* set group id on execution */
#define S_ISVTX                                 0001000    /* save swapped text even after use */

#define S_IPMT(mode_t_m)                        ((mode_t_m) & 000777)
#define S_IFMT(mode_t_m)                        ((mode_t_m) & 070000)

#define S_ISREG(mode_t_m)                       (S_IFMT(mode_t_m) == S_IFREG)
#define S_ISDIR(mode_t_m)                       (S_IFMT(mode_t_m) == S_IFDIR)
#define S_ISDEV(mode_t_m)                       (S_IFMT(mode_t_m) == S_IFDEV)
#define S_ISLNK(mode_t_m)                       (S_IFMT(mode_t_m) == S_IFLNK)
#define S_ISPROG(mode_t_m)                      (S_IFMT(mode_t_m) == S_IFPROG)
#define S_ISFIFO(mode_t_m)                      (S_IFMT(mode_t_m) == S_IFIFO)

#define S_IFREG                                 0000000
#define S_IFDIR                                 0010000
#define S_IFDEV                                 0020000
#define S_IFLNK                                 0030000
#define S_IFPROG                                0040000
#define S_IFIFO                                 0050000

/* stream definitions */
#define EOF                                     (-1)
#define ETX                                     0x03
#define EOT                                     0x04

/* IO operations on files */
#define IOCTL_PIPE__CLOSE                       _IO(PIPE, 0x00)
#define IOCTL_PIPE__CLEAR                       _IO(PIPE, 0x01)
#define IOCTL_PIPE__PERMANENT                   _IO(PIPE, 0x02)
#define IOCTL_VFS__NON_BLOCKING_RD_MODE         _IO(VFS,  0x00)
#define IOCTL_VFS__DEFAULT_RD_MODE              _IO(VFS,  0x01)
#define IOCTL_VFS__IS_NON_BLOCKING_RD_MODE      _IO(VFS,  0x02)
#define IOCTL_VFS__NON_BLOCKING_WR_MODE         _IO(VFS,  0x03)
#define IOCTL_VFS__DEFAULT_WR_MODE              _IO(VFS,  0x04)
#define IOCTL_VFS__IS_NON_BLOCKING_WR_MODE      _IO(VFS,  0x05)

/* file system identifier */
#define _VFS_FILE_SYSTEM_MAGIC_NO               0xD9EFD24F

/*==============================================================================
  Exported object types
==============================================================================*/
struct vfs_dir;
struct vfs_file;

/** device info. Doxygen documentation in drivers/driver.h */
struct vfs_dev_stat {
        u64_t st_size;                  /**< total size, in bytes */
        u8_t  st_major;                 /**< device major number  */
        u8_t  st_minor;                 /**< device minor number  */
};

/** file write/read attributes. Doxygen documentation in fs/fs.h */
struct vfs_fattr {
        bool non_blocking_rd:1;         /**< non-blocking file read access */
        bool non_blocking_wr:1;         /**< non-blocking file write access */
};

/** file system interface */
typedef struct vfs_FS_itf {
        int (*fs_init    )(void **fshdl, const char *path, const char *opts);
        int (*fs_release )(void *fshdl);
        int (*fs_open    )(void *fshdl, void **fhdl, fpos_t *fpos, const char *path, u32_t flags);
        int (*fs_close   )(void *fshdl, void  *fhdl, bool force);
        int (*fs_write   )(void *fshdl, void  *fhdl, const u8_t *src, size_t count, fpos_t *fpos, size_t *wrcnt, struct vfs_fattr attr);
        int (*fs_read    )(void *fshdl, void  *fhdl, u8_t *dst, size_t count, fpos_t *fpos, size_t *rdcnt, struct vfs_fattr attr);
        int (*fs_ioctl   )(void *fshdl, void  *fhdl, int iroq, void *arg);
        int (*fs_fstat   )(void *fshdl, void  *fhdl, struct stat *stat);
        int (*fs_flush   )(void *fshdl, void  *fhdl);
        int (*fs_mknod   )(void *fshdl, const char *path, const dev_t dev);
        int (*fs_sync    )(void *fshdl);
        int (*fs_opendir )(void *fshdl, const char *path, struct vfs_dir *dir);
        int (*fs_closedir)(void *fshdl, struct vfs_dir *dir);
        int (*fs_readdir )(void *fshdl, struct vfs_dir *dir);
    #if __OS_ENABLE_FSTAT__ == _YES_
        int (*fs_stat   )(void *fshdl, const char *path, struct stat *stat);
    #endif
    #if __OS_ENABLE_STATFS__ == _YES_
        int (*fs_statfs )(void *fshdl, struct statfs *statfs);
    #endif
    #if __OS_ENABLE_MKDIR__ == _YES_
        int (*fs_mkdir  )(void *fshdl, const char *path, mode_t);
    #endif
    #if __OS_ENABLE_MKFIFO__ == _YES_
        int (*fs_mkfifo )(void *fshdl, const char *path, mode_t);
    #endif
    #if __OS_ENABLE_REMOVE__ == _YES_
        int (*fs_remove )(void *fshdl, const char *path);
    #endif
    #if __OS_ENABLE_RENAME__ == _YES_
        int (*fs_rename )(void *fshdl, const char *old_name, const char *new_name);
    #endif
    #if __OS_ENABLE_CHMOD__ == _YES_
        int (*fs_chmod  )(void *fshdl, const char *path, mode_t mode);
    #endif
    #if __OS_ENABLE_CHOWN__ == _YES_
        int (*fs_chown  )(void *fshdl, const char *path, uid_t owner, gid_t group);
    #endif
        uint32_t fs_magic;
} vfs_FS_itf_t;

/** file flags */
typedef struct vfs_file_flags {
        bool                rd     :1;          //! file read mode
        bool                wr     :1;          //! file write mode
        bool                append :1;          //! file append mode (one of rd/wr is set)
        bool                eof    :1;          //! end of file
        bool                error  :1;          //! error occurred
        bool                seekmod:1;          //! file position modified
        struct vfs_fattr    fattr;
} vfs_file_flags_t;

/** file type */
struct vfs_file {
        res_header_t        header;
        void               *FS_hdl;
        const vfs_FS_itf_t *FS_if;
        void               *f_hdl;
        fpos_t              f_lseek;
        vfs_file_flags_t    f_flag;
};

typedef struct vfs_file FILE;
#define __FILE_TYPE_DEFINED__

/** directory type */
struct vfs_dir {
        res_header_t        header;
        void               *FS_hdl;
        const vfs_FS_itf_t *FS_if;
        void               *d_hdl;          //!< Directory descriptor
        size_t              d_items;        //!< number of items
        size_t              d_seek;         //!< seek
        dirent_t            dirent;         //!< directory entry data
};

typedef struct vfs_dir DIR;
#define __DIR_TYPE_DEFINED__

/** Path with CWD */
struct vfs_path {
        const char *CWD;
        const char *PATH;
};

/** Path slash correction */
enum path_correction {
        ADD_SLASH,
        SUB_SLASH,
        NO_SLASH_ACTION,
};

/*==============================================================================
  Exported API functions
==============================================================================*/
extern int  _vfs_init       (void);
extern int  _vfs_realpath   (char *path, enum path_correction corr);
extern int  _vfs_mount      (const struct vfs_path*, const struct vfs_path*, const struct vfs_FS_itf*, const char*);
extern int  _vfs_umount     (const struct vfs_path*);
extern int  _vfs_getmntentry(int, struct mntent*);
extern int  _vfs_mknod      (const struct vfs_path*, dev_t);
extern int  _vfs_mkdir      (const struct vfs_path*, mode_t);
extern int  _vfs_mkfifo     (const struct vfs_path*, mode_t);
extern int  _vfs_opendir    (const struct vfs_path*, DIR**);
extern int  _vfs_closedir   (DIR*);
extern int  _vfs_readdir    (DIR*, dirent_t**);
extern int  _vfs_seekdir    (DIR*, u32_t);
extern int  _vfs_telldir    (DIR*, u32_t*);
extern int  _vfs_remove     (const struct vfs_path*);
extern int  _vfs_rename     (const struct vfs_path*, const struct vfs_path*);
extern int  _vfs_chmod      (const struct vfs_path*, mode_t);
extern int  _vfs_chown      (const struct vfs_path*, uid_t, gid_t);
extern int  _vfs_stat       (const struct vfs_path*, struct stat*);
extern int  _vfs_statfs     (const struct vfs_path*, struct statfs*);
extern int  _vfs_fopen      (const struct vfs_path*, const char*, FILE**);
extern int  _vfs_fclose     (FILE*, bool);
extern int  _vfs_fwrite     (const void*, size_t, size_t*, FILE*);
extern int  _vfs_fread      (void*, size_t, size_t*, FILE*);
extern int  _vfs_fseek      (FILE*, i64_t, int);
extern int  _vfs_ftell      (FILE*, i64_t*);
extern int  _vfs_vfioctl    (FILE*, int, va_list);
extern int  _vfs_fstat      (FILE*, struct stat*);
extern int  _vfs_fflush     (FILE*);
extern int  _vfs_feof       (FILE*, int*);
extern int  _vfs_clearerr   (FILE*);
extern int  _vfs_ferror     (FILE*, int*);
extern void _vfs_sync       (void);

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
static inline u32_t vfs_filter_flags_for_device(u32_t flags)
{
        return (flags & (O_RDONLY | O_WRONLY | O_RDWR));
}

#ifdef __cplusplus
}
#endif

#endif /* _VFS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
