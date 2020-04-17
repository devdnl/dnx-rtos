/*=========================================================================*//**
@file    os.h

@author  Daniel Zorychta

@brief   dnx system main header

@note    Copyright (C) 2012, 2013  Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes dnx RTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


*//*==========================================================================*/

/**
\defgroup dnx-os-h <dnx/os.h>

This library contains macros and functions related directly with dnx RTOS system.

*/
/**@{*/

#ifndef _OS_H_
#define _OS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <config.h>
#include <stdbool.h>
#include <kernel/syscall.h>
#include <kernel/khooks.h>
#include <kernel/process.h>
#include <kernel/printk.h>
#include <mm/mm.h>
#include <drivers/drvctrl.h>

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
#ifndef _COMMIT_HASH
#define _COMMIT_HASH ""
#endif

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
#ifdef DOXYGEN
/**
 * @brief Memory usage details
 *
 * The type contains details of dynamic and static memory usage
 *
 * @see get_memory_usage_details()
 */
typedef struct {
        i32_t static_memory_usage;      /*!< The amount of memory that is used statically at build time.*/
        i32_t kernel_memory_usage;      /*!< The amount of memory used by kernel.*/
        i32_t filesystems_memory_usage; /*!< The amount of memory used by file systems.*/
        i32_t network_memory_usage;     /*!< The amount of memory used by network subsystem.*/
        i32_t modules_memory_usage;     /*!< The amount of memory used by modules (drivers).*/
        i32_t programs_memory_usage;    /*!< The amount of memory used by users' programs (applications).*/
        i32_t shared_memory_usage;      /*!< The amount of memory used by shared buffers.*/
        i32_t cached_memory_usage;      /*!< The anount of memory used by disc caches.*/
} memstat_t;
#else
typedef _mm_mem_usage_t memstat_t;
#endif

#ifdef DOXYGEN
/**
 * @brief Average CPU load
 *
 * The type contains average CPU load in 1, 5, and 15 minute periods. Values are
 * presented in tens of percents (1% is 10).
 *
 * @see get_average_CPU_load()
 */
typedef struct {
        u16_t avg1sec;                  /*!< average CPU laod within 1 second (1% = 10).*/
        u16_t avg1min;                  /*!< average CPU load within 1 minute (1% = 10).*/
        u16_t avg5min;                  /*!< average CPU load within 5 minutes (1% = 10).*/
        u16_t avg15min;                 /*!< average CPU load within 15 minutes (1% = 10).*/
} avg_CPU_load_t;
#endif

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  External object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief Function returns an amount of free memory.
 *
 * The function get_free_memory() return free memory in bytes. This is
 * the total amount of memory which can be used.
 *
 * @return Free memory in bytes.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("Free memory: %d bytes\n", get_free_memory());

        // ...

   @endcode
 */
//==============================================================================
static inline u32_t get_free_memory(void)
{
        return _builtinfunc(mm_get_mem_free);
}

//==============================================================================
/**
 * @brief Function returns an amount of used memory.
 *
 * The function get_used_memory() return used RAM in bytes. This value
 * is a sum of static and dynamic allocated memory.
 *
 * @return Used memory in bytes.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("Used memory: %d bytes\n", get_used_memory());

        // ...

   @endcode
 */
//==============================================================================
static inline u32_t get_used_memory(void)
{
        return _builtinfunc(mm_get_mem_usage);
}

//==============================================================================
/**
 * @brief Function returns the size of RAM.
 *
 * The function get_memory_size() return total memory size in bytes.
 *
 * @return Total memory size in bytes.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("Memory size: %d bytes\n", get_memory_size());

        // ...

   @endcode
 */
//==============================================================================
static inline u32_t get_memory_size(void)
{
        return _builtinfunc(mm_get_mem_size);
}

//==============================================================================
/**
 * @brief Function returns memory usage details.
 *
 * The function get_memory_usage_details() return detailed memory usage
 * pointed by <i>stat</i>.
 *
 * @param stat      memory information
 *
 * @exception | @ref EINVAL
 *
 * @return Return @b 0 on success. On error, @b positive value
 * is returned, and @b errno is set appropriately.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        errno = 0;
        memstat_t stat;
        if (get_memory_usage_details(&stat) == STD_RET_OK) {
                printf("Used memory by kernel      : %d\n"
                       "Used memory by file system : %d\n"
                       "Used memory by network     : %d\n"
                       "Used memory by modules     : %d\n"
                       "Used memory by applications: %d\n",
                       stat.kernel_memory_usage,
                       stat.filesystems_memory_usage,
                       stat.network_memory_usage,
                       stat.modules_memory_usage,
                       stat.applications_memory_usage);
        } else {
                perror(NULL);
        }

        // ...

   @endcode
 */
//==============================================================================
static inline int get_memory_usage_details(memstat_t *stat)
{
        return _builtinfunc(mm_get_mem_usage_details, stat);
}

//==============================================================================
/**
 * @brief Function returns memory usage of selected module (driver).
 *
 * The function get_module_memory_usage() return memory usage by specified
 * module number <i>module_number</i>.
 *
 * @param module_number     module number
 *
 * @return On success, return an amount of used memory of module in bytes.
 * On error, \b -1 is returned.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        uint number_of_modules = get_number_of_modules();
        for (uint i = 0; i < number_of_modules; i++) {
                printf("%s : %d\n", get_module_name(i), (int)get_module_memory_usage(i));
        }

        // ...

   @endcode
 */
//==============================================================================
static inline i32_t get_module_memory_usage(uint module_number)
{
        i32_t size = -1;
        _builtinfunc(mm_get_module_mem_usage, module_number, &size);
        return size;
}

//==============================================================================
/**
 * @brief Function returns system uptime in seconds.
 *
 * The function get_uptime() return 32-bit run time value in seconds.
 *
 * @return Uptime value in seconds.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("System works: %d seconds\n", get_uptime());

        // ...

   @endcode
 */
//==============================================================================
static inline u32_t get_uptime(void)
{
        return _builtinfunc(get_uptime_counter);
}

//==============================================================================
/**
 * @brief Function returns an average CPU load.
 *
 * The function get_average_CPU_load() fill average CPU load container
 * pointed by <i>avg_CPU_load</i>.
 *
 * @param avg_CPU_load          average CPU load container
 *
 * @return On success \b 0 is returned, otherwise \b -1.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        avg_CPU_load_t avg;
        get_average_CPU_load(&avg);

        printf("Average CPU load: %2d.%d%%, %2d.%d%%, %2d.%d%%\n",
                avg.min1  / 10, avg.min1  % 10,
                avg.min5  / 10, avg.min5  % 10,
                avg.min15 / 10, avg.min15 % 10);

        // ...

   @endcode
 */
//==============================================================================
static inline int get_average_CPU_load(avg_CPU_load_t *avg_CPU_load)
{
        return _builtinfunc(get_average_CPU_load, avg_CPU_load);
}

//==============================================================================
/**
 * @brief Function returns system tick counter.
 *
 * The function get_tick_counter() return number of system clock ticks.
 * The value is mostly incremented less than 1 millisecond. Incrementation
 * interval depends on task switch frequency.
 *
 * @return Number of system's ticks.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("Context was switched %lu times\n", get_tick_counter());

        // ...

   @endcode
 */
//==============================================================================
static inline u64_t get_tick_counter(void)
{
        return _builtinfunc(kernel_get_tick_counter);
}

//==============================================================================
/**
 * @brief Function returns up time in milliseconds.
 *
 * The function get_time_ms() return number of milliseconds which
 * elapsed after kernel start. Function is similar to get_uptime(), except
 * that return milliseconds instead of seconds. In this function the tick
 * counter value is calculated to milliseconds, what means that resolution of
 * this counter depends on system tick counter increase value.
 *
 * @return System work time in milliseconds.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("System works by %lu ms\n", get_time_ms());

        // ...

   @endcode
 *
 * @see get_uptime()
 */
//==============================================================================
static inline u64_t get_time_ms(void)
{
        return _builtinfunc(kernel_get_time_ms);
}

//==============================================================================
/**
 * @brief Function returns the name of current platform.
 *
 * The function get_platform_name() return platform name on which dnx RTOS
 * is working.
 *
 * @return Platform name.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("Platform name: %s\n", get_platform_name());

        // ...

   @endcode
 */
//==============================================================================
static inline const char *get_platform_name(void)
{
        return _CPUCTL_PLATFORM_NAME;
}

//==============================================================================
/**
 * @brief Function returns the operating system name.
 *
 * The function get_OS_name() return operating system name (dnx RTOS).
 *
 * @return Operating system name.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("Operating system name: %s\n", get_OS_name());

        // ...

   @endcode
 */
//==============================================================================
static inline const char *get_OS_name(void)
{
        return "dnx RTOS";
}

//==============================================================================
/**
 * @brief Function returns an operating system version.
 *
 * The function get_OS_version() return operating system version string.
 *
 * @return Operating system version string.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("Operating system version: %s\n", get_OS_version());

        // ...

   @endcode
 */
//==============================================================================
static inline const char *get_OS_version(void)
{
        return "2.3.0";
}

//==============================================================================
/**
 * @brief Function returns a codename of operating system version.
 *
 * The function get_OS_codename() return a codename string.
 *
 * @return Codename string.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("Codename: %s\n", get_OS_codename());

        // ...

   @endcode
 */
//==============================================================================
static inline const char *get_OS_codename(void)
{
        return "Falcon";
}

//==============================================================================
/**
 * @brief Function returns name of used kernel.
 *
 * The function get_kernel_name() return kernel name string.
 *
 * @return Kernel name string.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("Kernel name: %s\n", get_kernel_name());

        // ...

   @endcode
 */
//==============================================================================
static inline const char *get_kernel_name(void)
{
        return _KERNEL_NAME;
}

//==============================================================================
/**
 * @brief Funciton returns version of used kernel.
 *
 * The function get_kernel_version() return kernel version string.
 *
 * @return Kernel version string.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("Kernel version: %s\n", get_kernel_version());

        // ...

   @endcode
 */
//==============================================================================
static inline const char *get_kernel_version(void)
{
        return _KERNEL_VERSION;
}

//==============================================================================
/**
 * @brief Function returns author name.
 *
 * The function get_author_name() return author name.
 *
 * @return Author name.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("Author name: %s\n", get_author_name());

        // ...

   @endcode
 */
//==============================================================================
static inline const char *get_author_name(void)
{
        return "Daniel Zorychta";
}

//==============================================================================
/**
 * @brief Function returns author's email.
 *
 * The function get_author_email() return author's email address.
 *
 * @return Author's email address.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("Author's email: %s\n", get_author_email());

        // ...

   @endcode
 */
//==============================================================================
static inline const char *get_author_email(void)
{
        return "<daniel.zorychta@gmail.com>";
}

//==============================================================================
/**
 * @brief Function returns host name string.
 *
 * The function get_host_name() return host name string configured in
 * configuration files.
 *
 * @return Return host name.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("Host name: %s\n", get_host_name());

        // ...

   @endcode
 */
//==============================================================================
static inline const char *get_host_name(void)
{
        return __OS_HOSTNAME__;
}

//==============================================================================
/**
 * @brief Function returns name of current user.
 *
 * The function get_user_name() return name of current user. Function
 * return always "root" string, because user handling is not supported yet.
 *
 * @return Return user name.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("User name: %s\n", get_user_name());

        // ...

   @endcode
 */
//==============================================================================
static inline const char *get_user_name(void)
{
        return "root";
}

//==============================================================================
/**
 * @brief Function returns a name of selected module.
 *
 * The function get_module_name() return name of selected module by using
 * <i>modno</i> index.
 *
 * @param modno     module number
 *
 * @return Return module name.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("Module name: %s\n", get_module_name(0));

        // ...

   @endcode
 */
//==============================================================================
static inline const char *get_module_name(size_t modno)
{
        return _builtinfunc(module_get_name, modno);
}

//==============================================================================
/**
 * @brief Function returns an ID of selected module (by name).
 *
 * The function get_module_ID() return module ID selected by
 * name pointed by <i>name</i>.
 *
 * @param name     module name
 *
 * @return On success, return module index (ID). On error, \b -1 is returned.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("Module ID: %d\n", get_module_ID("crc"));

        // ...
   @endcode
 */
//==============================================================================
static inline int get_module_ID(const char *name)
{
        return _builtinfunc(module_get_ID, name);
}

//==============================================================================
/**
 * @brief Function returns an ID of selected module (by dev_t index).
 *
 * The function get_module_ID2() return module ID stored in dev_t type.
 *
 * @param dev           device ID
 *
 * @return Return module ID.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("Module ID: %d\n", get_module_ID2(dev));

        // ...

   @endcode
 */
//==============================================================================
static inline int get_module_ID2(dev_t dev)
{
        return _builtinfunc(dev_t__extract_modno, dev);
}

//==============================================================================
/**
 * @brief Function returns a major number of selected module.
 *
 * The function get_module_major() return module major number stored in
 * dev_t type.
 *
 * @param dev           device ID
 *
 * @return Return module major number.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("Module ID: %d\n", get_module_major(dev));

        // ...

   @endcode
 */
//==============================================================================
static inline int get_module_major(dev_t dev)
{
        return _builtinfunc(dev_t__extract_major, dev);
}

//==============================================================================
/**
 * @brief Function returns a minor number of selected module.
 *
 * The function get_module_minor() return module minor number stored in
 * dev_t type.
 *
 * @param dev           device ID
 *
 * @return Return module minor number.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        printf("Module ID: %d\n", get_module_minor(dev));

        // ...

   @endcode
 */
//==============================================================================
static inline int get_module_minor(dev_t dev)
{
        return _builtinfunc(dev_t__extract_minor, dev);
}

//==============================================================================
/**
 * @brief Function returns number of modules.
 *
 * The function get_number_of_modules() return number of registered
 * modules in system.
 *
 * @return Return number of registered modules in system.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        uint number_of_modules = get_number_of_modules();
        for (uint i = 0; i < number_of_modules; i++) {
                printf("%s : %d\n", get_module_name(i), get_module_memory_usage(i));
        }

        // ...

   @endcode
 */
//==============================================================================
static inline uint get_number_of_modules(void)
{
        return _builtinfunc(module_get_count);
}

//==============================================================================
/**
 * @brief Function returns number of instances of selected module index.
 *
 * The function get_number_of_module_instances() return number of instances
 * of selected module of index <i>n</i>.
 *
 * @param n             module index
 *
 * @return On success, number of instances is returned, otherwise \b -1 is returned.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        bool active;
        if (is_module_active(0, &active) == ESUCC && active) {
                // module active ...
        } else {
                // error or module inactive ...
        }

        // ...

   @endcode
 */
//==============================================================================
static inline ssize_t get_number_of_module_instances(size_t n)
{
        return _builtinfunc(module_get_number_of_instances, n);
}

//==============================================================================
/**
 * @brief Function restarts the system.
 *
 * The function restart system. Function does not write unsaved data.
 *
 * @return This function does not return.
 *
 * @b Example
 * @code
        #include <dnx/os.h>
        #include <unistd.h>

        // ...

        sync();

        // e.g. user requests system restart
        system_reboot();

   @endcode
 */
//==============================================================================
static inline void system_reboot(void)
{
        _builtinfunc(cpuctl_restart_system);
}

//==============================================================================
/**
 * @brief Function shutdown the system.
 *
 * The function shutdown system. Function does not write unsaved data.
 *
 * @return This function does not return.
 *
 * @b Example
 * @code
        #include <dnx/os.h>
        #include <unistd.h>

        // ...

        sync();

        // e.g. user requests system restart
        system_shutdown();

   @endcode
 */
//==============================================================================
static inline void system_shutdown(void)
{
        _builtinfunc(cpuctl_shutdown_system);
}

//==============================================================================
/**
 * @brief Function raed system log.
 *
 * The function syslog_read() read system log messages. Read message is
 * deleted from system ring buffer.
 *
 * @param  str          message string
 * @param  len          maximum string size
 * @param  from_time    time from log is get (time from system start)
 * @param  curr_time    log current time (time from system start)
 *
 * @return String size. 0 if system log is empty.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        char msg[128];
        struct timeval t = {0, 0};
        while (syslog_read(msg, sizeof(msg), &t, &t) {
                puts(msg);
        }

        // ...

   @endcode
 */
//==============================================================================
static inline size_t syslog_read(char *str, size_t len, const struct timeval *from_time, struct timeval *curr_time)
{
#if ((__OS_SYSTEM_MSG_ENABLE__ > 0) && (__OS_PRINTF_ENABLE__ > 0))
        size_t n = 0;
        syscall(SYSCALL_SYSLOGREAD, &n, str, &len, from_time, curr_time);
        return n;
#else
        (void)str;
        (void)len;
        (void)from_time;
        (void)curr_time;
        return 0;
#endif
}

//==============================================================================
/**
 * @brief Function clear system log.
 *
 * The function syslog_clear() clears system log messages.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        syslog_clear();

        // ...

   @endcode
 */
//==============================================================================
static inline void syslog_clear(void)
{
        _builtinfunc(printk_clear);
}

//==============================================================================
/**
 * @brief Function is used to detect occurred kernel panic.
 *
 * The function detect_kernel_panic() detect if in the last session
 * the Kernel Panic occurred. Kernel Panic message is redirected to the selected
 * file.
 *
 * @param  file         write kernel panic message into selected file
 *
 * @return If kernel panic occurred then \b true is returned, otherwise \b false.
 *
 * @b Example
 * @code
        #include <dnx/os.h>

        // ...

        detect_kernel_panic(stderr);

        // ...

   @endcode
 */
//==============================================================================
static inline bool detect_kernel_panic(FILE *file)
{
        bool r = false;
        syscall(SYSCALL_KERNELPANICDETECT, &r, file);
        return r;
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* _OS_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
