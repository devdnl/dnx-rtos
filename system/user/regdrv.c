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
#include "regdrv.h"
#include <string.h>
#include "vfs.h"
#include "io.h"

/* include here drivers headers */
#if defined(ARCH_stm32)
#include "uart.h"
#include "gpio.h"
#include "pll.h"
#include "tty.h"
#elif defined(ARCH_posix)
#include "uart.h"
#include "tty.h"
#include "pll.h"
#else
#endif

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define IMPORT_DRIVER_INTERFACE(drvmodule, drvname, devno, devpart)\
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
                 .drv_ioctl = drvmodule##_ioctl}}

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
struct driver_entry {
        char  *drv_name;
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
static const struct driver_entry driver_table[] =
{
        IMPORT_DRIVER_INTERFACE(UART, "uart1", UART_DEV_1, UART_PART_NONE),
        IMPORT_DRIVER_INTERFACE(GPIO, "gpio", GPIO_DEV_NONE, GPIO_PART_NONE),
        IMPORT_DRIVER_INTERFACE(PLL, "pll", PLL_DEV_NONE, PLL_PART_NONE),

        #if (TTY_NUMBER_OF_VT > 0)
        IMPORT_DRIVER_INTERFACE(TTY, "tty0", TTY_DEV_0, TTY_PART_NONE),
        #endif

        #if (TTY_NUMBER_OF_VT > 1)
        IMPORT_DRIVER_INTERFACE(TTY, "tty1", TTY_DEV_1, TTY_PART_NONE),
        #endif

        #if (TTY_NUMBER_OF_VT > 2)
        IMPORT_DRIVER_INTERFACE(TTY, "tty2", TTY_DEV_2, TTY_PART_NONE),
        #endif

        #if (TTY_NUMBER_OF_VT > 3)
        IMPORT_DRIVER_INTERFACE(TTY, "tty3", TTY_DEV_3, TTY_PART_NONE),
        #endif
};

/* pointers to memory handle used by drivers */
static void *driver_handle[ARRAY_SIZE(driver_table)];

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
        if (drv_name == NULL) {
                return STD_RET_ERROR;
        }

        for (u16_t i = 0; i < ARRAY_SIZE(driver_table); i++) {

                if (strcmp(driver_table[i].drv_name, drv_name) != 0) {
                        continue;
                }

                if (driver_table[i].drv_init(&driver_handle[i],
                                             driver_table[i].dev,
                                             driver_table[i].part) != STD_RET_OK) {

                        printk(FONT_COLOR_RED"Driver %s initialization error!"
                               RESET_ATTRIBUTES"\n", drv_name);

                        return STD_RET_ERROR;
                }

                if (node_path) {
                        if (vfs_mknod(node_path, (struct vfs_drv_interface *)
                                      &driver_table[i].drv_if) == STD_RET_OK) {

                                printk("Created node %s\n", node_path);
                                return STD_RET_OK;
                        } else {
                                driver_table[i].drv_release(driver_handle[i]);

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
        if (!drv_name) {
                return STD_RET_ERROR;
        }

        for (uint i = 0; i < ARRAY_SIZE(driver_table); i++) {
                if (strcmp(driver_table[i].drv_name, drv_name) == 0) {
                        return driver_table[i].drv_release(driver_handle[i]);
                }
        }

        return STD_RET_ERROR;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
