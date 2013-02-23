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
static prog_t *new_program(task_t app, const ch_t *name, uint_t stackSize, ch_t *arg);
static ch_t  **new_argument_table(ch_t *arg, const ch_t *name, int_t *argc);
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
        regAppData_t appData = regapp_GetAppData(name);

        if (appData.appPtr == NULL || *appData.stackSize < MINIMAL_STACK_SIZE) {
                return NULL;
        }

        return new_program(appData.appPtr, appData.appName, *appData.stackSize, argv);
}

//==============================================================================
/**
 * @brief Run task daemon as service. Function used on low level of system
 *        startup
 *
 * @param[in]  *name          task name
 * @param[in]  *argv          task arguments
 *
 * @return application handler
 */
//==============================================================================
stdRet_t start_daemon(const ch_t *name, ch_t *argv)
{
        if (exec(name, argv) != NULL) {
                kprint("%s daemon started\n", name);
                return STD_RET_OK;
        } else {
                kprint("\x1B[31m%s start failed\x1B[0m\n", name);
                return STD_RET_ERROR;
        }
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

        TaskSuspend(prog->parentTaskHandle);
        TaskResume(prog->parentTaskHandle);

        prog->taskHandle       = NULL;
        prog->parentTaskHandle = NULL;

        delete_argument_table(prog->argv, prog->argc);

        delete_task(TaskGetCurrentTaskHandle());
}

//==============================================================================
/**
 * @brief Function parse application arguments
 *
 * @param *argv         argument string
 * @param *findArg      argument to find
 * @param parseAs       type of result
 * @param *result       result buffer (depending of parse type)
 *
 * @retval STD_RET_OK         value of argument was finded and converted
 * @retval STD_RET_ERROR      cannot find argument specified
 */
//==============================================================================
stdRet_t ParseArg(ch_t *argv, ch_t *findArg, parseType_t parseAs, void *result)
{
        u8_t base;
        stdRet_t status = STD_RET_ERROR;

        /* check argument correctness */
        if (  !argv || parseAs >= PARSE_AS_UNKNOWN
           || (parseAs == PARSE_AS_EXIST ? 0 : !result)) {

                return STD_RET_ERROR;
        }

        u32_t findArgSize = strlen(findArg);

        /* scan argv line */
        while (*argv != '\0') {
                /* if find character which open string, parser must find end of a string */
                if (*argv == '"') {
                        ch_t *stringEnd;

                        argv++;

                        if ((stringEnd = strchr(argv, '"')) == NULL) {
                                argv++;
                        } else {
                                argv = stringEnd + 1;
                        }
                }

                /* check that argument is short or long */
                if (*argv == '-' && *(argv + 1) == '-') {
                        argv += 2;

                        if (findArgSize == 1) {
                                argv++;
                        }
                } else if (*argv == '-' && findArgSize == 1) {
                        argv++;
                }

                /* check if current argument is found */
                if (strncmp(argv, findArg, findArgSize) == 0) {
                        argv += findArgSize;

                        if (parseAs == PARSE_AS_STRING) {
                                ch_t character = *argv++;

                                if (character == '"') {
                                        /* try to find closed " */
                                        if (strchr(argv, '"') == NULL) {
                                                goto ParseArg_end;
                                        }

                                        ch_t *string = result;

                                        while ((character = *(argv++)) != '"') {
                                                *(string++) = character;
                                        }

                                        *(string++) = '\0';

                                        status = STD_RET_OK;
                                }
                        } else if (parseAs == PARSE_AS_CHAR) {
                                ch_t *character = result;

                                *character = *argv;

                                status = STD_RET_OK;

                        } else if (parseAs == PARSE_AS_EXIST) {
                                status = STD_RET_OK;

                        } else {
                                i32_t *value = (i32_t*)result;

                                switch (parseAs) {
                                case PARSE_AS_BIN: base = 2;  break;
                                case PARSE_AS_OCT: base = 8;  break;
                                case PARSE_AS_DEC: base = 10; break;
                                case PARSE_AS_HEX: base = 16; break;
                                default: goto ParseArg_end;
                                }

                                atoi(argv, base, value);

                                status = STD_RET_OK;
                        }

                        goto ParseArg_end;
                }

                argv++;
        }

        ParseArg_end:
        return status;
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
static prog_t *new_program(task_t app, const ch_t *name, uint_t stackSize, ch_t *arg)
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
        progHdl->parentTaskHandle = TaskGetCurrentTaskHandle();
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
static ch_t **new_argument_table(ch_t *arg, const ch_t *name, int_t *argc)
{
        int_t   arg_count  = 0;
        ch_t  **arg_table  = NULL;
        list_t *arg_list   = NULL;
        ch_t   *arg_string = NULL;

        if (arg == NULL || name == NULL || argc == NULL) {
                goto exit_error;
        }

        if ((arg_list = ListCreate()) == NULL) {
                goto exit_error;
        }

        if (ListAddItem(arg_list, ++arg_count, (ch_t*)name) < 0) {
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

                if (ListAddItem(arg_list, ++arg_count, arg_to_add) < 0) {
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

        for (int_t i = 0; i < arg_count; i++) {
                arg_table[i] = ListGetItemDataByNo(arg_list, 0);

                if (arg_table[i] == NULL) {
                        goto exit_error;
                }

                ListUnlinkItemDataByNo(arg_list, 0);
                ListRmItemByNo(arg_list, 0);
        }

        ListDelete(arg_list);

        *argc = arg_count;
        return arg_table;


        /* error occurred - memory/object deallocation */
exit_error:
        if (arg_table) {
                free(arg_table);
        }

        if (arg_list) {
                i32_t items_in_list = ListGetItemCount(arg_list);
                while (items_in_list-- > 0) {
                        ListUnlinkItemDataByNo(arg_list, 0);
                        ListRmItemByNo(arg_list, 0);
                }

                ListDelete(arg_list);
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
