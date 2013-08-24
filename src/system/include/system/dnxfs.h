#ifndef _DNXFS_H_
#define _DNXFS_H_
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

#define FILE_SYSTEM_INTERFACE(fsname)                                                    \
extern stdret_t fsname##_init   (void**, const char*);                                   \
extern stdret_t fsname##_release(void*);                                                 \
extern stdret_t fsname##_open   (void*, void**, fd_t*, u64_t*, const char*, int);        \
extern stdret_t fsname##_close  (void*, void*, fd_t, bool);                              \
extern size_t   fsname##_write  (void*, void*, fd_t, const void*, size_t, size_t, u64_t);\
extern size_t   fsname##_read   (void*, void*, fd_t, void*, size_t, size_t, u64_t);      \
extern stdret_t fsname##_ioctl  (void*, void*, fd_t, int, va_list);                      \
extern stdret_t fsname##_fstat  (void*, void*, fd_t, struct vfs_stat*);                  \
extern stdret_t fsname##_flush  (void*, void*, fd_t);                                    \
extern stdret_t fsname##_mkdir  (void*, const char*);                                    \
extern stdret_t fsname##_mknod  (void*, const char*, struct vfs_drv_interface*);         \
extern stdret_t fsname##_opendir(void*, const char*, dir_t*);                            \
extern stdret_t fsname##_remove (void*, const char*);                                    \
extern stdret_t fsname##_rename (void*, const char*, const char*);                       \
extern stdret_t fsname##_chmod  (void*, const char*, int);                               \
extern stdret_t fsname##_chown  (void*, const char*, int, int);                          \
extern stdret_t fsname##_stat   (void*, const char*, struct vfs_stat*);                  \
extern stdret_t fsname##_statfs (void*, struct vfs_statfs*)

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _DNXFS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
