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
#include "system.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/
/** port names */
enum I2C_DEV_NUMBER_enum
{
      #ifdef RCC_APB1ENR_I2C1EN
      #if (I2C1_ENABLE > 0)
            I2C_DEV_1,
      #endif
      #endif

      #ifdef RCC_APB1ENR_I2C2EN
      #if (I2C2_ENABLE > 0)
            I2C_DEV_2,
      #endif
      #endif

      I2C_DEV_LAST
};


/** statuses */
enum I2C_STATUS_enum
{
      I2C_STATUS_PORTNOTEXIST       = -1,
      I2C_STATUS_PORTLOCKED         = -2,
      I2C_STATUS_NOFREEMEM          = -3,
      I2C_STATUS_BADRQ              = -4,
      I2C_STATUS_BADARG             = -5,
      I2C_STATUS_TIMEOUT            = -6,
      I2C_STATUS_OVERRUN            = -7,
      I2C_STATUS_ACK_FAILURE        = -8,
      I2C_STATUS_ARB_LOST           = -9,
      I2C_STATUS_BUS_ERROR          = -10,
      I2C_STATUS_ERROR              = -11,
};


/** IO request for I2C driver */
enum I2C_IORq_enum
{
      I2C_IORQ_SETSLAVEADDR,                                /* [in]  u8_t slave address */
      I2C_IORQ_GETSLAVEADDR,                                /* [out] u8_t slave address */
      I2C_IORQ_SETSCLFREQ,                                  /* [in]  u32_t SCL frequency [Hz] */
};


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern stdRet_t I2C_Init(void);


extern stdRet_t I2C_Open(dev_t dev);


extern stdRet_t I2C_Close(dev_t dev);


extern stdRet_t I2C_Write(dev_t dev, void *src, size_t size, size_t seek);


extern stdRet_t I2C_Read(dev_t dev , void *dst, size_t size, size_t seek);


extern stdRet_t I2C_IOCtl(dev_t dev, IORq_t ioRQ, void *data);


#ifdef __cplusplus
}
#endif

#endif /* I2C_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
