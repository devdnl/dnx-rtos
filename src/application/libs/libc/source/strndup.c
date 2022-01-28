/*==============================================================================
File     strndup.c

Author   Daniel Zorychta

Brief    String duplication.

         Copyright (C) 2022 Daniel Zorychta <daniel.zorychta@gmail.com>

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
 * @brief The strndup() function is used to duplicate a string of size n.
 *
 * This function returns a pointer to a null-terminated byte string, which is
 * a duplicate of the string pointed to by s. The memory obtained is done
 * dynamically using malloc and hence it can be freed using free().
 *
 * @param s             string
 * @param n             string size
 *
 * @return It returns a pointer to the duplicated string s.
 */
//==============================================================================
char *strndup(const char *s, size_t n)
{
        extern size_t _libc_strlcpy(char *dst, const char *src, size_t size);
        extern void *_libc_malloc(size_t size);

        n += 1;
        char *dup = _libc_malloc(n);
        if (dup) {
                _libc_strlcpy(dup, s, n);
        }

        return dup;
}

/*==============================================================================
  End of file
==============================================================================*/
