/*=============================================================================================*//**
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


*//*==============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "regdrv.h"
#include <string.h>
#include "vfs.h"

/* include here drivers headers */
#include "uart.h"
#include "gpio.h"
#include "pll.h"
#include "i2c.h"
#include "ether.h"
#include "ds1307.h"
#include "tty.h"
#include "mpl115a2.h"


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
typedef struct
{
      ch_t  *drvName;
      stdRet_t (*drvInit   )(devx_t dev, fd_t part);
      stdRet_t (*drvRelease)(devx_t dev, fd_t part);
      struct vfs_drvcfg drvCfg;
} regDrv_t;


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
/* driver registration */
static const regDrv_t drvList[] =
{
      #ifdef UART_H_
      {
             .drvName    = "uart1",
             .drvInit    = UART_Init,
             .drvRelease = UART_Release,
             .drvCfg     = {.dev     = UART_DEV_1,
                            .part    = UART_PART_NONE,
                            .f_open  = UART_Open,
                            .f_close = UART_Close,
                            .f_write = UART_Write,
                            .f_read  = UART_Read,
                            .f_ioctl = UART_IOCtl}
      },
      #endif
      #ifdef GPIO_H_
      {
             .drvName    = "gpio",
             .drvInit    = GPIO_Init,
             .drvRelease = GPIO_Release,
             .drvCfg     = {.dev     = GPIO_DEV_NONE,
                            .part    = GPIO_PART_NONE,
                            .f_open  = GPIO_Open,
                            .f_close = GPIO_Close,
                            .f_write = GPIO_Write,
                            .f_read  = GPIO_Read,
                            .f_ioctl = GPIO_IOCtl}
      },
      #endif
      #ifdef PLL_H_
      {
             .drvName    = "pll",
             .drvInit    = PLL_Init,
             .drvRelease = PLL_Release,
             .drvCfg     = {.dev     = PLL_DEV_NONE,
                            .part    = PLL_PART_NONE,
                            .f_open  = PLL_Open,
                            .f_close = PLL_Close,
                            .f_write = PLL_Write,
                            .f_read  = PLL_Read,
                            .f_ioctl = PLL_IOCtl},
      },
      #endif
      #ifdef I2C_H_
      {
            .drvName    = "i2c1",
            .drvInit    = I2C_Init,
            .drvRelease = I2C_Release,
            .drvCfg     = {.dev     = I2C_DEV_1,
                           .part    = I2C_PART_NONE,
                           .f_open  = I2C_Open,
                           .f_close = I2C_Close,
                           .f_write = I2C_Write,
                           .f_read  = I2C_Read,
                           .f_ioctl = I2C_IOCtl},
      },
      #endif
      #ifdef ETH_H_
      {
             .drvName    = "eth0",
             .drvInit    = ETHER_Init,
             .drvRelease = ETHER_Release,
             .drvCfg     = {.dev     = ETH_DEV_1,
                            .part    = ETH_PART_NONE,
                            .f_open  = ETHER_Open,
                            .f_close = ETHER_Close,
                            .f_write = ETHER_Write,
                            .f_read  = ETHER_Read,
                            .f_ioctl = ETHER_IOCtl},
      },
      #endif
      #ifdef DS1307_H_
      {
             .drvName    = "ds1307nvm",
             .drvInit    = DS1307_Init,
             .drvRelease = DS1307_Release,
             .drvCfg     = {.dev     = DS1307_DEV_NVM,
                            .part    = DS1307_PART_NONE,
                            .f_open  = DS1307_Open,
                            .f_close = DS1307_Close,
                            .f_write = DS1307_Write,
                            .f_read  = DS1307_Read,
                            .f_ioctl = DS1307_IOCtl},
      },
      #endif
      #ifdef DS1307_H_
      {
             .drvName    = "ds1307rtc",
             .drvInit    = DS1307_Init,
             .drvRelease = DS1307_Release,
             .drvCfg     = {.dev     = DS1307_DEV_RTC,
                            .part    = DS1307_PART_NONE,
                            .f_open  = DS1307_Open,
                            .f_close = DS1307_Close,
                            .f_write = DS1307_Write,
                            .f_read  = DS1307_Read,
                            .f_ioctl = DS1307_IOCtl},
      },
      #endif
      #if (TTY_NUMBER_OF_VT > 0)
      {
             .drvName    = "tty0",
             .drvInit    = TTY_Init,
             .drvRelease = TTY_Release,
             .drvCfg     = {.dev     = TTY_DEV_0,
                            .part    = TTY_PART_NONE,
                            .f_open  = TTY_Open,
                            .f_close = TTY_Close,
                            .f_write = TTY_Write,
                            .f_read  = TTY_Read,
                            .f_ioctl = TTY_IOCtl},
      },
      #endif
      #if (TTY_NUMBER_OF_VT > 1)
      {
             .drvName    = "tty1",
             .drvInit    = TTY_Init,
             .drvRelease = TTY_Release,
             .drvCfg     = {.dev     = TTY_DEV_1,
                            .part    = TTY_PART_NONE,
                            .f_open  = TTY_Open,
                            .f_close = TTY_Close,
                            .f_write = TTY_Write,
                            .f_read  = TTY_Read,
                            .f_ioctl = TTY_IOCtl},
      },
      #endif
      #if (TTY_NUMBER_OF_VT > 2)
      {
             .drvName    = "tty2",
             .drvInit    = TTY_Init,
             .drvRelease = TTY_Release,
             .drvCfg     = {.dev     = TTY_DEV_2,
                            .part    = TTY_PART_NONE,
                            .f_open  = TTY_Open,
                            .f_close = TTY_Close,
                            .f_write = TTY_Write,
                            .f_read  = TTY_Read,
                            .f_ioctl = TTY_IOCtl},
      },
      #endif
      #if (TTY_NUMBER_OF_VT > 3)
      {
             .drvName    = "tty3",
             .drvInit    = TTY_Init,
             .drvRelease = TTY_Release,
             .drvCfg     = {.dev     = TTY_DEV_3,
                            .part    = TTY_PART_NONE,
                            .f_open  = TTY_Open,
                            .f_close = TTY_Close,
                            .f_write = TTY_Write,
                            .f_read  = TTY_Read,
                            .f_ioctl = TTY_IOCtl},
      },
      #endif
      #ifdef MPL115A2_H_
      {
             .drvName    = "mpl115a2",
             .drvInit    = MPL115A2_Init,
             .drvRelease = MPL115A2_Release,
             .drvCfg     = {.dev     = MPL115A2_DEV_NONE,
                            .part    = MPL115A2_PART_NONE,
                            .f_open  = MPL115A2_Open,
                            .f_close = MPL115A2_Close,
                            .f_write = MPL115A2_Write,
                            .f_read  = MPL115A2_Read,
                            .f_ioctl = MPL115A2_IOCtl},
      },
      #endif
};


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Function find driver name and then initialize device
 *
 * @param *drvName            driver name
 * @param *nodeName           file name in /dev/ directory
 *
 * @return driver depending value, all not equal to STD_RET_OK are errors
 */
//================================================================================================//
stdRet_t InitDrv(const ch_t *drvName, const ch_t *nodeName)
{
      stdRet_t status = STD_RET_ERROR;

      if (drvName && nodeName) {
            u16_t n = ARRAY_SIZE(drvList);

            for (u16_t i = 0; i < n; i++) {
                  if (strcmp(drvList[i].drvName, drvName) == 0) {
                        devx_t dev  = drvList[i].drvCfg.dev;
                        fd_t   part = drvList[i].drvCfg.part;

                        if (drvList[i].drvInit(dev, part) == STD_RET_OK) {

                              status = vfs_mknod(nodeName, (struct vfs_drvcfg*)&drvList[i].drvCfg);

                              if (status == STD_RET_ERROR) {
                                    drvList[i].drvRelease(dev, part);
                                    kprint("\x1B[31mCreate node %s failed\x1B[0m\n", nodeName);
                              } else {
                                    kprint("Created node %s\n", nodeName);
                              }
                        }

                        goto InitDrv_end;
                  }
            }

            kprint("\x1B[31mDriver %s does not exist!\x1B[0m\n", drvName);
      }

      InitDrv_end:
      return status;
}


//================================================================================================//
/**
 * @brief Function find driver name and then release device
 *
 * @param *drvName            driver name
 *
 * @return driver depending value, all not equal to STD_RET_OK are errors
 */
//================================================================================================//
stdRet_t ReleaseDrv(const ch_t *drvName)
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

/*==================================================================================================
                                            End of file
==================================================================================================*/
