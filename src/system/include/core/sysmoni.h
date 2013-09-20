#ifndef _SYSMONI_H_
#define _SYSMONI_H_
/*=========================================================================*//**
@file    sysmoni.h

@author  Daniel Zorychta

@brief   This module is used to monitoring system

@note    Copyright (C) 2012, 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
/* macro used by internal modules to stop system if condition is equal (!assert) */
#if CONFIG_SYSTEM_STOP_MACRO > 0
#define _stop_if(stop_condition)                if ((stop_condition)) {while(1);}
#else
#define _stop_if(stop_condition)
#endif

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
struct sysmoni_taskstat {
        u32_t   memory_usage;
        u32_t   opened_files;
        u32_t   cpu_usage;
        char   *task_name;
        task_t *task_handle;
        u32_t   free_stack;
        i16_t   priority;
};

struct sysmoni_used_memory {
        int used_kernel_memory;
        int used_system_memory;
        int used_modules_memory;
        int used_programs_memory;
};

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern stdret_t sysm_init                               (void);
extern bool     sysm_is_task_exist                      (task_t*);
extern stdret_t sysm_start_task_monitoring              (task_t*);
extern stdret_t sysm_stop_task_monitoring               (task_t*);
extern stdret_t sysm_get_task_stat                      (task_t*, struct sysmoni_taskstat*);
extern stdret_t sysm_get_ntask_stat                     (i32_t, struct sysmoni_taskstat*);
extern stdret_t sysm_get_used_memory                    (struct sysmoni_used_memory*);
extern int      sysm_get_number_of_monitored_tasks      (void);
extern void    *sysm_kmalloc                            (size_t);
extern void    *sysm_kcalloc                            (size_t, size_t);
extern void     sysm_kfree                              (void*);
extern void    *sysm_sysmalloc                          (size_t);
extern void    *sysm_syscalloc                          (size_t, size_t);
extern void     sysm_sysfree                            (void*);
extern void    *sysm_modmalloc                          (size_t, int);
extern void    *sysm_modcalloc                          (size_t, size_t, int);
extern void     sysm_modfree                            (void*, int);
extern i32_t    sysm_get_used_memory_by_module          (int);
extern void    *sysm_tskmalloc_as                       (task_t*, size_t);
extern void    *sysm_tskcalloc_as                       (task_t*, size_t, size_t);
extern void     sysm_tskfree_as                         (task_t*, void*);
extern void    *sysm_tskmalloc                          (size_t);
extern void    *sysm_tskcalloc                          (size_t, size_t);
extern void     sysm_tskfree                            (void*);
extern FILE    *sysm_fopen                              (const char*, const char*);
extern FILE    *sysm_freopen                            (const char*, const char*, FILE*);
extern int      sysm_fclose                             (FILE*);
extern DIR     *sysm_opendir                            (const char*);
extern int      sysm_closedir                           (DIR*);
extern u32_t    sysm_get_total_CPU_usage                (void);
extern void     sysm_disable_CPU_load_measurement       (void);
extern void     sysm_enable_CPU_load_measurement        (void);
extern void     sysm_task_switched_in                   (void);
extern void     sysm_task_switched_out                  (void);

#ifdef __cplusplus
}
#endif

#endif /* _SYSMONI_H_ */
/*==============================================================================
  End of file
==============================================================================*/
