#ifndef SYSTYPES_H_
#define SYSTYPES_H_
/*=========================================================================*//**
@file    systypes.h

@author  Daniel Zorychta

@brief   This file contains all system types

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <stddef.h>
#include "basic_types.h"
#include "ostypes.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/* disable conflicting types from standard libraries */
#undef stdin
#undef stdout
#undef malloc
#undef free

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
/** default system status */
enum status_enum
{
        STD_RET_OK      = 0,
        STD_RET_ERROR   = 1,
        STD_RET_UNKNOWN = 127,
};

/** file types */
typedef enum
{
        FILE_TYPE_REGULAR,
        FILE_TYPE_DIR,
        FILE_TYPE_DRV,
        FILE_TYPE_LINK
} tfile_t;

/** universal status type */
typedef signed char stdRet_t;

/** device number type */
typedef uint devx_t;

/** IO request type */
typedef u8_t IORq_t;

/** file descriptor */
typedef uint fd_t;

/** file system descriptor */
typedef uint fsd_t;

/** file type */
typedef struct
{
        uint     dev;
        uint     fd;
        stdRet_t (*f_close)(uint dev, uint fd);
        size_t   (*f_write)(uint dev, uint fd, void *src, size_t size, size_t nitems, size_t seek);
        size_t   (*f_read )(uint dev, uint fd, void *dst, size_t size, size_t nitmes, size_t seek);
        stdRet_t (*f_ioctl)(uint dev, uint fd, IORq_t iorq, void *data);
        stdRet_t (*f_stat )(uint dev, uint fd, void *stat);
        size_t   f_seek;
} FILE_t;

/** directory entry */
typedef struct
{
        ch_t   *name;
        size_t  size;
        tfile_t filetype;
} dirent_t;

/** directory type */
typedef struct dir_s
{
        dirent_t (*rddir)(fsd_t fsd, struct dir_s *dir);
        stdRet_t (*cldir)(fsd_t fsd, struct dir_s *dir);
        size_t     items;
        size_t     seek;
        void      *dd;
        fsd_t      fsd;
} DIR_t;

/** time structure */
typedef struct time_struct
{
        u8_t seconds;     /**< [BCD] */
        u8_t minutes;     /**< [BCD] */
        u8_t hours;       /**< [BCD] */
} bcdTime_t;

/** date structure */
typedef struct date_struct
{
        u8_t weekday;     /**< [BCD] */
        u8_t day;         /**< [BCD] */
        u8_t month;       /**< [BCD] */
        u8_t year;        /**< [BCD] */
} bcdDate_t;

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* SYSTYPES_H_ */
/*==============================================================================
  End of file
==============================================================================*/
