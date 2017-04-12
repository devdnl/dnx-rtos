/*==============================================================================
File     strlcat.c

Author   Daniel Zorychta

Brief    String functions.

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
#include <string.h>
#include "lib/strlcat.h"

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
size_t _strlcat(char *dst, const char *src, size_t size)
{
        char       *d = dst;
        const char *s = src;
        size_t      n = size;
        size_t      dlen;

        /* Find the end of dst and adjust bytes left but don't go past end */
        while (n-- != 0 && *d != '\0') {
                d++;
        }

        dlen = d - dst;
        n    = size - dlen;

        if (n == 0) {
                return (dlen + strlen(s));
        }

        while (*s != '\0') {
                if (n != 1) {
                        *d++ = *s;
                         n--;
                }
                s++;
        }

        *d = '\0';

        /* count does not include NUL */
        return (dlen + (s - src));
}

/*==============================================================================
  End of file
==============================================================================*/
