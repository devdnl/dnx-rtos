#ifndef _LIBFAT_USER_H_
#define _LIBFAT_USER_H_
/*=========================================================================*//**
@file    libfat_user.h

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
/*-----------------------------------------------------------------------
/  Low level disk interface modlue include file   (C)ChaN, 2013
/-----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

#include "libfat_conf.h"
#include "system/dnxfs.h"

/* Status of Disk Functions */
typedef uint8_t DSTATUS;

/* Results of Disk Functions */
typedef enum {
        RES_OK = 0,                     /* 0: Successful */
        RES_ERROR,                      /* 1: R/W Error */
        RES_WRPRT,                      /* 2: Write Protected */
        RES_NOTRDY,                     /* 3: Not Ready */
        RES_PARERR                      /* 4: Invalid Parameter */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */


extern DSTATUS  _libfat_disk_initialize (FILE*);
extern DSTATUS  _libfat_disk_status     (FILE*);
extern DRESULT  _libfat_disk_read       (FILE*, uint8_t*, uint32_t, uint8_t);
extern DRESULT  _libfat_disk_write      (FILE*, const uint8_t*, uint32_t, uint8_t);
extern DRESULT  _libfat_disk_ioctl      (FILE*, uint8_t, void*);
extern int      _libfat_create_mutex    (_LIBFAT_MUTEX_t*);              /* Create a sync object */
extern int      _libfat_lock_access    (_LIBFAT_MUTEX_t);               /* Lock sync object */
extern void     _libfat_unlock_access    (_LIBFAT_MUTEX_t);               /* Unlock sync object */
extern int      _libfat_delete_mutex  (_LIBFAT_MUTEX_t);               /* Delete a sync object */
extern uint32_t _libfat_get_fattime     (void);


#if _LIBFAT_USE_LFN == 2                               /* Memory functions */
extern void *_libfat_malloc(uint msize);                 /* Allocate memory block */
extern void  _libfat_free(void *mblock);                 /* Free memory block */
#endif

/* Disk Status Bits (DSTATUS) */
#define STA_NOINIT              0x01    /* Drive not initialized */
#define STA_NODISK              0x02    /* No medium in the drive */
#define STA_PROTECT             0x04    /* Write protected */


/* Command code for disk_ioctrl function */
/* Generic command (used by FatFs) */
#define CTRL_SYNC               0        /* Flush disk cache (for write functions) */
#define GET_SECTOR_COUNT        1        /* Get media size (for only f_mkfs()) */
#define GET_SECTOR_SIZE         2        /* Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
#define GET_BLOCK_SIZE          3        /* Get erase block size (for only f_mkfs()) */
#define CTRL_ERASE_SECTOR       4        /* Force erased a block of sectors (for only _USE_ERASE) */

/* Generic command (not used by FatFs) */
#define CTRL_POWER              5        /* Get/Set power status */
#define CTRL_LOCK               6        /* Lock/Unlock media removal */
#define CTRL_EJECT              7        /* Eject media */
#define CTRL_FORMAT             8        /* Create physical format on the media */

/* MMC/SDC specific ioctl command */
#define MMC_GET_TYPE            10        /* Get card type */
#define MMC_GET_CSD             11        /* Get CSD */
#define MMC_GET_CID             12        /* Get CID */
#define MMC_GET_OCR             13        /* Get OCR */
#define MMC_GET_SDSTAT          14        /* Get SD status */

/* ATA/CF specific ioctl command */
#define ATA_GET_REV             20        /* Get F/W revision */
#define ATA_GET_MODEL           21        /* Get model name */
#define ATA_GET_SN              22        /* Get serial number */

/* MMC card type flags (MMC_GET_TYPE) */
#define CT_MMC                  0x01                /* MMC ver 3 */
#define CT_SD1                  0x02                /* SD ver 1 */
#define CT_SD2                  0x04                /* SD ver 2 */
#define CT_SDC                  (CT_SD1|CT_SD2)     /* SD */
#define CT_BLOCK                0x08                /* Block addressing */


#ifdef __cplusplus
}
#endif

#endif

/*==============================================================================
  End of file
==============================================================================*/
