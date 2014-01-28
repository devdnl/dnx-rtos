/*=========================================================================*//**
@file    stat.h

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

#ifndef _STAT_H_
#define _STAT_H_

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
static inline int mknod(const char *path, dev_t dev)
{
        return vfs_mknod(path, dev);
}

static inline int mkdir(const char *path, mode_t mode)
{
        return vfs_mkdir(path, mode);
}

static inline int mkfifo(const char *path, mode_t mode)
{
        return vfs_mkfifo(path, mode);
}

static inline int chmod(const char *path, int mode)
{
        return vfs_chmod(path, mode);
}

static inline int stat(const char *path, struct stat *stat)
{
        return vfs_stat(path, stat);
}

static inline int fstat(FILE *file, struct stat *stat)
{
        return vfs_fstat(file, stat);
}

#ifdef __cplusplus
}
#endif

#endif /* _STAT_H_ */
/*==============================================================================
  End of file
==============================================================================*/
