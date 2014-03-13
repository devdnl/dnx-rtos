/*=========================================================================*//**
@file    os.h

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

#ifndef _OS_H_
#define _OS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <string.h>
#include <stdarg.h>
#include "config.h"
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
typedef struct sysmoni_taskstat taskstat_t;
typedef struct sysmoni_used_memory memstat_t;

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  External object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
//==============================================================================
/**
 * @brief void dnx_init(void)
 * The function <b>dnx_init</b>() starts dnx initialization process.
 *
 * @param None
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/os.h>
 *
 * int main()
 * {
 *         dnx_init();
 *         kernel_start();
 *         return 0;
 * }
 */
//==============================================================================
extern void dnx_init(void);

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief void kernel_start(void)
 * The function <b>kernel_start</b>() starts kernel. This function must be
 * called after <b>dnx_init</b>().
 *
 * @param None
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/os.h>
 *
 * int main()
 * {
 *         dnx_init();
 *         kernel_start();
 *         return 0;
 * }
 */
//==============================================================================
static inline void kernel_start(void)
{
        _kernel_start();
}

//==============================================================================
/**
 * @brief u32_t get_used_static_memory(void)
 * The function <b>get_used_static_memory</b>() return used static memory
 * in bytes. Usage of static memory is determined by configuration.
 *
 * @param None
 *
 * @errors None
 *
 * @return Number of bytes reserved by static memory.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Used static memory: %d bytes\n", get_used_static_memory());
 *
 * // ...
 */
//==============================================================================
static inline u32_t get_used_static_memory(void)
{
        return (CONFIG_RAM_SIZE - CONFIG_HEAP_SIZE);
}

//==============================================================================
/**
 * @brief u32_t get_free_memory(void)
 * The function <b>get_free_memory</b>() return free memory in bytes. This is
 * the total amount of memory which can be used.
 *
 * @param None
 *
 * @errors None
 *
 * @return Free memory in bytes.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Free memory: %d bytes\n", get_free_memory());
 *
 * // ...
 */
//==============================================================================
static inline u32_t get_free_memory(void)
{
        return memman_get_free_heap();
}

//==============================================================================
/**
 * @brief u32_t get_used_memory(void)
 * The function <b>get_used_memory</b>() return used memory in bytes. This value
 * is a sum of static and dynamic allocated memory.
 *
 * @param None
 *
 * @errors None
 *
 * @return Used memory in bytes.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Used memory: %d bytes\n", get_used_memory());
 *
 * // ...
 */
//==============================================================================
static inline u32_t get_used_memory(void)
{
        return (get_used_static_memory() + (CONFIG_HEAP_SIZE - memman_get_free_heap()));
}

//==============================================================================
/**
 * @brief u32_t get_memory_size(void)
 * The function <b>get_memory_size</b>() return total memory size in bytes.
 *
 * @param None
 *
 * @errors None
 *
 * @return Total memory size in bytes.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Memory size: %d bytes\n", get_memory_size());
 *
 * // ...
 */
//==============================================================================
static inline u32_t get_memory_size(void)
{
        return CONFIG_RAM_SIZE;
}

//==============================================================================
/**
 * @brief stdret_t get_detailed_memory_usage(memstat_t *stat)
 * The function <b>get_detailed_memory_usage</b>() return detailed memory usage
 * pointed by <i>stat</i>. <b>memstat_t</b> structure:
 * <pre>
 * typedef struct {
 *         int used_kernel_memory;
 *         int used_system_memory;
 *         int used_network_memory;
 *         int used_modules_memory;
 *         int used_programs_memory;
 * } memstat_t;
 * </pre>
 *
 * @param stat      memory information
 *
 * @errors EINVAL
 *
 * @return Return <b>STD_RET_OK</b> on success. On error, <b>STD_RET_ERROR</b>
 * is returned, and <b>errno</b> is set appropriately.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * errno = 0;
 * memstat_t stat;
 * if (get_detailed_memory_usage(&stat) == STD_RET_OK) {
 *         printf("Used memory by kernel  : %d\n"
 *                "Used memory by system  : %d\n"
 *                "Used memory by network : %d\n"
 *                "Used memory by modules : %d\n"
 *                "Used memory by programs: %d\n",
 *                stat.used_kernel_memory,
 *                stat.used_system_memory,
 *                stat.used_network_memory,
 *                stat.used_modules_memory,
 *                stat.used_programs_memory);
 * } else {
 *         perror(NULL);
 * }
 *
 * // ...
 */
//==============================================================================
static inline stdret_t get_detailed_memory_usage(memstat_t *stat)
{
        return sysm_get_used_memory(stat);
}

//==============================================================================
/**
 * @brief i32_t get_module_memory_usage(uint module_number)
 * The function <b>get_module_memory_usage</b>() return memory usage by specified
 * module number <i>module_number</i>.
 *
 * @param module_number     module number
 *
 * @errors None
 *
 * @return On success, return amount of used memory by module in bytes.
 * On error, -1 is returned.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * uint number_of_modules = get_number_of_modules();
 * for (uint i = 0; i < number_of_modules; i++) {
 *         printf("%s : %d\n", get_module_name(i), get_module_memory_usage(i));
 * }
 *
 * // ...
 */
//==============================================================================
static inline i32_t get_module_memory_usage(uint module_number)
{
        return sysm_get_used_memory_by_module(module_number);
}

//==============================================================================
/**
 * @brief u32_t get_uptime(void)
 * The function <b>get_uptime</b>() return 32-bit uptime in seconds.
 *
 * @param None
 *
 * @errors None
 *
 * @return Uptime value in seconds.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("System works: %d seconds\n", get_uptime());
 *
 * // ...
 */
//==============================================================================
static inline u32_t get_uptime(void)
{
        return _get_uptime_counter();
}

//==============================================================================
/**
 * @brief uint get_tick_counter(void)
 * The function <b>get_tick_counter</b>() return number of system clock ticks.
 *
 * @param None
 *
 * @errors None
 *
 * @return Number of system's ticks.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Context was switched %d times\n", get_tick_counter());
 *
 * // ...
 */
//==============================================================================
static inline uint get_tick_counter(void)
{
        return _kernel_get_tick_counter();
}

//==============================================================================
/**
 * @brief int get_time_ms(void)
 * The function <b>get_time_ms</b>() return number of milliseconds which
 * elapsed after kernel start. Function is similar to <b>get_uptime</b>(), except
 * that return milliseconds instead of seconds. In this function the tick
 * counter value is calculated to milliseconds, what means that resolution of
 * this counter depends on system tick counter increase value.
 *
 * @param None
 *
 * @errors None
 *
 * @return System work time in milliseconds.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("System works by %d ms\n", get_time_ms());
 *
 * // ...
 */
//==============================================================================
static inline uint get_time_ms(void)
{
        return _kernel_get_time_ms();
}

//==============================================================================
/**
 * @brief stdret_t get_task_stat(uint ntask, taskstat_t *stat)
 * The function <b>get_task_stat</b>() return <i>ntask</i> task information
 * pointed by <i>stat</i>. <b>taskstat_t</b> structure:
 * <pre>
 * typedef struct {
 *        u32_t   memory_usage;
 *        u32_t   opened_files;
 *        u32_t   cpu_usage;
 *        char   *task_name;
 *        task_t *task_handle;
 *        u32_t   free_stack;
 *        i16_t   priority;
 * } taskstat_t;
 * </pre>
 *
 * @param ntask     task number
 * @param stat      task information
 *
 * @errors None
 *
 * @return Return <b>STD_RET_OK</b> on success. On error, <b>STD_RET_ERROR</b>
 * is returned.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * uint number_of_tasks = get_number_of_monitored_tasks();
 * for (uint i = 0; i < number_of_tasks; i++) {
 *         taskstat_t stat;
 *         get_task_stat(i, &stat);
 *         printf("Memory usage: %d\n", stat.memory_usage);
 * }
 *
 * // ...
 */
//==============================================================================
static inline stdret_t get_task_stat(uint ntask, taskstat_t *stat)
{
        return sysm_get_ntask_stat(ntask, stat);
}

//==============================================================================
/**
 * @brief uint get_number_of_monitored_tasks(void)
 * The function <b>get_number_of_monitored_tasks</b>() return number of
 * monitored tasks. Real task number is larger by 1, because idle task is not
 * added to monitoring.
 *
 * @param None
 *
 * @errors None
 *
 * @return Return number of monitored tasks.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * uint number_of_tasks = get_number_of_monitored_tasks();
 * for (uint i = 0; i < number_of_tasks; i++) {
 *         taskstat_t stat;
 *         get_task_stat(i, &stat);
 *         printf("Memory usage: %d\n", stat.memory_usage);
 * }
 *
 * // ...
 */
//==============================================================================
static inline uint get_number_of_monitored_tasks(void)
{
        return sysm_get_number_of_monitored_tasks();
}

//==============================================================================
/**
 * @brief u32_t get_total_CPU_usage(void)
 * The function <b>get_total_CPU_usage</b>() return total CPU usage. The value
 * is a counter. After call of this function counter is reset.
 *
 * @param None
 *
 * @errors None
 *
 * @return CPU usage counter.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * u32_t total_cpu_usage = get_total_CPU_usage();
 * uint  number_of_tasks = get_number_of_monitored_tasks();
 *
 * for (uint i = 0; i < number_of_tasks; i++) {
 *         taskstat_t stat;
 *         get_task_stat(i, &stat);
 *
 *         printf("Task %s CPU usage: %d\n",
 *                get_task_name(i),
 *                (stat.cpu_usage * 100)  / total_cpu_usage);
 * }
 *
 * // ...
 */
//==============================================================================
static inline u32_t get_total_CPU_usage(void)
{
        return sysm_get_total_CPU_usage();
}

//==============================================================================
/**
 * @brief const char *get_platform_name(void)
 * The function <b>get_platform_name</b>() return platform name.
 *
 * @param None
 *
 * @errors None
 *
 * @return Platform name.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Platform name: %s\n", get_platform_name());
 *
 * // ...
 */
//==============================================================================
static inline const char *get_platform_name(void)
{
        return _CPUCTL_PLATFORM_NAME;
}

//==============================================================================
/**
 * @brief const char *get_OS_name(void)
 * The function <b>get_OS_name</b>() return operating system name.
 *
 * @param None
 *
 * @errors None
 *
 * @return Operating system name.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Operating system name: %s\n", get_OS_name());
 *
 * // ...
 */
//==============================================================================
static inline const char *get_OS_name(void)
{
        return "dnx";
}

//==============================================================================
/**
 * @brief const char *get_OS_version(void)
 * The function <b>get_OS_version</b>() return operating system version string.
 *
 * @param None
 *
 * @errors None
 *
 * @return Operating system version string.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Operating system version: %s\n", get_OS_version());
 *
 * // ...
 */
//==============================================================================
static inline const char *get_OS_version(void)
{
        return "1.3.0";
}

//==============================================================================
/**
 * @brief const char *get_kernel_name(void)
 * The function <b>get_kernel_name</b>() return kernel name string.
 *
 * @param None
 *
 * @errors None
 *
 * @return Kernel name string.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Kernel name: %s\n", get_kernel_name());
 *
 * // ...
 */
//==============================================================================
static inline const char *get_kernel_name(void)
{
        return "FreeRTOS";
}

//==============================================================================
/**
 * @brief const char *get_kernel_version(void)
 * The function <b>get_kernel_version</b>() return kernel version string.
 *
 * @param None
 *
 * @errors None
 *
 * @return Kernel version string.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Kernel version: %s\n", get_kernel_version());
 *
 * // ...
 */
//==============================================================================
static inline const char *get_kernel_version(void)
{
        return tskKERNEL_VERSION_NUMBER;
}

//==============================================================================
/**
 * @brief const char *get_author_name(void)
 * The function <b>get_author_name</b>() return author name.
 *
 * @param None
 *
 * @errors None
 *
 * @return Author name.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Author name: %s\n", get_author_name());
 *
 * // ...
 */
//==============================================================================
static inline const char *get_author_name(void)
{
        return "Daniel Zorychta";
}

//==============================================================================
/**
 * @brief const char *get_author_email(void)
 * The function <b>get_author_email</b>() return author's email address.
 *
 * @param None
 *
 * @errors None
 *
 * @return Author's email address.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Author's email: %s\n", get_author_email());
 *
 * // ...
 */
//==============================================================================
static inline const char *get_author_email(void)
{
        return "<daniel.zorychta@gmail.com>";
}

//==============================================================================
/**
 * @brief const char *get_host_name(void)
 * The function <b>get_host_name</b>() return host name.
 *
 * @param None
 *
 * @errors None
 *
 * @return Return host name.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Host name: %s\n", get_host_name());
 *
 * // ...
 */
//==============================================================================
static inline const char *get_host_name(void)
{
        return CONFIG_HOSTNAME;
}

//==============================================================================
/**
 * @brief const char *get_user_name(void)
 * The function <b>get_user_name</b>() return current user name. Function
 * return always "root" name, because user handling is not supported yet.
 *
 * @param None
 *
 * @errors None
 *
 * @return Return user name.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("User name: %s\n", get_user_name());
 *
 * // ...
 */
//==============================================================================
static inline const char *get_user_name(void)
{
        return "root";
}

//==============================================================================
/**
 * @brief const char *get_module_name(uint modid)
 * The function <b>get_module_name</b>() return module name selected by <i>modid</i>
 * index.
 *
 * @param modid     module ID
 *
 * @errors None
 *
 * @return Return module name.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Module name: %s\n", get_module_name(0));
 *
 * // ...
 */
//==============================================================================
static inline const char *get_module_name(uint modid)
{
        return _get_module_name(modid);
}

//==============================================================================
/**
 * @brief int get_module_number(const char *name)
 * The function <b>get_module_number</b>() return module index selected by
 * name pointed by <i>name</i>.
 *
 * @param name     module name
 *
 * @errors None
 *
 * @return On success, return module index (ID). On error, -1 is returned.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * printf("Module ID: %d\n", get_module_number("crc"));
 *
 * // ...
 */
//==============================================================================
static inline int get_module_number(const char *name)
{
        return _get_module_number(name);
}

//==============================================================================
/**
 * @brief uint get_number_of_modules(void)
 * The function <b>get_number_of_modules</b>() return number of registered
 * modules in system.
 *
 * @param None
 *
 * @errors None
 *
 * @return Return number of registered modules in system.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * uint number_of_modules = get_number_of_modules();
 * for (uint i = 0; i < number_of_modules; i++) {
 *         printf("%s : %d\n", get_module_name(i), get_module_memory_usage(i));
 * }
 *
 * // ...
 */
//==============================================================================
static inline uint get_number_of_modules(void)
{
        return _get_number_of_modules();
}

//==============================================================================
/**
 * @brief uint get_number_of_drivers(void)
 * The function <b>get_number_of_drivers</b>() return number of drivers. Each
 * module can contains many drivers e.g. UART module can handle UART1, UART2, etc.
 *
 * @param None
 *
 * @errors None
 *
 * @return Return number of drivers registered in system.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * uint number_of_drivers = get_number_of_drivers();
 * for (uint i = 0; i < number_of_drivers; i++) {
 *         printf("%s in module %s\n", get_driver_name(i), get_driver_module_name(i));
 * }
 *
 * // ...
 */
//==============================================================================
static inline uint get_number_of_drivers(void)
{
        return _get_number_of_drivers();
}

//==============================================================================
/**
 * @brief const char *get_driver_name(uint n)
 * The function <b>get_driver_name</b>() return driver name selected by <i>n</i>
 * index (ID).
 *
 * @param n     driver index
 *
 * @errors None
 *
 * @return Returns driver's name.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * uint number_of_drivers = get_number_of_drivers();
 * for (uint i = 0; i < number_of_drivers; i++) {
 *         printf("%s in module %s\n", get_driver_name(i), get_driver_module_name(i));
 * }
 *
 * // ...
 */
//==============================================================================
static inline const char *get_driver_name(uint n)
{
        return _get_driver_name(n);
}

//==============================================================================
/**
 * @brief const char *get_driver_module_name(uint n)
 * The function <b>get_driver_module_name</b>() return name of module which
 * contains driver indexed by <i>n</i>.
 *
 * @param n     driver index
 *
 * @errors None
 *
 * @return Returns name of module which contains driver.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * uint number_of_drivers = get_number_of_drivers();
 * for (uint i = 0; i < number_of_drivers; i++) {
 *         printf("%s in module %s\n", get_driver_name(i), get_driver_module_name(i));
 * }
 *
 * // ...
 */
//==============================================================================
static inline const char *get_driver_module_name(uint n)
{
        return _get_driver_module_name(n);
}

//==============================================================================
/**
 * @brief bool is_driver_active(uint n)
 * The function <b>is_driver_active</b>() return <b>true</b> if driver is activated,
 * otherwise <b>false</b>.
 *
 * @param n     driver index
 *
 * @errors None
 *
 * @return Returns <b>true</b> if driver is activated, otherwise <b>false</b>.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * if (is_driver_active(0)) {
 *         // ...
 * } else {
 *         // ...
 * }
 *
 * // ...
 */
//==============================================================================
static inline bool is_driver_active(uint n)
{
        return _is_driver_active(n);
}

//==============================================================================
/**
 * @brief void disable_CPU_load_measurement(void)
 * The function <b>disable_CPU_load_measurement</b>() disable CPU load
 * measurement.
 *
 * @param None
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * disable_CPU_load_measurement();
 *
 * u32_t total_cpu_usage = get_total_CPU_usage();
 * uint  number_of_tasks = get_number_of_monitored_tasks();
 *
 * for (uint i = 0; i < number_of_tasks; i++) {
 *         taskstat_t stat;
 *         get_task_stat(i, &stat);
 *
 *         printf("Task %s CPU usage: %d%%\n",
 *                get_task_name(i),
 *                (stat.cpu_usage * 100)  / total_cpu_usage);
 * }
 *
 * enable_CPU_load_measurement();
 *
 * // ...
 */
//==============================================================================
static inline void disable_CPU_load_measurement(void)
{
        sysm_disable_CPU_load_measurement();
}

//==============================================================================
/**
 * @brief void enable_CPU_load_measurement(void)
 * The function <b>enable_CPU_load_measurement</b>() enable CPU load
 * measurement.
 *
 * @param None
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * disable_CPU_load_measurement();
 *
 * u32_t total_cpu_usage = get_total_CPU_usage();
 * uint  number_of_tasks = get_number_of_monitored_tasks();
 *
 * for (uint i = 0; i < number_of_tasks; i++) {
 *         taskstat_t stat;
 *         get_task_stat(i, &stat);
 *
 *         printf("Task %s CPU usage: %d%%\n",
 *                get_task_name(i),
 *                (stat.cpu_usage * 100)  / total_cpu_usage);
 * }
 *
 * enable_CPU_load_measurement();
 *
 * // ...
 */
//==============================================================================
static inline void enable_CPU_load_measurement(void)
{
        sysm_enable_CPU_load_measurement();
}

//==============================================================================
/**
 * @brief void restart_system(void)
 * The function <b>restart_system</b>() restart system. Reset operation can
 * be software or hardware, but this depends on CPU port implementation.
 *
 * @param None
 *
 * @errors None
 *
 * @return This function not return.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * // e.g. user requests system restart
 * restart_system();
 */
//==============================================================================
static inline void restart_system(void)
{
        _cpuctl_restart_system();
}

//==============================================================================
/**
 * @brief task_t *program_new(const char *cmd, const char *cwd, FILE *stin, FILE *stout, FILE *sterr)
 * The function <b>program_new</b>() create new program pointed by <i>cmd</i>
 * with current working directory pointed by <i>cwd</i>. Standard input of
 * program is pointed by <i>stin</i>, standard output is pointed by <i>stout</i>
 * and standard error output is pointed by <i>sterr</i>.
 *
 * @param cmd           program name and argument list
 * @param stin          standard input file
 * @param stout         standard output file
 * @param sterr         standard error file
 *
 * @errors ENOMEM, EINVAL, ENOENT
 *
 * @return On success, return program object instance, otherwise NULL.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * errno = 0;
 * task_t *prog = program_new("ls /", "/", stdin, stdout, stderr);
 * if (prog) {
 *         program_wait_for_close(prog, MAX_DELAY_MS);
 *
 *         program_delete(prog);
 * } else {
 *         perror("Program not started");
 *
 *         // ...
 * }
 *
 * // ...
 */
//==============================================================================
static inline task_t *program_new(const char *cmd, const char *cwd, FILE *stin, FILE *stout, FILE *sterr)
{
        return _program_new(cmd, cwd, stin, stout, sterr);
}

//==============================================================================
/**
 * @brief int program_kill(prog_t *prog)
 * The function <b>program_kill</b>() kill working program. After kill object
 * must be deleted using <b>program_delete</b>().
 *
 * @param prog                  program object
 *
 * @errors EINVAL
 *
 * @return On success return 0, otherwise other than 0 value.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * errno = 0;
 * task_t *prog = program_new("ls /", "/", stdin, stdout, stderr);
 * if (prog) {
 *         if (program_wait_for_close(prog, 500) != 0) {
 *                 program_kill(prog);
 *         }
 *
 *         program_delete(prog);
 * } else {
 *         perror("Program not started");
 *
 *         // ...
 * }
 *
 * // ...
 */
//==============================================================================
static inline int program_kill(prog_t *prog)
{
        return _program_kill(prog);
}

//==============================================================================
/**
 * @brief int program_delete(prog_t *prog)
 * The function <b>program_delete</b>() delete closed program object. If program
 * is already running then object deletion is denied.
 *
 * @param prog                  program object
 *
 * @errors EINVAL, EAGAIN
 *
 * @return Return 0 on success. On error, different than 0 is returned, and
 * <b>errno</b> is set appropriately.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * errno = 0;
 * task_t *prog = program_new("ls /", "/", stdin, stdout, stderr);
 * if (prog) {
 *         program_wait_for_close(prog, MAX_DELAY_MS);
 *
 *         program_delete(prog);
 * } else {
 *         perror("Program not started");
 *
 *         // ...
 * }
 *
 * // ...
 */
//==============================================================================
static inline int program_delete(prog_t *prog)
{
        return _program_delete(prog);
}

//==============================================================================
/**
 * @brief int program_wait_for_close(prog_t *prog, const uint timeout)
 * The function <b>program_wait_for_close</b>() wait for program close.
 *
 * @param prog                  program object
 * @param timeout               wait timeout in ms
 *
 * @errors EINVAL, ETIME
 *
 * @return Return 0 on success. On error, different than 0 is returned, and
 * <b>errno</b> is set appropriately.
 *
 * @example
 * #include <dnx/os.h>
 *
 * // ...
 *
 * errno = 0;
 * task_t *prog = program_new("ls /", "/", stdin, stdout, stderr);
 * if (prog) {
 *         program_wait_for_close(prog, MAX_DELAY_MS);
 *
 *         program_delete(prog);
 * } else {
 *         perror("Program not started");
 *
 *         // ...
 * }
 *
 * // ...
 */
//==============================================================================
static inline int program_wait_for_close(prog_t *prog, const uint timeout)
{
        return _program_wait_for_close(prog, timeout);
}

//==============================================================================
/**
 * @brief bool program_is_closed(prog_t *prog)
 * The function <b>program_is_closed</b>() check if program is closed. This
 * function can be used to poll program status.
 *
 * @param prog                  program object
 *
 * @errors None
 *
 * @return Return <b>true</b> if closed. If is not closed then <b>false</b> is
 * returned.
 *
 * @example
 * #include <dnx/os.h>
 * #include <unistd.h>
 *
 * // ...
 *
 * errno = 0;
 * task_t *prog = program_new("ls /", "/", stdin, stdout, stderr);
 * if (prog) {
 *         while (!program_is_closed(prog)) {
 *                 sleep(1);
 *         }
 *
 *         program_delete(prog);
 * } else {
 *         perror("Program not started");
 *
 *         // ...
 * }
 *
 * // ...
 */
//==============================================================================
static inline bool program_is_closed(prog_t *prog)
{
        return _program_is_closed(prog);
}

#ifdef __cplusplus
}
#endif

#endif /* _OS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
