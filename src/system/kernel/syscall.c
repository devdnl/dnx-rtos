/*=========================================================================*//**
@file    syscall.c

@author  Daniel Zorychta

@brief   System call handling - kernel space

@note    Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Include files
==============================================================================*/
#include <string.h>
#include "config.h"
#include "fs/fsctrl.h"
#include "fs/vfs.h"
#include "drivers/drvctrl.h"
#include "kernel/syscall.h"
#include "kernel/kwrapper.h"
#include "kernel/printk.h"
#include "kernel/kpanic.h"
#include "kernel/errno.h"
#include "kernel/time.h"
#include "kernel/khooks.h"
#include "kernel/sysfunc.h"
#include "lib/sys/types.h"
#include "lib/cast.h"
#include "lib/unarg.h"
#include "lib/strlcat.h"
#include "lib/strlcpy.h"
#include "net/netm.h"
#include "mm/shm.h"
#include "lib/misc.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define SYSCALL_QUEUE_LENGTH            4

#define FS_CACHE_SYNC_PERIOD_MS         (1000 * __OS_SYSTEM_CACHE_SYNC_PERIOD__)

#define GETARG(type, var)               type var = va_arg(rq->args, type)
#define LOADARG(type)                   va_arg(rq->args, type)
#define GETRETURN(type, var)            type var = rq->retptr
#define GETTASKHDL()                    rq->task
#define GETPROCESS()                    (rq->client_proc)
#define GETTHREAD(_tid)                 _process_thread_get_container(GETPROCESS(), _tid)
#define UNUSED_RQ()                     UNUSED_ARG1(rq)

#define is_proc_valid(proc)             (_mm_is_object_in_heap(proc) && ((res_header_t*)proc)->type == RES_TYPE_PROCESS)
#define is_tid_in_range(proc, tid)      (tid < _process_get_max_threads(proc))

/*==============================================================================
  Local object types
==============================================================================*/
// SYSCALLS
typedef enum {
        SYSCALL_GETRUNTIMECTX,          // int errno (_dnxrtctx_t *ctx)
        SYSCALL_MALLOC,                 // int errno (size_t *blk_size, void **alloc_mem)
        SYSCALL_ZALLOC,                 // int errno (size_t *blk_size, void **alloc_mem)
        SYSCALL_FREE,                   // int errno (void *mem)
        SYSCALL_SHMCREATE,              // int errno (const char *key, size_t *size)
        SYSCALL_SHMATTACH,              // int errno (const char *key, void **mem, size_t *size)
        SYSCALL_SHMDETACH,              // int errno (const char *key)
        SYSCALL_SHMDESTROY,             // int errno (const char *key)
        SYSCALL_PROCESSWAIT,            // int errno (const pid_t *pid, int *status, const uint32_t *timeout)
        SYSCALL_PROCESSSTATSEEK,        // int errno (size_t *seek, _process_stat_t *stat)
        SYSCALL_THREADSTAT,             // int errno (pid_t *pid , tid_t *tid, _thread_stat_t *stat)
        SYSCALL_PROCESSSTATPID,         // int errno (pid_t *pid, _process_stat_t *stat)
        SYSCALL_PROCESSGETPID,          // int errno (pid_t *pid)
        SYSCALL_PROCESSGETPRIO,         // int errno (pid_t *pid, int *priority)
        SYSCALL_GETCWD,                 // int errno (char *buf, size_t *size)
        SYSCALL_SETCWD,                 // int errno (const char *cwd)
        SYSCALL_SYSLOGREAD,             // int errno (char *str, size_t *len, const struct timeval *from, struct timeval *current, size_t *count)
        SYSCALL_THREADCREATE,           // int errno (thread_func_t, _thread_attr_t *attr, void *arg, tid_t *tid)
        SYSCALL_SEMAPHOREOPEN,          // int errno (const size_t *cnt_max, const size_t *cnt_init, int *fd)
        SYSCALL_SEMAPHOREWAIT,          // int errno (int *fd_semaphore, uint32_t *timeout)
        SYSCALL_SEMAPHORESIGNAL,        // int errno (int *fd_semaphore)
        SYSCALL_SEMAPHOREGETVALUE,      // int errno (int *fd_semaphore, size_t *value)
        SYSCALL_MUTEXOPEN,              // int errno (const enum kmtx_type *mt, int *fd)
        SYSCALL_MUTEXLOCK,              // int errno (int *mutex, uint32_t *timeout)
        SYSCALL_MUTEXUNLOCK,            // int errno (int *mutex)
        SYSCALL_CLOSE,                  // int errno (int *fd)
        SYSCALL_QUEUEOPEN,              // int errno (const size_t *length, const size_t *item_size, int *fd)
        SYSCALL_QUEUERESET,             // int errno (int *fd_queue)
        SYSCALL_QUEUESEND,              // int errno (int *fd_queue, const void *item, const uint32_t *timeout)
        SYSCALL_QUEUERECEIVE,           // int errno (int *fd_queue, void *item, const uint32_t *timeout)
        SYSCALL_QUEUERECEIVEPEEK,       // int errno (int *fd_queue, void *item, const uint32_t *timeout)
        SYSCALL_QUEUEITEMSCOUNT,        // int errno (int *fd_queue, size_t *count)
        SYSCALL_QUEUEFREESPACE,         // int errno (int *fd_queue, size_t *count)
        SYSCALL_THREADKILL,             // int errno (tid_t *tid)
        SYSCALL_PROCESSCREATE,          // int errno (const char *command, _process_attr_t *attr, pid_t *pid)
        SYSCALL_PROCESSKILL,            // int errno (const pid_t *pid)
        SYSCALL_PROCESSABORT,           // int errno (void)
        SYSCALL_PROCESSEXIT,            // int errno (int *status)
        SYSCALL_MOUNT,                  // int errno (const char *FS_name, const char *src_path, const char *mount_point, const char *options)
        SYSCALL_UMOUNT,                 // int errno (const char *mount_point)
        SYSCALL_MKNOD,                  // int errno (const char *pathname, const char *mod_name, int *major, int *minor)
        SYSCALL_GETMNTENTRY,            // int errno (size_t *seek, struct mntent *mntent)
        SYSCALL_MKFIFO,                 // int errno (const char *pathname, mode_t *mode)
        SYSCALL_MKDIR,                  // int errno (const char *pathname, mode_t *mode)
        SYSCALL_OPENDIR,                // int errno (const char *pathname, DIR**)
        SYSCALL_CLOSEDIR,               // int errno (DIR *dir)
        SYSCALL_DIRSEEK,                // int errno (DIR *dir, const uint32_t *seek)
        SYSCALL_DIRTELL,                // int errno (DIR *dir, u32_t *pos)
        SYSCALL_READDIR,                // int errno (DIR *dir, dirent_t *dirent)
        SYSCALL_REMOVE,                 // int errno (const char *path)
        SYSCALL_RENAME,                 // int errno (const char *old_name, const char *new_name)
        SYSCALL_CHMOD,                  // int errno (const char *pathname, mode_t *mode)
        SYSCALL_CHOWN,                  // int errno (const char *pathname, uid_t *owner, gid_t *group)
        SYSCALL_STATFS,                 // int errno (const char *path, struct statfs *buf)
        SYSCALL_STAT,                   // int errno (const char *pathname, struct stat *buf)
        SYSCALL_FSTAT,                  // int errno (int *fd, struct stat *buf)
        SYSCALL_OPEN,                   // int errno (int *fd, const char *path, int flags)
        SYSCALL_WRITE,                  // int errno (int *fd, const void *src, size_t *count, size_t *written)
        SYSCALL_READ,                   // int errno (int *fd, void *dst, size_t *count, size_t *read)
        SYSCALL_SEEK64,                 // int errno (int *fd, i64_t *seek, int *origin)
        SYSCALL_IOCTL,                  // int errno (int *fd, int *request, va_list *arg)
        SYSCALL_FLUSH,                  // int errno (int *fd)
        SYSCALL_SYNC,                   // int errno (void)
        SYSCALL_GETTIMEOFDAY,           // int errno (struct timeval *tv, struct timezone *tz)
        SYSCALL_SETTIMEOFDAY,           // int errno (const struct timeval *tv, const struct timezone *tz)
        SYSCALL_DRIVERINIT,             // int errno (const char *mod_name, int *major, int *minor  const char *node_path, const void *config)
        SYSCALL_DRIVERRELEASE,          // int errno (const char *mod_name, int *major, int *minor)
        SYSCALL_KERNELPANICINFO,        // int errno (kernel_panic_info_t *info)
        SYSCALL_NETADD,                 // int errno (char *netname, NET_family_t *family, const char *if_path)
        SYSCALL_NETRM,                  // int errno (char *netname)
        SYSCALL_NETIFLIST,              // int errno (char *netname[], size_t *netname_len, size_t *count)
        SYSCALL_NETIFUP,                // int errno (const char *netname, const NET_generic_config_t *config)
        SYSCALL_NETIFDOWN,              // int errno (const char *netname)
        SYSCALL_NETIFSTATUS,            // int errno (const char *netname, NET_family_t *family, NET_generic_status_t *status)
        SYSCALL_NETSOCKETCREATE,        // int errno (const char *netname, NET_protocol_t *protocol, SOCKET**)
        SYSCALL_NETGETHOSTBYNAME,       // int errno (const char *netname, const char *name, void *addr)
        SYSCALL_NETSOCKETDESTROY,       // int errno (SOCKET *socket)
        SYSCALL_NETBIND,                // int errno (SOCKET *socket, const NET_generic_sockaddr_t *addr)
        SYSCALL_NETLISTEN,              // int errno (SOCKET *socket)
        SYSCALL_NETACCEPT,              // int errno (SOCKET *socket, SOCKET **new_socket)
        SYSCALL_NETRECV,                // int errno (SOCKET *socket, void *buf, size_t *len, NET_flags_t *flags, size_t *rcved)
        SYSCALL_NETSEND,                // int errno (SOCKET *socket, const void *buf,size_t *len, NET_flags_t *flags, size_t *sent)
        SYSCALL_NETSETRECVTIMEOUT,      // int errno (SOCKET *socket, uint32_t *timeout)
        SYSCALL_NETSETSENDTIMEOUT,      // int errno (SOCKET *socket, uint32_t *timeout)
        SYSCALL_NETCONNECT,             // int errno (SOCKET *socket, const NET_generic_sockaddr_t *addr)
        SYSCALL_NETDISCONNECT,          // int errno (SOCKET *socket)
        SYSCALL_NETSHUTDOWN,            // int errno (SOCKET *socket, NET_shut_t *how)
        SYSCALL_NETSENDTO,              // int errno (SOCKET *socket, const void *buf, size_t *len, NET_flags_t *flags, const NET_generic_sockaddr_t *to_sockaddr, size_t *sent)
        SYSCALL_NETRECVFROM,            // int errno (SOCKET *socket, void *buf, size_t *len, NET_flags_t *flags, NET_generic_sockaddr_t *from_sockaddr, size_t *rcved)
        SYSCALL_NETGETADDRESS,          // int errno (SOCKET *socket, NET_generic_sockaddr_t *addr)
        SYSCALL_NETHTON16,              // int errno (NET_family_t *family, uint16_t *value_in, uint16_t *value_out)
        SYSCALL_NETHTON32,              // int errno (NET_family_t *family, uint32_t *value_in, uint32_t *value_out)
        SYSCALL_NETHTON64,              // int errno (NET_family_t *family, uint64_t *value_in, uint64_t *value_out)
        SYSCALL_MSLEEP,                 // int errno (const uint32_t *mseconds)
        SYSCALL_USLEEP,                 // int errno (const uint32_t *useconds)
        SYSCALL_GETUID,                 // int errno (uid_t *uid)
        SYSCALL_GETGID,                 // int errno (gid_t *gid)
        SYSCALL_GETMEMDETAILS,          // int errno (memstat_t *stat)
        SYSCALL_GETMODMEMUSAGE,         // int errno (uint *module, int32_t *usage)
        SYSCALL_GETUPTIMEMS,            // int errno (uint64_t *uptime_ms)
        SYSCALL_GETAVGCPULOAD,          // int errno (_avg_CPU_load_t *stat)
        SYSCALL_GETPLATFORMNAME,        // int errno (const char **ref)
        SYSCALL_GETOSNAME,              // int errno (const char **ref)
        SYSCALL_GETOSVER,               // int errno (const char **ref)
        SYSCALL_GETOSCODENAME,          // int errno (const char **ref)
        SYSCALL_GETKERNELNAME,          // int errno (const char **ref)
        SYSCALL_GETKERNELVER,           // int errno (const char **ref)
        SYSCALL_GETHOSTNAME,            // int errno (char *buf, size_t *buf_len)
        SYSCALL_SETHOSTNAME,            // int errno (char *hostname)
        SYSCALL_GETDRIVERNAME,          // int errno (size_t *modno, const char **name_ref)
        SYSCALL_GETDRIVERID,            // int errno (const char *name, size_t *driver_id)
        SYSCALL_GETDRIVERCOUNT,         // int errno (size_t *count)
        SYSCALL_GETDRIVERINSTANCES,     // int errno (size_t id, size_t *count)
        SYSCALL_SYSTEMRESTART,          // int errno (void)
        SYSCALL_SYSTEMSHUTDOWN,         // int errno (void)
        SYSCALL_SYSLOGCLEAR,            // int errno (void)
        SYSCALL_GETACTIVETHREAD,        // int errno (tid_t *tid)
        SYSCALL_THREADEXIT,             // int errno (int *status)
        SYSCALL_SCHEDULERLOCK,          // int errno (void)
        SYSCALL_SCHEDULERUNLOCK,        // int errno (void)
        SYSCALL_THREADJOIN,             // int errno (const tid_t *tid , int *status, const uint32_t *timeout)
        SYSCALL_ISHEAPADDR,             // int errno (const void *addr)
        _SYSCALL_COUNT,
} syscall_t;

typedef struct {
        _process_t *client_proc;
        tid_t       client_thread;
        syscall_t   syscall_no;
        va_list     args;
} syscallrq_t;

typedef int (*syscallfunc_t)(syscallrq_t*);

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int syscall_do(void *rq);

static int syscall_getruntimectx(syscallrq_t *rq);
static int syscall_mount(syscallrq_t *rq);
static int syscall_umount(syscallrq_t *rq);
static int syscall_getmntentry(syscallrq_t *rq);
static int syscall_mknod(syscallrq_t *rq);
static int syscall_mkdir(syscallrq_t *rq);
static int syscall_mkfifo(syscallrq_t *rq);
static int syscall_opendir(syscallrq_t *rq);
static int syscall_closedir(syscallrq_t *rq);
static int syscall_readdir(syscallrq_t *rq);
static int syscall_remove(syscallrq_t *rq);
static int syscall_rename(syscallrq_t *rq);
static int syscall_chmod(syscallrq_t *rq);
static int syscall_chown(syscallrq_t *rq);
static int syscall_statfs(syscallrq_t *rq);
static int syscall_stat(syscallrq_t *rq);
static int syscall_fstat(syscallrq_t *rq);
static int syscall_open(syscallrq_t *rq);
static int syscall_close(syscallrq_t *rq);
static int syscall_write(syscallrq_t *rq);
static int syscall_read(syscallrq_t *rq);
static int syscall_seek64(syscallrq_t *rq);
static int syscall_ioctl(syscallrq_t *rq);
static int syscall_flush(syscallrq_t *rq);
static int syscall_sync(syscallrq_t *rq);
static int syscall_gettimeofday(syscallrq_t *rq);
static int syscall_settimeofday(syscallrq_t *rq);
static int syscall_driverinit(syscallrq_t *rq);
static int syscall_driverrelease(syscallrq_t *rq);
static int syscall_malloc(syscallrq_t *rq);
static int syscall_zalloc(syscallrq_t *rq);
static int syscall_free(syscallrq_t *rq);
static int syscall_syslogread(syscallrq_t *rq);
static int syscall_kernelpanicinfo(syscallrq_t *rq);
static int syscall_processcreate(syscallrq_t *rq);
static int syscall_processkill(syscallrq_t *rq);
static int syscall_processwait(syscallrq_t *rq);
static int syscall_processstatseek(syscallrq_t *rq);
static int syscall_processstatpid(syscallrq_t *rq);
static int syscall_processgetpid(syscallrq_t *rq);
static int syscall_processgetprio(syscallrq_t *rq);
static int syscall_processabort(syscallrq_t *rq);
static int syscall_processexit(syscallrq_t *rq);
static int syscall_threadstat(syscallrq_t *rq);
static int syscall_getcwd(syscallrq_t *rq);
static int syscall_setcwd(syscallrq_t *rq);
static int syscall_threadcreate(syscallrq_t *rq);
static int syscall_threadkill(syscallrq_t *rq);
static int syscall_semaphoreopen(syscallrq_t *rq);
static int syscall_mutexopen(syscallrq_t *rq);
static int syscall_queueopen(syscallrq_t *rq);
static int syscall_netifadd(syscallrq_t *rq);
static int syscall_netifrm(syscallrq_t *rq);
static int syscall_netiflist(syscallrq_t *rq);
static int syscall_netifup(syscallrq_t *rq);
static int syscall_netifdown(syscallrq_t *rq);
static int syscall_netifstatus(syscallrq_t *rq);
static int syscall_netsocketcreate(syscallrq_t *rq);
static int syscall_netsocketdestroy(syscallrq_t *rq);
static int syscall_netbind(syscallrq_t *rq);
static int syscall_netlisten(syscallrq_t *rq);
static int syscall_netaccept(syscallrq_t *rq);
static int syscall_netrecv(syscallrq_t *rq);
static int syscall_netsend(syscallrq_t *rq);
static int syscall_netgethostbyname(syscallrq_t *rq);
static int syscall_netsetrecvtimeout(syscallrq_t *rq);
static int syscall_netsetsendtimeout(syscallrq_t *rq);
static int syscall_netconnect(syscallrq_t *rq);
static int syscall_netdisconnect(syscallrq_t *rq);
static int syscall_netshutdown(syscallrq_t *rq);
static int syscall_netsendto(syscallrq_t *rq);
static int syscall_netrecvfrom(syscallrq_t *rq);
static int syscall_netgetaddress(syscallrq_t *rq);
static int syscall_nethton16(syscallrq_t *rq);
static int syscall_nethton32(syscallrq_t *rq);
static int syscall_nethton64(syscallrq_t *rq);
static int syscall_shmcreate(syscallrq_t *rq);
static int syscall_shmattach(syscallrq_t *rq);
static int syscall_shmdetach(syscallrq_t *rq);
static int syscall_shmdestroy(syscallrq_t *rq);
static int syscall_msleep(syscallrq_t *rq);
static int syscall_usleep(syscallrq_t *rq);
static int syscall_getgid(syscallrq_t *rq);
static int syscall_getuid(syscallrq_t *rq);
static int syscall_getmemdetails(syscallrq_t *rq);
static int syscall_getmodmemusage(syscallrq_t *rq);
static int syscall_getuptimems(syscallrq_t *rq);
static int syscall_getavgcpuload(syscallrq_t *rq);
static int syscall_getplatformname(syscallrq_t *rq);
static int syscall_getosname(syscallrq_t *rq);
static int syscall_getosver(syscallrq_t *rq);
static int syscall_getoscodename(syscallrq_t *rq);
static int syscall_getkernelname(syscallrq_t *rq);
static int syscall_getkernelver(syscallrq_t *rq);
static int syscall_gethostname(syscallrq_t *rq);
static int syscall_sethostname(syscallrq_t *rq);
static int syscall_getdrivername(syscallrq_t *rq);
static int syscall_getdriverid(syscallrq_t *rq);
static int syscall_getdrivercount(syscallrq_t *rq);
static int syscall_getdriverinstances(syscallrq_t *rq);
static int syscall_systemrestart(syscallrq_t *rq);
static int syscall_systemshutdown(syscallrq_t *rq);
static int syscall_syslogclear(syscallrq_t *rq);
static int syscall_getactivethread(syscallrq_t *rq);
static int syscall_threadexit(syscallrq_t *rq);
static int syscall_semaphorewait(syscallrq_t *rq);
static int syscall_semaphoresignal(syscallrq_t *rq);
static int syscall_semaphoregetvalue(syscallrq_t *rq);
static int syscall_mutexlock(syscallrq_t *rq);
static int syscall_mutexunlock(syscallrq_t *rq);
static int syscall_queuereset(syscallrq_t *rq);
static int syscall_queuesend(syscallrq_t *rq);
static int syscall_queuereceive(syscallrq_t *rq);
static int syscall_queuereceviepeek(syscallrq_t *rq);
static int syscall_queueitemscount(syscallrq_t *rq);
static int syscall_queuefreespace(syscallrq_t *rq);
static int syscall_dirtell(syscallrq_t *rq);
static int syscall_dirseek(syscallrq_t *rq);
static int syscall_schedulerlock(syscallrq_t *rq);
static int syscall_schedulerunlock(syscallrq_t *rq);
static int syscall_threadjoin(syscallrq_t *rq);
static int syscall_isheapaddr(syscallrq_t *rq);

/*==============================================================================
  Local objects
==============================================================================*/
/* syscall table */
static const syscallfunc_t syscalltab[] = {
        [SYSCALL_GETRUNTIMECTX] = syscall_getruntimectx,
        [SYSCALL_MOUNT] = syscall_mount,
        [SYSCALL_UMOUNT] = syscall_umount,
        [SYSCALL_SHMCREATE] = syscall_shmcreate,
        [SYSCALL_SHMATTACH] = syscall_shmattach,
        [SYSCALL_SHMDETACH] = syscall_shmdetach,
        [SYSCALL_SHMDESTROY] = syscall_shmdestroy,
        [SYSCALL_GETMNTENTRY] = syscall_getmntentry,
        [SYSCALL_MKNOD] = syscall_mknod,
        [SYSCALL_MKDIR] = syscall_mkdir,
        [SYSCALL_MKFIFO] = syscall_mkfifo,
        [SYSCALL_OPENDIR] = syscall_opendir,
        [SYSCALL_CLOSEDIR] = syscall_closedir,
        [SYSCALL_CLOSE] = syscall_close,
        [SYSCALL_READDIR] = syscall_readdir,
        [SYSCALL_REMOVE] = syscall_remove,
        [SYSCALL_RENAME] = syscall_rename,
        [SYSCALL_CHMOD] = syscall_chmod,
        [SYSCALL_CHOWN] = syscall_chown,
        [SYSCALL_STATFS] = syscall_statfs,
        [SYSCALL_STAT] = syscall_stat,
        [SYSCALL_FSTAT] = syscall_fstat,
        [SYSCALL_OPEN] = syscall_open,
        [SYSCALL_WRITE] = syscall_write,
        [SYSCALL_READ] = syscall_read,
        [SYSCALL_SEEK64] = syscall_seek64,
        [SYSCALL_IOCTL] = syscall_ioctl,
        [SYSCALL_FLUSH] = syscall_flush,
        [SYSCALL_SYNC] = syscall_sync,
        [SYSCALL_GETTIMEOFDAY] = syscall_gettimeofday,
        [SYSCALL_SETTIMEOFDAY] = syscall_settimeofday,
        [SYSCALL_DRIVERINIT] = syscall_driverinit,
        [SYSCALL_DRIVERRELEASE] = syscall_driverrelease,
        [SYSCALL_MALLOC] = syscall_malloc,
        [SYSCALL_ZALLOC] = syscall_zalloc,
        [SYSCALL_FREE] = syscall_free,
        [SYSCALL_SYSLOGREAD] = syscall_syslogread,
        [SYSCALL_KERNELPANICINFO] = syscall_kernelpanicinfo,
        [SYSCALL_PROCESSCREATE] = syscall_processcreate,
        [SYSCALL_PROCESSKILL] = syscall_processkill,
        [SYSCALL_PROCESSWAIT] = syscall_processwait,
        [SYSCALL_PROCESSSTATSEEK] = syscall_processstatseek,
        [SYSCALL_PROCESSSTATPID] = syscall_processstatpid,
        [SYSCALL_PROCESSGETPID] = syscall_processgetpid,
        [SYSCALL_PROCESSGETPRIO] = syscall_processgetprio,
        [SYSCALL_PROCESSABORT] = syscall_processabort,
        [SYSCALL_PROCESSEXIT] = syscall_processexit,
        [SYSCALL_THREADSTAT] = syscall_threadstat,
        [SYSCALL_GETCWD] = syscall_getcwd,
        [SYSCALL_SETCWD] = syscall_setcwd,
        [SYSCALL_THREADCREATE] = syscall_threadcreate,
        [SYSCALL_THREADKILL] = syscall_threadkill,
        [SYSCALL_SEMAPHOREOPEN] = syscall_semaphoreopen,
        [SYSCALL_MUTEXOPEN] = syscall_mutexopen,
        [SYSCALL_QUEUEOPEN] = syscall_queueopen,
        [SYSCALL_NETADD] = syscall_netifadd,
        [SYSCALL_NETRM] = syscall_netifrm,
        [SYSCALL_NETIFLIST] = syscall_netiflist,
        [SYSCALL_NETIFUP] = syscall_netifup,
        [SYSCALL_NETIFDOWN] = syscall_netifdown,
        [SYSCALL_NETIFSTATUS] = syscall_netifstatus,
        [SYSCALL_NETSOCKETCREATE] = syscall_netsocketcreate,
        [SYSCALL_NETSOCKETDESTROY] = syscall_netsocketdestroy,
        [SYSCALL_NETBIND] = syscall_netbind,
        [SYSCALL_NETLISTEN] = syscall_netlisten,
        [SYSCALL_NETACCEPT] = syscall_netaccept,
        [SYSCALL_NETRECV] = syscall_netrecv,
        [SYSCALL_NETSEND] = syscall_netsend,
        [SYSCALL_NETGETHOSTBYNAME] = syscall_netgethostbyname,
        [SYSCALL_NETSETRECVTIMEOUT] = syscall_netsetrecvtimeout,
        [SYSCALL_NETSETSENDTIMEOUT] = syscall_netsetsendtimeout,
        [SYSCALL_NETCONNECT] = syscall_netconnect,
        [SYSCALL_NETDISCONNECT] = syscall_netdisconnect,
        [SYSCALL_NETSHUTDOWN] = syscall_netshutdown,
        [SYSCALL_NETSENDTO] = syscall_netsendto,
        [SYSCALL_NETRECVFROM] = syscall_netrecvfrom,
        [SYSCALL_NETGETADDRESS] = syscall_netgetaddress,
        [SYSCALL_NETHTON16] = syscall_nethton16,
        [SYSCALL_NETHTON32] = syscall_nethton32,
        [SYSCALL_NETHTON64] = syscall_nethton64,
        [SYSCALL_MSLEEP] = syscall_msleep,
        [SYSCALL_USLEEP] = syscall_usleep,
        [SYSCALL_GETGID] = syscall_getgid,
        [SYSCALL_GETUID] = syscall_getuid,
        [SYSCALL_GETMEMDETAILS] = syscall_getmemdetails,
        [SYSCALL_GETMODMEMUSAGE] = syscall_getmodmemusage,
        [SYSCALL_GETUPTIMEMS] = syscall_getuptimems,
        [SYSCALL_GETAVGCPULOAD] = syscall_getavgcpuload,
        [SYSCALL_GETPLATFORMNAME] = syscall_getplatformname,
        [SYSCALL_GETOSNAME] = syscall_getosname,
        [SYSCALL_GETOSVER] = syscall_getosver,
        [SYSCALL_GETOSCODENAME] = syscall_getoscodename,
        [SYSCALL_GETKERNELNAME] = syscall_getkernelname,
        [SYSCALL_GETKERNELVER] = syscall_getkernelver,
        [SYSCALL_GETHOSTNAME] = syscall_gethostname,
        [SYSCALL_SETHOSTNAME] = syscall_sethostname,
        [SYSCALL_GETDRIVERNAME] = syscall_getdrivername,
        [SYSCALL_GETDRIVERID] = syscall_getdriverid,
        [SYSCALL_GETDRIVERCOUNT] = syscall_getdrivercount,
        [SYSCALL_GETDRIVERINSTANCES] = syscall_getdriverinstances,
        [SYSCALL_SYSTEMRESTART] = syscall_systemrestart,
        [SYSCALL_SYSTEMSHUTDOWN] = syscall_systemshutdown,
        [SYSCALL_SYSLOGCLEAR] = syscall_syslogclear,
        [SYSCALL_GETACTIVETHREAD] = syscall_getactivethread,
        [SYSCALL_THREADEXIT] = syscall_threadexit,
        [SYSCALL_SEMAPHOREWAIT] = syscall_semaphorewait,
        [SYSCALL_SEMAPHORESIGNAL] = syscall_semaphoresignal,
        [SYSCALL_SEMAPHOREGETVALUE] = syscall_semaphoregetvalue,
        [SYSCALL_MUTEXLOCK] = syscall_mutexlock,
        [SYSCALL_MUTEXUNLOCK] = syscall_mutexunlock,
        [SYSCALL_QUEUERESET] = syscall_queuereset,
        [SYSCALL_QUEUESEND] = syscall_queuesend,
        [SYSCALL_QUEUERECEIVE] = syscall_queuereceive,
        [SYSCALL_QUEUERECEIVEPEEK] = syscall_queuereceviepeek,
        [SYSCALL_QUEUEITEMSCOUNT] = syscall_queueitemscount,
        [SYSCALL_QUEUEFREESPACE] = syscall_queuefreespace,
        [SYSCALL_DIRSEEK] = syscall_dirseek,
        [SYSCALL_DIRTELL] = syscall_dirtell,
        [SYSCALL_SCHEDULERLOCK] = syscall_schedulerlock,
        [SYSCALL_SCHEDULERUNLOCK] = syscall_schedulerunlock,
        [SYSCALL_THREADJOIN] = syscall_threadjoin,
        [SYSCALL_ISHEAPADDR] = syscall_isheapaddr,
};

/*==============================================================================
  Exported objects
==============================================================================*/
_process_t *_kworker_proc;
const char *const dnx_RTOS_version = "3.0.1";
const char *const dnx_RTOS_name = "dnx RTOS";
const char *const dnx_RTOS_codename = "Hawk";
const char *const dnx_RTOS_platform_name = _CPUCTL_PLATFORM_NAME;

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief  Initialize system calls.
 */
//==============================================================================
int _syscall_init()
{
        static const _process_attr_t attr = {
                .fd_stdin  = -1,
                .fd_stdout = -1,
                .fd_stderr = -1,
                .p_stdin   = NULL,
                .p_stdout  = NULL,
                .p_stderr  = NULL,
                .cwd       = "/",
                .priority  = _PRIORITY_NORMAL,
                .detached  = true
        };

        int err = _process_create("kworker", &attr, NULL);
        if (!err) {
                err = _process_create(__OS_INIT_PROG__, &attr, NULL);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function call selected syscall [USERSPACE].
 *
 * @note   This function is accessible at address: 0x08000400 if start vector is set to 0x08000000.
 *
 * @param  syscall      syscall number
 * @param  ...          additional arguments
 *
 * @return Operation status (errno value).
 */
//==============================================================================
__attribute__((section (".syscall"))) int syscall(syscall_t syscall, va_list args)
{
        int err = ENOSYS;

        if (syscall < _SYSCALL_COUNT) {
                _process_clean_up_killed_processes();

                syscallrq_t syscallrq = {
                        .syscall_no = syscall,
                        .args       = args,
                };

                _task_get_process_container(_THIS_TASK, &syscallrq.client_proc, &syscallrq.client_thread);
                _assert(syscallrq.client_proc);
                _assert(is_tid_in_range(syscallrq.client_proc, syscallrq.client_thread));

                err = syscall_do(&syscallrq);
                if (err) _errno = err;
        }

        return err;
}

//==============================================================================
/**
 * @brief  Main syscall process (master) [KERNELSPACE].
 *
 * @param  argc         argument count
 * @param  argv         arguments
 *
 * @return Never exit (0)
 */
//==============================================================================
int _syscall_kworker_process(int argc, char *argv[])
{
        UNUSED_ARG2(argc, argv);

        _task_get_process_container(_THIS_TASK, &_kworker_proc, NULL);
        _assert(_kworker_proc);

        u64_t sync_period_ref = _kernel_get_time_ms();

        for (;;) {
                _sleep_ms(1000);

                if ( (_kernel_get_time_ms() - sync_period_ref) >= FS_CACHE_SYNC_PERIOD_MS) {
                        _vfs_sync();
                        sync_period_ref = _kernel_get_time_ms();
                }
        }

        return -1;
}

//==============================================================================
/**
 * @brief  Function is called in thread and realize requested syscall.
 *
 * @param  rq           request information
 *
 * @return Operation status (errno value).
 */
//==============================================================================
static int syscall_do(void *rq)
{
        syscallrq_t *sysrq = rq;

        _process_syscall_stat_inc(sysrq->client_proc, _kworker_proc);

        _process_enter_kernelspace(sysrq->client_proc, sysrq->syscall_no);
        int err = syscalltab[sysrq->syscall_no](sysrq);
        _process_exit_kernelspace(sysrq->client_proc);

        return err;
}

//==============================================================================
/**
 * @brief  This syscall return dnx RTOS runtime context.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_getruntimectx(syscallrq_t *rq)
{
        GETARG(_dnxrtctx_t*, ctx);

        ctx->global_ref  = &_global;
        ctx->errno_ref   = &_errno;
        ctx->app_ctx_ref = &_app_ctx;

        return 0;
}

//==============================================================================
/**
 * @brief  This syscall mount selected file system to selected path.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_mount(syscallrq_t *rq)
{
        GETARG(const char *, FS_name);

        struct vfs_path src_path;
        src_path.CWD  = _process_get_CWD(GETPROCESS());
        src_path.PATH = LOADARG(const char *);

        struct vfs_path mount_point;
        mount_point.CWD  = src_path.CWD;
        mount_point.PATH = LOADARG(const char *);

        GETARG(const char *, opts);

        return _mount(FS_name, &src_path, &mount_point, opts);
}

//==============================================================================
/**
 * @brief  This syscall unmount selected file system.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_umount(syscallrq_t *rq)
{
        struct vfs_path mount_point;
        mount_point.CWD  = _process_get_CWD(GETPROCESS());
        mount_point.PATH = LOADARG(const char *);

        return _umount(&mount_point);
}

//==============================================================================
/**
 * @brief  This syscall return information about selected file system.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_getmntentry(syscallrq_t *rq)
{
#if __OS_ENABLE_STATFS__ == _YES_
        GETARG(int *, seek);
        GETARG(struct mntent *, mntent);

        return _vfs_getmntentry(*seek, mntent);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall create device node.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_mknod(syscallrq_t *rq)
{
#if __OS_ENABLE_MKNOD__ == _YES_
        struct vfs_path pathname;
        pathname.CWD  = _process_get_CWD(GETPROCESS());
        pathname.PATH = LOADARG(const char *);
        GETARG(const char *, mod_name);
        GETARG(int *, major);
        GETARG(int *, minor);

        return _vfs_mknod(&pathname, _dev_t__create(_module_get_ID(mod_name), *major, *minor));
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall create directory.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_mkdir(syscallrq_t *rq)
{
#if __OS_ENABLE_MKDIR__ == _YES_
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);
        GETARG(mode_t *, mode);

        return _vfs_mkdir(&path, *mode);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall create FIFO pipe.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_mkfifo(syscallrq_t *rq)
{
#if __OS_ENABLE_MKFIFO__ == _YES_
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);
        GETARG(mode_t *, mode);

        return _vfs_mkfifo(&path, *mode);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall open selected directory.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_opendir(syscallrq_t *rq)
{
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);
        GETARG(kdir_t**, DIR);

        kdir_t *dir = NULL;
        int  err = _vfs_opendir(&path, &dir);
        if (!err) {
                err = _process_register_resource(GETPROCESS(), cast(res_header_t*, dir));
                if (err) {
                        _vfs_closedir(dir);
                        dir = NULL;
                } else {
                        *DIR = dir;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall close selected directory.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_closedir(syscallrq_t *rq)
{
        GETARG(kdir_t *, dir);

        int err = _process_release_resource(GETPROCESS(), cast(res_header_t*, dir), RES_TYPE_DIR);
        if (err == EFAULT) {
                res_header_t *res;
                if (_process_descriptor_get_resource(GETPROCESS(), 2, &res) == 0) {
                        const char *msg = "*** Error: object is not a dir! ***\n";
                        size_t wrcnt;
                        _vfs_fwrite(msg, strlen(msg), &wrcnt, cast(kfile_t*, res));
                }

                pid_t pid = 0;
                _process_get_pid(GETPROCESS(), &pid);
                _process_kill(pid);
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall read selected directory.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_readdir(syscallrq_t *rq)
{
        GETARG(kdir_t *, dir);
        GETARG(dirent_t *, dirent);

        return _vfs_readdir(dir, &dirent);
}

//==============================================================================
/**
 * @brief  This syscall remove selected file.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_remove(syscallrq_t *rq)
{
#if __OS_ENABLE_REMOVE__ == _YES_
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);

        return _vfs_remove(&path);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall rename selected file.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_rename(syscallrq_t *rq)
{
#if __OS_ENABLE_RENAME__ == _YES_
        struct vfs_path oldname;
        oldname.CWD  = _process_get_CWD(GETPROCESS());
        oldname.PATH = LOADARG(const char *);

        struct vfs_path newname;
        newname.CWD  = _process_get_CWD(GETPROCESS());
        newname.PATH = LOADARG(const char *);

        return _vfs_rename(&oldname, &newname);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall change mode of selected file.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_chmod(syscallrq_t *rq)
{
#if __OS_ENABLE_CHMOD__ == _YES_
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);
        GETARG(mode_t *, mode);

        return _vfs_chmod(&path, *mode);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall change owner and group of selected file.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_chown(syscallrq_t *rq)
{
#if __OS_ENABLE_CHOWN__ == _YES_
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);
        GETARG(uid_t *, owner);
        GETARG(gid_t *, group);

        return _vfs_chown(&path, *owner, *group);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall read statistics of selected file by path.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_stat(syscallrq_t *rq)
{
#if __OS_ENABLE_FSTAT__ == _YES_
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);
        GETARG(struct stat *, buf);

        return _vfs_stat(&path, buf);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall read statistics of selected file by FILE object.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_fstat(syscallrq_t *rq)
{
#if __OS_ENABLE_FSTAT__ == _YES_
        GETARG(int *, fd);
        GETARG(struct stat *, buf);

        res_header_t *res;
        int err = _process_descriptor_get_resource(GETPROCESS(), *fd, &res);
        if (!err) {
                if (res->type == RES_TYPE_FILE) {
                        err = _vfs_fstat(cast(kfile_t*, res), buf);
                } else {
                        err = EINVAL;
                }
        }

        return err;
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall read statistics of file system mounted in selected path.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_statfs(syscallrq_t *rq)
{
#if __OS_ENABLE_STATFS__ == _YES_
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);
        GETARG(struct statfs *, buf);

        return _vfs_statfs(&path, buf);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall open selected file.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_open(syscallrq_t *rq)
{
        GETARG(int *, fd);
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);
        GETARG(int *, flags);

        int desc;
        kfile_t *file;
        int err = _vfs_fopen(&path, *flags, &file);
        if (!err) {
                err = _process_descriptor_allocate(GETPROCESS(), &desc, &file->header);
                if (err) {
                        _vfs_fclose(file, true);
                } else {
                        *fd = desc;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall destroy selected descriptor.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_close(syscallrq_t *rq)
{
        GETARG(int *, desc);

        int err = _process_descriptor_free(GETPROCESS(), *desc, RES_TYPE_UNKNOWN);
        if (err) {
                res_header_t *res;
                if (_process_descriptor_get_resource(GETPROCESS(), 2, &res) == 0) {
                        const char *msg = "*** Error: descriptor is not valid! ***\n";
                        size_t wrcnt;
                        _vfs_fwrite(msg, strlen(msg), &wrcnt, cast(kfile_t*, res));
                }

                pid_t pid = 0;
                _process_get_pid(GETPROCESS(), &pid);
                _process_kill(pid);
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall write data to selected file.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_write(syscallrq_t *rq)
{
        GETARG(int *, fd);
        GETARG(const void *, buf);
        GETARG(size_t *, count);
        GETARG(size_t *, wrctr);

        res_header_t *res;
        int err = _process_descriptor_get_resource(GETPROCESS(), *fd, &res);
        if (!err) {
                if (res->type == RES_TYPE_FILE) {
                        err = _vfs_fwrite(buf, *count, wrctr, cast(kfile_t*, res));
                } else {
                        err = EINVAL;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall read data from selected file.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================;
static int syscall_read(syscallrq_t *rq)
{
        GETARG(int *, fd);
        GETARG(void *, buf);
        GETARG(size_t *, count);
        GETARG(size_t *, rdctr);

        res_header_t *res;
        int err = _process_descriptor_get_resource(GETPROCESS(), *fd, &res);
        if (!err) {
                if (res->type == RES_TYPE_FILE) {
                        err = _vfs_fread(buf, *count, rdctr, cast(kfile_t*, res));
                } else {
                        err = EINVAL;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall move file pointer.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================;
static int syscall_seek64(syscallrq_t *rq)
{
        GETARG(int *, fd);
        GETARG(i64_t *, lseek);
        GETARG(int *, orgin);

        res_header_t *res;
        int err = _process_descriptor_get_resource(GETPROCESS(), *fd, &res);
        if (!err) {
                if (res->type == RES_TYPE_FILE) {
                        err = _vfs_fseek(cast(kfile_t*, res), *lseek, *orgin);
                        if (!err) {
                                err = _vfs_ftell(cast(kfile_t*, res), lseek);
                        }
                } else {
                        err = EINVAL;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall perform not standard operation on selected file/device.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_ioctl(syscallrq_t *rq)
{
        GETARG(int *, fd);
        GETARG(int *, request);
        GETARG(va_list *, arg);

        res_header_t *res;
        int err = _process_descriptor_get_resource(GETPROCESS(), *fd, &res);
        if (!err) {
                if (res->type == RES_TYPE_FILE) {
                        err = _vfs_vfioctl(cast(kfile_t*, res), *request, *arg);
                } else {
                        err = EINVAL;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall flush buffers of selected file.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_flush(syscallrq_t *rq)
{
        GETARG(int *, fd);

        res_header_t *res;
        int err = _process_descriptor_get_resource(GETPROCESS(), *fd, &res);
        if (!err) {
                if (res->type == RES_TYPE_FILE) {
                        err = _vfs_fflush(cast(kfile_t*, res));
                } else {
                        err = EINVAL;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall synchronize all buffers of filesystems.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_sync(syscallrq_t *rq)
{
        UNUSED_RQ();
        _vfs_sync();
        return 0;
}

//==============================================================================
/**
 * @brief  This syscall return current time value (UTC timestamp).
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_gettimeofday(syscallrq_t *rq)
{
#if __OS_ENABLE_TIMEMAN__ == _YES_
        GETARG(struct timeval*, timeval);
        GETARG(struct timezone*, tz);

        int err = 0;

        if (timeval) {
                err = _gettime(timeval);
        }

        if (not err and tz) {
                tz->tz_minuteswest = (_ltimeoff_sec / 60);
                tz->tz_dsttime = 0;
        }

        return err;
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall set current system time (UTC timestamp).
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_settimeofday(syscallrq_t *rq)
{
#if __OS_ENABLE_TIMEMAN__ == _YES_
        GETARG(const struct timeval*, tv);
        GETARG(const struct timezone*, tz);

        int err = 0;

        if (tv) {
                err = _settime(&tv->tv_sec);
        }

        if (not err and tz) {
                _ltimeoff_sec = tz->tz_minuteswest * 60;
        }

        return err;
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall initialize selected driver and create node.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_driverinit(syscallrq_t *rq)
{
        GETARG(const char *, mod_name);
        GETARG(int *, major);
        GETARG(int *, minor);
        GETARG(const char *, node_path);
        GETARG(const void *, config);

        dev_t drvid = -1;
        return _driver_init(mod_name, *major, *minor,  node_path, config, &drvid);
}

//==============================================================================
/**
 * @brief  This syscall release selected driver.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_driverrelease(syscallrq_t *rq)
{
        GETARG(const char *, mod_name);
        GETARG(int *, major);
        GETARG(int *, minor);

        return _driver_release(_dev_t__create(_module_get_ID(mod_name), *major, *minor));
}

//==============================================================================
/**
 * @brief  This syscall allocate memory for application.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_malloc(syscallrq_t *rq)
{
        GETARG(size_t *, size);
        GETARG(void **, blk);

        void *mem = NULL;
        int   err = _kmalloc(_MM_PROG, *size, NULL, _MM_FLAG__DMA_CAPABLE, _MM_FLAG__DMA_CAPABLE, &mem);
        if (!err) {
                err = _process_register_resource(GETPROCESS(), mem);
                if (err) {
                        _kfree(_MM_PROG, &mem);
                } else {
                        *blk = &cast(res_header_t*, mem)[1];
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall allocate memory for application and clear allocated block.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_zalloc(syscallrq_t *rq)
{
        GETARG(size_t *, size);
        GETARG(void **, blk);

        void *mem = NULL;
        int   err = _kzalloc(_MM_PROG, *size, NULL, _MM_FLAG__DMA_CAPABLE, _MM_FLAG__DMA_CAPABLE, &mem);
        if (!err) {
                err = _process_register_resource(GETPROCESS(), mem);
                if (err) {
                        _kfree(_MM_PROG, &mem);
                } else {
                        *blk = &cast(res_header_t*, mem)[1];
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall free allocated memory by application.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_free(syscallrq_t *rq)
{
        GETARG(void *, mem);

        int err = _process_release_resource(GETPROCESS(), cast(res_header_t*, mem) - 1, RES_TYPE_MEMORY);
        if (err) {
                res_header_t *res;
                if (_process_descriptor_get_resource(GETPROCESS(), 2, &res) == 0) {
                        const char *msg = "*** Error: double free or corruption ***\n";
                        size_t wrcnt;
                        _vfs_fwrite(msg, strlen(msg), &wrcnt, cast(kfile_t*, res));
                }

                pid_t pid = 0;
                _process_get_pid(GETPROCESS(), &pid);
                _process_kill(pid);
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall read syslog and pass to selected buffer.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_syslogread(syscallrq_t *rq)
{
#if ((__OS_SYSTEM_MSG_ENABLE__ > 0) && (__OS_PRINTF_ENABLE__ > 0))
        GETARG(char *, str);
        GETARG(size_t *, len);
        GETARG(const struct timeval *, from_time);
        GETARG(struct timeval *, curr_time);
        GETARG(size_t *, count);

        *count = _printk_read(str, *len, from_time, curr_time);
        return 0;
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall check if kernel panic occurred in last session.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_kernelpanicinfo(syscallrq_t *rq)
{
        GETARG(struct _kernel_panic_info*, info);

        if (_kernel_panic_info(info)) {
                return 0;
        } else {
                return ENOENT;
        }
}

//==============================================================================
/**
 * @brief  This syscall create new process.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_processcreate(syscallrq_t *rq)
{
        GETARG(const char *, cmd);
        GETARG(_process_attr_t *, attr);
        GETARG(pid_t *, pid);

        return _process_create(cmd, attr, pid);
}

//==============================================================================
/**
 * @brief  This syscall destroy existing process.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_processkill(syscallrq_t *rq)
{
        GETARG(const pid_t *, pid);

        _process_t *proc = NULL;
        int err = _process_get_container(*pid, &proc);
        if (!err) {
                err = _process_kill(*pid);
                if (!err) {
                        _process_remove_zombie(proc, NULL);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall wait for process.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_processwait(syscallrq_t *rq)
{
        GETARG(const pid_t *, pid);
        GETARG(int *, status);
        GETARG(const uint32_t *, timeout);

        _process_t *proc = NULL;
        int err = _process_get_container(*pid, &proc);
        if (!err) {
                kflag_t *flag;
                err = _process_get_event_flags(proc, &flag);
                if (!err) {
                        const uint32_t mask = _PROCESS_EXIT_FLAG(0);
                        err = _flag_wait(flag, mask, *timeout);
                        if (!err) {
                                _process_remove_zombie(proc, status);
                                _sleep_ms(1); // force scheduler switch
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall read process statistics by seek.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_processstatseek(syscallrq_t *rq)
{
        GETARG(size_t *, seek);
        GETARG(_process_stat_t*, stat);

        return _process_get_stat_seek(*seek, stat);
}

//==============================================================================
/**
 * @brief  This syscall read thread statistics.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_threadstat(syscallrq_t *rq)
{
        GETARG(pid_t *, pid);
        GETARG(tid_t *, tid);
        GETARG(_thread_stat_t *, stat);

        return _process_thread_get_stat(*pid, *tid, stat);
}

//==============================================================================
/**
 * @brief  This syscall read process statistics by pid.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_processstatpid(syscallrq_t *rq)
{
        GETARG(pid_t *, pid);
        GETARG(_process_stat_t*, stat);

        return _process_get_stat_pid(*pid, stat);
}

//==============================================================================
/**
 * @brief  This syscall return PID of caller process.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_processgetpid(syscallrq_t *rq)
{
        GETARG(pid_t *, pid);

        return _process_get_pid(GETPROCESS(), pid);
}

//==============================================================================
/**
 * @brief  This syscall return PID's priority.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_processgetprio(syscallrq_t *rq)
{
        GETARG(pid_t *, pid);
        GETARG(int *, prio);

        return _process_get_priority(*pid, prio);
}

//==============================================================================
/**
 * @brief  This syscall abort process.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_processabort(syscallrq_t *rq)
{
        UNUSED_ARG1(rq);
        _process_abort(_process_get_active());
        for (;;) _sleep_ms(100);
        return EFAULT;
}

//==============================================================================
/**
 * @brief  This syscall exit process.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_processexit(syscallrq_t *rq)
{
        GETARG(int *, status);
        _process_exit(_process_get_active(), *status);
        for (;;) _sleep_ms(100);
        return EFAULT;
}

//==============================================================================
/**
 * @brief  This syscall return CWD of current process.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_getcwd(syscallrq_t *rq)
{
#if __OS_ENABLE_GETCWD__ == _YES_
        GETARG(char *, buf);
        GETARG(size_t *, size);

        int err = EINVAL;

        const char *cwd = NULL;
        if (buf && *size) {
                cwd = _process_get_CWD(GETPROCESS());
               _strlcpy(buf, cwd, *size);
               err = 0;
        }

        return err;
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall set CWD of current process.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_setcwd(syscallrq_t *rq)
{
#if __OS_ENABLE_GETCWD__ == _YES_
        GETARG(const char *, cwd);
        return _process_set_CWD(GETPROCESS(), cwd);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall create new thread.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_threadcreate(syscallrq_t *rq)
{
        GETARG(thread_func_t, func);
        GETARG(_thread_attr_t *, attr);
        GETARG(void *, arg);
        GETARG(tid_t *, tid);

        return _process_thread_create(GETPROCESS(), func, attr, arg, tid);
}

//==============================================================================
/**
 * @brief  This syscall destroy thread.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_threadkill(syscallrq_t *rq)
{
        GETARG(tid_t *, tid);

        return _process_thread_kill(rq->client_proc, *tid);
}

//==============================================================================
/**
 * @brief  This syscall create new semaphore descriptor.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_semaphoreopen(syscallrq_t *rq)
{
        GETARG(const size_t *, cnt_max);
        GETARG(const size_t *, cnt_init);
        GETARG(int *, fd);

        int desc = -1;
        ksem_t *sem = NULL;
        int err = _semaphore_create(*cnt_max, *cnt_init, &sem);
        if (not err) {
                err = _process_descriptor_allocate(GETPROCESS(), &desc, &sem->header);
                if (err) {
                        _semaphore_destroy(sem);
                } else {
                        *fd = desc;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall wait for semaphore.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_semaphorewait(syscallrq_t *rq)
{
        GETARG(int*, fd);
        GETARG(uint32_t*, timeout);

        res_header_t *res;
        int err = _process_descriptor_get_resource(GETPROCESS(), *fd, &res);
        if (!err) {
                err = _semaphore_wait(cast(ksem_t*, res), *timeout);
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall signal semaphore.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_semaphoresignal(syscallrq_t *rq)
{
        GETARG(int*, fd);

        res_header_t *res;
        int err = _process_descriptor_get_resource(GETPROCESS(), *fd, &res);
        if (!err) {
                err = _semaphore_signal(cast(ksem_t*, res));
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall wait for semaphore.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_semaphoregetvalue(syscallrq_t *rq)
{
        GETARG(int*, fd);
        GETARG(size_t*, value);

        res_header_t *res;
        int err = _process_descriptor_get_resource(GETPROCESS(), *fd, &res);
        if (!err) {
                err = _semaphore_get_value(cast(ksem_t*, res), value);
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall create new mutex.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_mutexopen(syscallrq_t *rq)
{
        GETARG(const enum kmtx_type *, type);
        GETARG(int *, fd);

        int desc = -1;
        kmtx_t *mtx = NULL;
        int err = _mutex_create(*type, &mtx);
        if (not err) {
                err = _process_descriptor_allocate(GETPROCESS(), &desc, &mtx->header);
                if (err) {
                        _mutex_destroy(mtx);
                } else {
                        *fd = desc;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall signal semaphore.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_mutexlock(syscallrq_t *rq)
{
        GETARG(int*, fd);
        GETARG(uint32_t*, timeout);

        res_header_t *res;
        int err = _process_descriptor_get_resource(GETPROCESS(), *fd, &res);
        if (!err) {
                err = _mutex_lock(cast(kmtx_t*, res), *timeout);
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall signal semaphore.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_mutexunlock(syscallrq_t *rq)
{
        GETARG(int*, fd);

        res_header_t *res;
        int err = _process_descriptor_get_resource(GETPROCESS(), *fd, &res);
        if (!err) {
                err = _mutex_unlock(cast(kmtx_t*, res));
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall create new queue.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_queueopen(syscallrq_t *rq)
{
        GETARG(const size_t *, length);
        GETARG(const size_t *, item_size);
        GETARG(int *, fd);

        int desc = -1;
        kqueue_t *queue = NULL;
        int err = _queue_create(*length, *item_size, &queue);
        if (not err) {
                err = _process_descriptor_allocate(GETPROCESS(), &desc, &queue->header);
                if (err) {
                        _queue_destroy(queue);
                } else {
                        *fd = desc;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall reset queue.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_queuereset(syscallrq_t *rq)
{
        GETARG(int*, fd);

        res_header_t *res;
        int err = _process_descriptor_get_resource(GETPROCESS(), *fd, &res);
        if (!err) {
                err = _queue_reset(cast(kqueue_t*, res));
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall send item to queue.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_queuesend(syscallrq_t *rq)
{
        GETARG(int*, fd);
        GETARG(const void*, item);
        GETARG(const u32_t*, timeout);

        res_header_t *res;
        int err = _process_descriptor_get_resource(GETPROCESS(), *fd, &res);
        if (!err) {
                err = _queue_send(cast(kqueue_t*, res), item, *timeout);
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall receive item from queue.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_queuereceive(syscallrq_t *rq)
{
        GETARG(int*, fd);
        GETARG(void*, item);
        GETARG(const u32_t*, timeout);

        res_header_t *res;
        int err = _process_descriptor_get_resource(GETPROCESS(), *fd, &res);
        if (!err) {
                err = _queue_receive(cast(kqueue_t*, res), item, *timeout);
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall receive item from queue without grab.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_queuereceviepeek(syscallrq_t *rq)
{
        GETARG(int*, fd);
        GETARG(void*, item);
        GETARG(const u32_t*, timeout);

        res_header_t *res;
        int err = _process_descriptor_get_resource(GETPROCESS(), *fd, &res);
        if (!err) {
                err = _queue_receive_peek(cast(kqueue_t*, res), item, *timeout);
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall return number of items in queue.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_queueitemscount(syscallrq_t *rq)
{
        GETARG(int*, fd);
        GETARG(size_t*, count);

        res_header_t *res;
        int err = _process_descriptor_get_resource(GETPROCESS(), *fd, &res);
        if (!err) {
                err = _queue_get_number_of_items(cast(kqueue_t*, res), count);
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall return number of items in queue.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_queuefreespace(syscallrq_t *rq)
{
        GETARG(int*, fd);
        GETARG(size_t*, count);

        res_header_t *res;
        int err = _process_descriptor_get_resource(GETPROCESS(), *fd, &res);
        if (!err) {
                err = _queue_get_space_available(cast(kqueue_t*, res), count);
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall add network interface.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_netifadd(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(const char *, netname);
        GETARG(NET_family_t *, family);
        GETARG(const char *, if_path);

        return _net_ifadd(netname, *family, if_path);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall remove network interface.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_netifrm(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(const char *, netname);

        return _net_ifrm(netname);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall list network interfaces.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_netiflist(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(char **, netname);
        GETARG(size_t *, netname_len);
        GETARG(size_t *, count);

        return _net_iflist(netname, *netname_len, count);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall up network.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_netifup(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(const char *, netname);
        GETARG(const NET_generic_config_t *, config);

        return _net_ifup(netname, config);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall down network.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_netifdown(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(const char *, netname);

        return _net_ifdown(netname);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall return network status.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_netifstatus(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(const char *, netname);
        GETARG(NET_family_t *, family);
        GETARG(NET_generic_status_t *, status);

        return _net_ifstatus(netname, family, status);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall create new socket.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_netsocketcreate(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(const char *, netname);
        GETARG(NET_protocol_t*, protocol);
        GETARG(SOCKET **, socket);

        int err = _net_socket_create(netname, *protocol, socket);
        if (!err) {
                err = _process_register_resource(GETPROCESS(), cast(res_header_t*, socket));
                if (err) {
                        _net_socket_destroy(*socket);
                        *socket = NULL;
                }
        }

        return err;
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall destroy socket.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_netsocketdestroy(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);

        int err = _process_release_resource(GETPROCESS(), cast(res_header_t*, socket), RES_TYPE_SOCKET);
        if (err) {
                res_header_t *res;
                if (_process_descriptor_get_resource(GETPROCESS(), 2, &res) == 0) {
                        const char *msg = "*** Error: object is not a socket! ***\n";
                        size_t wrcnt;
                        _vfs_fwrite(msg, strlen(msg), &wrcnt, cast(kfile_t*, res));
                }

                pid_t pid = 0;
                _process_get_pid(GETPROCESS(), &pid);
                _process_kill(pid);
        }

        return err;
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall bind socket with address.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_netbind(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);
        GETARG(const NET_generic_sockaddr_t *, addr);

        return _net_socket_bind(socket, addr);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall listen connection on selected socket.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_netlisten(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);

        return _net_socket_listen(socket);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall accept incoming connection.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_netaccept(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);
        GETARG(SOCKET **, new_socket);

        SOCKET *socknew = NULL;
        int err = _net_socket_accept(socket, &socknew);
        if (!err) {
                err = _process_register_resource(GETPROCESS(), cast(res_header_t*, socknew));
                if (err) {
                        _net_socket_destroy(socknew);
                        socknew = NULL;
                }
        }

        *new_socket = socknew;

        return err;
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall receive incoming bytes on socket.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_netrecv(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);
        GETARG(void *, buf);
        GETARG(size_t *, len);
        GETARG(NET_flags_t *, flags);
        GETARG(size_t *, recved);

        return _net_socket_recv(socket, buf, *len, *flags, recved);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall receive incoming bytes on socket.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_netrecvfrom(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);
        GETARG(void *, buf);
        GETARG(size_t *, len);
        GETARG(NET_flags_t *, flags);
        GETARG(NET_generic_sockaddr_t *, sockaddr);
        GETARG(size_t *, recved);

        return _net_socket_recvfrom(socket, buf, *len, *flags, sockaddr, recved);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall send buffer to socket.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_netsend(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);
        GETARG(const void *, buf);
        GETARG(size_t *, len);
        GETARG(NET_flags_t *, flags);
        GETARG(size_t *, sent);

        return _net_socket_send(socket, buf, *len, *flags, sent);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall send buffer to socket.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_netsendto(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);
        GETARG(const void *, buf);
        GETARG(size_t *, len);
        GETARG(NET_flags_t *, flags);
        GETARG(const NET_generic_sockaddr_t *, to_addr);
        GETARG(size_t *, sent);

        return _net_socket_sendto(socket, buf, *len, *flags, to_addr, sent);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall gets address of server by name.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_netgethostbyname(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(const char *, netname);
        GETARG(const char *, name);
        GETARG(NET_generic_sockaddr_t *, addr);

        return _net_gethostbyname(netname, name, addr);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall set receive timeout of socket.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_netsetrecvtimeout(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);
        GETARG(uint32_t *, timeout);

        return _net_socket_set_recv_timeout(socket, *timeout);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall set send timeout of socket.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_netsetsendtimeout(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);
        GETARG(uint32_t *, timeout);

        return _net_socket_set_send_timeout(socket, *timeout);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall connect socket to address.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_netconnect(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);
        GETARG(const NET_generic_sockaddr_t *, addr);

        return _net_socket_connect(socket, addr);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall disconnect socket.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_netdisconnect(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);

        return _net_socket_disconnect(socket);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall shut down selected connection direction.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_netshutdown(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);
        GETARG(NET_shut_t *, how);

        return _net_socket_shutdown(socket, *how);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall return socket address.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_netgetaddress(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);
        GETARG(NET_generic_sockaddr_t *, sockaddr);

        return _net_socket_getaddress(socket, sockaddr);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall return host to newtwork (or network to host) endianness
 *         conversion according to selected network family.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_nethton16(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(NET_family_t*, family);
        GETARG(uint16_t*, value_in);
        GETARG(uint16_t*, value_out);

        *value_out = _net_hton_u16(*family, *value_in);
        return 0;
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall return host to newtwork (or network to host) endianness
 *         conversion according to selected network family.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_nethton32(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(NET_family_t*, family);
        GETARG(uint32_t*, value_in);
        GETARG(uint32_t*, value_out);

        *value_out = _net_hton_u32(*family, *value_in);
        return 0;
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall return host to newtwork (or network to host) endianness
 *         conversion according to selected network family.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_nethton64(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(NET_family_t*, family);
        GETARG(uint64_t*, value_in);
        GETARG(uint64_t*, value_out);

        *value_out = _net_hton_u64(*family, *value_in);
        return 0;
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall creates shared memory region.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_shmcreate(syscallrq_t *rq)
{
#if __OS_ENABLE_SHARED_MEMORY__ == _YES_
        GETARG(const char *, key);
        GETARG(const size_t *, size);

        return _shm_create(key, *size);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall return shared memory region.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_shmattach(syscallrq_t *rq)
{
#if __OS_ENABLE_SHARED_MEMORY__ == _YES_
        GETARG(const char *, key);
        GETARG(void **, mem);
        GETARG(size_t *, size);

        pid_t pid = 0;
        int err = _process_get_pid(GETPROCESS(), &pid);
        if (!err) {
                err = _shm_attach(key, mem, size, pid);
        }

        return err;
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall destroy shared memory region.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_shmdetach(syscallrq_t *rq)
{
#if __OS_ENABLE_SHARED_MEMORY__ == _YES_
        GETARG(const char *, key);

        pid_t pid = 0;
        int err = _process_get_pid(GETPROCESS(), &pid);
        if (!err) {
                err = _shm_detach(key, pid);
        }

        return err;
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall destroy shared memory region.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_shmdestroy(syscallrq_t *rq)
{
#if __OS_ENABLE_SHARED_MEMORY__ == _YES_
        GETARG(const char *, key);

        return _shm_destroy(key);
#else
        UNUSED_ARG1(rq);
        return ENOSYS;
#endif
}

//==============================================================================
/**
 * @brief  This syscall suspend task/thread for specified time in milliseconds.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_msleep(syscallrq_t *rq)
{
        GETARG(const uint32_t *, msec);
        _sleep_ms(*msec);
        return 0;
}

//==============================================================================
/**
 * @brief  This syscall suspend task/thread for specified time in microseconds.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_usleep(syscallrq_t *rq)
{
        GETARG(const uint32_t *, usec);

        u32_t ms = *usec / 1000;
        u32_t us = *usec % 1000;

        if (ms) {
                _sleep_ms(ms);
        }

        if (us) {
                _cpuctl_delay_us(us);
        }

        return 0;
}

//==============================================================================
/**
 * @brief  This syscall get current group ID.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_getgid(syscallrq_t *rq)
{
        GETARG(gid_t *, gid);

        if (gid) {
                *gid = 0;
                return 0;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief  This syscall get current user ID.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_getuid(syscallrq_t *rq)
{
        GETARG(uid_t *, uid);

        if (uid) {
                *uid = 0;
                return 0;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief  This syscall get free memory (RAM).
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_getmemdetails(syscallrq_t *rq)
{
        GETARG(_mm_mem_usage_t*, details);

        return _mm_get_mem_usage_details(details);
}

//==============================================================================
/**
 * @brief  This syscall get free memory (RAM).
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_getmodmemusage(syscallrq_t *rq)
{
        GETARG(uint*, module);
        GETARG(int32_t*, usage);

        return _mm_get_module_mem_usage(*module, usage);
}

//==============================================================================
/**
 * @brief  This syscall get system uptime in seconds.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_getuptimems(syscallrq_t *rq)
{
        GETARG(uint64_t*, uptime_ms);

        if (uptime_ms) {
                *uptime_ms = _kernel_get_time_ms();
                return 0;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief  This syscall get average CPU load.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_getavgcpuload(syscallrq_t *rq)
{
        GETARG(_avg_CPU_load_t*, stat);

        return _get_average_CPU_load(stat);
}

//==============================================================================
/**
 * @brief  This syscall get platform name.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_getplatformname(syscallrq_t *rq)
{
        GETARG(const char **, ref);

        if (ref) {
                *ref = dnx_RTOS_platform_name;
                return 0;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief  This syscall get OS name.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_getosname(syscallrq_t *rq)
{
        GETARG(const char **, ref);

        if (ref) {
                *ref = dnx_RTOS_name;
                return 0;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief  This syscall get OS version.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_getosver(syscallrq_t *rq)
{
        GETARG(const char **, ref);

        if (ref) {
                *ref = dnx_RTOS_version;
                return 0;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief  This syscall get OS codename.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_getoscodename(syscallrq_t *rq)
{
        GETARG(const char **, ref);

        if (ref) {
                *ref = dnx_RTOS_codename;
                return 0;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief  This syscall get kernel name.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_getkernelname(syscallrq_t *rq)
{
        GETARG(const char **, ref);

        if (ref) {
                *ref = _KERNEL_NAME;
                return 0;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief  This syscall get kernel version.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_getkernelver(syscallrq_t *rq)
{
        GETARG(const char **, ref);

        if (ref) {
                *ref = _KERNEL_VERSION;
                return 0;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief  This syscall get host name.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_gethostname(syscallrq_t *rq)
{
        GETARG(char*, buf);
        GETARG(size_t*, buf_len);

        if (buf and buf_len and *buf_len) {
                _strlcpy(buf, __OS_HOSTNAME__, *buf_len);
                return 0;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief  This syscall get host name.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_sethostname(syscallrq_t *rq)
{
        GETARG(char*, buf);

        if (buf) {
                return ENOSYS;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief  This syscall get module name.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_getdrivername(syscallrq_t *rq)
{
        GETARG(size_t*, modno);
        GETARG(const char **, name_ref);

        if (name_ref) {
                *name_ref = _module_get_name(*modno);
                if (name_ref) {
                        return 0;
                } else {
                        return ENOENT;
                }
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief  This syscall get module ID.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_getdriverid(syscallrq_t *rq)
{
        GETARG(const char*, modname);
        GETARG(size_t *, driver_id);

        int id = _module_get_ID(modname);

        if (id < 0) {
                return ENOENT;
        } else {
                *driver_id = id;
                return 0;
        }
}

//==============================================================================
/**
 * @brief  This syscall get module ID.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_getdrivercount(syscallrq_t *rq)
{
        GETARG(size_t *, count);

        if (count) {
                *count = _module_get_count();
                return 0;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief  This syscall get module ID.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_getdriverinstances(syscallrq_t *rq)
{
        GETARG(size_t*, id);
        GETARG(size_t *, count);

        if (count) {
                ssize_t n =  _module_get_number_of_instances(*id);
                if (n >= 0) {
                        *count = n;
                        return 0;
                } else {
                        return ENOENT;
                }
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief  This syscall system restart.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_systemrestart(syscallrq_t *rq)
{
        UNUSED_ARG1(rq);
        _cpuctl_restart_system();
        return EFAULT;
}

//==============================================================================
/**
 * @brief  This syscall system restart.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_systemshutdown(syscallrq_t *rq)
{
        UNUSED_ARG1(rq);
        _cpuctl_shutdown_system();
        return EFAULT;
}

//==============================================================================
/**
 * @brief  This syscall system restart.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_syslogclear(syscallrq_t *rq)
{
        UNUSED_ARG1(rq);
        _printk_clear();
        return 0;
}

//==============================================================================
/**
 * @brief  This syscall get active thread.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_getactivethread(syscallrq_t *rq)
{
        GETARG(tid_t *, tid);

        if (tid) {
                *tid = rq->client_thread;
                return 0;
        } else {
                return EINVAL;
        }
}

//==============================================================================
/**
 * @brief  This syscall exit thread.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_threadexit(syscallrq_t *rq)
{
        GETARG(int*, status);

        return _process_thread_exit(*status);
}

//==============================================================================
/**
 * @brief  This syscall set dir position.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_dirseek(syscallrq_t *rq)
{
        GETARG(kdir_t*, dir);
        GETARG(const u32_t*, seek);

        return _vfs_seekdir(dir, *seek);
}

//==============================================================================
/**
 * @brief  This syscall get dir position.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_dirtell(syscallrq_t *rq)
{
        GETARG(kdir_t*, dir);
        GETARG(u32_t *, pos);

        return _vfs_telldir(dir, pos);
}

//==============================================================================
/**
 * @brief  This syscall lock scheduler.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_schedulerlock(syscallrq_t *rq)
{
        UNUSED_ARG1(rq);
        _kernel_scheduler_lock();
        return 0;
}

//==============================================================================
/**
 * @brief  This syscall unlock scheduler.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_schedulerunlock(syscallrq_t *rq)
{
        UNUSED_ARG1(rq);
        _kernel_scheduler_unlock();
        return 0;
}

//==============================================================================
/**
 * @brief  This syscall wait for thread join.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_threadjoin(syscallrq_t *rq)
{
        GETARG(const tid_t*, tid);
        GETARG(int*, status);
        GETARG(const u32_t*, timeout);

        int err = EINVAL;

        if ((*tid > 0) and (*tid < __OS_TASK_MAX_USER_THREADS__)) {

                kflag_t *flag;
                int err = _process_get_event_flags(rq->client_proc, &flag);
                if (not err) {
                        const u32_t mask = _PROCESS_EXIT_FLAG(*tid);
                        err = _flag_wait(flag, mask, *timeout);
                        if (not err) {
                                if (status) {
                                        err = _process_thread_get_status(GETPROCESS(), *tid, status);
                                }
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  This syscall wait for thread join.
 *
 * @param  rq                   syscall request
 *
 * @return One of errno value.
 */
//==============================================================================
static int syscall_isheapaddr(syscallrq_t *rq)
{
        GETARG(const void*, addr);

        if (_mm_is_object_in_heap(addr)) {
                return 0;
        } else {
                return ENOENT;
        }
}

/*==============================================================================
  End of file
==============================================================================*/
