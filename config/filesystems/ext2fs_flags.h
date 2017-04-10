/*=========================================================================*//**
@file    ext2fs_flags.h

@author  Daniel Zorychta

@brief   This file include driver configurations for this architecture

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes dnx RTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


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
