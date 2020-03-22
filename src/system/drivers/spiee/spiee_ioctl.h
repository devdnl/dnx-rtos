/*==============================================================================
File    spiee_ioctl.h

Author  Daniel Zorychta

Brief   SPI EEPROM

        Copyright (C) 2019 Daniel Zorychta <>

        This program is free software; you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation and modified by the dnx RTOS exception.

        NOTE: The modification  to the GPL is  included to allow you to
              distribute a combined work that includes dnx RTOS without
              being obliged to provide the source  code for proprietary
              components outside of the dnx RTOS.

        The dnx RTOS  is  distributed  in the hope  that  it will be useful,
        but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
        MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
        GNU General Public License for more details.

        Full license text is available on the following file: doc/license.txt.

==============================================================================*/

/**
@defgroup drv-spiee SPIEE Driver

\section drv-spiee-desc Description
Driver handles 25AAxx devices. The role of driver is handling memory paging and
programming times. By using this driver file systems can handle memory without
any page restrictions.

Device (drivers) connection table
| Grade | Device       | In                  | Out              |
| ----: | :----------- | :------------------ | :--------------- |
| 0     | 25AAxx       | -                   | SPI interface    |
| 1     | SPI driver   | SPI interface       | /dev/spi_ee      |
| 2     | SPIEE driver | /dev/spi_ee         | /dev/sda         |
| 3     | File system  | /dev/sda            | /mnt             |

\section drv-spiee-sup-arch Supported architectures
\li Any (noarch)

\section drv-spiee-ddesc Details
\subsection drv-spiee-ddesc-num Meaning of major and minor numbers
Only major number is used to identify device. Only minor number set to 0 is
accepted by driver.

\subsubsection drv-spiee-ddesc-numres Numeration restrictions
The minor number should be set to 0.

\subsection drv-spiee-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("SPIEE", 0, 0, "/dev/eeprom0");
@endcode
@code
driver_init("SPIEE", 1, 0, "/dev/eepprom1");
@endcode

\subsection drv-spiee-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("SPIEE", 0, 0);
@endcode
@code
driver_release("SPIEE", 1, 0);
@endcode

\subsection drv-spiee-ddesc-cfg Driver configuration
Driver can be configured only by ioctl() function. Configuration example:
\code
#include <sys/ioctl.h>

// ...

FILE *dev = fopen("/dev/eeprom", "r+");
if (dev) {
        static const SPIEE_config_t cfg = {
                .spi_path          = "/dev/spi-ee",     // SPI path
                .memory_size       = 131072,            // 1024Kib = 128KiB = 131072B
                .page_size         = 256,               // 256B page
        }

        if (ioctl(fileno(dev), IOCTL_SPIEE__CONFIGURE, &cfg) == 0) {
                puts("Configuration success");
        } else {
                perror("ioctl()");
        }

        fclose(dev);
} else {
        perror("/dev/eeprom");
}

// ...

\endcode

\subsection drv-spiee-ddesc-write Data write
Data to the driver can be write in the same way as regular file. Driver automatically
handles page segmentation and programming time. Data can be not aligned to
memory pages.

\subsection drv-spiee-ddesc-read Data read
Data from the driver can be read in the same way as regular file.

@{
*/

#ifndef _SPIEE_IOCTL_H_
#define _SPIEE_IOCTL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/ioctl_macros.h"
#include "drivers/class/device/ioctl.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 * @brief  SPI EEPROM driver configuration.
 * @param  [WR] @ref SPIEE_config_t*            driver configuration.
 * @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_SPIEE__CONFIGURE          _IOW(SPIEE, 0x00, SPIEE_config_t*)

/**
 * @brief  SPI EEPROM driver configuration by using string.
 * @param  [WR] const char*             configuration string
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_SPIEE__CONFIGURE_STR      IOCTL_DEVICE__CONFIGURE_STR

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Type defines possible number of bytes of address.
 */
typedef enum {
        SPIEE_ADDR__1_BYTE   = 1,        //!< Address is 1 byte long.
        SPIEE_ADDR__2_BYTES  = 2,        //!< Address is 2 byte long.
        SPIEE_ADDR__3_BYTES  = 3,        //!< Address is 3 byte long.
        SPIEE_ADDR__4_BYTES  = 4,        //!< Address is 4 byte long.
} SPIEE_addr_t;

/**
 * Type represent driver configuration.
 */
typedef struct {
        const char *spi_path;           /*!< SPI device file*/
        u32_t memory_size;              /*!< Memory size in bytes*/
        u16_t page_size;                /*!< EEPROM page size in bytes*/
        SPIEE_addr_t address_size;      /*!< Number of address bytes (1..4) */
} SPIEE_config_t;

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

#endif /* _SPIEE_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
