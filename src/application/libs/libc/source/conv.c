/*=========================================================================*//**
@file    conv.c

@author  Daniel Zorychta

@brief   Module with calculation and convert functions.

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

/*==============================================================================
  Include files
==============================================================================*/
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <dnx/misc.h>
#include <sys/types.h>
#include <sys/time.h>
#include "syscall.h"

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
extern struct tm _libc_tmbuf;

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
 * @brief  Reverse selected buffer
 * @param  begin        beggining of the buffer
 * @param  end          end of the buffer
 * @return None
 */
//==============================================================================
static void reverse_buffer(char *begin, char *end)
{
        while (end > begin) {
                char tmp = *end;
                *end--   = *begin;
                *begin++ = tmp;
        }
}

//==============================================================================
/**
 * @brief Function convert value to the character
 *
 * @param  val          converted value
 * @param *buf          result buffer
 * @param  base         conversion base
 * @param  usign_val    unsigned value conversion
 * @param  zeros_req    zeros required (added zeros to conversion)
 *
 * @return pointer in the buffer
 */
//==============================================================================
char *_libc_itoa(i64_t val, char *buf, u8_t base, bool usign_val, u8_t zeros_req)
{
        static const char digits[]  = "0123456789ABCDEF";
        char             *buf_start = buf;

        if (base >= 2 && base <= 16) {
                bool sign     = val < 0 && !usign_val;
                u8_t zero_cnt = 0;

                if (base == 10 && sign) {
                        val = -val;
                }

                u64_t quot, rem;
                do {
                        if (usign_val) {
                                quot = cast(u64_t, val) / base;
                                rem  = cast(u64_t, val) % base;
                        } else {
                                quot = cast(u64_t, val) / base;
                                rem  = cast(u64_t, val) % base;
                        }

                        *buf++ = digits[rem];
                        zero_cnt++;

                } while ((val = quot));

                while (zeros_req > zero_cnt) {
                        *buf++ = '0';
                        zero_cnt++;
                }

                if (sign) {
                        *buf++ = '-';
                }

                reverse_buffer(buf_start, buf - 1);
        }

        *buf = '\0';
        return buf_start;
}

//==============================================================================
/**
 * @brief Function convert double to string
 *
 * @note used software: nickgsuperstar@gmail.com & nickg@client9.com
 *                      https://code.google.com/p/stringencoders/
 *
 * @param[in]   value           input value
 * @param[out] *str             string - result
 * @param[in]   prec            precision
 * @param[in]   n               buffer size
 *
 * @return number of characters
 */
//==============================================================================
int _libc_dtoa(double value, char *str, int prec, int n)
{
        const double pow10[] = {1, 10, 100, 1000, 10000, 100000, 1000000,
                                10000000, 100000000, 1000000000};

        int   conv = 0;
        char *wstr = str;

        void push_char(const char c)
        {
                if (conv < n) {
                        *wstr++ = c;
                        conv++;
                }
        }

        /*
         * Hacky test for NaN
         * under -fast-math this won't work, but then you also won't
         * have correct nan values anyways.  The alternative is
         * to link with libmath (bad) or hack IEEE double bits (bad)
         */
        if (!(value == value)) {
                push_char('n');
                push_char('a');
                push_char('n');
                return conv;
        }

        /* if input is larger than thres_max, revert to exponential */
        const double thres_max = cast(double, 0x7FFFFFFF);

        double diff  = 0.0;

        if (prec < 0) {
                prec = 0;
        } else if (prec > 9) {
                /* precision of >= 10 can lead to overflow errors */
                prec = 9;
        }

        /* we'll work in positive values and deal with the negative sign issue later */
        int neg = 0;
        if (value < 0) {
                neg = 1;
                value = -value;
        }

        int    whole = (int) value;
        double tmp   = (value - whole) * pow10[prec];
        u32_t  frac  = (u32_t)tmp;

        diff = tmp - frac;

        if (diff > 0.5) {
                ++frac;

                /* handle rollover, e.g.  case 0.99 with prec 1 is 1.0  */
                if (frac >= pow10[prec]) {
                        frac = 0;
                        ++whole;
                }
        } else if (diff == 0.5 && ((frac == 0) || (frac & 1))) {
                /* if halfway, round up if odd, OR if last digit is 0.  That last part is strange */
                ++frac;
        }

        /* for very large numbers switch back to native sprintf for exponentials.
         anyone want to write code to replace this? */
        /*
         normal printf behavior is to print EVERY whole number digit
         which can be 100s of characters overflowing your buffers == bad
         */
        if (value > thres_max) {
                push_char('E');
                push_char('?');
                return conv;
        }

        if (prec == 0) {
                diff = value - whole;
                if (diff > 0.5) {
                        /* greater than 0.5, round up, e.g. 1.6 -> 2 */
                        ++whole;
                } else if (diff == 0.5 && (whole & 1)) {
                        /* exactly 0.5 and ODD, then round up */
                        /* 1.5 -> 2, but 2.5 -> 2 */
                        ++whole;
                }
        } else {
                int count = prec;

                /* now do fractional part, as an unsigned number */
                do {
                        --count;
                        push_char((char) (48 + (frac % 10)));
                } while (frac /= 10);

                /* add extra 0s */
                while (count-- > 0) {
                        push_char('0');
                }

                /* add decimal */
                push_char('.');
        }

        /* Do whole part. Take care of sign. Conversion. Number is reversed. */
        do {
                push_char((char) (48 + (whole % 10)));
        } while (whole /= 10);

        if (neg) {
                push_char('-');
        }

        reverse_buffer(str, wstr - 1);

        return conv;
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
char *_libc_strtoi(const char *string, int base, i32_t *value)
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
                return const_cast(string);
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

        return const_cast(string);
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
i32_t _libc_atoi(const char *str)
{
        i32_t result;
        _libc_strtoi(str, 10, &result);
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
double _libc_strtod(const char *str, char **end)
{
        double sign    = 1;
        double div     = 1;
        double number  = 0;
        int    i       = 0;
        int    decimal = 0;
        int    base    = 10;
        bool   isnum   = false;
        bool   point   = false;

        while (str[i] != '\0') {
                if (isspace(str[i])) {
                        if (isnum) {
                                break;
                        }
                } else {
                        isnum = true;

                        if ((str[i] == '0') && (tolower(str[i+1]) == 'x')) {
                                base = 16;
                                i   += 2;
                                continue;
                        }

                        char num = tolower(str[i]);

                        if ((num >= '0') && (num <= '9')) {
                                number *= base;
                                number += (double) (num - '0');
                        } else if ((num >= 'a') && (num <= 'f')) {
                                number *= base;
                                number += (double) (num - 'a') + 10;
                        } else if (num == '.' && !point) {
                                point = true;
                                i++;
                                continue;
                        } else if (num == '-') {
                                if (!isdigit((int)str[i + 1])) {
                                        i = 0;
                                        break;
                                } else {
                                        sign = -1;
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
                        } else if (isspace(num) || isprint(num)) {
                                break;
                        }

                        if (point) {
                                decimal++;
                        }
                }

                i++;
        }

        if (point) {
                for (int j = 0; j < decimal; j++) {
                        div *= base;
                }
        }

        if (end) {
                *end = (char *) &str[i];
        }

        return sign * (number / div);
}

//==============================================================================
/**
 * @brief Function convert string to float
 *
 * @param[in]  str             string
 * @param[out] end             the pointer to the character when conversion was finished
 *
 * @return converted value
 */
//==============================================================================
float _libc_strtof(const char *str, char **end)
{
        float  sign    = 1;
        float  div     = 1;
        float  number  = 0;
        int    i       = 0;
        int    decimal = 0;
        int    base    = 10;
        bool   isnum   = false;
        bool   point   = false;

        while (str[i] != '\0') {
                if (isspace(str[i])) {
                        if (isnum) {
                                break;
                        }
                } else {
                        isnum = true;

                        if ((str[i] == '0') && (tolower(str[i+1]) == 'x')) {
                                base = 16;
                                i   += 2;
                                continue;
                        }

                        char num = tolower(str[i]);

                        if ((num >= '0') && (num <= '9')) {
                                number *= base;
                                number += (float) (num - '0');
                        } else if ((num >= 'a') && (num <= 'f')) {
                                number *= base;
                                number += (float) (num - 'a') + 10;
                        } else if (num == '.' && !point) {
                                point = true;
                                i++;
                                continue;
                        } else if (num == '-') {
                                if (!isdigit((int)str[i + 1])) {
                                        i = 0;
                                        break;
                                } else {
                                        sign = -1;
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
                        } else if (isspace(num) || isprint(num)) {
                                break;
                        }

                        if (point) {
                                decimal++;
                        }
                }

                i++;
        }

        if (point) {
                for (int j = 0; j < decimal; j++) {
                        div *= base;
                }
        }

        if (end) {
                *end = (char *) &str[i];
        }

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
double _libc_atof(const char *str)
{
        return _libc_strtod(str, NULL);
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
time_t _libc_mktime(struct tm *tm)
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
struct tm *_libc_gmtime_r(const time_t *timer, struct tm *tmbuf)
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
struct tm *_libc_localtime_r(const time_t *timer, struct tm *tmbuf)
{
        if (timer) {
                struct timezone tz;
                int err = _libc_syscall(_LIBC_SYS_GETTIMEOFDAY, NULL, &tz);

                if (!err) {
                        time_t localtime = *timer + (tz.tz_minuteswest * 60);
                        struct tm *tm = _libc_gmtime_r(&localtime, tmbuf);
                        tmbuf->tm_isutc = 0;
                        return tm;
                }
        }

        return NULL;
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
struct tm *gmtime(const time_t *timer)
{
        return _libc_gmtime_r(timer, &(((_libc_app_ctx_t*)(*_libc_app_ctx))->_tmbuf));
}

//==============================================================================
/**
 * @brief  Convert time_t to tm as UTC time
 *
 * Uses the value pointed by timer to fill a tm structure with the values that
 * represent the corresponding time, expressed as a UTC time (i.e., the time
 * at the GMT timezone).
 *
 * @param[in]  timer    Pointer to an object of type time_t that contains a time value.
 *                      time_t is an alias of a fundamental arithmetic type
 *                      capable of representing times as returned by function time.
 *
 * @param[out] tm       Pointer to an object of type struct tm that will contains
 *                      converted timer value to time structure.
 *
 * @return A pointer to a tm structure with its members filled with the values
 *         that correspond to the UTC time representation of timer.
 */
//==============================================================================
struct tm *gmtime_r(const time_t *timer, struct tm *tm)
{
        return _libc_gmtime_r(timer, tm);
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
struct tm *localtime(const time_t *timer)
{
        return _libc_localtime_r(timer, &(((_libc_app_ctx_t*)(*_libc_app_ctx))->_tmbuf));
}

//==============================================================================
/**
 * @brief  Convert time_t to tm as local time
 *
 * Uses the value pointed by timer to fill a tm structure with the values that
 * represent the corresponding time, expressed for the local timezone.
 *
 * @param[in]  timer    Pointer to an object of type time_t that contains a time value.
 *                      time_t is an alias of a fundamental arithmetic type
 *                      capable of representing times as returned by function time.
 *
 * @param[out] tm       Pointer to an object of type struct tm that will contains
 *                      converted timer value to time structure.
 *
 * @return A pointer to a tm structure with its members filled with the values
 *         that correspond to the local time representation of timer.
 */
//==============================================================================
struct tm *localtime_r(const time_t *timer, struct tm *tm)
{
        return _libc_localtime_r(timer, tm);
}

/*==============================================================================
  End of file
==============================================================================*/
