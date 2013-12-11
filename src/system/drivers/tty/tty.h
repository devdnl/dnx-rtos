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
typedef struct tty_buffer   tty_buffer_t;
typedef struct tty_editline tty_editline_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
/* buffer support ----------------------------------------------------------- */
extern tty_buffer_t    *ttybfr_new              ();
extern void             ttybfr_delete           (tty_buffer_t*);
extern void             ttybfr_add_line         (tty_buffer_t*, const char*, size_t);

/* editline support --------------------------------------------------------- */
extern tty_editline_t  *ttyedit_new             (FILE*);
extern void             ttyedit_delete          (tty_editline_t*);
extern void             ttyedit_echo_enable     (tty_editline_t*);
extern void             ttyedit_echo_disable    (tty_editline_t*);

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
