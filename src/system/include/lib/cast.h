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

/**
\addtogroup dnx-misc-h
*/
/**@{*/

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
 * @brief The macro casts variable <i>var</i> to type <i>type</i>.
 * Use this macro to remove <b>const</b> attribute.
 *
 * @param type          casting type
 * @param var           variable to cast
 *
 * @b Example
 * @code
   // ...

   void func(char *str)
   {
          // ...
   }

   const char *str = "test";

   func(const_cast(char*, str));

   // ...
 * @endcode
 */
//==============================================================================
#ifndef __cplusplus
#define const_cast(type, var) ((type)(var))
#endif

//==============================================================================
/**
 * @brief The macro casts variable <i>var</i> to type <i>type</i>.
 * Use this macro to cast selected variable to any type.
 *
 * @param type          casting type
 * @param var           variable to cast
 *
 * @b Example
 * @code
   // ...

   void   *data   = calloc(1, 10);
   type_t *buffer = cast(type_t*, data);

   // ...
 * @endcode
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
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
