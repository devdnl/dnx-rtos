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

/**
 * @defgroup drv-i2c I2C Driver
 *
 * \section drv-i2c-desc Description
 * Driver handles I2C peripheral.
 *
 * \section drv-i2c-sup-arch Supported architectures
 * \li STM32F10x
 *
 * @{
 */

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
/**
 * Type defines possible modes of sub-addressing sequence (used e.g. in EEPROM).
 */
typedef enum {
        I2C_SUB_ADDR_MODE__DISABLED = 0,        //!< Sub-addressing disabled.
        I2C_SUB_ADDR_MODE__1_BYTE   = 1,        //!< Sub-address is 1 byte long.
        I2C_SUB_ADDR_MODE__2_BYTES  = 2,        //!< Sub-address is 2 byte long.
        I2C_SUB_ADDR_MODE__3_BYTES  = 3         //!< Sub-address is 3 byte long.
} I2C_sub_addr_mode_t;

/**
 * Type represents I2C peripheral configuration.
 *
 */
typedef struct {
        u16_t               address;            //!< Device address 8 or 10 bit.
        I2C_sub_addr_mode_t sub_addr_mode;      //!< Number of bytes of sub-address (EEPROM, RTC).
        bool                addr_10bit;         //!< @b true: 10 bit addressing enabled.
} I2C_config_t;

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 * @brief  Configure device
 * @param  [WR] @ref I2C_config_t*        device configuration
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_I2C__CONFIGURE            _IOW(I2C, 0, const I2C_config_t*)

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
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
