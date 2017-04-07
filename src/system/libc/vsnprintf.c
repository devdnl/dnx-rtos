/*=========================================================================*//**
@file    vsnprintf.c

@author  Daniel Zorychta

@brief   Print functions.

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes FreeRTOS without
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
#include <stdarg.h>
#include <lib/vsnprintf.h>

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
 * @brief Function convert arguments to stream
 *
 * @param[in] *buf           buffer for stream
 * @param[in]  size          buffer size
 * @param[in] *format        message format
 * @param[in]  arg           argument list
 *
 * @return number of printed characters
 *
 * Supported flags:
 *   %%         - print % character
 *                printf("%%"); => %
 *
 *   %c         - print selected character (the \0 character is skipped)
 *                printf("_%c_", 'x');  => _x_
 *                printf("_%c_", '\0'); => __
 *
 *   %s         - print selected string
 *                printf("%s", "Foobar"); => Foobar
 *
 *   %.*s       - print selected string but only the length passed by argument
 *                printf("%.*s\n", 3, "Foobar"); => Foo
 *
 *   %.ns       - print selected string but only the n length
 *                printf("%.3s\n", "Foobar"); => Foo
 *
 *   %d, %i     - print decimal integer values
 *                printf("%d, %i", -5, 10); => -5, 10
 *
 *   %u         - print unsigned decimal integer values
 *                printf("%u, %u", -1, 10); => 4294967295, 10
 *
 *   %x, %X     - print hexadecimal values ('x' for lower characters, 'X' for upper characters)
 *                printf("0x%x, 0x%X", 0x5A, 0xfa); => 0x5a, 0xFA
 *
 *   %0x?       - print decimal (d, i, u) or hex (x, X) values with leading zeros.
 *                The number of characters (at least) is determined by x. The ?
 *                means d, i, u, x, or X value representations.
 *                printf("0x02X, 0x03X", 0x5, 0x1F43); => 0x05, 0x1F43
 *
 *   %f         - print float number. Note: make sure that input value is the float!
 *                printf("Foobar: %f", 1.0); => Foobar: 1.000000
 *
 *   %l?        - print long long values, where ? means d, i, u, x, or X.
 *                NOTE: not supported
 *
 *   %p         - print pointer
 *                printf("Pointer: %p", main); => Pointer: 0x4028B4
 */
//==============================================================================
int vsnprintf(char *buf, size_t size, const char *format, va_list arg)
{
        return _builtinfunc(vsnprintf, buf, size, format, arg);
}

/*==============================================================================
  End of file
==============================================================================*/
