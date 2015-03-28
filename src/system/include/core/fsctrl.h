/*=========================================================================*//**
@file    fsctrl.h

@author  Daniel Zorychta

@brief   Drivers support.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _FSCTRL_
#define _FSCTRL_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "core/vfs.h"

/*==============================================================================
  Exported macros
==============================================================================*/
#define _FILE_SYSTEM_INTERFACE(fs_name)\
{.FS_name = #fs_name,\
 .FS_if   = {.fs_init    = _##fs_name##_init,\
             .fs_chmod   = _##fs_name##_chmod,\
             .fs_chown   = _##fs_name##_chown,\
             .fs_close   = _##fs_name##_close,\
             .fs_ioctl   = _##fs_name##_ioctl,\
             .fs_mkdir   = _##fs_name##_mkdir,\
             .fs_mkfifo  = _##fs_name##_mkfifo,\
             .fs_mknod   = _##fs_name##_mknod,\
             .fs_open    = _##fs_name##_open,\
             .fs_opendir = _##fs_name##_opendir,\
             .fs_read    = _##fs_name##_read,\
             .fs_release = _##fs_name##_release,\
             .fs_remove  = _##fs_name##_remove,\
             .fs_rename  = _##fs_name##_rename,\
             .fs_stat    = _##fs_name##_stat,\
             .fs_fstat   = _##fs_name##_fstat,\
             .fs_statfs  = _##fs_name##_statfs,\
             .fs_flush   = _##fs_name##_flush,\
             .fs_write   = _##fs_name##_write,\
             .fs_sync    = _##fs_name##_sync,\
             .fs_magic   = _VFS_FILE_SYSTEM_MAGIC_NO}}

#define _IMPORT_FILE_SYSTEM(fsname)                                                             \
extern API_FS_INIT(fsname, void**, const char*);                                                \
extern API_FS_RELEASE(fsname, void*);                                                           \
extern API_FS_OPEN(fsname, void*, void**, fd_t*, fpos_t*, const char*, vfs_open_flags_t);       \
extern API_FS_CLOSE(fsname, void*, void*, fd_t, bool);                                          \
extern API_FS_WRITE(fsname, void*, void*, fd_t, const u8_t*, size_t, fpos_t*, struct vfs_fattr);\
extern API_FS_READ(fsname, void*, void*, fd_t, u8_t*, size_t, fpos_t*, struct vfs_fattr);       \
extern API_FS_IOCTL(fsname, void*, void*, fd_t, int, void*);                                    \
extern API_FS_FSTAT(fsname, void*, void*, fd_t, struct stat*);                                  \
extern API_FS_FLUSH(fsname, void*, void*, fd_t);                                                \
extern API_FS_MKDIR(fsname, void*, const char*, mode_t);                                        \
extern API_FS_MKFIFO(fsname, void*, const char*, mode_t);                                       \
extern API_FS_MKNOD(fsname, void*, const char*, const dev_t);                                   \
extern API_FS_OPENDIR(fsname, void*, const char*, struct vfs_dir*);                             \
extern API_FS_REMOVE(fsname, void*, const char*);                                               \
extern API_FS_RENAME(fsname, void*, const char*, const char*);                                  \
extern API_FS_CHMOD(fsname, void*, const char*, mode_t);                                        \
extern API_FS_CHOWN(fsname, void*, const char*, uid_t, gid_t);                                  \
extern API_FS_STAT(fsname, void*, const char*, struct stat*);                                   \
extern API_FS_STATFS(fsname, void*, struct statfs*);                                            \
extern API_FS_SYNC(fsname, void*)

/*==============================================================================
  Exported object types
==============================================================================*/
struct _FS_entry {
      const char                    *FS_name;
      const struct vfs_FS_interface  FS_if;
};

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern int _mount (const char*, const char*, const char*);
extern int _umount(const char*);

#ifdef __cplusplus
}
#endif

#endif /* _FSCTRL_ */
/*==============================================================================
  End of file
==============================================================================*/
