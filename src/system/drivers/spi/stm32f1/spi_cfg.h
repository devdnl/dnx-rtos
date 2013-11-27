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

/*==============================================================================
  Exported macros
==============================================================================*/
/*----------------------------------------------------------------------------*/
#define _SPI1_CS_CFG(_no, _port, _pin)          enum _SPI1_CS##_no {_SPI1_CS##_no##_PORT = _port, _SPI1_CS##_no##_PIN = _pin, _SPI1_SLAVE_##_no = _no}
#define _SPI2_CS_CFG(_no, _port, _pin)          enum _SPI2_CS##_no {_SPI2_CS##_no##_PORT = _port, _SPI2_CS##_no##_PIN = _pin, _SPI2_SLAVE_##_no = _no}
#define _SPI3_CS_CFG(_no, _port, _pin)          enum _SPI3_CS##_no {_SPI3_CS##_no##_PORT = _port, _SPI3_CS##_no##_PIN = _pin, _SPI3_SLAVE_##_no = _no}
/*----------------------------------------------------------------------------*/

/* user SPI1 enable (1) or disable (0) */
#define _SPI1_ENABLE                            1

/* user SPI2 enable (1) or disable (0) */
#define _SPI2_ENABLE                            1

/* user SPI3 enable (1) or disable (0) */
#define _SPI3_ENABLE                            1

/* IRQ priority */
#define _SPI_IRQ_PRIORITY                       CONFIG_USER_IRQ_PRIORITY

/* SPI default configuration (for all devices) */
#define _SPI_DEFAULT_CFG_DUMMY_BYTE             0xFF
#define _SPI_DEFAULT_CFG_CLK_DIVIDER            SPI_CLK_DIV_2
#define _SPI_DEFAULT_CFG_MODE                   SPI_MODE_0
#define _SPI_DEFAULT_CFG_MASTER_MODE            true
#define _SPI_DEFAULT_CFG_MSB_FIRST              true

/* CS configurations for SPI1 */
#if defined(RCC_APB2ENR_SPI1EN) && (_SPI1_ENABLE > 0)
#define _SPI1_NUMBER_OF_SLAVES                  8
_SPI1_CS_CFG(0, GPIOA, 0);
_SPI1_CS_CFG(1, GPIOA, 0);
_SPI1_CS_CFG(2, GPIOA, 0);
_SPI1_CS_CFG(3, GPIOA, 0);
_SPI1_CS_CFG(4, GPIOA, 0);
_SPI1_CS_CFG(5, GPIOA, 0);
_SPI1_CS_CFG(6, GPIOA, 0);
_SPI1_CS_CFG(7, GPIOA, 0);
#endif

/* CS configurations for SPI2 */
#if defined(RCC_APB1ENR_SPI2EN) && (_SPI2_ENABLE > 0)
#define _SPI2_NUMBER_OF_SLAVES                  8
_SPI2_CS_CFG(0, GPIOA, 0);
_SPI2_CS_CFG(1, GPIOA, 0);
_SPI2_CS_CFG(2, GPIOA, 0);
_SPI2_CS_CFG(3, GPIOA, 0);
_SPI2_CS_CFG(4, GPIOA, 0);
_SPI2_CS_CFG(5, GPIOA, 0);
_SPI2_CS_CFG(6, GPIOA, 0);
_SPI2_CS_CFG(7, GPIOA, 0);
#endif

/* CS configurations for SPI3 */
#if defined(RCC_APB1ENR_SPI3EN) && (_SPI3_ENABLE > 0)
#define _SPI3_NUMBER_OF_SLAVES                  8
_SPI3_CS_CFG(0, GPIOA, 0);
_SPI3_CS_CFG(1, GPIOA, 0);
_SPI3_CS_CFG(2, GPIOA, 0);
_SPI3_CS_CFG(3, GPIOA, 0);
_SPI3_CS_CFG(4, GPIOA, 0);
_SPI3_CS_CFG(5, GPIOA, 0);
_SPI3_CS_CFG(6, GPIOA, 0);
_SPI3_CS_CFG(7, GPIOA, 0);
#endif

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
