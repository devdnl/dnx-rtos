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
#include "core/systypes.h"
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
extern _PTR     memchr(const _PTR, int, size_t);
extern int      memcmp(const _PTR, const _PTR, size_t);
extern _PTR     memcpy(_PTR, const _PTR, size_t);
extern _PTR     memmove(_PTR, const _PTR, size_t);
extern _PTR     memset(_PTR, int, size_t);
extern char    *strcat(char *, const char *);
extern char    *strchr(const char *, int);
extern int      strcmp(const char *, const char *);
extern int      strcoll(const char *, const char *);
extern char    *strcpy(char *, const char *);
extern size_t   strcspn(const char *, const char *);
extern size_t   strlen(const char *);
extern char    *strncat(char *, const char *, size_t);
extern int      strncmp(const char *, const char *, size_t);
extern char    *strncpy(char *, const char *, size_t);
extern char    *strpbrk(const char *, const char *);
extern char    *strrchr(const char *, int);
extern size_t   strspn(const char *, const char *);
extern char    *strstr(const char *, const char *);

#ifndef _REENT_ONLY
extern char    *strtok(char *, const char *);
#endif

extern size_t   strxfrm(char *, const char *, size_t);

#ifndef __STRICT_ANSI__
extern int      bcmp(const void *, const void *, size_t);
extern void     bcopy(const void *, void *, size_t);
extern void     bzero(void *, size_t);
extern int      ffs(int);
extern char    *index(const char *, int);
extern _PTR     memccpy(_PTR, const _PTR, int, size_t);
extern _PTR     mempcpy(_PTR, const _PTR, size_t);
extern _PTR     memmem(const _PTR, size_t, const _PTR, size_t);
extern char    *rindex(const char *, int);
extern char    *stpcpy(char *, const char *);
extern char    *stpncpy(char *, const char *, size_t);
extern int      strcasecmp(const char *, const char *);
extern char    *strcasestr(const char *, const char *);
extern char    *strdup(const char *);
extern char    *_strdup_r(struct _reent *, const char *);
extern char    *strndup(const char *, size_t);
extern char    *_strndup_r(struct _reent *, const char *, size_t);
extern char    *strerror_r(int, char *, size_t);
extern size_t   strlcat(char *, const char *, size_t);
extern size_t   strlcpy(char *, const char *, size_t);
extern int      strncasecmp(const char *, const char *, size_t);
extern size_t   strnlen(const char *, size_t);
extern char    *strsep(char **, const char *);
extern char    *strlwr(char *);
extern char    *strupr(char *);
#endif /* ! __STRICT_ANSI__ */

/*==============================================================================
  Exported inline functions
==============================================================================*/
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
