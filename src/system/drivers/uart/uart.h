/*=========================================================================*//**
@file    uart.h

@author  Daniel Zorychta

@brief   This file support UART peripheral.

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

#ifndef _UART_H_
#define _UART_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "uart_ioctl.h"
#include "gpio/gpio_ddi.h"
#include "gpio_ioctl.h"

#if defined(ARCH_stm32f1)
#include "stm32f1/stm32f10x.h"
#include "stm32f1/lib/stm32f10x_rcc.h"
#elif defined(ARCH_stm32f3)
#include "stm32f3/stm32f3xx.h"
#include "stm32f3/lib/stm32f3xx_ll_rcc.h"
#elif defined(ARCH_stm32f4)
#include "stm32f4/stm32f4xx.h"
#include "stm32f4/lib/stm32f4xx_rcc.h"
#elif defined(ARCH_stm32f7)
#include "stm32f7/stm32f7xx.h"
#include "stm32f7/lib/stm32f7xx_ll_rcc.h"
#elif defined(ARCH_stm32h7)
#include "stm32h7/stm32h7xx.h"
#include "stm32h7/lib/stm32h7xx_ll_rcc.h"
#elif defined(ARCH_efr32)
#include "efr32/efr32xx.h"
#include "efr32/lib/em_cmu.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
//* UARTs */
#if defined(ARCH_stm32f1)
enum {
        #if defined(RCC_APB2ENR_USART1EN)
        _UART1,
        #endif
        #if defined(RCC_APB1ENR_USART2EN)
        _UART2,
        #endif
        #if defined(RCC_APB1ENR_USART3EN)
        _UART3,
        #endif
        #if defined(RCC_APB1ENR_UART4EN)
        _UART4,
        #endif
        #if defined(RCC_APB1ENR_UART5EN)
        _UART5,
        #endif
        _UART_COUNT
};
#elif defined(ARCH_stm32f3)
enum {
        #if defined(RCC_APB2ENR_USART1EN)
        _UART1,
        #endif
        #if defined(RCC_APB1ENR_USART2EN)
        _UART2,
        #endif
        #if defined(RCC_APB1ENR_USART3EN)
        _UART3,
        #endif
        #if defined(RCC_APB1ENR_UART4EN)
        _UART4,
        #endif
        #if defined(RCC_APB1ENR_UART5EN)
        _UART5,
        #endif
        #if defined(RCC_APB2ENR_USART6EN)
        _UART6,
        #endif
        #if defined(RCC_APB1ENR_UART7EN)
        _UART7,
        #endif
        #if defined(RCC_APB1ENR_UART8EN)
        _UART8,
        #endif
        #if defined(RCC_APB2ENR_UART9EN)
        _UART9,
        #endif
        #if defined(RCC_APB2ENR_UART10EN)
        _UART10,
        #endif
        _UART_COUNT
};
#elif defined(ARCH_stm32f4)
enum {
        #if defined(RCC_APB2ENR_USART1EN)
        _UART1,
        #endif
        #if defined(RCC_APB1ENR_USART2EN)
        _UART2,
        #endif
        #if defined(RCC_APB1ENR_USART3EN)
        _UART3,
        #endif
        #if defined(RCC_APB1ENR_UART4EN)
        _UART4,
        #endif
        #if defined(RCC_APB1ENR_UART5EN)
        _UART5,
        #endif
        #if defined(RCC_APB2ENR_USART6EN)
        _UART6,
        #endif
        #if defined(RCC_APB1ENR_UART7EN)
        _UART7,
        #endif
        #if defined(RCC_APB1ENR_UART8EN)
        _UART8,
        #endif
        #if defined(RCC_APB2ENR_UART9EN)
        _UART9,
        #endif
        #if defined(RCC_APB2ENR_UART10EN)
        _UART10,
        #endif
        _UART_COUNT
};
#elif defined(ARCH_stm32f7)
enum {
        #if defined(RCC_APB2ENR_USART1EN)
        _UART1,
        #endif
        #if defined(RCC_APB1ENR_USART2EN)
        _UART2,
        #endif
        #if defined(RCC_APB1ENR_USART3EN)
        _UART3,
        #endif
        #if defined(RCC_APB1ENR_UART4EN)
        _UART4,
        #endif
        #if defined(RCC_APB1ENR_UART5EN)
        _UART5,
        #endif
        #if defined(RCC_APB2ENR_USART6EN)
        _UART6,
        #endif
        #if defined(RCC_APB1ENR_UART7EN)
        _UART7,
        #endif
        #if defined(RCC_APB1ENR_UART8EN)
        _UART8,
        #endif
        _UART_COUNT
};
#elif defined(ARCH_stm32h7)
enum {
        #if defined(RCC_APB2ENR_USART1EN)
        _UART1,
        #endif
        #if defined(RCC_APB1LENR_USART2EN)
        _UART2,
        #endif
        #if defined(RCC_APB1LENR_USART3EN)
        _UART3,
        #endif
        #if defined(RCC_APB1LENR_UART4EN)
        _UART4,
        #endif
        #if defined(RCC_APB1LENR_UART5EN)
        _UART5,
        #endif
        #if defined(RCC_APB2ENR_USART6EN)
        _UART6,
        #endif
        #if defined(RCC_APB1LENR_UART7EN)
        _UART7,
        #endif
        #if defined(RCC_APB1LENR_UART8EN)
        _UART8,
        #endif
        #if defined(RCC_APB1LENR_UART9EN)
        _UART8,
        #endif
        #if defined(RCC_APB2ENR_USART10EN)
        _UART8,
        #endif
        _UART_COUNT
};
#elif defined(ARCH_efr32)
#define _UART_COUNT USART_COUNT
#endif


/* USART handling structure */
struct UART_mem {
        queue_t *tx_queue;
        queue_t *rx_queue;
        mutex_t *port_lock_rx_mtx;
        mutex_t *port_lock_tx_mtx;
        void    *uarthdl;
        u8_t     major;
        u8_t     minor;

        struct UART_rich_config {
                GPIO_pin_in_port_t  RS485_DE_pin;
                enum UART_LIN_break LIN_break_bits;
                struct UART_config  basic;
        } config;
};

/*==============================================================================
  Exported objects
==============================================================================*/
extern struct UART_mem *_UART_mem[];

/*==============================================================================
  Exported functions
==============================================================================*/
extern int  _UART_LLD__turn_on(struct UART_mem *hdl);
extern int  _UART_LLD__turn_off(struct UART_mem *hdl);
extern void _UART_LLD__resume_transmit(struct UART_mem *hdl);
extern int  _UART_LLD__configure(struct UART_mem *hdl, const struct UART_rich_config *config);
extern void _UART_set_DE_pin(struct UART_mem *hdl, bool enable);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _UART_H_ */
/*==============================================================================
  End of file
==============================================================================*/
