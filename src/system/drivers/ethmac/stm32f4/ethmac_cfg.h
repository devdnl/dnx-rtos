/*=========================================================================*//**
@file    ethmac_cfg.h

@author  Daniel Zorychta

@brief   This driver support Ethernet interface.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
 * Enable (1) or disable (0) hardware checksum calculation
 */
#define ETHMAC_CHECKSUM_BY_HARDWARE     __ETHMAC_CHECKSUM_BY_HARDWARE__

/*
 * Ethernet MAC speed (ETH_Speed_100M or ETH_Speed_10M)
 */
#define ETHMAC_SPEED                    __ETHMAC_SPEED__

/*
 * Rx buffer count
 */
#define ETHMAC_RXBUFNB                  __ETHMAC_RXBUFNB__

/*
 * Tx buffer count
 */
#define ETHMAC_TXBUFNB                  __ETHMAC_TXBUFNB__

/*
 * PHY address
 */
#define ETHMAC_PHY_ADDRESS              __ETHMAC_PHY_ADDRESS__

/*
 * PHY reset delay [ms]
 */
#define ETHMAC_PHY_RESET_DELAY          __ETHMAC_PHY_RESET_DELAY__

/*
 * PHY configuration apply delay [ms]
 */
#define ETHMAC_PHY_CONFIG_DELAY         __ETHMAC_PHY_CONFIG_DELAY__

/*
 * Transceiver Status Register address
 *
 * For DP83848: 16
 * For LAN8700: 31
 */
#define ETHMAC_PHY_SR                   __ETHMAC_PHY_SR__

/*
 * PHY speed status 16-bit mask
- *
 * For DP83848: 0x0002
 * For LAN8700: 0x0004
 */
#define ETHMAC_PHY_SPEED_STATUS_BM      __ETHMAC_PHY_SPEED_STATUS_BM__

/*
 * PHY duplex status 16-bit mask
 *
 * For DP83848: 0x0004
 * For LAN8700: 0x0010
 */
#define ETHMAC_PHY_DUPLEX_STATUS_BM     __ETHMAC_PHY_DUPLEX_STATUS_BM__

/*
 * PHY link auto negotiation
 */
#define ETHMAC_PHY_AUTONEGOTIATION      __ETHMAC_PHY_AUTONEGOTIATION__

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
