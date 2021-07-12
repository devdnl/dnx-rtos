/*=========================================================================*//**
@file    sdspi_ioctl.h

@author  Daniel Zorychta

@brief   This file support ioctl request codes.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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


*//*==========================================================================*/

/**
@defgroup drv-sdspi SDSPI Driver (SPI SD Card mode driver)

\section drv-sdspi-desc Description
Driver handles SD Card by using SPI interface (mode). The SPI interface is handled
by file thereby can be used by any microcontroller architecture. Driver handle
all SD card communication; writing or reading the card, from user point of
view, looks like write or read a regular file. Data is automatically divided to
blocks. Entire SD card protocol is handled by this driver. Driver support
MBR partitions. Each partition can be mounted as regular file (automatic partition
offset).

@note Make sure that MISO pin is pulled-up. Pull-up is required to correct
      initialize the SD Card SPI interface.

Device (drivers) connection table
| Grade | Device       | In                  | Out              |
| ----: | :----------- | :------------------ | :--------------- |
| 0     | SD Card      | -                   | SPI interface    |
| 1     | SPI driver   | SPI interface       | /dev/spi_sda     |
| 2     | SDSPI driver | /dev/spi_sda        | /dev/sda         |
| 3     | File system  | /dev/sda            | /mnt             |

\section drv-sdspi-sup-arch Supported architectures
\li Any (noarch)

\section drv-sdspi-ddesc Details
\subsection drv-sdspi-ddesc-num Meaning of major and minor numbers
There is special meaning of major and minor numbers. The major number selects
SD card. There is possibility to use up to 256 SD Cards. The minor number has
no meaning.

\subsubsection drv-sdspi-ddesc-numres Numeration restrictions
Major number can be set in range 0 to 255. The minor number should be 0.
Each new major number is a new instance of driver that can handle SD Card.

\subsection drv-sdspi-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("SDSPI", 0, 0, "/dev/sda");
@endcode

Card 2:
@code
driver_init("SDSPI", 1, 0, "/dev/sdb");
@endcode

\subsection drv-sdspi-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("SDSPI", 0, 0);
@endcode
@code
driver_release("SDSPI", 1, 0);
@endcode

\subsection drv-sdspi-ddesc-cfg Driver configuration
Driver configuration can be done by using SDSPI_config_t object and ioctl()
function.

@code
#include <stdio.h>
#include <stdbool.h>
#include <sys/ioctl.h>

static const char *dev_path = "/dev/spi_sda";

FILE *dev = fopen(dev_path, "r+");
if (dev) {
        static const SDSPI_config_t cfg = {
                 .filepath = "/dev/spi_sda",
                 .timeout  = 1000
        };

        if (ioctl(fileno(dev), IOCTL_SDSPI__CONFIGURE, &cfg) != 0) {
            perror(dev_path);
        }

        fclose(dev);
} else {
      perror(dev_path);
}

...
@endcode


\subsection drv-sdspi-ddesc-write Data write
Writing data to device is the same as writing data to regular file.

\subsection drv-sdspi-ddesc-read Data read
Reading data from device is the same as reading data from regular file.

\subsection drv-sdspi-ddesc-mbr Card initialization
There is special ioctl() request (@ref IOCTL_SDSPI__INITIALIZE_CARD or
IOCTL_STORAGE__INITIALIZE) that initialize selected SD card.

<b> The SDSPI Driver configures some options of SPI interface. There are options
that should be configured by user manually:</b>
\li <b> SPI interface clock frequency,</b>
\li <b> SPI CS pin of SD card (SPI and GPIO configuration),</b>
\li <b> internal or external pull-up resistor connected to MISO pin (SD card requirement).</b>


Card initialization example code. In this example is assumption that SPI interface
is configured correctly and SPI interface is visible as <i>/dev/spi_sda</i> file.
The SDSPI driver uses this file as SD card interface (configured in project
setup tool).

\code
#include <stdio.h>
#include <sys/ioctl.h>

// SPI interface initialization
// ...

// creating SD card nodes
driver_init("SDSPI", 0, 0, "/dev/sda");

// SD Card initialization
FILE *f = fopen("/dev/sda", "r+");
if (f) {
        if (ioctl(fileno(f), IOCTL_STORAGE__INITIALIZE) != 0) {
                puts("SD initialization error");
        }

        fclose(f);
}

// file system mount
mkdir("/mnt", 0777);
mount("fatfs", "/dev/sda", "/mnt", "");

// ...

\endcode

@{
*/


#ifndef _SDSPI_IOCTL_H_
#define _SDSPI_IOCTL_H_

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
 *  @brief  SDSPI configuration.
 *  @param  [WR] @ref SDSPI_config_t *  SDSPI configuration object.
 *  @return On success 0 is returned.
 *          On error -1 is returned and @ref errno is set.
 */
#define IOCTL_SDSPI__CONFIGURE          _IOW(SDSPI, 0x00, SDSPI_config_t*)

/**
 *  @brief  Initialize SD card (OS storage request).
 *  @return On success (card initialized) 0 is returned.
 *          On error (card not initialized) -1 is returned and @ref errno is set.
 */
#define IOCTL_SDSPI__INITIALIZE_CARD    IOCTL_STORAGE__INITIALIZE

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * SDSPI module configuration structure.
 */
typedef struct {
        const char *filepath;           /*!< File path to SPI interface. */
        u32_t       timeout;            /*!< Timeout in milliseconds. */
} SDSPI_config_t;

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

#endif /* _SDSPI_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
