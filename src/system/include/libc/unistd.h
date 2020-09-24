/*=========================================================================*//**
@file    unistd.h

@author  Daniel Zorychta

@brief   Unix standard library.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <kernel/errno.h>
#include <cpu/cpuctl.h>
#include <lib/unarg.h>
#include <fs/vfs.h>

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
 * @param microseconds      number of microseconds to sleep (delay)
 *
 * @b Example
 * @code
        #include <unistd.h>

        // ...
        usleep(10);
        // code here will be executed after 1 system tick (at least 10us)
        usleep(10000);
        // code here will be executed after at least 10000us
        // ...
   @endcode
 *
 * @see sleep(), msleep(), sleep_until(), msleep_until()
 */
//==============================================================================
static inline void usleep(const u32_t microseconds)
{
        u32_t ms = microseconds / 1000;
        u32_t us = microseconds % 1000;

        if (ms) {
                _builtinfunc(sleep_ms, ms);
        }

        if (us) {
                _builtinfunc(cpuctl_delay_us, us);
        }
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
static inline u32_t prepare_sleep_until(void)
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
#if __OS_ENABLE_GETCWD__ == _YES_
        char *cwd;
        syscall(SYSCALL_GETCWD, &cwd, buf, &size);
        return cwd;
#else
        UNUSED_ARG1(size);
        return buf;
#endif
}

//==============================================================================
/**
 * @brief Function set current working directory.
 *
 * The chdir() function set current working directory of program.
 * This path should never disappear when program is running. Path is set by
 * reference not by value.
 *
 * @param cwd   Current Working Directory path.
 *
 * @return On success, this function return 0. On error -1 is returned and
 *         appropriate @ref errno value is set.
 *
 * @b Example
 * @code
        #include <unistd.h>

        // ...
        chdir("/new/path");
        // ...
   @endcode
 */
//==============================================================================
static inline int chdir(const char *cwd)
{
#if __OS_ENABLE_GETCWD__ == _YES_
        int result = -1;
        syscall(SYSCALL_SETCWD, &result, cwd);
        return result;
#else
        UNUSED_ARG1(cwd);
        _errno = ENOTSUP;
        return -1;
#endif
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
#if __OS_ENABLE_CHOWN__ == _YES_
        int r = -1;
        syscall(SYSCALL_CHOWN, &r, pathname, &owner, &group);
        return r;
#else
        UNUSED_ARG3(pathname, owner, group);
        _errno = ENOTSUP;
        return -1;
#endif
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

//==============================================================================
/**
 * @brief Function return group ID of current user.
 *
 * The getpid() function returns current group ID.
 *
 * @note Function returns always the root group ID (0).
 *
 * @b Example
 * @code
        #include <unistd.h>

        // ...
        gid_t gid = getgid();
        // ...
   @endcode
 */
//==============================================================================
static inline gid_t getgid(void)
{
        return 0;
}

//==============================================================================
/**
 * @brief Function return user ID.
 *
 * The getuid() function returns ID of current user.
 *
 * @note Function returns always the root user ID (0).
 *
 * @b Example
 * @code
        #include <unistd.h>

        // ...
        uid_t uid = getuid();
        // ...
   @endcode
 */
//==============================================================================
static inline uid_t getuid(void)
{
        return 0;
}

//==============================================================================
/**
 * @brief  Function open file as file descriptor.
 *
 * @param  path         file path
 * @param  flags        file open flags
 * @param  mode         optional mode
 *
 * @return On success file descriptor is returned (fd >= 0), otherwise -1.
 */
//==============================================================================
extern fd_t open(const char *path, int flags, ...);

//==============================================================================
/**
 * @brief  Function close file descriptor.
 *
 * @param  fd           file descriptor
 *
 * @return On success 0 is returned.
 */
//==============================================================================
extern int close(fd_t fd);

//==============================================================================
/**
 * @brief  Function read file.
 *
 * @param  fd           file descriptor
 * @param  buf          buffer
 * @param  count        bytes to read
 *
 * @return On success number of read bytes is returned, otherwise negative value.
 */
//==============================================================================
extern ssize_t read(fd_t fd, void *buf, size_t count);

//==============================================================================
/**
 * @brief  Function read file.
 *
 * @param  fd           file descriptor
 * @param  buf          buffer
 * @param  count        bytes to read
 *
 * @return On success number of written bytes is returned, otherwise negative value.
 */
//==============================================================================
extern ssize_t write(fd_t fd, const void *buf, size_t count);

//==============================================================================
/**
 * @brief  Function set file position.
 *
 * @param  fd           file descriptor
 * @param  offset       offset
 * @param  whence       seek mode
 *
 * @return On success return file offset after operation, otherwise negative value
 *         on error.
 */
//==============================================================================
extern off_t lseek(fd_t fd, off_t offset, int whence);

//==============================================================================
/**
 * @brief  Function remove selected file.
 *
 * @param  pathname     file to remove
 *
 * @return On success 0 is returned.
 */
//==============================================================================
extern int unlink(const char *pathname);

#ifdef __cplusplus
}
#endif

#endif /* _UNISTD_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
