/*=========================================================================*//**
@file    syscall.h

@author  Daniel Zorychta

@brief   System call handling

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _KRNSPACE_SYSCALL_H_
#define _KRNSPACE_SYSCALL_H_

/*==============================================================================
  Include files
==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
typedef enum {
        SYSCALL_MOUNT,                  // int: const char *FS_name, const char *src_path, const char *mount_point
        SYSCALL_UMOUNT,                 // int: const char *mount_point
        SYSCALL_GETMNTENTRY,
        SYSCALL_MKNOD,
        SYSCALL_MKDIR,
        SYSCALL_MKFIFO,
        SYSCALL_OPENDIR,
        SYSCALL_CLOSEDIR,
        SYSCALL_READDIR,
        SYSCALL_REMOVE,
        SYSCALL_RENAME,
        SYSCALL_CHMOD,
        SYSCALL_CHOWN,
        SYSCALL_STAT,
        SYSCALL_STATFS,
        SYSCALL_FOPEN,                  // FILE*: const char *path, const char *mode
        SYSCALL_FREOPEN,
        SYSCALL_FCLOSE,                 // int: FILE*)
        SYSCALL_FWRITE,                 // size_t: const void *src, size_t size, size_t count, FILE *file
        SYSCALL_FREAD,                  // size_t: void *dst, size_t size, size_t count, FILE *file
        SYSCALL_FSEEK,
        SYSCALL_FTELL,
        SYSCALL_IOCTL,
        SYSCALL_FSTAT,
        SYSCALL_FFLUSH,
        SYSCALL_FEOF,
        SYSCALL_CLEARERROR,
        SYSCALL_FERROR,
        SYSCALL_REWIND,
        SYSCALL_SYNC,
        SYSCALL_TIME,
        SYSCALL_STIME,
        SYSCALL_DRIVERINIT,
        SYSCALL_DRIVERRELEASE,
        SYSCALL_ALLOC,
        SYSCALL_FREE,
        _SYSCALL_COUNT
} syscall_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern void _syscall_init();

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _KRNSPACE_SYSCALL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
