/*=========================================================================*//**
@file    types.h

@author  Daniel Zorychta

@brief   System types.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/**
\defgroup sys-types-h <sys/types.h>

The library contains system types.

*/
/**@{*/

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
/** @brief Short version of uint8_t type. */
typedef uint8_t u8_t;

/** @brief Short version of int8_t type. */
typedef int8_t i8_t;

/** @brief Short version of uint16_t type. */
typedef uint16_t u16_t;

/** @brief Short version of int16_t type. */
typedef int16_t i16_t;

/** @brief Short version of uint32_t type. */
typedef uint32_t u32_t;

/** @brief Short version of int32_t type. */
typedef int32_t i32_t;

/** @brief Short version of uint64_t type. */
typedef uint64_t u64_t;

/** @brief Short version of int64_t type. */
typedef int64_t i64_t;

/** @brief Short version of unsigned int type. */
typedef unsigned int uint;

/** @brief Short version of unsigned long type. */
typedef unsigned long ulong;

/** @brief File position type. */
typedef uint64_t fpos_t;

/** @brief Size type with negative values. */
typedef int ssize_t;

/** @brief File descriptor. */
typedef intptr_t fd_t;

/** @brief File offset. */
typedef ssize_t off_t;

/**
 * @brief File mode.
 * @see S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRO, S_IWGRO, S_IXGRO, S_IROTH, S_IWOTH, S_IXOTH
 */
typedef u32_t mode_t;

/** @brief User ID. */
typedef u16_t uid_t;

/** @brief Group ID. */
typedef u16_t gid_t;

/**
 * @brief Process ID.
 *
 * The type represent process number.
 */
typedef u16_t pid_t;

/**
 * @brief Thread ID.
 *
 * The type represent thread number.
 */
typedef u8_t tid_t;

/**
 * @brief Device identifier.
 *
 * The type represents device ID (address of module and particular device). @n
 * Bit reservation: 15b module ID + 8b major + 8b minor. @n
 * Negative values means that device identifier is broken.
 */
typedef i32_t dev_t;

#ifndef DOXYGEN /* Doxygen description in time.h */
/** @brief Time representation type. */
struct tm {
        int tm_sec;             //!< Seconds after the minute        (0-60*)
        int tm_min;             //!< Minutes after the hour          (0-59)
        int tm_hour;            //!< Hours since midnight            (0-23)
        int tm_mday;            //!< Day of the month                (1-31)
        int tm_mon;             //!< Months since January            (0-11)
        int tm_year;            //!< Years since                     (1900)
        int tm_wday;            //!< Days since Sunday               (0-6)
        int tm_yday;            //!< Days since January 1            (0-365)
        int tm_isdst;           //!< Daylight Saving Time flag
        int tm_isutc;           //!< UTC time flag (dnx RTOS extension)
};
#define __TM_STRUCT_DEFINED__
#endif /* DOXYGEN */

#ifndef DOXYGEN /* Doxygen description in time.h */
/** @brief Clock representation type. */
typedef u64_t clock_t;
#define __CLOCK_TYPE_DEFINED__
#endif /* DOXYGEN */

#ifndef DOXYGEN /* Doxygen description in time.h */
/** @brief Time representation type. */
typedef u32_t time_t;
#define __TIME_TYPE_DEFINED__
#endif /* DOXYGEN */

#ifndef DOXYGEN /* Doxygen description in sys/time.h */
/** @brief Subseconds time representation */
typedef u32_t suseconds_t;
#define __SUSECONDS_TYPE_DEFINED__
#endif

#ifndef DOXYGEN
/** @brief Type representing time value. */
struct timeval {
        time_t tv_sec;          /*!< seconds */
        suseconds_t tv_usec;    /*!< microseconds */
};
#define __STRUCT_TIMEVAL_DEFINED__
#endif

#ifndef DOXYGEN // Doxygen documentation added to mntent.h file
/** @brief Structure that describes a mount table entry. */
struct mntent {
        const char *mnt_fsname; //!< Device or server for file system
        const char *mnt_dir;    //!< Directory mounted on
        u64_t       mnt_total;  //!< Device total size in bytes
        u64_t       mnt_free;   //!< Device free space in bytes
};
#endif

#ifndef DOXYGEN // Doxygen documentation inserted in dirent.h file
/** @brief Directory entry. */
typedef struct dirent {
        const char *d_name;         //!< File name
        u64_t       size;           //!< File size in bytes
        mode_t      mode;           //!< File mode (protection, file type)
        dev_t       dev;            //!< Device address (if file type is driver)
} dirent_t;
#endif

/** @brief File information. */
struct stat {
        u64_t   st_size;        /*!< Total size, in bytes.*/
        dev_t   st_dev;         /*!< ID of device containing file.*/
        mode_t  st_mode;        /*!< Protection, file type.*/
        uid_t   st_uid;         /*!< User ID of owner.*/
        gid_t   st_gid;         /*!< Group ID of owner.*/
        time_t  st_ctime;       /*!< Time of creation.*/
        time_t  st_mtime;       /*!< Time of last modification.*/
};

/** file system statistic */
struct statfs {
        u32_t       f_type;     /*!< File system type. @see @ref SYS_FS_TYPE*/
        u32_t       f_bsize;    /*!< Block size.*/
        u32_t       f_blocks;   /*!< Total blocks.*/
        u32_t       f_bfree;    /*!< Free blocks.*/
        u32_t       f_files;    /*!< Total file nodes in file system.*/
        u32_t       f_ffree;    /*!< Free file nodes in file system.*/
        const char *f_fsname;   /*!< File system name.*/
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

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
