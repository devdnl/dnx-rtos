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
#if __OS_TASK_KWORKER_MODE__ == 1
static void syscall_RTR(void *rq_queue);
#endif


static void syscall_mount(syscallrq_t *rq);
static void syscall_umount(syscallrq_t *rq);
#if __OS_ENABLE_STATFS__ == _YES_
static void syscall_getmntentry(syscallrq_t *rq);
#endif
#if __OS_ENABLE_MKNOD__ == _YES_
static void syscall_mknod(syscallrq_t *rq);
#endif
#if __OS_ENABLE_MKDIR__ == _YES_
static void syscall_mkdir(syscallrq_t *rq);
#endif
#if __OS_ENABLE_MKFIFO__ == _YES_
static void syscall_mkfifo(syscallrq_t *rq);
#endif
static void syscall_opendir(syscallrq_t *rq);
static void syscall_closedir(syscallrq_t *rq);
static void syscall_readdir(syscallrq_t *rq);
#if __OS_ENABLE_REMOVE__ == _YES_
static void syscall_remove(syscallrq_t *rq);
#endif
#if __OS_ENABLE_RENAME__ == _YES_
static void syscall_rename(syscallrq_t *rq);
#endif
#if __OS_ENABLE_CHMOD__ == _YES_
static void syscall_chmod(syscallrq_t *rq);
#endif
#if __OS_ENABLE_CHOWN__ == _YES_
static void syscall_chown(syscallrq_t *rq);
#endif
#if __OS_ENABLE_STATFS__ == _YES_
static void syscall_statfs(syscallrq_t *rq);
#endif
#if __OS_ENABLE_FSTAT__ == _YES_
static void syscall_stat(syscallrq_t *rq);
static void syscall_fstat(syscallrq_t *rq);
#endif
static void syscall_fopen(syscallrq_t *rq);
static void syscall_fclose(syscallrq_t *rq);
static void syscall_fwrite(syscallrq_t *rq);
static void syscall_fread(syscallrq_t *rq);
static void syscall_fseek(syscallrq_t *rq);
static void syscall_ioctl(syscallrq_t *rq);
static void syscall_fflush(syscallrq_t *rq);
static void syscall_sync(syscallrq_t *rq);
#if __OS_ENABLE_TIMEMAN__ == _YES_
static void syscall_gettime(syscallrq_t *rq);
static void syscall_settime(syscallrq_t *rq);
#endif
static void syscall_driverinit(syscallrq_t *rq);
static void syscall_driverrelease(syscallrq_t *rq);
static void syscall_malloc(syscallrq_t *rq);
static void syscall_zalloc(syscallrq_t *rq);
static void syscall_free(syscallrq_t *rq);
#if ((__OS_SYSTEM_MSG_ENABLE__ > 0) && (__OS_PRINTF_ENABLE__ > 0))
static void syscall_syslogread(syscallrq_t *rq);
#endif
static void syscall_kernelpanicdetect(syscallrq_t *rq);
static void syscall_processcreate(syscallrq_t *rq);
static void syscall_processkill(syscallrq_t *rq);
static void syscall_processcleanzombie(syscallrq_t *rq);
static void syscall_processgetsyncflag(syscallrq_t *rq);
static void syscall_processstatseek(syscallrq_t *rq);
static void syscall_processstatpid(syscallrq_t *rq);
static void syscall_processgetpid(syscallrq_t *rq);
static void syscall_processgetprio(syscallrq_t *rq);
static void syscall_threadstat(syscallrq_t *rq);
#if __OS_ENABLE_GETCWD__ == _YES_
static void syscall_getcwd(syscallrq_t *rq);
static void syscall_setcwd(syscallrq_t *rq);
#endif
static void syscall_threadcreate(syscallrq_t *rq);
static void syscall_threadkill(syscallrq_t *rq);
static void syscall_semaphorecreate(syscallrq_t *rq);
static void syscall_semaphoredestroy(syscallrq_t *rq);
static void syscall_mutexcreate(syscallrq_t *rq);
static void syscall_mutexdestroy(syscallrq_t *rq);
static void syscall_queuecreate(syscallrq_t *rq);
static void syscall_queuedestroy(syscallrq_t *rq);
#if __ENABLE_NETWORK__ == _YES_
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
#endif
#if __OS_ENABLE_SHARED_MEMORY__ == _YES_
static void syscall_shmcreate(syscallrq_t *rq);
static void syscall_shmattach(syscallrq_t *rq);
static void syscall_shmdetach(syscallrq_t *rq);
static void syscall_shmdestroy(syscallrq_t *rq);
#endif


/*==============================================================================
  Local objects
==============================================================================*/
#if __OS_TASK_KWORKER_MODE__ == 0
static queue_t *call_request;
#elif __OS_TASK_KWORKER_MODE__ == 1
static queue_t *call_nonblocking;
static queue_t *call_blocking;
#elif __OS_TASK_KWORKER_MODE__ == 2
#else
#error __OS_TASK_KWORKER_MODE__: unknown mode
#endif

/* syscall table */
static const syscallfunc_t syscalltab[] = {
        [SYSCALL_MOUNT ] = syscall_mount,
        [SYSCALL_UMOUNT] = syscall_umount,
        #if __OS_ENABLE_SHARED_MEMORY__ == _YES_
        [SYSCALL_SHMCREATE ] = syscall_shmcreate,
        [SYSCALL_SHMATTACH ] = syscall_shmattach,
        [SYSCALL_SHMDETACH ] = syscall_shmdetach,
        [SYSCALL_SHMDESTROY] = syscall_shmdestroy,
        #endif
        #if __OS_ENABLE_STATFS__ == _YES_
        [SYSCALL_GETMNTENTRY] = syscall_getmntentry,
        #endif
        #if __OS_ENABLE_MKNOD__ == _YES_
        [SYSCALL_MKNOD] = syscall_mknod,
        #endif
        #if __OS_ENABLE_MKDIR__ == _YES_
        [SYSCALL_MKDIR] = syscall_mkdir,
        #endif
        #if __OS_ENABLE_MKFIFO__ == _YES_
        [SYSCALL_MKFIFO] = syscall_mkfifo,
        #endif
        [SYSCALL_OPENDIR ] = syscall_opendir,
        [SYSCALL_CLOSEDIR] = syscall_closedir,
        [SYSCALL_READDIR ] = syscall_readdir,
        #if __OS_ENABLE_REMOVE__ == _YES_
        [SYSCALL_REMOVE] = syscall_remove,
        #endif
        #if __OS_ENABLE_RENAME__ == _YES_
        [SYSCALL_RENAME] = syscall_rename,
        #endif
        #if __OS_ENABLE_CHMOD__ == _YES_
        [SYSCALL_CHMOD] = syscall_chmod,
        #endif
        #if __OS_ENABLE_CHOWN__ == _YES_
        [SYSCALL_CHOWN] = syscall_chown,
        #endif
        #if __OS_ENABLE_STATFS__ == _YES_
        [SYSCALL_STATFS] = syscall_statfs,
        #endif
        #if __OS_ENABLE_FSTAT__ == _YES_
        [SYSCALL_STAT] = syscall_stat,
        #endif
        #if __OS_ENABLE_FSTAT__ == _YES_
        [SYSCALL_FSTAT] = syscall_fstat,
        #endif
        [SYSCALL_FOPEN ] = syscall_fopen,
        [SYSCALL_FCLOSE] = syscall_fclose,
        [SYSCALL_FWRITE] = syscall_fwrite,
        [SYSCALL_FREAD ] = syscall_fread,
        [SYSCALL_FSEEK ] = syscall_fseek,
        [SYSCALL_IOCTL ] = syscall_ioctl,
        [SYSCALL_FFLUSH] = syscall_fflush,
        [SYSCALL_SYNC  ] = syscall_sync,
        #if __OS_ENABLE_TIMEMAN__ == _YES_
        [SYSCALL_GETTIME] = syscall_gettime,
        [SYSCALL_SETTIME] = syscall_settime,
        #endif
        [SYSCALL_DRIVERINIT       ] = syscall_driverinit,
        [SYSCALL_DRIVERRELEASE    ] = syscall_driverrelease,
        [SYSCALL_MALLOC           ] = syscall_malloc,
        [SYSCALL_ZALLOC           ] = syscall_zalloc,
        [SYSCALL_FREE             ] = syscall_free,
        #if ((__OS_SYSTEM_MSG_ENABLE__ > 0) && (__OS_PRINTF_ENABLE__ > 0))
        [SYSCALL_SYSLOGREAD       ] = syscall_syslogread,
        #endif
        [SYSCALL_KERNELPANICDETECT ] = syscall_kernelpanicdetect,
        [SYSCALL_PROCESSCREATE     ] = syscall_processcreate,
        [SYSCALL_PROCESSKILL       ] = syscall_processkill,
        [SYSCALL_PROCESSCLEANZOMBIE] = syscall_processcleanzombie,
        [SYSCALL_PROCESSGETSYNCFLAG] = syscall_processgetsyncflag,
        [SYSCALL_PROCESSSTATSEEK   ] = syscall_processstatseek,
        [SYSCALL_PROCESSSTATPID    ] = syscall_processstatpid,
        [SYSCALL_PROCESSGETPID     ] = syscall_processgetpid,
        [SYSCALL_PROCESSGETPRIO    ] = syscall_processgetprio,
        [SYSCALL_THREADSTAT        ] = syscall_threadstat,
        #if __OS_ENABLE_GETCWD__ == _YES_
        [SYSCALL_GETCWD] = syscall_getcwd,
        [SYSCALL_SETCWD] = syscall_setcwd,
        #endif
        [SYSCALL_THREADCREATE    ] = syscall_threadcreate,
        [SYSCALL_THREADKILL      ] = syscall_threadkill,
        [SYSCALL_SEMAPHORECREATE ] = syscall_semaphorecreate,
        [SYSCALL_SEMAPHOREDESTROY] = syscall_semaphoredestroy,
        [SYSCALL_MUTEXCREATE     ] = syscall_mutexcreate,
        [SYSCALL_MUTEXDESTROY    ] = syscall_mutexdestroy,
        [SYSCALL_QUEUECREATE     ] = syscall_queuecreate,
        [SYSCALL_QUEUEDESTROY    ] = syscall_queuedestroy,
        #if __ENABLE_NETWORK__ == _YES_
        [SYSCALL_NETIFUP          ] = syscall_netifup,
        [SYSCALL_NETIFDOWN        ] = syscall_netifdown,
        [SYSCALL_NETIFSTATUS      ] = syscall_netifstatus,
        [SYSCALL_NETSOCKETCREATE  ] = syscall_netsocketcreate,
        [SYSCALL_NETSOCKETDESTROY ] = syscall_netsocketdestroy,
        [SYSCALL_NETBIND          ] = syscall_netbind,
        [SYSCALL_NETLISTEN        ] = syscall_netlisten,
        [SYSCALL_NETACCEPT        ] = syscall_netaccept,
        [SYSCALL_NETRECV          ] = syscall_netrecv,
        [SYSCALL_NETSEND          ] = syscall_netsend,
        [SYSCALL_NETGETHOSTBYNAME ] = syscall_netgethostbyname,
        [SYSCALL_NETSETRECVTIMEOUT] = syscall_netsetrecvtimeout,
        [SYSCALL_NETSETSENDTIMEOUT] = syscall_netsetsendtimeout,
        [SYSCALL_NETCONNECT       ] = syscall_netconnect,
        [SYSCALL_NETDISCONNECT    ] = syscall_netdisconnect,
        [SYSCALL_NETSHUTDOWN      ] = syscall_netshutdown,
        [SYSCALL_NETSENDTO        ] = syscall_netsendto,
        [SYSCALL_NETRECVFROM      ] = syscall_netrecvfrom,
        [SYSCALL_NETGETADDRESS    ] = syscall_netgetaddress,
        #endif
};

/*==============================================================================
  Exported objects
==============================================================================*/
_process_t *_kworker_proc;
#if (__OS_TASK_KWORKER_MODE__ == 0) || (__OS_TASK_KWORKER_MODE__ == 1)
pid_t       _syscall_client_PID[__OS_TASK_MAX_SYSTEM_THREADS__];
#endif

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

#if __OS_TASK_KWORKER_MODE__ == 0
        catcherr(err = _queue_create(SYSCALL_QUEUE_LENGTH, sizeof(syscallrq_t*),
                                     &call_request), exit);

#elif __OS_TASK_KWORKER_MODE__ == 1
        catcherr(err = _queue_create(SYSCALL_QUEUE_LENGTH, sizeof(syscallrq_t*),
                                     &call_nonblocking), exit);

        catcherr(err = _queue_create(SYSCALL_QUEUE_LENGTH, sizeof(syscallrq_t*),
                                     &call_blocking), exit);
#endif

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
#if __OS_TASK_KWORKER_MODE__ == 2
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
#else
                _process_t *proc; tid_t tid;
                _task_get_process_container(_THIS_TASK, &proc, &tid);

                _assert(proc);
                _assert(is_tid_in_range(proc, tid));

                flag_t *event_flags = NULL;
                _errno = _process_get_event_flags(proc, &event_flags);
                _assert(event_flags);

                if (!_errno && event_flags) {

                        syscallrq_t syscallrq = {
                                .syscall_no     = syscall,
                                .client_proc    = proc,
                                .client_thread  = tid,
                                .retptr         = retptr,
                                .err            = ESUCC
                        };

                        va_start(syscallrq.args, retptr);
                        {
                                syscallrq_t *syscallrq_ptr = &syscallrq;
                                queue_t     *call_rq       = NULL;

#if __OS_TASK_KWORKER_MODE__ == 0
                                call_rq = call_request;
#elif __OS_TASK_KWORKER_MODE__ == 1
                                if (syscall <= _SYSCALL_GROUP_0_OS_NON_BLOCKING) {
                                        call_rq = call_nonblocking;

                                } else if (syscall <= _SYSCALL_GROUP_1_BLOCKING) {
                                        call_rq = call_blocking;

                                } else {
                                        _errno = ENOSYS;
                                        va_end(syscallrq.args);
                                        return;
                                }
#endif

                                while (true) {
                                        if (_queue_send(call_rq,
                                                        &syscallrq_ptr,
                                                        2000) == ESUCC) {

                                                if (_flag_wait(event_flags,
                                                               _PROCESS_SYSCALL_FLAG(tid),
                                                               MAX_DELAY_MS) == ESUCC) {

                                                        if (syscallrq.err) {
                                                                _errno = syscallrq.err;
                                                        }
                                                }

                                                break;
                                        } else {
                                                _printk("syscall: busy timeout");
                                                _assert_msg(false, "Probably started to less I/O threads");
                                        }
                                }
                        }
                        va_end(syscallrq.args);
                }
        } else {
                _errno = ENOSYS;
        }
#endif
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

#if (__OS_TASK_KWORKER_MODE__ ==  0) || (__OS_TASK_KWORKER_MODE__ ==  1)
        static const thread_attr_t blocking_thread_attr = {
                .stack_depth = STACK_DEPTH_CUSTOM(__OS_IO_STACK_DEPTH__),
                .priority    = PRIORITY_NORMAL,
                .detached    = true
        };
#endif

        _task_get_process_container(_THIS_TASK, &_kworker_proc, NULL);
        _assert(_kworker_proc);

#if __OS_TASK_KWORKER_MODE__ == 1
        int iothrs_created = 0;

        for (int i = 0;
                (i < __OS_TASK_KWORKER_IO_THREADS__)
             && (i < __OS_TASK_MAX_SYSTEM_THREADS__ - 1); i++) {

                int err = _process_thread_create(_kworker_proc,
                                                 syscall_RTR,
                                                 &blocking_thread_attr,
                                                 call_blocking,
                                                 NULL);

                if (err) {
                        _assert_msg(false, "Fail in creating Ready-To-Run thread");
                        break;
                }

                iothrs_created++;
        }

        _printk("Created %d/%d Ready-To-Run IO threads",
                iothrs_created, __OS_TASK_KWORKER_IO_THREADS__);
#endif

        u64_t sync_period_ref = _kernel_get_time_ms();

        for (;;) {
#if __OS_TASK_KWORKER_MODE__ == 0
                syscallrq_t *sysrq = NULL;
                if (_queue_receive(call_request, &sysrq, FS_CACHE_SYNC_PERIOD_MS) == ESUCC) {

                        _process_clean_up_killed_processes();

                        if (sysrq->syscall_no <= _SYSCALL_GROUP_0_OS_NON_BLOCKING) {
                                syscall_do(sysrq);
                        } else {
                                _kernel_release_resources();

                                /* create new syscall task */
                                switch (_process_thread_create(_kworker_proc,
                                                               syscall_do,
                                                               &blocking_thread_attr,
                                                               sysrq, NULL) ) {
                                case ESUCC:
                                        _task_yield();
                                        break;

                                // destroy top process to get free memory
                                case ENOMEM:
                                        _assert_msg(false, "no free memory");
                                        _process_clean_up_killed_processes();
                                        _kernel_release_resources();
                                        _vfs_sync();
                                        _queue_send(call_request, &sysrq, MAX_DELAY_MS);
                                        break;

                                default:
                                        _queue_send(call_request, &sysrq, MAX_DELAY_MS);
                                        _sleep_ms(5);
                                        break;
                                }
                        }
                }
#elif __OS_TASK_KWORKER_MODE__ == 1
                syscallrq_t *sysrq = NULL;
                if (_queue_receive(call_nonblocking, &sysrq, FS_CACHE_SYNC_PERIOD_MS) == ESUCC) {
                        _process_clean_up_killed_processes();
                        _kernel_release_resources();
                        syscall_do(sysrq);
                }
#elif __OS_TASK_KWORKER_MODE__ == 2
                _sleep_ms(1000);
#endif

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

#if (__OS_TASK_KWORKER_MODE__ == 0) || (__OS_TASK_KWORKER_MODE__ == 1)
        tid_t tid = _process_get_active_thread(NULL);
        _assert(is_tid_in_range(_process_get_active(), tid));

        flag_t *flags = NULL;
        if (_process_get_event_flags(sysrq->client_proc, &flags) != ESUCC) {
                _kernel_panic_report(_KERNEL_PANIC_DESC_CAUSE_INTERNAL_1);
        }
        _assert(flags);

        _process_get_pid(sysrq->client_proc, &_syscall_client_PID[tid]);
        _assert(_syscall_client_PID[tid] > 0);

#if __OS_TASK_KWORKER_THREADS_PRIORITY__ == 1
        int priority = 0;
        _process_get_priority(_syscall_client_PID[tid], &priority);
        _task_set_priority(NULL, priority);
#endif
#endif
        _process_syscall_stat_inc(sysrq->client_proc, _kworker_proc);

        syscalltab[sysrq->syscall_no](sysrq);

#if (__OS_TASK_KWORKER_MODE__ == 0) || (__OS_TASK_KWORKER_MODE__ == 1)
        _syscall_client_PID[tid] = 0;

        if (_flag_set(flags, _PROCESS_SYSCALL_FLAG(sysrq->client_thread)) != ESUCC) {
                _assert(false);
        }
#endif
}

#if __OS_TASK_KWORKER_MODE__ == 1
//==============================================================================
/**
 * @brief  Function handle thread ready-to-run feature.
 *
 * @param  rq           request information
 */
//==============================================================================
static void syscall_RTR(void *rq_queue)
{
        for (;;) {
                syscallrq_t *sysrq;

                if (_queue_receive(rq_queue, &sysrq, MAX_DELAY_MS) == ESUCC) {
                        _process_clean_up_killed_processes();
                        syscall_do(sysrq);
                }
        }
}
#endif

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

#if __OS_ENABLE_STATFS__ == _YES_
//==============================================================================
/**
 * @brief  This syscall return information about selected file system.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_getmntentry(syscallrq_t *rq)
{
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
}
#endif

#if __OS_ENABLE_MKNOD__ == _YES_
//==============================================================================
/**
 * @brief  This syscall create device node.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_mknod(syscallrq_t *rq)
{
        struct vfs_path pathname;
        pathname.CWD  = _process_get_CWD(GETPROCESS());
        pathname.PATH = LOADARG(const char *);
        GETARG(const char *, mod_name);
        GETARG(int *, major);
        GETARG(int *, minor);

        SETERRNO(_vfs_mknod(&pathname, _dev_t__create(_module_get_ID(mod_name), *major, *minor)));
        SETRETURN(int, GETERRNO() ? 0 : -1);
}
#endif

#if __OS_ENABLE_MKDIR__ == _YES_
//==============================================================================
/**
 * @brief  This syscall create directory.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_mkdir(syscallrq_t *rq)
{
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);
        GETARG(mode_t *, mode);

        SETERRNO(_vfs_mkdir(&path, *mode));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}
#endif

#if __OS_ENABLE_MKFIFO__ == _YES_
//==============================================================================
/**
 * @brief  This syscall create FIFO pipe.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_mkfifo(syscallrq_t *rq)
{
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);
        GETARG(mode_t *, mode);

        SETERRNO(_vfs_mkfifo(&path, *mode));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}
#endif

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

#if __OS_ENABLE_REMOVE__ == _YES_
//==============================================================================
/**
 * @brief  This syscall remove selected file.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_remove(syscallrq_t *rq)
{
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);

        SETERRNO(_vfs_remove(&path));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}
#endif

#if __OS_ENABLE_RENAME__ == _YES_
//==============================================================================
/**
 * @brief  This syscall rename selected file.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_rename(syscallrq_t *rq)
{
        struct vfs_path oldname;
        oldname.CWD  = _process_get_CWD(GETPROCESS());
        oldname.PATH = LOADARG(const char *);

        struct vfs_path newname;
        newname.CWD  = _process_get_CWD(GETPROCESS());
        newname.PATH = LOADARG(const char *);

        SETERRNO(_vfs_rename(&oldname, &newname));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}
#endif

#if __OS_ENABLE_CHMOD__ == _YES_
//==============================================================================
/**
 * @brief  This syscall change mode of selected file.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_chmod(syscallrq_t *rq)
{
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);
        GETARG(mode_t *, mode);

        SETERRNO(_vfs_chmod(&path, *mode));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}
#endif

#if __OS_ENABLE_CHOWN__ == _YES_
//==============================================================================
/**
 * @brief  This syscall change owner and group of selected file.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_chown(syscallrq_t *rq)
{
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);
        GETARG(uid_t *, owner);
        GETARG(gid_t *, group);

        SETERRNO(_vfs_chown(&path, *owner, *group));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}
#endif

#if __OS_ENABLE_FSTAT__ == _YES_
//==============================================================================
/**
 * @brief  This syscall read statistics of selected file by path.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_stat(syscallrq_t *rq)
{
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);
        GETARG(struct stat *, buf);

        SETERRNO(_vfs_stat(&path, buf));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
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
        GETARG(FILE *, file);
        GETARG(struct stat *, buf);
        SETERRNO(_vfs_fstat(file, buf));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}
#endif

#if __OS_ENABLE_STATFS__ == _YES_
//==============================================================================
/**
 * @brief  This syscall read statistics of file system mounted in selected path.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_statfs(syscallrq_t *rq)
{
        struct vfs_path path;
        path.CWD  = _process_get_CWD(GETPROCESS());
        path.PATH = LOADARG(const char *);
        GETARG(struct statfs *, buf);

        SETERRNO(_vfs_statfs(&path, buf));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}
#endif

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
        GETARG(size_t *, size);
        GETARG(size_t *, count);
        GETARG(FILE*, file);

        size_t wrcnt = 0;
        SETERRNO(_vfs_fwrite(buf, (*count) * (*size), &wrcnt, file));
        SETRETURN(size_t, wrcnt / (*size));
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
        GETARG(size_t *, size);
        GETARG(size_t *, count);
        GETARG(FILE *, file);

        size_t rdcnt = 0;
        SETERRNO(_vfs_fread(buf, (*count) * (*size), &rdcnt, file));
        SETRETURN(size_t, rdcnt / (*size));
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

#if __OS_ENABLE_TIMEMAN__ == _YES_
//==============================================================================
/**
 * @brief  This syscall return current time value (UTC timestamp).
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_gettime(syscallrq_t *rq)
{
        GETARG(struct timeval*, timeval);
        SETERRNO(_gettime(timeval));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall set current system time (UTC timestamp).
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_settime(syscallrq_t *rq)
{
        GETARG(time_t *, time);
        SETERRNO(_settime(time));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}
#endif

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
        int   err = _kmalloc(_MM_PROG, *size, &mem);
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
        int   err = _kzalloc(_MM_PROG, *size, &mem);
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

#if ((__OS_SYSTEM_MSG_ENABLE__ > 0) && (__OS_PRINTF_ENABLE__ > 0))
//==============================================================================
/**
 * @brief  This syscall enable system log functionality in selected file.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_syslogread(syscallrq_t *rq)
{
        GETARG(char *, str);
        GETARG(size_t *, len);
        GETARG(const struct timeval *, from_time);
        GETARG(struct timeval *, curr_time);
        SETRETURN(size_t, _printk_read(str, *len, from_time, curr_time));
}
#endif

//==============================================================================
/**
 * @brief  This syscall check if kernel panic occurred in last session.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_kernelpanicdetect(syscallrq_t *rq)
{
        GETARG(FILE*, file);
        SETRETURN(bool, _kernel_panic_detect(file));
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

#if __OS_ENABLE_GETCWD__ == _YES_
//==============================================================================
/**
 * @brief  This syscall return CWD of current process.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_getcwd(syscallrq_t *rq)
{
        GETARG(char *, buf);
        GETARG(size_t *, size);

        const char *cwd = NULL;
        if (buf && *size) {
                cwd = _process_get_CWD(GETPROCESS());
               _strlcpy(buf, cwd, *size);
        }

        SETRETURN(char*, cwd ? buf : NULL);
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
        GETARG(const char *, cwd);
        SETERRNO(_process_set_CWD(GETPROCESS(), cwd));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}
#endif

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

#if __ENABLE_NETWORK__ == _YES_
//==============================================================================
/**
 * @brief  This syscall up network.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_netifup(syscallrq_t *rq)
{
        GETARG(NET_family_t *, family);
        GETARG(const NET_generic_config_t *, config);

        SETERRNO(_net_ifup(*family, config));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
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
        GETARG(NET_family_t *, family);

        SETERRNO(_net_ifdown(*family));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
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
        GETARG(NET_family_t *, family);
        GETARG(NET_generic_status_t *, status);

        SETERRNO(_net_ifstatus(*family, status));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
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
        GETARG(NET_family_t*, family);
        GETARG(NET_protocol_t*, protocol);

        SOCKET *socket = NULL;
        int     err    = _net_socket_create(*family, *protocol, &socket);
        if (err == ESUCC) {
                err = _process_register_resource(GETPROCESS(), cast(res_header_t*, socket));
                if (err != ESUCC) {
                        _net_socket_destroy(socket);
                        socket = NULL;
                }
        }

        SETERRNO(err);
        SETRETURN(SOCKET*, socket);
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
        GETARG(SOCKET *, socket);
        GETARG(const NET_generic_sockaddr_t *, addr);

        SETERRNO(_net_socket_bind(socket, addr));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
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
        GETARG(SOCKET *, socket);

        SETERRNO(_net_socket_listen(socket));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
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
        GETARG(SOCKET *, socket);
        GETARG(void *, buf);
        GETARG(size_t *, len);
        GETARG(NET_flags_t *, flags);

        size_t recved = 0;
        SETERRNO(_net_socket_recv(socket, buf, *len, *flags, &recved));
        SETRETURN(int, GETERRNO() == ESUCC ? cast(int, recved) : -1);
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
        GETARG(SOCKET *, socket);
        GETARG(void *, buf);
        GETARG(size_t *, len);
        GETARG(NET_flags_t *, flags);
        GETARG(NET_generic_sockaddr_t *, sockaddr);

        size_t recved = 0;
        SETERRNO(_net_socket_recvfrom(socket, buf, *len, *flags, sockaddr, &recved));
        SETRETURN(int, GETERRNO() == ESUCC ? cast(int, recved) : -1);
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
        GETARG(SOCKET *, socket);
        GETARG(const void *, buf);
        GETARG(size_t *, len);
        GETARG(NET_flags_t *, flags);

        size_t sent = 0;
        SETERRNO(_net_socket_send(socket, buf, *len, *flags, &sent));
        SETRETURN(int, GETERRNO() == ESUCC ? cast(int, sent) : -1);
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
        GETARG(SOCKET *, socket);
        GETARG(const void *, buf);
        GETARG(size_t *, len);
        GETARG(NET_flags_t *, flags);
        GETARG(const NET_generic_sockaddr_t *, to_addr);

        size_t sent = 0;
        SETERRNO(_net_socket_sendto(socket, buf, *len, *flags, to_addr, &sent));
        SETRETURN(int, GETERRNO() == ESUCC ? cast(int, sent) : -1);
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
        GETARG(NET_family_t *, family);
        GETARG(const char *, name);
        GETARG(NET_generic_sockaddr_t *, addr);

        SETERRNO(_net_gethostbyname(*family, name, addr));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
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
        GETARG(SOCKET *, socket);
        GETARG(uint32_t *, timeout);

        SETERRNO(_net_socket_set_recv_timeout(socket, *timeout));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
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
        GETARG(SOCKET *, socket);
        GETARG(uint32_t *, timeout);

        SETERRNO(_net_socket_set_send_timeout(socket, *timeout));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
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
        GETARG(SOCKET *, socket);
        GETARG(const NET_generic_sockaddr_t *, addr);

        SETERRNO(_net_socket_connect(socket, addr));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
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
        GETARG(SOCKET *, socket);

        SETERRNO(_net_socket_disconnect(socket));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
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
        GETARG(SOCKET *, socket);
        GETARG(NET_shut_t *, how);

        SETERRNO(_net_socket_shutdown(socket, *how));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
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
        GETARG(SOCKET *, socket);
        GETARG(NET_generic_sockaddr_t *, sockaddr);

        SETERRNO(_net_socket_getaddress(socket, sockaddr));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}
#endif

#if __OS_ENABLE_SHARED_MEMORY__ == _YES_
//==============================================================================
/**
 * @brief  This syscall creates shared memory region.
 *
 * @param  rq                   syscall request
 */
//==============================================================================
static void syscall_shmcreate(syscallrq_t *rq)
{
        GETARG(const char *, key);
        GETARG(const size_t *, size);
        SETERRNO(_shm_create(key, *size));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
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
        GETARG(const char *, key);
        GETARG(void **, mem);
        GETARG(size_t *, size);

        pid_t pid = 0;
        SETERRNO(_process_get_pid(GETPROCESS(), &pid));
        if (GETERRNO() == ESUCC) {
                SETERRNO(_shm_attach(key, mem, size, pid));
        }

        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
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
        GETARG(const char *, key);

        pid_t pid = 0;
        SETERRNO(_process_get_pid(GETPROCESS(), &pid));
        if (GETERRNO() == ESUCC) {
                SETERRNO(_shm_detach(key, pid));
        }

        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
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
        GETARG(const char *, key);
        SETERRNO(_shm_destroy(key));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
