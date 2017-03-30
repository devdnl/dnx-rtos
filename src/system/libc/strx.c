/*==============================================================================
File     strlcpy.c

Author   Daniel Zorychta

Brief

	 Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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


==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include <string.h>
#include "lib/strx.h"

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
size_t _strlcpy(char *dst, const char *src, size_t size)
{
        char       *d = dst;
        const char *s = src;
        size_t      n = size;

        /* Copy as many bytes as will fit */
        if (n != 0 && --n != 0) {
                do {
                        if ((*d++ = *s++) == 0) break;
                } while (--n != 0);
        }

        /* Not enough room in dst, add NUL and traverse rest of src */
        if (n == 0) {
                if (size != 0) *d = '\0';
                while (*s++);
        }

        /* count does not include NUL */
        return (s - src - 1);
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
