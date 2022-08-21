/*==============================================================================
File    fbridge_ioctl.h

Author  Daniel Zorychta

Brief   File Bridge

        Copyright (C) 2022 Daniel Zorychta <daniel.zorychta@gmail.com>

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
@defgroup drv-fbridge FBRIDGE Driver

\section drv-fbridge-desc Description
Driver create bridge between two files. Can be used to create bidirectional file
by using two queues.

\section drv-fbridge-sup-arch Supported architectures
\li noarch

\section drv-fbridge-ddesc Details
\subsection drv-fbridge-ddesc-num Meaning of major and minor numbers
\todo Meaning of major and minor numbers

\subsubsection drv-fbridge-ddesc-numres Numeration restrictions
\todo Numeration restrictions

\subsection drv-fbridge-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("FBRIDGE", 0, 0, "/dev/FBRIDGE0-0");
@endcode
@code
driver_init("FBRIDGE", 0, 1, "/dev/FBRIDGE0-1");
@endcode

\subsection drv-fbridge-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("FBRIDGE", 0, 0);
@endcode
@code
driver_release("FBRIDGE", 0, 1);
@endcode

\subsection drv-fbridge-ddesc-cfg Driver configuration
\todo Driver configuration

\subsection drv-fbridge-ddesc-write Data write
\todo Data write

\subsection drv-fbridge-ddesc-read Data read
\todo Data read

@{
*/

#ifndef _FBRIDGE_IOCTL_H_
#define _FBRIDGE_IOCTL_H_

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
 *  @brief  Example IOCTL request.
 *  @param  [WR] const FBRIDGE_conf_t * configuration structure
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_FBRIDGE__CONFIGURE        _IOW(FBRIDGE, 0x00, const FBRIDGE_conf_t *)

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct {
        const char *read_path;
        const char *write_path;
} FBRIDGE_conf_t;

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

#endif /* _FBRIDGE_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
