/*=========================================================================*//**
File     cache.c

Author   Daniel Zorychta

Brief    Cache management.

         Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#define cache_buf(cache)        cache[1]
#define MTX_TIMEOUT             MAX_DELAY_MS

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct cache {
        struct cache       *next;               //!< next cache object
        struct cache       *prev;               //!< previous cache object
        dev_t               dev;                //!< device ID (final medium)
        i32_t               temp;               //!< cache temperature
        u32_t               pos;                //!< file position (block number)
        size_t              size;               //!< block size
        bool                dirty;              //!< cache is dirty
        u8_t                buf[];              //!< block data
} cache_t;

typedef struct {
        cache_t            *list_head;          //!< the smallest cache
        mutex_t            *list_mtx;           //!< protection mutex
        bool                sync_needed;        //!< FS synchronization needed to free dirty caches
} cache_man_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
#if __OS_SYSTEM_FS_CACHE_ENABLE__ > 0
static cache_man_t cman;
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
 * @brief Function allocate new cache object and add to list.
 *
 * @param  dev          device
 * @param  blkpos       block position
 * @param  blksz        block size
 * @param  cache        pointer to cache object
 *
 * @return One of errno value.
 */
//==============================================================================
static int cache_alloc(dev_t dev, u32_t blkpos, size_t blksz, cache_t **cache)
{
        size_t free = _mm_get_mem_free();
        if (free < __OS_SYSTEM_CACHE_MIN_FREE__) {
                return ENOMEM;
        }

        int err = _kzalloc(_MM_CACHE, sizeof(cache_t) + blksz, cast(void*, cache));
        if (!err) {
                (*cache)->dev  = dev;
                (*cache)->pos  = blkpos;
                (*cache)->size = blksz;
                (*cache)->next = cman.list_head;

                if ((*cache)->next) {
                        (*cache)->next->prev = *cache;
                }

                cman.list_head = *cache;

                printk("CACHE: created (%d B)", blksz);
        }

        return err;
}

//==============================================================================
/**
 * @brief Function free selected cache and remove from list.
 *
 * @param  cache        cache object to free.
 *
 * @return One of errno value.
 */
//==============================================================================
static int cache_free(cache_t *cache)
{
        int err = EINVAL;

        if (cache) {
                printk("CACHE: freed (%d B)", cache->size);

                if (cache == cman.list_head) {
                        cman.list_head = cache->next;

                        if (cman.list_head) {
                                cman.list_head->prev = NULL;
                        }

                } else {
                        if (cache->next) {
                                cache->next->prev = cache->prev;
                        }

                        if (cache->prev) {
                                cache->prev->next = cache->next;
                        }
                }

                memset(cache, 0, sizeof(cache_t));

                err = _kfree(_MM_CACHE, cast(void*, &cache));
                if (err) {
                        _kernel_panic_report(_KERNEL_PANIC_DESC_CAUSE_INTERNAL);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Function search cache of selected parameters in list.
 *        Only files that are linked directly to drivers are supported. Regular
 *        files are not supported because can be buffered by parent file system.
 *
 * @param  dev          device
 * @param  blkpos       block position
 * @param  cache        cache result
 *
 * @return One of errno value.
 */
//==============================================================================
static int cache_find(dev_t dev, u32_t blkpos, cache_t **cache)
{
        int err = ENOENT;

        cache_t *c = cman.list_head;
        while (c) {
                cache_t *next = c->next;

                if ((c->dev == dev) && (c->pos == blkpos)) {

                        *cache = c;
                        err    = ESUCC;
                        break;
                }

                c = next;
        }

        return err;
}

//==============================================================================
/**
 * @brief Function analyze caches and find the lowest temperature of cache and
 *        calculate how many this caches is. Function calculate count of dirty
 *        caches and total number of caches.
 *
 * @param  tmin         minimal temperature
 * @param  tmincnt      number of caches of minimal temperature
 * @param  dirty        number of dirty caches
 * @param  count        number of all caches
 */
//==============================================================================
static void get_cache_stats(i32_t *tmin, size_t *tmincnt, size_t *dirty, size_t *count)
{
        *tmin    = INT32_MAX;
        *tmincnt = 0;
        *dirty   = 0;
        *count   = 0;

        cache_t *cache = cman.list_head;
        while (cache) {
                if (!cache->dirty) {
                        if (cache->temp < *tmin) {
                                *tmin    = cache->temp;
                                *tmincnt = 1;

                        } else if (cache->temp == *tmin) {
                                *tmincnt += 1;
                        }
                } else {
                        *dirty += 1;
                }

                *count += 1;
                cache   = cache->next;
        }
}

//==============================================================================
/**
 * @brief Function write block to selected device. If cache exist then block is
 *        write to the cache. If cache does not exist then new one is created.
 *        When CACHE_WRITE_THROUGH is used then data is write both to the cache
 *        and device.
 *
 * @param  dev          block device
 * @param  blkpos       block position
 * @param  blksz        block size
 * @param  blkcnt       block count
 * @param  buf          buffer to write from (blocks)
 * @param  mode         write mode
 *
 * @return One of errno value.
 */
//==============================================================================
static int _cache_write(dev_t dev, u32_t blkpos, size_t blksz, size_t blkcnt, const u8_t *buf, enum cache_mode mode)
{
        int err = ESUCC;

        if (mode == CACHE_WRITE_THROUGH) {
                fpos_t fpos  = cast(fpos_t, blkpos) * blksz;
                size_t wrcnt = 0;
                size_t wrsz  = blksz * blkcnt;
                struct vfs_fattr fattr = {false, false};

                err = _driver_write(dev, buf, wrsz, &fpos, &wrcnt, fattr);

                if (!err && (wrcnt != wrsz) ) {
                        err = EIO;
                }
        }

        while (!err && blkcnt--) {
                cache_t *cache = NULL;

                err = _mutex_lock(cman.list_mtx, MTX_TIMEOUT);
                if (!err) {

                        if (cache_find(dev, blkpos, &cache) != ESUCC) {

                                if (cache_alloc(dev, blkpos, blksz, &cache) != ESUCC) {
                                        cache = NULL;
                                }
                        }

                        if (cache) {
                                memcpy(&cache_buf(cache), buf, blksz);
                                cache->dirty = true;
                                cache->temp++;
                        }

                        _mutex_unlock(cman.list_mtx);
                }

                if (!err && !cache && mode != CACHE_WRITE_THROUGH) {
                        fpos_t fpos  = cast(fpos_t, blkpos) * blksz;
                        size_t wrcnt = 0;
                        struct vfs_fattr fattr = {false, false};

                        err = _driver_write(dev, buf, blksz, &fpos, &wrcnt, fattr);

                        if (!err && wrcnt != blksz) {
                                err = EIO;
                        }
                }

                blkpos++;
                buf += blksz;
        }

        return err;
}

//==============================================================================
/**
 * @brief Function read block from selected device. If cache exist then cache
 *        data is used. If cache does not exist then file is read and new cache
 *        is created.
 *
 * @param  dev          block dev
 * @param  blkpos       block position
 * @param  blksz        block size
 * @param  blkcnt       block count
 * @param  buf          buffer to read (blocks)
 *
 * @return One of errno value.
 */
//==============================================================================
static int _cache_read(dev_t dev, u32_t blkpos, size_t blksz, size_t blkcnt, u8_t *buf)
{
        int err = ESUCC;

        while (!err && blkcnt--) {
                cache_t *cache = NULL;

                err = _mutex_lock(cman.list_mtx, MTX_TIMEOUT);
                if (!err) {

                        if (cache_find(dev, blkpos, &cache) == ESUCC) {
                                memcpy(buf, &cache_buf(cache), blksz);
                                cache->temp++;

                        } else {
                                fpos_t fpos  = cast(fpos_t, blkpos) * blksz;
                                size_t rdcnt = 0;
                                struct vfs_fattr fattr = {false, false};

                                err = _driver_read(dev, buf, blksz, &fpos, &rdcnt, fattr);

                                if (!err && rdcnt != blksz) {
                                        err = EIO;
                                }

                                if (!err) {
                                        if (cache_alloc(dev, blkpos, blksz, &cache) == ESUCC) {
                                                memcpy(&cache_buf(cache), buf, blksz);
                                        }
                                }
                        }

                        _mutex_unlock(cman.list_mtx);
                }

                blkpos++;
                buf += blksz;
        }

        return err;
}
#endif

//==============================================================================
/**
 * @brief Function initialize cache subsystem.
 *
 * @return One of errno value.
 */
//==============================================================================
int _cache_init(void)
{
#if __OS_SYSTEM_FS_CACHE_ENABLE__ > 0
        return _mutex_create(MUTEX_TYPE_NORMAL, &cman.list_mtx);
#else
        return ESUCC;
#endif
}

//==============================================================================
/**
 * @brief Function synchronize all dirty caches. Function must be called from
 *        system thread that is prepared for file system handling.
 */
//==============================================================================
void _cache_sync(void)
{
#if __OS_SYSTEM_FS_CACHE_ENABLE__ > 0
        int err = _mutex_lock(cman.list_mtx, MTX_TIMEOUT);
        if (!err) {
                u16_t sync_cnt = 0;

                cache_t *cache = cman.list_head;

                while (cache) {
                        cache_t *next = cache->next;

                        if (cache->dirty) {

                                fpos_t fpos  = cast(fpos_t, cache->pos) * cache->size;
                                size_t wrcnt = 0;
                                struct vfs_fattr fattr = {false, false};

                                int e = _driver_write(cache->dev,
                                                      cast(const u8_t*, &cache_buf(cache)),
                                                      cache->size, &fpos, &wrcnt, fattr);
                                if (e) {
                                        printk("CACHE: sync error %d [%d:%d:%d]", e,
                                               _dev_t__extract_modno(cache->dev),
                                               _dev_t__extract_major(cache->dev),
                                               _dev_t__extract_minor(cache->dev));
                                } else {
                                        cache->dirty = false;
                                }

                                sync_cnt++;
                        }

                        cache->temp--;

                        cache = next;
                }

                cman.sync_needed = false;

                _mutex_unlock(cman.list_mtx);

                if (sync_cnt) {
                        printk("CACHE: synchronized %d blocks", sync_cnt);
                }
        }
#endif
}

//==============================================================================
/**
 * @brief Function drop all clear caches.
 */
//==============================================================================
void _cache_drop(void)
{
#if __OS_SYSTEM_FS_CACHE_ENABLE__ > 0
        int err = _mutex_lock(cman.list_mtx, MTX_TIMEOUT);
        if (!err) {
                u16_t dropped = 0;

                cache_t *cache = cman.list_head;

                while (cache) {
                        cache_t *next = cache->next;

                        if (!cache->dirty) {
                                cache_free(cache);
                                dropped++;
                        }

                        cache = next;
                }

                _mutex_unlock(cman.list_mtx);

                printk("CACHE: dropped %d blocks", dropped);
        }
#endif
}

//==============================================================================
/**
 * @brief Function reduce number of allocated caches if more free memory is needed.
 *
 * @param  size         size to reduce
 */
//==============================================================================
void _cache_reduce(size_t size)
{
#if __OS_SYSTEM_FS_CACHE_ENABLE__ > 0
        i32_t to_reduce = size;

        int err = _mutex_lock(cman.list_mtx, MTX_TIMEOUT);
        if (!err) {

                if (cman.list_head && to_reduce > 0) {

                        // Algorithm find caches that have the lowest
                        // temperature that means there are used very rare.
                        i32_t  tmin;
                        size_t tmincnt, dirty, count;
                        get_cache_stats(&tmin, &tmincnt, &dirty, &count);

                        while (to_reduce > 0 && dirty < count) {

                                cache_t *cache = cman.list_head;
                                while (cache && tmincnt > 0) {
                                        cache_t *next = cache->next;

                                        if (!cache->dirty && cache->temp == tmin) {
                                                to_reduce -= cache->size + sizeof(cache_t);
                                                cache_free(cache);
                                                tmincnt--;

                                                if (to_reduce <= 0) {
                                                        break;
                                                }
                                        }

                                        cache = next;
                                }

                                if (to_reduce > 0) {
                                        get_cache_stats(&tmin, &tmincnt, &dirty, &count);
                                }
                        }

                        // There is still not enough space so system try to
                        // synchronize all caches.
                        cman.sync_needed = (to_reduce > 0 && dirty > 0);

                        if (cman.sync_needed) {
                                printk("CACHE: sync needed", to_reduce);
                        }
                }

                _mutex_unlock(cman.list_mtx);
        }
#else
        UNUSED_ARG1(size);
#endif
}

//==============================================================================
/**
 * @brief Function return synchronization required flag.
 *
 * @return Synchronization flag.
 */
//==============================================================================
bool _cache_is_sync_needed(void)
{
#if __OS_SYSTEM_FS_CACHE_ENABLE__ > 0
        return cman.sync_needed;
#else
        return false;
#endif
}

//==============================================================================
/**
 * @brief  Function drop cache of selected device (sync on dirty pages).
 *         Function try to synchronize and drop cache of selected device file.
 *         If selected file is a regular file then operation is not continued
 *         because regular files are not cached directly. When file is a device
 *         file then cache is synchronized with storage and dropped from memory.
 *
 * @param  file         file to synchronize.
 *
 * @return One of errno value.
 */
//==============================================================================
int sys_cache_drop(FILE *file)
{
        int err = ESUCC;

#if __OS_SYSTEM_FS_CACHE_ENABLE__ > 0
        struct stat stat;
        err = _vfs_fstat(file, &stat);
        if (!err) {
                if (stat.st_type != FILE_TYPE_DRV) {
                        return ESUCC;
                }
        } else {
                return err;
        }

        err = _mutex_lock(cman.list_mtx, MTX_TIMEOUT);
        if (!err) {
                cache_t *cache = cman.list_head;

                while (!err && cache) {
                        cache_t *next = cache->next;

                        if (cache->dev == stat.st_dev) {

                                if (cache->dirty) {

                                        fpos_t fpos  = cast(fpos_t, cache->pos) * cache->size;
                                        size_t wrcnt = 0;
                                        struct vfs_fattr fattr = {false, false};

                                        err = _driver_write(cache->dev,
                                                            cast(const u8_t*, &cache_buf(cache)),
                                                            cache->size, &fpos, &wrcnt, fattr);
                                        if (err) {
                                                printk("CACHE: sync error %d [%d:%d:%d]", err,
                                                       _dev_t__extract_modno(cache->dev),
                                                       _dev_t__extract_major(cache->dev),
                                                       _dev_t__extract_minor(cache->dev));
                                        } else {
                                                cache->dirty = false;
                                        }
                                }

                                if (!err) {
                                        cache_free(cache);
                                }
                        }

                        cache = next;
                }

                _mutex_unlock(cman.list_mtx);
        }
#else
        UNUSED_ARG1(file);
#endif
        return err;
}

//==============================================================================
/**
 * @brief Function write block to selected file. If cache exist then block is
 *        write to the cache. If cache does not exist then new one is created.
 *        Only files that are linked with drivers are cached, other files are
 *        written directly.
 *
 * @param  file         file to write
 * @param  blkpos       block position
 * @param  blksz        block size
 * @param  blkcnt       block count
 * @param  buf          buffer to write from (blocks)
 * @param  mode         write mode
 *
 * @return One of errno value.
 */
//==============================================================================
int sys_cache_write(FILE *file, u32_t blkpos, size_t blksz, size_t blkcnt, const u8_t *buf, enum cache_mode mode)
{
        if (!file || !blksz || !blkcnt || !buf) {
                return EINVAL;
        }

        struct stat stat;
        int err = _vfs_fstat(file, &stat);
        if (!err) {
                if (stat.st_type == FILE_TYPE_DRV) {
#if __OS_SYSTEM_FS_CACHE_ENABLE__ > 0
                        err = _cache_write(stat.st_dev, blkpos, blksz, blkcnt, buf, mode);
#else
                        UNUSED_ARG1(mode);

                        fpos_t fpos  = cast(fpos_t, blkpos) * blksz;
                        size_t wrcnt = 0;
                        size_t wrsz  = blksz * blkcnt;
                        struct vfs_fattr fattr = {false, false};

                        err = _driver_write(stat.st_dev, buf, wrsz, &fpos, &wrcnt, fattr);

                        if (!err && (wrcnt != wrsz) ) {
                                err = EIO;
                        }
#endif
                } else {
                        err = _vfs_fseek(file, cast(i64_t, blkpos) * blksz, VFS_SEEK_SET);
                        if (!err) {
                                size_t wrcnt = 0;
                                size_t wrsz  = blksz * blkcnt;
                                err = _vfs_fwrite(buf, wrsz, &wrcnt, file);

                                if (!err && (wrcnt != wrsz) ) {
                                        err = EIO;
                                }
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Function read block from selected file. If cache exist then cache
 *        data is used. If cache does not exist then file is read and new cache
 *        is created. Function does not cache blocks from files that are not
 *        directly connected do drivers.
 *
 * @param  file         file to read
 * @param  blkpos       block position
 * @param  blksz        block size
 * @param  blkcnt       block count
 * @param  buf          buffer to read (blocks)
 *
 * @return One of errno value.
 */
//==============================================================================
int sys_cache_read(FILE *file, u32_t blkpos, size_t blksz, size_t blkcnt, u8_t *buf)
{
        if (!file || !blksz || !blkcnt || !buf) {
                return EINVAL;
        }

        struct stat stat;
        int err = _vfs_fstat(file, &stat);
        if (!err) {
                if (stat.st_type == FILE_TYPE_DRV) {
#if __OS_SYSTEM_FS_CACHE_ENABLE__ > 0
                        err = _cache_read(stat.st_dev, blkpos, blksz, blkcnt, buf);
#else
                        fpos_t fpos  = cast(fpos_t, blkpos) * blksz;
                        size_t rdcnt = 0;
                        size_t rdsz  = blksz * blkcnt;
                        struct vfs_fattr fattr = {false, false};

                        err = _driver_read(stat.st_dev, buf, rdsz, &fpos, &rdcnt, fattr);

                        if (!err && (rdcnt != rdsz)) {
                                err = EIO;
                        }
#endif
                } else {
                        err = _vfs_fseek(file, cast(i64_t, blkpos) * blksz, VFS_SEEK_SET);
                        if (!err) {
                                size_t rdcnt = 0;
                                size_t rdsz  = blksz * blkcnt;
                                err = _vfs_fread(buf, rdsz, &rdcnt, file);

                                if (!err && (rdcnt != rdsz)) {
                                        err = EIO;
                                }
                        }
                }
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
