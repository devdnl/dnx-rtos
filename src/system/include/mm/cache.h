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
/**
 * Cache modes.
 */
enum cache_mode {
        /**
         * Write-through cache directs write I/O onto cache and through to
         * underlying permanent storage before confirming I/O completion to
         * the host. This ensures data updates are safely stored on, for
         * example, a shared storage array, but has the disadvantage that I/O
         * still experiences latency based on writing to that storage.
         * Write-through cache is good for applications that write and then
         * re-read data frequently as data is stored in cache and results in
         * low read latency.
         */
        CACHE_WRITE_THROUGH,//!< CACHE_WRITE_THROUGH

        /**
         * Write-back cache is where write I/O is directed to cache and
         * completion is immediately confirmed to the host. This results in
         * low latency and high throughput for write-intensive applications,
         * but there is data availability exposure risk because the only copy
         * of the written data is in cache. As we will discuss later, suppliers
         * have added resiliency with products that duplicate writes. Users need
         * to consider whether write-back cache solutions offer enough
         * protection as data is exposed until it is staged to external storage.
         * Write-back cache is the best performing solution for mixed workloads
         * as both read and write I/O have similar response time levels.
         */
        CACHE_WRITE_BACK,   //!< CACHE_WRITE_BACK
};

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern int  sys_cache_write(FILE*, u32_t, size_t, const u8_t*, enum cache_mode);
extern int  sys_cache_read(FILE*, u32_t, size_t, u8_t*);
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
