/*=========================================================================*//**
@file    unistd.h

@author  Daniel Zorychta

@brief   Unix standard library.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/**
\defgroup unistd-h <unistd.h>

The <unistd.h> header defines miscellaneous symbolic constants and types, and
declares miscellaneous functions. The contents of this header are shown below.
Library contains only subset of functions and macros that exists in original
version of library.

@{*/

#ifndef _UNISTD_H_
#define _UNISTD_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <sys/types.h>
#include <kernel/syscall.h>
#include <kernel/kwrapper.h>

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief Function switches process to inactive state for selected time in seconds.
 *
 * The sleep() makes the calling thread sleep until seconds <i>seconds</i>
 * have elapsed.
 *
 * @param seconds   number of seconds to sleep
 *
 * @b Example
 * @code
        #include <unistd.h>

        // ...
        sleep(2);
        // code here will be executed after 2s sleep
        // ...
   @endcode
 *
 * @see msleep(), usleep(), sleep_until(), msleep_until()
 */
//==============================================================================
static inline void sleep(const uint seconds)
{
        _builtinfunc(sleep, seconds);
}

//==============================================================================
/**
 * @brief Function switches process to inactive state for selected time in milliseconds.
 *
 * The msleep() makes the calling thread sleep until milliseconds
 * <i>milliseconds</i> have elapsed.
 *
 * @note dnx RTOS extension function.
 *
 * @param milliseconds      number of milliseconds to sleep
 *
 * @b Example
 * @code
        #include <unistd.h>

        // ...
        msleep(10);
        // code here will be executed after 10ms sleep
        // ...
   @endcode
 *
 * @see sleep(), usleep(), sleep_until(), msleep_until()
 */
//==============================================================================
static inline void msleep(const uint milliseconds)
{
        _builtinfunc(sleep_ms, milliseconds);
}

//==============================================================================
/**
 * @brief Function switches process to inactive state for selected time in microseconds.
 *
 * The usleep() makes the calling thread sleep until microseconds
 * <i>microseconds</i> have elapsed.<p>
 *
 * @note Function is not fully supported by dnx RTOS. The task falls asleep for
 * at least 1ms (depends on context switch frequency) if the delay is lower than
 * or equal to 1000 microseconds.
 *
 * @param microseconds      number of microseconds to sleep
 *
 * @b Example
 * @code
        #include <unistd.h>

        // ...
        usleep(10);
        // code here will be executed after 1 system tick (at least 1ms)
        usleep(10000);
        // code here will be executed after at least 10ms
        // ...
   @endcode
 *
 * @see sleep(), msleep(), sleep_until(), msleep_until()
 */
//==============================================================================
static inline void usleep(const u32_t microseconds)
{
        u32_t ms = microseconds / 1000;
        _builtinfunc(sleep_ms, ms ? ms : 1);
}

//==============================================================================
/**
 * @brief Function prepares time reference to sleep.
 *
 * The prepare_sleep_until() function prepare tick counter to call
 * msleep_until_ms() and sleep_until() functions.
 *
 * @note dnx RTOS extension function.
 *
 * @return Time reference value.
 *
 * @b Example
 * @code
        #include <dnx/os.h>
        #include <unistd.h>

        // ...
        u32_t ref_time = prepare_sleep_until();

        for (;;) {
                // ...

                msleep_until(10, &ref_time);
        }
        // ...
   @endcode
 *
 * @see sleep(), msleep(), sleep_until(), msleep_until()
 */
//==============================================================================
static inline int prepare_sleep_until(void)
{
        return _builtinfunc(kernel_get_tick_counter);
}

//==============================================================================
/**
 * @brief Function switches process to inactive state for selected time in milliseconds.
 *
 * The msleep_until() makes the calling thread sleep until milliseconds
 * <i>milliseconds</i> have elapsed. Function produces more precise delay.
 *
 * @note dnx RTOS extension function.
 *
 * @param milliseconds      number of milliseconds to sleep
 * @param time_ref          time reference
 *
 * @b Example
 * @code
        #include <dnx/os.h>
        #include <unistd.h>

        // ...
        u32_t ref_time = prepare_sleep_until();

        for (;;) {
                // ...

                msleep_until(10, &ref_time);
        }
        // ...
   @endcode
 *
 * @see sleep(), msleep(), prepare_sleep_until(), sleep_until()
 */
//==============================================================================
static inline void msleep_until(const u32_t milliseconds, u32_t *time_ref)
{
        _builtinfunc(sleep_until_ms, milliseconds, time_ref);
}

//==============================================================================
/**
 * @brief Function switches process to inactive state for selected time in seconds.
 *
 * The sleep_until() makes the calling thread sleep until seconds
 * <i>seconds</i> have elapsed. Function produces more precise delay.
 *
 * @note dnx RTOS extension function.
 *
 * @param seconds               number of seconds to sleep
 * @param time_ref              time reference
 *
 * @b Example
 * @code
        #include <dnx/os.h>
        #include <unistd.h>

        // ...
        int ref_time = prepare_sleep_until();

        for (;;) {
                // ...

                sleep_until(1, &ref_time);
        }
        // ...
   @endcode
 *
 * @see sleep(), msleep(), prepare_sleep_until(), msleep_until()
 */
//==============================================================================
static inline void sleep_until(const uint seconds, u32_t *time_ref)
{
        _builtinfunc(sleep_until, seconds, time_ref);
}

//==============================================================================
/**
 * @brief Function copies current working directory path to buffer.
 *
 * The getcwd() function copies an absolute pathname of the current
 * working directory to the array pointed to by <i>buf</i>, which is of length
 * <i>size</i>.
 *
 * @param buf       buffer to store path
 * @param size      buffer length
 *
 * @return On success, these functions return a pointer to a string containing
 * the pathname of the current working directory. In the case getcwd() is the
 * same value as <i>buf</i>.
 *
 * @b Example
 * @code
        #include <unistd.h>

        // ...
        char *buf[100];
        getcwd(buf, 100);
        // ...
   @endcode
 */
//==============================================================================
static inline char *getcwd(char *buf, size_t size)
{
        char *cwd;
        syscall(SYSCALL_GETCWD, &cwd, buf, &size);
        return cwd;
}

//==============================================================================
/**
 * @brief Function returns PID of current process.
 *
 * The function getpid() return PID of current process (caller).
 *
 * @exception | @ref EINVAL
 * @exception | @ref ENOENT
 * @exception | @ref ESRCH
 *
 * @return Return PID on success. On error, 0 is returned.
 *
 * @b Example
 * @code
        #include <unistd.h>

        // ...

        pid_t pid = getpid();
        printf("PID of this process is: %d\n, pid);

        // ...
   @endcode
 *
 * @see process_getpid()
 */
//==============================================================================
static inline pid_t getpid(void)
{
        pid_t pid = 0;
        syscall(SYSCALL_PROCESSGETPID, &pid);
        return pid;
}

//==============================================================================
/**
 * @brief Function changes the ownership of file.
 *
 * The chown() changes the ownership of the file specified by <i>pathname</i>.
 *
 * @param pathname      path to file
 * @param owner         owner ID
 * @param group         group ID
 *
 * @exception | @ref EINVAL
 * @exception | @ref ENOENT
 * @exception | ...
 *
 * @return On success, zero is returned. On error, -1 is returned, and
 * <b>errno</b> is set appropriately.
 *
 * @b Example
 * @code
        #include <unistd.h>

        // ...
        chown("/foo/bar", 1000, 1000);
        // ...
   @endcode
 */
//==============================================================================
static inline int chown(const char *pathname, uid_t owner, gid_t group)
{
        int r = -1;
        syscall(SYSCALL_CHOWN, &r, pathname, &owner, &group);
        return r;
}

//==============================================================================
/**
 * @brief Function synchronizes files buffers with file systems.
 *
 * The sync() synchronizes files buffers with file systems.
 *
 * @b Example
 * @code
        #include <unistd.h>

        // ...
        sync();
        // ...
   @endcode
 */
//==============================================================================
static inline void sync(void)
{
        syscall(SYSCALL_SYNC, NULL);
}

#ifdef __cplusplus
}
#endif

#endif /* _UNISTD_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
