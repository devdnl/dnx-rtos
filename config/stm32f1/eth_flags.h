/*=========================================================================*//**
@file    eth_flags.h

@author  Daniel Zorychta

@brief   ETHMAC module configuration flags.

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

#ifndef _ETH_FLAGS_H_
#define _ETH_FLAGS_H_

#define __ETH_IRQ_PRIORITY__ CONFIG_USER_IRQ_PRIORITY
#define __ETH_CHECKSUM_BY_HARDWARE__ _YES_
#define __ETH_SPEED__ ETH_Speed_100M
#define __ETH_PHY_ADDRESS__ 0x01
#define __ETH_PHY_RESET_DELAY__ 250
#define __ETH_PHY_CONFIG_DELAY__ 250
#define __ETH_DEVICE__ DP83848
#define __ETH_PHY_SR__ 16
#define __ETH_PHY_SPEED_STATUS_BM__ 0x0002
#define __ETH_PHY_DUPLEX_STATUS_BM__ 0x0004

#endif /* _ETH_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
