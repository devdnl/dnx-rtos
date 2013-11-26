/*=========================================================================*//**
@file    ethmac_cfg.h

@author  Daniel Zorychta

@brief   This driver support Ethernet interface.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _ETHMAC_CFG_H_
#define _ETHMAC_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "config.h"

/*==============================================================================
  Exported macros
==============================================================================*/
/*
 * Ethernet MAC IRQ priority
 */
#define ETHMAC_IRQ_PRIORITY             CONFIG_USER_IRQ_PRIORITY

/*
 * Enable (1) or disable (0) hardware checksum calculation
 */
#define ETHMAC_CHECKSUM_BY_HARDWARE     1

/*
 * Ethernet MAC speed (ETH_Speed_100M or ETH_Speed_10M)
 */
#define ETHMAC_SPEED                    ETH_Speed_100M

/*
 * PHY address
 */
#define ETHMAC_PHY_ADDRESS              0x01

/*
 * PHY reset delay [ms]
 */
#define ETHMAC_PHY_RESET_DELAY          250

/*
 * PHY configuration apply delay [ms]
 */
#define ETHMAC_PHY_CONFIG_DELAY         250

/*
 * Transceiver Status Register address
 *
 * For DP83848: 16
 * For LAN8700: 31
 */
#define ETHMAC_PHY_SR                   16

/*
 * PHY speed status 16-bit mask
- *
 * For DP83848: 0x0002
 * For LAN8700: 0x0004
 */
#define ETHMAC_PHY_SPEED_STATUS_BM      0x0002

/*
 * PHY duplex status 16-bit mask
 *
 * For DP83848: 0x0004
 * For LAN8700: 0x0010
 */
#define ETHMAC_PHY_DUPLEX_STATUS_BM     0x0004

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

#endif /* _ETHMAC_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
