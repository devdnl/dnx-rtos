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
#include <dnx/thread.h>
#include "core/modctrl.h"
#include "core/printx.h"
#include "core/sysmoni.h"

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/
typedef task_t *dev_lock_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
/* pointers to memory handle used by drivers */
static void **driver_memory_region;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/
extern const char                 *_regdrv_module_name[];
extern const struct _driver_entry  _regdrv_driver_table[];
extern const int                   _regdrv_driver_table_array_size;
extern const int                   _regdrv_number_of_modules;

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Function find driver name and then initialize device
 *
 * @param drv_name            driver name
 * @param node_path           path name to create in the file system or NULL
 *
 * @return 0 on success, otherwise other value
 */
//==============================================================================
int _driver_init(const char *drv_name, const char *node_path)
{
        struct vfs_drv_interface drv_if;

        if (drv_name == NULL) {
                errno = EINVAL;
                return -EINVAL;
        }

        if (!driver_memory_region) {
                driver_memory_region = sysm_syscalloc(_regdrv_driver_table_array_size, sizeof(void*));
                if (!driver_memory_region) {
                        return -ENOMEM;
                }
        }

        for (int drvid = 0; drvid < _regdrv_driver_table_array_size; drvid++) {

                if (strcmp(_regdrv_driver_table[drvid].drv_name, drv_name) != 0) {
                        continue;
                }

                if (driver_memory_region[drvid]) {
                        printk(FONT_COLOR_RED"Driver %s is already initialized!"
                               RESET_ATTRIBUTES"\n", drv_name);

                        errno = EADDRINUSE;
                        return -EADDRINUSE;
                }

                for (int mod = 0; mod < _regdrv_number_of_modules; mod++) {
                        if (strcmp(_regdrv_module_name[mod], _regdrv_driver_table[drvid].mod_name) == 0) {

                                printk("Initializing %s... ", drv_name);

                                if (_regdrv_driver_table[drvid].drv_init(&driver_memory_region[drvid],
                                                                         _regdrv_driver_table[drvid].major,
                                                                         _regdrv_driver_table[drvid].minor)
                                                                         != STD_RET_OK) {

                                        printk(FONT_COLOR_RED"error"RESET_ATTRIBUTES"\n", drv_name);

                                        return 1;
                                }

                                if (driver_memory_region[drvid] == NULL)
                                        driver_memory_region[drvid] = (void*)(size_t)-1;

                                if (node_path) {
                                        drv_if = _regdrv_driver_table[drvid].drv_if;
                                        drv_if.handle = driver_memory_region[drvid];

                                        if (vfs_mknod(node_path, &drv_if) == STD_RET_OK) {
                                                printk("%s node created\n", node_path);
                                                return STD_RET_OK;
                                        } else {
                                                _regdrv_driver_table[drvid].drv_release(driver_memory_region[drvid]);

                                                printk(FONT_COLOR_RED"%s node create fail"
                                                       RESET_ATTRIBUTES"\n", node_path);

                                                return 1;
                                        }

                                } else {
                                        printk("initialized\n", drv_name);
                                        return 0;
                                }
                        }
                }
        }

        printk(FONT_COLOR_RED"Driver %s does not exist!"
               RESET_ATTRIBUTES"\n", drv_name);

        errno = EINVAL;

        return -EINVAL;
}

//==============================================================================
/**
 * @brief Function find driver name and then release device
 *
 * @param drv_name            driver name
 *
 * @return 0 on success, otherwise other value
 */
//==============================================================================
int _driver_release(const char *drv_name)
{
        if (!drv_name) {
                errno = EINVAL;
                return -EINVAL;
        }

        for (int i = 0; i < _regdrv_driver_table_array_size; i++) {
                if (strcmp(_regdrv_driver_table[i].drv_name, drv_name) == 0) {

                        stdret_t status = _regdrv_driver_table[i].drv_release(driver_memory_region[i]);
                        if (status == STD_RET_OK) {
                                driver_memory_region[i] = NULL;
                        }

                        return status;
                }
        }

        errno = EINVAL;
        return -EINVAL;
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
const char *_get_module_name(int module_number)
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
 * @return module number
 */
//==============================================================================
int _get_module_number(const char *module_name)
{
        if (!module_name)
                return _regdrv_number_of_modules;

        for (int module = 0; module < _regdrv_number_of_modules; module++) {
                if (strcmp(_regdrv_module_name[module], module_name) == 0) {
                        return module;
                }
        }

        printk(FONT_COLOR_RED"Module %s does not exist!"RESET_ATTRIBUTES"\n", module_name);
        return _regdrv_number_of_modules;
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
                critical_section_begin();
                if (*dev_lock == NULL) {
                        *dev_lock = task_get_handle();
                        status = true;
                } else {
                        errno = EBUSY;
                }
                critical_section_end();
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
                critical_section_begin();
                if (*dev_lock == task_get_handle() || force) {
                        *dev_lock = NULL;
                }
                critical_section_end();
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
                critical_section_begin();
                if (*dev_lock == task_get_handle()) {
                        status = true;
                }
                critical_section_end();
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
                critical_section_begin();
                if (*dev_lock != NULL) {
                        status = true;
                }
                critical_section_end();
        }

        return status;
}

/*==============================================================================
  End of file
==============================================================================*/
