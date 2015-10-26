/*=========================================================================*//**
@file    endian.h

@author  Daniel Zorychta

@brief

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _ENDIAN_H_
#define _ENDIAN_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <portable/cpuctl.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
#define LITTLE_ENDIAN           _BYTE_ORDER_LITTLE
#define BIG_ENDIAN              _BYTE_ORDER_BIG
#define BYTE_ORDER              _CPUCTL_BYTE_ORDER

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

#endif /* _ENDIAN_H_ */
/*==============================================================================
  End of file
==============================================================================*/
