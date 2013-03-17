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
#define TSKM_MONITOR_MEMORY_USAGE                CONFIG_MONITOR_MEMORY_USAGE

/* USER CFG: enable (1) or disable (0) task opened file monitoring */
#define TSKM_MONITOR_FILE_USAGE                  CONFIG_MONITOR_FILE_USAGE

/* USER CFG: enable (1) or disable (0) task CPU load monitoring */
#define TSKM_MONITOR_CPU_LOAD                    CONFIG_MONITOR_CPU_LOAD

/* ---------------------------------------------------------------------------*/
/* DIRECT FUNCTIONS IF MONITORING IS DISABLED */
#if (TSKM_MONITOR_MEMORY_USAGE == 0)
#define tskm_malloc_as(taskhdl, size)           memman_malloc(size, NULL)
#define tskm_calloc_as(taskhdl, nmemb, msize)   memman_calloc(nmemb, msize, NULL)
#define tskm_free_as(taskhdl, mem)              memman_free(mem)
#define tskm_malloc(size)                       memman_malloc(size, NULL)
#define tskm_calloc(nmemb, msize)               memman_calloc(nmemb, msize, NULL)
#define tskm_freemem_as(taskhdl, mem, size)     memman_free(mem)
#define tskm_free(mem)                          memman_free(mem)
#endif

/* DIRECT FUNCTIONS IF MONITORING IS DISABLED */
#if (TSKM_MONITOR_FILE_USAGE == 0)
#define tskm_fopen(path, mode)                  vfs_fopen(path, mode)
#define tskm_fclose(file)                       vfs_fclose(file)
#define tskm_opendir(path)                      vfs_opendir(path)
#define tskm_closedir(dir)                      vfs_closedir(dir)
#endif

/* DIRECT FUNCTIONS IF MONITORING DISABLED */
#if (TSKM_MONITOR_CPU_LOAD == 0)
#define tskm_task_switched_in()
#define tskm_task_switched_out()
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
#if (  (TSKM_MONITOR_MEMORY_USAGE == 0) \
    && (TSKM_MONITOR_FILE_USAGE == 0  ) \
    && (TSKM_MONITOR_CPU_LOAD == 0    ) )
#define tskm_init()
#define tskm_is_task_exist(taskhdl)             TRUE
#define tskm_start_task_monitoring(taskhdl)     STD_RET_OK
#define tskm_stop_task_monitoring(taskhdl)      STD_RET_OK
#define tskm_get_total_CPU_usage()              0
#define tskm_clear_total_CPU_usage()            NULL()
#define tskm_get_task_stat(taskhdl, statPtr)    !memset(statPtr, 0, sizeof(struct taskstat))
#define tskm_get_ntask_stat(item, statPtr)      !memset(statPtr, 0, sizeof(struct taskstat))
#define tskm_get_number_of_monitored_tasks()    0
#endif

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
struct taskstat {
        u32_t   memory_usage;
        u32_t   opened_files;
        u32_t   cpu_usage;
        char   *task_name;
        task_t *task_handle;
        u32_t   free_stack;
        i16_t   priority;
};

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
#if (  (TSKM_MONITOR_MEMORY_USAGE > 0) \
    || (TSKM_MONITOR_FILE_USAGE > 0  ) \
    || (TSKM_MONITOR_CPU_LOAD > 0    ) )
extern stdret_t tskm_init(void);
extern bool_t   tskm_is_task_exist(task_t*);
extern stdret_t tskm_start_task_monitoring(task_t*);
extern stdret_t tskm_stop_task_monitoring(task_t*);
extern u32_t    tskm_get_total_CPU_usage(void);
extern void     tskm_clear_total_CPU_usage(void);
extern stdret_t tskm_get_task_stat(task_t*, struct taskstat*);
extern stdret_t tskm_get_ntask_stat(i32_t, struct taskstat*);
extern int      tskm_get_number_of_monitored_tasks(void);
#endif

#if (TSKM_MONITOR_MEMORY_USAGE > 0)
extern stdret_t tskm_enable_fast_memory_monitoring(task_t*);
extern void    *tskm_malloc_as(task_t*, u32_t);
extern void    *tskm_malloc(u32_t);
extern void    *tskm_calloc_as(task_t*, u32_t, u32_t);
extern void    *tskm_calloc(u32_t, u32_t);
extern void     tskm_free_as(task_t*, void*);
extern void     tskm_freemem_as(task_t*, void*, u32_t);
extern void     tskm_free(void*);
#endif

#if (TSKM_MONITOR_FILE_USAGE > 0)
extern file_t  *tskm_fopen(const char*, const char*);
extern stdret_t tskm_fclose(file_t*);
extern dir_t   *tskm_opendir(const char*);
extern stdret_t tskm_closedir(dir_t*);
#endif

#if (TSKM_MONITOR_CPU_LOAD > 0)
extern void tskm_task_switched_in (void);
extern void tskm_task_switched_out(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* APPMONI_H_ */
/*==============================================================================
  End of file
==============================================================================*/
