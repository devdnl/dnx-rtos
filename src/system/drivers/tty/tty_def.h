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

#ifndef _TTY_DEF_H_
#define _TTY_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "tty_cfg.h"
#include "tty_ioctl.h"

/*==============================================================================
  Exported object types
==============================================================================*/
/** define part count */
#define _TTY_MINOR_NUMBER       0

/** devices number */
enum {
        #if _TTY_NUMBER_OF_VT > 0
        _TTY0,
        #endif
        #if _TTY_NUMBER_OF_VT > 1
        _TTY1,
        #endif
        #if _TTY_NUMBER_OF_VT > 2
        _TTY2,
        #endif
        #if _TTY_NUMBER_OF_VT > 3
        _TTY3,
        #endif
        #if _TTY_NUMBER_OF_VT > 4
        #error TTY support 4 virtual terminals!
        #endif
        _TTY_NUMBER
};

#ifdef __cplusplus
}
#endif

#endif /* _TTY_DEF_H_ */
/*==============================================================================
  End of file
==============================================================================*/
