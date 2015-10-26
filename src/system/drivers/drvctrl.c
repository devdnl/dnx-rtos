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
typedef struct drvmem {
        struct drvmem *next;
        void          *mem;
        dev_t          devid;
} drvmem_t;

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

static drvmem_t **drvmem;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/
extern const struct _module_entry _drvreg_module_table[];
extern const size_t               _drvreg_number_of_modules;

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief Check if device is running
 *
 * @param [in]  id      driver ID
 * @param [out] modno   module number
 * @param [out] mem     module memory
 *
 * @return On success ESUCC, otherwise other values.
 */
//==============================================================================
static int driver__get_module_no_and_mem(dev_t id, u16_t *modno, void **mem)
{
        int result = EINVAL;

        if (modno && mem) {
                result = ENODEV;

                *modno = _dev_t__extract_modno(id);

                _kernel_scheduler_lock();
                {
                        for (drvmem_t *drv = drvmem[*modno]; drv && result == ENODEV; drv = drv->next) {
                                if (drv->devid == id) {
                                        *mem   = drv->mem;
                                        result = ESUCC;
                                }
                        }
                }
                _kernel_scheduler_unlock();
        }

        return result;
}

//==============================================================================
/**
 * @brief  Register driver in system
 *
 * @param [in]  modno   module number
 * @param [in]  major   module major number
 * @param [in]  minor   module minor number
 * @param [out] drv     registered module memory
 *
 * @return On success ESUCC is returned. ENODEV and EADDRINUSE on error.
 */
//==============================================================================
static int driver__register(u16_t modno, u8_t major, u8_t minor, drvmem_t **drv)
{
        int result = ENODEV;

        if (modno < _drvreg_number_of_modules && drv) {

                result = ESUCC;

                _kernel_scheduler_lock();
                {
                        // find that module is not already initialized
                        for (drvmem_t *drv = drvmem[modno]; drv && result == ESUCC; drv = drv->next) {
                                if (  _dev_t__extract_major(drv->devid) == major
                                   && _dev_t__extract_minor(drv->devid) == minor) {

                                        result = EADDRINUSE;
                                }
                        }

                        // create new driver chain
                        if (result == ESUCC) {
                                result = _kzalloc(_MM_KRN, sizeof(drvmem_t), static_cast(void *, drv));
                                if (result == ESUCC) {
                                        (*drv)->devid = _dev_t__create(modno, major, minor);
                                        (*drv)->mem   = NULL;
                                        (*drv)->next  = NULL;

                                        if (drvmem[modno] == NULL) {
                                                drvmem[modno] = *drv;
                                        } else {
                                                drvmem_t *curr = drvmem[modno];
                                                for (; curr->next; curr = curr->next);
                                                curr->next = *drv;
                                        }
                                }
                        }
                }
                _kernel_scheduler_unlock();
        }

        return result;
}

//==============================================================================
/**
 * @brief  Remove driver from register list
 *
 * @param  devid        device id
 *
 * @return None
 */
//==============================================================================
static void driver__remove(dev_t devid)
{
        u16_t modno = _dev_t__extract_modno(devid);

        if (modno < _drvreg_number_of_modules) {

                _kernel_scheduler_lock();
                {
                        drvmem_t *prev = NULL;
                        drvmem_t *curr = drvmem[modno];

                        for (; curr; curr = curr->next) {
                                if (curr->devid == devid) {
                                        void *tofree = curr;

                                        if (prev) {
                                                prev->next = curr->next;
                                        } else {
                                                drvmem[modno] = curr->next;
                                        }

                                        _kfree(_MM_KRN, &tofree);

                                        break;
                                }

                                prev = curr;
                        }
                }
                _kernel_scheduler_unlock();
        }
}

//==============================================================================
/**
 * @brief  Initialize selected module
 *
 * @param  modno        module number
 * @param  major        module major number
 * @param  minor        module minor number
 * @param  mem          module memory to be allocated by driver
 *
 * @return One of errno value.
 */
//==============================================================================
static inline int driver__initialize(u16_t modno, u8_t major, u8_t minor, void **mem)
{
        return _drvreg_module_table[modno].IF.drv_init(mem, major, minor);
}

//==============================================================================
/**
 * @brief  Release module
 *
 * @param  modno        module number
 * @param  mem          module memory
 *
 * @return One of errno value.
 */
//==============================================================================
static inline int driver__release(u16_t modno, void *mem)
{
        return _drvreg_module_table[modno].IF.drv_release(mem);
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
int _driver_init(const char *module, u8_t major, u8_t minor, const char *node_path, dev_t *id)
{
        int result;

        // allocate modules memory handles
        if (drvmem == NULL) {
                result = _kzalloc(_MM_KRN, _drvreg_number_of_modules * sizeof(drvmem_t*),
                                  static_cast(void *,&drvmem));

                if (result != ESUCC) {
                        return result;
                }
        }

        // initialize selected module
        int       modno = _module_get_ID(module);
        drvmem_t *drv   = NULL;
        result          = driver__register(modno, major, minor, &drv);
        if (result == ESUCC) {

                _printk(DRV_INITIALIZING_FMT, module, major, minor);

                result = driver__initialize(modno, major, minor, &drv->mem);
                if (result == ESUCC) {
                        if (id) {
                                *id = drv->devid;
                        }

                        if (node_path) {
                                int result = _vfs_mknod(node_path, drv->devid);
                                if (result == ESUCC) {
                                        _printk(DRV_NODE_CREATED_FMT, node_path);

                                } else {
                                        driver__release(modno, drv->mem);
                                        driver__remove(drv->devid);
                                        _printk(DRV_NODE_FAIL_FMT, node_path);
                                }
                        } else {
                                _printk(DRV_INITIALIZED_FMT);
                        }

                } else {
                        driver__remove(drv->devid);
                        _printk(DRV_ERROR_FMT, result);
                }
        } else {
                switch (result) {
                case EADDRINUSE: _printk(DRV_ALREADY_INIT_FMT, module, major, minor); break;
                default        : _printk(MOD_NOT_EXIST_FMT, module); break;
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief Function release selected device by using device ID
 *
 * @param id                   device id
 *
 * @return One of error code (errno)
 */
//==============================================================================
int _driver_release(dev_t id)
{
        u16_t modno;
        void *mem;

        int result = driver__get_module_no_and_mem(id, &modno, &mem);
        if (result == ESUCC) {
                result = driver__release(modno, mem);
                if (result == ESUCC) {
                        driver__remove(id);
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief Function open selected driver
 *
 * @param id           module id
 * @param flags         flags
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _driver_open(dev_t id, u32_t flags)
{
        u16_t modno;
        void *mem;

        int result = driver__get_module_no_and_mem(id, &modno, &mem);
        if (result == ESUCC) {
                result = _drvreg_module_table[modno].IF.drv_open(mem, vfs_filter_flags_for_device(flags));
        }

        return result;
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
        u16_t modno;
        void *mem;

        int result = driver__get_module_no_and_mem(id, &modno, &mem);
        if (result == ESUCC) {
                result = _drvreg_module_table[modno].IF.drv_close(mem, force);
        }

        return result;
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
        u16_t modno;
        void *mem;

        int result = driver__get_module_no_and_mem(id, &modno, &mem);
        if (result == ESUCC) {
                result = _drvreg_module_table[modno].IF.drv_write(mem, src, count, fpos, wrcnt, fattr);
        }

        return result;
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
        u16_t modno;
        void *mem;

        int result = driver__get_module_no_and_mem(id, &modno, &mem);
        if (result == ESUCC) {
                result = _drvreg_module_table[modno].IF.drv_read(mem, dst, count, fpos, rdcnt, fattr);
        }

        return result;
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
        u16_t modno;
        void *mem;

        int result = driver__get_module_no_and_mem(id, &modno, &mem);
        if (result == ESUCC) {
                result = _drvreg_module_table[modno].IF.drv_ioctl(mem, request, arg);
        }

        return result;
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
        u16_t modno;
        void *mem;

        int result = driver__get_module_no_and_mem(id, &modno, &mem);
        if (result == ESUCC) {
                result = _drvreg_module_table[modno].IF.drv_flush(mem);
        }

        return result;
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
        u16_t modno;
        void *mem;

        int result = driver__get_module_no_and_mem(id, &modno, &mem);
        if (result == ESUCC) {
                result = _drvreg_module_table[modno].IF.drv_stat(mem, stat);
        }

        return result;
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
 * @brief Function return driver status
 *
 * @param n             module number
 *
 * @return Number of instances or -1 on error
 */
//==============================================================================
ssize_t _module_get_number_of_instances(size_t n)
{
        int result = -1;

        if (n < _drvreg_number_of_modules) {
                _kernel_scheduler_lock();
                {
                        result = 0;
                        for (drvmem_t *drv = drvmem[n]; drv; result++, drv = drv->next);
                }
                _kernel_scheduler_unlock();
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
int _device_lock(dev_lock_t *dev_lock)
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
int _device_unlock(dev_lock_t *dev_lock, bool force)
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
int _device_get_access(dev_lock_t *dev_lock)
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
bool _device_is_locked(dev_lock_t *dev_lock)
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
