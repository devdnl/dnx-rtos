#ifndef _IOCTL_MACROS_H_
#define _IOCTL_MACROS_H_
/*=========================================================================*//**
@file    ioctl_macros.h

@author  Daniel Zorychta

@brief   ioctl build macros.

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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/

/*==============================================================================
  Exported macros
==============================================================================*/
#define _DIR_NONE                       0x0
#define _DIR_RD                         0x1
#define _DIR_WR                         0x2
#define _DIR_RW                         0x3
#define _IORQ(nr, id, dir, size)        (((u32_t)((u32_t)(id) & 0xFFFF) << 16) | ((u32_t)(((size) / 4) & 0x3F) << 10) | ((u32_t)((dir) & 0x03) << 8) | ((u32_t)((nr) & 0xFF) << 0))

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

#endif /* _IOCTL_MACROS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
