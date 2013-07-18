#ifndef _FS_REGISTRATION_
#define _FS_REGISTRATION_
/*=========================================================================*//**
@file    fs_registration.h

@author  Daniel Zorychta

@brief   This file is used to registration file systems

@note    Copyright (C) 2012, 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "core/systypes.h"
#include "core/vfs.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
struct _FS_entry {
      const char                    *FS_name;
      const struct vfs_FS_interface  FS_if;
};

/*==============================================================================
  Exported object declarations
==============================================================================*/
extern const struct _FS_entry _FS_table[];
extern const uint             _FS_table_size;

/*==============================================================================
  Exported function prototypes
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _FS_REGISTRATION_ */
/*==============================================================================
  End of file
==============================================================================*/
