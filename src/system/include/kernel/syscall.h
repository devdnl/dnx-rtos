/*=========================================================================*//**
@file    syscall.h

@author  Daniel Zorychta

@brief   System call handling

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _KRNSPACE_SYSCALL_H_
#define _KRNSPACE_SYSCALL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <kernel/process.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
// SYSCALLS                                |----------------+---------------------------+-------------------------------------+---------------------------+---------------------------+-------------------------------------------+
typedef enum {// NAME                      | RETURN TYPE    | ARG 1                     | ARG 2                               | ARG 3                     | ARG 4                     | ARG 5                                     |
                                        // |----------------+---------------------------+-------------------------------------+---------------------------+---------------------------+-------------------------------------------+
        SYSCALL_MALLOC,                 // | void*          | size_t *size              |                                     |                           |                           |                                           |
        SYSCALL_ZALLOC,                 // | void*          | size_t *size              |                                     |                           |                           |                                           |
        SYSCALL_FREE,                   // | void           | void *mem                 |                                     |                           |                           |                                           |
    #if __OS_ENABLE_SHARED_MEMORY__ == _YES_
        SYSCALL_SHMCREATE,              // | int            | const char *key           | size_t *size                        |                           |                           |                                           |
        SYSCALL_SHMATTACH,              // | int            | const char *key           | void **mem                          | size_t *size              |                           |                                           |
        SYSCALL_SHMDETACH,              // | int            | const char *key           |                                     |                           |                           |                                           |
        SYSCALL_SHMDESTROY,             // | int            | const char *key           |                                     |                           |                           |                                           |
    #endif
        SYSCALL_PROCESSGETSYNCFLAG,     // | int            | pid_t *pid                | flag_t **obj                        |                           |                           |                                           |
        SYSCALL_PROCESSSTATSEEK,        // | int            | size_t *seek              | process_stat_t *stat                |                           |                           |                                           |
        SYSCALL_THREADSTAT,             // | int            | pid_t *pid                | tid_t *tid                          | thread_stat_t *stat       |                           |                                           |
        SYSCALL_PROCESSSTATPID,         // | int            | pid_t *pid                | process_stat_t *stat                |                           |                           |                                           |
        SYSCALL_PROCESSGETPID,          // | pid_t          |                           |                                     |                           |                           |                                           |
        SYSCALL_PROCESSGETPRIO,         // | int            | pid_t *pid                |                                     |                           |                           |                                           |
    #if __OS_ENABLE_GETCWD__ == _YES_
        SYSCALL_GETCWD,                 // | char*          | char *buf                 | size_t *size                        |                           |                           |                                           |
        SYSCALL_SETCWD,                 // | int            | const char *cwd           |                                     |                           |                           |                                           |
    #endif
    #if ((__OS_SYSTEM_MSG_ENABLE__ > 0) && (__OS_PRINTF_ENABLE__ > 0))
        SYSCALL_SYSLOGREAD,             // | size_t         | char *str                 | size_t *len                         | const struct timeval *from| struct timeval *current   |                                           |
    #endif
        SYSCALL_THREADCREATE,           // | tid_t          | thread_func_t             | thread_attr_t *attr                 | void *arg                 |                           |                                           |
        SYSCALL_SEMAPHORECREATE,        // | sem_t*         | const size_t *cnt_max     | const size_t *cnt_init              |                           |                           |                                           |
        SYSCALL_SEMAPHOREDESTROY,       // | void           | sem_t *semaphore          |                                     |                           |                           |                                           |
        SYSCALL_MUTEXCREATE,            // | mutex_t*       | const enum mutex_type *tp |                                     |                           |                           |                                           |
        SYSCALL_MUTEXDESTROY,           // | void           | mutex_t *mutex            |                                     |                           |                           |                                           |
        SYSCALL_QUEUECREATE,            // | queue_t*       | const size_t *length      | const size_t *item_size             |                           |                           |                                           |
        SYSCALL_QUEUEDESTROY,           // | void           | queue_t *queue            |                                     |                           |                           |                                           |
#define _SYSCALL_GROUP_0_OS_NON_BLOCKING  SYSCALL_QUEUEDESTROY // this group ends at ^this^ syscall --------------------------+---------------------------+---------------------------+-------------------------------------------+
        SYSCALL_THREADKILL,             // | int            | tid_t *tid                |                                     |                           |                           |                                           |
        SYSCALL_PROCESSCREATE,          // | pid_t          | const char *command       | process_attr_t *attr                |                           |                           |                                           |
        SYSCALL_PROCESSCLEANZOMBIE,     // | int            | pid_t *pid                | int *status                         |                           |                           |                                           |
        SYSCALL_PROCESSKILL,            // | int            | pid_t *pid                |                                     |                           |                           |                                           |
        SYSCALL_MOUNT,                  // | int            | const char *FS_name       | const char *src_path                | const char *mount_point   | const char *options       |                                           |
        SYSCALL_UMOUNT,                 // | int            | const char *mount_point   |                                     |                           |                           |                                           |
    #if __OS_ENABLE_MKNOD__ == _YES_
        SYSCALL_MKNOD,                  // | int            | const char *pathname      | const char *mod_name                | int *major                | int *minor                |                                           |
    #endif
    #if __OS_ENABLE_STATFS__ == _YES_
        SYSCALL_GETMNTENTRY,            // | int            | int *seek                 | struct mntent *mntent               |                           |                           |                                           |
    #endif
    #if __OS_ENABLE_MKFIFO__ == _YES_
        SYSCALL_MKFIFO,                 // | int            | const char *pathname      | mode_t *mode                        |                           |                           |                                           |
    #endif
    #if __OS_ENABLE_MKDIR__ == _YES_
        SYSCALL_MKDIR,                  // | int            | const char *pathname      | mode_t *mode                        |                           |                           |                                           |
    #endif
        SYSCALL_OPENDIR,                // | DIR*           | const char *pathname      |                                     |                           |                           |                                           |
        SYSCALL_CLOSEDIR,               // | int            | DIR *dir                  |                                     |                           |                           |                                           |
        SYSCALL_READDIR,                // | dirent_t*      | DIR *dir                  |                                     |                           |                           |                                           |
    #if __OS_ENABLE_REMOVE__ == _YES_
        SYSCALL_REMOVE,                 // | int            | const char *path          |                                     |                           |                           |                                           |
    #endif
    #if __OS_ENABLE_RENAME__ == _YES_
        SYSCALL_RENAME,                 // | int            | const char *old_name      | const char *new_name                |                           |                           |                                           |
    #endif
    #if __OS_ENABLE_CHMOD__ == _YES_
        SYSCALL_CHMOD,                  // | int            | const char *pathname      | mode_t *mode                        |                           |                           |                                           |
    #endif
    #if __OS_ENABLE_CHOWN__ == _YES_
        SYSCALL_CHOWN,                  // | int            | const char *pathname      | uid_t *owner                        | gid_t *group              |                           |                                           |
    #endif
    #if __OS_ENABLE_STATFS__ == _YES_
        SYSCALL_STATFS,                 // | int            | const char *path          | struct statfs *buf                  |                           |                           |                                           |
    #endif
    #if __OS_ENABLE_FSTAT__ == _YES_
        SYSCALL_STAT,                   // | int            | const char *pathname      | struct stat *buf                    |                           |                           |                                           |
    #endif
    #if __OS_ENABLE_FSTAT__ == _YES_
        SYSCALL_FSTAT,                  // | int            | FILE *file                | struct stat *buf                    |                           |                           |                                           |
    #endif
        SYSCALL_FOPEN,                  // | FILE*          | const char *path          | const char *mode                    |                           |                           |                                           |
        SYSCALL_FCLOSE,                 // | int            | FILE *file                |                                     |                           |                           |                                           |
        SYSCALL_FWRITE,                 // | size_t         | const void *src           | size_t *size                        | size_t *count             | FILE *file                |                                           |
        SYSCALL_FREAD,                  // | size_t         | void *dst                 | size_t *size                        | size_t *count             | FILE *file                |                                           |
        SYSCALL_FSEEK,                  // | int            | FILE *file                | i64_t  *seek                        | int    *origin            |                           |                                           |
        SYSCALL_IOCTL,                  // | int            | FILE *file                | int *request                        | va_list *arg              |                           |                                           |
        SYSCALL_FFLUSH,                 // | int            | FILE *file                |                                     |                           |                           |                                           |
        SYSCALL_SYNC,                   // | void           |                           |                                     |                           |                           |                                           |
    #if __OS_ENABLE_TIMEMAN__ == _YES_
        SYSCALL_GETTIME,                // | int            | struct timeval *          |                                     |                           |                           |                                           |
        SYSCALL_SETTIME,                // | int            | time_t *time              |                                     |                           |                           |                                           |
    #endif
        SYSCALL_DRIVERINIT,             // | dev_t          | const char *mod_name      | int *major                          | int *minor                | const char *node_path     | const void *config                        |
        SYSCALL_DRIVERRELEASE,          // | int            | const char *mod_name      | int *major                          | int *minor                |                           |                                           |
        SYSCALL_KERNELPANICDETECT,      // | bool           | FILE *file                |                                     |                           |                           |                                           |
    #if __ENABLE_NETWORK__ == _YES_
        SYSCALL_NETIFUP,                // | int            | NET_family_t *family      | const NET_generic_config_t *config  |                           |                           |                                           |
        SYSCALL_NETIFDOWN,              // | int            | NET_family_t *family      |                                     |                           |                           |                                           |
        SYSCALL_NETIFSTATUS,            // | int            | NET_family_t *family      | NET_generic_status_t *status        |                           |                           |                                           |
        SYSCALL_NETSOCKETCREATE,        // | SOCKET*        | NET_family_t *family      | NET_protocol_t *protocol            |                           |                           |                                           |
        SYSCALL_NETSOCKETDESTROY,       // | void           | SOCKET *socket            |                                     |                           |                           |                                           |
        SYSCALL_NETBIND,                // | int            | SOCKET *socket            | const NET_generic_sockaddr_t *addr  |                           |                           |                                           |
        SYSCALL_NETLISTEN,              // | int            | SOCKET *socket            |                                     |                           |                           |                                           |
        SYSCALL_NETACCEPT,              // | int            | SOCKET *socket            | SOCKET **new_socket                 |                           |                           |                                           |
        SYSCALL_NETRECV,                // | int            | SOCKET *socket            | void *buf                           | size_t *len               | NET_flags_t *flags        |                                           |
        SYSCALL_NETSEND,                // | int            | SOCKET *socket            | const void *buf                     | size_t *len               | NET_flags_t *flags        |                                           |
        SYSCALL_NETGETHOSTBYNAME,       // | int            | NET_family_t *family      | const char *name                    | void *addr                | size_t *addr_size         |                                           |
        SYSCALL_NETSETRECVTIMEOUT,      // | int            | SOCKET *socket            | uint32_t *timeout                   |                           |                           |                                           |
        SYSCALL_NETSETSENDTIMEOUT,      // | int            | SOCKET *socket            | uint32_t *timeout                   |                           |                           |                                           |
        SYSCALL_NETCONNECT,             // | int            | SOCKET *socket            | const NET_generic_sockaddr_t *addr  |                           |                           |                                           |
        SYSCALL_NETDISCONNECT,          // | int            | SOCKET *socket            |                                     |                           |                           |                                           |
        SYSCALL_NETSHUTDOWN,            // | int            | SOCKET *socket            | NET_shut_t *how                     |                           |                           |                                           |
        SYSCALL_NETSENDTO,              // | int            | SOCKET *socket            | const void *buf                     | size_t *len               | NET_flags_t *flags        | const NET_generic_sockaddr_t *to_sockaddr |
        SYSCALL_NETRECVFROM,            // | int            | SOCKET *socket            | void *buf                           | size_t *len               | NET_flags_t *flags        | NET_generic_sockaddr_t *from_sockaddr     |
        SYSCALL_NETGETADDRESS,          // | int            | SOCKET *socket            | NET_generic_sockaddr_t *addr        |                           |                           |                                           |
    #endif
#define _SYSCALL_GROUP_1_BLOCKING       _SYSCALL_COUNT // network group ----------------+-------------------------------------+---------------------------+---------------------------+-------------------------------------------+
        _SYSCALL_COUNT
} syscall_t;

/*==============================================================================
  Exported objects
==============================================================================*/
extern struct _process *_kworker_proc;
#if (__OS_TASK_KWORKER_MODE__ == 0) || (__OS_TASK_KWORKER_MODE__ == 1)
extern pid_t _syscall_client_PID[__OS_TASK_MAX_SYSTEM_THREADS__];
#endif

/*==============================================================================
  Exported functions
==============================================================================*/
extern void syscall(syscall_t syscall, void *retptr, ...);
extern int  _syscall_init();
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
