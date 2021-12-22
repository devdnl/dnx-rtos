/*=========================================================================*//**
@file    process.c

@author  Daniel Zorychta

@brief   This file support processes

@note    Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "fs/vfs.h"
#include "net/netm.h"
#include "kernel/process.h"
#include "kernel/kwrapper.h"
#include "kernel/kpanic.h"
#include "kernel/printk.h"
#include "kernel/sysfunc.h"
#include "kernel/khooks.h"
#include "lib/llist.h"
#include "lib/cast.h"
#include "lib/misc.h"
#include "mm/shm.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define foreach_resource(_v, _l)        for (res_header_t *_v = _l; _v; _v = _v->next)
#define foreach_process(_v, _l)         for (_process_t *_v = _l; _v; _v = cast(_process_t*, _v->header.next))

#define NO_SYSCALL                      UINT8_MAX
#define SHEBANGLEN                      64

// critical section - mutex wait must set to max possible
#define ATOMIC(_mtx) for (int __ = 0; __ == 0;)\
        for (int _e = _mutex_lock(_mtx, _MAX_DELAY_MS); _e == 0 && __ == 0; _mutex_unlock(_mtx), __++)

#define PROC_MAX_THREADS(proc)          (((proc)->flag & FLAG_KWORKER) ? __OS_TASK_MAX_SYSTEM_THREADS__ : __OS_TASK_MAX_USER_THREADS__)

#define is_proc_valid(proc)             (_mm_is_object_in_heap(proc) && (proc->header.self == proc) && (proc->header.type == RES_TYPE_PROCESS))
#define is_tid_in_range(proc, tid)      (tid < PROC_MAX_THREADS(proc))

#define FLAG_DETACHED                   (1 << 0)
#define FLAG_KWORKER                    (1 << 1)
#define FLAG_PROC_RUN                   (1 << 2)

#define PID_MIN                         1
#define PID_MAX                         999

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
typedef struct _prog_data pdata_t;

typedef struct {
        task_t          *task;                  //!< task
        u32_t            timecnt;               //!< counter used to calculate CPU load
        int              status;                //!< thread exit status
        int              errnov;                //!< thread error number
        u16_t            CPU_load;              //!< CPU load
        u16_t            syscalls_per_sec;      //!< syscalls/s
        u16_t            syscalls_ctr;          //!< syscall counter
        u16_t            stack_size;            //!< stack size
        bool             kernelspace;           //!< execution in kernel space
        tid_t            id;                    //!< thread id
        u8_t             curr_syscall;          //!< current syscall
} task_data_t;

struct _process {
        res_header_t     header;                //!< resource header
        kflag_t          *event;                //!< events for exit indicator and syscall finish
        kfile_t          *f_stdin;              //!< stdin file
        kfile_t          *f_stdout;             //!< stdout file
        kfile_t          *f_stderr;             //!< stderr file
        void            *globals;               //!< address to global variables
        res_header_t    *res_list;              //!< list of used resources
        u32_t            res_list_size;         //!< size of resources list
        char            *cwd;                   //!< current working path
        const _program_entry_t *pdata;          //!< program data
        char            **argv;                 //!< program arguments
        u8_t             argc;                  //!< number of arguments
        pid_t            pid;                   //!< process ID
        u8_t             flag;                  //!< control flags
        u8_t             curr_task;             //!< current working task (thread)
        task_data_t      taskdata[];            //!< array of tasks data
};

typedef struct {
        void            *func;
        struct _process *proc;
        task_data_t     *task_data;
        void            *arg;
} task_args_t;

enum {
        STORAGE_IDX_PROC,
        STORAGE_IDX_TASK_DATA,
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void process_code(void *mainfn);
static void thread_code(void *args);
static void process_destroy_all_resources(_process_t *proc);
static int  resource_destroy(res_header_t *resource);
static int  argtab_create(const char *str, u8_t *argc, char **argv[]);
static void argtab_destroy(char **argv);
static int  find_program(const char *name, const _program_entry_t **prog);
static int  allocate_process_globals(_process_t *proc, const _program_entry_t *usrprog);
static int  process_apply_attributes(_process_t *proc, const _process_attr_t *attr);
static void process_get_stat(_process_t *proc, _process_stat_t *stat);
static void process_move_list(_process_t *proc, _process_t **list_from, _process_t **list_to);
static int  get_pid(pid_t *pid);
static void destroy_all_tasks(_process_t *proc, bool with_zero);

#if __OS_SYSTEM_SHEBANG_ENABLE__ > 0
static bool is_cmd_path(const char *cmd);
static int  analyze_shebang(_process_t *proc, const char *cmd, char **cmdarg);
#endif

/*==============================================================================
  Local object definitions
==============================================================================*/
static pid_t          PID_cnt;
static _process_t    *active_process_list;
static _process_t    *destroy_process_list;
static _process_t    *zombie_process_list;
static _process_t    *active_process;
static u32_t          CPU_total_time_last;
static _avg_CPU_load_t avg_CPU_load_calc;
static _avg_CPU_load_t avg_CPU_load_result;
static kmtx_t        *process_mtx;
static kmtx_t        *kworker_mtx;

/*==============================================================================
  Exported object definitions
==============================================================================*/
/* CPU total time */
u32_t _CPU_total_time = 0;

/* standard input */
void *_stdin = NULL;

/* standard output */
void *_stdout = NULL;

/* standard error */
void *_stderr = NULL;

/* error number */
int _errno = ESUCC;

/* global variables */
void *_global = NULL;

/*==============================================================================
  External object definitions
==============================================================================*/
extern u32_t _uptime_counter_sec;

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
int _process_create(const char *cmd, const _process_attr_t *attr, pid_t *pid)
{
        if (!process_mtx) {
                _assert(_mutex_create(KMTX_TYPE_RECURSIVE, &process_mtx) == ESUCC);
        }

        if (!kworker_mtx) {
                _assert(_mutex_create(KMTX_TYPE_RECURSIVE, &kworker_mtx) == ESUCC);
        }

        if (!cmd) {
                return ENOENT;
        }

        if (!attr) {
                return EINVAL;
        }

        char        *cmdarg    = NULL;
        _process_t  *proc      = NULL;
        task_args_t *task_args = NULL;
        size_t       threads   = isstreq(cmd, "kworker") ? __OS_TASK_MAX_SYSTEM_THREADS__
                                                         : __OS_TASK_MAX_USER_THREADS__;

        int err = _kzalloc(_MM_KRN, sizeof(_process_t) + (sizeof(task_data_t) * threads),
                           _CPUCTL_FAST_MEM, 0, 0, cast(void**, &proc));
        if (!err) {
                proc->header.self = proc;
                proc->header.type = RES_TYPE_PROCESS;

                err = process_apply_attributes(proc, attr);
                if (err) goto finish;

#if __OS_SYSTEM_SHEBANG_ENABLE__ > 0
                u8_t  level   = 8;
                const char *c = cmd;
                while (c && level-- && is_cmd_path(c)) {

                        err = analyze_shebang(proc, c, &cmdarg);

                        if (c && c != cmd) {
                                _kfree(_MM_KRN, cast(void**, &c));
                        }

                        c = cmdarg;

                        if (err) goto finish;
                }
#endif

                err = argtab_create(cmdarg ? cmdarg : cmd, &proc->argc, &proc->argv);
                if (err) goto finish;

                if (cmdarg) {
                        _kfree(_MM_KRN, cast(void**, &cmdarg));
                }

                err = find_program(proc->argv[0], &proc->pdata);
                if (err) goto finish;

                err = allocate_process_globals(proc, proc->pdata);
                if (err) goto finish;

                err = get_pid(&proc->pid);
                if (err) goto finish;

                if (proc->pdata->main != _syscall_kworker_process) {
                        err = _flag_create(&proc->event);
                        if (err) goto finish;
                } else {
                        proc->flag |= FLAG_KWORKER;
                }

                err = _kzalloc(_MM_KRN, sizeof(*task_args), _CPUCTL_FAST_MEM, 0, 0, cast(void**, &task_args));
                if (err) goto finish;

                ATOMIC(process_mtx) {
                        proc->taskdata[0].stack_size  = *proc->pdata->stack_depth + _STACK_DEPTH_MINIMAL;
                        proc->taskdata[0].kernelspace = (proc->flag & FLAG_KWORKER);
                        proc->taskdata[0].id = 0;
                        proc->taskdata[0].syscalls_per_sec = NO_SYSCALL;

                        task_args->func = proc->pdata->main;
                        task_args->proc = proc;
                        task_args->task_data = &proc->taskdata[0];

                        err = _task_create(process_code,
                                           proc->pdata->name,
                                           *proc->pdata->stack_depth + _STACK_DEPTH_MINIMAL,
                                           task_args,
                                           &proc->taskdata[0].task);
                        if (!err) {
                                if (attr) {
                                        _task_set_priority(proc->taskdata[0].task,
                                                           attr->priority);
                                }

                                if (pid) {
                                        *pid = proc->pid;
                                }

                                if (active_process_list == NULL) {
                                        active_process_list = proc;

                                } else {
                                        proc->header.next = cast(res_header_t*, active_process_list);
                                        active_process_list = proc;
                                }
                        }
                }
        }

        finish:
        if (err) {
                if (task_args) {
                        _kfree(_MM_KRN, cast(void**, &task_args));
                }

                if (cmdarg) {
                        _kfree(_MM_KRN, cast(void**, &cmdarg));
                }

                if (proc) {
                        process_destroy_all_resources(proc);
                        proc->header.self = NULL;
                        proc->header.type = RES_TYPE_UNKNOWN;
                        _kfree(_MM_KRN, cast(void**, &proc));
                }
        }

        return err;
}

//==============================================================================
/**
 * Function clean up killed processes. If process has parent then is moved to
 * the zombie list. Only parent can remove zombie process.
 */
//==============================================================================
void _process_clean_up_killed_processes(void)
{
        if (destroy_process_list) {
                ATOMIC(process_mtx) {
                        while (destroy_process_list) {
                                _process_t *proc = destroy_process_list;

                                process_destroy_all_resources(proc);

                                if (not (proc->flag & FLAG_DETACHED)) {
                                        process_move_list(proc,
                                                          &destroy_process_list,
                                                          &zombie_process_list);
                                } else {
                                        destroy_process_list = cast(_process_t *, proc->header.next);
                                        _flag_destroy(proc->event);
                                        proc->event = NULL;
                                        proc->header.self = NULL;
                                        proc->header.type = RES_TYPE_UNKNOWN;
                                        _kfree(_MM_KRN, cast(void*, &proc));
                                }
                        }
                }
        }
}

//==============================================================================
/**
 * Kill selected process. Kill moves process to the process destroy list. There
 * are releases resources used by process.
 *
 * @param  pid          process ID
 *
 * @return One of errno value.
 */
//==============================================================================
int _process_kill(pid_t pid)
{
        int err = ESRCH;

        ATOMIC(process_mtx) {
                foreach_process(proc, active_process_list) {

                        if (proc->pid == pid) {
                                if (proc->event) {
                                        _flag_set(proc->event, _PROCESS_EXIT_FLAG(0));
                                }

                                destroy_all_tasks(proc, true);

                                process_move_list(proc,
                                                  &active_process_list,
                                                  &destroy_process_list);

                                err = ESUCC;
                                break;
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * Function remove zombie process.
 *
 * @param  proc         zombie process object
 * @param  status       program status
 */
//==============================================================================
void _process_remove_zombie(_process_t *proc, int *status)
{
        _assert(is_proc_valid(proc));

        ATOMIC(process_mtx) {
                _process_t *prev = NULL;
                foreach_process(p, zombie_process_list) {
                        if (p == proc) {

                                if (prev) {
                                        prev->header.next = proc->header.next;
                                } else {
                                        zombie_process_list = cast(_process_t *,
                                                                   proc->header.next);
                                }

                                if (status) {
                                        *status = proc->taskdata[0].status;
                                }

                                _flag_destroy(proc->event);
                                _kfree(_MM_KRN, cast(void*, &proc));

                                break;
                        } else {
                                prev = p;
                        }
                }
        }
}

//==============================================================================
/**
 * Function exit from process execution. Is called when main() function execute
 * return function or if user call exit() function.
 *
 * @param  proc         process container
 * @param  status       status (exit code)
 */
//==============================================================================
void _process_exit(_process_t *proc, int status)
{
        if (is_proc_valid(proc)) {
                proc->taskdata[0].status = status;

                va_list none;
                if (proc->f_stdin) {
                        _vfs_vfioctl(proc->f_stdin, IOCTL_VFS__DEFAULT_RD_MODE, none);
                        _vfs_vfioctl(proc->f_stdin, IOCTL_VFS__DEFAULT_WR_MODE, none);
                }

                if (proc->f_stdout) {
                        _vfs_vfioctl(proc->f_stdout, IOCTL_VFS__DEFAULT_RD_MODE, none);
                        _vfs_vfioctl(proc->f_stdout, IOCTL_VFS__DEFAULT_WR_MODE, none);
                }

                if (proc->f_stderr) {
                        _vfs_vfioctl(proc->f_stderr, IOCTL_VFS__DEFAULT_RD_MODE, none);
                        _vfs_vfioctl(proc->f_stderr, IOCTL_VFS__DEFAULT_WR_MODE, none);
                }


                ATOMIC(process_mtx) {
                        destroy_all_tasks(proc, false);

                        process_move_list(proc, &active_process_list, &destroy_process_list);

                        proc->taskdata[0].task = NULL;

                        _process_clean_up_killed_processes();

                        if (proc->event) {
                                _flag_set(proc->event, _PROCESS_EXIT_FLAG(0));
                        }
                }

                _task_exit();
        } else {
                _assert(is_proc_valid(proc));
        }
}

//==============================================================================
/**
 * @brief  The function works almost the same as _process_exit(), but set exit
 *         code to -1 and print on process's terminal suitable message.
 *
 * @param  proc         process container
 */
//==============================================================================
void _process_abort(_process_t *proc)
{
        if (is_proc_valid(proc)) {
                static const char *aborted = "Aborted\n";
                size_t wrcnt;
                _vfs_fwrite(aborted, strlen(aborted), &wrcnt, proc->f_stderr);
                _process_exit(proc, -1);
        } else {
                _assert(is_proc_valid(proc));
        }
}

//==============================================================================
/**
 * @brief  Function get CWD path of selected process.
 *
 * @param  proc         process container
 *
 * @return CWD path (always valid).
 */
//==============================================================================
const char *_process_get_CWD(_process_t *proc)
{
        if (is_proc_valid(proc)) {
                return proc->cwd ? proc->cwd : "/";
        } else {
                return "/";
        }
}

//==============================================================================
/**
 * @brief  Function set CWD path of selected process. If NULL passed then
 *         CWD is set to empty string ("").
 *
 * @param  proc         process container
 * @param  CWD          current working directory path (can be NULL)
 *
 * @return One of errno value.
 */
//==============================================================================
int _process_set_CWD(_process_t *proc, const char *CWD)
{
        int err = EINVAL;

        if (is_proc_valid(proc) && CWD && CWD[0] == '/') {
                char *cwd;
                err = _kmalloc(_MM_KRN, strsize(CWD), NULL, 0, 0, cast(void*, &cwd));
                if (!err) {
                        strcpy(cwd, CWD);
                        _vfs_realpath(cwd, SUB_SLASH);

                        if (proc->cwd) {
                                _kfree(_MM_KRN, cast(void*, &proc->cwd));
                        }

                        proc->cwd = cwd;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function return collected statistics of process.
 *
 * @param  seek     process seek (start from 0)
 * @param  stat     process statistics
 *
 * @return One of errno value (ESUCC, EINVAL, ENOENT).
 */
//==============================================================================
int _process_get_stat_seek(size_t seek, _process_stat_t *stat)
{
        int err = EINVAL;

        if (stat) {
                err = ENOENT;

                ATOMIC(process_mtx) {
                        _process_t *proc = NULL;

                        foreach_process(p, active_process_list) {
                                if (seek == 0) {
                                        proc = p;
                                        goto finish;
                                }

                                seek--;
                        }

                        foreach_process(p, destroy_process_list) {
                                if (seek == 0) {
                                        proc = p;
                                        goto finish;
                                }

                                seek--;
                        }

                        foreach_process(p, zombie_process_list) {
                                if (seek == 0) {
                                        proc = p;
                                        goto finish;
                                }

                                seek--;
                        }

                        finish:
                        if (proc) {
                                process_get_stat(proc, stat);
                                err = ESUCC;
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function return collected process statistics.
 *
 * @param  pid      PID
 * @param  stat     process statistics
 *
 * @return One of errno value (ESUCC, EINVAL, ENOENT).
 */
//==============================================================================
int _process_get_stat_pid(pid_t pid, _process_stat_t *stat)
{
        int err = EINVAL;

        if (pid) {
                err = ENOENT;

                ATOMIC(process_mtx) {
                        _process_t *proc = NULL;

                        foreach_process(p, active_process_list) {
                                if (p->pid == pid) {
                                        proc = p;
                                        goto finish;
                                }
                        }

                        foreach_process(p, destroy_process_list) {
                                if (p->pid == pid) {
                                        proc = p;
                                        goto finish;
                                }
                        }

                        foreach_process(p, zombie_process_list) {
                                if (p->pid == pid) {
                                        proc = p;
                                        goto finish;
                                }
                        }

                        finish:
                        if (proc) {
                                process_get_stat(proc, stat);
                                err = ESUCC;
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function return stderr file of selected process.
 *
 * @param  proc         process container
 *
 * @return File pointer. Can be NULL if process does not exists or file not set.
 */
//==============================================================================
kfile_t *_process_get_stderr(_process_t *proc)
{
        if (is_proc_valid(proc)) {
                return proc->f_stderr;
        } else {
                return NULL;
        }
}

//==============================================================================
/**
 * @brief  Function return name of selected process.
 *
 * @param  proc         process container
 *
 * @return On success process name is returned, otherwise NULL.
 */
//==============================================================================
const char *_process_get_name(_process_t *proc)
{
        if (is_proc_valid(proc)) {
                return proc->pdata->name;
        } else {
                return NULL;
        }
}

//==============================================================================
/**
 * @brief  Function return number of all processes (even zombies).
 *
 * @return Number of processes.
 */
//==============================================================================
size_t _process_get_count(void)
{
        size_t count = 0;

        ATOMIC(process_mtx) {
                foreach_process(proc, active_process_list) {
                        count++;
                }

                foreach_process(p, destroy_process_list) {
                        count++;
                }

                foreach_process(p, zombie_process_list) {
                        count++;
                }
        }

        return count;
}

//==============================================================================
/**
 * @brief  Function return active process.
 *
 * @return Object of active process or NULL if unknown process (e.g. idle task).
 */
//==============================================================================
_process_t *_process_get_active(void)
{
        return active_process;
}

//==============================================================================
/**
 * @brief Function return PID of selected process.
 *
 * @param proc         process container
 * @param pid          process PID
 *
 * @return One of errno value.
 */
//==============================================================================
int _process_get_pid(_process_t *proc, pid_t *pid)
{
        int err = EINVAL;

        if (is_proc_valid(proc) && pid) {
                *pid = proc->pid;
                err  = ESUCC;
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function return event flags object.
 *
 * @param  proc         process container
 * @param  flag         pointer to flag set
 *
 * @return One of errno value.
 */
//==============================================================================
int _process_get_event_flags(_process_t *proc, kflag_t **flag)
{
        int err = EINVAL;

        if (is_proc_valid(proc) && flag) {
                ATOMIC(process_mtx) {
                        *flag = proc->event;
                        err   = ESUCC;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function return priority of selected process.
 *
 * @param  pid      process ID
 * @param  prio     priority
 *
 * @return One of errno value.
 */
//==============================================================================
int _process_get_priority(pid_t pid, int *prio)
{
        int err = EINVAL;

        if (pid && prio) {
                _kernel_scheduler_lock();
                {
                        if (active_process->pid == pid) {
                                *prio = _task_get_priority(active_process->taskdata[0].task);
                                err   = ESUCC;

                        } else {
                                foreach_process(proc, active_process_list) {
                                        if (proc->pid == pid) {
                                                *prio  = _task_get_priority(proc->taskdata[0].task);
                                                err = ESUCC;
                                                break;
                                        }
                                }
                        }
                }
                _kernel_scheduler_unlock();
        }

        return err;
}

//==============================================================================
/**
 * Function return container of selected process by PID.
 *
 * @param  pid          process ID
 * @param  process      process container
 *
 * @return One of errno value.
 */
//==============================================================================
int _process_get_container(pid_t pid, _process_t **process)
{
        int err = EINVAL;

        if (pid && process) {
                _kernel_scheduler_lock();
                {
                        if (active_process->pid == pid) {
                                *process = active_process;
                                err = ESUCC;
                        }

                        if (err) {
                                foreach_process(proc, active_process_list) {
                                        if (proc->pid == pid) {
                                                *process = proc;
                                                err = ESUCC;
                                                break;
                                        }
                                }
                        }

                        if (err) {
                                foreach_process(proc, destroy_process_list) {
                                        if (proc->pid == pid) {
                                                *process = proc;
                                                err = ESUCC;
                                                break;
                                        }
                                }
                        }

                        if (err) {
                                foreach_process(proc, zombie_process_list) {
                                        if (proc->pid == pid) {
                                                *process = proc;
                                                err = ESUCC;
                                                break;
                                        }
                                }
                        }
                }
                _kernel_scheduler_unlock();
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function return thread ID of active thread
 *
 * @param  process              process (can be NULL, then active process is used)
 *
 * @return Thread ID.
 */
//==============================================================================
tid_t _process_get_active_thread(_process_t *process)
{
        tid_t tid = UINT8_MAX;

        _kernel_scheduler_lock();
        {
                if (process == NULL) {
                        process = active_process;
                }

                if (process) {
                        tid = process->curr_task;
                }

                _assert(tid < PROC_MAX_THREADS(active_process));
        }
        _kernel_scheduler_unlock();

        return tid;
}


//==============================================================================
/**
 * @brief  Function return PID of active process.
 *
 * @return Thread ID.
 */
//==============================================================================
pid_t _process_get_active_process_pid(void)
{
        pid_t pid = 0;

        _kernel_scheduler_lock();
        {
                if (active_process) {
                        pid = active_process->pid;
                }
        }
        _kernel_scheduler_unlock();

        return pid;
}

//==============================================================================
/**
 * @brief  Function return max number of threads that can be handled by process.
 *
 * @param  proc         process
 *
 * @return On success number of threads, otherwise 0.
 */
//==============================================================================
u8_t _process_get_max_threads(_process_t *proc)
{
        u8_t threads = 0;

        if (is_proc_valid(proc)) {
                threads = PROC_MAX_THREADS(proc);
        }

        return threads;
}

//==============================================================================
/**
 * @brief  Function register selected resource in selected process.
 *
 * @param  proc         process container
 * @param  resource     resource to register
 *
 * @return One of errno value.
 */
//==============================================================================
int _process_register_resource(_process_t *proc, res_header_t *resource)
{
        if (is_proc_valid(proc)) {
                kmtx_t *mtx = (proc == _kworker_proc) ? kworker_mtx : process_mtx;

                ATOMIC(mtx) {
                        if (proc->res_list == NULL) {
                                proc->res_list = resource;
                        } else {
                                resource->next = proc->res_list;
                                proc->res_list = resource;
                        }

                        proc->res_list_size++;
                }

                return ESUCC;
        } else {
                return ESRCH;
        }
}

//==============================================================================
/**
 * @brief  Function release selected resource of selected type (type is confirmation).
 *
 * @param  proc         process container
 * @param  resource     resource address to release
 * @param  type         resource type
 *
 * @return One of errno value.
 */
//==============================================================================
int _process_release_resource(_process_t *proc, res_header_t *resource, res_type_t type)
{
        int err = ESRCH;

        if (is_proc_valid(proc) && (resource->self == resource)) {
                err = ENOENT;
                res_header_t *obj_to_destroy = NULL;
                kmtx_t *mtx = (proc == _kworker_proc) ? kworker_mtx : process_mtx;

                ATOMIC(mtx) {
                        res_header_t *prev     = NULL;
                        int           max_deep = proc->res_list_size + 1;

                        foreach_resource(curr, proc->res_list) {
                                if (curr == resource) {
                                        if (curr->type == type) {
                                                if (proc->res_list == curr) {
                                                        proc->res_list = proc->res_list->next;
                                                } else {
                                                        prev->next = curr->next;
                                                }

                                                obj_to_destroy = curr;
                                                proc->res_list_size--;
                                        } else {
                                                err = EFAULT;
                                        }

                                        break;
                                } else {

                                        prev = curr;

                                        if (--max_deep == 0) {
                                                _assert(max_deep > 0);
                                                _kernel_panic_report(_KERNEL_PANIC_DESC_CAUSE_INTERNAL_2);
                                        }
                                }
                        }
                }

                if (obj_to_destroy) {
                        err = resource_destroy(obj_to_destroy);
                        if (err) {
                                _kernel_panic_report(_KERNEL_PANIC_DESC_CAUSE_INTERNAL_3);
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function create a new thread for selected process.
 *
 * @param[in ]  proc         process container
 * @param[in ]  func         thread function
 * @param[in ]  attr         thread attributes
 * @param[in ]  arg          thread argument
 * @param[out]  tid          thread ID
 *
 * @return One of errno value.
 */
//==============================================================================
int _process_thread_create(_process_t           *proc,
                           thread_func_t         func,
                           const _thread_attr_t *attr,
                           void                 *arg,
                           tid_t                *tid)
{
        int err = EINVAL;

        if (!is_proc_valid(proc) || !proc->taskdata || !func) {
                return err;
        }

        ATOMIC(process_mtx) {
                u8_t  threads = PROC_MAX_THREADS(proc);
                tid_t id      = 1;
                for (; id < threads; id++) {
                        if (proc->taskdata[id].task == NULL) {
                                memset(&proc->taskdata[id], 0, sizeof(proc->taskdata[id]));
                                break;
                        }
                }

                if (id < threads) {
                        task_args_t *args = NULL;
                        err = _kmalloc(_MM_KRN, sizeof(*args), _CPUCTL_FAST_MEM, 0, 0, cast(void*, &args));
                        if (!err) {

                                args->func = func;
                                args->arg  = arg;
                                args->proc = proc;
                                args->task_data = &proc->taskdata[id];

                                proc->taskdata[id].stack_size  = (attr ? attr->stack_depth : 0) + _STACK_DEPTH_MINIMAL;
                                proc->taskdata[id].kernelspace = (proc->flag & FLAG_KWORKER);
                                proc->taskdata[id].id = id;
                                proc->taskdata[id].curr_syscall = NO_SYSCALL;

                                if (proc->event) {
                                        _flag_clear(proc->event, _PROCESS_EXIT_FLAG(id)
                                                               | _PROCESS_SYSCALL_FLAG(id));
                                }

                                err = _task_create(thread_code, "",
                                                   proc->taskdata[id].stack_size,
                                                   args, &proc->taskdata[id].task);
                                if (!err) {

                                        if (attr) {
                                                _task_set_priority(proc->taskdata[id].task,
                                                                   attr->priority);
                                        }

                                        if (tid) {
                                                *tid = id;
                                        }

                                } else {
                                        _kfree(_MM_KRN, cast(void*, &args));
                                }
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function kill selected thread.
 *
 * @param  proc         process
 * @param  tid          thread ID
 *
 * @return One of errno value.
 */
//==============================================================================
int _process_thread_kill(_process_t *proc, tid_t tid)
{
        int err = EINVAL;

        if (is_proc_valid(proc) && (tid > 0) && is_tid_in_range(proc, tid)) {
                ATOMIC(process_mtx) {
                        if (proc->taskdata[tid].task) {
                                _task_destroy(proc->taskdata[tid].task);
                                proc->taskdata[tid].task = NULL;
                        }

                        if (proc->event) {
                                _flag_set(proc->event, _PROCESS_EXIT_FLAG(tid));
                        }

                        err = ESUCC;
                }
        }

        return err;
}

//==============================================================================
/**
 * Function return task of selected thread (low level function).
 *
 * @param proc          process
 * @param tid           thread ID
 *
 * @return Task handle.
 */
//==============================================================================
task_t *_process_thread_get_task(_process_t *proc, tid_t tid)
{
        task_t *task = NULL;

        if (is_proc_valid(proc) && is_tid_in_range(proc, tid)) {
                ATOMIC(process_mtx) {
                        task = proc->taskdata[tid].task;
                }
        }

        return task;
}

//==============================================================================
/**
 * Function return thread statistics.
 *
 * @param  pid          process ID
 * @param  tid          thread ID
 * @param  stat         statistics
 *
 * @return One of errno value.
 */
//==============================================================================
int _process_thread_get_stat(pid_t pid, tid_t tid, _thread_stat_t *stat)
{
        int err = EINVAL;

        if (pid && stat) {
                err = ENOENT;

                ATOMIC(process_mtx) {
                        _process_t *proc = NULL;

                        foreach_process(p, active_process_list) {
                                if (p->pid == pid) {
                                        proc = p;
                                        goto finish;
                                }
                        }

                        foreach_process(p, destroy_process_list) {
                                if (p->pid == pid) {
                                        proc = p;
                                        goto finish;
                                }
                        }

                        foreach_process(p, zombie_process_list) {
                                if (p->pid == pid) {
                                        proc = p;
                                        goto finish;
                                }
                        }

                        finish:
                        if (  proc && (is_tid_in_range(proc, tid))
                           && proc->taskdata[tid].task) {

                                memset(stat, 0, sizeof(_thread_stat_t));

                                stat->tid              = tid;
                                stat->CPU_load         = proc->taskdata[tid].CPU_load;
                                stat->priority         = _task_get_priority(proc->taskdata[tid].task);
                                stat->stack_size       = proc->taskdata[tid].stack_size + _STACK_DEPTH_MINIMAL;
                                stat->stack_max_usage  = stat->stack_size - _task_get_free_stack(proc->taskdata[tid].task);
                                stat->syscalls_per_sec = proc->taskdata[tid].syscalls_per_sec;

                                err = ESUCC;
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * Function exit from calling thread.
 *
 * @param  exit         exit code
 */
//==============================================================================
void _process_thread_exit(int exit)
{
        void _kill_thread_helper(void *arg)
        {
                _process_t *proc; tid_t tid;
                _task_get_process_container(arg, &proc, &tid);
                _process_thread_kill(proc, tid);
                _task_exit();
        }

        _process_t *proc; tid_t tid;
        _task_get_process_container(NULL, &proc, &tid);

        if (is_proc_valid(proc) and is_tid_in_range(proc, tid)) {

                if (tid == 0) {
                        _process_exit(proc, exit);

                } else {
                        proc->taskdata[tid].status = exit;

                        while (true) {
                                int err = _task_create(_kill_thread_helper, "", _STACK_DEPTH_MINIMAL,
                                                       _task_get_handle(), NULL);
                                while (!err) {
                                        _sleep_ms(10);
                                }

                                _sleep_ms(100);
                        }
                }
        }
}

//==============================================================================
/**
 * @brief  Function return thread exit status.
 *
 * @param  proc         process
 * @param  tid          thread id
 * @param  status       destiantion status pointer
 *
 * @return On success 0 is returned, otherwise -1.
 */
//==============================================================================
int _process_thread_get_status(_process_t *proc, tid_t tid, int *status)
{
        if (is_proc_valid(proc) and is_tid_in_range(proc, tid) and status) {
                *status = proc->taskdata[tid].status;
                return 0;
        } else {
                return -1;
        }
}

//==============================================================================
/**
 * Function increase syscall request counter (stats).
 *
 * @param proc          process
 * @param kworker       kworker process
 */
//==============================================================================
void _process_syscall_stat_inc(_process_t *proc, _process_t *kworker)
{
        if (is_proc_valid(proc) and is_proc_valid(kworker)) {
                ATOMIC(process_mtx) {
                        tid_t i = _process_get_active_thread(proc);
                        proc->taskdata[i].syscalls_ctr++;
                        kworker->taskdata[0].syscalls_ctr++;
                }
        }
}

//==============================================================================
/**
 * @brief  Function return process container and thread ID associated with task.
 *
 * @param taskhdl       task (NULL for this task)
 * @param proc          process object (can be NULL)
 * @param tid           thread ID (can be NULL)
 *
 * @return On success process container pointer is returned, otherwise NULL.
 */
//==============================================================================
void _task_get_process_container(task_t *taskhdl, _process_t **proc, tid_t *tid)
{
        ATOMIC(process_mtx) {
                taskhdl = taskhdl ? taskhdl : _task_get_handle();
                _assert(taskhdl);

                if (proc) {
                        _process_t *p = _task_get_storage_pointer(taskhdl, STORAGE_IDX_PROC);
                        _assert(p);
                        *proc = p;
                }

                if (tid) {
                        task_data_t *task_data = _task_get_storage_pointer(taskhdl, STORAGE_IDX_TASK_DATA);
                        _assert(task_data);
                        *tid = task_data->id;
                }
        }
}

//==============================================================================
/**
 * @brief Function calculate general CPU load.
 *        Calculate are average values for 1/60, 1, 5, and 15 minutes.
 */
//==============================================================================
void _calculate_CPU_load(void)
{
        // calculates 1 second CPU load of all processes
        avg_CPU_load_calc.avg1sec = 0;

        // atomic section is not needed because this function is called from IRQ
        foreach_process(proc, active_process_list) {

                for (int i = 0; i < PROC_MAX_THREADS(proc); i++) {
                        if (proc->taskdata[i].task) {
                                proc->taskdata[i].CPU_load = proc->taskdata[i].timecnt / (_CPU_total_time / 1000);
                                proc->taskdata[i].timecnt  = 0;
                                avg_CPU_load_calc.avg1sec  += proc->taskdata[i].CPU_load;

                                proc->taskdata[i].syscalls_per_sec = proc->taskdata[i].syscalls_ctr;
                                proc->taskdata[i].syscalls_ctr = 0;
                        }
                }
        }

        avg_CPU_load_result.avg1sec = avg_CPU_load_calc.avg1sec;

        _CPU_total_time     = 0;
        CPU_total_time_last = 0;

        // calculates average CPU load
        avg_CPU_load_calc.avg1min  += avg_CPU_load_result.avg1sec;
        avg_CPU_load_calc.avg5min  += avg_CPU_load_result.avg1sec;
        avg_CPU_load_calc.avg15min += avg_CPU_load_result.avg1sec;

        if (_uptime_counter_sec % 60 == 0) {
                avg_CPU_load_result.avg1min = avg_CPU_load_calc.avg1min / 60;
                avg_CPU_load_calc.avg1min   = 0;
        }

        if (_uptime_counter_sec % 300 == 0) {
                avg_CPU_load_result.avg5min = avg_CPU_load_calc.avg5min / 300;
                avg_CPU_load_calc.avg5min   = 0;
        }

        if (_uptime_counter_sec % 900 == 0) {
                avg_CPU_load_result.avg15min = avg_CPU_load_calc.avg15min / 900;
                avg_CPU_load_calc.avg15min   = 0;
        }
}

//==============================================================================
/**
 * @brief Function return calculated average CPU load.
 *
 * @param avg   average measurements
 *
 * @return On success 0 is returned, otherwise -1.
 */
//==============================================================================
int _get_average_CPU_load(_avg_CPU_load_t *avg)
{
        if (avg) {
                *avg = avg_CPU_load_result;
                return 0;
        } else {
                return -1;
        }
}

//==============================================================================
/**
 * @brief  Function check if all processes are consistent.
 *
 * @return Return true if all is consistent, otherwise false.
 */
//==============================================================================
bool _process_is_consistent(void)
{
        bool sanity_ok = true;

        _kernel_scheduler_lock();
        {
                sanity_ok = (PID_cnt >= PID_MIN) && (PID_cnt <= PID_MAX);
                if (!sanity_ok) goto end;

                res_header_t *f = (void*)process_mtx;
                sanity_ok = (f->self == f) && (f->type == RES_TYPE_MUTEX);
                if (!sanity_ok) goto end;

                f = (void*)kworker_mtx;
                sanity_ok = (f->self == f) && (f->type == RES_TYPE_MUTEX);
                if (!sanity_ok) goto end;

                sanity_ok = _mm_is_object_in_heap(active_process_list);
                if (!sanity_ok) goto end;

                sanity_ok = destroy_process_list ? _mm_is_object_in_heap(destroy_process_list) : true;
                if (!sanity_ok) goto end;

                sanity_ok = zombie_process_list ? _mm_is_object_in_heap(zombie_process_list) : true;
                if (!sanity_ok) goto end;

                sanity_ok = active_process ? _mm_is_object_in_heap(active_process) : true;
                if (!sanity_ok) goto end;

                foreach_process(p, active_process_list) {

                        sanity_ok = is_proc_valid(p);
                        if (!sanity_ok) goto end;

                        sanity_ok = (p->pdata != NULL);
                        if (!sanity_ok) goto end;

                        bool found = (p->pdata->main == _syscall_kworker_process);
                        const _program_table_desc_t *const program_table = _get_programs_table();
                        for (size_t i = 0; (i < program_table->number_of_programs) && !found; i++) {
                                found = (p->pdata == &program_table->program_entry[i]);
                        }

                        sanity_ok = (strnlen(p->pdata->name, 256) < 256);
                        if (!sanity_ok) goto end;

                        sanity_ok = (p->pdata->stack_depth != 0);
                        if (!sanity_ok) goto end;

                        sanity_ok = _mm_is_rom_address(p->pdata->main);
                        if (!sanity_ok) goto end;

                        res_header_t *flag = (void*)p->event;
                        sanity_ok = (p->pdata->main != _syscall_kworker_process) ?
                                    ( _mm_is_object_in_heap(flag)
                                    && (flag->self == flag)
                                    && (flag->type == RES_TYPE_FLAG)) : true;
                        if (!sanity_ok) goto end;

                        f = (void*)p->f_stdin;
                        sanity_ok = (f != NULL) ? (_mm_is_object_in_heap(f)
                                                  && (f->self == f)
                                                  && (f->type == RES_TYPE_FILE))
                                                : true;
                        if (!sanity_ok) goto end;

                        f = (void*)p->f_stdout;
                        sanity_ok = (f != NULL) ? (_mm_is_object_in_heap(f)
                                                  && (f->self == f)
                                                  && (f->type == RES_TYPE_FILE))
                                                : true;
                        if (!sanity_ok) goto end;

                        f = (void*)p->f_stderr;
                        sanity_ok = (f != NULL) ? (_mm_is_object_in_heap(f)
                                                  && (f->self == f)
                                                  && (f->type == RES_TYPE_FILE))
                                                : true;
                        if (!sanity_ok) goto end;

                        sanity_ok = (p->pdata->globals_size && (*p->pdata->globals_size > 0))
                                    ? _mm_is_object_in_heap(p->globals) : true;
                        if (!sanity_ok) goto end;

                        size_t n = 0;
                        size_t m = p->res_list_size;
                        res_header_t *res = p->res_list;
                        while (res && m) {

                                sanity_ok =  _mm_is_object_in_heap(res)
                                          && (res->self == res)
                                          && (  (res->type == RES_TYPE_MUTEX)
                                             || (res->type == RES_TYPE_SEMAPHORE)
                                             || (res->type == RES_TYPE_QUEUE)
                                             || (res->type == RES_TYPE_DIR)
                                             || (res->type == RES_TYPE_MEMORY)
                                             || (res->type == RES_TYPE_SOCKET)
                                             || (res->type == RES_TYPE_FLAG)
                                             || (res->type == RES_TYPE_FILE)
                                             || (res->type == RES_TYPE_FILE)
                                             || (res->type == RES_TYPE_FILE) );
                                if (!sanity_ok) goto end;

                                res = res->next;
                                n++;
                                m--;
                        }

                        sanity_ok = (n == p->res_list_size) && (m == 0);
                        if (!sanity_ok) goto end;

                        sanity_ok = (p->cwd != NULL);
                        if (!sanity_ok) goto end;

                        sanity_ok = _mm_is_object_in_heap(p->argv) && (p->argc >= 1);
                        if (!sanity_ok) goto end;

                        sanity_ok = (p->pid >= PID_MIN) && (p->pid <= PID_MAX);
                        if (!sanity_ok) goto end;

                        sanity_ok = (p->pdata->main == _syscall_kworker_process)
                                  ? ((p->flag & FLAG_KWORKER) == FLAG_KWORKER) : true;
                        if (!sanity_ok) goto end;

                        sanity_ok = (p->pdata->main != _syscall_kworker_process)
                                  ? ((p->flag & ~FLAG_DETACHED) == 0) : true;
                        if (!sanity_ok) goto end;

                        for (int i = 0; i < PROC_MAX_THREADS(p); i++) {
                                sanity_ok = (p->taskdata[i].task)
                                          ? _mm_is_object_in_heap(p->taskdata[i].task) : true;
                                if (!sanity_ok) goto end;

                                sanity_ok = (p->taskdata[i].task) ? (p->taskdata[i].stack_size > 0) : true;
                                if (!sanity_ok) goto end;
                        }
                }
        }

        end:
        _kernel_scheduler_unlock();

        return sanity_ok;
}

//==============================================================================
/**
 * @brief  Function mark thread that enter kernel space.
 *
 * @param  proc         process
 */
//==============================================================================
void _process_enter_kernelspace(_process_t *proc, u8_t syscall)
{
        if (is_proc_valid(proc)) {
                ATOMIC(process_mtx) {
                        tid_t i = _process_get_active_thread(proc);
                        proc->taskdata[i].kernelspace = true;
                        proc->taskdata[i].curr_syscall = syscall;
                }
        }
}

//==============================================================================
/**
 * @brief  Function mark thread that exit kernel space.
 *
 * @param  proc         process
 */
//==============================================================================
void _process_exit_kernelspace(_process_t *proc)
{
        if (is_proc_valid(proc)) {
                ATOMIC(process_mtx) {
                        tid_t i = _process_get_active_thread(proc);
                        proc->taskdata[i].kernelspace = false;
                        proc->taskdata[i].curr_syscall = 0xFF;
                }
        }
}

//==============================================================================
/**
 * @brief  Function get kernelspace indicator.
 *
 * @param  proc         process
 * @param  tid          thread ID
 *
 * @return Kernelspace indicator.
 */
//==============================================================================
bool _process_is_kernelspace(_process_t *proc, tid_t tid)
{
        bool kernelspace = false;

        if (is_proc_valid(proc) && is_tid_in_range(proc, tid)) {
                ATOMIC(process_mtx) {
                        kernelspace = proc->taskdata[tid].kernelspace;
                }
        }

        return kernelspace;
}

//==============================================================================
/**
 * @brief  Function get current syscall.
 *
 * @param  proc         process
 *
 * @return Current syscall.
 */
//==============================================================================
u8_t _process_get_curr_syscall(_process_t *proc, tid_t tid)
{
        bool syscall = NO_SYSCALL;

        if (is_proc_valid(proc) && is_tid_in_range(proc, tid)) {
                ATOMIC(process_mtx) {
                        syscall = proc->taskdata[tid].curr_syscall;
                }
        }

        return syscall;
}

//==============================================================================
/**
 * @brief  This function start user process code [USERLAND].
 *
 * @param  mainfn         process argument - process main function
 */
//==============================================================================
static void process_code(void *arg)
{
        _assert(arg);

        task_args_t args = *cast(task_args_t*, arg);
        _kfree(_MM_KRN, &arg);

        _task_set_storage_pointer(NULL, STORAGE_IDX_PROC, args.proc);
        _task_set_storage_pointer(NULL, STORAGE_IDX_TASK_DATA, args.task_data);
        _task_yield();

        process_func_t func = args.func;
        args.task_data->status = func(args.proc->argc, args.proc->argv);

        _process_exit(args.proc, args.task_data->status);
}

//==============================================================================
/**
 * @brief  This function start user thread code [USERLAND].
 *
 * @param  arg          thread arguments - main function and argument value.
 */
//==============================================================================
static void thread_code(void *arg)
{
        _assert(arg);

        task_args_t args = *cast(task_args_t*, arg);
        _kfree(_MM_KRN, cast(void*, &arg));

        _task_set_storage_pointer(NULL, STORAGE_IDX_PROC, args.proc);
        _task_set_storage_pointer(NULL, STORAGE_IDX_TASK_DATA, args.task_data);
        _task_yield();

        thread_func_t func = args.func;
        args.task_data->status = func(args.arg);

        ATOMIC(process_mtx) {
                if (args.proc->event) {
                        _flag_set(args.proc->event, _PROCESS_EXIT_FLAG(args.task_data->id));
                }

                args.task_data->task = NULL;
        }

        _task_exit();
}

//==============================================================================
/**
 * Function move process from selected list to another.
 *
 * @param  proc         process to move
 * @param  list_from    source list
 * @param  list_to      destination list
 */
//==============================================================================
static void process_move_list(_process_t *proc, _process_t **list_from, _process_t **list_to)
{
        ATOMIC(process_mtx) {
                _process_t *prev = NULL;
                foreach_process(p, *list_from) {
                        if (p == proc) {

                                if (prev) {
                                        prev->header.next = proc->header.next;
                                } else {
                                        *list_from = cast(_process_t*, proc->header.next);
                                }

                                proc->header.next = cast(struct res_header*, *list_to);

                                *list_to = proc;

                                break;
                        } else {
                                prev = p;
                        }
                }
        }
}

//==============================================================================
/**
 * @brief  Function destroy (release) all process resources. Function does not
 *         destroy process object.
 *
 * @param  proc     selected process container
 */
//==============================================================================
static void process_destroy_all_resources(_process_t *proc)
{
        destroy_all_tasks(proc, true);

        if (proc->argv) {
                argtab_destroy(proc->argv);
                proc->argv = NULL;
                proc->argc = 0;
        }

        // close files, directories, sockets, etc
        res_header_t *resource_curr = proc->res_list;
        res_header_t *resource_prev = NULL;
        res_header_t *resource_next = NULL;

        while (resource_curr) {
                resource_next = resource_curr->next;

                if (resource_curr->type != RES_TYPE_MEMORY) {

                        if (resource_curr == proc->res_list) {
                                proc->res_list = resource_next;
                        } else {
                                if (resource_prev) {
                                        resource_prev->next = resource_next;
                                }
                        }

                        int err = resource_destroy(resource_curr);
                        if (err != ESUCC) {
                                printk("PROCESS: PID %d: unknown object %p\n",
                                       proc->pid, resource_curr);
                        }
                } else {
                        resource_prev = resource_curr;
                }

                resource_curr = resource_next;
        }

        // free all other resources
        while (proc->res_list) {
                res_header_t *resource = proc->res_list;
                proc->res_list = resource->next;

                int err = resource_destroy(resource);
                if (err != ESUCC) {
                        printk("PROCESS: PID %d: unknown object %p\n", proc->pid, resource);
                }
        }

        if (proc->cwd) {
                _kfree(_MM_KRN, cast(void*, &proc->cwd));
        }

        // detach from all shared memory regions
#if __OS_ENABLE_SHARED_MEMORY__ > 0
        _shm_detach_anywhere(proc->pid);
#endif

        proc->res_list_size = 0;
        proc->res_list = NULL;
        proc->f_stdin  = NULL;
        proc->f_stdout = NULL;
        proc->f_stderr = NULL;
        proc->globals  = NULL;
}

//==============================================================================
/**
 * @brief  Function gets process statistics.
 *
 * @param  proc         process
 * @param  stat         statistics container
 */
//==============================================================================
static void process_get_stat(_process_t *proc, _process_stat_t *stat)
{
        memset(stat, 0, sizeof(_process_stat_t));

        stat->name     = proc->pdata->name;
        stat->pid      = proc->pid;
        stat->priority = (proc->taskdata[0].task) ? _task_get_priority(proc->taskdata[0].task) : 0;

        u8_t threads = PROC_MAX_THREADS(proc);
        for (tid_t tid = 0; tid < threads; tid++) {
                if (proc->taskdata[tid].task) {
                        stat->threads_count++;
                        stat->CPU_load         += proc->taskdata[tid].CPU_load;
                        stat->syscalls_per_sec += proc->taskdata[tid].syscalls_per_sec;
                        stat->stack_max_usage  += (proc->taskdata[tid].stack_size - _task_get_free_stack(proc->taskdata[tid].task));
                        stat->stack_size       += proc->taskdata[tid].stack_size;
                }
        }

        foreach_resource(res, proc->res_list) {
                switch (res->type) {
                case RES_TYPE_FILE:
                        stat->files_count++;
                        break;

                case RES_TYPE_DIR:
                        stat->dir_count++;
                        break;

                case RES_TYPE_MUTEX:
                        stat->mutexes_count++;
                        break;

                case RES_TYPE_QUEUE:
                        stat->queue_count++;
                        break;

                case RES_TYPE_FLAG:
                case RES_TYPE_SEMAPHORE:
                        stat->semaphores_count++;
                        break;

                case RES_TYPE_MEMORY:
                        stat->memory_block_count++;
                        stat->memory_usage += _mm_get_block_size(res);
                        break;

                case RES_TYPE_SOCKET:
                        stat->socket_count++;
                        break;

                default:
                        break;
                }
        }

        stat->memory_usage = _mm_align(stat->memory_usage);
}

//==============================================================================
/**
 * @brief  Function apply process attributes.
 *
 * @param  proc     process
 * @param  atrr     attributes
 *
 * @return One of errno value.
 */
//==============================================================================
static int process_apply_attributes(_process_t *proc, const _process_attr_t *attr)
{
        int err = ESUCC;

        if (attr) {
                /*
                 * Apply Current Working Directory path
                 */
                size_t cwdlen = attr->cwd ? strsize(attr->cwd) : strsize("/");

                err = _kmalloc(_MM_KRN, cwdlen, NULL, 0, 0, cast(void*, &proc->cwd));
                if (!err) {
                        if (attr->cwd && attr->cwd[0] == '/') {
                                strcpy(proc->cwd, attr->cwd);
                        } else {
                                strcpy(proc->cwd, "/");
                        }

                        _vfs_realpath(proc->cwd, SUB_SLASH);
                } else {
                        goto finish;
                }

                /*
                 * Apply stdin settings
                 * - if f_stdin is set then function use this resource as reference
                 * - if f_stdin is NULL and p_stdin is NULL then function set stdin as NULL
                 * - if f_stdin is NULL and p_stdin is valid then function open new file and use as stdin
                 */
                if (attr->f_stdin) {
                        proc->f_stdin = attr->f_stdin;

                } else if (attr->p_stdin) {
                        struct vfs_path cpath;
                        cpath.CWD  = proc->cwd;
                        cpath.PATH = attr->p_stdin;

                        err = _vfs_fopen(&cpath, "a+", &proc->f_stdin);
                        if (!err) {
                                _process_register_resource(proc, cast(res_header_t*, proc->f_stdin));
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
                                struct vfs_path cpath;
                                cpath.CWD  = proc->cwd;
                                cpath.PATH = attr->p_stdout;

                                err = _vfs_fopen(&cpath, "a", &proc->f_stdout);
                                if (!err) {
                                        _process_register_resource(proc, cast(res_header_t*, proc->f_stdout));
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
                                struct vfs_path cpath;
                                cpath.CWD  = proc->cwd;
                                cpath.PATH = attr->p_stderr;

                                err = _vfs_fopen(&cpath, "a", &proc->f_stderr);
                                if (!err) {
                                        _process_register_resource(proc, cast(res_header_t*, proc->f_stderr));
                                } else {
                                        goto finish;
                                }
                        }
                }

                /*
                 * Set detached flag
                 */
                proc->flag |= attr->detached ? FLAG_DETACHED : 0;
        }

        finish:
        return err;
}

//==============================================================================
/**
 * @brief  Function destroy all process tasks.
 *
 * @param  proc         process container
 * @param  with_zero    destroy all tasks with task number zero (main process task)
 */
//==============================================================================
static void destroy_all_tasks(_process_t *proc, bool with_zero)
{
        u8_t threads = PROC_MAX_THREADS(proc);

        for (tid_t tid = with_zero ? 0 : 1; tid < threads; tid++) {
                if (proc->taskdata[tid].task) {
                        _task_destroy(proc->taskdata[tid].task);
                        proc->taskdata[tid].task = NULL;
                }
        }
}

//==============================================================================
/**
 * @brief Function create PID number.
 *
 * @return On success PID number is returned, otherwise 0.
 */
//==============================================================================
static int get_pid(pid_t *pid)
{
        int err = ESRCH;

        ATOMIC(process_mtx) {
                int n = PID_MAX + 1;
                while (pid && (n-- > 0)) {
                        if (++PID_cnt >= (PID_MAX + 1)) {
                                PID_cnt = PID_MIN;
                        }

                        bool found = false;

                        foreach_process(proc, active_process_list) {
                                found = proc->pid == PID_cnt;
                                if (found) break;
                        }

                        if (found) continue;

                        foreach_process(proc, active_process_list) {
                                found = proc->pid == PID_cnt;
                                if (found) break;
                        }

                        if (found) continue;

                        foreach_process(proc, active_process_list) {
                                found = proc->pid == PID_cnt;
                                if (found) break;
                        }

                        if (found) {
                                continue;
                        } else {
                                *pid = PID_cnt;
                                err  = ESUCC;
                                break;
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function destroy (release) selected resource.
 *
 * @param  resource     resource to release
 *
 * @return One of errno value.
 */
//==============================================================================
static int resource_destroy(res_header_t *resource)
{
        if (!_mm_is_object_in_heap(resource)) {
                printk("Resource object %p out of heap", resource);
                return EINVAL;
        }

        res_header_t *res2free = resource;

        switch (resource->type) {
        case RES_TYPE_FILE:
                _vfs_fclose(cast(kfile_t*, res2free), true);
                break;

        case RES_TYPE_DIR:
                _vfs_closedir(cast(kdir_t*, res2free));
                break;

        case RES_TYPE_MEMORY:
                _kfree(_MM_PROG, cast(void*, &res2free));
                break;

        case RES_TYPE_MUTEX:
                _mutex_destroy(cast(kmtx_t*, res2free));
                break;

        case RES_TYPE_QUEUE:
                _queue_destroy(cast(kqueue_t*, res2free));
                break;

        case RES_TYPE_SEMAPHORE:
                _semaphore_destroy(cast(ksem_t*, res2free));
                break;

        case RES_TYPE_FLAG:
                _flag_destroy(cast(kflag_t*, res2free));
                break;

        case RES_TYPE_SOCKET:
#if _ENABLE_NETWORK_ == _YES_
                _net_socket_destroy(cast(SOCKET*, res2free));
                break;
#else
                return ENOTSUP;
#endif

        default:
                return EINVAL;
        }

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function check if first command argument is a path.
 *
 * @param  cmd          command to check
 *
 * @return True if command is in path form, otherwise false.
 */
//==============================================================================
#if __OS_SYSTEM_SHEBANG_ENABLE__ > 0
static bool is_cmd_path(const char *cmd)
{
        cmd += strspn(cmd, " ");
        return (  (cmd[0] == '.' && cmd[1] == '/')
               || (cmd[0] == '.' && cmd[1] == '.' && cmd[2] == '/')
               || (cmd[0] == '/') );
}
#endif

//==============================================================================
/**
 * @brief  Function check if argument is a file with shebang (#!).
 *
 * @param  proc         process
 * @param  cmd          RAW command
 * @param  cmdarg       commands after shebang analyze
 *
 * @return One of errno value.
 */
//==============================================================================
#if __OS_SYSTEM_SHEBANG_ENABLE__ > 0
static int analyze_shebang(_process_t *proc, const char *cmd, char **cmdarg)
{
        // allocations
        char *filename = NULL;
        char *line     = NULL;
        kfile_t *file     = NULL;

        // skip leading spaces
        cmd += strspn(cmd, " ");

        // argument start
        char  *args    = strchr(cmd, ' ');
               args   += args ? 1 : 0;
        size_t argslen = args ? strsize(args) : 0;

        // file name length
        size_t fnlen = args ? ((u32_t)args - (u32_t)cmd) : strsize(cmd);

        // allocate file name
        int err = _kmalloc(_MM_KRN, fnlen, NULL, 0, 0, cast(void**, &filename));
        if (err) goto finish;
        sys_strlcpy(filename, cmd, fnlen);

        // allocate line
        err = _kzalloc(_MM_KRN, SHEBANGLEN, NULL, 0, 0, cast(void**, &line));
        if (err) goto finish;

        // open file
        struct vfs_path path;
        path.CWD  = proc->cwd;
        path.PATH = filename;
        err       = _vfs_fopen(&path, "r", &file);
        if (err) goto finish;

        // check that file is set to execute
        struct stat st;
        st.st_mode = 0;
        err = _vfs_fstat(file, &st);
        if (err) goto finish;
        if ((st.st_mode & S_IXUSR) == 0) {
                err = EACCES;
                goto finish;
        }

        // read line
        size_t rd = 0;
        err = _vfs_fread(line, SHEBANGLEN - 1, &rd, file);
        if (err) goto finish;

        // check shebang
        if (line[0] == '#' && line[1] == '!') {
                char *p  = line + 2;
                      p += strspn(p, " ");

                char *end = strchr(p, '\r');
                end = end ? end : strchr(p, '\n');

                if (end) {
                        *end = '\0';
                }

                size_t arglen = strsize(p) + strsize(filename) + argslen;

                err = _kmalloc(_MM_KRN, arglen, NULL, 0, 0, cast(void**, cmdarg));
                if (!err) {
                        sys_strlcpy(*cmdarg, p, arglen);
                        sys_strlcat(*cmdarg, " ", arglen);
                        sys_strlcat(*cmdarg, filename, arglen);

                        if (args) {
                                sys_strlcat(*cmdarg, " ", arglen);
                                sys_strlcat(*cmdarg, args, arglen);
                        }
                }
        }

        finish:
        if (filename) {
                _kfree(_MM_KRN, cast(void**, &filename));
        }

        if (line) {
                _kfree(_MM_KRN, cast(void**, &line));
        }

        if (file) {
                _vfs_fclose(file, true);
        }

        return err;
}
#endif

//==============================================================================
/**
 * @brief Function create new table with argument pointers.
 *
 * @param[in]  str              argument string
 * @param[out] argc             number of argument
 * @param[out] argv             pointer to pointer of argument array
 *
 * @return One of errno value.
 */
//==============================================================================
static int argtab_create(const char *str, u8_t *argc, char **argv[])
{
        int err = EINVAL;

        if (!isstrempty(str) && argc && argv) {

                llist_t *largs;
                err = _llist_create_krn(_MM_KRN, NULL, NULL, &largs);

                if (!err) {
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
                                err = _kmalloc(_MM_KRN, str_len + 1, NULL, 0, 0, cast(void**, &arg));
                                if (!err) {
                                        _strlcpy(arg, start, str_len + 1);

                                        if (_llist_push_back(largs, arg) == NULL) {
                                                _kfree(_MM_KRN, cast(void*, &arg));
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
                        err = _kmalloc(_MM_KRN, (no_of_args + 1) * sizeof(char*), NULL, 0, 0, cast(void*, &arg));
                        if (!err) {
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

        return err;
}

//==============================================================================
/**
 * @brief  Function remove argument table.
 *
 * @param  argv          pointer to argument table (must be ended with NULL)
 */
//==============================================================================
static void argtab_destroy(char **argv)
{
        if (argv) {
                int n = 0;
                while (argv[n]) {
                        _kfree(_MM_KRN, cast(void*, &argv[n]));
                        n++;
                }

                _kfree(_MM_KRN, cast(void*, &argv));
        }
}

//==============================================================================
/**
 * @brief  Return program table pointer.
 *
 * @return Program table address.
 */
//==============================================================================
const _program_table_desc_t *_get_programs_table(void)
{
        // standalone version
//        extern void *__text_end;
//        const _program_table_desc_t *const prog_table = ((void *)&__text_end);

        // monolihtic version
        extern const _program_table_desc_t _program_table_desc;
        const _program_table_desc_t *const prog_table = &_program_table_desc;

        return prog_table;
}

//==============================================================================
/**
 * @brief Function find program by name and return program descriptor container.
 *
 * @param name         program name
 * @param prog         program container
 *
 * @return One of errno value.
 */
//==============================================================================
static int find_program(const char *name, const _program_entry_t **prog)
{
        static const size_t kworker_stack_depth  = 0;
        static const size_t kworker_globals_size = 0;
        static const _program_entry_t kworker   = {.globals_size = &kworker_globals_size,
                                                    .main         = _syscall_kworker_process,
                                                    .name         = "kworker",
                                                    .stack_depth  = &kworker_stack_depth};

        int err = ENOENT;

        if (strncmp(name, "kworker", 32) == 0) {
                *prog = &kworker;
                err   = ESUCC;

        } else {
                const _program_table_desc_t *const prog_table = _get_programs_table();

                if ((prog_table->magic == 0) and (prog_table->number_of_programs > 0)
                    and (prog_table->number_of_programs != UINT32_MAX)) {

                        for (size_t i = 0; i < prog_table->number_of_programs; i++) {
                                if (strncmp(prog_table->program_entry[i].name, name, 128) == 0) {
                                        *prog  = &prog_table->program_entry[i];
                                        err = ESUCC;
                                        break;
                                }
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function allocate global variables for selected process.
 *
 * @param  proc         process
 * @param  usrprog      program descriptor
 *
 * @return One of errno value.
 */
//==============================================================================
static int allocate_process_globals(_process_t *proc, const _program_entry_t *usrprog)
{
        int err = ESUCC;

        if (*usrprog->globals_size > 0) {
                res_header_t *mem;
                err = _kzalloc(_MM_PROG, *usrprog->globals_size, NULL,
                               _MM_FLAG__DMA_CAPABLE, _MM_FLAG__DMA_CAPABLE,
                               cast(void*, &mem));
                if (!err) {
                        proc->globals = &mem[1];
                        _process_register_resource(proc, mem);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function copy task context to standard variables (stdin, stdout, stderr,
 *         global, errno). Function is called when this task is already switched.
 *         See FreeRTOSConfig.h file.
 *
 * @param  task         current task
 * @param  proc         process container
 * @param  task_data    task data
 */
//==============================================================================
void _task_switched_in(task_t *task, void *proc, void *task_data)
{
        UNUSED_ARG1(task);

        CPU_total_time_last = _CPU_total_time;
        active_process = proc;

        if (active_process && (active_process->header.type == RES_TYPE_PROCESS)) {
                active_process->curr_task = cast(task_data_t*, task_data)->id;
                _errno  = cast(task_data_t*, task_data)->errnov;
                _stdin  = active_process->f_stdin;
                _stdout = active_process->f_stdout;
                _stderr = active_process->f_stderr;
                _global = active_process->globals;

        } else {
                _stdin  = NULL;
                _stdout = NULL;
                _stderr = NULL;
                _global = NULL;
                _errno  = 0;
        }
}

//==============================================================================
/**
 * @brief  Function copy standard variables (stdin, stdout, stderr, global, errno)
 *         to task context. Function is called before this task context to be
 *         switched.
 *         See FreeRTOSConfig.h file.
 *
 * @param  task         current task
 * @param  proc         process container
 * @param  task_data    task data
 */
//==============================================================================
void _task_switched_out(task_t *task, void *proc, void *task_data)
{
        UNUSED_ARG2(task, proc);

        if (active_process && (active_process->header.type == RES_TYPE_PROCESS)) {
                cast(task_data_t*, task_data)->errnov = _errno;
                active_process->f_stdin  = _stdin;
                active_process->f_stdout = _stdout;
                active_process->f_stderr = _stderr;
                active_process->globals  = _global;

                #if (__OS_MONITOR_CPU_LOAD__ > 0)
                _CPU_total_time += _cpuctl_get_CPU_load_counter_delta();
                active_process->taskdata[active_process->curr_task].timecnt += (_CPU_total_time - CPU_total_time_last);
                #endif
        } else {
                #if (__OS_MONITOR_CPU_LOAD__ > 0)
                _CPU_total_time += _cpuctl_get_CPU_load_counter_delta();
                #endif
        }
}

/*==============================================================================
  End of file
==============================================================================*/
