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
#define malloc(size)                            sysm_tskmalloc(size)
#define calloc(nitems, isize)                   sysm_tskcalloc(nitems, isize)
#define free(mem)                               sysm_tskfree(mem)

/** ENVIRONMENT DEFINITIONS */
#define get_used_static_memory()                (CONFIG_RAM_SIZE - CONFIG_HEAP_SIZE)
#define get_free_memory()                       memman_get_free_heap()
#define get_used_memory()                       (get_used_static_memory() + (CONFIG_HEAP_SIZE - memman_get_free_heap()))
#define get_memory_size()                       CONFIG_RAM_SIZE
#define get_uptime()                            get_uptime_counter()
#define get_task_stat(ntask, statPtr)           sysm_get_ntask_stat(ntask, statPtr)
#define get_number_of_monitored_tasks()         sysm_get_number_of_monitored_tasks()
#define get_total_CPU_usage()                   sysm_get_total_CPU_usage()
#define clear_total_CPU_usage()                 sysm_clear_total_CPU_usage()
#define reboot()                                cpuctl_restart_system()
#define get_platform_name()                     CPUCTL_PLATFORM_NAME
#define get_OS_name()                           "dnx"
#define get_OS_version()                        "0.9.17"
#define get_kernel_name()                       "FreeRTOS"
#define get_kernel_version()                    tskKERNEL_VERSION_NUMBER
#define get_host_name()                         CONFIG_HOSTNAME
#define getcwd(buf, size)                       strncpy(buf, _get_this_task_data()->f_cwd, size)
#define get_used_memory_by_kernel()             sysm_get_used_kernel_memory()
#define get_used_memory_by_system()             sysm_get_used_system_memory()
#define get_used_memory_by_modules()            sysm_get_used_modules_memory()
#define get_used_memory_by_programs()           sysm_get_used_program_memory()
#define get_module_memory_usage(modid)          sysm_get_module_used_memory(modid)
#define get_module_name(modid)                  regdrv_get_module_name(modid)
#define get_module_count()                      REGDRV_NUMBER_OF_REGISTERED_MODULES

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
