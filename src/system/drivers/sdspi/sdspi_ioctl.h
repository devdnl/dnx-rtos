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
@defgroup drv-sdspi SDSPI Driver

\section drv-sdspi-desc Description
Driver handles SD Card by using SPI interface (mode). The SPI interface is handled
by file thereby can be used by any microcontroller architecture. Driver handle
all SD card communication; writing or reading the card, from user point of
view, looks like write or read a regular file. Data is automatically divided to
blocks. Entire SD card protocol is handled by this driver. Driver support
MBR partitions. Each partition can be mounted as regular file (automatic partition
offset).

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
SD card 0 or 1. The minor number selects as follow:
\li 0: entire card volume
\li 1: partition 1 (MBR only)
\li 2: partition 2 (MBR only)
\li 3: partition 3 (MBR only)
\li 4: partition 4 (MBR only)

\subsubsection drv-sdspi-ddesc-numres Numeration restrictions
Major number can be set in range 0 to 1. The minor number can be used in range
from 0 to 4.

\subsection drv-sdspi-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("SDSPI", 0, 0, "/dev/sda");         // entire volume
driver_init("SDSPI", 0, 1, "/dev/sda1");        // partition 1
driver_init("SDSPI", 0, 2, "/dev/sda2");        // partition 2
driver_init("SDSPI", 0, 3, "/dev/sda3");        // partition 3
driver_init("SDSPI", 0, 4, "/dev/sda4");        // partition 4
@endcode

Card 2:
@code
driver_init("SDSPI", 1, 0, "/dev/sdb");         // entire volume
driver_init("SDSPI", 1, 1, "/dev/sdb1");        // partition 1
driver_init("SDSPI", 1, 2, "/dev/sdb2");        // partition 2
driver_init("SDSPI", 1, 3, "/dev/sdb3");        // partition 3
driver_init("SDSPI", 1, 4, "/dev/sdb4");        // partition 4
@endcode

If card does not contains any partitions then SDSPIx:0 only can be used. If card
has e.g. only 1 partition (MBR exist) then only one entry can be created. Created
device files can be used directly by file systems.

\subsection drv-sdspi-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("SDSPI", 0, 4);          // disable supporting of partition number 4
@endcode
@code
driver_release("SDSPI", 1, 1);          // disable supporting of partition number 1
@endcode

\subsection drv-sdspi-ddesc-cfg Driver configuration
Driver configuration can be done by using Configtool. There is possibility to set
SPI interface file path and additional options.

\subsection drv-sdspi-ddesc-write Data write
Writing data to device is the same as writing data to regular file.

\subsection drv-sdspi-ddesc-read Data read
Reading data from device is the same as reading data from regular file.

\subsection drv-sdspi-ddesc-mbr Card initialization
There is special ioctl() request (@ref IOCTL_SDSPI__INITIALIZE_CARD) that initialize
selected SD card. Initialization can be done on any partition (e.g. sda1) giving
the same effect as initialization on master card file (e.g. sda, sdb). If
initialization is already done on selected partition then is not necessary to
initialize remained partitions.

After initialization the MBR table should be read by driver. In this case the
ioctl() request (@ref IOCTL_SDSPI__READ_MBR) should be used. This procedure
load all MBR entries and set required offsets in partition files (/dev/sd<i>x</i><b>y</b>).

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
driver_init("SDSPI", 0, 1, "/dev/sda1");

// SD Card initialization
FILE *f = fopen("/dev/sda", "r+");
if (f) {
        if (ioctl(f, IOCTL_STORAGE__INITIALIZE) != 0) {
                puts("SD initialization error");

        } else {
                if (ioctl(f, IOCTL_STORAGE__READ_MBR) != 0) {
                        puts("SD read MBR error");
                }
        }

        fclose(f);
}

// file system mount
mkdir("/mnt", 0777);
mount("fatfs", "/dev/sda1", "/mnt", "");

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
 *  @brief  Initialize SD card (OS storage request).
 *  @return On success (card initialized) 0 is returned.
 *          On error (card not initialized) -1 is returned and @ref errno is set.
 */
#define IOCTL_SDSPI__INITIALIZE_CARD    IOCTL_STORAGE__INITIALIZE

/**
 *  @brief  Read card's MBR sector and detect partitions (OS storage request).
 *  @return On success (MBR detected) 0 is returned.
 *          On error -1 (MBR not exist or IO error) is returned and @ref errno is set.
 */
#define IOCTL_SDSPI__READ_MBR           IOCTL_STORAGE__READ_MBR

/*==============================================================================
  Exported object types
==============================================================================*/

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
