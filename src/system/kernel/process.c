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
#include <sys/ioctl.h>
#include "fs/vfs.h"
#include "kernel/process.h"
#include "lib/llist.h"
#include "kernel/kwrapper.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
typedef struct process {
        pid_t            pid;                   /* process ID                         */
        pid_t            parent;                /* parent process                     */
        task_t          *task;                  /* pointer to this task handle        */
        struct process  *next;                  /* next process (chain)               */
        FILE            *f_stdin;               /* stdin file                         */
        FILE            *f_stdout;              /* stdout file                        */
        FILE            *f_stderr;              /* stderr file                        */
        FILE            *f_list;                /* chain of open files                */
        const char      *cwd;                   /* current working path               */
        void            *globals;               /* address to global variables        */
        int              errnov;                /* program error number               */
        u32_t            timecnt;               /* counter used to calculate CPU load */
} process_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
static pid_t      PID_cnt = 1;
static process_t *head;
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
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
int _process_create(void (*func)(void*), const char *name, const uint stack_depth, void *argv, pid_t *pid)
{
        int result = EINVAL;

//        if (func && name && stack_depth && pid) {
//                process_t *proc;
//                task_t    *task;
//
//                result = _kmalloc(sizeof(process_t), &proc);
//                if (result == ESUCC) {
//
//                }
//        }
//
//        return result;
//
//
//        process_t *proc = _kmalloc(sizeof(process_t));
//        task_t    *task = _task_new(func, name, stack_depth, argv, proc);
//
//        if (proc && task) {
//                proc->task =
//                proc->pid  =
//        }
//
//
//        if (empty) {
//
//        } else {
//
//        }
//
//        _task_new();
//
//        _process_new(kworker_thread, "kworker", CONFIG_RTOS_SYSCALL_STACK_DEPTH, NULL, true);

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
        if (active_process && active_process->cwd) {
                return active_process->cwd;
        } else {
                return "";
        }
}

//==============================================================================
/**
 * @brief Function copy task context to standard variables (stdin, stdout, stderr,
 *        global, errno)
 */
//==============================================================================
void _copy_task_context_to_standard_variables(void)
{
        active_process = _task_get_tag(THIS_TASK);
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

/*==============================================================================
  End of file
==============================================================================*/
