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
#include "lib/llist.h"
#include "lib/cast.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define USERSPACE
#define KERNELSPACE
#define foreach_process_resources(_v, _l) for (res_header_t *_v = _l; _v; _v = _v->next)

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
typedef struct process {
        res_header_t     header;                /* resource header                    */
        task_t          *task;                  /* pointer to task handle             */
        FILE            *f_stdin;               /* stdin file                         */
        FILE            *f_stdout;              /* stdout file                        */
        FILE            *f_stderr;              /* stderr file                        */
        void            *globals;               /* address to global variables        */
        res_header_t    *res_list;              /* list of used resources             */
        const char      *cwd;                   /* current working path               */
        char            **argv;                 /* program arguments                  */
        int              argc;                  /* number of arguments                */
        int              status;                /* program status (return value)      */
        pid_t            pid;                   /* process ID                         */
        pid_t            ppid;                  /* parent process ID                  */
        int              errnov;                /* program error number               */
        u32_t            timecnt;               /* counter used to calculate CPU load */
        i32_t            usedmem;               /* used memory                        */
} process_t;

typedef struct thread {
        res_header_t     header;                /* resource header                    */
        task_t          *task;                  /* pointer to task handle             */
        process_t       *process;               /* reference to process               */
} thread_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void process_code(void *arg);
static void process_free_resources(process_t *proc);
static int  argtab_create(const char *str, int *argc, char **argv[]);
static void argtab_destroy(char **argv);
static int  find_program(const char *name, const struct _prog_data **prog);
static int  allocate_process_globals(process_t *proc, const struct _prog_data *usrprog);
static int  apply_process_attributes(process_t *proc, const process_attr_t *attr);
static void chain_push(res_header_t **chain, res_header_t *item);
static res_header_t *chain_at(res_header_t *chain, size_t i);
static res_header_t *chain_take(res_header_t *chain, res_header_t *target);

/*==============================================================================
  Local object definitions
==============================================================================*/
static pid_t      PID_cnt = 1;
static process_t *process_list_head;
static process_t *process_list_tail;
static process_t *active_process;

/*==============================================================================
  Exported object definitions
==============================================================================*/
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
 * @param[out] pid      PID of created process (can be NULL)
 * @param[in]  attr     process attributes (use NULL for default attributes)
 * @param[in]  cmd      command (name + arguments)
 *
 * @return One of errno value.
 */
//==============================================================================
KERNELSPACE int _process_create(pid_t *pid, const process_attr_t *attr, const char *cmd)
{
        int result = EINVAL;

        if (cmd && cmd[0] != '\0') {
                process_t *proc;
                result = _kzalloc(_MM_KRN, sizeof(process_t), static_cast(void**, &proc));
                if (result == ESUCC) {

                        // parse program's arguments
                        result = argtab_create(cmd, &proc->argc, &proc->argv);
                        if (result != ESUCC)
                                goto finish;

                        // find program
                        const struct _prog_data *usrprog;
                        result = find_program(proc->argv[0], &usrprog);
                        if (result != ESUCC)
                                goto finish;

                        // allocate program's global variables
                        result = allocate_process_globals(proc, usrprog);
                        if (result != ESUCC)
                                goto finish;

                        // set configured program settings
                        result = apply_process_attributes(proc, attr);
                        if (result != ESUCC)
                                goto finish;

                        // get parent PID
                        process_t *pproc = _task_get_tag(_THIS_TASK);
                        if (pproc) {
                                proc->ppid = pproc->pid;
                        }

                        // set default program settings
                        proc->pid = PID_cnt++;

                        // create program's task
                        result = _task_create(process_code,
                                              usrprog->name,
                                              *usrprog->stack_depth,
                                              usrprog->main,
                                              proc,
                                              &proc->task);

                        if (result == ESUCC) {
                                if (pid) {
                                        *pid = proc->pid;
                                }

                                _critical_section_begin();
                                if (process_list_head == NULL) {
                                        process_list_head = proc;
                                        process_list_tail = proc;
                                } else {
                                        process_list_tail->header.next = proc;
                                        process_list_tail = proc;
                                }
                                _critical_section_end();

                                proc->header.type = RES_TYPE_PROCESS;
                        }

                        finish:
                        if (result != ESUCC) {
                                process_free_resources(proc);
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
 * @parma  status       process exit status (can be NULL)
 *
 * @return One of errno value
 */
//==============================================================================
KERNELSPACE int _process_destroy(pid_t pid, int *status)
{
        int result = EINVAL;

        process_t *proc = process_list_head;

        while (proc) {
                if (proc->pid == pid) {
                        _critical_section_begin();
                        {
                                process_list_head = proc->header.next;

                                if (process_list_tail == proc)
                                        process_list_tail = process_list_head;
                        }
                        _critical_section_end();

                        if (status) {
                                *status = proc->status;
                        }

                        process_free_resources(proc);
                        _kfree(_MM_KRN, static_cast(void**, &proc));

                        result = ESUCC;
                } else {
                        proc = proc->header.next;
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief  Clear all process resources, set exit code and close task. Process is
 *         not removed from process list.
 *
 * @param  taskhdl      task handle (contain PID)
 * @param  status       status (exit code)
 *
 * @return One of errno value
 */
//==============================================================================
KERNELSPACE int _process_exit(task_t *taskhdl, int status)
{
        int result = EINVAL;

        if (taskhdl) {
                process_t *proc = _task_get_tag(taskhdl);
                if (proc) {
                        proc->status = status;
                        process_free_resources(proc);
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief  Works almost the same as _process_exit() but set exit code to -1 and
 *         print on process's terminal suitable message.
 *
 * @param  pid          process ID
 *
 * @return One of errno value
 */
//==============================================================================
KERNELSPACE int _process_abort(task_t *taskhdl)
{
        process_t *proc = _task_get_tag(taskhdl);
        if (proc) {
                const char *aborted = "Aborted\n";
                size_t      wrcnt;
                _vfs_fwrite(aborted, strlen(aborted), &wrcnt, proc->f_stderr);
        }

        return _process_exit(taskhdl, -1);
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
KERNELSPACE const char *_process_get_CWD()
{
        const char *cwd = "";

        _critical_section_begin();
        if (active_process && active_process->cwd) {
                cwd = active_process->cwd;
        }
        _critical_section_end();

        return cwd;
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
int _process_memalloc(task_t *taskhdl, size_t size, void **mem, bool clear)
{
        int result = EINVAL;

        process_t *proc = _task_get_tag(taskhdl);
        if (proc) {
                if (proc->header.type == RES_TYPE_THREAD) {
                        proc = reinterpret_cast(thread_t*, proc)->process;
                }

                if (proc && proc->header.type == RES_TYPE_PROCESS) {
                        void *blk;

                        if (clear) {
                                result = _kzalloc(_MM_PROG, size, &blk, &proc->usedmem);
                        } else {
                                result = _kmalloc(_MM_PROG, size, &blk, &proc->usedmem);
                        }

                        if (result == ESUCC) {
                                chain_push(&proc->res_list, blk);
                                *mem = &reinterpret_cast(res_header_t*, blk)[1];
                        }
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
int _process_memfree(task_t *taskhdl, void *mem)
{
        int result = EINVAL;

        process_t *proc = _task_get_tag(taskhdl);
        if (proc) {
                if (proc->header.type == RES_TYPE_THREAD) {
                        proc = reinterpret_cast(thread_t*, proc)->process;
                }

                if (proc && proc->header.type == RES_TYPE_PROCESS) {
                        void *blk = reinterpret_cast(res_header_t*, mem) - 1;

                        foreach_process_resources(res, proc->res_list) {
                                if (res == blk) {
                                        result = _kfree(_MM_PROG, &blk, &proc->usedmem);
                                        if (result == EFAULT) {
                                                const char *msg = "*** Error: double free or corruption ***\n";
                                                size_t wrcnt;
                                                _vfs_fwrite(msg, strlen(msg), &wrcnt, proc->f_stderr);
                                        }

                                        break;
                                }
                        }
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
KERNELSPACE int _thread_create()
{
        return ENOMEM;
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
KERNELSPACE int _thread_destroy(thread_t *thread)
{
        return EINVAL;
}

//==============================================================================
/**
 * @brief Function copy task context to standard variables (stdin, stdout, stderr,
 *        global, errno)
 */
//==============================================================================
KERNELSPACE void _copy_task_context_to_standard_variables(void)
{
        active_process = _task_get_tag(_THIS_TASK);

        if (active_process->header.type == RES_TYPE_THREAD) {
                active_process = reinterpret_cast(thread_t*, active_process)->process;
        }

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
 * @brief Function copy standard variables (stdin, stdout, stderr, global, errno)
 *        to task context
 */
//==============================================================================
KERNELSPACE void _copy_standard_variables_to_task_context(void)
{
        if (active_process) {
                active_process->f_stdin  = stdin;
                active_process->f_stdout = stdout;
                active_process->f_stderr = stderr;
                active_process->globals  = global;
                active_process->errnov   = _errno;
        }
}

//==============================================================================
/**
 * @brief  This function start user code [USERLAND]
 *
 * @param  arg          process argument - process main function
 *
 * @return None
 */
//==============================================================================
USERSPACE static void process_code(void *arg)
{
        process_func_t funcmain = arg;
        process_t     *proc     = _task_get_tag(_THIS_TASK);

        proc->status  = funcmain(proc->argc, proc->argv);
        proc->task = NULL;

        syscall(SYSCALL_EXIT, &proc->status);

        _task_exit();
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
KERNELSPACE static void process_free_resources(process_t *proc)
{
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
        foreach_process_resources(res, proc->res_list) {
                if (res->type == RES_TYPE_THREAD) {
                        _task_suspend(reinterpret_cast(thread_t*, res)->task);
                }
        }

        // free all resources
        foreach_process_resources(res, proc->res_list) {
                res_header_t *res2free = res;
                switch (res->type) {
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
                        _thread_destroy(static_cast(thread_t*, res2free));
                        break;

                default:
                        break;
                }
        }

        proc->f_stdin  = NULL;
        proc->f_stdout = NULL;
        proc->f_stderr = NULL;
        proc->globals  = NULL;
        proc->cwd      = NULL;

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
KERNELSPACE static int argtab_create(const char *str, int *argc, char **argv[])
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
 * @brief Function remove argument table
 *
 * @param argv          pointer to argument table (must be ended with NULL)
 */
//==============================================================================
KERNELSPACE static void argtab_destroy(char **argv)
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
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
KERNELSPACE static int find_program(const char *name, const struct _prog_data **prog)
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
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
KERNELSPACE static int allocate_process_globals(process_t *proc, const struct _prog_data *usrprog)
{
        int result = ESUCC;

        if (*usrprog->globals_size > 0) {
                res_header_t *mem;
                result = _kzalloc(_MM_PROG,
                                  *usrprog->globals_size,
                                  static_cast(void*, &mem),
                                  &proc->usedmem);

                if (result == ESUCC) {
                        proc->globals = &mem[1];
                        chain_push(&proc->res_list, mem);
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
KERNELSPACE static int apply_process_attributes(process_t *proc, const process_attr_t *attr)
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
                                chain_push(&proc->res_list, reinterpret_cast(res_header_t*, proc->f_stdin));
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
                                        chain_push(&proc->res_list, reinterpret_cast(res_header_t*, proc->f_stdout));
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
                                        chain_push(&proc->res_list, reinterpret_cast(res_header_t*, proc->f_stdout));
                                } else {
                                        goto finish;
                                }
                        }
                }

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
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
KERNELSPACE static void chain_push(res_header_t **chain, res_header_t *item)
{
        if (chain) {
                if (*chain == NULL) {
                        *chain = item;
                } else {
                        res_header_t *ch = *chain;
                        while (ch) {
                                if (ch->next == NULL) {
                                        ch->next = item;
                                        break;
                                } else {
                                        ch = ch->next;
                                }
                        }
                }
        }
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
KERNELSPACE static res_header_t *chain_at(res_header_t *chain, size_t i)
{
        while (chain && i) {
                chain = chain->next;
                i--;
        }

        return chain;
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
KERNELSPACE static res_header_t *chain_take(res_header_t *chain, res_header_t *target)
{
        while (chain) {
                if (chain->next == target) {
                        chain->next = target->next;
                        return target;
                }
        }

        return NULL;
}

/*==============================================================================
  End of file
==============================================================================*/
