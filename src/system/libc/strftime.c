/*=========================================================================*//**
@file    strftime.c

@author  Daniel Zorychta

@brief

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

/*==============================================================================
  Include files
==============================================================================*/
#include <time.h>
#include <config.h>
#include <stdbool.h>
#include <stdio.h>
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
#if (__OS_PRINTF_ENABLE__ > 0) && (__OS_ENABLE_TIMEMAN__ == _YES_)
/** days of week */
static const char *week_day_abbr[] = {
        "Sun", "Mon", "Tue",
        "Wed", "Thu", "Fri",
        "Sat"
};

static const char *week_day_full[] = {
        "Sunday",    "Monday", "Tuesday",
        "Wednesday", "Thrusday",
        "Friday",    "Saturday"
};

/** month names */
static const char *month_abbr[] = {
        "Jan", "Feb", "Mar",
        "Apr", "May", "Jun",
        "Jul", "Aug", "Sep",
        "Oct", "Nov", "Dec"
};

static const char *month_full[] = {
        "January", "February", "March",
        "April",   "May",      "June",
        "July",    "August",   "September",
        "October", "November", "December"
};
#endif

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
#if __OS_ENABLE_TIMEMAN__ == _YES_
//==============================================================================
/**
 * @brief  Format time as string
 *
 * Copies into ptr the content of format, expanding its format specifiers into
 * the corresponding values that represent the time described in timeptr, with
 * a limit of maxsize characters.
 *
 * @param  buf          Pointer to the destination array where the resulting
 *                      C string is copied.
 * @param  size         Maximum number of characters to be copied to buf,
 *                      including the terminating null-character.
 * @param  format       C string containing any combination of regular characters
 *                      and special format specifiers. These format specifiers
 *                      are replaced by the function to the corresponding values
 *                      to represent the time specified in timeptr.
 * @param  timeptr      Pointer to a tm structure that contains a calendar time
 *                      broken down into its components (see struct tm).
 *
 * @return If the length of the resulting C string, including the terminating
 *         null-character, doesn't exceed maxsize, the function returns the
 *         total number of characters copied to buf (not including the terminating
 *         null-character).
 *         Otherwise, it returns zero, and the contents of the array pointed by
 *         buf are indeterminate.
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
 *       z - ISO 8601 offset from UTC in timezone (1 minute=1, 1 hour=100) +0100, -1230
 */
//==============================================================================
size_t strftime(char *buf, size_t size, const char *format, const struct tm *timeptr)
{
#if (__OS_PRINTF_ENABLE__ > 0)
        size_t n = 0;

        if (buf && size && format && timeptr) {
                size--;

                bool _do = true;
                char ch  = '\0';

                void put_ch(const char c)
                {
                        *buf++ = c;
                        _do    = (--size != 0);
                        n++;
                }

                bool get_fch()
                {
                        ch  = *format++;
                        _do = (ch != '\0');
                        return _do;
                }

                while (_do && size) {
                        if (!get_fch())
                                break;

                        if (ch == '%') {
                                if (!get_fch())
                                        break;

                                size_t m = 0;

                                switch (ch) {
                                case '%':
                                        put_ch(ch);
                                        break;

                                case 'n':
                                        put_ch('\n');
                                        break;

                                case 'H':
                                        m = snprintf(buf, size, "%02d", timeptr->tm_hour);
                                        break;

                                case 'I':
                                        m = snprintf(buf, size, "%02d", timeptr->tm_hour > 12 ? timeptr->tm_hour - 12 : timeptr->tm_hour);
                                        break;

                                case 'M':
                                        m = snprintf(buf, size, "%02d", timeptr->tm_min);
                                        break;

                                case 'S':
                                        m = snprintf(buf, size, "%02d", timeptr->tm_sec);
                                        break;

                                case 'a':
                                        m = snprintf(buf, size, "%s", week_day_abbr[timeptr->tm_wday]);
                                        break;

                                case 'A':
                                        m = snprintf(buf, size, "%s", week_day_full[timeptr->tm_wday]);
                                        break;

                                case 'b':
                                case 'h':
                                        m = snprintf(buf, size, "%s", month_abbr[timeptr->tm_mon]);
                                        break;

                                case 'B':
                                        m = snprintf(buf, size, "%s", month_full[timeptr->tm_mon]);
                                        break;

                                case 'C':
                                        m = snprintf(buf, size, "%02d", (timeptr->tm_year + 1900) / 100);
                                        break;

                                case 'y':
                                        m = snprintf(buf, size, "%02d", (timeptr->tm_year + 1900) % 100);
                                        break;

                                case 'Y':
                                        m = snprintf(buf, size, "%d", timeptr->tm_year + 1900);
                                        break;

                                case 'd':
                                        m = snprintf(buf, size, "%02d", timeptr->tm_mday);
                                        break;

                                case 'p':
                                        m = snprintf(buf, size, "%s", timeptr->tm_hour > 12 ? "PM" : "AM");
                                        break;

                                case 'j':
                                        m = snprintf(buf, size, "%03d", timeptr->tm_yday + 1);
                                        break;

                                case 'm':
                                        m = snprintf(buf, size, "%02d", timeptr->tm_mon + 1);
                                        break;

                                case 'X':
                                        m = snprintf(buf, size, "%02d:%02d:%02d", timeptr->tm_hour, timeptr->tm_min, timeptr->tm_sec);
                                        break;

                                case 'F':
                                        m = snprintf(buf, size, "%d-%02d-%02d", timeptr->tm_year+1900, timeptr->tm_mon+1, timeptr->tm_mday);
                                        break;

                                case 'z': {
                                        i32_t timeoff = timeptr->tm_isutc ? 0 : _ltimeoff;
                                        m = snprintf(buf, size, "%c%02d%02d",
                                                     (timeoff < 0 ? '-':'+'),
                                                     (timeoff < 0 ? -timeoff : timeoff) / 3600,
                                                     (timeoff < 0 ? -timeoff : timeoff) / 60 % 60);
                                        break;
                                }

                                case 'D':
                                case 'x':
                                        m = snprintf(buf, size, "%02d/%02d/%02d", timeptr->tm_mon+1, timeptr->tm_mday, (timeptr->tm_year+1900) % 100);
                                        break;
                                }

                                n    += m;
                                buf  += m;
                                size -= m;

                        } else {
                                put_ch(ch);
                        }
                }

                *buf = '\0';
        }

        return n;
#else
        UNUSED_ARG4(buf, size, format, timeptr);
        return 0;
#endif
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
