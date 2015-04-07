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
typedef enum task_type {
        TASK_TYPE_RAW,
        TASK_TYPE_PROCESS,
        TASK_TYPE_THREAD
} task_type_t;

typedef struct task_desc {
        pid_t            t_PID;
        FILE            *t_stdin;               /* stdin file                         */
        FILE            *t_stdout;              /* stdout file                        */
        FILE            *t_stderr;              /* stderr file                        */
        FILE            *t_file_chain;          /* chain of open files                */
        const char      *t_cwd;                 /* current working path               */
        void            *t_globals;             /* address to global variables        */
        task_t          *t_parent;              /* parent task                        */
        task_t          *t_self;                /* pointer to this task handle        */
        task_t          *t_next;                /* next task (chain)                  */
        u32_t            t_load_time;           /* counter used to calculate CPU load */
        int              t_errno;               /* program error number               */
        task_type_t      t_task_type:2;         /* task type                          */
} task_desc_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
static       u32_t PID_cnt             = 1;
static const uint  mutex_wait_attempts = 10;

/* active task descriptor */
task_desc_t *task_desc;

/*==============================================================================
  Exported object definitions
==============================================================================*/
/* standard input */
FILE *stdin;

/* standard output */
FILE *stdout;

/* standard error */
FILE *stderr;

/* global variables */
struct _GVAR_STRUCT_NAME *global;

/* error number */
int _errno;

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
const char *_process_get_CWD()
{
        if (task_desc) {
                return task_desc->t_cwd;
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
        task_desc = _task_get_tag(THIS_TASK);
        if (task_desc) {
                stdin  = task_desc->t_stdin;
                stdout = task_desc->t_stdout;
                stderr = task_desc->t_stderr;
                global = task_desc->t_globals;
                _errno = task_desc->t_errno;
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
        if (task_desc) {
                task_desc->t_stdin   = stdin;
                task_desc->t_stdout  = stdout;
                task_desc->t_stderr  = stderr;
                task_desc->t_globals = global;
                task_desc->t_errno   = _errno;
        }
}

/*==============================================================================
  End of file
==============================================================================*/
