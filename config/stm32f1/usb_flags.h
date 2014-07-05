/*=========================================================================*//**
@file    usb_flags.h

@author  Daniel Zorychta

@brief   USB module configuration flags.

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

#ifndef _USB_FLAGS_H_
#define _USB_FLAGS_H_

#define __USB_PULLUP_PIN__ USB_EN
#define __USB_IRQ_PRIORITY__ CONFIG_USER_IRQ_PRIORITY
#define __USB_ENDPOINT0_SIZE__ 8

#endif /* _USB_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
