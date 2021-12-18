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
#include "lib/cast.h"
#include "lib/unarg.h"
#include "lib/strlcat.h"
#include "lib/strlcpy.h"
#include "net/netm.h"
#include "mm/shm.h"
#include "dnx/misc.h"

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
#define SETRETURN(type, var)            if (rq->retptr) {*((type*)rq->retptr) = (var);}
#define SETERRNO(var)                   rq->err = var
#define GETERRNO()                      rq->err
#define UNUSED_RQ()                     UNUSED_ARG1(rq)

#define is_proc_valid(proc)             (_mm_is_object_in_heap(proc) && ((res_header_t*)proc)->type == RES_TYPE_PROCESS)
#define is_tid_in_range(proc, tid)      (tid < _process_get_max_threads(proc))

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        void       *retptr;
        _process_t *client_proc;
        tid_t       client_thread;
        syscall_t   syscall_no;
        va_list     args;
        int         err;
} syscallrq_t;

typedef void (*syscallfunc_t)(syscallrq_t*);

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void syscall_do(void *rq);

static void syscall_mount(syscallrq_t *rq);
static void syscall_umount(syscallrq_t *rq);
static void syscall_getmntentry(syscallrq_t *rq);
static void syscall_mknod(syscallrq_t *rq);
static void syscall_mkdir(syscallrq_t *rq);
static void syscall_mkfifo(syscallrq_t *rq);
static void syscall_opendir(syscallrq_t *rq);
static void syscall_closedir(syscallrq_t *rq);
static void syscall_readdir(syscallrq_t *rq);
static void syscall_remove(syscallrq_t *rq);
static void syscall_rename(syscallrq_t *rq);
static void syscall_chmod(syscallrq_t *rq);
static void syscall_chown(syscallrq_t *rq);
static void syscall_statfs(syscallrq_t *rq);
static void syscall_stat(syscallrq_t *rq);
static void syscall_fstat(syscallrq_t *rq);
static void syscall_fopen(syscallrq_t *rq);
static void syscall_fclose(syscallrq_t *rq);
static void syscall_fwrite(syscallrq_t *rq);
static void syscall_fread(syscallrq_t *rq);
static void syscall_fseek(syscallrq_t *rq);
static void syscall_ftell(syscallrq_t *rq);
static void syscall_ioctl(syscallrq_t *rq);
static void syscall_fflush(syscallrq_t *rq);
static void syscall_feof(syscallrq_t *rq);
static void syscall_ferror(syscallrq_t *rq);
static void syscall_clearerr(syscallrq_t *rq);
static void syscall_sync(syscallrq_t *rq);
static void syscall_gettimeofday(syscallrq_t *rq);
static void syscall_settimeofday(syscallrq_t *rq);
static void syscall_driverinit(syscallrq_t *rq);
static void syscall_driverrelease(syscallrq_t *rq);
static void syscall_malloc(syscallrq_t *rq);
static void syscall_zalloc(syscallrq_t *rq);
static void syscall_free(syscallrq_t *rq);
static void syscall_syslogread(syscallrq_t *rq);
static void syscall_kernelpanicinfo(syscallrq_t *rq);
static void syscall_processcreate(syscallrq_t *rq);
static void syscall_processkill(syscallrq_t *rq);
static void syscall_processcleanzombie(syscallrq_t *rq);
static void syscall_processgetsyncflag(syscallrq_t *rq);
static void syscall_processstatseek(syscallrq_t *rq);
static void syscall_processstatpid(syscallrq_t *rq);
static void syscall_processgetpid(syscallrq_t *rq);
static void syscall_processgetprio(syscallrq_t *rq);
static void syscall_processabort(syscallrq_t *rq);
static void syscall_processexit(syscallrq_t *rq);
static void syscall_threadstat(syscallrq_t *rq);
static void syscall_getcwd(syscallrq_t *rq);
static void syscall_setcwd(syscallrq_t *rq);
static void syscall_threadcreate(syscallrq_t *rq);
static void syscall_threadkill(syscallrq_t *rq);
static void syscall_threadgetstatus(syscallrq_t *rq);
static void syscall_semaphorecreate(syscallrq_t *rq);
static void syscall_semaphoredestroy(syscallrq_t *rq);
static void syscall_mutexcreate(syscallrq_t *rq);
static void syscall_mutexdestroy(syscallrq_t *rq);
static void syscall_queuecreate(syscallrq_t *rq);
static void syscall_queuedestroy(syscallrq_t *rq);
static void syscall_netifadd(syscallrq_t *rq);
static void syscall_netifrm(syscallrq_t *rq);
static void syscall_netiflist(syscallrq_t *rq);
static void syscall_netifup(syscallrq_t *rq);
static void syscall_netifdown(syscallrq_t *rq);
static void syscall_netifstatus(syscallrq_t *rq);
static void syscall_netsocketcreate(syscallrq_t *rq);
static void syscall_netsocketdestroy(syscallrq_t *rq);
static void syscall_netbind(syscallrq_t *rq);
static void syscall_netlisten(syscallrq_t *rq);
static void syscall_netaccept(syscallrq_t *rq);
static void syscall_netrecv(syscallrq_t *rq);
static void syscall_netsend(syscallrq_t *rq);
static void syscall_netgethostbyname(syscallrq_t *rq);
static void syscall_netsetrecvtimeout(syscallrq_t *rq);
static void syscall_netsetsendtimeout(syscallrq_t *rq);
static void syscall_netconnect(syscallrq_t *rq);
static void syscall_netdisconnect(syscallrq_t *rq);
static void syscall_netshutdown(syscallrq_t *rq);
static void syscall_netsendto(syscallrq_t *rq);
static void syscall_netrecvfrom(syscallrq_t *rq);
static void syscall_netgetaddress(syscallrq_t *rq);
static void syscall_nethton16(syscallrq_t *rq);
static void syscall_nethton32(syscallrq_t *rq);
static void syscall_nethton64(syscallrq_t *rq);
static void syscall_shmcreate(syscallrq_t *rq);
static void syscall_shmattach(syscallrq_t *rq);
static void syscall_shmdetach(syscallrq_t *rq);
static void syscall_shmdestroy(syscallrq_t *rq);
static void syscall_msleep(syscallrq_t *rq);
static void syscall_usleep(syscallrq_t *rq);
static void syscall_getgid(syscallrq_t *rq);
static void syscall_getuid(syscallrq_t *rq);
static void syscall_getmemdetails(syscallrq_t *rq);
static void syscall_getmodmemusage(syscallrq_t *rq);
static void syscall_getuptimems(syscallrq_t *rq);
static void syscall_getavgcpuload(syscallrq_t *rq);
static void syscall_getplatformname(syscallrq_t *rq);
static void syscall_getosname(syscallrq_t *rq);
static void syscall_getosver(syscallrq_t *rq);
static void syscall_getoscodename(syscallrq_t *rq);
static void syscall_getkernelname(syscallrq_t *rq);
static void syscall_getkernelver(syscallrq_t *rq);
static void syscall_gethostname(syscallrq_t *rq);
static void syscall_getdrivername(syscallrq_t *rq);
static void syscall_getdriverid(syscallrq_t *rq);
static void syscall_getdrivercount(syscallrq_t *rq);
static void syscall_getdriverinstances(syscallrq_t *rq);
static void syscall_systemrestart(syscallrq_t *rq);
static void syscall_systemshutdown(syscallrq_t *rq);
static void syscall_syslogclear(syscallrq_t *rq);
static void syscall_flagwait(syscallrq_t *rq);
static void sycall_getactivethread(syscallrq_t *rq);
static void syscall_threadexit(syscallrq_t *rq);
static void syscall_semaphorewait(syscallrq_t *rq);
static void syscall_semaphoresignal(syscallrq_t *rq);
static void syscall_semaphoregetvalue(syscallrq_t *rq);
static void syscall_mutexlock(syscallrq_t *rq);
static void syscall_mutexunlock(syscallrq_t *rq);
static void syscall_queuereset(syscallrq_t *rq);
static void syscall_queuesend(syscallrq_t *rq);
static void syscall_queuereceive(syscallrq_t *rq);
static void syscall_queuereceviepeek(syscallrq_t *rq);
static void syscall_queueitemscount(syscallrq_t *rq);
static void syscall_queuefreespace(syscallrq_t *rq);
static void syscall_dirtell(syscallrq_t *rq);
static void syscall_dirseek(syscallrq_t *rq);
static void syscall_schedulerlock(syscallrq_t *rq);
static void syscall_schedulerunlock(syscallrq_t *rq);

/*==============================================================================
  Local objects
==============================================================================*/
/* syscall table */
static const syscallfunc_t syscalltab[] = {
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
        [SYSCALL_READDIR] = syscall_readdir,
        [SYSCALL_REMOVE] = syscall_remove,
        [SYSCALL_RENAME] = syscall_rename,
        [SYSCALL_CHMOD] = syscall_chmod,
        [SYSCALL_CHOWN] = syscall_chown,
        [SYSCALL_STATFS] = syscall_statfs,
        [SYSCALL_STAT] = syscall_stat,
        [SYSCALL_FSTAT] = syscall_fstat,
        [SYSCALL_FOPEN] = syscall_fopen,
        [SYSCALL_FCLOSE] = syscall_fclose,
        [SYSCALL_FWRITE] = syscall_fwrite,
        [SYSCALL_FREAD] = syscall_fread,
        [SYSCALL_FSEEK] = syscall_fseek,
        [SYSCALL_FTELL] = syscall_ftell,
        [SYSCALL_IOCTL] = syscall_ioctl,
        [SYSCALL_FFLUSH] = syscall_fflush,
        [SYSCALL_FEOF] = syscall_feof,
        [SYSCALL_FERROR] = syscall_ferror,
        [SYSCALL_CLEARERR] = syscall_clearerr,
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
        [SYSCALL_PROCESSCLEANZOMBIE] = syscall_processcleanzombie,
        [SYSCALL_PROCESSGETSYNCFLAG] = syscall_processgetsyncflag,
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
        [SYSCALL_THREADGETSTATUS] = syscall_threadgetstatus,
        [SYSCALL_SEMAPHORECREATE] = syscall_semaphorecreate,
        [SYSCALL_SEMAPHOREDESTROY] = syscall_semaphoredestroy,
        [SYSCALL_MUTEXCREATE] = syscall_mutexcreate,
        [SYSCALL_MUTEXDESTROY] = syscall_mutexdestroy,
        [SYSCALL_QUEUECREATE] = syscall_queuecreate,
        [SYSCALL_QUEUEDESTROY] = syscall_queuedestroy,
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
        [SYSCALL_GETDRIVERNAME] = syscall_getdrivername,
        [SYSCALL_GETDRIVERID] = syscall_getdriverid,
        [SYSCALL_GETDRIVERCOUNT] = syscall_getdrivercount,
        [SYSCALL_GETDRIVERINSTANCES] = syscall_getdriverinstances,
        [SYSCALL_SYSTEMRESTART] = syscall_systemrestart,
        [SYSCALL_SYSTEMSHUTDOWN] = syscall_systemshutdown,
        [SYSCALL_SYSLOGCLEAR] = syscall_syslogclear,
        [SYSCALL_FLAGWAIT] = syscall_flagwait,
        [SYSCALL_GETACTIVETHREAD] = sycall_getactivethread,
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
        static const process_attr_t attr = {
                .f_stdin  = NULL,
                .f_stdout = NULL,
                .f_stderr = NULL,
                .p_stdin  = NULL,
                .p_stdout = NULL,
                .p_stderr = NULL,
                .cwd      = "/",
                .priority = PRIORITY_NORMAL,
                .detached = true
        };

        int err;

        catcherr(err = _process_create("kworker", &attr, NULL), exit);

        catcherr(err = _process_create(__OS_INIT_PROG__, &attr, NULL), exit);

        exit:
        return err;
}

//==============================================================================
/**
 * @brief  Function call selected syscall [USERSPACE].
 *
 * @param  syscall      syscall number
 * @param  retptr       pointer to return value
 * @param  ...          additional arguments
 */
//==============================================================================
void syscall(syscall_t syscall, void *retptr, ...)
{
        if (syscall < _SYSCALL_COUNT) {
                _process_t *proc; tid_t tid;
                _task_get_process_container(_THIS_TASK, &proc, &tid);

                _assert(proc);
                _assert(is_tid_in_range(proc, tid));

                _process_clean_up_killed_processes();

                syscallrq_t syscallrq = {
                        .syscall_no     = syscall,
                        .client_proc    = proc,
                        .client_thread  = tid,
                        .retptr         = retptr,
                        .err            = ESUCC,
                };

                va_start(syscallrq.args, retptr);
                syscall_do(&syscallrq);
                va_end(syscallrq.args);

                if (syscallrq.err) {
                        _errno = syscallrq.err;
                }
        }
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
 */
//==============================================================================
static void syscall_do(void *rq)
{
        syscallrq_t *sysrq = rq;

        if (!is_proc_valid(sysrq->client_proc)) {
                /*
                 * This message means that during execution this syscall,
                 * the client process does not exists anymore.
                 */
                printk("Invalid client process!");
                return;
        }

        _process_syscall_stat_inc(sysrq->client_proc, _kworker_proc);

        _process_enter_kernelspace(sysrq->client_proc, sysrq->syscall_no);
        syscalltab[sysrq->syscall_no](sysrq);
        _process_exit_kernelspace(sysrq->client_proc);
}

//==============================================================================
/**
 * @brief  This syscall mount selected file system to selected path.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_mount(syscallrq_t *rq)
{
        GETARG(const char *, FS_name);

        struct vfs_path src_path;
        src_path.CWD  = _process_get_CWD(GETPROCESS());
        src_path.PATH = LOADARG(const char *);

        struct vfs_path mount_point;
        mount_point.CWD  = src_path.CWD;
        mount_point.PATH = LOADARG(const char *);

        GETARG(const char *, opts);

        SETERRNO(_mount(FS_name, &src_path, &mount_point, opts));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall unmount selected file system.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_umount(syscallrq_t *rq)
{
        struct vfs_path mount_point;
        mount_point.CWD  = _process_get_CWD(GETPROCESS());
        mount_point.PATH = LOADARG(const char *);

        SETERRNO(_umount(&mount_point));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall return information about selected file system.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_getmntentry(syscallrq_t *rq)
{

#if __OS_ENABLE_STATFS__ == _YES_
        GETARG(int *, seek);
        GETARG(struct mntent *, mntent);
        SETERRNO(_vfs_getmntentry(*seek, mntent));

        int ret;
        switch (GETERRNO()) {
        case ESUCC : ret =  0; break;
        case ENOENT: ret =  1; break;
        default    : ret = -1; break;
        }

        SETRETURN(int, ret);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall create device node.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_mknod(syscallrq_t *rq)
{
#if __OS_ENABLE_MKNOD__ == _YES_
        struct vfs_path pathname;
        pathname.CWD  = _process_get_CWD(GETPROCESS());
        pathname.PATH = LOADARG(const char *);
        GETARG(const char *, mod_name);
        GETARG(int *, major);
        GETARG(int *, minor);

        SETERRNO(_vfs_mknod(&pathname, _dev_t__create(_module_get_ID(mod_name), *major, *minor)));
        SETRETURN(int, GETERRNO() ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall create directory.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_mkdir(syscallrq_t *rq)
{
#if __OS_ENABLE_MKDIR__ == _YES_
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);
        GETARG(mode_t *, mode);

        SETERRNO(_vfs_mkdir(&path, *mode));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall create FIFO pipe.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_mkfifo(syscallrq_t *rq)
{
#if __OS_ENABLE_MKFIFO__ == _YES_
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);
        GETARG(mode_t *, mode);

        SETERRNO(_vfs_mkfifo(&path, *mode));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall open selected directory.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_opendir(syscallrq_t *rq)
{
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);

        DIR *dir = NULL;
        int  err = _vfs_opendir(&path, &dir);
        if (err == ESUCC) {
                err = _process_register_resource(GETPROCESS(), cast(res_header_t*, dir));
                if (err != ESUCC) {
                        _vfs_closedir(dir);
                        dir = NULL;
                }
        }

        SETERRNO(err);
        SETRETURN(DIR*, dir);
}

//==============================================================================
/**
 * @brief  This syscall close selected directory.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_closedir(syscallrq_t *rq)
{
        GETARG(DIR *, dir);

        int err = _process_release_resource(GETPROCESS(), cast(res_header_t*, dir), RES_TYPE_DIR);
        if (err == EFAULT) {
                const char *msg = "*** Error: object is not a dir! ***\n";
                size_t wrcnt;
                _vfs_fwrite(msg, strlen(msg), &wrcnt, _process_get_stderr(GETPROCESS()));

                pid_t pid = 0;
                _process_get_pid(GETPROCESS(), &pid);
                _process_kill(pid);
        }

        SETERRNO(err);
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);

}

//==============================================================================
/**
 * @brief  This syscall read selected directory.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_readdir(syscallrq_t *rq)
{
        GETARG(DIR *, dir);
        dirent_t *dirent = NULL;
        SETERRNO(_vfs_readdir(dir, &dirent));
        SETRETURN(dirent_t*, dirent);
}

//==============================================================================
/**
 * @brief  This syscall remove selected file.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_remove(syscallrq_t *rq)
{
#if __OS_ENABLE_REMOVE__ == _YES_
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);

        SETERRNO(_vfs_remove(&path));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall rename selected file.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_rename(syscallrq_t *rq)
{
#if __OS_ENABLE_RENAME__ == _YES_
        struct vfs_path oldname;
        oldname.CWD  = _process_get_CWD(GETPROCESS());
        oldname.PATH = LOADARG(const char *);

        struct vfs_path newname;
        newname.CWD  = _process_get_CWD(GETPROCESS());
        newname.PATH = LOADARG(const char *);

        SETERRNO(_vfs_rename(&oldname, &newname));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall change mode of selected file.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_chmod(syscallrq_t *rq)
{
#if __OS_ENABLE_CHMOD__ == _YES_
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);
        GETARG(mode_t *, mode);

        SETERRNO(_vfs_chmod(&path, *mode));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall change owner and group of selected file.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_chown(syscallrq_t *rq)
{
#if __OS_ENABLE_CHOWN__ == _YES_
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);
        GETARG(uid_t *, owner);
        GETARG(gid_t *, group);

        SETERRNO(_vfs_chown(&path, *owner, *group));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall read statistics of selected file by path.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_stat(syscallrq_t *rq)
{
#if __OS_ENABLE_FSTAT__ == _YES_
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);
        GETARG(struct stat *, buf);

        SETERRNO(_vfs_stat(&path, buf));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall read statistics of selected file by FILE object.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_fstat(syscallrq_t *rq)
{
#if __OS_ENABLE_FSTAT__ == _YES_
        GETARG(FILE *, file);
        GETARG(struct stat *, buf);
        SETERRNO(_vfs_fstat(file, buf));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall read statistics of file system mounted in selected path.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_statfs(syscallrq_t *rq)
{
#if __OS_ENABLE_STATFS__ == _YES_
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);
        GETARG(struct statfs *, buf);

        SETERRNO(_vfs_statfs(&path, buf));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall open selected file.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_fopen(syscallrq_t *rq)
{
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);
        GETARG(const char *, mode);

        FILE *file = NULL;
        int   err  = _vfs_fopen(&path, mode, &file);
        if (err == ESUCC) {
                err = _process_register_resource(GETPROCESS(), cast(res_header_t*, file));
                if (err != ESUCC) {
                        _vfs_fclose(file, true);
                        file = NULL;
                }
        }

        SETERRNO(err);
        SETRETURN(FILE*, file);
}

//==============================================================================
/**
 * @brief  This syscall close selected file.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_fclose(syscallrq_t *rq)
{
        GETARG(FILE *, file);

        int err = _process_release_resource(GETPROCESS(), cast(res_header_t*, file), RES_TYPE_FILE);
        if (err == EFAULT) {
                const char *msg = "*** Error: object is not a file! ***\n";
                size_t wrcnt;
                _vfs_fwrite(msg, strlen(msg), &wrcnt, _process_get_stderr(GETPROCESS()));

                pid_t pid = 0;
                _process_get_pid(GETPROCESS(), &pid);
                _process_kill(pid);
        }

        SETERRNO(err);
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall write data to selected file.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_fwrite(syscallrq_t *rq)
{
        GETARG(const uint8_t *, buf);
        GETARG(size_t *, count);
        GETARG(FILE*, file);

        size_t wrcnt = 0;
        SETERRNO(_vfs_fwrite(buf, *count, &wrcnt, file));
        SETRETURN(size_t, wrcnt);
}

//==============================================================================
/**
 * @brief  This syscall read data from selected file.
 *
 * @param  rq                   syscall request
 */
//==============================================================================;
static void syscall_fread(syscallrq_t *rq)
{
        GETARG(uint8_t *, buf);
        GETARG(size_t *, count);
        GETARG(FILE *, file);

        size_t rdcnt = 0;
        SETERRNO(_vfs_fread(buf, *count, &rdcnt, file));
        SETRETURN(size_t, rdcnt);
}

//==============================================================================
/**
 * @brief  This syscall move file pointer.
 *
 * @param  rq                   syscall request
 */
//==============================================================================;
static void syscall_fseek(syscallrq_t *rq)
{
        GETARG(FILE *, file);
        GETARG(i64_t *, lseek);
        GETARG(int *, orgin);
        SETERRNO(_vfs_fseek(file, *lseek, *orgin));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall return file pointer position.
 *
 * @param  rq                   syscall request
 */
//==============================================================================;
static void syscall_ftell(syscallrq_t *rq)
{
        GETARG(FILE *, file);
        i64_t lseek = 0;
        SETERRNO(_vfs_ftell(file, &lseek));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall perform not standard operation on selected file/device.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_ioctl(syscallrq_t *rq)
{
        GETARG(FILE *, file);
        GETARG(int *, request);
        GETARG(va_list *, arg);
        SETERRNO(_vfs_vfioctl(file, *request, *arg));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall flush buffers of selected file.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_fflush(syscallrq_t *rq)
{
        GETARG(FILE *, file);
        SETERRNO(_vfs_fflush(file));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall check file end.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_feof(syscallrq_t *rq)
{
        GETARG(FILE *, file);
        int eof = 1;
        SETERRNO(_vfs_feof(file, &eof));
        SETRETURN(int, eof);
}

//==============================================================================
/**
 * @brief  This syscall check file error.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_ferror(syscallrq_t *rq)
{
        GETARG(FILE *, file);
        int error = 1;
        SETERRNO(_vfs_ferror(file, &error));
        SETRETURN(int, error);
}

//==============================================================================
/**
 * @brief  This syscall clear file EOF and err flags.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_clearerr(syscallrq_t *rq)
{
        GETARG(FILE *, file);
        SETERRNO(_vfs_clearerr(file));
}

//==============================================================================
/**
 * @brief  This syscall synchronize all buffers of filesystems.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_sync(syscallrq_t *rq)
{
        UNUSED_RQ();
        _vfs_sync();
}

//==============================================================================
/**
 * @brief  This syscall return current time value (UTC timestamp).
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_gettimeofday(syscallrq_t *rq)
{
#if __OS_ENABLE_TIMEMAN__ == _YES_
        GETARG(struct timeval*, timeval);
        SETERRNO(_gettime(timeval));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall set current system time (UTC timestamp).
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_settimeofday(syscallrq_t *rq)
{
#if __OS_ENABLE_TIMEMAN__ == _YES_
        GETARG(time_t *, time);
        SETERRNO(_settime(time));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall initialize selected driver and create node.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_driverinit(syscallrq_t *rq)
{
        GETARG(const char *, mod_name);
        GETARG(int *, major);
        GETARG(int *, minor);
        GETARG(const char *, node_path);
        GETARG(const void *, config);
        dev_t drvid = -1;
        SETERRNO(_driver_init(mod_name, *major, *minor,  node_path, config, &drvid));
        SETRETURN(dev_t, GETERRNO() == ESUCC ? drvid : -1);
}

//==============================================================================
/**
 * @brief  This syscall release selected driver.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_driverrelease(syscallrq_t *rq)
{
        GETARG(const char *, mod_name);
        GETARG(int *, major);
        GETARG(int *, minor);
        SETERRNO(_driver_release(_dev_t__create(_module_get_ID(mod_name), *major, *minor)));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall allocate memory for application.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_malloc(syscallrq_t *rq)
{
        GETARG(size_t *, size);

        void *mem = NULL;
        int   err = _kmalloc(_MM_PROG, *size, NULL, _MM_FLAG__DMA_CAPABLE, _MM_FLAG__DMA_CAPABLE, &mem);
        if (err == ESUCC) {
                err = _process_register_resource(GETPROCESS(), mem);
                if (err != ESUCC) {
                        _kfree(_MM_PROG, &mem);
                }
        }

        SETERRNO(err);
        SETRETURN(void*, mem ? &cast(res_header_t*, mem)[1] : NULL);
}

//==============================================================================
/**
 * @brief  This syscall allocate memory for application and clear allocated block.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_zalloc(syscallrq_t *rq)
{
        GETARG(size_t *, size);

        void *mem = NULL;
        int   err = _kzalloc(_MM_PROG, *size, NULL, _MM_FLAG__DMA_CAPABLE, _MM_FLAG__DMA_CAPABLE, &mem);
        if (err == ESUCC) {
                err = _process_register_resource(GETPROCESS(), mem);
                if (err != ESUCC) {
                        _kfree(_MM_PROG, &mem);
                }
        }

        SETERRNO(err);
        SETRETURN(void*,  mem ? &cast(res_header_t*, mem)[1] : NULL);
}

//==============================================================================
/**
 * @brief  This syscall free allocated memory by application.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_free(syscallrq_t *rq)
{
        GETARG(void *, mem);

        int err = _process_release_resource(GETPROCESS(), cast(res_header_t*, mem) - 1, RES_TYPE_MEMORY);
        if (err != ESUCC) {
                const char *msg = "*** Error: double free or corruption ***\n";
                size_t wrcnt;
                _vfs_fwrite(msg, strlen(msg), &wrcnt, _process_get_stderr(GETPROCESS()));

                pid_t pid = 0;
                _process_get_pid(GETPROCESS(), &pid);
                _process_kill(pid);
        }

        SETERRNO(err);
}

//==============================================================================
/**
 * @brief  This syscall read syslog and pass to selected buffer.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_syslogread(syscallrq_t *rq)
{
#if ((__OS_SYSTEM_MSG_ENABLE__ > 0) && (__OS_PRINTF_ENABLE__ > 0))
        GETARG(char *, str);
        GETARG(size_t *, len);
        GETARG(const struct timeval *, from_time);
        GETARG(struct timeval *, curr_time);
        SETRETURN(size_t, _printk_read(str, *len, from_time, curr_time));
#else
        SETERRNO(ENOSYS);
        SETRETURN(size_t, 0);
#endif
}

//==============================================================================
/**
 * @brief  This syscall check if kernel panic occurred in last session.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_kernelpanicinfo(syscallrq_t *rq)
{
        GETARG(struct _kernel_panic_info*, info);
        SETRETURN(bool, _kernel_panic_info(info));
}

//==============================================================================
/**
 * @brief  This syscall create new process.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_processcreate(syscallrq_t *rq)
{
        GETARG(const char *, cmd);
        GETARG(process_attr_t *, attr);
        pid_t pid = 0;
        SETERRNO(_process_create(cmd, attr, &pid));
        SETRETURN(pid_t, pid);
}

//==============================================================================
/**
 * @brief  This syscall destroy existing process.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_processcleanzombie(syscallrq_t *rq)
{
        GETARG(pid_t *, pid);
        GETARG(int *, status);

        _process_t *proc = NULL;
        int err = _process_get_container(*pid, &proc);
        if (!err) {
                _process_remove_zombie(proc, status);
        }

        SETERRNO(err);
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall destroy existing process.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_processkill(syscallrq_t *rq)
{
        GETARG(pid_t *, pid);
        SETERRNO(_process_kill(*pid));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall return exit semaphore.
 *         then
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_processgetsyncflag(syscallrq_t *rq)
{
        GETARG(pid_t *, pid);
        GETARG(flag_t **, flag);

        _process_t *proc = NULL;
        int err = _process_get_container(*pid, &proc);
        if (!err) {
                err = _process_get_event_flags(proc, flag);
        }

        SETERRNO(err);
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall read process statistics by seek.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_processstatseek(syscallrq_t *rq)
{
        GETARG(size_t *, seek);
        GETARG(process_stat_t*, stat);
        SETERRNO(_process_get_stat_seek(*seek, stat));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall read thread statistics.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_threadstat(syscallrq_t *rq)
{
        GETARG(pid_t*, pid);
        GETARG(tid_t*, tid);
        GETARG(thread_stat_t*, stat);
        SETERRNO(_process_thread_get_stat(*pid, *tid, stat));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall read process statistics by pid.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_processstatpid(syscallrq_t *rq)
{
        GETARG(pid_t *, pid);
        GETARG(process_stat_t*, stat);
        SETERRNO(_process_get_stat_pid(*pid, stat));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall return PID of caller process.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_processgetpid(syscallrq_t *rq)
{
        pid_t pid = -1;
        SETERRNO(_process_get_pid(GETPROCESS(), &pid));
        SETRETURN(pid_t, pid);
}

//==============================================================================
/**
 * @brief  This syscall return PID's priority.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_processgetprio(syscallrq_t *rq)
{
        GETARG(pid_t *, pid);
        int prio = 0;
        SETERRNO(_process_get_priority(*pid, &prio));
        SETRETURN(int, prio);
}

//==============================================================================
/**
 * @brief  This syscall abort process.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_processabort(syscallrq_t *rq)
{
        UNUSED_ARG1(rq);
        _process_abort(_process_get_active());
        for (;;);
}

//==============================================================================
/**
 * @brief  This syscall exit process.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_processexit(syscallrq_t *rq)
{
        GETARG(int *, status);
        _process_exit(_process_get_active(), *status);
        for (;;);
}

//==============================================================================
/**
 * @brief  This syscall return CWD of current process.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_getcwd(syscallrq_t *rq)
{
#if __OS_ENABLE_GETCWD__ == _YES_
        GETARG(char *, buf);
        GETARG(size_t *, size);

        const char *cwd = NULL;
        if (buf && *size) {
                cwd = _process_get_CWD(GETPROCESS());
               _strlcpy(buf, cwd, *size);
        }

        SETRETURN(char*, cwd ? buf : NULL);
#else
        SETERRNO(ENOSYS);
        SETRETURN(char*, NULL);
#endif
}

//==============================================================================
/**
 * @brief  This syscall set CWD of current process.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_setcwd(syscallrq_t *rq)
{
#if __OS_ENABLE_GETCWD__ == _YES_
        GETARG(const char *, cwd);
        SETERRNO(_process_set_CWD(GETPROCESS(), cwd));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall create new thread.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_threadcreate(syscallrq_t *rq)
{
        GETARG(thread_func_t, func);
        GETARG(thread_attr_t *, attr);
        GETARG(void *, arg);

        tid_t tid = 0;
        SETERRNO(_process_thread_create(GETPROCESS(), func, attr, arg, &tid));
        SETRETURN(tid_t, tid);
}

//==============================================================================
/**
 * @brief  This syscall destroy thread.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_threadkill(syscallrq_t *rq)
{
        GETARG(tid_t *, tid);
        SETERRNO(_process_thread_kill(rq->client_proc, *tid));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  Return thread exit status.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_threadgetstatus(syscallrq_t *rq)
{
        GETARG(tid_t *, tid);
        GETARG(int   *, status);
        SETERRNO(_process_thread_get_status(GETPROCESS(), *tid, status));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall create new semaphore.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_semaphorecreate(syscallrq_t *rq)
{
        GETARG(const size_t *, cnt_max);
        GETARG(const size_t *, cnt_init);

        sem_t *sem = NULL;
        int err    = _semaphore_create(*cnt_max, *cnt_init, &sem);
        if (err == ESUCC) {
                err = _process_register_resource(GETPROCESS(), cast(res_header_t*, sem));
                if (err != ESUCC) {
                        _semaphore_destroy(sem);
                        sem = NULL;
                }
        }

        SETERRNO(err);
        SETRETURN(sem_t*, sem);
}

//==============================================================================
/**
 * @brief  This syscall destroy selected semaphore.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_semaphoredestroy(syscallrq_t *rq)
{
        GETARG(sem_t *, sem);

        int err = _process_release_resource(GETPROCESS(), cast(res_header_t*, sem), RES_TYPE_SEMAPHORE);
        if (err != ESUCC) {
                const char *msg = "*** Error: object is not a semaphore! ***\n";
                size_t wrcnt;
                _vfs_fwrite(msg, strlen(msg), &wrcnt, _process_get_stderr(GETPROCESS()));

                pid_t pid = 0;
                _process_get_pid(GETPROCESS(), &pid);
                _process_kill(pid);
        }

        SETERRNO(err);
}

//==============================================================================
/**
 * @brief  This syscall create new mutex.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_mutexcreate(syscallrq_t *rq)
{
        GETARG(const enum mutex_type *, type);

        mutex_t *mtx = NULL;
        int err      = _mutex_create(*type, &mtx);
        if (err == ESUCC) {
                err = _process_register_resource(GETPROCESS(), cast(res_header_t*, mtx));
                if (err != ESUCC) {
                        _mutex_destroy(mtx);
                        mtx = NULL;
                }
        }

        SETERRNO(err);
        SETRETURN(mutex_t*, mtx);
}

//==============================================================================
/**
 * @brief  This syscall destroy selected mutex.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_mutexdestroy(syscallrq_t *rq)
{
        GETARG(mutex_t *, mtx);

        int err = _process_release_resource(GETPROCESS(), cast(res_header_t*, mtx), RES_TYPE_MUTEX);
        if (err != ESUCC) {
                const char *msg = "*** Error: object is not a mutex! ***\n";
                size_t wrcnt;
                _vfs_fwrite(msg, strlen(msg), &wrcnt, _process_get_stderr(GETPROCESS()));

                pid_t pid = 0;
                _process_get_pid(GETPROCESS(), &pid);
                _process_kill(pid);
        }

        SETERRNO(err);
}

//==============================================================================
/**
 * @brief  This syscall create new queue.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_queuecreate(syscallrq_t *rq)
{
        GETARG(const size_t *, length);
        GETARG(const size_t *, item_size);

        queue_t *q = NULL;
        int err    = _queue_create(*length, *item_size, &q);
        if (err == ESUCC) {
                err = _process_register_resource(GETPROCESS(), cast(res_header_t*, q));
                if (err != ESUCC) {
                        _queue_destroy(q);
                        q = NULL;
                }
        }

        SETERRNO(err);
        SETRETURN(queue_t*, q);
}

//==============================================================================
/**
 * @brief  This syscall destroy selected queue.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_queuedestroy(syscallrq_t *rq)
{
        GETARG(queue_t *, q);

        int err = _process_release_resource(GETPROCESS(), cast(res_header_t*, q), RES_TYPE_QUEUE);
        if (err != ESUCC) {
                const char *msg = "*** Error: object is not a queue! ***\n";
                size_t wrcnt;
                _vfs_fwrite(msg, strlen(msg), &wrcnt, _process_get_stderr(GETPROCESS()));

                pid_t pid = 0;
                _process_get_pid(GETPROCESS(), &pid);
                _process_kill(pid);
        }

        SETERRNO(err);
}

//==============================================================================
/**
 * @brief  This syscall add network interface.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netifadd(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(const char *, netname);
        GETARG(NET_family_t *, family);
        GETARG(const char *, if_path);

        SETERRNO(_net_ifadd(netname, *family, if_path));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall remove network interface.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netifrm(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(const char *, netname);

        SETERRNO(_net_ifrm(netname));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall list network interfaces.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netiflist(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(char **, netname);
        GETARG(size_t *, netname_len);

        SETERRNO(_net_iflist(netname, *netname_len));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall up network.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netifup(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(const char *, netname);
        GETARG(const NET_generic_config_t *, config);

        SETERRNO(_net_ifup(netname, config));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall down network.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netifdown(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(const char *, netname);

        SETERRNO(_net_ifdown(netname));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall return network status.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netifstatus(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(const char *, netname);
        GETARG(NET_family_t *, family);
        GETARG(NET_generic_status_t *, status);

        SETERRNO(_net_ifstatus(netname, family, status));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall create new socket.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netsocketcreate(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(const char *, netname);
        GETARG(NET_protocol_t*, protocol);

        SOCKET *socket = NULL;
        int     err    = _net_socket_create(netname, *protocol, &socket);
        if (err == ESUCC) {
                err = _process_register_resource(GETPROCESS(), cast(res_header_t*, socket));
                if (err != ESUCC) {
                        _net_socket_destroy(socket);
                        socket = NULL;
                }
        }

        SETERRNO(err);
        SETRETURN(SOCKET*, socket);
#else
        SETERRNO(ENOSYS);
        SETRETURN(SOCKET*, NULL);
#endif
}

//==============================================================================
/**
 * @brief  This syscall destroy socket.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netsocketdestroy(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);

        int err = _process_release_resource(GETPROCESS(), cast(res_header_t*, socket), RES_TYPE_SOCKET);
        if (err != ESUCC) {
                const char *msg = "*** Error: object is not a socket! ***\n";
                size_t wrcnt;
                _vfs_fwrite(msg, strlen(msg), &wrcnt, _process_get_stderr(GETPROCESS()));

                pid_t pid = 0;
                _process_get_pid(GETPROCESS(), &pid);
                _process_kill(pid);
        }

        SETERRNO(err);
#else
        SETERRNO(ENOSYS);
#endif
}

//==============================================================================
/**
 * @brief  This syscall bind socket with address.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netbind(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);
        GETARG(const NET_generic_sockaddr_t *, addr);

        SETERRNO(_net_socket_bind(socket, addr));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall listen connection on selected socket.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netlisten(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);

        SETERRNO(_net_socket_listen(socket));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall accept incoming connection.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netaccept(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);
        GETARG(SOCKET **, new_socket);

        SOCKET *socknew = NULL;
        int     err     = _net_socket_accept(socket, &socknew);
        if (!err) {
                err = _process_register_resource(GETPROCESS(), cast(res_header_t*, socknew));
                if (err) {
                        _net_socket_destroy(socknew);
                        socknew = NULL;
                }
        }

        *new_socket = socknew;

        SETERRNO(err);
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall receive incoming bytes on socket.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netrecv(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);
        GETARG(void *, buf);
        GETARG(size_t *, len);
        GETARG(NET_flags_t *, flags);

        size_t recved = 0;
        SETERRNO(_net_socket_recv(socket, buf, *len, *flags, &recved));
        SETRETURN(int, GETERRNO() == ESUCC ? cast(int, recved) : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall receive incoming bytes on socket.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netrecvfrom(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);
        GETARG(void *, buf);
        GETARG(size_t *, len);
        GETARG(NET_flags_t *, flags);
        GETARG(NET_generic_sockaddr_t *, sockaddr);

        size_t recved = 0;
        SETERRNO(_net_socket_recvfrom(socket, buf, *len, *flags, sockaddr, &recved));
        SETRETURN(int, GETERRNO() == ESUCC ? cast(int, recved) : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall send buffer to socket.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netsend(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);
        GETARG(const void *, buf);
        GETARG(size_t *, len);
        GETARG(NET_flags_t *, flags);

        size_t sent = 0;
        SETERRNO(_net_socket_send(socket, buf, *len, *flags, &sent));
        SETRETURN(int, GETERRNO() == ESUCC ? cast(int, sent) : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall send buffer to socket.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netsendto(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);
        GETARG(const void *, buf);
        GETARG(size_t *, len);
        GETARG(NET_flags_t *, flags);
        GETARG(const NET_generic_sockaddr_t *, to_addr);

        size_t sent = 0;
        SETERRNO(_net_socket_sendto(socket, buf, *len, *flags, to_addr, &sent));
        SETRETURN(int, GETERRNO() == ESUCC ? cast(int, sent) : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall gets address of server by name.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netgethostbyname(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(const char *, netname);
        GETARG(const char *, name);
        GETARG(NET_generic_sockaddr_t *, addr);

        SETERRNO(_net_gethostbyname(netname, name, addr));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall set receive timeout of socket.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netsetrecvtimeout(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);
        GETARG(uint32_t *, timeout);

        SETERRNO(_net_socket_set_recv_timeout(socket, *timeout));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall set send timeout of socket.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netsetsendtimeout(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);
        GETARG(uint32_t *, timeout);

        SETERRNO(_net_socket_set_send_timeout(socket, *timeout));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall connect socket to address.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netconnect(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);
        GETARG(const NET_generic_sockaddr_t *, addr);

        SETERRNO(_net_socket_connect(socket, addr));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall disconnect socket.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netdisconnect(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);

        SETERRNO(_net_socket_disconnect(socket));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall shut down selected connection direction.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netshutdown(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);
        GETARG(NET_shut_t *, how);

        SETERRNO(_net_socket_shutdown(socket, *how));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall return socket address.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netgetaddress(syscallrq_t *rq)
{
#if _ENABLE_NETWORK_ == _YES_
        GETARG(SOCKET *, socket);
        GETARG(NET_generic_sockaddr_t *, sockaddr);

        SETERRNO(_net_socket_getaddress(socket, sockaddr));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall return host to newtwork (or network to host) endianness
 *         conversion according to selected network family.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_nethton16(syscallrq_t *rq)
{
        GETARG(NET_family_t*, family);
        GETARG(uint16_t*, value);

#if _ENABLE_NETWORK_ == _YES_
        SETRETURN(uint16_t, _net_hton_u16(*family, *value));
#else
        UNUSED_ARG1(family);
        SETERRNO(ENOSYS);
        SETRETURN(uint16_t, *value);
#endif
}

//==============================================================================
/**
 * @brief  This syscall return host to newtwork (or network to host) endianness
 *         conversion according to selected network family.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_nethton32(syscallrq_t *rq)
{
        GETARG(NET_family_t*, family);
        GETARG(uint32_t*, value);

#if _ENABLE_NETWORK_ == _YES_
        SETRETURN(uint32_t, _net_hton_u32(*family, *value));
#else
        UNUSED_ARG1(family);
        SETERRNO(ENOSYS);
        SETRETURN(uint32_t, *value);
#endif
}

//==============================================================================
/**
 * @brief  This syscall return host to newtwork (or network to host) endianness
 *         conversion according to selected network family.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_nethton64(syscallrq_t *rq)
{
        GETARG(NET_family_t*, family);
        GETARG(uint64_t*, value);

#if _ENABLE_NETWORK_ == _YES_
        SETRETURN(uint64_t, _net_hton_u64(*family, *value));
#else
        UNUSED_ARG1(family);
        SETERRNO(ENOSYS);
        SETRETURN(uint64_t, *value);
#endif
}

//==============================================================================
/**
 * @brief  This syscall creates shared memory region.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_shmcreate(syscallrq_t *rq)
{
#if __OS_ENABLE_SHARED_MEMORY__ == _YES_
        GETARG(const char *, key);
        GETARG(const size_t *, size);
        SETERRNO(_shm_create(key, *size));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall return shared memory region.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_shmattach(syscallrq_t *rq)
{
#if __OS_ENABLE_SHARED_MEMORY__ == _YES_
        GETARG(const char *, key);
        GETARG(void **, mem);
        GETARG(size_t *, size);

        pid_t pid = 0;
        SETERRNO(_process_get_pid(GETPROCESS(), &pid));
        if (GETERRNO() == ESUCC) {
                SETERRNO(_shm_attach(key, mem, size, pid));
        }

        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall destroy shared memory region.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_shmdetach(syscallrq_t *rq)
{
#if __OS_ENABLE_SHARED_MEMORY__ == _YES_
        GETARG(const char *, key);

        pid_t pid = 0;
        SETERRNO(_process_get_pid(GETPROCESS(), &pid));
        if (GETERRNO() == ESUCC) {
                SETERRNO(_shm_detach(key, pid));
        }

        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall destroy shared memory region.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_shmdestroy(syscallrq_t *rq)
{
#if __OS_ENABLE_SHARED_MEMORY__ == _YES_
        GETARG(const char *, key);
        SETERRNO(_shm_destroy(key));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
#else
        SETERRNO(ENOSYS);
        SETRETURN(int, -1);
#endif
}

//==============================================================================
/**
 * @brief  This syscall suspend task/thread for specified time in milliseconds.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_msleep(syscallrq_t *rq)
{
        GETARG(const uint32_t *, msec);
        _sleep_ms(*msec);
}

//==============================================================================
/**
 * @brief  This syscall suspend task/thread for specified time in microseconds.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_usleep(syscallrq_t *rq)
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
}

//==============================================================================
/**
 * @brief  This syscall get current group ID.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_getgid(syscallrq_t *rq)
{
        SETRETURN(gid_t, 0);
}

//==============================================================================
/**
 * @brief  This syscall get current user ID.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_getuid(syscallrq_t *rq)
{
        SETRETURN(gid_t, 0);
}

//==============================================================================
/**
 * @brief  This syscall get free memory (RAM).
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_getmemdetails(syscallrq_t *rq)
{
        GETARG(_mm_mem_usage_t*, details);
        SETERRNO(_mm_get_mem_usage_details(details));
        SETRETURN(int, _mm_get_mem_free());
}

//==============================================================================
/**
 * @brief  This syscall get free memory (RAM).
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_getmodmemusage(syscallrq_t *rq)
{
        GETARG(uint*, module);
        GETARG(int32_t*, usage);
        SETERRNO(_mm_get_module_mem_usage(*module, usage));
        SETRETURN(int, GETERRNO() ? -1 : 0);
}

//==============================================================================
/**
 * @brief  This syscall get system uptime in seconds.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_getuptimems(syscallrq_t *rq)
{
        SETRETURN(uint64_t, _kernel_get_time_ms());
}

//==============================================================================
/**
 * @brief  This syscall get average CPU load.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_getavgcpuload(syscallrq_t *rq)
{
        GETARG(avg_CPU_load_t*, stat);
        SETERRNO(_get_average_CPU_load(stat));
        SETRETURN(int, GETERRNO() ? -1 : 0);
}

//==============================================================================
/**
 * @brief  This syscall get platform name.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_getplatformname(syscallrq_t *rq)
{
        SETRETURN(const char*, dnx_RTOS_platform_name);
}

//==============================================================================
/**
 * @brief  This syscall get OS name.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_getosname(syscallrq_t *rq)
{
        SETRETURN(const char*, dnx_RTOS_name);
}

//==============================================================================
/**
 * @brief  This syscall get OS version.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_getosver(syscallrq_t *rq)
{
        SETRETURN(const char*, dnx_RTOS_version);
}

//==============================================================================
/**
 * @brief  This syscall get OS codename.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_getoscodename(syscallrq_t *rq)
{
        SETRETURN(const char*, dnx_RTOS_codename);
}

//==============================================================================
/**
 * @brief  This syscall get kernel name.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_getkernelname(syscallrq_t *rq)
{
        SETRETURN(const char*, _KERNEL_NAME);
}

//==============================================================================
/**
 * @brief  This syscall get kernel version.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_getkernelver(syscallrq_t *rq)
{
        SETRETURN(const char*, _KERNEL_VERSION);
}

//==============================================================================
/**
 * @brief  This syscall get host name.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_gethostname(syscallrq_t *rq)
{
        GETARG(char*, buf);
        GETARG(size_t*, buf_len);

        if (buf and buf_len and *buf_len) {
                _strlcpy(buf, __OS_HOSTNAME__, *buf_len);
                SETRETURN(int, 0);
        } else {
                SETERRNO(EINVAL);
                SETRETURN(int, -1);
        }
}

//==============================================================================
/**
 * @brief  This syscall get module name.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_getdrivername(syscallrq_t *rq)
{
        GETARG(size_t*, modno);
        SETRETURN(const char*, _module_get_name(*modno));
}

//==============================================================================
/**
 * @brief  This syscall get module ID.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_getdriverid(syscallrq_t *rq)
{
        GETARG(const char*, modname);
        SETRETURN(int, _module_get_ID(modname));
}

//==============================================================================
/**
 * @brief  This syscall get module ID.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_getdrivercount(syscallrq_t *rq)
{
        SETRETURN(size_t, _module_get_count());
}

//==============================================================================
/**
 * @brief  This syscall get module ID.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_getdriverinstances(syscallrq_t *rq)
{
        GETARG(size_t*, id);
        SETRETURN(ssize_t, _module_get_number_of_instances(*id));
}

//==============================================================================
/**
 * @brief  This syscall system restart.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_systemrestart(syscallrq_t *rq)
{
        UNUSED_ARG1(rq);
        _cpuctl_restart_system();
}

//==============================================================================
/**
 * @brief  This syscall system restart.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_systemshutdown(syscallrq_t *rq)
{
        UNUSED_ARG1(rq);
        _cpuctl_shutdown_system();
}

//==============================================================================
/**
 * @brief  This syscall system restart.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_syslogclear(syscallrq_t *rq)
{
        UNUSED_ARG1(rq);
        _printk_clear();
}

//==============================================================================
/**
 * @brief  This syscall system restart.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_flagwait(syscallrq_t *rq)
{
        GETARG(flag_t*, flag);
        GETARG(uint32_t*, mask);
        GETARG(uint32_t*, timeout);
        SETERRNO(_flag_wait(flag, *mask, *timeout));
        SETRETURN(bool, GETERRNO() ? false : true);
}

//==============================================================================
/**
 * @brief  This syscall get active thread.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void sycall_getactivethread(syscallrq_t *rq)
{
        SETRETURN(int, rq->client_thread);
}

//==============================================================================
/**
 * @brief  This syscall exit thread.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_threadexit(syscallrq_t *rq)
{
        GETARG(int*, status);
        _process_thread_exit(*status);
}

//==============================================================================
/**
 * @brief  This syscall wait for semaphore.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_semaphorewait(syscallrq_t *rq)
{
        GETARG(sem_t*, sem);
        GETARG(u32_t*, timeout);
        SETERRNO(_semaphore_wait(sem, *timeout));
        SETRETURN(bool, GETERRNO() == 0);
}

//==============================================================================
/**
 * @brief  This syscall signal semaphore.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_semaphoresignal(syscallrq_t *rq)
{
        GETARG(sem_t*, sem);
        SETERRNO(_semaphore_signal(sem));
        SETRETURN(bool, GETERRNO() == 0);
}

//==============================================================================
/**
 * @brief  This syscall wait for semaphore.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_semaphoregetvalue(syscallrq_t *rq)
{
        GETARG(sem_t*, sem);
        size_t value = 0;
        SETERRNO(_semaphore_get_value(sem, &value));
        SETRETURN(int, value);
}


//==============================================================================
/**
 * @brief  This syscall signal semaphore.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_mutexlock(syscallrq_t *rq)
{
        GETARG(mutex_t*, mutex);
        GETARG(uint32_t*, timeout);
        SETERRNO(_mutex_lock(mutex, *timeout));
        SETRETURN(bool, GETERRNO() == 0);
}

//==============================================================================
/**
 * @brief  This syscall signal semaphore.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_mutexunlock(syscallrq_t *rq)
{
        GETARG(mutex_t*, mutex);
        SETERRNO(_mutex_unlock(mutex));
        SETRETURN(bool, GETERRNO() == 0);
}

//==============================================================================
/**
 * @brief  This syscall reset queue.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_queuereset(syscallrq_t *rq)
{
        GETARG(queue_t*, queue);
        SETERRNO(_queue_reset(queue));
        SETRETURN(bool, GETERRNO() == 0);
}

//==============================================================================
/**
 * @brief  This syscall send item to queue.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_queuesend(syscallrq_t *rq)
{
        GETARG(queue_t*, queue);
        GETARG(const void*, item);
        GETARG(const u32_t*, timeout);
        SETERRNO(_queue_send(queue, item, *timeout));
        SETRETURN(bool, GETERRNO() == 0);
}

//==============================================================================
/**
 * @brief  This syscall receive item from queue.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_queuereceive(syscallrq_t *rq)
{
        GETARG(queue_t*, queue);
        GETARG(void*, item);
        GETARG(const u32_t*, timeout);
        SETERRNO(_queue_receive(queue, item, *timeout));
        SETRETURN(bool, GETERRNO() == 0);
}

//==============================================================================
/**
 * @brief  This syscall receive item from queue without grab.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_queuereceviepeek(syscallrq_t *rq)
{
        GETARG(queue_t*, queue);
        GETARG(void*, item);
        GETARG(const u32_t*, timeout);
        SETERRNO(_queue_receive_peek(queue, item, *timeout));
        SETRETURN(bool, GETERRNO() == 0);
}

//==============================================================================
/**
 * @brief  This syscall return number of items in queue.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_queueitemscount(syscallrq_t *rq)
{
        GETARG(queue_t*, queue);
        size_t items = -1;
        SETERRNO(_queue_get_number_of_items(queue, &items));
        SETRETURN(int, items);
}

//==============================================================================
/**
 * @brief  This syscall return number of items in queue.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_queuefreespace(syscallrq_t *rq)
{
        GETARG(queue_t*, queue);
        size_t items = -1;
        SETERRNO(_queue_get_space_available(queue, &items));
        SETRETURN(int, items);
}

//==============================================================================
/**
 * @brief  This syscall set dir position.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_dirseek(syscallrq_t *rq)
{
        GETARG(DIR*, dir);
        GETARG(const u32_t*, seek);
        SETERRNO(_vfs_seekdir(dir, *seek));
}

//==============================================================================
/**
 * @brief  This syscall get dir position.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_dirtell(syscallrq_t *rq)
{
        GETARG(DIR*, dir);
        u32_t seek = 0;
        SETERRNO(_vfs_telldir(dir, &seek));
        SETRETURN(u32_t, seek);
}

//==============================================================================
/**
 * @brief  This syscall lock scheduler.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_schedulerlock(syscallrq_t *rq)
{
        UNUSED_ARG1(rq);
        _kernel_scheduler_lock();
}

//==============================================================================
/**
 * @brief  This syscall unlock scheduler.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_schedulerunlock(syscallrq_t *rq)
{
        UNUSED_ARG1(rq);
        _kernel_scheduler_unlock();
}

/*==============================================================================
  End of file
==============================================================================*/
