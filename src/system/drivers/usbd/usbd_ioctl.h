/*=========================================================================*//**
@file    usbd_ioctl.h

@author  Daniel Zorychta

@brief   USB-Device module.

@note    Copyright (C) 2014  Daniel Zorychta <daniel.zorychta@gmail.com>

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
@defgroup drv-usbd USBD Driver

\section drv-usbd-desc Description
Driver handles USB Device peripheral. All data read from specified Endpoints
should be handled by application. Incoming data is controlled by host device
and slave device is responsible for correct handling USB protocol. There is no
possibility to send RAW data. To obtain more information please read USB protocol
specification. There are example applications that can be used to implement own
USB device:
\li src/programs/usbdevkbrd -- keyboard example
\li src/programs/usbdevmouse -- mouse example
\li src/programs/usbdevserial -- serial port example
\li src/programs/usbdevstorage -- storage device example

\section drv-usbd-sup-arch Supported architectures
\li stm32f1

\section drv-usbd-ddesc Details
\subsection drv-usbd-ddesc-num Meaning of major and minor numbers
The major number selects USB peripheral. The minor number selects USB Endpoint.

\subsubsection drv-usbd-ddesc-numres Numeration restrictions
Maximum major number depends on number of USB peripherals in microcontroller.
The minor number depends on count of USB Endpoints.

\subsection drv-usbd-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("USBD", 0, 0, "/dev/usb0_ep0");     // SETUP Endpoint
driver_init("USBD", 0, 1, "/dev/usb0_ep1");
driver_init("USBD", 0, 2, "/dev/usb0_ep2");
driver_init("USBD", 0, 3, "/dev/usb0_ep3");
driver_init("USBD", 0, 4, "/dev/usb0_ep4");
@endcode

\subsection drv-usbd-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("USBD", 0, 2);
driver_release("USBD", 0, 3);
@endcode

\subsection drv-usbd-ddesc-cfg Driver configuration
A moment when driver should be configured depends on USB protocol. User should
read commands from Endpoint 0 and do requested commands. One of first commands
is configuration request. To start USB peripheral one should send specified
command:
@code
ioctl(ep0, IOCTL_USBD__START);
@endcode
After this operation a USB host will send several commands. Commands should be
handled by application code.

\subsection drv-usbd-ddesc-write Data write
In most cases writing data to Endpoint is the same as writing to regular file.
One should keep in mind that data flow is controlled directly by host device and
selected device class. Writing is possible only when Endpoint is set to write
(regulated by device-type class). In this case driver is waiting for data to be
readout by host.

\subsection drv-usbd-ddesc-read Data read
In most cases reading data from Endpoint is the same as reading to regular file.
One should keep in mind that data flow is controlled directly by host device and
selected device class. Reading is possible only when Endpoint is set to read
(regulated by device-type class). IN this case driver is waiting for host data
write.

@{
*/

#ifndef _USBD_IOCTL_H_
#define _USBD_IOCTL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/ioctl_macros.h"
#include "usb_std.h"
#if defined(ARCH_stm32f1)
#include "stm32f1/usbd_cfg.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// TODO USBD module shall be more universal

/*==============================================================================
  Exported macros
==============================================================================*/
//------------------------------------------------------------------------------
// Constants related with USB module (do not edit)
//------------------------------------------------------------------------------
#define _USBD_NUMBER_OF_ENDPOINTS               8

#ifdef _USBD_ENDPOINT0_SIZE
#define USBD_EP0_SIZE                           _USBD_ENDPOINT0_SIZE
#else
#define USBD_EP0_SIZE                           8
#endif



//------------------------------------------------------------------------------
// Helper macros
//------------------------------------------------------------------------------
//==============================================================================
/**
 * @brief Macro set configuration IN for selected endpoint (an item of usb_ep_config_t)
 *
 * @param usbd_transfer_t__transfer      transfer type
 * @param u16_t__size                    size of buffer
 */
//==============================================================================
#define USBD_EP_CONFIG_IN(usbd_transfer_t__transfer, u16_t__size)\
        {.IN_enabled = true, .OUT_enabled = false, .transfer_type = usbd_transfer_t__transfer, .IN_buffer_size = u16_t__size, .OUT_buffer_size = 0}

//==============================================================================
/**
 * @brief Macro set configuration OUT for selected endpoint (an item of usb_ep_config_t)
 *
 * @param usbd_transfer_t__transfer      transfer type
 * @param u16_t__size                    size of buffer
 */
//==============================================================================
#define USBD_EP_CONFIG_OUT(usbd_transfer_t__transfer, u16_t__size)\
        {.IN_enabled = false, .OUT_enabled = true, .transfer_type = usbd_transfer_t__transfer, .IN_buffer_size = 0, .OUT_buffer_size = u16_t__size}

//==============================================================================
/**
 * @brief Macro set configuration IN & OUT for selected endpoint (an item of usb_ep_config_t)
 *
 * @param usbd_transfer_t__transfer      transfer type
 * @param u16_t__in_size                 size of IN buffer
 * @param u16_t__out_size                size of OUT buffer
 */
//==============================================================================
#define USBD_EP_CONFIG_IN_OUT(usbd_transfer_t__transfer, u16_t__in_size, u16_t__out_size)\
        {.IN_enabled = true, .OUT_enabled = true, .transfer_type = usbd_transfer_t__transfer, .IN_buffer_size = u16_t__in_size, .OUT_buffer_size = u16_t__out_size}

//==============================================================================
/**
 * @brief Macro disable selected endpoint (an item of usb_ep_config_t)
 */
//==============================================================================
#define USBD_EP_CONFIG_DISABLED()\
        {.IN_enabled = false, .OUT_enabled = false, .transfer_type = 0, .IN_buffer_size = 0, .OUT_buffer_size = 0}



//------------------------------------------------------------------------------
// IOCTL definitions
//------------------------------------------------------------------------------
//==============================================================================
/**
 * @brief The request starts the USB device (the device will be visible by the host).
 * @return On success 0 is returned, otherwise -1.
 */
//==============================================================================
#define IOCTL_USBD__START                       _IO (USBD, 0x00)

//==============================================================================
/**
 * @brief The request stops the USB device (the device will not be visible by the host).
 * @return On success 0 is returned, otherwise -1.
 */
//==============================================================================
#define IOCTL_USBD__STOP                        _IO (USBD, 0x01)

//==============================================================================
/**
 * @brief The request configures Endpoints 1 to 7. The settings of the Endpoint 0
 *        from given configuration are ignored.
 * @param [WR] usbd_ep_config_t endpoint configuration
 * @return On success 0 is returned, otherwise -1.
 */
//==============================================================================
#define IOCTL_USBD__CONFIGURE_EP_1_7            _IOW(USBD, 0x02, const usbd_ep_config_t*)

//==============================================================================
/**
 * @brief The request sets the address of the device (received from host in the SETUP packet).
 * @param [WR] uint16_t address
 * @return On success 0 is returned, otherwise -1.
 */
//==============================================================================
#define IOCTL_USBD__SET_ADDRESS                 _IOW(USBD, 0x03, uint16_t*)

//==============================================================================
/**
 * @brief The request sends the ZLP by the selected endpoint.
 * @return On success 0 is returned, otherwise -1.
 */
//==============================================================================
#define IOCTL_USBD__SEND_ZLP                    _IO (USBD, 0x04)

//==============================================================================
/**
 * @brief The request sets a STALL status in the selected endpoint.
 * @param [WR] The endpoint number determined by the 'uint16_t' type. The IN/OUT endpoint
 *        is recognized by most significant bit as is defined by the USB standard
 * @return On success 0 is returned, otherwise -1.
 */
//==============================================================================
#define IOCTL_USBD__SET_EP_STALL                _IOW(USBD, 0x05,  uint16_t*)

//==============================================================================
/**
 * @brief The request sets a VALID (ACK) status in the selected endpoint.
 * @param [WR] The endpoint number determined by the 'uint16_t' type. The IN/OUT endpoint
 *        is recognized by most significant bit as is defined by the USB standard.
 * @return On success 0 is returned, otherwise -1.
 */
//==============================================================================
#define IOCTL_USBD__SET_EP_VALID                _IOW(USBD, 0x06,  uint16_t*)

//==============================================================================
/**
 * @brief The request returns a USB reset status.
 * @param [RD] The pointer to the bool that indicate that reset was occurred.
 * @return On success 0 is returned, otherwise -1.
 */
//==============================================================================
#define IOCTL_USBD__WAS_RESET                   _IOR(USBD, 0x07, bool*)

//==============================================================================
/**
 * @brief The request wait for the SETUP packet.
 * @param [RD] The pointer to the usb_setup_container_t that contains setup packet buffer
 *        and read-timeout configuration.
 * @return On success 0 is returned, otherwise -1.
 */
//==============================================================================
#define IOCTL_USBD__GET_SETUP_PACKET            _IOR(USBD, 0x08, usbd_setup_container_t*)

//==============================================================================
/**
 * @brief The request sets the error status (IN STALL, OUT STALL) in the
 *        selected endpoint.
 * @return On success 0 is returned, otherwise -1.
 */
//==============================================================================
#define IOCTL_USBD__SET_ERROR_STATUS            _IO (USBD, 0x09)

//==============================================================================
/**
 * @brief The request gets the error counter. The counter is cleared after read operation.
 * @param [RD] The pointer to integer of 'uint16_t' type
 * @return On success 0 is returned, otherwise -1.
 */
//==============================================================================
#define IOCTL_USBD__GET_ERROR_COUNTER           _IOR(USBD, 0x0A, uint16_t*)

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Endpoint configuration structure.
 */
struct usbd_ep_config {
#ifdef DOXYGEN
        bool            IN_enabled;             /*!< IN enabled.*/
        bool            OUT_enabled;            /*!< OUT enabled.*/
        usb_transfer_t  transfer_type;          /*!< Transfer type.*/
        u16_t           IN_buffer_size;         /*!< IN buffer size.*/
        u16_t           OUT_buffer_size;        /*!< OUT buffer size.*/
#else
        bool            IN_enabled      : 1;    /*!< IN enabled.*/
        bool            OUT_enabled     : 1;    /*!< OUT enabled.*/
        usb_transfer_t  transfer_type   : 2;    /*!< Transfer type.*/
        u16_t           IN_buffer_size  : 10;   /*!< IN buffer size.*/
        u16_t           OUT_buffer_size : 10;   /*!< OUT buffer size.*/
#endif
};

/**
 * Endpoints configurations (the ENDP0 configuration is ignored).
 */
typedef struct {
        struct usbd_ep_config ep[_USBD_NUMBER_OF_ENDPOINTS];
} USBD_ep_config_t;

/**
 * SETUP packet container and receive timeout.
 */
typedef struct {
        usb_setup_packet_t packet;      /*!< Setup packet.*/
        int                timeout;     /*!< Timeout in milliseconds.*/
} USBD_setup_container_t;

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

#endif /* _USBD_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
