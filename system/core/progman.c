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
#include "oswrap.h"
#include "progman.h"
#include "regprg.h"
#include "taskmoni.h"
#include "memman.h"
#include "dlist.h"
#include <string.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#undef stdin
#undef stdout

#define monitored_calloc_as(task, nmemb, msize) tskm_calloc_as(task, nmemb, msize)
#define monitored_malloc_as(task, size)         tskm_malloc_as(task, size)
#define monitored_free_as(task, mem)            tskm_free_as(task, mem)
#define monitored_calloc(nmemb, msize)          tskm_calloc(nmemb, msize)
#define monitored_malloc(size)                  tskm_malloc(size)
#define monitored_free(mem)                     tskm_free(mem)
#define calloc(nmemb, msize)                    memman_calloc(nmemb, msize)
#define malloc(size)                            memman_malloc(size)
#define free(mem)                               memman_free(mem)

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
struct program_data {
        int (*main)(int, char**);
        char            *cwd;
        enum prg_status *status;
        int             *exit_code;
        FILE_t          *stdin;
        FILE_t          *stdout;
        char           **argv;
        int              argc;
        uint             globals_size;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void   set_status(enum prg_status *status_ptr, enum prg_status status);
static char **new_argument_table(char *arg, const char *name, int *argc);
static void   delete_argument_table(char **argv, int argc);
static void   task_program_startup(void *argv);

/*==============================================================================
  Local object definitions
==============================================================================*/

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
 * @return NULL if error, otherwise task handle
 */
//==============================================================================
task_t *prgm_new_program(char *name, char *args, char *cwd, FILE_t *stdin,
                         FILE_t *stdout, enum prg_status *status, int *exit_code)
{
        struct program_data *pdata   = NULL;
        task_t              *taskhdl = NULL;
        struct regprg_pdata  regpdata;

        if (!name || !args || !cwd) {
                return NULL;
        }

        if (regprg_get_program_data(name, &regpdata) != STD_RET_OK) {
                return NULL;
        }

        if ((pdata = calloc(1, sizeof(struct program_data))) == NULL) {
                return NULL;
        }

        if ((pdata->argv = new_argument_table(args, name, &pdata->argc)) == NULL) {
                free(pdata);
                return NULL;
        }

        pdata->main         = regpdata.main_function;
        pdata->cwd          = cwd;
        pdata->stdin        = stdin;
        pdata->stdout       = stdout;
        pdata->globals_size = *regpdata.globals_size;
        pdata->status       = status;
        pdata->exit_code    = exit_code;

        if (status)
                *status = PROGRAM_RUNNING;

        if (exit_code)
                *exit_code = STD_RET_UNKNOWN;

        taskhdl = new_task(task_program_startup, regpdata.program_name,
                           *regpdata.stack_depth, pdata);

        if (taskhdl == NULL) {
                delete_argument_table(pdata->argv, pdata->argc);
                free(pdata);
                return NULL;
        }

        return taskhdl;
}

//==============================================================================
/**
 * @brief Function delete running program
 *
 * @param *taskhdl              task handle
 */
//==============================================================================
void prgm_delete_program(task_t *taskhdl)
{
        struct task_data    *tdata;
        struct program_data *pdata;

        if (taskhdl == NULL)
                return;

        tdata = get_task_data(taskhdl);
        if (tdata) {
                if (tdata->f_global_vars) {
                        monitored_free_as(taskhdl, tdata->f_global_vars);
                        tdata->f_global_vars = NULL;
                }

                pdata = tdata->f_user;
                if (pdata) {
                        if (pdata->argv) {
                                delete_argument_table(pdata->argv, pdata->argc);
                        }

                        if (pdata->exit_code) {
                                *pdata->exit_code = STD_RET_OK;
                        }

                        if (pdata->status) {
                                *pdata->status = PROGRAM_ENDED;
                        }

                        free(pdata);
                        tdata->f_user = NULL;
                }
        }

        delete_task(taskhdl);
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
        struct program_data *pdata     = argv;
        struct task_data    *tdata     = NULL;
        void                *taskmem   = NULL;
        int                  exit_code = STD_RET_UNKNOWN;

        if (!(tdata = get_this_task_data())) {
                free(pdata);
                set_status(pdata->status, PROGRAM_HANDLE_ERROR);
                task_exit();
        }

        tdata->f_user   = pdata;
        tdata->f_stdin  = pdata->stdin;
        tdata->f_stdout = pdata->stdout;
        tdata->f_cwd    = pdata->cwd;

        if (pdata->globals_size) {
                if (!(taskmem = monitored_calloc(1, pdata->globals_size))) {
                        set_status(pdata->status, PROGRAM_NOT_ENOUGH_FREE_MEMORY);
                        goto task_exit;
                }

                tdata->f_global_vars = taskmem;
        }

        exit_code = pdata->main(pdata->argc, pdata->argv);

        set_status(pdata->status, PROGRAM_ENDED);

        task_exit:
        if (pdata->exit_code) {
                *pdata->exit_code = exit_code;
        }

        if (tdata->f_global_vars) {
                monitored_free(tdata->f_global_vars);
                tdata->f_global_vars = NULL;
        }

        if (pdata->argv) {
                delete_argument_table(pdata->argv, pdata->argc);
        }

        free(pdata);
        tdata->f_user = NULL;

        task_exit();
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
static void delete_argument_table(char **argv, int argc)
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
