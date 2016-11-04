/*=========================================================================*//**
File     shm.c

Author   Daniel Zorychta

Brief    Shared memory management.

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
#include "kernel/shm.h"
#include "kernel/ktypes.h"
#include "kernel/errno.h"
#include "kernel/kwrapper.h"
#include "lib/cast.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define ATOMIC for (int __ = 0; __ == 0;)\
        for (_kernel_scheduler_lock(); __ == 0; _kernel_scheduler_unlock(), __++)

#define foreach_region(_v, _l)\
        for (shm_region_t *_v = _l; _v; _v = _v->next)

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct shm_region {
        struct shm_region *next;
        char               name[12];
        int16_t            attached;
        size_t             size;
        uint8_t            blk[];
} shm_region_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
static shm_region_t *shm_list;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * Function create shared memory region.
 *
 * @param key   memory region ID
 * @param size  memory region size
 *
 * @return One of errno value.
 */
//==============================================================================
int _shm_create(const char *key, size_t size)
{
        int err = EINVAL;

        if ((key != NULL) && (key[0] != '\0') && (size > 0)) {
                err = ESUCC;

                ATOMIC {
                        foreach_region(region, shm_list) {
                                if (strncmp(region->name, key, sizeof(region->name)) == 0) {
                                        err = EEXIST;
                                }
                        }

                        if (!err) {
                                shm_region_t *region = NULL;
                                err = _kzalloc(_MM_SHM, sizeof(shm_region_t) + size, cast(void *, &region));

                                if (!err) {
                                        region->size = size;
                                        strncpy(region->name, key, sizeof(region->name));
                                        region->next = shm_list;
                                        shm_list = region;
                                }
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * Function destroy shared memory region.
 *
 * @param key   memory region ID
 *
 * @return One of errno value.
 */
//==============================================================================
int _shm_destroy(const char *key)
{
        int err = EINVAL;

        if ((key != NULL) && (key[0] != '\0')) {
                err = ENOENT;

                ATOMIC {
                        shm_region_t *prev = NULL;
                        foreach_region(region, shm_list) {
                                if (strncmp(region->name, key, sizeof(region->name)) == 0) {
                                        if (region->attached == 0) {
                                                if (prev) {
                                                        prev->next = region->next;
                                                } else {
                                                        shm_list = region->next;
                                                }

                                                err = _kfree(_MM_SHM, cast(void*, &region));

                                        } else {
                                                err = EADDRINUSE;
                                        }

                                        break;
                                }

                                prev = region;
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * Function return region parameters.
 *
 * @param key   memory region ID
 * @param mem   memory region address
 * @param size  memory region size
 *
 * @return One of errno value.
 */
//==============================================================================
int _shm_at(const char *key, void **mem, size_t *size)
{
        int err = EINVAL;

        if ((key != NULL) && (key[0] != '\0') && mem && size) {
                err = ENOENT;

                ATOMIC {
                        foreach_region(region, shm_list) {
                                if (strncmp(region->name, key, sizeof(region->name)) == 0) {
                                        region->attached++;
                                        *mem  = region->blk;
                                        *size = region->size;
                                        err   = ESUCC;
                                }
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * Function detach region.
 *
 * @param key   memory region ID
 *
 * @return One of errno value.
 */
//==============================================================================
int _shm_detach(const char *key)
{
        int err = EINVAL;

        if ((key != NULL) && (key[0] != '\0')) {
                err = ENOENT;

                ATOMIC {
                        foreach_region(region, shm_list) {
                                if (strncmp(region->name, key, sizeof(region->name)) == 0) {
                                        if (region->attached > 0) {
                                                region->attached--;
                                        }

                                        err = ESUCC;
                                }
                        }
                }
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
