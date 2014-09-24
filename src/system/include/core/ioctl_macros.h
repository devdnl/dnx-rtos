/*=========================================================================*//**
@file    ioctl_macros.h

@author  Daniel Zorychta

@brief   ioctl build macros.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _IOCTL_MACROS_H_
#define _IOCTL_MACROS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/

/*==============================================================================
  Exported macros
==============================================================================*/
#define _IO(g, n)               (((u32_t)(g) << 16) | ((u32_t)(n) & 0xFFFF))
#define _IOR(g, n, t)           (((u32_t)(g) << 16) | ((u32_t)(n) & 0xFFFF))
#define _IOW(g, n, t)           (((u32_t)(g) << 16) | ((u32_t)(n) & 0xFFFF))
#define _IOWR(g, n, t)          (((u32_t)(g) << 16) | ((u32_t)(n) & 0xFFFF))

enum _IO_GROUP {
        _IO_GROUP_AFIO,
        _IO_GROUP_CRC,
        _IO_GROUP_ETH,
        _IO_GROUP_GPIO,
        _IO_GROUP_PLL,
        _IO_GROUP_CLOCK,
        _IO_GROUP_SDSPI,
        _IO_GROUP_SDIO,
        _IO_GROUP_SPI,
        _IO_GROUP_TTY,
        _IO_GROUP_UART,
        _IO_GROUP_USART,
        _IO_GROUP_WDG,
        _IO_GROUP_WWDG,
        _IO_GROUP_PIPE,
        _IO_GROUP_VFS,
        _IO_GROUP_I2C,
        _IO_GROUP_I2S,
        _IO_GROUP_TIM,
        _IO_GROUP_PWM,
        _IO_GROUP_SYSCFG,
        _IO_GROUP_CEC,
        _IO_GROUP_HDMI,
        _IO_GROUP_DMA,
        _IO_GROUP_USB,
        _IO_GROUP_OTG,
        _IO_GROUP_MEM,
        _IO_GROUP_RTC,
        _IO_GROUP_BKP,
        _IO_GROUP_CAN,
        _IO_GROUP_1WIRE,
        _IO_GROUP_ADC,
        _IO_GROUP_DAC,
        _IO_GROUP_AES,
        _IO_GROUP_DES,
        _IO_GROUP_CRYPTO,
        _IO_GROUP_HASH,
        _IO_GROUP_RAND,
        _IO_GROUP_NULL,
        _IO_GROUP_FULL,
        _IO_GROUP_ZERO,
        _IO_GROUP_SMB,
        _IO_GROUP_FSMC,
        _IO_GROUP_FDMC,
        _IO_GROUP_CPU,
        _IO_GROUP_FLASH,
        _IO_GROUP_RAM,
        _IO_GROUP_IRQ,
        _IO_GROUP_LOOP,
        _IO_GROUP_GENERIC,
};

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _IOCTL_MACROS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
