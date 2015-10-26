/*=========================================================================*//**
@file    i2c_ioctl.h

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

#ifndef _I2C_IOCTL_H_
#define _I2C_IOCTL_H_

/*==============================================================================
  Include files
==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct {
        u32_t frequency_Hz;     // SCL line frequency in Hz
        u16_t address;          // device address 8 or 10 bit
        u8_t  subaddrlen;       // 0-3: number of bytes of sub-address (EEPROM, RTC)
        bool  addr10bit;        // true: 10 bit mode address enabled
} i2c_config_t;

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 * @brief  Changes a device address TODO I2C SET ADDRESS to remove
 * @param  int*         a new address (7 or 10-bit)
 * @return Returns  0 on success.
 *         Returns -1 on error and appropriate error number is set.
 */
#define IOCTL_I2C__SET_ADDRESS          _IOW(I2C, 0, int*) // FIXME unnecessary

/** TODO I2C configure
 * @brief  Configure device
 * @param  i2c_config_t*        device configuration
 * @return Returns  0 on success.
 *         Returns -1 on error and appropriate error number is set.
 */
#define IOCTL_I2C__CONFIGURE            _IOW(I2C, 1, i2c_config_t*)

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _I2C_IOCTL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
