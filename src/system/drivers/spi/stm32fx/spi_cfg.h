/*=========================================================================*//**
@file    spi_cfg.h

@author  Daniel Zorychta

@brief   SPI driver configuration

@note    Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes dnx RTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


*//*==========================================================================*/

#ifndef _SPI_CFG_H_
#define _SPI_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/

/*==============================================================================
  Exported macros
==============================================================================*/
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
#define _SPI4_USE_DMA                           __SPI_SPI4_USE_DMA__
#define _SPI5_USE_DMA                           __SPI_SPI5_USE_DMA__
#define _SPI6_USE_DMA                           __SPI_SPI6_USE_DMA__

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
