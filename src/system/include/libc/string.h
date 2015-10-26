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
#include <_ansi.h>

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
 * The <b>memchr</b>() function scans the initial <i>n</i> bytes of the memory area
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
 * @return The <b>memchr</b>() function return a pointer to the matching byte
 * or <b>NULL</b> if the character does not occur in the given memory area.
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
 *  The <b>memcmp</b>() function compares the first <i>n</i> bytes (each interpreted as
 *  unsigned char) of the memory areas <i>s1</i> and <i>s2</i>.
 *
 * @param s1        memory area 1
 * @param s2        memory area 2
 * @param n         memory size
 *
 * @errors None
 *
 * @return The <b>memcmp</b>() function returns an integer less than, equal to, or
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
 *         // buffers are equal
 * } else {
 *         // buffers are not equal
 * }
 * // ...
 */
//==============================================================================
extern int memcmp(const _PTR s1, const _PTR s2, size_t n);

//==============================================================================
/**
 * @brief void *memcpy(void *dest, const void * src, size_t n)
 *  The <b>memcpy</b>() function copies <i>n</i> bytes from memory area <i>src</i> to
 *  memory area <i>dest</i>. The memory areas must not overlap. Use <b>memmove</b>()
 *  if the memory areas do overlap.
 *
 * @param dest      destination memory area
 * @param src       source memory area
 * @param n         memory size
 *
 * @errors None
 *
 * @return The <b>memcpy</b>() function returns a pointer to <i>dest</i>.
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
 * The <b>memmove</b>() function copies <i>n</i> bytes from memory area <i>src</i> to
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
 * @return The <b>memmove</b>() function returns a pointer to <i>dest</i>.
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
 * The <b>memset</b>() function fills the first <i>n</i> bytes of the memory area
 * pointed to by <i>s</i> with the constant byte <i>c</i>.
 *
 * @param s         memory area
 * @param c         value to set
 * @param n         memory size
 *
 * @errors None
 *
 * @return The <b>memset</b>() function returns a pointer to the memory area <i>s</i>.
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
 * The <b>strcat</b>() function appends the <i>src</i> string to the <i>dest</i> string,
 * overwriting the terminating <b>NULL</b> byte ('\0') at the end of <i>dest</i>, and
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
 * @return The <b>strcat</b>() function return a pointer to the resulting string <i>dest</i>.
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
 * The <b>strncat</b>() function is similar to <b>strcat</b>(), except that:<p>
 *
 * <ul>
 *     <li>it will use at most n bytes from src; and
 *     <li><i>src</i> does not need to be null-terminated if it contains
 *                    <i>n</i> or more bytes.
 * </ul>
 *
 * As with <b>strcat</b>(), the resulting string in <i>dest</i> is always null-terminated.<p>
 *
 * If <i>src</i> contains <i>n</i> or more bytes, <b>strncat</b>() writes <i>n+1</i>
 * bytes to <i>dest</i> (<i>n</i> from <i>src</i> plus the terminating null
 * byte). Therefore, the size of <i>dest</i> must be at least <i>strlen(dest) + n + 1</i>.
 *
 * @param dest      destination string
 * @param src       source string
 *
 * @errors None
 *
 * @return The <b>strncat</b>() function return a pointer to the resulting string <i>dest</i>.
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
 * The <b>strchr</b>() function returns a pointer to the first occurrence of
 * the character <i>c</i> in the string <i>s</i>.
 *
 * @param s         string
 * @param c         character to find
 *
 * @errors None
 *
 * @return The <b>strchr</b>() function return a pointer to the matched
 * character or <b>NULL</b> if the character is not found. The terminating
 * null byte is considered part of the string, so that if <i>c</i> is specified
 * as '\0', these functions return a pointer to the terminator.
 *
 * @example
 * // ...
 * const char *str = "/foo/bar";
 * char       *ptr;
 * if ((ptr = strchr(str, '/'))) {
 *         // the first '/' character found ...
 * }
 * // ...
 */
//==============================================================================
extern char *strchr(const char *s, int c);

//==============================================================================
/**
 * @brief char *strrchr(const char *s, int c)
 * The <b>strrchr</b>() function returns a pointer to the last occurrence of
 * the character <i>c</i> in the string <i>s</i>.
 *
 * @param s         string
 * @param c         character to find
 *
 * @errors None
 *
 * @return The <b>strrchr</b>() function return a pointer to the matched
 * character or <b>NULL</b> if the character is not found. The terminating
 * null byte is considered part of the string, so that if <i>c</i> is specified
 * as '\0', these functions return a pointer to the terminator.
 *
 * @example
 * // ...
 * const char *str = "/foo/bar";
 * char       *ptr;
 * if ((ptr = strrchr(str, '/'))) {
 *         // the last '/' character found ...
 * }
 * // ...
 */
//==============================================================================
extern char *strrchr(const char *s, int c);

//==============================================================================
/**
 * @brief int strcmp(const char *s1, const char *s2)
 * The <b>strcmp</b>() function compares the two strings <i>s1</i> and <i>s2</i>.
 * It returns an integer less than, equal to, or greater than zero if <i>s1</i>
 * is found, respectively, to be less than, to match, or be greater than <i>s2</i>.
 *
 * @param s1        string 1
 * @param s2        string 2
 *
 * @errors None
 *
 * @return The <b>strcmp</b>() function return an integer less than, equal to, or
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
 * The <b>strncmp</b>() function is similar to <b>strcmp</b>(), except it compares the only
 * first (at most) <i>n</i> bytes of <i>s1</i> and <i>s2</i>.
 *
 * @param s1        string 1
 * @param s2        string 2
 * @param n         max strings size
 *
 * @errors None
 *
 * @return The <b>strncmp</b>() function return an integer less than, equal to, or
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

//==============================================================================
/**
 * @brief int strcoll(const char *s1, const char *s2)
 * The <b>strcoll</b>() function compares the two strings <i>s1</i> and <i>s2</i>.  It
 * returns an integer less than, equal to, or greater than zero if <i>s1</i> is
 * found, respectively, to be less than, to match, or be greater than
 * <i>s2</i>.
 *
 * @param s1        string 1
 * @param s2        string 2
 *
 * @errors None
 *
 * @return The <b>strcoll</b>() function returns an integer less than, equal to, or
 * greater than zero if <i>s1</i> is found, respectively, to be less than, to
 * match, or be greater than <i>s2</i>, when both are interpreted as
 * appropriate for the current locale.
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
extern int strcoll(const char *s1, const char *s2);

//==============================================================================
/**
 * @brief char *strcpy(char *dest, const char *src)
 * The <b>strcpy</b>() function copies the string pointed to by <i>src</i>, including
 * the terminating null byte ('\0'), to the buffer pointed to by <i>dest</i>.
 * The strings may not overlap, and the destination string dest must be
 * large enough to receive the copy.
 *
 * @param dest      destination
 * @param src       source
 *
 * @errors None
 *
 * @return The <b>strcpy</b>() function return a pointer to the destination string
 * <i>dest</i>.
 *
 * @example
 * // ...
 * char *str1 = malloc(100);
 * if (str1) {
 *         strcpy(str1, "Foo bar");
 *
 *         // ...
 * }
 *
 * // ...
 */
//==============================================================================
extern char *strcpy(char *dest, const char *src);

//==============================================================================
/**
 * @brief char *strncpy(char *s1, const char *s2, size_t n)
 * The <b>strncpy</b>() function is similar to <b>strcpy</b>(), except that at most <i>n</i>
 * bytes of <i>src</i> are copied.  Warning: If there is no null byte among the
 * first <i>n</i> bytes of <i>src</i>, the string placed in <i>dest</i> will
 * not be null-terminated.
 *
 * If the length of <i>src</i> is less than <i>n</i>, <b>strncpy</b>() writes
 * additional null bytes to <i>dest</i> to ensure that a total of <i>n</i> bytes
 * are written.
 *
 * @param dest      destination
 * @param src       source
 * @param n         max string length
 *
 * @errors None
 *
 * @return The <b>strncpy</b>() function return a pointer to the destination string
 * <i>dest</i>.
 *
 * @example
 * // ...
 * char *str1 = calloc(1, 100);
 * if (str1) {
 *         strncpy(str1, "Foo bar", 99);
 *
 *         // ...
 * }
 *
 * // ...
 */
//==============================================================================
extern char *strncpy(char *s1, const char *s2, size_t n);

//==============================================================================
/**
 * @brief size_t strspn(const char *s, const char *accept)
 * The <b>strspn</b>() function calculates the length (in bytes) of the initial
 * segment of <i>s</i> which consists entirely of bytes in <i>accept</i>.
 *
 * @param s         string
 * @param accept    string of characters to accept
 *
 * @errors None
 *
 * @return The <b>strspn</b>() function returns the number of bytes in the initial
 * segment of <i>s</i> which consist only of bytes from <i>accept</i>.
 *
 * @example
 * // ...
 * const char *foo = "/foo/bar";
 * size_t      accepted_chars = strspn(foo, "/fo");
 * // accepted_chars = 5
 * // ...
 */
//==============================================================================
extern size_t strspn(const char *s, const char *accept);

//==============================================================================
/**
 * @brief size_t strcspn(const char *s, const char *reject)
 * The <b>strcspn</b>() function calculates the length of the initial segment
 * of <i>s</i> which consists entirely of bytes not in <i>reject</i>.
 *
 * @param s         string
 * @param reject    string of characters to reject
 *
 * @errors None
 *
 * @return The <b>strcspn</b>() function returns the number of bytes in the initial
 * segment of <i>s</i> which are not in the string <i>reject</i>.
 *
 * @example
 * // ...
 * const char *foo = "/foo/bar";
 * size_t      rejected_chars = strcspn(foo, "ar");
 * // rejected_chars = 6 (stops on 'a')
 * // ...
 */
//==============================================================================
extern size_t strcspn(const char *s, const char *reject);

//==============================================================================
/**
 * @brief size_t strlen(const char *s)
 * The <b>strlen</b>() function calculates the length of the string <i>s</i>,
 * excluding the terminating null byte ('\0').
 *
 * @param s         string
 *
 * @errors None
 *
 * @return The <b>strlen</b>() function returns the number of bytes in the string <i>s</i>.
 *
 * @example
 * // ...
 * const char *foo = "/foo/bar";
 * size_t      len = strlen(foo);
 * // ...
 */
//==============================================================================
extern size_t strlen(const char *s);

//==============================================================================
/**
 * @brief char *strpbrk(const char *s, const char *accept)
 * The <b>strpbrk</b>() function locates the first occurrence in the string <i>s</i>
 * of any of the bytes in the string <i>accept</i>.
 *
 * @param s         string
 * @param accept    characters to find
 *
 * @errors None
 *
 * @return The <b>strpbrk</b>() function returns a pointer to the byte in <i>s</i> that
 * matches one of the bytes in <i>accept</i>, or <b>NULL</b> if no such byte is found.
 *
 * @example
 * // ...
 * const char *foo = "/foo/bar";
 * char       *loc = strpbrk(foo, "ab");
 *
 * // loc points to 'a' character
 * // ...
 */
//==============================================================================
extern char *strpbrk(const char *s, const char *accept);

//==============================================================================
/**
 * @brief char *strstr(const char *haystack, const char *needle)
 * The <b>strstr</b>() function finds the first occurrence of the substring
 * <i>needle</i> in the string <i>haystack</i>.  The terminating null bytes
 * ('\0') are not compared.
 *
 * @param s         string
 * @param accept    characters to find
 *
 * @errors None
 *
 * @return The function return a pointer to the beginning of the located
 * substring, or <b>NULL</b> if the substring is not found.
 *
 * @example
 * // ...
 * const char *foo = "this is example string";
 * char       *loc = strstr(foo, "is example");
 *
 * // loc points to "is example string"
 * // ...
 */
//==============================================================================
extern char *strstr(const char *haystack, const char *needle);

//==============================================================================
/**
 * @brief char *strcasestr(const char *haystack, const char *needle)
 * The <b>strcasestr</b>() function is like <b>strstr</b>(), but ignores the case of
 * both arguments.
 *
 * @param s         string
 * @param accept    characters to find
 *
 * @errors None
 *
 * @return The function return a pointer to the beginning of the located
 * substring, or <b>NULL</b> if the substring is not found.
 *
 * @example
 * // ...
 * const char *foo = "this Is Example string";
 * char       *loc = strcasestr(foo, "is example");
 *
 * // loc points to "Is Example string"
 * // ...
 */
//==============================================================================
extern char *strcasestr(const char *haystack, const char *needle);

//==============================================================================
/**
 * @brief int strcasecmp(const char *s1, const char *s2)
 * The <b>strcasecmp</b>() function compares the two strings <i>s1</i> and <i>s2</i>,
 * ignoring the case of the characters.  It returns an integer less
 * than, equal to, or greater than zero if <i>s1</i> is found, respectively, to
 * be less than, to match, or be greater than <i>s2</i>.
 *
 * @param s1        string 1
 * @param s2        string 2
 *
 * @errors None
 *
 * @return The <b>strcasecmp</b>() function return an integer less than, equal to, or
 * greater than zero if <i>s1</i> is found, respectively, to be less than, to
 * match, or be greater than <i>s2</i>.
 *
 * @example
 * // ...
 * const char *foo = "FOO";
 * const char *bar = "foo";
 *
 * if (strcasecmp(foo, bar) == 0) {
 *         // result: functions are equal
 * }
 *
 * // ...
 */
//==============================================================================
extern int strcasecmp(const char *s1, const char *s2);

//==============================================================================
/**
 * @brief int strncasecmp(const char *s1, const char *s2, size_t n)
 * The <b>strncasecmp</b>() function is similar to <b>strcasecmp</b>(), except it
 * compares the only first <i>n</i> bytes of <i>s1</i>.
 *
 * @param s1        string 1
 * @param s2        string 2
 * @param n         max number of bytes to compare
 *
 * @errors None
 *
 * @return The <b>strncasecmp</b>() function return an integer less than, equal to, or
 * greater than zero if <i>s1</i> or the first <i>n</i> bytes thereof is found,
 * respectively, to be less than, to match, or be greater than <i>s2</i>.
 *
 * @example
 * // ...
 * const char *foo = "FOO";
 * const char *bar = "foo";
 *
 * if (strncasecmp(foo, bar, 3) == 0) {
 *         // result: functions are equal
 * }
 *
 * // ...
 */
//==============================================================================
extern int strncasecmp(const char *s1, const char *s2, size_t n);

//==============================================================================
/**
 * @brief size_t strnlen(const char *s, size_t maxlen)
 * The <b>strnlen</b>() function returns the number of bytes in the string
 * pointed to by <i>s</i>, excluding the terminating null byte ('\0'), but at
 * most <i>maxlen</i>. In doing this, <b>strnlen</b>() looks only at the first
 * <i>maxlen</i> bytes at <i>s</i> and never beyond <i>s + maxlen</i>.
 *
 * @param s         string
 * @param maxlen    max string length
 *
 * @errors None
 *
 * @return The <b>strnlen</b>() function returns strlen(s), if that is less than
 * <i>maxlen</i>, or <i>maxlen</i> if there is no null byte ('\0') among the
 * first <i>maxlen</i> bytes pointed to by <i>s</i>.
 *
 * @example
 * // ...
 * const char *foo = "/foo/bar";
 * size_t      len = strnlen(foo, 10);
 * // ...
 */
//==============================================================================
extern size_t strnlen(const char *s, size_t maxlen);

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief char *strerror(int errnum)
 * The <b>strerror</b>() function returns a pointer to a string that describes the
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
extern const char *strerror(int errnum);

#ifdef __cplusplus
}
#endif

#endif /* _STRING_H_ */
/*==============================================================================
  End of file
==============================================================================*/
