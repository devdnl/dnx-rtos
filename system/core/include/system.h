#ifndef SYSTEM_H_
#define SYSTEM_H_
/*=========================================================================*//**
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


*//*==========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "basic_types.h"
#include "systypes.h"
#include "memman.h"
#include "oswrap.h"
#include "io.h"
#include "runtime.h"
#include "oshooks.h"
#include "dlist.h"
#include "taskmoni.h"
#include "regfs.h"
#include "regdrv.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/** MEMORY MANAGEMENT DEFINTIONS */
#define malloc(size)                      tskm_malloc(size)
#define calloc(nitems, isize)             tskm_calloc(nitems, isize)
#define free(mem)                         tskm_free(mem)

/** FILE ACCESS MANAGEMENT */
#define getmntentry(item, mntentPtr)      tskm_getmntentry(item, mntentPtr)
#define mknod(path, drv_cfgPtr)           tskm_mknod(path, drv_cfgPtr)
#define mkdir(path)                       tskm_mkdir(path)
#define opendir(path)                     tskm_opendir(path)
#define closedir(dir)                     tskm_closedir(dir)
#define readdir(dir)                      tskm_readdir(dir)
#define remove(path)                      tskm_remove(path)
#define rename(oldName, newName)          tskm_rename(oldName, newName)
#define chmod(path, mode)                 tskm_chmod(path, mode)
#define chown(path, owner, group)         tskm_chown(path, owner, group)
#define stat(path, statPtr)               tskm_stat(path, statPtr)
#define statfs(path, statfsPtr)           tskm_statfs(path, statfsPtr)
#define fopen(path, mode)                 tskm_fopen(path, mode)
#define fclose(file)                      tskm_fclose(file)
#define fwrite(ptr, isize, nitems, file)  tskm_fwrite(ptr, isize, nitems, file)
#define fread(ptr, isize, nitems, file)   tskm_fread(ptr, isize, nitems, file)
#define fseek(file, offset, mode)         tskm_fseek(file, offset, mode)
#define ftell(file)                       tskm_ftell(file)
#define ioctl(file, rq, data)             tskm_ioctl(file, rq, data)
#define fstat(file, statPtr)              tskm_fstat(file, stat)

/** PROGRAM LEVEL DEFINITIONS */
#define milisleep(msdelay)                TaskDelay(msdelay)
#define sleep(seconds)                    TaskDelay((seconds) * 1000)
#define SleepUntil(lastTime, sleepTime)   TaskDelayUntil(lastTime, sleepTime)
#define SystemGetStackFreeSpace()         TaskGetStackFreeSpace(THIS_TASK)
#define SystemEnterCritical()             TaskEnterCritical()
#define SystemExitCritical()              TaskExitCritical()
#define SystemDisableIRQ()                TaskDisableIRQ()
#define SystemEnableIRQ()                 TaskEnableIRQ()
#define SystemGetAppHandle()              TaskGetCurrentTaskHandle()
#define SystemAppSuspend()                TaskSuspend(NULL)
#define SystemGetFreeMemSize()            memman_get_free_heap()
#define SystemGetUsedMemSize()            memman_get_used_heap()
#define SystemGetMemSize()                MEMMAN_HEAP_SIZE
#define SystemGetUptime()                 GetUptimeCnt()
#define SystemGetTaskCount()              TaskGetNumberOfTasks()
#define SystemGetOSTickCnt()              TaskGetTickCount()
#define SystemGetAppName()                TaskGetName(NULL)
#define SystemGetTaskStat(item, statPtr)  tskm_get_ntask_stat(item, statPtr)
#define SystemGetMoniTaskCount()          tskm_get_task_count()
#define SystemReboot()                    cpuctl_system_restart()
#define SystemGetOSName()                 "dnx"
#define SystemGetKernelName()             "FreeRTOS"
#define SystemGetOSVersion()              "0.6.1"
#define SystemGetKernelVersion()          "7.3.0"
#define SystemGetHostname()               CONFIG_HOSTNAME
#define getcwd(buf, size)                 strncpy(buf, cwd, size)

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_H_ */
/*==============================================================================
  End of file
==============================================================================*/
