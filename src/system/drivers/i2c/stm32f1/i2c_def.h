/*=========================================================================*//**
@file    i2c_def.h

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

#ifndef _I2C_DEF_H_
#define _I2C_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "stm32f1/i2c_ioctl.h"
#include "stm32f1/i2c_cfg.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
enum _I2C_major {
#if defined(I2C1) && (_I2C1_ENABLE > 0)
        _I2C1,
#endif
#if defined(I2C2) && (_I2C2_ENABLE > 0)
        _I2C2,
#endif
        _I2C_NUMBER_OF_PERIPHERALS
};

enum _I2C_minor {
        _I2C_DEV_0,
        _I2C_DEV_1,
        _I2C_DEV_2,
        _I2C_DEV_3,
        _I2C_DEV_4,
        _I2C_DEV_5,
        _I2C_DEV_6,
        _I2C_DEV_7
};

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

#endif /* _I2C_DEF_H_ */
/*==============================================================================
  End of file
==============================================================================*/
