/*=========================================================================*//**
@file    string.h

@author  Daniel Zorychta

@brief

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _STRING_H_
#define _STRING_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "core/printx.h"

#include "_ansi.h"

#define __need_size_t
#include <stddef.h>

/*==============================================================================
  Exported macros
==============================================================================*/
#ifndef _PTR
#define _PTR    void *
#endif

#ifndef NULL
#define NULL 0
#endif

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
//==============================================================================
/**
 * @brief void *memchr(const void *s, int c, size_t n)
 * The memchr() function scans the initial <i>n</i> bytes of the memory area
 * pointed to by <i>s</i> for the first instance of <i>c</i>.  Both <i>c</i>
 * and the bytes of the memory area pointed to by <i>s</i> are interpreted
 * as unsigned char.
 *
 * @param s         memory to search
 * @param c         value to search
 * @param n         memory size
 *
 * @errors None
 *
 * @return The memchr() function return a pointer to the matching byte
 * or NULL if the character does not occur in the given memory area.
 *
 * @example
 * // ...
 * int buffer[100];
 * // ...
 * int *ptr = memchr(buffer, 12, 100);
 * // ...
 */
//==============================================================================
extern _PTR memchr(const _PTR s, int c, size_t n);

//==============================================================================
/**
 * @brief int memcmp(const void *s1, const void *s2, size_t n)
 *  The memcmp() function compares the first <i>n</i> bytes (each interpreted as
 *  unsigned char) of the memory areas <i>s1</i> and <i>s2</i>.
 *
 * @param s1        memory area 1
 * @param s2        memory area 2
 * @param n         memory size
 *
 * @errors None
 *
 * @return The memcmp() function returns an integer less than, equal to, or
 * greater than zero if the first <i>n</i> bytes of <i>s1</i> is found, respectively,
 * to be less than, to match, or be greater than the first <i>n</i> bytes of <i>s2</i>.<p>
 *
 * For a nonzero return value, the sign is determined by the sign of the
 * difference between the first pair of bytes (interpreted as unsigned
 * char) that differ in <i>s1</i> and <i>s2</i>.
 *
 * @example
 * // ...
 * int buffer1[100];
 * int buffer2[100];
 * // ...
 * if (memcmp(buffer1, buffer2, 100) == 0) {
 *         // buffer are equal
 * }
 * // ...
 */
//==============================================================================
extern int memcmp(const _PTR s1, const _PTR s2, size_t n);

//==============================================================================
/**
 * @brief void *memcpy(void *dest, const void * src, size_t n)
 *  The memcpy() function copies <i>n</i> bytes from memory area <i>src</i> to
 *  memory area <i>dest</i>. The memory areas must not overlap. Use memmove()
 *  if the memory areas do overlap.
 *
 * @param dest      destination memory area
 * @param src       source memory area
 * @param n         memory size
 *
 * @errors None
 *
 * @return The memcpy() function returns a pointer to <i>dest</i>.
 *
 * @example
 * // ...
 * int buffer1[100];
 * int buffer2[100];
 * // ...
 * memcpy(buffer1, buffer2, 100);
 * // ...
 */
//==============================================================================
extern _PTR memcpy(_PTR dest, const _PTR src, size_t n);

//==============================================================================
/**
 * @brief void *memmove(void *dest, const void *src, size_t n)
 * The memmove() function copies <i>n</i> bytes from memory area <i>src</i> to
 * memory area <i>dest</i>. The memory areas may overlap: copying takes place as
 * though the bytes in <i>src</i> are first copied into a temporary array that
 * does not overlap <i>src</i> or <i>dest</i>, and the bytes are then copied
 * from the temporary array to <i>dest</i>.
 *
 * @param dest      destination memory area
 * @param src       source memory area
 * @param n         memory size
 *
 * @errors None
 *
 * @return The memmove() function returns a pointer to <i>dest</i>.
 *
 * @example
 * // ...
 * int buffer1[100];
 * int buffer2[100];
 * // ...
 * memmove(buffer1, buffer2, 100);
 * // ...
 */
//==============================================================================
extern _PTR memmove(_PTR dest, const _PTR src, size_t n);

//==============================================================================
/**
 * @brief void *memset(void *s, int c, size_t n)
 * The memset() function fills the first <i>n</i> bytes of the memory area
 * pointed to by <i>s</i> with the constant byte <i>c</i>.
 *
 * @param s         memory area
 * @param c         value to set
 * @param n         memory size
 *
 * @errors None
 *
 * @return The memset() function returns a pointer to the memory area <i>s</i>.
 *
 * @example
 * // ...
 * int buffer[100];
 * memset(buffer, 0, 100);
 * // ...
 */
//==============================================================================
extern _PTR memset(_PTR s, int c, size_t n);

//==============================================================================
/**
 * @brief char *strcat(char *dest, const char *src)
 * The strcat() function appends the <i>src</i> string to the <i>dest</i> string,
 * overwriting the terminating null byte ('\0') at the end of <i>dest</i>, and
 * then adds a terminating null byte.  The strings may not overlap, and the
 * <i>dest</i> string must have enough space for the result.  If <i>dest</i> is
 * not large enough, program behavior is unpredictable; buffer overruns are a
 * favorite avenue for attacking secure programs.
 *
 * @param dest      destination string
 * @param src       source string
 *
 * @errors None
 *
 * @return The strcat() function return a pointer to the resulting string <i>dest</i>.
 *
 * @example
 * // ...
 * char       *str   = calloc(1, 100);
 * const char *str_a = "string 1";
 * const char *str_b = "string 2";
 * strcat(str, str_a);
 * strcat(str, str_b);
 *
 * // result: str == "string 1string 2"
 * // ...
 */
//==============================================================================
extern char *strcat(char *dest, const char *src);

//==============================================================================
/**
 * @brief char *strncat(char *dest, const char *src, size_t n)
 * The strncat() function is similar to strcat(), except that:<p>
 *
 * <ul>
 *     <li>it will use at most n bytes from src; and
 *     <li><i>src</i> does not need to be null-terminated if it contains
 *                    <i>n</i> or more bytes.
 * </ul>
 *
 * As with strcat(), the resulting string in <i>dest</i> is always null-terminated.<p>
 *
 * If <i>src</i> contains <i>n</i> or more bytes, strncat() writes <i>n+1</i>
 * bytes to <i>dest</i> (<i>n</i> from <i>src</i> plus the terminating null
 * byte). Therefore, the size of <i>dest</i> must be at least <i>strlen(dest) + n + 1</i>.
 *
 * @param dest      destination string
 * @param src       source string
 *
 * @errors None
 *
 * @return The strncat() function return a pointer to the resulting string <i>dest</i>.
 *
 * @example
 * // ...
 * char       *str   = calloc(1, 100);
 * const char *str_a = "string 1";
 * const char *str_b = "string 2";
 * strncat(str, str_a, 50);
 * strncat(str, str_b, 50);
 *
 * // result: str == "string 1string 2"
 * // ...
 */
//==============================================================================
extern char *strncat(char *, const char *, size_t);

//==============================================================================
/**
 * @brief char *strchr(const char *s, int c)
 * The strchr() function returns a pointer to the first occurrence of
 * the character <i>c</i> in the string <i>s</i>.
 *
 * @param s         string
 * @param c         character to find
 *
 * @errors None
 *
 * @return The strchr() function return a pointer to the matched
 * character or NULL if the character is not found. The terminating
 * null byte is considered part of the string, so that if <i>c</i> is specified
 * as '\0', these functions return a pointer to the terminator.
 *
 * @example
 * // ...
 * const char *str = "/foo/bar";
 * char       *ptr;
 * if ((ptr = strchr(str, '/'))) {
 *         // first '/' character found ...
 * }
 * // ...
 */
//==============================================================================
extern char *strchr(const char *s, int c);

//==============================================================================
/**
 * @brief char *strrchr(const char *s, int c)
 * The strrchr() function returns a pointer to the last occurrence of
 * the character <i>c</i> in the string <i>s</i>.
 *
 * @param s         string
 * @param c         character to find
 *
 * @errors None
 *
 * @return The strrchr() function return a pointer to the matched
 * character or NULL if the character is not found. The terminating
 * null byte is considered part of the string, so that if <i>c</i> is specified
 * as '\0', these functions return a pointer to the terminator.
 *
 * @example
 * // ...
 * const char *str = "/foo/bar";
 * char       *ptr;
 * if ((ptr = strrchr(str, '/'))) {
 *         // last '/' character found ...
 * }
 * // ...
 */
//==============================================================================
extern char *strrchr(const char *s, int c);

//==============================================================================
/**
 * @brief int strcmp(const char *s1, const char *s2)
 * The strcmp() function compares the two strings <i>s1</i> and <i>s2</i>.
 * It returns an integer less than, equal to, or greater than zero if <i>s1</i>
 * is found, respectively, to be less than, to match, or be greater than <i>s2</i>.
 *
 * @param s1        string 1
 * @param s2        string 2
 *
 * @errors None
 *
 * @return The strcmp() function return an integer less than, equal to, or
 * greater than zero if <i>s1</i> is found, respectively, to be less than, to
 * match, or be greater than <i>s2</i>.
 *
 * @example
 * // ...
 * const char *str1 = "/foo/bar1";
 * const char *str2 = "/foo/bar2";
 * if (strcmp(str1, str2) == 0) {
 *         // strings are equal
 * } else {
 *         // strings are not equal
 * }
 * // ...
 */
//==============================================================================
extern int strcmp(const char *s1, const char *s2);

//==============================================================================
/**
 * @brief int strncmp(const char *s1, const char *s2, size_t n)
 * The strncmp() function is similar to strcmp(), except it compares the only
 * first (at most) <i>n</i> bytes of <i>s1</i> and <i>s2</i>.
 *
 * @param s1        string 1
 * @param s2        string 2
 * @param n         max strings size
 *
 * @errors None
 *
 * @return The strncmp() function return an integer less than, equal to, or
 * greater than zero if <i>s1</i> is found, respectively, to be less than, to
 * match, or be greater than <i>s2</i>.
 *
 * @example
 * // ...
 * const char *str1 = "/foo/bar1";
 * const char *str2 = "/foo/bar2";
 * if (strncmp(str1, str2, 4) == 0) {
 *         // strings are equal
 * } else {
 *         // strings are not equal
 * }
 * // ...
 */
//==============================================================================
extern int strncmp(const char *s1, const char *s2, size_t n);


extern int strcoll(const char *, const char *);
extern char *strcpy(char *, const char *);
extern size_t strcspn(const char *, const char *);
extern size_t strlen(const char *);
extern char *strncpy(char *, const char *, size_t);
extern char *strpbrk(const char *, const char *);
extern size_t strspn(const char *, const char *);
extern char *strstr(const char *, const char *);

#ifndef _REENT_ONLY
extern char *strtok(char *, const char *);
#endif

#ifndef __STRICT_ANSI__
extern int strcasecmp(const char *, const char *);
extern int strncasecmp(const char *, const char *, size_t);
extern size_t strnlen(const char *, size_t);
extern char *strlwr(char *);
extern char *strupr(char *);
#endif /* ! __STRICT_ANSI__ */

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief char *strerror(int errnum)
 * The strerror() function returns a pointer to a string that describes the
 * error code passed in the argument <i>errnum</i>.
 *
 * @param errnum        error number
 *
 * @errors None
 *
 * @return Functions return the appropriate error description string.
 *
 * @example
 * // ...
 * puts(strerror(errno));
 * // ...
 */
//==============================================================================
static inline char *strerror(int errnum)
{
        return (char *)sys_strerror(errnum);
}

#ifdef __cplusplus
}
#endif

#endif /* _STRING_H_ */
/*==============================================================================
  End of file
==============================================================================*/
