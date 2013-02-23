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
#include "regapp.h"
#include "oswrap.h"
#include "taskmoni.h"
#include "io.h"
#include "dlist.h"
#include <string.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define calloc(nmemb, msize)              tskm_calloc(nmemb, msize)
#define malloc(size)                      tskm_malloc(size)
#define free(mem)                         tskm_free(mem)

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static prog_t *new_program(task_t app, const ch_t *name, uint stackSize, ch_t *arg);
static ch_t  **new_argument_table(ch_t *arg, const ch_t *name, int *argc);
static void    delete_argument_table(ch_t **argv, int argc);

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
 * @brief Run task as program using only task name
 *
 * @param[in]  *name          task name
 * @param[in]  *argv          task arguments
 *
 * @return application handler
 */
//==============================================================================
prog_t *exec(const ch_t *name, ch_t *argv)
{
        regAppData_t appData = regapp_get_program_data(name);

        if (appData.appPtr == NULL || *appData.stackSize < MINIMAL_STACK_SIZE) {
                return NULL;
        }

        return new_program(appData.appPtr, appData.appName, *appData.stackSize, argv);
}

//==============================================================================
/**
 * @brief Function terminate program freeing program's object
 *
 * @param *prog                 pointer to the program object
 *
 * @retval STD_STATUS_OK        freed success
 * @retval STD_STATUS_ERROR     freed error, bad pointer
 */
//==============================================================================
stdRet_t kill_prog(prog_t *prog)
{
        if (prog) {
                if (prog->taskHandle) {
                        delete_task(prog->taskHandle);
                }

                delete_argument_table(prog->argv, prog->argc);

                free(prog);

                return STD_RET_OK;
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Self terminate program
 *
 * @param *appObj             application object
 * @param  exitCode           return value
 */
//==============================================================================
void exit_prog(prog_t *prog, stdRet_t exitCode)
{
        prog->exitCode = exitCode;

        suspend_task(prog->parentTaskHandle);
        resume_task(prog->parentTaskHandle);

        prog->taskHandle       = NULL;
        prog->parentTaskHandle = NULL;

        delete_argument_table(prog->argv, prog->argc);

        delete_task(get_task_handle());
}

//==============================================================================
/**
 * @brief This function start task as program
 *
 * @param[in]   app                 program function
 * @param[in]  *name                program name
 * @param[in]   stackSize           program stack size
 * @param[in]  *arg                 program arguments
 *
 * @return program handler pointer if success, otherwise NULL
 */
//==============================================================================
static prog_t *new_program(task_t app, const ch_t *name, uint stackSize, ch_t *arg)
{
        prog_t *progHdl;

        if (!app || !name || !stackSize || !arg) {
                return NULL;
        }

        progHdl = calloc(1, sizeof(prog_t));
        if (progHdl == NULL) {
                return NULL;
        }

        progHdl->argv = new_argument_table(arg, name, &progHdl->argc);
        if (progHdl->argv == NULL) {
                free(progHdl);
                return NULL;
        }

        progHdl->exitCode         = STD_RET_UNKNOWN;
        progHdl->parentTaskHandle = get_task_handle();
        progHdl->taskHandle       = NULL;
        progHdl->stdin            = NULL;
        progHdl->stdout           = NULL;

        /* start application task */
        if (new_task(app, name, stackSize, progHdl, 0,
                       &progHdl->taskHandle) != OS_OK) {

                free(progHdl->argv);
                free(progHdl);
                progHdl = NULL;
        }

        return progHdl;
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
        int   arg_count  = 0;
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
