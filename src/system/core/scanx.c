/*=========================================================================*//**
@file    scanx.c

@author  Daniel Zorychta

@brief   Basic scan functions.

@note    Copyright (C) 2012, 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
 =============================================================================*/
#include "config.h"
#include "core/scanx.h"
#include "core/vfs.h"
#include "core/printx.h"
#include "core/sysmoni.h"
#include "core/conv.h"
#include <dnx/misc.h>
#include <string.h>

/*==============================================================================
 Local symbolic constants/macros
==============================================================================*/
#define BUFSIZ                  CONFIG_STREAM_BUFFER_LENGTH

/*==============================================================================
 Local types, enums definitions
==============================================================================*/

/*==============================================================================
 Local function prototypes
==============================================================================*/

/*==============================================================================
 Local object definitions
==============================================================================*/

/*==============================================================================
 Exported object definitions
==============================================================================*/

/*==============================================================================
 Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief Function scan stream
 *
 * @param[in]  *stream        file
 * @param[in]  *format        message format
 * @param[out]  ...           output
 *
 * @return number of scanned elements
 */
//==============================================================================
int sys_fscanf(FILE *stream, const char *format, ...)
{
#if (CONFIG_SCANF_ENABLE > 0)
        va_list arg;
        va_start(arg, format);
        int n = sys_vfscanf(stream, format, arg);
        va_end(arg);
        return n;
#else
        UNUSED_ARG(stream);
        UNUSED_ARG(format);
        return 0;
#endif
}

//==============================================================================
/**
 * @brief Function scan stream
 *
 * @param[in]  *stream        file
 * @param[in]  *format        message format
 * @param[out]  arg           output arguments
 *
 * @return number of scanned elements
 */
//==============================================================================
int sys_vfscanf(FILE *stream, const char *format, va_list arg)
{
#if (CONFIG_SCANF_ENABLE > 0)
        char *str = sysm_syscalloc(BUFSIZ, sizeof(char));
        if (!str)
                return 0;

        int n = 0;
        if (sys_fgets(str, BUFSIZ, stream) == str) {
                char *lf;
                if ((lf = strchr(str, '\n')) != NULL) {
                        *lf = '\0';
                }

                n = sys_vsscanf(str, format, arg);
        }

        sysm_sysfree(str);
        return n;
#else
        UNUSED_ARG(stream);
        UNUSED_ARG(format);
        UNUSED_ARG(arg);
        return 0;
#endif
}

//==============================================================================
/**
 * @brief Function scan arguments defined by format (multiple argument version)
 *
 * @param[in]  *str           data buffer
 * @param[in]  *format        scan format
 * @param[out]  ...           output
 *
 * @return number of scanned elements
 */
//==============================================================================
int sys_sscanf(const char *str, const char *format, ...)
{
#if (CONFIG_SCANF_ENABLE > 0)
        va_list args;
        va_start(args, format);
        int n = sys_vsscanf(str, format, args);
        va_end(args);
        return n;
#else
        UNUSED_ARG(str);
        UNUSED_ARG(format);
        return 0;
#endif
}

//============================================================================//
/**
 * @brief Function scan arguments defined by format (argument list version)
 *
 * @param[in]  *str           data buffer
 * @param[in]  *format        scan format
 * @param[out]  ...           output
 *
 * @return number of scanned elements
 */
//============================================================================//
int sys_vsscanf(const char *str, const char *format, va_list args)
{
#if (CONFIG_SCANF_ENABLE > 0)
        int    read_fields = 0;
        char   chr;
        int    value;
        char  *strs;
        int    sign;
        char  *string;
        u16_t  bfr_size;

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

                                        uint var;

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

                                        double *value = va_arg(args, double*);
                                        if (value) {
                                                char *end;
                                                *value = sys_strtod(str, &end);
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
        UNUSED_ARG(str);
        UNUSED_ARG(format);
        UNUSED_ARG(args);
        return 0;
#endif
}

/*==============================================================================
 End of file
==============================================================================*/
