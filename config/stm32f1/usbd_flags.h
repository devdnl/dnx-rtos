/*=========================================================================*//**
@file    usbd_flags.h

@author  Daniel Zorychta

@brief   USB-Device module configuration flags.

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

/*
 * NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as suffix
 *       and prefix) are exported to the single configuration file
 *       (by using Configtool) when entire project configuration is exported.
 *       All other flag definitions and statements are ignored.
 */

#ifndef _USBD_FLAGS_H_
#define _USBD_FLAGS_H_

#define __USBD_PULLUP_PIN__ USB_EN
#define __USBD_IRQ_PRIORITY__ CONFIG_USER_IRQ_PRIORITY
#define __USBD_ENDPOINT0_SIZE__ 8

#endif /* _USBD_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
