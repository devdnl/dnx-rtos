#ifndef APPMONI_H_
#define APPMONI_H_
/*=============================================================================================*//**
@file    appmoni.h

@author  Daniel Zorychta

@brief   This module is used to monitoring all applications

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
#include "systypes.h"
#include "memman.h"
#include "vfs.h"
#include "oswrap.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/
/* USER CFG: enable (1) or disable (0) application memory usage monitoring */
#define APP_MONITOR_MEMORY_USAGE                (1)

/* USEF CFG: enable (1) or disable (0) application opened file monitoring */
#define APP_MONITOR_FILE_USAGE                  (1)

/* ---------------------------------------------------------------------------------------------- */
/* DIRECT FUNCTIONS IF MONITORING IS DISABLED */
#if (APP_MONITOR_MEMORY_USAGE == 0)
#define moni_malloc(size)                       mm_malloc(size)
#define moni_calloc(nmemb, msize)               mm_calloc(nmemb, msize)
#define moni_free(mem)                          mm_free(mem)
#endif

/* DIRECT FUNCTIONS IF MONITORING IS DISABLED */
#if (APP_MONITOR_FILE_USAGE == 0)
#define moni_fopen(path, mode)                  vfs_fopen(path, mode)
#define moni_fclose(file)                       vfs_fclose(file)
#endif

/* DIRECT FUNCTION BECAUSE MONITORING IS NOT NECESSARY */
#define moni_mount(path, fs_cfgPtr)             vfs_mount(path, fs_cfgPtr)
#define moni_umount(path)                       vfs_umount(path)
#define moni_getmntentry(item, mntentPtr)       vfs_getmntentry(item, mntentPtr)
#define moni_mknod(path, drv_cfgPtr)            vfs_mknod(path, drv_cfgPtr)
#define moni_mkdir(path)                        vfs_mkdir(path)
#define moni_opendir(path)                      vfs_opendir(path)
#define moni_closedir(dir)                      vfs_closedir(dir)
#define moni_readdir(dir)                       vfs_readdir(dir)
#define moni_remove(path)                       vfs_remove(path)
#define moni_rename(oldName, newName)           vfs_rename(oldName, newName)
#define moni_chmod(path, mode)                  vfs_chmod(path, mode)
#define moni_chown(path, owner, group)          vfs_chown(path, owner, group)
#define moni_stat(path, statPtr)                vfs_stat(path, statPtr)
#define moni_statfs(path, statfsPtr)            vfs_statfs(path, statfsPtr)
#define moni_fwrite(ptr, isize, nitems, file)   vfs_fwrite(ptr, isize, nitems, file)
#define moni_fread(ptr, isize, nitems, file)    vfs_fread(ptr, isize, nitems, file)
#define moni_fseek(file, offset, mode)          vfs_fseek(file, offset, mode)
#define moni_ftell(file)                        vfs_ftell(file)
#define moni_ioctl(file, rq, data)              vfs_ioctl(file, rq, data)
#define moni_fstat(file, statPtr)               vfs_fstat(file, stat)

/* IF MONITOR MODULE IS NOT USED DISABLE INITIALIZATION */
#if ((APP_MONITOR_MEMORY_USAGE == 0) && (APP_MONITOR_FILE_USAGE == 0))
#define moni_init()
#define moni_addTask(pid);
#define moni_delTask(pid);
#define moni_getTaskStat(item, statPtr);
#endif

/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/
struct taskstat {
      u32_t  memUsage;
      u32_t  fileUsage;
      u32_t  cpuUsage;
      u32_t  cpuUsageTotal;
      ch_t  *taskName;
      task_t taskHdl;
      u32_t  taskFreeStack;
      i16_t  taskPriority;
};


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
#if ((APP_MONITOR_MEMORY_USAGE > 0) || (APP_MONITOR_FILE_USAGE > 0))
extern stdRet_t moni_Init       (void);
extern stdRet_t moni_AddTask    (task_t taskHdl);
extern stdRet_t moni_DelTask    (task_t taskHdl);
extern stdRet_t moni_GetTaskStat(i32_t item, struct taskstat *stat);
#endif

#if (APP_MONITOR_MEMORY_USAGE > 0)
extern void *moni_malloc(u32_t size);
extern void *moni_calloc(u32_t nmemb, u32_t msize);
extern void  moni_free  (void *mem);
#endif

#if (APP_MONITOR_FILE_USAGE > 0)
extern FILE_t   *moni_fopen(const ch_t *path, const ch_t *mode);
extern stdRet_t  moni_fclose(FILE_t *file);
#endif

#ifdef __cplusplus
}
#endif

#endif /* APPMONI_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
