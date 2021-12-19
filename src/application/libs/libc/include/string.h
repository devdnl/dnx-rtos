/*=========================================================================*//**
@file    string.h

@author  Daniel Zorychta

@brief   String support library.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/**
\defgroup string-h <string.h>

The library provides string manipulation functions.

@{
*/

#ifndef _STRING_H_
#define _STRING_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <_ansi.h>

#ifndef DOXYGEN
#define __need_size_t
#include <stddef.h>
#include <kernel/syscall.h>
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 * @brief A value reserved for indicating that the pointer does not refer to a valid object.
 */
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
 * @brief Function scans memory area for character.
 *
 * The memchr() function scans the initial <i>n</i> bytes of the memory area
 * pointed to by <i>s</i> for the first instance of <i>c</i>.  Both <i>c</i>
 * and the bytes of the memory area pointed to by <i>s</i> are interpreted
 * as unsigned char.
 *
 * @param s         memory to search
 * @param c         value to search
 * @param n         memory size
 *
 * @return The memchr() function return a pointer to the matching byte
 * or @ref NULL if the character does not occur in the given memory area.
 *
 * @b Example
 * @code
        // ...
        int buffer[100];
        // ...
        int *ptr = memchr(buffer, 12, 100);
        // ...
   @endcode
 *
 * @see strchr(), strrchr()
 */
//==============================================================================
extern void *memchr(const void *s, int c, size_t n);

//==============================================================================
/**
 * @brief Function compares memory blocks.
 *
 *  The memcmp() function compares the first <i>n</i> bytes (each interpreted as
 *  unsigned char) of the memory areas <i>s1</i> and <i>s2</i>.
 *
 * @param s1        memory area 1
 * @param s2        memory area 2
 * @param n         memory size
 *
 * @return The memcmp() function returns an integer less than, equal to, or
 * greater than zero if the first <i>n</i> bytes of <i>s1</i> is found, respectively,
 * to be less than, to match, or be greater than the first <i>n</i> bytes of <i>s2</i>.<p>
 *
 * For a nonzero return value, the sign is determined by the sign of the
 * difference between the first pair of bytes (interpreted as unsigned
 * char) that differ in <i>s1</i> and <i>s2</i>.
 *
 * @b Example
 * @code
        // ...
        int buffer1[100];
        int buffer2[100];
        // ...
        if (memcmp(buffer1, buffer2, 100) == 0) {
                // buffers are equal
        } else {
                // buffers are not equal
        }
        // ...
   @endcode
 */
//==============================================================================
extern int memcmp(const void *s1, const void *s2, size_t n);

//==============================================================================
/**
 * @brief Function copies memory blocks.
 *
 * The memcpy() function copies <i>n</i> bytes from memory area <i>src</i> to
 * memory area <i>dest</i>. The memory areas must not overlap. Use memmove()
 * if the memory areas do overlap.
 *
 * @param dest      destination memory area
 * @param src       source memory area
 * @param n         memory size
 *
 * @return The memcpy() function returns a pointer to <i>dest</i>.
 *
 * @b Example
 * @code
        // ...
        int buffer1[100];
        int buffer2[100];
        // ...
        memcpy(buffer1, buffer2, 100);
        // ...
   @endcode
 *
 * @see memmove()
 */
//==============================================================================
extern void *memcpy(void *dest, const void *src, size_t n);

//==============================================================================
/**
 * @brief Function moves memory blocks.
 *
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
 * @return The memmove() function returns a pointer to <i>dest</i>.
 *
 * @b Example
 * @code
        // ...
        int buffer1[100];
        int buffer2[100];
        // ...
        memmove(buffer1, buffer2, 100);
        // ...
   @endcode
 *
 * @see memcpy()
 */
//==============================================================================
extern void *memmove(void *dest, const void *src, size_t n);

//==============================================================================
/**
 * @brief Function fills memory block.
 *
 * The memset() function fills the first <i>n</i> bytes of the memory area
 * pointed to by <i>s</i> with the constant byte <i>c</i>.
 *
 * @param s         memory area
 * @param c         value to set
 * @param n         memory size
 *
 * @return The memset() function returns a pointer to the memory area <i>s</i>.
 *
 * @b Example
 * @code
        // ...
        int buffer[100];
        memset(buffer, 0, 100);
        // ...
   @endcode
 *
 * @see memcpy()
 */
//==============================================================================
extern void *memset(void *s, int c, size_t n);

//==============================================================================
/**
 * @brief Function appends strings.
 *
 * The strcat() function appends the <i>src</i> string to the <i>dest</i> string,
 * overwriting the terminating @ref NULL byte ('\0') at the end of <i>dest</i>, and
 * then adds a terminating null byte.  The strings may not overlap, and the
 * <i>dest</i> string must have enough space for the result.  If <i>dest</i> is
 * not large enough, program behavior is unpredictable; buffer overruns are a
 * favorite avenue for attacking secure programs.
 *
 * @param dest      destination string
 * @param src       source string
 *
 * @return The strcat() function return a pointer to the resulting string <i>dest</i>.
 *
 * @b Example
 * @code
        // ...
        char       *str   = calloc(1, 100);
        const char *str_a = "string 1";
        const char *str_b = "string 2";
        strcat(str, str_a);
        strcat(str, str_b);

        // result: str == "string 1string 2"
        // ...
   @endcode
 *
 * @see strncat()
 */
//==============================================================================
extern char *strcat(char *dest, const char *src);

//==============================================================================
/**
 * @brief Function appends strings with limits.
 *
 * The strncat() function is similar to strcat(), except that:
 *
 * @arg it will use at most <i>n</i> bytes from <i>src</i>; and
 * @arg <i>src</i> does not need to be null-terminated if it contains <i>n</i> or more bytes.
 *
 * As with strcat(), the resulting string in <i>dest</i> is always null-terminated.
 *
 * If <i>src</i> contains <i>n</i> or more bytes, strncat() writes <i>n+1</i>
 * bytes to <i>dest</i> (<i>n</i> from <i>src</i> plus the terminating null
 * byte). Therefore, the size of <i>dest</i> must be at least strlen(dest) + n + 1.
 *
 * @param dest      destination string
 * @param src       source string
 *
 * @return The strncat() function return a pointer to the resulting string <i>dest</i>.
 *
 * @b Example
 * @code
        // ...
        char       *str   = calloc(1, 100);
        const char *str_a = "string 1";
        const char *str_b = "string 2";
        strncat(str, str_a, 50);
        strncat(str, str_b, 50);

        // result: str == "string 1string 2"
        // ...
   @endcode
 *
 * @see strcat(), strlcat()
 */
//==============================================================================
extern char *strncat(char *dest, const char *src, size_t);

//==============================================================================
/**
 * @brief Function returns a pointer to the first occurrence of character.
 *
 * The strchr() function returns a pointer to the first occurrence of
 * the character <i>c</i> in the string <i>s</i>.
 *
 * @param s         string
 * @param c         character to find
 *
 * @return The strchr() function return a pointer to the matched
 * character or @ref NULL if the character is not found. The terminating
 * null byte is considered part of the string, so that if <i>c</i> is specified
 * as '\0', these functions return a pointer to the terminator.
 *
 * @b Example
 * @code
        // ...
        const char *str = "/foo/bar";
        char       *ptr;
        if ((ptr = strchr(str, '/'))) {
                // the first '/' character found ...
        }
        // ...
   @endcode
 *
 * @see strrchr(), memchr(), strfch(), strlch()
 */
//==============================================================================
extern char *strchr(const char *s, int c);

//==============================================================================
/**
 * @brief Function returns a pointer to the last occurrence of character.
 *
 * The strrchr() function returns a pointer to the last occurrence of
 * the character <i>c</i> in the string <i>s</i>.
 *
 * @param s         string
 * @param c         character to find
 *
 * @return The strrchr() function return a pointer to the matched
 * character or @ref NULL if the character is not found. The terminating
 * null byte is considered part of the string, so that if <i>c</i> is specified
 * as '\0', these functions return a pointer to the terminator.
 *
 * @b Example
 * @code
        // ...
        const char *str = "/foo/bar";
        char       *ptr;
        if ((ptr = strrchr(str, '/'))) {
                // the last '/' character found ...
        }
        // ...
   @endcode
 *
 * @see strchr(), memchr(), strfch(), strlch()
 */
//==============================================================================
extern char *strrchr(const char *s, int c);

//==============================================================================
/**
 * @brief Function compares the two strings.
 *
 * The strcmp() function compares the two strings <i>s1</i> and <i>s2</i>.
 * It returns an integer less than, equal to, or greater than zero if <i>s1</i>
 * is found, respectively, to be less than, to match, or be greater than <i>s2</i>.
 *
 * @param s1        string 1
 * @param s2        string 2
 *
 * @return The strcmp() function return an integer less than, equal to, or
 * greater than zero if <i>s1</i> is found, respectively, to be less than, to
 * match, or be greater than <i>s2</i>.
 *
 * @b Example
 * @code
        // ...
        const char *str1 = "/foo/bar1";
        const char *str2 = "/foo/bar2";
        if (strcmp(str1, str2) == 0) {
                // strings are equal
        } else {
                // strings are not equal
        }
        // ...
   @endcode
 *
 * @see strncmp(), strcasecmp(), strncasecmp(), memcmp(), isstreq(), isstreqn()
 */
//==============================================================================
extern int strcmp(const char *s1, const char *s2);

//==============================================================================
/**
 * @brief Function compares the two strings with limits.
 *
 * The strncmp() function is similar to strcmp(), except it compares the only
 * first (at most) <i>n</i> bytes of <i>s1</i> and <i>s2</i>.
 *
 * @param s1        string 1
 * @param s2        string 2
 * @param n         max strings size
 *
 * @return The strncmp() function return an integer less than, equal to, or
 * greater than zero if <i>s1</i> is found, respectively, to be less than, to
 * match, or be greater than <i>s2</i>.
 *
 * @b Example
 * @code
        // ...
        const char *str1 = "/foo/bar1";
        const char *str2 = "/foo/bar2";
        if (strncmp(str1, str2, 4) == 0) {
                // strings are equal
        } else {
                // strings are not equal
        }
        // ...
   @endcode
 *
 * @see strcmp(), strcasecmp(), strncasecmp(),  memcmp(), isstreq(), isstreqn()
 */
//==============================================================================
extern int strncmp(const char *s1, const char *s2, size_t n);

//==============================================================================
/**
 * @brief Function compares thw two strings
 *
 * The strcoll() function compares the two strings <i>s1</i> and <i>s2</i>. It
 * returns an integer less than, equal to, or greater than zero if <i>s1</i> is
 * found, respectively, to be less than, to match, or be greater than
 * <i>s2</i>. The result is dependent on the LC_COLLATE setting of the location.
 *
 * @param s1        string 1
 * @param s2        string 2
 *
 * @return The strcoll() function returns an integer less than, equal to, or
 * greater than zero if <i>s1</i> is found, respectively, to be less than, to
 * match, or be greater than <i>s2</i>, when both are interpreted as
 * appropriate for the current locale.
 *
 * @b Example
 * @code
        // ...
        const char *str1 = "/foo/bar1";
        const char *str2 = "/foo/bar2";
        if (strcoll(str1, str2) == 0) {
                // strings are equal
        } else {
                // strings are not equal
        }
        // ...
   @endcode
 *
 * @see strcmp(), strncmp(), memcmp()
 */
//==============================================================================
extern int strcoll(const char *s1, const char *s2);

//==============================================================================
/**
 * @brief Function copies the string to the buffer.
 *
 * The strcpy() function copies the string pointed to by <i>src</i>, including
 * the terminating null byte ('\0'), to the buffer pointed to by <i>dest</i>.
 * The strings may not overlap, and the destination string <i>dest</i> must be
 * large enough to receive the copy.
 *
 * @param dest      destination
 * @param src       source
 *
 * @return The strcpy() function return a pointer to the destination string
 * <i>dest</i>.
 *
 * @b Example
 * @code
        // ...
        char *str1 = malloc(100);
        if (str1) {
                strcpy(str1, "Foo bar");

                // ...
        }

        // ...
   @endcode
 *
 * @ref strncpy(), memcpy()
 */
//==============================================================================
extern char *strcpy(char *dest, const char *src);

//==============================================================================
/**
 * @brief Function copies the string to the buffer with limits.
 *
 * The strncpy() function is similar to strcpy(), except that at most <i>n</i>
 * bytes of <i>src</i> are copied.
 *
 * @warning If there is no null byte among the first <i>n</i> bytes of <i>src</i>,
 * the string placed in <i>dest</i> will not be null-terminated.
 *
 * If the length of <i>src</i> is less than <i>n</i>, strncpy() writes
 * additional null bytes to <i>dest</i> to ensure that a total of <i>n</i> bytes
 * are written.
 *
 * @param dest      destination
 * @param src       source
 * @param n         max string length
 *
 * @return The strncpy() function return a pointer to the destination string
 * <i>dest</i>.
 *
 * @b Example
 * @code
        // ...
        char *str1 = calloc(1, 100);
        if (str1) {
                strncpy(str1, "Foo bar", 99);

                // ...
        }

        // ...
   @endcode
 *
 * @ref strcpy(), memcpy(), strlcpy()
 */
//==============================================================================
extern char *strncpy(char *dest, const char *src, size_t n);

//==============================================================================
/**
 * @brief Function calculates the length of segment.
 *
 * The strspn() function calculates the length (in bytes) of the initial
 * segment of <i>s</i> which consists entirely of bytes in <i>accept</i>.
 *
 * @param s         string
 * @param accept    string of characters to accept
 *
 * @return The strspn() function returns the number of bytes in the initial
 * segment of <i>s</i> which consist only of bytes from <i>accept</i>.
 *
 * @b Example
 * @code
        // ...
        const char *foo = "/foo/bar";
        size_t      accepted_chars = strspn(foo, "/fo");
        // accepted_chars: 3
        // ...
   @endcode
 *
 * @see strcspn(), strlen()
 */
//==============================================================================
extern size_t strspn(const char *s, const char *accept);

//==============================================================================
/**
 * @brief Function calculates the length of segment.
 *
 * The strcspn() function calculates the length of the initial segment
 * of <i>s</i> which consists entirely of bytes not in <i>reject</i>.
 *
 * @param s         string
 * @param reject    string of characters to reject
 *
 * @return The strcspn() function returns the number of bytes in the initial
 * segment of <i>s</i> which are not in the string <i>reject</i>.
 *
 * @b Example
 * @code
        // ...
        const char *foo = "/foo/bar";
        size_t      rejected_chars = strcspn(foo, "ar");
        // rejected_chars: 6 (stops on 'a')
        // ...
   @endcode
 *
 * @see strspn(), strlen()
 */
//==============================================================================
extern size_t strcspn(const char *s, const char *reject);

//==============================================================================
/**
 * @brief Function calculates the length of the string.
 *
 * The strlen() function calculates the length of the string <i>s</i>,
 * excluding the terminating null byte ('\0').
 *
 * @param s         string
 *
 * @return The strlen() function returns the number of bytes in the string <i>s</i>.
 *
 * @b Example
 * @code
        // ...
        const char *foo = "/foo/bar";
        size_t      len = strlen(foo);
        // ...
   @endcode
 *
 * @see strnlen(), strsize()
 */
//==============================================================================
extern size_t strlen(const char *s);

//==============================================================================
/**
 * @brief Function calculates the length of the string with limits.
 *
 * The strnlen() function returns the number of bytes in the string
 * pointed to by <i>s</i>, excluding the terminating null byte ('\0'), but at
 * most <i>maxlen</i>. In doing this, strnlen() looks only at the first
 * <i>maxlen</i> bytes at <i>s</i> and never beyond <i>s + maxlen</i>.
 *
 * @param s         string
 * @param maxlen    max string length
 *
 * @return The strnlen() function returns strlen(s), if that is less than
 * <i>maxlen</i>, or <i>maxlen</i> if there is no null byte ('\0') among the
 * first <i>maxlen</i> bytes pointed to by <i>s</i>.
 *
 * @b Example
 * @code
        // ...
        const char *foo = "/foo/bar";
        size_t      len = strnlen(foo, 10);
        // ...
   @endcode
 *
 * @see strlen(), strsize()
 */
//==============================================================================
extern size_t strnlen(const char *s, size_t maxlen);

//==============================================================================
/**
 * @brief Function locates the first occurrence bytes in the string.
 *
 * The strpbrk() function locates the first occurrence in the string <i>s</i>
 * of any of the bytes in the string <i>accept</i>.
 *
 * @param s         string
 * @param accept    characters to find
 *
 * @return The strpbrk() function returns a pointer to the byte in <i>s</i> that
 * matches one of the bytes in <i>accept</i>, or @ref NULL if no such byte is found.
 *
 * @b Example
 * @code
        // ...
        const char *foo = "/foo/bar";
        char       *loc = strpbrk(foo, "ab");

        // loc points to 'b' character
        // ...
   @endcode
 *
 * @see strchr(), strrchr()
 */
//==============================================================================
extern char *strpbrk(const char *s, const char *accept);

//==============================================================================
/**
 * @brief Function finds the first occurrence of substring in the string.
 *
 * The strstr() function finds the first occurrence of the substring
 * <i>needle</i> in the string <i>haystack</i>.  The terminating null bytes
 * ('\0') are not compared.
 *
 * @param haystack  string
 * @param needle    characters to find
 *
 * @return The function return a pointer to the beginning of the located
 * substring, or @ref NULL if the substring is not found.
 *
 * @b Example
 * @code
        // ...
        const char *foo = "this is example string";
        char       *loc = strstr(foo, "is example");

        // loc points to "is example string"
        // ...
   @endcode
 *
 * @see strcasestr()
 */
//==============================================================================
extern char *strstr(const char *haystack, const char *needle);

//==============================================================================
/**
 * @brief Function finds the first occurrence of substring in the string.
 *
 * The strcasestr() function is like strstr(), but ignores the case of
 * both arguments.
 *
 * @param haystack  string
 * @param needle    characters to find
 *
 * @return The function return a pointer to the beginning of the located
 * substring, or @ref NULL if the substring is not found.
 *
 * @b Example
 * @code
        // ...
        const char *foo = "this Is Example string";
        char       *loc = strcasestr(foo, "is example");

        // loc points to "Is Example string"
        // ...
   @endcode
 *
 * @see strstr()
 */
//==============================================================================
extern char *strcasestr(const char *haystack, const char *needle);

//==============================================================================
/**
 * @brief Function compares the two strings and ignore case of the characters.
 *
 * The strcasecmp() function compares the two strings <i>s1</i> and <i>s2</i>,
 * ignoring the case of the characters.  It returns an integer less
 * than, equal to, or greater than zero if <i>s1</i> is found, respectively, to
 * be less than, to match, or be greater than <i>s2</i>.
 *
 * @param s1        string 1
 * @param s2        string 2
 *
 * @return The strcasecmp() function return an integer less than, equal to, or
 * greater than zero if <i>s1</i> is found, respectively, to be less than, to
 * match, or be greater than <i>s2</i>.
 *
 * @b Example
 * @code
        // ...
        const char *foo = "FOO";
        const char *bar = "foo";

        if (strcasecmp(foo, bar) == 0) {
                // result: functions are equal
        }

        // ...
   @endcode
 *
 * @see strncasecmp(), strcmp(), strncmp(), memcmp()
 */
//==============================================================================
extern int strcasecmp(const char *s1, const char *s2);

//==============================================================================
/**
 * @brief Function compares the two strings and ignore case of the characters with limits.
 *
 * The strncasecmp() function is similar to strcasecmp(), except it
 * compares the only first <i>n</i> bytes of <i>s1</i>.
 *
 * @param s1        string 1
 * @param s2        string 2
 * @param n         max number of bytes to compare
 *
 * @return The strncasecmp() function return an integer less than, equal to, or
 * greater than zero if <i>s1</i> or the first <i>n</i> bytes thereof is found,
 * respectively, to be less than, to match, or be greater than <i>s2</i>.
 *
 * @b Example
 * @code
        // ...
        const char *foo = "FOO";
        const char *bar = "foo";

        if (strncasecmp(foo, bar, 3) == 0) {
                // result: functions are equal
        }

        // ...
   @endcode
 *
 * @see strcasecmp(), strcmp(), strncmp(), memcmp()
 */
//==============================================================================
extern int strncasecmp(const char *s1, const char *s2, size_t n);

//==============================================================================
/**
 * @brief Function returns a pointer to an error string.
 *
 * The strerror() function returns a pointer to a string that describes the
 * error code passed in the argument <i>errnum</i>.
 *
 * @param errnum        error number
 *
 * @return Functions return the appropriate error description string.
 *
 * @b Example
 * @code
        // ...
        puts(strerror(errno));
        // ...
   @endcode
 *
 * @see errno, perror()
 */
//==============================================================================
extern const char *strerror(int errnum);

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief The strndup() function is used to duplicate a string of size n.
 *
 * This function returns a pointer to a null-terminated byte string, which is
 * a duplicate of the string pointed to by s. The memory obtained is done
 * dynamically using malloc and hence it can be freed using free().
 *
 * @param s             string
 * @param n             string size
 *
 * @return It returns a pointer to the duplicated string s.
 *
 * @b Example
 * @code
        // ...
        char *duplicate = strndup("My string", 4);
        // duplicate is: "My s"
        // ...
        free(duplicate);
        // ...
   @endcode
 *
 * @see strdup, free
 */
//==============================================================================
static inline char *strndup(const char *s, size_t n)
{
        extern size_t _strlcpy(char *dst, const char *src, size_t size);

        n += 1;
        char *dup = NULL;
        syscall(SYSCALL_MALLOC, &dup, &n);
        if (dup) {
                _strlcpy(dup, s, n);
        }
        return dup;
}

//==============================================================================
/**
 * @brief The strdup() function is used to duplicate a string.
 *
 * This function returns a pointer to a null-terminated byte string, which is
 * a duplicate of the string pointed to by s. The memory obtained is done
 * dynamically using malloc and hence it can be freed using free().
 *
 * @param s             string
 *
 * @return It returns a pointer to the duplicated string s.
 *
 * @b Example
 * @code
        // ...
        char *duplicate = strdup("My string");
        // ...
        free(duplicate);
        // ...
   @endcode
 *
 * @see strndup, free
 */
//==============================================================================
static inline char *strdup(const char *s)
{
        return strndup(s, strlen(s));
}

#ifdef __cplusplus
}
#endif

#endif /* _STRING_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
