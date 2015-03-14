/*=========================================================================*//**
@file    printx.h

@author  Daniel Zorychta

@brief   Basic print functions

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

#ifndef _PRINTX_H_
#define _PRINTX_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <stdarg.h>
#include <stddef.h>
#include "core/vfs.h"
#include "config.h"

/*==============================================================================
  Exported macros
==============================================================================*/
/** VT100 terminal commands */
#define ENABLE_LINE_WRAP                        "\033[?7h"
#define SET_NEW_LINE_MODE                       "\033[20h"
#define SET_LINE_FEED_MODE                      "\033[20l"
#define CLEAR_SCREEN                            "\033[2J\033[H"
#define ERASE_LINE                              "\033[2K"
#define ERASE_LINE_END                          "\033[K"
#define CURSOR_HOME                             "\033[H"
#define CURSOR_FORWARD(n)                       "\033["#n"C"
#define CURSOR_BACKWARD(n)                      "\033["#n"D"
#define CURSOR_OFF                              "\033[?25l"
#define CURSOR_ON                               "\033[?25h"

#if (CONFIG_COLOR_TERMINAL_ENABLE > 0)
#       define RESET_ATTRIBUTES                 "\033[0m"
#       define FONT_BLINKING                    "\033[5m"
#       define FONT_UNDERLINE                   "\033[4m"
#       define FONT_NORMAL                      "\033[0m"
#       define FONT_BOLD                        "\033[1m"
#       define FONT_COLOR_BLACK                 "\033[30m"
#       define FONT_COLOR_GRAY                  "\033[1;30m"
#       define FONT_COLOR_RED                   "\033[31m"
#       define FONT_COLOR_GREEN                 "\033[32m"
#       define FONT_COLOR_YELLOW                "\033[1;33m"
#       define FONT_COLOR_BROWN                 "\033[33m"
#       define FONT_COLOR_BLUE                  "\033[34m"
#       define FONT_COLOR_LIGHT_BLUE            "\033[1;34m"
#       define FONT_COLOR_MAGENTA               "\033[35m"
#       define FONT_COLOR_PINK                  "\033[1;35m"
#       define FONT_COLOR_CYAN                  "\033[36m"
#       define FONT_COLOR_WHITE                 "\033[37m"
#       define BACK_COLOR_BLACK                 "\033[40m"
#       define BACK_COLOR_GRAY                  "\033[1;40m"
#       define BACK_COLOR_RED                   "\033[41m"
#       define BACK_COLOR_GREEN                 "\033[42m"
#       define BACK_COLOR_YELLOW                "\033[43m"
#       define BACK_COLOR_BROWN                 "\033[1;43m"
#       define BACK_COLOR_BLUE                  "\033[44m"
#       define BACK_COLOR_MAGENTA               "\033[45m"
#       define BACK_COLOR_PINK                  "\033[1;45m"
#       define BACK_COLOR_CYAN                  "\033[46m"
#       define BACK_COLOR_WHITE                 "\033[47m"
#else
#       define RESET_ATTRIBUTES
#       define FONT_BLINKING
#       define FONT_UNDERLINE
#       define FONT_NORMAL
#       define FONT_BOLD
#       define FONT_COLOR_BLACK
#       define FONT_COLOR_GRAY
#       define FONT_COLOR_RED
#       define FONT_COLOR_GREEN
#       define FONT_COLOR_YELLOW
#       define FONT_COLOR_BROWN
#       define FONT_COLOR_BLUE
#       define FONT_COLOR_LIGHT_BLUE
#       define FONT_COLOR_MAGENTA
#       define FONT_COLOR_PINK
#       define FONT_COLOR_CYAN
#       define FONT_COLOR_WHITE
#       define BACK_COLOR_BLACK
#       define BACK_COLOR_GRAY
#       define BACK_COLOR_RED
#       define BACK_COLOR_GREEN
#       define BACK_COLOR_YELLOW
#       define BACK_COLOR_BROWN
#       define BACK_COLOR_BLUE
#       define BACK_COLOR_MAGENTA
#       define BACK_COLOR_PINK
#       define BACK_COLOR_CYAN
#       define BACK_COLOR_WHITE
#endif

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern void        _printk         (const char*, ...);
extern void        _printk_enable  (char*);
extern void        _printk_disable (void);
extern int         _snprintf       (char*, size_t, const char*, ...);
extern int         _fprintf        (FILE*, const char*, ...);
extern int         _vfprintf       (FILE*, const char*, va_list);
extern int         _vsnprintf      (char*, size_t, const char*, va_list);
extern const char *_strerror       (int);
extern void        _perror         (const char*);
extern int         _fputc          (int, FILE*);
extern int         _f_puts         (const char*, FILE*, bool);
extern int         _getc           (FILE*);
extern char       *_fgets          (char*, int, FILE*);
extern char       *_ctime_r        (const time_t *timer, const struct tm *tm, char *buf);
extern size_t      _strftime       (char*, size_t, const char*, const struct tm*);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _PRINTX_H_ */
/*==============================================================================
  End of file
==============================================================================*/
