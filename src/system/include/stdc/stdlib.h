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

/*==============================================================================
  Exported macros
==============================================================================*/
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

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


///** MEMORY MANAGEMENT DEFINTIONS */
//#ifndef malloc
//#define malloc(size_t__size)                                    sysm_tskmalloc(size_t__size)
//#endif
//
//#ifndef calloc
//#define calloc(size_t__nitems, size_t__isize)                   sysm_tskcalloc(size_t__nitems, size_t__isize)
//#endif
//
//#ifndef free
//#define free(void__pmem)                                        sysm_tskfree(void__pmem)
//#endif



#ifdef __cplusplus
}
#endif

#endif /* _STDLIB_H_ */
/*==============================================================================
  End of file
==============================================================================*/
