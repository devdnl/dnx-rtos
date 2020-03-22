/*=========================================================================*//**
@file    time.h

@author  Daniel Zorychta

@brief   Get/Set system time.

@note    Copyright (C) 2018 Daniel Zorychta <daniel.zorychta@gmail.com>

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
\defgroup sys-time-h <sys/time.h>

The library provides system set/get time functions.

@{
*/

#ifndef _SYS_TIME_H_
#define _SYS_TIME_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <sys/types.h>
#include <stddef.h>
#include <kernel/syscall.h>
#include <kernel/kwrapper.h>
#include <kernel/errno.h>
#include <lib/unarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * @brief Type representing subseconds.
 *
 * Alias of a fundamental arithmetic type capable of representing time in microseconds.
 *
 * @see gettimeofday(), settimeofday()
 */
#ifndef __SUSECONDS_TYPE_DEFINED__
typedef u32_t suseconds_t;
#endif

/**
 * @brief Type representing time value.
 *
 * @see gettimeofday(), settimeofday()
 */
#ifndef __STRUCT_TIMEVAL_DEFINED__
struct timeval {
        time_t tv_sec;          /*!< seconds */
        suseconds_t tv_usec;    /*!< microseconds */
};
#endif

/**
 * @brief Type representing timezone.
 *
 * @see gettimeofday(), settimeofday()
 */
struct timezone {
        int tz_minuteswest;     /*!< minutes west of Greenwitch */
        int tz_dsttime;         /*!< type of DST correction */
};

/*==============================================================================
  Exported objects
==============================================================================*/
#ifndef DOXYGEN
extern int _ltimeoff;
#endif

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief  Get system time
 *
 * The function can get the time as well as a timezone. If either tv or tz is
 * NULL, the corresponding structure is not get.
 *
 * @param  tv   time value
 * @param  tz   time zone
 *
 * @return The function return 0 for success, or -1 for failure and errno is
 *         set appropriately.
 *
 * @b Example
 * @code
        #include <sys/time.h>

        //...

        struct timeval timeval;

        gettimeofday(&timeval, NULL);

        struct tm tm;
        if (UTC) {
                gmtime_r(&timeval.tv_sec, &tm);
        } else {
                localtime_r(&timeval.tv_sec, &tm);
        }

        printf(asctime(&tm));

        //...
   @endcode
 *
 * @see settimeofday(), time()
 */
//==============================================================================
static inline int gettimeofday(struct timeval *tv, struct timezone *tz)
{
#if __OS_ENABLE_TIMEMAN__ ==_YES_
        int err = 0;

        if (tv) {
                tv->tv_sec  = 0;
                tv->tv_usec = 0;
                syscall(SYSCALL_GETTIME, &err, tv);
        }

        if (tz) {
                tz->tz_minuteswest = _ltimeoff;
                tz->tz_dsttime     = 0;
        }

        return err;
#else
        UNUSED_ARG2(tv, tz);
        return -1;
#endif
}

//==============================================================================
/**
 * @brief  Set system time
 *
 * The function can set the time as well as a timezone. If either tv or tz is
 * NULL, the corresponding structure is not set.
 *
 * @param  tv   time value
 * @param  tz   time zone
 *
 * @return The function return 0 for success, or -1 for failure and errno is
 *         set appropriately.
 *
 * @see gettimeofday(), time(), stime()
 */
//==============================================================================
static inline int settimeofday(const struct timeval *tv, const struct timezone *tz)
{
#if __OS_ENABLE_TIMEMAN__ == _YES_
        int err = 0;

        if (tv) {
                syscall(SYSCALL_SETTIME, &err, &tv->tv_sec);
        }

        if (!err && tz) {
                _ltimeoff = tz->tz_minuteswest;
        }

        return err;
#else
        UNUSED_ARG2(tv, tz);
        return -1;
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* _SYS_TIME_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
