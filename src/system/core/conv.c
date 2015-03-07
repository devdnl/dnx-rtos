/*=========================================================================*//**
@file    conv.c

@author  Daniel Zorychta

@brief   Module with calculation and convert functions.

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

/*==============================================================================
  Include files
==============================================================================*/
#include "core/conv.h"
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <dnx/misc.h>

/*==============================================================================
  Local macros
==============================================================================*/
#define YEAR0                   1900
#define DAY0                    4       // day 0 was Thursday
#define EPOCH_YR                1970
#define SECS_MIN                60
#define SECS_HOUR               3600
#define SECS_DAY                86400
#define SECS_YEAR               31536000
#define YEARSIZE(year)          (is_leap_year(year) ? 366 : 365)

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
/** number of days in months for leap and normal years */
static const uint8_t _ytab[2][12] = {
        {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
        {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

/*==============================================================================
  Exported objects
==============================================================================*/
/** statically allocated time structure */
struct tm _tmbuf;

/** local time offset */
int _ltimeoff;

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief  Function check that given year is leap year or not
 * @param  year         year
 * @return If year is leap then true is returned, otherwise false
 */
//==============================================================================
static bool is_leap_year(int year)
{
        return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

//==============================================================================
/**
 * @brief Function convert ASCII to the number
 * When function find any other character than number (depended of actual base)
 * immediately finished operation and return pointer when bad character was
 * found. If base is 0 then function recognize type of number used in string.
 * For hex values "0x" is recognized, for octal values "0" at the beginning of
 * string is recognized, for binary "0b" is recognized, and for decimals values
 * none above.
 *
 * @param[in]  string       string to decode
 * @param[in]  base         decode base
 * @param[out] value        pointer to result
 *
 * @return pointer in string when operation was finished
 */
//==============================================================================
char *_strtoi(const char *string, int base, i32_t *value)
{
        *value = 0;

        i32_t sign = 1;
        if (string[0] == '-') {
                string++;
                sign = -1;
        } else if (string[0] == '+') {
                string++;
                sign = 1;
        }

        if (base == 0) {
                if (strncmp("0x", string, 2) == 0) {
                        base    = 16;
                        string += 2;
                } else if (strncmp("0b", string, 2) == 0) {
                        base    = 2;
                        string += 2;
                } else if (strncmp("0", string, 1) == 0) {
                        base    = 8;
                        string += 1;
                } else {
                        base = 10;
                }
        } else if (base < 2 || base > 16) {
                return const_cast(char*, string);
        }

        char  character;
        bool  char_found = false;
        while ((character = *string) != '\0') {
                /* if space exist, atoi continue finding correct character */
                if ((character == ' ') && (char_found == false)) {
                        string++;
                        continue;
                } else {
                        char_found = true;
                }

                /* check character range */
                if (character >= 'a') {
                        character -= 'a' - 10;
                } else if (character >= 'A') {
                        character -= 'A' - 10;
                } else if (character >= '0') {
                        character -= '0';
                } else {
                        break;
                }

                /* check character range according to actual base */
                if (character >= base) {
                        break;
                }

                /* compute value */
                *value = *value * base;
                *value = *value + character;

                string++;
        }

        *value *= sign;

        return const_cast(char*, string);
}

//==============================================================================
/**
 * @brief Function convert string to integer
 *
 * @param[in] str       string
 *
 * @return converted value
 */
//==============================================================================
i32_t _atoi(const char *str)
{
        i32_t result;
        _strtoi(str, 10, &result);
        return result;
}

//==============================================================================
/**
 * @brief Function convert string to double
 *
 * @param[in]  str             string
 * @param[out] end             the pointer to the character when conversion was finished
 *
 * @return converted value
 */
//==============================================================================
double _strtod(const char *str, char **end)
{
        double sign    = 1;
        double div     = 1;
        double number  = 0;
        int    i       = 0;
        int    decimal = 0;
        bool   point   = false;

        while (str[i] != '\0') {
                char num = str[i];

                if (num >= '0' && num <= '9') {
                        number *= 10;
                        number += (double) (num - '0');
                } else if (num == '.' && !point) {
                        point = true;
                        i++;
                        continue;
                } else if (num == '-') {
                        sign = -1;
                        if (!isdigit((int)str[i + 1])) {
                                i = 0;
                                break;
                        }
                        i++;
                        continue;
                } else if (num == '+') {
                        if (!isdigit((int)str[i + 1])) {
                                i = 0;
                                break;
                        }
                        i++;
                        continue;
                } else if (strchr(" \n\t+", num) == NULL) {
                        break;
                }

                if (point) {
                        decimal++;
                }

                i++;
        }

        if (point) {
                for (int j = 0; j < decimal; j++) {
                        div *= 10;
                }
        }

        if (end)
                *end = (char *) &str[i];

        return sign * (number / div);
}


//==============================================================================
/**
 * @brief Function convert string to float
 *
 * @param[in] str      string
 *
 * @return converted value
 */
//==============================================================================
double _atof(const char *str)
{
        return _strtod(str, NULL);
}

//==============================================================================
/**
 * @brief  Convert date to UNIX time (Epoch)
 *
 * @param  tm           pointer to struct tm object
 *
 * @return UNIX time value (Epoch)
 */
//==============================================================================
u32_t _mktime(struct tm *tm)
{
        if (  tm->tm_mday >= 1
           && tm->tm_mday <= 31
           && tm->tm_mon  >= 0
           && tm->tm_mon  <= 11
           && tm->tm_hour <= 23
           && tm->tm_min  <= 59
           && tm->tm_sec  <= 59  ) {

                uint16_t yday = 0;
                for (int i = 0; i < tm->tm_mon; i++) {
                        yday += _ytab[is_leap_year(tm->tm_year)][i];
                }

                yday += tm->tm_mday - 1;

                return tm->tm_sec + tm->tm_min * SECS_MIN + tm->tm_hour * SECS_HOUR
                       + yday * SECS_DAY + (tm->tm_year - 70) * SECS_YEAR + ((tm->tm_year - 69) / 4) * SECS_DAY
                       - ((tm->tm_year - 1) / 100) * SECS_DAY + ((tm->tm_year + 299) / 400) * SECS_DAY;
        } else {
                return -1;
        }
}

//==============================================================================
/**
 * @brief  Convert UNIX time (Epoch) to date
 *
 * @param[in]  timer        time value (Epoch)
 * @param[out] tmbuf        user's tm buffer
 *
 * @return On success return tmpbuf, otherwise NULL.
 */
//==============================================================================
struct tm *_gmtime_r(const time_t *timer, struct tm *tmbuf)
{
        if (timer && tmbuf) {
                time_t time = *timer;
                u32_t  year = EPOCH_YR;

                u32_t dayclock = (u32_t)time % SECS_DAY;
                u32_t dayno    = (u32_t)time / SECS_DAY;

                tmbuf->tm_sec  = dayclock % SECS_MIN;
                tmbuf->tm_min  = (dayclock % SECS_HOUR) / SECS_MIN;
                tmbuf->tm_hour = dayclock / SECS_HOUR;
                tmbuf->tm_wday = (dayno + DAY0) % 7;

                while (dayno >= (u32_t)YEARSIZE(year)) {
                        dayno -= YEARSIZE(year);
                        year++;
                }

                tmbuf->tm_year = year - YEAR0;
                tmbuf->tm_yday = dayno;
                tmbuf->tm_mon  = 0;

                while (dayno >= (u32_t)_ytab[is_leap_year(year)][tmbuf->tm_mon]) {
                        dayno -= _ytab[is_leap_year(year)][tmbuf->tm_mon];
                        tmbuf->tm_mon++;
                }

                tmbuf->tm_mday  = dayno + 1;
                tmbuf->tm_isdst = 0;
                tmbuf->tm_isutc = 1;

                return tmbuf;
        } else {
                return NULL;
        }
}

//==============================================================================
/**
 * @brief  Convert UNIX time (Epoch) to date (local time)
 *
 * @param[in]  timer        time value (Epoch)
 * @param[out] tmbuf        user's tm buffer
 *
 * @return On success return tmpbuf, otherwise NULL.
 */
//==============================================================================
struct tm *_localtime_r(const time_t *timer, struct tm *tmbuf)
{
        if (timer) {
                time_t localtime = *timer + _ltimeoff;
                struct tm *tm = _gmtime_r(&localtime, tmbuf);
                tmbuf->tm_isutc = 0;
                return tm;
        } else {
                return NULL;
        }
}

/*==============================================================================
  End of file
==============================================================================*/
