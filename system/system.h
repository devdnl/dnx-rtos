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
/** BASIC DEFINITIONS */
#define malloc(size)                      mm_malloc(size)
#define calloc(nitems, isize)             mm_calloc(nitems, isize)
#define free(mem)                         mm_free(mem)

/** FILE SYSTEM DEFINITIONS */
#define mount(node, path)                 vfs_mount(node, path)
#define umount(path)                      vfs_umount(path)
#define fopen(path, mode)                 vfs_fopen(path, mode)
#define fclose(file)                      vfs_fclose(file)
#define fwrite(ptr, isize, nitems, file)  vfs_fwrite(ptr, isize, nitems, file)
#define fread(ptr, isize, nitems, file)   vfs_fread(ptr, isize, nitems, file)
#define fseek(file, offset, mode)         vfs_fseek(file, offset, mode)
#define ioctl(file, rq, data)             vfs_ioctl(file, rq, data)
#define opendir(path)                     vfs_opendir(path)
#define readdir(dir)                      vfs_readdir(dir)
#define closedir(dir)                     vfs_closedir(dir)
#define remove(direntry)                  vfs_remove(direntry)
#define rename(oldName, newName)          vfs_rename(oldName, newName)

/** APPLICATION LEVEL DEFINITIONS */
#define Sleep(delay)                      TaskDelay(delay)
#define SleepUntil(lastTime, sleepTime)   TaskDelayUntil(lastTime, sleepTime)
#define SystemGetStackFreeSpace()         TaskGetStackFreeSpace(THIS_TASK)
#define SystemEnterCritical()             TaskEnterCritical()
#define SystemExitCritical()              TaskExitCritical()
#define SystemDisableIRQ()                TaskDisableIRQ()
#define SystemEnableIRQ()                 TaskEnableIRQ()
#define SystemGetPID()                    TaskGetPID()
#define SystemGetAppHandle()              TaskGetCurrentTaskHandle()
#define SystemAppSuspend()                TaskSuspend(NULL)
#define SystemGetFreeMemSize()            mm_GetFreeHeapSize()
#define SystemGetUsedMemSize()            mm_GetUsedHeapSize()
#define SystemGetMemSize()                MEMMAN_HEAP_SIZE
#define SystemGetHostname()               LwIP_GetHostname()
#define SystemGetUptime()                 GetUptimeCnt()
#define SystemGetTaskCount()              TaskGetNumberOfTasks()
#define SystemGetRunTimeStats(dst)        TaskGetRunTimeStats(dst)
#define SystemGetOSTickCnt()              TaskGetTickCount()
#define SystemGetAppName()                TaskGetName(THIS_APP)

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
