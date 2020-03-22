/*==============================================================================
File    nvm_ioctl.h

Author  Daniel Zorychta

Brief   NVM driver

        Copyright (C) 2018 Daniel Zorychta <daniel.zorychta@gmail.com>

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
@defgroup drv-nvm NVM Driver

\section drv-nvm-desc Description
Driver handles non-volatile memory (e.g. flash) where sector erase is required
to write new data. Make sure that device is able to write/read byte-by-byte. Some
devices accept only aligned addresses and data (e.g. stm32f1 family requires
address and data aligned to 16-bits at write operation; read is possible
byte-by-byte).

\section drv-nvm-sup-arch Supported architectures
\li stm32f4

\section drv-nvm-ddesc Details
\subsection drv-nvm-ddesc-num Meaning of major and minor numbers
The major number determines a number of block device (e.g. nvmblk0, nvmblk1).
The minor number has no meaning and should be set to 0.

\subsubsection drv-nvm-ddesc-numres Numeration restrictions
The major number can be in range of 0-255. The minor number should be set always
to 0.

\subsection drv-nvm-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("NVM", 0, 0, "/dev/nvmblk0");
@endcode
@code
driver_init("NVM", 1, 0, "/dev/nvmblk1");
@endcode

\subsection drv-nvm-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("NVM", 0, 0);
@endcode
@code
driver_release("NVM", 1, 0);
@endcode

\subsection drv-nvm-ddesc-cfg Driver configuration
To configure driver the ioctl() function should be used:

@code
#include <stdio.h>
#include <sys/ioctl.h>

// ...

f = fopen("/dev/nvm", "r+");
if (f) {

        static const NVM_config_t conf = {
                .start_sector = 0,
                .number_of_sectors = 12
        };

        ioctl(fileno(f), IOCTL_NVM__CONFIGURE, &conf);

        fclose(f);
}

// ...
@endcode

Start sector is a sector from driver handles memory. This is the offset at
entire NVM memory block. After configuration all sectors are relative to selected
one.

\subsection drv-nvm-ddesc-write Data write
Writing to the device works in the same way as writing to a regular file. There
is only one exception - bytes that were already written must be erased by using
ioctl() function with IOCTL_NVM__SECTOR_ERASE.

\subsubsection drv-nvm-ddsec-erase Sector erase
Selected sector can be erase by using ioctl() function:
@code
#include <stdio.h>
#include <sys/ioctl.h>

// ...

FILE *f = fopen("/dev/nvm", "r+");
if (f) {
        size_t sector = 8;
        ioctl(fileno(f), IOCTL_NVM__SECTOR_ERASE, &sector);

        // ...
}

// ...
@endcode

There is possibility to erase entire memory block:
@code
#include <stdio.h>
#include <sys/ioctl.h>

// ...

FILE *f = fopen("/dev/nvm", "r+");
if (f) {
        ioctl(fileno(f), IOCTL_NVM__MASS_ERASE);

        // ...
}

// ...
@endcode

\subsection drv-nvm-ddesc-read Data read
Reading from the device works in the same way as reading from a regular file.

@{
*/

#ifndef _NVM_IOCTL_H_
#define _NVM_IOCTL_H_

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
 *  Request configure NVM block.
 *  @param  [WR] const NVM_config_t* configuration object
 *  @return On success 0 is returned, otherwise -1.
 *
 *  @b Example
 *  @code
        #include <stdio.h>
        #include <sys/ioctl.h>

        // ...

        f = fopen("/dev/nvm", "r+");
        if (f) {

                static const NVM_config_t conf = {
                        .start_sector = 0,
                        .number_of_sectors = 12
                };

                ioctl(fileno(f), IOCTL_NVM__CONFIGURE, &conf);

                fclose(f);
        }

        // ...
    @endcode
 */
#define IOCTL_NVM__CONFIGURE            _IOW(NVM, 0, const NVM_config_t*)

/**
 *  Request erase selected sector. Sector number is relative to start sector in
 *  configuration. Erasing sector #0 (for selected NVM block) in device configured
 *  with start sector set to 10, driver erase sector 10. In case when user select
 *  sector #5 to erase then sector 15 will be erased (relative sector number is
 *  automatically calculated to absolute one).
 *  @param  [WR] const size_t* sector to erase
 *  @return On success 0 is returned, otherwise -1.
 *
 *  @b Example
 *  @code
        #include <stdio.h>
        #include <sys/ioctl.h>

        // ...

        FILE *f = fopen("/dev/nvm", "r+");
        if (f) {
                size_t sector = 8;
                ioctl(fileno(f), IOCTL_NVM__SECTOR_ERASE, &sector);

                // ...
        }

        // ...
    @endcode
 */
#define IOCTL_NVM__SECTOR_ERASE         _IOW(NVM, 1, const size_t*)


/**
 *  Request erase entire memory block.
 *  @return On success 0 is returned, otherwise -1.
 *
 *  @b Example
 *  @code
        #include <stdio.h>
        #include <sys/ioctl.h>

        // ...

        FILE *f = fopen("/dev/nvm", "r+");
        if (f) {
                ioctl(fileno(f), IOCTL_NVM__MASS_ERASE);

                // ...
        }

        // ...
    @endcode
 */
#define IOCTL_NVM__MASS_ERASE           _IO(NVM, 2)


/**
 *  Request return size of selected sector. Sector sizes can be different.
 *  @param  [WR,RD] NVM_sector_size_t* sector size descriptor
 *  @return On success 0 is returned, otherwise -1.
 *
 *  @b Example
 *  @code
        #include <stdio.h>
        #include <sys/ioctl.h>

        // ...

        FILE *f = fopen("/dev/nvm", "r+");
        if (f) {
                NVM_sector_size_t secsize;
                secsize.sector = 8;
                secsize.size   = 0;

                ioctl(fileno(f), IOCTL_NVM__GET_SECTOR_SIZE, &secsize);

                printf("Sector %d size: %d bytes\n", secsize.sector, secsize.size);

                // ...
        }

        // ...
    @endcode
 */
#define IOCTL_NVM__GET_SECTOR_SIZE      _IOWR(NVM, 3, NVM_sector_size_t*)


/**
 *  Request return number of sectors in block.
 *  @param  [RD] size_t* number of sectors in block
 *  @return On success 0 is returned, otherwise -1.
 *
 *  @b Example
 *  @code
        #include <stdio.h>
        #include <sys/ioctl.h>

        // ...

        FILE *f = fopen("/dev/nvm", "r+");
        if (f) {
                size_t sectors = 0;
                ioctl(fileno(f), IOCTL_NVM__GET_SECTOR_COUNT, &sectors);

                printf("There is %d sectors in this device\n", sectors);

                // ...
        }

        // ...
    @endcode
 */
#define IOCTL_NVM__GET_SECTOR_COUNT     _IOR(NVM, 4, size_t*)

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * This type represent NVM device memory operation range.
 */
typedef struct {
        uint32_t start_sector;          /*!< Start sector (first sector). */
        uint32_t number_of_sectors;     /*!< Number of handled sectors. */
} NVM_config_t;

/**
 * This type represent sector size.
 */
typedef struct {
        size_t sector;                  /*!< Requested sector number (relative). */
        size_t size;                    /*!< Sector size (return value). */
} NVM_sector_size_t;

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

#endif /* _NVM_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
