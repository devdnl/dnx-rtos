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
#include "system/ioctl_macros.h"

/*==============================================================================
  Exported object types
==============================================================================*/
/** define part count */
#define TTY_MINOR_NUMBER        0

/** devices number */
enum TTY_major_number {
#if TTY_NUMBER_OF_VT > 0
        TTY_DEV_0,
#endif
#if TTY_NUMBER_OF_VT > 1
        TTY_DEV_1,
#endif
#if TTY_NUMBER_OF_VT > 2
        TTY_DEV_2,
#endif
#if TTY_NUMBER_OF_VT > 3
        TTY_DEV_3,
#endif
#if TTY_NUMBER_OF_VT > 4
#error "TTY support 4 virtual terminals!"
#endif
        TTY_DEV_COUNT
};

/* IO requests ('T''T' - 0x5454 id) */
#define TTY_IORQ_GET_CURRENT_TTY                _IORQ(0x00, 0x5454, _DIR_RD, sizeof(int*))
#define TTY_IORQ_SWITCH_TTY_TO                  _IORQ(0x01, 0x5454, _DIR_WR, sizeof(int))
#define TTY_IORQ_CLEAN_TTY                      _IORQ(0x02, 0x5454, _DIR_NONE, 0)
#define TTY_IORQ_GET_COL                        _IORQ(0x03, 0x5454, _DIR_RD, sizeof(int*))
#define TTY_IORQ_GET_ROW                        _IORQ(0x04, 0x5454, _DIR_RD, sizeof(int*))
#define TTY_IORQ_CLEAR_SCR                      _IORQ(0x05, 0x5454, _DIR_NONE, 0)
#define TTY_IORQ_ECHO_ON                        _IORQ(0x06, 0x5454, _DIR_NONE, 0)
#define TTY_IORQ_ECHO_OFF                       _IORQ(0x07, 0x5454, _DIR_NONE, 0)

#ifdef __cplusplus
}
#endif

#endif /* _TTY_DEF_H_ */
/*==============================================================================
  End of file
==============================================================================*/
