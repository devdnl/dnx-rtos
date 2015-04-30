/*=========================================================================*//**
@file    types.h

@author  Daniel Zorychta

@brief

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _TYPES_H_
#define _TYPES_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
/** short name types */
typedef uint8_t         u8_t;
typedef int8_t          i8_t;
typedef uint16_t        u16_t;
typedef int16_t         i16_t;
typedef uint32_t        u32_t;
typedef int32_t         i32_t;
typedef uint64_t        u64_t;
typedef int64_t         i64_t;
typedef unsigned int    uint;
typedef unsigned long   ulong;

/* special types */
typedef uint64_t        fpos_t;
typedef int             ssize_t;

/** file descriptor */
typedef uint fd_t;

/** file mode */
typedef int mode_t;

/** user ID */
typedef int uid_t;

/** group ID */
typedef int gid_t;

/** process ID */
typedef u32_t pid_t;

/** device */
typedef int dev_t;

/** time type */
struct tm {
        int tm_sec;       //!> seconds after the minute        0-60*
        int tm_min;       //!> minutes after the hour          0-59
        int tm_hour;      //!> hours since midnight            0-23
        int tm_mday;      //!> day of the month                1-31
        int tm_mon;       //!> months since January            0-11
        int tm_year;      //!> years since                     1900
        int tm_wday;      //!> days since Sunday               0-6
        int tm_yday;      //!> days since January 1            0-365
        int tm_isdst;     //!> Daylight Saving Time flag
        int tm_isutc;     //!> UTC time flag (dnx extension)
};
#define __TM_STRUCT_DEFINED__

/** clock type */
typedef u32_t clock_t;
#define __CLOCK_TYPE_DEFINED__

/** time type */
typedef u32_t time_t;
#define __TIME_TYPE_DEFINED__

/** structure describing a mount table entry */
struct mntent {
        const char *mnt_fsname;         /**< device or server for filesystem */
        const char *mnt_dir;            /**< directory mounted on            */
        u64_t       mnt_total;          /**< device total size               */
        u64_t       mnt_free;           /**< device free                     */
};

/** file type */
typedef enum tfile {
        FILE_TYPE_UNKNOWN,
        FILE_TYPE_REGULAR,
        FILE_TYPE_DIR,
        FILE_TYPE_DRV,
        FILE_TYPE_LINK,
        FILE_TYPE_PROGRAM,
        FILE_TYPE_PIPE
} tfile_t;

/** directory entry */
typedef struct dirent {
        char   *name;
        u64_t   size;
        tfile_t filetype;
        dev_t   dev;
} dirent_t;

/** file statistics */
struct stat {
        u64_t   st_size;                /**< total size, in bytes         */
        u32_t   st_dev;                 /**< ID of device containing file */
        u32_t   st_mode;                /**< protection                   */
        u32_t   st_uid;                 /**< user ID of owner             */
        u32_t   st_gid;                 /**< group ID of owner            */
        u32_t   st_atime;               /**< time of last access          */
        u32_t   st_mtime;               /**< time of last modification    */
        tfile_t st_type;                /**< type of file                 */
};

/** file system statistic */
struct statfs {
        u32_t       f_type;             /**< file system type       */
        u32_t       f_bsize;            /**< block size             */
        u32_t       f_blocks;           /**< total blocks           */
        u32_t       f_bfree;            /**< free blocks            */
        u32_t       f_files;            /**< total file nodes in FS */
        u32_t       f_ffree;            /**< free file nodes in FS  */
        const char *f_fsname;           /**< FS name                */
};

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _TYPES_H_ */
/*==============================================================================
  End of file
==============================================================================*/
