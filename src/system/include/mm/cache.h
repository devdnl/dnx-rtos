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

#ifndef _MM_CACHE_H_
#define _MM_CACHE_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "fs/vfs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern int  sys_cache_write(FILE*, u32_t, size_t, const u8_t*, bool);
extern int  sys_cache_read(FILE*, u32_t, size_t, u8_t*, bool);
extern int  _cache_init(void);
extern void _cache_sync(void);
extern void _cache_drop(void);
extern void _cache_reduce(size_t);
extern bool _cache_is_sync_needed(void);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _MM_CACHE_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
