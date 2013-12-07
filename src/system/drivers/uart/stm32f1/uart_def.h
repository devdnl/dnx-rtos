/*=========================================================================*//**
@file    usart_def.h

@author  Daniel Zorychta

@brief   This file support statuses and request of USART peripherals

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

#ifndef _UART_DEF_H_
#define _UART_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "core/ioctl_macros.h"
#include "stm32f1/uart_cfg.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/* UART minor number */
#define _UART_MINOR_NUMBER                              0

/* port names */
enum
{
        #if defined(RCC_APB2ENR_USART1EN) && (_UART1_ENABLE > 0)
        _UART1,
        #endif
        #if defined(RCC_APB1ENR_USART2EN) && (_UART2_ENABLE > 0)
        _UART2,
        #endif
        #if defined(RCC_APB1ENR_USART3EN) && (_UART3_ENABLE > 0)
        _UART3,
        #endif
        #if defined(RCC_APB1ENR_UART4EN) && (_UART4_ENABLE > 0)
        _UART4,
        #endif
        #if defined(RCC_APB1ENR_UART5EN) && (_UART5_ENABLE > 0)
        _UART5,
        #endif
        _UART_NUMBER
};

/* IO request for UART driver */
#define UART_IORQ_SET_CONFIGURATION                     _IOW('U', 0x00, struct UART_config*)
#define UART_IORQ_GET_CONFIGURATION                     _IOR('U', 0x01, struct UART_config*)
#define UART_IORQ_GET_CHAR_UNBLOCKING                   _IOR('U', 0x02, char*)

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
enum UART_parity {
        UART_PARITY_OFF,
        UART_PARITY_ODD,
        UART_PARITY_EVEN
};

enum UART_stop_bits {
        UART_STOP_BIT_1,
        UART_STOP_BIT_2
};

enum UART_LIN_break_length {
        UART_LIN_BREAK_10_BITS,
        UART_LIN_BREAK_11_BITS
};

struct UART_config {
        enum UART_parity                parity                  : 2;
        enum UART_stop_bits             stop_bits               : 1;
        enum UART_LIN_break_length      LIN_break_length        : 1;
        bool                            tx_enable               : 1;
        bool                            rx_enable               : 1;
        bool                            lin_mode_enable         : 1;
        bool                            hardware_flow_ctrl      : 1;
        u32_t                           baud                    : 24;
};

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
 Exported function prototypes
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _UART_DEF_H_ */
/*==============================================================================
  End of file
==============================================================================*/
