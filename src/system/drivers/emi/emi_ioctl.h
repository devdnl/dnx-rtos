/*==============================================================================
File    emi_ioctl.h

Author  Daniel Zorychta

Brief   Flexible Memory Controller

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
@defgroup drv-emi EMI Driver (Flexible Memory Controller)

\section drv-emi-desc Description
Driver handles ...

\section drv-emi-sup-arch Supported architectures
\li stm32f4, stm32f7, stm32h7

\section drv-emi-ddesc Details
\subsection drv-emi-ddesc-num Meaning of major and minor numbers
\todo Meaning of major and minor numbers

\subsubsection drv-emi-ddesc-numres Numeration restrictions
\todo Numeration restrictions

\subsection drv-emi-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("EMI", 0, 0, "/dev/EMI0-0");
@endcode
@code
driver_init("EMI", 0, 1, "/dev/EMI0-1");
@endcode

\subsection drv-emi-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("EMI", 0, 0);
@endcode
@code
driver_release("EMI", 0, 1);
@endcode

\subsection drv-emi-ddesc-cfg Driver configuration
\todo Driver configuration

\subsection drv-emi-ddesc-write Data write
\todo Data write

\subsection drv-emi-ddesc-read Data read
\todo Data read

@{
*/

#ifndef _EMI_IOCTL_H_
#define _EMI_IOCTL_H_

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

#endif /* _EMI_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
