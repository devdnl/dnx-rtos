/*=========================================================================*//**
@file    usb_ioctl.h

@author  Daniel Zorychta

@brief   USB-Device module.

@note    Copyright (C) 2014  Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _USB_IOCTL_H_
#define _USB_IOCTL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "core/ioctl_macros.h"
#include "usb_std.h"
#include "usb_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
//------------------------------------------------------------------------------
// Constants related with USB module (do not edit)
//------------------------------------------------------------------------------
#define _USB_NUMBER_OF_ENDPOINTS                8
#define USB_EP0_SIZE                            _USB_ENDPOINT0_SIZE



//------------------------------------------------------------------------------
// Helper macros
//------------------------------------------------------------------------------
//==============================================================================
/**
 * @brief Macro set configuration IN for selected endpoint (an item of usb_ep_config_t)
 *
 * @param transfer      transfer type
 * @param size          size of buffer
 */
//==============================================================================
#define USB_EP_CONFIG_IN(usb_transfer_t__transfer, u16_t__size)\
        {.IN_enabled = true, .OUT_enabled = false, .transfer_type = usb_transfer_t__transfer, .IN_buffer_size = u16_t__size, .OUT_buffer_size = 0}

//==============================================================================
/**
 * @brief Macro set configuration OUT for selected endpoint (an item of usb_ep_config_t)
 *
 * @param transfer      transfer type
 * @param size          size of buffer
 */
//==============================================================================
#define USB_EP_CONFIG_OUT(usb_transfer_t__transfer, u16_t__size)\
        {.IN_enabled = false, .OUT_enabled = true, .transfer_type = usb_transfer_t__transfer, .IN_buffer_size = 0, .OUT_buffer_size = u16_t__size}

//==============================================================================
/**
 * @brief Macro set configuration IN & OUT for selected endpoint (an item of usb_ep_config_t)
 *
 * @param transfer      transfer type
 * @param size          size of buffer
 */
//==============================================================================
#define USB_EP_CONFIG_IN_OUT(usb_transfer_t__transfer, u16_t__in_size, u16_t__out_size)\
        {.IN_enabled = true, .OUT_enabled = true, .transfer_type = usb_transfer_t__transfer, .IN_buffer_size = u16_t__in_size, .OUT_buffer_size = u16_t__out_size}

//==============================================================================
/**
 * @brief Macro disable selected endpoint (an item of usb_ep_config_t)
 */
//==============================================================================
#define USB_EP_CONFIG_DISABLED()\
        {.IN_enabled = false, .OUT_enabled = false, .transfer_type = 0, .IN_buffer_size = 0, .OUT_buffer_size = 0}



//------------------------------------------------------------------------------
// IOCTL definitions
//------------------------------------------------------------------------------
//==============================================================================
/**
 * @brief The request starts the USB device (the device will be visible by the host)
 * @param None
 * @return On success returns STD_RET_OK, otherwise STD_RET_ERROR
 */
//==============================================================================
#define IOCTL_USB__START                        _IO (_IO_GROUP_USB, 0x00)

//==============================================================================
/**
 * @brief The request stops the USB device (the device will not be visible by the host)
 * @param None
 * @return On success returns STD_RET_OK, otherwise STD_RET_ERROR
 */
//==============================================================================
#define IOCTL_USB__STOP                         _IO (_IO_GROUP_USB, 0x01)

//==============================================================================
/**
 * @brief The request configures Endpoints 1 to 7. The settings of the Endpoint 0 from given configuration are ignored.
 * @param The pointer to the object of type 'const usb_ep_config_t'
 * @return On success returns STD_RET_OK, otherwise STD_RET_ERROR
 */
//==============================================================================
#define IOCTL_USB__CONFIGURE_EP_1_7             _IOW(_IO_GROUP_USB, 0x02, const usb_ep_config_t*)

//==============================================================================
/**
 * @brief The request sets the address of the device (received from host in the SETUP packet).
 * @param The address determined by the 'int' type
 * @return On success returns STD_RET_OK, otherwise STD_RET_ERROR
 */
//==============================================================================
#define IOCTL_USB__SET_ADDRESS                  _IOW(_IO_GROUP_USB, 0x03, int)

//==============================================================================
/**
 * @brief The request sends the ZLP by the selected endpoint
 * @param None
 * @return On success returns STD_RET_OK, otherwise STD_RET_ERROR
 */
//==============================================================================
#define IOCTL_USB__SEND_ZLP                     _IO (_IO_GROUP_USB, 0x04)

//==============================================================================
/**
 * @brief The request sets a STALL status in the selected endpoint
 * @param The endpoint number determined by the 'int' type. The IN/OUT endpoint
 *        is recognized by most significant bit as is defined by the USB standard
 * @return On success returns STD_RET_OK, otherwise STD_RET_ERROR
 */
//==============================================================================
#define IOCTL_USB__SET_EP_STALL                 _IOW(_IO_GROUP_USB, 0x05, int)

//==============================================================================
/**
 * @brief The request sets a VALID (ACK) status in the selected endpoint
 * @param The endpoint number determined by the 'int' type. The IN/OUT endpoint
 *        is recognized by most significant bit as is defined by the USB standard
 * @return On success returns STD_RET_OK, otherwise STD_RET_ERROR
 */
//==============================================================================
#define IOCTL_USB__SET_EP_VALID                 _IOW(_IO_GROUP_USB, 0x06, int)

//==============================================================================
/**
 * @brief The request returns a USB reset status
 * @param None
 * @return On success returns true or false, otherwise STD_RET_ERROR
 */
//==============================================================================
#define IOCTL_USB__WAS_RESET                    _IO (_IO_GROUP_USB, 0x07)

//==============================================================================
/**
 * @brief The request wait for the SETUP packet
 * @param The pointer to the usb_setup_container_t that contains setup packet buffer
 *        and read-timeout configuration
 * @return On success returns STD_RET_OK, otherwise STD_RET_ERROR
 */
//==============================================================================
#define IOCTL_USB__GET_SETUP_PACKET             _IOR(_IO_GROUP_USB, 0x08, usb_setup_container_t*)

//==============================================================================
/**
 * @brief The request sets the error status (IN STALL, OUT STALL) in the selected endpoint
 * @param None
 * @return On success returns STD_RET_OK, otherwise STD_RET_ERROR
 */
//==============================================================================
#define IOCTL_USB__SET_ERROR_STATUS             _IO (_IO_GROUP_USB, 0x09)

//==============================================================================
/**
 * @brief The request gets the error counter. The counter is cleared after read operation.
 * @param The pointer to integer of 'int' type
 * @return On success returns STD_RET_OK, otherwise STD_RET_ERROR
 */
//==============================================================================
#define IOCTL_USB__GET_ERROR_COUNTER            _IOR(_IO_GROUP_USB, 0x0A, int*)

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Endpoint configuration (the ENDP0 configuration is ignored)
 */
typedef struct {
        struct usb_ep_config {
                bool            IN_enabled      : 1;
                bool            OUT_enabled     : 1;
                usb_transfer_t  transfer_type   : 2;
                u16_t           IN_buffer_size  : 10;
                u16_t           OUT_buffer_size : 10;
        } ep[_USB_NUMBER_OF_ENDPOINTS];
} usb_ep_config_t;

/**
 * SETUP packet container and receive timeout
 */
typedef struct {
        usb_setup_packet_t packet;
        int                timeout;
} usb_setup_container_t;

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

#endif /* _USB_IOCTL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
