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
#define STDIO_BUFFER_SIZE                 100
#define KPRINT_BUFFER_SIZE                80


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
   STD_RET_ALLOCERROR         = 2,
   STD_RET_UNKNOWN            = 127,
};


/** universal status type */
typedef signed char stdRet_t;


/** device number type */
typedef u8_t nod_t;


/** IO request type */
typedef u8_t IORq_t;


/** file type */
typedef struct
{
      stdRet_t (*close)(nod_t dev);
      size_t   (*write)(nod_t dev, void *src, size_t size, size_t nitems, size_t seek);
      size_t   (*read )(nod_t dev, void *dst, size_t size, size_t nitmes, size_t seek);
      stdRet_t (*ioctl)(nod_t dev, IORq_t iorq, void *data);
      u32_t    fd;
      size_t   seek;
} FILE_t;


/** application standard arguments type */
typedef struct appArgs_struct
{
      void *arg;                      /* pointer to the argument */
      u8_t tty;                       /* stdin fifo */
      void *ChildTaskHandle;          /* FreeRTOS task handling for children */
      void *ParentTaskHandle;         /* FreeRTOS task handling for parent */
      stdRet_t    exitCode;           /* exit code */
} appArgs_t;


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
