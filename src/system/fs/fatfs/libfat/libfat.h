/*=========================================================================*//**
@file    libfat.h

@author  Daniel Zorychta

@brief   FAT file system library based on ChaN's code.

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
/*-----------------------------------------------------------------------------/
/  FatFs - FAT file system module include file  R0.09b    (C)ChaN, 2013
/------------------------------------------------------------------------------/
/ FatFs module is a generic FAT file system module for small embedded systems.
/ This is a free software that opened for education, research and commercial
/ developments under license policy of following terms.
/
/  Copyright (C) 2013, ChaN, all right reserved.
/
/ * The FatFs module is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial product UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-----------------------------------------------------------------------------*/

#ifndef _LIBFAT_H_
#define _LIBFAT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "fs/fs.h"                      /* dnx file system support */
#include "libfat_conf.h"                /* FatFs configuration options */

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
#ifndef EOF
#define EOF (-1)
#endif

/* file attributes */
#define LIBFAT_FA_READ                  0x01
#define LIBFAT_FA_OPEN_EXISTING         0x00
#define LIBFAT_FA__ERROR                0x80
#define LIBFAT_FA_WRITE                 0x02
#define LIBFAT_FA_CREATE_NEW            0x04
#define LIBFAT_FA_CREATE_ALWAYS         0x08
#define LIBFAT_FA_OPEN_ALWAYS           0x10
#define LIBFAT_FA__WRITTEN              0x20
#define LIBFAT_FA__DIRTY                0x40

/* FAT sub type (FATFS.fs_type) */
#define LIBFAT_FS_FAT12                 1
#define LIBFAT_FS_FAT16                 2
#define LIBFAT_FS_FAT32                 3

/* File attribute bits for directory entry */
#define LIBFAT_AM_RDO                   0x01        /* Read only */
#define LIBFAT_AM_HID                   0x02        /* Hidden */
#define LIBFAT_AM_SYS                   0x04        /* System */
#define LIBFAT_AM_VOL                   0x08        /* Volume label */
#define LIBFAT_AM_LFN                   0x0F        /* LFN entry */
#define LIBFAT_AM_DIR                   0x10        /* Directory */
#define LIBFAT_AM_ARC                   0x20        /* Archive */
#define LIBFAT_AM_MASK                  0x3F        /* Mask of defined bits */

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
/* Type of path name strings on FatFs API */
#if _LIBFAT_LFN_UNICODE
#       if !_LIBFAT_USE_LFN
#               error _LIBFAT_LFN_UNICODE must be 0 in non-LFN cfg.
#       endif
#       ifndef _INC_TCHAR
                typedef wchar_t TCHAR;
#               define _T(x) L ## x
#               define _TEXT(x) L ## x
#       endif
#else /* ANSI/OEM string */
#       ifndef _INC_TCHAR
                typedef char TCHAR;
#               define _T(x) x
#               define _TEXT(x) x
#       endif
#endif


/* File system object structure (FATFS) */
typedef struct FATFS {
        uint8_t         fs_type;                /* FAT sub-type (0:Not mounted) */
        FILE           *srcfile;                /* file system source file */
        uint8_t         csize;                  /* Sectors per cluster (1,2,4...128) */
        uint8_t         n_fats;                 /* Number of FAT copies (1,2) */
        uint8_t         wflag;                  /* win[] dirty flag (1:must be written back) */
        uint8_t         fsi_flag;               /* fsinfo dirty flag (1:must be written back) */
        uint16_t        id;                     /* File system mount ID */
        uint16_t        n_rootdir;              /* Number of root directory entries (FAT12/16) */
#if _LIBFAT_MAX_SS != 512
        uint16_t        ssize;                  /* Bytes per sector (512, 1024, 2048 or 4096) */
#endif
        _LIBFAT_MUTEX_t sobj;                   /* Identifier of sync object */
        uint32_t        last_clust;             /* Last allocated cluster */
        uint32_t        free_clust;             /* Number of free clusters */
        uint32_t        fsi_sector;             /* fsinfo sector (FAT32) */
        uint32_t        n_fatent;               /* Number of FAT entries (= number of clusters + 2) */
        uint32_t        fsize;                  /* Sectors per FAT */
        uint32_t        volbase;                /* Volume start sector */
        uint32_t        fatbase;                /* FAT start sector */
        uint32_t        dirbase;                /* Root directory start sector (FAT32:Cluster#) */
        uint32_t        database;               /* Data start sector */
        uint32_t        winsect;                /* Current sector appearing in the win[] */
        uint8_t         win[_LIBFAT_MAX_SS];    /* Disk access window for Directory, FAT (and Data on tiny cfg) */
        /* File access control feature */
#if _LIBFAT_FS_LOCK
        struct FILESEM {
                struct FATFS   *fs;             /* File ID 1, volume (NULL:blank entry) */
                uint32_t        clu;            /* File ID 2, directory */
                uint16_t        idx;            /* File ID 3, directory index */
                uint16_t        ctr;            /* File open counter, 0:none, 0x01..0xFF:read open count, 0x100:write mode */
        } files[_LIBFAT_FS_LOCK];
#endif
} FATFS;

/* File object structure */
typedef struct {
        FATFS          *fs;                     /* Pointer to the related file system object (**do not change order**) */
        uint16_t        id;                     /* Owner file system mount ID (**do not change order**) */
        uint8_t         flag;                   /* File status flags */
        uint8_t         pad1;
        uint32_t        fptr;                   /* File read/write pointer (0ed on file open) */
        uint32_t        fsize;                  /* File size */
        uint32_t        sclust;                 /* File data start cluster (0:no data cluster, always 0 when fsize is 0) */
        uint32_t        clust;                  /* Current cluster of fpter */
        uint32_t        dsect;                  /* Current data sector of fpter */
        uint32_t        dir_sect;               /* Sector containing the directory entry */
        uint8_t        *dir_ptr;                /* Pointer to the directory entry in the window */
#if _LIBFAT_FS_LOCK
        uint            lockid;                 /* File lock ID (index of file semaphore table Files[]) */
#endif
#if !_FS_TINY
        uint8_t         buf[_LIBFAT_MAX_SS];    /* File data read/write buffer */
#endif
} FATFILE;


/* Directory object structure (DIR) */
typedef struct {
        FATFS          *fs;                     /* Pointer to the owner file system object (**do not change order**) */
        uint16_t        id;                     /* Owner file system mount ID (**do not change order**) */
        uint16_t        index;                  /* Current read/write index number */
        uint32_t        sclust;                 /* Table start cluster (0:Root dir) */
        uint32_t        clust;                  /* Current cluster */
        uint32_t        sect;                   /* Current sector */
        uint8_t        *dir;                    /* Pointer to the current SFN entry in the win[] */
        uint8_t        *fn;                     /* Pointer to the SFN (in/out) {file[8],ext[3],status[1]} */
#if _LIBFAT_USE_LFN
        wchar_t        *lfn;                    /* Pointer to the LFN working buffer */
        uint16_t        lfn_idx;                /* Last matched LFN index number (0xFFFF:No LFN) */
#endif
} FATDIR;


/* File status structure (FILINFO) */
typedef struct {
        uint32_t        fsize;                  /* File size */
        uint16_t        fdate;                  /* Last modified date */
        uint16_t        ftime;                  /* Last modified time */
        uint8_t         fattrib;                /* Attribute */
        TCHAR           fname[13];              /* Short file name (8.3 format) */
#if _LIBFAT_USE_LFN
        TCHAR          *lfname;                 /* Pointer to the LFN buffer */
        uint            lfsize;                 /* Size of LFN buffer in TCHAR */
#endif
} FILEINFO;


/* File function return code (FRESULT) */
typedef enum {
        FR_OK = 0,                              /* (0)  Succeeded */
        FR_DISK_ERR,                            /* (1)  A hard error occurred in the low level disk I/O layer */
        FR_INT_ERR,                             /* (2)  Assertion failed */
        FR_NOT_READY,                           /* (3)  The physical drive cannot work */
        FR_NO_FILE,                             /* (4)  Could not find the file */
        FR_NO_PATH,                             /* (5)  Could not find the path */
        FR_INVALID_NAME,                        /* (6)  The path name format is invalid */
        FR_DENIED,                              /* (7)  Access denied due to prohibited access or directory full */
        FR_EXIST,                               /* (8)  Access denied due to prohibited access */
        FR_INVALID_OBJECT,                      /* (9)  The file/directory object is invalid */
        FR_WRITE_PROTECTED,                     /* (10) The physical drive is write protected */
        FR_INVALID_DRIVE,                       /* (11) The logical drive number is invalid */
        FR_NOT_ENABLED,                         /* (12) The volume has no work area */
        FR_NO_FILESYSTEM,                       /* (13) There is no valid FAT volume */
        FR_MKFS_ABORTED,                        /* (14) The f_mkfs() aborted due to any parameter error */
        FR_TIMEOUT,                             /* (15) Could not get a grant to access the volume within defined period */
        FR_LOCKED,                              /* (16) The operation is rejected according to the file sharing policy */
        FR_NOT_ENOUGH_CORE,                     /* (17) LFN working buffer could not be allocated */
        FR_TOO_MANY_OPEN_FILES,                 /* (18) Number of open files > _FS_SHARE */
        FR_INVALID_PARAMETER                    /* (19) Given parameter is invalid */
} FRESULT;

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern FRESULT  libfat_mount     (FILE*, FATFS*);
extern FRESULT  libfat_umount    (FATFS*);
extern FRESULT  libfat_sync      (FATFS*);
extern FRESULT  libfat_open      (FATFS*, FATFILE*, const TCHAR*, uint8_t);
extern FRESULT  libfat_read      (FATFILE*, void*, uint, uint*);
extern FRESULT  libfat_lseek     (FATFILE*, uint32_t);
extern FRESULT  libfat_close     (FATFILE*);
extern FRESULT  libfat_opendir   (FATFS*, FATDIR*, const TCHAR*);
extern FRESULT  libfat_readdir   (FATDIR*, FILEINFO*);
extern FRESULT  libfat_stat      (FATFS*, const TCHAR*, FILEINFO*);
extern FRESULT  libfat_write     (FATFILE*, const void*, uint, uint*);
extern FRESULT  libfat_getfree   (FATFS *fs, uint32_t *nclst);
extern FRESULT  libfat_truncate  (FATFILE*);
extern FRESULT  libfat_flush     (FATFILE*);
extern FRESULT  libfat_unlink    (FATFS*, const TCHAR*);
extern FRESULT  libfat_mkdir     (FATFS*, const TCHAR*);
extern FRESULT  libfat_chmod     (FATFS*, const TCHAR*, uint8_t, uint8_t);
extern FRESULT  libfat_utime     (FATFS*, const TCHAR*, const FILEINFO*);
extern FRESULT  libfat_rename    (FATFS*, const TCHAR*, const TCHAR*);

/*==============================================================================
  Exported inline functions
==============================================================================*/
static inline int libfat_eof(FATFILE *fp)
{
        return fp->fptr == fp->fsize ? 1 : 0;
}

static inline int libfat_error(FATFILE *fp)
{
        return fp->flag & LIBFAT_FA__ERROR ? 1 : 0;
}

static inline uint32_t libfat_tell(FATFILE *fp)
{
        return fp->fptr;
}

static inline uint32_t libfat_size(FATFILE *fp)
{
        return fp->fsize;
}

#ifdef __cplusplus
}
#endif

#endif /* _LIBFAT_H_ */
/*==============================================================================
  End of file
==============================================================================*/
