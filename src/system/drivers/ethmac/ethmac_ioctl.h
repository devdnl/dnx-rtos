/*=========================================================================*//**
@file    ethmac_ioctl.h

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
@defgroup drv-ethmac ETHMAC Driver

\section drv-ethmac-desc Description
Driver handles Ethernet MAC peripheral. In general usage this driver is handled
directly by protocol stack.

\section drv-ethmac-sup-arch Supported architectures
\li stm32f1
\li stm32f4

\section drv-ethmac-ddesc Details
\subsection drv-ethmac-ddesc-num Meaning of major and minor numbers
The major number determines selection of Ethernet peripheral. Minor number
has no meaning and should be set to 0.

\subsection drv-ethmac-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("ETHMAC", 0, 0, "/dev/eth0");
@endcode

\subsection drv-ethmac-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("ETHMAC", 0, 0);
@endcode

\subsection drv-ethmac-ddesc-cfg Driver configuration
Entire driver configuration is realized by using configuration files in
the <tt>./config</tt> directory or by using Configtool.

\subsection drv-ethmac-ddesc-write Data write
Data to driver can be written as for regular file but if buffer is bigger than
MTU then buffer is send is few parts. In general case this method is not
recommended to handle communication protocol. To handle driver more efficient
please use ioctl() family requests.

\subsection drv-ethmac-ddesc-read Data read
Data from driver can be received as from regular file but with exception that
read data buffer should be a size of ETH_MAX_PACKET_SIZE. In general case this
method is not recommended to handle communication protocol. To handle driver more
efficient please use ioctl() family requests.

\subsection drv-ethmac-ddesc-conf Peripheral start
There are only two steps that should be done to start Ethernet peripheral:

\li MAC address configuration (@ref IOCTL_ETHMAC__SET_MAC_ADDR),
\li Ethernet peripheral start (@ref IOCTL_ETHMAC__ETHERNET_START).

After this operations packets will be received and transmitted.

\subsection drv-ethmac-ddesc-pkthdl Packet handling
To send and receive packets driver uses special data chains (@ref ETHMAC_packet_chain_t).
This object is a chain buffer that sum of all chain links payloads is a size of
entire packet. Object can contain only single packet. In both cases, receiving
and transmitting, the chain buffer should be created by user.

There is a big advantage of placing packets in to chain buffers -- there is no
need to allocate a single packet buffer, thus payload data can originate from e.g.
different memories (RAM, ROM, etc). The chain buffer behave as simple one
direction linked list.

\subsubsection drv-ethmac-ddesc-pktrcv Packet receiving
To receive packet one should wait for reception of Ethernet peripheral. To check
this event the ioctl() request should be used: @ref IOCTL_ETHMAC__WAIT_FOR_PACKET.
User should specify the timeout value. If packet is received then
@ref ETHMAC_packet_wait_t object indicate a size of packet. This information should
be used to create specified @ref ETHMAC_packet_chain_t object. The object should
be prepared by application that receive packet, example:
\code
// initialization
// ...

FILE *eth = fopen(...);

// ...

// reception thread
while (true) {
        // wait for packet to be received
        ETHMAC_packet_wait_t wait = {.timeout  = MAX_DELAY_MS, .pkt_size = 0};

        if (ioctl(fileno(eth), IOCTL_ETHMAC__WAIT_FOR_PACKET, &wait) != 0) {
                // ... error handling
        }

        // receive packet from peripheral buffer
        ETHMAC_packet_chain_t bufch;
        bufch.next         = NULL;                     // there is single chain
        bufch.payload      = malloc(wait.pkt_size);    // allocate memory for packet
        bufch.total_size   = wait.pkt_size;            // total size, packet size
        bufch.payload_size = wait.pkt_size;            // one chain has size the same as total size

        if (ioctl(fileno(eth), IOCTL_ETHMAC__RECEIVE_PACKET_TO_CHAIN, &bufch) != 0) {
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

\subsubsection drv-ethmac-ddesc-pktrans Packet transmitting
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
        ETHMAC_packet_chain_t bufch;
        bufch.next         = NULL;        // there is single chain
        bufch.payload      = data_ptr;    // pointer to data buffer
        bufch.total_size   = data_len;    // total size - packet size
        bufch.payload_size = data_len;    // one chain has size the same as total size

        if (ioctl(fileno(eth), IOCTL_ETHMAC__SEND_PACKET_FROM_CHAIN, &bufch) != 0) {
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
 * @brief  Wait for receive of Rx packet.
 * @param  [WR,RD] @ref ETHMAC_packet_wait_t*        timeout value and received size.
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_ETHMAC__WAIT_FOR_PACKET                   _IOWR(ETHMAC, 0x00, ethmac_packet_wait_t*)

/**
 * @brief  Set MAC address.
 * @param  [WR] @ref u8_t[6]: pointer to buffer of 6 elements.
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_ETHMAC__SET_MAC_ADDR                      _IOW(ETHMAC, 0x01, u8_t*)

/**
 * @brief  Get MAC address.
 * @param  [RD] @ref u8_t[6]: pointer to buffer of 6 elements.
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_ETHMAC__GET_MAC_ADDR                      _IOR(ETHMAC, 0x02, u8_t*)

/**
 * @brief  Send packet from chain buffer.
 * @param  [WR] @ref ETHMAC_packet_chain_t*       chain buffer reference.
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_ETHMAC__SEND_PACKET                       _IOW(ETHMAC, 0x03, ethmac_packet_t*)

/**
 * @brief  Receive packet to chain buffer.
 * @param  [RD] @ref ETHMAC_packet_chain_t*       chain buffer reference (each chain must have allocated memory!).
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_ETHMAC__RECEIVE_PACKET                    _IOR(ETHMAC, 0x04, ethmac_packet_t*)

/**
 * @brief  Starts Ethernet interface.
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_ETHMAC__ETHERNET_START                    _IO(ETHMAC, 0x05)

/**
 * @brief  Stop Ethernet interface.
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_ETHMAC__ETHERNET_STOP                     _IO(ETHMAC, 0x06)

/**
 * @brief  Return link status.
 * @param  [RD] @ref ETHMAC_link_status_t*        link status.
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set.
 */
#define IOCTL_ETHMAC__GET_LINK_STATUS                   _IOR(ETHMAC, 0x07, ETHMAC_link_status_t*)

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Type represent packet chain.
 */
typedef struct ETHMAC_packet_chain {
        void  *payload;         /*!< Payload.*/
        u16_t  payload_size;    /*!< Payload size.*/
} ETHMAC_packet_t;

/**
 * Type represent link status.
 */
typedef enum {
        ETHMAC_LINK_STATUS__CONNECTED,          /*!< Link connected.*/
        ETHMAC_LINK_STATUS__DISCONNECTED        /*!< Link disconnected.*/
} ETHMAC_link_status_t;

/**
 * Type represent packet waiting with selected timeout.
 */
typedef struct {
        uint32_t timeout;    /*!< Timeout value in milliseconds. Value is set by user at request.*/
        size_t   pkt_size;   /*!< Size of received packet. Value is set by driver at response.*/
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
