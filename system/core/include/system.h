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
#include "runtime.h"
#include "oshooks.h"
#include "dlist.h"
#include "taskmoni.h"

#if !defined(ARCH_posix)
#include "netconf.h"
#endif


/*==================================================================================================
                                  Exported symbolic constants/macros
==================================================================================================*/
/** MEMORY MANAGEMENT DEFINTIONS */
#define malloc(size)                      moni_malloc(size)
#define calloc(nitems, isize)             moni_calloc(nitems, isize)
#define free(mem)                         moni_free(mem)

/** FILE ACCESS MANAGEMENT */
#define mount(path, fs_cfgPtr)            moni_mount(path, fs_cfgPtr)
#define umount(path)                      moni_umount(path)
#define getmntentry(item, mntentPtr)      moni_getmntentry(item, mntentPtr)
#define mknod(path, drv_cfgPtr)           moni_mknod(path, drv_cfgPtr)
#define mkdir(path)                       moni_mkdir(path)
#define opendir(path)                     moni_opendir(path)
#define closedir(dir)                     moni_closedir(dir)
#define readdir(dir)                      moni_readdir(dir)
#define remove(path)                      moni_remove(path)
#define rename(oldName, newName)          moni_rename(oldName, newName)
#define chmod(path, mode)                 moni_chmod(path, mode)
#define chown(path, owner, group)         moni_chown(path, owner, group)
#define stat(path, statPtr)               moni_stat(path, statPtr)
#define statfs(path, statfsPtr)           moni_statfs(path, statfsPtr)
#define fopen(path, mode)                 moni_fopen(path, mode)
#define fclose(file)                      moni_fclose(file)
#define fwrite(ptr, isize, nitems, file)  moni_fwrite(ptr, isize, nitems, file)
#define fread(ptr, isize, nitems, file)   moni_fread(ptr, isize, nitems, file)
#define fseek(file, offset, mode)         moni_fseek(file, offset, mode)
#define ftell(file)                       moni_ftell(file)
#define ioctl(file, rq, data)             moni_ioctl(file, rq, data)
#define fstat(file, statPtr)              moni_fstat(file, stat)

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
#define SystemGetFreeMemSize()            memman_GetFreeHeapSize()
#define SystemGetUsedMemSize()            memman_GetUsedHeapSize()
#define SystemGetMemSize()                MEMMAN_HEAP_SIZE
#if defined(NETCONF_H_)
#define SystemGetHostname()               LwIP_GetHostname()
#else
#define SystemGetHostname()               "localhost"
#endif
#define SystemGetUptime()                 GetUptimeCnt()
#define SystemGetTaskCount()              TaskGetNumberOfTasks()
#define SystemGetOSTickCnt()              TaskGetTickCount()
#define SystemGetAppName()                TaskGetName(NULL)
#define SystemGetTaskStat(item, statPtr)  moni_GetTaskStat(item, statPtr)
#define SystemGetMoniTaskCount()          moni_GetTaskCount()
#define SystemReboot()                    cpuctl_SystemReboot()
#define SystemGetOSName()                 "dnx"
#define SystemGetKernelName()             "FreeRTOS"
#define SystemGetOSVersion()              "0.5.1"
#define SystemGetKernelVersion()          "7.1.1"


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
