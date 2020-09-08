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
#include "dnx/misc.h"
#include "mm/shm.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define USERSPACE
#define KERNELSPACE
#define foreach_resource(_v, _l)        for (res_header_t *_v = _l; _v; _v = _v->next)
#define foreach_process(_v, _l)         for (_process_t *_v = _l; _v; _v = cast(_process_t*, _v->header.next))

#define SHEBANGLEN                      64

// critical section - mutex wait must set to max possible
#define ATOMIC(_mtx) for (int __ = 0; __ == 0;)\
        for (int _e = _mutex_lock(_mtx, MAX_DELAY_MS); _e == 0 && __ == 0; _mutex_unlock(_mtx), __++)

#define PROC_MAX_THREADS(proc)          (((proc)->flag & FLAG_KWORKER) ? __OS_TASK_MAX_SYSTEM_THREADS__ : __OS_TASK_MAX_USER_THREADS__)

#define is_proc_valid(proc)             (_mm_is_object_in_heap(proc) && (proc->header.type == RES_TYPE_PROCESS))
#define is_tid_in_range(proc, tid)      ((tid > 0) && (tid < PROC_MAX_THREADS(proc)))

#define FLAG_DETACHED                   (1 << 0)
#define FLAG_KWORKER                    (1 << 1)

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
typedef struct _prog_data pdata_t;

typedef struct {
        task_t          *task;          //!< task
        u32_t            timecnt;       //!< counter used to calculate CPU load
        u16_t            CPU_load;      //!< CPU load
        u16_t            syscalls;      //!< syscalls/s
        u16_t            syscalls_ctr;  //!< syscall counter
        u16_t            stack_size;    //!< stack size
} task_data_t;

struct _process {
        res_header_t     header;        //!< resource header
        task_data_t     *taskdata;      //!< tasks data
        flag_t          *event;         //!< events for exit indicator and syscall finish
        FILE            *f_stdin;       //!< stdin file
        FILE            *f_stdout;      //!< stdout file
        FILE            *f_stderr;      //!< stderr file
        void            *globals;       //!< address to global variables
        res_header_t    *res_list;      //!< list of used resources
        u32_t            res_list_size; //!< size of resources list
        char            *cwd;           //!< current working path
        const pdata_t   *pdata;         //!< program data
        char            **argv;         //!< program arguments
        u8_t             argc;          //!< number of arguments
        pid_t            pid;           //!< process ID
        int              errnov;        //!< program error number
        i8_t             status;        //!< program status (return value)
        u8_t             flag;          //!< control flags
        u8_t             curr_task;     //!< current working task (thread)
};

typedef struct {
        thread_func_t   func;
        void           *arg;
} thread_args_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void process_code(void *mainfn);
static void thread_code(void *args);
static void process_destroy_all_resources(_process_t *proc);
static int  resource_destroy(res_header_t *resource);
static int  argtab_create(const char *str, u8_t *argc, char **argv[]);
static void argtab_destroy(char **argv);
static int  find_program(const char *name, const struct _prog_data **prog);
static int  allocate_process_globals(_process_t *proc, const struct _prog_data *usrprog);
static int  process_apply_attributes(_process_t *proc, const process_attr_t *attr);
static void process_get_stat(_process_t *proc, process_stat_t *stat);
static void process_move_list(_process_t *proc, _process_t **list_from, _process_t **list_to);
static int  get_pid(pid_t *pid);

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
static avg_CPU_load_t avg_CPU_load_calc;
static avg_CPU_load_t avg_CPU_load_result;
static mutex_t       *process_mtx;
static mutex_t       *kworker_mtx;

/*==============================================================================
  Exported object definitions
==============================================================================*/
/* CPU total time */
u32_t _CPU_total_time = 0;

/* standard input */
FILE *stdin = NULL;

/* standard output */
FILE *stdout = NULL;

/* standard error */
FILE *stderr = NULL;

/* error number */
int _errno = ESUCC;

/* global variables */
struct _GVAR_STRUCT_NAME *global = NULL;

/*==============================================================================
  External object definitions
==============================================================================*/
extern const struct _prog_data _prog_table[];
extern const int               _prog_table_size;
extern u32_t                   _uptime_counter_sec;

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
        if (!process_mtx) {
                _assert(_mutex_create(MUTEX_TYPE_RECURSIVE, &process_mtx) == ESUCC);
        }

        if (!kworker_mtx) {
                _assert(_mutex_create(MUTEX_TYPE_RECURSIVE, &kworker_mtx) == ESUCC);
        }

        if (!cmd) {
                return ENOENT;
        }

        char       *cmdarg = NULL;
        _process_t *proc   = NULL;
        int err = _kzalloc(_MM_KRN, sizeof(_process_t), cast(void**, &proc));
        if (!err) {
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

                err = _kzalloc(_MM_KRN, sizeof(task_data_t) * PROC_MAX_THREADS(proc),
                               cast(void*, &proc->taskdata));
                if (err) goto finish;

                ATOMIC(process_mtx) {
                        err = _task_create(process_code,
                                           proc->pdata->name,
                                           *proc->pdata->stack_depth,
                                           proc->pdata->main,
                                           proc,
                                           &proc->taskdata[0].task);
                        if (!err) {
                                proc->taskdata[0].stack_size = *proc->pdata->stack_depth;

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
                                        proc->header.next = cast(res_header_t*,
                                                                 active_process_list);

                                        active_process_list = proc;
                                }
                        }
                }
        }

        finish:
        if (err) {
                if (cmdarg) {
                        _kfree(_MM_KRN, cast(void**, &cmdarg));
                }

                if (proc) {
                        process_destroy_all_resources(proc);
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
KERNELSPACE void _process_clean_up_killed_processes(void)
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
KERNELSPACE int _process_kill(pid_t pid)
{
        int err = ESRCH;

        ATOMIC(process_mtx) {
                foreach_process(proc, active_process_list) {
                        if (proc->pid == pid) {
                                if (proc->event) {
                                        _flag_set(proc->event, _PROCESS_EXIT_FLAG(0));
                                }

                                u8_t threads = PROC_MAX_THREADS(proc);

                                for (int i = 0; i < threads; i++) {
                                        if (proc->taskdata && proc->taskdata[i].task) {
                                                _task_destroy(proc->taskdata[i].task);
                                                proc->taskdata[i].task = NULL;
                                        }
                                }

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
KERNELSPACE void _process_remove_zombie(_process_t *proc, int *status)
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
                                        *status = proc->status;
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
KERNELSPACE void _process_exit(_process_t *proc, int status)
{
        if (is_proc_valid(proc) && proc->taskdata) {
                proc->status = status;

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
                        if (proc->event) {
                                _flag_set(proc->event, _PROCESS_EXIT_FLAG(0));
                        }

                        u8_t threads = PROC_MAX_THREADS(proc);

                        for (int i = 1; i < threads; i++) {
                                if (proc->taskdata[i].task) {
                                        _task_destroy(proc->taskdata[i].task);
                                        proc->taskdata[i].task = NULL;
                                }
                        }

                        process_move_list(proc, &active_process_list, &destroy_process_list);

                        proc->taskdata[0].task = NULL;
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
KERNELSPACE void _process_abort(_process_t *proc)
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
KERNELSPACE const char *_process_get_CWD(_process_t *proc)
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
KERNELSPACE int _process_set_CWD(_process_t *proc, const char *CWD)
{
        int err = EINVAL;

        if (is_proc_valid(proc) && CWD && CWD[0] == '/') {
                char *cwd;
                err = _kmalloc(_MM_KRN, strsize(CWD), cast(void*, &cwd));
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
KERNELSPACE int _process_get_stat_seek(size_t seek, process_stat_t *stat)
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
KERNELSPACE int _process_get_stat_pid(pid_t pid, process_stat_t *stat)
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
KERNELSPACE FILE *_process_get_stderr(_process_t *proc)
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
KERNELSPACE const char *_process_get_name(_process_t *proc)
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
KERNELSPACE size_t _process_get_count(void)
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
KERNELSPACE _process_t *_process_get_active(void)
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
KERNELSPACE int _process_get_pid(_process_t *proc, pid_t *pid)
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
KERNELSPACE int _process_get_event_flags(_process_t *proc, flag_t **flag)
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
KERNELSPACE int _process_get_priority(pid_t pid, int *prio)
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
KERNELSPACE int _process_get_container(pid_t pid, _process_t **process)
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
KERNELSPACE tid_t _process_get_active_thread(_process_t *process)
{
        tid_t tid = UINT8_MAX;

        _kernel_scheduler_lock();

        if (process == NULL) {
                process = active_process;
        }

        if (process) {
                tid = process->curr_task;
        }

        _assert(tid < PROC_MAX_THREADS(active_process));

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
KERNELSPACE pid_t _process_get_active_process_pid(void)
{
        _kernel_scheduler_lock();

        pid_t pid = 0;

        if (active_process) {
                pid = active_process->pid;
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
KERNELSPACE int _process_register_resource(_process_t *proc, res_header_t *resource)
{
        if (is_proc_valid(proc)) {
                mutex_t *mtx = (proc == _kworker_proc) ? kworker_mtx : process_mtx;

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
KERNELSPACE int _process_release_resource(_process_t *proc, res_header_t *resource, res_type_t type)
{
        int err = ESRCH;

        if (is_proc_valid(proc)) {
                err = ENOENT;
                res_header_t *obj_to_destroy = NULL;
                mutex_t *mtx = (proc == _kworker_proc) ? kworker_mtx : process_mtx;

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
KERNELSPACE int _process_thread_create(_process_t          *proc,
                                       thread_func_t        func,
                                       const thread_attr_t *attr,
                                       void                *arg,
                                       tid_t               *tid)
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
                                break;
                        }
                }

                if (id < threads) {

                        thread_args_t *args = NULL;
                        err = _kmalloc(_MM_KRN, sizeof(thread_args_t), cast(void*, &args));
                        if (!err) {

                                args->func = func;
                                args->arg  = arg;

                                proc->taskdata[id].stack_size = (attr ? attr->stack_depth : STACK_DEPTH_LOW);

                                err = _task_create(thread_code, "",
                                                   proc->taskdata[id].stack_size,
                                                   args, proc, &proc->taskdata[id].task);
                                if (!err) {

                                        if (proc->event) {
                                                _flag_clear(proc->event,
                                                            _PROCESS_EXIT_FLAG(id) |
                                                            _PROCESS_SYSCALL_FLAG(id));
                                        }

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
KERNELSPACE int _process_thread_kill(_process_t *proc, tid_t tid)
{
        int err = EINVAL;

        if (is_proc_valid(proc) && proc->taskdata && is_tid_in_range(proc, tid)) {
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
KERNELSPACE task_t *_process_thread_get_task(_process_t *proc, tid_t tid)
{
        task_t *task = NULL;

        if (is_proc_valid(proc) && proc->taskdata && is_tid_in_range(proc, tid)) {
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
KERNELSPACE int _process_thread_get_stat(pid_t pid, tid_t tid, thread_stat_t *stat)
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
                        if (  proc && (is_tid_in_range(proc, tid) || (tid == 0))
                           && proc->taskdata && proc->taskdata[tid].task) {

                                memset(stat, 0, sizeof(thread_stat_t));

                                stat->tid             = tid;
                                stat->CPU_load        = proc->taskdata[tid].CPU_load;
                                stat->priority        = _task_get_priority(proc->taskdata[tid].task);
                                stat->stack_size      = proc->taskdata[tid].stack_size;
                                stat->stack_max_usage = stat->stack_size - _task_get_free_stack(proc->taskdata[tid].task);
                                stat->syscalls        = proc->taskdata[tid].syscalls;

                                err = ESUCC;
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * Function increase syscall request counter (stats).
 *
 * @param proc          process
 * @param kworker       kworker process
 */
//==============================================================================
KERNELSPACE void _process_syscall_stat_inc(_process_t *proc, _process_t *kworker)
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
 * @param proc          process object
 * @param tid           thread ID
 *
 * @return On success process container pointer is returned, otherwise NULL.
 */
//==============================================================================
KERNELSPACE void _task_get_process_container(task_t *taskhdl, _process_t **proc, tid_t *tid)
{
        ATOMIC(process_mtx) {
                taskhdl = taskhdl ? taskhdl : _task_get_handle();
                _assert(taskhdl);

                _process_t *p = _task_get_tag(taskhdl);
                _assert(p);

                if (proc) {
                        *proc = p;
                }

                u8_t threads = PROC_MAX_THREADS(p);

                for (int i = 0; i < threads; i++) {
                        if (p->taskdata && (p->taskdata[i].task == taskhdl)) {
                                if (tid) {
                                        *tid = i;
                                }
                                break;
                        }
                }
        }
}

//==============================================================================
/**
 * @brief Function calculate general CPU load.
 *        Calculate are average values for 1/60, 1, 5, and 15 minutes.
 */
//==============================================================================
KERNELSPACE void _calculate_CPU_load(void)
{
        // calculates 1 second CPU load of all processes
        avg_CPU_load_calc.avg1sec = 0;

        // atomic section is not needed because this function is called from IRQ
        foreach_process(proc, active_process_list) {

                for (int i = 0; i < PROC_MAX_THREADS(proc); i++) {
                        if (proc->taskdata && proc->taskdata[i].task) {
                                proc->taskdata[i].CPU_load = proc->taskdata[i].timecnt / (_CPU_total_time / 1000);
                                proc->taskdata[i].timecnt  = 0;
                                avg_CPU_load_calc.avg1sec  += proc->taskdata[i].CPU_load;

                                proc->taskdata[i].syscalls = proc->taskdata[i].syscalls_ctr;
                                proc->taskdata[i].syscalls_ctr = 0;
                        }
                }
        }

        avg_CPU_load_result.avg1sec = avg_CPU_load_calc.avg1sec;

        _CPU_total_time     = 0;
        CPU_total_time_last = 0;

        // calculates 1 min average CPU load
        avg_CPU_load_calc.avg1min += avg_CPU_load_calc.avg1sec;
        avg_CPU_load_calc.avg1min /= 2;
        if (_uptime_counter_sec % 60 == 0) {
                avg_CPU_load_result.avg1min = avg_CPU_load_calc.avg1min;

                // calculates 5 min average CPU load
                avg_CPU_load_calc.avg5min  += avg_CPU_load_calc.avg1min;
                avg_CPU_load_calc.avg5min  /= 2;

                if (_uptime_counter_sec % 300 == 0) {
                        avg_CPU_load_result.avg5min = avg_CPU_load_calc.avg5min;

                        // calculates 15 min average CPU load
                        avg_CPU_load_calc.avg15min += avg_CPU_load_calc.avg5min;
                        avg_CPU_load_calc.avg15min /= 2;

                        if (_uptime_counter_sec % 900 == 0) {
                                avg_CPU_load_result.avg15min = avg_CPU_load_calc.avg15min;
                        }
                }
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
USERSPACE int _get_average_CPU_load(avg_CPU_load_t *avg)
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
 * @brief  This function start user process code [USERLAND].
 *
 * @param  mainfn         process argument - process main function
 */
//==============================================================================
USERSPACE static void process_code(void *mainfn)
{
        _assert(mainfn);

        process_func_t  funcmain = mainfn;
        _process_t     *proc     = _task_get_tag(_THIS_TASK);

        proc->status = funcmain(proc->argc, proc->argv);

        _process_exit(proc, proc->status);
}

//==============================================================================
/**
 * @brief  This function start user thread code [USERLAND].
 *
 * @param  arg          thread arguments - main function and argument value.
 */
//==============================================================================
USERSPACE static void thread_code(void *arg)
{
        _assert(arg);

        thread_args_t args = *cast(thread_args_t*, arg);
        _kfree(_MM_KRN, cast(void*, &arg));

        _assert(args.func);
        args.func(args.arg);

        _process_t *proc;
        tid_t       tid;
        _task_get_process_container(_THIS_TASK, &proc, &tid);
        _assert(is_proc_valid(proc));
        _assert(is_tid_in_range(proc, tid));

        ATOMIC(process_mtx) {
                if (proc->event) {
                        _flag_set(proc->event, _PROCESS_EXIT_FLAG(tid));
                }

                proc->taskdata[tid].task = NULL;
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
        u8_t threads = PROC_MAX_THREADS(proc);

        if (proc->taskdata) {
                for (tid_t tid = 0; tid < threads; tid++) {
                        if (proc->taskdata[tid].task) {
                                _task_destroy(proc->taskdata[tid].task);
                                proc->taskdata[tid].task = NULL;
                        }
                }

                _kfree(_MM_KRN, cast(void*, &proc->taskdata));
        }

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
static void process_get_stat(_process_t *proc, process_stat_t *stat)
{
        memset(stat, 0, sizeof(process_stat_t));

        stat->name     = proc->pdata->name;
        stat->pid      = proc->pid;
        stat->priority = (proc->taskdata && proc->taskdata[0].task) ? _task_get_priority(proc->taskdata[0].task) : 0;

        u8_t threads = PROC_MAX_THREADS(proc);
        for (tid_t tid = 0; tid < threads; tid++) {
                if (proc->taskdata && proc->taskdata[tid].task) {
                        stat->threads_count++;
                        stat->CPU_load        += proc->taskdata[tid].CPU_load;
                        stat->syscalls        += proc->taskdata[tid].syscalls;
                        stat->stack_max_usage += (proc->taskdata[tid].stack_size - _task_get_free_stack(proc->taskdata[tid].task));
                        stat->stack_size      += proc->taskdata[tid].stack_size;
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
static int process_apply_attributes(_process_t *proc, const process_attr_t *attr)
{
        int err = ESUCC;

        if (attr) {
                /*
                 * Apply Current Working Directory path
                 */
                size_t cwdlen = attr->cwd ? strsize(attr->cwd) : strsize("/");

                err = _kmalloc(_MM_KRN, cwdlen, cast(void*, &proc->cwd));
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
 * @brief Function create PID number.
 *
 * @return On success PID number is returned, otherwise 0.
 */
//==============================================================================
static int get_pid(pid_t *pid)
{
        int err = ESRCH;

        ATOMIC(process_mtx) {
                int n = 1000;
                while (pid && (n-- > 0)) {
                        if (++PID_cnt >= 1000) {
                                PID_cnt = 1;
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
                _vfs_fclose(cast(FILE*, res2free), true);
                break;

        case RES_TYPE_DIR:
                _vfs_closedir(cast(DIR*, res2free));
                break;

        case RES_TYPE_MEMORY:
                _kfree(_MM_PROG, cast(void*, &res2free));
                break;

        case RES_TYPE_MUTEX:
                _mutex_destroy(cast(mutex_t*, res2free));
                break;

        case RES_TYPE_QUEUE:
                _queue_destroy(cast(queue_t*, res2free));
                break;

        case RES_TYPE_SEMAPHORE:
                _semaphore_destroy(cast(sem_t*, res2free));
                break;

        case RES_TYPE_FLAG:
                _flag_destroy(cast(flag_t*, res2free));
                break;

        case RES_TYPE_SOCKET:
#if __ENABLE_NETWORK__ == _YES_
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
        FILE *file     = NULL;

        // skip leading spaces
        cmd += strspn(cmd, " ");

        // argument start
        char  *args    = strchr(cmd, ' ');
               args   += args ? 1 : 0;
        size_t argslen = args ? strsize(args) : 0;

        // file name length
        size_t fnlen = args ? ((u32_t)args - (u32_t)cmd) : strsize(cmd);

        // allocate file name
        int err = _kmalloc(_MM_KRN, fnlen, cast(void**, &filename));
        if (err) goto finish;
        strlcpy(filename, cmd, fnlen);

        // allocate line
        err = _kzalloc(_MM_KRN, SHEBANGLEN, cast(void**, &line));
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

                err = _kmalloc(_MM_KRN, arglen, cast(void**, cmdarg));
                if (!err) {
                        strlcpy(*cmdarg, p, arglen);
                        strlcat(*cmdarg, " ", arglen);
                        strlcat(*cmdarg, filename, arglen);

                        if (args) {
                                strlcat(*cmdarg, " ", arglen);
                                strlcat(*cmdarg, args, arglen);
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
                                err = _kmalloc(_MM_KRN, str_len + 1, cast(void**, &arg));
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
                        err = _kmalloc(_MM_KRN, (no_of_args + 1) * sizeof(char*), cast(void*, &arg));
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
 * @brief Function find program by name and return program descriptor container.
 *
 * @param name         program name
 * @param prog         program container
 *
 * @return One of errno value.
 */
//==============================================================================
static int find_program(const char *name, const struct _prog_data **prog)
{
        static const size_t kworker_stack_depth  = STACK_DEPTH_CUSTOM(__OS_IO_STACK_DEPTH__);
        static const size_t kworker_globals_size = 0;
        static const struct _prog_data kworker   = {.globals_size = &kworker_globals_size,
                                                    .main         = _syscall_kworker_process,
                                                    .name         = "kworker",
                                                    .stack_depth  = &kworker_stack_depth};

        int err = ENOENT;

        if (strncmp(name, "kworker", 32) == 0) {
                *prog = &kworker;
                err   = ESUCC;

        } else {
                for (int i = 0; i < _prog_table_size; i++) {
                        if (strncmp(_prog_table[i].name, name, 128) == 0) {
                                *prog  = &_prog_table[i];
                                err = ESUCC;
                                break;
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
static int allocate_process_globals(_process_t *proc, const struct _prog_data *usrprog)
{
        int err = ESUCC;

        if (*usrprog->globals_size > 0) {
                res_header_t *mem;
                err = _kzalloc(_MM_PROG, *usrprog->globals_size, cast(void*, &mem));
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
 * @param  task_tag     current task tag
 */
//==============================================================================
KERNELSPACE void _task_switched_in(task_t *task, void *task_tag)
{
        CPU_total_time_last = _CPU_total_time;
        active_process = task_tag;

        if (active_process && (active_process->header.type == RES_TYPE_PROCESS)) {
                stdin  = active_process->f_stdin;
                stdout = active_process->f_stdout;
                stderr = active_process->f_stderr;
                global = active_process->globals;
                _errno = active_process->errnov;

                u8_t threads = PROC_MAX_THREADS(active_process);

                for (u8_t i = 0; i < threads; i++) {
                        if (active_process->taskdata[i].task == task) {
                                active_process->curr_task = i;
                        }
                }

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
 *         to task context. Function is called before this task context to be
 *         switched.
 *         See FreeRTOSConfig.h file.
 *
 * @param  task         current task
 * @param  task_tag     current task tag
 */
//==============================================================================
KERNELSPACE void _task_switched_out(task_t *task, void *task_tag)
{
        UNUSED_ARG2(task, task_tag);

        if (active_process && (active_process->header.type == RES_TYPE_PROCESS)) {
                active_process->f_stdin  = stdin;
                active_process->f_stdout = stdout;
                active_process->f_stderr = stderr;
                active_process->globals  = global;
                active_process->errnov   = _errno;

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
