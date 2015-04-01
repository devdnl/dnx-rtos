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

/*==============================================================================
  Include files
==============================================================================*/
#include "fs/fsctrl.h"
#include "fs/fs.h"
#include <dnx/misc.h>

/*==============================================================================
  External objects
==============================================================================*/
#if (__ENABLE_LFS__)
_IMPORT_FILE_SYSTEM(lfs);
#endif
#if (__ENABLE_PROCFS__)
_IMPORT_FILE_SYSTEM(procfs);
#endif
#if (__ENABLE_FATFS__)
_IMPORT_FILE_SYSTEM(fatfs);
#endif
#if (__ENABLE_DEVFS__)
_IMPORT_FILE_SYSTEM(devfs);
#endif
#if (__ENABLE_EXT2FS__)
_IMPORT_FILE_SYSTEM(ext2fs);
#endif

/*==============================================================================
  Exported objects
==============================================================================*/
const struct _FS_entry _FS_table[] = {
        #if (__ENABLE_LFS__)
        _FILE_SYSTEM_INTERFACE(lfs),
        #endif
        #if (__ENABLE_PROCFS__)
        _FILE_SYSTEM_INTERFACE(procfs),
        #endif
        #if (__ENABLE_FATFS__)
        _FILE_SYSTEM_INTERFACE(fatfs),
        #endif
        #if (__ENABLE_DEVFS__)
        _FILE_SYSTEM_INTERFACE(devfs),
        #endif
        #if (__ENABLE_EXT2FS__)
        _FILE_SYSTEM_INTERFACE(ext2fs),
        #endif
};

const uint _FS_table_size = ARRAY_SIZE(_FS_table);

/*==============================================================================
  End of file
==============================================================================*/
