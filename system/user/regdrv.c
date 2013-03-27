/*=========================================================================*//**
@file    regdrv.c

@author  Daniel Zorychta

@brief   This file is used to registration drivers

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "user/regdrv.h"
#include "core/vfs.h"
#include "core/io.h"

/* include here modules headers */
#if defined(ARCH_stm32f1)
#include "drivers/uart/uart.h"
#include "drivers/gpio/gpio.h"
#include "drivers/pll/pll.h"
#include "drivers/tty/tty.h"
#elif defined(ARCH_posix)
#include "drivers/uart/uart.h"
#include "drivers/tty/tty.h"
#include "drivers/pll/pll.h"
#else
#endif

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define USE_MODULE(module_name)                 #module_name

#define USE_DRIVER_INTERFACE(drvmodule, drvname, devno, devpart)\
{.drv_name    = drvname,\
 .dev         = devno,\
 .part        = devpart,\
 .drv_init    = drvmodule##_init,\
 .drv_release = drvmodule##_release,\
 .drv_if      = {.handle    = NULL,\
                 .drv_open  = drvmodule##_open,\
                 .drv_close = drvmodule##_close,\
                 .drv_write = drvmodule##_write,\
                 .drv_read  = drvmodule##_read,\
                 .drv_ioctl = drvmodule##_ioctl,\
                 .drv_flush = drvmodule##_flush}}

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
struct driver_entry {
        const char *drv_name;
        uint  dev;
        uint  part;
        stdret_t (*drv_init   )(void **drvhdl, uint dev, uint part);
        stdret_t (*drv_release)(void *drvhdl);
        struct vfs_drv_interface drv_if;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
/* a table of used modules */
static const char *regdrv_used_modules[REGDRV_NUMBER_OF_REGISTERED_DRIVER_MODULES] = {
        USE_MODULE(UART),
        USE_MODULE(GPIO),
        USE_MODULE(PLL),
        USE_MODULE(TTY),
};

/* a table of a drivers interfaces */
static const struct driver_entry regdrv_driver_table[] = {
        USE_DRIVER_INTERFACE(UART, "uart1", UART_DEV_1   , UART_PART_NONE),
#ifdef GPIO_H_
        USE_DRIVER_INTERFACE(GPIO, "gpio" , GPIO_DEV_NONE, GPIO_PART_NONE),
#endif
        USE_DRIVER_INTERFACE(PLL , "pll"  , PLL_DEV_NONE , PLL_PART_NONE ),
        USE_DRIVER_INTERFACE(TTY , "tty0" , TTY_DEV_0    , TTY_PART_NONE ),
        USE_DRIVER_INTERFACE(TTY , "tty1" , TTY_DEV_1    , TTY_PART_NONE ),
        USE_DRIVER_INTERFACE(TTY , "tty2" , TTY_DEV_2    , TTY_PART_NONE ),
        USE_DRIVER_INTERFACE(TTY , "tty3" , TTY_DEV_3    , TTY_PART_NONE ),
};

/* pointers to memory handle used by drivers */
static void *regdrv_driver_handle[ARRAY_SIZE(regdrv_driver_table)];

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Function find driver name and then initialize device
 *
 * @param *drv_name           driver name
 * @param *node_path          path name to create in the file system
 *
 * @return driver depending value, everything not equal to STD_RET_OK are errors
 */
//==============================================================================
stdret_t init_driver(const char *drv_name, const char *node_path)
{
        struct vfs_drv_interface drv_if;

        if (drv_name == NULL) {
                return STD_RET_ERROR;
        }

        for (uint drvid = 0; drvid < ARRAY_SIZE(regdrv_driver_table); drvid++) {

                if (strcmp(regdrv_driver_table[drvid].drv_name, drv_name) != 0) {
                        continue;
                }

                if (regdrv_driver_handle[drvid]) {
                        printk(FONT_COLOR_RED"Driver %s is already initialized!"
                               RESET_ATTRIBUTES"\n", drv_name);

                        return STD_RET_ERROR;
                }

                if (regdrv_driver_table[drvid].drv_init(&regdrv_driver_handle[drvid],
                                                        regdrv_driver_table[drvid].dev,
                                                        regdrv_driver_table[drvid].part)
                                                        != STD_RET_OK) {

                        if (regdrv_driver_handle[drvid] == NULL)
                                regdrv_driver_handle[drvid] = (void*)(size_t)drvid;

                        printk(FONT_COLOR_RED"Driver %s initialization error!"
                               RESET_ATTRIBUTES"\n", drv_name);

                        return STD_RET_ERROR;
                }

                if (node_path) {
                        drv_if = regdrv_driver_table[drvid].drv_if;
                        drv_if.handle = regdrv_driver_handle[drvid];

                        if (vfs_mknod(node_path, &drv_if) == STD_RET_OK) {
                                printk("Created node %s\n", node_path);
                                return STD_RET_OK;
                        } else {
                                regdrv_driver_table[drvid].drv_release(regdrv_driver_handle[drvid]);

                                printk(FONT_COLOR_RED"Create node %s failed"
                                       RESET_ATTRIBUTES"\n", node_path);

                                return STD_RET_ERROR;
                        }

                } else {
                        printk("Driver %s initialized\n", drv_name);
                        return STD_RET_OK;
                }
        }

        printk(FONT_COLOR_RED"Driver %s does not exist!"
               RESET_ATTRIBUTES"\n", drv_name);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function find driver name and then release device
 *
 * @param *drv_name           driver name
 *
 * @return driver depending value, all not equal to STD_RET_OK are errors
 */
//==============================================================================
stdret_t release_driver(const char *drv_name)
{
        stdret_t status;

        if (!drv_name) {
                return STD_RET_ERROR;
        }

        for (uint i = 0; i < ARRAY_SIZE(regdrv_driver_table); i++) {
                if (strcmp(regdrv_driver_table[i].drv_name, drv_name) == 0) {

                        status = regdrv_driver_table[i].drv_release(regdrv_driver_handle[i]);

                        if (status == STD_RET_OK) {
                                regdrv_driver_handle[i] = NULL;
                        }

                        return status;
                }
        }

        return STD_RET_ERROR;
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
const char *regdrv_get_module_name(uint module_number)
{
        if (module_number >= REGDRV_NUMBER_OF_REGISTERED_DRIVER_MODULES)
                return NULL;
        else
                return regdrv_used_modules[module_number];
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
int regdrv_get_module_number(const char *module_name)
{
        if (!module_name)
                return ARRAY_SIZE(regdrv_used_modules);

        for (uint module = 0; module < ARRAY_SIZE(regdrv_used_modules); module++) {
                if (strcmp(regdrv_used_modules[module], module_name) == 0) {
                        return module;
                }
        }

        printk(FONT_COLOR_RED"Module %s does not exist!"RESET_ATTRIBUTES"\n", module_name);
        return ARRAY_SIZE(regdrv_used_modules);
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
