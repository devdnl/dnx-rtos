/*=========================================================================*//**
@file    driver_registration.c

@author  Daniel Zorychta

@brief   This file is used to registration drivers

@note    Copyright (C) 2012, 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "drivers/driver_registration.h"

/* include here modules headers */
#if defined(ARCH_stm32f1)
#include "drivers/uart.h"
#include "drivers/gpio.h"
#include "drivers/pll.h"
#include "drivers/tty.h"
#include "drivers/sdspi.h"
#elif defined(ARCH_posix)
#include "drivers/uart.h"
#include "drivers/tty.h"
#include "drivers/pll.h"
#else
#endif

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define USE_MODULE(module_name)                 #module_name

#define USE_DRIVER_INTERFACE(_drvmodule, _drvname, _major, _minor)\
{.drv_name    = _drvname,\
 .major       = _major,\
 .minor       = _minor,\
 .drv_init    = _##_drvmodule##_init,\
 .drv_release = _##_drvmodule##_release,\
 .drv_if      = {.handle    = NULL,\
                 .drv_open  = _##_drvmodule##_open,\
                 .drv_close = _##_drvmodule##_close,\
                 .drv_write = _##_drvmodule##_write,\
                 .drv_read  = _##_drvmodule##_read,\
                 .drv_ioctl = _##_drvmodule##_ioctl,\
                 .drv_info  = _##_drvmodule##_info,\
                 .drv_flush = _##_drvmodule##_flush}}

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/

/*==============================================================================
  Exported object definitions
==============================================================================*/
/* a table of names of used modules */
const char *_regdrv_module_name[] = {
        USE_MODULE(UART),
        USE_MODULE(GPIO),
        USE_MODULE(PLL),
        USE_MODULE(TTY),
        USE_MODULE(SDSPI),
};

/* a table of a drivers interfaces */
const struct _driver_entry _regdrv_driver_table[] = {
        USE_DRIVER_INTERFACE(UART , "uart1" , UART_DEV_1     , UART_PART_NONE),
        USE_DRIVER_INTERFACE(GPIO , "gpio"  , GPIO_DEV_NONE  , GPIO_PART_NONE),
        USE_DRIVER_INTERFACE(PLL  , "pll"   , PLL_DEV_NONE   , PLL_PART_NONE ),
        USE_DRIVER_INTERFACE(TTY  , "tty0"  , TTY_DEV_0      , TTY_PART_NONE ),
        USE_DRIVER_INTERFACE(TTY  , "tty1"  , TTY_DEV_1      , TTY_PART_NONE ),
        USE_DRIVER_INTERFACE(TTY  , "tty2"  , TTY_DEV_2      , TTY_PART_NONE ),
        USE_DRIVER_INTERFACE(TTY  , "tty3"  , TTY_DEV_3      , TTY_PART_NONE ),
        USE_DRIVER_INTERFACE(SDSPI, "sdspi" , SDSPI_MAJOR_NO , SDSPI_MINOR_NO),
};

/* number of items in above tables */
const uint _regdrv_driver_table_array_size = ARRAY_SIZE(_regdrv_driver_table);
const uint _regdrv_number_of_modules       = ARRAY_SIZE(_regdrv_module_name);

/*==============================================================================
  Function definitions
==============================================================================*/

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
