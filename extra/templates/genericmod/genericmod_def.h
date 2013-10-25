/*=========================================================================*//**
@file    genericmod_def.h

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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "genericmod_cfg.h"
#include "system/ioctl_macros.h"

/*==============================================================================
  Exported macros
==============================================================================*/
#define GENERICMOD_IORQ_EX1         _IO('M', 0x00)
#define GENERICMOD_IORQ_EX2         _IOR('M', 0x01, int*)
#define GENERICMOD_IORQ_EX3         _IOW('M', 0x02, int)
#define GENERICMOD_IORQ_EX4         _IOWR('M', 0x03, int*)

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

#endif /* _GENERICMOD_DEF_H_ */
/*==============================================================================
  End of file
==============================================================================*/
