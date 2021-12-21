/*==============================================================================
File     syscall.h

Author   Daniel Zorychta

Brief    Syscall handing.

         Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

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


==============================================================================*/

/**
@defgroup syscall-h <dnx/syscall.h>

Detailed Doxygen description.
*/
/**@{*/

#ifndef _LIBC_DNX_SYSCALL_H_
#define _LIBC_DNX_SYSCALL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
#ifndef MAX_DELAY_MS
#define MAX_DELAY_MS                    (UINT32_MAX - 1000)
#endif

#ifndef MAX_DELAY_S
#define MAX_DELAY_S                     (MAX_DELAY_MS / 1000)
#endif

/** PRIORITIES */
#ifndef PRIORITY
#define PRIORITY(prio)                  (prio + (__OS_TASK_MAX_PRIORITIES__ / 2))
#endif

#ifndef PRIORITY_LOWEST
#define PRIORITY_LOWEST                 (-(int)(__OS_TASK_MAX_PRIORITIES__ / 2))
#endif

#ifndef PRIORITY_NORMAL
#define PRIORITY_NORMAL                 0
#endif

#ifndef PRIORITY_HIGHEST
#define PRIORITY_HIGHEST                ((int)(__OS_TASK_MAX_PRIORITIES__ / 2))
#endif

/*==============================================================================
  Exported object types
==============================================================================*/
typedef enum {// NAME                        | RETURN TYPE    | ARG 1                     | ARG 2                               | ARG 3                               | ARG 4                     | ARG 5                                     |
                                          // |----------------+---------------------------+-------------------------------------+-------------------------------------+---------------------------+-------------------------------------------+
        _LIBC_SYS_GETRUNTIMECTX,          // | int            | _dnxrtctx_t *ctx
        _LIBC_SYS_MALLOC,                 // | void*          | size_t *size              |                                     |                                     |                           |                                           |
        _LIBC_SYS_ZALLOC,                 // | void*          | size_t *size              |                                     |                                     |                           |                                           |
        _LIBC_SYS_FREE,                   // | void           | void *mem                 |                                     |                                     |                           |                                           |
        _LIBC_SYS_SHMCREATE,              // | int            | const char *key           | size_t *size                        |                                     |                           |                                           |
        _LIBC_SYS_SHMATTACH,              // | int            | const char *key           | void **mem                          | size_t *size                        |                           |                                           |
        _LIBC_SYS_SHMDETACH,              // | int            | const char *key           |                                     |                                     |                           |                                           |
        _LIBC_SYS_SHMDESTROY,             // | int            | const char *key           |                                     |                                     |                           |                                           |
        _LIBC_SYS_PROCESSWAIT,            // | int            | const pid_t *pid          | int *status                         | const uint32_t *timeout             |                           |                                           |
        _LIBC_SYS_PROCESSSTATSEEK,        // | int            | size_t *seek              | process_stat_t *stat                |                                     |                           |                                           |
        _LIBC_SYS_THREADSTAT,             // | int            | pid_t *pid                | tid_t *tid                          | thread_stat_t *stat                 |                           |                                           |
        _LIBC_SYS_PROCESSSTATPID,         // | int            | pid_t *pid                | process_stat_t *stat                |                                     |                           |                                           |
        _LIBC_SYS_PROCESSGETPID,          // | pid_t          |                           |                                     |                                     |                           |                                           |
        _LIBC_SYS_PROCESSGETPRIO,         // | int            | pid_t *pid                |                                     |                                     |                           |                                           |
        _LIBC_SYS_GETCWD,                 // | char*          | char *buf                 | size_t *size                        |                                     |                           |                                           |
        _LIBC_SYS_SETCWD,                 // | int            | const char *cwd           |                                     |                                     |                           |                                           |
        _LIBC_SYS_SYSLOGREAD,             // | size_t         | char *str                 | size_t *len                         | const struct timeval *from          | struct timeval *current   |                                           |
        _LIBC_SYS_THREADCREATE,           // | tid_t          | thread_func_t             | thread_attr_t *attr                 | void *arg                           |                           |                                           |
        _LIBC_SYS_SEMAPHORECREATE,        // | sem_t*         | const size_t *cnt_max     | const size_t *cnt_init              |                                     |                           |                                           |
        _LIBC_SYS_SEMAPHOREDESTROY,       // | void           | sem_t *semaphore          |                                     |                                     |                           |                                           |
        _LIBC_SYS_SEMAPHOREWAIT,          // | bool           | sem_t *semaphore          | uint32_t *timeout
        _LIBC_SYS_SEMAPHORESIGNAL,        // | bool           | sem_t *semaphore
        _LIBC_SYS_SEMAPHOREGETVALUE,      // | int            | sen_t *semaphore
        _LIBC_SYS_MUTEXCREATE,            // | mutex_t*       | const enum mutex_type *tp |                                     |                                     |                           |                                           |
        _LIBC_SYS_MUTEXDESTROY,           // | void           | mutex_t *mutex            |                                     |                                     |                           |                                           |
        _LIBC_SYS_MUTEXLOCK,              // | bool           | mutex_t *mutex            | uint32_t *timeout
        _LIBC_SYS_MUTEXUNLOCK,            // | bool           | mutex_t *mutex
        _LIBC_SYS_QUEUECREATE,            // | queue_t*       | const size_t *length      | const size_t *item_size             |                                     |                           |                                           |
        _LIBC_SYS_QUEUEDESTROY,           // | void           | queue_t *queue            |                                     |                                     |                           |                                           |
        _LIBC_SYS_QUEUERESET,             // | bool           | queue_t *queue
        _LIBC_SYS_QUEUESEND,              // | bool           | queue_t *queue            | const void *item                    | const uint32_t *timeout
        _LIBC_SYS_QUEUERECEIVE,           // | bool           | queue_t *queue            | void *item                          | const uint32_t *timeout
        _LIBC_SYS_QUEUERECEIVEPEEK,       // | bool           | queue_t *queue            | void *item                          | const uint32_t *timeout
        _LIBC_SYS_QUEUEITEMSCOUNT,        // | int            | queue_t *queue
        _LIBC_SYS_QUEUEFREESPACE,         // | int            | queue_t *queue
        _LIBC_SYS_THREADKILL,             // | int            | tid_t *tid                |                                     |                                     |                           |                                           |
        _LIBC_SYS_PROCESSCREATE,          // | pid_t          | const char *command       | process_attr_t *attr                |                                     |                           |                                           |
        _LIBC_SYS_PROCESSKILL,            // | int            | const pid_t *pid          |                                     |                                     |                           |                                           |
        _LIBC_SYS_PROCESSABORT,           // | int            |                           |                                     |                                     |                           |                                           |
        _LIBC_SYS_PROCESSEXIT,            // | void           | int *status               |                                     |                                     |                           |                                           |
        _LIBC_SYS_MOUNT,                  // | int            | const char *FS_name       | const char *src_path                | const char *mount_point             | const char *options       |                                           |
        _LIBC_SYS_UMOUNT,                 // | int            | const char *mount_point   |                                     |                                     |                           |                                           |
        _LIBC_SYS_MKNOD,                  // | int            | const char *pathname      | const char *mod_name                | int *major                          | int *minor                |                                           |
        _LIBC_SYS_GETMNTENTRY,            // | int            | int *seek                 | struct mntent *mntent               |                                     |                           |                                           |
        _LIBC_SYS_MKFIFO,                 // | int            | const char *pathname      | mode_t *mode                        |                                     |                           |                                           |
        _LIBC_SYS_MKDIR,                  // | int            | const char *pathname      | mode_t *mode                        |                                     |                           |                                           |
        _LIBC_SYS_OPENDIR,                // | DIR*           | const char *pathname      |                                     |                                     |                           |                                           |
        _LIBC_SYS_CLOSEDIR,               // | int            | DIR *dir                  |                                     |                                     |                           |                                           |
        _LIBC_SYS_DIRSEEK,                // | void           | DIR *dir                  | const uint32_t *seek
        _LIBC_SYS_DIRTELL,                // | u32_t          | DIR *dir
        _LIBC_SYS_READDIR,                // | dirent_t*      | DIR *dir                  |                                     |                                     |                           |                                           |
        _LIBC_SYS_REMOVE,                 // | int            | const char *path          |                                     |                                     |                           |                                           |
        _LIBC_SYS_RENAME,                 // | int            | const char *old_name      | const char *new_name                |                                     |                           |                                           |
        _LIBC_SYS_CHMOD,                  // | int            | const char *pathname      | mode_t *mode                        |                                     |                           |                                           |
        _LIBC_SYS_CHOWN,                  // | int            | const char *pathname      | uid_t *owner                        | gid_t *group                        |                           |                                           |
        _LIBC_SYS_STATFS,                 // | int            | const char *path          | struct statfs *buf                  |                                     |                           |                                           |
        _LIBC_SYS_STAT,                   // | int            | const char *pathname      | struct stat *buf                    |                                     |                           |                                           |
        _LIBC_SYS_FSTAT,                  // | int            | FILE *file                | struct stat *buf                    |                                     |                           |                                           |
        _LIBC_SYS_FOPEN,                  // | FILE*          | const char *path          | const char *mode                    |                                     |                           |                                           |
        _LIBC_SYS_FCLOSE,                 // | int            | FILE *file                |                                     |                                     |                           |                                           |
        _LIBC_SYS_FWRITE,                 // | size_t         | const void *src           | size_t *count                       | FILE *file                          |                           |                                           |
        _LIBC_SYS_FREAD,                  // | size_t         | void *dst                 | size_t *count                       | FILE *file                          |                           |                                           |
        _LIBC_SYS_FSEEK,                  // | int            | FILE *file                | i64_t  *seek                        | int    *origin                      |                           |                                           |
        _LIBC_SYS_FTELL,                  // | i64_t          | FILE *file                |                                     |                                     |                           |                                           |
        _LIBC_SYS_IOCTL,                  // | int            | FILE *file                | int *request                        | va_list *arg                        |                           |                                           |
        _LIBC_SYS_FFLUSH,                 // | int            | FILE *file                |                                     |                                     |                           |                                           |
        _LIBC_SYS_FEOF,                   // | int            | FILE *file                |                                     |                                     |                           |                                           |
        _LIBC_SYS_FERROR,                 // | int            | FILE *file                |                                     |                                     |                           |                                           |
        _LIBC_SYS_CLEARERR,               // | void           | FILE *file                |                                     |                                     |                           |                                           |
        _LIBC_SYS_SYNC,                   // | void           |                           |                                     |                                     |                           |                                           |
        _LIBC_SYS_GETTIMEOFDAY,           // | int            | struct timeval *tv        | struct timezone *tz                 |                                     |                           |                                           |
        _LIBC_SYS_SETTIMEOFDAY,           // | int            | const struct timeval *tv  | const struct timezone *tz           |                                     |                           |                                           |
        _LIBC_SYS_DRIVERINIT,             // | dev_t          | const char *mod_name      | int *major                          | int *minor                          | const char *node_path     | const void *config                        |
        _LIBC_SYS_DRIVERRELEASE,          // | int            | const char *mod_name      | int *major                          | int *minor                          |                           |                                           |
        _LIBC_SYS_KERNELPANICINFO,        // | bool           | kernel_panic_info_t *info |                                     |                                     |                           |                                           |
        _LIBC_SYS_NETADD,                 // | int            | char *netname             | NET_family_t *family                | const char *if_path                 |                           |                                           |
        _LIBC_SYS_NETRM    ,              // | int            | char *netname             |                                     |                                     |                           |                                           |
        _LIBC_SYS_NETIFLIST,              // | int            | char *netname[]           | size_t *netname_len                 |                                     |                           |                                           |
        _LIBC_SYS_NETIFUP,                // | int            | const char *netname       | const NET_generic_config_t *config  |                                     |                           |                                           |
        _LIBC_SYS_NETIFDOWN,              // | int            | const char *netname       |                                     |                                     |                           |                                           |
        _LIBC_SYS_NETIFSTATUS,            // | int            | const char *netname       | NET_family_t *family                | NET_generic_status_t *status        |                           |                                           |
        _LIBC_SYS_NETSOCKETCREATE,        // | SOCKET*        | const char *netname       | NET_protocol_t *protocol            |                                     |                           |                                           |
        _LIBC_SYS_NETGETHOSTBYNAME,       // | int            | const char *netname       | const char *name                    | void *addr                          | size_t *addr_size         |                                           |
        _LIBC_SYS_NETSOCKETDESTROY,       // | void           | SOCKET *socket            |                                     |                                     |                           |                                           |
        _LIBC_SYS_NETBIND,                // | int            | SOCKET *socket            | const NET_generic_sockaddr_t *addr  |                                     |                           |                                           |
        _LIBC_SYS_NETLISTEN,              // | int            | SOCKET *socket            |                                     |                                     |                           |                                           |
        _LIBC_SYS_NETACCEPT,              // | int            | SOCKET *socket            | SOCKET **new_socket                 |                                     |                           |                                           |
        _LIBC_SYS_NETRECV,                // | int            | SOCKET *socket            | void *buf                           | size_t *len                         | NET_flags_t *flags        |                                           |
        _LIBC_SYS_NETSEND,                // | int            | SOCKET *socket            | const void *buf                     | size_t *len                         | NET_flags_t *flags        |                                           |
        _LIBC_SYS_NETSETRECVTIMEOUT,      // | int            | SOCKET *socket            | uint32_t *timeout                   |                                     |                           |                                           |
        _LIBC_SYS_NETSETSENDTIMEOUT,      // | int            | SOCKET *socket            | uint32_t *timeout                   |                                     |                           |                                           |
        _LIBC_SYS_NETCONNECT,             // | int            | SOCKET *socket            | const NET_generic_sockaddr_t *addr  |                                     |                           |                                           |
        _LIBC_SYS_NETDISCONNECT,          // | int            | SOCKET *socket            |                                     |                                     |                           |                                           |
        _LIBC_SYS_NETSHUTDOWN,            // | int            | SOCKET *socket            | NET_shut_t *how                     |                                     |                           |                                           |
        _LIBC_SYS_NETSENDTO,              // | int            | SOCKET *socket            | const void *buf                     | size_t *len                         | NET_flags_t *flags        | const NET_generic_sockaddr_t *to_sockaddr |
        _LIBC_SYS_NETRECVFROM,            // | int            | SOCKET *socket            | void *buf                           | size_t *len                         | NET_flags_t *flags        | NET_generic_sockaddr_t *from_sockaddr     |
        _LIBC_SYS_NETGETADDRESS,          // | int            | SOCKET *socket            | NET_generic_sockaddr_t *addr        |                                     |                           |                                           |
        _LIBC_SYS_NETHTON16,              // | uint16_t       | NET_family_t *family      | uint16_t *value                     |                                     |                           |                                           |
        _LIBC_SYS_NETHTON32,              // | uint32_t       | NET_family_t *family      | uint32_t *value                     |                                     |                           |                                           |
        _LIBC_SYS_NETHTON64,              // | uint64_t       | NET_family_t *family      | uint64_t *value                     |                                     |                           |                                           |
        _LIBC_SYS_MSLEEP,                 // | void           | const uint32_t *mseconds  |                                     |                                     |                           |                                           |
        _LIBC_SYS_USLEEP,                 // | void           | const uint32_t *useconds  |                                     |                                     |                           |                                           |
        _LIBC_SYS_GETUID,                 // | uid_t          |                           |                                     |                                     |                           |                                           |
        _LIBC_SYS_GETGID,                 // | gid_t          |                           |                                     |                                     |                           |                                           |
        _LIBC_SYS_GETMEMDETAILS,          // | int            | memstat_t *stat           |                                     |                                     |                           |                                           |
        _LIBC_SYS_GETMODMEMUSAGE,         // | int            | uint *module              | int32_t *usage                      |                                     |                           |                                           |
        _LIBC_SYS_GETUPTIMEMS,            // | uint64_t       |                           |                                     |                                     |                           |                                           |
        _LIBC_SYS_GETAVGCPULOAD,          // | int            | avg_CPU_load_t *stat      |                                     |                                     |                           |                                           |
        _LIBC_SYS_GETPLATFORMNAME,        // | const char*    |                           |                                     |                                     |                           |                                           |
        _LIBC_SYS_GETOSNAME,              // | const char*    |                           |                                     |                                     |                           |                                           |
        _LIBC_SYS_GETOSVER,               // | const char*    |                           |                                     |                                     |                           |                                           |
        _LIBC_SYS_GETOSCODENAME,          // | const char*    |                           |                                     |                                     |                           |                                           |
        _LIBC_SYS_GETKERNELNAME,          // | const char*    |                           |                                     |                                     |                           |                                           |
        _LIBC_SYS_GETKERNELVER,           // | const char*    |                           |                                     |                                     |                           |                                           |
        _LIBC_SYS_GETHOSTNAME,            // | int            | char *buf                 | size_t *buf_len                     |                                     |                           |                                           |
        _LIBC_SYS_GETDRIVERNAME,          // | const char*    | size_t *modno             |                                     |                                     |                           |                                           |
        _LIBC_SYS_GETDRIVERID,            // | int            | const char *name          |                                     |                                     |                           |                                           |
        _LIBC_SYS_GETDRIVERCOUNT,         // | size_t         |                           |                                     |                                     |                           |                                           |
        _LIBC_SYS_GETDRIVERINSTANCES,     // | ssize_t        | size_t id                 |                                     |                                     |                           |                                           |
        _LIBC_SYS_SYSTEMRESTART,          // | void           |                           |                                     |                                     |                           |                                           |
        _LIBC_SYS_SYSTEMSHUTDOWN,         // | void           |                           |                                     |                                     |                           |                                           |
        _LIBC_SYS_SYSLOGCLEAR,            // | void           |                           |                                     |                                     |                           |                                           |
        _LIBC_SYS_FLAGWAIT,               // | bool           | flag_t *flag              | uint32_t *mask                      | uint32_t *timeout                   |                           |                                           |
        _LIBC_SYS_GETACTIVETHREAD,        // | int            |                           |                                     |                                     |                           |                                           |
        _LIBC_SYS_THREADEXIT,             // | void           | int *status               |                                     |                                     |                           |                                           |
        _LIBC_SYS_SCHEDULERLOCK,          // | void           |                           |                                     |                                     |                           |                                           |
        _LIBC_SYS_SCHEDULERUNLOCK,        // | void           |                           |                                     |                                     |                           |                                           |
        _LIBC_SYS_THREADJOIN,             // | int            | const tid_t *tid          | int *status                         | const uint32_t *timeout
} _libc_syscall_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern void libc_syscall(_libc_syscall_t syscall, void *retptr, ...);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _LIBC_DNX_SYSCALL_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
