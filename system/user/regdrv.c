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
#define IMPORT_DRIVER_INTERFACE(classname, drvname, devno, devpart)\
{.drvName    = drvname,\
 .drvInit    = classname##_Init,\
 .drvRelease = classname##_Release,\
 .drvCfg     = {.dev    = devno,\
               .part    = devpart,\
               .f_open  = classname##_Open,\
               .f_close = classname##_Close,\
               .f_write = classname##_Write,\
               .f_read  = classname##_Read,\
               .f_ioctl = classname##_IOCtl}}

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
typedef struct {
        char  *drvName;
        stdRet_t (*drvInit   )(devx_t dev, fd_t part);
        stdRet_t (*drvRelease)(devx_t dev, fd_t part);
        struct vfs_drvcfg drvCfg;
} regDrv_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
/* driver registration */
static const regDrv_t drvList[] =
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
 * @param *drvName            driver name
 * @param *nodeName           file name in /dev/ directory
 *
 * @return driver depending value, everything not equal to STD_RET_OK are errors
 */
//==============================================================================
stdRet_t init_driver(const char *drvName, const char *nodeName)
{
        if (drvName == NULL) {
                return STD_RET_ERROR;
        }

        u16_t n = ARRAY_SIZE(drvList);

        for (u16_t i = 0; i < n; i++) {
                if (strcmp(drvList[i].drvName, drvName) != 0) {
                        continue;
                }

                devx_t dev  = drvList[i].drvCfg.dev;
                fd_t   part = drvList[i].drvCfg.part;

                if (drvList[i].drvInit(dev, part) != STD_RET_OK) {
                        printk(FONT_COLOR_RED"Driver %s initialization error!"
                               RESET_ATTRIBUTES"\n", drvName);

                        return STD_RET_ERROR;
                }

                if (nodeName) {
                        if (vfs_mknod(nodeName, (struct vfs_drvcfg*)
                                      &drvList[i].drvCfg) == STD_RET_OK) {

                                printk("Created node %s\n", nodeName);
                                return STD_RET_OK;
                        } else {
                                drvList[i].drvRelease(dev, part);
                                printk(FONT_COLOR_RED"Create node %s failed"
                                       RESET_ATTRIBUTES"\n", nodeName);
                                return STD_RET_ERROR;
                        }

                } else {
                        printk("Driver %s initialized\n", drvName);
                        return STD_RET_OK;
                }
        }

        printk(FONT_COLOR_RED"Driver %s does not exist!"
               RESET_ATTRIBUTES"\n", drvName);

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Function find driver name and then release device
 *
 * @param *drvName            driver name
 *
 * @return driver depending value, all not equal to STD_RET_OK are errors
 */
//==============================================================================
stdRet_t release_driver(const char *drvName)
{
      stdRet_t status = STD_RET_ERROR;

      if (drvName) {
            u16_t n = ARRAY_SIZE(drvList);

            for (u16_t i = 0; i < n; i++) {
                  if (strcmp(drvList[i].drvName, drvName) == 0) {
                        devx_t dev  = drvList[i].drvCfg.dev;
                        fd_t   part = drvList[i].drvCfg.part;

                        status = drvList[i].drvRelease(dev, part);

                        break;
                  }
            }
      }

      return status;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
