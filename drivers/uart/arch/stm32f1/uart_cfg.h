#ifndef UART_CFG_H_
#define UART_CFG_H_
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
/** user UART1 enable (1) or disable (0) */
#define UART_1_ENABLE                           1

/** user UART2 enable (1) or disable (0) */
#define UART_2_ENABLE                           0

/** user UART3 enable (1) or disable (0) */
#define UART_3_ENABLE                           0

/** user UART4 enable (1) or disable (0) */
#define UART_4_ENABLE                           0

/** user UART5 enable (1) or disable (0) */
#define UART_5_ENABLE                           0


/** UART wake method: idle line (0) or address mark (1) */
#define UART_DEFAULT_WAKE_METHOD                0

/** parity enable (1) or disable (0) */
#define UART_DEFAULT_PARITY_ENABLE              0

/** even parity (0) or odd parity (1) */
#define UART_DEFAULT_PARITY_SELECTION           0

/** disable (0) or enable (1) UART transmitter */
#define UART_DEFAULT_TX_ENABLE                  1

/** disable (0) or enable (1) UART receiver */
#define UART_DEFAULT_RX_ENABLE                  1

/** receiver wakeup: active mode (0) or mute mode (1) */
#define UART_DEFAULT_RX_WAKEUP_MODE             0

/** LIN mode disable (0) or enable (1) */
#define UART_DEFAULT_LIN_ENABLE                 0

/** 1 stop bit (0) or 2 stop bits (1) */
#define UART_DEFAULT_STOP_BITS                  0

/** LIN break detector length: 10 bits (0) or 11 bits (1) */
#define UART_DEFAULT_LIN_BREAK_LEN_DET          0

/** address of the USART node (in the multiprocessor mode), 4-bit length */
#define UART_DEFAULT_MULTICOM_ADDRESS           0

/** baud rate */
#define UART_DEFAULT_BAUDRATE                   115200

/** CTS hardware flow control enable (1) or disable (0) */
#define UART_DEFAULT_CTS_ENABLE                 0

/** RTS hardware flow control enable (1) or disable (0) */
#define UART_DEFAULT_RTS_ENABLE                 0


/** PCLK1 frequency */
#define UART_PCLK1_FREQ                         36000000UL

/** PCLK2 frequency */
#define UART_PCLK2_FREQ                         72000000UL

/** RX buffer size [B] */
#define UART_RX_BUFFER_SIZE                     128


/** uart part */
#define UART_PART_NONE                          0


/** port names */
enum UART_DEV_NUMBER
{
#if defined(RCC_APB2ENR_USART1EN) && (UART_1_ENABLE > 0)
        UART_DEV_1,
#endif

#if defined(RCC_APB1ENR_USART2EN) && (UART_2_ENABLE > 0)
        UART_DEV_2,
#endif

#if defined(RCC_APB1ENR_USART3EN) && (UART_3_ENABLE > 0)
        UART_DEV_3,
#endif

#if defined(RCC_APB1ENR_UART4EN) && (UART_4_ENABLE > 0)
        UART_DEV_4,
#endif

#if defined(RCC_APB1ENR_UART5EN) && (UART_5_ENABLE > 0)
        UART_DEV_5,
#endif

        UART_DEV_COUNT
};

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

#endif /* UART_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
