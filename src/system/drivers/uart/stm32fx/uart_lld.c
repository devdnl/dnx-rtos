/*=========================================================================*//**
@file    uart_lld.c

@author  Daniel Zorychta

@brief   This file support USART peripherals for STM32F1/F3/F4/F7/H7.
         * STM32F7 architecture is supported without extra features.
         * STM32H7 architecture is supported without extra features.

@note    Copyright (C) 2020 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/driver.h"

#include "uart.h"
#include "uart_ioctl.h"

#if defined(ARCH_stm32f1)
#include "stm32f1/stm32f10x.h"
#include "stm32f1/lib/stm32f10x_rcc.h"
#include "stm32f1/dma_ddi.h"
#define RDR                     DR
#define TDR                     DR
#define USART_CLKSOURCE_PCLK1   1
#define USART_CLKSOURCE_PCLK2   2
#define NDTR                    CNDTR
#define _DMA_DDI_get_stream     _DMA_DDI_get_channel
typedef DMA_Channel_t           DMA_Stream_TypeDef;
#elif defined(ARCH_stm32f3)
#include "stm32f3/stm32f3xx.h"
#include "stm32f3/lib/stm32f3xx_ll_rcc.h"
#include "stm32f3/dma_ddi.h"
#define SR                      ISR
#define USART_SR_RXNE           USART_ISR_RXNE
#define USART_SR_ORE            USART_ISR_ORE
#define USART_SR_TC             USART_ISR_TC
#define USART_SR_IDLE           USART_ISR_IDLE
#define USART_SR_TXE            USART_ISR_TXE
#define NDTR                    CNDTR
#define _DMA_DDI_get_stream     _DMA_DDI_get_channel
typedef DMA_Channel_TypeDef     DMA_Stream_TypeDef;
#elif defined(ARCH_stm32f4)
#include "stm32f4/stm32f4xx.h"
#include "stm32f4/lib/stm32f4xx_rcc.h"
#include "stm32f4/dma_ddi.h"
#define RDR                     DR
#define TDR                     DR
#define USART_CLKSOURCE_PCLK1   1
#define USART_CLKSOURCE_PCLK2   2
#elif defined(ARCH_stm32f7)
#include "stm32f7/stm32f7xx.h"
#include "stm32f7/lib/stm32f7xx_ll_rcc.h"
#include "stm32f7/dma_ddi.h"
#define SR                      ISR
#define USART_SR_RXNE           USART_ISR_RXNE
#define USART_SR_ORE            USART_ISR_ORE
#define USART_SR_TC             USART_ISR_TC
#define USART_SR_IDLE           USART_ISR_IDLE
#define USART_SR_TXE            USART_ISR_TXE
#elif defined(ARCH_stm32h7)
#include "stm32h7/stm32h7xx.h"
#include "stm32h7/lib/stm32h7xx_ll_rcc.h"
#define SR                      ISR
#define USART_SR_RXNE           USART_ISR_RXNE_RXFNE
#define USART_SR_ORE            USART_ISR_ORE
#define USART_SR_TC             USART_ISR_TC
#define USART_SR_IDLE           USART_ISR_IDLE
#define USART_SR_TXE            USART_ISR_TXE_TXFNF
#define _DMA_DDI_reserve(...)   0
#define _DMA_DDI_release(...)
#define _DMA_DDI_get_stream(...) ENOTSUP
#endif

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define DMA_BUFFER_MASK         (_CPUCTL_CACHE_ALIGN - 1)
#define DMA_ALIGN_UP(n)         (((n) + DMA_BUFFER_MASK) & ~DMA_BUFFER_MASK)
#define DMA_ALIGN_DOWN(n)       ((n) & ~DMA_BUFFER_MASK)

#define USE_DMA __UART_UART1_DMA_MODE__ || __UART_UART2_DMA_MODE__\
             || __UART_UART3_DMA_MODE__ || __UART_UART4_DMA_MODE__\
             || __UART_UART5_DMA_MODE__ || __UART_UART6_DMA_MODE__\
             || __UART_UART7_DMA_MODE__ || __UART_UART8_DMA_MODE__\
             || __UART_UART9_DMA_MODE__ || __UART_UART10_DMA_MODE__

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/* UART registers */
typedef struct {
        USART_TypeDef  *UART;
        __IO uint32_t  *APBENR;
        __IO uint32_t  *APBRSTR;
        const uint32_t  APBENR_UARTEN;
        const uint32_t  APBRSTR_UARTRST;
        const IRQn_Type IRQn;
        const u32_t     IRQ_priority;
        const u32_t     CLKSRC;
        const bool      use_DMA;
        const u16_t     DMA_buf_len;
        const u8_t      DMA_channel;
        const u8_t      DMA_major;
        const u8_t      DMA_rx_stream_pri;
        const u8_t      DMA_rx_stream_alt;
} UART_setup_t;

/* UART DMA buffer */
typedef struct {
        u16_t               rd_idx;
        u32_t               desc;
        DMA_Stream_TypeDef *stream;
        u8_t               *buf;
        u16_t               buflen;
        u8_t                buf_holder[];
} uart_dma_t;

/* UART specific handle */
typedef struct {
        uart_dma_t *DMA;
} uarthdl_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
#if USE_DMA
#if !defined(ARCH_stm32h7)
static bool dma_half_and_finish(DMA_Stream_TypeDef *stream, u8_t sr, void *arg);
#endif
#endif

/*==============================================================================
  Local object definitions
==============================================================================*/
MODULE_NAME(UART);

// all registers which are need to control particular UART peripheral
static const UART_setup_t UART[] = {
        #if defined(RCC_APB2ENR_USART1EN)
        {
                .UART            = USART1,
                .IRQn            = USART1_IRQn,
                .IRQ_priority    = __UART_UART1_IRQ_PRIORITY__,
                .use_DMA         = __UART_UART1_DMA_MODE__,
                .DMA_buf_len     = __UART_UART1_DMA_BUF_LEN__,
                #if defined(ARCH_stm32h7)
                .APBENR          = &RCC->APB2ENR,
                .APBRSTR         = &RCC->APB2RSTR,
                .APBENR_UARTEN   = RCC_APB2ENR_USART1EN,
                .APBRSTR_UARTRST = RCC_APB2RSTR_USART1RST,
                #else
                .APBENR          = &RCC->APB2ENR,
                .APBRSTR         = &RCC->APB2RSTR,
                .APBENR_UARTEN   = RCC_APB2ENR_USART1EN,
                .APBRSTR_UARTRST = RCC_APB2RSTR_USART1RST,
                #endif
                #if defined(ARCH_stm32f1)
                .CLKSRC            = USART_CLKSOURCE_PCLK2,
                .DMA_channel       = UINT8_MAX,
                .DMA_major         = 0,
                .DMA_rx_stream_pri = 5,
                .DMA_rx_stream_alt = UINT8_MAX,
                #elif defined(ARCH_stm32f3)
                .CLKSRC            = LL_RCC_USART1_CLKSOURCE,
                .DMA_channel       = UINT8_MAX,
                .DMA_major         = 0,
                .DMA_rx_stream_pri = 5,
                .DMA_rx_stream_alt = UINT8_MAX,
                #elif defined(ARCH_stm32f4)
                .CLKSRC            = USART_CLKSOURCE_PCLK2,
                .DMA_channel       = 4,
                .DMA_major         = 1,
                .DMA_rx_stream_pri = 2,
                .DMA_rx_stream_alt = 5,
                #elif defined(ARCH_stm32f7)
                .CLKSRC            = LL_RCC_USART1_CLKSOURCE,
                .DMA_channel       = 4,
                .DMA_major         = 1,
                .DMA_rx_stream_pri = 2,
                .DMA_rx_stream_alt = 5,
                #elif defined(ARCH_stm32h7)
                .CLKSRC            = LL_RCC_USART16_CLKSOURCE,
                .DMA_channel       = 41,
                #endif
        },
        #endif
        #if defined(RCC_APB1ENR_USART2EN) || defined(RCC_APB1LENR_USART2EN)
        {
                .UART            = USART2,
                .IRQn            = USART2_IRQn,
                .IRQ_priority    = __UART_UART2_IRQ_PRIORITY__,
                .use_DMA         = __UART_UART2_DMA_MODE__,
                .DMA_buf_len     = __UART_UART2_DMA_BUF_LEN__,
                #if defined(ARCH_stm32h7)
                .APBENR          = &RCC->APB1LENR,
                .APBRSTR         = &RCC->APB1LRSTR,
                .APBENR_UARTEN   = RCC_APB1LENR_USART2EN,
                .APBRSTR_UARTRST = RCC_APB1LRSTR_USART2RST,
                #else
                .APBENR          = &RCC->APB1ENR,
                .APBRSTR         = &RCC->APB1RSTR,
                .APBENR_UARTEN   = RCC_APB1ENR_USART2EN,
                .APBRSTR_UARTRST = RCC_APB1RSTR_USART2RST,
                #endif
                #if defined(ARCH_stm32f1)
                .CLKSRC            = USART_CLKSOURCE_PCLK1,
                .DMA_channel       = UINT8_MAX,
                .DMA_major         = 0,
                .DMA_rx_stream_pri = 6,
                .DMA_rx_stream_alt = UINT8_MAX,
                #elif defined(ARCH_stm32f3)
                .CLKSRC            = LL_RCC_USART2_CLKSOURCE,
                .DMA_channel       = UINT8_MAX,
                .DMA_major         = 0,
                .DMA_rx_stream_pri = 6,
                .DMA_rx_stream_alt = UINT8_MAX,
                #elif defined(ARCH_stm32f4)
                .CLKSRC            = USART_CLKSOURCE_PCLK1,
                .DMA_channel       = 4,
                .DMA_major         = 0,
                .DMA_rx_stream_pri = 5,
                .DMA_rx_stream_alt = 5,
                #elif defined(ARCH_stm32f7)
                .CLKSRC            = LL_RCC_USART2_CLKSOURCE,
                .DMA_channel       = 4,
                .DMA_major         = 0,
                .DMA_rx_stream_pri = 5,
                .DMA_rx_stream_alt = 5,
                #elif defined(ARCH_stm32h7)
                .CLKSRC            = LL_RCC_USART234578_CLKSOURCE,
                .DMA_channel       = 43,
                #endif
        },
        #endif
        #if defined(RCC_APB1ENR_USART3EN) || defined(RCC_APB1LENR_USART3EN)
        {
                .UART            = USART3,
                .IRQn            = USART3_IRQn,
                .IRQ_priority    = __UART_UART3_IRQ_PRIORITY__,
                .use_DMA         = __UART_UART3_DMA_MODE__,
                .DMA_buf_len     = __UART_UART3_DMA_BUF_LEN__,
                #if defined(ARCH_stm32h7)
                .APBENR          = &RCC->APB1LENR,
                .APBRSTR         = &RCC->APB1LRSTR,
                .APBENR_UARTEN   = RCC_APB1LENR_USART3EN,
                .APBRSTR_UARTRST = RCC_APB1LRSTR_USART3RST,
                #else
                .APBENR          = &RCC->APB1ENR,
                .APBRSTR         = &RCC->APB1RSTR,
                .APBENR_UARTEN   = RCC_APB1ENR_USART3EN,
                .APBRSTR_UARTRST = RCC_APB1RSTR_USART3RST,
                #endif
                #if defined(ARCH_stm32f1)
                .CLKSRC            = USART_CLKSOURCE_PCLK1,
                .DMA_channel       = UINT8_MAX,
                .DMA_major         = 0,
                .DMA_rx_stream_pri = 3,
                .DMA_rx_stream_alt = UINT8_MAX,
                #elif defined(ARCH_stm32f3)
                .CLKSRC            = LL_RCC_USART3_CLKSOURCE,
                .DMA_channel       = UINT8_MAX,
                .DMA_major         = 0,
                .DMA_rx_stream_pri = 3,
                .DMA_rx_stream_alt = UINT8_MAX,
                #elif defined(ARCH_stm32f4)
                .CLKSRC            = USART_CLKSOURCE_PCLK1,
                .DMA_channel       = 4,
                .DMA_major         = 0,
                .DMA_rx_stream_pri = 1,
                .DMA_rx_stream_alt = 1,
                #elif defined(ARCH_stm32f7)
                .CLKSRC            = LL_RCC_USART3_CLKSOURCE,
                .DMA_channel       = 4,
                .DMA_major         = 0,
                .DMA_rx_stream_pri = 1,
                .DMA_rx_stream_alt = 1,
                #elif defined(ARCH_stm32h7)
                .CLKSRC            = LL_RCC_USART234578_CLKSOURCE,
                .DMA_channel       = 45,
                #endif
        },
        #endif
        #if defined(RCC_APB1ENR_UART4EN) || defined(RCC_APB1LENR_UART4EN)
        {
                .UART            = UART4,
                .IRQn            = UART4_IRQn,
                .IRQ_priority    = __UART_UART4_IRQ_PRIORITY__,
                .use_DMA         = __UART_UART4_DMA_MODE__,
                .DMA_buf_len     = __UART_UART4_DMA_BUF_LEN__,
                #if defined(ARCH_stm32h7)
                .APBENR          = &RCC->APB1LENR,
                .APBRSTR         = &RCC->APB1LRSTR,
                .APBENR_UARTEN   = RCC_APB1LENR_UART4EN,
                .APBRSTR_UARTRST = RCC_APB1LRSTR_UART4RST,
                #else
                .APBENR          = &RCC->APB1ENR,
                .APBRSTR         = &RCC->APB1RSTR,
                .APBENR_UARTEN   = RCC_APB1ENR_UART4EN,
                .APBRSTR_UARTRST = RCC_APB1RSTR_UART4RST,
                #endif
                #if defined(ARCH_stm32f1)
                .CLKSRC            = USART_CLKSOURCE_PCLK1,
                .DMA_channel       = UINT8_MAX,
                .DMA_major         = 1,
                .DMA_rx_stream_pri = 3,
                .DMA_rx_stream_alt = UINT8_MAX,
                #elif defined(ARCH_stm32f3)
                .CLKSRC            = LL_RCC_UART4_CLKSOURCE,
                .DMA_channel       = UINT8_MAX,
                .DMA_major         = 1,
                .DMA_rx_stream_pri = 3,
                .DMA_rx_stream_alt = UINT8_MAX,
                #elif defined(ARCH_stm32f4)
                .CLKSRC            = USART_CLKSOURCE_PCLK1,
                .DMA_channel       = 4,
                .DMA_major         = 0,
                .DMA_rx_stream_pri = 2,
                .DMA_rx_stream_alt = 2,
                #elif defined(ARCH_stm32f7)
                .CLKSRC            = LL_RCC_UART4_CLKSOURCE,
                .DMA_channel       = 4,
                .DMA_major         = 0,
                .DMA_rx_stream_pri = 2,
                .DMA_rx_stream_alt = 2,
                #elif defined(ARCH_stm32h7)
                .CLKSRC            = LL_RCC_USART234578_CLKSOURCE,
                .DMA_channel       = 63,
                #endif
        },
        #endif
        #if defined(RCC_APB1ENR_UART5EN) || defined(RCC_APB1LENR_UART5EN)
        {
                .UART            = UART5,
                .IRQn            = UART5_IRQn,
                .IRQ_priority    = __UART_UART5_IRQ_PRIORITY__,
                .use_DMA         = __UART_UART5_DMA_MODE__,
                .DMA_buf_len     = __UART_UART5_DMA_BUF_LEN__,
                #if defined(ARCH_stm32h7)
                .APBENR          = &RCC->APB1LENR,
                .APBRSTR         = &RCC->APB1LRSTR,
                .APBENR_UARTEN   = RCC_APB1LENR_UART5EN,
                .APBRSTR_UARTRST = RCC_APB1LRSTR_UART5RST,
                #else
                .APBENR          = &RCC->APB1ENR,
                .APBRSTR         = &RCC->APB1RSTR,
                .APBENR_UARTEN   = RCC_APB1ENR_UART5EN,
                .APBRSTR_UARTRST = RCC_APB1RSTR_UART5RST,
                #endif
                #if defined(ARCH_stm32f1)
                .CLKSRC            = USART_CLKSOURCE_PCLK1,
                .DMA_channel       = UINT8_MAX,
                .DMA_major         = 1,
                .DMA_rx_stream_pri = 4,
                .DMA_rx_stream_alt = UINT8_MAX,
                #elif defined(ARCH_stm32f3)
                .CLKSRC            = LL_RCC_UART5_CLKSOURCE,
                .DMA_channel       = UINT8_MAX,
                .DMA_major         = 1,
                .DMA_rx_stream_pri = UINT8_MAX,
                .DMA_rx_stream_alt = UINT8_MAX,
                #elif defined(ARCH_stm32f4)
                .CLKSRC            = USART_CLKSOURCE_PCLK1,
                .DMA_channel       = 4,
                .DMA_major         = 0,
                .DMA_rx_stream_pri = 0,
                .DMA_rx_stream_alt = 0,
                #elif defined(ARCH_stm32f7)
                .CLKSRC            = LL_RCC_UART5_CLKSOURCE,
                .DMA_channel       = 4,
                .DMA_major         = 0,
                .DMA_rx_stream_pri = 0,
                .DMA_rx_stream_alt = 0,
                #elif defined(ARCH_stm32h7)
                .CLKSRC            = LL_RCC_USART234578_CLKSOURCE,
                .DMA_channel       = 65,
                #endif
        },
        #endif
        #if defined(RCC_APB2ENR_USART6EN)
        {
                .UART            = USART6,
                .IRQn            = USART6_IRQn,
                .IRQ_priority    = __UART_UART6_IRQ_PRIORITY__,
                .use_DMA         = __UART_UART6_DMA_MODE__,
                .DMA_buf_len     = __UART_UART6_DMA_BUF_LEN__,
                #if defined(ARCH_stm32h7)
                .APBENR          = &RCC->APB2ENR,
                .APBRSTR         = &RCC->APB2RSTR,
                .APBENR_UARTEN   = RCC_APB2ENR_USART6EN,
                .APBRSTR_UARTRST = RCC_APB2RSTR_USART6RST,
                #else
                .APBENR          = &RCC->APB2ENR,
                .APBRSTR         = &RCC->APB2RSTR,
                .APBENR_UARTEN   = RCC_APB2ENR_USART6EN,
                .APBRSTR_UARTRST = RCC_APB2RSTR_USART6RST,
                #endif
                #if defined(ARCH_stm32f1)
                .CLKSRC            = USART_CLKSOURCE_PCLK2,
                .DMA_channel       = UINT8_MAX,
                .DMA_major         = 0,
                .DMA_rx_stream_pri = UINT8_MAX,
                .DMA_rx_stream_alt = UINT8_MAX,
                #elif defined(ARCH_stm32f3)
                .CLKSRC          = LL_RCC_USART6_CLKSOURCE,
                #elif defined(ARCH_stm32f4)
                .CLKSRC          = USART_CLKSOURCE_PCLK2,
                .DMA_channel       = 5,
                .DMA_major         = 1,
                .DMA_rx_stream_pri = 1,
                .DMA_rx_stream_alt = 2,
                #elif defined(ARCH_stm32f7)
                .CLKSRC            = LL_RCC_USART6_CLKSOURCE,
                .DMA_channel       = 5,
                .DMA_major         = 1,
                .DMA_rx_stream_pri = 1,
                .DMA_rx_stream_alt = 2,
                #elif defined(ARCH_stm32h7)
                .CLKSRC            = LL_RCC_USART16_CLKSOURCE,
                .DMA_channel       = 71,
                #endif
        },
        #endif
        #if defined(RCC_APB1ENR_UART7EN) || defined(RCC_APB1LENR_UART7EN)
        {
                .UART            = UART7,
                .IRQn            = UART7_IRQn,
                .IRQ_priority    = __UART_UART7_IRQ_PRIORITY__,
                .use_DMA         = __UART_UART7_DMA_MODE__,
                .DMA_buf_len     = __UART_UART7_DMA_BUF_LEN__,
                #if defined(ARCH_stm32h7)
                .APBENR          = &RCC->APB1LENR,
                .APBRSTR         = &RCC->APB1LRSTR,
                .APBENR_UARTEN   = RCC_APB1LENR_UART7EN,
                .APBRSTR_UARTRST = RCC_APB1LRSTR_UART7RST,
                #else
                .APBENR          = &RCC->APB1ENR,
                .APBRSTR         = &RCC->APB1RSTR,
                .APBENR_UARTEN   = RCC_APB1ENR_UART7EN,
                .APBRSTR_UARTRST = RCC_APB1RSTR_UART7RST,
                #endif
                #if defined(ARCH_stm32f1)
                .CLKSRC            = USART_CLKSOURCE_PCLK1,
                .DMA_channel       = UINT8_MAX,
                .DMA_major         = 0,
                .DMA_rx_stream_pri = UINT8_MAX,
                .DMA_rx_stream_alt = UINT8_MAX,
                #elif defined(ARCH_stm32f3)
                .CLKSRC          = LL_RCC_UART7_CLKSOURCE,
                #elif defined(ARCH_stm32f4)
                .CLKSRC          = USART_CLKSOURCE_PCLK1,
                .DMA_channel       = 5,
                .DMA_major         = 0,
                .DMA_rx_stream_pri = 3,
                .DMA_rx_stream_alt = 3,
                #elif defined(ARCH_stm32f7)
                .CLKSRC            = LL_RCC_UART7_CLKSOURCE,
                .DMA_channel       = 5,
                .DMA_major         = 0,
                .DMA_rx_stream_pri = 3,
                .DMA_rx_stream_alt = 3,
                #elif defined(ARCH_stm32h7)
                .CLKSRC            = LL_RCC_USART234578_CLKSOURCE,
                .DMA_channel       = 79,
                #endif
        },
        #endif
        #if defined(RCC_APB1ENR_UART8EN) || defined(RCC_APB1LENR_UART8EN)
        {
                .UART            = UART8,
                .IRQn            = UART8_IRQn,
                .IRQ_priority    = __UART_UART8_IRQ_PRIORITY__,
                .use_DMA         = __UART_UART8_DMA_MODE__,
                .DMA_buf_len     = __UART_UART8_DMA_BUF_LEN__,
                #if defined(ARCH_stm32h7)
                .APBENR          = &RCC->APB1LENR,
                .APBRSTR         = &RCC->APB1LRSTR,
                .APBENR_UARTEN   = RCC_APB1LENR_UART8EN,
                .APBRSTR_UARTRST = RCC_APB1LRSTR_UART8RST,
                #else
                .APBENR          = &RCC->APB1ENR,
                .APBRSTR         = &RCC->APB1RSTR,
                .APBENR_UARTEN   = RCC_APB1ENR_UART8EN,
                .APBRSTR_UARTRST = RCC_APB1RSTR_UART8RST,
                #endif
                #if defined(ARCH_stm32f1)
                .CLKSRC            = USART_CLKSOURCE_PCLK1,
                .DMA_channel       = UINT8_MAX,
                .DMA_major         = 0,
                .DMA_rx_stream_pri = UINT8_MAX,
                .DMA_rx_stream_alt = UINT8_MAX,
                #elif defined(ARCH_stm32f3)
                .CLKSRC          = LL_RCC_UART8_CLKSOURCE,
                #elif defined(ARCH_stm32f4)
                .CLKSRC            = USART_CLKSOURCE_PCLK1,
                .DMA_channel       = 5,
                .DMA_major         = 0,
                .DMA_rx_stream_pri = 6,
                .DMA_rx_stream_alt = 6,
                #elif defined(ARCH_stm32f7)
                .CLKSRC            = LL_RCC_UART8_CLKSOURCE,
                .DMA_channel       = 5,
                .DMA_major         = 0,
                .DMA_rx_stream_pri = 6,
                .DMA_rx_stream_alt = 6,
                #elif defined(ARCH_stm32h7)
                .CLKSRC            = LL_RCC_USART234578_CLKSOURCE,
                .DMA_channel       = 81,
                #endif
        },
        #endif
        #if defined(RCC_APB2ENR_UART9EN)
        {
                .UART            = UART9,
                .IRQn            = UART9_IRQn,
                .IRQ_priority    = __UART_UART9_IRQ_PRIORITY__,
                .use_DMA         = __UART_UART9_DMA_MODE__,
                .DMA_buf_len     = __UART_UART9_DMA_BUF_LEN__,
                #if defined(ARCH_stm32h7)
                .APBENR          = &RCC->APB2ENR,
                .APBRSTR         = &RCC->APB2RSTR,
                .APBENR_UARTEN   = RCC_APB2ENR_UART9EN,
                .APBRSTR_UARTRST = RCC_APB2RSTR_UART9RST,
                #else
                .APBENR          = &RCC->APB2ENR,
                .APBRSTR         = &RCC->APB2RSTR,
                .APBENR_UARTEN   = RCC_APB2ENR_UART9EN,
                .APBRSTR_UARTRST = RCC_APB2RSTR_UART9RST,
                #endif
                #if defined(ARCH_stm32f1)
                .CLKSRC            = USART9_CLKSOURCE_PCLK1,
                .DMA_channel       = UINT8_MAX,
                .DMA_major         = 0,
                .DMA_rx_stream_pri = UINT8_MAX,
                .DMA_rx_stream_alt = UINT8_MAX,
                #elif defined(ARCH_stm32f3)
                .CLKSRC            = LL_RCC_UART9_CLKSOURCE,
                #elif defined(ARCH_stm32f4)
                .CLKSRC            = USART9_CLKSOURCE_PCLK1,
                .DMA_channel       = UINT8_MAX,
                .DMA_major         = UINT8_MAX,
                .DMA_rx_stream_pri = UINT8_MAX,
                .DMA_rx_stream_alt = UINT8_MAX,
                #elif defined(ARCH_stm32f7)
                .CLKSRC            = LL_RCC_UART9_CLKSOURCE,
                #elif defined(ARCH_stm32h7)
                .CLKSRC            = LL_RCC_USART16910_CLKSOURCE,
                .DMA_channel       = UINT8_MAX,
                #endif
        },
        #endif
        #if defined(RCC_APB2ENR_UART10EN) || defined(RCC_APB2ENR_USART10EN)
        {
                .UART            = UART10,
                .IRQn            = UART10_IRQn,
                .IRQ_priority    = __UART_UART10_IRQ_PRIORITY__,
                .use_DMA         = __UART_UART10_DMA_MODE__,
                .DMA_buf_len     = __UART_UART10_DMA_BUF_LEN__,
                #if defined(ARCH_stm32h7)
                .APBENR          = &RCC->APB2ENR,
                .APBRSTR         = &RCC->APB2RSTR,
                .APBENR_UARTEN   = RCC_APB2ENR_UART10EN,
                .APBRSTR_UARTRST = RCC_APB2RSTR_UART10RST,
                #else
                .APBENR          = &RCC->APB2ENR,
                .APBRSTR         = &RCC->APB2RSTR,
                .APBENR_UARTEN   = RCC_APB2ENR_UART10EN,
                .APBRSTR_UARTRST = RCC_APB2RSTR_UART10RST,
                #endif
                #if defined(ARCH_stm32f1)
                .CLKSRC            = USART_CLKSOURCE_PCLK1,
                .DMA_channel       = UINT8_MAX,
                .DMA_major         = 0,
                .DMA_rx_stream_pri = UINT8_MAX,
                .DMA_rx_stream_alt = UINT8_MAX,
                #elif defined(ARCH_stm32f3)
                .CLKSRC            = LL_RCC_UART10_CLKSOURCE,
                #elif defined(ARCH_stm32f4)
                .CLKSRC            = USART_CLKSOURCE_PCLK1,
                .DMA_channel       = UINT8_MAX,
                .DMA_major         = UINT8_MAX,
                .DMA_rx_stream_pri = UINT8_MAX,
                .DMA_rx_stream_alt = UINT8_MAX,
                #elif defined(ARCH_stm32f7)
                .CLKSRC            = LL_RCC_UART10_CLKSOURCE,
                #elif defined(ARCH_stm32h7)
                .CLKSRC            = LL_RCC_USART16910_CLKSOURCE,
                .DMA_channel       = UINT8_MAX,
                #endif
        }
        #endif
};

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief Function enable USART clock
 *
 * @param hdl           uart handle
 *
 * @return One of errno value
 */
//==============================================================================
int _UART_LLD__turn_on(struct UART_mem *hdl)
{
        int err = EADDRINUSE;

        const UART_setup_t *SETUP = &UART[hdl->major];

        if (!(*SETUP->APBENR & SETUP->APBENR_UARTEN)) {

                uarthdl_t *uarthdl;
                err = sys_zalloc(sizeof(*uarthdl), cast(void**, &uarthdl));
                if (!err) {

                        #if USE_DMA
                        if (SETUP->use_DMA) {

                                size_t buf_len = DMA_ALIGN_UP(UART[hdl->major].DMA_buf_len);
                                buf_len = max(_CPUCTL_CACHE_ALIGN, buf_len);

                                uart_dma_t *dmabuf;
                                size_t dma_buf_size = sizeof(*dmabuf) + buf_len + _CPUCTL_CACHE_ALIGN;

                                err = sys_zalloc2(dma_buf_size, NULL, _MM_FLAG__DMA_CAPABLE,
                                                  _MM_FLAG__DMA_CAPABLE, cast(void**, &dmabuf));
                                if (!err) {
                                        dmabuf->buflen = buf_len;
                                        dmabuf->buf    = (void*)DMA_ALIGN_UP((u32_t)&dmabuf->buf_holder);

                                        dmabuf->desc = _DMA_DDI_reserve(SETUP->DMA_major,
                                                                        SETUP->DMA_rx_stream_pri);
                                        if (dmabuf->desc == 0) {
                                                dmabuf->desc = _DMA_DDI_reserve(SETUP->DMA_major,
                                                                                SETUP->DMA_rx_stream_alt);
                                        }

                                        if (dmabuf->desc) {
                                                err = _DMA_DDI_get_stream(dmabuf->desc, &dmabuf->stream);
                                        }

                                        if (err or (dmabuf->desc == 0)) {
                                                dev_dbg(hdl, "DMA not accessible, using IRQ mode", 0);
                                                sys_free(cast(void*, &dmabuf));
                                        } else {
                                                uarthdl->DMA = dmabuf;
                                        }

                                } else {
                                        dev_dbg(hdl, "no free memory to enable DMA mode", 0);
                                }

                                err = 0;
                        }
                        #endif
                }

                if (!err) {
                        hdl->uarthdl = uarthdl;

                        SET_BIT(*SETUP->APBRSTR, SETUP->APBRSTR_UARTRST);
                        CLEAR_BIT(*SETUP->APBRSTR, SETUP->APBRSTR_UARTRST);
                        SET_BIT(*SETUP->APBENR, SETUP->APBENR_UARTEN);

                        NVIC_EnableIRQ(SETUP->IRQn);
                        NVIC_SetPriority(SETUP->IRQn, SETUP->IRQ_priority);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Function disable USART clock
 *
 * @param hdl           uart handle
 *
 * @return One of errno value.
 */
//==============================================================================
int _UART_LLD__turn_off(struct UART_mem *hdl)
{
        uarthdl_t *uarthdl = hdl->uarthdl;
        const UART_setup_t *SETUP =  &UART[hdl->major];

        #if USE_DMA
        if (uarthdl->DMA) {
                _DMA_DDI_release(uarthdl->DMA->desc);
                sys_free(cast(void*, &uarthdl->DMA));
        }
        #endif

        NVIC_DisableIRQ(SETUP->IRQn);
        SET_BIT(*SETUP->APBRSTR, SETUP->APBRSTR_UARTRST);
        CLEAR_BIT(*SETUP->APBRSTR, SETUP->APBRSTR_UARTRST);
        CLEAR_BIT(*SETUP->APBENR, SETUP->APBENR_UARTEN);

        return ESUCC;
}

//==============================================================================
/**
 * @brief Function resume transmission of buffer.
 *
 * @param hdl           uart handle
 */
//==============================================================================
void _UART_LLD__resume_transmit(struct UART_mem *hdl)
{
        const UART_setup_t *SETUP =  &UART[hdl->major];

        if (!(SETUP->UART->CR1 & USART_CR1_TCIE)) {
                sys_critical_section_begin();

                if (hdl->config.basic.mode == UART_MODE__RS485) {
                        _UART_set_DE_pin(hdl, true);
                }

                SET_BIT(SETUP->UART->CR1, USART_CR1_TCIE);

                sys_critical_section_end();
        }
}

//==============================================================================
/**
 * @brief Function configure selected UART.
 *
 * @param hdl           uart handle
 * @param config        configuration structure
 */
//==============================================================================
int _UART_LLD__configure(struct UART_mem *hdl, const struct UART_rich_config *config)
{
        uarthdl_t *uarthdl = hdl->uarthdl;
        const UART_setup_t *SETUP = &UART[hdl->major];
        u32_t PCLK = 0;

        /* set baud */
#if defined(ARCH_stm32f1) || defined(ARCH_stm32f4)
        LL_RCC_ClocksTypeDef freq;
        LL_RCC_GetSystemClocksFreq(&freq);

        switch (SETUP->CLKSRC) {
        case USART_CLKSOURCE_PCLK1:
                PCLK = freq.PCLK1_Frequency;
                break;

        case USART_CLKSOURCE_PCLK2:
                PCLK = freq.PCLK2_Frequency;
                break;
        default:
                dev_dbg(hdl, "invalid CLKSRC!", 0);
                break;
        }
#elif defined(ARCH_stm32f7) || defined(ARCH_stm32f3)
        switch (SETUP->CLKSRC) {
        #if defined(LL_RCC_USART1_CLKSOURCE)
        case LL_RCC_USART1_CLKSOURCE:
        #endif
        #if defined(LL_RCC_USART2_CLKSOURCE)
        case LL_RCC_USART2_CLKSOURCE:
        #endif
        #if defined(LL_RCC_USART3_CLKSOURCE)
        case LL_RCC_USART3_CLKSOURCE:
        #endif
        #if defined(LL_RCC_USART6_CLKSOURCE)
        case LL_RCC_USART6_CLKSOURCE:
        #endif
                PCLK = LL_RCC_GetUSARTClockFreq(SETUP->CLKSRC);
                break;
        #if defined(LL_RCC_UART4_CLKSOURCE)
        case LL_RCC_UART4_CLKSOURCE:
        #endif
        #if defined(LL_RCC_UART5_CLKSOURCE)
        case LL_RCC_UART5_CLKSOURCE:
        #endif
        #if defined(LL_RCC_UART7_CLKSOURCE)
        case LL_RCC_UART7_CLKSOURCE:
        #endif
        #if defined(LL_RCC_UART8_CLKSOURCE)
        case LL_RCC_UART8_CLKSOURCE:
        #endif
                PCLK = LL_RCC_GetUARTClockFreq(SETUP->CLKSRC);
                break;

        default:
                break;
        }
#elif defined(ARCH_stm32h7)
        PCLK = LL_RCC_GetUSARTClockFreq(SETUP->CLKSRC);
#endif

        SETUP->UART->BRR = (PCLK / (config->basic.baud)) + 1;

        /* set 8 bit word length and wake idle line */
        CLEAR_BIT(SETUP->UART->CR1, USART_CR1_M | USART_CR1_WAKE);

        /* set parity */
        switch (config->basic.parity) {
        case UART_PARITY__OFF:
                CLEAR_BIT(SETUP->UART->CR1, USART_CR1_PCE | USART_CR1_M);
                break;
        case UART_PARITY__EVEN:
                CLEAR_BIT(SETUP->UART->CR1, USART_CR1_PS);
                SET_BIT(SETUP->UART->CR1, USART_CR1_PCE | USART_CR1_M);
                break;
        case UART_PARITY__ODD:
                SET_BIT(SETUP->UART->CR1, USART_CR1_PS);
                SET_BIT(SETUP->UART->CR1, USART_CR1_PCE | USART_CR1_M);
                break;
        }

        /* transmitter enable */
        if (config->basic.tx_enable) {
                SET_BIT(SETUP->UART->CR1, USART_CR1_TE);
        } else {
                CLEAR_BIT(SETUP->UART->CR1, USART_CR1_TE);
        }

        /* receiver enable */
        if (config->basic.rx_enable) {
                SET_BIT(SETUP->UART->CR1, USART_CR1_RE);
        } else {
                CLEAR_BIT(SETUP->UART->CR1, USART_CR1_RE);
        }

        /* FIFO enable */
        #if defined(USART_CR1_FIFOEN)
        SET_BIT(SETUP->UART->CR1, USART_CR1_FIFOEN);
        #endif

        /* enable LIN if configured */
        if (config->basic.mode == UART_MODE__LIN) {
                SET_BIT(SETUP->UART->CR2, USART_CR2_LINEN);
        } else {
                CLEAR_BIT(SETUP->UART->CR2, USART_CR2_LINEN);
        }

        /* configure stop bits */
        if (config->basic.stop_bits == UART_STOP_BIT__1) {
                CLEAR_BIT(SETUP->UART->CR2, USART_CR2_STOP);
        } else {
                CLEAR_BIT(SETUP->UART->CR2, USART_CR2_STOP);
                SET_BIT(SETUP->UART->CR2, USART_CR2_STOP_1);
        }

        /* clock configuration (synchronous mode) */
        CLEAR_BIT(SETUP->UART->CR2, USART_CR2_CLKEN | USART_CR2_CPOL | USART_CR2_CPHA | USART_CR2_LBCL);

        /* LIN break detection length */
        if (config->LIN_break_bits == UART_LIN_BREAK__10_BITS) {
                CLEAR_BIT(SETUP->UART->CR2, USART_CR2_LBDL);
        } else {
                SET_BIT(SETUP->UART->CR2, USART_CR2_LBDL);
        }

        /* hardware flow control */
        if (config->basic.features & UART_FEATURE__HARDWARE_FLOW_CTRL) {
                SET_BIT(SETUP->UART->CR3, USART_CR3_CTSE | USART_CR3_RTSE);
        } else {
                CLEAR_BIT(SETUP->UART->CR3, USART_CR3_CTSE | USART_CR3_RTSE);
        }

        /* configure single wire mode */
        if (config->basic.features & UART_FEATURE__SINGLE_WIRE) {
                SET_BIT(SETUP->UART->CR3, USART_CR3_HDSEL);
        } else {
                CLEAR_BIT(SETUP->UART->CR3, USART_CR3_HDSEL);
        }

#if USE_DMA
        if (uarthdl->DMA) {
#if !defined(ARCH_stm32h7)
                _DMA_DDI_config_t dma_conf;
                memset(&config, 0, sizeof(config));
                dma_conf.IRQ_priority = __CPU_DEFAULT_IRQ_PRIORITY__;
                dma_conf.arg       = hdl;
                dma_conf.cb_finish = dma_half_and_finish;
                dma_conf.cb_half   = dma_half_and_finish;
                dma_conf.cb_next   = NULL;
                dma_conf.release   = false;
                dma_conf.NDT       = uarthdl->DMA->buflen;
                dma_conf.PA        = cast(u32_t, &UART[hdl->major].UART->RDR);
#if defined(ARCH_stm32f1) || defined(ARCH_stm32f3)
                dma_conf.MA        = cast(u32_t, uarthdl->DMA->buf);
                dma_conf.CR        = DMA_CCRx_MINC_ENABLE
                                   | DMA_CCRx_DIR_P2M
                                   | DMA_CCRx_CIRC_ENABLE
                                   | DMA_CCRx_MSIZE_BYTE
                                   | DMA_CCRx_PSIZE_BYTE;

#elif defined(ARCH_stm32f4) || defined(ARCH_stm32f7)
                dma_conf.MA[0]     = cast(u32_t, uarthdl->DMA->buf);
                dma_conf.MA[1]     = 0;
                dma_conf.FC        = 0;
                dma_conf.CR        = DMA_SxCR_CHSEL_SEL(UART[hdl->major].DMA_channel)
                                   | DMA_SxCR_MINC_ENABLE
                                   | DMA_SxCR_DIR_P2M
                                   | DMA_SxCR_CIRC
                                   | DMA_SxCR_MSIZE_BYTE
                                   | DMA_SxCR_PSIZE_BYTE;
#endif

                _DMA_DDI_transfer(uarthdl->DMA->desc, &dma_conf);

                SET_BIT(SETUP->UART->CR1, USART_CR1_IDLEIE);
                SET_BIT(SETUP->UART->CR3, USART_CR3_DMAR);
#endif
        } else
#endif
        {
                SET_BIT(SETUP->UART->CR1, USART_CR1_RXNEIE);
        }

        /* enable UART */
        SET_BIT(SETUP->UART->CR1, USART_CR1_UE);

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function receive bytes from DMA buffer.
 *
 * @param  hdl          uart handle
 *
 * @return True if task should yield, otherwise false.
 */
//==============================================================================
static bool receive_from_DMA_buffer(struct UART_mem *hdl)
{
#if USE_DMA
        uarthdl_t *uarthdl = hdl->uarthdl;

        bool yield = false;

        _cpuctl_invalidate_dcache_by_addr(uarthdl->DMA->buf, uarthdl->DMA->buflen);

        int wr_idx = uarthdl->DMA->buflen - uarthdl->DMA->stream->NDTR;

        while (wr_idx != uarthdl->DMA->rd_idx) {
                u8_t byte = uarthdl->DMA->buf[uarthdl->DMA->rd_idx];

                if (++uarthdl->DMA->rd_idx >= uarthdl->DMA->buflen) {
                        uarthdl->DMA->rd_idx = 0;
                }

                bool woken = false;
                sys_queue_send_from_ISR(hdl->rx_queue, &byte, &woken);
                yield |= woken;
        }

        return yield;
#else
        return false;
#endif
}

//==============================================================================
/**
 * @brief  Function called by DMA peripheral on half and full transfers.
 *
 * @param  stream       DMA stream
 * @param  sr           status register
 * @param  arg          user's argument (uart handle)
 *
 * @return True if task should yield, otherwise false.
 */
//==============================================================================
#if USE_DMA
#if !defined(ARCH_stm32h7)
static bool dma_half_and_finish(DMA_Stream_TypeDef *stream, u8_t sr, void *arg)
{
        UNUSED_ARG3(stream, sr, arg);
        return receive_from_DMA_buffer(arg);
}
#endif
#endif

//==============================================================================
/**
 * @brief Interrupt handling
 *
 * @param major         major device number
 */
//==============================================================================
static void IRQ_handle(u8_t major)
{
        bool yield = false;

        const UART_setup_t *DEV = &UART[major];
        struct UART_mem *hdl = _UART_mem[major];

        /* IDLE line interrupt handler */
        if ((DEV->UART->CR1 & USART_CR1_IDLEIE) && (DEV->UART->SR & (USART_SR_IDLE))) {
                #if defined(ARCH_stm32f1) || defined(ARCH_stm32f4)
                volatile u8_t DR = DEV->UART->RDR;
                UNUSED_ARG1(DR);
                #endif

                yield |= receive_from_DMA_buffer(hdl);

                #if defined(ARCH_stm32f3) || defined(ARCH_stm32f7) || defined(ARCH_stm32h7)
                WRITE_REG(DEV->UART->ICR, USART_ICR_IDLECF);
                #endif
        }

        /* receiver interrupt handler */
        while ((DEV->UART->CR1 & USART_CR1_RXNEIE) && (DEV->UART->SR & (USART_SR_RXNE | USART_SR_ORE))) {

                u8_t DR = DEV->UART->RDR;

                bool woken = false;
                sys_queue_send_from_ISR(hdl->rx_queue, &DR, &woken);
                yield |= woken;

                #if defined(ARCH_stm32f3) || defined(ARCH_stm32f7) || defined(ARCH_stm32h7)
                if (DEV->UART->SR & USART_SR_ORE) {
                        WRITE_REG(DEV->UART->ICR, USART_ICR_ORECF);
                }
                #endif
        }

        /* transmitter interrupt handler */
        size_t items = 0;
        if (sys_queue_get_number_of_items_from_ISR(hdl->tx_queue, &items) == 0) {
                if (items > 0) {
                        if (hdl->config.basic.mode == UART_MODE__RS485) {
                                _UART_set_DE_pin(hdl, true);
                        }

                        SET_BIT(DEV->UART->CR1, USART_CR1_TCIE);
                }
        }

        if ((DEV->UART->CR1 & USART_CR1_TCIE) && (DEV->UART->SR & USART_SR_TC)) {

                do {
                        bool woken = false;
                        u8_t byte;
                        if (sys_queue_receive_from_ISR(hdl->tx_queue, &byte, &woken) == 0) {
                                DEV->UART->TDR = byte;

                        } else {
                                CLEAR_BIT(DEV->UART->CR1, USART_CR1_TCIE);

                                if (hdl->config.basic.mode == UART_MODE__RS485) {
                                        _UART_set_DE_pin(hdl, false);
                                }

                                break;
                        }

                        yield |= woken;

                } while (DEV->UART->SR & USART_SR_TXE);
        }

        /* yield thread if data send or received */
        sys_thread_yield_from_ISR(yield);
}

//==============================================================================
/**
 * @brief USART1 Interrupt
 */
//==============================================================================
#if defined(RCC_APB2ENR_USART1EN)
void USART1_IRQHandler(void)
{
        IRQ_handle(_UART1);
}
#endif

//==============================================================================
/**
 * @brief USART2 Interrupt
 */
//==============================================================================
#if defined(RCC_APB1ENR_USART2EN) || defined(RCC_APB1LENR_USART2EN)
void USART2_IRQHandler(void)
{
        IRQ_handle(_UART2);
}
#endif

//==============================================================================
/**
 * @brief USART3 Interrupt
 */
//==============================================================================
#if defined(RCC_APB1ENR_USART3EN) || defined(RCC_APB1LENR_USART3EN)
void USART3_IRQHandler(void)
{
        IRQ_handle(_UART3);
}
#endif

//==============================================================================
/**
 * @brief UART4 Interrupt
 */
//==============================================================================
#if defined(RCC_APB1ENR_UART4EN) || defined(RCC_APB1LENR_UART4EN)
void UART4_IRQHandler(void)
{
        IRQ_handle(_UART4);
}
#endif

//==============================================================================
/**
 * @brief UART5 Interrupt
 */
//==============================================================================
#if defined(RCC_APB1ENR_UART5EN) || defined(RCC_APB1LENR_UART5EN)
void UART5_IRQHandler(void)
{
        IRQ_handle(_UART5);
}
#endif

//==============================================================================
/**
 * @brief UART6 Interrupt
 */
//==============================================================================
#if defined(RCC_APB2ENR_USART6EN)
void USART6_IRQHandler(void)
{
        IRQ_handle(_UART6);
}
#endif

//==============================================================================
/**
 * @brief UART7 Interrupt
 */
//==============================================================================
#if defined(RCC_APB1ENR_UART7EN) || defined(RCC_APB1LENR_UART7EN)
void UART7_IRQHandler(void)
{
        IRQ_handle(_UART7);
}
#endif

//==============================================================================
/**
 * @brief UART8 Interrupt
 */
//==============================================================================
#if defined(RCC_APB1ENR_UART8EN) || defined(RCC_APB1LENR_UART8EN)
void UART8_IRQHandler(void)
{
        IRQ_handle(_UART8);
}
#endif

//==============================================================================
/**
 * @brief UART9 Interrupt
 */
//==============================================================================
#if defined(RCC_APB2ENR_UART9EN)
void UART9_IRQHandler(void)
{
        IRQ_handle(_UART9);
}
#endif

//==============================================================================
/**
 * @brief UART10 Interrupt
 */
//==============================================================================
#if defined(RCC_APB2ENR_UART10EN) || defined(RCC_APB2ENR_USART10EN)
void UART10_IRQHandler(void)
{
        IRQ_handle(_UART10);
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
