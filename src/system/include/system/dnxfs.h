/*=========================================================================*//**
@file    dnxfs.h

@author  Daniel Zorychta

@brief   This function provide all required function needed to write file systems.

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

#ifndef _DNXFS_H_
#define _DNXFS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "core/systypes.h"
#include "core/sysmoni.h"
#include "core/vfs.h"
#include "kernel/kwrapper.h"
#include <errno.h>

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
#ifdef _DNX_H_
#error "dnx.h and dnxfs.h cannot never included together!"
#endif

#undef  calloc
#define calloc(size_t__nmemb, size_t__msize)    sysm_syscalloc(size_t__nmemb, size_t__msize)

#undef  malloc
#define malloc(size_t__size)                    sysm_sysmalloc(size_t__size)

#undef  free
#define free(void__pmem)                        sysm_sysfree(void__pmem)

#define STOP_IF(condition)                      _stop_if(condition)

#define API_FS_INIT(fsname, ...)                stdret_t _##fsname##_init(__VA_ARGS__)
#define API_FS_RELEASE(fsname, ...)             stdret_t _##fsname##_release(__VA_ARGS__)
#define API_FS_OPEN(fsname, ...)                stdret_t _##fsname##_open(__VA_ARGS__)
#define API_FS_CLOSE(fsname, ...)               stdret_t _##fsname##_close(__VA_ARGS__)
#define API_FS_WRITE(fsname, ...)               size_t _##fsname##_write(__VA_ARGS__)
#define API_FS_READ(fsname, ...)                size_t _##fsname##_read(__VA_ARGS__)
#define API_FS_IOCTL(fsname, ...)               stdret_t _##fsname##_ioctl(__VA_ARGS__)
#define API_FS_FSTAT(fsname, ...)               stdret_t _##fsname##_fstat(__VA_ARGS__)
#define API_FS_FLUSH(fsname, ...)               stdret_t _##fsname##_flush(__VA_ARGS__)
#define API_FS_MKDIR(fsname, ...)               stdret_t _##fsname##_mkdir(__VA_ARGS__)
#define API_FS_MKNOD(fsname, ...)               stdret_t _##fsname##_mknod(__VA_ARGS__)
#define API_FS_OPENDIR(fsname, ...)             stdret_t _##fsname##_opendir(__VA_ARGS__)
#define API_FS_REMOVE(fsname, ...)              stdret_t _##fsname##_remove(__VA_ARGS__)
#define API_FS_RENAME(fsname, ...)              stdret_t _##fsname##_rename(__VA_ARGS__)
#define API_FS_CHMOD(fsname, ...)               stdret_t _##fsname##_chmod(__VA_ARGS__)
#define API_FS_CHOWN(fsname, ...)               stdret_t _##fsname##_chown(__VA_ARGS__)
#define API_FS_STAT(fsname, ...)                stdret_t _##fsname##_stat(__VA_ARGS__)
#define API_FS_STATFS(fsname, ...)              stdret_t _##fsname##_statfs(__VA_ARGS__)

#define _IMPORT_FILE_SYSTEM(fsname)                                                 \
extern API_FS_INIT(fsname, void**, const char*);                                    \
extern API_FS_RELEASE(fsname, void*);                                               \
extern API_FS_OPEN(fsname, void*, void**, fd_t*, u64_t*, const char*, int);         \
extern API_FS_CLOSE(fsname, void*, void*, fd_t, bool, const task_t*);               \
extern API_FS_WRITE(fsname, void*, void*, fd_t, const u8_t*, size_t, u64_t*);       \
extern API_FS_READ(fsname, void*, void*, fd_t, u8_t*, size_t, u64_t*);              \
extern API_FS_IOCTL(fsname, void*, void*, fd_t, int, void*);                        \
extern API_FS_FSTAT(fsname, void*, void*, fd_t, struct vfs_stat*);                  \
extern API_FS_FLUSH(fsname, void*, void*, fd_t);                                    \
extern API_FS_MKDIR(fsname, void*, const char*);                                    \
extern API_FS_MKNOD(fsname, void*, const char*, const struct vfs_drv_interface*);   \
extern API_FS_OPENDIR(fsname, void*, const char*, struct vfs_dir*);                 \
extern API_FS_REMOVE(fsname, void*, const char*);                                   \
extern API_FS_RENAME(fsname, void*, const char*, const char*);                      \
extern API_FS_CHMOD(fsname, void*, const char*, int);                               \
extern API_FS_CHOWN(fsname, void*, const char*, int, int);                          \
extern API_FS_STAT(fsname, void*, const char*, struct vfs_stat*);                   \
extern API_FS_STATFS(fsname, void*, struct vfs_statfs*)

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/

/*==============================================================================
  Exported inline function
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _DNXFS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
