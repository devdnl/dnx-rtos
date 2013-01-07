#ifndef I2C_H_
#define I2C_H_
/*=============================================================================================*//**
@file    i2c.h

@author  Daniel Zorychta

@brief   This driver support I2C

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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


*//*==============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "i2c_cfg.h"
#include "i2c_def.h"
#include "stm32f10x.h"
#include "sysdrv.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/
#define I2C_PART_NONE         0

/** port names */
enum I2C_DEV_NUMBER_enum
{
      #ifdef RCC_APB1ENR_I2C1EN
      #if (I2C1_ENABLE > 0)
            I2C_DEV_1,
      #define I2C_DEV_1_DEFINED
      #endif
      #endif

      #ifdef RCC_APB1ENR_I2C2EN
      #if (I2C2_ENABLE > 0)
            I2C_DEV_2,
      #define I2C_DEV_2_DEFINED
      #endif
      #endif

      I2C_DEV_LAST
};


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
DRIVER_INTERFACE_CLASS(I2C);


#ifdef __cplusplus
}
#endif

#endif /* I2C_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
