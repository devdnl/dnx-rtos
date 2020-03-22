/*=========================================================================*//**
@file    vsscanf.c

@author  Daniel Zorychta

@brief   Print functions.

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
#include "lib/vsscanf.h"
#include "lib/conv.h"
#include "dnx/misc.h"
#include "config.h"
#include <stdint.h>

/*==============================================================================
  Local macros
==============================================================================*/
#ifndef EOF
#define EOF (-1)
#endif

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

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Function scan arguments defined by format (argument list version)
 *
 * @param[in]  *str           data buffer
 * @param[in]  *format        scan format
 * @param[out]  args          output
 *
 * @return number of scanned elements
 */
//==============================================================================
int _vsscanf(const char *str, const char *format, va_list args)
{
#if (__OS_SCANF_ENABLE__ > 0)
        int       read_fields = 0;
        char      chr;
        int       value;
        char     *strs;
        int       sign;
        char     *string;
        uint16_t  bfr_size;

        if (!str || !format) {
                return EOF;
        }

        if (str[0] == '\0') {
                return EOF;
        }

        while ((chr = *format++) != '\0') {
                if (chr == '%') {
                        chr = *format++;

                        /* calculate buffer size */
                        bfr_size = 0;
                        while (chr >= '0' && chr <= '9') {
                                bfr_size *= 10;
                                bfr_size += chr - '0';
                                chr       = *format++;
                        }

                        if (bfr_size == 0)
                            bfr_size = UINT16_MAX;

                        switch (chr) {
                        case '%':
                                if (*str == '%') {
                                        str++;
                                        continue;
                                } else {
                                        goto sscanf_end;
                                }
                                break;

                        case 'u':
                        case 'd':
                        case 'i':
                                value = 0;
                                sign  = 1;

                                while (*str == ' ') {
                                        str++;
                                }

                                if (*str == '-') {
                                        sign = -1;
                                        str++;
                                }

                                strs  = (char*)str;

                                while (*str >= '0' && *str <= '9' && bfr_size > 0) {
                                        value *= 10;
                                        value += *str - '0';
                                        str++;
                                        bfr_size--;
                                }

                                if (str != strs) {
                                        int *var = va_arg(args, int*);

                                        if (var) {
                                                *var = value * sign;
                                                read_fields++;
                                        }
                                }
                                break;

                        case 'x':
                        case 'X':
                                value = 0;
                                sign  = 1;

                                while (*str == ' ') {
                                        str++;
                                }

                                if (*str == '-') {
                                        sign = -1;
                                        str++;
                                }

                                strs  = (char*)str;

                                while (  (  (*str >= '0' && *str <= '9')
                                         || (*str >= 'a' && *str <= 'f')
                                         || (*str >= 'A' && *str <= 'F') )
                                      && (bfr_size > 0) ) {

                                        int var;

                                        if (*str >= 'a') {
                                                var = *str - 'a' + 10;
                                        } else if (*str >= 'A') {
                                                var = *str - 'A' + 10;
                                        } else if (*str >= '0') {
                                                var = *str - '0';
                                        } else {
                                                var = 0;
                                        }

                                        value *= 16;
                                        value += var;
                                        str++;
                                        bfr_size--;
                                }

                                if (strs != str) {
                                        int *var = va_arg(args, int*);

                                        if (var) {
                                                *var = value * sign;
                                                read_fields++;
                                        }
                                }
                                break;

                        case 'o':
                                value = 0;
                                sign  = 1;

                                while (*str == ' ') {
                                        str++;
                                }

                                if (*str == '-') {
                                        sign = -1;
                                        str++;
                                }

                                strs  = (char*)str;

                                while (*str >= '0' && *str <= '7' && bfr_size > 0) {
                                        value *= 8;
                                        value += *str - '0';
                                        str++;
                                        bfr_size--;
                                }

                                if (str != strs) {
                                        int *var = va_arg(args, int*);

                                        if (var) {
                                                *var = value * sign;
                                                read_fields++;
                                        }
                                }
                                break;

                        case 'c':
                                if (*str >= ' ') {
                                        char *var = va_arg(args, char*);

                                        if (var) {
                                                *var = *str;
                                                read_fields++;
                                        }
                                        str++;
                                }
                                break;

                        case 's':
                                string = va_arg(args, char*);
                                if (string) {
                                        while (*str != '\n' && *str != '\r' && *str != '\0' && *str != ' ' && bfr_size > 0) {
                                                *string++ = *str++;
                                                bfr_size--;
                                        }
                                        *string++ = '\0';

                                        if (bfr_size != 0)
                                                str++;

                                        read_fields++;
                                }
                                break;

                        case 'f':
                        case 'F':
                        case 'g':
                        case 'G':
                                if (str) {
                                        while (*str == ' ') {
                                                str++;
                                        }

                                        float *value = va_arg(args, float*);
                                        if (value) {
                                                char *end;
                                                *value = _strtod(str, &end);
                                                str += ((int)end - (int)str);

                                                if (*end != '\0')
                                                        str++;
                                        }
                                }
                                break;
                        }
                } else if (chr <= ' ') {
                        while (*str <= ' ' && *str != '\0') {
                                str++;
                        }

                        if (*str == '\0') {
                                break;
                        }
                } else {
                        while (*str == chr && chr != '%' && chr > ' ' && chr != '\0') {
                                str++;
                                chr = *format++;
                        }

                        if (chr == '%' || chr <= ' ') {
                                format--;
                                continue;
                        } else {
                                break;
                        }
                }
        }

sscanf_end:
        return read_fields;
#else
        UNUSED_ARG3(str, format, args);
        return 0;
#endif
}

/*==============================================================================
  End of file
==============================================================================*/
