/*=========================================================================*//**
@file    drvctrl.c

@author  Daniel Zorychta

@brief   Modules support.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include "kernel/kwrapper.h"
#include "drivers/drvctrl.h"
#include "kernel/printk.h"
#include "kernel/process.h"
#include "lib/vt100.h"
#include "lib/llist.h"
#include "fs/vfs.h"
#include "dnx/misc.h"

/*==============================================================================
  Local macros
==============================================================================*/
#undef errno

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        void *mem;
        dev_t devid;
} drvinfo_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
#if ((CONFIG_SYSTEM_MSG_ENABLE > 0) && (CONFIG_PRINTF_ENABLE > 0))
static const char *DRV_ALREADY_INIT_FMT = VT100_FONT_COLOR_RED"Driver '%s%d-%d' is already initialized!"VT100_RESET_ATTRIBUTES"\n";
static const char *DRV_INITIALIZING_FMT = "Initializing %s%d-%d... ";
static const char *DRV_ERROR_FMT        = VT100_FONT_COLOR_RED"error (%d)"VT100_RESET_ATTRIBUTES"\n";
static const char *DRV_NODE_CREATED_FMT = "%s node created\n";
static const char *DRV_NODE_FAIL_FMT    = VT100_FONT_COLOR_RED"%s node create fail"VT100_RESET_ATTRIBUTES"\n";
static const char *DRV_INITIALIZED_FMT  = "initialized\n";
static const char *MOD_NOT_EXIST_FMT    = VT100_FONT_COLOR_RED"Module '%s' does not exist!"VT100_RESET_ATTRIBUTES"\n";
#else
static const char *DRV_ALREADY_INIT_FMT = "";
static const char *DRV_INITIALIZING_FMT = "";
static const char *DRV_ERROR_FMT        = "";
static const char *DRV_NODE_CREATED_FMT = "";
static const char *DRV_NODE_FAIL_FMT    = "";
static const char *DRV_INITIALIZED_FMT  = "";
static const char *MOD_NOT_EXIST_FMT    = "";
#endif

/* pointers to memory handle used by drivers */
static llist_t *drvlst;
static mutex_t *drvlst_mtx;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/
extern const struct _module_entry _drvreg_module_table[];
extern const size_t               _drvreg_number_of_modules;
//extern const char           *const _drvreg_module_name[];
//extern const uint                  _drvreg_size_of_driver_table;

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief Check if device is running
 *
 * @param id            driver ID
 *
 * @return Pointer to device descriptor or NULL on error.
 */
//==============================================================================
static drvinfo_t *get_driver_info(dev_t id)
{
        drvinfo_t *drv = NULL;

        int result = _mutex_lock(drvlst_mtx, MAX_DELAY_MS);
        if (result == ESUCC) {

                drvinfo_t drvinfo = {.devid = id};
                int pos = _llist_find_begin(drvlst, &drvinfo);
                if (pos >= 0) {
                        drv = _llist_at(drvlst, pos);
                }

                _mutex_unlock(drvlst_mtx);
        }

        return drv;
}

//==============================================================================
/**
 * @brief  Function compare 2 drvinfo_t objects (without comparing drv's memory address)
 *
 * @param  a    object a
 * @param  b    object b
 *
 * @return On equal 0 is returned, otherwise 1 or -1 (see llist)
 */
//==============================================================================
static int compare_drvinfo_obj(const void *a, const void *b)
{
        const drvinfo_t *obj_a = a;
        const drvinfo_t *obj_b = b;

        return (obj_a->devid == obj_b->devid) ? 0 : 1;
}

//==============================================================================
/**
 * @brief Function find driver name and then initialize device
 *
 * @param [IN]  module              module name
 * @param [IN]  major               major number
 * @param [IN]  minor               minor number
 * @param [IN]  node_path           path name to create in the file system (can be NULL or empty string)
 * @param [OUT] id                  module id (can be NULL)
 *
 * @return One of error code (errno)
 */
//==============================================================================
int _driver_init(const char *module, int major, int minor, const char *node_path, dev_t *id)
{
        int result;

        // allocate list which contains addresses of modules memory
        if (drvlst == NULL && drvlst_mtx == NULL) {
                result  = _llist_create_krn(_MM_KRN, compare_drvinfo_obj, NULL, &drvlst);
                result |= _mutex_create(MUTEX_TYPE_RECURSIVE, &drvlst_mtx);
        } else if (drvlst == NULL) {
                result = ENOMEM;
        } else if (drvlst_mtx == NULL) {
                result = ENOMEM;
        } else {
                result = ESUCC;
        }

        // initialize selected module
        if (result == ESUCC && module && major >= 0 && minor >= 0) {

                int modid = _module_get_ID(module);
                if (modid >= 0) {

                        drvinfo_t drvinfo;
                        drvinfo.devid = _dev_t__create(modid, major, minor);
                        drvinfo.mem   = NULL;

                        // check if module is already initialized
//                        result = _mutex_lock(drvlst_mtx, MAX_DELAY_MS);
                        if (result == ESUCC) {
                                if (_llist_find_begin(drvlst, &drvinfo) == -1) {

                                        _printk(DRV_INITIALIZING_FMT, module, major, minor);

                                        result = _drvreg_module_table[modid].IF.drv_init(&drvinfo.mem,
                                                                                          major,
                                                                                          minor);

                                        if (result == ESUCC) {
                                                if (_llist_push_emplace_back(drvlst, sizeof(drvinfo_t), &drvinfo)) {
                                                        if (id) {
                                                                *id = drvinfo.devid;
                                                        }

                                                        if (node_path) {
                                                                int result = _vfs_mknod(node_path, drvinfo.devid);
                                                                if (result == ESUCC) {
                                                                        _printk(DRV_NODE_CREATED_FMT, node_path);
                                                                } else {
                                                                        _drvreg_module_table[modid].IF.drv_release(drvinfo.mem);
                                                                        _llist_pop_back(drvlst);
                                                                        _printk(DRV_NODE_FAIL_FMT, node_path);
                                                                }
                                                        } else {
                                                                _printk(DRV_INITIALIZED_FMT);
                                                        }
                                                } else {
                                                        _drvreg_module_table[modid].IF.drv_release(drvinfo.mem);
                                                        result = ENOMEM;
                                                }
                                        } else {
                                                _printk(DRV_ERROR_FMT, result);
                                        }

                                } else {
                                        _printk(DRV_ALREADY_INIT_FMT, module, major, minor);
                                        result = EADDRINUSE;
                                }

//                                _mutex_unlock(drvlst_mtx);
                        }
                } else {
                        _printk(MOD_NOT_EXIST_FMT, module);
                        result = EINVAL;
                }
        } else {
                result = EINVAL;
        }

        return result;
}

//==============================================================================
/**
 * @brief Function release selected device by using device ID
 *
 * @param dev                   device id
 *
 * @return One of error code (errno)
 */
//==============================================================================
int _driver_release(dev_t dev)
{
        int result = _mutex_lock(drvlst_mtx, MAX_DELAY_MS);
        if (result == ESUCC) {

                drvinfo_t drvinfo = {.devid = dev};
                int pos = _llist_find_begin(drvlst, &drvinfo);
                if (pos >= 0) {
                        drvinfo_t *drv = _llist_at(drvlst, pos);

                        result = _drvreg_module_table[_dev_t__extract_modid(dev)].IF.drv_release(drv->mem);

                        if (result == ESUCC) {
                                _llist_erase(drvlst, pos);
                        }
                }

                _mutex_unlock(drvlst_mtx);
        }

        return result;
}

//==============================================================================
/**
 * @brief Function open selected driver
 *
 * @param id            module id
 * @param flags         flags
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _driver_open(dev_t id, u32_t flags)
{
        drvinfo_t *drv = get_driver_info(id);
        if (drv) {
                dev_t modid = _dev_t__extract_modid(drv->devid);
                return _drvreg_module_table[modid].IF.drv_open(drv->mem, vfs_filter_flags_for_device(flags));
        } else {
                return ENODEV;
        }
}

//==============================================================================
/**
 * @brief Function close selected driver
 *
 * @param id            module id
 * @param force         force close request
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _driver_close(dev_t id, bool force)
{
        drvinfo_t *drv = get_driver_info(id);
        if (drv) {
                dev_t modid = _dev_t__extract_modid(drv->devid);
                return _drvreg_module_table[modid].IF.drv_close(drv->mem, force);
        } else {
                return ENODEV;
        }
}

//==============================================================================
/**
 * @brief Function write data to driver
 *
 * @param id            module id
 * @param src           data source
 * @param count         buffer size
 * @param fpos          file position
 * @param wrcnt         number of written bytes
 * @param fattr         file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _driver_write(dev_t id, const u8_t *src, size_t count, fpos_t *fpos, size_t *wrcnt, struct vfs_fattr fattr)
{
        drvinfo_t *drv = get_driver_info(id);
        if (drv) {
                dev_t modid = _dev_t__extract_modid(drv->devid);
                return _drvreg_module_table[modid].IF.drv_write(drv->mem,
                                                                 src,
                                                                 count,
                                                                 fpos,
                                                                 wrcnt,
                                                                 fattr);
        } else {
                return ENODEV;
        }
}

//==============================================================================
/**
 * @brief Function read data to driver
 *
 * @param id            module id
 * @param dst           data destination
 * @param count         buffer size
 * @param fpos          file position
 * @param rdcnt         number of read bytes
 * @param fattr         file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _driver_read(dev_t id, u8_t *dst, size_t count, fpos_t *fpos, size_t *rdcnt, struct vfs_fattr fattr)
{
        drvinfo_t *drv = get_driver_info(id);
        if (drv) {
                dev_t modid = _dev_t__extract_modid(drv->devid);
                return _drvreg_module_table[modid].IF.drv_read(drv->mem,
                                                                dst,
                                                                count,
                                                                fpos,
                                                                rdcnt,
                                                                fattr);
        } else {
                return ENODEV;
        }
}

//==============================================================================
/**
 * @brief IO control
 *
 * @param id            module id
 * @param request       io request
 * @param arg           argument
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _driver_ioctl(dev_t id, int request, void *arg)
{
        drvinfo_t *drv = get_driver_info(id);
        if (drv) {
                dev_t modid = _dev_t__extract_modid(drv->devid);
                return _drvreg_module_table[modid].IF.drv_ioctl(drv->mem, request, arg);
        } else {
                return ENODEV;
        }
}

//==============================================================================
/**
 * @brief Flush device buffer (forces write)
 *
 * @param id            module id
 * @param request       io request
 * @param arg           argument
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _driver_flush(dev_t id)
{
        drvinfo_t *drv = get_driver_info(id);
        if (drv) {
                dev_t modid = _dev_t__extract_modid(drv->devid);
                return _drvreg_module_table[modid].IF.drv_flush(drv->mem);
        } else {
                return ENODEV;
        }
}

//==============================================================================
/**
 * @brief Device information
 *
 * @param id            module id
 * @param stat          status object
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _driver_stat(dev_t id, struct vfs_dev_stat *stat)
{
        drvinfo_t *drv = get_driver_info(id);
        if (drv) {
                dev_t modid = _dev_t__extract_modid(drv->devid);
                return _drvreg_module_table[modid].IF.drv_stat(drv->mem, stat);
        } else {
                return ENODEV;
        }
}

//==============================================================================
/**
 * @brief Function return module name
 *
 * @param module_number         the module number in the table
 *
 * @return pointer to module's name or NULL if error
 */
//==============================================================================
const char *_module_get_name(size_t module_number)
{
        if (module_number >= _drvreg_number_of_modules)
                return NULL;
        else
                return _drvreg_module_table[module_number].name;
}

//==============================================================================
/**
 * @brief Function return module number
 *
 * @param[in] *module_name      module name
 *
 * @return module number, -1 on error
 */
//==============================================================================
int _module_get_ID(const char *module_name)
{
        if (module_name) {
                for (size_t module = 0; module < _drvreg_number_of_modules; module++) {
                        if (strcmp(_drvreg_module_table[module].name, module_name) == 0) {
                                return module;
                        }
                }
        }

        return -1;
}

//==============================================================================
/**
 * @brief Function returns number of modules
 *
 * @param None
 *
 * @return Number of modules
 */
//==============================================================================
size_t _module_get_count(void)
{
        return _drvreg_number_of_modules;
};

//==============================================================================
/**
 * @brief  Returns number of loaded modules
 *
 * @param  None
 *
 * @return Number of loaded modules or -1 if error.
 */
//==============================================================================
ssize_t _module_get_count_loaded(void)
{
        int count = -1;

        if (_mutex_lock(drvlst_mtx, MAX_DELAY_MS) == ESUCC) {
                count = _llist_size(drvlst);
                _mutex_unlock(drvlst_mtx);
        }

        return count;
}

//==============================================================================
/**
 * @brief Function return driver status
 *
 * @param n             module number
 * @param active        module activity
 *
 * @return true if driver is active (initialized), otherwise false
 */
//==============================================================================
int _module_is_active(size_t n, bool *active)
{
        int result = EINVAL;

        if (n < _drvreg_number_of_modules && active) {
                result = _mutex_lock(drvlst_mtx, MAX_DELAY_MS);
                if (result == ESUCC) {

                        *active = false;

                        _llist_foreach(drvinfo_t*, drvinfo, drvlst) {
                                size_t drvid = _dev_t__extract_modid(drvinfo->devid);
                                if (drvid == n) {
                                        *active = true;
                                        break;
                                }
                        }

                        _mutex_unlock(drvlst_mtx);
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief  Function gets information about loaded driver
 *
 * @param  [IN]  seek           driver selection
 * @param  [OUT] name           module name
 * @param  [OUT] major          major number
 * @param  [OUT] minor          minor number
 *
 * @return One of errno value.
 */
//==============================================================================
int _module_get_driver_info(size_t seek, const char **name, int *major, int *minor)
{
        int result = EINVAL;

        if (seek < _drvreg_number_of_modules && name && major && minor) {
                result = _mutex_lock(drvlst_mtx, MAX_DELAY_MS);
                if (result == ESUCC) {

                        drvinfo_t *drvinfo = _llist_at(drvlst, seek);
                        if (drvinfo) {
                                *name  = _module_get_name(_dev_t__extract_modid(drvinfo->devid));
                                *major = _dev_t__extract_major(drvinfo->devid);
                                *minor = _dev_t__extract_minor(drvinfo->devid);
                                result = ESUCC;
                        }

                        _mutex_unlock(drvlst_mtx);
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief Function lock device for this task
 *
 * ERRNO: EBUSY
 *
 * @param *dev_lock     pointer to device lock object
 *
 * @return One of errno value (ESUCC for success)
 */
//==============================================================================
int _lock_device(dev_lock_t *dev_lock)
{
        int result = EINVAL;

        if (dev_lock) {
                _kernel_scheduler_lock();
                {
                        if (*dev_lock == NULL) {
                                *dev_lock = _process_get_syscall_sem_by_task(_THIS_TASK);
                                if (*dev_lock) {
                                        result = ESUCC;
                                }
                        } else {
                                result = EBUSY;
                        }
                }
                _kernel_scheduler_unlock();
        }

        return result;
}

//==============================================================================
/**
 * @brief Function unlock before locked device
 *
 * @param *dev_lock     pointer to device lock object
 * @param  force        true: force unlock
 *
 * @return One of errno value (ESUCC for success)
 */
//==============================================================================
int _unlock_device(dev_lock_t *dev_lock, bool force)
{
        int result = EINVAL;

        if (dev_lock) {
                _kernel_scheduler_lock();
                {
                        if (*dev_lock == _process_get_syscall_sem_by_task(_THIS_TASK) || force) {
                                *dev_lock = NULL;
                                result    = ESUCC;
                        } else {
                                result = EBUSY;
                        }
                }
                _kernel_scheduler_unlock();
        }

        return result;
}

//==============================================================================
/**
 * @brief Function check that current task has access to device
 *
 * @param *dev_lock     pointer to device lock object
 *
 * @return One of errno value (ESUCC for access granted)
 */
//==============================================================================
int _get_access_to_device(dev_lock_t *dev_lock)
{
        int result = EINVAL;

        if (dev_lock) {
                _kernel_scheduler_lock();
                {
                        if (*dev_lock == _process_get_syscall_sem_by_task(_THIS_TASK)) {
                                result = ESUCC;
                        } else {
                                result = EBUSY;
                        }
                }
                _kernel_scheduler_unlock();
        }

        return result;
}

//==============================================================================
/**
 * @brief Function check that device is locked
 *
 * @param *dev_lock     pointer to device lock object
 *
 * @return true if locked, otherwise false
 */
//==============================================================================
bool _is_device_locked(dev_lock_t *dev_lock)
{
        bool result = false;

        if (dev_lock) {
                result = *dev_lock != NULL;
        }

        return result;
}

/*==============================================================================
  End of file
==============================================================================*/
