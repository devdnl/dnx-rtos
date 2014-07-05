/*=========================================================================*//**
@file    misc.h

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

#ifndef _MISC_H_
#define _MISC_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
//==============================================================================
/**
 * @brief ARRAY_SIZE(array)
 * The macro <b>ARRAY_SIZE</b>() calculate size of an array <i>array</i>.
 *
 * @param array     array name
 *
 * @errors None
 *
 * @return Array size (number of elements).
 *
 * @example
 * // ...
 *
 * int buf[100];
 * for (int i = 0; i < ARRAY_SIZE(buf); i++) {
 *         // ...
 * }
 *
 * // ...
 */
//==============================================================================
#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))

//==============================================================================
/**
 * @brief ARRAY_ITEM_SIZE(array)
 * The macro <b>ARRAY_ITEM_SIZE</b>() calculate size of an array <i>array</i>
 * element.
 *
 * @param array     array name
 *
 * @errors None
 *
 * @return Item size of array <i>array</i>.
 *
 * @example
 * // ...
 *
 * int buf[100];
 * printf("Item size: %d", ARRAY_ITEM_SIZE(buf));
 *
 * // ...
 */
//==============================================================================
#define ARRAY_ITEM_SIZE(array) (sizeof(array[0]))

//==============================================================================
/**
 * @brief UNUSED_ARG(argument)
 * The macro <b>UNUSED_ARG</b>() disable compiler warning if argument
 * <i>argument</i> is not used.
 *
 * @param argument  argument
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * // ...
 *
 * void some_function(int a, int b)
 * {
 *         UNUSED_ARG(b);
 *
 *         printf("%d", a);
 * }
 *
 * // ...
 */
//==============================================================================
#define UNUSED_ARG(argument) (void)argument

//==============================================================================
/**
 * @brief FIRST_CHARACTER(char__pstr)
 * The macro <b>FIRST_CHARACTER</b>() return the first character of string.
 *
 * @param char__pstr    string
 *
 * @errors None
 *
 * @return Returns the first character of string.
 *
 * @example
 * // ...
 *
 * const char *str = "foo bar";
 *
 * if (FIRST_CHARACTER(str) == 'f') {
 *         // ...
 * } else {
 *         // ...
 * }
 *
 * // ...
 */
//==============================================================================
#define FIRST_CHARACTER(char__pstr) char__pstr[0]

//==============================================================================
/**
 * @brief LAST_CHARACTER(char__pstr)
 * The macro <b>LAST_CHARACTER</b>() return the last character of string.
 *
 * @param char__pstr    string
 *
 * @errors None
 *
 * @return Returns the last character of string.
 *
 * @example
 * // ...
 *
 * const char *str = "foo bar";
 *
 * if (LAST_CHARACTER(str) == 'r') {
 *         // ...
 * } else {
 *         // ...
 * }
 *
 * // ...
 */
//==============================================================================
#define LAST_CHARACTER(char__pstr) char__pstr[strlen(char__pstr) - 1]

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
#define static_cast(type, var) (type)(var)

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
#define const_cast(type, var) (type)(var)

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
#define reinterpret_cast(type, var) (type)(var)

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

#endif /* _MISC_H_ */
/*==============================================================================
  End of file
==============================================================================*/
