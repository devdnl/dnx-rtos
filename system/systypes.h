#ifndef SYSTYPES_H_
#define SYSTYPES_H_
/*=============================================================================================*//**
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


*//*==============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include <stddef.h>
#include "basic_types.h"


/*==================================================================================================
                                  Exported symbolic constants/macros
==================================================================================================*/
/* disable conflicting types from standard libraries */
#undef stdin
#undef stdout


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/
/** default system status */
enum status_enum
{
      STD_RET_OK                 = 0,
      STD_RET_ERROR              = 1,
      STD_RET_UNKNOWN            = 127,
};

/** universal status type */
typedef signed char stdRet_t;


/** device number type */
typedef size_t nod_t;


/** task/application ID */
typedef size_t PID_t;


/** IO request type */
typedef u8_t IORq_t;


/** file descriptor */
typedef size_t fd_t;


/** file type */
typedef struct
{
      u32_t    dev;
      u32_t    fd_part;
      stdRet_t (*close)(u32_t dev, u32_t fd_part);
      size_t   (*write)(u32_t dev, u32_t fd_part, void *src, size_t size, size_t nitems, size_t seek);
      size_t   (*read )(u32_t dev, u32_t fd_part, void *dst, size_t size, size_t nitmes, size_t seek);
      stdRet_t (*ioctl)(u32_t dev, u32_t fd_part, IORq_t iorq, void *data);
      size_t   seek;
} FILE_t;


/** directory entry */
typedef struct
{
      ch_t   *name;
      size_t  size;
      bool_t  isfile;
} dirent_t;


/** dir type */
typedef struct dir_s
{
      dirent_t  (*rddir)(struct dir_s *dir);
      size_t    items;
      size_t    seek;
      void     *dd;
} DIR_t;


/** application standard arguments type */
typedef struct appArgs_struct
{
      void    *arg;                       /* pointer to the argument */
      FILE_t  *stdin;                     /* file used only to read keyboard */
      FILE_t  *stdout;                    /* file used only to write to terminal */
      void    *TaskHandle;                /* FreeRTOS task handling for children */
      void    *ParentTaskHandle;          /* FreeRTOS task handling for parent */
      stdRet_t exitCode;                  /* exit code */
} app_t;


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


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/


#ifdef __cplusplus
}
#endif

#endif /* SYSTYPES_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
