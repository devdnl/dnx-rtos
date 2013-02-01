/*=============================================================================================*//**
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


*//*==============================================================================================*/

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "runtime.h"
#include "regapp.h"
#include "oswrap.h"
#include "taskmoni.h"
#include "io.h"
#include <string.h>


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
#define calloc(nmemb, msize)              moni_calloc(nmemb, msize)
#define malloc(size)                      moni_malloc(size)
#define free(mem)                         moni_free(mem)


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static app_t *RunAsApp(task_t app, const ch_t *appName, uint_t stackSize, void *arg);
static app_t *RunAsDaemon(task_t app, const ch_t *appName, uint_t stackSize, void *arg);


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief This function start task as application
 *
 * @param[in]  app                  application function
 * @param[in]  *appName             application name
 * @param[in]  stackSize            application stack size
 * @param[in]  *arg                 application arguments
 *
 * @return application handler
 */
//================================================================================================//
static app_t *RunAsApp(task_t app, const ch_t *appName, uint_t stackSize, void *arg)
{
      app_t *appHandle = NULL;

      /* check pointers values */
      if (!app || !appName || !stackSize) {
            goto RunAsApp_end;
      }

      /* allocate memory for application handler */
      appHandle = calloc(1, sizeof(app_t));

      if (appHandle) {
            /* initialize stdio data */
            appHandle->arg              = arg;
            appHandle->exitCode         = STD_RET_UNKNOWN;
            appHandle->parentTaskHandle = TaskGetCurrentTaskHandle();
            appHandle->taskHandle       = NULL;
            appHandle->stdin            = NULL;
            appHandle->stdout           = NULL;

            /* start application task */
            if (TaskCreate(app, appName, stackSize, appHandle, 0, &appHandle->taskHandle) != OS_OK) {
                  free(appHandle);
                  appHandle = NULL;
            }
      }

      RunAsApp_end:
      return appHandle;
}


//================================================================================================//
/**
 * @brief This function start task as daemon
 *
 * @param[in]  app                   application function
 * @param[in]  *appName              application name
 * @param[in]  stackSize             application stack size
 * @param[in]  *arg                  application arguments
 *
 * @return application handler
 */
//================================================================================================//
static app_t *RunAsDaemon(task_t app, const ch_t *appName, uint_t stackSize, void *arg)
{
      app_t *appHandle = NULL;

      /* check pointers values */
      if (!app|| !appName  || !stackSize) {
            goto RunAsDaemon_end;
      }

      /* allocate memory for application handler */
      appHandle = calloc(1, sizeof(app_t));

      if (appHandle) {
            /* set default values */
            appHandle->arg              = arg;
            appHandle->exitCode         = STD_RET_UNKNOWN;
            appHandle->stdin            = NULL;
            appHandle->stdout           = NULL;
            appHandle->parentTaskHandle = TaskGetCurrentTaskHandle();
            appHandle->taskHandle       = NULL;

            /* start daemon task */
            if (TaskCreate(app, appName, stackSize, appHandle, 0, &appHandle->taskHandle) != OS_OK) {
                  free(appHandle);
                  appHandle = NULL;
            }
      }

      RunAsDaemon_end:
      return appHandle;
}


//================================================================================================//
/**
 * @brief Run task as application using only task name
 *
 * @param[in]  *name          task name
 * @param[in]  *argv          task arguments
 *
 * @return application handler
 */
//================================================================================================//
app_t *Exec(const ch_t *name, ch_t *argv)
{
      regAppData_t appData = regapp_GetAppData(name);

      if (appData.appPtr == NULL || *appData.stackSize < MINIMAL_STACK_SIZE) {
            return NULL;
      }

      return RunAsApp(appData.appPtr, appData.appName, *appData.stackSize, argv);
}


//================================================================================================//
/**
 * @brief Run task as daemon using only task name
 *
 * @param[in]  *name          task name
 * @param[in]  *argv          task arguments
 *
 * @return application handler
 */
//================================================================================================//
app_t *Execd(const ch_t *name, ch_t *argv)
{
      regAppData_t appData = regapp_GetAppData(name);

      if (appData.appPtr == NULL) {
            return NULL;
      }

      return RunAsDaemon(appData.appPtr, appData.appName, *appData.stackSize, argv);
}


//================================================================================================//
/**
 * @brief Run task daemon as service. Function used on low level of system startup
 *
 * @param[in]  *name          task name
 * @param[in]  *argv          task arguments
 *
 * @return application handler
 */
//================================================================================================//
stdRet_t StartDaemon(const ch_t *name, ch_t *argv)
{
      if (Execd(name, argv) != NULL) {
            kprint("%s daemon started\n", name);
            return STD_RET_OK;
      } else {
            kprint("\x1B[31m%s start failed\x1B[0m\n", name);
            return STD_RET_ERROR;
      }
}


//================================================================================================//
/**
 * @brief Function free application handler
 *
 * @param *appArgs      pointer to the appArgs structure which contains application handler
 *
 * @retval STD_STATUS_OK            freed success
 * @retval STD_STATUS_ERROR         freed error, bad pointer
 */
//================================================================================================//
stdRet_t KillApp(app_t *appArgs)
{
      stdRet_t status = STD_RET_ERROR;

      if (appArgs) {
            if (appArgs->taskHandle) {
                  TaskDelete(appArgs->taskHandle);
            }

            free(appArgs);

            status = STD_RET_OK;
      }

      return status;
}


//================================================================================================//
/**
 * @brief Terminate application
 *
 * @param *appObj             application object
 * @param  exitCode           return value
 */
//================================================================================================//
void CloseApp(app_t *appObj, stdRet_t exitCode)
{
      /* set exit code */
      appObj->exitCode = exitCode;

      TaskSuspend(appObj->parentTaskHandle);
      TaskResume(appObj->parentTaskHandle);

      appObj->taskHandle       = NULL;
      appObj->parentTaskHandle = NULL;

      TaskTerminate();
}


//================================================================================================//
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
//================================================================================================//
stdRet_t ParseArg(ch_t *argv, ch_t *findArg, parseType_t parseAs, void *result)
{
      u8_t base;
      stdRet_t status = STD_RET_ERROR;

      /* check argument correctness */
      if (!argv || parseAs >= PARSE_AS_UNKNOWN || (parseAs == PARSE_AS_EXIST ? 0 : !result)) {
            goto ParseArg_end;
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


/*==================================================================================================
                                            End of file
==================================================================================================*/
