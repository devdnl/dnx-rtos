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

/**
 * @defgroup drv-ethmac Ethernet MAC Driver
 *
 * \section drv-ethmac-desc Description
 * Driver handles Ethernet MAC peripheral.
 *
 * \section drv-ethmac-sup-arch Supported architectures
 * \li STM32F10x (Connectivity line microcontrollers)
 *
 * \section drv-ethmac-ddesc Details
 * \subsection drv-ethmac-ddesc-num Meaning of major and minor numbers
 * The major number determines selection of Ethernet peripheral. Minor number
 * has no meaning and should be set to 0.
 *
 * \subsection drv-ethmac-ddesc-init Driver initialization
 * To initialize driver the following code can be used:
 *
 * @code
   driver_init("ETHMAC", 0, 0, "/dev/eth0");
   @endcode
 *
 * \subsection drv-ethmac-ddesc-release Driver release
 * To release driver the following code can be used:
 * @code
   driver_release("ETHMAC", 0, 0);
   @endcode
 *
 * \subsection drv-ethmac-ddesc-cfg Driver configuration
 * Entire driver configuration is realized by using configuration files in
 * the <tt>./config</tt> directory or by using Configtool.
 *
 * \subsection drv-ethmac-ddesc-write Data write
 * \todo Write access
 *
 * \subsection drv-ethmac-ddesc-read Data read
 * \todo Read access
 *
 * \subsection drv-ethmac-ddesc-pkthdl Packet handling
 * \todo Packet handling
 * @{
 */

#ifndef _ETHMAC_IOCTL_H_
#define _ETHMAC_IOCTL_H_

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
 * Size of Ethernet Packet.
 */
#define ETHMAC_PACKET_SIZE                              1520

/**
 * @brief  Wait for receive of Rx packet.
 * @param  [WR,RD] @ref ETHMAC_packet_wait_t*        timeout value and received size
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_ETHMAC__WAIT_FOR_PACKET                   _IOWR(ETHMAC, 0x00, ethmac_packet_wait_t*)

/**
 * @brief  Set MAC address.
 * @param  [WR] @ref u8_t[6]: pointer to buffer of 6 elements
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_ETHMAC__SET_MAC_ADDR                      _IOW(ETHMAC, 0x01, u8_t*)

/**
 * @brief  Send packet from chain buffer.
 * @param  [WR] @ref ETHMAC_packet_chain_t*       chain buffer reference
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_ETHMAC__SEND_PACKET_FROM_CHAIN            _IOW(ETHMAC, 0x02, ethmac_packet_chain_t*)

/**
 * @brief  Receive packet to chain buffer
 * @param  [RD] @ref ETHMAC_packet_chain_t*       chain buffer reference (each chain must have allocated memory!)
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_ETHMAC__RECEIVE_PACKET_TO_CHAIN           _IOR(ETHMAC, 0x03, ethmac_packet_chain_t*)

/**
 * @brief  Starts Ethernet interface
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_ETHMAC__ETHERNET_START                    _IO(ETHMAC, 0x04)

/**
 * @brief  Stop Ethernet interface
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_ETHMAC__ETHERNET_STOP                     _IO(ETHMAC, 0x05)

/**
 * @brief  Return link status
 * @param  [RD] @ref ETHMAC_link_status_t*        link status
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_ETHMAC__GET_LINK_STATUS                   _IOR(ETHMAC, 0x06, ethmac_link_status_t*)

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Type represent packet chain.
 */
typedef struct ETHMAC_packet_chain {
        struct ETHMAC_packet_chain *next;               //!< Next chain of payload.
        void                       *payload;            //!< Payload.
        u16_t                       total_size;         //!< Total size.
        u16_t                       payload_size;       //!< Payload size.
} ETHMAC_packet_chain_t;

/**
 * Type represent link status.
 */
typedef enum {
        ETHMAC_LINK_STATUS__CONNECTED,          //!< Link connected
        ETHMAC_LINK_STATUS__DISCONNECTED        //!< Link disconnected
} ETHMAC_link_status_t;

/**
 * Type represent packet waiting with selected timeout.
 */
typedef struct {
        int    timeout;    //!< Timeout value in milliseconds. Value is set by user at request.
        size_t pkt_size;   //!< Size of received packet. Value is set by system at response.
} ETHMAC_packet_wait_t;

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
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
