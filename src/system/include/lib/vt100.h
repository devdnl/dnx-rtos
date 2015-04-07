/*=========================================================================*//**
@file    vt1400.h

@author  Daniel Zorychta

@brief   VT100 commands

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

#ifndef _VT100_H_
#define _VT100_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/** VT100 terminal commands */
#define VT100_ENABLE_LINE_WRAP                  "\033[?7h"
#define VT100_SET_NEW_LINE_MODE                 "\033[20h"
#define VT100_SET_LINE_FEED_MODE                "\033[20l"
#define VT100_CLEAR_SCREEN                      "\033[2J\033[H"
#define VT100_ERASE_LINE                        "\033[2K"
#define VT100_ERASE_LINE_END                    "\033[K"
#define VT100_CURSOR_HOME                       "\033[H"
#define VT100_CURSOR_FORWARD(n)                 "\033["#n"C"
#define VT100_CURSOR_BACKWARD(n)                "\033["#n"D"
#define VT100_CURSOR_OFF                        "\033[?25l"
#define VT100_CURSOR_ON                         "\033[?25h"
#define VT100_ERASE_LINE_FROM_CUR               "\033[K"
#define VT100_SHIFT_CURSOR_RIGHT(t)             "\033["#t"C"
#define VT100_SHIFT_CURSOR_LEFT(t)              "\033["#t"D"
#define VT100_CLEAR_LINE                        "\r\033[K"
#define VT100_RESET_ATTRIBUTES                  "\033[0m"
#define VT100_DISABLE_LINE_WRAP                 "\033[7l"
#define VT100_CURSOR_OFF                        "\033[?25l"
#define VT100_CURSOR_ON                         "\033[?25h"
#define VT100_CURSOR_HOME                       "\033[H"
#define VT100_SAVE_CURSOR_POSITION              "\0337"
#define VT100_RESTORE_CURSOR_POSITION           "\0338"
#define VT100_SET_CURSOR_POSITION(r, c)         "\033["#r";"#c"H"
#define VT100_QUERY_CURSOR_POSITION             "\033[6n"
#define VT100_ARROW_UP                          "\033[A"
#define VT100_ARROW_UP_STDOUT                   "\033^[A"
#define VT100_ARROW_DOWN                        "\033[B"
#define VT100_ARROW_DOWN_STDOUT                 "\033^[B"
#define VT100_TAB                               "\033^[T"
#define VT100_ARROW_LEFT                        "\033[D"
#define VT100_ARROW_RIGHT                       "\033[C"
#define VT100_HOME                              "\033[1~"
#define VT100_INS                               "\033[2~"
#define VT100_DEL                               "\033[3~"
#define VT100_END_1                             "\033[4~"
#define VT100_END_2                             "\033OF"
#define VT100_PGUP                              "\033[5~"
#define VT100_PGDN                              "\033[6~"
#define VT100_F1                                "\033OP"
#define VT100_F2                                "\033OQ"
#define VT100_F3                                "\033OR"
#define VT100_F4                                "\033OS"
#define VT100_F5                                "\033[16~"
#define VT100_F6                                "\033[17~"
#define VT100_F7                                "\033[18~"
#define VT100_F8                                "\033[19~"
#define VT100_F9                                "\033[20~"
#define VT100_F10                               "\033[21~"
#define VT100_F11                               "\033[23~"
#define VT100_F12                               "\033[24~"


#if (CONFIG_COLOR_TERMINAL_ENABLE > 0)
#       define VT100_RESET_ATTRIBUTES           "\033[0m"
#       define VT100_FONT_BLINKING              "\033[5m"
#       define VT100_FONT_UNDERLINE             "\033[4m"
#       define VT100_FONT_NORMAL                "\033[0m"
#       define VT100_FONT_BOLD                  "\033[1m"
#       define VT100_FONT_COLOR_BLACK           "\033[30m"
#       define VT100_FONT_COLOR_GRAY            "\033[1;30m"
#       define VT100_FONT_COLOR_RED             "\033[31m"
#       define VT100_FONT_COLOR_GREEN           "\033[32m"
#       define VT100_FONT_COLOR_YELLOW          "\033[1;33m"
#       define VT100_FONT_COLOR_BROWN           "\033[33m"
#       define VT100_FONT_COLOR_BLUE            "\033[34m"
#       define VT100_FONT_COLOR_LIGHT_BLUE      "\033[1;34m"
#       define VT100_FONT_COLOR_MAGENTA         "\033[35m"
#       define VT100_FONT_COLOR_PINK            "\033[1;35m"
#       define VT100_FONT_COLOR_CYAN            "\033[36m"
#       define VT100_FONT_COLOR_WHITE           "\033[37m"
#       define VT100_BACK_COLOR_BLACK           "\033[40m"
#       define VT100_BACK_COLOR_GRAY            "\033[1;40m"
#       define VT100_BACK_COLOR_RED             "\033[41m"
#       define VT100_BACK_COLOR_GREEN           "\033[42m"
#       define VT100_BACK_COLOR_YELLOW          "\033[43m"
#       define VT100_BACK_COLOR_BROWN           "\033[1;43m"
#       define VT100_BACK_COLOR_BLUE            "\033[44m"
#       define VT100_BACK_COLOR_MAGENTA         "\033[45m"
#       define VT100_BACK_COLOR_PINK            "\033[1;45m"
#       define VT100_BACK_COLOR_CYAN            "\033[46m"
#       define VT100_BACK_COLOR_WHITE           "\033[47m"
#else
#       define VT100_RESET_ATTRIBUTES
#       define VT100_FONT_BLINKING
#       define VT100_FONT_UNDERLINE
#       define VT100_FONT_NORMAL
#       define VT100_FONT_BOLD
#       define VT100_FONT_COLOR_BLACK
#       define VT100_FONT_COLOR_GRAY
#       define VT100_FONT_COLOR_RED
#       define VT100_FONT_COLOR_GREEN
#       define VT100_FONT_COLOR_YELLOW
#       define VT100_FONT_COLOR_BROWN
#       define VT100_FONT_COLOR_BLUE
#       define VT100_FONT_COLOR_LIGHT_BLUE
#       define VT100_FONT_COLOR_MAGENTA
#       define VT100_FONT_COLOR_PINK
#       define VT100_FONT_COLOR_CYAN
#       define VT100_FONT_COLOR_WHITE
#       define VT100_BACK_COLOR_BLACK
#       define VT100_BACK_COLOR_GRAY
#       define VT100_BACK_COLOR_RED
#       define VT100_BACK_COLOR_GREEN
#       define VT100_BACK_COLOR_YELLOW
#       define VT100_BACK_COLOR_BROWN
#       define VT100_BACK_COLOR_BLUE
#       define VT100_BACK_COLOR_MAGENTA
#       define VT100_BACK_COLOR_PINK
#       define VT100_BACK_COLOR_CYAN
#       define VT100_BACK_COLOR_WHITE
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

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _VT100_H_ */
/*==============================================================================
  End of file
==============================================================================*/
