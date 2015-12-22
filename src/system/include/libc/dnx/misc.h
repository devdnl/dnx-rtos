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
#include <lib/cast.h>
#include <lib/unarg.h>

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
 * @brief UNUSED_ARGx(argument)
 * The macro <b>UNUSED_ARGx</b>() disable compiler warning if argument
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
 *         UNUSED_ARG1(b);
 *
 *         printf("%d", a);
 * }
 *
 * // ...
 */
//==============================================================================
// defined in <lib/unarg.h>

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
#define FIRST_CHARACTER(char__pstr) (char__pstr)[0]
#define strfch(char__pstr) FIRST_CHARACTER(char__pstr)

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
#define LAST_CHARACTER(char__pstr) (char__pstr)[strlen((char__pstr)) - 1]
#define strlch(char__pstr) LAST_CHARACTER(char__pstr)

//==============================================================================
/**
 * @brief min(a, b)
 * The macro <b>min()</b> compares variables <i>a</i> and <i>b</i> and returns
 * the smaller value. Use this macro with caution because of side effects
 * possibility (e.g. by using increment or decrement instructions).
 *
 * @param a             variable a
 * @param b             variable b
 *
 * @errors None
 *
 * @return The smaller value.
 *
 * @example
 * // ...
 *
 * printf("%d\n", min(5, 6)); // prints 5
 *
 * // ...
 */
//==============================================================================
#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

//==============================================================================
/**
 * @brief max(a, b)
 * The macro <b>max()</b> compares variables <i>a</i> and <i>b</i> and returns
 * the higher value. Use this macro with caution because of side effects
 * possibility (e.g. by using increment or decrement instructions).
 *
 * @param a             variable a
 * @param b             variable b
 *
 * @errors None
 *
 * @return The higher value.
 *
 * @example
 * // ...
 *
 * printf("%d\n", max(5, 6)); // prints 6
 *
 * // ...
 */
//==============================================================================
#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

//==============================================================================
/**
 * @brief isstreq(_stra, _strb)
 * The macro <b>isstreq()</b> compares string <i>_stra</i> and <i>_strb</i> and
 * returns true or false depending on compare result (true if equal).
 *
 * @param _stra         string a
 * @param _strb         string b
 *
 * @errors None
 *
 * @return If strings are equal then true is returned, otherwise false.
 *
 * @example
 * // ...
 *
 * #include <dnx/misc.h>
 *
 * const char *str1 = "str1";
 * const char *str2 = "str2";
 *
 * if (isstreq(str1, str2)) {
 *      puts("Strings are equal");
 * } else {
 *      puts("Strings are not equal");
 * }
 *
 * // ...
 */
//==============================================================================
#define isstreq(_stra, _strb) (strcmp(_stra, _strb) == 0)

//==============================================================================
/**
 * @brief isstreqn(_stra, _strb, _n)
 * The macro <b>isstreqn()</b> compares string <i>_stra</i> and <i>_strb</i> and
 * returns true or false depending on compare result (true if equal). User
 * determines size of strings to compare.
 *
 * @param _stra         string a
 * @param _strb         string b
 * @param _n            string length
 *
 * @errors None
 *
 * @return If strings are equal then true is returned, otherwise false.
 *
 * @example
 * // ...
 *
 * #include <dnx/misc.h>
 *
 * const char *str1 = "str1";
 * const char *str2 = "str2";
 *
 * if (isstreqn(str1, str2, 3)) {
 *      puts("Strings are equal");
 * } else {
 *      puts("Strings are not equal");
 * }
 *
 * // ...
 */
//==============================================================================
#define isstreqn(_stra, _strb, _n) (strncmp(_stra, _strb, _n) == 0)

//==============================================================================
/**
 * @brief strsize(_str)
 * The macro <b>strsize()</b> returns total size of string (with \0 terminator).
 * Function is helpful when user allocate new string in memory.
 *
 * @param _str          string
 *
 * @errors None
 *
 * @return If strings are equal then true is returned, otherwise false.
 *
 * @example
 * // ...
 *
 * #include <dnx/misc.h>
 *
 * char *str = malloc(strsize("My string"));
 *
 * // ...
 */
//==============================================================================
#define strsize(_str) (strlen(_str) + 1)

//==============================================================================
/**
 * @brief and
 * The macro <b>and</b> is an logical AND (&&) in C++11 style.
 *
 * @param None
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * // ...
 *
 * #include <dnx/misc.h>
 *
 * if (1 == 1 and 2 == 2) {
 *      // ...
 * }
 *
 * // ...
 */
//==============================================================================
#ifndef __cplusplus
#define and &&
#endif

//==============================================================================
/**
 * @brief or
 * The macro <b>or</b> is an logical OR (||) in C++11 style.
 *
 * @param None
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * // ...
 *
 * #include <dnx/misc.h>
 *
 * if (1 == 1 or 2 == 2) {
 *      // ...
 * }
 *
 * // ...
 */
//==============================================================================
#ifndef __cplusplus
#define or ||
#endif

//==============================================================================
/**
 * @brief not
 * The macro <b>not</b> is an logical NOT (!) in C++11 style.
 *
 * @param None
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * // ...
 *
 * #include <dnx/misc.h>
 *
 * if (not (1 == 1 and 2 == 2)) {
 *      // ...
 * }
 *
 * // ...
 */
//==============================================================================
#ifndef __cplusplus
#define not !
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

#endif /* _MISC_H_ */
/*==============================================================================
  End of file
==============================================================================*/
