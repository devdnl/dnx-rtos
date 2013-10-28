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
#include "core/drivers.h"
#include "system/dnxmodule.h"

#include "stm32f1/uart_def.h"
#include "stm32f1/gpio_def.h"
#include "stm32f1/pll_def.h"
#include "stm32f1/sdspi_def.h"
#include "stm32f1/ethmac_def.h"
#include "tty_def.h"

/**
 * NOTE: To use drivers' ioctl definitions add include to file: ./src/system/include/system/ioctl.h
 */

/*==============================================================================
  External objects
==============================================================================*/
_IMPORT_MODULE(UART);
_IMPORT_MODULE(GPIO);
_IMPORT_MODULE(PLL);
_IMPORT_MODULE(TTY);
_IMPORT_MODULE(SDSPI);
_IMPORT_MODULE(ETHMAC);

/*==============================================================================
  Exported object definitions
==============================================================================*/
const char *_regdrv_module_name[] = {
        _USE_MODULE(UART),
        _USE_MODULE(GPIO),
        _USE_MODULE(PLL),
        _USE_MODULE(TTY),
        _USE_MODULE(SDSPI),
        _USE_MODULE(ETHMAC),
};

const struct _driver_entry _regdrv_driver_table[] = {
        _USE_DRIVER_INTERFACE(UART  , "uart1" , UART_DEV_1         , UART_MINOR_NUMBER  ),
        _USE_DRIVER_INTERFACE(GPIO  , "gpio"  , GPIO_MAJOR_NUMBER  , GPIO_MINOR_NUMBER  ),
        _USE_DRIVER_INTERFACE(PLL   , "pll"   , PLL_MAJOR_NUMBER   , PLL_MINOR_NUMBER   ),
        _USE_DRIVER_INTERFACE(TTY   , "tty0"  , TTY_DEV_0          , TTY_MINOR_NUMBER   ),
        _USE_DRIVER_INTERFACE(TTY   , "tty1"  , TTY_DEV_1          , TTY_MINOR_NUMBER   ),
        _USE_DRIVER_INTERFACE(TTY   , "tty2"  , TTY_DEV_2          , TTY_MINOR_NUMBER   ),
        _USE_DRIVER_INTERFACE(TTY   , "tty3"  , TTY_DEV_3          , TTY_MINOR_NUMBER   ),
        _USE_DRIVER_INTERFACE(SDSPI , "sdspi" , SDSPI_MAJOR_NUMBER , SDSPI_MINOR_NUMBER ),
        _USE_DRIVER_INTERFACE(ETHMAC, "ethmac", ETHMAC_MAJOR_NUMBER, ETHMAC_MINOR_NUMBER),
};

const int _regdrv_driver_table_array_size = ARRAY_SIZE(_regdrv_driver_table);
const int _regdrv_number_of_modules       = ARRAY_SIZE(_regdrv_module_name);

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
