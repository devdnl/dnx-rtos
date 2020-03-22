/*=========================================================================*//**
File     shm.c

Author   Daniel Zorychta

Brief    Shared memory management.

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
#include <stdbool.h>
#include "mm/shm.h"
#include "kernel/ktypes.h"
#include "kernel/errno.h"
#include "kernel/kwrapper.h"
#include "lib/cast.h"
#include "dnx/misc.h"

#if __OS_ENABLE_SHARED_MEMORY__ > 0

/*==============================================================================
  Local macros
==============================================================================*/
#define foreach_region(_v, _l)\
        for (shm_region_t *_v = _l; _v; _v = _v->next)

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct pid_list {
        struct pid_list *next;
        pid_t            pid[6];
} pid_list_t;

typedef struct shm_region {
        struct shm_region *next;
        char               name[12];
        size_t             size;
        pid_list_t         attached_pids;
        uint8_t            blk[];
} shm_region_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int  attach_pid(shm_region_t *region, pid_t pid);
static int  detach_pid(shm_region_t *region, pid_t pid);
static bool is_pid_attached(const shm_region_t *region, pid_t pid);
static bool is_pid_list_empty(const shm_region_t *region);

/*==============================================================================
  Local objects
==============================================================================*/
static struct {
        shm_region_t *list;
        mutex_t      *mtx;
} SHM;

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
 * @brief  Function initialize shared memory subsystem.
 *
 * @return One of errno value.
 */
//==============================================================================
int _shm_init(void)
{
        return _mutex_create(MUTEX_TYPE_NORMAL, &SHM.mtx);
}

//==============================================================================
/**
 * @brief Function create shared memory region.
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

        if (!isstrempty(key) && (size > 0)) {
                err = _mutex_lock(SHM.mtx, MAX_DELAY_MS);
                if (!err) {
                        foreach_region(region, SHM.list) {
                                if (isstreqn(region->name, key, sizeof(region->name))) {
                                        err = EEXIST;
                                        break;
                                }
                        }

                        if (!err) {
                                shm_region_t *region = NULL;
                                err = _kzalloc(_MM_SHM, sizeof(shm_region_t) + size, cast(void *, &region));

                                if (!err) {
                                        region->size = size;
                                        strncpy(region->name, key, sizeof(region->name));
                                        region->next = SHM.list;
                                        SHM.list = region;
                                }
                        }

                        _mutex_unlock(SHM.mtx);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Function destroy shared memory region.
 *
 * @param key   memory region ID
 *
 * @return One of errno value.
 */
//==============================================================================
int _shm_destroy(const char *key)
{
        int err = EINVAL;

        if (!isstrempty(key)) {
                err = _mutex_lock(SHM.mtx, MAX_DELAY_MS);
                if (!err) {
                        err = ENOENT;

                        shm_region_t *prev = NULL;

                        foreach_region(region, SHM.list) {

                                if (isstreqn(region->name, key, sizeof(region->name))) {

                                        if (is_pid_list_empty(region)) {
                                                if (prev) {
                                                        prev->next = region->next;
                                                } else {
                                                        SHM.list = region->next;
                                                }

                                                err = _kfree(_MM_SHM, cast(void*, &region));

                                        } else {
                                                err = EADDRINUSE;
                                        }

                                        break;
                                }

                                prev = region;
                        }

                        _mutex_unlock(SHM.mtx);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Function return region parameters.
 *
 * @param key   memory region ID
 * @param mem   memory region address
 * @param size  memory region size
 * @param pid   process ID
 *
 * @return One of errno value.
 */
//==============================================================================
int _shm_attach(const char *key, void **mem, size_t *size, pid_t pid)
{
        int err = EINVAL;

        if (!isstrempty(key) && mem && size && pid) {

                err = _mutex_lock(SHM.mtx, MAX_DELAY_MS);
                if (!err) {

                        err = ENOENT;

                        foreach_region(region, SHM.list) {
                                if (isstreqn(region->name, key, sizeof(region->name))) {

                                        err = attach_pid(region, pid);
                                        if (!err) {
                                                *mem  = region->blk;
                                                *size = region->size;
                                        }

                                        break;
                                }
                        }

                        _mutex_unlock(SHM.mtx);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Function detach region.
 *
 * @param key   memory region ID
 * @param pid   process ID
 *
 * @return One of errno value.
 */
//==============================================================================
int _shm_detach(const char *key, pid_t pid)
{
        int err = EINVAL;

        if (!isstrempty(key) && pid) {

                err = _mutex_lock(SHM.mtx, MAX_DELAY_MS);
                if (!err) {

                        err = ENOENT;

                        foreach_region(region, SHM.list) {
                                if (isstreqn(region->name, key, sizeof(region->name))) {
                                        err = detach_pid(region, pid);
                                        break;
                                }
                        }

                        _mutex_unlock(SHM.mtx);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Function detach selected PID from all regions. Only for system purpose.
 *
 * @param pid   process ID
 *
 * @return One of errno value.
 */
//==============================================================================
int _shm_detach_anywhere(pid_t pid)
{
        int err = EINVAL;

        if (pid) {
                err = _mutex_lock(SHM.mtx, MAX_DELAY_MS);
                if (!err) {

                        foreach_region(region, SHM.list) {
                                detach_pid(region, pid);
                        }

                        _mutex_unlock(SHM.mtx);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function attach PID to selected shared memory region.
 *
 * @param  region       region to attach to
 * @param  pid          process ID
 *
 * @return One of errno value.
 */
//==============================================================================
static int attach_pid(shm_region_t *region, pid_t pid)
{
        int err = EADDRINUSE;

        if (not is_pid_attached(region, pid)) {

                pid_list_t *lst = &region->attached_pids;

                while (lst) {
                        for (u8_t i = 0; i < ARRAY_SIZE(lst->pid); i++) {
                                if (lst->pid[i] == 0) {
                                        lst->pid[i] = pid;
                                        err         = ESUCC;
                                        goto finish;
                                }
                        }

                        if (!lst->next) {
                                err = _kzalloc(_MM_SHM, sizeof(pid_list_t),
                                               cast(void*, &lst->next));
                                if (err) {
                                        break;
                                }
                        }

                        lst = lst->next;
                }
        }

        finish:
        return err;
}

//==============================================================================
/**
 * @brief  Function detach PID from selected shared memory region.
 *
 * @param  region       region to detach from
 * @param  pid          process ID
 *
 * @return One of errno value.
 */
//==============================================================================
static int detach_pid(shm_region_t *region, pid_t pid)
{
        int err = EFAULT;

        if (is_pid_attached(region, pid)) {

                pid_list_t *lst = &region->attached_pids;
                pid_list_t *prv = NULL;
                bool   detached = false;

                while (!detached && lst) {
                        u8_t empty = 0;

                        for (u8_t i = 0; i < ARRAY_SIZE(lst->pid); i++) {
                                if (lst->pid[i] == pid) {
                                        lst->pid[i] = 0;
                                        detached    = true;
                                }

                                empty += (lst->pid[i] == 0) ? 1 : 0;
                        }

                        if (detached) {
                                if ( (empty == ARRAY_SIZE(lst->pid)) && (prv != NULL) ) {

                                        prv->next = lst->next;

                                        _kfree(_MM_SHM, cast(void*, &lst));
                                }
                        } else {
                                prv = lst;
                                lst = lst->next;
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function check if PID is already attached to selected shared memory region.
 *
 * @param  region       region to examine
 * @param  pid          process ID
 *
 * @return Return true if PID is already attached, otherwise false.
 */
//==============================================================================
static bool is_pid_attached(const shm_region_t *region, pid_t pid)
{
        const pid_list_t *lst = &region->attached_pids;

        while (lst) {
                for (u8_t i = 0; i < ARRAY_SIZE(lst->pid); i++) {
                        if (lst->pid[i] == pid) {
                                return true;
                        }
                }

                lst = lst->next;
        }

        return false;
}

//==============================================================================
/**
 * @brief  Function check if selected shared memory region is empty (processes
 *         are not attached).
 *
 * @param  region       region to examine
 *
 * @return Return true if region PID list is empty, otherwise false.
 */
//==============================================================================
static bool is_pid_list_empty(const shm_region_t *region)
{
        const pid_list_t *lst = &region->attached_pids;

        if (lst->next) {
                return false;

        } else {
                for (u8_t i = 0; i < ARRAY_SIZE(lst->pid); i++) {
                        if (lst->pid[i] != 0) {
                                return false;
                        }
                }
        }

        return true;
}

#endif
/*==============================================================================
  End of file
==============================================================================*/
