/*=========================================================================*//**
@file    genericmod_ioctl.h

@author  Author

@brief   This driver support generic device definitions (e.g. used in ioctl()).

@note    Copyright (C) year  Author <email>

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

#ifndef _GENERICMOD_DEF_H_
#define _GENERICMOD_DEF_H_

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
#define IOCTL_GENERICMOD__EX1           _IO(_IO_GROUP_GENERIC, 0x00)
#define IOCTL_GENERICMOD__EX2           _IOR(_IO_GROUP_GENERIC, 0x01, int*)
#define IOCTL_GENERICMOD__EX3           _IOW(_IO_GROUP_GENERIC, 0x02, int)
#define IOCTL_GENERICMOD__EX4           _IOWR(_IO_GROUP_GENERIC, 0x03, int*)

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

#endif /* _GENERICMOD_IOCTL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
