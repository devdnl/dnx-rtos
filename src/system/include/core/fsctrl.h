#ifndef _FSCTRL_
#define _FSCTRL_
/*=========================================================================*//**
@file    fsctrl.h

@author  Daniel Zorychta

@brief   Drivers support.

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
#include "core/vfs.h"

/*==============================================================================
  Exported macros
==============================================================================*/
#define _USE_FILE_SYSTEM_INTERFACE(fs_name)\
{.FS_name = #fs_name,\
 .FS_if   = {.fs_init    = _##fs_name##_init,\
             .fs_chmod   = _##fs_name##_chmod,\
             .fs_chown   = _##fs_name##_chown,\
             .fs_close   = _##fs_name##_close,\
             .fs_ioctl   = _##fs_name##_ioctl,\
             .fs_mkdir   = _##fs_name##_mkdir,\
             .fs_mknod   = _##fs_name##_mknod,\
             .fs_open    = _##fs_name##_open,\
             .fs_opendir = _##fs_name##_opendir,\
             .fs_read    = _##fs_name##_read,\
             .fs_release = _##fs_name##_release,\
             .fs_remove  = _##fs_name##_remove,\
             .fs_rename  = _##fs_name##_rename,\
             .fs_stat    = _##fs_name##_stat,\
             .fs_fstat   = _##fs_name##_fstat,\
             .fs_statfs  = _##fs_name##_statfs,\
             .fs_flush   = _##fs_name##_flush,\
             .fs_write   = _##fs_name##_write}}

/*==============================================================================
  Exported object types
==============================================================================*/
struct _FS_entry {
      const char                    *FS_name;
      const struct vfs_FS_interface  FS_if;
};

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern stdret_t mount   (const char*, const char*, const char*);
extern stdret_t umount  (const char*);

#ifdef __cplusplus
}
#endif

#endif /* _FSCTRL_ */
/*==============================================================================
  End of file
==============================================================================*/
