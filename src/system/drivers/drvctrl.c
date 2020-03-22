/*=========================================================================*//**
@file    drvctrl.c

@author  Daniel Zorychta

@brief   Modules support.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include "kernel/kwrapper.h"
#include "drivers/drvctrl.h"
#include "kernel/printk.h"
#include "kernel/process.h"
#include "kernel/syscall.h"
#include "kernel/sysfunc.h"
#include "lib/vt100.h"
#include "lib/llist.h"
#include "fs/vfs.h"
#include "dnx/misc.h"

/*==============================================================================
  Local macros
==============================================================================*/
#undef errno

#define DRIVER_NAME             "Driver %s%d-%d"
#define DRIVER_NAME_ARGS        module, major, minor

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
        int err = EINVAL;

        if (modno && mem && id != -1) {
                err = ENODEV;

                *modno = _dev_t__extract_modno(id);

                if (*modno < _drvreg_number_of_modules) {
                        _kernel_scheduler_lock();
                        {
                                for (drvmem_t *drv = drvmem[*modno];
                                     drv != NULL && err == ENODEV;
                                     drv = drv->next) {

                                        if (drv->devid == id) {
                                                *mem = drv->mem;
                                                err  = ESUCC;
                                        }
                                }
                        }
                        _kernel_scheduler_unlock();
                }
        }

        return err;
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
        int err = ENODEV;

        if (modno < _drvreg_number_of_modules && drv) {

                err = ESUCC;

                _kernel_scheduler_lock();
                {
                        // find that module is not already initialized
                        for (drvmem_t *drv = drvmem[modno]; drv && !err; drv = drv->next) {
                                if (  _dev_t__extract_major(drv->devid) == major
                                   && _dev_t__extract_minor(drv->devid) == minor) {

                                        err = EADDRINUSE;
                                }
                        }

                        // create new driver chain
                        if (!err) {
                                err = _kzalloc(_MM_KRN, sizeof(drvmem_t), cast(void *, drv));
                                if (!err) {
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

        return err;
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
        int err;

        // allocate modules memory handles
        if (drvmem == NULL) {
                err = _kzalloc(_MM_KRN, _drvreg_number_of_modules * sizeof(drvmem_t*),
                                  cast(void *,&drvmem));

                if (err) {
                        return err;
                }
        }

        // initialize selected module
        int       modno = _module_get_ID(module);
        drvmem_t *drv   = NULL;
        err             = driver__register(modno, major, minor, &drv);
        if (!err) {

                err = driver__initialize(modno, major, minor, &drv->mem);
                if (!err) {
                        if (id) {
                                *id = drv->devid;
                        }

                        if (node_path) {
                                struct vfs_path cpath;
                                cpath.CWD  = NULL;
                                cpath.PATH = node_path;

                                err = _vfs_mknod(&cpath, drv->devid);
                                if (!err) {
                                        printk(DRIVER_NAME" initialized as %s", DRIVER_NAME_ARGS, node_path);

                                } else {
                                        driver__release(modno, drv->mem);
                                        driver__remove(drv->devid);
                                        printk(DRIVER_NAME" node create fail (%d)", DRIVER_NAME_ARGS, err);
                                }
                        } else {
                                printk(DRIVER_NAME" initialized", DRIVER_NAME_ARGS);
                        }

                } else {
                        driver__remove(drv->devid);
                        printk(DRIVER_NAME" initialization error (%d)", DRIVER_NAME_ARGS, err);
                }
        } else {
                switch (err) {
                case EADDRINUSE: printk(DRIVER_NAME" already initialized", DRIVER_NAME_ARGS); break;
                default        : printk(DRIVER_NAME" not exist", DRIVER_NAME_ARGS); break;
                }
        }

        return err;
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

        int err = driver__get_module_no_and_mem(id, &modno, &mem);
        if (!err) {
#if ((__OS_SYSTEM_MSG_ENABLE__ > 0) && (__OS_PRINTF_ENABLE__ > 0))
                u8_t        major  = _dev_t__extract_major(id);
                u8_t        minor  = _dev_t__extract_minor(id);
                const char *module = _module_get_name(modno);
#endif

                err = driver__release(modno, mem);
                if (!err) {
                        driver__remove(id);
                        printk(DRIVER_NAME" released", DRIVER_NAME_ARGS);
                } else {
                        printk(DRIVER_NAME" release fail (%d)", DRIVER_NAME_ARGS, err);
                }
        }

        return err;
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

        int err = driver__get_module_no_and_mem(id, &modno, &mem);
        if (!err) {
                err = _drvreg_module_table[modno].IF.drv_open(mem, vfs_filter_flags_for_device(flags));
        }

        return err;
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

        int err = driver__get_module_no_and_mem(id, &modno, &mem);
        if (!err) {
                err = _drvreg_module_table[modno].IF.drv_close(mem, force);
        }

        return err;
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

        int err = driver__get_module_no_and_mem(id, &modno, &mem);
        if (!err) {
                err = _drvreg_module_table[modno].IF.drv_write(mem, src, count, fpos, wrcnt, fattr);
        }

        return err;
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

        int err = driver__get_module_no_and_mem(id, &modno, &mem);
        if (!err) {
                err = _drvreg_module_table[modno].IF.drv_read(mem, dst, count, fpos, rdcnt, fattr);
        }

        return err;
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

        int err = driver__get_module_no_and_mem(id, &modno, &mem);
        if (!err) {
                err = _drvreg_module_table[modno].IF.drv_ioctl(mem, request, arg);
        }

        return err;
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

        int err = driver__get_module_no_and_mem(id, &modno, &mem);
        if (!err) {
                err = _drvreg_module_table[modno].IF.drv_flush(mem);
        }

        return err;
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

        int err = driver__get_module_no_and_mem(id, &modno, &mem);
        if (!err) {
                stat->st_major = _dev_t__extract_major(id);
                stat->st_minor = _dev_t__extract_minor(id);
                stat->st_size  = 0;
                err = _drvreg_module_table[modno].IF.drv_stat(mem, stat);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function return instance of selected module.
 *
 * @param  module_name          module name
 * @param  major                module major number
 * @param  minor                module minor number
 * @param  mem                  memory instance
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
int _module_get_instance(const char *module_name, u8_t major, u8_t minor, void **mem)
{
        dev_t dev = _dev_t__create(_module_get_ID(module_name), major, minor);

        u16_t  modno = 0;
        return driver__get_module_no_and_mem(dev, &modno, mem);
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
i32_t _module_get_ID(const char *module_name)
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
 * @brief Function return number of module instances.
 *
 * @param n             module number
 *
 * @return Number of instances or -1 on error.
 */
//==============================================================================
ssize_t _module_get_number_of_instances(size_t n)
{
        int instances = -1;

        if (n < _drvreg_number_of_modules) {
                _kernel_scheduler_lock();
                {
                        instances = 0;
                        for (drvmem_t *drv = drvmem[n]; drv; instances++, drv = drv->next);
                }
                _kernel_scheduler_unlock();
        }

        return instances;
}

//==============================================================================
/**
 * @brief Function lock device for this task
 *
 * Doxygen documentation in drivers/driver.h.
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
        int err = EINVAL;

        if (dev_lock) {
                _kernel_scheduler_lock();
                {
                        if (*dev_lock == 0) {

#if (__OS_TASK_KWORKER_MODE__ == 0) || (__OS_TASK_KWORKER_MODE__ == 1)
                                *dev_lock = _syscall_client_PID[_process_get_active_thread()];
#elif (__OS_TASK_KWORKER_MODE__ == 2)
                                *dev_lock = _process_get_active_process_pid();
#endif
                                if (*dev_lock == 0) {
                                        _process_get_pid(_kworker_proc, dev_lock);
                                }

                                err = ESUCC;
                        } else {
                                err = EBUSY;
                        }
                }
                _kernel_scheduler_unlock();
        }

        return err;
}

//==============================================================================
/**
 * @brief Function unlock before locked device
 *
 * Doxygen documentation in drivers/driver.h.
 *
 * @param *dev_lock     pointer to device lock object
 * @param  force        true: force unlock
 *
 * @return One of errno value (ESUCC for success)
 */
//==============================================================================
int _device_unlock(dev_lock_t *dev_lock, bool force)
{
        int err = EINVAL;

        if (dev_lock) {
                _kernel_scheduler_lock();
                {
                        pid_t kworker_pid = 0;
                        _process_get_pid(_kworker_proc, &kworker_pid);

                        pid_t client_pid = 0;
#if (__OS_TASK_KWORKER_MODE__ == 0) || (__OS_TASK_KWORKER_MODE__ == 1)
                        client_pid = _syscall_client_PID[_process_get_active_thread()];
#elif (__OS_TASK_KWORKER_MODE__ == 2)
                        client_pid = _process_get_active_process_pid();
#endif

                        if (   force    == true
                           || *dev_lock == client_pid
                           || *dev_lock == kworker_pid) {

                                *dev_lock = 0;
                                err = ESUCC;
                        } else {
                                err = EBUSY;
                        }
                }
                _kernel_scheduler_unlock();
        }

        return err;
}

//==============================================================================
/**
 * @brief Function check that current task has access to device
 *
 * Doxygen documentation in drivers/driver.h.
 *
 * @param *dev_lock     pointer to device lock object
 *
 * @return One of errno value (ESUCC for access granted)
 */
//==============================================================================
int _device_get_access(dev_lock_t *dev_lock)
{
        int err = EINVAL;

        if (dev_lock) {
                _kernel_scheduler_lock();
                {
                        pid_t kworker_pid = 0;
                        _process_get_pid(_kworker_proc, &kworker_pid);

                        pid_t client_pid = 0;
#if (__OS_TASK_KWORKER_MODE__ == 0) || (__OS_TASK_KWORKER_MODE__ == 1)
                        client_pid = _syscall_client_PID[_process_get_active_thread()];
#elif (__OS_TASK_KWORKER_MODE__ == 2)
                        client_pid = _process_get_active_process_pid();
#endif

                        if (  *dev_lock == client_pid
                           || *dev_lock == kworker_pid) {

                                err = ESUCC;
                        } else {
                                err = EBUSY;
                        }
                }
                _kernel_scheduler_unlock();
        }

        return err;
}

//==============================================================================
/**
 * @brief Function check that device is locked
 *
 * Doxygen documentation in drivers/driver.h.
 *
 * @param *dev_lock     pointer to device lock object
 *
 * @return true if locked, otherwise false
 */
//==============================================================================
bool _device_is_locked(dev_lock_t *dev_lock)
{
        bool err = false;

        if (dev_lock) {
                err = *dev_lock != 0;
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
