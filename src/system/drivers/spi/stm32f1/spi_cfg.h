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
#include "stm32f1/spi_ioctl.h"

/*==============================================================================
  Exported macros
==============================================================================*/
/*----------------------------------------------------------------------------*/
#define _SPI1_CS_CFG(no, name)  enum _SPI1_CS_##no {_SPI1_CS##no##_PORT = (u32_t)GPIO_PIN_PORT(name), _SPI1_CS##no##_PIN_BM = GPIO_PIN_MASK(name)}
#define _SPI2_CS_CFG(no, name)  enum _SPI2_CS_##no {_SPI2_CS##no##_PORT = (u32_t)GPIO_PIN_PORT(name), _SPI2_CS##no##_PIN_BM = GPIO_PIN_MASK(name)}
#define _SPI3_CS_CFG(no, name)  enum _SPI3_CS_##no {_SPI3_CS##no##_PORT = (u32_t)GPIO_PIN_PORT(name), _SPI3_CS##no##_PIN_BM = GPIO_PIN_MASK(name)}
/*----------------------------------------------------------------------------*/

/*
 * enable (1) or disable (0) SPI1 peripheral
 */
#define _SPI1_ENABLE                            __SPI_SPI1_ENABLE__

/*
 * enable (1) or disable (0) SPI2 peripheral
 */
#define _SPI2_ENABLE                            __SPI_SPI2_ENABLE__

/*
 * enable (1) or disable (0) SPI3 peripheral
 */
#define _SPI3_ENABLE                            __SPI_SPI3_ENABLE__

/*
 * IRQ priority for SPI1
 */
#define _SPI1_IRQ_PRIORITY                      __SPI_SPI1_PRIORITY__

/*
 * IRQ priority for SPI2
 */
#define _SPI2_IRQ_PRIORITY                      __SPI_SPI2_PRIORITY__

/*
 * IRQ priority for SPI3
 */
#define _SPI3_IRQ_PRIORITY                      __SPI_SPI3_PRIORITY__

/*
 *  SPI default configuration (for all devices)
 */
#define _SPI_DEFAULT_CFG_DUMMY_BYTE             __SPI_DEFAULT_DUMMY_BYTE__
#define _SPI_DEFAULT_CFG_CLK_DIVIDER            __SPI_DEFAULT_CLK_DIV__
#define _SPI_DEFAULT_CFG_MODE                   __SPI_DEFAULT_MODE__
#define _SPI_DEFAULT_CFG_MSB_FIRST              __SPI_DEFAULT_MSB_FIRST__

/*
 *  CS configurations for SPI1 (_SPI1)
 */
#if defined(RCC_APB2ENR_SPI1EN) && (_SPI1_ENABLE > 0)
#define _SPI1_NUMBER_OF_SLAVES                  __SPI_SPI1_NUMBER_OF_CS__
#if _SPI1_NUMBER_OF_SLAVES >= 1
_SPI1_CS_CFG(0, __SPI_SPI1_CS0_PIN_NAME__);
#endif
#if _SPI1_NUMBER_OF_SLAVES >= 2
_SPI1_CS_CFG(1, __SPI_SPI1_CS1_PIN_NAME__);
#endif
#if _SPI1_NUMBER_OF_SLAVES >= 3
_SPI1_CS_CFG(2, __SPI_SPI1_CS2_PIN_NAME__);
#endif
#if _SPI1_NUMBER_OF_SLAVES >= 4
_SPI1_CS_CFG(3, __SPI_SPI1_CS3_PIN_NAME__);
#endif
#if _SPI1_NUMBER_OF_SLAVES >= 5
_SPI1_CS_CFG(4, __SPI_SPI1_CS4_PIN_NAME__);
#endif
#if _SPI1_NUMBER_OF_SLAVES >= 6
_SPI1_CS_CFG(5, __SPI_SPI1_CS5_PIN_NAME__);
#endif
#if _SPI1_NUMBER_OF_SLAVES >= 7
_SPI1_CS_CFG(6, __SPI_SPI1_CS6_PIN_NAME__);
#endif
#if _SPI1_NUMBER_OF_SLAVES >= 8
_SPI1_CS_CFG(7, __SPI_SPI1_CS7_PIN_NAME__);
#endif
#endif

/*
 * CS configurations for SPI2 (_SPI2)
 */
#if defined(RCC_APB1ENR_SPI2EN) && (_SPI2_ENABLE > 0)
#define _SPI2_NUMBER_OF_SLAVES                  __SPI_SPI2_NUMBER_OF_CS__
#if _SPI2_NUMBER_OF_SLAVES >= 1
_SPI2_CS_CFG(0, __SPI_SPI2_CS0_PIN_NAME__);
#endif
#if _SPI2_NUMBER_OF_SLAVES >= 2
_SPI2_CS_CFG(1, __SPI_SPI2_CS1_PIN_NAME__);
#endif
#if _SPI2_NUMBER_OF_SLAVES >= 3
_SPI2_CS_CFG(2, __SPI_SPI2_CS2_PIN_NAME__);
#endif
#if _SPI2_NUMBER_OF_SLAVES >= 4
_SPI2_CS_CFG(3, __SPI_SPI2_CS3_PIN_NAME__);
#endif
#if _SPI2_NUMBER_OF_SLAVES >= 5
_SPI2_CS_CFG(4, __SPI_SPI2_CS4_PIN_NAME__);
#endif
#if _SPI2_NUMBER_OF_SLAVES >= 6
_SPI2_CS_CFG(5, __SPI_SPI2_CS5_PIN_NAME__);
#endif
#if _SPI2_NUMBER_OF_SLAVES >= 7
_SPI2_CS_CFG(6, __SPI_SPI2_CS6_PIN_NAME__);
#endif
#if _SPI2_NUMBER_OF_SLAVES >= 8
_SPI2_CS_CFG(7, __SPI_SPI2_CS7_PIN_NAME__);
#endif
#endif

/*
 * CS configurations for SPI3 (_SPI3)
 */
#if defined(RCC_APB1ENR_SPI3EN) && (_SPI3_ENABLE > 0)
#define _SPI3_NUMBER_OF_SLAVES                  __SPI_SPI3_NUMBER_OF_CS__
#if _SPI3_NUMBER_OF_SLAVES >= 1
_SPI3_CS_CFG(0, __SPI_SPI3_CS0_PIN_NAME__);
#endif
#if _SPI3_NUMBER_OF_SLAVES >= 2
_SPI3_CS_CFG(1, __SPI_SPI3_CS1_PIN_NAME__);
#endif
#if _SPI3_NUMBER_OF_SLAVES >= 3
_SPI3_CS_CFG(2, __SPI_SPI3_CS2_PIN_NAME__);
#endif
#if _SPI3_NUMBER_OF_SLAVES >= 4
_SPI3_CS_CFG(3, __SPI_SPI3_CS3_PIN_NAME__);
#endif
#if _SPI3_NUMBER_OF_SLAVES >= 5
_SPI3_CS_CFG(4, __SPI_SPI3_CS4_PIN_NAME__);
#endif
#if _SPI3_NUMBER_OF_SLAVES >= 6
_SPI3_CS_CFG(5, __SPI_SPI3_CS5_PIN_NAME__);
#endif
#if _SPI3_NUMBER_OF_SLAVES >= 7
_SPI3_CS_CFG(6, __SPI_SPI3_CS6_PIN_NAME__);
#endif
#if _SPI3_NUMBER_OF_SLAVES >= 8
_SPI3_CS_CFG(7, __SPI_SPI3_CS7_PIN_NAME__);
#endif
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
