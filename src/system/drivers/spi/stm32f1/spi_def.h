/*=========================================================================*//**
@file    spi_def.h

@author  Daniel Zorychta

@brief   SPI module definitions.

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
#include "stm32f1/spi_ioctl.h"
#include "stm32f1/spi_cfg.h"

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
/** major number */
enum {
        #if defined(RCC_APB2ENR_SPI1EN) && (_SPI1_ENABLE > 0)
        _SPI1,
        #endif
        #if defined(RCC_APB1ENR_SPI2EN) && (_SPI2_ENABLE > 0)
        _SPI2,
        #endif
        #if defined(RCC_APB1ENR_SPI3EN) && (_SPI3_ENABLE > 0)
        _SPI3,
        #endif
        _NUMBER_OF_SPI_PERIPHERALS
};

/** minor number */
enum {
        _SPI_CS0 = 0,
        _SPI_CS1 = 1,
        _SPI_CS2 = 2,
        _SPI_CS3 = 3,
        _SPI_CS4 = 4,
        _SPI_CS5 = 5,
        _SPI_CS6 = 6,
        _SPI_CS7 = 7
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
