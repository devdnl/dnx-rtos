/*==============================================================================
File     ctype.h

Author   Daniel Zorychta

Brief    The library provides character classification routines.

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

/**
\defgroup ctype-h <ctype.h>

The library provides character classification routines.

*/
/**@{*/

#ifndef _CTYPE_H_
#define _CTYPE_H_

/*==============================================================================
  Include files
==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Flags representing
 */
enum ctype_flags {
  _ISupper  = (1 << 0),        /*!< UPPERCASE flag.*/
  _ISlower  = (1 << 1),        /*!< lowercase flag.*/
  _ISalpha  = (1 << 2),        /*!< Alphabetic flag.*/
  _ISdigit  = (1 << 3),        /*!< Numeric flag.*/
  _ISxdigit = (1 << 4),        /*!< Hexadecimal numeric flag.*/
  _ISspace  = (1 << 5),        /*!< Whitespace flag.*/
  _ISprint  = (1 << 6),        /*!< Printing flag.*/
  _ISgraph  = (1 << 7),        /*!< Graphical flag.*/
  _ISblank  = (1 << 8),        /*!< Blank (usually SPC and TAB) flag.*/
  _IScntrl  = (1 << 9),        /*!< Control character flag.*/
  _ISpunct  = (1 << 10),       /*!< Punctuation flag.*/
  _ISalnum  = (1 << 11)        /*!< Alphanumeric flag.*/
};

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief Function checks whether c is a 7-bit unsigned char value that fits
 *        into the ASCII character set.
 *
 * Function checks whether c is a 7-bit unsigned char value that fits into the
 * ASCII character set.
 *
 * @param c             character
 *
 * @return Non-zero value when character fits into the ASCII character set.
 */
//==============================================================================
static inline int isascii(int c)
{
        return c < 128;
}

//==============================================================================
/**
 * @brief Function checks for a blank character; that is, a space or a tab.
 *
 * Function checks for a blank character; that is, a space or a tab.
 *
 * @param c             character
 *
 * @return Non-zero value when character is blank.
 */
//==============================================================================
static inline int isblank(int c)
{
        return c == ' ' || c == '\t' ? _ISblank : 0;
}


//==============================================================================
/**
 * @brief Function checks for a control character.
 *
 * Function checks for a control character.
 *
 * @param c             character
 *
 * @return Non-zero value when character is control.
 */
//==============================================================================
static inline int iscntrl(int c)
{
        return c < 32 ? _IScntrl : 0;
}

//==============================================================================
/**
 * @brief Function checks for a digit (0 through 9).
 *
 * Function checks for a digit (0 through 9).
 *
 * @param c             character
 *
 * @return Non-zero value when character is digit.
 */
//==============================================================================
static inline int isdigit(int c)
{
        return c >= '0' && c <= '9' ? _ISdigit : 0;
}


//==============================================================================
/**
 * @brief Function checks for any printable character except space.
 *
 * Function checks for any printable character except space.
 *
 * @param c             character
 *
 * @return Non-zero value when character is printable.
 */
//==============================================================================
static inline int isgraph(int c)
{
        return c >= 33 && c <= 126 ? _ISgraph : 0;
}

//==============================================================================
/**
 * @brief Function checks for a lower-case character.
 *
 * Function checks for a lower-case character.
 *
 * @param c             character
 *
 * @return Non-zero value when character is lower case.
 */
//==============================================================================
static inline int islower(int c)
{
        return c >= 'a' && c <= 'z' ? _ISlower : 0;
}

//==============================================================================
/**
 * @brief Function checks for an uppercase letter.
 *
 * Function checks for an uppercase letter.
 *
 * @param c             character
 *
 * @return Non-zero value when character is upper case.
 */
//==============================================================================
static inline int isupper(int c)
{
        return c >= 'A' && c <= 'Z' ? _ISupper : 0;
}

//==============================================================================
/**
 * @brief Function v
 *
 * Function checks for any printable character including space.
 *
 * @param c             character
 *
 * @return Non-zero value when character is printable including space.
 */
//==============================================================================
static inline int isprint(int c)
{
        return c >= 32 && c <= 126 ? _ISprint : 0;
}

//==============================================================================
/**
 * @brief Function checks for any printable character which is not a space or
 *        an alphanumeric character.
 *
 * Function checks for any printable character which is not a space or
 * an alphanumeric character.
 *
 * @param c             character
 *
 * @return Non-zero value when character is printable and is not alphanumeric.
 */
//==============================================================================
static inline int ispunct(int c)
{
        return (c >= 33 && c <= 47) || (c >= 58  && c <= 64 )
            || (c >= 91 && c <= 96) || (c >= 123 && c <= 126) ? _ISpunct : 0;
}

//==============================================================================
/**
 * @brief Function checks for white-space characters.
 *
 * Function checks for white-space characters. In the "C" and "POSIX" locales,
 * these are: space, form-feed ('\\f'), newline ('\\n'), carriage return ('\\r'),
 * horizontal tab ('\\t'), and vertical tab ('\\v').
 *
 * @param c             character
 *
 * @return Non-zero value when character is space (any).
 */
//==============================================================================
static inline int isspace(int c)
{
        return (c >= 9 && c <= 13) || c == 32 ? _ISspace : 0;
}

//==============================================================================
/**
 * @brief Function checks for a hexadecimal digits.
 *
 * Function checks for a hexadecimal digits, that is, one of:
 * <b>0 1 2 3 4 5 6 7 8 9 a b c d e f A B C D E F</b>.
 *
 * @param c             character
 *
 * @return Non-zero value when character is hexadecimal digit.
 */
//==============================================================================
static inline int isxdigit(int c)
{
        return isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') ? _ISxdigit : 0;
}

//==============================================================================
/**
 * @brief Function converts the letter c to upper case, if possible.
 *
 * Function converts the letter c to upper case, if possible.
 * If c is not an unsigned char value, or EOF, the behavior of function is undefined.
 *
 * @param c             character
 *
 * @return The value returned is that of the converted letter, or c if the
 *         conversion was not possible.
 */
//==============================================================================
static inline int toupper(int c)
{
        return (c >= 'a' && c <= 'z') ? c - 32 : c;
}

//==============================================================================
/**
 * @brief Function converts the letter c to lower case, if possible.
 *
 * Function converts the letter c to lower case, if possible.
 * If c is not an unsigned char value, or EOF, the behavior of function is undefined.
 *
 * @param c             character
 *
 * @return The value returned is that of the converted letter, or c if the
 *         conversion was not possible.
 */
//==============================================================================
static inline int tolower(int c)
{
        return (c >= 'A' && c <= 'Z') ? c + 32 : c;
}

//==============================================================================
/**
 * @brief Function checks for an alphabetic character.
 *
 * Function checks for an alphabetic character; in the standard "C" locale,
 * it is equivalent to (isupper(c) || islower(c)). In some locales, there may
 * be additional characters for which isalpha() is true-letters which are
 * neither upper case nor lower case.
 *
 * @param c             character
 *
 * @return Non-zero value when character is alphanumeric.
 */
//==============================================================================
static inline int isalpha(int c)
{
        return isupper(c) || islower(c);
}

//==============================================================================
/**
 * @brief Function checks for an alphanumeric character.
 *
 * Function checks for an alphanumeric character; it is equivalent to
 * (isalpha(c) || isdigit(c)).
 *
 * @param c             character
 *
 * @return Non-zero value when character is alphanumeric.
 */
//==============================================================================
static inline int isalnum(int c)
{
        return isalpha(c) || isdigit(c);
}

#ifdef __cplusplus
}
#endif

#endif /* _CTYPE_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
