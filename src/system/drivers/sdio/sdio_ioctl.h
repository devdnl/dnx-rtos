/*==============================================================================
File    sdio_ioctl.h

Author  Daniel Zorychta

Brief   SD Card Interface Driver.

        Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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
@defgroup drv-sdio SDIO Driver

\section drv-sdio-desc Description
Driver handles SD Cards in native mode (SDIO).

\section drv-sdio-sup-arch Supported architectures
\li stm32f4

\section drv-sdio-ddesc Details
\subsection drv-sdio-ddesc-num Meaning of major and minor numbers
There is special meaning of major and minor numbers. The major number selects
SD card. There is possibility to use up to 256 SD Cards. The minor number has
no meaning.

@note The stm32f4 port supports only 1 SD card.

\subsubsection drv-sdio-ddesc-numres Numeration restrictions
Major number can be set in range 0 to 255. The minor number can be used to 0.
Each new major number is a new instance of driver that can handle
SD Card.

\subsection drv-sdio-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("SDIO", 0, 0, "/dev/sda");
@endcode

Card 2:
@code
driver_init("SDIO", 1, 0, "/dev/sdb");
@endcode

\subsection drv-sdio-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("SDIO", 0, 0);
@endcode
@code
driver_release("SDIO", 1, 0);
@endcode

\subsection drv-sdio-ddesc-cfg Driver configuration
Driver configuration can be done only by using configuration files (by Configtool).
Configuration is static because there is no necessary to change driver
configuration during runtime.

\subsection drv-sdio-ddesc-write Data write
Writing data to device is the same as writing data to regular file with some
restrictions: the seek position should be aligned to sector size (512 bytes)
and buffer size should be multiple of 512 bytes.

\subsection drv-sdio-ddesc-read Data read
Reading data from device is the same as reading data from regular file with some
restrictions: the seek position should be aligned to sector size (512 bytes)
and buffer size should be multiple of 512 bytes.

\subsection drv-sdio-ddesc-mbr Card initialization
There is special ioctl() request (@ref IOCTL_SDIO__INITIALIZE_CARD or
IOCTL_STORAGE__INITIALIZE) that initialize selected SD card. Initialization
can be done on any partition (e.g. sda1) giving the same effect as initialization
on master card file (e.g. sda, sdb). If initialization is already done on
selected partition then is not necessary to initialize remained partitions.

After initialization the MBR table should be read by driver. In this case the
ioctl() request (@ref IOCTL_SDIO__READ_MBR or IOCTL_STORAGE__READ_MBR)
should be used. This procedure load all MBR entries and set required offsets in
partition files (/dev/sd<i>x</i><b>y</b>).

Card initialization example code.

\code
#include <stdio.h>
#include <sys/ioctl.h>

// creating SD card nodes
driver_init("SDIO", 0, 0, "/dev/sda");

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

#ifndef _SDIO_IOCTL_H_
#define _SDIO_IOCTL_H_

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
 *  @brief  Initialize SD card (OS storage request).
 *  @return On success (card initialized) 0 is returned.
 *          On error (card not initialized) -1 is returned and @ref errno is set.
 */
#define IOCTL_SDIO__INITIALIZE_CARD     IOCTL_STORAGE__INITIALIZE

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

#endif /* _SDIO_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
