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
#include "uart.h"
#include "uart_ioctl.h"
#include "efr32/uart_cfg.h"
#include "efr32/efr32xx.h"
#include "efr32/lib/em_cmu.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/* UART registers */
typedef struct {
        USART_TypeDef  *usart;
        uint32_t        enable_mask;
        const IRQn_Type rx_IRQn;
        const IRQn_Type tx_IRQn;
} UART_regs_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void USART_Reset(USART_TypeDef *usart);

/*==============================================================================
  Local object definitions
==============================================================================*/
// all registers which are need to control particular UART peripheral
static const UART_regs_t UART[] = {
        #if USART_COUNT >= 1
        {
                .usart           = USART0,
                .enable_mask     = CMU_HFPERCLKEN0_USART0,
                .rx_IRQn         = USART0_RX_IRQn,
                .tx_IRQn         = USART0_TX_IRQn,
        },
        #endif
        #if USART_COUNT >= 2
        {
                .usart           = USART1,
                .enable_mask     = CMU_HFPERCLKEN0_USART1,
                .rx_IRQn         = USART1_RX_IRQn,
                .tx_IRQn         = USART1_TX_IRQn,
        },
        #endif
        #if USART_COUNT >= 3
        {
                .usart           = USART2,
                .enable_mask     = CMU_HFPERCLKEN0_USART2,
                .rx_IRQn         = USART2_RX_IRQn,
                .tx_IRQn         = USART2_TX_IRQn,
        },
        #endif
        #if USART_COUNT >= 4
        {
                .usart           = USART3,
                .enable_mask     = CMU_HFPERCLKEN0_USART3,
                .rx_IRQn         = USART3_RX_IRQn,
                .tx_IRQn         = USART3_TX_IRQn,
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
int _UART_LLD__turn_on(u8_t major)
{
        if (!(CMU->HFPERCLKEN0 & UART[major].enable_mask)) {

                CMU_ClockEnable(cmuClock_HFPER, true);
                CMU_ClockEnable(cmuClock_USART0, true);

                USART_Reset(UART[major].usart);

                NVIC_EnableIRQ(UART[major].rx_IRQn);
                NVIC_EnableIRQ(UART[major].tx_IRQn);
                NVIC_SetPriority(UART[major].rx_IRQn, _CPU_IRQ_SAFE_PRIORITY_);
                NVIC_SetPriority(UART[major].tx_IRQn, _CPU_IRQ_SAFE_PRIORITY_);

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
        NVIC_DisableIRQ(UART[major].rx_IRQn);
        NVIC_DisableIRQ(UART[major].tx_IRQn);

        USART_Reset(UART[major].usart);
        CMU->HFPERCLKEN0 &= ~UART[major].enable_mask;

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
        u8_t data = *_UART_mem[major]->Tx_buffer.src_ptr++;
        _UART_mem[major]->Tx_buffer.data_size--;
        UART[major].usart->TXDATA = data;

        UART[major].usart->IEN |= USART_IEN_TXC;
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
        UART[major].usart->IEN &= ~USART_IEN_TXC;
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
        UART[major].usart->IEN |= USART_IEN_RXDATAV;
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
        UART[major].usart->IEN &= ~USART_IEN_RXDATAV;
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
        USART_TypeDef *usart = UART[major].usart;

        /* set 8 bit word length, parity and stop bits*/
        u32_t FRAME = USART_FRAME_DATABITS_EIGHT;

        switch (config->parity) {
        default:
        case UART_PARITY__OFF:  FRAME |= USART_FRAME_PARITY_NONE; break;
        case UART_PARITY__EVEN: FRAME |= USART_FRAME_PARITY_EVEN; break;
        case UART_PARITY__ODD:  FRAME |= USART_FRAME_PARITY_ODD;  break;
        }

        switch (config->stop_bits) {
        default:
        case UART_STOP_BIT__1: FRAME |= USART_FRAME_STOPBITS_ONE; break;
        case UART_STOP_BIT__2: FRAME |= USART_FRAME_STOPBITS_TWO; break;
        }

        usart->FRAME = FRAME;

        /* set baud */
        u32_t freq = CMU_ClockFreqGet(cmuClock_HFPER);

        /* Calculate and set CLKDIV with fractional bits.
         * The addend (oversample*baudrate)/2 in the first line is to round the
         * divisor up by half the divisor before the division in order to reduce the
         * integer division error, which consequently results in a higher baudrate
         * than desired. */
        u32_t       clkdiv     = 0;
        const u32_t oversample = 16;

#if defined(_USART_CLKDIV_DIV_MASK) && (_USART_CLKDIV_DIV_MASK >= 0x7FFFF8UL)
        clkdiv = 32 * freq + (oversample * config->baud) / 2;
        clkdiv /= (oversample * config->baud);
        clkdiv -= 32;
        clkdiv *= 8;
#else
        clkdiv = 4 * freq + (oversample * config->baud) / 2;
        clkdiv /= (oversample * config->baud);
        clkdiv -= 4;
        clkdiv *= 64;
#endif

        clkdiv &= _USART_CLKDIV_DIV_MASK;

        usart->CTRL  &= ~_USART_CTRL_OVS_MASK;
        usart->CTRL  |= oversample << _USART_CTRL_OVS_SHIFT;
        usart->CLKDIV = clkdiv;

        /* single wire mode */
        if (config->single_wire_mode) {
                usart->CTRL |= USART_CTRL_LOOPBK;
        } else {
                usart->CTRL &= USART_CTRL_LOOPBK;
        }

        /* transmitter enable */
        if (config->tx_enable) {
                usart->ROUTEPEN  |= USART_ROUTEPEN_TXPEN;
                usart->ROUTELOC0 &= _USART_ROUTELOC0_TXLOC_MASK;
                usart->ROUTELOC0 |= ((_UART_TX_PIN_LOC & 0x3F) << _USART_ROUTELOC0_TXLOC_SHIFT);
        } else {
                usart->ROUTEPEN  &= ~USART_ROUTEPEN_TXPEN;
        }

        /* receiver enable */
        if (config->rx_enable) {
                usart->ROUTEPEN  |= USART_ROUTEPEN_RXPEN;
                usart->ROUTELOC0 &= _USART_ROUTELOC0_RXLOC_MASK;
                usart->ROUTELOC0 |= ((_UART_RX_PIN_LOC & 0x3F) << _USART_ROUTELOC0_RXLOC_SHIFT);
        } else {
                usart->ROUTEPEN  &= ~USART_ROUTEPEN_RXPEN;
        }

        /* hardware flow control */
        if (config->hardware_flow_ctrl) {
                usart->ROUTELOC1 &= _USART_ROUTELOC1_RTSLOC_MASK | _USART_ROUTELOC1_CTSLOC_MASK;

                usart->ROUTELOC1 |= ((_UART_RTS_PIN_LOC & 0x3F) << _USART_ROUTELOC1_RTSLOC_SHIFT);
                usart->ROUTELOC1 |= ((_UART_CTS_PIN_LOC & 0x3F) << _USART_ROUTELOC1_CTSLOC_SHIFT);

                usart->ROUTEPEN  |= USART_ROUTEPEN_RTSPEN | USART_ROUTEPEN_CTSPEN;

        } else {
                usart->ROUTEPEN  &= ~(USART_ROUTEPEN_RTSPEN | USART_ROUTEPEN_CTSPEN);
        }

        _UART_LLD__rx_resume(major);

        usart->CMD |= (config->tx_enable ? USART_CMD_TXEN : 0)
                    | (config->rx_enable ? USART_CMD_RXEN : 0);
}

//==============================================================================
/**
 * @brief Function configure selected UART.
 *
 * @param major         major device number
 * @param config        configuration structure
 */
//==============================================================================
static void USART_Reset(USART_TypeDef *usart)
{
        /* Make sure disabled first, before resetting other registers */
        usart->CMD = USART_CMD_RXDIS
                   | USART_CMD_TXDIS
                   | USART_CMD_MASTERDIS
                   | USART_CMD_RXBLOCKDIS
                   | USART_CMD_TXTRIDIS
                   | USART_CMD_CLEARTX
                   | USART_CMD_CLEARRX;

        usart->CTRL      = _USART_CTRL_RESETVALUE;
        usart->FRAME     = _USART_FRAME_RESETVALUE;
        usart->TRIGCTRL  = _USART_TRIGCTRL_RESETVALUE;
        usart->CLKDIV    = _USART_CLKDIV_RESETVALUE;
        usart->IEN       = _USART_IEN_RESETVALUE;
        usart->IFC       = _USART_IFC_MASK;

#if defined(_USART_ROUTEPEN_MASK) || defined(_UART_ROUTEPEN_MASK)
        usart->ROUTEPEN  = _USART_ROUTEPEN_RESETVALUE;
        usart->ROUTELOC0 = _USART_ROUTELOC0_RESETVALUE;
        usart->ROUTELOC1 = _USART_ROUTELOC1_RESETVALUE;
#else
        usart->ROUTE     = _USART_ROUTE_RESETVALUE;
#endif

#if defined(_USART_INPUT_RESETVALUE)
        usart->INPUT     = _USART_INPUT_RESETVALUE;
#endif

#if defined(_USART_I2SCTRL_RESETVALUE)
        usart->I2SCTRL   = _USART_I2SCTRL_RESETVALUE;
#endif
}

//==============================================================================
/**
 * @brief Interrupt Rx handling
 *
 * @param major         major device number
 */
//==============================================================================
static void IRQ_Rx_handle(u8_t major)
{
        USART_TypeDef *usart = UART[major].usart;

        /* receiver interrupt handler */
        int received = 0;
        while (usart->STATUS & USART_STATUS_RXDATAV) {
                u8_t data = usart->RXDATA;

                if (_UART_FIFO__write(&_UART_mem[major]->Rx_FIFO, &data)) {
                        received++;
                }
        }

        // set receive semaphore to number of received bytes
        bool yield = received > 0;

        while (received--) {
                sys_semaphore_signal_from_ISR(_UART_mem[major]->data_read_sem, NULL);
        }

        /* yield thread if data received */
        sys_thread_yield_from_ISR(yield);
}

//==============================================================================
/**
 * @brief Interrupt Tx handling
 *
 * @param major         major device number
 */
//==============================================================================
static void IRQ_Tx_handle(u8_t major)
{
        USART_TypeDef *usart = UART[major].usart;

        /* transmitter interrupt handler */
        if (usart->STATUS & USART_STATUS_TXC) {

                if (_UART_mem[major]->Tx_buffer.data_size && _UART_mem[major]->Tx_buffer.src_ptr) {
                        usart->TXDATA = *(_UART_mem[major]->Tx_buffer.src_ptr++);

                        if (--_UART_mem[major]->Tx_buffer.data_size == 0) {
                                _UART_mem[major]->Tx_buffer.src_ptr = NULL;
                        }
                } else {
                        usart->IEN &= ~USART_IEN_TXC;
                        sys_semaphore_signal_from_ISR(_UART_mem[major]->write_ready_sem, NULL);
                }

                /* yield thread if data send */
                sys_thread_yield_from_ISR(true);
        }
}

#if USART_COUNT >= 1
//==============================================================================
/**
 * @brief USART0 Rx Interrupt
 */
//==============================================================================
void USART0_RX_Handler(void)
{
        IRQ_Rx_handle(0);
}
#endif

#if USART_COUNT >= 1
//==============================================================================
/**
 * @brief USART0 Tx Interrupt
 */
//==============================================================================
void USART0_TX_Handler(void)
{
        IRQ_Tx_handle(0);
}
#endif

#if USART_COUNT >= 2
//==============================================================================
/**
 * @brief USART1 Rx Interrupt
 */
//==============================================================================
void USART1_RX_Handler(void)
{
        IRQ_Rx_handle(1);
}
#endif

#if USART_COUNT >= 2
//==============================================================================
/**
 * @brief USART1 Tx Interrupt
 */
//==============================================================================
void USART1_TX_Handler(void)
{
        IRQ_Tx_handle(1);
}
#endif

#if USART_COUNT >= 3
//==============================================================================
/**
 * @brief USART2 Rx Interrupt
 */
//==============================================================================
void USART2_RX_Handler(void)
{
        IRQ_Rx_handle(2);
}
#endif

#if USART_COUNT >= 3
//==============================================================================
/**
 * @brief USART2 Tx Interrupt
 */
//==============================================================================
void USART2_TX_Handler(void)
{
        IRQ_Tx_handle(2);
}
#endif

#if USART_COUNT >= 4
//==============================================================================
/**
 * @brief USART3 Rx Interrupt
 */
//==============================================================================
void USART3_RX_Handler(void)
{
        IRQ_Rx_handle(3);
}
#endif

#if USART_COUNT >= 4
//==============================================================================
/**
 * @brief USART3 Tx Interrupt
 */
//==============================================================================
void USART3_TX_Handler(void)
{
        IRQ_Tx_handle(3);
}
#endif

#endif
/*==============================================================================
  End of file
==============================================================================*/
