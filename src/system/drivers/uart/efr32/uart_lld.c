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
#if defined(ARCH_efr32)
#include "efr32/uart_lld.h"
#include "efr32/uart_cfg.h"
#include "efr32/efr32xx.h"
#include "efr32/lib/em_cmu.h"
#include "../uart_ioctl.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/* UART registers */
typedef struct { // FIXME
        USART_TypeDef  *UART;
        __IO uint32_t  *APBENR;
        __IO uint32_t  *APBRSTR;
        const uint32_t  APBENR_UARTEN;
        const uint32_t  APBRSTR_UARTRST;
        const IRQn_Type IRQn;
        const u32_t     PRIORITY;
} UART_regs_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
// all registers which are need to control particular UART peripheral
static const UART_regs_t UART[] = { // FIXME
        #if USART_COUNT >= 1
        {
                .UART            = USART1,
                .APBENR          = &RCC->APB2ENR,
                .APBRSTR         = &RCC->APB2RSTR,
                .APBENR_UARTEN   = RCC_APB2ENR_USART1EN,
                .APBRSTR_UARTRST = RCC_APB2RSTR_USART1RST,
                .IRQn            = USART1_IRQn,
                .PRIORITY        = _UART1_IRQ_PRIORITY
        },
        #endif
        #if USART_COUNT >= 2
        {
                .UART            = USART2,
                .APBENR          = &RCC->APB1ENR,
                .APBRSTR         = &RCC->APB1RSTR,
                .APBENR_UARTEN   = RCC_APB1ENR_USART2EN,
                .APBRSTR_UARTRST = RCC_APB1RSTR_USART2RST,
                .IRQn            = USART2_IRQn,
                .PRIORITY        = _UART2_IRQ_PRIORITY
        },
        #endif
        #if USART_COUNT >= 3
        {
                .UART            = USART3,
                .APBENR          = &RCC->APB1ENR,
                .APBRSTR         = &RCC->APB1RSTR,
                .APBENR_UARTEN   = RCC_APB1ENR_USART3EN,
                .APBRSTR_UARTRST = RCC_APB1RSTR_USART3RST,
                .IRQn            = USART3_IRQn,
                .PRIORITY        = _UART3_IRQ_PRIORITY
        },
        #endif
        #if USART_COUNT >= 4
        {
                .UART            = UART4,
                .APBENR          = &RCC->APB1ENR,
                .APBRSTR         = &RCC->APB1RSTR,
                .APBENR_UARTEN   = RCC_APB1ENR_UART4EN,
                .APBRSTR_UARTRST = RCC_APB1RSTR_UART4RST,
                .IRQn            = UART4_IRQn,
                .PRIORITY        = _UART4_IRQ_PRIORITY
        },
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
int _UART_LLD__turn_on(u8_t major) // TODO
{
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
int _UART_LLD__turn_off(u8_t major) // TODO
{
}

//==============================================================================
/**
 * @brief Function transmit currently setup buffer.
 *
 * @param major         UART number
 */
//==============================================================================
void _UART_LLD__transmit(u8_t major) // TODO
{
}

//==============================================================================
/**
 * @brief Function abort pending transmission.
 *
 * @param major         UART number
 */
//==============================================================================
void _UART_LLD__abort_trasmission(u8_t major) // TODO
{
}

//==============================================================================
/**
 * @brief Function resume byte receiving.
 *
 * @param major         UART number
 */
//==============================================================================
void _UART_LLD__rx_resume(u8_t major) // TODO
{
}

//==============================================================================
/**
 * @brief Function hold byte receiving.
 *
 * @param major         UART number
 */
//==============================================================================
void _UART_LLD__rx_hold(u8_t major) // TODO
{
}

//==============================================================================
/**
 * @brief Function configure selected UART.
 *
 * @param major         major device number
 * @param config        configuration structure
 */
//==============================================================================
void _UART_LLD__configure(u8_t major, const struct UART_config *config) // TODO
{
}

//==============================================================================
/**
 * @brief Interrupt Rx handling
 *
 * @param major         major device number
 */
//==============================================================================
static void IRQ_Rx_handle(u8_t major) // TODO
{

}

//==============================================================================
/**
 * @brief Interrupt Tx handling
 *
 * @param major         major device number
 */
//==============================================================================
static void IRQ_Tx_handle(u8_t major) // TODO
{

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
#if defined(RCC_APB1ENR_UART7EN)
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
#if defined(RCC_APB1ENR_UART8EN)
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
#if defined(RCC_APB2ENR_UART10EN)
void UART10_IRQHandler(void)
{
        IRQ_handle(_UART10);
}
#endif

#endif
/*==============================================================================
  End of file
==============================================================================*/
