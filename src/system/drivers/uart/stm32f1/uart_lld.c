/*=========================================================================*//**
@file    uart_lld.c

@author  Daniel Zorychta

@brief   This file support USART peripherals

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

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/driver.h"
#if defined(ARCH_stm32f1)
#include "uart.h"
#include "uart_ioctl.h"
#include "stm32f1/uart_cfg.h"
#include "stm32f1/stm32f10x.h"
#include "stm32f1/lib/stm32f10x_rcc.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/* UART registers */
typedef struct {
        USART_t        *UART;
        __IO uint32_t  *APBENR;
        __IO uint32_t  *APBRSTR;
        const uint32_t  APBENR_UARTEN;
        const uint32_t  APBRSTR_UARTRST;
        const IRQn_Type IRQn;
} UART_regs_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
// all registers which are need to control particular UART peripheral
static const UART_regs_t UART[] = {
        #if defined(RCC_APB2ENR_USART1EN)
        {
                .UART            = USART1,
                .APBENR          = &RCC->APB2ENR,
                .APBRSTR         = &RCC->APB2RSTR,
                .APBENR_UARTEN   = RCC_APB2ENR_USART1EN,
                .APBRSTR_UARTRST = RCC_APB2RSTR_USART1RST,
                .IRQn            = USART1_IRQn,
        },
        #endif
        #if defined(RCC_APB1ENR_USART2EN)
        {
                .UART            = USART2,
                .APBENR          = &RCC->APB1ENR,
                .APBRSTR         = &RCC->APB1RSTR,
                .APBENR_UARTEN   = RCC_APB1ENR_USART2EN,
                .APBRSTR_UARTRST = RCC_APB1RSTR_USART2RST,
                .IRQn            = USART2_IRQn,
        },
        #endif
        #if defined(RCC_APB1ENR_USART3EN)
        {
                .UART            = USART3,
                .APBENR          = &RCC->APB1ENR,
                .APBRSTR         = &RCC->APB1RSTR,
                .APBENR_UARTEN   = RCC_APB1ENR_USART3EN,
                .APBRSTR_UARTRST = RCC_APB1RSTR_USART3RST,
                .IRQn            = USART3_IRQn,
        },
        #endif
        #if defined(RCC_APB1ENR_UART4EN)
        {
                .UART            = UART4,
                .APBENR          = &RCC->APB1ENR,
                .APBRSTR         = &RCC->APB1RSTR,
                .APBENR_UARTEN   = RCC_APB1ENR_UART4EN,
                .APBRSTR_UARTRST = RCC_APB1RSTR_UART4RST,
                .IRQn            = UART4_IRQn,
        },
        #endif
        #if defined(RCC_APB1ENR_UART5EN)
        {
                .UART            = UART5,
                .APBENR          = &RCC->APB1ENR,
                .APBRSTR         = &RCC->APB1RSTR,
                .APBENR_UARTEN   = RCC_APB1ENR_UART5EN,
                .APBRSTR_UARTRST = RCC_APB1RSTR_UART5RST,
                .IRQn            = UART5_IRQn,
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
 * @param[in] UART              UART registers
 *
 * @return One of errno value
 */
//==============================================================================
int _UART_LLD__turn_on(u8_t major)
{
        if (!(*UART[major].APBENR & UART[major].APBENR_UARTEN)) {
                SET_BIT(*UART[major].APBRSTR, UART[major].APBRSTR_UARTRST);
                CLEAR_BIT(*UART[major].APBRSTR, UART[major].APBRSTR_UARTRST);
                SET_BIT(*UART[major].APBENR, UART[major].APBENR_UARTEN);

                NVIC_EnableIRQ(UART[major].IRQn);
                NVIC_SetPriority(UART[major].IRQn, _CPU_IRQ_SAFE_PRIORITY_);

                return ESUCC;
        } else {
                return EADDRINUSE;
        }
}

//==============================================================================
/**
 * @brief Function disable USART clock
 *
 * @param[in] UART              UART registers
 *
 * @return One of errno value.
 */
//==============================================================================
int _UART_LLD__turn_off(u8_t major)
{
        NVIC_DisableIRQ(UART[major].IRQn);
        SET_BIT(*UART[major].APBRSTR, UART[major].APBRSTR_UARTRST);
        CLEAR_BIT(*UART[major].APBRSTR, UART[major].APBRSTR_UARTRST);
        CLEAR_BIT(*UART[major].APBENR, UART[major].APBENR_UARTEN);
        return ESUCC;
}


//==============================================================================
/**
 * @brief Function transmit currently setup buffer.
 *
 * @param major         UART number
 */
//==============================================================================
void _UART_LLD__transmit(u8_t major)
{
        SET_BIT(UART[major].UART->CR1, USART_CR1_TCIE);
}

//==============================================================================
/**
 * @brief Function abort pending transmission.
 *
 * @param major         UART number
 */
//==============================================================================
void _UART_LLD__abort_trasmission(u8_t major)
{
        CLEAR_BIT(UART[major].UART->CR1, USART_CR1_TCIE);
}

//==============================================================================
/**
 * @brief Function resume byte receiving.
 *
 * @param major         UART number
 */
//==============================================================================
void _UART_LLD__rx_resume(u8_t major)
{
        SET_BIT(UART[major].UART->CR1, USART_CR1_RXNEIE);
}

//==============================================================================
/**
 * @brief Function hold byte receiving.
 *
 * @param major         UART number
 */
//==============================================================================
void _UART_LLD__rx_hold(u8_t major)
{
        CLEAR_BIT(UART[major].UART->CR1, USART_CR1_RXNEIE);
}

//==============================================================================
/**
 * @brief Function configure selected UART.
 *
 * @param major         major device number
 * @param config        configuration structure
 */
//==============================================================================
void _UART_LLD__configure(u8_t major, const struct UART_config *config)
{
        const UART_regs_t *DEV = &UART[major];

        /* set baud */
        RCC_ClocksTypeDef freq;
        RCC_GetClocksFreq(&freq);

        u32_t PCLK = (DEV->UART == USART1) ? freq.PCLK2_Frequency : freq.PCLK1_Frequency;

        DEV->UART->BRR = (PCLK / (config->baud)) + 1;

        /* set 8 bit word length and wake idle line */
        CLEAR_BIT(DEV->UART->CR1, USART_CR1_M | USART_CR1_WAKE);

        /* set parity */
        switch (config->parity) {
        case UART_PARITY__OFF:
                CLEAR_BIT(DEV->UART->CR1, USART_CR1_PCE | USART_CR1_M);
                break;
        case UART_PARITY__EVEN:
                CLEAR_BIT(DEV->UART->CR1, USART_CR1_PS);
                SET_BIT(DEV->UART->CR1, USART_CR1_PCE | USART_CR1_M);
                break;
        case UART_PARITY__ODD:
                SET_BIT(DEV->UART->CR1, USART_CR1_PS);
                SET_BIT(DEV->UART->CR1, USART_CR1_PCE | USART_CR1_M);
                break;
        }

        /* transmitter enable */
        if (config->tx_enable) {
                SET_BIT(DEV->UART->CR1, USART_CR1_TE);
        } else {
                CLEAR_BIT(DEV->UART->CR1, USART_CR1_TE);
        }

        /* receiver enable */
        if (config->rx_enable) {
                SET_BIT(DEV->UART->CR1, USART_CR1_RE);
        } else {
                CLEAR_BIT(DEV->UART->CR1, USART_CR1_RE);
        }

        /* enable LIN if configured */
        if (config->LIN_mode_enable) {
                SET_BIT(DEV->UART->CR2, USART_CR2_LINEN);
        } else {
                CLEAR_BIT(DEV->UART->CR2, USART_CR2_LINEN);
        }

        /* configure stop bits */
        if (config->stop_bits == UART_STOP_BIT__1) {
                CLEAR_BIT(DEV->UART->CR2, USART_CR2_STOP);
        } else {
                CLEAR_BIT(DEV->UART->CR2, USART_CR2_STOP);
                SET_BIT(DEV->UART->CR2, USART_CR2_STOP_1);
        }

        /* clock configuration (synchronous mode) */
        CLEAR_BIT(DEV->UART->CR2, USART_CR2_CLKEN | USART_CR2_CPOL | USART_CR2_CPHA | USART_CR2_LBCL);

        /* LIN break detection length */
        if (config->LIN_break_length == UART_LIN_BREAK__10_BITS) {
                CLEAR_BIT(DEV->UART->CR2, USART_CR2_LBDL);
        } else {
                SET_BIT(DEV->UART->CR2, USART_CR2_LBDL);
        }

        /* hardware flow control */
        if (config->hardware_flow_ctrl) {
                SET_BIT(DEV->UART->CR3, USART_CR3_CTSE | USART_CR3_RTSE);
        } else {
                CLEAR_BIT(DEV->UART->CR3, USART_CR3_CTSE | USART_CR3_RTSE);
        }

        /* configure single wire mode */
        if (config->single_wire_mode) {
                SET_BIT(DEV->UART->CR3, USART_CR3_HDSEL);
        } else {
                CLEAR_BIT(DEV->UART->CR3, USART_CR3_HDSEL);
        }

        /* enable RXNE interrupt */
        SET_BIT(DEV->UART->CR1, USART_CR1_RXNEIE);

        /* enable UART */
        SET_BIT(DEV->UART->CR1, USART_CR1_UE);
}

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

        const UART_regs_t *DEV = &UART[major];

        /* receiver interrupt handler */
        int received = 0;
        while ((DEV->UART->CR1 & USART_CR1_RXNEIE) && (DEV->UART->SR & (USART_SR_RXNE | USART_SR_ORE))) {
                u8_t DR = DEV->UART->DR;

                if (_UART_FIFO__write(&_UART_mem[major]->Rx_FIFO, &DR)) {
                        received++;
                }
        }

        /* transmitter interrupt handler */
        if ((DEV->UART->CR1 & USART_CR1_TCIE) && (DEV->UART->SR & USART_SR_TC)) {

                if (_UART_mem[major]->Tx_buffer.data_size && _UART_mem[major]->Tx_buffer.src_ptr) {
                        DEV->UART->DR = *(_UART_mem[major]->Tx_buffer.src_ptr++);

                        if (--_UART_mem[major]->Tx_buffer.data_size == 0) {
                                _UART_mem[major]->Tx_buffer.src_ptr = NULL;
                        }
                } else {
                        CLEAR_BIT(DEV->UART->CR1, USART_CR1_TCIE);
                        sys_semaphore_signal_from_ISR(_UART_mem[major]->write_ready_sem, NULL);
                        yield = true;
                }
        }

        // set receive semaphore to number of received bytes
        while (received--) {
                sys_semaphore_signal_from_ISR(_UART_mem[major]->data_read_sem, NULL);
                yield = true;
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
#if defined(RCC_APB1ENR_USART2EN)
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
#if defined(RCC_APB1ENR_USART3EN)
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
#if defined(RCC_APB1ENR_UART4EN)
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
#if defined(RCC_APB1ENR_UART5EN)
void UART5_IRQHandler(void)
{
        IRQ_handle(_UART5);
}
#endif

#endif
/*==============================================================================
  End of file
==============================================================================*/
