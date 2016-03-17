/*=========================================================================*//**
@file    uart_ioctl.h

@author  Daniel Zorychta

@brief   This file support UART ioctl request codes.

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
 * @defgroup drv-uart UART Driver
 *
 * \section drv-uart-desc Description
 * Driver handles UART peripheral.
 *
 * \section drv-uart-sup-arch Supported architectures
 * \li STM32F10x
 *
 * @{
 */

#ifndef _UART_IOCTL_H_
#define _UART_IOCTL_H_

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
 *  @brief  Set UART configuration.
 *  @param  [WR] struct @ref UART_config * UART peripheral configuration
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_UART__SET_CONFIGURATION           _IOW(UART, 0x00, struct UART_config*)

/**
 *  @brief  Gets UART configuration.
 *  @param  [RD] struct @ref UART_config * UART peripheral configuration
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_UART__GET_CONFIGURATION           _IOR(UART, 0x01, struct UART_config*)

/**
 *  @brief  Gets character from UART in non-blocking mode.
 *  @param  [RD] char * read character
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_UART__GET_CHAR_UNBLOCKING         _IOR(UART, 0x02, char*)

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Type represent parity configuration.
 */
enum UART_parity {
        UART_PARITY__OFF,                       //!< Parity disabled.
        UART_PARITY__ODD,                       //!< Odd parity enabled.
        UART_PARITY__EVEN                       //!< Even parity enabled.
};

/**
 * Type represent stop bit configuration.
 */
enum UART_stop_bit {
        UART_STOP_BIT__1,                       //!< One stop bit.
        UART_STOP_BIT__2                        //!< Two stop bit.
};

/**
 * Type represent LIN break mode configuration.
 */
enum UART_LIN_break {
        UART_LIN_BREAK__10_BITS,                //!< Break condition 10 bits long.
        UART_LIN_BREAK__11_BITS                 //!< Break condition 11 bits long.
};

/**
 * Type represent UART configuration.
 */
struct UART_config {
        enum UART_parity    parity;             //!< Parity configuration.
        enum UART_stop_bit  stop_bits;          //!< Stop bits configuration.
        enum UART_LIN_break LIN_break_length;   //!< LIN break length.
        bool                tx_enable;          //!< Tx channel enable.
        bool                rx_enable;          //!< Rx channel enable.
        bool                LIN_mode_enable;    //!< LIN mode enable.
        bool                hardware_flow_ctrl; //!< Hardware flow control enable (RTS, CTS).
        bool                single_wire_mode;   //!< Single wire mode enable.
        u32_t               baud;               //!< Baudrate.
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

#endif /* _UART_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
