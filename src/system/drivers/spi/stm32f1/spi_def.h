/*=========================================================================*//**
@file    spi_def.h

@author  Daniel Zorychta

@brief

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

#ifndef _SPI_DEF_H_
#define _SPI_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "core/ioctl_macros.h"
#include "stm32f1/spi_cfg.h"

/*==============================================================================
  Exported macros
==============================================================================*/
#define SPI_IORQ_SET_CONFIGURATION                      _IOW ('S', 0x00, struct SPI_config*)
#define SPI_IORQ_GET_CONFIGURATION                      _IOR ('S', 0x01, struct SPI_config*)
#define SPI_IORQ_LOCK                                   _IO  ('S', 0x02)
#define SPI_IORQ_UNLOCK                                 _IO  ('S', 0x03)
#define SPI_IORQ_SELECT                                 _IO  ('S', 0x04)
#define SPI_IORQ_DESELECT                               _IO  ('S', 0x05)
#define SPI_IORQ_TRANSMIT                               _IOWR('S', 0x06, u8_t*)

/*==============================================================================
  Exported object types
==============================================================================*/
/** port names */
enum SPI_MAJOR_NUMBER
{
        #if defined(RCC_APB2ENR_SPI1EN) && (_SPI1_ENABLE > 0)
        _SPI_DEV_1,
        #endif
        #if defined(RCC_APB1ENR_SPI2EN) && (_SPI2_ENABLE > 0)
        _SPI_DEV_2,
        #endif
        #if defined(RCC_APB1ENR_SPI3EN) && (_SPI3_ENABLE > 0)
        _SPI_DEV_3,
        #endif
        _SPI_DEV_NUMBER
};

/* SPI clock divider */
enum SPI_clk_divider {
        SPI_CLK_DIV_2,
        SPI_CLK_DIV_4,
        SPI_CLK_DIV_8,
        SPI_CLK_DIV_16,
        SPI_CLK_DIV_32,
        SPI_CLK_DIV_64,
        SPI_CLK_DIV_128,
        SPI_CLK_DIV_256
};

/* SPI modes */
enum SPI_mode {
        SPI_MODE_0,     /* CPOL = 0; CPHA = 0 (SCK 0 at idle, capture on rising edge)  */
        SPI_MODE_1,     /* CPOL = 0; CPHA = 1 (SCK 0 at idle, capture on falling edge) */
        SPI_MODE_2,     /* CPOL = 1; CPHA = 0 (SCK 1 at idle, capture on falling edge) */
        SPI_MODE_3      /* CPOL = 1; CPHA = 1 (SCK 1 at idle, capture on rising edge)  */
};

/* SPI configuration type */
struct SPI_config {
        u8_t                    dummy_byte  : 8;
        enum SPI_clk_divider    clk_divider : 3;
        enum SPI_mode           mode        : 2;
        bool                    msb_first   : 1;
};

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

#endif /* _SPI_DEF_H_ */
/*==============================================================================
  End of file
==============================================================================*/
