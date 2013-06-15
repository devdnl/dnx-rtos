#ifndef DNX_H_
#define DNX_H_
/*=========================================================================*//**
@file    dnx.h

@author  Daniel Zorychta

@brief   dnx system main header

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
#include <string.h>
#include <stdarg.h>
#include "config.h"
#include "core/systypes.h"
#include "core/memman.h"
#include "core/progman.h"
#include "core/sysmoni.h"
#include "core/io.h"
#include "user/regfs.h"
#include "user/regdrv.h"
#include "kernel/kwrapper.h"
#include "kernel/khooks.h"
#include "portable/cpuctl.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/** MEMORY MANAGEMENT DEFINTIONS */
#define malloc(size_t__size)                                    sysm_tskmalloc(size_t__size)
#define calloc(size_t__nitems, size_t__isize)                   sysm_tskcalloc(size_t__nitems, size_t__isize)
#define free(void__pmem)                                        sysm_tskfree(void__pmem)

/** ENVIRONMENT DEFINITIONS */
#define get_used_static_memory()                                (CONFIG_RAM_SIZE - CONFIG_HEAP_SIZE)
#define get_free_memory()                                       memman_get_free_heap()
#define get_used_memory()                                       (get_used_static_memory() + (CONFIG_HEAP_SIZE - memman_get_free_heap()))
#define get_memory_size()                                       CONFIG_RAM_SIZE
#define get_uptime()                                            _get_uptime_counter()
#define get_task_stat(i32_t__ntask, struct_taskstat__pstat)     sysm_get_ntask_stat(i32_t__ntask, struct_taskstat__pstat)
#define get_number_of_monitored_tasks()                         sysm_get_number_of_monitored_tasks()
#define get_total_CPU_usage()                                   sysm_get_total_CPU_usage()
#define clear_total_CPU_usage()                                 sysm_clear_total_CPU_usage()
#define restart()                                               _cpuctl_restart_system()
#define get_platform_name()                                     CPUCTL_PLATFORM_NAME
#define get_OS_name()                                           "dnx"
#define get_OS_version()                                        "0.9.25"
#define get_kernel_name()                                       "FreeRTOS"
#define get_kernel_version()                                    tskKERNEL_VERSION_NUMBER
#define get_host_name()                                         CONFIG_HOSTNAME
#define getcwd(char__pbuf, size_t__size)                        strncpy(char__pbuf, _get_this_task_data()->f_cwd, size_t__size)
#define get_used_memory_by_kernel()                             sysm_get_used_kernel_memory()
#define get_used_memory_by_system()                             sysm_get_used_system_memory()
#define get_used_memory_by_modules()                            sysm_get_used_modules_memory()
#define get_used_memory_by_programs()                           sysm_get_used_program_memory()
#define get_module_memory_usage(uint__modid)                    sysm_get_module_used_memory(uint__modid)
#define get_module_name(uint__modid)                            regdrv_get_module_name(uint__modid)
#define get_number_of_modules()                                 REGDRV_NUMBER_OF_REGISTERED_MODULES

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern void dnx_init(void);

#ifdef __cplusplus
}
#endif

#endif /* DNX_H_ */
/*==============================================================================
  End of file
==============================================================================*/
