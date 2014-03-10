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

/* special types */
typedef uint64_t        fpos_t;
typedef int             ssize_t;

/** universal status type */
typedef enum stdret
{
        STD_RET_OK      = 0,
        STD_RET_ERROR   = 1,
} stdret_t;

/** file descriptor */
typedef uint fd_t;

/** file mode */
typedef int mode_t;

/** device */
typedef uint dev_t;

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
