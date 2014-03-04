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
#include "core/sysmoni.h"
#include "core/conv.h"
#include "core/progman.h"

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
typedef struct
{
        int quot;               /* quotient */
        int rem;                /* remainder */
} div_t;

typedef struct
{
        long quot;              /* quotient */
        long rem;               /* remainder */
} ldiv_t;

#ifndef __STRICT_ANSI__
typedef struct
{
        long long int quot;     /* quotient */
        long long int rem;      /* remainder */
} lldiv_t;
#endif

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern int      abs(int);
extern _PTR     bsearch(const _PTR, const _PTR, size_t, size_t, int (*_compar(const _PTR, const _PTR)));
extern div_t    div(int, int);
extern long     labs(long);
extern ldiv_t   ldiv(long __numer, long __denom);
extern _VOID    qsort(_PTR __base, size_t __nmemb, size_t __size, int(*_compar)(const _PTR, const _PTR));
extern int      rand(_VOID);
extern _VOID    srand(unsigned __seed);

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief void *malloc(size_t size)
 * The malloc() function allocates <i>size</i> bytes and returns a pointer to the
 * allocated memory. The memory is not initialized.  If size is 0, then malloc()
 * returns either NULL.
 *
 * @param size      size bytes to allocate
 *
 * @errors ENOMEM
 *
 * @return Return a pointer to the allocated memory, which is suitably aligned
 * for any built-in type.  On error, these functions return NULL.  NULL may
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
        return sysm_tskmalloc(size);
}

//==============================================================================
/**
 * @brief void *calloc(size_t n, size_t size)
 * The calloc() function allocates memory for an array of <i>n</i> elements
 * of <i>size</i> bytes each and returns a pointer to the allocated memory.
 * The memory is set to zero.
 *
 * @param n         number of elements
 * @param size      size of elements
 *
 * @errors ENOMEM
 *
 * @return Return a pointer to the allocated memory, which is suitably aligned
 * for any built-in type.  On error, these functions return NULL.  NULL may
 * also be returned by a successful call to function with a <i>n</i> or <i>size</i> of zero.
 *
 * @example
 * // ...
 * int *buffer = calloc(100, sizeof(int));
 * // ...
 */
//==============================================================================
static inline void *calloc(size_t n, size_t size)
{
        return sysm_tskcalloc(n, size);
}

//==============================================================================
/**
 * @brief void free(void *ptr)
 * The free() function frees the memory space pointed to by <i>ptr</i>, which
 * must have been returned by a previous call to malloc(), calloc(),
 * realloc(). If <i>ptr</i> is NULL, no operation is performed.
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
        sysm_tskfree(ptr);
}

//==============================================================================
/**
 * @brief void *realloc(void *ptr, size_t size)
 * The realloc() function changes the size of the memory block pointed
 * to by <i>ptr</i> to size bytes. The contents will be unchanged in the range
 * from the start of the region up to the minimum of the old and new
 * sizes.  If the new size is larger than the old size, the added memory
 * will not be initialized.  If <i>ptr</i> is NULL, then the call is equivalent
 * to malloc(size), for all values of <i>size</i>; if <i>size</i> is equal to
 * zero, and <i>ptr</i> is not NULL, then the call is equivalent to free(ptr).
 * Unless <i>ptr<i> is NULL, it must have been returned by an earlier call to
 * malloc(), calloc() or realloc().  If the area pointed to was moved, a
 * free(ptr) is done.
 *
 * @param ptr       pointer to memory space
 * @param size      size of new memory space
 *
 * @errors ENOMEM
 *
 * @return The realloc() function returns a pointer to the newly allocated
 * memory, which is suitably aligned for any built-in type and may be
 * different from <i>ptr</i>, or NULL if the request fails.  If <i>size</i>
 * was equal to 0, either NULL or a pointer suitable to be passed to free()
 * is returned. If realloc() fails the original block is left untouched;
 * it is not freed or moved.
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
static inline void *realloc(void *ptr, size_t size)
{
        if (!ptr) {
                return sysm_tskmalloc(size);
        } else {
                if (size == 0) {
                        sysm_tskfree(ptr);
                        return NULL;
                } else {
                        return ptr;
                }
        }
}

//==============================================================================
/**
 * @brief void abort(void)
 * The abort() function kills program which execute it.
 *
 * @param None
 *
 * @errors None
 *
 * @return The abort() function never returns.
 *
 * @example
 * // ...
 * abort();
 */
//==============================================================================
static inline void abort(void)
{
        _abort();
}

//==============================================================================
/**
 * @brief void exit(int status)
 * The exit() function kills program which execute it and program return
 * <i>status</i> value.
 *
 * @param status        status to be return by program
 *
 * @errors None
 *
 * @return The exit() function never returns.
 *
 * @example
 * // ...
 * exit(EXIT_FAILURE);
 */
//==============================================================================
static inline void exit(int status)
{
        _exit(status);
}

//==============================================================================
/**
 * @brief int system(const char *command)
 * The system() executes a command specified in <i>command</i> and returns after
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
        return _system(command);
}

//==============================================================================
/**
 * @brief char *getenv(const char *name)
 * The getenv() function searches the environment list to find the environment
 * variable name, and returns a pointer to the corresponding value string.<p>
 *
 * Function is not supported by dnx RTOS and always returns NULL.
 *
 * @param name          variable name
 *
 * @errors None
 *
 * @return Returns a pointer to the value in the environment, or NULL if
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
 * The getsubopt() parses the list of comma-separated suboptions.<p>
 *
 * Function is not supported by dnx RTOS and always returns -1.
 *
 * @param optionp           option pointer
 * @param tokens            tokens to find
 * @param valuep            value pointer
 *
 * @errors None
 *
 * @return If the first suboption in <i>optionp</i> is recognized, getsubopt()
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
 * The atoi() function converts the initial portion of the string pointed
 * to by <i>str</i> to int. The behavior is the same as <b>strtol</b>(nptr, NULL, 10);
 * except that atoi() does not detect errors.
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
        return sys_atoi(str);
}

//==============================================================================
/**
 * @brief int atol(const char *str)
 * The atol() function converts the initial portion of the string pointed
 * to by <i>str</i> to int. The behavior is the same as <b>strtol</b>(nptr, NULL, 10);
 * except that atoi() does not detect errors.
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
        return sys_atoi(str);
}


static inline i32_t strtol(const char *str, char **endptr, int base)
{
        i32_t result;
        char *end = sys_strtoi(str, base, &result);
        *endptr = end;
        return result;
}

static inline char *strtoi(const char *str, int base, i32_t *result)
{
        return sys_strtoi(str, base, result);
}

static inline double atof(const char *str)
{
        return sys_atof(str);
}

static inline double strtod(const char *str, char **end)
{
        return sys_strtod(str, end);
}

static inline float strtof(const char *str, char **end)
{
        return (float)sys_strtod(str, end);
}

#ifdef __cplusplus
}
#endif

#endif /* _STDLIB_H_ */
/*==============================================================================
  End of file
==============================================================================*/
