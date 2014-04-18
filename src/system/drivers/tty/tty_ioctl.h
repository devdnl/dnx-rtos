/*=========================================================================*//**
@file    tty_ioctl.h

@author  Daniel Zorychta

@brief   This file support TTY ioctl request codes.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _TTY_IOCTL_H_
#define _TTY_IOCTL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "core/ioctl_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
#define TTY_IORQ_GET_CURRENT_TTY                _IOR('T', 0x00, int*)
#define TTY_IORQ_SWITCH_TTY_TO                  _IOW('T', 0x01, int )
#define TTY_IORQ_GET_COL                        _IOR('T', 0x02, int*)
#define TTY_IORQ_GET_ROW                        _IOR('T', 0x03, int*)
#define TTY_IORQ_CLEAR_SCR                      _IO( 'T', 0x04)
#define TTY_IORQ_ECHO_ON                        _IO( 'T', 0x05)
#define TTY_IORQ_ECHO_OFF                       _IO( 'T', 0x06)
#define TTY_IORQ_SET_EDITLINE                   _IOW('T', 0x07, const char*)
#define TTY_IORQ_GET_NUMBER_OF_TTYS             _IOR('T', 0x08, int*)

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

#endif /* _TTY_IOCTL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
