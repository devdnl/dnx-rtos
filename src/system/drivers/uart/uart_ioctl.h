/*=========================================================================*//**
@file    uart_ioctl.h

@author  Daniel Zorychta

@brief   This file support UART ioctl request codes.

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
@defgroup drv-uart UART Driver

\section drv-uart-desc Description
Driver handles UART peripheral.

\section drv-uart-sup-arch Supported architectures
\li stm32f1
\li stm32f4
\li stm32f7
\li efr32

\section drv-uart-ddesc Details
\subsection drv-uart-ddesc-num Meaning of major and minor numbers
Some manufactures enumerate devices starting from 1 instead of 0 (e.g. ST).
In this case major number starts from 0 and is connected to the first device
e.g. UART1.
\arg major number selects UART peripheral e.g.:@n
     major = 0 -> UART0 (or UART1 if numerated from 1)

The minor number has no meaning and should be always set to 0.

\subsubsection drv-uart-ddesc-numres Numeration restrictions
Number of peripherals determines how big major number can be. If there is
only one UART peripheral then the major number is always 0. The minor number
should be always set to 0.

\subsection drv-uart-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("UART", 0, 0, "/dev/ttyS0");        // UART0
driver_init("UART", 1, 0, "/dev/ttyS1");        // UART1
@endcode

\subsection drv-uart-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("UART", 0, 0);
driver_release("UART", 1, 0);
@endcode

\subsection drv-uart-ddesc-cfg Driver configuration
The default driver values should be configured in project configuration (Configtool).
Runtime configuration should be always done at system startup or later in
application. In this case the ioctl() function should be used and UART_config_t object.

\subsection drv-uart-ddesc-write Data write
Writing data to device is the same as writing data to regular file.
File position is ignored because device handle stream.

\subsection drv-uart-ddesc-read Data read
Reading data from device is the same as reading data from regular file. Driver
support not blocking character read by using ioctl() function (by using following
requests: @ref IOCTL_UART__GET_CHAR_UNBLOCKING or @ref IOCTL_VFS__NON_BLOCKING_RD_MODE
with fread() function). File position is ignored because device handle stream.

@{
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
 *  @param  [WR] struct @ref UART_config_t * UART peripheral configuration
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_UART__SET_CONFIGURATION           _IOW(UART, 0x00, struct UART_config*)

/**
 *  @brief  Gets UART configuration.
 *  @param  [RD] struct @ref UART_config_t * UART peripheral configuration
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
typedef struct UART_config {
        enum UART_parity    parity;             /*!< Parity configuration.*/
        enum UART_stop_bit  stop_bits;          /*!< Stop bits configuration.*/
        enum UART_LIN_break LIN_break_length;   /*!< LIN break length.*/
        bool                tx_enable;          /*!< Tx channel enable.*/
        bool                rx_enable;          /*!< Rx channel enable.*/
        bool                LIN_mode_enable;    /*!< LIN mode enable.*/
        bool                hardware_flow_ctrl; /*!< Hardware flow control enable (RTS, CTS).*/
        bool                single_wire_mode;   /*!< Single wire mode enable.*/
        u32_t               baud;               /*!< Baudrate.*/
} UART_config_t;

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
