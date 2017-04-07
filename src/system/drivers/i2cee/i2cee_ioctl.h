/*=========================================================================*//**
@file    i2cee_ioctl.h

@author  Daniel Zorychta

@brief   I2C EEPROM driver.

@note    Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes FreeRTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


*//*==========================================================================*/

/**
@defgroup drv-i2cee I2CEE Driver

\section drv-i2cee-desc Description
Driver handles 24Cxx devices. The role of driver is handling memory paging and
programming times. By using this driver file systems can handle memory without
any page restrictions.

Device (drivers) connection table
| Grade | Device       | In                  | Out              |
| ----: | :----------- | :------------------ | :--------------- |
| 0     | 24cXX        | -                   | I2C interface    |
| 1     | I2C driver   | I2C interface       | /dev/i2c_ee      |
| 2     | I2CEE driver | /dev/i2c_ee         | /dev/sda         |
| 3     | File system  | /dev/sda            | /mnt             |


\section drv-i2cee-sup-arch Supported architectures
\li Any (noarch)

\section drv-i2cee-ddesc Details
\subsection drv-i2cee-ddesc-num Meaning of major and minor numbers
Only major number is used to identify device. Only minor number set to 0 is
accepted by driver.

\subsubsection drv-i2cee-ddesc-numres Numeration restrictions
The minor number should be set to 0.

\subsection drv-i2cee-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("I2CEE", 0, 0, "/dev/24c32");
@endcode
@code
driver_init("I2CEE", 1, 0, "/dev/24c64");
@endcode

\subsection drv-i2cee-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("I2CEE", 0, 0);
@endcode
@code
driver_release("I2CEE", 1, 0);
@endcode

\subsection drv-i2cee-ddesc-cfg Driver configuration
Driver can be configured only by ioctl() function. Configuration example:
\code
#include <sys/ioctl.h>

// ...

FILE *dev = fopen("/dev/24c32", "r+");
if (dev) {
        static const I2CEE_config_t cfg = {
                .i2c_path          = "/dev/i2c0",       // I2C path
                .memory_size       = 4096,              // 32kb = 4096B
                .page_size         = 32,                // 32B page
                .page_prog_time_ms = 10                 // 10ms write cycle
        }

        if (ioctl(dev, IOCTL_I2CEE__CONFIGURE, &cfg) == 0) {
                puts("Configuration success");
        } else {
                perror("ioctl()");
        }

        fclose(dev);
} else {
        perror("/dev/24c16");
}

// ...

\endcode

\subsection drv-i2cee-ddesc-write Data write
Data to the driver can be write in the same way as regular file. Driver automatically
handles page segmentation and programming time. Data can be not aligned to
memory pages.

\subsection drv-i2cee-ddesc-read Data read
Data from the driver can be read in the same way as regular file.

@{
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
        const char *i2c_path;           /*!< I2C device file*/
        u32_t memory_size;              /*!< Memory size in bytes*/
        u16_t page_size;                /*!< EEPROM page size in bytes*/
        u16_t page_prog_time_ms;        /*!< Time of programming single page in milliseconds*/
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
