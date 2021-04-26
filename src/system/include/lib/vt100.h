/*=========================================================================*//**
@file    vt100.h

@author  Daniel Zorychta

@brief   VT100 commands

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

/**
\defgroup dnx-vt100-h <dnx/vt100.h>
*/
/**@{*/

#ifndef _LIB_VT100_H_
#define _LIB_VT100_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "config.h"
#include "kernel/builtinfunc.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/** @brief Enables line wrap. */
#define VT100_ENABLE_LINE_WRAP                  "\033[?7h"

/** @brief Disable line wrap. */
#define VT100_DISABLE_LINE_WRAP                 "\033[7l"

/** @brief Set new line mode. */
#define VT100_SET_NEW_LINE_MODE                 "\033[20h"

/** @brief Set line feed mode. */
#define VT100_SET_LINE_FEED_MODE                "\033[20l"

/** @brief Clear screen. */
#define VT100_CLEAR_SCREEN                      "\033[2J\033[H"

/** @brief Erase current line. */
#define VT100_ERASE_LINE                        "\033[2K"

/** @brief Erase line end (from cursor).
 *  @see   VT100_ERASE_LINE_FROM_CUR */
#define VT100_ERASE_LINE_END                    "\033[K"

/** @brief Move cursor home. */
#define VT100_CURSOR_HOME                       "\033[H"

/** @brief Move cursor up.
 *  @param n number of lines */
#define VT100_CURSOR_UP(n)                     "\033["#n"A"

/** @brief Move cursor down.
 *  @param n number of lines */
#define VT100_CURSOR_DOWN(n)                    "\033["#n"B"

/** @brief Move cursor right
 *  @param n number of characters to move.
 *  @see   VT100_SHIFT_CURSOR_RIGHT() */
#define VT100_CURSOR_FORWARD(n)                 "\033["#n"C"

/** @brief Move cursor left
 *  @param n number of characters to move.
 *  @see   VT100_SHIFT_CURSOR_LEFT() */
#define VT100_CURSOR_BACKWARD(n)                "\033["#n"D"

/** @brief Disable cursor */
#define VT100_CURSOR_OFF                        "\033[?25l"

/** @brief Enable cursor */
#define VT100_CURSOR_ON                         "\033[?25h"

/** @brief Erase line from cursor position.
 *  @see   VT100_ERASE_LINE_END */
#define VT100_ERASE_LINE_FROM_CUR               "\033[K"

/** @brief Move cursor right
 *  @param n number of characters to move.
 *  @see   VT100_CURSOR_FORWARD() */
#define VT100_SHIFT_CURSOR_RIGHT(n)             "\033["#n"C"

/** @brief Move cursor left
 *  @param n number of characters to move.
 *  @see   VT100_CURSOR_BACKWARD() */
#define VT100_SHIFT_CURSOR_LEFT(n)              "\033["#n"D"

/** @brief Clears line and move cursor at the beginning of line. */
#define VT100_CLEAR_LINE                        "\r\033[K"

/** @brief Reset text attributes. */
#define VT100_RESET_ATTRIBUTES                  "\033[0m"

/** @brief Save cursor position.
 *  @see   VT100_RESTORE_CURSOR_POSITION */
#define VT100_SAVE_CURSOR_POSITION              "\0337"

/** @brief Restore cursor position save by using VT100_SAVE_CURSOR_POSITION macro.
 *  @see   VT100_SAVE_CURSOR_POSITION */
#define VT100_RESTORE_CURSOR_POSITION           "\0338"

/** @brief Set cursor position.
 *  @param r row
 *  @param c column */
#define VT100_SET_CURSOR_POSITION(r, c)         "\033["#r";"#c"H"

/** @brief Query cursor position. */
#define VT100_QUERY_CURSOR_POSITION             "\033[6n"

/** @brief Arrow Up key code. */
#define VT100_ARROW_UP                          "\033[A"

/** @brief Arrow Up key code (stdout version). */
#define VT100_ARROW_UP_STDOUT                   "\033^[A"

/** @brief Arrow Down key code. */
#define VT100_ARROW_DOWN                        "\033[B"

/** @brief Arrow Down key code (stdout version). */
#define VT100_ARROW_DOWN_STDOUT                 "\033^[B"

/** @brief TAB key code. */
#define VT100_TAB                               "\033^[T"

/** @brief Arrow left key code. */
#define VT100_ARROW_LEFT                        "\033[D"

/** @brief Arrow right key code. */
#define VT100_ARROW_RIGHT                       "\033[C"

/** @brief HOME key code. */
#define VT100_HOME                              "\033[1~"

/** @brief Insert key code. */
#define VT100_INS                               "\033[2~"

/** @brief DEL key code. */
#define VT100_DEL                               "\033[3~"

/** @brief END key code (variant 1). */
#define VT100_END_1                             "\033[4~"

/** @brief END key code (variant 2). */
#define VT100_END_2                             "\033OF"

/** @brief PgUp key code. */
#define VT100_PGUP                              "\033[5~"

/** @brief PgDn key code. */
#define VT100_PGDN                              "\033[6~"

/** @brief F1 key code. */
#define VT100_F1_XTERM                          "\033OP"
#define VT100_F1_VT                             "\033[11~"

/** @brief F2 key code. */
#define VT100_F2_XTERM                          "\033OQ"
#define VT100_F2_VT                             "\033[12~"

/** @brief F3 key code. */
#define VT100_F3_XTERM                          "\033OR"
#define VT100_F3_VT                             "\033[13~"

/** @brief F4 key code. */
#define VT100_F4_XTERM                          "\033OS"
#define VT100_F4_VT                             "\033[14~"

/** @brief F5 key code. */
#define VT100_F5_XTERM                          "\033[16~"
#define VT100_F5_VT                             "\033[15~"

/** @brief F6 key code. */
#define VT100_F6_VT                             "\033[17~"

/** @brief F7 key code. */
#define VT100_F7_VT                             "\033[18~"

/** @brief F8 key code. */
#define VT100_F8_VT                             "\033[19~"

/** @brief F9 key code. */
#define VT100_F9_VT                             "\033[20~"

/** @brief F10 key code. */
#define VT100_F10_VT                            "\033[21~"

/** @brief F11 key code. */
#define VT100_F11_VT                            "\033[23~"

/** @brief F12 key code. */
#define VT100_F12_VT                            "\033[24~"


#if (__OS_COLOR_TERMINAL_ENABLE__ > 0)
#define VT100_FONT_BLINKING              "\033[5m"
#define VT100_FONT_UNDERLINE             "\033[4m"
#define VT100_FONT_NORMAL                "\033[0m"
#define VT100_FONT_BOLD                  "\033[1m"
#define VT100_FONT_COLOR_BLACK           "\033[30m"
#define VT100_FONT_COLOR_GRAY            "\033[1;30m"
#define VT100_FONT_COLOR_RED             "\033[31m"
#define VT100_FONT_COLOR_GREEN           "\033[32m"
#define VT100_FONT_COLOR_YELLOW          "\033[1;33m"
#define VT100_FONT_COLOR_BROWN           "\033[33m"
#define VT100_FONT_COLOR_BLUE            "\033[34m"
#define VT100_FONT_COLOR_LIGHT_BLUE      "\033[1;34m"
#define VT100_FONT_COLOR_MAGENTA         "\033[35m"
#define VT100_FONT_COLOR_PINK            "\033[1;35m"
#define VT100_FONT_COLOR_CYAN            "\033[36m"
#define VT100_FONT_COLOR_WHITE           "\033[37m"
#define VT100_BACK_COLOR_BLACK           "\033[40m"
#define VT100_BACK_COLOR_GRAY            "\033[1;40m"
#define VT100_BACK_COLOR_RED             "\033[41m"
#define VT100_BACK_COLOR_GREEN           "\033[42m"
#define VT100_BACK_COLOR_YELLOW          "\033[43m"
#define VT100_BACK_COLOR_BROWN           "\033[1;43m"
#define VT100_BACK_COLOR_BLUE            "\033[44m"
#define VT100_BACK_COLOR_MAGENTA         "\033[45m"
#define VT100_BACK_COLOR_PINK            "\033[1;45m"
#define VT100_BACK_COLOR_CYAN            "\033[46m"
#define VT100_BACK_COLOR_WHITE           "\033[47m"
#else
/** @brief Enable blinking font. To disable blinking use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_FONT_BLINKING

/** @brief Enable font underline. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_FONT_UNDERLINE

/** @brief Enable normal font. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_FONT_NORMAL

/** @brief Enable bold font. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_FONT_BOLD

/** @brief Enable black font color. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_FONT_COLOR_BLACK

/** @brief Enable gray font color. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_FONT_COLOR_GRAY

/** @brief Enable red font color. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_FONT_COLOR_RED

/** @brief Enable green font color. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_FONT_COLOR_GREEN

/** @brief Enable yellow font color. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_FONT_COLOR_YELLOW

/** @brief Enable brown font color. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_FONT_COLOR_BROWN

/** @brief Enable blue font color. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_FONT_COLOR_BLUE

/** @brief Enable light blue font color. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_FONT_COLOR_LIGHT_BLUE

/** @brief Enable magenta font color. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_FONT_COLOR_MAGENTA

/** @brief Enable pink font color. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_FONT_COLOR_PINK

/** @brief Enable cyan font color. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_FONT_COLOR_CYAN

/** @brief Enable white font color. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_FONT_COLOR_WHITE

/** @brief Enable  background font color. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_BACK_COLOR_BLACK

/** @brief Enable gray background font color. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_BACK_COLOR_GRAY

/** @brief Enable red background font color. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_BACK_COLOR_RED

/** @brief Enable green background font color. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_BACK_COLOR_GREEN

/** @brief Enable yellow background font color. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_BACK_COLOR_YELLOW

/** @brief Enable brown background font color. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_BACK_COLOR_BROWN

/** @brief Enable blue background font color. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_BACK_COLOR_BLUE

/** @brief Enable magenta background font color. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_BACK_COLOR_MAGENTA

/** @brief Enable pink background font color. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_BACK_COLOR_PINK

/** @brief Enable cyan background font color. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_BACK_COLOR_CYAN

/** @brief Enable white background font color. To disable text attribute use @ref VT100_RESET_ATTRIBUTES. */
#define VT100_BACK_COLOR_WHITE
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

/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* _LIB_VT100_H_ */
/*==============================================================================
  End of file
==============================================================================*/
