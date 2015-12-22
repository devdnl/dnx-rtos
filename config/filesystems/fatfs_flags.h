/*=========================================================================*//**
@file    fatfs_flags.h

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

#ifndef _FATFS_FLAGS_H_
#define _FATFS_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > File Systems > fatfs",
               function() this:LoadFile("filesystems/filesystems_flags.h") end)
++*/

/*--
this:AddWidget("Checkbox", "Enable Long File Names")
--*/
#define __FATFS_LFN_ENABLE__ _YES_

/*--
this:AddWidget("Combobox")
this:AddItem("U.S. (437)",                        "437" )
this:AddItem("Arabic (720)",                      "720" )
this:AddItem("Greek (737)",                       "737" )
this:AddItem("Baltic (775)",                      "775" )
this:AddItem("Multilingual Latin 1 (850)",        "850" )
this:AddItem("Latin 2 (852)",                     "852" )
this:AddItem("Cyrillic (855)",                    "855" )
this:AddItem("Turkish (857)",                     "857" )
this:AddItem("Multilingual Latin 1 + Euro (858)", "858" )
this:AddItem("Hebrew (862)",                      "862" )
this:AddItem("Russian (866)",                     "866" )
this:AddItem("Thai (874)",                        "874" )
this:AddItem("Japanese Shift-JIS (932)",          "932" )
this:AddItem("Simplified Chinese GBK (936)",      "936" )
this:AddItem("Korean (949)",                      "949" )
this:AddItem("Traditional Chinese Big5 (950)",    "950" )
this:AddItem("Central Europe (1250)",             "1250")
this:AddItem("Cyrillic (1251)",                   "1251")
this:AddItem("Latin 1 (1252)",                    "1252")
this:AddItem("Greek (1253)",                      "1253")
this:AddItem("Turkish (1254)",                    "1254")
this:AddItem("Hebrew (1255)",                     "1255")
this:AddItem("Arabic (1256)",                     "1256")
this:AddItem("Baltic (1257)",                     "1257")
this:AddItem("Vietnam (1258)",                    "1258")
--*/
#define __FATFS_LFN_CODEPAGE__ 852

#endif /* _FATFS_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
