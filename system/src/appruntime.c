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
#include "appruntime.h"
#include "regapp.h"


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/


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
 * @param[out] *status        status
 *
 * @return application handler
 */
//================================================================================================//
appArgs_t *RunAsApp(pdTASK_CODE app, const ch_t *appName, u32_t stackSize, void *arg, stdRet_t *status)
{
      *status = STD_RET_ALLOCERROR;
      appArgs_t *appHandle = NULL;

      /* check pointers values */
      if (!app || !appName || !stackSize || !status)
            goto RunAsApp_end;

      /* allocate memory for application handler */
      appHandle = (appArgs_t *)Malloc(sizeof(appArgs_t));

      if (!appHandle)
            goto RunAsApp_end;
      else
            appHandle->arg = NULL;

      /* allocate memory for STDIO */
      appHandle->stdin  = (stdioFIFO_t *)Malloc(sizeof(stdioFIFO_t));
      appHandle->stdout = (stdioFIFO_t *)Malloc(sizeof(stdioFIFO_t));

      if (!appHandle->stdin || !appHandle->stdout)
      {
            if (!appHandle->stdin)
                  Free(appHandle->stdin);

            if (!appHandle->stdout)
                  Free(appHandle->stdout);

            appHandle->stdin  = NULL;
            appHandle->stdout = NULL;

            goto RunAsApp_end;
      }

      /* initialize stdio data */
      appHandle->arg              = arg;
      appHandle->exitCode         = STD_RET_UNKNOWN;
      appHandle->stdin->Level     = 0;
      appHandle->stdin->RxIdx     = 0;
      appHandle->stdin->TxIdx     = 0;
      appHandle->stdout->Level    = 0;
      appHandle->stdout->RxIdx    = 0;
      appHandle->stdout->TxIdx    = 0;
      appHandle->ParentTaskHandle = TaskGetCurrentTaskHandle();

      /* start application task */
      if (TaskCreate(app, appName, stackSize, appHandle, 2, appHandle->ChildTaskHandle) != pdPASS)
      {
            Free(appHandle->stdin);
            Free(appHandle->stdout);
            Free(appHandle);
            appHandle = NULL;
            goto RunAsApp_end;
      }

      *status = STD_RET_OK;

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
 * @param[out] *status        status
 *
 * @return application handler
 */
//================================================================================================//
appArgs_t *RunAsDaemon(pdTASK_CODE app, const ch_t *appName, u32_t stackSize, void *arg, stdRet_t *status)
{
      *status = STD_RET_ALLOCERROR;
      appArgs_t *appHandle = NULL;

      /* check pointers values */
      if (!app|| !appName  || !stackSize || !status)
            goto RunAsDaemon_end;

      /* allocate memory for application handler */
      appHandle = (appArgs_t *)Malloc(sizeof(appArgs_t));

      if (!appHandle)
            goto RunAsDaemon_end;

      /* set default values */
      appHandle->arg              = arg;
      appHandle->exitCode         = STD_RET_UNKNOWN;
      appHandle->stdin            = NULL;
      appHandle->stdout           = NULL;
      appHandle->ParentTaskHandle = TaskGetCurrentTaskHandle();

      /* start daemon task */
      if (TaskCreate(app, appName, stackSize, appHandle, 2, appHandle->ChildTaskHandle) != pdPASS)
      {
            Free(appHandle);
            appHandle = NULL;
            goto RunAsDaemon_end;
      }

      status = STD_RET_OK;

      RunAsDaemon_end:
            return appHandle;
}


//================================================================================================//
/**
 * @brief Run task as application using only task name
 *
 * @param[in]  *name          task name
 * @param[in]  *argv          task arguments
 * @param[out] *status        status
 *
 * @return application handler
 */
//================================================================================================//
appArgs_t *Exec(const ch_t *name, ch_t *argv, stdRet_t *status)
{
      regAppData_t appData = REGAPP_GetAppData(name);

      if (appData.appPtr == NULL)
      {
            *status = STD_RET_ERROR;
            return NULL;
      }

      return RunAsApp(appData.appPtr, appData.appName, appData.stackSize, argv, status);
}


//================================================================================================//
/**
 * @brief Run task as daemon using only task name
 *
 * @param[in]  *name          task name
 * @param[in]  *argv          task arguments
 * @param[out] *status        status
 *
 * @return application handler
 */
//================================================================================================//
appArgs_t *Execd(const ch_t *name, ch_t *argv, stdRet_t *status)
{
      regAppData_t appData = REGAPP_GetAppData(name);

      if (appData.appPtr == NULL)
      {
            *status = STD_RET_ERROR;
            return NULL;
      }

      return RunAsDaemon(appData.appPtr, appData.appName, appData.stackSize, argv, status);
}


//================================================================================================//
/**
 * @brief Function freed STDIO buffers
 *
 * @param *appArgs      pointer to the appArgs structure which contains pointers to the STDIO
 *
 * @retval STD_STATUS_OK            freed success
 * @retval STD_STATUS_ERROR         freed error, bad pointer
 */
//================================================================================================//
stdRet_t FreeAppStdio(appArgs_t *appArgs)
{
      if (appArgs)
      {
            if (appArgs->stdin)
                  Free(appArgs->stdin);

            if (appArgs->stdout)
                  Free(appArgs->stdout);

            Free(appArgs);

            return STD_RET_OK;
      }
      else
      {
            return STD_RET_ERROR;
      }
}


//================================================================================================//
/**
 * @brief Terminate application
 *
 * @param *stdio              stdio
 * @param exitCode            return value
 */
//================================================================================================//
void TerminateApplication(appArgs_t *appArgument, stdRet_t exitCode)
{
      u32_t killTries = 100;

      while (killTries)
      {
            if (appArgument->stdout->Level)
                  TaskDelay(10);
            else
                  break;

            killTries--;
      }

      /* set exit code */
      appArgument->exitCode = exitCode;

      TaskTerminate();
}


/*==================================================================================================
                                            End of file
==================================================================================================*/
