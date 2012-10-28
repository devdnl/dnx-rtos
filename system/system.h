#ifndef SYSTEM_H_
#define SYSTEM_H_
/*=============================================================================================*//**
@file    oswrap.h

@author  Daniel Zorychta

@brief   System main header

@note    Copyright (C) 2012  Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "memman.h"
#include "oswrap.h"
#include "print.h"
#include "appruntime.h"
#include "cpu.h"
#include "regdrv.h"
#include "proc.h"
#include "vfs.h"
#include "hooks.h"
#include "netconf.h"


/*==================================================================================================
                                  Exported symbolic constants/macros
==================================================================================================*/
/** APPLICATION LEVEL DEFINITIONS */
#define Sleep(delay)                      TaskDelay(delay)
#define SystemGetStackFreeSpace()         TaskGetStackFreeSpace(THIS_TASK)
#define SystemEnterCritical()             TaskEnterCritical()
#define SystemExitCritical()              TaskExitCritical()
#define SystemDisableIRQ()                TaskDisableIRQ()
#define SystemEnableIRQ()                 TaskEnableIRQ()
#define SystemGetPID()                    TaskGetPID()
#define SystemGetAppHandle()              TaskGetCurrentTaskHandle()
#define SystemAppSuspend()                TaskSuspend(NULL)
#define SystemGetFreeMemSize()            GetFreeHeapSize()
#define SystemGetUsedMemSize()            GetUsedHeapSize()
#define SystemGetMemSize()                MEMMAN_HEAP_SIZE
#define SystemGetHostname()               LwIP_GetHostname()
#define SystemGetUptime()                 GetUptimeCnt()
#define SystemGetTaskCount()              TaskGetNumberOfTasks()
#define SystemGetRunTimeStats(dst)        TaskGetRunTimeStats(dst)

/** application preamble */
#define APPLICATION(name)                 void name(void *appArgument)
#define APP_SEC_BEGIN                     { InitApp();
#define APP_SEC_END                       Exit(appmain(argv));}

#define InitApp()                         FILE_t *stdin  = ((app_t*)appArgument)->stdin;  \
                                          FILE_t *stdout = ((app_t*)appArgument)->stdout; \
                                          ch_t   *argv   = ((app_t*)appArgument)->arg;    \
                                          (void)stdin; (void)stdout; (void)argv


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/


#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_H_ */
/*==================================================================================================
                                             End of file
==================================================================================================*/
