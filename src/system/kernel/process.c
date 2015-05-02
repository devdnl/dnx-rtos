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
//#include <sys/ioctl.h>
#include "fs/vfs.h"
#include "kernel/process.h"
#include "kernel/kwrapper.h"
#include "lib/llist.h"
#include "lib/cast.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
typedef enum {
        TASK_TYPE_PROCESS,
        TASK_TYPE_THREAD
} task_type_t;

typedef struct process {
        task_type_t      type;                  /* task type                          */
        struct process  *next;                  /* next process                       */
        task_t          *task;                  /* pointer to task handle             */
        FILE            *f_stdin;               /* stdin file                         */
        FILE            *f_stdout;              /* stdout file                        */
        FILE            *f_stderr;              /* stderr file                        */
        FILE            *res_list;              /* list of used resources             */
        const char      *cwd;                   /* current working path               */
        void            *globals;               /* address to global variables        */
        char            **argv;                 /* program arguments                  */
        int              argc;                  /* number of arguments                */
        int              ret;                   /* program return value               */
        pid_t            pid;                   /* process ID                         */
        pid_t            ppid;                  /* parent process ID                  */
        int              errnov;                /* program error number               */
        u32_t            timecnt;               /* counter used to calculate CPU load */

        struct {
                bool    f_stdin_ref:1;          /* stdin file is referenced           */
                bool    f_stdout_ref:1;         /* stdout file is referenced          */
                bool    f_stderr_ref:1;         /* stderr file is referenced          */
        } flag;
} process_t;

typedef struct thread {
        task_type_t      type;                  /* task type                          */
        struct thread   *next;                  /* next thread                        */
        task_t          *task;                  /* pointer to task handle             */
        struct process  *process;               /* process of this thread             */
} thread_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void process_startup(void *arg);
static int  argtab_create(const char *str, int *argc, char **argv[]);
static void argtab_destroy(char **argv);

/*==============================================================================
  Local object definitions
==============================================================================*/
static pid_t      PID_cnt = 1;
static process_t *process_list_head;
static process_t *process_list_tail;
static process_t *active_process;

const size_t kworker_stack_depth  = STACK_DEPTH_LOW;
const size_t kworker_globals_size = 0;
const struct _prog_data kworker   = {.globals_size = &kworker_globals_size,
                                     .main         = _syscall_kworker_master,
                                     .name         = "kworker",
                                     .stack_depth  = &kworker_stack_depth};

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
int _process_create(pid_t *pid, const process_attr_t *attr, const char *cmd)
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
                        const struct _prog_data *usrprog = NULL;
                        if (strncmp(proc->argv[0], "kworker", 32) == 0) {
                                usrprog = &kworker;
                        } else {
                                for (int i = 0; i < _prog_table_size; i++) {
                                        if (strncmp(_prog_table[i].name, proc->argv[0], 128) == 0) {
                                                usrprog = &_prog_table[i];
                                        }
                                }
                        }

                        if (usrprog == NULL) {
                                result = ENOENT;
                                goto finish;
                        }

                        // allocate program's global variables
                        if (*usrprog->globals_size > 0) {
                                result = _kzalloc(_MM_KRN, *usrprog->globals_size, &proc->globals);
                                if (result != ESUCC)
                                        goto finish;
                        }

                        // set configured program settings
                        if (attr) {
                                if (attr->f_stdin) {
                                        proc->f_stdin          = attr->f_stdin;
                                        proc->flag.f_stdin_ref = true;

                                } else if (attr->p_stdin) {
                                        result = _vfs_fopen(attr->p_stdin, "a+", &proc->f_stdin);
                                        if (result != ESUCC)
                                                goto finish;
                                }

                                if (attr->f_stdout) {
                                        proc->f_stdout          = attr->f_stdout;
                                        proc->flag.f_stdout_ref = true;

                                } else if (attr->p_stdout) {
                                        if (strcmp(attr->p_stdout, attr->p_stdin) == 0) {
                                                proc->f_stdout = proc->f_stdin;
                                        } else {
                                                result = _vfs_fopen(attr->p_stdout, "a", &proc->f_stdout);
                                                if (result != ESUCC)
                                                        goto finish;
                                        }
                                }

                                if (attr->f_stderr) {
                                        proc->f_stderr          = attr->f_stderr;
                                        proc->flag.f_stderr_ref = true;

                                } else if (attr->p_stderr) {
                                        if (strcmp(attr->p_stderr, attr->p_stdin) == 0) {
                                                proc->f_stderr = proc->f_stdin;

                                        } else if (strcmp(attr->p_stderr, attr->p_stdout) == 0) {
                                                proc->f_stderr = proc->f_stdout;

                                        } else {
                                                result = _vfs_fopen(attr->p_stderr, "a", &proc->f_stderr);
                                                if (result != ESUCC)
                                                        goto finish;
                                        }
                                }

                                proc->cwd = attr->cwd;
                        }

                        // get parent PID
                        process_t *pproc = _task_get_tag(_THIS_TASK);
                        if (pproc) {
                                proc->ppid = pproc->pid;
                        }

                        // set default program settings
                        proc->pid      = PID_cnt++;
                        proc->errnov   = ESUCC;
                        proc->timecnt  = 0;
                        proc->res_list = NULL;

                        // create program's task
                        result = _task_create(process_startup,
                                              usrprog->name,
                                              *usrprog->stack_depth,
                                              usrprog->main,
                                              proc,
                                              &proc->task);
                        if (result == ESUCC) {
                                *pid = proc->pid;

                                if (process_list_head == NULL) {
                                        process_list_head = proc;
                                        process_list_tail = proc;
                                } else {
                                        process_list_tail->next = proc;
                                        process_list_tail = proc;
                                }
                        }

                        finish:
                        if (result != ESUCC) {
                                if (proc->globals)
                                        _kfree(_MM_KRN, &proc->globals);

                                if (proc->argv)
                                        argtab_destroy(proc->argv);

                                if (proc->f_stdin && !proc->flag.f_stdin_ref)
                                        _vfs_fclose(proc->f_stdin, false);

                                if (proc->f_stdout && !proc->flag.f_stdout_ref)
                                        _vfs_fclose(proc->f_stdout, false);

                                if (proc->f_stderr && !proc->flag.f_stderr_ref)
                                        _vfs_fclose(proc->f_stderr, false);

                                _kfree(_MM_KRN, static_cast(void**, &proc));
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
int _process_destroy(pid_t pid)
{
        return EINVAL;
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
const char *_process_get_CWD()
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
 * @brief Function copy task context to standard variables (stdin, stdout, stderr,
 *        global, errno)
 */
//==============================================================================
void _copy_task_context_to_standard_variables(void)
{
        active_process = _task_get_tag(_THIS_TASK);

        if (active_process->type == TASK_TYPE_THREAD) {
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
void _copy_standard_variables_to_task_context(void)
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
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void process_startup(void *arg)
{
        process_func_t funcmain = arg;
        process_t     *proc     = _task_get_tag(_THIS_TASK);

        proc->ret  = funcmain(proc->argc, proc->argv);
        proc->task = NULL;

        _task_exit();
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
static int argtab_create(const char *str, int *argc, char **argv[])
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
static void argtab_destroy(char **argv)
{
        if (argv) {
                int n = 0;
                while (argv[n]) {
                        _kfree(_MM_KRN, static_cast(void*, &argv[n]));
                }

                _kfree(_MM_KRN, static_cast(void*, &argv));
        }
}

/*==============================================================================
  End of file
==============================================================================*/
