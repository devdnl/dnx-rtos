#ifndef APPRUNTIME_H_
#define APPRUNTIME_H_
/*=============================================================================================*//**
@file    appruntime.h

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

#ifdef __cplusplus
   extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "system.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/
/** simpler definition of terminating application */
#define Exit(exitCode)                    TerminateApplication(stdout, exitCode)


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern appArgs_t   *RunAsApp(pdTASK_CODE app, ch_t *appName, u32_t stackSize, void *arg);
extern appArgs_t   *RunAsDaemon(pdTASK_CODE app, ch_t *appName, u32_t stackSize, void *arg);
extern stdStatus_t FreeAppStdio(appArgs_t *appArgs);
extern void        TerminateApplication(stdioFIFO_t *stdout, stdStatus_t exitCode);


#ifdef __cplusplus
   }
#endif

#endif /* APPRUNTIME_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
