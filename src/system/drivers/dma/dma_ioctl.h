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
Driver handles ...

\section drv-dma-sup-arch Supported architectures
\li stm32f4

\section drv-dma-ddesc Details
\subsection drv-dma-ddesc-num Meaning of major and minor numbers
\todo Meaning of major and minor numbers

\subsubsection drv-dma-ddesc-numres Numeration restrictions
\todo Numeration restrictions

\subsection drv-dma-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("DMA", 0, 0, "/dev/DMA0-0");
@endcode
@code
driver_init("DMA", 0, 1, "/dev/DMA0-1");
@endcode

\subsection drv-dma-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("DMA", 0, 0);
@endcode
@code
driver_release("DMA", 0, 1);
@endcode

\subsection drv-dma-ddesc-cfg Driver configuration
\todo Driver configuration

\subsection drv-dma-ddesc-write Data write
\todo Data write

\subsection drv-dma-ddesc-read Data read
\todo Data read

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
#define IOCTL_DMA__TRANSFER             _IOWR(DMA, 0x00, DMA_transfer_t*)

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct {
        u8_t  *src;
        u8_t  *dst;
        size_t count;
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
