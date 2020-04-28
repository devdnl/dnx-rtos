/*=========================================================================*//**
@file    wdg_ioctl.h

@author  Daniel Zorychta

@brief   WDG driver ioctl request codes.

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
@defgroup drv-wdg WDG Driver

\section drv-wdg-desc Description
Driver handles watchdog peripheral.

\section drv-wdg-sup-arch Supported architectures
\li stm32f1
\li stm32f4
\li stm32f7

\section drv-wdg-ddesc Details
\subsection drv-wdg-ddesc-num Meaning of major and minor numbers
Both major and minor numbers should be set to 0.

\subsubsection drv-wdg-ddesc-numres Numeration restrictions
Both major and minor numbers should be set to 0.

\subsection drv-wdg-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("WDG", 0, 0, "/dev/wdg");
@endcode

@note Watchdog peripheral starts counting reset time from driver initialization.
      Make sure that system initialization is not too long or watchdog driver
      is initialized at the end of system startup to protect against unexpected
      reset.

\subsection drv-wdg-ddesc-release Driver release
There is not possibility to release WDG driver.

\subsection drv-wdg-ddesc-cfg Driver configuration
Watchdog can be configured only at project configuration by using Configtool.
There is no possibility to reconfigure watchdog peripheral at runtime.

\subsection drv-wdg-ddesc-write Data write
Operation not supported.

\subsection drv-wdg-ddesc-read Data read
Operation not supported.

\subsection drv-wdg-ddesc-reset Watchdog reset
Watchdog should be reseted by using ioctl() function. Example:

\code
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

//...

FILE *wdg = fopen("/dev/wdg", "r+");
if (!wdg) {
        abort();
}

//...


if (ioctl(fileno(wdg), IOCTL_WDG__RESET) != 0) {
    // no permission or IO error
}

//...

\endcode

@{
*/

#ifndef _WDG_IOCTL_H_
#define _WDG_IOCTL_H_

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
 *  @brief Reset watchdog counter.
 *
 *  If this request won't send on time the watchdog resets the system.
 *
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_WDG__RESET                _IO(WDG, 0x00)

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

#endif /* _WDG_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
