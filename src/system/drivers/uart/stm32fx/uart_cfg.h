/*=========================================================================*//**
@file    usart_cfg.h

@author  Daniel Zorychta

@brief   This file support configuration of USART peripherals

@note    Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _UART_CFG_H_
#define _UART_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/* RX buffer size [B] */
#define _UART_RX_BUFFER_SIZE                    __UART_RX_BUFFER_LEN__

/* UART default configuration */
#define _UART_DEFAULT_PARITY                    __UART_DEFAULT_PARITY__
#define _UART_DEFAULT_STOP_BITS                 __UART_DEFAULT_STOP_BITS__
#define _UART_DEFAULT_LIN_BREAK_LEN             __UART_DEFAULT_LIN_BREAK_LEN__
#define _UART_DEFAULT_TX_ENABLE                 __UART_DEFAULT_TX_ENABLE__
#define _UART_DEFAULT_RX_ENABLE                 __UART_DEFAULT_RX_ENABLE__
#define _UART_DEFAULT_LIN_MODE_ENABLE           __UART_DEFAULT_LIN_MODE_ENABLE__
#define _UART_DEFAULT_HW_FLOW_CTRL              __UART_DEFAULT_HW_FLOW_CTRL__
#define _UART_DEFAULT_SINGLE_WIRE_MODE          __UART_DEFAULT_SINGLE_WIRE_MODE__
#define _UART_DEFAULT_BAUD                      __UART_DEFAULT_BAUD__
#define _UART_IRQ_PRIORITY                      __UART_IRQ_PRIORITY__

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _UART_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
