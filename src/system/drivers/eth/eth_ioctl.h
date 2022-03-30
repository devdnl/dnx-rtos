/*=========================================================================*//**
@file    ETH_ioctl.h

@author  Daniel Zorychta

@brief   This driver support Ethernet interface ioctl request codes.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/**
@defgroup drv-eth ETH Driver

\section drv-eth-desc Description
Driver handles Ethernet MAC peripheral. In general usage this driver is handled
directly by protocol stack.

\section drv-eth-sup-arch Supported architectures
\li stm32f1
\li stm32f4
\li stm32f7

\section drv-eth-ddesc Details
\subsection drv-eth-ddesc-num Meaning of major and minor numbers
The major number determines selection of Ethernet peripheral. Minor number
has no meaning and should be set to 0.

\subsection drv-eth-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("ETH", 0, 0, "/dev/eth0");
@endcode

\subsection drv-eth-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("ETH", 0, 0);
@endcode

\subsection drv-eth-ddesc-cfg Driver configuration
Entire driver configuration is realized by using configuration files in
the <tt>./config</tt> directory or by using Configtool.

\subsection drv-eth-ddesc-write Data write
Data to driver can be written as for regular file but if buffer is bigger than
MTU then buffer is send in few parts.

\subsection drv-eth-ddesc-read Data read
Data from driver can be received as regular file but with exception that
read data buffer should be a size of ETH_MAX_PACKET_SIZE.

\subsection drv-eth-ddesc-conf Peripheral start
There are only two steps that should be done to start Ethernet peripheral:

\li Ethernet device configuration (@ref IOCTL_ETH__CONFIGURE),
\li Ethernet peripheral start (@ref IOCTL_ETH__START).

After this operations packets will be received and transmitted.

\subsection drv-eth-ddesc-pkthdl Packet handling
To send and receive packets driver uses special data chains (@ref ETH_packet_t).
This object is a chain buffer that sum of all chain links payloads is a size of
entire packet. Object can contain only single packet. In both cases, receiving
and transmitting, the chain buffer should be created by user.

\subsubsection drv-eth-ddesc-pktrcv Packet receiving
Packet receive example code:
\code
// initialization
// ...

FILE *eth = fopen(...);

// ...

// reception thread
while (true) {
        // receive packet from peripheral buffer
        uint8_t buf[ETH_MAX_PACKET_SIZE];
        ETH_packet_t pkt;
        pkt.payload = buf;
        pkt.length  = sizeof(buf);         // maximal size

        if (ioctl(fileno(eth), IOCTL_ETH__RECEIVE_PACKET, &pkt) != 0) {
                // ... error handling
        }

        // ... received data handling
        // size_t len = pkt.length;     // real size of received packet
}
\endcode

\subsubsection drv-eth-ddesc-pktrans Packet transmitting
Packet sending example:
\code
// initialization
// ...

FILE *eth = fopen(...);

// ...

// transmit thread
while (true) {
        // ... data to send should be prepared earlier

        // buffer prepare - buffer can be created dynamically
        uint8_t buf[ETH_MAX_PACKET_SIZE];
        buf[0] = ...;
        buf[1] = ...;
        buf[...] = ...;

        ETH_packet_t pkt;
        pkt.payload = buf;
        pkt.payload_size = sizeof(buf);

        if (ioctl(fileno(eth), IOCTL_ETH__SEND_PACKET, &pkt) != 0) {
                // ... error handling
        }

        // ... packet transmitted
}
\endcode
@{
*/

#ifndef _ETH_IOCTL_H_
#define _ETH_IOCTL_H_

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
 * Max packet size
 */
#ifndef IOCTL_ETH_MAX_PACKET_SIZE
#define IOCTL_ETH_MAX_PACKET_SIZE               1524U
#endif

/**
 * @brief  Set MAC address.
 * @param  [WR] @ref ETH_config_t: pointer to configuration object.
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_ETH__CONFIGURE                    _IOW(ETH, 0x00, ETH_config_t*)

/**
 * @brief  Receive packet to chain buffer.
 * @param  [RD] @ref ETH_packet_t*       chain buffer reference (each chain must have allocated memory!).
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_ETH__RECEIVE_PACKET               _IOR(ETH, 0x01, ETH_packet_t*)

/**
 * @brief  Send packet from chain buffer.
 * @param  [WR] @ref ETH_packet_t*              chain buffer reference.
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_ETH__SEND_PACKET                  _IOW(ETH, 0x02, ETH_packet_t*)

/**
 * @brief  Starts Ethernet interface.
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_ETH__START                        _IO(ETH, 0x03)

/**
 * @brief  Stop Ethernet interface.
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_ETH__STOP                         _IO(ETH, 0x04)

/**
 * @brief  Return link status.
 * @param  [RD] @ref ETH_status_t*        link status.
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_ETH__GET_STATUS                   _IOR(ETH, 0x05, ETH_status_t*)

/**
 * @brief  Set RX timeout.
 * @param  [RD] @ref u32_t* rx timeout in milliseconds.
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_ETH__SET_RX_TIMEOUT               _IOW(ETH, 0x06, const u32_t*)

/**
 * @brief  Set RX timeout.
 * @param  [RD] @ref u32_t* tx timeout in milliseconds.
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_ETH__SET_TX_TIMEOUT               _IOW(ETH, 0x07, const u32_t*)

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Type represent packet chain.
 */
typedef struct {
        void  *payload;         /*!< Payload.*/
        u16_t  length;          /*!< Payload length.*/
} ETH_packet_t;

/**
 * Type represent ethernet status.
 */
typedef struct {
        enum {
                ETH_STATE__RESET,
                ETH_STATE__READY,
                ETH_STATE__ERROR,
        } state;

        enum {
                ETH_LINK_STATUS__DISCONNECTED,       /*!< Link disconnected.*/
                ETH_LINK_STATUS__CONNECTED,          /*!< Link connected.*/
                ETH_LINK_STATUS__PHY_ERROR,          /*!< PHY error.*/
        } link_status;

        enum ETH_speed {
                ETH_SPEED__10Mbps,
                ETH_SPEED__100Mbps,
        } speed;

        enum ETH_duplex {
                ETH_DUPLEX__HALF,
                ETH_DUPLEX__FULL,
        } duplex;

        bool  configured;
        u8_t  MAC[6];
        u64_t rx_packets;
        u64_t tx_packets;
        u64_t rx_bytes;
        u64_t tx_bytes;
        u32_t rx_dropped_frames;
} ETH_status_t;

/**
 * Type represent configuration.
 */
typedef struct {
        u8_t MAC[6];
        enum ETH_speed speed;
        enum ETH_duplex duplex;
        bool auto_negotiation;
} ETH_config_t;

/**
 * Type represent packet waiting with selected timeout.
 */
typedef struct {
        uint32_t wait_timeout;          /*!< Timeout value in milliseconds. Value is set by user at request.*/
        uint16_t packet_length;         /*!< Size of received packet. Value is set by driver at response.*/
} ETH_packet_wait_t;

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

#endif /* _ETH_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
