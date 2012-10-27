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
#include "basic_types.h"
#include "systypes.h"
#include "projdefs.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/
/** simpler definition of terminating application */
#define Exit(exitCode)                    TerminateApplication(appArgument, exitCode)


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/
/** type which define parse possiblities */
typedef enum parseType_enum
{
      PARSE_AS_BIN,
      PARSE_AS_OCT,
      PARSE_AS_DEC,
      PARSE_AS_HEX,
      PARSE_AS_STRING,
      PARSE_AS_CHAR,
      PARSE_AS_EXIST,
      PARSE_AS_UNKNOWN
} parseType_t;


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern app_t    *RunAsApp(pdTASK_CODE app, const ch_t *appName, u32_t stackSize, void *arg);
extern app_t    *RunAsDaemon(pdTASK_CODE app, const ch_t *appName, u32_t stackSize, void *arg);
extern app_t    *Exec(const ch_t *name, ch_t *argv);
extern app_t    *Execd(const ch_t *name, ch_t *argv);
extern stdRet_t StartDeamon(const ch_t *name, ch_t *argv);
extern stdRet_t FreeApphdl(app_t *appArgs);
extern void     TerminateApplication(app_t *appArgument, stdRet_t exitCode);
extern stdRet_t ParseArg(ch_t *argv, ch_t *findArg, parseType_t parseAs, void *result);

#ifdef __cplusplus
}
#endif

#endif /* APPRUNTIME_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
