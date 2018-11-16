/*=========================================================================*//**
@file    tty.h

@author  Daniel Zorychta

@brief   TTY driver header file. Internal usage only.

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

#ifndef _TTY_H_
#define _TTY_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct ttybfr  ttybfr_t;
typedef struct ttyedit ttyedit_t;
typedef struct ttycmd  ttycmd_t;

typedef enum ttycmd_resp {
        TTYCMD_BUSY,
        TTYCMD_KEY_ENTER,
        TTYCMD_KEY_ENDTEXT,
        TTYCMD_KEY_BACKSPACE,
        TTYCMD_KEY_TAB,
        TTYCMD_KEY_DELETE,
        TTYCMD_KEY_ARROW_LEFT,
        TTYCMD_KEY_ARROW_RIGHT,
        TTYCMD_KEY_ARROW_UP,
        TTYCMD_KEY_ARROW_DOWN,
        TTYCMD_KEY_HOME,
        TTYCMD_KEY_END,
        TTYCMD_KEY_F1,
        TTYCMD_KEY_F2,
        TTYCMD_KEY_F3,
        TTYCMD_KEY_F4,
        TTYCMD_KEY_F5,
        TTYCMD_KEY_F6,
        TTYCMD_KEY_F7,
        TTYCMD_KEY_F8,
        TTYCMD_KEY_F9,
        TTYCMD_KEY_F10,
        TTYCMD_KEY_F11,
        TTYCMD_KEY_F12,
        TTYCMD_KEY_CHAR,
        TTYCMD_NUMBER_OF_CMDS
} ttycmd_resp_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
/* buffer support ----------------------------------------------------------- */
extern int              ttybfr_create                   (ttybfr_t**);
extern int              ttybfr_destroy                  (ttybfr_t*);
extern void             ttybfr_put                      (ttybfr_t*, const char*, size_t);
extern void             ttybfr_flush                    (ttybfr_t*);
extern void             ttybfr_clear                    (ttybfr_t*);
extern const char      *ttybfr_get_line                 (ttybfr_t*, int);
extern const char      *ttybfr_get_fresh_line           (ttybfr_t*);
extern void             ttybfr_clear_fresh_line_counter (ttybfr_t*);

/* editline support --------------------------------------------------------- */
extern int              ttyedit_create                  (FILE*, ttyedit_t**);
extern int              ttyedit_destroy                 (ttyedit_t*);
extern void             ttyedit_enable_echo             (ttyedit_t*);
extern void             ttyedit_disable_echo            (ttyedit_t*);
extern bool             ttyedit_is_echo_enabled         (ttyedit_t*);
extern char            *ttyedit_get_value               (ttyedit_t*);
extern void             ttyedit_set_value               (ttyedit_t*, const char*, bool);
extern void             ttyedit_clear                   (ttyedit_t*);
extern void             ttyedit_insert_char             (ttyedit_t*, const char);
extern void             ttyedit_remove_char             (ttyedit_t*);
extern void             ttyedit_delete_char             (ttyedit_t*);
extern void             ttyedit_move_cursor_left        (ttyedit_t*);
extern void             ttyedit_move_cursor_right       (ttyedit_t*);
extern void             ttyedit_move_cursor_home        (ttyedit_t*);
extern void             ttyedit_move_cursor_end         (ttyedit_t*);

/* vt100 command analyze ---------------------------------------------------- */
extern int              ttycmd_create                   (ttycmd_t**);
extern int              ttycmd_destroy                  (ttycmd_t*);
extern ttycmd_resp_t    ttycmd_analyze                  (ttycmd_t*, const char);
extern bool             ttycmd_is_idle                  (ttycmd_t*);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _TTY_H_ */
/*==============================================================================
  End of file
==============================================================================*/
