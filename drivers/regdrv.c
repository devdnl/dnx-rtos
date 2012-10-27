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
#include "systypes.h"
#include <string.h>

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
      ch_t     *drvName;
      stdRet_t (*init)(nod_t);
      stdRet_t (*open)(nod_t);
      stdRet_t (*close)(nod_t);
      size_t   (*write)(nod_t dev, void *src, size_t size, size_t nitems, size_t seek);
      size_t   (*read )(nod_t dev, void *dst, size_t size, size_t nitmes, size_t seek);
      stdRet_t (*ioctl)(nod_t, IORq_t, void*);
      stdRet_t (*release)(nod_t);
      nod_t    device;
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
      {
             "uart1",               UART_Init,              UART_Open,
             UART_Close,            UART_Write,             UART_Read,
             UART_IOCtl,            UART_Release,           UART_DEV_1
      },
      {
             "gpio",                GPIO_Init,              GPIO_Open,
             GPIO_Close,            GPIO_Write,             GPIO_Read,
             GPIO_IOCtl,            GPIO_Release,           GPIO_DEV_NONE
      },
      {
             "pll",                 PLL_Init,               PLL_Open,
             PLL_Close,             PLL_Write,              PLL_Read,
             PLL_IOCtl,             PLL_Release,            PLL_DEV_NONE
      },
      {
             "i2c1",                I2C_Init,               I2C_Open,
             I2C_Close,             I2C_Write,              I2C_Read,
             I2C_IOCtl,             I2C_Release,            I2C_DEV_1
      },
      {
             "eth0",                ETHER_Init,             ETHER_Open,
             ETHER_Close,           ETHER_Write,            ETHER_Read,
             ETHER_IOCtl,           ETHER_Release,          ETH_DEV_1
      },
      #ifdef DS1307_DEV_NVM
      {
             "ds1307nvm",           DS1307_Init,            DS1307_Open,
             DS1307_Close,          DS1307_Write,           DS1307_Read,
             DS1307_IOCtl,          DS1307_Release,         DS1307_DEV_NVM
      },
      #endif
      #ifdef DS1307_DEV_RTC
      {
             "ds1307rtc",           DS1307_Init,            DS1307_Open,
             DS1307_Close,          DS1307_Write,           DS1307_Read,
             DS1307_IOCtl,          DS1307_Release,         DS1307_DEV_RTC
      },
      #endif
      #if (TTY_NUMBER_OF_VT > 0)
      {
             "tty0",                TTY_Init,               TTY_Open,
             TTY_Close,             TTY_Write,              TTY_Read,
             TTY_IOCtl,             TTY_Release,            TTY_DEV_0
      },
      #endif
      #if (TTY_NUMBER_OF_VT > 1)
      {
             "tty1",                TTY_Init,               TTY_Open,
             TTY_Close,             TTY_Write,              TTY_Read,
             TTY_IOCtl,             TTY_Release,            TTY_DEV_1
      },
      #endif
      #if (TTY_NUMBER_OF_VT > 2)
      {
             "tty2",                TTY_Init,               TTY_Open,
             TTY_Close,             TTY_Write,              TTY_Read,
             TTY_IOCtl,             TTY_Release,            TTY_DEV_2
      },
      #endif
      #if (TTY_NUMBER_OF_VT > 3)
      {
             "tty3",                TTY_Init,               TTY_Open,
             TTY_Close,             TTY_Write,              TTY_Read,
             TTY_IOCtl,             TTY_Release,            TTY_DEV_3
      },
      #endif
      #ifdef MPL115A2_DEV_NONE
      {
             "mpl115a2",            MPL115A2_Init,          MPL115A2_Open,
             MPL115A2_Close,        MPL115A2_Write,         MPL115A2_Read,
             MPL115A2_IOCtl,        MPL115A2_Release,       MPL115A2_DEV_NONE
      },
      #endif
};


/* drivers node names */
static struct devName_struct
{
      ch_t *node[ARRAY_SIZE(drvList)];
} *devName;


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
stdRet_t InitDrv(const ch_t *drvName, ch_t *nodeName)
{
      stdRet_t status = STD_RET_ERROR;
      u32_t i;

      if (drvName && nodeName)
      {
            if (devName == NULL)
            {
                  devName = Calloc(1, sizeof(struct devName_struct));

                  if (devName == NULL)
                  {
                        goto InitDrv_End;
                  }
            }

            for (i = 0; i < ARRAY_SIZE(drvList); i++)
            {
                  if (strcmp(drvList[i].drvName, drvName) == 0)
                  {
                        status = drvList[i].init(drvList[i].device);

                        if (status == STD_RET_OK)
                        {
                              devName->node[i] = nodeName;
                              kprint("Created node /dev/%s\n", nodeName);
                        }
                        else
                        {
                              kprint("\x1B[31mCreate node /dev/%s failed\x1B[0m\n", nodeName);
                        }
                        break;
                  }
            }
      }

      InitDrv_End:

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
      u32_t i;

      if (drvName && devName)
      {
            for (i = 0; i < ARRAY_SIZE(drvList); i++)
            {
                  if (strcmp(drvList[i].drvName, drvName) == 0)
                  {
                        status = drvList[i].release(drvList[i].device);

                        if (status == STD_RET_OK && devName->node[i])
                        {
                              kprint("Removed node /dev/%s\n", devName->node[i]);
                              Free(devName->node[i]);
                              devName->node[i] = NULL;
                        }
                        break;
                  }
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function returns regisered driver list
 *
 * @param *drvNode            name of driver in /dev/ path
 *
 * @return driver depending value, all not equal to STD_RET_OK are errors
 */
//================================================================================================//
stdRet_t GetDrvData(const ch_t *drvNode, regDrvData_t *drvdata)
{
      stdRet_t status = STD_RET_ERROR;

      regDrvData_t drvPtrs;
      drvPtrs.open   = NULL;
      drvPtrs.close  = NULL;
      drvPtrs.write  = NULL;
      drvPtrs.read   = NULL;
      drvPtrs.ioctl  = NULL;
      drvPtrs.device = 0;

      if (drvNode)
      {
            for (u8_t i = 0; i < ARRAY_SIZE(drvList); i++)
            {
                  if (strcmp(devName->node[i], drvNode) == 0)
                  {
                        drvPtrs.open   = drvList[i].open;
                        drvPtrs.close  = drvList[i].close;
                        drvPtrs.write  = drvList[i].write;
                        drvPtrs.read   = drvList[i].read;
                        drvPtrs.ioctl  = drvList[i].ioctl;
                        drvPtrs.device = drvList[i].device;

                        *drvdata = drvPtrs;

                        status = STD_RET_OK;

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
