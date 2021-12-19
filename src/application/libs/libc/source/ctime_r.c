/*=========================================================================*//**
@file    ctime_r.c

@author  Daniel Zorychta

@brief   Time functions.

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Include files
==============================================================================*/
#include <time.h>
#include <config.h>
#include <dnx/misc.h>

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/
#if (__OS_PRINTF_ENABLE__ > 0) && (__OS_ENABLE_TIMEMAN__ == _YES_)
/** buffer used to store converted time to string */
extern char _libc_timestr[32];
#endif

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
#if __OS_ENABLE_TIMEMAN__ == _YES_
//==============================================================================
/**
 * @brief  Convert time value (Epoch) to human readable string: Www Mmm dd hh:mm:ss zzzzz yyyy
 *
 * @param  timer        UNIX time value (can be NULL)
 * @param  tm           time structure (can be NULL)
 * @param  buf          buffer where string is filled (at least 32 bytes)
 *
 * @return Pointer to statically allocated string buffer. This function is not
 *         thread safe.
 */
//==============================================================================
static char *_ctime_r(const time_t *timer, const struct tm *tm, char *buf)
{
#if (__OS_PRINTF_ENABLE__ > 0)
        if (timer || tm) {
                if (buf == NULL) {
                        buf = _libc_timestr;
                }

                struct tm t;

                if (!timer) {
                        t = *tm;
                } else {
                        localtime_r(timer, &t);
                }

                strftime(buf, sizeof(_libc_timestr), "%a %b %d %X %z %Y%n", &t);

                return buf;
        } else {
                return NULL;
        }
#else
        UNUSED_ARG3(timer, tm, buf);
        return NULL;
#endif
}

//==============================================================================
/**
 * @brief  Convert time_t value to string
 *
 * Interprets the value pointed by timer as a calendar time and converts it to
 * a C-string containing a human-readable version of the corresponding time and
 * date, in terms of local time.
 *
 * The returned string has the following format:
 *
 *      Www Mmm dd hh:mm:ss zzzzz yyyy
 *
 * Where Www is the weekday, Mmm the month (in letters), dd the day of the month,
 * hh:mm:ss the time, and yyyy the year.
 *
 * The string is followed by a new-line character ('\n') and terminated with
 * a null-character.
 *
 * This function is equivalent to:
 *
 *      asctime(localtime(timer))
 *
 * @param  timer        Pointer to an object of type time_t that contains a time value.
 *                      time_t is an alias of a fundamental arithmetic type
 *                      capable of representing times as returned by function time.
 *
 * @return A C-string containing the date and time information in a human-readable format.
 *         The returned value points to an internal array whose validity or
 *         value may be altered by any subsequent call to asctime or ctime.
 */
//==============================================================================
char *ctime(const time_t *timer)
{
        return _ctime_r(timer, NULL, NULL);
}

//==============================================================================
/**
 * @brief  Convert time_t value to string
 *
 * Interprets the value pointed by timer as a calendar time and converts it to
 * a C-string containing a human-readable version of the corresponding time and
 * date, in terms of local time.
 *
 * The returned string has the following format:
 *
 *      Www Mmm dd hh:mm:ss zzzzz yyyy
 *
 * Where Www is the weekday, Mmm the month (in letters), dd the day of the month,
 * hh:mm:ss the time, and yyyy the year.
 *
 * The string is followed by a new-line character ('\n') and terminated with
 * a null-character.
 *
 * This function is equivalent to:
 *
 *      asctime(localtime(timer))
 *
 * @param  timer        Pointer to an object of type time_t that contains a time value.
 *                      time_t is an alias of a fundamental arithmetic type
 *                      capable of representing times as returned by function time.
 *
 * @param  buf          Pointer to the buffer where generated string is stored.
 *                      The size of the buffer must be at least 32 bytes long.
 *
 * @return A C-string containing the date and time information in a human-readable format.
 *         The returned value points to an internal array whose validity or
 *         value may be altered by any subsequent call to asctime or ctime.
 */
//==============================================================================
char *ctime_r(const time_t *timer, char *buf)
{
        return _ctime_r(timer, NULL, buf);
}

//==============================================================================
/**
 * @brief  Convert tm structure to string
 *
 * Interprets the contents of the tm structure pointed by timeptr as a calendar
 * time and converts it to a C-string containing a human-readable version of the
 * corresponding date and time.
 * The returned string has the following format:
 *
 *      Www Mmm dd hh:mm:ss yyyy
 *
 * Where Www is the weekday, Mmm the month (in letters), dd the day of the month,
 * hh:mm:ss the time, and yyyy the year.
 *
 * The string is followed by a new-line character ('\n') and terminated with
 * a null-character.
 *
 * @param  timeptr      Pointer to a tm structure that contains a calendar time
 *                      broken down into its components (see struct tm).
 *
 * @return A C-string containing the date and time information in a human-readable
 *         format.
 *         The returned value points to an internal array whose validity or value
 *         may be altered by any subsequent call to asctime or ctime.
 */
//==============================================================================
char *asctime(const struct tm *timeptr)
{
        return _ctime_r(NULL, timeptr, NULL);
}

//==============================================================================
/**
 * @brief  Convert tm structure to string
 *
 * Interprets the contents of the tm structure pointed by timeptr as a calendar
 * time and converts it to a C-string containing a human-readable version of the
 * corresponding date and time.
 * The returned string has the following format:
 *
 *      Www Mmm dd hh:mm:ss yyyy
 *
 * Where Www is the weekday, Mmm the month (in letters), dd the day of the month,
 * hh:mm:ss the time, and yyyy the year.
 *
 * The string is followed by a new-line character ('\n') and terminated with
 * a null-character.
 *
 * @param  timeptr      Pointer to a tm structure that contains a calendar time
 *                      broken down into its components (see struct tm).
 *
 * @param  buf          Pointer to the buffer where generated string is stored.
 *                      The size of the buffer must be at least 32 bytes long.
 *
 * @return A C-string containing the date and time information in a human-readable
 *         format.
 *         The returned value points to an internal array whose validity or value
 *         may be altered by any subsequent call to asctime or ctime.
 */
//==============================================================================
char *asctime_r(const struct tm *timeptr, char *buf)
{
        return _ctime_r(NULL, timeptr, buf);
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
