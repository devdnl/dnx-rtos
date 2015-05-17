/*=========================================================================*//**
@file    progman.c

@author  Daniel Zorychta

@brief   This file support programs layer

@note    Copyright (C) 2012, 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Include files
==============================================================================*/
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "fs/vfs.h"
#include "kernel/process.h"
#include "kernel/kwrapper.h"
#include "kernel/kpanic.h"
#include "kernel/printk.h"
#include "lib/llist.h"
#include "lib/cast.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define USERSPACE
#define KERNELSPACE
#define foreach_resource(_v, _l)        for (res_header_t *_v = _l; _v; _v = _v->next)
#define foreach_process(_v)             for (_process_t *_v = process_list; _v; _v = reinterpret_cast(_process_t*, _v->header.next))

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
typedef struct _prog_data pdata_t;

struct _process {
        res_header_t     header;                /* resource header                    */
        task_t          *task;                  /* pointer to task handle             */
        FILE            *f_stdin;               /* stdin file                         */
        FILE            *f_stdout;              /* stdout file                        */
        FILE            *f_stderr;              /* stderr file                        */
        void            *globals;               /* address to global variables        */
        res_header_t    *res_list;              /* list of used resources             */
        const char      *cwd;                   /* current working path               */
        const pdata_t   *pdata;                 /* program data                       */
        sem_t           *exit_sem;              /* process exit semaphore             */
        char            **argv;                 /* program arguments                  */
        u16_t            argc;                  /* number of arguments                */
        u16_t            status;                /* program status (return value)      */
        pid_t            pid;                   /* process ID                         */
        int              errnov;                /* program error number               */
        u32_t            timecnt;               /* counter used to calculate CPU load */
        bool             has_parent:1;          /* process has parent                 */
};

struct _thread {
        res_header_t     header;                /* resource header                    */
        task_t          *task;                  /* pointer to task handle             */
        _process_t      *process;               /* process ID (thread owner)          */
        void            *arg;                   /* thread function argument           */
        sem_t           *exit_sem;              /* thread exit semaphore (join func.) */
        tid_t            tid;                   /* thread ID                          */
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void process_code(void *mainfn);
static void thread_code(void *thrfunc);
static int  process_thread_destroy(_thread_t *thread);
static void process_destroy_all_resources(_process_t *proc);
static bool resource_destroy(res_header_t *resource);
static int  argtab_create(const char *str, u16_t *argc, char **argv[]);
static void argtab_destroy(char **argv);
static int  find_program(const char *name, const struct _prog_data **prog);
static int  allocate_process_globals(_process_t *proc, const struct _prog_data *usrprog);
static int  process_apply_attributes(_process_t *proc, const process_attr_t *attr);
static void process_get_stat(_process_t *proc, process_stat_t *stat);

/*==============================================================================
  Local object definitions
==============================================================================*/
static pid_t       PID_cnt;
static tid_t       TID_cnt;
static _process_t *process_list;
static _process_t *active_process;
static bool        CPU_load_enabled;
static u32_t       CPU_total_time_last;

/*==============================================================================
  Exported object definitions
==============================================================================*/
/* CPU total time */
u32_t _CPU_total_time;

/* standard input */
FILE *stdin;

/* standard output */
FILE *stdout;

/* standard error */
FILE *stderr;

/* error number */
int _errno;

/* global variables */
struct _GVAR_STRUCT_NAME *global;

/*==============================================================================
  External object definitions
==============================================================================*/
extern const struct _prog_data _prog_table[];
extern const int               _prog_table_size;

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief  Create a new process
 *
 * @param[in]  cmd      command (name + arguments)
 * @param[in]  attr     process attributes (use NULL for default attributes)
 * @param[out] pid      PID of created process (can be NULL)
 *
 * @return One of errno value.
 */
//==============================================================================
KERNELSPACE int _process_create(const char *cmd, const process_attr_t *attr, pid_t *pid)
{
        int result = EINVAL;

        if (cmd && cmd[0] != '\0') {
                _process_t *proc;
                result = _kzalloc(_MM_KRN, sizeof(_process_t), static_cast(void**, &proc));
                if (result == ESUCC) {

                        proc->header.type = RES_TYPE_PROCESS;

                        // parse program's arguments
                        result = argtab_create(cmd, &proc->argc, &proc->argv);
                        if (result != ESUCC)
                                goto finish;

                        // find program
                        result = find_program(proc->argv[0], &proc->pdata);
                        if (result != ESUCC)
                                goto finish;

                        // allocate program's global variables
                        result = allocate_process_globals(proc, proc->pdata);
                        if (result != ESUCC)
                                goto finish;

                        // set configured program settings
                        result = process_apply_attributes(proc, attr);
                        if (result != ESUCC)
                                goto finish;

                        // set default program settings
                        proc->pid = ++PID_cnt;

                        // create program's task
                        result = _task_create(process_code,
                                              proc->pdata->name,
                                              *proc->pdata->stack_depth,
                                              proc->pdata->main,
                                              proc,
                                              &proc->task);

                        if (result == ESUCC) {
                                if (attr) {
                                        _task_set_priority(proc->task, attr->priority);
                                }

                                if (pid) {
                                        *pid = proc->pid;
                                }

                                _kernel_scheduler_lock();
                                {
                                        if (process_list == NULL) {
                                                process_list = proc;
                                        } else {
                                                proc->header.next = static_cast(res_header_t*, process_list);
                                                process_list      = proc;
                                        }
                                }
                                _kernel_scheduler_unlock();
                        }

                        finish:
                        if (result != ESUCC) {
                                process_destroy_all_resources(proc);
                                _kfree(_MM_KRN, static_cast(void**, &proc));
                        }
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief  Free all process's resources and remove from process list
 *
 * @param  pid          process ID
 * @param  status       process exit status (can be NULL)
 *
 * @return One of errno value
 */
//==============================================================================
KERNELSPACE int _process_destroy(pid_t pid, int *status)
{
        int result = EINVAL;

        _process_t *prev = NULL;
        foreach_process(curr) {
                if (curr->pid == pid) {
                        _kernel_scheduler_lock();
                        {
                                if (process_list == curr) {
                                        process_list = static_cast(_process_t*, curr->header.next);
                                } else {
                                        prev->header.next = curr->header.next;
                                }
                        }
                        _kernel_scheduler_unlock();

                        if (curr->exit_sem) {
                                _semaphore_destroy(curr->exit_sem);
                                curr->exit_sem = NULL;
                        }

                        if (status) {
                                *status = curr->status;
                        }

                        process_destroy_all_resources(curr);

                        curr->header.next = NULL;
                        curr->header.type = RES_TYPE_UNKNOWN;
                        _kfree(_MM_KRN, static_cast(void**, &curr));

                        result = ESUCC;
                        break;
                }

                prev = curr;
        }

        return result;
}

//==============================================================================
/**
 * @brief  Clear all process resources, set exit code and close task. Process is
 *         not removed from process list (zombie state).
 *
 * @param  proc         process container
 * @param  status       status (exit code)
 *
 * @return One of errno value
 */
//==============================================================================
KERNELSPACE int _process_exit(_process_t *proc, int status)
{
        int result = EINVAL;

        if (proc && proc->header.type == RES_TYPE_PROCESS) {
                proc->status = status;
                process_destroy_all_resources(proc);
                result = ESUCC;
        }

        return result;
}

//==============================================================================
/**
 * @brief  It works almost the same as _process_exit() but set exit code to -1 and
 *         print on process's terminal suitable message.
 *
 * @param  proc         process container
 *
 * @return One of errno value
 */
//==============================================================================
KERNELSPACE int _process_abort(_process_t *proc)
{
        if (proc && proc->header.type == RES_TYPE_PROCESS) {
                static const char *aborted = "Aborted\n";
                size_t wrcnt;
                _vfs_fwrite(aborted, strlen(aborted), &wrcnt, proc->f_stderr);
        }

        return _process_exit(proc, -1);
}

//==============================================================================
/**
 * @brief  Function get CWD path for selected task
 *
 * @param  proc         process container
 *
 * @return CWD path (always valid)
 */
//==============================================================================
KERNELSPACE const char *_process_get_CWD(_process_t *proc)
{
        if (proc && proc->header.type == RES_TYPE_PROCESS) {
                return proc->cwd;
        } else {
                return "";
        }
}

//==============================================================================
/**
 * @brief  Function register selected resource in selected task/process
 *
 * @param  proc         process container
 * @param  resource     resource to register
 *
 * @return One of errno value.
 */
//==============================================================================
KERNELSPACE int _process_register_resource(_process_t *proc, res_header_t *resource)
{
        if (proc && proc->header.type == RES_TYPE_PROCESS) {
                _kernel_scheduler_lock();
                {
                        if (proc->res_list == NULL) {
                                proc->res_list = resource;
                        } else {
                                resource->next = proc->res_list;
                                proc->res_list = resource;
                        }
                }
                _kernel_scheduler_unlock();

                return ESUCC;
        } else {
                return ESRCH;
        }
}

//==============================================================================
/**
 * @brief  Function release selected resource of selected type (type is confirmation)
 *
 * @param  proc         process container
 * @param  resource     resource address to release
 * @param  type         resource type
 *
 * @return One of errno value.
 */
//==============================================================================
KERNELSPACE int _process_release_resource(_process_t *proc, res_header_t *resource, res_type_t type)
{
        int result = ESRCH;

        if (proc && proc->header.type == RES_TYPE_PROCESS) {
                _kernel_scheduler_lock();

                result = ENOENT;

                res_header_t *prev     = NULL;
                int           max_deep = 1024;

                foreach_resource(curr, proc->res_list) {
                        if (curr == resource) {
                                if (curr->type == type) {
                                        if (proc->res_list == curr) {
                                                proc->res_list = proc->res_list->next;
                                        } else {
                                                prev->next = curr->next;
                                        }

                                        if (resource_destroy(curr)) {
                                                result = ESUCC;
                                        } else {
                                                _kernel_panic_report(_KERNEL_PANIC_DESC_CAUSE_INTERNAL);
                                        }
                                } else {
                                        result = EFAULT;
                                        break;
                                }
                        }

                        prev = curr;

                        if (--max_deep == 0) {
                                _kernel_panic_report(_KERNEL_PANIC_DESC_CAUSE_INTERNAL);
                        }
                }

                _kernel_scheduler_unlock();
        }

        return result;
}

//==============================================================================
/**
 * @brief  Function return collected process statistics
 *
 * @param  seek     process seek (start from 0)
 * @param  stat     process statistics
 *
 * @return One of errno value (ESUCC, EINVAL, ENOENT).
 */
//==============================================================================
KERNELSPACE int _process_get_stat_seek(size_t seek, process_stat_t *stat)
{
        int result = EINVAL;

        if (stat) {
                result = ENOENT;

                _kernel_scheduler_lock();

                foreach_process(proc) {
                        if (seek == 0) {
                                process_get_stat(proc, stat);
                                result = ESUCC;
                                break;
                        }

                        seek--;
                }

                _kernel_scheduler_unlock();
        }

        return result;
}

//==============================================================================
/**
 * @brief  Function return collected process statistics
 *
 * @param  pid      PID
 * @param  stat     process statistics
 *
 * @return One of errno value (ESUCC, EINVAL, ENOENT).
 */
//==============================================================================
KERNELSPACE int _process_get_stat_pid(pid_t pid, process_stat_t *stat)
{
        int result = EINVAL;

        if (pid) {
                result = ENOENT;

                _kernel_scheduler_lock();

                foreach_process(proc) {
                        if (proc->pid == pid) {
                                process_get_stat(proc, stat);
                                result = ESUCC;
                                break;
                        }
                }

                _kernel_scheduler_unlock();
        }

        return result;
}

//==============================================================================
/**
 * @brief  Function return stderr file of selected task/process
 *
 * @param  proc         process container
 *
 * @return On success file pointer is returned, otherwise NULL.
 */
//==============================================================================
KERNELSPACE FILE *_process_get_stderr(_process_t *proc)
{
        if (proc && proc->header.type == RES_TYPE_PROCESS) {
                return proc->f_stderr;
        } else {
                return NULL;
        }
}

//==============================================================================
/**
 * @brief  Function return name of selected task/process
 *
 * @param  proc         process container
 *
 * @return On success process name is returned, otherwise NULL.
 */
//==============================================================================
KERNELSPACE const char *_process_get_name(_process_t *proc)
{
        if (proc && proc->header.type == RES_TYPE_PROCESS) {
                return proc->pdata->name;
        } else {
                return NULL;
        }
}

//==============================================================================
/**
 * @brief  Function return PID of selected task/process
 *
 * @param[in]  proc         process container
 * @param[out] pid          process PID
 *
 * @return One of errno value.
 */
//==============================================================================
KERNELSPACE int _process_get_pid(_process_t *proc, pid_t *pid)
{
        int result = EINVAL;

        if (proc && proc->header.type == RES_TYPE_PROCESS && pid) {
                *pid   = proc->pid;
                result = ESUCC;
        }

        return result;
}

//==============================================================================
/**
 * @brief  Function return exit semaphore handle
 *
 * @param  proc         process container
 * @param  sem          pointer to semaphore pointer
 *
 * @return One of errno value
 */
//==============================================================================
KERNELSPACE int _process_get_exit_sem(pid_t pid, sem_t **sem)
{
        int result = EINVAL;

        if (pid && sem) {
                _kernel_scheduler_lock();

                foreach_process(proc) {
                        if (proc->pid == pid) {
                                *sem   = proc->exit_sem;
                                result = ESUCC;
                                break;
                        }
                }

                _kernel_scheduler_unlock();
        }

        return result;
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
KERNELSPACE int _process_get_priority(pid_t pid, int *prio)
{
        int result = EINVAL;

        if (pid && prio) {
                _kernel_scheduler_lock();

                foreach_process(proc) {
                        if (proc->pid == pid) {
                                *prio  = _task_get_priority(proc->task);
                                result = ESUCC;
                                break;
                        }
                }

                _kernel_scheduler_unlock();
        }

        return result;
}

//==============================================================================
/**
 * @brief  Function return process container of selected task
 *
 * @param  taskhdl      task
 * @param  master       true: main process, false: thread (can be NULL)
 *
 * @return On success process container pointer is returned, otherwise NULL.
 */
//==============================================================================
KERNELSPACE _process_t *_process_get_container_by_task(task_t *taskhdl, bool *master)
{
        _process_t *process = _task_get_tag(taskhdl);

        if (process) {
                if (process->header.type == RES_TYPE_THREAD) {
                        process = reinterpret_cast(_thread_t*, process)->process;

                        if (master) {
                                *master = false;
                        }
                } else {
                        if (master) {
                                *master = true;
                        }
                }
        }

        return process;
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
KERNELSPACE int _process_thread_create(_process_t          *proc,
                                       thread_func_t        func,
                                       const thread_attr_t *attr,
                                       void                *arg,
                                       tid_t               *tid,
                                       task_t              **task)
{
        int result = EINVAL;

        if (proc && proc->header.type == RES_TYPE_PROCESS && func) {
                _thread_t *thread;
                result = _kzalloc(_MM_KRN, sizeof(_thread_t), static_cast(void*, &thread));
                if (result == ESUCC) {
                        thread->header.type = RES_TYPE_THREAD;
                        thread->process     = proc;
                        thread->arg         = arg;

                        _semaphore_create(1, 0, &thread->exit_sem);
                        if (result != ESUCC)
                                goto finish;

//                        _kernel_scheduler_lock(); // FIXME is it necessary?

                        result = _task_create(thread_code,
                                              "thread", // FIXME
                                              (attr ? attr->stack_depth : STACK_DEPTH_HUGE),
                                              func,
                                              thread,
                                              &thread->task);
                        if (result != ESUCC)
                                goto finish;

                        result = _process_register_resource(proc, static_cast(res_header_t*, thread));
                        if (result != ESUCC)
                                goto finish;

                        thread->tid = ++TID_cnt;

                        if (attr) {
                                _task_set_priority(thread->task, attr->priority);
                        }

                        if (tid) {
                                *tid = thread->tid;
                        }

                        if (task) {
                                *task = thread->task;
                        }

                        finish:
                        if (result != ESUCC) {
                                process_thread_destroy(thread);
                        }

//                        _kernel_scheduler_unlock(); // FIXME is it necessary?
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
KERNELSPACE _thread_t *_process_thread_get_container(_process_t *proc, tid_t tid)
{
        _thread_t *thread = NULL;

        if (proc && proc->header.type == RES_TYPE_PROCESS && tid) {
                foreach_resource(res, proc->res_list) {
                        if (res->type == RES_TYPE_THREAD) {
                                if (reinterpret_cast(_thread_t *, res)->tid == tid) {
                                        thread = static_cast(_thread_t *, res);
                                        break;
                                }
                        }
                }
        }

        return thread;
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
KERNELSPACE task_t *_process_thread_get_task(_thread_t *thread)
{
        return thread && thread->header.type == RES_TYPE_THREAD ? thread->task : NULL;
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
KERNELSPACE tid_t _process_thread_get_tid(_thread_t *thread)
{
        return thread && thread->header.type == RES_TYPE_THREAD ? thread->tid : 0;
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
KERNELSPACE int _process_thread_exit(_thread_t *thread)
{
        int result = EINVAL;

        if (thread && thread->header.type == RES_TYPE_THREAD) {
                if (thread->task) {
                        _task_suspend(thread->task);
                        _task_destroy(thread->task);
                        thread->task = NULL;
                }

                if (thread->exit_sem) {
                        result = _semaphore_signal(thread->exit_sem);
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief  Function return exit semaphore that is used to synchronize parent
 *         thread (process) with child process.
 *
 * @param  proc         process object
 * @param  tid          thread ID
 * @param  sem          semaphore pointer
 *
 * @return One of errno value.
 */
//==============================================================================
KERNELSPACE int _process_thread_get_exit_sem(_process_t *proc, tid_t tid, sem_t **sem)
{
        int result = EINVAL;

        if (proc && proc->header.type == RES_TYPE_PROCESS && tid && sem) {
                _kernel_scheduler_lock();

                foreach_resource(res, proc->res_list) {
                        _thread_t *thread = static_cast(_thread_t*, res);
                        if (thread->header.type == RES_TYPE_THREAD && thread->tid == tid) {
                                *sem   = thread->exit_sem;
                                result = ESUCC;
                                break;
                        }
                }

                _kernel_scheduler_unlock();
        }

        return result;
}

//==============================================================================
/**
 * @brief  Function enables CPU load measurement
 *
 * @param  None
 *
 * @return None
 */
//==============================================================================
USERSPACE void _CLM_enable(void)
{
        _kernel_scheduler_lock();
        {
                _CPU_total_time     = 0;
                CPU_total_time_last = 0;
                CPU_load_enabled    = true;
        }
        _kernel_scheduler_unlock();
}

//==============================================================================
/**
 * @brief  Function disables CPU load measurement
 *
 * @param  None
 *
 * @return None
 */
//==============================================================================
USERSPACE void _CLM_disable(void)
{
        CPU_load_enabled = false;
}

//==============================================================================
/**
 * @brief  This function start user process code [USERLAND]
 *
 * @param  mainfn         process argument - process main function
 *
 * @return None
 */
//==============================================================================
USERSPACE static void process_code(void *mainfn)
{
        process_func_t funcmain = mainfn;
        _process_t     *proc    = _task_get_tag(_THIS_TASK);

        proc->status = funcmain(proc->argc, proc->argv);

        if (proc->has_parent) {
                syscall(SYSCALL_EXIT, NULL, &proc->status);
        } else {
                syscall(SYSCALL_PROCESSDESTROY, NULL, &proc->pid, NULL);
        }

        _task_exit();
}

//==============================================================================
/**
 * @brief  This function start user thread code [USERLAND]
 *
 * @param  thrfunc        process argument - process main function
 *
 * @return None
 */
//==============================================================================
USERSPACE static void thread_code(void *thrfunc)
{
        thread_func_t func   = thrfunc;
        _thread_t    *thread = _task_get_tag(_THIS_TASK);

        func(thread->arg);

        syscall(SYSCALL_THREADEXIT, NULL, &thread->tid);

        _task_exit();
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static int process_thread_destroy(_thread_t *thread)
{
        int result = EINVAL;

        if (thread) {
                if (thread->task) {
                        _task_suspend(thread->task);
                        _task_destroy(thread->task);
                        thread->task = NULL;
                }

                if (thread->exit_sem) {
                        _semaphore_destroy(thread->exit_sem);
                        thread->exit_sem = NULL;
                }

                result = _kfree(_MM_KRN, static_cast(void*, &thread));
        }

        return result;
}

//==============================================================================
/**
 * @brief  Function destroy (release) all process resources
 *
 * @param  proc     selected process container
 *
 * @return None
 */
//==============================================================================
static void process_destroy_all_resources(_process_t *proc)
{
        if (proc->exit_sem) {
                _semaphore_signal(proc->exit_sem);
        }

        if (proc->task) {
                _task_destroy(proc->task);
                proc->task = NULL;
        }

        if (proc->argv) {
                argtab_destroy(proc->argv);
                proc->argv = NULL;
                proc->argc = 0;
        }

        // suspend all threads
        foreach_resource(res, proc->res_list) {
                if (res->type == RES_TYPE_THREAD) {
                        _task_suspend(reinterpret_cast(_thread_t*, res)->task);
                }
        }

        // free all resources
        foreach_resource(res, proc->res_list) {
                if (resource_destroy(res) == false) {
                        _printk("Unknown object: %p\n", res);
                }
        }

        proc->res_list = NULL;
        proc->f_stdin  = NULL;
        proc->f_stdout = NULL;
        proc->f_stderr = NULL;
        proc->globals  = NULL;
        proc->cwd      = NULL;
}

//==============================================================================
/**
 * @brief  Function gets process statistics
 *
 * @param  proc         process
 * @param  stat         statistics container
 *
 * @return None
 */
//==============================================================================
static void process_get_stat(_process_t *proc, process_stat_t *stat)
{
        memset(stat, 0, sizeof(process_stat_t));

        stat->name            = proc->pdata->name;
        stat->pid             = proc->pid;
        stat->stack_size      = *proc->pdata->stack_depth;
        stat->stack_max_usage = stat->stack_size - _task_get_free_stack(proc->task);
        stat->threads_count   = 1;
        stat->zombie          = proc->task == NULL;
        stat->priority        = _task_get_priority(proc->task);

        foreach_resource(res, proc->res_list) {
                switch (res->type) {
                case RES_TYPE_FILE      : stat->files_count++; break;
                case RES_TYPE_DIR       : stat->dir_count++; break;
                case RES_TYPE_MUTEX     : stat->mutexes_count++; break;
                case RES_TYPE_QUEUE     : stat->queue_count++; break;
                case RES_TYPE_SEMAPHORE : stat->semaphores_count++; break;
                case RES_TYPE_THREAD    : stat->threads_count++; break;
                case RES_TYPE_MEMORY    : stat->memory_block_count++;
                                          stat->memory_usage += _mm_get_block_size(res);
                                          break;
                default: break;
                }
        }

        stat->cpu_load_total_cnt = _CPU_total_time;
        stat->cpu_load_cnt       = proc->timecnt;
        proc->timecnt            = 0;
}

//==============================================================================
/**
 * @brief  Function apply process attributes
 *
 * @param  proc     process
 * @param  atrr     attributes
 *
 * @return One of errno value.
 */
//==============================================================================
static int process_apply_attributes(_process_t *proc, const process_attr_t *attr)
{
        int result = ESUCC;

        if (attr) {
                /*
                 * Apply stdin settings
                 * - if f_stdin is set then function use this resource as reference
                 * - if f_stdin is NULL and p_stdin is NULL then function set stdin as NULL
                 * - if f_stdin is NULL and p_stdin is valid then function open new file and use as stdin
                 */
                if (attr->f_stdin) {
                        proc->f_stdin = attr->f_stdin;

                } else if (attr->p_stdin) {
                        result = _vfs_fopen(attr->p_stdin, "a+", &proc->f_stdin);
                        if (result == ESUCC) {
                                _process_register_resource(proc, static_cast(res_header_t*, proc->f_stdin));
                        } else {
                                goto finish;
                        }
                }

                /*
                 * Apply stdout settings
                 * - if f_stdout is set then function use this resource as reference
                 * - if f_stdout is NULL and p_stdout is NULL then function set stdout as NULL
                 * - if f_stdout is NULL and p_stdout is valid then function open new file and use as stdout
                 * - if p_stdout is the same as p_stdin then function use stdin as reference of stdout
                 */
                if (attr->f_stdout) {
                        proc->f_stdout = attr->f_stdout;

                } else if (attr->p_stdout) {
                        if (strcmp(attr->p_stdout, attr->p_stdin) == 0) {
                                proc->f_stdout = proc->f_stdin;

                        } else {
                                result = _vfs_fopen(attr->p_stdout, "a", &proc->f_stdout);
                                if (result == ESUCC) {
                                        _process_register_resource(proc, static_cast(res_header_t*, proc->f_stdout));
                                } else {
                                        goto finish;
                                }
                        }
                }

                /*
                 * Apply stderr settings
                 * - if f_stderr is set then function use this resource as reference
                 * - if f_stderr is NULL and p_stderr is NULL then function set stderr as NULL
                 * - if f_stderr is NULL and p_stderr is valid then function open new file and use as stderr
                 * - if p_stderr is the same as p_stdin then function use stdin as reference of stderr
                 * - if p_stderr is the same as p_stdout then function use stdout as reference of stderr
                 */
                if (attr->f_stderr) {
                        proc->f_stderr = attr->f_stderr;

                } else if (attr->p_stderr) {
                        if (strcmp(attr->p_stderr, attr->p_stdin) == 0) {
                                proc->f_stderr = proc->f_stdin;

                        } else if (strcmp(attr->p_stderr, attr->p_stdout) == 0) {
                                proc->f_stderr = proc->f_stdout;

                        } else {
                                result = _vfs_fopen(attr->p_stderr, "a", &proc->f_stderr);
                                if (result == ESUCC) {
                                        _process_register_resource(proc, static_cast(res_header_t*, proc->f_stderr));
                                } else {
                                        goto finish;
                                }
                        }
                }

                /*
                 * Create exit semaphore object if parent exist
                 */
                if (attr->has_parent) {
                        result = _semaphore_create(1, 0, &proc->exit_sem);
                        if (result != ESUCC) {
                                goto finish;
                        }
                }

                /*
                 * Apply no-parent attribute
                 */
                proc->has_parent = attr->has_parent;

                /*
                 * Apply Current Working Directory path
                 */
                proc->cwd = attr->cwd;
        }

        finish:
        return result;
}

//==============================================================================
/**
 * @brief  Function destroy (release) selected resource
 *
 * @param  resource     resource to release
 *
 * @return On success true is returned, otherwise false.
 */
//==============================================================================
static bool resource_destroy(res_header_t *resource)
{
        res_header_t *res2free = resource;

        switch (resource->type) {
        case RES_TYPE_FILE:
                _vfs_fclose(static_cast(FILE*, res2free), true);
                break;

        case RES_TYPE_DIR:
                _vfs_closedir(static_cast(DIR*, res2free));
                break;

        case RES_TYPE_MEMORY:
                _kfree(_MM_PROG, static_cast(void*, &res2free));
                break;

        case RES_TYPE_MUTEX:
                _mutex_destroy(static_cast(mutex_t*, res2free));
                break;

        case RES_TYPE_QUEUE:
                _queue_destroy(static_cast(queue_t*, res2free));
                break;

        case RES_TYPE_SEMAPHORE:
                _semaphore_destroy(static_cast(sem_t*, res2free));
                break;

        case RES_TYPE_THREAD:
                process_thread_destroy(static_cast(_thread_t*, res2free));
                break;

        default:
                return false;
        }

        return true;
}

//==============================================================================
/**
 * @brief Function create new table with argument pointers
 *
 * @param[in]  str              argument string
 * @param[out] argc             number of argument
 * @param[out] argv             pointer to pointer of argument array
 *
 * @return One of errno value.
 */
//==============================================================================
static int argtab_create(const char *str, u16_t *argc, char **argv[])
{
        int result = EINVAL;

        if (str && argc && argv) {

                llist_t *largs;
                result = _llist_create_krn(_MM_KRN, NULL, NULL, &largs);

                if (result == ESUCC) {
                        // parse arguments
                        while (*str != '\0') {
                                // skip spaces
                                str += strspn(str, " ");

                                // select character to find as end of argument
                                bool quo = false;
                                char find = ' ';
                                if (*str == '\'' || *str == '"') {
                                        quo = true;
                                        find = *str;
                                        str++;
                                }

                                // find selected character
                                const char *start = str;
                                const char *end   = strchr(str, find);

                                // check if string end is reached
                                if (!end) {
                                        end = strchr(str, '\0');
                                } else {
                                        end++;
                                }

                                // calculate argument length (without nul character)
                                int str_len = end - start;
                                if (str_len == 0)
                                        break;

                                if (quo || *(end - 1) == ' ') {
                                        str_len--;
                                }

                                // add argument to list
                                char *arg;
                                result = _kmalloc(_MM_KRN, str_len + 1, static_cast(void**, &arg));
                                if (result == ESUCC) {
                                        strncpy(arg, start, str_len);
                                        arg[str_len] = '\0';

                                        if (_llist_push_back(largs, arg) == NULL) {
                                                goto finish;
                                        }
                                } else {
                                        goto finish;
                                }

                                // next token
                                str = end;
                        }

                        // create table with arguments
                        int no_of_args = _llist_size(largs);
                        *argc = no_of_args;

                        char **arg = NULL;
                        result = _kmalloc(_MM_KRN, (no_of_args + 1) * sizeof(char*), static_cast(void*, &arg));
                        if (result == ESUCC) {
                                for (int i = 0; i < no_of_args; i++) {
                                        arg[i] = _llist_take_front(largs);
                                }

                                arg[no_of_args] = NULL;
                                *argv = arg;
                        }

                        finish:
                        _llist_destroy(largs);
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief  Function remove argument table
 *
 * @param  argv          pointer to argument table (must be ended with NULL)
 *
 * @return None
 */
//==============================================================================
static void argtab_destroy(char **argv)
{
        if (argv) {
                int n = 0;
                while (argv[n]) {
                        _kfree(_MM_KRN, static_cast(void*, &argv[n]));
                        n++;
                }

                _kfree(_MM_KRN, static_cast(void*, &argv));
        }
}

//==============================================================================
/**
 * @brief  Function find program by name and return program descriptor container
 *
 * @param[in]  name         program name
 * @param[out] prog         program container
 *
 * @return One of errno value.
 */
//==============================================================================
static int find_program(const char *name, const struct _prog_data **prog)
{
        static const size_t kworker_stack_depth  = STACK_DEPTH_LOW;
        static const size_t kworker_globals_size = 0;
        static const struct _prog_data kworker   = {.globals_size = &kworker_globals_size,
                                                    .main         = _syscall_kworker_master,
                                                    .name         = "kworker",
                                                    .stack_depth  = &kworker_stack_depth};

        int result = ENOENT;

        if (strncmp(name, "kworker", 32) == 0) {
                *prog  = &kworker;
                result = ESUCC;

        } else {
                for (int i = 0; i < _prog_table_size; i++) {
                        if (strncmp(_prog_table[i].name, name, 128) == 0) {
                                *prog  = &_prog_table[i];
                                result = ESUCC;
                                break;
                        }
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief  Function allocate global variable for selected process
 *
 * @param  proc         process
 * @param  usrprog      program descriptor
 *
 * @return One of errno value.
 */
//==============================================================================
static int allocate_process_globals(_process_t *proc, const struct _prog_data *usrprog)
{
        int result = ESUCC;

        if (*usrprog->globals_size > 0) {
                res_header_t *mem;
                result = _kzalloc(_MM_PROG, *usrprog->globals_size, static_cast(void*, &mem));

                if (result == ESUCC) {
                        proc->globals = &mem[1];
                        _process_register_resource(proc, mem);
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief  Function copy task context to standard variables (stdin, stdout, stderr,
 *         global, errno)
 *
 * @param  None
 *
 * @return None
 */
//==============================================================================
KERNELSPACE void _task_switched_in(void)
{
#if (CONFIG_MONITOR_CPU_LOAD > 0)
        if (CPU_load_enabled) {
                CPU_total_time_last = _CPU_total_time;
                _CPU_total_time    += _cpuctl_get_CPU_load_counter_delta();
        }
#endif

        active_process = _process_get_container_by_task(_THIS_TASK, NULL);

        if (active_process) {
                stdin  = active_process->f_stdin;
                stdout = active_process->f_stdout;
                stderr = active_process->f_stderr;
                global = active_process->globals;
                _errno = active_process->errnov;
        } else {
                stdin  = NULL;
                stdout = NULL;
                stderr = NULL;
                global = NULL;
                _errno = 0;
        }
}

//==============================================================================
/**
 * @brief  Function copy standard variables (stdin, stdout, stderr, global, errno)
 *         to task context
 *
 * @param  None
 *
 * @return None
 */
//==============================================================================
KERNELSPACE void _task_switched_out(void)
{
        if (active_process) {
                active_process->f_stdin  = stdin;
                active_process->f_stdout = stdout;
                active_process->f_stderr = stderr;
                active_process->globals  = global;
                active_process->errnov   = _errno;

                #if (CONFIG_MONITOR_CPU_LOAD > 0)
                if (CPU_load_enabled) {
                        _CPU_total_time += _cpuctl_get_CPU_load_counter_delta();
                        active_process->timecnt += (_CPU_total_time - CPU_total_time_last);
                }
                #endif
        } else {
                #if (CONFIG_MONITOR_CPU_LOAD > 0)
                        if (CPU_load_enabled) {
                                _CPU_total_time += _cpuctl_get_CPU_load_counter_delta();
                        }
                #endif
        }
}

/*==============================================================================
  End of file
==============================================================================*/
