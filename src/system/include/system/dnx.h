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

#ifndef _DNX_H_
#define _DNX_H_

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
#include "core/sysmoni.h"
#include "core/modctrl.h"
#include "core/progman.h"
#include "kernel/kwrapper.h"
#include "kernel/khooks.h"
#include "portable/cpuctl.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  External object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern void dnx_init(void);

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief Function start kernel scheduler
 */
//==============================================================================
static inline void kernel_start(void)
{
        _kernel_start();
}

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
 * @brief Function return tick counter
 *
 * @return a tick counter value
 */
//==============================================================================
static inline int get_tick_counter(void)
{
        return _kernel_get_tick_counter();
}

//==============================================================================
/**
 * @brief Function return OS time in milliseconds
 *
 * @return a OS time in milliseconds
 */
//==============================================================================
static inline int get_time_ms(void)
{
        return _kernel_get_time_ms();
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
 * @brief Function return name of CPU
 *
 * @return pointer to CPU name string
 */
//==============================================================================
static inline const char *get_platform_name(void)
{
        return _CPUCTL_PLATFORM_NAME;
}

//==============================================================================
/**
 * @brief Function return name of operating system
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
 * @brief Function return operating system version
 *
 * @return pointer to string
 */
//==============================================================================
static inline const char *get_OS_version(void)
{
        return "1.2.0";
}

//==============================================================================
/**
 * @brief Function return name of kernel
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
 * @brief Function return name of author
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
 * @brief Function return author's email
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
 * @brief Function return kernel version
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
 * @brief Function return host name
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
        extern const int _regdrv_number_of_modules;
        return _regdrv_number_of_modules;
}

//==============================================================================
/**
 * @brief Function disable CPU load measurement
 */
//==============================================================================
static inline void disable_CPU_load_measurement(void)
{
        sysm_disable_CPU_load_measurement();
}

//==============================================================================
/**
 * @brief Function enable CPU load measurement
 */
//==============================================================================
static inline void enable_CPU_load_measurement(void)
{
        sysm_enable_CPU_load_measurement();
}

//==============================================================================
/**
 * @brief Function restart system
 */
//==============================================================================
static inline void restart_system(void)
{
        _cpuctl_restart_system();
}

//==============================================================================
/**
 * @brief Function start new program by name
 *
 * Errno: EINVAL, ENOMEM, ENOENT
 *
 * @param cmd           program name and argument list
 * @param stin          standard input file
 * @param stout         standard output file
 * @param sterr         standard error file
 *
 * @return NULL if error, otherwise program handle
 */
//==============================================================================
static inline task_t *program_new(const char *cmd, const char *cwd, FILE *stin, FILE *stout, FILE *sterr)
{
        return _program_new(cmd, cwd, stin, stout, sterr);
}

//==============================================================================
/**
 * @brief Kill started program
 *
 * @param prog                  program object
 *
 * @return 0 if success, otherwise other value
 */
//==============================================================================
static inline int program_kill(prog_t *prog)
{
        return _program_kill(prog);
}

//==============================================================================
/**
 * @brief Function delete running program
 *
 * @param prog                  program object
 *
 * @return 0 on success, otherwise other value
 */
//==============================================================================
static inline int program_delete(prog_t *prog)
{
        return _program_delete(prog);
}

//==============================================================================
/**
 * @brief Wait for program close
 *
 * @param prog                  program object
 * @param timeout               wait timeout
 *
 * @return 0 if closed, otherwise other value
 */
//==============================================================================
static inline int program_wait_for_close(prog_t *prog, const uint timeout)
{
        return _program_wait_for_close(prog, timeout);
}

//==============================================================================
/**
 * @brief Check if program is closed
 *
 * @param prog                  program object
 *
 * @return true if program closed, otherwise false
 */
//==============================================================================
static inline bool program_is_closed(prog_t *prog)
{
        return _program_is_closed(prog);
}

#ifdef __cplusplus
}
#endif

#endif /* _DNX_H_ */
/*==============================================================================
  End of file
==============================================================================*/
