/*=========================================================================*//**
@file    cast.h

@author  Daniel Zorychta

@brief

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _LIB_CAST_H_
#define _LIB_CAST_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "kernel/builtinfunc.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
//==============================================================================
/**
 * @brief static_cast(type, var)
 * The macro <b>static_cast</b>() cast variable <i>var</i> to type <i>type</i>.
 * Use this macro to remove <b>static</b> attribute.
 *
 * @param type          casting type
 * @param var           variable to cast
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * // ...
 *
 * static void *data   = calloc(1, 10);
 * void        *buffer = static_cast(void*, data);
 *
 * // ...
 */
//==============================================================================
#ifndef __cplusplus
#define static_cast(type, var) ((type)(var))
#endif

//==============================================================================
/**
 * @brief const_cast(type, var)
 * The macro <b>const_cast</b>() cast variable <i>var</i> to type <i>type</i>.
 * Use this macro to remove <b>const</b> attribute.
 *
 * @param type          casting type
 * @param var           variable to cast
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * // ...
 *
 * void func(char *str)
 * {
 *        // ...
 * }
 *
 * const char *str = "test";
 *
 * func(const_cast(char*, str));
 *
 * // ...
 */
//==============================================================================
#ifndef __cplusplus
#define const_cast(type, var) ((type)(var))
#endif

//==============================================================================
/**
 * @brief const_cast(type, var)
 * The macro <b>const_cast</b>() cast variable <i>var</i> to type <i>type</i>.
 * Use this macro to reinterpret selected variable type.
 *
 * @param type          casting type
 * @param var           variable to cast
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * // ...
 *
 * void   *data   = calloc(1, 10);
 * type_t *buffer = reinterpret_cast(type_t*, data);
 *
 * // ...
 */
//==============================================================================
#ifndef __cplusplus
#define reinterpret_cast(type, var) ((type)(var))
#endif

//==============================================================================
/**
 * @brief cast(type, var)
 * The macro <b>cast</b>() cast variable <i>var</i> to type <i>type</i>.
 * Use this macro to cast selected variable type.
 *
 * @param type          casting type
 * @param var           variable to cast
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * // ...
 *
 * void   *data   = calloc(1, 10);
 * type_t *buffer = cast(type_t*, data);
 *
 * // ...
 */
//==============================================================================
#ifndef __cplusplus
#define cast(type, var) ((type)(var))
#endif

/*==============================================================================
  Exported object types
==============================================================================*/

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

#endif /* _LIB_CAST_H_ */
/*==============================================================================
  End of file
==============================================================================*/
