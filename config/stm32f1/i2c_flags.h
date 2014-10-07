/*=========================================================================*//**
@file    i2c_flags.h

@author  Daniel Zorychta

@brief   This driver support I2C peripherals.

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

#ifndef _I2C_FLAGS_H_
#define _I2C_FLAGS_H_

/*
 * __I2Cx_DEV_x_ADDRESS: u16_t
 * __I2Cx_DEV_x_10BIT_ADDR: bool
 * __I2C1_DEV_0_SEND_SUB_ADDR: 0 - none; 1 - 1 Byte; 2 - 2 Bytes; 3 - 3 Bytes
 */

#define __I2C1_ENABLE__ _YES_
#define __I2C1_USE_DMA__ _YES_
#define __I2C1_SCL_FREQ__ 100000
#define __I2C1_IRQ_PRIO__ CONFIG_USER_IRQ_PRIORITY
#define __I2C1_NUMBER_OF_DEVICES__ 1
#define __I2C1_DEV_0_ADDRESS__ 0xD0
#define __I2C1_DEV_0_10BIT_ADDR__ _NO_
#define __I2C1_DEV_0_SEND_SUB_ADDR__ 1
#define __I2C1_DEV_1_ADDRESS__ 0xA0
#define __I2C1_DEV_1_10BIT_ADDR__ _NO_
#define __I2C1_DEV_1_SEND_SUB_ADDR__ 0
#define __I2C1_DEV_2_ADDRESS__ 0xA0
#define __I2C1_DEV_2_10BIT_ADDR__ _NO_
#define __I2C1_DEV_2_SEND_SUB_ADDR__ 0
#define __I2C1_DEV_3_ADDRESS__ 0xA0
#define __I2C1_DEV_3_10BIT_ADDR__ _NO_
#define __I2C1_DEV_3_SEND_SUB_ADDR__ 0
#define __I2C1_DEV_4_ADDRESS__ 0xA0
#define __I2C1_DEV_4_10BIT_ADDR__ _NO_
#define __I2C1_DEV_4_SEND_SUB_ADDR__ 0
#define __I2C1_DEV_5_ADDRESS__ 0xA0
#define __I2C1_DEV_5_10BIT_ADDR__ _NO_
#define __I2C1_DEV_5_SEND_SUB_ADDR__ 0
#define __I2C1_DEV_6_ADDRESS__ 0xA0
#define __I2C1_DEV_6_10BIT_ADDR__ _NO_
#define __I2C1_DEV_6_SEND_SUB_ADDR__ 0
#define __I2C1_DEV_7_ADDRESS__ 0xA0
#define __I2C1_DEV_7_10BIT_ADDR__ _NO_
#define __I2C1_DEV_7_SEND_SUB_ADDR__ 0

#define __I2C2_ENABLE__ _NO_
#define __I2C2_USE_DMA__ _YES_
#define __I2C2_SCL_FREQ__ 400000
#define __I2C2_IRQ_PRIO__ CONFIG_USER_IRQ_PRIORITY
#define __I2C2_NUMBER_OF_DEVICES__ 1
#define __I2C2_DEV_0_ADDRESS__ 0xA0
#define __I2C2_DEV_0_10BIT_ADDR__ _NO_
#define __I2C2_DEV_0_SEND_SUB_ADDR__ 2
#define __I2C2_DEV_1_ADDRESS__ 0xD0
#define __I2C2_DEV_1_10BIT_ADDR__ _NO_
#define __I2C2_DEV_1_SEND_SUB_ADDR__ 0
#define __I2C2_DEV_2_ADDRESS__ 0xD0
#define __I2C2_DEV_2_10BIT_ADDR__ _NO_
#define __I2C2_DEV_2_SEND_SUB_ADDR__ 0
#define __I2C2_DEV_3_ADDRESS__ 0xD0
#define __I2C2_DEV_3_10BIT_ADDR__ _NO_
#define __I2C2_DEV_3_SEND_SUB_ADDR__ 0
#define __I2C2_DEV_4_ADDRESS__ 0xD0
#define __I2C2_DEV_4_10BIT_ADDR__ _NO_
#define __I2C2_DEV_4_SEND_SUB_ADDR__ 0
#define __I2C2_DEV_5_ADDRESS__ 0xD0
#define __I2C2_DEV_5_10BIT_ADDR__ _NO_
#define __I2C2_DEV_5_SEND_SUB_ADDR__ 0
#define __I2C2_DEV_6_ADDRESS__ 0xD0
#define __I2C2_DEV_6_10BIT_ADDR__ _NO_
#define __I2C2_DEV_6_SEND_SUB_ADDR__ 0
#define __I2C2_DEV_7_ADDRESS__ 0xD0
#define __I2C2_DEV_7_10BIT_ADDR__ _NO_
#define __I2C2_DEV_7_SEND_SUB_ADDR__ 0

#endif /* _I2C_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
