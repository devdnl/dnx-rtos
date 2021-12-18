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
// SYSCALLS                                |----------------+---------------------------+-------------------------------------+-------------------------------------+---------------------------+-------------------------------------------+
typedef enum {// NAME                      | RETURN TYPE    | ARG 1                     | ARG 2                               | ARG 3                               | ARG 4                     | ARG 5                                     |
                                        // |----------------+---------------------------+-------------------------------------+-------------------------------------+---------------------------+-------------------------------------------+
        SYSCALL_MALLOC,                 // | void*          | size_t *size              |                                     |                                     |                           |                                           |
        SYSCALL_ZALLOC,                 // | void*          | size_t *size              |                                     |                                     |                           |                                           |
        SYSCALL_FREE,                   // | void           | void *mem                 |                                     |                                     |                           |                                           |
        SYSCALL_SHMCREATE,              // | int            | const char *key           | size_t *size                        |                                     |                           |                                           |
        SYSCALL_SHMATTACH,              // | int            | const char *key           | void **mem                          | size_t *size                        |                           |                                           |
        SYSCALL_SHMDETACH,              // | int            | const char *key           |                                     |                                     |                           |                                           |
        SYSCALL_SHMDESTROY,             // | int            | const char *key           |                                     |                                     |                           |                                           |
        SYSCALL_PROCESSGETSYNCFLAG,     // | int            | pid_t *pid                | flag_t **obj                        |                                     |                           |                                           |
        SYSCALL_PROCESSSTATSEEK,        // | int            | size_t *seek              | process_stat_t *stat                |                                     |                           |                                           |
        SYSCALL_THREADSTAT,             // | int            | pid_t *pid                | tid_t *tid                          | thread_stat_t *stat                 |                           |                                           |
        SYSCALL_PROCESSSTATPID,         // | int            | pid_t *pid                | process_stat_t *stat                |                                     |                           |                                           |
        SYSCALL_PROCESSGETPID,          // | pid_t          |                           |                                     |                                     |                           |                                           |
        SYSCALL_PROCESSGETPRIO,         // | int            | pid_t *pid                |                                     |                                     |                           |                                           |
        SYSCALL_GETCWD,                 // | char*          | char *buf                 | size_t *size                        |                                     |                           |                                           |
        SYSCALL_SETCWD,                 // | int            | const char *cwd           |                                     |                                     |                           |                                           |
        SYSCALL_SYSLOGREAD,             // | size_t         | char *str                 | size_t *len                         | const struct timeval *from          | struct timeval *current   |                                           |
        SYSCALL_THREADCREATE,           // | tid_t          | thread_func_t             | thread_attr_t *attr                 | void *arg                           |                           |                                           |
        SYSCALL_SEMAPHORECREATE,        // | sem_t*         | const size_t *cnt_max     | const size_t *cnt_init              |                                     |                           |                                           |
        SYSCALL_SEMAPHOREDESTROY,       // | void           | sem_t *semaphore          |                                     |                                     |                           |                                           |
        SYSCALL_SEMAPHOREWAIT,          // | bool           | sem_t *semaphore          | uint32_t *timeout
        SYSCALL_SEMAPHORESIGNAL,        // | bool           | sem_t *semaphore
        SYSCALL_SEMAPHOREGETVALUE,      // | int            | sen_t *semaphore
        SYSCALL_MUTEXCREATE,            // | mutex_t*       | const enum mutex_type *tp |                                     |                                     |                           |                                           |
        SYSCALL_MUTEXDESTROY,           // | void           | mutex_t *mutex            |                                     |                                     |                           |                                           |
        SYSCALL_MUTEXLOCK,              // | bool           | mutex_t *mutex            | uint32_t *timeout
        SYSCALL_MUTEXUNLOCK,            // | bool           | mutex_t *mutex
        SYSCALL_QUEUECREATE,            // | queue_t*       | const size_t *length      | const size_t *item_size             |                                     |                           |                                           |
        SYSCALL_QUEUEDESTROY,           // | void           | queue_t *queue            |                                     |                                     |                           |                                           |
        SYSCALL_QUEUERESET,             // | bool           | queue_t *queue
        SYSCALL_QUEUESEND,              // | bool           | queue_t *queue            | const void *item                    | const uint32_t *timeout
        SYSCALL_QUEUERECEIVE,           // | bool           | queue_t *queue            | void *item                          | const uint32_t *timeout
        SYSCALL_QUEUERECEIVEPEEK,       // | bool           | queue_t *queue            | void *item                          | const uint32_t *timeout
        SYSCALL_QUEUEITEMSCOUNT,        // | int            | queue_t *queue
        SYSCALL_QUEUEFREESPACE,         // | int            | queue_t *queue
        SYSCALL_THREADKILL,             // | int            | tid_t *tid                |                                     |                                     |                           |                                           |
        SYSCALL_THREADGETSTATUS,        // | int            | tid_t *tid                | int *status                         |                                     |                           |                                           |
        SYSCALL_PROCESSCREATE,          // | pid_t          | const char *command       | process_attr_t *attr                |                                     |                           |                                           |
        SYSCALL_PROCESSCLEANZOMBIE,     // | int            | pid_t *pid                | int *status                         |                                     |                           |                                           |
        SYSCALL_PROCESSKILL,            // | int            | pid_t *pid                |                                     |                                     |                           |                                           |
        SYSCALL_PROCESSABORT,           // | int            |                           |                                     |                                     |                           |                                           |
        SYSCALL_PROCESSEXIT,            // | void           | int *status               |                                     |                                     |                           |                                           |
        SYSCALL_MOUNT,                  // | int            | const char *FS_name       | const char *src_path                | const char *mount_point             | const char *options       |                                           |
        SYSCALL_UMOUNT,                 // | int            | const char *mount_point   |                                     |                                     |                           |                                           |
        SYSCALL_MKNOD,                  // | int            | const char *pathname      | const char *mod_name                | int *major                          | int *minor                |                                           |
        SYSCALL_GETMNTENTRY,            // | int            | int *seek                 | struct mntent *mntent               |                                     |                           |                                           |
        SYSCALL_MKFIFO,                 // | int            | const char *pathname      | mode_t *mode                        |                                     |                           |                                           |
        SYSCALL_MKDIR,                  // | int            | const char *pathname      | mode_t *mode                        |                                     |                           |                                           |
        SYSCALL_OPENDIR,                // | DIR*           | const char *pathname      |                                     |                                     |                           |                                           |
        SYSCALL_CLOSEDIR,               // | int            | DIR *dir                  |                                     |                                     |                           |                                           |
        SYSCALL_DIRSEEK,                // | void           | DIR *dir                  | const uint32_t *seek
        SYSCALL_DIRTELL,                // | u32_t          | DIR *dir
        SYSCALL_READDIR,                // | dirent_t*      | DIR *dir                  |                                     |                                     |                           |                                           |
        SYSCALL_REMOVE,                 // | int            | const char *path          |                                     |                                     |                           |                                           |
        SYSCALL_RENAME,                 // | int            | const char *old_name      | const char *new_name                |                                     |                           |                                           |
        SYSCALL_CHMOD,                  // | int            | const char *pathname      | mode_t *mode                        |                                     |                           |                                           |
        SYSCALL_CHOWN,                  // | int            | const char *pathname      | uid_t *owner                        | gid_t *group                        |                           |                                           |
        SYSCALL_STATFS,                 // | int            | const char *path          | struct statfs *buf                  |                                     |                           |                                           |
        SYSCALL_STAT,                   // | int            | const char *pathname      | struct stat *buf                    |                                     |                           |                                           |
        SYSCALL_FSTAT,                  // | int            | FILE *file                | struct stat *buf                    |                                     |                           |                                           |
        SYSCALL_FOPEN,                  // | FILE*          | const char *path          | const char *mode                    |                                     |                           |                                           |
        SYSCALL_FCLOSE,                 // | int            | FILE *file                |                                     |                                     |                           |                                           |
        SYSCALL_FWRITE,                 // | size_t         | const void *src           | size_t *count                       | FILE *file                          |                           |                                           |
        SYSCALL_FREAD,                  // | size_t         | void *dst                 | size_t *count                       | FILE *file                          |                           |                                           |
        SYSCALL_FSEEK,                  // | int            | FILE *file                | i64_t  *seek                        | int    *origin                      |                           |                                           |
        SYSCALL_FTELL,                  // | i64_t          | FILE *file                |                                     |                                     |                           |                                           |
        SYSCALL_IOCTL,                  // | int            | FILE *file                | int *request                        | va_list *arg                        |                           |                                           |
        SYSCALL_FFLUSH,                 // | int            | FILE *file                |                                     |                                     |                           |                                           |
        SYSCALL_FEOF,                   // | int            | FILE *file                |                                     |                                     |                           |                                           |
        SYSCALL_FERROR,                 // | int            | FILE *file                |                                     |                                     |                           |                                           |
        SYSCALL_CLEARERR,               // | void           | FILE *file                |                                     |                                     |                           |                                           |
        SYSCALL_SYNC,                   // | void           |                           |                                     |                                     |                           |                                           |
        SYSCALL_GETTIMEOFDAY,           // | int            | struct timeval *          |                                     |                                     |                           |                                           |
        SYSCALL_SETTIMEOFDAY,           // | int            | time_t *time              |                                     |                                     |                           |                                           |
        SYSCALL_DRIVERINIT,             // | dev_t          | const char *mod_name      | int *major                          | int *minor                          | const char *node_path     | const void *config                        |
        SYSCALL_DRIVERRELEASE,          // | int            | const char *mod_name      | int *major                          | int *minor                          |                           |                                           |
        SYSCALL_KERNELPANICINFO,        // | bool           | kernel_panic_info_t *info |                                     |                                     |                           |                                           |
        SYSCALL_NETADD,                 // | int            | char *netname             | NET_family_t *family                | const char *if_path                 |                           |                                           |
        SYSCALL_NETRM    ,              // | int            | char *netname             |                                     |                                     |                           |                                           |
        SYSCALL_NETIFLIST,              // | int            | char *netname[]           | size_t *netname_len                 |                                     |                           |                                           |
        SYSCALL_NETIFUP,                // | int            | const char *netname       | const NET_generic_config_t *config  |                                     |                           |                                           |
        SYSCALL_NETIFDOWN,              // | int            | const char *netname       |                                     |                                     |                           |                                           |
        SYSCALL_NETIFSTATUS,            // | int            | const char *netname       | NET_family_t *family                | NET_generic_status_t *status        |                           |                                           |
        SYSCALL_NETSOCKETCREATE,        // | SOCKET*        | const char *netname       | NET_protocol_t *protocol            |                                     |                           |                                           |
        SYSCALL_NETGETHOSTBYNAME,       // | int            | const char *netname       | const char *name                    | void *addr                          | size_t *addr_size         |                                           |
        SYSCALL_NETSOCKETDESTROY,       // | void           | SOCKET *socket            |                                     |                                     |                           |                                           |
        SYSCALL_NETBIND,                // | int            | SOCKET *socket            | const NET_generic_sockaddr_t *addr  |                                     |                           |                                           |
        SYSCALL_NETLISTEN,              // | int            | SOCKET *socket            |                                     |                                     |                           |                                           |
        SYSCALL_NETACCEPT,              // | int            | SOCKET *socket            | SOCKET **new_socket                 |                                     |                           |                                           |
        SYSCALL_NETRECV,                // | int            | SOCKET *socket            | void *buf                           | size_t *len                         | NET_flags_t *flags        |                                           |
        SYSCALL_NETSEND,                // | int            | SOCKET *socket            | const void *buf                     | size_t *len                         | NET_flags_t *flags        |                                           |
        SYSCALL_NETSETRECVTIMEOUT,      // | int            | SOCKET *socket            | uint32_t *timeout                   |                                     |                           |                                           |
        SYSCALL_NETSETSENDTIMEOUT,      // | int            | SOCKET *socket            | uint32_t *timeout                   |                                     |                           |                                           |
        SYSCALL_NETCONNECT,             // | int            | SOCKET *socket            | const NET_generic_sockaddr_t *addr  |                                     |                           |                                           |
        SYSCALL_NETDISCONNECT,          // | int            | SOCKET *socket            |                                     |                                     |                           |                                           |
        SYSCALL_NETSHUTDOWN,            // | int            | SOCKET *socket            | NET_shut_t *how                     |                                     |                           |                                           |
        SYSCALL_NETSENDTO,              // | int            | SOCKET *socket            | const void *buf                     | size_t *len                         | NET_flags_t *flags        | const NET_generic_sockaddr_t *to_sockaddr |
        SYSCALL_NETRECVFROM,            // | int            | SOCKET *socket            | void *buf                           | size_t *len                         | NET_flags_t *flags        | NET_generic_sockaddr_t *from_sockaddr     |
        SYSCALL_NETGETADDRESS,          // | int            | SOCKET *socket            | NET_generic_sockaddr_t *addr        |                                     |                           |                                           |
        SYSCALL_NETHTON16,              // | uint16_t       | NET_family_t *family      | uint16_t *value                     |                                     |                           |                                           |
        SYSCALL_NETHTON32,              // | uint32_t       | NET_family_t *family      | uint32_t *value                     |                                     |                           |                                           |
        SYSCALL_NETHTON64,              // | uint64_t       | NET_family_t *family      | uint64_t *value                     |                                     |                           |                                           |
        SYSCALL_MSLEEP,                 // | void           | const uint32_t *mseconds  |                                     |                                     |                           |                                           |
        SYSCALL_USLEEP,                 // | void           | const uint32_t *useconds  |                                     |                                     |                           |                                           |
        SYSCALL_GETUID,                 // | uid_t          |                           |                                     |                                     |                           |                                           |
        SYSCALL_GETGID,                 // | gid_t          |                           |                                     |                                     |                           |                                           |
        SYSCALL_GETMEMDETAILS,          // | int            | memstat_t *stat           |                                     |                                     |                           |                                           |
        SYSCALL_GETMODMEMUSAGE,         // | int            | uint *module              | int32_t *usage                      |                                     |                           |                                           |
        SYSCALL_GETUPTIMEMS,            // | uint64_t       |                           |                                     |                                     |                           |                                           |
        SYSCALL_GETAVGCPULOAD,          // | int            | avg_CPU_load_t *stat      |                                     |                                     |                           |                                           |
        SYSCALL_GETPLATFORMNAME,        // | const char*    |                           |                                     |                                     |                           |                                           |
        SYSCALL_GETOSNAME,              // | const char*    |                           |                                     |                                     |                           |                                           |
        SYSCALL_GETOSVER,               // | const char*    |                           |                                     |                                     |                           |                                           |
        SYSCALL_GETOSCODENAME,          // | const char*    |                           |                                     |                                     |                           |                                           |
        SYSCALL_GETKERNELNAME,          // | const char*    |                           |                                     |                                     |                           |                                           |
        SYSCALL_GETKERNELVER,           // | const char*    |                           |                                     |                                     |                           |                                           |
        SYSCALL_GETHOSTNAME,            // | int            | char *buf                 | size_t *buf_len                     |                                     |                           |                                           |
        SYSCALL_GETDRIVERNAME,          // | const char*    | size_t *modno             |                                     |                                     |                           |                                           |
        SYSCALL_GETDRIVERID,            // | int            | const char *name          |                                     |                                     |                           |                                           |
        SYSCALL_GETDRIVERCOUNT,         // | size_t         |                           |                                     |                                     |                           |                                           |
        SYSCALL_GETDRIVERINSTANCES,     // | ssize_t        | size_t id                 |                                     |                                     |                           |                                           |
        SYSCALL_SYSTEMRESTART,          // | void           |                           |                                     |                                     |                           |                                           |
        SYSCALL_SYSTEMSHUTDOWN,         // | void           |                           |                                     |                                     |                           |                                           |
        SYSCALL_SYSLOGCLEAR,            // | void           |                           |                                     |                                     |                           |                                           |
        SYSCALL_FLAGWAIT,               // | bool           | flag_t *flag              | uint32_t *mask                      | uint32_t *timeout                   |                           |                                           |
        SYSCALL_GETACTIVETHREAD,        // | int            |                           |                                     |                                     |                           |                                           |
        SYSCALL_THREADEXIT,             // | void           | int *status               |                                     |                                     |                           |                                           |
        SYSCALL_SCHEDULERLOCK,          // | void           |                           |                                     |                                     |                           |                                           |
        SYSCALL_SCHEDULERUNLOCK,        // | void           |                           |                                     |                                     |                           |                                           |
        _SYSCALL_COUNT,
} syscall_t;

/*==============================================================================
  Exported objects
==============================================================================*/
extern struct _process *_kworker_proc;

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
