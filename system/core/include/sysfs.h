#ifndef SYSFS_H_
#define SYSFS_H_
/*=========================================================================*//**
@file    sysfs.h

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
#include "systypes.h"
#include "memman.h"
#include "oswrap.h"
#include "vfs.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
#ifdef SYSTEM_H_
#error "system.h and sysfs.h cannot never included together!"
#endif

#ifndef kcalloc
#define kcalloc(nmemb, msize)             memman_calloc(nmemb, msize, NULL)
#endif

#ifndef kmalloc
#define kmalloc(size)                     memman_malloc(size, NULL)
#endif

#ifndef kfree
#define kfree(mem)                        memman_free(mem)
#endif

#define FILE_SYSTEM_INTERFACE(fsname)                                                 \
extern stdret_t  fsname##_init   (void**, const char*);                               \
extern stdret_t  fsname##_release(void*);                                             \
extern stdret_t  fsname##_open   (void*, fd_t*, size_t*, const char*, const char*);   \
extern stdret_t  fsname##_close  (void*, fd_t);                                       \
extern size_t    fsname##_write  (void*, fd_t, void*, size_t, size_t, size_t);        \
extern size_t    fsname##_read   (void*, fd_t, void*, size_t, size_t, size_t);        \
extern stdret_t  fsname##_ioctl  (void*, fd_t, iorq_t, void*);                        \
extern stdret_t  fsname##_mkdir  (void*, const char*);                                \
extern stdret_t  fsname##_mknod  (void*, const char*, struct vfs_drv_interface*);     \
extern stdret_t  fsname##_opendir(void*, const char*, dir_t*);                        \
extern stdret_t  fsname##_remove (void*, const char*);                                \
extern stdret_t  fsname##_rename (void*, const char*, const char*);                   \
extern stdret_t  fsname##_chmod  (void*, const char*, u32_t);                         \
extern stdret_t  fsname##_chown  (void*, const char*, u16_t, u16_t);                  \
extern stdret_t  fsname##_stat   (void*, const char*, struct vfs_statf*);             \
extern stdret_t  fsname##_fstat  (void*, fd_t, struct vfs_statf*);                    \
extern stdret_t  fsname##_statfs (void*, struct vfs_statfs*)

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

#endif /* SYSFS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
