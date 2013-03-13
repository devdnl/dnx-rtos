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

#ifndef calloc
#define calloc(nmemb, msize)              memman_calloc(nmemb, msize)
#endif

#ifndef malloc
#define malloc(size)                      memman_malloc(size)
#endif

#ifndef free
#define free(mem)                         memman_free(mem)
#endif

#define FILE_SYSTEM_INTERFACE(fsname)                                                 \
extern stdret_t  fsname##_init   (const char*, fsd_t*);                               \
extern stdret_t  fsname##_open   (fsd_t, fd_t*, size_t*, const char*, const char*);   \
extern stdret_t  fsname##_close  (fsd_t, fd_t);                                       \
extern size_t    fsname##_write  (fsd_t, fd_t, void*, size_t, size_t, size_t);        \
extern size_t    fsname##_read   (fsd_t, fd_t, void*, size_t, size_t, size_t);        \
extern stdret_t  fsname##_ioctl  (fsd_t, fd_t, iorq_t, void*);                        \
extern stdret_t  fsname##_mkdir  (fsd_t, const char*);                                \
extern stdret_t  fsname##_mknod  (fsd_t, const char*, struct vfs_drvcfg*);            \
extern stdret_t  fsname##_opendir(fsd_t, const char*, dir_t*);                        \
extern stdret_t  fsname##_remove (fsd_t, const char*);                                \
extern stdret_t  fsname##_rename (fsd_t, const char*, const char*);                   \
extern stdret_t  fsname##_chmod  (fsd_t, const char*, u32_t);                         \
extern stdret_t  fsname##_chown  (fsd_t, const char*, u16_t, u16_t);                  \
extern stdret_t  fsname##_stat   (fsd_t, const char*, struct vfs_stat*);              \
extern stdret_t  fsname##_fstat  (fsd_t, fd_t, struct vfs_stat*);                     \
extern stdret_t  fsname##_statfs (fsd_t, struct vfs_statfs*);                         \
extern stdret_t  fsname##_release(fsd_t)

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
