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
 * \li STM32F10x
 *
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
