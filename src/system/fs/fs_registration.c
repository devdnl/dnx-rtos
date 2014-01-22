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
#include "core/fsctrl.h"
#include <dnx/fs.h>
#include <dnx/misc.h>

/*==============================================================================
  External objects
==============================================================================*/
_IMPORT_FILE_SYSTEM(lfs);
_IMPORT_FILE_SYSTEM(procfs);
_IMPORT_FILE_SYSTEM(fatfs);
_IMPORT_FILE_SYSTEM(devfs);

/*==============================================================================
  Exported objects
==============================================================================*/
const struct _FS_entry _FS_table[] = {
        _USE_FILE_SYSTEM_INTERFACE(lfs),
        _USE_FILE_SYSTEM_INTERFACE(procfs),
        _USE_FILE_SYSTEM_INTERFACE(fatfs),
        _USE_FILE_SYSTEM_INTERFACE(devfs),
};

const uint _FS_table_size = ARRAY_SIZE(_FS_table);

/*==============================================================================
  End of file
==============================================================================*/
