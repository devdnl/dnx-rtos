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

#ifndef _SYSMONI_H_
#define _SYSMONI_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "config.h"
#include "core/memman.h"
#include "core/vfs.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
#define _KERNEL_PANIC_DESC_VALID1               0x7C63EE8E
#define _KERNEL_PANIC_DESC_VALID2               0xCF4C5484

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
struct _sysmoni_taskstat {
        u32_t   memory_usage;
        u32_t   opened_files;
        u32_t   cpu_usage;
        char   *task_name;
        task_t *task_handle;
        u32_t   free_stack;
        i16_t   priority;
};

struct _sysmoni_used_memory {
        int used_kernel_memory;
        int used_system_memory;
        int used_network_memory;
        int used_modules_memory;
        int used_programs_memory;
};


enum _kernel_panic_desc_cause {
        _KERNEL_PANIC_DESC_CAUSE_SEGFAULT = 0,
        _KERNEL_PANIC_DESC_CAUSE_STACKOVF = 1,
        _KERNEL_PANIC_DESC_CAUSE_CPUFAULT = 2,
        _KERNEL_PANIC_DESC_CAUSE_UNKNOWN  = 3
};

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern int    _sysm_init                         (void);
extern void   _sysm_lock_access                  (void);
extern void   _sysm_unlock_access                (void);
extern bool   _sysm_is_task_exist                (task_t*);
extern int    _sysm_start_task_monitoring        (task_t*, size_t);
extern int    _sysm_stop_task_monitoring         (task_t*);
extern int    _sysm_get_task_stat                (task_t*, struct _sysmoni_taskstat*);
extern int    _sysm_get_ntask_stat               (uint, struct _sysmoni_taskstat*);
extern int    _sysm_get_used_memory              (struct _sysmoni_used_memory*);
extern uint   _sysm_get_number_of_monitored_tasks(void);
extern void  *_sysm_kmalloc                      (size_t);
extern void  *_sysm_kcalloc                      (size_t, size_t);
extern void   _sysm_kfree                        (void*);
extern void  *_sysm_sysmalloc                    (size_t);
extern void  *_sysm_syscalloc                    (size_t, size_t);
extern void   _sysm_sysfree                      (void*);
extern void  *_sysm_netmalloc                    (size_t);
extern void  *_sysm_netcalloc                    (size_t, size_t);
extern void   _sysm_netfree                      (void*);
extern void  *_sysm_modmalloc                    (size_t, uint);
extern void  *_sysm_modcalloc                    (size_t, size_t, uint);
extern void   _sysm_modfree                      (void*, uint);
extern i32_t  _sysm_get_used_memory_by_module    (uint);
extern void  *_sysm_tskmalloc_as                 (task_t*, size_t);
extern void  *_sysm_tskcalloc_as                 (task_t*, size_t, size_t);
extern void   _sysm_tskfree_as                   (task_t*, void*);
extern void  *_sysm_tskmalloc                    (size_t);
extern void  *_sysm_tskcalloc                    (size_t, size_t);
extern void   _sysm_tskfree                      (void*);
extern FILE  *_sysm_fopen                        (const char*, const char*);
extern FILE  *_sysm_freopen                      (const char*, const char*, FILE*);
extern int    _sysm_fclose                       (FILE*);
extern DIR   *_sysm_opendir                      (const char*);
extern int    _sysm_closedir                     (DIR*);
extern u32_t  _sysm_get_total_CPU_usage          (void);
extern void   _sysm_disable_CPU_load_measurement (void);
extern void   _sysm_enable_CPU_load_measurement  (void);
extern void   _sysm_task_switched_in             (void);
extern void   _sysm_task_switched_out            (void);
extern bool   _sysm_kernel_panic_detect          (bool);
extern void   _sysm_kernel_panic_report          (const char*, enum _kernel_panic_desc_cause);

#ifdef __cplusplus
}
#endif

#endif /* _SYSMONI_H_ */
/*==============================================================================
  End of file
==============================================================================*/
