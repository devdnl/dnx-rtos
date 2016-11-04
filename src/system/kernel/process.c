/*=========================================================================*//**
@file    process.c

@author  Daniel Zorychta

@brief   This file support processes

@note    Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "net/netm.h"
#include "kernel/process.h"
#include "kernel/kwrapper.h"
#include "kernel/kpanic.h"
#include "kernel/printk.h"
#include "lib/llist.h"
#include "lib/cast.h"
#include "dnx/misc.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define USERSPACE
#define KERNELSPACE
#define foreach_resource(_v, _l)        for (res_header_t *_v = _l; _v; _v = _v->next)
#define foreach_process(_v, _l)         for (_process_t *_v = _l; _v; _v = cast(_process_t*, _v->header.next))

#define catcherrno(_x)                  for (int _ = 0; _ < 1; _++) for (int *__ = &_x; __; __ = 0)
#define try(_x)                         if ((*__ = (_x)) != 0) break
#define onsuccess(_x)                   if ((_x) == 0)
#define onfailure(_x)                   if ((_x) != 0)

#define ATOMIC for (int __ = 0; __ == 0;)\
        for (_kernel_scheduler_lock(); __ == 0; _kernel_scheduler_unlock(), __++)

#if __OS_TASK_MAX_THREADS__ > 12
#error Maximum number of threads is 12
#endif

#if __OS_ENABLE_SYS_ASSERT__ > 0
#define PROCESS_ASSERT(assert)          while (not (assert))
#else
#define PROCESS_ASSERT(assert)
#endif

#define is_proc_valid(proc)             (proc && proc->header.type == RES_TYPE_PROCESS)
#define is_tid_in_range(tid)            ((tid >= 0) && (tid < __OS_TASK_MAX_THREADS__))

#define FLAG_DETACHED                   (1 << 0)

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
typedef struct _prog_data pdata_t;

struct _process {
        res_header_t     header;                        //!< resource header
        task_t          *task[__OS_TASK_MAX_THREADS__]; //!< process tasks
        flag_t          *event;                         //!< events for exit indicator and syscall finish
        FILE            *f_stdin;                       //!< stdin file
        FILE            *f_stdout;                      //!< stdout file
        FILE            *f_stderr;                      //!< stderr file
        void            *globals;                       //!< address to global variables
        res_header_t    *res_list;                      //!< list of used resources
        const char      *cwd;                           //!< current working path
        const pdata_t   *pdata;                         //!< program data
        char            **argv;                         //!< program arguments
        u8_t             argc;                          //!< number of arguments
        pid_t            pid;                           //!< process ID
        int              errnov;                        //!< program error number
        u32_t            timecnt;                       //!< counter used to calculate CPU load
        u16_t            CPU_load;                      //!< CPU load (10 = 1%)
        i8_t             status;                        //!< program status (return value)
        u8_t             flag;                          //!< control flags
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

/*==============================================================================
  Local object definitions
==============================================================================*/
static pid_t          PID_cnt;
static _process_t    *active_process_list;
static _process_t    *destroy_process_list;
static _process_t    *zombie_process_list;
static _process_t    *active_process;
static tid_t          active_thread;
static u32_t          CPU_total_time_last;
static avg_CPU_load_t avg_CPU_load_calc;
static avg_CPU_load_t avg_CPU_load_result;

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
int _errno = NULL;

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
        int         err  = EINVAL;
        _process_t *proc = NULL;

        catcherrno(err) {
                try(!cmd || cmd[0] == '\0' ? EINVAL : ESUCC);
                try(_kzalloc(_MM_KRN, sizeof(_process_t), cast(void**, &proc)));

                proc->header.type = RES_TYPE_PROCESS;

                try(argtab_create(cmd, &proc->argc, &proc->argv));

                try(find_program(proc->argv[0], &proc->pdata));

                try(allocate_process_globals(proc, proc->pdata));

                try(process_apply_attributes(proc, attr));

                try(get_pid(&proc->pid));

                if (proc->pdata->main != _syscall_kworker_process) {
                        try (_flag_create(&proc->event));
                }

                ATOMIC {
                        catcherrno(err) {
                                try(_task_create(process_code,
                                                 proc->pdata->name,
                                                 *proc->pdata->stack_depth,
                                                 proc->pdata->main,
                                                 proc,
                                                 &proc->task[0]));
                        } onsuccess(err) {
                                if (attr) {
                                        _task_set_priority(proc->task[0], attr->priority);
                                }

                                if (pid) {
                                        *pid = proc->pid;
                                }

                                if (active_process_list == NULL) {
                                        active_process_list = proc;
                                } else {
                                        proc->header.next = cast(res_header_t*, active_process_list);
                                        active_process_list      = proc;
                                }
                        }
                }

        } onfailure(err) {
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
        ATOMIC {
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

        ATOMIC {
                foreach_process(proc, active_process_list) {
                        if (proc->pid == pid) {
                                if (proc->event) {
                                        _flag_set(proc->event, _PROCESS_EXIT_FLAG(0));
                                }

                                for (int i = 0; i < __OS_TASK_MAX_THREADS__; i++) {
                                        if (proc->task[i]) {
                                                _task_destroy(proc->task[i]);
                                                proc->task[i] = NULL;
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
        PROCESS_ASSERT(is_proc_valid(proc));

        ATOMIC {
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
        if (is_proc_valid(proc)) {
                proc->status = status;

                ATOMIC {
                        if (proc->event) {
                                _flag_set(proc->event, _PROCESS_EXIT_FLAG(0));
                        }

                        for (int i = 1; i < __OS_TASK_MAX_THREADS__; i++) {
                                if (proc->task[i]) {
                                        _task_destroy(proc->task[i]);
                                        proc->task[i] = NULL;
                                }
                        }

                        process_move_list(proc, &active_process_list, &destroy_process_list);

                        proc->task[0] = NULL;
                }

                _task_exit();
        } else {
                PROCESS_ASSERT(is_proc_valid(proc));
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
                PROCESS_ASSERT(is_proc_valid(proc));
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
                return proc->cwd ? proc->cwd : "";
        } else {
                return "";
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
        if (is_proc_valid(proc)) {
                proc->cwd = CWD ? CWD : "";
                return ESUCC;
        } else {
                return EINVAL;
        }
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

                ATOMIC {
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

                ATOMIC {
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

        ATOMIC {
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
                ATOMIC {
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
                ATOMIC {
                        if (active_process->pid == pid) {
                                *prio = _task_get_priority(active_process->task[0]);
                                err   = ESUCC;

                        } else {
                                foreach_process(proc, active_process_list) {
                                        if (proc->pid == pid) {
                                                *prio  = _task_get_priority(proc->task[0]);
                                                err = ESUCC;
                                                break;
                                        }
                                }
                        }
                }
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
                ATOMIC {
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
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function return container of active thread
 *
 * @return Thread ID.
 */
//==============================================================================
KERNELSPACE tid_t _process_get_active_thread(void)
{
        return active_thread;
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
                ATOMIC {
                        if (proc->res_list == NULL) {
                                proc->res_list = resource;
                        } else {
                                resource->next = proc->res_list;
                                proc->res_list = resource;
                        }
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

                ATOMIC {
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

                                                obj_to_destroy = curr;
                                        } else {
                                                err = EFAULT;
                                        }

                                        break;
                                } else {

                                        prev = curr;

                                        if (--max_deep == 0) {
                                                PROCESS_ASSERT(max_deep > 0);
                                                _kernel_panic_report(_KERNEL_PANIC_DESC_CAUSE_INTERNAL);
                                        }
                                }
                        }
                }

                if (obj_to_destroy) {
                        err = resource_destroy(obj_to_destroy);
                        if (err != ESUCC) {
                                _kernel_panic_report(_KERNEL_PANIC_DESC_CAUSE_INTERNAL);
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

        if (!is_proc_valid(proc) || !func) {
                return err;
        }

        ATOMIC {
                tid_t id = 1;
                for (; id < __OS_TASK_MAX_THREADS__; id++) {
                        if (proc->task[id] == NULL) {
                                break;
                        }
                }

                if (id < __OS_TASK_MAX_THREADS__) {

                        catcherrno(err) {
                                thread_args_t *args = NULL;

                                try(_kmalloc(_MM_KRN, sizeof(thread_args_t), cast(void*, &args)));

                                args->func = func;
                                args->arg  = arg;

                                try (_task_create(thread_code, "",
                                                  (attr ? attr->stack_depth : STACK_DEPTH_LOW),
                                                  args, proc, &proc->task[id]));

                                onsuccess(err) {

                                        if (proc->event) {
                                                _flag_clear(proc->event,
                                                            _PROCESS_EXIT_FLAG(id) |
                                                            _PROCESS_SYSCALL_FLAG(id));
                                        }

                                        if (attr) {
                                                _task_set_priority(proc->task[id],
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

        if (is_proc_valid(proc) && is_tid_in_range(tid)) {
                ATOMIC {
                        _task_destroy(proc->task[tid]);
                        proc->task[tid] = NULL;

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

        if (is_proc_valid(proc) && is_tid_in_range(tid)) {
                ATOMIC {
                        task = proc->task[tid];
                }
        }

        return task;

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
        ATOMIC {

                taskhdl = taskhdl ? taskhdl : _task_get_handle();
                PROCESS_ASSERT(taskhdl);

                _process_t *p = _task_get_tag(taskhdl);
                if (proc) {
                        *proc = p;
                }

                for (int i = 0; i < __OS_TASK_MAX_THREADS__; i++) {
                        if (p->task[i] == taskhdl) {
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

        ATOMIC {
                foreach_process(proc, active_process_list) {
                        proc->CPU_load = proc->timecnt / (_CPU_total_time / 1000);
                        proc->timecnt  = 0;
                        avg_CPU_load_calc.avg1sec += proc->CPU_load;
                }
        }

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
        PROCESS_ASSERT(mainfn);

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
        PROCESS_ASSERT(arg);

        thread_args_t args = *cast(thread_args_t*, arg);
        _kfree(_MM_KRN, cast(void*, &arg));

        PROCESS_ASSERT(args.func);
        args.func(args.arg);

        _process_t *proc;
        tid_t       tid;
        _task_get_process_container(_THIS_TASK, &proc, &tid);
        PROCESS_ASSERT(is_proc_valid(proc));
        PROCESS_ASSERT(is_tid_in_range(tid));

        ATOMIC {
                if (proc->event) {
                        _flag_set(proc->event, _PROCESS_EXIT_FLAG(tid));
                }

                proc->task[tid] = NULL;
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
        ATOMIC {
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
        for (tid_t tid = 0; tid < __OS_TASK_MAX_THREADS__; tid++) {
                if (proc->task[tid]) {
                        _task_destroy(proc->task[tid]);
                        proc->task[tid] = NULL;
                }
        }

        if (proc->argv) {
                argtab_destroy(proc->argv);
                proc->argv = NULL;
                proc->argc = 0;
        }

        // free all resources
        while (proc->res_list) {
                res_header_t *resource = proc->res_list;
                proc->res_list = resource->next;

                int err = resource_destroy(resource);
                if (err != ESUCC) {
                        _printk("Unknown object: %p\n", resource);
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
 * @brief  Function gets process statistics.
 *
 * @param  proc         process
 * @param  stat         statistics container
 */
//==============================================================================
static void process_get_stat(_process_t *proc, process_stat_t *stat)
{
        memset(stat, 0, sizeof(process_stat_t));

        stat->name            = proc->pdata->name;
        stat->pid             = proc->pid;
        stat->stack_size      = proc->task[0] ? *proc->pdata->stack_depth : 0;
        stat->stack_max_usage = proc->task[0] ? (stat->stack_size - _task_get_free_stack(proc->task[0])) : 0;
        stat->priority        = proc->task[0] ? _task_get_priority(proc->task[0]) : 0;
        stat->CPU_load        = proc->CPU_load;
        stat->memory_usage   += sizeof(_process_t);
        stat->memory_usage   += proc->task[0] ? (*proc->pdata->stack_depth * sizeof(StackType_t)) : 0;
        stat->memory_usage   += proc->event ? sizeof(flag_t) : 0;
        stat->threads_count   = 0;
        stat->socket_count    = 0;
        stat->threads_count   = 0;

        for (int i = 0; proc->argv && i < proc->argc; i++) {
                stat->memory_usage += strnlen(proc->argv[i], 256);
        }

        for (tid_t tid = 0; tid < __OS_TASK_MAX_THREADS__; tid++) {
                if (proc->task[tid]) {
                        stat->threads_count++;
                        stat->memory_usage += sizeof(task_t);
                }
        }

        foreach_resource(res, proc->res_list) {
                switch (res->type) {
                case RES_TYPE_FILE:
                        stat->files_count++;
                        stat->memory_usage += sizeof(FILE);
                        break;

                case RES_TYPE_DIR:
                        stat->dir_count++;
                        stat->memory_usage += sizeof(DIR);
                        break;

                case RES_TYPE_MUTEX:
                        stat->mutexes_count++;
                        stat->memory_usage += sizeof(mutex_t);
                        break;

                case RES_TYPE_QUEUE:
                        stat->queue_count++;
                        stat->memory_usage += sizeof(queue_t);
                        break;

                case RES_TYPE_FLAG:
                case RES_TYPE_SEMAPHORE:
                        stat->semaphores_count++;
                        stat->memory_usage += sizeof(sem_t);
                        break;

                case RES_TYPE_MEMORY:
                        stat->memory_block_count++;
                        stat->memory_usage += _mm_get_block_size(res);
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
                 * Apply stdin settings
                 * - if f_stdin is set then function use this resource as reference
                 * - if f_stdin is NULL and p_stdin is NULL then function set stdin as NULL
                 * - if f_stdin is NULL and p_stdin is valid then function open new file and use as stdin
                 */
                if (attr->f_stdin) {
                        proc->f_stdin = attr->f_stdin;

                } else if (attr->p_stdin) {
                        struct vfs_path cpath;
                        cpath.CWD  = attr->cwd;
                        cpath.PATH = attr->p_stdin;

                        err = _vfs_fopen(&cpath, "a+", &proc->f_stdin);
                        if (err == ESUCC) {
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
                                cpath.CWD  = attr->cwd;
                                cpath.PATH = attr->p_stdout;

                                err = _vfs_fopen(&cpath, "a", &proc->f_stdout);
                                if (err == ESUCC) {
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
                                cpath.CWD  = attr->cwd;
                                cpath.PATH = attr->p_stderr;

                                err = _vfs_fopen(&cpath, "a", &proc->f_stderr);
                                if (err == ESUCC) {
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

                /*
                 * Apply Current Working Directory path
                 */
                proc->cwd = attr->cwd;
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

        ATOMIC {
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

        if (str && argc && argv) {

                llist_t *largs;
                err = _llist_create_krn(_MM_KRN, NULL, NULL, &largs);

                if (err == ESUCC) {
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
                                if (err == ESUCC) {
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
                        err = _kmalloc(_MM_KRN, (no_of_args + 1) * sizeof(char*), cast(void*, &arg));
                        if (err == ESUCC) {
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
        static const size_t kworker_stack_depth  = __OS_SYSCALL_STACK_DEPTH__;
        static const size_t kworker_globals_size = 0;
        static const struct _prog_data kworker   = {.globals_size = &kworker_globals_size,
                                                    .main         = _syscall_kworker_process,
                                                    .name         = "kworker",
                                                    .stack_depth  = &kworker_stack_depth};

        int err = ENOENT;

        if (strncmp(name, "kworker", 32) == 0) {
                *prog  = &kworker;
                err = ESUCC;

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

                if (err == ESUCC) {
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
 */
//==============================================================================
KERNELSPACE void _task_switched_in(void)
{
#if (__OS_MONITOR_CPU_LOAD__ > 0)
        _CPU_total_time    += _cpuctl_get_CPU_load_counter_delta();
        CPU_total_time_last = _CPU_total_time;
#endif

        active_process = NULL;
        active_thread  = -1;
        _task_get_process_container(_THIS_TASK, &active_process, &active_thread);

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
 *         to task context. Function is called before this task context to be
 *         switched.
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

                #if (__OS_MONITOR_CPU_LOAD__ > 0)
                _CPU_total_time         += _cpuctl_get_CPU_load_counter_delta();
                active_process->timecnt += (_CPU_total_time - CPU_total_time_last);
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
