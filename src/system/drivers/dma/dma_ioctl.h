/*==============================================================================
File    dma_ioctl.h

Author  Daniel Zorychta

Brief   General usage DMA driver.

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
@defgroup drv-dma DMA Driver

\section drv-dma-desc Description
Driver handles Direct Memory Access controller. Driver provide ioctl() request
that start DMA memory-to-memory transaction. Beside that driver provide internal
interface for drivers purpose called DDI (Direct Driver Interface). This interface
depends on selected CPU architecture.

@note
The stm32f4 microcontroller accepts memory-to-memory transfers only on DMA2. This
is a hardware limitation.

\section drv-dma-sup-arch Supported architectures
\li stm32f4

\section drv-dma-ddesc Details
\subsection drv-dma-ddesc-num Meaning of major and minor numbers
The major number select DMA peripheral. The minor number is not used.
Some manufactures enumerate devices starting from 1 instead of 0 (e.g. ST).
In this case major number starts from 0 and is connected to the first device
e.g. DMA1.

\subsubsection drv-dma-ddesc-numres Numeration restrictions
The number of peripherals determines how big the major number can be. If there is
only one DMA peripheral then the major number is always 0.
The minor number is not used at all and only 0 is accepted.

\subsection drv-dma-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("DMA", 0, 0, "/dev/DMA1");
@endcode
@code
driver_init("DMA", 1, 0, "/dev/DMA2");
@endcode

\subsection drv-dma-ddesc-release Driver release
Once initialized driver is protected and release is not supported.

\subsection drv-dma-ddesc-cfg Driver configuration
Driver is not configurable.

\subsection drv-dma-ddesc-write Data write
Write is not supported. To start transfer use ioctl() request.

\subsection drv-dma-ddesc-read Data read
Read is not supported. To start transfer use ioctl() request.

\subsection drv-dma-ddesc-transfer Starting DMA transfer
The DMA transfer starts when DMA_transfer_t structure is filled correctly and
ioctl() request is used. Example:

@code
// ...

FILE *f = fopen("/dev/DMA2", "r+");
if (f) {

        DMA_transfer_t t = {
                .src  = SOURCE_PTR,
                .dst  = DESTINATION_PTR,
                .size = SIZE_IN_BYTES
        };

        if (ioctl(f, IOCTL_DMA__TRANSFER, &t) == 0) {
                // success ...
        } else {
                // error ...
        }

        fclose(f);
} else {
        perror("/dev/DMA2");
}

// ...
@endcode

@{
*/

#ifndef _DMA_IOCTL_H_
#define _DMA_IOCTL_H_

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
 *  @brief  Request copy selected memory by using DMA.
 *  @param  [WR,RD] @ref DMA_transfer_t*        DMA transfer descriptor
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_DMA__TRANSFER             _IOWR(DMA, 0x00, const DMA_transfer_t*)

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct {
        u8_t  *src;     /*!< Source address. */
        u8_t  *dst;     /*!< Destination address. */
        size_t size;    /*!< Transfer size in bytes. */
} DMA_transfer_t;

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

#endif /* _DMA_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
