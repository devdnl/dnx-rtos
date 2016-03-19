/*=========================================================================*//**
@file    wdg_ioctl.h

@author  Daniel Zorychta

@brief   WDG driver ioctl request codes.

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
 * @defgroup drv-wdg Watchdog Driver
 *
 * \section drv-wdg-desc Description
 * Driver handles watchdog peripheral.
 *
 * \section drv-wdg-sup-arch Supported architectures
 * \li STM32F10x
 *
 * @todo Details
 *
 *
 * @{
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
 *  @brief Reset watchdog counter. If this request won't send on time the
 *         watchdog resets the system.
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
