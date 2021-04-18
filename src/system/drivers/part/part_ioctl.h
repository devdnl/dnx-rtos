/*==============================================================================
File    part_ioctl.h

Author  Daniel Zorychta

Brief   Partition driver

        Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

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
@defgroup drv-part PART Driver

\section drv-part-desc Description
Driver handles virtual device volume partitioning.

\section drv-part-sup-arch Supported architectures
\li noarch

\section drv-part-ddesc Details
\subsection drv-part-ddesc-num Meaning of major and minor numbers
Only major number selected device, minor is not used and must be set to 0.

\subsubsection drv-part-ddesc-numres Numeration restrictions
Minor number must be set to 0.

\subsection drv-part-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("PART", 0, 0, "/dev/PART0-0");
@endcode
@code
driver_init("PART", 1, 0, "/dev/PART1-0");
@endcode

\subsection drv-part-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("PART", 0, 0);
@endcode
@code
driver_release("PART", 1, 0);
@endcode

\subsection drv-part-ddesc-cfg Driver configuration
Driver can be configured by using IOCTL_PART__CONFIGURE ioctl request.

\subsection drv-part-ddesc-write Data write
The same as regular file.

\subsection drv-part-ddesc-read Data read
The same as regular file.

@{
*/

#ifndef _PART_IOCTL_H_
#define _PART_IOCTL_H_

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
 *  @brief  Configure driver.
 *  @param  [WR] const PART_config_t*
 *  @return On success 0 is returned, otherwise -1.
 *
 *  @b Example
 *  @code
    #include <sys/ioctl.h>

    //...

    driver_init("PART", 0, 0, "/dev/ee0p1");
    f = fopen("/dev/ee0p1", "r+");
    if (f) {
            static const PART_config_t cfg = {
                    .path          = "/dev/ee0",
                    .block_size    = 256,
                    .offset_blocks = 0,
                    .total_blocks  = 256
            };
            ioctl(fileno(f), IOCTL_PART__CONFIGURE, &cfg);
            fclose(f);
    }

    driver_init("PART", 1, 0, "/dev/ee0p2");
    f = fopen("/dev/ee0p2", "r+");
    if (f) {
            static const PART_config_t cfg = {
                    .path          = "/dev/ee0",
                    .block_size    = 256,
                    .offset_blocks = 256,
                    .total_blocks  = 256
            };
            ioctl(fileno(f), IOCTL_PART__CONFIGURE, &cfg);
            fclose(f);
    }

    //...
    @endcode
 */
#define IOCTL_PART__CONFIGURE           _IOW(PART, 0x00, const PART_config_t*)

/**
 *  @brief  Automatically detects partitions geometry by reading MBR.
 *  @param  none
 *  @return On success 0 is returned, otherwise -1.
 *
 *  @b Example
 *  @code
    #include <sys/ioctl.h>

    //...

    static const PART_config_t cfg = {path: "/dev/usb0"};
    driver_init2("PART", 0, 0, "/dev/usb0p0", &cfg);
    driver_init2("PART", 0, 1, "/dev/usb0p1", &cfg);
    driver_init2("PART", 0, 2, "/dev/usb0p2", &cfg);
    driver_init2("PART", 0, 3, "/dev/usb0p3", &cfg);

    f = fopen("/dev/usb0p0", "r+");  // any file with the same major can be opened
    if (f) {
            // after this ioctl, partitions geometry fill be updated
            ioctl(fileno(f), IOCTL_PART__READ_MBR);
            fclose(f);
    }

    //...
    @endcode
 */
#define IOCTL_PART__READ_MBR            IOCTL_STORAGE__READ_MBR

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct {
        const char *path;               /*!< Device path. */
        u32_t block_size;               /*!< Block size in bytes. */
        u32_t offset_blocks;            /*!< Offset in number of blocks. */
        u32_t total_blocks;             /*!< Total size in number of blocks. */
} PART_config_t;

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

#endif /* _PART_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
