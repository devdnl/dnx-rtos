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
typedef enum {// NAME                      | RETURN TYPE    | ARG 1                     | ARG 2                     | ARG 3                     | ARG 4                     |
        SYSCALL_FTELL,                  // | i64_t          | FILE *file                |                           |                           |                           |
        SYSCALL_FEOF,                   // | int            | FILE *file                |                           |                           |                           |
        SYSCALL_FERROR,                 // | int            | FILE *file                |                           |                           |                           |
        SYSCALL_CLEARERROR,             // | void           | FILE *file                |                           |                           |                           |
        SYSCALL_MALLOC,                 // | void*          | size_t *size              |                           |                           |                           |
        SYSCALL_ZALLOC,                 // | void*          | size_t *size              |                           |                           |                           |
        SYSCALL_FREE,                   // | void           | void *mem                 |                           |                           |                           |
        SYSCALL_RESTART,                // | void           |                           |                           |                           |                           |
        SYSCALL_PROCESSGETEXITSEM,      // | int            | pid_t *pid                | sem_t **sem               |                           |                           |
        SYSCALL_PROCESSSTATSEEK,        // | int            | size_t *seek              | process_stat_t *stat      |                           |                           |
        SYSCALL_PROCESSSTATPID,         // | int            | pid_t *pid                | process_stat_t *stat      |                           |                           |
        SYSCALL_PROCESSGETPID,          // | pid_t          |                           |                           |                           |                           |
        SYSCALL_PROCESSGETPRIO,         // | int            | pid_t *pid                |                           |                           |                           |
        SYSCALL_GETCWD,                 // | char*          | char *buf                 | size_t *size              |                           |                           |
        SYSCALL_THREADCREATE,           // | tid_t          | thread_func_t             | thread_attr_t *attr       | void *arg                 |                           |
        SYSCALL_THREADEXIT,             // | void           | tid_t *tid                |                           |                           |                           |
        SYSCALL_THREADGETEXITSEM,       // | int            | tid_t *tid                | sem_t **sem               |                           |                           |
        SYSCALL_SEMAPHORECREATE,        // | sem_t*         | const size_t *cnt_max     | const size_t *cnt_init    |                           |                           |
        SYSCALL_SEMAPHOREDESTROY,       // | void           | sem_t *semaphore          |                           |                           |                           |
        SYSCALL_MUTEXCREATE,            // | mutex_t*       | const enum mutex_type *tp |                           |                           |                           |
        SYSCALL_MUTEXDESTROY,           // | void           | mutex_t *mutex            |                           |                           |                           |
        SYSCALL_QUEUECREATE,            // | queue_t*       | const size_t *length      | const size_t *item_size   |                           |                           |
        SYSCALL_QUEUEDESTROY,           // | void           | queue_t *queue            |                           |                           |                           |
#define _SYSCALL_GROUP_0_OS_NON_BLOCKING  SYSCALL_QUEUEDESTROY    /* this group ends at this syscall */
        SYSCALL_KERNELPANICDETECT,      // | bool           | bool *showmsg             |                           |                           |                           |
        SYSCALL_ABORT,                  // | void           |                           |                           |                           |                           |
        SYSCALL_EXIT,                   // | void           | int *status               |                           |                           |                           |
        SYSCALL_THREADDESTROY,          // | int            | tid_t *tid                |                           |                           |                           |
        SYSCALL_PROCESSCREATE,          // | pid_t          | const char *command       | process_attr_t *attr      |                           |                           |
        SYSCALL_PROCESSDESTROY,         // | int            | pid_t *pid                | int *status               |                           |                           |
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
        SYSCALL_IOCTL,                  // | int            | FILE *file                | int *request              | va_list *arg              |                           |
        SYSCALL_FSTAT,                  // | int            | FILE *file                | struct stat *buf          |                           |                           |
        SYSCALL_FFLUSH,                 // | int            | FILE *file                |                           |                           |                           |
        SYSCALL_SYNC,                   // | void           |                           |                           |                           |                           |
        SYSCALL_GETTIME,                // | time_t         |                           |                           |                           |                           |
        SYSCALL_SETTIME,                // | int            | time_t *time              |                           |                           |                           |
        SYSCALL_DRIVERINIT,             // | int            | const char *drv_name      | const char *node_path     |                           |                           |
        SYSCALL_DRIVERRELEASE,          // | int            | const char *drv_name      |                           |                           |                           |
        SYSCALL_SYSLOGENABLE,           // | int            | const char *pathname      |                           |                           |                           |
        SYSCALL_SYSLOGDISABLE,          // | int            |                           |                           |                           |                           |
        SYSCALL_SYSTEM,                 // | int            | const char *command       | pid_t *pid                | sem_t **exit_sem          |                           |
#define _SYSCALL_GROUP_1_FS_BLOCKING      SYSCALL_SYSTEM          /* this group ends at this syscall */
        _SYSCALL_COUNT
#define _SYSCALL_GROUP_2_NET_BLOCKING     _SYSCALL_COUNT          /* not used group */
} syscall_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern void syscall(syscall_t syscall, void *retptr, ...);
extern void _syscall_init();
extern int  _syscall_kworker_process(int, char**);

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
