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
 * @param[in] app                   application function
 * @param[in] *appName              application name
 * @param[in] stackSize             application stack size
 * @param[in] *arg                  application arguments
 *
 * @return pointer to the structure with pointers to the STDIO and arguments
 */
//================================================================================================//
appArgs_t *RunAsApp(pdTASK_CODE app, ch_t *appName, u32_t stackSize, void *arg)
{
      appArgs_t *appArgs = (appArgs_t *)Malloc(sizeof(appArgs_t));

      if (!appArgs)
            goto StartApplication_end;
      else
            appArgs->arg = NULL;

      appArgs->stdin  = (stdioFIFO_t *)Malloc(sizeof(stdioFIFO_t));
      appArgs->stdout = (stdioFIFO_t *)Malloc(sizeof(stdioFIFO_t));

      if (!appArgs->stdin || !appArgs->stdout)
      {
            if (!appArgs->stdin)
                  Free(appArgs->stdin);

            if (!appArgs->stdout)
                  Free(appArgs->stdout);

            appArgs->stdin  = NULL;
            appArgs->stdout = NULL;
      }
      else
      {
            /* initialize stdio data */
            appArgs->arg           = arg;
            appArgs->stdin->Level  = 0;
            appArgs->stdin->RxIdx  = 0;
            appArgs->stdin->TxIdx  = 0;
            appArgs->stdout->Level = 0;
            appArgs->stdout->RxIdx = 0;
            appArgs->stdout->TxIdx = 0;

            /* start application task */
            if (TaskCreate(app, appName, stackSize, appArgs, 2, appArgs->taskHandle) != pdPASS)
            {
                  Free(appArgs->stdin);
                  Free(appArgs->stdout);
                  Free(appArgs);
                  appArgs = NULL;
            }
      }

      StartApplication_end:
            return appArgs;
}


//================================================================================================//
/**
 * @brief This function start task as daemon
 *
 * @param[in] app                   application function
 * @param[in] *appName              application name
 * @param[in] stackSize             application stack size
 * @param[in] *arg                  application arguments
 *
 * @return pointer to the structure with pointers to the STDIO and arguments
 */
//================================================================================================//
appArgs_t *RunAsDaemon(pdTASK_CODE app, ch_t *appName, u32_t stackSize, void *arg)
{
      appArgs_t *appArgs = (appArgs_t *)Malloc(sizeof(appArgs_t));

      if (appArgs)
      {
            appArgs->stdin  = NULL;
            appArgs->stdout = NULL;
            appArgs->arg    = arg;

            /* start daemon task */
            if (TaskCreate(app, appName, stackSize, appArgs, 2, appArgs->taskHandle) != pdPASS)
            {
                  Free(appArgs);
                  appArgs = NULL;
            }
      }

      return appArgs;
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
stdStatus_t FreeAppStdio(appArgs_t *appArgs)
{
      if (appArgs)
      {
            if (appArgs->stdin)
                  Free(appArgs->stdin);

            if (appArgs->stdout)
                  Free(appArgs->stdout);

            Free(appArgs);

            return STD_STATUS_OK;
      }
      else
      {
            return STD_STATUS_ERROR;
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
void TerminateApplication(stdioFIFO_t *stdout, stdStatus_t exitCode)
{
      TaskDelay(10);
      fclearSTDIO(stdout);
      fputChar(stdout, exitCode);
      TaskTerminate();
}


/*==================================================================================================
                                            End of file
==================================================================================================*/
