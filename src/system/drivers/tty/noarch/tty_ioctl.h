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
/**
 *  @brief  Gets current TTY number
 *  @param  int *
 *  @return On success 0 is returned, otherwise -1
 */
#define IOCTL_TTY__GET_CURRENT_TTY              _IOR(TTY, 0x00, int*)

/**
 *  @brief  Swtich terminal to selected one
 *  @param  int
 *  @return On success 0 is returned, otherwise -1
 */
#define IOCTL_TTY__SWITCH_TTY_TO                _IOW(TTY, 0x01, int )

/**
 *  @brief  Gets number of columns
 *  @param  int *
 *  @return On success 0 is returned, otherwise -1
 */
#define IOCTL_TTY__GET_COL                      _IOR(TTY, 0x02, int*)

/**
 *  @brief  Gets number of rows
 *  @param  int *
 *  @return On success 0 is returned, otherwise -1
 */
#define IOCTL_TTY__GET_ROW                      _IOR(TTY, 0x03, int*)

/**
 *  @brief  Clear screen
 *  @param  None
 *  @return On success 0 is returned, otherwise -1
 */
#define IOCTL_TTY__CLEAR_SCR                    _IO(TTY, 0x04)

/**
 *  @brief  Enable terminal echo
 *  @param  None
 *  @return On success 0 is returned, otherwise -1
 */
#define IOCTL_TTY__ECHO_ON                      _IO(TTY, 0x05)

/**
 *  @brief  Disable terminal echo
 *  @param  None
 *  @return On success 0 is returned, otherwise -1
 */
#define IOCTL_TTY__ECHO_OFF                     _IO(TTY, 0x06)

/**
 *  @brief  Set edit line to specified user's text (string)
 *  @param  const char *
 *  @return On success 0 is returned, otherwise -1
 */
#define IOCTL_TTY__SET_EDITLINE                 _IOW(TTY, 0x07, const char*)


/**
 *  @brief  Gets number of virtual terminals
 *  @param  int *
 *  @return On success 0 is returned, otherwise -1
 */
#define IOCTL_TTY__GET_NUMBER_OF_TTYS           _IOR(TTY, 0x08, int*)

/**
 *  @brief  Refreshes last line
 *  @param  None
 *  @return On success 0 is returned, otherwise -1
 */
#define IOCTL_TTY__REFRESH_LAST_LINE            _IO(TTY, 0x09)

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
