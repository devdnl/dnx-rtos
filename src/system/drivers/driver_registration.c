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

/*==============================================================================
  Include files
==============================================================================*/
#include "core/modctrl.h"
#include "core/module.h"
#include <dnx/misc.h>

#ifdef ARCH_stm32f1
#       if (__ENABLE_GPIO__)
#               include "stm32f1/gpio_def.h"
#       endif
#       if (__ENABLE_AFIO__)
#               include "stm32f1/afio_def.h"
#       endif
#       if (__ENABLE_UART__)
#               include "stm32f1/uart_def.h"
#       endif
#       if (__ENABLE_PLL__)
#               include "stm32f1/pll_def.h"
#       endif
#       if (__ENABLE_SDSPI__)
#               include "stm32f1/sdspi_def.h"
#       endif
#       if (__ENABLE_ETH__)
#               include "stm32f1/ethmac_def.h"
#       endif
#       if (__ENABLE_SPI__)
#               include "stm32f1/spi_def.h"
#       endif
#       if (__ENABLE_CRC__)
#               include "stm32f1/crc_def.h"
#       endif
#       if (__ENABLE_WDG__)
#               include "stm32f1/wdg_def.h"
#       endif
#else
#       error Unknown CPU architecture!
#endif

#if (__ENABLE_TTY__)
#       include "tty_def.h"
#endif

/**
 * NOTE: To use drivers' ioctl definitions add include to file: ./src/system/include/sys/ioctl.h
 */

/*==============================================================================
  External objects
==============================================================================*/
#if (__ENABLE_GPIO__)
_IMPORT_MODULE(GPIO);
#endif

#if (__ENABLE_AFIO__)
_IMPORT_MODULE(afio);
#endif

#if (__ENABLE_UART__)
_IMPORT_MODULE(UART);
#endif

#if (__ENABLE_PLL__)
_IMPORT_MODULE(PLL);
#endif

#if (__ENABLE_TTY__)
_IMPORT_MODULE(TTY);
#endif

#if (__ENABLE_SDSPI__)
_IMPORT_MODULE(SDSPI);
#endif

#if (__ENABLE_ETH__)
_IMPORT_MODULE(ETHMAC);
#endif

#if (__ENABLE_CRC__)
_IMPORT_MODULE(CRCCU);
#endif

#if (__ENABLE_WDG__)
_IMPORT_MODULE(WDG);
#endif

#if (__ENABLE_SPI__)
_IMPORT_MODULE(SPI);
#endif

/*==============================================================================
  Exported object definitions
==============================================================================*/
const char *const _regdrv_module_name[] = {
#if (__ENABLE_GPIO__)
        _USE_MODULE(GPIO),
#endif

#if (__ENABLE_AFIO__)
        _USE_MODULE(afio),
#endif

#if (__ENABLE_UART__)
        _USE_MODULE(UART),
#endif

#if (__ENABLE_PLL__)
        _USE_MODULE(PLL),
#endif

#if (__ENABLE_TTY__)
        _USE_MODULE(TTY),
#endif

#if (__ENABLE_SDSPI__)
        _USE_MODULE(SDSPI),
#endif

#if (__ENABLE_ETH__)
        _USE_MODULE(ETHMAC),
#endif

#if (__ENABLE_CRC__)
        _USE_MODULE(CRCCU),
#endif

#if (__ENABLE_WDG__)
        _USE_MODULE(WDG),
#endif

#if (__ENABLE_SPI__)
        _USE_MODULE(SPI),
#endif
};

const struct _driver_entry _regdrv_driver_table[] = {
#if (__ENABLE_UART__ && _UART1_ENABLE)
        _USE_DRIVER_INTERFACE(UART, "uart1", _UART1, _UART_MINOR_NUMBER),
#endif
#if (__ENABLE_UART__ && _UART2_ENABLE)
        _USE_DRIVER_INTERFACE(UART, "uart2", _UART2, _UART_MINOR_NUMBER),
#endif
#if (__ENABLE_UART__ && _UART3_ENABLE)
        _USE_DRIVER_INTERFACE(UART, "uart3", _UART3, _UART_MINOR_NUMBER),
#endif
#if (__ENABLE_UART__ && _UART4_ENABLE)
        _USE_DRIVER_INTERFACE(UART, "uart4", _UART4, _UART_MINOR_NUMBER),
#endif

#if (__ENABLE_PLL__)
        _USE_DRIVER_INTERFACE(PLL, "pll", _PLL_MAJOR_NUMBER, _PLL_MINOR_NUMBER),
#endif
#if (__ENABLE_TTY__ && _TTY_NUMBER_OF_VT > 0)
        _USE_DRIVER_INTERFACE(TTY, "tty0", _TTY0, _TTY_MINOR_NUMBER),
#endif
#if (__ENABLE_TTY__ && _TTY_NUMBER_OF_VT > 1)
        _USE_DRIVER_INTERFACE(TTY, "tty1", _TTY1, _TTY_MINOR_NUMBER),
#endif
#if (__ENABLE_TTY__ && _TTY_NUMBER_OF_VT > 2)
        _USE_DRIVER_INTERFACE(TTY, "tty2", _TTY2, _TTY_MINOR_NUMBER),
#endif
#if (__ENABLE_TTY__ && _TTY_NUMBER_OF_VT > 3)
        _USE_DRIVER_INTERFACE(TTY, "tty3", _TTY3, _TTY_MINOR_NUMBER),
#endif

#if (__ENABLE_SDSPI__)
        _USE_DRIVER_INTERFACE(SDSPI, "sda" , _SDSPI_CARD_0, _SDSPI_FULL_VOLUME),
        _USE_DRIVER_INTERFACE(SDSPI, "sda1", _SDSPI_CARD_0, _SDSPI_PARTITION_1),
        _USE_DRIVER_INTERFACE(SDSPI, "sda2", _SDSPI_CARD_0, _SDSPI_PARTITION_2),
        _USE_DRIVER_INTERFACE(SDSPI, "sda3", _SDSPI_CARD_0, _SDSPI_PARTITION_3),
        _USE_DRIVER_INTERFACE(SDSPI, "sda4", _SDSPI_CARD_0, _SDSPI_PARTITION_4),
#endif

#if (__ENABLE_ETH__)
        _USE_DRIVER_INTERFACE(ETHMAC, "ethmac", _ETHMAC_MAJOR_NUMBER, _ETHMAC_MINOR_NUMBER),
#endif

#if (__ENABLE_CRC__)
        _USE_DRIVER_INTERFACE(CRCCU, "crc", _CRC_MAJOR_NUMBER, _CRC_MINOR_NUMBER),
#endif

#if (__ENABLE_WDG__)
        _USE_DRIVER_INTERFACE(WDG, "wdg", _WDG_MAJOR_NUMBER, _WDG_MINOR_NUMBER),
#endif

#if (__ENABLE_SPI__ && _SPI1_ENABLE && _SPI1_NUMBER_OF_SLAVES >= 1)
        _USE_DRIVER_INTERFACE(SPI, "spi1_cs0", _SPI1, 0),
#endif
#if (__ENABLE_SPI__ && _SPI1_ENABLE && _SPI1_NUMBER_OF_SLAVES >= 2)
        _USE_DRIVER_INTERFACE(SPI, "spi1_cs1", _SPI1, 1),
#endif
#if (__ENABLE_SPI__ && _SPI1_ENABLE && _SPI1_NUMBER_OF_SLAVES >= 3)
        _USE_DRIVER_INTERFACE(SPI, "spi1_cs2", _SPI1, 2),
#endif
#if (__ENABLE_SPI__ && _SPI1_ENABLE && _SPI1_NUMBER_OF_SLAVES >= 4)
        _USE_DRIVER_INTERFACE(SPI, "spi1_cs3", _SPI1, 3),
#endif
#if (__ENABLE_SPI__ && _SPI1_ENABLE && _SPI1_NUMBER_OF_SLAVES >= 5)
        _USE_DRIVER_INTERFACE(SPI, "spi1_cs4", _SPI1, 4),
#endif
#if (__ENABLE_SPI__ && _SPI1_ENABLE && _SPI1_NUMBER_OF_SLAVES >= 6)
        _USE_DRIVER_INTERFACE(SPI, "spi1_cs5", _SPI1, 5),
#endif
#if (__ENABLE_SPI__ && _SPI1_ENABLE && _SPI1_NUMBER_OF_SLAVES >= 7)
        _USE_DRIVER_INTERFACE(SPI, "spi1_cs6", _SPI1, 6),
#endif
#if (__ENABLE_SPI__ && _SPI1_ENABLE && _SPI1_NUMBER_OF_SLAVES >= 8)
        _USE_DRIVER_INTERFACE(SPI, "spi1_cs7", _SPI1, 7),
#endif

#if (__ENABLE_SPI__ && _SPI2_ENABLE && _SPI2_NUMBER_OF_SLAVES >= 1)
        _USE_DRIVER_INTERFACE(SPI, "spi2_cs0", _SPI2, 0),
#endif
#if (__ENABLE_SPI__ && _SPI2_ENABLE && _SPI2_NUMBER_OF_SLAVES >= 2)
        _USE_DRIVER_INTERFACE(SPI, "spi2_cs1", _SPI2, 1),
#endif
#if (__ENABLE_SPI__ && _SPI2_ENABLE && _SPI2_NUMBER_OF_SLAVES >= 3)
        _USE_DRIVER_INTERFACE(SPI, "spi2_cs2", _SPI2, 2),
#endif
#if (__ENABLE_SPI__ && _SPI2_ENABLE && _SPI2_NUMBER_OF_SLAVES >= 4)
        _USE_DRIVER_INTERFACE(SPI, "spi2_cs3", _SPI2, 3),
#endif
#if (__ENABLE_SPI__ && _SPI2_ENABLE && _SPI2_NUMBER_OF_SLAVES >= 5)
        _USE_DRIVER_INTERFACE(SPI, "spi2_cs4", _SPI2, 4),
#endif
#if (__ENABLE_SPI__ && _SPI2_ENABLE && _SPI2_NUMBER_OF_SLAVES >= 6)
        _USE_DRIVER_INTERFACE(SPI, "spi2_cs5", _SPI2, 5),
#endif
#if (__ENABLE_SPI__ && _SPI2_ENABLE && _SPI2_NUMBER_OF_SLAVES >= 7)
        _USE_DRIVER_INTERFACE(SPI, "spi2_cs6", _SPI2, 6),
#endif
#if (__ENABLE_SPI__ && _SPI2_ENABLE && _SPI2_NUMBER_OF_SLAVES >= 8)
        _USE_DRIVER_INTERFACE(SPI, "spi2_cs7", _SPI2, 7),
#endif

#if (__ENABLE_SPI__ && _SPI3_ENABLE && _SPI3_NUMBER_OF_SLAVES >= 1)
        _USE_DRIVER_INTERFACE(SPI, "spi3_cs0", _SPI3, 0),
#endif
#if (__ENABLE_SPI__ && _SPI3_ENABLE && _SPI3_NUMBER_OF_SLAVES >= 2)
        _USE_DRIVER_INTERFACE(SPI, "spi3_cs1", _SPI3, 1),
#endif
#if (__ENABLE_SPI__ && _SPI3_ENABLE && _SPI3_NUMBER_OF_SLAVES >= 3)
        _USE_DRIVER_INTERFACE(SPI, "spi3_cs2", _SPI3, 2),
#endif
#if (__ENABLE_SPI__ && _SPI3_ENABLE && _SPI3_NUMBER_OF_SLAVES >= 4)
        _USE_DRIVER_INTERFACE(SPI, "spi3_cs3", _SPI3, 3),
#endif
#if (__ENABLE_SPI__ && _SPI3_ENABLE && _SPI3_NUMBER_OF_SLAVES >= 5)
        _USE_DRIVER_INTERFACE(SPI, "spi3_cs4", _SPI3, 4),
#endif
#if (__ENABLE_SPI__ && _SPI3_ENABLE && _SPI3_NUMBER_OF_SLAVES >= 6)
        _USE_DRIVER_INTERFACE(SPI, "spi3_cs5", _SPI3, 5),
#endif
#if (__ENABLE_SPI__ && _SPI3_ENABLE && _SPI3_NUMBER_OF_SLAVES >= 7)
        _USE_DRIVER_INTERFACE(SPI, "spi3_cs6", _SPI3, 6),
#endif
#if (__ENABLE_SPI__ && _SPI3_ENABLE && _SPI3_NUMBER_OF_SLAVES >= 8)
        _USE_DRIVER_INTERFACE(SPI, "spi3_cs7", _SPI3, 7),
#endif

#if (__ENABLE_AFIO__)
        _USE_DRIVER_INTERFACE(afio, "afio", _AFIO_MAJOR_NUMBER, _AFIO_MINOR_NUMBER),
#endif

#if (__ENABLE_GPIO__)
        _USE_DRIVER_INTERFACE(GPIO, "gpio", _GPIO_MAJOR_NUMBER, _GPIO_MINOR_NUMBER),
#endif
};

const uint _regdrv_size_of_driver_table = ARRAY_SIZE(_regdrv_driver_table);
const uint _regdrv_number_of_modules    = ARRAY_SIZE(_regdrv_module_name);

/*==============================================================================
  End of file
==============================================================================*/
