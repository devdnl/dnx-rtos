/*==============================================================================
File     locale.h

Author   Daniel Zorychta

Brief    The library provides location specific settings.

         Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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


==============================================================================*/

/**
\defgroup locale-h <locale.h>

The library provides location specific settings.

@note Library is added only in compatibility purposes. Library in the dnx RTOS
      is not fully supported.
*/
/**@{*/

#ifndef _LOCALE_H_
#define _LOCALE_H_

/*==============================================================================
  Include files
==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 * Affects all character functions.
 */
#define LC_CTYPE        0

/**
 * Affects decimal-point formatting and the information provided by localeconv() function.
 */
#define LC_NUMERIC      1

/**
 * Affects the strftime function.
 */
#define LC_TIME         2

/**
 * Affects strcoll and strxfrm functions.
 */
#define LC_COLLATE      3

/**
 * Affects the monetary information provided by localeconv function.
 */
#define LC_MONETARY     4

/**
 * Sets everything.
 */
#define LC_ALL          6

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Type represent the location specific settings.
 */
typedef struct lconv {
        /**
        Decimal point character used for non-monetary values.
        */
        char *decimal_point;

        /**
        Thousands place separator character used for non-monetary values.
        */
        char *thousands_sep;

        /**
        A string that indicates the size of each group of digits in
        non-monetary quantities. Each character represents an integer value,
        which designates the number of digits in the current group. A value
        of 0 means that the previous value is to be used for the rest of the
        groups.
        */
        char *grouping;

        /**
        It is a string of the international currency symbols used. The first
        three characters are those specified by ISO 4217:1987 and the fourth
        is the character, which separates the currency symbol from the
        monetary quantity.
        */
        char *int_curr_symbol;

        /**
        The local symbol used for currency.
        */
        char *currency_symbol;

        /**
        The decimal point character used for monetary values.
        */
        char *mon_decimal_point;

        /**
        The thousands place grouping character used for monetary values.
        */
        char *mon_thousands_sep;

        /**
        A string whose elements defines the size of the grouping of digits in
        monetary values. Each character represents an integer value which
        designates the number of digits in the current group. A value of 0 means
        that the previous value is to be used for the rest of the groups.
        */
        char *mon_grouping;

        /**
        The character used for positive monetary values.
        */
        char *positive_sign;

        /**
        The character used for negative monetary values.
        */
        char *negative_sign;

        /**
        Number of digits to show after the decimal point in international
        monetary values.
        */
        char int_frac_digits;

        /**
        Number of digits to show after the decimal point in monetary values.
        */
        char frac_digits;

        /**
        If equals to 1, then the currency_symbol appears before a positive
        monetary value. If equals to 0, then the currency_symbol appears after
        a positive monetary value.
        */
        char p_cs_precedes;

        /**
        If equals to 1, then the currency_symbol is separated by a space from
        a positive monetary value. If equals to 0, then there is no space
        between the currency_symbol and a positive monetary value.
        */
        char p_sep_by_space;

        /**
        If equals to 1, then the currency_symbol precedes a negative monetary
        value. If equals to 0, then the currency_symbol succeeds a negative
        monetary value.
        */
        char n_cs_precedes;

        /**
        If equals to 1, then the currency_symbol is separated by a space from
        a negative monetary value. If equals to 0, then there is no space
        between the currency_symbol and a negative monetary value.
        */
        char n_sep_by_space;

        /**
        Represents the position of the positive_sign in a positive monetary
        value.
        */
        char p_sign_posn;

        /**
        Represents the position of the negative_sign in a negative monetary
        value.
        */
        char n_sign_posn;
} lconv;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
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
extern char *setlocale(int category, const char *locale);

//==============================================================================
/**
 * @brief  Function get locale formatting parameters for quantities.
 *
 * Retrieves the values provided in the current locale object to format
 * parameters for quantities. These are returned in an object of type
 * struct lconv.
 *
 * @return On success, a pointer to a C string identifying the locale currently
 *         set for the category.
 */
//==============================================================================
extern struct lconv *localeconv(void);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _LOCALE_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
