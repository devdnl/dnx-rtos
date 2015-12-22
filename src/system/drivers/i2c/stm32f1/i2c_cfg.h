/*=========================================================================*//**
@file    i2c_cfg.h

@author  Daniel Zorychta

@brief   This driver support I2C peripherals.

@note    Copyright (C) 2014  Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _I2C_CFG_H_
#define _I2C_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "stm32f1/stm32f10x.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
#define _I2C1_USE_DMA           __I2C1_USE_DMA__
#define _I2C1_IRQ_PRIO          __I2C1_IRQ_PRIO__
#define _I2C1_FREQUENCY         __I2C1_SCL_FREQ__

#define _I2C2_USE_DMA           __I2C2_USE_DMA__
#define _I2C2_IRQ_PRIO          __I2C2_IRQ_PRIO__
#define _I2C2_FREQUENCY         __I2C2_SCL_FREQ__

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

#endif /* _I2C_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
