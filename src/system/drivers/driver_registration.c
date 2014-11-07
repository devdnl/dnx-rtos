/*=========================================================================*//**
@file    driver_registration.c

@author  Daniel Zorychta

@brief   This file is used to registration drivers

@note    Copyright (C) 2012-2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Modules include files
==============================================================================*/
/* CT: module definition includes */
#if (__ENABLE_LOOP__)
#       ifdef ARCH_noarch
#               include "noarch/loop_def.h"
#       endif
#endif
#if (__ENABLE_GPIO__)
#       ifdef ARCH_stm32f1
#               include "stm32f1/gpio_def.h"
#       endif
#endif
#if (__ENABLE_AFIOM__)
#       ifdef ARCH_stm32f1
#               include "stm32f1/afiom_def.h"
#       endif
#endif
#if (__ENABLE_UART__)
#       ifdef ARCH_stm32f1
#               include "stm32f1/uart_def.h"
#       endif
#endif
#if (__ENABLE_PLL__)
#       ifdef ARCH_stm32f1
#               include "stm32f1/pll_def.h"
#       endif
#endif
#if (__ENABLE_SDSPI__)
#       ifdef ARCH_noarch
#               include "noarch/sdspi_def.h"
#       endif
#endif
#if (__ENABLE_ETHMAC__)
#       ifdef ARCH_stm32f1
#               include "stm32f1/ethmac_def.h"
#       endif
#endif
#if (__ENABLE_SPI__)
#       ifdef ARCH_stm32f1
#               include "stm32f1/spi_def.h"
#       endif
#endif
#if (__ENABLE_CRCM__)
#       ifdef ARCH_stm32f1
#               include "stm32f1/crcm_def.h"
#       endif
#endif
#if (__ENABLE_WDG__)
#       ifdef ARCH_stm32f1
#               include "stm32f1/wdg_def.h"
#       endif
#endif
#if (__ENABLE_USBD__)
#       ifdef ARCH_stm32f1
#               include "stm32f1/usbd_def.h"
#       endif
#endif
#if (__ENABLE_IRQ__)
#       ifdef ARCH_stm32f1
#               include "stm32f1/irq_def.h"
#       endif
#endif
#if (__ENABLE_I2C__)
#       ifdef ARCH_stm32f1
#               include "stm32f1/i2c_def.h"
#       endif
#endif
#if (__ENABLE_TTY__)
#       ifdef ARCH_noarch
#               include "noarch/tty_def.h"
#       endif
#endif

/**
 * NOTE: To use driver's ioctl definitions add include to file: ./src/system/include/sys/ioctl.h
 */

/*==============================================================================
  Modules interfaces
==============================================================================*/
/* CT: import of module interface */
#if (__ENABLE_LOOP__)
        _IMPORT_MODULE_INTERFACE(LOOP);
#endif
#if (__ENABLE_GPIO__)
        _IMPORT_MODULE_INTERFACE(GPIO);
#endif
#if (__ENABLE_AFIOM__)
        _IMPORT_MODULE_INTERFACE(AFIOM);
#endif
#if (__ENABLE_UART__)
        _IMPORT_MODULE_INTERFACE(UART);
#endif
#if (__ENABLE_PLL__)
        _IMPORT_MODULE_INTERFACE(PLL);
#endif
#if (__ENABLE_TTY__)
        _IMPORT_MODULE_INTERFACE(TTY);
#endif
#if (__ENABLE_SDSPI__)
        _IMPORT_MODULE_INTERFACE(SDSPI);
#endif
#if (__ENABLE_ETHMAC__)
        _IMPORT_MODULE_INTERFACE(ETHMAC);
#endif
#if (__ENABLE_CRCM__)
        _IMPORT_MODULE_INTERFACE(CRCM);
#endif
#if (__ENABLE_WDG__)
        _IMPORT_MODULE_INTERFACE(WDG);
#endif
#if (__ENABLE_SPI__)
        _IMPORT_MODULE_INTERFACE(SPI);
#endif
#if (__ENABLE_USBD__)
        _IMPORT_MODULE_INTERFACE(USBD);
#endif
#if (__ENABLE_IRQ__)
        _IMPORT_MODULE_INTERFACE(IRQ);
#endif
#if (__ENABLE_I2C__)
        _IMPORT_MODULE_INTERFACE(I2C);
#endif

/*==============================================================================
  Exported object definitions
==============================================================================*/
/**
 * Variable contains all names of modules. Each module must be registered here
 * only 1 time.
 */
const char *const _regdrv_module_name[] = {
        #if (__ENABLE_LOOP__)
        _MODULE_NAME(LOOP),
        #endif
        #if (__ENABLE_GPIO__)
        _MODULE_NAME(GPIO),
        #endif
        #if (__ENABLE_AFIOM__)
        _MODULE_NAME(AFIOM),
        #endif
        #if (__ENABLE_UART__)
        _MODULE_NAME(UART),
        #endif
        #if (__ENABLE_PLL__)
        _MODULE_NAME(PLL),
        #endif
        #if (__ENABLE_TTY__)
        _MODULE_NAME(TTY),
        #endif
        #if (__ENABLE_SDSPI__)
        _MODULE_NAME(SDSPI),
        #endif
        #if (__ENABLE_ETHMAC__)
        _MODULE_NAME(ETHMAC),
        #endif
        #if (__ENABLE_CRCM__)
        _MODULE_NAME(CRCM),
        #endif
        #if (__ENABLE_WDG__)
        _MODULE_NAME(WDG),
        #endif
        #if (__ENABLE_SPI__)
        _MODULE_NAME(SPI),
        #endif
        #if (__ENABLE_USBD__)
        _MODULE_NAME(USBD),
        #endif
        #if (__ENABLE_IRQ__)
        _MODULE_NAME(IRQ),
        #endif
        #if (__ENABLE_I2C__)
        _MODULE_NAME(I2C),
        #endif
};


/**
 * This table contains interfaces of drivers. Each driver have interface
 * connected to its module.
 */
const struct _driver_entry _regdrv_driver_table[] = {
        /* LOOP ============================================================= */
        #if (__ENABLE_LOOP__ && _LOOP_NUMBER_OF_DEVICES > 0)
        _DRIVER_INTERFACE(LOOP, "loop0", _LOOP0, _LOOP_MINOR_NUMBER),
        #endif
        #if (__ENABLE_LOOP__ && _LOOP_NUMBER_OF_DEVICES > 1)
        _DRIVER_INTERFACE(LOOP, "loop1", _LOOP1, _LOOP_MINOR_NUMBER),
        #endif
        #if (__ENABLE_LOOP__ && _LOOP_NUMBER_OF_DEVICES > 2)
        _DRIVER_INTERFACE(LOOP, "loop2", _LOOP2, _LOOP_MINOR_NUMBER),
        #endif
        #if (__ENABLE_LOOP__ && _LOOP_NUMBER_OF_DEVICES > 3)
        _DRIVER_INTERFACE(LOOP, "loop3", _LOOP3, _LOOP_MINOR_NUMBER),
        #endif
        #if (__ENABLE_LOOP__ && _LOOP_NUMBER_OF_DEVICES > 4)
        _DRIVER_INTERFACE(LOOP, "loop4", _LOOP4, _LOOP_MINOR_NUMBER),
        #endif
        #if (__ENABLE_LOOP__ && _LOOP_NUMBER_OF_DEVICES > 5)
        _DRIVER_INTERFACE(LOOP, "loop5", _LOOP5, _LOOP_MINOR_NUMBER),
        #endif
        #if (__ENABLE_LOOP__ && _LOOP_NUMBER_OF_DEVICES > 6)
        _DRIVER_INTERFACE(LOOP, "loop6", _LOOP6, _LOOP_MINOR_NUMBER),
        #endif
        #if (__ENABLE_LOOP__ && _LOOP_NUMBER_OF_DEVICES > 7)
        _DRIVER_INTERFACE(LOOP, "loop7", _LOOP7, _LOOP_MINOR_NUMBER),
        #endif

        /* UART ==============================================================*/
        #if (__ENABLE_UART__ && _UART1_ENABLE)
        _DRIVER_INTERFACE(UART, "uart1", _UART1, _UART_MINOR_NUMBER),
        #endif
        #if (__ENABLE_UART__ && _UART2_ENABLE)
        _DRIVER_INTERFACE(UART, "uart2", _UART2, _UART_MINOR_NUMBER),
        #endif
        #if (__ENABLE_UART__ && _UART3_ENABLE)
        _DRIVER_INTERFACE(UART, "uart3", _UART3, _UART_MINOR_NUMBER),
        #endif
        #if (__ENABLE_UART__ && _UART4_ENABLE)
        _DRIVER_INTERFACE(UART, "uart4", _UART4, _UART_MINOR_NUMBER),
        #endif
        #if (__ENABLE_UART__ && _UART5_ENABLE)
        _DRIVER_INTERFACE(UART, "uart5", _UART5, _UART_MINOR_NUMBER),
        #endif

        /* PLL ===============================================================*/
        #if (__ENABLE_PLL__)
        _DRIVER_INTERFACE(PLL, "pll", _PLL_MAJOR_NUMBER, _PLL_MINOR_NUMBER),
        #endif

        /* TTY ===============================================================*/
        #if (__ENABLE_TTY__ && _TTY_NUMBER_OF_VT > 0)
        _DRIVER_INTERFACE(TTY, "tty0", _TTY0, _TTY_MINOR_NUMBER),
        #endif
        #if (__ENABLE_TTY__ && _TTY_NUMBER_OF_VT > 1)
        _DRIVER_INTERFACE(TTY, "tty1", _TTY1, _TTY_MINOR_NUMBER),
        #endif
        #if (__ENABLE_TTY__ && _TTY_NUMBER_OF_VT > 2)
        _DRIVER_INTERFACE(TTY, "tty2", _TTY2, _TTY_MINOR_NUMBER),
        #endif
        #if (__ENABLE_TTY__ && _TTY_NUMBER_OF_VT > 3)
        _DRIVER_INTERFACE(TTY, "tty3", _TTY3, _TTY_MINOR_NUMBER),
        #endif

        /* SDSPI =============================================================*/
        #if (__ENABLE_SDSPI__) && (SDSPI_NUMBER_OF_CARDS >= 1)
        _DRIVER_INTERFACE(SDSPI, "sdspia" , _SDSPI_CARD_0, _SDSPI_FULL_VOLUME),
        _DRIVER_INTERFACE(SDSPI, "sdspia1", _SDSPI_CARD_0, _SDSPI_PARTITION_1),
        _DRIVER_INTERFACE(SDSPI, "sdspia2", _SDSPI_CARD_0, _SDSPI_PARTITION_2),
        _DRIVER_INTERFACE(SDSPI, "sdspia3", _SDSPI_CARD_0, _SDSPI_PARTITION_3),
        _DRIVER_INTERFACE(SDSPI, "sdspia4", _SDSPI_CARD_0, _SDSPI_PARTITION_4),
        #endif
        #if (__ENABLE_SDSPI__) && (SDSPI_NUMBER_OF_CARDS >= 2)
        _DRIVER_INTERFACE(SDSPI, "sdspib" , _SDSPI_CARD_1, _SDSPI_FULL_VOLUME),
        _DRIVER_INTERFACE(SDSPI, "sdspib1", _SDSPI_CARD_1, _SDSPI_PARTITION_1),
        _DRIVER_INTERFACE(SDSPI, "sdspib2", _SDSPI_CARD_1, _SDSPI_PARTITION_2),
        _DRIVER_INTERFACE(SDSPI, "sdspib3", _SDSPI_CARD_1, _SDSPI_PARTITION_3),
        _DRIVER_INTERFACE(SDSPI, "sdspib4", _SDSPI_CARD_1, _SDSPI_PARTITION_4),
        #endif

        /* ETH ===============================================================*/
        #if (__ENABLE_ETHMAC__)
        _DRIVER_INTERFACE(ETHMAC, "ethmac", _ETHMAC_MAJOR_NUMBER, _ETHMAC_MINOR_NUMBER),
        #endif

        /* CRC ===============================================================*/
        #if (__ENABLE_CRCM__)
        _DRIVER_INTERFACE(CRCM, "crcm", _CRCM_MAJOR_NUMBER, _CRCM_MINOR_NUMBER),
        #endif

        /* WDG ===============================================================*/
        #if (__ENABLE_WDG__)
        _DRIVER_INTERFACE(WDG, "wdg", _WDG_MAJOR_NUMBER, _WDG_MINOR_NUMBER),
        #endif

        /* SPI ===============================================================*/
        #if (__ENABLE_SPI__ && _SPI1_ENABLE && _SPI1_NUMBER_OF_SLAVES >= 1)
        _DRIVER_INTERFACE(SPI, "spi1-0", _SPI1, 0),
        #endif
        #if (__ENABLE_SPI__ && _SPI1_ENABLE && _SPI1_NUMBER_OF_SLAVES >= 2)
        _DRIVER_INTERFACE(SPI, "spi1-1", _SPI1, 1),
        #endif
        #if (__ENABLE_SPI__ && _SPI1_ENABLE && _SPI1_NUMBER_OF_SLAVES >= 3)
        _DRIVER_INTERFACE(SPI, "spi1-2", _SPI1, 2),
        #endif
        #if (__ENABLE_SPI__ && _SPI1_ENABLE && _SPI1_NUMBER_OF_SLAVES >= 4)
        _DRIVER_INTERFACE(SPI, "spi1-3", _SPI1, 3),
        #endif
        #if (__ENABLE_SPI__ && _SPI1_ENABLE && _SPI1_NUMBER_OF_SLAVES >= 5)
        _DRIVER_INTERFACE(SPI, "spi1-4", _SPI1, 4),
        #endif
        #if (__ENABLE_SPI__ && _SPI1_ENABLE && _SPI1_NUMBER_OF_SLAVES >= 6)
        _DRIVER_INTERFACE(SPI, "spi1-5", _SPI1, 5),
        #endif
        #if (__ENABLE_SPI__ && _SPI1_ENABLE && _SPI1_NUMBER_OF_SLAVES >= 7)
        _DRIVER_INTERFACE(SPI, "spi1-6", _SPI1, 6),
        #endif
        #if (__ENABLE_SPI__ && _SPI1_ENABLE && _SPI1_NUMBER_OF_SLAVES >= 8)
        _DRIVER_INTERFACE(SPI, "spi1-7", _SPI1, 7),
        #endif

        #if (__ENABLE_SPI__ && _SPI2_ENABLE && _SPI2_NUMBER_OF_SLAVES >= 1)
        _DRIVER_INTERFACE(SPI, "spi2-0", _SPI2, 0),
        #endif
        #if (__ENABLE_SPI__ && _SPI2_ENABLE && _SPI2_NUMBER_OF_SLAVES >= 2)
        _DRIVER_INTERFACE(SPI, "spi2-1", _SPI2, 1),
        #endif
        #if (__ENABLE_SPI__ && _SPI2_ENABLE && _SPI2_NUMBER_OF_SLAVES >= 3)
        _DRIVER_INTERFACE(SPI, "spi2-2", _SPI2, 2),
        #endif
        #if (__ENABLE_SPI__ && _SPI2_ENABLE && _SPI2_NUMBER_OF_SLAVES >= 4)
        _DRIVER_INTERFACE(SPI, "spi2-3", _SPI2, 3),
        #endif
        #if (__ENABLE_SPI__ && _SPI2_ENABLE && _SPI2_NUMBER_OF_SLAVES >= 5)
        _DRIVER_INTERFACE(SPI, "spi2-4", _SPI2, 4),
        #endif
        #if (__ENABLE_SPI__ && _SPI2_ENABLE && _SPI2_NUMBER_OF_SLAVES >= 6)
        _DRIVER_INTERFACE(SPI, "spi2-5", _SPI2, 5),
        #endif
        #if (__ENABLE_SPI__ && _SPI2_ENABLE && _SPI2_NUMBER_OF_SLAVES >= 7)
        _DRIVER_INTERFACE(SPI, "spi2-6", _SPI2, 6),
        #endif
        #if (__ENABLE_SPI__ && _SPI2_ENABLE && _SPI2_NUMBER_OF_SLAVES >= 8)
        _DRIVER_INTERFACE(SPI, "spi2-7", _SPI2, 7),
        #endif

        #if (__ENABLE_SPI__ && _SPI3_ENABLE && _SPI3_NUMBER_OF_SLAVES >= 1)
        _DRIVER_INTERFACE(SPI, "spi3-0", _SPI3, 0),
        #endif
        #if (__ENABLE_SPI__ && _SPI3_ENABLE && _SPI3_NUMBER_OF_SLAVES >= 2)
        _DRIVER_INTERFACE(SPI, "spi3-1", _SPI3, 1),
        #endif
        #if (__ENABLE_SPI__ && _SPI3_ENABLE && _SPI3_NUMBER_OF_SLAVES >= 3)
        _DRIVER_INTERFACE(SPI, "spi3-2", _SPI3, 2),
        #endif
        #if (__ENABLE_SPI__ && _SPI3_ENABLE && _SPI3_NUMBER_OF_SLAVES >= 4)
        _DRIVER_INTERFACE(SPI, "spi3-3", _SPI3, 3),
        #endif
        #if (__ENABLE_SPI__ && _SPI3_ENABLE && _SPI3_NUMBER_OF_SLAVES >= 5)
        _DRIVER_INTERFACE(SPI, "spi3-4", _SPI3, 4),
        #endif
        #if (__ENABLE_SPI__ && _SPI3_ENABLE && _SPI3_NUMBER_OF_SLAVES >= 6)
        _DRIVER_INTERFACE(SPI, "spi3-5", _SPI3, 5),
        #endif
        #if (__ENABLE_SPI__ && _SPI3_ENABLE && _SPI3_NUMBER_OF_SLAVES >= 7)
        _DRIVER_INTERFACE(SPI, "spi3-6", _SPI3, 6),
        #endif
        #if (__ENABLE_SPI__ && _SPI3_ENABLE && _SPI3_NUMBER_OF_SLAVES >= 8)
        _DRIVER_INTERFACE(SPI, "spi3-7", _SPI3, 7),
        #endif

        /* AFIO ==============================================================*/
        #if (__ENABLE_AFIOM__)
        _DRIVER_INTERFACE(AFIOM, "afiom", _AFIOM_MAJOR_NUMBER, _AFIOM_MINOR_NUMBER),
        #endif

        /* GPIO ==============================================================*/
        #if (__ENABLE_GPIO__)
        _DRIVER_INTERFACE(GPIO, "gpio", _GPIO_MAJOR_NUMBER, _GPIO_MINOR_NUMBER),
        #endif

        /* USB ===============================================================*/
        #if (__ENABLE_USBD__)
        _DRIVER_INTERFACE(USBD, "usbd-ep0", _USBD_MAJOR_NUMBER, _USBD_MINOR_NUMBER_EP_0),
        _DRIVER_INTERFACE(USBD, "usbd-ep1", _USBD_MAJOR_NUMBER, _USBD_MINOR_NUMBER_EP_1),
        _DRIVER_INTERFACE(USBD, "usbd-ep2", _USBD_MAJOR_NUMBER, _USBD_MINOR_NUMBER_EP_2),
        _DRIVER_INTERFACE(USBD, "usbd-ep3", _USBD_MAJOR_NUMBER, _USBD_MINOR_NUMBER_EP_3),
        _DRIVER_INTERFACE(USBD, "usbd-ep4", _USBD_MAJOR_NUMBER, _USBD_MINOR_NUMBER_EP_4),
        _DRIVER_INTERFACE(USBD, "usbd-ep5", _USBD_MAJOR_NUMBER, _USBD_MINOR_NUMBER_EP_5),
        _DRIVER_INTERFACE(USBD, "usbd-ep6", _USBD_MAJOR_NUMBER, _USBD_MINOR_NUMBER_EP_6),
        _DRIVER_INTERFACE(USBD, "usbd-ep7", _USBD_MAJOR_NUMBER, _USBD_MINOR_NUMBER_EP_7),
        #endif

        /* IRQ ===============================================================*/
        #if (__ENABLE_IRQ__)
        _DRIVER_INTERFACE(IRQ, "irq", _IRQ_MAJOR_NUMBER, _IRQ_MINOR_NUMBER),
        #endif

        /* I2C ===============================================================*/
        #if (__ENABLE_I2C__ && _I2C1_ENABLE && _I2C1_NUMBER_OF_DEVICES >= 1)
        _DRIVER_INTERFACE(I2C, "i2c1-0", _I2C1, _I2C_DEV_0),
        #endif
        #if (__ENABLE_I2C__ && _I2C1_ENABLE && _I2C1_NUMBER_OF_DEVICES >= 2)
        _DRIVER_INTERFACE(I2C, "i2c1-1", _I2C1, _I2C_DEV_1),
        #endif
        #if (__ENABLE_I2C__ && _I2C1_ENABLE && _I2C1_NUMBER_OF_DEVICES >= 3)
        _DRIVER_INTERFACE(I2C, "i2c1-2", _I2C1, _I2C_DEV_2),
        #endif
        #if (__ENABLE_I2C__ && _I2C1_ENABLE && _I2C1_NUMBER_OF_DEVICES >= 4)
        _DRIVER_INTERFACE(I2C, "i2c1-3", _I2C1, _I2C_DEV_3),
        #endif
        #if (__ENABLE_I2C__ && _I2C1_ENABLE && _I2C1_NUMBER_OF_DEVICES >= 5)
        _DRIVER_INTERFACE(I2C, "i2c1-4", _I2C1, _I2C_DEV_4),
        #endif
        #if (__ENABLE_I2C__ && _I2C1_ENABLE && _I2C1_NUMBER_OF_DEVICES >= 6)
        _DRIVER_INTERFACE(I2C, "i2c1-5", _I2C1, _I2C_DEV_5),
        #endif
        #if (__ENABLE_I2C__ && _I2C1_ENABLE && _I2C1_NUMBER_OF_DEVICES >= 7)
        _DRIVER_INTERFACE(I2C, "i2c1-6", _I2C1, _I2C_DEV_6),
        #endif
        #if (__ENABLE_I2C__ && _I2C1_ENABLE && _I2C1_NUMBER_OF_DEVICES >= 8)
        _DRIVER_INTERFACE(I2C, "i2c1-7", _I2C1, _I2C_DEV_7),
        #endif

        #if (__ENABLE_I2C__ && _I2C2_ENABLE && _I2C2_NUMBER_OF_DEVICES >= 1)
        _DRIVER_INTERFACE(I2C, "i2c2-0", _I2C2, _I2C_DEV_0),
        #endif
        #if (__ENABLE_I2C__ && _I2C2_ENABLE && _I2C2_NUMBER_OF_DEVICES >= 2)
        _DRIVER_INTERFACE(I2C, "i2c2-1", _I2C2, _I2C_DEV_1),
        #endif
        #if (__ENABLE_I2C__ && _I2C2_ENABLE && _I2C2_NUMBER_OF_DEVICES >= 3)
        _DRIVER_INTERFACE(I2C, "i2c2-2", _I2C2, _I2C_DEV_2),
        #endif
        #if (__ENABLE_I2C__ && _I2C2_ENABLE && _I2C2_NUMBER_OF_DEVICES >= 4)
        _DRIVER_INTERFACE(I2C, "i2c2-3", _I2C2, _I2C_DEV_3),
        #endif
        #if (__ENABLE_I2C__ && _I2C2_ENABLE && _I2C2_NUMBER_OF_DEVICES >= 5)
        _DRIVER_INTERFACE(I2C, "i2c2-4", _I2C2, _I2C_DEV_4),
        #endif
        #if (__ENABLE_I2C__ && _I2C2_ENABLE && _I2C2_NUMBER_OF_DEVICES >= 6)
        _DRIVER_INTERFACE(I2C, "i2c2-5", _I2C2, _I2C_DEV_5),
        #endif
        #if (__ENABLE_I2C__ && _I2C2_ENABLE && _I2C2_NUMBER_OF_DEVICES >= 7)
        _DRIVER_INTERFACE(I2C, "i2c2-6", _I2C2, _I2C_DEV_6),
        #endif
        #if (__ENABLE_I2C__ && _I2C2_ENABLE && _I2C2_NUMBER_OF_DEVICES >= 8)
        _DRIVER_INTERFACE(I2C, "i2c2-7", _I2C2, _I2C_DEV_7),
        #endif
};


/** variable contains number of registered drivers */
const uint _regdrv_size_of_driver_table = ARRAY_SIZE(_regdrv_driver_table);

/** variable contains number of registered modules */
const uint _regdrv_number_of_modules    = ARRAY_SIZE(_regdrv_module_name);

/*==============================================================================
  End of file
==============================================================================*/
