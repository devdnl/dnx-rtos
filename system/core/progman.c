/*=========================================================================*//**
@file    progman.c

@author  Daniel Zorychta

@brief   This file support programs layer

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "progman.h"
#include "regprg.h"
#include "oswrap.h"
#include "taskmoni.h"
#include "dlist.h"
#include <string.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#undef stdin
#undef stdout

#define m_calloc(nmemb, msize)          tskm_calloc(nmemb, msize)
#define m_malloc(size)                  tskm_malloc(size)
#define m_free(mem)                     tskm_free(mem)
#define calloc(nmemb, msize)            memman_calloc(nmemb, msize)
#define malloc(size)                    memman_malloc(size)
#define free(mem)                       memman_free(mem)

#define MTX_BTIME_FOR_PLIST             1
#define PROGRAM_DEFAULT_PRIORITY        0
#define PROCESS_DEFAULT_PRIORITY        0
#define SEM_PROGRAM_BLOCK_TIME          250

#define break_if_out_of_ram(ptr)        while ((u32_t)ptr < 0x20000000 || (u32_t)ptr > 0x2000FFFF);

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
struct data_of_running_program {
        FILE_t *stdin;                          /* stdin file                   */
        FILE_t *stdout;                         /* stdout file                  */
        char   *cwd;                            /* current working path         */
        void   *global_vars;                    /* address to global variables  */
        int   (*main_function)(int, char**);    /* program's main function      */
        char   *name;                           /* program's name               */
        char   *args;                           /* not formated argument string */
        char  **argv;                           /* table with arguments         */
        int    *exit_code;                      /* program's exit code          */
        task_t *parent_task;                    /* program's parent task        */
        int     argc;                           /* argument table               */
        uint    globals_size;                   /* size of global variables     */
        enum    prg_status *status;             /* pointer to task status       */
};

struct program_cache {
        struct {
                task_t *taskhdl;
                void   *address;
        } globals;

        struct {
                task_t *taskhdl;
                FILE_t *file;
        } stdin;

        struct {
                task_t *taskhdl;
                FILE_t *file;
        } stdout;
};

struct process {
        list_t  *list;
        mutex_t *list_mtx;
        uint     read_counter;
        pid_t    pid_counter;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static i32_t add_process_to_list(task_t *task);
static void  delete_process_from_list(pid_t pid);
static void  lock_write_to_process_list(void);
static void  unlock_write_to_process_list(void);

static void     set_status(enum prg_status *status_ptr, enum prg_status status);
static char   **new_argument_table(char *arg, const char *name, int *argc);
static void     delete_argument_table(char **argv, int argc);
static void     task_program_startup(void *argv);

/*==============================================================================
  Local object definitions
==============================================================================*/
static struct process process;
static list_t  *program_list;
static struct program_cache cache;


/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Function initialize program manager
 *
 * @retval STD_RET_OK           manager variables initialized successfully
 * @retval STD_RET_ERROR        variables not initialized
 */
//==============================================================================
stdRet_t prgm_init(void)
{
        process.list     = new_list();
        process.list_mtx = new_mutex();
        program_list     = new_list();

        if (!process.list || !process.list_mtx|| !program_list) {
                return STD_RET_ERROR;
        }

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function start new process
 *
 * @param[in ]  func          process code
 * @param[in ] *name          process name
 * @param[in ]  stack         stack deep
 * @param[in ] *argv          argument pointer
 *
 * @return PID number, otherwise -1 if error
 */
//==============================================================================
pid_t prgm_new_process(void (*func)(void*), char *name, uint stack, void *argv)
{
        task_t *task;
        pid_t   pid  = -1;

        task = new_task(func, name, stack, argv, PROCESS_DEFAULT_PRIORITY);
        if (task) {
                if (add_process_to_list(task) >= 0) {
                        pid = process.pid_counter++;
                } else {
                        delete_task(task);
                }
        }

        return pid;
}

//==============================================================================
/**
 * @brief Function returns task handle based on PID
 *
 * @param pid           process PID
 *
 * @return task handle object, otherwise NULL if error
 */
//==============================================================================
task_t *prgm_get_process_task_handle(pid_t pid)
{
        lock_write_to_process_list();
        task_t *task = list_get_iditem_data(process.list, pid);
        unlock_write_to_process_list();

        return task;
}

//==============================================================================
/**
 * @brief Function returns current process PID
 *
 * @return PID, otherwise -1 if error
 */
//==============================================================================
pid_t prgm_get_pid(void)
{
        task_t *current_task = get_task_handle();
        task_t *task;
        int     total_process;
        pid_t   pid = -1;

        lock_write_to_process_list();

        total_process = list_get_item_count(process.list);

        for (int item = 0; item < total_process; item++) {
                task = list_get_nitem_data(process.list, item);

                if (task == current_task) {
                        list_get_nitem_ID(process.list, item, (u32_t*)&pid);
                        break;
                }
        }

        unlock_write_to_process_list();

        return pid;
}

//==============================================================================
/**
 * @brief Function terminate process
 */
//==============================================================================
void prgm_process_terminate(void)
{
        delete_process_from_list(prgm_get_pid());
        delete_task(THIS_TASK);
}










//==============================================================================
/**
 * @brief Function start new program by name
 *
 * @param *name         program name
 * @param *args         program argument string
 * @param *fstdin       stdin file
 * @param *fstdout      stdout file
 * @oaram *cwd          current working path
 *
 * @return NULL if error, otherwise task handle
 */
//==============================================================================
task_t *prgm_new_program(char *name, char *args, char *cwd, FILE_t *fstdin,
                         FILE_t *fstdout, enum prg_status *status, int *exit_code)
{
        struct data_of_running_program *progdata = NULL;
        struct regprg_pdata             regpdata;
        task_t                         *taskhdl  = NULL;

        if (!name || !args || !cwd) {
                return NULL;
        }

        if (regprg_get_program_data(name, &regpdata) != STD_RET_OK) {
                return NULL;
        }

        if ((progdata = calloc(1, sizeof(struct data_of_running_program))) == NULL) {
                goto error;
        }

        progdata->args          = args;
        progdata->globals_size  = *regpdata.globals_size;
        progdata->global_vars   = NULL;
        progdata->main_function = regpdata.main_function;
        progdata->name          = regpdata.program_name;
        progdata->cwd           = cwd;
        progdata->stdin         = fstdin;
        progdata->stdout        = fstdout;
        progdata->status        = status;
        progdata->exit_code     = exit_code;
        progdata->parent_task   = get_task_handle();

        taskhdl = new_task(task_program_startup, regpdata.program_name, *regpdata.stack_deep,
                           progdata, PROGRAM_DEFAULT_PRIORITY);

        if (taskhdl != NULL) {

                suspend_all_tasks();

                i32_t item = list_add_item(program_list,
                                           (u32_t)taskhdl, progdata);
                resume_all_tasks();

                if (item < 0) {
                        goto error;
                }

                if (status) {
                        *status = PROGRAM_RUNNING;
                }

                /* DNLFIXME: wziac pod uwage ze task moze roznie wystartowac a rodzic moze byc przelaczony */

                resume_task(taskhdl);

                return taskhdl;
        }

        /* an error occurred */
        error:
        if (taskhdl) {
                delete_task(taskhdl);
        }

        if (progdata) {
                free(progdata);
        }

        return NULL;
}

//==============================================================================
/**
 * @brief Function wait to program end
 *
 * @param *taskhdl              task handle
 * @param *status               program status
 */
//==============================================================================
void prgm_wait_for_program_end(task_t *taskhdl, enum prg_status *status)
{
        if (!taskhdl || !status) {
                return;
        }

        while (*status == PROGRAM_RUNNING) {
                sleep(1);
        }
}

//==============================================================================
/**
 * @brief Function returns stdin file
 *
 * @return stdin file or NULL if doesn't exist
 */
//==============================================================================
FILE_t *prgm_get_program_stdin(void)
{
        struct data_of_running_program *pdata;
        task_t *taskhdl = get_task_handle();
        FILE_t *fstdin  = NULL;

        suspend_all_tasks();

        if (cache.stdin.taskhdl == taskhdl) {
                fstdin = cache.stdin.file;
        } else {
                if ((pdata = list_get_iditem_data(program_list,
                                                  (u32_t)taskhdl))) {

                        cache.stdin.taskhdl = taskhdl;
                        cache.stdin.file    = pdata->stdin;
                        fstdin = pdata->stdin;
                }
        }






        if ((pdata = list_get_iditem_data(program_list, (u32_t)taskhdl))) {
                while (fstdin != pdata->stdin);
        }
        break_if_out_of_ram(fstdin); /* DNLTEST NULL assert */




        resume_all_tasks();
        return fstdin;
}

//==============================================================================
/**
 * @brief Function returns stdout file
 *
 * @param taskhdl       task handle
 *
 * @return stdout file or NULL if doesn't exist
 */
//==============================================================================
FILE_t *prgm_get_program_stdout(void)
{
        struct data_of_running_program *pdata;
        task_t *taskhdl = get_task_handle();
        FILE_t *fstdout = NULL;

        suspend_all_tasks();

        if (cache.stdout.taskhdl == taskhdl) {
                fstdout = cache.stdout.file;
        } else {
                if ((pdata = list_get_iditem_data(program_list,
                                                  (u32_t)taskhdl))) {

                        cache.stdout.taskhdl = taskhdl;
                        cache.stdout.file    = pdata->stdout;
                        fstdout = pdata->stdout;
                }
        }




        if ((pdata = list_get_iditem_data(program_list, (u32_t)taskhdl))) {
                while (fstdout != pdata->stdout);
        }
        break_if_out_of_ram(fstdout); /* DNLTEST NULL assert */





        resume_all_tasks();
        return fstdout;
}

//==============================================================================
/**
 * @brief Function returns global variable address
 *
 * @param taskhdl       task handle
 *
 * @return pointer to globals or NULL
 */
//==============================================================================
void *prgm_get_program_globals(void)
{
        struct data_of_running_program *pdata;
        task_t *taskhdl = get_task_handle();
        void   *globals = NULL;

        suspend_all_tasks();

        if (cache.globals.taskhdl == taskhdl) {
                globals = cache.globals.address;
        } else {
                if ((pdata = list_get_iditem_data(program_list,
                                                  (u32_t)taskhdl))) {

                        cache.globals.taskhdl = taskhdl;
                        cache.globals.address = pdata->global_vars;
                        globals = pdata->global_vars;
                }
        }





        if ((pdata = list_get_iditem_data(program_list, (u32_t)taskhdl))) {
                while (globals != pdata->global_vars);
        }
        break_if_out_of_ram(globals); /* DNLTEST NULL assert */





        resume_all_tasks();
        return globals;
}

//==============================================================================
/**
 * @brief Function returns current working path
 *
 * @param taskhdl       task handle
 *
 * @return current working path pointer or NULL if error
 */
//==============================================================================
ch_t *prgm_get_program_cwd(void)
{
        struct data_of_running_program *pdata;
        ch_t *cwd = NULL;

        suspend_all_tasks();

        if ((pdata = list_get_iditem_data(program_list,
                                          (u32_t)get_task_handle()))) {

                cwd = pdata->cwd;
        }



        break_if_out_of_ram(cwd); /* DNLTEST NULL assert */



        resume_all_tasks();
        return cwd;
}

//==============================================================================
/**
 * @brief Function adds process to list
 *
 * @param *task         task handle
 *
 * @return list item number
 */
//==============================================================================
static i32_t add_process_to_list(task_t *task)
{
        i32_t item = -1;

        if (task == NULL) {
                return -1;
        }

        while (TRUE) {
                if (mutex_lock(process.list_mtx, 1000) == MUTEX_LOCKED) {
                        if (process.read_counter == 0) {
                                item = list_add_item(process.list,
                                                     process.pid_counter,
                                                     task);

                                mutex_unlock(process.list_mtx);
                                break;
                        }

                        mutex_unlock(process.list_mtx);
                }
        }

        return item;
}

//==============================================================================
/**
 * @brief Function adds process to list
 *
 * @param pid           PID
 *
 * @return list item number
 */
//==============================================================================
static void delete_process_from_list(pid_t pid)
{
        while (TRUE) {
                if (mutex_lock(process.list_mtx, 1000) == MUTEX_LOCKED) {
                        if (process.read_counter == 0) {
                                list_unlink_iditem_data(process.list, (u32_t)pid);
                                list_rm_iditem(process.list, (u32_t)pid);
                                mutex_unlock(process.list_mtx);
                                break;
                        }

                        mutex_unlock(process.list_mtx);
                }
        }
}

//==============================================================================
/**
 * @brief Function increase read counter of the process list
 *
 * @retval STD_RET_OK           counter increased
 * @retval STD_RET_ERROR        no operation
 */
//==============================================================================
static void lock_write_to_process_list(void)
{
        while (mutex_lock(process.list_mtx, 1000) != MUTEX_LOCKED);
        process.read_counter++;
        mutex_unlock(process.list_mtx);
}

//==============================================================================
/**
 * @brief Function decrease read counter of the process list
 *
 * @retval STD_RET_OK           counter increased
 * @retval STD_RET_ERROR        no operation
 */
//==============================================================================
static void unlock_write_to_process_list(void)
{
        while (mutex_lock(process.list_mtx, 1000) != MUTEX_LOCKED);
        process.read_counter--;
        mutex_unlock(process.list_mtx);
}





//==============================================================================
/**
 * @brief Program startup
 *
 * @param *argv         pointer to program's informations
 */
//==============================================================================
static void task_program_startup(void *argv)
{
        struct data_of_running_program *progdata = argv;
        int exit_code = STD_RET_UNKNOWN;

        /* suspend this task to finalize parent function */
//        suspend_this_task();

        if (progdata->globals_size) {
                progdata->global_vars = m_calloc(1, progdata->globals_size);
                if (progdata == NULL) {
                        set_status(progdata->status, PROGRAM_NOT_ENOUGH_FREE_MEMORY);
                        goto task_exit;
                }
        }

        if ((progdata->argv = new_argument_table(progdata->args, progdata->name,
                                                 &progdata->argc)) == NULL) {

                set_status(progdata->status, PROGRAM_ARGUMENTS_PARSE_ERROR);
                goto task_exit;
        }

        exit_code = progdata->main_function(progdata->argc, progdata->argv);
        set_status(progdata->status, PROGRAM_ENDED);

        task_exit:
        if (progdata->parent_task) {
                suspend_task(progdata->parent_task);
                resume_task(progdata->parent_task);
        }

        if (progdata->exit_code) {
                *progdata->exit_code = exit_code;
        }

        if (progdata->global_vars) {
                m_free(progdata->global_vars);
        }

        if (progdata->argv) {
                delete_argument_table(progdata->argv, progdata->argc);
        }

        suspend_all_tasks();
        list_rm_iditem(program_list, (u32_t)get_task_handle());
        resume_all_tasks();

        cache.globals.taskhdl = NULL;
        cache.stdin.taskhdl   = NULL;
        cache.stdout.taskhdl  = NULL;

        terminate_task();
}

//==============================================================================
/**
 * @brief Function set program status
 *
 * @param *status_ptr           pointer to status
 * @param  status               status
 *
 * @retval STD_RET_OK           manager variables initialized successfully
 * @retval STD_RET_ERROR        variables not initialized
 */
//==============================================================================
static void set_status(enum prg_status *status_ptr, enum prg_status status)
{
        if (status_ptr) {
                *status_ptr = status;
        }
}

//==============================================================================
/**
 * @brief Function create new table with argument pointers
 *
 * @param[in]  *arg             argument string
 * @param[in]  *name            program name (argument argv[0])
 * @param[out] *arg_count       number of argument
 *
 * @return argument table pointer if success, otherwise NULL
 */
//==============================================================================
static char **new_argument_table(char *arg, const ch_t *name, int *argc)
{
        int     arg_count  = 0;
        char  **arg_table  = NULL;
        list_t *arg_list   = NULL;
        char   *arg_string = NULL;

        if (arg == NULL || name == NULL || argc == NULL) {
                goto exit_error;
        }

        if ((arg_list = new_list()) == NULL) {
                goto exit_error;
        }

        if (list_add_item(arg_list, ++arg_count, (ch_t*)name) < 0) {
                goto exit_error;
        }

        if (arg[0] == '\0') {
                goto add_args_to_table;
        }

        if ((arg_string = m_calloc(strlen(arg) + 1, sizeof(ch_t))) == NULL) {
                goto exit_error;
        }

        strcpy(arg_string, arg);

        while (*arg_string != '\0') {
                ch_t *arg_to_add = NULL;

                if (*arg_string == '\'') {
                        arg_to_add = ++arg_string;

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

                } else if (*arg_string == '"') {
                        arg_to_add = ++arg_string;

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

                if (list_add_item(arg_list, ++arg_count, arg_to_add) < 0) {
                        goto exit_error;
                }

                /* terminate argument */
                if (*arg_string == '\0') {
                        break;
                } else {
                        *arg_string++ = '\0';
                }
        }

add_args_to_table:
        if ((arg_table = m_calloc(arg_count, sizeof(ch_t*))) == NULL) {
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

        delete_list(arg_list);

        *argc = arg_count;
        return arg_table;


        /* error occurred - memory/object deallocation */
exit_error:
        if (arg_table) {
                m_free(arg_table);
        }

        if (arg_list) {
                i32_t items_in_list = list_get_item_count(arg_list);
                while (items_in_list-- > 0) {
                        list_unlink_nitem_data(arg_list, 0);
                        list_rm_nitem(arg_list, 0);
                }

                delete_list(arg_list);
        }

        if (arg_string) {
                m_free(arg_string);
        }

        *argc = 0;
        return NULL;
}

//==============================================================================
/**
 * @brief Function remove argument table
 *
 * @param **argv        pointer to argument table
 * @param   argc        argument count
 */
//==============================================================================
static void delete_argument_table(char **argv, int argc)
{
        if (argv == NULL) {
                return;
        }

        if (argc > 1) {
                if (argv[1]) {
                        m_free(argv[1]);
                }
        }

        m_free(argv);
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
