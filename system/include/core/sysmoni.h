#ifndef SYSMONI_H_
#define SYSMONI_H_
/*=========================================================================*//**
@file    sysmoni.h

@author  Daniel Zorychta

@brief   This module is used to monitoring system

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
#include "config.h"
#include "core/systypes.h"
#include "core/memman.h"
#include "core/vfs.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/* USER CFG: enable (1) or disable (0) kernel memory usage monitoring */
#define SYSM_MONITOR_KERNEL_MEMORY_USAGE        CONFIG_MONITOR_KERNEL_MEMORY_USAGE

/* USER CFG: enable (1) or disable (0) system memory usage monitoring */
#define SYSM_MONITOR_SYSTEM_MEMORY_USAGE        CONFIG_MONITOR_SYSTEM_MEMORY_USAGE

/* USER CFG: enable (1) or disable (0) module (driver) memory usage monitoring */
#define SYSM_MONITOR_MODULE_MEMORY_USAGE        CONFIG_MONITOR_MODULE_MEMORY_USAGE

/* USER CFG: enable (1) or disable (0) task memory usage monitoring */
#define SYSM_MONITOR_TASK_MEMORY_USAGE          CONFIG_MONITOR_TASK_MEMORY_USAGE

/* USER CFG: enable (1) or disable (0) task opened file monitoring */
#define SYSM_MONITOR_TASK_FILE_USAGE            CONFIG_MONITOR_TASK_FILE_USAGE

/* USER CFG: enable (1) or disable (0) task CPU load monitoring */
#define SYSM_MONITOR_CPU_LOAD                   CONFIG_MONITOR_CPU_LOAD

/* ---------------------------------------------------------------------------*/
#define sysm_umount(path)                       vfs_umount(path)
#define sysm_getmntentry(item, mntentPtr)       vfs_getmntentry(item, mntentPtr)
#define sysm_mknod(path, drv_cfgPtr)            vfs_mknod(path, drv_cfgPtr)
#define sysm_mkdir(path)                        vfs_mkdir(path)
#define sysm_readdir(dir)                       vfs_readdir(dir)
#define sysm_remove(path)                       vfs_remove(path)
#define sysm_rename(oldName, newName)           vfs_rename(oldName, newName)
#define sysm_chmod(path, mode)                  vfs_chmod(path, mode)
#define sysm_chown(path, owner, group)          vfs_chown(path, owner, group)
#define sysm_stat(path, statPtr)                vfs_stat(path, statPtr)
#define sysm_statfs(path, statfsPtr)            vfs_statfs(path, statfsPtr)
#define sysm_fwrite(ptr, isize, nitems, file)   vfs_fwrite(ptr, isize, nitems, file)
#define sysm_fread(ptr, isize, nitems, file)    vfs_fread(ptr, isize, nitems, file)
#define sysm_fseek(file, offset, mode)          vfs_fseek(file, offset, mode)
#define sysm_ftell(file)                        vfs_ftell(file)
#define sysm_ioctl(file, ...)                   vfs_ioctl(file, __VA_ARGS__)
#define sysm_fstat(file, statPtr)               vfs_fstat(file, stat)
#define sysm_fflush(file)                       vfs_fflush(file)
#define sysm_feof(file)                         vfs_feof(file)
#define sysm_rewind(file)                       vfs_rewind(file)

#if (SYSM_MONITOR_TASK_MEMORY_USAGE == 0)
#define sysm_tskmalloc_as(taskhdl, size)        memman_malloc(size, NULL)
#define sysm_tskcalloc_as(taskhdl, nmemb, msize)memman_calloc(nmemb, msize, NULL)
#define sysm_tskfree_as(taskhdl, mem)           memman_free(mem)
#define sysm_tskmalloc(size)                    memman_malloc(size, NULL)
#define sysm_tskcalloc(nmemb, msize)            memman_calloc(nmemb, msize, NULL)
#define sysm_tskfreemem_as(taskhdl, mem, size)  memman_free(mem)
#define sysm_tskfree(mem)                       memman_free(mem)
#define sysm_get_used_program_memory()          0
#endif

#if (SYSM_MONITOR_KERNEL_MEMORY_USAGE == 0)
#define sysm_kmalloc(size)                      memman_malloc(size, NULL)
#define sysm_kcalloc(count, size)               memman_calloc(count, size, NULL)
#define sysm_kfree(mem)                         memman_free(mem)
#define sysm_get_used_kernel_memory()           0
#endif

#if (SYSM_MONITOR_SYSTEM_MEMORY_USAGE == 0)
#define sysm_sysmalloc(size)                    memman_malloc(size, NULL)
#define sysm_syscalloc(count, size)             memman_calloc(count, size, NULL)
#define sysm_sysfree(mem)                       memman_free(mem)
#define sysm_get_used_system_memory()           0
#endif

#if (SYSM_MONITOR_MODULE_MEMORY_USAGE == 0)
#define sysm_modmalloc(size, mod)               memman_malloc(size, NULL)
#define sysm_modcalloc(count, size, mod)        memman_calloc(count, size, NULL)
#define sysm_modfree(mem, mod)                  memman_free(mem)
#define sysm_get_used_modules_memory()          0
#define sysm_get_module_used_memory(mod)        0
#endif

#if (SYSM_MONITOR_TASK_FILE_USAGE == 0)
#define sysm_fopen(path, mode)                  vfs_fopen(path, mode)
#define sysm_freopen(path, mode, file)          vfs_freopen(path, mode, file)
#define sysm_fclose(file)                       vfs_fclose(file)
#define sysm_opendir(path)                      vfs_opendir(path)
#define sysm_closedir(dir)                      vfs_closedir(dir)
#endif

#if (SYSM_MONITOR_CPU_LOAD == 0)
#define sysm_get_total_CPU_usage()              1
#define sysm_clear_total_CPU_usage()
#define sysm_task_switched_in()
#define sysm_task_switched_out()
#endif

#if (  (SYSM_MONITOR_TASK_MEMORY_USAGE == 0 ) \
    && (SYSM_MONITOR_TASK_FILE_USAGE == 0   ) \
    && (SYSM_MONITOR_CPU_LOAD == 0          ) )
#define sysm_init()                             STD_RET_OK
#endif

/* IF MONITOR MODULE IS NOT USED DISABLE INITIALIZATION */
#if (  (SYSM_MONITOR_TASK_MEMORY_USAGE == 0) \
    && (SYSM_MONITOR_TASK_FILE_USAGE == 0  ) \
    && (SYSM_MONITOR_CPU_LOAD == 0         ) )
#define sysm_is_task_exist(taskhdl)             TRUE
#define sysm_start_task_monitoring(taskhdl)     STD_RET_OK
#define sysm_stop_task_monitoring(taskhdl)      STD_RET_OK
#define sysm_get_task_stat(taskhdl, statPtr)    !memset(statPtr, 0, sizeof(struct taskstat))
#define sysm_get_ntask_stat(item, statPtr)      !memset(statPtr, 0, sizeof(struct taskstat))
#define sysm_get_number_of_monitored_tasks()    0
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
#if (  (SYSM_MONITOR_TASK_MEMORY_USAGE > 0  ) \
    || (SYSM_MONITOR_TASK_FILE_USAGE > 0    ) \
    || (SYSM_MONITOR_CPU_LOAD > 0           ) )
extern stdret_t sysm_init(void);
#endif

#if (  (SYSM_MONITOR_TASK_MEMORY_USAGE > 0) \
    || (SYSM_MONITOR_TASK_FILE_USAGE > 0  ) \
    || (SYSM_MONITOR_CPU_LOAD > 0         ) )
extern bool     sysm_is_task_exist(task_t*);
extern stdret_t sysm_start_task_monitoring(task_t*);
extern stdret_t sysm_stop_task_monitoring(task_t*);
extern stdret_t sysm_get_task_stat(task_t*, struct taskstat*);
extern stdret_t sysm_get_ntask_stat(i32_t, struct taskstat*);
extern int      sysm_get_number_of_monitored_tasks(void);
#endif

#if (SYSM_MONITOR_KERNEL_MEMORY_USAGE > 0)
extern void *sysm_kmalloc(size_t);
extern void *sysm_kcalloc(size_t, size_t);
extern void  sysm_kfree(void*);
extern i32_t sysm_get_used_kernel_memory(void);
#endif

#if (SYSM_MONITOR_SYSTEM_MEMORY_USAGE > 0)
extern void *sysm_sysmalloc(size_t);
extern void *sysm_syscalloc(size_t, size_t);
extern void  sysm_sysfree(void*);
extern i32_t sysm_get_used_system_memory(void);
#endif

#if (SYSM_MONITOR_MODULE_MEMORY_USAGE > 0)
extern void *sysm_modmalloc(size_t, uint);
extern void *sysm_modcalloc(size_t, size_t, uint);
extern void  sysm_modfree(void*, uint);
extern i32_t sysm_get_used_modules_memory(void);
extern i32_t sysm_get_module_used_memory(uint);
#endif

#if (SYSM_MONITOR_TASK_MEMORY_USAGE > 0)
extern void *sysm_tskmalloc_as(task_t*, u32_t);
extern void *sysm_tskcalloc_as(task_t*, u32_t, u32_t);
extern void  sysm_tskfree_as(task_t*, void*);
extern void *sysm_tskmalloc(u32_t);
extern void *sysm_tskcalloc(u32_t, u32_t);
extern void  sysm_tskfree(void*);
extern i32_t sysm_get_used_program_memory(void);
#endif

#if (SYSM_MONITOR_TASK_FILE_USAGE > 0)
extern file_t  *sysm_fopen(const char*, const char*);
extern file_t  *sysm_freopen(const char*, const char*, file_t*);
extern stdret_t sysm_fclose(file_t*);
extern dir_t   *sysm_opendir(const char*);
extern stdret_t sysm_closedir(dir_t*);
#endif

#if (SYSM_MONITOR_CPU_LOAD > 0)
extern u32_t sysm_get_total_CPU_usage(void);
extern void  sysm_clear_total_CPU_usage(void);
extern void  sysm_task_switched_in(void);
extern void  sysm_task_switched_out(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SYSMONI_H_ */
/*==============================================================================
  End of file
==============================================================================*/
