/*=========================================================================*//**
@file    misc.h

@author  Daniel Zorychta

@brief   Miscellaneous macros and functions

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes dnx RTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


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
#include <stdbool.h>
#include <lib/cast.h>
#include <lib/unarg.h>
#include <lib/strlcat.h>
#include <lib/strlcpy.h>

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
 * @brief Macro check if string is NULL or empty.
 *
 * The macro isstrempty() check if string is a NULL pointer (then detected also
 * as empty) or is empty ("").
 *
 * @param _str          string to check
 *
 * @return Return \b true if string is NULL or is empty, otherwise \b false.
 *
 * @b Example
 * @code
        // ...

        #include <dnx/misc.h>

        const char *str1 = "str1";

        if (isstrempty(str1)) {
             puts("String is empty!");
        } else {
             puts(str1);
        }

        // ...

   @endcode
 */
//==============================================================================
#define isstrempty(_str) (((_str) == NULL) || ((_str)[0] == '\0'))

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

//==============================================================================
/**
 * @brief Macro tells compiler that selected structure is packed.
 */
//==============================================================================
#define PACKED __attribute__((packed))

//==============================================================================
/**
 * @brief Macro returns division rounded up.
 * @note  Macro should be used only for positive values.
 */
//==============================================================================
#define CEILING(x,y) (((x) + (y) - 1) / (y))

//==============================================================================
/**
 * @brief Macro check if operation is success (ESUCC, 0), if not then goto
 *        user-defined label.
 */
//==============================================================================
#define catcherr(op, errlabel) if ((op) != 0) {goto errlabel;}

//==============================================================================
/**
 * @brief Macro check if value is in range.
 */
//==============================================================================
#define IS_IN_RANGE(val, min, max) (((val) > (min)) && ((val) < (max)))

//==============================================================================
/**
 * @brief Macro check if value is in sharp range.
 */
//==============================================================================
#define IS_IN_RANGE_SHARP(val, min, max) (((val) >= (min)) && ((val) <= (max)))

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
//==============================================================================
/**
 * @brief The strlcpy() function copy strings.
 *
 * The strlcpy() function copies up to size - 1 characters from the NUL-terminated
 * string src to dst, NUL-terminating the result.
 *
 * Note, however, that if strlcat() traverses size characters without finding
 * a NUL, the length of the string is considered to be size and the destination
 * string will not be NUL-terminated (since there was no space for the NUL).
 *
 * @param  dst  destination buffer
 * @param  src  source buffer
 * @param  size destination buffer size
 *
 * @return The strlcpy() function return the total length of the string they
 *         tried to create. For strlcpy() that means the length of src.
 */
//==============================================================================
static inline size_t strlcpy(char *dst, const char *src, size_t size)
{
        return _strlcpy(dst, src, size);
}

//==============================================================================
/**
 * @brief The strlcat() function concatenate strings.
 *
 * The strlcat() function appends the NUL-terminated string src to the end of
 * dst. It will append at most size - strlen(dst) - 1 bytes, NUL-terminating
 * the result.
 *
 * @param  dst  destination buffer
 * @param  src  source buffer
 * @param  size destination buffer size
 *
 * @return The strlcat() function return the total length of the string it
 *         tried to create. For strlcat() that means the initial length of dst
 *         plus the length of src. While this may seem somewhat confusing, it
 *         was done to make truncation detection simple.
 */
//==============================================================================
static inline size_t strlcat(char *dst, const char *src, size_t size)
{
        return _strlcat(dst, src, size);
}

//==============================================================================
/**
 * @brief  Function check if selected object is allocated in heap.
 *
 * @param  ptr          object's pointer
 *
 * @return If object is in heap then true is returned, otherwise false.
 */
//==============================================================================
static inline bool is_object_in_heap(void *ptr)
{
        extern bool _mm_is_object_in_heap(void *ptr);
        return _mm_is_object_in_heap(ptr);
}

//==============================================================================
/**
 * @brief  Function replace characters in string.
 *
 * @param  str          string (in/out buffer)
 * @param  from         find character
 * @param  to           replace character
 *
 * @return Number of replaced characters.
 */
//==============================================================================
static inline int strchrrep(char *str, char from, char to)
{
        extern int _strchrrep(char *str, char from, char to);
        return _strchrrep(str, from, to);
}

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
