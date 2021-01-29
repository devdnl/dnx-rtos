/*==============================================================================
File    urandom_ioctl.h

Author  Daniel Zorychta

Brief   Pseudo random generator device

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
@defgroup drv-urandom URANDOM Driver

\section drv-urandom-desc Description
Driver read data from pseudo random generator output.

\section drv-urandom-sup-arch Supported architectures
\li noarch

\section drv-urandom-ddesc Details
\subsection drv-urandom-ddesc-num Meaning of major and minor numbers
Major number select device.

\subsubsection drv-urandom-ddesc-numres Numeration restrictions
Minor number has no meaning.

\subsection drv-urandom-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("URANDOM", 0, 0, "/dev/URANDOM0-0");
@endcode
@code
driver_init("URANDOM", 0, 1, "/dev/URANDOM0-1");
@endcode

\subsection drv-urandom-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("URANDOM", 0, 0);
@endcode
@code
driver_release("URANDOM", 0, 1);
@endcode

\subsection drv-urandom-ddesc-cfg Driver configuration
No configuration.

\subsection drv-urandom-ddesc-write Data write
Write operation is not supported.

\subsection drv-urandom-ddesc-read Data read
The same as regular file.

@{
*/

#ifndef _URANDOM_IOCTL_H_
#define _URANDOM_IOCTL_H_

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
 *  @brief  IOCTL set PRNG seed.
 *  @param  [WR] const uint*
 *  @return On success 0 is returned, otherwise -1.
 *
 *  @b Example
 *  @code
    #include <sys/ioctl.h>

    //...

    uint seed = 1234;
    ioctl(fileno(urnd), IOCTL_URANDOM__SET_SEED, &seed);

    //...
    @endcode
 */
#define IOCTL_URANDOM__SET_SEED         _IOW(URANDOM, 0x00, const uint*)

/**
 *  @brief  IOCTL get PRNG seed.
 *  @param  [RD] uint*
 *  @return On success 0 is returned, otherwise -1.
 *
 *  @b Example
 *  @code
    #include <sys/ioctl.h>

    //...

    uint seed = 0;
    ioctl(fileno(urnd), IOCTL_URANDOM__GET_SEED, &seed);

    //...
    @endcode
 */
#define IOCTL_URANDOM__GET_SEED         _IOR(URANDOM, 0x01, uint*)

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

#endif /* _URANDOM_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
