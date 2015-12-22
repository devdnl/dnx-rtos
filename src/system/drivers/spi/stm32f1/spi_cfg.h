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
/*
 * IRQ priority
 */
#define _SPI1_IRQ_PRIORITY                      __SPI_SPI1_IRQ_PRIORITY__
#define _SPI2_IRQ_PRIORITY                      __SPI_SPI2_IRQ_PRIORITY__
#define _SPI3_IRQ_PRIORITY                      __SPI_SPI3_IRQ_PRIORITY__

/*
 *  SPI default configuration (for all devices)
 */
#define _SPI_DEFAULT_CFG_FLUSH_BYTE             __SPI_DEFAULT_FLUSH_BYTE__
#define _SPI_DEFAULT_CFG_CLK_DIVIDER            __SPI_DEFAULT_CLK_DIV__
#define _SPI_DEFAULT_CFG_MODE                   __SPI_DEFAULT_MODE__
#define _SPI_DEFAULT_CFG_MSB_FIRST              __SPI_DEFAULT_MSB_FIRST__

/*
 * DMA enable (1) or disable (0)
 */
#define _SPI1_USE_DMA                           __SPI_SPI1_USE_DMA__
#define _SPI2_USE_DMA                           __SPI_SPI2_USE_DMA__
#define _SPI3_USE_DMA                           __SPI_SPI3_USE_DMA__

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
