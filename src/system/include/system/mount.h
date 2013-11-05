/*=========================================================================*//**
@file    mount.h

@author  Daniel Zorychta

@brief   Library with mount file system tools

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

#ifndef _MOUNT_H_
#define _MOUNT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "core/fsctrl.h"

/*==============================================================================
  Exported macros
==============================================================================*/

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
//==============================================================================
/**
 * @brief Function mount file system
 *
 * @param FS_name        file system name
 * @param src_path       path to file with source data
 * @param mount_point    mount point of file system
 *
 * @return 0 if success, otherwise 1
 */
//==============================================================================
static inline int mount(const char *FS_name, const char *src_path, const char *mount_point)
{
        return _mount(FS_name, src_path, mount_point);
}

//==============================================================================
/**
 * @brief Function unmount file system
 *
 * @param *mount_point   path to file system
 *
 * @return 0 if success, otherwise 1
 */
//==============================================================================
static inline int umount(const char *mount_point)
{
        return _umount(mount_point);
}

#ifdef __cplusplus
}
#endif

#endif /* _MOUNT_H_ */
/*==============================================================================
  End of file
==============================================================================*/
