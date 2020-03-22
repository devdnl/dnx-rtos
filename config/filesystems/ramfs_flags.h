/*==============================================================================
@file    ramfs_flags.h

@author  Daniel Zorychta

@brief   RAM file system configuration.

@note    Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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


==============================================================================*/

/*
 * NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as suffix
 *       and prefix) are exported to the single configuration file
 *       (by using Configtool) when entire project configuration is exported.
 *       All other flag definitions and statements are ignored.
 */

#ifndef _RAMFS_FLAGS_H_
#define _RAMFS_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > File Systems > ramfs",
               function() this:LoadFile("filesystems/filesystems_flags.h") end)
++*/

/*--
this:AddWidget("Spinbox", 8, 4096, "File chain size (bytes)")
--*/
#define __RAMFS_FILE_CHAIN_SIZE__ 32

#endif /* _RAMFS_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
