/*=========================================================================*//**
@file    ext2fs_flags.h

@author  Daniel Zorychta

@brief   This file include driver configurations for this architecture

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*
 * NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as suffix
 *       and prefix) are exported to the single configuration file
 *       (by using Configtool) when entire project configuration is exported.
 *       All other flag definitions and statements are ignored.
 */

#ifndef _EXT2FS_FLAGS_H_
#define _EXT2FS_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > File Systems > ext2fs",
               function() this:LoadFile("filesystems/filesystems_flags.h") end)
++*/

/*--
this:AddWidget("Spinbox", 4, 1024, "Number of blocks in cache (block has few sectors)")
--*/
#define __EXT2FS_CACHE_SIZE__ 4

#endif /* _EXT2FS_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
