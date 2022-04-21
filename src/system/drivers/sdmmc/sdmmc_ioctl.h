/*==============================================================================
File    sdmmc_ioctl.h

Author  Daniel Zorychta

Brief   SD/MMC driver

        Copyright (C) 2022 Daniel Zorychta <daniel.zorychta@gmail.com>

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
@defgroup drv-sdmmc SDMMC Driver

\section drv-sdmmc-desc Description
Driver handles SD/MMC/eMMC cards/chips.

\section drv-sdmmc-sup-arch Supported architectures
\li stm32h7

\section drv-sdmmc-ddesc Details
\subsection drv-sdmmc-ddesc-num Meaning of major and minor numbers
There is special meaning of major and minor numbers. The major number selects
SD/MMC/eMMC card. There is possibility to use up to 256 SD Cards.
The minor number has no meaning and should be set to 0.

\subsubsection drv-sdmmc-ddesc-numres Numeration restrictions
Major number can be set in range 0 to 255. The minor number should be set to 0.
Each new major number is a new instance of driver.

\subsection drv-sdmmc-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("SDMMC", 0, 0, "/dev/sda");
@endcode

Card 2:
@code
driver_init("SDMMC", 1, 0, "/dev/sdb");
@endcode

\subsection drv-sdmmc-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("SDMMC", 0, 0);
@endcode
@code
driver_release("SDMMC", 1, 0);
@endcode

\subsection drv-sdmmc-ddesc-cfg Driver configuration
Driver configuration can be done only by using configuration files (by Configtool).
Configuration is static because there is no necessary to change driver
configuration during runtime.

\subsection drv-sdmmc-ddesc-write Data write
Writing data to device is the same as writing data to regular file with some
restrictions: the seek position should be aligned to sector size (512 bytes)
and buffer size should be multiple of 512 bytes.

\subsection drv-sdmmc-ddesc-read Data read
Reading data from device is the same as reading data from regular file with some
restrictions: the seek position should be aligned to sector size (512 bytes)
and buffer size should be multiple of 512 bytes.

\subsection drv-sdmmc-ddesc-mbr Card initialization
There is special ioctl() request (@ref IOCTL_SDIO__INITIALIZE_CARD or
IOCTL_STORAGE__INITIALIZE) that initialize selected SD card. Initialization
can be done on any partition (e.g. sda1) giving the same effect as initialization
on master card file (e.g. sda, sdb). If initialization is already done on
selected partition then is not necessary to initialize remained partitions.

Card initialization example code.

\code
#include <stdio.h>
#include <sys/ioctl.h>

// creating SD card nodes
driver_init("SDMMC", 0, 0, "/dev/sda");

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
mount("fatfs", "/dev/sda1", "/mnt", "");

// ...

\endcode

@{
*/

#ifndef _SDMMC_IOCTL_H_
#define _SDMMC_IOCTL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/ioctl_macros.h"
#include "drivers/class/storage/ioctl.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 *  @brief  Initialize SD/MMC card (OS storage request).
 *  @return On success (card initialized) 0 is returned.
 *          On error (card not initialized) -1 is returned and @ref errno is set.
 */
#define IOCTL_SDMMC__INITIALIZE_CARD    IOCTL_STORAGE__INITIALIZE

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

#endif /* _SDMMC_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
