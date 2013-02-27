/*=========================================================================*//**
@file    appruntime.c

@author  Daniel Zorychta

@brief   This file support runtime environment for applications

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
#include "runtime.h"
#include "regprg.h"
#include "oswrap.h"
#include "taskmoni.h"
#include "io.h"
#include "dlist.h"
#include <string.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#undef stdin
#undef stdout

#define calloc(nmemb, msize)            tskm_calloc(nmemb, msize)
#define malloc(size)                    tskm_malloc(size)
#define free(mem)                       tskm_free(mem)

#define MTX_BTIME_FOR_PLIST             1
#define PROGRAM_DEFAULT_PRIORITY        0
#define SEM_PROGRAM_BLOCK_TIME          250

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
struct program_object {
        int    exit_code;
        sem_t  wait_sem;
        task_t taskhdl;
        enum   prg_status status;
};

struct data_of_running_program {
        FILE_t *stdin;                          /* stdin file                   */
        FILE_t *stdout;                         /* stdout file                  */
        char   *cwd;                            /* current working path         */
        void   *global_vars;                    /* address to global variables  */
        prog_t *proghdl;                        /* pointer to program handler   */
        int   (*main_function)(int, char**);    /* program's main function      */
        char   *name;                           /* program's name               */
        char   *args;                           /* not formated argument string */
        char  **argv;                           /* table with arguments         */
        int     argc;                           /* argument table               */
        uint    globals_size;                   /* size of global variables     */
};

struct program_mangement {
        list_t *list_of_running_programs;

        struct {
                struct {
                        task_t taskhdl;
                        void  *address;
                } globals;

                struct {
                        task_t  taskhdl;
                        FILE_t *file;
                } stdin;

                struct {
                        task_t  taskhdl;
                        FILE_t *file;
                } stdout;
        } cache;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static stdRet_t init_program_management(void);
static ch_t   **new_argument_table(ch_t *arg, const ch_t *name, int *argc);
static void     delete_argument_table(ch_t **argv, int argc);
static void     task_program_startup(void *argv);

/*==============================================================================
  Local object definitions
==============================================================================*/
static struct program_mangement pman;

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

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
 * @return NULL if error, otherwise program handler
 */
//==============================================================================
prog_t *new_program(ch_t *name, ch_t *args, FILE_t *fstdin, FILE_t *fstdout, ch_t *cwd)
{
        struct data_of_running_program *progdata = NULL;
        struct program_object          *proghdl  = NULL;
        struct regprg_pdata             regpdata;

        if (init_program_management() != STD_RET_OK) {
                return NULL;
        }

        if (regprg_get_program_data(name, &regpdata) != STD_RET_OK) {
                return NULL;
        }

        if ((proghdl = calloc(1, sizeof(struct program_object))) == NULL) {
                goto error;
        }

        if ((progdata = calloc(1, sizeof(struct data_of_running_program))) == NULL) {
                goto error;
        }

        if ((proghdl->wait_sem = new_semaphore()) == NULL) {
                goto error;
        }

        semaphore_take(proghdl->wait_sem, 0);
        proghdl->exit_code      = STD_RET_UNKNOWN;
        proghdl->status         = PROGRAM_INITING;

        progdata->args          = args;
        progdata->globals_size  = *regpdata.globals_size;
        progdata->main_function = regpdata.main_function;
        progdata->name          = regpdata.name;
        progdata->cwd           = cwd;
        progdata->stdin         = fstdin;
        progdata->stdout        = fstdout;
        progdata->proghdl       = proghdl;

        if (new_task(task_program_startup, regpdata.name, regpdata.stack_deep,
                     progdata, PROGRAM_DEFAULT_PRIORITY, &proghdl->taskhdl) == OS_OK) {

                suspend_all_tasks();

                i32_t item = list_add_item(pman.list_of_running_programs,
                                           (u32_t)proghdl->taskhdl, progdata);
                resume_all_tasks();

                if (item < 0) {
                        goto error;
                }

                resume_task(proghdl->taskhdl);

                return proghdl;
        }

        /* an error occurred */
        error:
        if (proghdl) {
                if (proghdl->taskhdl) {
                        delete_task(proghdl->taskhdl);
                }

                if (proghdl->wait_sem) {
                        delete_semaphore(proghdl->wait_sem);
                }

                free(proghdl);
        }

        if (progdata) {
                free(progdata);
        }

        return NULL;
}

//==============================================================================
/**
 * @brief Function kill running program
 *
 * @param *proghdl      program handle
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdRet_t delete_program(prog_t *proghdl)
{
        if (proghdl) {
                suspend_all_tasks();
                if (proghdl->taskhdl) {
                        delete_task(proghdl->taskhdl);

                        list_rm_iditem(pman.list_of_running_programs,
                                       (u32_t)proghdl->taskhdl);
                }
                resume_all_tasks();

                delete_semaphore(proghdl->wait_sem);
                free(proghdl);
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function returns program status
 *
 * @param *proghdl      program handle
 *
 * @return status
 */
//==============================================================================
enum prg_status get_program_status(prog_t *proghdl)
{
        enum prg_status status = PROGRAM_HANDLE_ERROR;

        if (proghdl) {
                suspend_all_tasks();
                status = proghdl->status;
                resume_all_tasks();
        }

        return status;
}

//==============================================================================
/**
 * @brief Function wait to program end
 *
 * @param *proghdl      program handle
 */
//==============================================================================
void wait_for_program_end(prog_t *proghdl)
{
        if (proghdl) {
                if (  proghdl->status == PROGRAM_RUNNING
                   || proghdl->status == PROGRAM_INITING ) {

                        semaphore_take(proghdl->wait_sem, SEM_PROGRAM_BLOCK_TIME);
                }
        }
}

//==============================================================================
/**
 * @brief Function returns stdin file
 *
 * @return stdin file or NULL if doesn't exist
 */
//==============================================================================
FILE_t *get_program_stdin(void)
{
        struct data_of_running_program *pdata;
        task_t  taskhdl = get_task_handle();
        FILE_t *fstdin  = NULL;

        suspend_all_tasks();

        if (pman.cache.stdin.taskhdl == taskhdl) {
                fstdin = pman.cache.stdin.file;
        } else {
                if ((pdata = list_get_iditem_data(pman.list_of_running_programs,
                                                  (u32_t)taskhdl))) {

                        pman.cache.stdin.taskhdl = taskhdl;
                        pman.cache.stdin.file    = pdata->stdin;
                        fstdin = pdata->stdin;
                }
        }

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
FILE_t *get_program_stdout(void)
{
        struct data_of_running_program *pdata;
        task_t  taskhdl = get_task_handle();
        FILE_t *fstdout = NULL;

        suspend_all_tasks();

        if (pman.cache.stdout.taskhdl == taskhdl) {
                fstdout = pman.cache.stdout.file;
        } else {
                if ((pdata = list_get_iditem_data(pman.list_of_running_programs,
                                                  (u32_t)taskhdl))) {

                        pman.cache.stdout.taskhdl = taskhdl;
                        pman.cache.stdout.file    = pdata->stdout;
                        fstdout = pdata->stdout;
                }
        }

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
void *get_program_globals(void)
{
        struct data_of_running_program *pdata;
        task_t taskhdl = get_task_handle();
        void  *globals = NULL;

        suspend_all_tasks();

        if (pman.cache.globals.taskhdl == taskhdl) {
                globals = pman.cache.globals.address;
        } else {
                if ((pdata = list_get_iditem_data(pman.list_of_running_programs,
                                                  (u32_t)taskhdl))) {

                        pman.cache.globals.taskhdl = taskhdl;
                        pman.cache.globals.address = pdata->global_vars;
                        globals = pdata->global_vars;
                }
        }

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
ch_t *get_program_cwd(void)
{
        struct data_of_running_program *pdata;
        ch_t *cwd = NULL;

        suspend_all_tasks();

        if ((pdata = list_get_iditem_data(pman.list_of_running_programs,
                                          (u32_t)get_task_handle()))) {

                cwd = pdata->cwd;
        }

        resume_all_tasks();

        return cwd;
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

        /* suspend this task to finalize parent function */
        suspend_this_task();

        progdata->proghdl->status = PROGRAM_INITING;

        if ((progdata->global_vars = calloc(1, progdata->globals_size)) == NULL) {
                progdata->proghdl->status = PROGRAM_NOT_ENOUGH_FREE_MEMORY;
                goto task_exit;
        }

        if ((progdata->argv = new_argument_table(progdata->args, progdata->name,
                                                 &progdata->argc)) == NULL) {

                progdata->proghdl->status = PROGRAM_ARGUMENTS_PARSE_ERROR;
                goto task_exit;
        }

        progdata->proghdl->status    = PROGRAM_RUNNING;
        progdata->proghdl->exit_code = progdata->main_function(progdata->argc,
                                                               progdata->argv);
        progdata->proghdl->status    = PROGRAM_ENDED;

        task_exit:
        semaphore_give(progdata->proghdl->wait_sem);
        progdata->proghdl->taskhdl = NULL;

        if (progdata->global_vars) {
                free(progdata->global_vars);
        }

        if (progdata->argv) {
                delete_argument_table(progdata->argv, progdata->argc);
        }

        suspend_all_tasks();
        list_rm_iditem(pman.list_of_running_programs, (u32_t)get_task_handle());
        resume_all_tasks();

        pman.cache.globals.taskhdl = NULL;
        pman.cache.stdin.taskhdl   = NULL;
        pman.cache.stdout.taskhdl  = NULL;

        terminate_task();
}

//==============================================================================
/**
 * @brief Function initialize program manager
 *
 * @retval STD_RET_OK           manager variables initialized successfully
 * @retval STD_RET_ERROR        variables not initialized
 */
//==============================================================================
static stdRet_t init_program_management(void)
{
        if (pman.list_of_running_programs != NULL) {
                return STD_RET_OK;
        }

        if ((pman.list_of_running_programs = new_list()) != NULL) {
                return STD_RET_OK;
        }

        return STD_RET_ERROR;
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
static ch_t **new_argument_table(ch_t *arg, const ch_t *name, int *argc)
{
        int     arg_count  = 0;
        ch_t  **arg_table  = NULL;
        list_t *arg_list   = NULL;
        ch_t   *arg_string = NULL;

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

        if ((arg_string = calloc(strlen(arg) + 1, sizeof(ch_t))) == NULL) {
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
        if ((arg_table = calloc(arg_count, sizeof(ch_t*))) == NULL) {
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
                free(arg_table);
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
                free(arg_string);
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
static void delete_argument_table(ch_t **argv, int argc)
{
        if (argv == NULL) {
                return;
        }

        if (argc > 1) {
                if (argv[1]) {
                        free(argv[1]);
                }
        }

        free(argv);
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
