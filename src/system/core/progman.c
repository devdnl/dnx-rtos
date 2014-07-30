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
#include <dnx/thread.h>
#include "core/progman.h"
#include "core/sysmoni.h"
#include "core/list.h"
#include "kernel/kwrapper.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
struct prog {
        int            (*func)(int, char**);
        const char      *cwd;
        void            *mem;
        FILE            *stdin;
        FILE            *stdout;
        FILE            *stderr;
        sem_t           *exit_sem;
        task_t          *task;
        char            **argv;
        int              argc;
        uint             mem_size;
        int              exit_code;
        u32_t            valid;
};

struct thread {
        void           (*func)(void*);
        void            *arg;
        void            *mem;
        FILE            *stdin;
        FILE            *stdout;
        FILE            *stderr;
        sem_t           *exit_sem;
        task_t          *task;
        u32_t            valid;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static char   **new_argument_table      (const char *str, int *argc);
static void     delete_argument_table   (char **argv);
static void     program_startup         (void *argv);
static stdret_t get_program_data        (const char *name, struct _prog_data *prg_data);

/*==============================================================================
  Local object definitions
==============================================================================*/
static const u32_t prog_valid_number   = 0x3B6BF19D;
static const u32_t thread_valid_number = 0x8843D463;

/*==============================================================================
  Exported object definitions
==============================================================================*/
/* standard input */
FILE                    *stdin;

/* standard output */
FILE                    *stdout;

/* standard error */
FILE                    *stderr;

/* global variables */
struct __global_vars__  *global;

/* error number */
int                      _errno;

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
 * @brief Make thread as orphan task
 *
 * @param taskhdl       task handle
 */
//==============================================================================
static void make_RAW_task(task_t *taskhdl)
{
        _task_data_t *task_data = _task_get_data_of(taskhdl);

        task_data->f_mem         = NULL;
        task_data->f_stdin       = NULL;
        task_data->f_stdout      = NULL;
        task_data->f_stderr      = NULL;
        task_data->f_task_object = NULL;
        task_data->f_task_type   = TASK_TYPE_RAW;
}

//==============================================================================
/**
 * @brief Program startup
 *
 * @param *arg          pointer to program's informations
 */
//==============================================================================
static void program_startup(void *arg)
{
        if (arg) {
                prog_t *prog = arg;

                prog->mem = sysm_tskcalloc(1, prog->mem_size);
                if (prog->mem || prog->mem_size == 0) {

                        _task_data_t *task_data = _task_get_data();
                        task_data->f_mem         = prog->mem;
                        task_data->f_cwd         = prog->cwd;
                        task_data->f_errno       = 0;
                        task_data->f_stderr      = prog->stderr;
                        task_data->f_stdin       = prog->stdin;
                        task_data->f_stdout      = prog->stdout;
                        task_data->f_task_object = prog;
                        task_data->f_task_type   = TASK_TYPE_PROCESS;

                        stdin  = prog->stdin;
                        stdout = prog->stdout;
                        stderr = prog->stderr;
                        global = prog->mem;
                        errno  = 0;

                        prog->exit_code = prog->func(prog->argc, prog->argv);

                        vfs_ioctl(stdin , IOCTL_VFS__NON_BLOCKING_RD_MODE);
                        sys_getc(stdin);

                        vfs_ioctl(stdin , IOCTL_VFS__DEFAULT_RD_MODE);
                        vfs_ioctl(stdin , IOCTL_VFS__DEFAULT_WR_MODE);
                        vfs_ioctl(stdout, IOCTL_VFS__DEFAULT_RD_MODE);
                        vfs_ioctl(stdout, IOCTL_VFS__DEFAULT_WR_MODE);
                        vfs_ioctl(stderr, IOCTL_VFS__DEFAULT_RD_MODE);
                        vfs_ioctl(stderr, IOCTL_VFS__DEFAULT_WR_MODE);

                        if (prog->mem) {
                                memset(prog->mem, 0, prog->mem_size);
                                sysm_tskfree(prog->mem);
                        }

                        make_RAW_task(prog->task);
                } else {
                        prog->exit_code = EXIT_FAILURE;
                }

                semaphore_signal(prog->exit_sem);
        }

        task_exit();
}

//==============================================================================
/**
 * @brief Start user thread function
 *
 * @param arg           function argument
 */
//==============================================================================
static void thread_startup(void *arg)
{
        if (arg) {
                thread_t     *thread    = arg;
                _task_data_t *task_data = _task_get_data();

                task_data->f_task_type   = TASK_TYPE_THREAD;
                task_data->f_task_object = thread;
                task_data->f_mem         = thread->mem;
                task_data->f_stdin       = thread->stdin;
                task_data->f_stdout      = thread->stdout;
                task_data->f_stderr      = thread->stderr;

                stdin  = thread->stdin;
                stdout = thread->stdout;
                stderr = thread->stderr;
                global = thread->mem;
                errno  = 0;

                thread->func(thread->arg);

                make_RAW_task(thread->task);
                semaphore_signal(thread->exit_sem);
        }

        task_exit();
}

//==============================================================================
/**
 * @brief Function create new table with argument pointers
 *
 * @param[in]  *str             argument string
 * @param[out] *arg_count       number of argument
 *
 * @return argument table pointer if success, otherwise NULL
 */
//==============================================================================
static char **new_argument_table(const char *str, int *argc)
{
        int     arg_count  = 0;
        char  **arg_table  = NULL;
        list_t *arg_list   = NULL;
        char   *arg_string = NULL;
        bool    first_quos = false;
        bool    first_quod = false;

        if (str == NULL || argc == NULL) {
                errno = EINVAL;
                goto exit_error;
        }

        if ((arg_list = list_new()) == NULL) {
                goto exit_error;
        }

        if (str[0] == '\0') {
                errno = EINVAL;
                goto exit_error;
        }

        const char *arg_start = str;
        while (*arg_start == ' ') {
                arg_start++;
        }

        if (*arg_start == '\'') {
                arg_start++;
                first_quos = true;
        } else if (*arg_start == '"') {
                arg_start++;
                first_quod = true;
        }

        if ((arg_string = sysm_syscalloc(strlen(arg_start) + 1, sizeof(char))) == NULL) {
                goto exit_error;
        }

        strcpy(arg_string, arg_start);
        arg_start = arg_string;

        while (*arg_string != '\0') {
                char *arg_to_add = NULL;

                if (*arg_string == '\'' || first_quos) {
                        if (first_quos) {
                                first_quos = false;
                        } else {
                                ++arg_string;
                        }

                        arg_to_add = arg_string;

                        while (*arg_string != '\0') {
                                if ( *arg_string == '\''
                                   && (  *(arg_string + 1) == ' '
                                      || *(arg_string + 1) == '\0') ) {
                                        break;
                                }

                                arg_string++;
                        }

                        if (*arg_string == '\0') {
                                goto exit_error;
                        }

                } else if (*arg_string == '"' || first_quod) {
                        if (first_quod) {
                                first_quod = false;
                        } else {
                                ++arg_string;
                        }

                        arg_to_add = arg_string;

                        while (*arg_string != '\0') {
                                if ( *arg_string == '"'
                                   && (  *(arg_string + 1) == ' '
                                      || *(arg_string + 1) == '\0') ) {
                                        break;
                                }

                                arg_string++;
                        }

                        if (*arg_string == '\0') {
                                goto exit_error;
                        }

                } else if (*arg_string != ' ') {
                        arg_to_add = arg_string;

                        while (*arg_string != ' ' && *arg_string != '\0') {
                                arg_string++;
                        }
                } else {
                        arg_string++;
                        continue;
                }

                /* add argument to list */
                if (arg_to_add == NULL) {
                        goto exit_error;
                }

                if (list_add_item(arg_list, arg_count++, arg_to_add) < 0) {
                        goto exit_error;
                }

                /* terminate argument */
                if (*arg_string == '\0') {
                        break;
                } else {
                        *arg_string++ = '\0';
                }
        }

        /* add args to table */
        if ((arg_table = sysm_syscalloc(arg_count + 1, sizeof(char*))) == NULL) {
                goto exit_error;
        }

        for (int i = 0; i < arg_count; i++) {
                arg_table[i] = list_get_nitem_data(arg_list, 0);

                if (arg_table[i] == NULL) {
                        goto exit_error;
                }

                list_unlink_nitem_data(arg_list, 0);
                list_rm_nitem(arg_list, 0);
        }

        list_delete(arg_list);

        *argc = arg_count;
        return arg_table;


        /* error occurred - memory/object deallocation */
exit_error:
        if (arg_table) {
                sysm_sysfree(arg_table);
        }

        if (arg_list) {
                i32_t items_in_list = list_get_item_count(arg_list);
                while (items_in_list-- > 0) {
                        list_unlink_nitem_data(arg_list, 0);
                        list_rm_nitem(arg_list, 0);
                }

                list_delete(arg_list);
        }

        if (arg_string) {
                sysm_sysfree((char *)arg_start);
        }

        *argc = 0;
        return NULL;
}

//==============================================================================
/**
 * @brief Function remove argument table
 *
 * @param **argv        pointer to argument table
 */
//==============================================================================
static void delete_argument_table(char **argv)
{
        if (argv == NULL) {
                return;
        }

        if (argv[0]) {
                sysm_sysfree(argv[0]);
        }

        sysm_sysfree(argv);
}

//==============================================================================
/**
 * @brief Function returns pointer to all program data necessary to start program
 *
 * @param [in]  *name           program name
 * @param [out] *pdata          program data
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t get_program_data(const char *name, struct _prog_data *prg_data)
{
        if (!prg_data || !name) {
                errno = EINVAL;
                return STD_RET_ERROR;
        }

        for (int i = 0; i < _prog_table_size; i++) {
                if (strcmp(name, _prog_table[i].program_name) == 0) {
                        *prg_data = _prog_table[i];
                        return STD_RET_OK;
                }
        }

        errno = EINVAL;
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Kill process
 *
 * @param taskhdl       task handle
 * @param status        process kill status
 *
 * @return 0 on success, otherwise other value
 */
//==============================================================================
static int process_kill(task_t *taskhdl, int status)
{
        if (taskhdl) {
                switch (_task_get_data_of(taskhdl)->f_task_type) {
                case TASK_TYPE_RAW:
                        sysm_lock_access();
                        _task_delete(taskhdl);
                        sysm_unlock_access();
                        break;
                case TASK_TYPE_PROCESS: {
                        prog_t *prog    = _task_get_data_of(taskhdl)->f_task_object;
                        prog->exit_code = status;
                        _program_kill(prog);
                        break;
                }
                case TASK_TYPE_THREAD:
                        _thread_cancel(_task_get_data_of(taskhdl)->f_task_object);
                        break;
                default:
                        return -ESRCH;
                }

                return 0;
        }

        return -EINVAL;
}

//==============================================================================
/**
 * @brief Function validate program object
 *
 * Errno: EINVAL
 *
 * @param prog          program object
 *
 * @return true if object is valid, otherwise false
 */
//==============================================================================
static bool prog_is_valid(prog_t *prog)
{
        if (prog) {
                if (prog->valid == prog_valid_number) {
                        return true;
                }
        }

        errno = EINVAL;
        return false;
}

//==============================================================================
/**
 * @brief Function validate thread object
 *
 * Errno: EINVAL
 *
 * @param thread        thread object
 *
 * @return true if object is valid, otherwise false
 */
//==============================================================================
static bool thread_is_valid(thread_t *thread)
{
        if (thread) {
                if (thread->valid == thread_valid_number) {
                        return true;
                }
        }

        errno = EINVAL;
        return false;
}

//==============================================================================
/**
 * @brief Function start new program by name
 *
 * Errno: EINVAL, ENOMEM, ENOENT
 *
 * @param cmd           program name and argument list
 * @param stin          standard input file
 * @param stout         standard output file
 * @param sterr         standard error file
 *
 * @return NULL if error, otherwise program handle
 */
//==============================================================================
prog_t *_program_new(const char *cmd, const char *cwd, FILE *stin, FILE *stout, FILE *sterr)
{
        if (!cmd || !cwd) {
                errno = EINVAL;
                return NULL;
        }

        prog_t *prog = sysm_tskcalloc(1, sizeof(prog_t));
        if (prog) {

                prog->argv = new_argument_table(cmd, &prog->argc);
                if (prog->argv) {

                        struct _prog_data prog_data;
                        if (get_program_data(prog->argv[0], &prog_data) == STD_RET_OK) {

                                prog->exit_sem = semaphore_new(1, 1);
                                if (prog->exit_sem) {

                                        prog->mem      = NULL;
                                        prog->cwd      = cwd;
                                        prog->stdin    = stin;
                                        prog->stdout   = stout;
                                        prog->stderr   = sterr;
                                        prog->func     = *prog_data.main_function;
                                        prog->mem_size = *prog_data.globals_size;
                                        prog->task     = task_new(program_startup,
                                                                  prog_data.program_name,
                                                                  prog_data.stack_depth,
                                                                  prog);

                                        if (prog->task) {
                                                prog->valid = prog_valid_number;
                                                return prog;
                                        }
                                }
                        } else {
                                errno = ENOENT;
                        }
                }
        }

        if (prog->argv) {
                delete_argument_table(prog->argv);
        }

        if (prog->exit_sem) {
                semaphore_delete(prog->exit_sem);
        }

        if (prog) {
                sysm_tskfree(prog);
                prog = NULL;
        }

        return prog;
}

//==============================================================================
/**
 * @brief Function delete running program
 *
 * @param prog                  program object
 *
 * @return 0 on success, otherwise other value
 */
//==============================================================================
int _program_delete(prog_t *prog)
{
        if (prog_is_valid(prog)) {
                if (semaphore_wait(prog->exit_sem, 0)) {
                        semaphore_delete(prog->exit_sem);
                        delete_argument_table(prog->argv);
                        prog->valid = 0;
                        sysm_tskfree(prog);
                        return 0;
                } else {
                        errno = EAGAIN;
                        return -EAGAIN;
                }
        }

        return -EINVAL;
}

//==============================================================================
/**
 * @brief Kill started program
 *
 * @param prog                  program object
 *
 * @return 0 if success, otherwise other value
 */
//==============================================================================
int _program_kill(prog_t *prog)
{
        if (prog_is_valid(prog)) {
                if (semaphore_wait(prog->exit_sem, 0)) {
                        semaphore_signal(prog->exit_sem);
                        return 0;
                } else {
                        sysm_lock_access();

                        if (prog->task != task_get_handle()) {
                                task_suspend(prog->task);
                        }

                        if (prog->mem) {
                                sysm_tskfree_as(prog->task, prog->mem);
                                prog->mem = NULL;
                        }

                        make_RAW_task(prog->task);
                        semaphore_signal(prog->exit_sem);
                        _task_delete(prog->task);

                        sysm_unlock_access();
                        return 0;
                }
        }

        errno = EINVAL;
        return -EINVAL;
}

//==============================================================================
/**
 * @brief Wait for program close
 *
 * @param prog                  program object
 * @param timeout               wait timeout in ms
 *
 * @return 0 if closed, otherwise other value
 */
//==============================================================================
int _program_wait_for_close(prog_t *prog, const uint timeout)
{
        if (prog_is_valid(prog)) {
                if (semaphore_wait(prog->exit_sem, timeout)) {
                        semaphore_signal(prog->exit_sem);
                        return 0;
                } else {
                        errno = ETIME;
                        return -ETIME;
                }
        }

        return -EINVAL;
}

//==============================================================================
/**
 * @brief Check if program is closed
 *
 * @param prog                  program object
 *
 * @return true if program closed, otherwise false
 */
//==============================================================================
bool _program_is_closed(prog_t *prog)
{
        if (prog_is_valid(prog)) {
                if (semaphore_wait(prog->exit_sem, 0)) {
                        semaphore_signal(prog->exit_sem);
                        return true;
                }
        }

        return false;
}

//==============================================================================
/**
 * @brief Function delete any kind of task
 *
 * @param taskhdl       task handle
 */
//==============================================================================
void _task_kill(task_t *taskhdl)
{
        process_kill(taskhdl, -1);
}

//==============================================================================
/**
 * @brief Function close program immediately and set exit code
 *
 * @param status        exit value
 */
//==============================================================================
void _exit(int status)
{
        process_kill(task_get_handle(), status);

        /* wait to kill program */
        for (;;);
}

//==============================================================================
/**
 * @brief Function close program with error code
 */
//==============================================================================
void _abort(void)
{
        process_kill(task_get_handle(), -1);

        /* wait to kill program */
        for (;;);
}

//==============================================================================
/**
 * @brief Function start program in shell
 *
 * @param command       command string
 *
 * @return program status
 */
//==============================================================================
int _system(const char *command)
{
        if (!command)
                return 1;

        _task_data_t *task_data = _task_get_data();

        prog_t *prog = _program_new(command,
                                    task_data->f_cwd,
                                    task_data->f_stdin,
                                    task_data->f_stdout,
                                    task_data->f_stderr);
        if (prog) {
                _program_wait_for_close(prog, MAX_DELAY_MS);
                int status = prog->exit_code;
                _program_delete(prog);
                return status;
        } else {
                if (errno) {
                        return -errno;
                }
        }

        return EXIT_FAILURE;
}

//==============================================================================
/**
 * @brief Create new thread of configured task (program or RAW task)
 *
 * @param func          thread function
 * @param stack_depth   stack depth
 * @param arg           thread argument
 *
 * @return thread object if success, otherwise NULL
 */
//==============================================================================
thread_t *_thread_new(void (*func)(void*), const int stack_depth, void *arg)
{
        if (!func || !stack_depth) {
                errno = EINVAL;
                return NULL;
        }

        thread_t *thread = sysm_tskmalloc(sizeof(thread_t));
        sem_t    *sem    = semaphore_new(1, 1);
        if (thread && sem) {
                _task_data_t *task_data = _task_get_data();

                thread->arg      = arg;
                thread->exit_sem = sem;
                thread->mem      = task_data->f_mem;
                thread->stdin    = task_data->f_stdin;
                thread->stdout   = task_data->f_stdout;
                thread->stderr   = task_data->f_stderr;
                thread->func     = func;
                thread->task     = task_new(thread_startup, task_get_name(), stack_depth, thread);

                if (thread->task) {
                        thread->valid = thread_valid_number;
                        return thread;
                }
        }

        if (sem) {
                semaphore_delete(sem);
        }

        if (thread) {
                sysm_tskfree(thread);
                thread = NULL;
        }

        return thread;
}

//==============================================================================
/**
 * @brief Function wait for thread exit
 *
 * @param thread        thread object
 *
 * @return 0 on success, otherwise 1
 */
//==============================================================================
int _thread_join(thread_t *thread)
{
        if (thread_is_valid(thread)) {
                if (semaphore_wait(thread->exit_sem, MAX_DELAY_MS)) {
                        semaphore_signal(thread->exit_sem);
                        return 0;
                } else {
                        errno = ETIME;
                        return -ETIME;
                }
        }

        return 1;
}

//==============================================================================
/**
 * @brief Cancel current working thread
 *
 * @return 0 on success, otherwise other
 */
//==============================================================================
int _thread_cancel(thread_t *thread)
{
        if (thread_is_valid(thread)) {
                if (semaphore_wait(thread->exit_sem, 0)) {
                        semaphore_signal(thread->exit_sem);
                        return 0;
                } else {
                        sysm_lock_access();

                        if (thread->task != task_get_handle()) {
                                task_suspend(thread->task);
                        }

                        make_RAW_task(thread->task);
                        semaphore_signal(thread->exit_sem);
                        _task_delete(thread->task);

                        sysm_unlock_access();
                        return 0;
                }
        }

        return 1;
}

//==============================================================================
/**
 * @brief Check if thread is finished
 *
 * @param thread        thread object
 *
 * @return true if finished, otherwise false
 */
//==============================================================================
bool _thread_is_finished(thread_t *thread)
{
        if (thread_is_valid(thread)) {
                if (semaphore_wait(thread->exit_sem, 0)) {
                        semaphore_signal(thread->exit_sem);
                        return true;
                }
        }
        return false;
}

//==============================================================================
/**
 * @brief Delete thread object
 *
 * @param thread        thread object
 *
 * @return 0 on success, on error 1
 */
//==============================================================================
int _thread_delete(thread_t *thread)
{
        if (thread_is_valid(thread)) {
                if (semaphore_wait(thread->exit_sem, 0)) {
                        semaphore_delete(thread->exit_sem);
                        thread->valid = 0;
                        sysm_tskfree(thread);
                        return 0;
                } else {
                        errno = EAGAIN;
                        return 1;
                }
        }

        return 1;
}

//==============================================================================
/**
 * @brief Function copy task context to standard variables (stdin, stdout, stderr,
 *        global, errno)
 */
//==============================================================================
void _copy_task_context_to_standard_variables(void)
{
        _task_data_t *task_data = _task_get_data();
        if (task_data) {
                stdin  = task_data->f_stdin;
                stdout = task_data->f_stdout;
                stderr = task_data->f_stderr;
                global = task_data->f_mem;
                errno  = task_data->f_errno;
        } else {
                stdin  = NULL;
                stdout = NULL;
                stderr = NULL;
                global = NULL;
                errno  = 0;
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
        _task_data_t *task_data = _task_get_data();
        if (task_data) {
                task_data->f_stdin  = stdin;
                task_data->f_stdout = stdout;
                task_data->f_stderr = stderr;
                task_data->f_errno  = errno;
                task_data->f_mem    = global;
        }
}

/*==============================================================================
  End of file
==============================================================================*/
