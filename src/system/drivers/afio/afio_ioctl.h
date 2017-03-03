/*=========================================================================*//**
@file    afio_ioctl.h

@author  Daniel Zorychta

@brief   This driver support AFIO ioctl request codes.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the  Free Software  Foundation;  either version 2 of the License, or
         any later version.

         This  program  is  distributed  in the hope that  it will be useful,
         but  WITHOUT  ANY  WARRANTY;  without  even  the implied warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         You  should  have received a copy  of the GNU General Public License
         along  with  this  program;  if not,  write  to  the  Free  Software
         Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


*//*==========================================================================*/

/**
 * @defgroup drv-afio AFIO Driver
 *
 * \section drv-afio-desc Description
 * Driver configures AFIO peripheral according to configuration. Driver is
 * responsible for setting Alternative Function Input Output functionality;
 * configures GPIO and other microcontroller-specific options (e.g. IRQ pin
 * selection, JTAG configuration, etc).
 *
 * \section drv-afio-sup-arch Supported architectures
 * \li stm32f1
 *
 * \section drv-afio-ddesc Details
 * \subsection drv-afio-ddesc-num Meaning of major and minor numbers
 * There is no special meaning of major-minor numbers. The peripheral is only
 * accessible by using major-minor set to 0.
 *
 * \subsection drv-afio-ddesc-init Driver initialization
 * To initialize driver the following code can be used:
 *
 * @code
   driver_init("AFIO", 0, 0, "/dev/AFIO");
   @endcode
 * or:
   @code
   driver_init("AFIO", 0, 0, NULL);  // without creating device node
   @endcode
 *
 * \subsection drv-afio-ddesc-release Driver release
 * To release driver the following code can be used:
 * @code
   driver_release("AFIO", 0, 0);
   @endcode
 *
 * \subsection drv-afio-ddesc-cfg Driver configuration
 * Entire driver configuration is realized by using configuration files in
 * the <tt>./config</tt> directory or by using Configtool.
 *
 * \subsection drv-afio-ddesc-write Data write
 * There is no possibility to write any data to the device.
 *
 * \subsection drv-afio-ddesc-read Data read
 * There is no possibility to read any data from the device.
 * @{
 */

#ifndef _AFIO_IOCTL_H_
#define _AFIO_IOCTL_H_

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

#endif /* _AFIO_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
