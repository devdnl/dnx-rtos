/*=========================================================================*//**
@file    tty.h

@author  Daniel Zorychta

@brief   TTY driver header file. Internal usage only.

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
        TTYCMD_KEY_BACKSPACE,
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
        TTYCMD_SIZE_CAPTURED,
        TTYCMD_NUMBER_OF_CMDS
} ttycmd_resp_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
/* buffer support ----------------------------------------------------------- */
extern ttybfr_t        *ttybfr_new                      ();
extern void             ttybfr_delete                   (ttybfr_t*);
extern void             ttybfr_add_line                 (ttybfr_t*, const char*, size_t);
extern void             ttybfr_clear                    (ttybfr_t*);
extern const char      *ttybfr_get_line                 (ttybfr_t*, int);
extern const char      *ttybfr_get_fresh_line           (ttybfr_t*);
extern void             ttybfr_clear_fresh_line_counter (ttybfr_t*);

/* editline support --------------------------------------------------------- */
extern ttyedit_t       *ttyedit_new                     (FILE*);
extern void             ttyedit_delete                  (ttyedit_t*);
extern void             ttyedit_echo_enable             (ttyedit_t*);
extern void             ttyedit_echo_disable            (ttyedit_t*);
extern char            *ttyedit_get                     (ttyedit_t*);
extern void             ttyedit_set                     (ttyedit_t*, const char*, bool);
extern void             ttyedit_clear                   (ttyedit_t*);
extern void             ttyedit_insert_char             (ttyedit_t*, const char);
extern void             ttyedit_remove_char             (ttyedit_t*);
extern void             ttyedit_delete_char             (ttyedit_t*);
extern void             ttyedit_move_cursor_left        (ttyedit_t*);
extern void             ttyedit_move_cursor_right       (ttyedit_t*);
extern void             ttyedit_move_cursor_home        (ttyedit_t*);
extern void             ttyedit_move_cursor_end         (ttyedit_t*);

/* vt100 command analyze ---------------------------------------------------- */
extern ttycmd_t        *ttycmd_new                      ();
extern void             ttycmd_delete                   (ttycmd_t*);
extern ttycmd_resp_t    ttycmd_analyze                  (ttycmd_t*, const char);
extern bool             ttycmd_is_idle                  (ttycmd_t*);
extern void             ttycmd_get_size                 (ttycmd_t*, u16_t*, u16_t*);

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
