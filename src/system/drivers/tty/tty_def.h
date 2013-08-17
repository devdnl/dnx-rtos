#ifndef _TTY_DEF_H_
#define _TTY_DEF_H_
/*=========================================================================*//**
@file    tty_def.h

@author  Daniel Zorychta

@brief   This file support global definitions of TTY

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
#include "tty_cfg.h"

/*==============================================================================
  Exported object types
==============================================================================*/
/** define part count */
#define TTY_MINOR_NO                    0

/** TTY requests */
enum TTY_IORQ_enum {
        TTY_IORQ_GET_CURRENT_TTY,               /* [out] int*   */
        TTY_IORQ_SWITCH_TTY_TO,                 /* [in ] int    */
        TTY_IORQ_CLEAN_TTY,                     /* none         */
        TTY_IORQ_GET_COL,                       /* [out] int*   */
        TTY_IORQ_GET_ROW,                       /* [out] int*   */
        TTY_IORQ_CLEAR_SCR,                     /* none         */
        TTY_IORQ_ECHO_ON,                       /* none         */
        TTY_IORQ_ECHO_OFF,                      /* none         */
};

#ifdef __cplusplus
}
#endif

#endif /* _TTY_DEF_H_ */
/*==============================================================================
  End of file
==============================================================================*/
