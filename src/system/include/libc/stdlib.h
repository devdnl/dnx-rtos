/*=========================================================================*//**
@file    stdlib.h

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

#ifndef _STDLIB_H_
#define _STDLIB_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <sys/types.h>
//#include "core/sysmoni.h"
#include "lib/conv.h"
#include "kernel/syscall.h"

#include <machine/ieeefp.h>
#include "_ansi.h"

#define __need_size_t
#define __need_wchar_t
#include <stddef.h>

/*==============================================================================
  Exported macros
==============================================================================*/
#define EXIT_FAILURE    1
#define EXIT_SUCCESS    0

#ifndef NULL
#define NULL            0
#endif

#define RAND_MAX        __RAND_MAX

#ifndef _PTR
#define _PTR            void *
#endif

#ifndef _VOID
#define _VOID           void
#endif

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct {
        int quot;               /* quotient */
        int rem;                /* remainder */
} div_t;

typedef struct {
        long quot;              /* quotient */
        long rem;               /* remainder */
} ldiv_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
//==============================================================================
/**
 * @brief int abs(int j)
 * The <b>abs</b>() function computes the absolute value of the integer argument <i>j</i>.
 *
 * @param j         value to convert
 *
 * @errors None
 *
 * @return Returns the absolute value of the integer argument, of the
 * appropriate integer type for the function.
 *
 * @example
 * // ...
 * int foo = -100;
 * int bar = abs(foo);
 * // ...
 */
//==============================================================================
extern int abs(int);

//==============================================================================
/**
 * @brief long int labs(long int j)
 * The <b>labs</b>() function computes the absolute value of the integer argument <i>j</i>.
 *
 * @param j         value to convert
 *
 * @errors None
 *
 * @return Returns the absolute value of the integer argument, of the
 * appropriate integer type for the function.
 *
 * @example
 * // ...
 * long int foo = -100;
 * long int bar = labs(foo);
 * // ...
 */
//==============================================================================
extern long labs(long);

//==============================================================================
/**
 * @brief void *bsearch(const void *key, const void *base, size_t nmemb, size_t size, int (*compar(const void *, const void *)))
 * The <b>bsearch</b>() function searches an array of <i>nmemb</i> objects, the initial
 * member of which is pointed to by <i>base</i>, for a member that matches the object
 * pointed to by <i>key</i>. The size of each member of the array is specified by <i>size</i>.<p>
 *
 * The contents of the array should be in ascending sorted order according
 * to the comparison function referenced by <i>compar</i>. The <i>compar</i> routine is
 * expected to have two arguments which point to the key object and to an
 * array member, in that order, and should return an integer less than,
 * equal to, or greater than zero if the key object is found, respectively,
 * to be less than, to match, or be greater than the array member.
 *
 * @param key       object to compare
 * @param base      initial object array
 * @param nmemb     number of objects in array
 * @param size      object size
 * @param compar    function used to compare objects
 *
 * @errors None
 *
 * @return The <b>bsearch</b>() function returns a pointer to a matching member of the
 * array, or <b>NULL</b> if no match is found.  If there are multiple elements
 * that match the key, the element returned is unspecified.
 *
 * @example
 * #include <stdio.h>
 * #include <stdlib.h>
 * #include <string.h>
 *
 * struct mi {
 *     int nr;
 *     char *name;
 * } months[] = {
 *     { 1, "jan" }, { 2, "feb" }, { 3, "mar" }, { 4, "apr" },
 *     { 5, "may" }, { 6, "jun" }, { 7, "jul" }, { 8, "aug" },
 *     { 9, "sep" }, {10, "oct" }, {11, "nov" }, {12, "dec" }
 * };
 *
 * const int nr_of_months = (sizeof(months)/sizeof(months[0]));
 *
 * static int compmi(const void *m1, const void *m2)
 * {
 *     struct mi *mi1 = (struct mi *) m1;
 *     struct mi *mi2 = (struct mi *) m2;
 *     return strcmp(mi1->name, mi2->name);
 * }
 *
 * int main(int argc, char **argv)
 * {
 *     int i;
 *
 *     qsort(months, nr_of_months, sizeof(struct mi), compmi);
 *     for (i = 1; i < argc; i++) {
 *         struct mi key, *res;
 *         key.name = argv[i];
 *         res = bsearch(&key, months, nr_of_months,
 *                       sizeof(struct mi), compmi);
 *         if (res == NULL)
 *             printf("'%s': unknown month\n", argv[i]);
 *         else
 *             printf("%s: month #%d\n", res->name, res->nr);
 *     }
 *
 *     exit(EXIT_SUCCESS);
 * }
 */
//==============================================================================
extern _PTR bsearch(const _PTR, const _PTR, size_t, size_t, int (*_compar(const _PTR, const _PTR)));

//==============================================================================
/**
 * @brief void qsort(void *base, size_t nmemb, size_t size, int(*compar)(const void *, const void *))
 * The <b>qsort</b>() function sorts an array with <i>nmemb</i> elements of size <i>size</i>.
 * The <i>base</i> argument points to the start of the array.<p>
 *
 * The contents of the array are sorted in ascending order according to
 * a comparison function pointed to by <i>compar</i>, which is called with two
 * arguments that point to the objects being compared.<p>
 *
 * The comparison function must return an integer less than, equal to,
 * or greater than zero if the first argument is considered to be
 * respectively less than, equal to, or greater than the second. If two
 * members compare as equal, their order in the sorted array is undefined.
 *
 * @param base      initial object array
 * @param nmemb     number of objects in array
 * @param size      object size
 * @param compar    function used to compare objects
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <stdio.h>
 * #include <stdlib.h>
 * #include <string.h>
 *
 * struct mi {
 *     int nr;
 *     char *name;
 * } months[] = {
 *     { 1, "jan" }, { 2, "feb" }, { 3, "mar" }, { 4, "apr" },
 *     { 5, "may" }, { 6, "jun" }, { 7, "jul" }, { 8, "aug" },
 *     { 9, "sep" }, {10, "oct" }, {11, "nov" }, {12, "dec" }
 * };
 *
 * const int nr_of_months = (sizeof(months)/sizeof(months[0]));
 *
 * static int compmi(const void *m1, const void *m2)
 * {
 *     struct mi *mi1 = (struct mi *) m1;
 *     struct mi *mi2 = (struct mi *) m2;
 *     return strcmp(mi1->name, mi2->name);
 * }
 *
 * int main(int argc, char **argv)
 * {
 *     int i;
 *
 *     qsort(months, nr_of_months, sizeof(struct mi), compmi);
 *     for (i = 1; i < argc; i++) {
 *         struct mi key, *res;
 *         key.name = argv[i];
 *         res = bsearch(&key, months, nr_of_months,
 *                       sizeof(struct mi), compmi);
 *         if (res == NULL)
 *             printf("'%s': unknown month\n", argv[i]);
 *         else
 *             printf("%s: month #%d\n", res->name, res->nr);
 *     }
 *
 *     exit(EXIT_SUCCESS);
 * }
 */
//==============================================================================
extern _VOID qsort(_PTR __base, size_t __nmemb, size_t __size, int(*_compar)(const _PTR, const _PTR));

//==============================================================================
/**
 * @brief div_t div(int numerator, int denominator)
 * The <b>div</b>() function computes the value <i>numerator/denominator</i> and
 * returns the quotient and remainder in a structure named <b>div_t</b> that
 * contains two integer members (in unspecified order) named <b>quot</b> and
 * <b>rem</b>. The quotient is rounded toward zero. The result satisfies
 * <i>quot * denominator + rem = numerator</i>.<p>
 *
 * <b>div_t</b> structure:
 * <pre>
 * typedef struct {
 *         int quot;    // quotient
 *         int rem;     // remainder
 * } div_t;
 * </pre>
 *
 * @param numerator     a numerator
 * @param denominator   a denominator
 *
 * @errors None
 *
 * @return The <b>div_t</b> structure.
 *
 * @example
 * // ...
 * div_t = div(1, 2);
 * // ...
 */
//==============================================================================
extern div_t div(int, int);

//==============================================================================
/**
 * @brief ldiv_t ldiv(long int numerator, long int denominator)
 * The <b>ldiv</b>() function computes the value <i>numerator/denominator</i> and
 * returns the quotient and remainder in a structure named <b>ldiv_t</b> that
 * contains two integer members (in unspecified order) named <b>quot</b> and
 * <b>rem</b>. The quotient is rounded toward zero. The result satisfies
 * <i>quot * denominator + rem = numerator</i>.<p>
 *
 * <b>div_t</b> structure:
 * <pre>
 * typedef struct {
 *         long int quot;       // quotient
 *         long int rem;        // remainder
 * } ldiv_t;
 * </pre>
 *
 * @param numerator     a numerator
 * @param denominator   a denominator
 *
 * @errors None
 *
 * @return The <b>ldiv_t</b> structure.
 *
 * @example
 * // ...
 * ldiv_t = ldiv(1, 2);
 * // ...
 */
//==============================================================================
extern ldiv_t ldiv(long numer, long denom);

//==============================================================================
/**
 * @brief int rand(void)
 * The <b>rand</b>() function returns a pseudo-random integer in the range 0 to
 * <B>RAND_MAX</b> inclusive (i.e., the mathematical range [0, RAND_MAX]).
 *
 * @param None
 *
 * @errors None
 *
 * @return The <b>rand</b>() function return a value between 0 and <b>RAND_MAX</b> (inclusive).
 *
 * @example
 * // ...
 * int foo = rand();
 * // ...
 */
//==============================================================================
extern int rand(_VOID);

//==============================================================================
/**
 * @brief void srand(unsigned int seed)
 * The <b>srand</b>() function sets its argument as the seed for a new sequence
 * of pseudo-random integers to be returned by <b>rand</b>().  These sequences
 * are repeatable by calling <b>srand</b>() with the same seed value.
 *
 * @param seed      new seed value
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * // ...
 * #include <dnx/os.h>
 *
 * srand(get_tick_counter());
 * int foo = rand();
 * // ...
 */
//==============================================================================
extern _VOID srand(unsigned __seed);

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief void *malloc(size_t size)
 * The <b>malloc</b>() function allocates <i>size</i> bytes and returns a pointer to the
 * allocated memory. The memory is not initialized.  If size is 0, then <b>malloc</b>()
 * returns either <b>NULL</b>.
 *
 * @param size      size bytes to allocate
 *
 * @errors ENOMEM
 *
 * @return Returns a pointer to the allocated memory, which is suitably aligned
 * for any built-in type.  On error, these functions return <b>NULL</b>.  <b>NULL</b> may
 * also be returned by a successful call to function with a <i>size</i> of zero.
 *
 * @example
 * // ...
 * int *buffer = malloc(100 * sizeof(int));
 * // ...
 */
//==============================================================================
static inline void *malloc(size_t size)
{
        void *mem = NULL;
        _syscall(SYSCALL_MALLOC, &mem, size);
        return mem;
}

//==============================================================================
/**
 * @brief void *calloc(size_t n, size_t size)
 * The <b>calloc</b>() function allocates memory for an array of <i>n</i> elements
 * of <i>size</i> bytes each and returns a pointer to the allocated memory.
 * The memory is set to zero.
 *
 * @param n         number of elements
 * @param size      size of elements
 *
 * @errors ENOMEM
 *
 * @return Returns a pointer to the allocated memory, which is suitably aligned
 * for any built-in type.  On error, these functions return <b>NULL</b>.  <b>NULL</b> may
 * also be returned by a successful call to function with a <i>n</i> or <i>size</i>
 * of zero.
 *
 * @example
 * // ...
 * int *buffer = calloc(100, sizeof(int));
 * // ...
 */
//==============================================================================
static inline void *calloc(size_t n, size_t size)
{
        void *mem = NULL;
        _syscall(SYSCALL_ZALLOC, &mem, size * n);
        return mem;
}

//==============================================================================
/**
 * @brief void free(void *ptr)
 * The <b>free</b>() function frees the memory space pointed to by <i>ptr</i>, which
 * must have been returned by a previous call to <b>malloc</b>(), <b>calloc</b>(),
 * <b>realloc</b>(). If <i>ptr</i> is <b>NULL</b>, no operation is performed.
 *
 * @param ptr       pointer to memory space to be freed
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * // ...
 * int *buffer = calloc(100, sizeof(int));
 * if (buffer) {
 *         // ...
 *
 *         free(buffer);
 * }
 * // ...
 */
//==============================================================================
static inline void free(void *ptr)
{
        _syscall(SYSCALL_FREE, NULL, ptr);
}

//==============================================================================
/**
 * @brief void *realloc(void *ptr, size_t size)
 * The <b>realloc</b>() function changes the size of the memory block pointed
 * to by <i>ptr</i> to size bytes. The contents will be unchanged in the range
 * from the start of the region up to the minimum of the old and new
 * sizes.  If the new size is larger than the old size, the added memory
 * will not be initialized.  If <i>ptr</i> is <b>NULL</b>, then the call is equivalent
 * to malloc(size), for all values of <i>size</i>; if <i>size</i> is equal to
 * zero, and <i>ptr</i> is not <b>NULL</b>, then the call is equivalent to free(ptr).
 * Unless <i>ptr</i> is <b>NULL</b>, it must have been returned by an earlier call to
 * <b>malloc</b>(), <b>calloc</b>() or <b>realloc</b>().  If the area pointed to was moved, a
 * free(ptr) is done.
 *
 * @param ptr       pointer to memory space
 * @param size      size of new memory space
 *
 * @errors ENOMEM
 *
 * @return The <b>realloc</b>() function returns a pointer to the newly allocated
 * memory, which is suitably aligned for any built-in type and may be
 * different from <i>ptr</i>, or <b>NULL</b> if the request fails.  If <i>size</i>
 * was equal to 0, either <b>NULL</b> or a pointer suitable to be passed to <b>free</b>()
 * is returned. If <b>realloc</b>() fails the original block is left untouched;
 * it is not freed or moved.
 *
 * @example
 * // ...
 * int *buffer = realloc(NULL, 100);
 * if (buffer) {
 *         // ...
 *
 *         free(buffer);
 * }
 * // ...
 */
//==============================================================================
static inline void *realloc(void *ptr, size_t size)
{
        extern _PTR memcpy(_PTR dest, const _PTR src, size_t n);

        if (size) {
                void *mem = malloc(size);
                if (ptr == NULL)
                        return mem;

                if (mem) {
                        memcpy(mem, ptr, size);
                        free(ptr);
                        return mem;
                }
        }

        return NULL;
}

//==============================================================================
/**
 * @brief void abort(void)
 * The <b>abort</b>() function kills program which execute it.
 *
 * @param None
 *
 * @errors None
 *
 * @return The <b>abort</b>() function never returns.
 *
 * @example
 * // ...
 * abort();
 */
//==============================================================================
static inline void abort(void)
{
//        _abort(); TODO syscall
        for (;;); // no return function - this makes C++ compiler happy
}

//==============================================================================
/**
 * @brief void exit(int status)
 * The <b>exit</b>() function kills program which execute it and program return
 * <i>status</i> value.
 *
 * @param status        status to be return by program
 *
 * @errors None
 *
 * @return The <b>exit</b>() function never returns.
 *
 * @example
 * // ...
 * exit(EXIT_FAILURE);
 */
//==============================================================================
static inline void exit(int status)
{
//        _exit(status); TODO syscall
        for (;;); // no return function - this makes C++ compiler happy
}

//==============================================================================
/**
 * @brief int system(const char *command)
 * The <b>system</b>() executes a command specified in <i>command</i> and returns after
 * the command has been completed.
 *
 * @param command       command to execute
 *
 * @errors None
 *
 * @return Returns 0 if program was successfully executed. On error 1 and negative
 * values are returned.
 *
 * @example
 * // ...
 * int s = system("rm /foo/bar");
 */
//==============================================================================
static inline int system(const char *command)
{
//        return _system(command); TODO syscall
}

//==============================================================================
/**
 * @brief char *getenv(const char *name)
 * The <b>getenv</b>() function searches the environment list to find the environment
 * variable name, and returns a pointer to the corresponding value string.<p>
 *
 * Function is not supported by dnx RTOS and always returns <b>NULL</b>.
 *
 * @param name          variable name
 *
 * @errors None
 *
 * @return Returns a pointer to the value in the environment, or <b>NULL</b> if
 * there is no match.
 *
 * @example
 * // ...
 * char *var = getenv("foo");
 * // ...
 */
//==============================================================================
static inline char *getenv(const char *name)
{
        (void) name;
        return NULL;
}

//==============================================================================
/**
 * @brief int getsubopt(char **optionp, char *const *tokens, char **valuep)
 * The <b>getsubopt</b>() parses the list of comma-separated suboptions.<p>
 *
 * Function is not supported by dnx RTOS and always returns -1.
 *
 * @param optionp           option pointer
 * @param tokens            tokens to find
 * @param valuep            value pointer
 *
 * @errors None
 *
 * @return If the first suboption in <i>optionp</i> is recognized, <b>getsubopt</b>()
 * returns the index of the matching suboption element in <i>tokens</i>.
 * Otherwise, -1 is returned and <i>*valuep</i> is the entire <i>name[=value]</i>
 * string.
 *
 * @example
 * // ...
 */
//==============================================================================
static inline int getsubopt(char **optionp, char *const *tokens, char **valuep)
{
        (void) optionp;
        (void) tokens;
        (void) valuep;

        return -1;
}

//==============================================================================
/**
 * @brief int atoi(const char *str)
 * The <b>atoi</b>() function converts the initial portion of the string pointed
 * to by <i>str</i> to int. The behavior is the same as <b>strtol</b>(nptr, NULL, 10);
 * except that <b>atoi</b>() does not detect errors.
 *
 * @param str           string to convert
 *
 * @errors None
 *
 * @return The converted value.
 *
 * @example
 * // ...
 * int val = atoi("125");
 * // ...
 */
//==============================================================================
static inline int atoi(const char *str)
{
        return _atoi(str);
}

//==============================================================================
/**
 * @brief int atol(const char *str)
 * The <b>atol</b>() function converts the initial portion of the string pointed
 * to by <i>str</i> to int. The behavior is the same as <b>strtol</b>(nptr, NULL, 10);
 * except that <b>atoi</b>() does not detect errors.
 *
 * @param str           string to convert
 *
 * @errors None
 *
 * @return The converted value.
 *
 * @example
 * // ...
 * int val = atol("125");
 * // ...
 */
//==============================================================================
static inline int atol(const char *str)
{
        return _atoi(str);
}

//==============================================================================
/**
 * @brief i32_t strtol(const char *nptr, char **endptr, int base)
 * The <b>strtol</b>() function converts the initial part of the string in <i>nptr</i>
 * to a 32-bit value according to the given <i>base</i>, which must be
 * between 2 and 16 inclusive, or be the special value 0.<p>
 *
 * The string may begin with an arbitrary amount of white space (as
 * determined by <b>isspace</b>()) followed by a single optional '+' or '-'
 * sign. If base is zero or 16, the number will be read in base 16; otherwise,
 * a zero base is taken as 10 (decimal) unless the next character is '0', in
 * which case it is taken as 8 (octal).<p>
 *
 * If <i>endptr</i> is not <b>NULL</b>, <b>strtol</b>()stores the address of the first
 * invalid character in <i>*endptr</i>.  If there were no digits at all,
 * <b>strtol</b>() stores the original value of <i>nptr</i> in <i>*endptr</i> (and
 * returns 0).  In particular, if <i>*nptr</i> is not '\0' but <i>**endptr</i>
 * is '\0' on return, the entire string is valid.
 *
 * @param nptr          string to convert
 * @param endptr        points to first not converted character
 * @param base          calculation base
 *
 * @errors None
 *
 * @return The <b>strtol</b>() function returns the result of the conversion, unless
 * the value would underflow or overflow.
 *
 * @example
 * const char *end;
 * const char *str;
 * i32_t       val;
 *
 * // convert string to decimal value
 * str = "123";
 * val = strtol(str, &end, 10);
 *
 * // convert string to hex value
 * str = "1FF";
 * val = strtol(str, &end, 16);
 *
 * // convert string to octal
 * str = "77"
 * val = strtol(str, &end, 8);
 *
 * // convert string automatically
 * str = "0x45";
 * val = strtol(str, &end, 0);
 *
 * str = "056";
 * val = strtol(str, &end, 0);
 *
 * str = "123";
 * val = strtol(str, &end, 0);
 *
 * str = "0b1110";
 * val = strtol(str, &end, 0);
 */
//==============================================================================
static inline i32_t strtol(const char *nptr, char **endptr, int base)
{
        i32_t result;
        char *end = _strtoi(nptr, base, &result);
        if (endptr)
                *endptr = end;
        return result;
}

//==============================================================================
/**
 * @brief double atof(const char *nptr)
 * The <b>atof</b>() function converts the initial portion of the string pointed
 * to by <i>nptr</i> to double.  The behavior is the same as
 * <pre>strtod(nptr, NULL);</pre>
 * except that <b>atof</b>() does not detect errors.
 *
 * @param nptr          string to convert
 *
 * @errors None
 *
 * @return The converted value.
 *
 * @example
 * // convert string to decimal value
 * const char *str = "123.56";
 * double      val = atof(str);
 */
//==============================================================================
static inline double atof(const char *nptr)
{
        return _atof(nptr);
}

//==============================================================================
/**
 * @brief double strtod(const char *nptr, char **endptr)
 * The function convert the initial portion of the string pointed to by <i>nptr</i>
 * to double representation.
 *
 * @param nptr          string to convert
 * @param endptr        points to first not converted character
 *
 * @errors None
 *
 * @return These functions return the converted value, if any.
 *
 * @example
 * // convert string to decimal value
 * const char *str = "123.56";
 * double      val = strtod(str, NULL);
 */
//==============================================================================
static inline double strtod(const char *nptr, char **endptr)
{
        return _strtod(nptr, endptr);
}

//==============================================================================
/**
 * @brief float strtof(const char *nptr, char **endptr)
 * The function convert the initial portion of the string pointed to by <i>nptr</i>
 * to float representation.
 *
 * @param nptr          string to convert
 * @param endptr        points to first not converted character
 *
 * @errors None
 *
 * @return These functions return the converted value, if any.
 *
 * @example
 * // convert string to decimal value
 * const char *str = "123.56";
 * float       val = strtof(str, NULL);
 */
//==============================================================================
static inline float strtof(const char *nptr, char **endptr)
{
        return (float)_strtod(nptr, endptr);
}

#ifdef __cplusplus
}
#endif

#endif /* _STDLIB_H_ */
/*==============================================================================
  End of file
==============================================================================*/
