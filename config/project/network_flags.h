/*=========================================================================*//**
@file    flags.h

@author  Daniel Zorychta

@brief   Global project flags which are included from compiler command line

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

#ifndef _NETWORK_FLAGS_H_
#define _NETWORK_FLAGS_H_

#define __NETWORK_MAC_ADDR_0__ 0x50
#define __NETWORK_MAC_ADDR_1__ 0xE5
#define __NETWORK_MAC_ADDR_2__ 0x49
#define __NETWORK_MAC_ADDR_3__ 0x37
#define __NETWORK_MAC_ADDR_4__ 0xB5
#define __NETWORK_MAC_ADDR_5__ 0xBD
#define __NETWORK_ETHIF_FILE__ "/dev/eth0"

#endif /* _NETWORK_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
