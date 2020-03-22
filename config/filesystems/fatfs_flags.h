/*==============================================================================
@file    fatfs_flags.h

@author  Daniel Zorychta

@brief   FAT file system configuration.

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

#ifndef _FATFS_FLAGS_H_
#define _FATFS_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 1, "Home > File Systems > fatfs",
               function() this:LoadFile("filesystems/filesystems_flags.h") end)
++*/

/*--
this:AddWidget("Combobox")
this:AddItem("U.S. (437)",                        "437" )
this:AddItem("Arabic (720)",                      "720" )
this:AddItem("Greek (737)",                       "737" )
this:AddItem("KBL (771)",                         "771" )
this:AddItem("Baltic (775)",                      "775" )
this:AddItem("Latin 1 (850)",                     "850" )
this:AddItem("Latin 2 (852)",                     "852" )
this:AddItem("Cyrillic (855)",                    "855" )
this:AddItem("Turkish (857)",                     "857" )
this:AddItem("Portuguese (860)",                  "860" )
this:AddItem("Icelandic (861)",                   "861" )
this:AddItem("Hebrew (862)",                      "862" )
this:AddItem("Canadian French (863)",             "863" )
this:AddItem("Arabic (864)",                      "864" )
this:AddItem("Nordic (865)",                      "865" )
this:AddItem("Russian (866)",                     "866" )
this:AddItem("Greek 2 (869)",                     "869" )
this:AddItem("Japanese (DBCS) (932)",             "932" )
this:AddItem("Simplified Chinese (DBCS) (936)",   "936" )
this:AddItem("Korean (DBCS) (949)",               "949" )
this:AddItem("Traditional Chinese (DBCS) (950)",  "950" )
--*/
#define __FATFS_LFN_CODEPAGE__ 852

/*--
this:AddWidget("Checkbox", "Buffered files (fast)")
this:SetToolTip("When option is disabled, file system uses the sector buffer in the file system "..
                "object instead of the sector buffer in the individual file object for file "..
                "data transfer. This reduces memory consumption 512 bytes each file object.")
--*/
#define __FATFS_BUFFERED_FILE_ENABLE__ _YES_

#endif /* _FATFS_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
