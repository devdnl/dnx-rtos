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
typedef enum {                          // | RETURN TYPE    | ARG 1                     | ARG 2                     | ARG 3                     | ARG 4                     |
        SYSCALL_MOUNT,                  // | int            | const char *FS_name       | const char *src_path      | const char *mount_point   |                           |
        SYSCALL_UMOUNT,                 // | int            | const char *mount_point   |                           |                           |                           |
        SYSCALL_GETMNTENTRY,            // | int            | int *seek                 | struct mntent *mntent     |                           |                           |
        SYSCALL_MKNOD,                  // | int            | const char *pathname      | dev_t *device             |                           |                           |
        SYSCALL_MKDIR,                  // | int            | const char *pathname      | mode_t *mode              |                           |                           |
        SYSCALL_MKFIFO,                 // | int            | const char *pathname      | mode_t *mode              |                           |                           |
        SYSCALL_OPENDIR,                // | DIR*           | const char *pathname      |                           |                           |                           |
        SYSCALL_CLOSEDIR,               // | int            | DIR *dir                  |                           |                           |                           |
        SYSCALL_READDIR,                // | dirent_t*      | DIR *dir                  |                           |                           |                           |
        SYSCALL_REMOVE,                 // | int            | const char *path          |                           |                           |                           |
        SYSCALL_RENAME,                 // | int            | const char *old_name      | const char *new_name      |                           |                           |
        SYSCALL_CHMOD,                  // | int            | const char *pathname      | mode_t *mode              |                           |                           |
        SYSCALL_CHOWN,                  // | int            | const char *pathname      | uid_t *owner              | gid_t *group              |                           |
        SYSCALL_STAT,                   // | int            | const char *pathname      | struct stat *buf          |                           |                           |
        SYSCALL_STATFS,                 // | int            | const char *path          | struct statfs *buf        |                           |                           |
        SYSCALL_FOPEN,                  // | FILE*          | const char *path          | const char *mode          |                           |                           |
        SYSCALL_FCLOSE,                 // | int            | FILE *file                |                           |                           |                           |
        SYSCALL_FWRITE,                 // | size_t         | const void *src           | size_t *size              | size_t *count             | FILE *file                |
        SYSCALL_FREAD,                  // | size_t         | void *dst                 | size_t *size              | size_t *count             | FILE *file                |
        SYSCALL_FSEEK,                  // | int            | FILE *file                | i64_t  *seek              | int    *origin            |                           |
        SYSCALL_FTELL,                  // | i64_t          | FILE *file                |                           |                           |                           |
        SYSCALL_IOCTL,                  // | int            | FILE *file                | int *request              | va_list *arg              |                           |
        SYSCALL_FSTAT,                  // | int            | FILE *file                | struct stat *buf          |                           |                           |
        SYSCALL_FFLUSH,                 // | int            | FILE *file                |                           |                           |                           |
        SYSCALL_FEOF,                   // | int            | FILE *file                |                           |                           |                           |
        SYSCALL_CLEARERROR,             // | void           | FILE *file                |                           |                           |                           |
        SYSCALL_FERROR,                 // | int            | FILE *file                |                           |                           |                           |
        SYSCALL_SYNC,                   // | void           |                           |                           |                           |                           |
        SYSCALL_GETTIME,                // | time_t         |                           |                           |                           |                           |
        SYSCALL_SETTIME,                // | int            | time_t *time              |                           |                           |                           |
        SYSCALL_DRIVERINIT,             // | int            | const char *drv_name      | const char *node_path     |                           |                           |
        SYSCALL_DRIVERRELEASE,          // | int            | const char *drv_name      |                           |                           |                           |
        SYSCALL_MALLOC,                 // | void*          | size_t *size              |                           |                           |                           |
        SYSCALL_ZALLOC,                 // | void*          | size_t *size              |                           |                           |                           |
        SYSCALL_FREE,                   // | void           | void *mem                 |                           |                           |                           |
        SYSCALL_SYSLOGENABLE,           // | int            | const char *pathname      |                           |                           |                           |
        SYSCALL_SYSLOGDISABLE,          // | int            |                           |                           |                           |                           |
        SYSCALL_RESTART,                // | void           |                           |                           |                           |                           |
        SYSCALL_KERNELPANICDETECT,      // | bool           | bool *showmsg             |                           |                           |                           |
        SYSCALL_ABORT,                  // | void           |                           |                           |                           |                           |
        SYSCALL_EXIT,                   // | void           | int *status               |                           |                           |                           |
        SYSCALL_SYSTEM,                 // | int            | const char *command       |                           |                           |                           |
        SYSCALL_PROCESSCREATE,          // | pid_t          | const char *command       | process_attr_t *attr      |                           |                           |
        SYSCALL_PROCESSDESTROY,         // | int            | pid_t *pid                | int *status               |                           |                           |
        SYSCALL_PROCESSSTATSEEK,        // | int            | size_t *seek              | process_stat_t *stat      |                           |                           |
        SYSCALL_PROCESSSTATPID,         // | int            | pid_t *pid                | process_stat_t *stat      |                           |                           |
        SYSCALL_PROCESSGETPID,          // | pid_t          |                           |                           |                           |                           |
        SYSCALL_GETCWD,                 // | char*          | char *buf                 | size_t *size              |                           |                           |
        _SYSCALL_COUNT
} syscall_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern void syscall(syscall_t syscall, void *retptr, ...);
extern void _syscall_init();
extern int  _syscall_kworker_master(int, char**);

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
