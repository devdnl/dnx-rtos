#ifndef APPMONI_H_
#define APPMONI_H_
/*=========================================================================*//**
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


*//*==========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "systypes.h"
#include "memman.h"
#include "vfs.h"
#include "config.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/* USER CFG: enable (1) or disable (0) task memory usage monitoring */
#define TSK_MONITOR_MEMORY_USAGE                CONFIG_MONITOR_MEMORY_USAGE

/* USER CFG: enable (1) or disable (0) task opened file monitoring */
#define TSK_MONITOR_FILE_USAGE                  CONFIG_MONITOR_FILE_USAGE

/* USER CFG: enable (1) or disable (0) task CPU load monitoring */
#define TSK_MONITOR_CPU_LOAD                    CONFIG_MONITOR_CPU_LOAD

/* ---------------------------------------------------------------------------*/
/* DIRECT FUNCTIONS IF MONITORING IS DISABLED */
#if (TSK_MONITOR_MEMORY_USAGE == 0)
#define tskm_malloc(size)                       memman_malloc(size)
#define tskm_calloc(nmemb, msize)               memman_calloc(nmemb, msize)
#define tskm_free(mem)                          memman_free(mem)
#endif

/* DIRECT FUNCTIONS IF MONITORING IS DISABLED */
#if (TSK_MONITOR_FILE_USAGE == 0)
#define tskm_fopen(path, mode)                  vfs_fopen(path, mode)
#define tskm_fclose(file)                       vfs_fclose(file)
#define tskm_opendir(path)                      vfs_opendir(path)
#define tskm_closedir(dir)                      vfs_closedir(dir)
#endif

/* DIRECT FUNCTIONS IF MONITORING DISABLED */
#if (TSK_MONITOR_CPU_LOAD == 0)
#define tskm_TaskSwitchedIn()
#define tskm_TaskSwitchedOut()
#endif

/* DIRECT FUNCTION BECAUSE MONITORING IS NOT NECESSARY */
#define tskm_umount(path)                       vfs_umount(path)
#define tskm_getmntentry(item, mntentPtr)       vfs_getmntentry(item, mntentPtr)
#define tskm_mknod(path, drv_cfgPtr)            vfs_mknod(path, drv_cfgPtr)
#define tskm_mkdir(path)                        vfs_mkdir(path)
#define tskm_readdir(dir)                       vfs_readdir(dir)
#define tskm_remove(path)                       vfs_remove(path)
#define tskm_rename(oldName, newName)           vfs_rename(oldName, newName)
#define tskm_chmod(path, mode)                  vfs_chmod(path, mode)
#define tskm_chown(path, owner, group)          vfs_chown(path, owner, group)
#define tskm_stat(path, statPtr)                vfs_stat(path, statPtr)
#define tskm_statfs(path, statfsPtr)            vfs_statfs(path, statfsPtr)
#define tskm_fwrite(ptr, isize, nitems, file)   vfs_fwrite(ptr, isize, nitems, file)
#define tskm_fread(ptr, isize, nitems, file)    vfs_fread(ptr, isize, nitems, file)
#define tskm_fseek(file, offset, mode)          vfs_fseek(file, offset, mode)
#define tskm_ftell(file)                        vfs_ftell(file)
#define tskm_ioctl(file, rq, data)              vfs_ioctl(file, rq, data)
#define tskm_fstat(file, statPtr)               vfs_fstat(file, stat)

/* IF MONITOR MODULE IS NOT USED DISABLE INITIALIZATION */
#if (  (TSK_MONITOR_MEMORY_USAGE == 0) \
    && (TSK_MONITOR_FILE_USAGE == 0  ) \
    && (TSK_MONITOR_CPU_LOAD == 0    ) )
#define tskm_Init()
#define tskm_AddTask(pid)
#define tskm_DelTask(pid)
#define tskm_GetTaskStat(item, statPtr)         !memset(statPtr, 0, sizeof(struct taskstat))
#define tskm_GetTaskHdlStat(hdl, statPtr)       !memset(statPtr, 0, sizeof(struct taskstat))
#define tskm_GetTaskCount()                     0
#endif

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
struct taskstat {
        u32_t  memory_usage;
        u32_t  opened_files;
        u32_t  cpu_usage;
        u32_t  cpu_usage_total;
        ch_t  *task_name;
        task_t task_handle;
        u32_t  free_stack;
        i16_t  priority;
};

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
#if (  (TSK_MONITOR_MEMORY_USAGE > 0) \
    || (TSK_MONITOR_FILE_USAGE > 0  ) \
    || (TSK_MONITOR_CPU_LOAD > 0    ) )
extern stdRet_t  tskm_add_task         (task_t taskHdl);
extern stdRet_t  tskm_remove_task      (task_t taskHdl);
extern stdRet_t  tskm_get_ntask_stat   (i32_t item, struct taskstat *stat);
extern stdRet_t  tskm_get_task_stat    (task_t taskHdl, struct taskstat *stat);
extern u16_t     tskm_get_task_count   (void);
#endif
#if (TSK_MONITOR_MEMORY_USAGE > 0)
extern void     *tskm_malloc           (u32_t size);
extern void     *tskm_calloc           (u32_t nmemb, u32_t msize);
extern void      tskm_free             (void *mem);
#endif
#if (TSK_MONITOR_FILE_USAGE > 0)
extern FILE_t   *tskm_fopen            (const ch_t *path, const ch_t *mode);
extern stdRet_t  tskm_fclose           (FILE_t *file);
extern DIR_t    *tskm_opendir          (const ch_t *path);
extern stdRet_t  tskm_closedir         (DIR_t *dir);
#endif
#if (TSK_MONITOR_CPU_LOAD > 0)
extern void      tskm_task_switched_in (void);
extern void      tskm_task_switched_out(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* APPMONI_H_ */
/*==============================================================================
  End of file
==============================================================================*/
