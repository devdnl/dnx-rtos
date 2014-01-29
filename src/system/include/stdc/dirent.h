/*=========================================================================*//**
@file    dirent.h

@author  Daniel Zorychta

@brief

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

#ifndef _DIRENT_H_
#define _DIRENT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "core/vfs.h"

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
 * @brief Function open selected directory
 *
 * @param *name         directory path
 *
 * @retval NULL if file can't be created
 */
//==============================================================================
static inline DIR *opendir(const char *path)
{
        return sysm_opendir(path);
}

//==============================================================================
/**
 * @brief Function close opened directory
 *
 * @param *DIR                pinter to directory
 *
 * @retval 0                  file closed successfully
 * @retval -1                 file not closed
 */
//==============================================================================
static inline int closedir(DIR *dir)
{
        return sysm_closedir(dir);
}

//==============================================================================
/**
 * @brief Function read next item of opened directory
 *
 * @param[in] *dir                  directory object
 *
 * @return element attributes
 */
//==============================================================================
static inline dirent_t readdir(DIR *dir)
{
        return vfs_readdir(dir);
}

#ifdef __cplusplus
}
#endif

#endif /* _DIRENT_H_ */
/*==============================================================================
  End of file
==============================================================================*/
