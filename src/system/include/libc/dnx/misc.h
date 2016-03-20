/*=========================================================================*//**
@file    misc.h

@author  Daniel Zorychta

@brief   Miscellaneous macros and functions

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

/**
\defgroup dnx-misc-h <dnx/misc.h>

dnx RTOS miscellaneous macros and functions.

*/
/**@{*/

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
 * @brief Macro calculate numbers of items of selected array.
 *
 * The macro ARRAY_SIZE() calculate number of items of array <i>array</i>.
 * The type of calculated value is size_t.
 *
 * @param array     array name
 *
 * @return Array size (number of elements).
 *
 * @b Example
 * @code
        // ...

        int buf[100];
        for (int i = 0; i < ARRAY_SIZE(buf); i++) {
                // ...
        }

        // ...
   @endcode
 *
 * @see ARRAY_ITEM_SIZE()
 */
//==============================================================================
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

//==============================================================================
/**
 * @brief Macro calculate size of item of selected array.
 *
 * The macro ARRAY_ITEM_SIZE() calculate size of item of selected array
 * <i>array</i>
 *
 * @param array     array name
 *
 * @return Item size of array <i>array</i>.
 *
 * @b Example
 * @code
        // ...

        int buf[100];
        printf("Item size: %d", ARRAY_ITEM_SIZE(buf));

        // ...

   @endcode
 *
 * @see ARRAY_SIZE()
 */
//==============================================================================
#define ARRAY_ITEM_SIZE(array) (sizeof(array[0]))

//==============================================================================
/**
 * @brief Macro family that disables warning of unused argument or variable.
 *
 * The macro UNUSED_ARGx() disable compiler warning if argument
 * <i>args</i> is not used, where <i>x</i> is number of arguments that are
 * disabled in range of 1 to 8.
 *
 * @param args  arguments
 *
 * @b Example
 * @code
        // ...

        void some_function(int a, int b)
        {
                UNUSED_ARG1(b);

                printf("%d", a);

                UNUSED_ARG2(a, b);
        }

        // ...
   @endcode
 */
//==============================================================================
#ifdef DOXYGEN
#define UNUSED_ARGx(args)
#else
// defined in <lib/unarg.h>
#endif

//==============================================================================
/**
 * @brief Macro returns first character of string.
 *
 * The macro FIRST_CHARACTER() return the first character of string.
 *
 * @param char__pstr    string
 *
 * @return Returns the first character of string.
 *
 * @b Example
 * @code
        // ...

        const char *str = "foo bar";

        if (FIRST_CHARACTER(str) == 'f') {
                // ...
        } else {
                // ...
        }

        // ...

   @endcode
 *
 * @see strfch(), strlch(), LAST_CHARACTER()
 */
//==============================================================================
#define FIRST_CHARACTER(char__pstr) (char__pstr)[0]

//==============================================================================
/**
 * @brief Macro returns first character of string.
 *
 * The macro strfch() return the first character of string.
 *
 * @param char__pstr    string
 *
 * @return Returns the first character of string.
 *
 * @b Example
 * @code
        // ...

        const char *str = "foo bar";

        if (strfch(str) == 'f') {
                // ...
        } else {
                // ...
        }

        // ...

   @endcode
 *
 * @see FIRST_CHARACTER(), strlch(), LAST_CHARACTER()
 */
//==============================================================================
#define strfch(char__pstr) FIRST_CHARACTER(char__pstr)

//==============================================================================
/**
 * @brief Macro returns last character of string.
 *
 * The macro LAST_CHARACTER() return the last character of string.
 *
 * @param char__pstr    string
 *
 * @return Returns the last character of string.
 *
 * @b Example
 * @code
        // ...

        const char *str = "foo bar";

        if (LAST_CHARACTER(str) == 'r') {
                // ...
        } else {
                // ...
        }

        // ...

   @endcode
 *
 * @see strlch(), strfch(), FIRST_CHARACTER()
 */
//==============================================================================
#define LAST_CHARACTER(char__pstr) (char__pstr)[strlen((char__pstr)) - 1]

//==============================================================================
/**
 * @brief Macro returns last character of string.
 *
 * The macro strlch() return the last character of string.
 *
 * @param char__pstr    string
 *
 * @return Returns the last character of string.
 *
 * @b Example
 * @code
        // ...

        const char *str = "foo bar";

        if (strlch(str) == 'r') {
                // ...
        } else {
                // ...
        }

        // ...

   @endcode
 *
 * @see LAST_CHARACTER(), strfch(), FIRST_CHARACTER()
 */
//==============================================================================
#define strlch(char__pstr) LAST_CHARACTER(char__pstr)

//==============================================================================
/**
 * @brief Macro returns minimal value of given values.
 *
 * The macro min() compares variables <i>a</i> and <i>b</i> and returns
 * the smaller value.
 *
 * @warning Use this macro with caution because of side effects
 * possibility (e.g. by using increment or decrement instructions).
 *
 * @param a             variable a
 * @param b             variable b
 *
 * @return The smaller value.
 *
 * @b Example
 * @code
        // ...

        printf("%d\n", min(5, 6)); // prints 5

        // ...

   @endcode
 *
 * @see max()
 */
//==============================================================================
#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

//==============================================================================
/**
 * @brief Macro returns maximum value of given values.
 *
 * The macro max() compares variables <i>a</i> and <i>b</i> and returns
 * the higher value.
 *
 * @warning Use this macro with caution because of side effects
 * possibility (e.g. by using increment or decrement instructions).
 *
 * @param a             variable a
 * @param b             variable b
 *
 * @return The higher value.
 *
 * @b Example
 * @code
        // ...

        printf("%d\n", max(5, 6)); // prints 6

        // ...

   @endcode
 *
 * @see min()
 */
//==============================================================================
#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

//==============================================================================
/**
 * @brief Macro compares two strings.
 *
 * The macro isstreq() compares string <i>_stra</i> and <i>_strb</i> and
 * returns \b true or \b false depending on compare result (true if equal).
 *
 * @param _stra         string a
 * @param _strb         string b
 *
 * @return If strings are equal then true is returned, otherwise false.
 *
 * @b Example
 * @code
        // ...

        #include <dnx/misc.h>

        const char *str1 = "str1";
        const char *str2 = "str2";

        if (isstreq(str1, str2)) {
             puts("Strings are equal");
        } else {
             puts("Strings are not equal");
        }

        // ...

   @endcode
 *
 * @see strcmp()
 */
//==============================================================================
#define isstreq(_stra, _strb) (strcmp(_stra, _strb) == 0)

//==============================================================================
/**
 * @brief Macro compares two strings with limited size.
 *
 * The macro isstreqn() compares string <i>_stra</i> and <i>_strb</i> and
 * returns \b true or \b false depending on compare result (true if equal). User
 * determines size of strings to compare.
 *
 * @param _stra         string a
 * @param _strb         string b
 * @param _n            string length
 *
 * @return If strings are equal then \b true is returned, otherwise \b false.
 *
 * @b Example
 * @code
        // ...

        #include <dnx/misc.h>

        const char *str1 = "str1";
        const char *str2 = "str2";

        if (isstreqn(str1, str2, 3)) {
             puts("Strings are equal");
        } else {
             puts("Strings are not equal");
        }

        // ...

   @endcode
 *
 * @see strncmp()
 */
//==============================================================================
#define isstreqn(_stra, _strb, _n) (strncmp(_stra, _strb, _n) == 0)

//==============================================================================
/**
 * @brief Macro returns string size (with nul terminator).
 *
 * The macro strsize() returns total size of string (with \0 terminator).
 * Function is helpful when user allocate new string in memory.
 *
 * @param _str          string
 *
 * @return String size.
 *
 * @b Example
 * @code
        // ...

        #include <dnx/misc.h>

        char *str = malloc(strsize("My string"));

        // ...

   @endcode
 *
 * @see strlen()
 */
//==============================================================================
#define strsize(_str) (strlen(_str) + 1)

//==============================================================================
/**
 * @brief Macro is alias of logical \b AND operator: &&
 *
 * The macro @ref and is an logical AND (&&) in C++11 style.
 *
 * @b Example
 * @code
        // ...

        #include <dnx/misc.h>

        if (1 == 1 and 2 == 2) {
             // ...
        }

        // ...

   @endcode
 *
 * @see or, not
 */
//==============================================================================
#ifndef __cplusplus
#define and &&
#endif

//==============================================================================
/**
 * @brief Macro is alias of logical \b OR operator: ||
 *
 * The macro @ref or is an logical OR (||) in C++11 style.
 *
 * @b Example
 * @code
        // ...

        #include <dnx/misc.h>

        if (1 == 1 or 2 == 2) {
             // ...
        }

        // ...

   @endcode
 *
 * @see and, not
 */
//==============================================================================
#ifndef __cplusplus
#define or ||
#endif

//==============================================================================
/**
 * @brief Macro is alias of logical \b NOT operator: !
 *
 * The macro @ref not is an logical NOT (!) in C++11 style.
 *
 * @b Example
 * @code
        // ...

        #include <dnx/misc.h>

        if (not (1 == 1 and 2 == 2)) {
             // ...
        }

        // ...

   @endcode
 *
 * @see and, or
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

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
