#ifndef IO_H_
#define IO_H_
/*=========================================================================*//**
@file    io.h

@author  Daniel Zorychta

@brief   This file support standard io functions

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <stdarg.h>
#include "systypes.h"
#include "config.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/** stream values */
#define EOF                                     (-1)

/** stdio buffer size */
#define BUFSIZ                                  CONFIG_FSCANF_STREAM_BUFFER_SIZE

/** translate function to STDC */
#define printf(...)                             io_fprintf(stdout, __VA_ARGS__)
#define fprintf(...)                            io_fprintf(__VA_ARGS__)
#define snprintf(bfr, size, ...)                io_snprintf(bfr, size, __VA_ARGS__)
#define sprintf(bfr, ...)                       io_snprintf(bfr, UINT16_MAX, __VA_ARGS__)
#define vsnprintf(bfr, size, args)              io_vsnprintf(bfr, size, args)
#define printk(...)                             io_printk(__VA_ARGS__)
#define enable_printk(path)                     io_enable_printk(path)
#define disable_printk()                        io_disable_printk()
#define scanf(format, ...)                      io_fscanf(stdin, format, __VA_ARGS__)
#define fscanf(stream, format, ...)             io_fscanf(stream, format, __VA_ARGS__)
#define sscanf(str, format, ...)                io_sscanf(str, format, __VA_ARGS__)
#define vsscanf(str, format, args)              io_vsscanf(str, format, args)
#define putc(c, stream)                         io_fputc(c, stream)
#define fputc(c, stream)                        io_fputc(c, stream)
#define fputs(s, stream)                        io_fputs(s, stream)
#define putchar(c)                              io_fputc(c, stdout)
#define getchar()                               io_getc(stdin)
#define getc(stream)                            io_getc(stream)
#define fgets(str, size, stream)                io_fgets(str, size, stream)
#define atoi(string, base, valuePtr)            io_atoi(string, base, valuePtr)
#define atof(string)                            io_atof(string)
#define strtod(string, end)                     io_strtod(string, end)

/** VT100 terminal commands */
#define ENABLE_LINE_WRAP                        "\e[?7h"
#define SET_NEW_LINE_MODE                       "\e[20h"
#define SET_LINE_FEED_MODE                      "\e[20l"
#define CLEAR_SCREEN                            "\e[2J"
#define ERASE_LINE                              "\e[2K"
#define ERASE_LINE_END                          "\e[K"
#define CURSOR_HOME                             "\e[H"
#if (CONFIG_COLOR_TERM_ENABLE > 0)
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
extern void   io_printk(const char *format, ...);
extern void   io_enable_printk(char *filename);
extern void   io_disable_printk(void);
#endif
#if (CONFIG_PRINTF_ENABLE > 0)
extern int    io_snprintf(char *buf, u32_t size, const char *format, ...);
extern int    io_fprintf(file_t *file, const char *format, ...);
extern int    io_vsnprintf(char *buf, size_t size, const char *format, va_list arg);
#endif
#if (CONFIG_SCANF_ENABLE > 0)
extern int    io_fscanf(file_t *buf, const char *format, ...);
extern int    io_sscanf(const char *str, const char *format, ...);
extern int    io_vsscanf(const char *str, const char *format, va_list args);
#endif
extern double io_strtod(const char*, char**);
extern char  *io_atoi(char *string, u8_t base, i32_t *value);
extern double io_atof(const char*);
extern int    io_fputc(int c, file_t *buf);
extern int    io_fputs(const char*, file_t*);
extern int    io_getc(file_t *buf);
extern char  *io_fgets(char *str, int size, file_t *stream);

#ifdef __cplusplus
}
#endif

#endif /* IO_H_ */
/*==============================================================================
  End of file
==============================================================================*/
