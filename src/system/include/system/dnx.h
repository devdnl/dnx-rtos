#ifndef _DNX_H_
#define _DNX_H_
/*=========================================================================*//**
@file    dnx.h

@author  Daniel Zorychta

@brief   dnx system main header

@note    Copyright (C) 2012, 2013  Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "core/drivers.h"
#include "core/fsctrl.h"
#include "kernel/kwrapper.h"
#include "kernel/khooks.h"
#include "portable/cpuctl.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/* MEMORY MANAGEMENT DEFINTIONS */
#define malloc(size_t__size)                    sysm_tskmalloc(size_t__size)
#define calloc(size_t__nitems, size_t__isize)   sysm_tskcalloc(size_t__nitems, size_t__isize)
#define free(void__pmem)                        sysm_tskfree(void__pmem)

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  External object declarations
==============================================================================*/
extern const int _regdrv_number_of_modules;

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern void _dnx_init(void);

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief Function return used static memory
 *
 * @return used static memory (configured at precompilation time)
 */
//==============================================================================
static inline u32_t get_used_static_memory(void)
{
        return (CONFIG_RAM_SIZE - CONFIG_HEAP_SIZE);
}

//==============================================================================
/**
 * @brief Function return free memory
 *
 * @return a free memory
 */
//==============================================================================
static inline u32_t get_free_memory(void)
{
        return memman_get_free_heap();
}

//==============================================================================
/**
 * @brief Function return used memory
 *
 * @return used memory
 */
//==============================================================================
static inline u32_t get_used_memory(void)
{
        return (get_used_static_memory() + (CONFIG_HEAP_SIZE - memman_get_free_heap()));
}

//==============================================================================
/**
 * @brief Function return memory size
 *
 * @return a memory size
 */
//==============================================================================
static inline u32_t get_memory_size(void)
{
        return CONFIG_RAM_SIZE;
}

//==============================================================================
/**
 * @brief Function return detailed memory usage
 *
 * @param[out] *stat    memory usage object
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static inline stdret_t get_detailed_memory_usage(struct sysmoni_used_memory *stat)
{
        return sysm_get_used_memory(stat);
}

//==============================================================================
/**
 * @brief Function return memory usage of selected module
 *
 * @param[in] module_number
 *
 * @return used memory by selected module
 */
//==============================================================================
static inline i32_t get_module_memory_usage(uint module_number)
{
        return sysm_get_used_memory_by_module(module_number);
}

//==============================================================================
/**
 * @brief Function return uptime
 *
 * @return an uptime
 */
//==============================================================================
static inline u32_t get_uptime(void)
{
        return _get_uptime_counter();
}

//==============================================================================
/**
 * @brief Function return task status
 *
 * @param[in]   ntask           task number
 * @param[out] *stat            object for statistics
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static inline stdret_t get_task_stat(i32_t ntask, struct sysmoni_taskstat *stat)
{
        return sysm_get_ntask_stat(ntask, stat);
}

//==============================================================================
/**
 * @brief Function return a number of monitored tasks
 *
 * @return a number of monitored task
 */
//==============================================================================
static inline int get_number_of_monitored_tasks(void)
{
        return sysm_get_number_of_monitored_tasks();
}

//==============================================================================
/**
 * @brief Function return total CPU usage
 *
 * @return a CPU usage value
 */
//==============================================================================
static inline u32_t get_total_CPU_usage(void)
{
        return sysm_get_total_CPU_usage();
}

//==============================================================================
/**
 * @brief Function clear CPU usage
 */
//==============================================================================
static inline void clear_total_CPU_usage(void)
{
        sysm_clear_total_CPU_usage();
}

//==============================================================================
/**
 * @brief Function restart system
 */
//==============================================================================
static inline void restart(void)
{
        _cpuctl_restart_system();
}

//==============================================================================
/**
 * @brief Function return name of CPU
 *
 * @return pointer to CPU name string
 */
//==============================================================================
static inline const char *get_platform_name(void)
{
        return CPUCTL_PLATFORM_NAME;
}

//==============================================================================
/**
 * @brief Function return x name
 *
 * @return pointer to string
 */
//==============================================================================
static inline const char *get_OS_name(void)
{
        return "dnx";
}

//==============================================================================
/**
 * @brief Function return x name
 *
 * @return pointer to string
 */
//==============================================================================
static inline const char *get_OS_version(void)
{
        return "1.0.19";
}

//==============================================================================
/**
 * @brief Function return x name
 *
 * @return pointer to string
 */
//==============================================================================
static inline const char *get_kernel_name(void)
{
        return "FreeRTOS";
}

//==============================================================================
/**
 * @brief Function return x name
 *
 * @return pointer to string
 */
//==============================================================================
static inline const char *get_author_name(void)
{
        return "Daniel Zorychta";
}

//==============================================================================
/**
 * @brief Function return x name
 *
 * @return pointer to string
 */
//==============================================================================
static inline const char *get_author_email(void)
{
        return "<daniel.zorychta@gmail.com>";
}

//==============================================================================
/**
 * @brief Function return x name
 *
 * @return pointer to string
 */
//==============================================================================
static inline const char *get_kernel_version(void)
{
        return tskKERNEL_VERSION_NUMBER;
}

//==============================================================================
/**
 * @brief Function return x name
 *
 * @return pointer to string
 */
//==============================================================================
static inline const char *get_host_name(void)
{
        return CONFIG_HOSTNAME;
}

//==============================================================================
/**
 * @brief Function return x name
 *
 * @param[out] *buf     output buffer
 * @param[in]   size    buffer size
 *
 * @return buf pointer on success, otherwise NULL pointer
 */
//==============================================================================
static inline char *getcwd(char *buf, size_t size)
{
        return strncpy(buf, _get_this_task_data()->f_cwd, size);
}

//==============================================================================
/**
 * @brief Function return module name
 *
 * @param[in] modid     module ID
 *
 * @return module name
 */
//==============================================================================
static inline const char *get_module_name(uint modid)
{
        return _get_module_name(modid);
}

//==============================================================================
/**
 * @brief Function return number of modules
 *
 * @return number of modules
 */
//==============================================================================
static inline uint get_number_of_modules(void)
{
        return _regdrv_number_of_modules;
}

#ifdef __cplusplus
}
#endif

#endif /* _DNX_H_ */
/*==============================================================================
  End of file
==============================================================================*/
