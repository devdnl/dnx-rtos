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
typedef enum {
        _LIBC_SYS_GETRUNTIMECTX,          // int errno (_dnxrtctx_t *ctx)
        _LIBC_SYS_MALLOC,                 // int errno (size_t *blk_size, void **alloc_mem)
        _LIBC_SYS_ZALLOC,                 // int errno (size_t *blk_size, void **alloc_mem)
        _LIBC_SYS_FREE,                   // int errno (void *mem)
        _LIBC_SYS_SHMCREATE,              // int errno (const char *key, size_t *size)
        _LIBC_SYS_SHMATTACH,              // int errno (const char *key, void **mem, size_t *size)
        _LIBC_SYS_SHMDETACH,              // int errno (const char *key)
        _LIBC_SYS_SHMDESTROY,             // int errno (const char *key)
        _LIBC_SYS_PROCESSWAIT,            // int errno (const pid_t *pid, int *status, const uint32_t *timeout)
        _LIBC_SYS_PROCESSSTATSEEK,        // int errno (size_t *seek, _process_stat_t *stat)
        _LIBC_SYS_THREADSTAT,             // int errno (pid_t *pid , tid_t *tid, _thread_stat_t *stat)
        _LIBC_SYS_PROCESSSTATPID,         // int errno (pid_t *pid, _process_stat_t *stat)
        _LIBC_SYS_PROCESSGETPID,          // int errno (pid_t *pid)
        _LIBC_SYS_PROCESSGETPRIO,         // int errno (pid_t *pid, int *priority)
        _LIBC_SYS_GETCWD,                 // int errno (char *buf, size_t *size)
        _LIBC_SYS_SETCWD,                 // int errno (const char *cwd)
        _LIBC_SYS_SYSLOGREAD,             // int errno (char *str, size_t *len, const struct timeval *from, struct timeval *current, size_t *count)
        _LIBC_SYS_THREADCREATE,           // int errno (thread_func_t, _thread_attr_t *attr, void *arg, tid_t *tid)
        _LIBC_SYS_SEMAPHOREOPEN,          // int errno (const size_t *cnt_max, const size_t *cnt_init, int *fd)
        _LIBC_SYS_SEMAPHOREWAIT,          // int errno (int *fd_semaphore, uint32_t *timeout)
        _LIBC_SYS_SEMAPHORESIGNAL,        // int errno (int *fd_semaphore)
        _LIBC_SYS_SEMAPHOREGETVALUE,      // int errno (int *fd_semaphore, size_t *value)
        _LIBC_SYS_MUTEXOPEN,              // int errno (const enum kmtx_type *mt, int *fd)
        _LIBC_SYS_MUTEXLOCK,              // int errno (int *mutex, uint32_t *timeout)
        _LIBC_SYS_MUTEXUNLOCK,            // int errno (int *mutex)
        _LIBC_SYS_CLOSE,                  // int errno (int *fd)
        _LIBC_SYS_QUEUEOPEN,              // int errno (const size_t *length, const size_t *item_size, int *fd)
        _LIBC_SYS_QUEUERESET,             // int errno (int *fd_queue)
        _LIBC_SYS_QUEUESEND,              // int errno (int *fd_queue, const void *item, const uint32_t *timeout)
        _LIBC_SYS_QUEUERECEIVE,           // int errno (int *fd_queue, void *item, const uint32_t *timeout)
        _LIBC_SYS_QUEUERECEIVEPEEK,       // int errno (int *fd_queue, void *item, const uint32_t *timeout)
        _LIBC_SYS_QUEUEITEMSCOUNT,        // int errno (int *fd_queue, size_t *count)
        _LIBC_SYS_QUEUEFREESPACE,         // int errno (int *fd_queue, size_t *count)
        _LIBC_SYS_THREADKILL,             // int errno (tid_t *tid)
        _LIBC_SYS_PROCESSCREATE,          // int errno (const char *command, _process_attr_t *attr, pid_t *pid)
        _LIBC_SYS_PROCESSKILL,            // int errno (const pid_t *pid)
        _LIBC_SYS_PROCESSABORT,           // int errno (void)
        _LIBC_SYS_PROCESSEXIT,            // int errno (int *status)
        _LIBC_SYS_MOUNT,                  // int errno (const char *FS_name, const char *src_path, const char *mount_point, const char *options)
        _LIBC_SYS_UMOUNT,                 // int errno (const char *mount_point)
        _LIBC_SYS_MKNOD,                  // int errno (const char *pathname, const char *mod_name, int *major, int *minor)
        _LIBC_SYS_GETMNTENTRY,            // int errno (size_t *seek, struct mntent *mntent)
        _LIBC_SYS_MKFIFO,                 // int errno (const char *pathname, mode_t *mode)
        _LIBC_SYS_DIRSEEK,                // int errno (int *fd_dir, const uint32_t *seek)
        _LIBC_SYS_DIRTELL,                // int errno (int *fd_dir, u32_t *pos)
        _LIBC_SYS_DIRREAD,                // int errno (int *fd_dir, dirent_t **dirent)
        _LIBC_SYS_REMOVE,                 // int errno (const char *path)
        _LIBC_SYS_RENAME,                 // int errno (const char *old_name, const char *new_name)
        _LIBC_SYS_CHMOD,                  // int errno (const char *pathname, mode_t *mode)
        _LIBC_SYS_CHOWN,                  // int errno (const char *pathname, uid_t *owner, gid_t *group)
        _LIBC_SYS_STATFS,                 // int errno (const char *path, struct statfs *buf)
        _LIBC_SYS_STAT,                   // int errno (const char *pathname, struct stat *buf)
        _LIBC_SYS_FSTAT,                  // int errno (int *fd, struct stat *buf)
        _LIBC_SYS_OPEN,                   // int errno (int *fd, const char *path, int flags, mode_t mode)
        _LIBC_SYS_WRITE,                  // int errno (int *fd, const void *src, size_t *count, size_t *written)
        _LIBC_SYS_READ,                   // int errno (int *fd, void *dst, size_t *count, size_t *read)
        _LIBC_SYS_SEEK64,                 // int errno (int *fd, i64_t *seek, int *origin)
        _LIBC_SYS_IOCTL,                  // int errno (int *fd, int *request, va_list *arg)
        _LIBC_SYS_FLUSH,                  // int errno (int *fd)
        _LIBC_SYS_SYNC,                   // int errno (void)
        _LIBC_SYS_GETTIMEOFDAY,           // int errno (struct timeval *tv, struct timezone *tz)
        _LIBC_SYS_SETTIMEOFDAY,           // int errno (const struct timeval *tv, const struct timezone *tz)
        _LIBC_SYS_DRIVERINIT,             // int errno (const char *mod_name, int *major, int *minor  const char *node_path, const void *config)
        _LIBC_SYS_DRIVERRELEASE,          // int errno (const char *mod_name, int *major, int *minor)
        _LIBC_SYS_KERNELPANICINFO,        // int errno (kernel_panic_info_t *info)
        _LIBC_SYS_NETADD,                 // int errno (char *netname, NET_family_t *family, const char *if_path)
        _LIBC_SYS_NETRM,                  // int errno (char *netname)
        _LIBC_SYS_NETIFLIST,              // int errno (char *netname[], size_t *netname_len, size_t *count)
        _LIBC_SYS_NETIFUP,                // int errno (const char *netname, const NET_generic_config_t *config, size_t *config_size)
        _LIBC_SYS_NETIFDOWN,              // int errno (const char *netname)
        _LIBC_SYS_NETIFSTATUS,            // int errno (const char *netname, NET_family_t *family, NET_generic_status_t *status, size_t *status_size)
        _LIBC_SYS_NETGETHOSTBYNAME,       // int errno (const char *netname, const char *name, void *addr, size_t *addr_size)
        _LIBC_SYS_NETSOCKETCREATE,        // int errno (int *fd, const char *netname, NET_protocol_t *protocol)
        _LIBC_SYS_NETBIND,                // int errno (int *fd, const NET_generic_sockaddr_t *addr, size_t *addr_size)
        _LIBC_SYS_NETLISTEN,              // int errno (int *fd)
        _LIBC_SYS_NETACCEPT,              // int errno (int *fd, int *new_fd)
        _LIBC_SYS_NETRECV,                // int errno (int *fd, void *buf, size_t *len, NET_flags_t *flags, size_t *rcved)
        _LIBC_SYS_NETSEND,                // int errno (int *fd, const void *buf,size_t *len, NET_flags_t *flags, size_t *sent)
        _LIBC_SYS_NETSETRECVTIMEOUT,      // int errno (int *fd, uint32_t *timeout)
        _LIBC_SYS_NETSETSENDTIMEOUT,      // int errno (int *fd, uint32_t *timeout)
        _LIBC_SYS_NETCONNECT,             // int errno (int *fd, const NET_generic_sockaddr_t *addr, size_t *addr_size)
        _LIBC_SYS_NETDISCONNECT,          // int errno (int *fd)
        _LIBC_SYS_NETSHUTDOWN,            // int errno (int *fd, NET_shut_t *how)
        _LIBC_SYS_NETSENDTO,              // int errno (int *fd, const void *buf, size_t *len, NET_flags_t *flags, const NET_generic_sockaddr_t *to_sockaddr, size_t *to_sockaddr_size, size_t *sent)
        _LIBC_SYS_NETRECVFROM,            // int errno (int *fd, void *buf, size_t *len, NET_flags_t *flags, NET_generic_sockaddr_t *from_sockaddr, size_t *from_sockaddr_size, size_t *rcved)
        _LIBC_SYS_NETGETADDRESS,          // int errno (int *fd, NET_generic_sockaddr_t *addr, size_t *addr_size)
        _LIBC_SYS_NETHTON16,              // int errno (NET_family_t *family, uint16_t *value_in, uint16_t *value_out)
        _LIBC_SYS_NETHTON32,              // int errno (NET_family_t *family, uint32_t *value_in, uint32_t *value_out)
        _LIBC_SYS_NETHTON64,              // int errno (NET_family_t *family, uint64_t *value_in, uint64_t *value_out)
        _LIBC_SYS_MSLEEP,                 // int errno (const uint32_t *mseconds)
        _LIBC_SYS_USLEEP,                 // int errno (const uint32_t *useconds)
        _LIBC_SYS_GETUID,                 // int errno (uid_t *uid)
        _LIBC_SYS_GETGID,                 // int errno (gid_t *gid)
        _LIBC_SYS_GETMEMDETAILS,          // int errno (memstat_t *stat)
        _LIBC_SYS_GETMODMEMUSAGE,         // int errno (uint *module, int32_t *usage)
        _LIBC_SYS_GETUPTIMEMS,            // int errno (uint64_t *uptime_ms)
        _LIBC_SYS_GETAVGCPULOAD,          // int errno (_avg_CPU_load_t *stat)
        _LIBC_SYS_GETPLATFORMNAME,        // int errno (const char **ref)
        _LIBC_SYS_GETOSNAME,              // int errno (const char **ref)
        _LIBC_SYS_GETOSVER,               // int errno (const char **ref)
        _LIBC_SYS_GETOSCODENAME,          // int errno (const char **ref)
        _LIBC_SYS_GETKERNELNAME,          // int errno (const char **ref)
        _LIBC_SYS_GETKERNELVER,           // int errno (const char **ref)
        _LIBC_SYS_GETHOSTNAME,            // int errno (char *buf, size_t *buf_len)
        _LIBC_SYS_SETHOSTNAME,            // int errno (char *hostname)
        _LIBC_SYS_GETDRIVERNAME,          // int errno (size_t *modno, const char **name_ref)
        _LIBC_SYS_GETDRIVERID,            // int errno (const char *name, size_t *driver_id)
        _LIBC_SYS_GETDRIVERCOUNT,         // int errno (size_t *count)
        _LIBC_SYS_GETDRIVERINSTANCES,     // int errno (size_t id, size_t *count)
        _LIBC_SYS_SYSTEMRESTART,          // int errno (void)
        _LIBC_SYS_SYSTEMSHUTDOWN,         // int errno (void)
        _LIBC_SYS_SYSLOGCLEAR,            // int errno (void)
        _LIBC_SYS_GETACTIVETHREAD,        // int errno (tid_t *tid)
        _LIBC_SYS_THREADEXIT,             // int errno (int *status)
        _LIBC_SYS_SCHEDULERLOCK,          // int errno (void)
        _LIBC_SYS_SCHEDULERUNLOCK,        // int errno (void)
        _LIBC_SYS_THREADJOIN,             // int errno (const tid_t *tid , int *status, const uint32_t *timeout)
        _LIBC_SYS_ISHEAPADDR,             // int errno (const void *addr)
} _libc_syscall_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern int _libc_syscall(_libc_syscall_t syscall, ...);

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
