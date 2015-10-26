/*=========================================================================*//**
@file    spi_cfg.h

@author  Daniel Zorychta

@brief   SPI driver configuration

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

#ifndef _SPI_CFG_H_
#define _SPI_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "stm32f1/stm32f10x.h"
#include "stm32f1/gpio_cfg.h"
#include "../spi_ioctl.h"

/*==============================================================================
  Exported macros
==============================================================================*/
/*----------------------------------------------------------------------------*/
#define _SPI_CS_CFG(SPI, no, name)  enum _##SPI##_CS_##no {_##SPI##_CS##no##_PORT = (u32_t)GPIO_PIN_PORT(name), _##SPI##_CS##no##_PIN_BM = GPIO_PIN_MASK(name)}
/*----------------------------------------------------------------------------*/

/*
 * enable (1) or disable (0) SPI peripheral
 */
#define _SPI1_ENABLE                            __SPI_SPI1_ENABLE__
#define _SPI2_ENABLE                            __SPI_SPI2_ENABLE__
#define _SPI3_ENABLE                            __SPI_SPI3_ENABLE__

/*
 * IRQ priority
 */
#define _SPI1_IRQ_PRIORITY                      __SPI_SPI1_PRIORITY__
#define _SPI2_IRQ_PRIORITY                      __SPI_SPI2_PRIORITY__
#define _SPI3_IRQ_PRIORITY                      __SPI_SPI3_PRIORITY__

/*
 *  SPI default configuration (for all devices)
 */
#define _SPI_DEFAULT_CFG_DUMMY_BYTE             __SPI_DEFAULT_DUMMY_BYTE__
#define _SPI_DEFAULT_CFG_CLK_DIVIDER            __SPI_DEFAULT_CLK_DIV__
#define _SPI_DEFAULT_CFG_MODE                   __SPI_DEFAULT_MODE__
#define _SPI_DEFAULT_CFG_MSB_FIRST              __SPI_DEFAULT_MSB_FIRST__

/*
 * NUMBER OF DEVICES CONNECTED TO THE SPI PERIPHERAL
 */
#define _SPI1_NUMBER_OF_SLAVES                  __SPI_SPI1_NUMBER_OF_CS__
#define _SPI2_NUMBER_OF_SLAVES                  __SPI_SPI2_NUMBER_OF_CS__
#define _SPI3_NUMBER_OF_SLAVES                  __SPI_SPI3_NUMBER_OF_CS__

/*
 * DMA enable (1) or disable (0)
 */
#define _SPI1_USE_DMA                           __SPI_SPI1_USE_DMA__
#define _SPI2_USE_DMA                           __SPI_SPI2_USE_DMA__
#define _SPI3_USE_DMA                           __SPI_SPI3_USE_DMA__

/*
 *  CS configurations for SPI1 (_SPI1)
 */
_SPI_CS_CFG(SPI1, 0, __SPI_SPI1_CS0_PIN_NAME__);
_SPI_CS_CFG(SPI1, 1, __SPI_SPI1_CS1_PIN_NAME__);
_SPI_CS_CFG(SPI1, 2, __SPI_SPI1_CS2_PIN_NAME__);
_SPI_CS_CFG(SPI1, 3, __SPI_SPI1_CS3_PIN_NAME__);
_SPI_CS_CFG(SPI1, 4, __SPI_SPI1_CS4_PIN_NAME__);
_SPI_CS_CFG(SPI1, 5, __SPI_SPI1_CS5_PIN_NAME__);
_SPI_CS_CFG(SPI1, 6, __SPI_SPI1_CS6_PIN_NAME__);
_SPI_CS_CFG(SPI1, 7, __SPI_SPI1_CS7_PIN_NAME__);

/*
 * CS configurations for SPI2 (_SPI2)
 */
_SPI_CS_CFG(SPI2, 0, __SPI_SPI2_CS0_PIN_NAME__);
_SPI_CS_CFG(SPI2, 1, __SPI_SPI2_CS1_PIN_NAME__);
_SPI_CS_CFG(SPI2, 2, __SPI_SPI2_CS2_PIN_NAME__);
_SPI_CS_CFG(SPI2, 3, __SPI_SPI2_CS3_PIN_NAME__);
_SPI_CS_CFG(SPI2, 4, __SPI_SPI2_CS4_PIN_NAME__);
_SPI_CS_CFG(SPI2, 5, __SPI_SPI2_CS5_PIN_NAME__);
_SPI_CS_CFG(SPI2, 6, __SPI_SPI2_CS6_PIN_NAME__);
_SPI_CS_CFG(SPI2, 7, __SPI_SPI2_CS7_PIN_NAME__);

/*
 * CS configurations for SPI3 (_SPI3)
 */
_SPI_CS_CFG(SPI3, 0, __SPI_SPI3_CS0_PIN_NAME__);
_SPI_CS_CFG(SPI3, 1, __SPI_SPI3_CS1_PIN_NAME__);
_SPI_CS_CFG(SPI3, 2, __SPI_SPI3_CS2_PIN_NAME__);
_SPI_CS_CFG(SPI3, 3, __SPI_SPI3_CS3_PIN_NAME__);
_SPI_CS_CFG(SPI3, 4, __SPI_SPI3_CS4_PIN_NAME__);
_SPI_CS_CFG(SPI3, 5, __SPI_SPI3_CS5_PIN_NAME__);
_SPI_CS_CFG(SPI3, 6, __SPI_SPI3_CS6_PIN_NAME__);
_SPI_CS_CFG(SPI3, 7, __SPI_SPI3_CS7_PIN_NAME__);


/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _SPI_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
