/*==============================================================================
@file    ext4fs_flags.h

@author  Daniel Zorychta

@brief   EXT4 file system

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
 * NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as prefix
 *       and suffix) are exported to the single configuration file
 *       (by using Configtool) when entire project configuration is exported.
 *       All other flag definitions and statements are ignored.
 */

#ifndef _EXT4FS_FLAGS_H_
#define _EXT4FS_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > File Systems > ext4fs",
               function() this:LoadFile("filesystems/filesystems_flags.h") end)
++*/

/*--
this:AddWidget("Combobox", "EXT feature set")
this:AddItem("EXT2", "2")
this:AddItem("EXT3", "3")
this:AddItem("EXT4", "4")
--*/
#define __EXT4FS_CFG_FEATURE__ 4

/*--
this:AddWidget("Combobox", "Journaling")
this:AddItem("Disable", "0")
this:AddItem("Enable", "1")
--*/
#define __EXT4FS_CFG_JOURNALING__ 1

/*--
this:AddWidget("Combobox", "Directory indexing")
this:AddItem("Disable", "0")
this:AddItem("Enable", "1")
--*/
#define __EXT4FS_CFG_DIR_INDEXING__ 1

/*--
this:AddWidget("Spinbox", 1, 128, "Number of cache blocks")
this:SetToolTip("This value determines how many blocks is cached. The 4 blocks "..
                "is good enough to get nice read/write speed. Single cache "..
                "allocate the size of block (at least 1 KiB).")
--*/
#define __EXT4FS_CFG_BLK_CACHE_SIZE__ 1

#endif /* _EXT4FS_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
