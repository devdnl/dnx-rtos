/*==============================================================================
File    timenc_ioctl.h

Author  Daniel Zorychta

Brief   Timer driven AB encoder

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
@defgroup drv-timenc TIMENC Driver

\section drv-timenc-desc Description
Driver handles ...

\section drv-timenc-sup-arch Supported architectures
\li stm32f7

\section drv-timenc-ddesc Details
\subsection drv-timenc-ddesc-num Meaning of major and minor numbers
\todo Meaning of major and minor numbers

\subsubsection drv-timenc-ddesc-numres Numeration restrictions
\todo Numeration restrictions

\subsection drv-timenc-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("TIMENC", 0, 0, "/dev/TIMENC0-0");
@endcode
@code
driver_init("TIMENC", 0, 1, "/dev/TIMENC0-1");
@endcode

\subsection drv-timenc-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("TIMENC", 0, 0);
@endcode
@code
driver_release("TIMENC", 0, 1);
@endcode

\subsection drv-timenc-ddesc-cfg Driver configuration
\todo Driver configuration

\subsection drv-timenc-ddesc-write Data write
\todo Data write

\subsection drv-timenc-ddesc-read Data read
\todo Data read

@{
*/

#ifndef _TIMENC_IOCTL_H_
#define _TIMENC_IOCTL_H_

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
 *  @brief  Encoder configuration.
 *  @param  [WR] const TIMENC_config_t*
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_TIMENC__CONFIGURE         _IOW(TIMENC, 0x00, const TIMENC_config_t*)

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct {
        GPIO_pin_in_port_t pin_A;
        GPIO_pin_in_port_t pin_B;
} TIMENC_config_t;

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

#endif /* _TIMENC_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
