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
#include "core/systypes.h"
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
static inline void *malloc(size_t size)
{
        return sysm_tskmalloc(size);
}

static inline void *calloc(size_t n, size_t size)
{
        return sysm_tskcalloc(n, size);
}

static inline void free(void *mem)
{
        sysm_tskfree(mem);
}

static inline void *realloc(void *mem, size_t size)
{
        (void) size;

        return mem;
}

static inline void *reallocf(void *mem, size_t size)
{
        (void) size;

        free(mem);
        return NULL;
}

static inline void abort(void)
{
        _abort();
}

static inline void exit(int status)
{
        _exit(status);
}

static inline int system(const char *str)
{
        return _system(str);
}

static inline char *getenv(const char *name)
{
        (void) name;
        return NULL;
}

static inline int getsubopt(char **optionp, char *const *tokens, char **valuep)
{
        (void) optionp;
        (void) tokens;
        (void) valuep;

        return -1;
}

static inline int atoi(const char *str)
{
        return sys_atoi(str);
}

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
