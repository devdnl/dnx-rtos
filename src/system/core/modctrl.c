/*=========================================================================*//**
@file    modctrl.c

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
#include "core/modctrl.h"
#include "core/printx.h"
#include "core/sysmoni.h"
#include "core/vfs.h"

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
#if ((CONFIG_SYSTEM_MSG_ENABLE > 0) && (CONFIG_PRINTF_ENABLE > 0))
static const char *drv_already_init_str = FONT_COLOR_RED"Driver '%s' is already initialized!"RESET_ATTRIBUTES"\n";
static const char *drv_initializing_str = "Initializing %s... ";
static const char *drv_error_str        = FONT_COLOR_RED"error"RESET_ATTRIBUTES"\n";
static const char *drv_node_created_str = "%s node created\n";
static const char *drv_node_fail_str    = FONT_COLOR_RED"%s node create fail"RESET_ATTRIBUTES"\n";
static const char *drv_initialized_str  = "initialized\n";
static const char *drv_not_exist_str    = FONT_COLOR_RED"Driver '%s' does not exist!" RESET_ATTRIBUTES"\n";
static const char *mod_not_exist_str    = FONT_COLOR_RED"Module %s does not exist!"RESET_ATTRIBUTES"\n";
#else
static const char *drv_already_init_str = "";
static const char *drv_initializing_str = "";
static const char *drv_error_str        = "";
static const char *drv_node_created_str = "";
static const char *drv_node_fail_str    = "";
static const char *drv_initialized_str  = "";
static const char *drv_not_exist_str    = "";
static const char *mod_not_exist_str    = "";
#endif

/* pointers to memory handle used by drivers */
static void **driver_memory_region;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/
extern const char           *const _regdrv_module_name[];
extern const struct _driver_entry  _regdrv_driver_table[];
extern const uint                  _regdrv_size_of_driver_table;
extern const uint                  _regdrv_number_of_modules;

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief Check if device have allocated memory and is in range
 *
 * @param id            driver ID
 *
 * @return true if device is correct, otherwise false
 */
//==============================================================================
static bool is_device_valid(dev_t id)
{
        if (id < (dev_t)_regdrv_size_of_driver_table) {
                if (driver_memory_region[id]) {
                        return true;
                }
        }

        errno = EFAULT;
        return false;
}

//==============================================================================
/**
 * @brief Function find driver name and then initialize device
 *
 * @param [IN]  drv_name            driver name
 * @param [IN]  node_path           path name to create in the file system or NULL
 * @param [OUT] id                  driver id
 *
 * @return One of error code (errno)
 */
//==============================================================================
int _driver_init(const char *drv_name, const char *node_path, dev_t *id)
{
        if (drv_name == NULL) {
                return EINVAL;
        }

        if (!driver_memory_region) {
                driver_memory_region = _sysm_syscalloc(_regdrv_size_of_driver_table, sizeof(void*));
                if (!driver_memory_region) {
                        return ENOMEM;
                }
        }

        for (uint drvid = 0; drvid < _regdrv_size_of_driver_table; drvid++) {

                if (strcmp(_regdrv_driver_table[drvid].drv_name, drv_name) != 0) {
                        continue;
                }

                if (driver_memory_region[drvid]) {
                        _printk(drv_already_init_str, drv_name);
                        return EADDRINUSE;
                }

                _printk(drv_initializing_str, drv_name);

                int status = _regdrv_driver_table[drvid].interface->drv_init(&driver_memory_region[drvid],
                                                                             _regdrv_driver_table[drvid].major,
                                                                             _regdrv_driver_table[drvid].minor);
                if (status != ESUCC) {
                        _printk(drv_error_str, drv_name);
                        return status;
                }

                if (driver_memory_region[drvid] == NULL)
                        driver_memory_region[drvid] = (void*)(size_t)-1;

                if (node_path) {
                        if (_vfs_mknod(node_path, drvid) == STD_RET_OK) { // FIXME
                                _printk(drv_node_created_str, node_path);
                                return drvid;
                        } else {
                                _regdrv_driver_table[drvid].interface->drv_release(driver_memory_region[drvid]);
                                _printk(drv_node_fail_str, node_path);
                                return -1;
                        }

                } else {
                        _printk(drv_initialized_str);
                        return ESUCC;
                }
        }

        _printk(drv_not_exist_str, drv_name);
        return EINVAL;
}

//==============================================================================
/**
 * @brief Function find driver name and then release device
 *
 * @param drv_name            driver name
 *
 * @return One of error code (errno)
 */
//==============================================================================
int _driver_release(const char *drv_name)
{
        if (!drv_name) {
                return EINVAL;
        }

        for (uint i = 0; i < _regdrv_size_of_driver_table; i++) {
                if (strcmp(_regdrv_driver_table[i].drv_name, drv_name) == 0) {

                        if (driver_memory_region[i]) {
                                int status = _regdrv_driver_table[i].interface->drv_release(driver_memory_region[i]);
                                if (status == ESUCC) {
                                        driver_memory_region[i] = NULL;
                                }
                                return status;
                        } else {
                                return ENXIO;
                        }
                }
        }

        return EINVAL;
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
        if (is_device_valid(id)) {
                return _regdrv_driver_table[id].interface->drv_open(driver_memory_region[id],
                                                                    vfs_filter_flags_for_device(flags));
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
        if (is_device_valid(id)) {
                return _regdrv_driver_table[id].interface->drv_close(driver_memory_region[id], force);
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
        if (is_device_valid(id)) {
                return _regdrv_driver_table[id].interface->drv_write(driver_memory_region[id],
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
        if (is_device_valid(id)) {
                return _regdrv_driver_table[id].interface->drv_read(driver_memory_region[id],
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
        if (is_device_valid(id)) {
                return _regdrv_driver_table[id].interface->drv_ioctl(driver_memory_region[id], request, arg);
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
        if (is_device_valid(id)) {
                return _regdrv_driver_table[id].interface->drv_flush(driver_memory_region[id]);
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
        if (is_device_valid(id)) {
                return _regdrv_driver_table[id].interface->drv_stat(driver_memory_region[id], stat);
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
const char *_get_module_name(uint module_number)
{
        if (module_number >= _regdrv_number_of_modules)
                return NULL;
        else
                return _regdrv_module_name[module_number];
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
int _get_module_number(const char *module_name)
{
        if (!module_name)
                return -1;

        for (uint module = 0; module < _regdrv_number_of_modules; module++) {
                if (strcmp(_regdrv_module_name[module], module_name) == 0) {
                        return module;
                }
        }

        _printk(mod_not_exist_str, module_name);
        return -1;
}

//==============================================================================
/**
 * @brief Function returns number of modules
 */
//==============================================================================
uint _get_number_of_modules(void)
{
        return _regdrv_number_of_modules;
};

//==============================================================================
/**
 * @brief Return number of drivers
 *
 * @return number of drivers
 */
//==============================================================================
uint _get_number_of_drivers(void)
{
        return _regdrv_size_of_driver_table;
}

//==============================================================================
/**
 * @brief Return driver name
 *
 * @param n             driver number
 *
 * @return driver name, NULL on error
 */
//==============================================================================
const char *_get_driver_name(uint n)
{
        if (n < _regdrv_size_of_driver_table) {
                return _regdrv_driver_table[n].drv_name;
        } else {
                return NULL;
        }
}

//==============================================================================
/**
 * @brief Returns driver ID
 *
 * @param name          driver name
 *
 * @return On success driver ID is returned. On error -1 is returned.
 */
//==============================================================================
int _get_driver_ID(const char *name)
{
        for (uint i = 0; i < _regdrv_size_of_driver_table; i++) {
                if (strcmp(_regdrv_driver_table[i].drv_name, name) == 0) {
                        return i;
                }
        }

        return -1;
}

//==============================================================================
/**
 * @brief Return driver module name
 *
 * @param n             driver number
 *
 * @return driver module name, NULL on error
 */
//==============================================================================
const char *_get_driver_module_name(uint n)
{
        if (n < _regdrv_size_of_driver_table) {
                return _regdrv_driver_table[n].mod_name;
        } else {
                return NULL;
        }
}

//==============================================================================
/**
 * @brief Function return driver status
 *
 * @param n             driver number
 *
 * @return true if driver is active (initialized), otherwise false
 */
//==============================================================================
bool _is_driver_active(uint n)
{
        if (n < _regdrv_size_of_driver_table) {
                if (driver_memory_region[n] != NULL) {
                        return true;
                }
        }

        return false;
}

//==============================================================================
/**
 * @brief Function lock device for this task
 *
 * ERRNO: EBUSY
 *
 * @param *dev_lock     pointer to device lock object
 *
 * @return true if device is successfully locked, otherwise false
 */
//==============================================================================
bool _lock_device(dev_lock_t *dev_lock)
{
        bool status = false;

        if (dev_lock) {
                _critical_section_begin();
                if (*dev_lock == NULL) {
                        *dev_lock = _task_get_handle();
                        status = true;
                } else {
                        errno = EBUSY;
                }
                _critical_section_end();
        }

        return status;
}

//==============================================================================
/**
 * @brief Function unlock before locked device
 *
 * @param *dev_lock     pointer to device lock object
 * @param  force        true: force unlock
 */
//==============================================================================
void _unlock_device(dev_lock_t *dev_lock, bool force)
{
        if (dev_lock) {
                _critical_section_begin();
                if (*dev_lock == _task_get_handle() || force) {
                        *dev_lock = NULL;
                }
                _critical_section_end();
        }
}

//==============================================================================
/**
 * @brief Function check that current task has access to device
 *
 * @param *dev_lock     pointer to device lock object
 *
 * @return true if access granted, otherwise false
 */
//==============================================================================
bool _is_device_access_granted(dev_lock_t *dev_lock)
{
        bool status = false;

        if (dev_lock) {
                _critical_section_begin();
                if (*dev_lock == _task_get_handle()) {
                        status = true;
                }
                _critical_section_end();
        }

        return status;
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
        bool status = false;

        if (dev_lock) {
                _critical_section_begin();
                if (*dev_lock != NULL) {
                        status = true;
                }
                _critical_section_end();
        }

        return status;
}

/*==============================================================================
  End of file
==============================================================================*/
