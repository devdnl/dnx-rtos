/*=========================================================================*//**
File     cache.h

Author   Daniel Zorychta

Brief    Cache management.

         Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/**
@defgroup CACHE_H_ CACHE_H_

Detailed Doxygen description.
*/
/**@{*/

#ifndef _CACHE_H_
#define _CACHE_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
struct sys_cache;

typedef int (*sys_cache_unlink_fn_t)(struct sys_cache*);

typedef struct sys_cache {
        void                 *next;
        void                 *prev;
        void                 *object;
        sys_cache_unlink_fn_t unlink;
        bool                  dirty;
        size_t                size;
        uint8_t               buffer[];
} sys_cache_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern int  sys_cache_alloc(size_t, void*, sys_cache_unlink_fn_t, sys_cache_t**);
extern int  sys_cache_free(sys_cache_t*);
extern void sys_cache_reduce(size_t);
extern bool sys_cache_is_sync_needed(void);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _CACHE_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
