/*==============================================================================
File    sdio_ioctl.h

Author  Daniel Zorychta

Brief   SD Card Interface Driver

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
Driver handles ...

\section drv-sdio-sup-arch Supported architectures
\li stm32f4

\section drv-sdio-ddesc Details
\subsection drv-sdio-ddesc-num Meaning of major and minor numbers
\todo Meaning of major and minor numbers

\subsubsection drv-sdio-ddesc-numres Numeration restrictions
\todo Numeration restrictions

\subsection drv-sdio-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("SDIO", 0, 0, "/dev/SDIO0-0");
@endcode
@code
driver_init("SDIO", 0, 1, "/dev/SDIO0-1");
@endcode

\subsection drv-sdio-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("SDIO", 0, 0);
@endcode
@code
driver_release("SDIO", 0, 1);
@endcode

\subsection drv-sdio-ddesc-cfg Driver configuration
\todo Driver configuration

\subsection drv-sdio-ddesc-write Data write
\todo Data write

\subsection drv-sdio-ddesc-read Data read
\todo Data read

@{
*/

#ifndef _SDIO_IOCTL_H_
#define _SDIO_IOCTL_H_

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
#define IOCTL_SDIO__INITIALIZE_CARD     IOCTL_STORAGE__INITIALIZE

/**
 *  @brief  Read card's MBR sector and detect partitions (OS storage request).
 *  @return On success (MBR detected) 0 is returned.
 *          On error -1 (MBR not exist or IO error) is returned and @ref errno is set.
 */
#define IOCTL_SDIO__READ_MBR            IOCTL_STORAGE__READ_MBR

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
