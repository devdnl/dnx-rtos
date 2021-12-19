/*==============================================================================
File     strchrrep.c

Author   Daniel Zorychta

Brief    String character replace.

         Copyright (C) 2019 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "lib/strchrrep.h"

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
 * @brief  Function replace characters in string.
 *
 * @param  str          string (in/out buffer)
 * @param  from         find character
 * @param  to           replace character
 *
 * @return Number of replaced characters.
 */
//==============================================================================
int _strchrrep(char *str, char from, char to)
{
        int repl = 0;

        while (str && *str != '\0') {
                if (*str == from) {
                        *str = to;
                        repl++;
                }
                str++;
        }

        return repl;
}

/*==============================================================================
  End of file
==============================================================================*/
