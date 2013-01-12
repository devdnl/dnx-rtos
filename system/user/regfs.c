/*=============================================================================================*//**
@file    regfs.c

@author  Daniel Zorychta

@brief   This file is used to registration file systems

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
#include "regfs.h"
#include <string.h>
#include "vfs.h"
#include "print.h"

/* include here drivers headers */


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
#define IMPORT_FS_INTERFACE_CLASS(classname, fsname, devno, devpart)\
{.drvName    = drvname,\
 .drvInit    = classname##_Init,\
 .drvRelease = classname##_Release,\
 .drvCfg     = {.dev     = devno,\
               .part    = devpart,\
               .f_open  = classname##_Open,\
               .f_close = classname##_Close,\
               .f_write = classname##_Write,\
               .f_read  = classname##_Read,\
               .f_ioctl = classname##_IOCtl}}


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
      IMPORT_DRIVER_INTERFACE_CLASS(UART, "uart1", UART_DEV_1, UART_PART_NONE),
      #endif

      #ifdef GPIO_H_
      IMPORT_DRIVER_INTERFACE_CLASS(GPIO, "gpio", GPIO_DEV_NONE, GPIO_PART_NONE),
      #endif

      #ifdef PLL_H_
      IMPORT_DRIVER_INTERFACE_CLASS(PLL, "pll", PLL_DEV_NONE, PLL_PART_NONE),
      #endif

      #ifdef I2C_H_
      IMPORT_DRIVER_INTERFACE_CLASS(I2C, "i2c1", I2C_DEV_1, I2C_PART_NONE),
      #endif

      #ifdef ETH_H_
      IMPORT_DRIVER_INTERFACE_CLASS(ETHER, "eth0", ETH_DEV_1, ETH_PART_NONE),
      #endif

      #ifdef DS1307_H_
      IMPORT_DRIVER_INTERFACE_CLASS(DS1307, "ds1307nvm", DS1307_DEV_NVM, DS1307_PART_NONE),
      #endif

      #ifdef DS1307_H_
      IMPORT_DRIVER_INTERFACE_CLASS(DS1307, "ds1307rtc", DS1307_DEV_RTC, DS1307_PART_NONE),
      #endif

      #if (TTY_NUMBER_OF_VT > 0)
      IMPORT_DRIVER_INTERFACE_CLASS(TTY, "tty0", TTY_DEV_0, TTY_PART_NONE),
      #endif

      #if (TTY_NUMBER_OF_VT > 1)
      IMPORT_DRIVER_INTERFACE_CLASS(TTY, "tty1", TTY_DEV_1, TTY_PART_NONE),
      #endif

      #if (TTY_NUMBER_OF_VT > 2)
      IMPORT_DRIVER_INTERFACE_CLASS(TTY, "tty2", TTY_DEV_2, TTY_PART_NONE),
      #endif

      #if (TTY_NUMBER_OF_VT > 3)
      IMPORT_DRIVER_INTERFACE_CLASS(TTY, "tty3", TTY_DEV_3, TTY_PART_NONE),
      #endif

      #ifdef MPL115A2_H_
      IMPORT_DRIVER_INTERFACE_CLASS(MPL115A2, "mpl115a2", MPL115A2_DEV_NONE, MPL115A2_PART_NONE),
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
 * @brief
 */
//================================================================================================//
stdRet_t mount(ch_t *fstype, ch_t *srcpath, ch_t *mountpoint)
{

}


//================================================================================================//
/**
 * @brief
 */
//================================================================================================//
stdRet_t umount(ch_t *mountpoint)
{

}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
