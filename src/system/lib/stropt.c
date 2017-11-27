/*==============================================================================
File     stropt.c

Author   Daniel Zorychta

Brief    String options analyze library.

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
#include <stdbool.h>
#include <stdint.h>
#include "lib/stropt.h"
#include "lib/strlcat.h"
#include "lib/strlcpy.h"
#include "lib/conv.h"

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
 * @brief  Function try find selected variable in opts.
 *
 * @param  opts         option line
 * @param  var          finding variable
 * @param  len          found value length
 *
 * @return Pointer to found value.
 */
//==============================================================================
const char *find_var(const char *opts, const char *var, size_t *len)
{
        char token[32];
        _strlcpy(token, var, sizeof(token));
        _strlcat(token, "=", sizeof(token));

        const char *val = NULL;

        opts = strstr(opts, token);
        if (opts) {
                val = opts + strlen(token);
                char *end = strchr(val, ',');

                if (end) {
                        *len = end - val;
                } else {
                        *len = strlen(val);
                }
        }

        return val;
}

//==============================================================================
/**
 * @brief  Function try find selected flag.
 *
 * @param  opts         option line
 * @param  flag         finding flag
 *
 * @return If flag found true is returned, false otherwise.
 */
//==============================================================================
bool find_flag(const char *opts, const char *flag)
{
        const char *b = NULL;

        do {
                 b = strstr(opts, flag);

                 if (b) {
                         const char *e = b + strlen(flag);

                         if (*e == ',' || *e == '\0') {
                                 return true;
                         }

                         opts = e;
                 }

        } while (b);

        return false;
}

//==============================================================================
/**
 * @brief  Function return integer value from given configuration.
 *
 * Function found expression: VAR=VAL.
 *
 * @param opts          options to analyse
 * @param var           variable to search
 * @param default       value used if variable does not found
 *
 * @return Integer (on error or when variable does not found the default value).
 */
//==============================================================================
int _stropt_get_int(const char *opts, const char *var, int defval)
{
        int retval = defval;

        if (opts && var) {

                size_t len = 0;
                const char *val = find_var(opts, var, &len);

                if (val && len) {
                        int32_t value = defval;
                        _strtoi(val, 0, &value);
                        retval = value;
                }
        }

        return retval;
}

//==============================================================================
/**
 * @brief  Function return integer value from given configuration.
 *
 * Function found expression: VAR=STR.
 *
 * @param opts          options to analyze
 * @param var           variable to search
 * @param len           string length
 *
 * @return On success string pointer and length passed by len pointer, otherwise NULL.
 */
//==============================================================================
const char *_stropt_get_string_ref(const char *opts, const char *var, size_t *len)
{
        const char *str = NULL;

        if (opts && var && len) {
                str = find_var(opts, var, len);
        }

        return str;
}

//==============================================================================
/**
 * @brief  Function copy string from option.
 *
 * Function found expression: VAR=STR.
 *
 * @param opts          options to analyze
 * @param var           variable to search
 * @param buf           destination buffer
 * @param buflen        destination buffer length
 *
 * @return On success string pointer and length passed by len pointer, otherwise NULL.
 */
//==============================================================================
size_t _stropt_get_string_copy(const char *opts, const char *var, char *buf, size_t buflen)
{
        size_t copied = 0;

        if (opts && var && buf && buflen) {
                size_t len = 0;
                const char *str = find_var(opts, var, &len);

                if (str && len) {
                        copied = len + 1 < buflen ? len + 1 : buflen;
                        _strlcpy(buf, str, copied);
                }
        }

        return copied;
}

//==============================================================================
/**
 * @brief  Function return integer value from given configuration.
 *
 * Function found expression: VAR=BOOL(false, true, on, off, enable, disable, yes, no).
 *
 * @param opts          options to analyse
 * @param var           variable to search
 * @param len           string length
 *
 * @return On success return 1 or 0
 */
//==============================================================================
int _stropt_get_bool(const char *opts, const char *var, int defval)
{
        int retval = defval;

        if (opts && var) {

                size_t len = 0;
                const char *val = find_var(opts, var, &len);

                if (val && len) {
                        if (  (strncmp(val, "false", 5) == 0)
                           || (strncmp(val, "off", 3) == 0)
                           || (strncmp(val, "disable", 7) == 0)
                           || (strncmp(val, "no", 2) == 0)) {

                                retval = 0;

                        } else if (  (strncmp(val, "true", 4) == 0)
                                  || (strncmp(val, "on", 2) == 0)
                                  || (strncmp(val, "enable", 6) == 0)
                                  || (strncmp(val, "yes", 3) == 0)) {

                                retval = 1;
                        }
                }
        }

        return retval;
}

//==============================================================================
/**
 * @brief  Function check if flag exist.
 *
 * @param opts          options to analyse
 * @param flag          flag to find
 *
 * @return On success return true, otherwise false.
 */
//==============================================================================
bool _stropt_is_flag(const char *opts, const char *flag)
{
        if (opts && flag) {
                return find_flag(opts, flag);
        } else {
                return false;
        }
}

/*==============================================================================
  End of file
==============================================================================*/
