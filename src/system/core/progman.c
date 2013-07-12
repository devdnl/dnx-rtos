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
#include "core/progman.h"
#include "core/sysmoni.h"
#include "core/list.h"
#include "user/regprg.h"
#include "kernel/kwrapper.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#undef stdin
#undef stdout
#undef stderr

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
struct program_data {
        int (*main)(int, char**);
        const char      *cwd;
        enum prog_state *status;
        int             *exit_code;
        FILE            *stdin;
        FILE            *stdout;
        FILE            *stderr;
        char            **argv;
        int              argc;
        uint             globals_size;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void   set_status(enum prog_state *status_ptr, enum prog_state status);
static char **new_argument_table(const char *arg, const char *name, int *argc);
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
 * @param *cwd          current working dir
 * @param *stdin        stdin file
 * @param *stdout       stdout file
 * @param *status       program status
 * @param *exit_code    exit code
 *
 * @return NULL if error, otherwise task handle
 */
//==============================================================================
task_t *prgm_new_program(const char *name, const char *args, const char *cwd, FILE *stdin,
                         FILE *stdout, enum prog_state *status, int *exit_code)
{
        struct program_data *pdata   = NULL;
        task_t              *taskhdl = NULL;
        struct regprg_pdata  regpdata;

        if (!name || !args || !cwd) {
                set_status(status, PROGRAM_ARGUMENTS_PARSE_ERROR);
                return NULL;
        }

        if (regprg_get_program_data(name, &regpdata) != STD_RET_OK) {
                set_status(status, PROGRAM_DOES_NOT_EXIST);
                return NULL;
        }

        if ((pdata = sysm_syscalloc(1, sizeof(struct program_data))) == NULL) {
                set_status(status, PROGRAM_NOT_ENOUGH_FREE_MEMORY);
                return NULL;
        }

        if ((pdata->argv = new_argument_table(args, name, &pdata->argc)) == NULL) {
                sysm_sysfree(pdata);
                set_status(status, PROGRAM_ARGUMENTS_PARSE_ERROR);
                return NULL;
        }

        pdata->main         = regpdata.main_function;
        pdata->cwd          = cwd;
        pdata->stdin        = stdin;
        pdata->stdout       = stdout;
        pdata->stderr       = stdout;
        pdata->globals_size = *regpdata.globals_size;
        pdata->status       = status;
        pdata->exit_code    = exit_code;

        set_status(status, PROGRAM_RUNNING);

        if (exit_code) {
                *exit_code = STD_RET_UNKNOWN;
        }

        taskhdl = new_task(task_program_startup, regpdata.program_name,
                           *regpdata.stack_depth, pdata);

        if (taskhdl == NULL) {
                set_status(status, PROGRAM_HANDLE_ERROR);
                delete_argument_table(pdata->argv, pdata->argc);
                sysm_sysfree(pdata);
                return NULL;
        }

        return taskhdl;
}

//==============================================================================
/**
 * @brief Function delete running program
 *
 * @param *taskhdl              task handle
 * @param  exit_code            program exit value
 */
//==============================================================================
void prgm_delete_program(task_t *taskhdl, int exit_code)
{
        struct task_data    *tdata;
        struct program_data *pdata;

        if (taskhdl == NULL) {
                return;
        }

        if ((tdata = _get_task_data(taskhdl))) {
                if (tdata->f_global_vars) {
                        sysm_tskfree_as(taskhdl, tdata->f_global_vars);
                        tdata->f_global_vars = NULL;
                }

                if ((pdata = tdata->f_user)) {
                        if (pdata->argv) {
                                delete_argument_table(pdata->argv, pdata->argc);
                        }

                        if (pdata->exit_code) {
                                *pdata->exit_code = exit_code;
                        }

                        if (pdata->status) {
                                *pdata->status = PROGRAM_ENDED;
                        }

                        sysm_sysfree(pdata);
                        tdata->f_user = NULL;
                }
        }

        delete_task(taskhdl);
}

//==============================================================================
/**
 * @brief Function close program immediately and set exit code
 *
 * @param status        exit value
 */
//==============================================================================
void prgm_exit(int status)
{
        prgm_delete_program(get_task_handle(), status);

        /* wait to kill program */
        for (;;);
}

//==============================================================================
/**
 * @brief Function close program with error code
 */
//==============================================================================
void prgm_abort(void)
{
        prgm_delete_program(get_task_handle(), -1);

        /* wait to kill program */
        for (;;);
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
        struct program_data *prog_data = argv;
        struct task_data    *task_data = NULL;
        void                *task_mem  = NULL;
        int                  exit_code = STD_RET_UNKNOWN;

        if (!(task_data = _get_this_task_data())) {
                sysm_sysfree(prog_data);
                set_status(prog_data->status, PROGRAM_HANDLE_ERROR);
                task_exit();
        }

        task_data->f_user    = prog_data;
        task_data->f_stdin   = prog_data->stdin;
        task_data->f_stdout  = prog_data->stdout;
        task_data->f_stderr  = prog_data->stdout;
        task_data->f_cwd     = prog_data->cwd;
        task_data->f_program = true;

        if (prog_data->globals_size) {
                if (!(task_mem = sysm_tskcalloc(1, prog_data->globals_size))) {
                        set_status(prog_data->status, PROGRAM_NOT_ENOUGH_FREE_MEMORY);
                        goto task_exit;
                }

                task_data->f_global_vars = task_mem;
        }

        exit_code = prog_data->main(prog_data->argc, prog_data->argv);

        set_status(prog_data->status, PROGRAM_ENDED);

        task_exit:
        if (prog_data->exit_code) {
                *prog_data->exit_code = exit_code;
        }

        if (task_data->f_global_vars) {
                sysm_tskfree(task_data->f_global_vars);
                task_data->f_global_vars = NULL;
        }

        if (prog_data->argv) {
                delete_argument_table(prog_data->argv, prog_data->argc);
        }

        sysm_sysfree(prog_data);
        task_data->f_user = NULL;

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
static void set_status(enum prog_state *status_ptr, enum prog_state status)
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
static char **new_argument_table(const char *arg, const char *name, int *argc)
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

        if (list_add_item(arg_list, ++arg_count, (char*)name) < 0) {
                goto exit_error;
        }

        if (arg[0] == '\0') {
                goto add_args_to_table;
        }

        if ((arg_string = sysm_syscalloc(strlen(arg) + 1, sizeof(char))) == NULL) {
                goto exit_error;
        }

        strcpy(arg_string, arg);

        while (*arg_string != '\0') {
                char *arg_to_add = NULL;

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

        delete_list(arg_list);

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

                delete_list(arg_list);
        }

        if (arg_string) {
                sysm_sysfree(arg_string);
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
                        sysm_sysfree(argv[1]);
                }
        }

        sysm_sysfree(argv);
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/