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
#include "core/vfs.h"
#include "core/progman.h"
#include "core/sysmoni.h"
#include "core/llist.h"
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
        struct prog     *this;
        char            **argv;
        int              argc;
        uint             mem_size;
        int              exit_code;
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
        struct thread   *this;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static char **new_argument_table      (const char *str, int *argc);
static void   delete_argument_table   (int argc, char **argv);
static void   program_startup         (void *argv);
static int    get_program_data        (const char *name, struct _prog_data *prg_data);
static void   restore_stdio_defaults  (task_t *task);

/*==============================================================================
  Local object definitions
==============================================================================*/
static const uint  mutex_wait_attempts = 10;

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

                prog->mem = _sysm_tskcalloc(1, prog->mem_size);
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

                        restore_stdio_defaults(prog->task);

                        if (prog->mem) {
                                memset(prog->mem, 0, prog->mem_size);
                                _sysm_tskfree(prog->mem);
                        }

                        make_RAW_task(prog->task);
                } else {
                        prog->exit_code = EXIT_FAILURE;
                }

                _semaphore_signal(prog->exit_sem);
        }

        _task_exit();
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
                _semaphore_signal(thread->exit_sem);
        }

        _task_exit();
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
        char **argv = NULL;

        if (str && argc && str[0] != '\0') {
                llist_t *args = _llist_new(_sysm_sysmalloc, _sysm_sysfree, NULL, NULL);

                if (args) {
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
                                char *arg = _sysm_sysmalloc(str_len + 1);
                                if (arg) {
                                        strncpy(arg, start, str_len);
                                        arg[str_len] = '\0';

                                        if (_llist_push_back(args, arg) == NULL) {
                                                _llist_delete(args);
                                                errno = ENOMEM;
                                                return NULL;
                                        }
                                } else {
                                        _llist_delete(args);
                                        errno = ENOMEM;
                                        return NULL;
                                }

                                // next token
                                str = end;
                        }

                        // create table with arguments
                        int no_of_args = _llist_size(args);
                        *argc = no_of_args;

                        argv = _sysm_sysmalloc((no_of_args + 1) * sizeof(char*));
                        if (argv) {
                                for (int i = 0; i < no_of_args; i++) {
                                        argv[i] = _llist_take_front(args);
                                }

                                argv[no_of_args] = NULL;
                        }

                        _llist_delete(args);
                }
        }

        return argv;
}

//==============================================================================
/**
 * @brief Function remove argument table
 *
 * @param argc          number of arguments
 * @param argv          pointer to argument table
 */
//==============================================================================
static void delete_argument_table(int argc, char **argv)
{
        if (argv) {
                for (int i = 0; i < argc; i++) {
                        _sysm_sysfree(argv[i]);
                }

                _sysm_sysfree(argv);
        }
}

//==============================================================================
/**
 * @brief Function returns pointer to all program data necessary to start program
 *
 * @param [in]  *name           program name
 * @param [out] *pdata          program data
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int get_program_data(const char *name, struct _prog_data *prg_data)
{
        if (prg_data && name) {
                for (int i = 0; i < _prog_table_size; i++) {
                        if (strcmp(name, _prog_table[i].program_name) == 0) {
                                *prg_data = _prog_table[i];
                                return ESUCC;
                        }
                }
        }

        return EINVAL;
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
                default:
                case TASK_TYPE_RAW:
                        if (taskhdl == _task_get_handle()) {
                                _task_delete(taskhdl);
                        } else {
                                _sysm_lock_access();
                                _task_delete(taskhdl);
                                _sysm_unlock_access();
                        }
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
        if (prog && prog->this == prog) {
                return true;
        } else {
                errno = EINVAL;
                return false;
        }
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
        if (thread && thread->this == thread) {
                return true;
        } else {
                errno = EINVAL;
                return false;
        }
}

//==============================================================================
/**
 * @brief Do some attempts to check if task exit from mutex section
 *
 * @param task          task to examine
 * @param attempts      number of attempts
 *
 * @return None
 */
//==============================================================================
static void wait_for_end_of_mutex_section(task_t *task, size_t attempts)
{
        while (attempts && _task_get_data_of(task)->f_mutex_section > 0) {
                sleep_ms(1);    // 1 tick delay
                attempts--;
        }
}

//==============================================================================
/**
 * @brief Restores default configuration of STDIO files
 *
 * @param task          task
 *
 * @return None
 */
//==============================================================================
static void restore_stdio_defaults(task_t *task)
{
        struct _task_data *data = _task_get_data_of(task);

        _vfs_ioctl(data->f_stdout, IOCTL_TTY__ECHO_ON);

        _vfs_ioctl(data->f_stdin , IOCTL_VFS__NON_BLOCKING_RD_MODE);
        _getc(data->f_stdin);

        _vfs_ioctl(data->f_stdin , IOCTL_VFS__DEFAULT_RD_MODE);
        _vfs_ioctl(data->f_stdin , IOCTL_VFS__DEFAULT_WR_MODE);
        _vfs_ioctl(data->f_stdout, IOCTL_VFS__DEFAULT_RD_MODE);
        _vfs_ioctl(data->f_stdout, IOCTL_VFS__DEFAULT_WR_MODE);
        _vfs_ioctl(data->f_stderr, IOCTL_VFS__DEFAULT_RD_MODE);
        _vfs_ioctl(data->f_stderr, IOCTL_VFS__DEFAULT_WR_MODE);
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

        prog_t *prog = _sysm_tskcalloc(1, sizeof(prog_t));
        if (prog) {

                prog->argv = new_argument_table(cmd, &prog->argc);
                if (prog->argv) {

                        struct _prog_data prog_data;
                        if (get_program_data(prog->argv[0], &prog_data) == ESUCC) {

                                prog->exit_sem = _semaphore_new(1, 0);
                                if (prog->exit_sem) {

                                        prog->mem      = NULL;
                                        prog->cwd      = cwd;
                                        prog->stdin    = stin;
                                        prog->stdout   = stout;
                                        prog->stderr   = sterr;
                                        prog->func     = *prog_data.main_function;
                                        prog->mem_size = *prog_data.globals_size;
                                        prog->this     = prog;
                                        prog->task     = _task_new(program_startup,
                                                                   prog_data.program_name,
                                                                   *prog_data.stack_depth,
                                                                   prog);

                                        if (prog->task) {
                                                return prog;
                                        } else {
                                                prog->this  = NULL;
                                        }

                                        _semaphore_delete(prog->exit_sem);
                                }
                        } else {
                                errno = ENOENT;
                        }

                        delete_argument_table(prog->argc, prog->argv);
                }

                _sysm_tskfree(prog);
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
                if (_semaphore_wait(prog->exit_sem, 0)) {
                        _semaphore_delete(prog->exit_sem);
                        delete_argument_table(prog->argc, prog->argv);
                        prog->this = NULL;
                        _sysm_tskfree(prog);
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
                if (_semaphore_wait(prog->exit_sem, 0)) {
                        _semaphore_signal(prog->exit_sem);
                        return 0;
                } else {
                        _sysm_lock_access();

                        if (prog->task != _task_get_handle()) {
                                _task_get_data_of(prog->task)->f_task_kill = true;
                                wait_for_end_of_mutex_section(prog->task, mutex_wait_attempts);
                                _task_suspend(prog->task);
                        }

                        if (prog->mem) {
                                _sysm_tskfree_as(prog->task, prog->mem);
                                prog->mem = NULL;
                        }

                        restore_stdio_defaults(prog->task);
                        make_RAW_task(prog->task);
                        _semaphore_signal(prog->exit_sem);
                        _sysm_unlock_access();
                        _task_delete(prog->task);
                        return 0;
                }
        }

        errno = EINVAL;
        return -EINVAL;
}

//==============================================================================
/**
 * @brief  Return exit code of program
 *
 * @param prog                  program object
 *
 * @return On success exit code is returned, otherwise -EINVAL
 */
//==============================================================================
int _program_get_exit_code(prog_t *prog)
{
        if (prog_is_valid(prog)) {
                return prog->exit_code;
        } else {
                errno = EINVAL;
                return -EINVAL;
        }
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
                if (_semaphore_wait(prog->exit_sem, timeout)) {
                        _semaphore_signal(prog->exit_sem);
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
                if (_semaphore_wait(prog->exit_sem, 0)) {
                        _semaphore_signal(prog->exit_sem);
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
        process_kill(_task_get_handle(), status);

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
        _fprintf(stdout, "Aborted\n");

        process_kill(_task_get_handle(), -1);

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

        thread_t *thread = _sysm_tskmalloc(sizeof(thread_t));
        sem_t    *sem    = _semaphore_new(1, 0);
        if (thread && sem) {
                _task_data_t *task_data = _task_get_data();

                thread->arg      = arg;
                thread->exit_sem = sem;
                thread->mem      = task_data->f_mem;
                thread->stdin    = task_data->f_stdin;
                thread->stdout   = task_data->f_stdout;
                thread->stderr   = task_data->f_stderr;
                thread->func     = func;
                thread->this     = thread;
                thread->task     = _task_new(thread_startup, _task_get_name(), stack_depth, thread);

                if (thread->task) {
                        return thread;
                } else {
                        thread->this  = NULL;
                }
        }

        if (sem) {
                _semaphore_delete(sem);
        }

        if (thread) {
                _sysm_tskfree(thread);
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
                if (_semaphore_wait(thread->exit_sem, MAX_DELAY_MS)) {
                        _semaphore_signal(thread->exit_sem);
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
                if (_semaphore_wait(thread->exit_sem, 0)) {
                        _semaphore_signal(thread->exit_sem);
                        return 0;
                } else {
                        _sysm_lock_access();

                        if (thread->task != _task_get_handle()) {
                                _task_get_data_of(thread->task)->f_task_kill = true;
                                wait_for_end_of_mutex_section(thread->task, mutex_wait_attempts);
                                _task_suspend(thread->task);
                        }

                        restore_stdio_defaults(thread->task);
                        make_RAW_task(thread->task);
                        _semaphore_signal(thread->exit_sem);
                        _sysm_unlock_access();
                        _task_delete(thread->task);
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
                if (_semaphore_wait(thread->exit_sem, 0)) {
                        _semaphore_signal(thread->exit_sem);
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
                if (_semaphore_wait(thread->exit_sem, 0)) {
                        _semaphore_delete(thread->exit_sem);
                        thread->this = NULL;
                        _sysm_tskfree(thread);
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
