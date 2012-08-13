#ifndef I2C_CFG_H_
#define I2C_CFG_H_
/*=============================================================================================*//**
@file    i2c_cfg.h

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


/*==================================================================================================
                                     I2C Default Configuration
==================================================================================================*/
/** enable (1) or disable (0) I2C1 support */
#define I2C1_ENABLE                       (1)

/** enable (1) or disable (0) I2C2 support */
#define I2C2_ENABLE                       (0)

/** set peripheral clock frequency [MHz] */
#define I2C_PERIPHERAL_FREQ               (36)

/** set SCL frequency [Hz] */
#define I2C_DEFAULT_SCL_FREQ              (50000)


/*==================================================================================================
                                           Error handling
==================================================================================================*/
#if (I2C_PERIPHERAL_FREQ > 36 || I2C_PERIPHERAL_FREQ < 2)
#error "I2C_PERIPHERAL_FREQ should be between range of 2 - 36 MHz"
#endif

#ifdef __cplusplus
}
#endif

#endif /* I2C_CFG_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
