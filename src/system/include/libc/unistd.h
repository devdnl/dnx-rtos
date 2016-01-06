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
 * @brief void sleep(const uint seconds)
 * The <b>sleep</b>() makes the calling thread sleep until seconds <i>seconds</i>
 * have elapsed.
 *
 * @param seconds   number of seconds to sleep
 *
 * @errors None
 *
 * @return None
 *
 * @b Example
 * @code
        #include <unistd.h>

        // ...
        sleep(2);
        // code here will be executed after 2s sleep
        // ...
   @endcode
 */
//==============================================================================
static inline void sleep(const uint seconds)
{
        _builtinfunc(sleep, seconds);
}

//==============================================================================
/**
 * @brief void sleep_ms(const uint milliseconds)
 * The <b>sleep_ms</b>() makes the calling thread sleep until milliseconds
 * <i>milliseconds</i> have elapsed.
 *
 * @note dnx RTOS extension function.
 *
 * @param milliseconds      number of milliseconds to sleep
 *
 * @errors None
 *
 * @return None
 *
 * @b Example
 * @code
        #include <unistd.h>

        // ...
        sleep_ms(10);
        // code here will be executed after 10ms sleep
        // ...
   @endcode
 */
//==============================================================================
static inline void sleep_ms(const uint milliseconds)
{
        _builtinfunc(sleep_ms, milliseconds);
}

//==============================================================================
/**
 * @brief void usleep(const u32_t microseconds)
 * The <b>usleep</b>() makes the calling thread sleep until microseconds
 * <i>microseconds</i> have elapsed.<p>
 *
 * Function is not full supported by dnx RTOS. The task falls asleep for at least
 * 1ms if the delay is lower than or equal to 1000 microseconds.
 *
 * @param microseconds      number of microseconds to sleep
 *
 * @errors None
 *
 * @return None
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
 */
//==============================================================================
static inline void usleep(const u32_t microseconds)
{
        u32_t ms = microseconds / 1000;
        _builtinfunc(sleep_ms, ms ? ms : 1);
}

//==============================================================================
/**
 * @brief int prepare_sleep_until(void)
 * The <b>prepare_sleep_until</b>() function prepare tick counter to call
 * <b>sleep_until_ms</b>() and <b>sleep_until</b>() functions.
 *
 * @note dnx RTOS extension function.
 *
 * @param None
 *
 * @errors None
 *
 * @return Current tick counter.
 *
 * @b Example
 * @code
        #include <dnx/os.h>
        #include <unistd.h>

        // ...
        u32_t ref_time = prepare_sleep_until();

        for (;;) {
                // ...

                sleep_until_ms(10, &ref_time);
        }
        // ...
   @endcode
 */
//==============================================================================
static inline int prepare_sleep_until(void)
{
        return _builtinfunc(kernel_get_tick_counter);
}

//==============================================================================
/**
 * @brief void sleep_until_ms(const u32_t milliseconds, u32_t *ref_time_ticks)
 * The <b>sleep_until_ms</b>() makes the calling thread sleep until milliseconds
 * <i>milliseconds</i> have elapsed. Function produces more precise delay.
 *
 * @note dnx RTOS extension function.
 *
 * @param milliseconds      number of milliseconds to sleep
 * @param ref_time_ticks    time reference
 *
 * @errors None
 *
 * @return None
 *
 * @b Example
 * @code
        #include <dnx/os.h>
        #include <unistd.h>

        // ...
        u32_t ref_time = prepare_sleep_until();

        for (;;) {
                // ...

                sleep_until_ms(10, &ref_time);
        }
        // ...
   @endcode
 */
//==============================================================================
static inline void sleep_until_ms(const u32_t milliseconds, u32_t *ref_time_ticks)
{
        _builtinfunc(sleep_until_ms, milliseconds, ref_time_ticks);
}

//==============================================================================
/**
 * @brief void sleep_until(const uint seconds, int *ref_time_ticks)
 * The <b>sleep_until</b>() makes the calling thread sleep until seconds
 * <i>seconds</i> have elapsed. Function produces more precise delay.
 *
 * @note dnx RTOS extension function.
 *
 * @param seconds               number of seconds to sleep
 * @param ref_time_ticks        time reference
 *
 * @errors None
 *
 * @return None
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
 */
//==============================================================================
static inline void sleep_until(const uint seconds, u32_t *ref_time_ticks)
{
        _builtinfunc(sleep_until, seconds, ref_time_ticks);
}

//==============================================================================
/**
 * @brief char *getcwd(char *buf, size_t size)
 * The <b>getcwd</b>() function copies an absolute pathname of the current
 * working directory to the array pointed to by <i>buf</i>, which is of length
 * <i>size</i>.
 *
 * @param buf       buffer to store path
 * @param size      buffer length
 *
 * @errors None
 *
 * @return On success, these functions return a pointer to a string containing
 * the pathname of the current working directory. In the case <b>getcwd</b>() is the
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
 * @brief pid_t getpid(void)
 * The function <b>getpid</b>() return PID of current process (caller).
 *
 * @param None
 *
 * @errors EINVAL, ENOENT, ESRCH
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
 * @brief int chown(const char *pathname, uid_t owner, gid_t group)
 * The <b>chown</b>() changes the ownership of the file specified by <i>pathname</i>.<p>
 *
 * @param pathname      path to file
 * @param owner         owner ID
 * @param group         group ID
 *
 * @errors EINVAL, ENOENT, ...
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
 * @brief void sync(void)
 * The <b>sync</b>() synchronize system files with buffers.
 *
 * @param None
 *
 * @errors None
 *
 * @return None
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
