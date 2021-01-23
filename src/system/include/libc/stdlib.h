/*=========================================================================*//**
@file    stdlib.h

@author  Daniel Zorychta

@brief   Standard library.

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
\defgroup stdlib-h <stdlib.h>

The library provides functions to allocate memory, search data, sort items,
program control, and so on.

@{
*/

#ifndef _STDLIB_H_
#define _STDLIB_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <sys/types.h>
#include <lib/conv.h>
#include <lib/cast.h>
#include <kernel/syscall.h>
#include <kernel/kwrapper.h>
#include <kernel/builtinfunc.h>
#include <machine/ieeefp.h>
#include <_ansi.h>

#ifndef DOXYGEN
#define __need_size_t
#define __need_wchar_t
#include <stddef.h>
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 * @brief General failure status
 */
#define EXIT_FAILURE    1

/**
 * @brief General success status
 */
#define EXIT_SUCCESS    0

/**
 * @brief A value reserved for indicating that the pointer does not refer to a valid object.
 */
#ifndef NULL
#define NULL            0
#endif

/**
 * @brief Is the maximum value that can be returned by rand() function.
 */
#define RAND_MAX        __RAND_MAX

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * @brief Divide operation result.
 *
 * Type is used to present quotient and remainder value of divide operation for
 * integer values.
 *
 * @see div()
 */
typedef struct {
        int quot;               /*!< Quotient.*/
        int rem;                /*!< Remainder.*/
} div_t;

/**
 * @brief Divide operation result.
 *
 * Type is used to present quotient and remainder value of divide operation for
 * long integer values.
 *
 * @see ldiv()
 */
typedef struct {
        long quot;              /*!< Quotient.*/
        long rem;               /*!< Remainder.*/
} ldiv_t;

/**
 * @brief Type describe compare function
 *
 * @param a     pointer to user object a
 * @param b     pointer to user object b
 *
 * @return If objects are equal then \b 0 is returned, otherwise \b -1 or \b 1.
 *
 * @see bsearch(), qsort()
 */
typedef int (*compar_t)(const void *a, const void *b);

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
//==============================================================================
/**
 * @brief Function calculate absolute value of given integer.
 *
 * The abs() function computes the absolute value of the integer argument <i>j</i>.
 *
 * @param j         value to convert
 *
 * @return Returns the absolute value of the integer argument, of the
 * appropriate integer type for the function.
 *
 * @b Example
 * @code
        // ...
        int foo = -100;
        int bar = abs(foo); // bar = 100
        // ...
   @endcode
 *
 * @see labs()
 */
//==============================================================================
extern int abs(int j);

//==============================================================================
/**
 * @brief Function calculate absolute value of given long integer.
 *
 * The labs() function computes the absolute value of the integer argument <i>j</i>.
 *
 * @param j         value to convert
 *
 * @return Returns the absolute value of the integer argument, of the
 * appropriate integer type for the function.
 *
 * @b Example
 * @code
        // ...
        long int foo = -100;
        long int bar = labs(foo); // bar = 100
        // ...
   @endcode
 *
 * @see abs()
 */
//==============================================================================
extern long labs(long j);

//==============================================================================
/**
 * @brief Function searches an array of user defined objects.
 *
 * The bsearch() function searches an array of <i>nmemb</i> objects, the initial
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
 * @return The bsearch() function returns a pointer to a matching member of the
 * array, or @ref NULL if no match is found.  If there are multiple elements
 * that match the key, the element returned is unspecified.
 *
 * @b Example
 * @code
        #include <stdio.h>
        #include <stdlib.h>
        #include <string.h>

        struct mi {
            int nr;
            char *name;
        } months[] = {
            { 1, "jan" }, { 2, "feb" }, { 3, "mar" }, { 4, "apr" },
            { 5, "may" }, { 6, "jun" }, { 7, "jul" }, { 8, "aug" },
            { 9, "sep" }, {10, "oct" }, {11, "nov" }, {12, "dec" }
        };

        const int nr_of_months = (sizeof(months)/sizeof(months[0]));

        static int compmi(const void *m1, const void *m2)
        {
            struct mi *mi1 = (struct mi *) m1;
            struct mi *mi2 = (struct mi *) m2;
            return strcmp(mi1->name, mi2->name);
        }

        int main(int argc, char **argv)
        {
            int i;

            qsort(months, nr_of_months, sizeof(struct mi), compmi);
            for (i = 1; i < argc; i++) {
                struct mi key, *res;
                key.name = argv[i];
                res = bsearch(&key, months, nr_of_months,
                              sizeof(struct mi), compmi);
                if (res == NULL)
                    printf("'%s': unknown month\n", argv[i]);
                else
                    printf("%s: month #%d\n", res->name, res->nr);
            }

            exit(EXIT_SUCCESS);
        }
   @endcode
 *
 * @see compar_t
 */
//==============================================================================
extern void *bsearch(const void *key,
                     const void *base,
                     size_t      nmemb,
                     size_t      size,
                     compar_t    compar);

//==============================================================================
/**
 * @brief Function is implementation of quick sort algorithm.
 *
 * The qsort() function sorts an array with <i>nmemb</i> elements of size <i>size</i>.
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
 * @b Example
 * @code
        #include <stdio.h>
        #include <stdlib.h>
        #include <string.h>

        struct mi {
            int nr;
            char *name;
        } months[] = {
            { 1, "jan" }, { 2, "feb" }, { 3, "mar" }, { 4, "apr" },
            { 5, "may" }, { 6, "jun" }, { 7, "jul" }, { 8, "aug" },
            { 9, "sep" }, {10, "oct" }, {11, "nov" }, {12, "dec" }
        };

        const int nr_of_months = (sizeof(months)/sizeof(months[0]));

        static int compmi(const void *m1, const void *m2)
        {
            struct mi *mi1 = (struct mi *) m1;
            struct mi *mi2 = (struct mi *) m2;
            return strcmp(mi1->name, mi2->name);
        }

        int main(int argc, char **argv)
        {
            int i;

            qsort(months, nr_of_months, sizeof(struct mi), compmi);
            for (i = 1; i < argc; i++) {
                struct mi key, *res;
                key.name = argv[i];
                res = bsearch(&key, months, nr_of_months,
                              sizeof(struct mi), compmi);
                if (res == NULL)
                    printf("'%s': unknown month\n", argv[i]);
                else
                    printf("%s: month #%d\n", res->name, res->nr);
            }

            exit(EXIT_SUCCESS);
        }
   @endcode
 *
 * @see compar_t
 */
//==============================================================================
extern void qsort(void    *base,
                  size_t   nmemb,
                  size_t   size,
                  compar_t compar);

//==============================================================================
/**
 * @brief Function divides two integers.
 *
 * The div() function computes the value <i>numerator/denominator</i> and
 * returns the quotient and remainder in a structure named div_t that
 * contains two integer members (in unspecified order) named @b quot and
 * @b rem. The quotient is rounded toward zero.@n The result satisfies
 * <i>quot * denominator + rem = numerator</i>.
 *
 * @param numerator     a numerator
 * @param denominator   a denominator
 *
 * @return The result in the div_t structure.
 *
 * @b Example
 * @code
        // ...
        div_t = div(1, 2);
        // ...
   @endcode
 *
 * @see div_t
 */
//==============================================================================
extern div_t div(int numerator, int denominator);

//==============================================================================
/**
 * @brief Function divides two long integers.
 *
 * The ldiv() function computes the value <i>numerator/denominator</i> and
 * returns the quotient and remainder in a structure named <b>ldiv_t</b> that
 * contains two integer members (in unspecified order) named @b quot and
 * @b rem. The quotient is rounded toward zero.@n The result satisfies
 * <i>quot * denominator + rem = numerator</i>.<p>
 *
 * @param numerator     a numerator
 * @param denominator   a denominator
 *
 * @return The result in the ldiv_t structure.
 *
 * @b Example
 * @code
        // ...
        ldiv_t = ldiv(1, 2);
        // ...
   @endcode
 *
 * @see ldiv_t
 */
//==============================================================================
extern ldiv_t ldiv(long numerator, long denominator);

//==============================================================================
/**
 * @brief Function returns a pseudo-random integer.
 *
 * The rand() function returns a pseudo-random integer in the range 0 to
 * @ref RAND_MAX inclusive (i.e., the mathematical range [0, RAND_MAX]).
 *
 * @return The rand() function return a value between @b 0 and @ref RAND_MAX (inclusive).
 *
 * @b Example
 * @code
        // ...
        int foo = rand();
        // ...
   @endcode
 *
 * @see RAND_MAX
 */
//==============================================================================
extern int rand(void);

//==============================================================================
/**
 * @brief Function returns a pseudo-random integer.
 *
 * The rand() function returns a pseudo-random integer in the range 0 to
 * @ref RAND_MAX inclusive (i.e., the mathematical range [0, RAND_MAX]).
 *
 * @param  seed
 *
 * @return The rand() function return a value between @b 0 and @ref RAND_MAX (inclusive).
 *
 * @b Example
 * @code
        // ...
        unsigned int seed = 213;
        int foo = rand_r(&seed);
        // ...
   @endcode
 *
 * @see RAND_MAX
 */
//==============================================================================
extern int rand_r(unsigned int *seed);

//==============================================================================
/**
 * @brief Function sets the seed for rand() function.
 *
 * The srand() function sets its argument as the seed for a new sequence
 * of pseudo-random integers to be returned by rand().  These sequences
 * are repeatable by calling srand() with the same seed value.
 *
 * @param seed      new seed value
 *
 * @b Example
 * @code
        // ...
        #include <stdlib.h>
        #include <dnx/os.h>

        srand(get_tick_counter());
        int foo = rand();
        // ...
   @endcode
 */
//==============================================================================
extern void srand(unsigned int seed);

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief Function allocates memory block.
 *
 * The malloc() function allocates <i>size</i> bytes and returns a pointer to the
 * allocated memory. The memory is not initialized.  If size is 0, then malloc()
 * returns either @ref NULL.
 *
 * @param size      size bytes to allocate
 *
 * @exception | @ref ENOMEM
 *
 * @return Returns a pointer to the allocated memory, which is suitably aligned
 * for any built-in type.  On error, these functions return @ref NULL.  The
 * @ref NULL pointer may also be returned by a successful call to function with
 * a <i>size</i> of zero.
 *
 * @b Example
 * @code
        // ...
        int *buffer = malloc(100 * sizeof(int));
        // ...
   @endcode
 *
 * @see calloc(), realloc(), free()
 */
//==============================================================================
static inline void *malloc(size_t size)
{
        void *mem = NULL;
        syscall(SYSCALL_MALLOC, &mem, &size);
        return mem;
}

//==============================================================================
/**
 * @brief Function allocates memory block.
 *
 * The calloc() function allocates memory for an array of <i>n</i> elements
 * of <i>size</i> bytes each and returns a pointer to the allocated memory.
 * The memory is set to zero.
 *
 * @param n         number of elements
 * @param size      size of elements
 *
 * @exception | @ref ENOMEM
 *
 * @return Returns a pointer to the allocated memory, which is suitably aligned
 * for any built-in type.  On error, these functions return @ref NULL. The
 * @ref NULL pointer may also be returned by a successful call to function with
 * a <i>n</i> or <i>size</i> of zero.
 *
 * @b Example
 * @code
        // ...
        int *buffer = calloc(100, sizeof(int));
        // ...
   @endcode
 *
 * @see malloc(), realloc(), free()
 */
//==============================================================================
static inline void *calloc(size_t n, size_t size)
{
        void   *mem   = NULL;
        size_t  bsize = n * size;
        syscall(SYSCALL_ZALLOC, &mem, &bsize);
        return mem;
}

//==============================================================================
/**
 * @brief Function frees allocated memory block.
 *
 * The free() function frees the memory space pointed to by <i>ptr</i>, which
 * must have been returned by a previous call to malloc(), calloc(),
 * realloc(). If <i>ptr</i> is @ref NULL, no operation is performed.
 *
 * @param ptr       pointer to memory space to be freed
 *
 * @exception | @ref ESRCH
 * @exception | @ref ENOENT
 * @exception | @ref EFAULT
 * @exception | @ref EINVAL
 *
 * @b Example
 * @code
        // ...
        int *buffer = calloc(100, sizeof(int));
        if (buffer) {
                // ...

                free(buffer);
        }
        // ...
   @endcode
 *
 * @see malloc(), calloc(), realloc()
 */
//==============================================================================
static inline void free(void *ptr)
{
        if (ptr) {
                syscall(SYSCALL_FREE, NULL, ptr);
        }
}

//==============================================================================
/**
 * @brief Function changes the size of allocated memory block.
 *
 * The realloc() function changes the size of the memory block pointed
 * to by <i>ptr</i> to size bytes. The contents will be unchanged in the range
 * from the start of the region up to the minimum of the old and new
 * sizes.  If the new size is larger than the old size, the added memory
 * will not be initialized.  If <i>ptr</i> is @ref NULL, then the call is equivalent
 * to malloc(size), for all values of <i>size</i>; if <i>size</i> is equal to
 * zero, and <i>ptr</i> is not @ref NULL, then the call is equivalent to free(ptr).
 * Unless <i>ptr</i> is @ref NULL, it must have been returned by an earlier call to
 * malloc(), calloc() or realloc().  If the area pointed to was moved, a
 * free(ptr) is done.
 *
 * @param ptr       pointer to memory space
 * @param size      size of new memory space
 *
 * @exception | @ref ENOMEM
 * @exception | @ref ESRCH
 * @exception | @ref ENOENT
 * @exception | @ref EFAULT
 * @exception | @ref EINVAL
 *
 * @return The realloc() function returns a pointer to the newly allocated
 * memory, which is suitably aligned for any built-in type and may be
 * different from <i>ptr</i>, or @ref NULL if the request fails.  If <i>size</i>
 * was equal to 0, either @ref NULL or a pointer suitable to be passed to free()
 * is returned. If realloc() fails the original block is left untouched;
 * it is not freed or moved.
 *
 * @b Example
 * @code
        // ...
        int *buffer = realloc(NULL, 100);
        if (buffer) {
                // ...

                free(buffer);
        }
        // ...
   @endcode
 *
 * @see malloc(), calloc(), free()
 */
//==============================================================================
static inline void *realloc(void *ptr, size_t size)
{
        extern void* memcpy(void* dest, const void* src, size_t n);

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
 * @brief Function kills current program.
 *
 * The abort() function kills program which execute it.
 *
 * @return The abort() function never returns.
 *
 * @b Example
 * @code
        // ...
        abort();
        // no continuation...
   @endcode
 *
 * @see exit()
 */
//==============================================================================
static inline void abort(void)
{
        extern void _process_abort(struct _process*);
        _builtinfunc(process_abort, (struct _process*)_builtinfunc(task_get_tag, _THIS_TASK));
}

//==============================================================================
/**
 * @brief Function kills program with exit status.
 *
 * The exit() function kills program which execute it and program return
 * <i>status</i> value.
 *
 * @param status        status to be return by program
 *
 * @return The exit() function never returns.
 *
 * @b Example
 * @code
        // ...
        exit(EXIT_FAILURE);
        // no continuation...
   @endcode
 *
 * @see abort()
 */
//==============================================================================
static inline void exit(int status)
{
        extern void _process_exit(struct _process*, int);
        _builtinfunc(process_exit, (struct _process*)_builtinfunc(task_get_tag, _THIS_TASK), status);
        for (;;); // makes compiler happy
}

//==============================================================================
/**
 * @brief Function executes specified command.
 *
 * The system() executes a command specified in <i>command</i> and returns after
 * the command has been completed.
 *
 * @param command       command to execute
 *
 * @exception | ...
 *
 * @return Returns shell status if program was successfully executed.
 * On error -1 value is returned.
 *
 * @b Example
 * @code
        // ...
        int s = system("rm /foo/bar");
   @endcode
 */
//==============================================================================
extern int system(const char *command);

//==============================================================================
/**
 * @brief Function searches the environment variable.
 *
 * The getenv() function searches the environment list to find the environment
 * variable name, and returns a pointer to the corresponding value string.
 *
 * @note Function is not supported by dnx RTOS and always returns @ref NULL.
 *
 * @param name          variable name
 *
 * @return Returns a pointer to the value in the environment, or @ref NULL if
 * there is no match.
 *
 * @b Example
 * @code
        // ...
        char *var = getenv("foo");
        // ...
   @endcode
 */
//==============================================================================
static inline char *getenv(const char *name)
{
        (void) name;
        return NULL;
}

//==============================================================================
/**
 * @brief Function parses the list of suboptions.
 *
 * The getsubopt() parses the list of comma-separated suboptions.
 *
 * @note Function is not supported by dnx RTOS and always returns -1.
 *
 * @param optionp           option pointer
 * @param tokens            tokens to find
 * @param valuep            value pointer
 *
 * @return If the first suboption in <i>optionp</i> is recognized, getsubopt()
 * returns the index of the matching suboption element in <i>tokens</i>.
 * Otherwise, -1 is returned and <i>*valuep</i> is the entire <i>name[=value]</i>
 * string.
 *
 * @b Example
 * @code
        #include <stdlib.h>
        // ...
        char *tokens[] = {"HOME", "PATH", "LOGNAME", (char *) NULL };
        char *value;
        int opt, index;


        while ((opt = getopt(argc, argv, "e:")) != -1) {
            switch(opt)  {
            case 'e' :
                while ((index = getsubopt(&optarg, tokens, &value)) != -1) {
                    switch(index) {
        // ...
                }
                break;
        // ...
            }
        }
        ...
   @endcode
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
 * @brief Function converts string to integer.
 *
 * The atoi() function converts the initial portion of the string pointed
 * to by <i>str</i> to int. The behavior is the same as strtol(nptr, NULL, 10);
 * except that atoi() does not detect errors.
 *
 * @param str           string to convert
 *
 * @return The converted value.
 *
 * @b Example
 * @code
        // ...
        int val = atoi("125");
        // ...
   @endcode
 *
 * @see atol(), strtol(), atof(), strtod(), strtof()
 */
//==============================================================================
static inline int atoi(const char *str)
{
        return _builtinfunc(atoi, str);
}

//==============================================================================
/**
 * @brief Function converts string to long integer.
 *
 * The atol() function converts the initial portion of the string pointed
 * to by <i>str</i> to long. The behavior is the same as strtol(nptr, NULL, 10);
 * except that atol() does not detect errors.
 *
 * @param str           string to convert
 *
 * @return The converted value.
 *
 * @b Example
 * @code
        // ...
        int val = atol("125");
        // ...
   @endcode
 *
 * @see atoi(), strtol(), atof(), strtod(), strtof()
 */
//==============================================================================
static inline long atol(const char *str)
{
        return _builtinfunc(atoi, str);
}

//==============================================================================
/**
 * @brief Function converts string to 32-bit integer value.
 *
 * The strtol() function converts the initial part of the string in <i>nptr</i>
 * to a 32-bit value according to the given <i>base</i>, which must be
 * between 2 and 16 inclusive, or be the special value 0.
 *
 * The string may begin with an arbitrary amount of white space (as
 * determined by isspace()) followed by a single optional '+' or '-'
 * sign. If base is zero or 16, the number will be read in base 16; otherwise,
 * a zero base is taken as 10 (decimal) unless the next character is '0', in
 * which case it is taken as 8 (octal).<p>
 *
 * If <i>endptr</i> is not @ref NULL, strtol()stores the address of the first
 * invalid character in <i>*endptr</i>.  If there were no digits at all,
 * strtol() stores the original value of <i>nptr</i> in <i>*endptr</i> (and
 * returns 0).  In particular, if <i>*nptr</i> is not '\0' but <i>**endptr</i>
 * is '\0' on return, the entire string is valid.
 *
 * @param nptr          string to convert
 * @param endptr        points to first not converted character
 * @param base          calculation base
 *
 * @return The strtol() function returns the result of the conversion, unless
 * the value would underflow or overflow.
 *
 * @b Example
 * @code
        const char *end;
        const char *str;
        i32_t       val;

        // convert string to decimal value
        str = "123";
        val = strtol(str, &end, 10);

        // convert string to hex value
        str = "1FF";
        val = strtol(str, &end, 16);

        // convert string to octal
        str = "77"
        val = strtol(str, &end, 8);

        // convert string automatically
        str = "0x45";
        val = strtol(str, &end, 0);

        str = "056";
        val = strtol(str, &end, 0);

        str = "123";
        val = strtol(str, &end, 0);

        str = "0b1110";
        val = strtol(str, &end, 0);
   @endcode
 *
 * @see atoi(), atol(), atof(), strtod(), strtof()
 */
//==============================================================================
static inline i32_t strtol(const char *nptr, char **endptr, int base)
{
        i32_t result;
        char *end = _builtinfunc(strtoi, nptr, base, &result);
        if (endptr)
                *endptr = end;
        return result;
}

//==============================================================================
/**
 * @brief Function converts string to double.
 *
 * The atof() function converts the initial portion of the string pointed
 * to by <i>nptr</i> to double.  The behavior is the same as
 * strtod(nptr, NULL) except that atof() does not detect errors.
 *
 * @param nptr          string to convert
 *
 * @return The converted value.
 *
 * @b Example
 * @code
        // convert string to decimal value
        const char *str = "123.56";
        double      val = atof(str);
   @endcode
 *
 * @see atoi(), atol(), strtol(), strtod(), strtof()
 */
//==============================================================================
static inline double atof(const char *nptr)
{
        return _builtinfunc(atof, nptr);
}

//==============================================================================
/**
 * @brief Function converts string to double.
 *
 * The function convert the initial portion of the string pointed to by <i>nptr</i>
 * to double representation.
 *
 * @param nptr          string to convert
 * @param endptr        points to first not converted character
 *
 * @return These functions return the converted value, if any.
 *
 * @b Example
 * @code
        // convert string to decimal value
        const char *str = "123.56";
        double      val = strtod(str, NULL);
   @endcode
 *
 * @see atoi(), atol(), atof(), strtol(), strtof()
 */
//==============================================================================
static inline double strtod(const char *nptr, char **endptr)
{
        return _builtinfunc(strtod, nptr, endptr);
}

//==============================================================================
/**
 * @brief Function converts string to float.
 *
 * The function convert the initial portion of the string pointed to by <i>nptr</i>
 * to float representation.
 *
 * @param nptr          string to convert
 * @param endptr        points to first not converted character
 *
 * @return These functions return the converted value, if any.
 *
 * @b Example
 * @code
        // convert string to decimal value
        const char *str = "123.56";
        float       val = strtof(str, NULL);
   @endcode
 *
 * @see atoi(), atol(), atof(), strtol(), strtod()
 */
//==============================================================================
static inline float strtof(const char *nptr, char **endptr)
{
        return (float)_builtinfunc(strtof, nptr, endptr);
}

#ifdef __cplusplus
}
#endif

#endif /* _STDLIB_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
