/*=========================================================================*//**
@file    usb_cfg.h

@author  Daniel Zorychta

@brief   USB-Device module.

@note    Copyright (C) 2014  Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _USB_CFG_H_
#define _USB_CFG_H_

/*==============================================================================
  Include files
==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 * Pin connected to pull-up resistor used by system to indicate USB connection
 */
#define _USB_CONFIG_PIN_PULLUP          __USB_PULLUP_PIN__

/**
 * Interrupt priority
 */
#define _USB_IRQ_PRIORITY               __USB_IRQ_PRIORITY__

/**
 * Endpoint 0 size
 * Possible values: 8, 16, 32, 64
 * If value is out of this range then enpoint is disabled.
 * NOTE: make sure that Device Descriptor's bMaxPacketSize0 field has the same value!
 */
#define _USB_ENDPOINT0_SIZE             __USB_ENDPOINT0_SIZE__

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

#endif /* _USB_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
