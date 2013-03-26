#ifndef BASIC_TYPES_H_
#define BASIC_TYPES_H_
/*=========================================================================*//**
@file    basic_types.h

@author  Daniel Zorychta

@brief   This file define basic types.

@note    Copyright (C) 2012  Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <stdint.h>
#include <stddef.h>

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/** TRUE definition */
#ifndef TRUE
#define TRUE                              (0 == 0)
#endif

/** FALSE definition */
#ifndef FALSE
#define FALSE                             (0 != 0)
#endif

/** define NULL pointer */
#ifndef NULL
#define NULL                              ((void*)0)
#endif

/** RESET definition */
#ifndef RESET
#define RESET                             (0)
#endif

/** SET definition */
#ifndef SET
#define SET                               (!RESET)
#endif

/** array element count */
#define ARRAY_SIZE(array)                 (sizeof(array)/sizeof(array[0]))
#define ARRAY_ITEM_SIZE(array)            (sizeof(array[0]))
#define FIELD_SIZEOF(t, f)                (sizeof(((t*)0)->f))

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
/** define boolean type */
typedef uint8_t bool;

/** define flag type */
typedef uint8_t flag_t;

/** short types */
typedef uint8_t      u8_t;
typedef int8_t       i8_t;
typedef uint16_t     u16_t;
typedef int16_t      i16_t;
typedef uint32_t     u32_t;
typedef int32_t      i32_t;
typedef uint64_t     u64_t;
typedef int64_t      i64_t;
typedef unsigned int uint;

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* BASIC_TYPES_H_ */
/*==============================================================================
  End of file
==============================================================================*/
