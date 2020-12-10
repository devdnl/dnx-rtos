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
#include "stm32fx/uart_cfg.h"

#if defined(ARCH_stm32f1)
#include "stm32f1/stm32f10x.h"
#include "stm32f1/lib/stm32f10x_rcc.h"
#define RDR                     DR
#define TDR                     DR
#define USART_CLKSOURCE_PCLK1   1
#define USART_CLKSOURCE_PCLK2   2
#elif defined(ARCH_stm32f3)
#include "stm32f3/stm32f3xx.h"
#include "stm32f3/lib/stm32f3xx_ll_rcc.h"
#define SR                      ISR
#define USART_SR_RXNE           USART_ISR_RXNE
#define USART_SR_ORE            USART_ISR_ORE
#define USART_SR_TC             USART_ISR_TC
#elif defined(ARCH_stm32f4)
#include "stm32f4/stm32f4xx.h"
#include "stm32f4/lib/stm32f4xx_rcc.h"
#define RDR                     DR
#define TDR                     DR
#define USART_CLKSOURCE_PCLK1   1
#define USART_CLKSOURCE_PCLK2   2
#elif defined(ARCH_stm32f7)
#include "stm32f7/stm32f7xx.h"
#include "stm32f7/lib/stm32f7xx_ll_rcc.h"
#define SR                      ISR
#define USART_SR_RXNE           USART_ISR_RXNE
#define USART_SR_ORE            USART_ISR_ORE
#define USART_SR_TC             USART_ISR_TC
#elif defined(ARCH_stm32h7)
#include "stm32h7/stm32h7xx.h"
#include "stm32h7/lib/stm32h7xx_ll_rcc.h"
#define SR                      ISR
#define USART_SR_RXNE           USART_ISR_RXNE_RXFNE
#define USART_SR_ORE            USART_ISR_ORE
#define USART_SR_TC             USART_ISR_TC
#endif

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

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
        const u32_t     CLKSRC;
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
                .IRQn            = USART1_IRQn,
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
                #if defined(ARCH_stm32f1) || defined(ARCH_stm32f4)
                .CLKSRC          = USART_CLKSOURCE_PCLK2,
                #elif defined(ARCH_stm32f3)
                .CLKSRC          = LL_RCC_USART1_CLKSOURCE,
                #elif defined(ARCH_stm32f7)
                .CLKSRC          = LL_RCC_USART1_CLKSOURCE,
                #elif defined(ARCH_stm32h7)
                .CLKSRC          = LL_RCC_USART16_CLKSOURCE,
                #endif
        },
        #endif
        #if defined(RCC_APB1ENR_USART2EN) || defined(RCC_APB1LENR_USART2EN)
        {
                .UART            = USART2,
                .IRQn            = USART2_IRQn,
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
                #if defined(ARCH_stm32f1) || defined(ARCH_stm32f4)
                .CLKSRC          = USART_CLKSOURCE_PCLK1,
                #elif defined(ARCH_stm32f3)
                .CLKSRC          = LL_RCC_USART2_CLKSOURCE,
                #elif defined(ARCH_stm32f7)
                .CLKSRC          = LL_RCC_USART2_CLKSOURCE,
                #elif defined(ARCH_stm32h7)
                .CLKSRC          = LL_RCC_USART234578_CLKSOURCE,
                #endif
        },
        #endif
        #if defined(RCC_APB1ENR_USART3EN) || defined(RCC_APB1LENR_USART3EN)
        {
                .UART            = USART3,
                .IRQn            = USART3_IRQn,
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
                #if defined(ARCH_stm32f1) || defined(ARCH_stm32f4)
                .CLKSRC          = USART_CLKSOURCE_PCLK1,
                #elif defined(ARCH_stm32f3)
                .CLKSRC          = LL_RCC_USART3_CLKSOURCE,
                #elif defined(ARCH_stm32f7)
                .CLKSRC          = LL_RCC_USART3_CLKSOURCE,
                #elif defined(ARCH_stm32h7)
                .CLKSRC          = LL_RCC_USART234578_CLKSOURCE,
                #endif
        },
        #endif
        #if defined(RCC_APB1ENR_UART4EN) || defined(RCC_APB1LENR_UART4EN)
        {
                .UART            = UART4,
                .IRQn            = UART4_IRQn,
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
                #if defined(ARCH_stm32f1) || defined(ARCH_stm32f4)
                .CLKSRC          = USART_CLKSOURCE_PCLK1,
                #elif defined(ARCH_stm32f3)
                .CLKSRC          = LL_RCC_UART4_CLKSOURCE,
                #elif defined(ARCH_stm32f7)
                .CLKSRC          = LL_RCC_UART4_CLKSOURCE,
                #elif defined(ARCH_stm32h7)
                .CLKSRC          = LL_RCC_USART234578_CLKSOURCE,
                #endif
        },
        #endif
        #if defined(RCC_APB1ENR_UART5EN) || defined(RCC_APB1LENR_UART5EN)
        {
                .UART            = UART5,
                .IRQn            = UART5_IRQn,
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
                #if defined(ARCH_stm32f1) || defined(ARCH_stm32f4)
                .CLKSRC          = USART_CLKSOURCE_PCLK1,
                #elif defined(ARCH_stm32f3)
                .CLKSRC          = LL_RCC_UART5_CLKSOURCE,
                #elif defined(ARCH_stm32f7)
                .CLKSRC          = LL_RCC_UART5_CLKSOURCE,
                #elif defined(ARCH_stm32h7)
                .CLKSRC          = LL_RCC_USART234578_CLKSOURCE,
                #endif
        },
        #endif
        #if defined(RCC_APB2ENR_USART6EN)
        {
                .UART            = USART6,
                .IRQn            = USART6_IRQn,
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
                #if defined(ARCH_stm32f1) || defined(ARCH_stm32f4)
                .CLKSRC          = USART_CLKSOURCE_PCLK2,
                #elif defined(ARCH_stm32f3)
                .CLKSRC          = LL_RCC_USART6_CLKSOURCE,
                #elif defined(ARCH_stm32f7)
                .CLKSRC          = LL_RCC_USART6_CLKSOURCE,
                #elif defined(ARCH_stm32h7)
                .CLKSRC          = LL_RCC_USART16_CLKSOURCE,
                #endif
        },
        #endif
        #if defined(RCC_APB1ENR_UART7EN) || defined(RCC_APB1LENR_UART7EN)
        {
                .UART            = UART7,
                .IRQn            = UART7_IRQn,
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
                #if defined(ARCH_stm32f1) || defined(ARCH_stm32f4)
                .CLKSRC          = USART_CLKSOURCE_PCLK1,
                #elif defined(ARCH_stm32f3)
                .CLKSRC          = LL_RCC_UART7_CLKSOURCE,
                #elif defined(ARCH_stm32f7)
                .CLKSRC          = LL_RCC_UART7_CLKSOURCE,
                #elif defined(ARCH_stm32h7)
                .CLKSRC          = LL_RCC_USART234578_CLKSOURCE,
                #endif
        },
        #endif
        #if defined(RCC_APB1ENR_UART8EN) || defined(RCC_APB1LENR_UART8EN)
        {
                .UART            = UART8,
                .IRQn            = UART8_IRQn,
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
                #if defined(ARCH_stm32f1) || defined(ARCH_stm32f4)
                .CLKSRC          = USART_CLKSOURCE_PCLK1,
                #elif defined(ARCH_stm32f3)
                .CLKSRC          = LL_RCC_UART8_CLKSOURCE,
                #elif defined(ARCH_stm32f7)
                .CLKSRC          = LL_RCC_UART8_CLKSOURCE,
                #elif defined(ARCH_stm32h7)
                .CLKSRC          = LL_RCC_USART234578_CLKSOURCE,
                #endif
        },
        #endif
        #if defined(RCC_APB2ENR_UART9EN)
        {
                .UART            = UART9,
                .IRQn            = UART9_IRQn,
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
                #if defined(ARCH_stm32f1) || defined(ARCH_stm32f4)
                .CLKSRC          = USART9_CLKSOURCE_PCLK1,
                #elif defined(ARCH_stm32f3)
                .CLKSRC          = LL_RCC_UART9_CLKSOURCE,
                #elif defined(ARCH_stm32f7)
                .CLKSRC          = LL_RCC_UART9_CLKSOURCE,
                #elif defined(ARCH_stm32h7)
                .CLKSRC          = LL_RCC_USART16910_CLKSOURCE,
                #endif
        },
        #endif
        #if defined(RCC_APB2ENR_UART10EN) || defined(RCC_APB2ENR_USART10EN)
        {
                .UART            = UART10,
                .IRQn            = UART10_IRQn,
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
                #if defined(ARCH_stm32f1) || defined(ARCH_stm32f4)
                .CLKSRC          = USART_CLKSOURCE_PCLK1,
                #elif defined(ARCH_stm32f3)
                .CLKSRC          = LL_RCC_UART10_CLKSOURCE,
                #elif defined(ARCH_stm32f7)
                .CLKSRC          = LL_RCC_UART10_CLKSOURCE,
                #elif defined(ARCH_stm32h7)
                .CLKSRC          = LL_RCC_USART16910_CLKSOURCE,
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
                NVIC_SetPriority(UART[major].IRQn, _UART_IRQ_PRIORITY);

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
        sys_critical_section_begin();
        SET_BIT(UART[major].UART->CR1, USART_CR1_TCIE);
        sys_critical_section_end();
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
        sys_critical_section_begin();
        CLEAR_BIT(UART[major].UART->CR1, USART_CR1_TCIE);
        sys_critical_section_end();
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
        sys_critical_section_begin();
        SET_BIT(UART[major].UART->CR1, USART_CR1_RXNEIE);
        sys_critical_section_end();
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
        sys_critical_section_begin();
        CLEAR_BIT(UART[major].UART->CR1, USART_CR1_RXNEIE);
        sys_critical_section_end();
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
        u32_t PCLK = 0;

        /* set baud */
#if defined(ARCH_stm32f1) || defined(ARCH_stm32f4)
        LL_RCC_ClocksTypeDef freq;
        LL_RCC_GetSystemClocksFreq(&freq);

        switch (DEV->CLKSRC) {
        case USART_CLKSOURCE_PCLK1:
                PCLK = freq.PCLK1_Frequency;
                break;

        case USART_CLKSOURCE_PCLK2:
                PCLK = freq.PCLK2_Frequency;
                break;
        default:
                printk("UART: invalid CLKSRC!");
                break;
        }
#elif defined(ARCH_stm32f7) || defined(ARCH_stm32f3)
        switch (DEV->CLKSRC) {
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
                PCLK = LL_RCC_GetUSARTClockFreq(DEV->CLKSRC);
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
                PCLK = LL_RCC_GetUARTClockFreq(DEV->CLKSRC);
                break;

        default:
                break;
        }
#elif defined(ARCH_stm32h7)
        PCLK = LL_RCC_GetUSARTClockFreq(DEV->CLKSRC);
#endif

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

        /* FIFO enable */
        #if defined(USART_CR1_FIFOEN)
        SET_BIT(DEV->UART->CR1, USART_CR1_FIFOEN);
        #endif

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
                u8_t DR = DEV->UART->RDR;

                if (_UART_FIFO__write(&_UART_mem[major]->Rx_FIFO, &DR)) {
                        received++;
                }
        }

        /* transmitter interrupt handler */
        if ((DEV->UART->CR1 & USART_CR1_TCIE) && (DEV->UART->SR & USART_SR_TC)) {

                if (_UART_mem[major]->Tx_buffer.data_size && _UART_mem[major]->Tx_buffer.src_ptr) {
                        DEV->UART->TDR = *(_UART_mem[major]->Tx_buffer.src_ptr++);

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
