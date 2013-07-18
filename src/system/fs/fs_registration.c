/*=========================================================================*//**
@file    fs_registration.c

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
#include "fs/fs_registration.h"
#include "core/vfs.h"

/* include here FS headers */
#include "fs/lfs.h"
#include "fs/appfs.h"
#include "fs/procfs.h"
#include "fs/fatfs.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define USE_FILE_SYSTEM_INTERFACE(fs_name)\
{.FS_name = #fs_name,\
 .FS_if   = {.fs_init    = fs_name##_init,\
             .fs_chmod   = fs_name##_chmod,\
             .fs_chown   = fs_name##_chown,\
             .fs_close   = fs_name##_close,\
             .fs_ioctl   = fs_name##_ioctl,\
             .fs_mkdir   = fs_name##_mkdir,\
             .fs_mknod   = fs_name##_mknod,\
             .fs_open    = fs_name##_open,\
             .fs_opendir = fs_name##_opendir,\
             .fs_read    = fs_name##_read,\
             .fs_release = fs_name##_release,\
             .fs_remove  = fs_name##_remove,\
             .fs_rename  = fs_name##_rename,\
             .fs_stat    = fs_name##_stat,\
             .fs_fstat   = fs_name##_fstat,\
             .fs_statfs  = fs_name##_statfs,\
             .fs_flush   = fs_name##_flush,\
             .fs_write   = fs_name##_write}}

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/

/*==============================================================================
  Exported object definitions
==============================================================================*/
/* driver registration */
const struct _FS_entry _FS_table[] =
{
        USE_FILE_SYSTEM_INTERFACE(lfs),
        USE_FILE_SYSTEM_INTERFACE(appfs),
        USE_FILE_SYSTEM_INTERFACE(procfs),
        USE_FILE_SYSTEM_INTERFACE(fatfs),
};

/* driver list size */
const uint _FS_table_size = ARRAY_SIZE(_FS_table);

/*==============================================================================
  Function definitions
==============================================================================*/

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
