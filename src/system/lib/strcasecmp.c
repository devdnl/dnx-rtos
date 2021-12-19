/*==============================================================================
File    strcasecmp.c

Author  Daniel Zorychta

Brief   String compare with no case sensitive.

        Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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

==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include <stdint.h>
#include <stddef.h>
#include <ctype.h>

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Function compares the two strings and ignore case of the characters.
 *
 * The strcasecmp() function compares the two strings <i>s1</i> and <i>s2</i>,
 * ignoring the case of the characters.  It returns an integer less
 * than, equal to, or greater than zero if <i>s1</i> is found, respectively, to
 * be less than, to match, or be greater than <i>s2</i>.
 *
 * @param s1        string 1
 * @param s2        string 2
 *
 * @return The strcasecmp() function return an integer less than, equal to, or
 * greater than zero if <i>s1</i> is found, respectively, to be less than, to
 * match, or be greater than <i>s2</i>.
 *
 * @b Example
 * @code
        // ...
        const char *foo = "FOO";
        const char *bar = "foo";

        if (strcasecmp(foo, bar) == 0) {
                // result: functions are equal
        }

        // ...
   @endcode
 *
 * @see strncasecmp(), strcmp(), strncmp(), memcmp()
 */
//==============================================================================
int strcasecmp(const char *s1, const char *s2)
{
        if (!s1 || !s2) {
                return 1;
        }

        do {
                int d = tolower(*s1++) - tolower(*s2++);
                if (d != 0) {
                        return d;
                }

                if (*s1 == '\0' || *s2 == '\0') {
                        return *s1 - *s2;
                }

        } while (*s1 && *s2);

        return 0;
}

//==============================================================================
/**
 * @brief Function compares the two strings and ignore case of the characters with limits.
 *
 * The strncasecmp() function is similar to strcasecmp(), except it
 * compares the only first <i>n</i> bytes of <i>s1</i>.
 *
 * @param s1        string 1
 * @param s2        string 2
 * @param n         max number of bytes to compare
 *
 * @return The strncasecmp() function return an integer less than, equal to, or
 * greater than zero if <i>s1</i> or the first <i>n</i> bytes thereof is found,
 * respectively, to be less than, to match, or be greater than <i>s2</i>.
 *
 * @b Example
 * @code
        // ...
        const char *foo = "FOO";
        const char *bar = "foo";

        if (strncasecmp(foo, bar, 3) == 0) {
                // result: functions are equal
        }

        // ...
   @endcode
 *
 * @see strcasecmp(), strcmp(), strncmp(), memcmp()
 */
//==============================================================================
int strncasecmp(const char *s1, const char *s2, size_t n)
{
        if (!s1 || !s2 || !n) {
                return 1;
        }

        do {
                int d = tolower(*s1++) - tolower(*s2++);
                if (d != 0) {
                        return d;
                }

                if (--n == 0) {
                        break;
                }

                if (*s1 == '\0' || *s2 == '\0') {
                        return *s1 - *s2;
                }

        } while (*s1 && *s2);

        return 0;
}

/*==============================================================================
  End of file
==============================================================================*/
