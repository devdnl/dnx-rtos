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
MTU then buffer is send is few parts. In general case this method is not
recommended to handle communication protocol. To handle driver more efficient
please use ioctl() family requests.

\subsection drv-eth-ddesc-read Data read
Data from driver can be received as from regular file but with exception that
read data buffer should be a size of ETH_MAX_PACKET_SIZE. In general case this
method is not recommended to handle communication protocol. To handle driver more
efficient please use ioctl() family requests.

\subsection drv-eth-ddesc-conf Peripheral start
There are only two steps that should be done to start Ethernet peripheral:

\li MAC address configuration (@ref IOCTL_ETH__SET_MAC_ADDR),
\li Ethernet peripheral start (@ref IOCTL_ETH__START).

After this operations packets will be received and transmitted.

\subsection drv-eth-ddesc-pkthdl Packet handling
To send and receive packets driver uses special data chains (@ref ETH_packet_t).
This object is a chain buffer that sum of all chain links payloads is a size of
entire packet. Object can contain only single packet. In both cases, receiving
and transmitting, the chain buffer should be created by user.

There is a big advantage of placing packets in to chain buffers -- there is no
need to allocate a single packet buffer, thus payload data can originate from e.g.
different memories (RAM, ROM, etc). The chain buffer behave as simple one
direction linked list.

\subsubsection drv-eth-ddesc-pktrcv Packet receiving
To receive packet one should wait for reception of Ethernet peripheral. To check
this event the ioctl() request should be used: @ref IOCTL_ETH__WAIT_FOR_PACKET.
User should specify the timeout value. If packet is received then
@ref ETH_packet_wait_t object indicate a size of packet. This information should
be used to create specified @ref ETH_packet_t object. The object should
be prepared by application that receive packet, example:
\code
// initialization
// ...

FILE *eth = fopen(...);

// ...

// reception thread
while (true) {
        // wait for packet to be received
        ETH_packet_wait_t wait = {.timeout  = MAX_DELAY_MS, .pkt_size = 0};

        if (ioctl(fileno(eth), IOCTL_ETH__WAIT_FOR_PACKET, &wait) != 0) {
                // ... error handling
        }

        // receive packet from peripheral buffer
        uint8_t buf[1520];
        ETH_packet_t pkt;
        pkt.payload = buf;
        pkt.payload_size = sizeof(buf);

        if (ioctl(fileno(eth), IOCTL_ETH__RECEIVE_PACKET, &pkt) != 0) {
                // ... error handling
        }

        // ... received data handling

        free(bufch.payload); // can be allocated only one time below the loop.
}
\endcode

The input buffer can be created by using many chain links and the total chain
size can be much bigger than received packet. The driver will fill the chain
buffer by packet data and will set correct total size value. Only the first
chain link has updated the total_size value.

\subsubsection drv-eth-ddesc-pktrans Packet transmitting
To transmit packet the chain buffer should be used. The operation is opposite to
packet receive but data sending is handled by the same chain buffer object type.
The packet can be divided to many small parts. Packet sending example:
\code
// initialization
// ...

FILE *eth = fopen(...);

// ...

// transmit thread
while (true) {
        // ... data to send should be prepared earlier

        // buffer prepare - buffer can be created dynamically
        uint8_t buf[1520];
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

The best solution is to create specified packets dynamically at runtime. When
data successively is added to chain buffer then new payload chains can be
allocated and added to the buffer. The single chain will contain only small part
of data. One should keep in mind that total_size field in first chain should be
updated when new chain link is added, this field in other chain links is
ignored by driver.
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
 * @brief  Set MAC address.
 * @param  [WR] @ref ETH_config_t: pointer to configuration object.
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_ETH__CONFIGURE                    _IOW(ETH, 0x00, ETH_config_t*)

/**
 * @brief  Wait for receive of Rx packet.
 * @param  [WR,RD] @ref ETH_packet_wait_t*        timeout value and received size.
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_ETH__WAIT_FOR_PACKET              _IOWR(ETH, 0x01, ETH_packet_wait_t*)

/**
 * @brief  Receive packet to chain buffer.
 * @param  [RD] @ref ETH_packet_t*       chain buffer reference (each chain must have allocated memory!).
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_ETH__RECEIVE_PACKET               _IOR(ETH, 0x02, ETH_packet_t*)

/**
 * @brief  Send packet from chain buffer.
 * @param  [WR] @ref ETH_packet_t*              chain buffer reference.
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_ETH__SEND_PACKET                  _IOW(ETH, 0x03, ETH_packet_t*)

/**
 * @brief  Starts Ethernet interface.
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_ETH__START                        _IO(ETH, 0x04)

/**
 * @brief  Stop Ethernet interface.
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_ETH__STOP                         _IO(ETH, 0x05)

/**
 * @brief  Return link status.
 * @param  [RD] @ref ETH_status_t*        link status.
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_ETH__GET_STATUS                   _IOR(ETH, 0x06, ETH_status_t*)

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Type represent packet chain.
 */
typedef struct {
        void  *payload;         /*!< Payload.*/
        u16_t  lenght;          /*!< Payload size.*/
} ETH_packet_t;

/**
 * Type represent configuration.
 */
typedef struct {
        u8_t MAC[6];
} ETH_config_t;

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
                ETH_LINK_STATUS__CONNECTED,          /*!< Link connected.*/
                ETH_LINK_STATUS__DISCONNECTED        /*!< Link disconnected.*/
        } link_status;

        u8_t  MAC[6];
        u64_t rx_packets;
        u64_t tx_packets;
        u64_t rx_bytes;
        u64_t tx_bytes;
        u32_t rx_missed_frames_mfa;
        u32_t rx_missed_frames_mfc;
} ETH_status_t;

/**
 * Type represent packet waiting with selected timeout.
 */
typedef struct {
        uint32_t timeout;    /*!< Timeout value in milliseconds. Value is set by user at request.*/
        size_t   pkt_size;   /*!< Size of received packet. Value is set by driver at response.*/
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
