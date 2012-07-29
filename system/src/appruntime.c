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
 * @brief This function start selected application
 *
 * @param[in] app                   application function
 * @param[in] *appName              application name
 * @param[in] stackSize             application stack size
 * @param[in] *arg                  application arguments
 *
 * @retval NULL if error occur otherwise correct pointer
 */
//================================================================================================//
stdio_t *StartApplication(pdTASK_CODE app, ch_t *appName, u32_t stackSize, void *arg)
{
      stdio_t *stdioPtr = NULL;

      /* allocate memory for stdio structure */
      stdioPtr = (stdio_t*)Malloc(sizeof(stdio_t));

      if (stdioPtr != NULL)
      {
            /* initialize stdio data */
            stdioPtr->arg          = arg;
            stdioPtr->stdin.Level  = 0;
            stdioPtr->stdin.RxIdx  = 0;
            stdioPtr->stdin.TxIdx  = 0;
            stdioPtr->stdout.Level = 0;
            stdioPtr->stdout.RxIdx = 0;
            stdioPtr->stdout.TxIdx = 0;

            /* start application task */
            TaskCreate(app, appName, stackSize, stdioPtr, 2, NULL);
      }

      return stdioPtr;
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
      fclearSTDIO(stdout);
      fputChar(stdout, exitCode);
      TaskTerminate();
}


/*==================================================================================================
                                            End of file
==================================================================================================*/
