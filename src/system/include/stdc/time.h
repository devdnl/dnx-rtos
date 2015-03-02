/*=========================================================================*//**
@file    time.h

@author  Daniel Zorychta

@brief   Time standard library

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _TIME_H_
#define _TIME_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <sys/types.h>
#include <stddef.h>
#include "kernel/kwrapper.h"
#include "core/conv.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 * This macro expands to an expression representing the number of clock ticks
 * per second.
 * Clock ticks are units of time of a constant but system-specific length,
 * as those returned by function clock.
 * Dividing a count of clock ticks by this expression yields the number of seconds.
 */
#define CLOCKS_PER_SEC          1000

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Alias of a fundamental arithmetic type capable of representing clock tick counts.
 * Clock ticks are units of time of a constant but system-specific length, as
 * those returned by function clock.
 * This is the type returned by clock.
 */
#ifndef __CLOCK_TYPE_DEFINED__
typedef u32_t clock_t;
#endif

/**
 * Alias of a fundamental arithmetic type capable of representing times, as those
 * returned by function time.
 * For historical reasons, it is generally implemented as an integral value
 * representing the number of seconds elapsed since 00:00 hours, Jan 1, 1970 UTC
 * (i.e., a unix timestamp). Although libraries may implement this type using
 * alternative time representations.
 * Portable programs should not use values of this type directly, but always rely
 * on calls to elements of the standard library to translate them to portable types.
 */
#ifndef __TIME_TYPE_DEFINED__
typedef u32_t time_t;
#endif

/**
 * Structure containing a calendar date and time broken down into its components.
 * The structure contains nine members of type int (in any order), which are:
 */
#ifndef __TM_STRUCT_DEFINED__
struct tm {
        int tm_sec;       //!> seconds after the minute        0-60*
        int tm_min;       //!> minutes after the hour          0-59
        int tm_hour;      //!> hours since midnight            0-23
        int tm_mday;      //!> day of the month                1-31
        int tm_mon;       //!> months since January            0-11
        int tm_year;      //!> years since                     1900
        int tm_wday;      //!> days since Sunday               0-6
        int tm_yday;      //!> days since January 1            0-365
        int tm_isdst;     //!> Daylight Saving Time flag
};
#endif

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
 * @brief  Clock program
 *
 * Returns the processor time consumed by the program.
 * The value returned is expressed in clock ticks, which are units of time of
 * a constant but system-specific length (with a relation of CLOCKS_PER_SEC
 * clock ticks per second).
 * The epoch used as reference by clock varies between systems, but it is related
 * to the program execution (generally its launch). To calculate the actual
 * processing time of a program, the value returned by clock shall be compared
 * to a value returned by a previous call to the same function.
 *
 * @param  None
 *
 * @return The number of clock ticks elapsed since an epoch related to the
 *         particular program execution. On failure, the function returns a value
 *         of -1.
 */
//==============================================================================
static inline clock_t clock(void)
{
        return _kernel_get_time_ms();
}

//==============================================================================
/**
 * @brief  Calculates the difference in seconds between beginning and end.
 *
 * @param  end          Higher bound of the time interval whose length is calculated.
 * @param  beginning    Lower bound of the time interval whose length is calculated.
 *                      If this describes a time point later than end, the result
 *                      is negative.
 *
 * @return The result of (end-beginning) in seconds as a floating-point value of
 *         type double.
 */
//==============================================================================
static inline double difftime(time_t end, time_t beginning)
{
        return end - beginning;
}

//==============================================================================
/**
 * @brief  Convert tm structure to time_t
 *
 * This function performs the reverse translation that localtime does.
 * The values of the members tm_wday and tm_yday of timeptr are ignored, and
 * the values of the other members are interpreted even if out of their valid
 * ranges (see struct tm). For example, tm_mday may contain values above 31,
 * which are interpreted accordingly as the days that follow the last day of
 * the selected month.
 * A call to this function automatically adjusts the values of the members of
 * timeptr if they are off-range or -in the case of tm_wday and tm_yday- if they
 * have values that do not match the date described by the other members.
 *
 * @param  timeptr      Pointer to a tm structure that contains a calendar time
 *                      broken down into its components (see struct tm)
 *
 * @return A time_t value corresponding to the calendar time passed as argument.
 *         If the calendar time cannot be represented, a value of -1 is returned.
 */
//==============================================================================
static inline time_t mktime(struct tm *timeptr)
{
        return _date_to_epoch(timeptr->tm_mday,
                              timeptr->tm_mon+1,
                              timeptr->tm_year+1900,
                              timeptr->tm_hour,
                              timeptr->tm_min,
                              timeptr->tm_sec);
}

//==============================================================================
/**
 * @brief  Get current time
 *
 * The function returns this value, and if the argument is not a null pointer,
 * it also sets this value to the object pointed by timer.
 * The value returned generally represents the number of seconds since 00:00
 * hours, Jan 1, 1970 UTC (i.e., the current unix timestamp). Although libraries
 * may use a different representation of time: Portable programs should not use
 * the value returned by this function directly, but always rely on calls to
 * other elements of the standard library to translate them to portable types
 * (such as localtime, gmtime or difftime).
 *
 * @param  timer        Pointer to an object of type time_t, where the time
 *                      value is stored.
 *                      Alternatively, this parameter can be a null pointer,
 *                      in which case the parameter is not used (the function
 *                      still returns a value of type time_t with the result).
 *
 * @return The current calendar time as a time_t object.
 *         If the argument is not a null pointer, the return value is the same
 *         as the one stored in the location pointed by argument timer.
 *         If the function could not retrieve the calendar time, it returns
 *         a value of -1.
 */
//==============================================================================
static inline time_t time(time_t *timer)
{
        time_t t   = -1;
        FILE  *rtc = _vfs_fopen(CONFIG_RTC_FILE_PATH, "r");
        if (rtc) {
                if (_vfs_fread(&t, sizeof(time_t), 1, rtc) == 1) {
                        if (timer) {
                                *timer = t;
                        }
                }
                _vfs_fclose(rtc);
        }

        return t;
}

//==============================================================================
/**
 * @brief  Set system's time
 *
 * stime() sets the system's idea of the time and date. The time, pointed to by
 * timer, is measured in seconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).
 *
 * @param  timer        pointer to an object of type time_t, where the time
 *                      value is stored.
 *
 * @return On success 0 is returned.
 *         On error -1 is returned.
 */
//==============================================================================
static inline int stime(time_t *timer)
{
        int    ret = -1;
        FILE  *rtc = _vfs_fopen(CONFIG_RTC_FILE_PATH, "w");
        if (rtc) {
                ret = _vfs_fwrite(timer, sizeof(time_t), 1, rtc) == 1 ? 0 : -1;
                _vfs_fclose(rtc);
        }

        return ret;
}

//==============================================================================
/**
 * @brief  Setup time zone by setting difference between UTC and local time
 *
 * @param  tdiff        time difference in seconds (can be negative)
 *
 * @return None
 */
//==============================================================================
static inline void stimezone(int tdiff)
{
        _ltimeoff = tdiff;
}

//==============================================================================
/**
 * @brief  Return difference in seconds between UTC and local time
 *
 * @param  None
 *
 * @return Difference between UTC and local time in seconds.
 */
//==============================================================================
static inline int timezone()
{
        return _ltimeoff;
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
static inline char *asctime(const struct tm *timeptr)
{
        return _ctime(NULL, timeptr);
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
 *      Www Mmm dd hh:mm:ss yyyy
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
static inline char *ctime(const time_t *timer)
{
        return _ctime(timer, NULL);
}

//==============================================================================
/**
 * @brief  Convert time_t to tm as UTC time
 *
 * Uses the value pointed by timer to fill a tm structure with the values that
 * represent the corresponding time, expressed as a UTC time (i.e., the time
 * at the GMT timezone).
 *
 * @param  timer        Pointer to an object of type time_t that contains a time value.
 *                      time_t is an alias of a fundamental arithmetic type
 *                      capable of representing times as returned by function time.
 *
 * @return A pointer to a tm structure with its members filled with the values
 *         that correspond to the UTC time representation of timer.
 */
//==============================================================================
static inline struct tm *gmtime(const time_t *timer)
{
        return _gmtime_r(timer, &_tmbuf);
}

//==============================================================================
/**
 * @brief  Convert time_t to tm as local time
 *
 * Uses the value pointed by timer to fill a tm structure with the values that
 * represent the corresponding time, expressed for the local timezone.
 *
 * @param  timer        Pointer to an object of type time_t that contains a time value.
 *                      time_t is an alias of a fundamental arithmetic type
 *                      capable of representing times as returned by function time.
 *
 * @return A pointer to a tm structure with its members filled with the values
 *         that correspond to the local time representation of timer.
 */
//==============================================================================
static inline struct tm *localtime(const time_t *timer)
{
        return _lotime_r(timer, &_tmbuf);
}

//==============================================================================
/**
 * @brief  Format time as string
 *
 * Copies into ptr the content of format, expanding its format specifiers into
 * the corresponding values that represent the time described in timeptr, with
 * a limit of maxsize characters.
 *
 * @param  ptr          Pointer to the destination array where the resulting
 *                      C string is copied.
 * @param  maxsize      Maximum number of characters to be copied to ptr,
 *                      including the terminating null-character.
 * @param  format       C string containing any combination of regular characters
 *                      and special format specifiers. These format specifiers
 *                      are replaced by the function to the corresponding values
 *                      to represent the time specified in timeptr.
 * @param timeptr       Pointer to a tm structure that contains a calendar time
 *                      broken down into its components (see struct tm).
 *
 * @return If the length of the resulting C string, including the terminating
 *         null-character, doesn't exceed maxsize, the function returns the
 *         total number of characters copied to ptr (not including the terminating
 *         null-character).
 *         Otherwise, it returns zero, and the contents of the array pointed by
 *         ptr are indeterminate.
 *
 * @note Supported flags:
 *       % - % character
 *       n - new line
 *       H - Hour in 24h format (00-23)
 *       I - Hour in 12h format (01-12)
 *       M - Minute (00-59)
 *       S - Second (00-61)
 *       A - Full weekday name
 *       a - Abbreviated weekday name
 *       B - Full month name
 *       b - Abbreviated month name
 *       h - Abbreviated month name
 *       C - Year divided by 100 and truncated to integer (00-99) (century)
 *       y - Year, last two digits (00-99)
 *       Y - Year
 *       d - Day of the month, zero-padded (01-31)
 *       p - AM or PM designation
 *       j - Day of the year (001-366)
 *       m - Month as a decimal number (01-12)
 *       X - Time representation                                14:55:02
 *       F - Short YYYY-MM-DD date, equivalent to %Y-%m-%d      2001-08-23
 *       D - Short MM/DD/YY date, equivalent to %m/%d/%y        08/23/01
 *       x - Short MM/DD/YY date, equivalent to %m/%d/%y        08/23/01
 */
//==============================================================================
static inline size_t strftime(char *ptr, size_t maxsize, const char *format, const struct tm *timeptr)
{
        return _strftime(ptr, maxsize, format, timeptr);
}

#ifdef __cplusplus
}
#endif

#endif /* _TIME_H_ */
/*==============================================================================
  End of file
==============================================================================*/
