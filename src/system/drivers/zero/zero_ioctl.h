/*==============================================================================
File    zero_ioctl.h

Author  Daniel Zorychta

Brief   Zero data device

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
@defgroup drv-zero ZERO Driver

\section drv-zero-desc Description
Driver output zeros.

\section drv-zero-sup-arch Supported architectures
\li noarch

\section drv-zero-ddesc Details
\subsection drv-zero-ddesc-num Meaning of major and minor numbers
Major number selects device.

\subsubsection drv-zero-ddesc-numres Numeration restrictions
Minor number is not used and should be set to 0.

\subsection drv-zero-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("ZERO", 0, 0, "/dev/ZERO0-0");
@endcode
@code
driver_init("ZERO", 0, 1, "/dev/ZERO0-1");
@endcode

\subsection drv-zero-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("ZERO", 0, 0);
@endcode
@code
driver_release("ZERO", 0, 1);
@endcode

\subsection drv-zero-ddesc-cfg Driver configuration
No driver configuration.

\subsection drv-zero-ddesc-write Data write
Write operation is not supported.

\subsection drv-zero-ddesc-read Data read
The same as regular file.

@{
*/

#ifndef _ZERO_IOCTL_H_
#define _ZERO_IOCTL_H_

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

#endif /* _ZERO_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
