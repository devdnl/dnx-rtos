#ifndef I2C_DEF_H_
#define I2C_DEF_H_
/*=============================================================================================*//**
@file    i2c_def.h

@author  Daniel Zorychta

@brief   This driver support I2C definitions

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


/*==================================================================================================
                                           I2C Definitions
==================================================================================================*/
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
      I2C_IORQ_GETSTATUS,                                   /* [out] stdRet_t status */
};


/*==================================================================================================
                                           Error handling
==================================================================================================*/


#ifdef __cplusplus
}
#endif

#endif /* I2C_DEF_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
