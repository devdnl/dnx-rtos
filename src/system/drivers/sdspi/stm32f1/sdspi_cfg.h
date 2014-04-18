/*=========================================================================*//**
@file    sdspi_cfg.h

@author  Daniel Zorychta

@brief   This file support configuration for SD in SPI mode

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

#ifndef _SDSPI_CFG_H_
#define _SDSPI_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "stm32f1/stm32f10x.h"
#include "stm32f1/gpio_cfg.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/** card waiting timeout [ms] */
#define SDSPI_TIMEOUT                           __SDSPI_TIMEOUT__

/** supported SPI peripheral (1-3) */
#define SDSPI_PORT                              __SDSPI_SPI_PORT__

/** SPI peripheral divider (2-256) */
#define SDSPI_SPI_CLOCK_DIVIDER                 __SDSPI_SPI_CLK_DIV__

/** card select pin operation */
#define SDSPI_SD_CS_PIN                         __SDSPI_SD_CS_PIN__

/** enable (1) or disable (0) DMA support */
#define SDSPI_ENABLE_DMA                        __SDSPI_ENABLE_DMA__

/** DMA IRQ priority */
#define SDSPI_DMA_IRQ_PRIORITY                  __SDSPI_DMA_IRQ_PRIORITY__

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _SDSPI_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
