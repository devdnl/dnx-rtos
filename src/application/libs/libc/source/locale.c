/*==============================================================================
File     locale.c

Author   Daniel Zorychta

Brief    The library provides location specific settings.

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
#include "locale.h"
#include "stddef.h"
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
static const struct lconv locale = {
        .decimal_point     = ".",
        .thousands_sep     = " ",
        .grouping          = NULL,
        .int_curr_symbol   = "???",
        .currency_symbol   = "?",
        .mon_decimal_point = ".",
        .mon_thousands_sep = " ",
        .mon_grouping      = NULL,
        .positive_sign     = "",
        .negative_sign     = "-",
        .int_frac_digits   = 2,
        .frac_digits       = 2,
        .p_cs_precedes     = 0,
        .p_sep_by_space    = 0,
        .n_cs_precedes     = 0,
        .n_sep_by_space    = 0,
        .p_sign_posn       = 0,
        .n_sign_posn       = 0
};

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
 * @brief  Function set locale values.
 *
 * Sets locale information to be used by the current program, either changing
 * the entire locale or portions of it. The function can also be used to
 * retrieve the current locale's name by passing NULL as the value for argument
 * locale.
 *
 * @note Function is not supported by dnx RTOS. Function always return NULL.
 *
 * @param  category     category to set
 * @param  locale       value to set
 *
 * @return On success, a pointer to a C string identifying the locale currently
 *         set for the category.
 */
//==============================================================================
char *setlocale(int category, const char *locale)
{
        UNUSED_ARG2(category, locale);
        return NULL;
}

//==============================================================================
/**
 * @brief  Function get locale formatting parameters for quantities.
 *
 * Retrieves the values provided in the current locale object to format
 * parameters for quantities. These are returned in an object of type
 * struct lconv.
 *
 * @param  category     category to set
 * @param  locale       value to set
 *
 * @return On success, a pointer to a C string identifying the locale currently
 *         set for the category.
 */
//==============================================================================
struct lconv *localeconv(void)
{
        return const_cast(&locale);
}

/*==============================================================================
  End of file
==============================================================================*/
