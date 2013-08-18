#ifndef IO_H_
#define IO_H_
/*=========================================================================*//**
@file    io.h

@author  Daniel Zorychta

@brief   This file support standard io functions

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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <stdarg.h>
#include "config.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/** translate function to STDC */
#define printf(...)                                                     io_fprintf(stdout, __VA_ARGS__)
#define fprintf(FILE__stream, ...)                                      io_fprintf(FILE__stream, __VA_ARGS__)
#define snprintf(char__bfr, size_t__size, ...)                          io_snprintf(char__bfr, size_t__size, __VA_ARGS__)
#define sprintf(char__bfr, ...)                                         io_snprintf(char__bfr, UINT16_MAX, __VA_ARGS__)
#define vsnprintf(char__bfr, size_t__size, va_list__args)               io_vsnprintf(char__bfr, size_t__size, va_list__args)
#define printk(...)                                                     io_printk(__VA_ARGS__)
#define enable_printk(char__path)                                       io_enable_printk(char__path)
#define disable_printk()                                                io_disable_printk()
#define scanf(const_char__format, ...)                                  io_fscanf(stdin, const_char__format, __VA_ARGS__)
#define fscanf(FILE__stream, const_char__format, ...)                   io_fscanf(FILE__stream, const_char__format, __VA_ARGS__)
#define sscanf(const_char__str, const_char__format, ...)                io_sscanf(const_char__str, const_char__format, __VA_ARGS__)
#define vsscanf(const_char__str, const_char__format, va_list__args)     io_vsscanf(const_char__str, const_char__format, va_list__args)
#define putc(int__c, FILE__stream)                                      io_fputc(int__c, FILE__stream)
#define fputc(int__c, FILE__stream)                                     io_fputc(int__c, FILE__stream)
#define fputs(const_char__s, FILE__stream)                              io_f_puts(const_char__s, FILE__stream, false)
#define puts(const_char__s)                                             io_f_puts(const_char__s, stdout, true)
#define putchar(int__c)                                                 io_fputc(int__c, stdout)
#define getchar()                                                       io_getc(stdin)
#define getc(FILE__stream)                                              io_getc(FILE__stream)
#define fgets(char__pstr, int__size, FILE__stream)                      io_fgets(char__pstr, int__size, FILE__stream)
#define atoi(const_char__str)                                           io_atoi(const_char__str)
#define strtoi(const_char__str, int__base, i32_t__presult)              io_strtoi(const_char__str, int__base, i32_t__presult)
#define atof(const_char__str)                                           io_atof(const_char__str)
#define strtod(const_char__pstr, char__ppend)                           io_strtod(const_char__pstr, char__ppend)

/** VT100 terminal commands */
#define ENABLE_LINE_WRAP                        "\e[?7h"
#define SET_NEW_LINE_MODE                       "\e[20h"
#define SET_LINE_FEED_MODE                      "\e[20l"
#define CLEAR_SCREEN                            "\e[2J"
#define ERASE_LINE                              "\e[2K"
#define ERASE_LINE_END                          "\e[K"
#define CURSOR_HOME                             "\e[H"
#define CURSOR_FORWARD(n)                       "\e["#n"C"
#define CURSOR_BACKWARD(n)                      "\e["#n"D"
#if (CONFIG_COLOR_TERMINAL_ENABLE > 0)
#define RESET_ATTRIBUTES                        "\e[0m"
#define FONT_BLINKING                           "\e[5m"
#define FONT_UNDERLINE                          "\e[4m"
#define FONT_NORMAL                             "\e[0m"
#define FONT_BOLD                               "\e[1m"
#define FONT_COLOR_BLACK                        "\e[30m"
#define FONT_COLOR_RED                          "\e[31m"
#define FONT_COLOR_GREEN                        "\e[32m"
#define FONT_COLOR_YELLOW                       "\e[33m"
#define FONT_COLOR_BLUE                         "\e[34m"
#define FONT_COLOR_MAGENTA                      "\e[35m"
#define FONT_COLOR_CYAN                         "\e[36m"
#define FONT_COLOR_WHITE                        "\e[37m"
#define BACK_COLOR_BLACK                        "\e[40m"
#define BACK_COLOR_RED                          "\e[41m"
#define BACK_COLOR_GREEN                        "\e[42m"
#define BACK_COLOR_YELLOW                       "\e[43m"
#define BACK_COLOR_BLUE                         "\e[44m"
#define BACK_COLOR_MAGENTA                      "\e[45m"
#define BACK_COLOR_CYAN                         "\e[46m"
#define BACK_COLOR_WHITE                        "\e[47m"
#else
#define RESET_ATTRIBUTES
#define FONT_BLINKING
#define FONT_UNDERLINE
#define FONT_NORMAL
#define FONT_BOLD
#define FONT_COLOR_BLACK
#define FONT_COLOR_RED
#define FONT_COLOR_GREEN
#define FONT_COLOR_YELLOW
#define FONT_COLOR_BLUE
#define FONT_COLOR_MAGENTA
#define FONT_COLOR_CYAN
#define FONT_COLOR_WHITE
#define BACK_COLOR_BLACK
#define BACK_COLOR_RED
#define BACK_COLOR_GREEN
#define BACK_COLOR_YELLOW
#define BACK_COLOR_BLUE
#define BACK_COLOR_MAGENTA
#define BACK_COLOR_CYAN
#define BACK_COLOR_WHITE
#endif

#if ((CONFIG_SYSTEM_MSG_ENABLE == 0) || (CONFIG_PRINTF_ENABLE == 0))
#define io_printk(...)
#define io_enable_printk(...)
#define io_disable_printk(...)
#endif
#if (CONFIG_PRINTF_ENABLE == 0)
#define io_snprintf(...)                        0
#define io_fprintf(...)                         0
#define io_vsnprintf(...)                       0
#endif
#if (CONFIG_SCANF_ENABLE == 0)
#define io_fscanf(...)                          0
#define io_sscanf(...)                          0
#define io_vsscanf(...)                         0
#endif

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
#if ((CONFIG_SYSTEM_MSG_ENABLE > 0) && (CONFIG_PRINTF_ENABLE > 0))
extern void   io_printk(const char*, ...);
extern void   io_enable_printk(char*);
extern void   io_disable_printk(void);
#endif
#if (CONFIG_PRINTF_ENABLE > 0)
extern int    io_snprintf(char*, size_t, const char*, ...);
extern int    io_fprintf(FILE*, const char*, ...);
extern int    io_vsnprintf(char*, size_t, const char*, va_list);
#endif
#if (CONFIG_SCANF_ENABLE > 0)
extern int    io_fscanf(FILE*, const char*, ...);
extern int    io_sscanf(const char*, const char*, ...);
extern int    io_vsscanf(const char*, const char*, va_list);
#endif
extern double io_strtod(const char*, char**);
extern i32_t  io_atoi(const char *str);
extern char  *io_strtoi(const char*, int, i32_t*);
extern double io_atof(const char*);
extern int    io_fputc(int, FILE*);
extern int    io_f_puts(const char*, FILE*, bool);
extern int    io_getc(FILE*);
extern char  *io_fgets(char*, int, FILE *);

#ifdef __cplusplus
}
#endif

#endif /* IO_H_ */
/*==============================================================================
  End of file
==============================================================================*/
