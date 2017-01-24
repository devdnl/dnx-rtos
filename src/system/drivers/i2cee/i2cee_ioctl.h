/*=========================================================================*//**
@file    i2cee_ioctl.h

@author  Daniel Zorychta

@brief   I2C EEPROM driver.

@note    Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

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
 * @defgroup drv-i2cee I2C EEPROM driver for 24Cxx devices.
 *
 * \section drv-i2cee-desc Description
 * Driver handles 24Cxx devices.
 *
 * \section drv-i2c-sup-arch Supported architectures
 * \li Any
 *
 * @todo Details
 *
 *
 * @{
 */

#ifndef _I2CEE_IOCTL_H_
#define _I2CEE_IOCTL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/ioctl_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 * @brief  I2C EEPROM driver configuration.
 * @param  [WR] @ref I2CEE_config_t*            driver configuration.
 * @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_I2CEE__CONFIGURE          _IOW(I2CEE, 0x00, I2CEE_config_t*)

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Type represent driver configuration.
 */
typedef struct {
        const char *i2c_path;           //!< I2C device file.
        u32_t memory_size;              //!< Memory size in bytes.
        u16_t page_size;                //!< EEPROM page size.
        u16_t page_prog_time_ms;        //!< Time of programming single page in milliseconds.
} I2CEE_config_t;

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

#endif /* _I2CEE_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
