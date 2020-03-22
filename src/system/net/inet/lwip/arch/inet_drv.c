/*=========================================================================*//**
@file    inet_drv.c

@author  Daniel Zorychta

@brief   Network manager. Driver handling.

@note    Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Include files
==============================================================================*/
#include <string.h>
#include "inet_types.h"
#include "kernel/sysfunc.h"
#include "drivers/ioctl_requests.h"

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief  Function initializes hardware interface.
 *         The function should be used to initialize interface parameters like
 *         MAC address, etc. By using this function the network interface should
 *         be started.
 *
 * @param  inet         inet container
 *
 * @return One of @ref errno value.
 *
 * @note   Called from network interface thread.
 */
//==============================================================================
int _inetdrv_hardware_init(inet_t *inet)
{
        /* set MAC address */
        int err = sys_ioctl(inet->if_file, IOCTL_ETHMAC__GET_MAC_ADDR, inet->netif.hwaddr);
        if (err) {
                LWIP_DEBUGF(LWIP_DBG_LEVEL_SERIOUS, ("_inetdrv_hardware_init: MAC set fail\n"));
                return err;
        }

        /* start Ethernet interface */
        err = sys_ioctl(inet->if_file, IOCTL_ETHMAC__ETHERNET_START);
        if (err) {
                LWIP_DEBUGF(LWIP_DBG_LEVEL_SERIOUS, ("_inetdrv_hardware_init: start fail\n"));
                return err;
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function de-initialize configured network interface.
 *
 * @param  inet         inet container
 *
 * @return One of @ref errno value.
 *
 * @note   Called from network interface thread.
 */
//==============================================================================
int _inetdrv_hardware_deinit(inet_t *inet)
{
        int err = sys_ioctl(inet->if_file, IOCTL_ETHMAC__ETHERNET_STOP);
        if (err) {
                LWIP_DEBUGF(LWIP_DBG_LEVEL_SERIOUS, ("_inetdrv_hardware_deinit: stop fail\n"));
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function receive packet from the network interface.
 *         This function should receive incoming packet from network interface.
 *         Function should try receive packet by specified time passed by
 *         'input_timeout'. If function does not receive any packet due to
 *         specified time then function should exit.
 *         If packet was received then the function should allocate buffer for
 *         transfer by using pbuf_alloc() function and put this buffer to then
 *         TCPIP stack by using inet->netif.input() function. If packets are
 *         received every loop then the function should not exit.
 *
 * @param  inet                 inet container
 * @param  input_timeout        packet receive timeout
 *
 * @note   Called from network interface thread.
 */
//==============================================================================
void _inetdrv_handle_input(inet_t *inet, u32_t timeout)
{
        ETHMAC_packet_wait_t pw = {.timeout = timeout};
        int r = sys_ioctl(inet->if_file, IOCTL_ETHMAC__WAIT_FOR_PACKET, &pw);

        while (r == 0 && pw.pkt_size > 0) {
                // NOTE: subtract packet size by 4 to discard CRC32
                struct pbuf *p = pbuf_alloc(PBUF_RAW, pw.pkt_size - 4, PBUF_RAM);
                if (p) {
                        ETHMAC_packet_t pkt;
                        pkt.payload = p->payload;
                        pkt.payload_size = p->len;

                        r = sys_ioctl(inet->if_file, IOCTL_ETHMAC__RECEIVE_PACKET, &pkt);

                        if (r == 0) {
                                LWIP_DEBUGF(INET_DEBUG, ("_inetdrv_handle_input: received = %d\n", p->tot_len));

                                inet->rx_packets++;
                                inet->rx_bytes += p->tot_len;

                                if (inet->netif.input(p, &inet->netif) != ERR_OK) {
                                        pbuf_free(p);
                                }
                        } else {
                                LWIP_DEBUGF(INET_DEBUG, ("_inetdrv_handle_input: receive error\n"));
                                pbuf_free(p);
                        }

                        r = sys_ioctl(inet->if_file, IOCTL_ETHMAC__WAIT_FOR_PACKET, &pw);
                } else {
                        LWIP_DEBUGF(INET_DEBUG, ("_inetdrv_handle_input: not enough free memory\n"));
                        sys_sleep_ms(10);
                }
        }

        LWIP_DEBUGF(INET_DEBUG, ("_inetdrv_handle_input: packet receive timeout\n"));
}

//==============================================================================
/**
 * @brief  This function should do the actual transmission of the packet.
 *         The packet is contained in the pbuf that is passed to the function.
 *         This pbuf might be chained. To get access to the network manager
 *         object, use netif->state pointer.
 *
 * @param  netif        the lwip network interface structure
 * @param  p            the MAC packet to send (e.g. IP packet including MAC addresses and type)
 *
 * @return ERR_OK if the packet could be sent an err_t value if the packet couldn't be sent.
 *
 * @note   Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *         strange results. You might consider waiting for space in the DMA queue
 *         to become available since the stack doesn't retry to send a packet
 *         dropped because of memory failure (except for the TCP timers).
 *
 * @note   Called from TCPIP thread.
 */
//==============================================================================
err_t _inetdrv_handle_output(struct netif *netif, struct pbuf *p)
{
      inet_t *inet = netif->state;

      LWIP_DEBUGF(INET_DEBUG, ("_inetdrv_handle_output: packet size %d\n", p->tot_len));

      ETHMAC_packet_t pkt;
      pkt.payload = p->payload;
      pkt.payload_size = p->len;

      if (sys_ioctl(inet->if_file, IOCTL_ETHMAC__SEND_PACKET, &pkt) == 0) {
              inet->tx_packets++;
              inet->tx_bytes += pkt.payload_size;
              return ERR_OK;
      } else {
              LWIP_DEBUGF(LWIP_DBG_LEVEL_SERIOUS, ("_inetdrv_handle_output: packet send error\n"));
              return ERR_IF;
      }
}

//==============================================================================
/**
 * @brief  Function check state of the network connection.
 *         The function is called when packet was not received (by timeout).
 *         The function should return a status of the physical network connection.
 *         If link is disconnected then function should return false, if link
 *         is connected then should return true.
 *
 * @param  inet                 inet container
 *
 * @return If link is connected then true is returned, otherwise false.
 *
 * @note   Called at system startup.
 */
//==============================================================================
bool _inetdrv_is_link_connected(inet_t *inet)
{
        ETHMAC_link_status_t linkstat;
        if (sys_ioctl(inet->if_file, IOCTL_ETHMAC__GET_LINK_STATUS, &linkstat) == 0) {
                return linkstat == ETHMAC_LINK_STATUS__CONNECTED;
        } else {
                return false;
        }
}

/*==============================================================================
  End of file
==============================================================================*/
