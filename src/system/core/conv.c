/*=========================================================================*//**
@file    conv.c

@author  Daniel Zorychta

@brief   Module with calculation and convert functions.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Include files
==============================================================================*/
#include "core/conv.h"
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <dnx/misc.h>

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
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief Function convert ASCII to the number
 * When function find any other character than number (depended of actual base)
 * immediately finished operation and return pointer when bad character was
 * found. If base is 0 then function recognize type of number used in string.
 * For hex values "0x" is recognized, for octal values "0" at the beginning of
 * string is recognized, for binary "0b" is recognized, and for decimals values
 * none above.
 *
 * @param[in]  *string       string to decode
 * @param[in]   base         decode base
 * @param[out] *value        pointer to result
 *
 * @return pointer in string when operation was finished
 */
//==============================================================================
char *_strtoi(const char *string, int base, i32_t *value)
{
        *value = 0;

        i32_t sign = 1;
        if (string[0] == '-') {
                string++;
                sign = -1;
        } else if (string[0] == '+') {
                string++;
                sign = 1;
        }

        if (base == 0) {
                if (strncmp("0x", string, 2) == 0) {
                        base    = 16;
                        string += 2;
                } else if (strncmp("0b", string, 2) == 0) {
                        base    = 2;
                        string += 2;
                } else if (strncmp("0", string, 1) == 0) {
                        base    = 8;
                        string += 1;
                } else {
                        base = 10;
                }
        } else if (base < 2 || base > 16) {
                return const_cast(char*, string);
        }

        char  character;
        bool  char_found = false;
        while ((character = *string) != '\0') {
                /* if space exist, atoi continue finding correct character */
                if ((character == ' ') && (char_found == false)) {
                        string++;
                        continue;
                } else {
                        char_found = true;
                }

                /* check character range */
                if (character >= 'a') {
                        character -= 'a' - 10;
                } else if (character >= 'A') {
                        character -= 'A' - 10;
                } else if (character >= '0') {
                        character -= '0';
                } else {
                        break;
                }

                /* check character range according to actual base */
                if (character >= base) {
                        break;
                }

                /* compute value */
                *value = *value * base;
                *value = *value + character;

                string++;
        }

        *value *= sign;

        return const_cast(char*, string);
}

//==============================================================================
/**
 * @brief Function convert string to integer
 *
 * @param[in] *str      string
 *
 * @return converted value
 */
//==============================================================================
i32_t _atoi(const char *str)
{
        i32_t result;
        _strtoi(str, 10, &result);
        return result;
}

//==============================================================================
/**
 * @brief Function convert string to double
 *
 * @param[in]  *str             string
 * @param[out] **end            the pointer to the character when conversion was finished
 *
 * @return converted value
 */
//==============================================================================
double _strtod(const char *str, char **end)
{
        double sign    = 1;
        double div     = 1;
        double number  = 0;
        int    i       = 0;
        int    decimal = 0;
        bool   point   = false;

        while (str[i] != '\0') {
                char num = str[i];

                if (num >= '0' && num <= '9') {
                        number *= 10;
                        number += (double) (num - '0');
                } else if (num == '.' && !point) {
                        point = true;
                        i++;
                        continue;
                } else if (num == '-') {
                        sign = -1;
                        if (!isdigit((int)str[i + 1])) {
                                i = 0;
                                break;
                        }
                        i++;
                        continue;
                } else if (num == '+') {
                        if (!isdigit((int)str[i + 1])) {
                                i = 0;
                                break;
                        }
                        i++;
                        continue;
                } else if (strchr(" \n\t+", num) == NULL) {
                        break;
                }

                if (point) {
                        decimal++;
                }

                i++;
        }

        if (point) {
                for (int j = 0; j < decimal; j++) {
                        div *= 10;
                }
        }

        if (end)
                *end = (char *) &str[i];

        return sign * (number / div);
}


//==============================================================================
/**
 * @brief Function convert string to float
 *
 * @param[in] *str      string
 *
 * @return converted value
 */
//==============================================================================
double _atof(const char *str)
{
        return _strtod(str, NULL);
}

/*==============================================================================
  End of file
==============================================================================*/
