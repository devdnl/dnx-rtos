/*=========================================================================*//**
@file    usart_cfg.h

@author  Daniel Zorychta

@brief   This file support configuration of USART peripherals

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _UART_CFG_H_
#define _UART_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "stm32f1/stm32f10x.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/* UART1 enable (1) or disable (0) */
#define _UART1_ENABLE                           1

/* UART2 enable (1) or disable (0) */
#define _UART2_ENABLE                           1

/* UART3 enable (1) or disable (0) */
#define _UART3_ENABLE                           1

/* UART4 enable (1) or disable (0) */
#define _UART4_ENABLE                           1

/* UART5 enable (1) or disable (0) */
#define _UART5_ENABLE                           1

/* UART1 IRQ priority */
#define _UART1_IRQ_PRIORITY                     CONFIG_USER_IRQ_PRIORITY

/* UART1 IRQ priority */
#define _UART2_IRQ_PRIORITY                     CONFIG_USER_IRQ_PRIORITY

/* UART1 IRQ priority */
#define _UART3_IRQ_PRIORITY                     CONFIG_USER_IRQ_PRIORITY

/* UART1 IRQ priority */
#define _UART4_IRQ_PRIORITY                     CONFIG_USER_IRQ_PRIORITY

/* UART1 IRQ priority */
#define _UART5_IRQ_PRIORITY                     CONFIG_USER_IRQ_PRIORITY

/* RX buffer size [B] */
#define _UART_RX_BUFFER_SIZE                    128

/* UART default configuration */
#define _UART_DEFAULT_PARITY                    UART_PARITY_OFF
#define _UART_DEFAULT_STOP_BITS                 UART_STOP_BIT_1
#define _UART_DEFAULT_LIN_BREAK_LEN             UART_LIN_BREAK_10_BITS
#define _UART_DEFAULT_TX_ENABLE                 true
#define _UART_DEFAULT_RX_ENABLE                 true
#define _UART_DEFAULT_LIN_MODE_ENABLE           false
#define _UART_DEFAULT_HW_FLOW_CTRL              false
#define _UART_DEFAULT_BAUD                      115200

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
