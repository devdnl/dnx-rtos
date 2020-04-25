/*=========================================================================*//**
@file    afm_ioctl.h

@author  Daniel Zorychta

@brief   This driver support AFM ioctl request codes.

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
@defgroup drv-afm AFM Driver (Alternative Function Module)

\section drv-afm-desc Description
Driver configure alternative functions of microcontroller e.g. JTAG pins disable,
external interrupt pin assignment, GPIO remap, etc.

\section drv-afm-sup-arch Supported architectures
\li stm32f1
\li stm32f4
\li stm32f7

\section drv-afm-ddesc Details
\subsection drv-afm-ddesc-num Meaning of major and minor numbers
There is no special meaning of major-minor numbers. The peripheral is only
accessible by using major-minor set to 0.

\subsection drv-afm-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("AFM", 0, 0, "/dev/AFM");
@endcode
or:
@code
driver_init("AFM", 0, 0, NULL);  // without creating device node
@endcode

\subsection drv-afm-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("AFM", 0, 0);
@endcode

\subsection drv-afm-ddesc-cfg Driver configuration
Entire driver configuration is realized by using configuration files in
the <tt>./config</tt> directory or by using Configtool.

\subsection drv-afm-ddesc-write Data write
There is no possibility to write any data to the device.

\subsection drv-afm-ddesc-read Data read
There is no possibility to read any data from the device.

@{
*/

#ifndef _AFM_IOCTL_H_
#define _AFM_IOCTL_H_

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

#endif /* _AFM_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
