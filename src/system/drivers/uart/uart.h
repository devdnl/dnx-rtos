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

#if defined(ARCH_stm32f1)
#include "stm32fx/uart_cfg.h"
#include "stm32f1/stm32f10x.h"
#include "stm32f1/lib/stm32f10x_rcc.h"
#elif defined(ARCH_stm32f4)
#include "stm32fx/uart_cfg.h"
#include "stm32f4/stm32f4xx.h"
#include "stm32f4/lib/stm32f4xx_rcc.h"
#elif defined(ARCH_stm32f7)
#include "stm32fx/uart_cfg.h"
#include "stm32f7/stm32f7xx.h"
#include "stm32f7/lib/stm32f7xx_ll_rcc.h"
#elif defined(ARCH_efr32)
#include "efr32/uart_cfg.h"
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
#elif defined(ARCH_efr32)
#define _UART_COUNT USART_COUNT
#endif


/* USART handling structure */
struct UART_mem {
        // Rx FIFO
        struct Rx_FIFO {
                u8_t            buffer[_UART_RX_BUFFER_SIZE];
                u16_t           buffer_level;
                u16_t           read_index;
                u16_t           write_index;
        } Rx_FIFO;

        // Tx FIFO
        struct Tx_buffer {
                const u8_t     *src_ptr;
                size_t          data_size;
        } Tx_buffer;

        // UART control
        sem_t                  *write_ready_sem;
        sem_t                  *data_read_sem;
        mutex_t                *port_lock_rx_mtx;
        mutex_t                *port_lock_tx_mtx;
        u8_t                    major;
        struct UART_config      config;
};

/*==============================================================================
  Exported objects
==============================================================================*/
extern struct UART_mem *_UART_mem[];

/*==============================================================================
  Exported functions
==============================================================================*/
extern int  _UART_LLD__turn_on(u8_t major);
extern int  _UART_LLD__turn_off(u8_t major);
extern void _UART_LLD__transmit(u8_t major);
extern void _UART_LLD__abort_trasmission(u8_t major);
extern void _UART_LLD__rx_resume(u8_t major);
extern void _UART_LLD__rx_hold(u8_t major);
extern void _UART_LLD__configure(u8_t major, const struct UART_config *config);
extern bool _UART_FIFO__write(struct Rx_FIFO *fifo, u8_t *data);

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
