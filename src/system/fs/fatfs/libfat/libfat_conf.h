#ifndef _LIBFAT_CONF_H_
#define _LIBFAT_CONF_H_
/*=========================================================================*//**
@file    libfat_conf.c

@author  Daniel Zorychta

@brief   FAT file system library based od ChaN's code.

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

/*------------------------------------------------------------------------------
 * Functions and Buffer Configurations
 * ---------------------------------------------------------------------------*/

/* When _LIBFAT_FS_TINY is set to 1, FatFs uses the sector buffer in the file system
 * object instead of the sector buffer in the individual file object for file
 * data transfer. This reduces memory consumption 512 bytes each file object.
 */
#define _LIBFAT_FS_TINY         0       /* 0:Normal or 1:Tiny */


/*------------------------------------------------------------------------------
 * Locale and Namespace Configurations
 *----------------------------------------------------------------------------*/

/* The _CODE_PAGE specifies the OEM code page to be used on the target system.
 * Incorrect setting of the code page can cause a file open failure.
 *
 * 932  - Japanese Shift-JIS (DBCS, OEM, Windows)
 * 936  - Simplified Chinese GBK (DBCS, OEM, Windows)
 * 949  - Korean (DBCS, OEM, Windows)
 * 950  - Traditional Chinese Big5 (DBCS, OEM, Windows)
 * 1250 - Central Europe (Windows)
 * 1251 - Cyrillic (Windows)
 * 1252 - Latin 1 (Windows)
 * 1253 - Greek (Windows)
 * 1254 - Turkish (Windows)
 * 1255 - Hebrew (Windows)
 * 1256 - Arabic (Windows)
 * 1257 - Baltic (Windows)
 * 1258 - Vietnam (OEM, Windows)
 * 437  - U.S. (OEM)
 * 720  - Arabic (OEM)
 * 737  - Greek (OEM)
 * 775  - Baltic (OEM)
 * 850  - Multilingual Latin 1 (OEM)
 * 858  - Multilingual Latin 1 + Euro (OEM)
 * 852  - Latin 2 (OEM)
 * 855  - Cyrillic (OEM)
 * 866  - Russian (OEM)
 * 857  - Turkish (OEM)
 * 862  - Hebrew (OEM)
 * 874  - Thai (OEM, Windows)
 * 1    - ASCII only (Valid for non LFN cfg.)
 */
#define _LIBFAT_CODE_PAGE        852


/* The _USE_LFN option switches the LFN support.
 *
 * 0: Disable LFN feature. _LIBFAT_MAX_LFN and _LIBFAT_LFN_UNICODE have no effect.
 * 1: Enable LFN with static working buffer on the BSS. Always NOT reentrant.
 * 2: Enable LFN with dynamic working buffer on the STACK.
 * 3: Enable LFN with dynamic working buffer on the HEAP.
 *
 * The LFN working buffer occupies (_LIBFAT_MAX_LFN + 1) * 2 bytes. To enable LFN,
 * Unicode handling functions ff_convert() and ff_wtoupper() must be added
 * to the project. When enable to use heap, memory control functions
 * ff_memalloc() and ff_memfree() must be added to the project. */
#define _LIBFAT_USE_LFN         3       /* 0 to 3 */
#define _LIBFAT_MAX_LFN         255     /* Maximum LFN length to handle (12 to 255) */


/* To switch the character code set on FatFs API to Unicode,
 * enable LFN feature and set _LIBFAT_LFN_UNICODE to 1. */
#define _LIBFAT_LFN_UNICODE     0       /* 0:ANSI/OEM or 1:Unicode */

/*------------------------------------------------------------------------------
 * Physical Drive Configurations
 * ----------------------------------------------------------------------------*/

/* Maximum sector size to be handled.
 * Always set 512 for memory card and hard disk but a larger value may be
 * required for on-board flash memory, floppy disk and optical disk.
 * When _LIBFAT_MAX_SS is larger than 512, it configures FatFs to variable sector size
 * and GET_SECTOR_SIZE command must be implemented to the disk_ioctl function.
 */
#define _LIBFAT_MAX_SS          512     /* 512, 1024, 2048 or 4096 */


/* To enable sector erase feature, set _USE_ERASE to 1. CTRL_ERASE_SECTOR command
 * should be added to the disk_ioctl function.
 */
#define _LIBFAT_USE_ERASE       0       /* 0:Disable or 1:Enable */


/*---------------------------------------------------------------------------/
/ System Configurations
/----------------------------------------------------------------------------*/

/* A header file that defines sync object types on the O/S, such as
 * windows.h, ucos_ii.h and semphr.h, must be included prior to libfat.h.
 */
#define _LIBFAT_FS_TIMEOUT      15000   /* Timeout period in unit of time ticks */
#define _LIBFAT_SYNC_t          mutex_t*/* O/S dependent type of sync object. e.g. HANDLE, OS_EVENT*, ID and etc.. */


/* To enable file lock control feature, set _LIBFAT_FS_LOCK to 1 or greater.
 * The value defines how many files can be opened simultaneously.
 */
#define _LIBFAT_FS_LOCK         0       /* 0:Disable or >=1:Enable */

#endif /* _LIBFAT_CONF_H_ */
