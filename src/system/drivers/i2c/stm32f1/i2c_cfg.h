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
#define _I2C_DEVICE(periph, dev, addr, addr10bit, send_subaddr) \
        enum periph##_DEV_##dev {periph##_DEV_##dev##_ADDRESS = addr,\
                  periph##_DEV_##dev##_10BIT_ADDR_MODE = addr10bit,\
                  periph##_DEV_##dev##_SEND_SUB_ADDRESS = send_subaddr}

#define _I2C1_ENABLE            __I2C1_ENABLE__
#define _I2C1_FREQUENCY         __I2C1_SCL_FREQ__
#define _I2C1_USE_DMA           __I2C1_USE_DMA__
#define _I2C1_IRQ_PRIO          __I2C1_IRQ_PRIO__
#define _I2C1_NUMBER_OF_DEVICES __I2C1_NUMBER_OF_DEVICES__
_I2C_DEVICE(_I2C1, 0, __I2C1_DEV_0_ADDRESS, __I2C1_DEV_0_10BIT_ADDR, __I2C1_DEV_0_SEND_SUB_ADDR);
_I2C_DEVICE(_I2C1, 1, __I2C1_DEV_1_ADDRESS, __I2C1_DEV_1_10BIT_ADDR, __I2C1_DEV_1_SEND_SUB_ADDR);
_I2C_DEVICE(_I2C1, 2, __I2C1_DEV_2_ADDRESS, __I2C1_DEV_2_10BIT_ADDR, __I2C1_DEV_2_SEND_SUB_ADDR);
_I2C_DEVICE(_I2C1, 3, __I2C1_DEV_3_ADDRESS, __I2C1_DEV_3_10BIT_ADDR, __I2C1_DEV_3_SEND_SUB_ADDR);
_I2C_DEVICE(_I2C1, 4, __I2C1_DEV_4_ADDRESS, __I2C1_DEV_4_10BIT_ADDR, __I2C1_DEV_4_SEND_SUB_ADDR);
_I2C_DEVICE(_I2C1, 5, __I2C1_DEV_5_ADDRESS, __I2C1_DEV_5_10BIT_ADDR, __I2C1_DEV_5_SEND_SUB_ADDR);
_I2C_DEVICE(_I2C1, 6, __I2C1_DEV_6_ADDRESS, __I2C1_DEV_6_10BIT_ADDR, __I2C1_DEV_6_SEND_SUB_ADDR);
_I2C_DEVICE(_I2C1, 7, __I2C1_DEV_7_ADDRESS, __I2C1_DEV_7_10BIT_ADDR, __I2C1_DEV_7_SEND_SUB_ADDR);

#define _I2C2_ENABLE            __I2C2_ENABLE__
#define _I2C2_FREQUENCY         __I2C2_SCL_FREQ__
#define _I2C2_USE_DMA           __I2C2_USE_DMA__
#define _I2C2_IRQ_PRIO          __I2C2_IRQ_PRIO__
#define _I2C2_NUMBER_OF_DEVICES __I2C2_NUMBER_OF_DEVICES__
_I2C_DEVICE(_I2C2, 0, __I2C2_DEV_0_ADDRESS, __I2C2_DEV_0_10BIT_ADDR, __I2C2_DEV_0_SEND_SUB_ADDR);
_I2C_DEVICE(_I2C2, 1, __I2C2_DEV_1_ADDRESS, __I2C2_DEV_1_10BIT_ADDR, __I2C2_DEV_1_SEND_SUB_ADDR);
_I2C_DEVICE(_I2C2, 2, __I2C2_DEV_2_ADDRESS, __I2C2_DEV_2_10BIT_ADDR, __I2C2_DEV_2_SEND_SUB_ADDR);
_I2C_DEVICE(_I2C2, 3, __I2C2_DEV_3_ADDRESS, __I2C2_DEV_3_10BIT_ADDR, __I2C2_DEV_3_SEND_SUB_ADDR);
_I2C_DEVICE(_I2C2, 4, __I2C2_DEV_4_ADDRESS, __I2C2_DEV_4_10BIT_ADDR, __I2C2_DEV_4_SEND_SUB_ADDR);
_I2C_DEVICE(_I2C2, 5, __I2C2_DEV_5_ADDRESS, __I2C2_DEV_5_10BIT_ADDR, __I2C2_DEV_5_SEND_SUB_ADDR);
_I2C_DEVICE(_I2C2, 6, __I2C2_DEV_6_ADDRESS, __I2C2_DEV_6_10BIT_ADDR, __I2C2_DEV_6_SEND_SUB_ADDR);
_I2C_DEVICE(_I2C2, 7, __I2C2_DEV_7_ADDRESS, __I2C2_DEV_7_10BIT_ADDR, __I2C2_DEV_7_SEND_SUB_ADDR);


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
