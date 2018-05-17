/*=========================================================================*//**
@file    getc.c

@author  Daniel Zorychta

@brief   Character get functions.

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Include files
==============================================================================*/
#include <config.h>
#include <stdio.h>
#include "lib/unarg.h"

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
 * @brief Function get character from file
 *
 * @param *stream            source file
 *
 * @retval character
 */
//==============================================================================
int getc(FILE *stream)
{
#if (__OS_PRINTF_ENABLE__ > 0)
        if (!stream) {
                return EOF;
        }

        char chr = 0;
        if (fread(&chr, sizeof(char), 1, stream) != 0) {
                if (ferror(stream) || feof(stream)) {
                        return EOF;
                }
        } else {
                return EOF;
        }

        return chr;
#else
        UNUSED_ARG1(stream);
        return EOF;
#endif
}

/*==============================================================================
  End of file
==============================================================================*/
