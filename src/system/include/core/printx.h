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
#define ENABLE_LINE_WRAP                        "\e[?7h"
#define SET_NEW_LINE_MODE                       "\e[20h"
#define SET_LINE_FEED_MODE                      "\e[20l"
#define CLEAR_SCREEN                            "\e[2J"
#define ERASE_LINE                              "\e[2K"
#define ERASE_LINE_END                          "\e[K"
#define CURSOR_HOME                             "\e[H"
#define CURSOR_FORWARD(n)                       "\e["#n"C"
#define CURSOR_BACKWARD(n)                      "\e["#n"D"
#define CURSOR_OFF                              "\e[?25l"
#define CURSOR_ON                               "\e[?25h"

#if (CONFIG_COLOR_TERMINAL_ENABLE > 0)
#       define RESET_ATTRIBUTES                 "\e[0m"
#       define FONT_BLINKING                    "\e[5m"
#       define FONT_UNDERLINE                   "\e[4m"
#       define FONT_NORMAL                      "\e[0m"
#       define FONT_BOLD                        "\e[1m"
#       define FONT_COLOR_BLACK                 "\e[30m"
#       define FONT_COLOR_RED                   "\e[31m"
#       define FONT_COLOR_GREEN                 "\e[32m"
#       define FONT_COLOR_YELLOW                "\e[33m"
#       define FONT_COLOR_BLUE                  "\e[34m"
#       define FONT_COLOR_MAGENTA               "\e[35m"
#       define FONT_COLOR_CYAN                  "\e[36m"
#       define FONT_COLOR_WHITE                 "\e[37m"
#       define BACK_COLOR_BLACK                 "\e[40m"
#       define BACK_COLOR_RED                   "\e[41m"
#       define BACK_COLOR_GREEN                 "\e[42m"
#       define BACK_COLOR_YELLOW                "\e[43m"
#       define BACK_COLOR_BLUE                  "\e[44m"
#       define BACK_COLOR_MAGENTA               "\e[45m"
#       define BACK_COLOR_CYAN                  "\e[46m"
#       define BACK_COLOR_WHITE                 "\e[47m"
#else
#       define RESET_ATTRIBUTES
#       define FONT_BLINKING
#       define FONT_UNDERLINE
#       define FONT_NORMAL
#       define FONT_BOLD
#       define FONT_COLOR_BLACK
#       define FONT_COLOR_RED
#       define FONT_COLOR_GREEN
#       define FONT_COLOR_YELLOW
#       define FONT_COLOR_BLUE
#       define FONT_COLOR_MAGENTA
#       define FONT_COLOR_CYAN
#       define FONT_COLOR_WHITE
#       define BACK_COLOR_BLACK
#       define BACK_COLOR_RED
#       define BACK_COLOR_GREEN
#       define BACK_COLOR_YELLOW
#       define BACK_COLOR_BLUE
#       define BACK_COLOR_MAGENTA
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
extern void             printk                  (const char*, ...);
extern void             printk_enable           (char*);
extern void             printk_disable          (void);
extern int              sys_snprintf            (char*, size_t, const char*, ...);
extern int              sys_fprintf             (FILE*, const char*, ...);
extern int              sys_vfprintf            (FILE*, const char*, va_list);
extern int              sys_vsnprintf           (char*, size_t, const char*, va_list);
extern const char      *sys_strerror            (int);
extern void             sys_perror              (const char*);
extern int              sys_fputc               (int, FILE*);
extern int              sys_f_puts              (const char*, FILE*, bool);
extern int              sys_getc                (FILE*);
extern char            *sys_fgets               (char*, int, FILE*);

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
