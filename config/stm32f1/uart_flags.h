/*=========================================================================*//**
@file    uart_flags.h

@author  Daniel Zorychta

@brief   UART module configuration flags.

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

/*
 * NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as suffix
 *       and prefix) are exported to the single configuration file
 *       (by using Configtool) when entire project configuration is exported.
 *       All other flag definitions and statements are ignored.
 */

#ifndef _UART_FLAGS_H_
#define _UART_FLAGS_H_

#define __UART_UART1_ENABLE__ _YES_
#define __UART_UART2_ENABLE__ _NO_
#define __UART_UART3_ENABLE__ _NO_
#define __UART_UART4_ENABLE__ _NO_
#define __UART_UART5_ENABLE__ _NO_
#define __UART_UART1_PRIORITY__ CONFIG_USER_IRQ_PRIORITY
#define __UART_UART2_PRIORITY__ CONFIG_USER_IRQ_PRIORITY
#define __UART_UART3_PRIORITY__ CONFIG_USER_IRQ_PRIORITY
#define __UART_UART4_PRIORITY__ CONFIG_USER_IRQ_PRIORITY
#define __UART_UART5_PRIORITY__ CONFIG_USER_IRQ_PRIORITY
#define __UART_RX_BUFFER_LEN__ 128
#define __UART_DEFAULT_PARITY__ UART_PARITY_OFF
#define __UART_DEFAULT_STOP_BITS__ UART_STOP_BIT_1
#define __UART_DEFAULT_LIN_BREAK_LEN__ UART_LIN_BREAK_10_BITS
#define __UART_DEFAULT_TX_ENABLE__ _YES_
#define __UART_DEFAULT_RX_ENABLE__ _YES_
#define __UART_DEFAULT_LIN_MODE_ENABLE__ _NO_
#define __UART_DEFAULT_HW_FLOW_CTRL__ _NO_
#define __UART_DEFAULT_SINGLE_WIRE_MODE__ _NO_
#define __UART_DEFAULT_BAUD__ 115200

#endif /* _UART_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
