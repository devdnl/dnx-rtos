/*=========================================================================*//**
File     cache.c

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

/*==============================================================================
  Include files
==============================================================================*/
#include <string.h>
#include "mm/cache.h"
#include "mm/mm.h"
#include "kernel/errno.h"
#include "kernel/kwrapper.h"
#include "kernel/kpanic.h"
#include "kernel/sysfunc.h"
#include "lib/cast.h"
#include "lib/unarg.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define ATOMIC for (int __ = 0; __ == 0;)\
        for (_kernel_scheduler_lock(); __ == 0; _kernel_scheduler_unlock(), __++)

#define container_of(ptr, type, member) (type *)( (void *)(ptr) - offsetof(type,member) )

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct mem_cache {
        struct mem_cache *next;         //!< should be the first
        struct mem_cache *prev;
        bool              insync;
        sys_cache_t       cache;        //!< should be the last
} mem_cache_t;

typedef struct {
        mem_cache_t *list_head;         //!< the smallest cache
        mem_cache_t *list_tail;         //!< the highest cache
        bool         sync_needed;       //!< FS synchronization needed to free dirty caches
} cache_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
#if __OS_SYSTEM_FS_CACHE_ENABLE__ > 0
static cache_t CACHE;
#endif

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
#if __OS_SYSTEM_FS_CACHE_ENABLE__ > 0
//==============================================================================
/**
 * Function add new cache to list.
 *
 * @param  memcache     cache object (system object).
 */
//==============================================================================
static void link_cache(mem_cache_t *memcache)
{
        if (CACHE.list_head == NULL) {
                CACHE.list_head = memcache;
                CACHE.list_tail = memcache;
        } else {
                mem_cache_t *mc = CACHE.list_head;
                while (mc) {
                        mem_cache_t *next = mc->next;

                        if (memcache->cache.size <= mc->cache.size) {
                                memcache->next = mc;
                                memcache->prev = mc->prev;

                                if (mc->prev) {
                                        mc->prev->next = memcache;
                                } else {
                                        CACHE.list_head = memcache;
                                }

                                mc->prev = memcache;

                                break;
                        }

                        if (next == NULL) {
                                mc->next        = memcache;
                                memcache->prev  = mc;
                                CACHE.list_tail = memcache;
                                break;
                        }

                        mc = next;
                }
        }
}
#endif

//==============================================================================
/**
 * Function allocate new cache object.
 *
 * @param  size         cache size
 * @param  object       user's object
 * @param  unlink       cache unlink function (user function called when cache is freeing)
 * @param  cache        pointer to cache pointer
 *
 * @return One of errno value.
 */
//==============================================================================
int sys_cache_alloc(size_t size, void *object, sys_cache_unlink_fn_t unlink, sys_cache_t **cache)
{
#if __OS_SYSTEM_FS_CACHE_ENABLE__ > 0
        int err = EINVAL;

        if (size && unlink && cache) {
                mem_cache_t *memcache;
                err = _kzalloc(_MM_CACHE, sizeof(mem_cache_t) + size, cast(void*, &memcache));
                if (!err) {
                        ATOMIC {
                                memcache->cache.object = object;
                                memcache->cache.unlink = unlink;
                                memcache->cache.size   = size;

                                link_cache(memcache);

                                printk("CACHE: created (%dB)", size); // TEST

                                *cache = &memcache->cache;
                        }
                }
        }

        return err;
#else
        UNUSED_ARG4(size, object, unlink, cache);
        return ENOTSUP;
#endif
}

//==============================================================================
/**
 * Function free selected cache.
 *
 * @param  cache        cache object to free.
 *
 * @return One of errno value.
 */
//==============================================================================
int sys_cache_free(sys_cache_t *cache)
{
#if __OS_SYSTEM_FS_CACHE_ENABLE__ > 0
        int err = EINVAL;

        if (cache) {
                mem_cache_t *memcache = container_of(cache, mem_cache_t, cache);

                printk("CACHE: freed (%dB)", cache->size); // TEST

                ATOMIC {
                        if (memcache == CACHE.list_head) {
                                CACHE.list_head = memcache->next;
                        }

                        if (memcache == CACHE.list_tail) {
                                CACHE.list_tail = memcache->prev;
                        }

                        if (memcache->prev) {
                                memcache->prev->next = memcache->next;
                        }

                        if (memcache->next) {
                                memcache->next->prev = memcache->prev;
                        }

                        memset(memcache, 0, sizeof(mem_cache_t));
                }

                err = _kfree(_MM_CACHE, cast(void*, &memcache));
                if (err) {
                        _kernel_panic_report(_KERNEL_PANIC_DESC_CAUSE_INTERNAL);
                }
        }

        return err;
#else
        UNUSED_ARG1(cache);
        return ENOTSUP;
#endif
}

//==============================================================================
/**
 * Function reduce number of allocated caches if more free memory is needed.
 *
 * @param  size         size to reduce
 */
//==============================================================================
void sys_cache_reduce(size_t size)
{
#if __OS_SYSTEM_FS_CACHE_ENABLE__ > 0
        i32_t to_reduce = size;
        bool  dirty     = false;

        // This algorithm try to find the cache that is equal or bigger than required
        // space by memory management. Freed are only not dirty caches.
        ATOMIC {
                mem_cache_t *memcache = CACHE.list_head;
                while (memcache) {
                        mem_cache_t *next = memcache->next;

                        if (memcache->cache.size >= size && !memcache->cache.dirty) {
                                int err = memcache->cache.unlink(&memcache->cache);
                                if (!err) {
                                        to_reduce -= memcache->cache.size + sizeof(mem_cache_t);
                                        sys_cache_free(&memcache->cache);

                                        if (to_reduce <= 0) {
                                                printk("CACHE: reduced (%dB)", size - to_reduce); // TEST
                                                break;
                                        }
                                }
                        } else {
                                dirty |= memcache->cache.dirty;
                        }

                        memcache = next;
                }
        }

        // Function return if required space is reached. Starting next algorithm is
        // not needed.
        if (to_reduce <= 0) {
                CACHE.sync_needed = false;
                return;
        }

        // This algorithm try to free all the biggest caches to meet memory space
        // requirement. Freed are only not dirty caches.
        ATOMIC {
                mem_cache_t *memcache = CACHE.list_tail;
                while (memcache) {
                        mem_cache_t *prev = memcache->prev;

                        if (!memcache->cache.dirty) {
                                int err = memcache->cache.unlink(&memcache->cache);
                                if (!err) {
                                        to_reduce -= memcache->cache.size + sizeof(mem_cache_t);
                                        sys_cache_free(&memcache->cache);

                                        if (to_reduce <= 0) {
                                                printk("CACHE: reduced (%dB)", size - to_reduce); // TEST
                                                break;
                                        }
                                }
                        } else {
                                dirty |= memcache->cache.dirty;
                        }

                        memcache = prev;
                }
        }

        // There is still not enough space so system try to synchronize
        // all file systems.
        CACHE.sync_needed = (to_reduce > 0 && dirty);


        if (CACHE.sync_needed) { // TEST
                printk("CACHE: sync needed", to_reduce); // TEST
        }
#else
        UNUSED_ARG1(size);
#endif
}

//==============================================================================
/**
 * Function return synchronization required flag.
 *
 * @return Synchronization flag.
 */
//==============================================================================
bool sys_cache_is_sync_needed(void)
{
#if __OS_SYSTEM_FS_CACHE_ENABLE__ > 0
        return CACHE.sync_needed;
#else
        return false;
#endif
}

/*==============================================================================
  End of file
==============================================================================*/
