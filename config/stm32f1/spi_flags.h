/*=========================================================================*//**
@file    spi_flags.h

@author  Daniel Zorychta

@brief   SPI module configuration flags.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _SPI_FLAGS_H_
#define _SPI_FLAGS_H_

#define __SPI_SPI1_ENABLE__ _NO_
#define __SPI_SPI2_ENABLE__ _NO_
#define __SPI_SPI3_ENABLE__ _YES_
#define __SPI_SPI1_PRIORITY__ CONFIG_USER_IRQ_PRIORITY
#define __SPI_SPI2_PRIORITY__ CONFIG_USER_IRQ_PRIORITY
#define __SPI_SPI3_PRIORITY__ CONFIG_USER_IRQ_PRIORITY
#define __SPI_DEFAULT_DUMMY_BYTE__ 0xFF
#define __SPI_DEFAULT_CLK_DIV__ SPI_CLK_DIV_4
#define __SPI_DEFAULT_MODE__ SPI_MODE_0
#define __SPI_DEFAULT_MSB_FIRST__ _YES_
#define __SPI_SPI1_NUMBER_OF_CS__ 1
#define __SPI_SPI2_NUMBER_OF_CS__ 1
#define __SPI_SPI3_NUMBER_OF_CS__ 1
#define __SPI_SPI1_USE_DMA__ _YES_
#define __SPI_SPI2_USE_DMA__ _YES_
#define __SPI_SPI3_USE_DMA__ _YES_
#define __SPI_SPI1_CS0_PIN_NAME__ SD_CS
#define __SPI_SPI1_CS1_PIN_NAME__ NC_GPIOD_15
#define __SPI_SPI1_CS2_PIN_NAME__ NC_GPIOD_15
#define __SPI_SPI1_CS3_PIN_NAME__ NC_GPIOD_15
#define __SPI_SPI1_CS4_PIN_NAME__ NC_GPIOD_15
#define __SPI_SPI1_CS5_PIN_NAME__ NC_GPIOD_15
#define __SPI_SPI1_CS6_PIN_NAME__ NC_GPIOD_15
#define __SPI_SPI1_CS7_PIN_NAME__ NC_GPIOD_15
#define __SPI_SPI2_CS0_PIN_NAME__ NC_GPIOD_15
#define __SPI_SPI2_CS1_PIN_NAME__ NC_GPIOD_15
#define __SPI_SPI2_CS2_PIN_NAME__ NC_GPIOD_15
#define __SPI_SPI2_CS3_PIN_NAME__ NC_GPIOD_15
#define __SPI_SPI2_CS4_PIN_NAME__ NC_GPIOD_15
#define __SPI_SPI2_CS5_PIN_NAME__ NC_GPIOD_15
#define __SPI_SPI2_CS6_PIN_NAME__ NC_GPIOD_15
#define __SPI_SPI2_CS7_PIN_NAME__ NC_GPIOD_15
#define __SPI_SPI3_CS0_PIN_NAME__ SD_CS
#define __SPI_SPI3_CS1_PIN_NAME__ NC_GPIOD_15
#define __SPI_SPI3_CS2_PIN_NAME__ NC_GPIOD_15
#define __SPI_SPI3_CS3_PIN_NAME__ NC_GPIOD_15
#define __SPI_SPI3_CS4_PIN_NAME__ NC_GPIOD_15
#define __SPI_SPI3_CS5_PIN_NAME__ NC_GPIOD_15
#define __SPI_SPI3_CS6_PIN_NAME__ NC_GPIOD_15
#define __SPI_SPI3_CS7_PIN_NAME__ NC_GPIOD_15

#endif /* _SPI_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
