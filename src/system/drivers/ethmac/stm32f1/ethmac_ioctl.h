/*=========================================================================*//**
@file    ethmac_ioctl.h

@author  Daniel Zorychta

@brief   This driver support Ethernet interface ioctl request codes.

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

#ifndef _ETHMAC_IOCTL_H_
#define _ETHMAC_IOCTL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "core/ioctl_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/** @brief  Initialize Ethernet interface
 *  @param  None
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_ETH__ETHERNET_INIT                        _IO (_IO_GROUP_ETH, 0x00)

/** @brief  Gets Rx flag state
 *  @param  bool *
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_ETH__GET_RX_FLAG                          _IOR(_IO_GROUP_ETH, 0x01, bool*)

/** @brief  Clear Rx flag
 *  @param  None
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_ETH__CLEAR_RX_FLAG                        _IO (_IO_GROUP_ETH, 0x02)

/** @brief  Set MAC address
 *  @param  u8_t[6] (pointer to buffer)
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_ETH__SET_MAC_ADR                          _IOW(_IO_GROUP_ETH, 0x03, u8_t*)

/** @brief  Gets Rx packet size
 *  @param  u32_t *
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_ETH__GET_RX_PACKET_SIZE                   _IOR(_IO_GROUP_ETH, 0x04, u32_t*)

/** @brief  Gets Rx packet (chain mode)
 *  @param  struct ethmac_frame *
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_ETH__GET_RX_PACKET_CHAIN_MODE             _IOR(_IO_GROUP_ETH, 0x05, struct ethmac_frame*)

/** @brief  Gets Rx buffer unavailable status
 *  @param  bool *
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_ETH__GET_RX_BUFFER_UNAVAILABLE_STATUS     _IOR(_IO_GROUP_ETH, 0x06, bool*)

/** @brief  Clear Rx buffer unavailable flag
 *  @param  None
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_ETH__CLEAR_RX_BUFFER_UNAVAILABLE_STATUS   _IO (_IO_GROUP_ETH, 0x07)

/** @brief  Resume DMA reception
 *  @param  None
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_ETH__RESUME_DMA_RECEPTION                 _IO (_IO_GROUP_ETH, 0x08)

/** @brief  Set Tx frame length (chain mode)
 *  @param  int *
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_ETH__SET_TX_FRAME_LENGTH_CHAIN_MODE       _IOW(_IO_GROUP_ETH, 0x09, int*)

/** @brief  Gets current Tx buffer
 *  @param  u8_t ** (pointer to buffer)
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_ETH__GET_CURRENT_TX_BUFFER                _IOR(_IO_GROUP_ETH, 0x0A, u8_t**)

/** @brief  Initialize DMA Tx descriptor list (chain mode)
 *  @param  struct ethmac_DMA_description *
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_ETH__INIT_DMA_TX_DESC_LIST_CHAIN_MODE     _IOW(_IO_GROUP_ETH, 0x0B, struct ethmac_DMA_description*)

/** @brief  Initialize DMA Rx decsriptor list (chain mode)
 *  @param  struct ethmac_DMA_description *
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_ETH__INIT_DMA_RX_DESC_LIST_CHAIN_MODE     _IOW(_IO_GROUP_ETH, 0x0C, struct ethmac_DMA_description*)

/** @brief  Enable Rx interrupt
 *  @param  None
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_ETH__ENABLE_RX_IRQ                        _IO (_IO_GROUP_ETH, 0x0D)

/** @brief  Enable Tx hardware checksum
 *  @param  None
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_ETH__ENABLE_TX_HARDWARE_CHECKSUM          _IO (_IO_GROUP_ETH, 0x0E)

/** @brief  Starts Ethernet interface
 *  @param  None
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_ETH__ETHERNET_START                       _IO (_IO_GROUP_ETH, 0x0F)

/** @brief  Deinitialize Ethernet interface
 *  @param  None
 *  @return STD_RET_OK, STD_RET_ERROR
 */
#define IOCTL_ETH__ETHERNET_DEINIT                      _IO (_IO_GROUP_ETH, 0x10)

/*==============================================================================
  Exported object types
==============================================================================*/
struct ethmac_DMA_description {
        u8_t *buffer;
        u8_t  buffer_count;
};

struct ethmac_frame {
        u8_t *buffer;
        u32_t length;
};

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

#endif /* _ETHMAC_IOCTL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
