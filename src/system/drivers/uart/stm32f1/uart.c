/*=========================================================================*//**
@file    usart.c

@author  Daniel Zorychta

@brief   This file support USART peripherals

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
#include "drivers/uart.h"
#include "stm32f1/stm32f10x.h"

MODULE_NAME(UART);

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define MTX_BLOCK_TIME                          0
#define TXC_WAIT_TIME                           60000

#define force_lock_recursive_mutex(mtx)         while (lock_recursive_mutex(mtx, 10) != MUTEX_LOCKED)

/** IRQ priorities */
#define IRQ_PRIORITY                            0xDF

/** translation of configuration bits to function-like macros */
#define wakeup_USART_on_address_mark(usart)     usart->CR1 |=  USART_CR1_WAKE
#define wakeup_USART_on_idle_line(usart)        usart->CR1 &= ~USART_CR1_WAKE
#define enable_parity_check(usart)              usart->CR1 |=  USART_CR1_PCE
#define disable_parity_check(usart)             usart->CR1 &= ~USART_CR1_PCE
#define enable_odd_parity(usart)                usart->CR1 |=  USART_CR1_PS
#define enable_even_parity(usart)               usart->CR1 &= ~USART_CR1_PS
#define enable_transmitter(usart)               usart->CR1 |=  USART_CR1_TE
#define disable_transmitter(usart)              usart->CR1 &= ~USART_CR1_TE
#define enable_receiver(usart)                  usart->CR1 |=  USART_CR1_RE
#define disable_receiver(usart)                 usart->CR1 &= ~USART_CR1_RE
#define receiver_wakeup_in_mute_mode(usart)     usart->CR1 |=  USART_CR1_RWU
#define receiver_wakeup_in_active_mode(usart)   usart->CR1 &= ~USART_CR1_RWU
#define enable_LIN_mode(usart)                  usart->CR2 |=  USART_CR2_LINEN
#define disable_LIN_mode(usart)                 usart->CR2 &= ~USART_CR2_LINEN
#define set_1_stop_bit(usart)                   usart->CR2 &= ~USART_CR2_STOP
#define set_2_stop_bits(usart)                  while (0) {usart->CR2 &= ~USART_CR2_STOP; usart->CR2 |= USART_CR2_STOP_1;}
#define detect_10_bit_LIN_break(usart)          usart->CR2 &= ~USART_CR2_LBDL
#define detect_11_bit_LIN_break(usart)          usart->CR2 |=  USART_CR2_LBDL
#define set_address_node(usart, adr)            while (0) {usart->CR2 &= ~USART_CR2_ADD; usart->CR2 |= (adr & USART_CR2_ADD);}
#define set_baud_rate(usart, clk, baud)         usart->BRR = (u16_t)(((u32_t)clk / (u32_t)baud) + 1)
#define enable_CTS(usart)                       usart->CR3 |=  USART_CR3_CTSE
#define disable_CTS(usart)                      usart->CR3 &= ~USART_CR3_CTSE
#define enable_RTS(usart)                       usart->CR3 |=  USART_CR3_RTSE
#define disable_RTS(usart)                      usart->CR3 &= ~USART_CR3_RTSE
#define enable_USART(usart)                     usart->CR1 |=  USART_CR1_UE
#define disable_USART(usart)                    usart->CR1 &= ~UART_CR1_UE1
#define enable_RXNE_IRQ(usart)                  usart->CR1 |=  USART_CR1_RXNEIE
#define disable_RXNE_IRQ(usart)                 usart->CR1 &= ~UART_CR1_RXNEIE
#define enable_TXE_IRQ(usart)                   usart->CR1 |=  USART_CR1_TXEIE
#define disable_TXE_IRQ(usart)                  usart->CR1 &= ~USART_CR1_TXEIE

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/** USART handling structure */
struct USART_data {
        struct Rx_FIFO {
                u8_t  buffer[UART_RX_BUFFER_SIZE];
                u16_t buffer_level;
                u16_t read_index;
                u16_t write_index;
        } Rx_FIFO;

        struct Tx_buffer {
                const u8_t *src_ptr;
                size_t     data_size;
        } Tx_buffer;

        sem_t   *data_write_sem;
        mutex_t *port_lock_mtx;
        task_t  *task;
        USART_t *USART;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static stdret_t turn_on_USART(USART_t *USART);
static stdret_t turn_off_USART(USART_t *USART);
static stdret_t enable_USART_interrupts(USART_t *USART);
static void IRQ_handler(struct USART_data *USART_data);

/*==============================================================================
  Local object definitions
==============================================================================*/
/* addresses of UART devices */
static USART_t *const USART_peripherals[UART_DEV_COUNT] = {
#if defined(RCC_APB2ENR_USART1EN) && (UART_1_ENABLE > 0)
        USART1,
#endif
#if defined(RCC_APB1ENR_USART2EN) && (UART_2_ENABLE > 0)
        USART2,
#endif
#if defined(RCC_APB1ENR_USART3EN) && (UART_3_ENABLE > 0)
        USART3,
#endif
#if defined(RCC_APB1ENR_UART4EN)  && (UART_4_ENABLE > 0)
        UART4,
#endif
#if defined(RCC_APB1ENR_UART5EN)  && (UART_5_ENABLE > 0)
        UART5,
#endif
};

/* structure which identify USARTs data in the IRQs */
static struct USART_data *USART_data[UART_DEV_COUNT];

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize USART devices
 *
 * @param[out] **drvhdl         driver's memory handler
 * @param[in]  dev              device number
 * @param[in]  part             device part
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t UART_init(void **drvhdl, uint dev, uint part)
{
        (void)part;

        if (!drvhdl || dev >= UART_DEV_COUNT) {
                return STD_RET_ERROR;
        }

        if (!(USART_data[dev] = calloc(1, sizeof(struct USART_data)))) {
                return STD_RET_ERROR;
        }

        *drvhdl = USART_data[dev];

        USART_data[dev]->USART = USART_peripherals[dev];
        USART_data[dev]->data_write_sem = new_semaphore();
        USART_data[dev]->port_lock_mtx = new_recursive_mutex();

        if (!USART_data[dev]->data_write_sem || !USART_data[dev]->port_lock_mtx) {
                goto error;
        } else {
                take_semaphore(USART_data[dev]->data_write_sem, 1);

                if (turn_on_USART(USART_data[dev]->USART) != STD_RET_OK) {
                        goto error;
                }

                enable_USART(USART_data[dev]->USART);

                if ((u32_t) USART_data[dev]->USART == USART1_BASE) {
                        set_baud_rate(USART_data[dev]->USART, UART_PCLK2_FREQ, UART_DEFAULT_BAUDRATE);
                } else {
                        set_baud_rate(USART_data[dev]->USART, UART_PCLK1_FREQ, UART_DEFAULT_BAUDRATE);
                }

                if (UART_DEFAULT_WAKE_METHOD) {
                        wakeup_USART_on_address_mark(USART_data[dev]->USART);
                } else {
                        wakeup_USART_on_idle_line(USART_data[dev]->USART);
                }

                if (UART_DEFAULT_PARITY_ENABLE) {
                        enable_parity_check(USART_data[dev]->USART);
                } else {
                        disable_parity_check(USART_data[dev]->USART);
                }

                if (UART_DEFAULT_PARITY_SELECTION) {
                        enable_odd_parity(USART_data[dev]->USART);
                } else {
                        enable_even_parity(USART_data[dev]->USART);
                }

                if (UART_DEFAULT_TX_ENABLE) {
                        enable_transmitter(USART_data[dev]->USART);
                } else {
                        disable_transmitter(USART_data[dev]->USART);
                }

                if (UART_DEFAULT_RX_ENABLE) {
                        enable_receiver(USART_data[dev]->USART);
                } else {
                        disable_receiver(USART_data[dev]->USART);
                }

                if (UART_DEFAULT_RX_WAKEUP_MODE) {
                        receiver_wakeup_in_mute_mode(USART_data[dev]->USART);
                } else {
                        receiver_wakeup_in_active_mode(USART_data[dev]->USART);
                }

                if (UART_DEFAULT_LIN_ENABLE) {
                        enable_LIN_mode(USART_data[dev]->USART);
                } else {
                        disable_LIN_mode(USART_data[dev]->USART);
                }

                if (UART_DEFAULT_STOP_BITS) {
                        set_2_stop_bits(USART_data[dev]->USART);
                } else {
                        set_1_stop_bit(USART_data[dev]->USART);
                }

                if (UART_DEFAULT_LIN_BREAK_LEN_DET) {
                        detect_11_bit_LIN_break(USART_data[dev]->USART);
                } else {
                        detect_10_bit_LIN_break(USART_data[dev]->USART);
                }

                if (UART_DEFAULT_CTS_ENABLE) {
                        enable_CTS(USART_data[dev]->USART);
                } else {
                        disable_CTS(USART_data[dev]->USART);
                }

                if (UART_DEFAULT_RTS_ENABLE) {
                        enable_RTS(USART_data[dev]->USART);
                } else {
                        disable_RTS(USART_data[dev]->USART);
                }

                set_address_node(USART_data[dev]->USART, UART_DEFAULT_MULTICOM_ADDRESS);

                if (enable_USART_interrupts(USART_data[dev]->USART) != STD_RET_OK) {
                        goto error;
                }

                enable_RXNE_IRQ(USART_data[dev]->USART);

                return STD_RET_OK;
        }

error:
        if (USART_data[dev]->data_write_sem) {
                delete_semaphore(USART_data[dev]->data_write_sem);
        }

        if (USART_data[dev]->port_lock_mtx) {
                delete_recursive_mutex(USART_data[dev]->port_lock_mtx);
        }

        free(USART_data[dev]);
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Release USART devices
 *
 * @param[in] *drvhdl           driver's memory handler
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t UART_release(void *drvhdl)
{
        struct USART_data *hdl = drvhdl;

        if (!hdl) {
                return STD_RET_ERROR;
        }

        force_lock_recursive_mutex(hdl->port_lock_mtx);
        enter_critical();
        unlock_recursive_mutex(hdl->port_lock_mtx);
        delete_recursive_mutex(hdl->port_lock_mtx);
        delete_semaphore(hdl->data_write_sem);
        turn_off_USART(hdl->USART);
        free(hdl);
        exit_critical();

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Opens specified port
 *
 * @param[in] *drvhdl           driver's memory handler
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t UART_open(void *drvhdl)
{
        struct USART_data *hdl = drvhdl;

        if (!hdl) {
                return STD_RET_ERROR;
        }

        if (lock_recursive_mutex(hdl->port_lock_mtx, MTX_BLOCK_TIME) == MUTEX_LOCKED) {
                hdl->task = get_task_handle();
                return STD_RET_OK;
        } else {
                return STD_RET_ERROR;
        }
}

//==============================================================================
/**
 * @brief Function close opened port
 *
 * @param[in] *drvhdl           driver's memory handler
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t UART_close(void *drvhdl)
{
        struct USART_data *hdl = drvhdl;

        if (!hdl) {
                return STD_RET_ERROR;
        }

        if (lock_recursive_mutex(hdl->port_lock_mtx, MTX_BLOCK_TIME) == MUTEX_LOCKED) {
                hdl->task = NULL;
                unlock_recursive_mutex(hdl->port_lock_mtx);     /* give this mutex */
                unlock_recursive_mutex(hdl->port_lock_mtx);     /* give mutex from open */
                return STD_RET_OK;
        } else {
                return STD_RET_ERROR;
        }
}

//==============================================================================
/**
 * @brief Write data to UART (ISR or DMA)
 *
 * @param[in] *drvhdl           driver's memory handle
 * @param[in] *src              source
 * @param[in] size              size
 * @param[in] nitems            n items to read
 * @param[in] lseek             lseek
 *
 * @retval number of written nitems
 */
//==============================================================================
size_t UART_write(void *drvhdl, const void *src, size_t size, size_t nitems, u64_t lseek)
{
        (void) lseek;

        struct USART_data *hdl = drvhdl;
        size_t n = 0;

        if (!hdl) {
                return n;
        }

        if (!src || !size || !nitems) {
                return n;
        }

        if (lock_recursive_mutex(hdl->port_lock_mtx, MTX_BLOCK_TIME) == MUTEX_LOCKED) {
                hdl->Tx_buffer.src_ptr   = src;
                hdl->Tx_buffer.data_size = size * nitems;

                enable_TXE_IRQ(hdl->USART);
                take_semaphore(hdl->data_write_sem, TXC_WAIT_TIME);

                n = nitems;

                unlock_recursive_mutex(hdl->port_lock_mtx);
        }

        return n;
}

//==============================================================================
/**
 * @brief Read data from UART Rx buffer
 *
 * @param[in]  *drvhdl          driver's memory handle
 * @param[out] *dst             destination
 * @param[in]  size             size
 * @param[in]  nitems           n items to read
 * @param[in]  lseek            seek
 *
 * @retval number of read nitems
 */
//==============================================================================
size_t UART_read(void *drvhdl, void *dst, size_t size, size_t nitems, u64_t lseek)
{
        (void) lseek;

        struct USART_data *hdl = drvhdl;
        size_t n = 0;
        size_t data_size;
        u8_t   *dst_ptr;

        if (!hdl) {
                return n;
        }

        if (!dst || !size || !nitems) {
                return n;
        }

        if (lock_recursive_mutex(hdl->port_lock_mtx, MTX_BLOCK_TIME) == MUTEX_LOCKED) {
                dst_ptr   = (u8_t *)dst;
                data_size = nitems * size;

                do {
                        enter_critical();

                        if (hdl->Rx_FIFO.buffer_level > 0) {
                                *dst_ptr = hdl->Rx_FIFO.buffer[hdl->Rx_FIFO.read_index++];

                                if (hdl->Rx_FIFO.read_index >= UART_RX_BUFFER_SIZE)
                                        hdl->Rx_FIFO.read_index = 0;

                                hdl->Rx_FIFO.buffer_level--;
                                data_size--;
                                n++;

                                exit_critical();
                        } else {
                                exit_critical();
                                suspend_this_task();
                        }
                } while (data_size);

                n /= size;

                unlock_recursive_mutex(hdl->port_lock_mtx);
        }

        return n;
}

//==============================================================================
/**
 * @brief Direct IO control
 *
 * @param[in]     *drvhdl       driver's memory handle
 * @param[in]     iorq          IO reqest
 * @param[in,out] args          additional arguments
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t UART_ioctl(void *drvhdl, int iorq, va_list args)
{
        struct USART_data *hdl = drvhdl;
        stdret_t status = STD_RET_OK;
        u8_t *out_ptr;

        if (!hdl) {
                return STD_RET_ERROR;
        }

        if (lock_recursive_mutex(hdl->port_lock_mtx, MTX_BLOCK_TIME) == MUTEX_LOCKED) {
                switch (iorq) {
                case UART_IORQ_ENABLE_WAKEUP_IDLE:
                        wakeup_USART_on_idle_line(hdl->USART);
                        break;

                case UART_IORQ_ENABLE_WAKEUP_ADDRESS_MARK:
                        wakeup_USART_on_address_mark(hdl->USART);
                        break;

                case UART_IORQ_ENABLE_PARITY_CHECK:
                        enable_parity_check(hdl->USART);
                        break;

                case UART_IORQ_DISABLE_PARITY_CHECK:
                        disable_parity_check(hdl->USART);
                        break;

                case UART_IORQ_SET_ODD_PARITY:
                        enable_odd_parity(hdl->USART);
                        break;

                case UART_IORQ_SET_EVEN_PARITY:
                        enable_even_parity(hdl->USART);
                        break;

                case UART_IORQ_ENABLE_RECEIVER_WAKEUP_MUTE:
                        receiver_wakeup_in_mute_mode(hdl->USART);
                        break;

                case UART_IORQ_DISABLE_RECEIVER_WAKEUP_MUTE:
                        receiver_wakeup_in_active_mode(hdl->USART);
                        break;

                case UART_IORQ_ENABLE_LIN_MODE:
                        enable_LIN_mode(hdl->USART);
                        break;

                case UART_IORQ_DISABLE_LIN_MODE:
                        disable_LIN_mode(hdl->USART);
                        break;

                case UART_IORQ_SET_1_STOP_BIT:
                        set_1_stop_bit(hdl->USART);
                        break;

                case UART_IORQ_SET_2_STOP_BITS:
                        set_2_stop_bits(hdl->USART);
                        break;

                case UART_IORQ_SET_LIN_BRK_DETECTOR_11_BITS:
                        detect_11_bit_LIN_break(hdl->USART);
                        break;

                case UART_IORQ_SET_LIN_BRK_DETECTOR_10_BITS:
                        detect_10_bit_LIN_break(hdl->USART);
                        break;

                case UART_IORQ_SET_ADDRESS_NODE:
                        set_address_node(hdl->USART, va_arg(args, int));
                        break;

                case UART_IORQ_ENABLE_CTS:
                        enable_CTS(hdl->USART);
                        break;

                case UART_IORQ_DISABLE_CTS:
                        disable_CTS(hdl->USART);
                        break;

                case UART_IORQ_ENABLE_RTS:
                        enable_RTS(hdl->USART);
                        break;

                case UART_IORQ_DISABLE_RTS:
                        disable_RTS(hdl->USART);
                        break;

                case UART_IORQ_GET_BYTE:
                        enter_critical();

                        if (!(out_ptr = va_arg(args, u8_t*))) {
                                exit_critical();
                                status = STD_RET_ERROR;
                                break;
                        }

                        if (hdl->Rx_FIFO.buffer_level > 0) {
                                *out_ptr = hdl->Rx_FIFO.buffer[hdl->Rx_FIFO.read_index++];

                                if (hdl->Rx_FIFO.read_index >= UART_RX_BUFFER_SIZE)
                                        hdl->Rx_FIFO.read_index = 0;

                                hdl->Rx_FIFO.buffer_level--;
                        } else {
                                status = STD_RET_ERROR;
                        }

                        exit_critical();
                        break;

                case UART_IORQ_GET_BYTE_BLOCKING:
                        while (TRUE) {
                                enter_critical();

                                if (!(out_ptr = va_arg(args, u8_t*))) {
                                        exit_critical();
                                        status = STD_RET_ERROR;
                                        break;
                                }

                                if (hdl->Rx_FIFO.buffer_level > 0) {
                                        *out_ptr = hdl->Rx_FIFO.buffer[hdl->Rx_FIFO.read_index++];

                                        if (hdl->Rx_FIFO.read_index >= UART_RX_BUFFER_SIZE)
                                                hdl->Rx_FIFO.read_index = 0;

                                        hdl->Rx_FIFO.buffer_level--;

                                        exit_critical();
                                        break;
                                } else {
                                        exit_critical();
                                        suspend_this_task();
                                }
                        }
                        break;

                case UART_IORQ_SEND_BYTE:
                        while (!(hdl->USART->SR & USART_SR_TXE)) {
                                sleep_ms(1);
                        }

                        hdl->USART->DR = va_arg(args, int);
                        break;

                case UART_IORQ_SET_BAUDRATE:
                        if ((u32_t)hdl->USART == USART1_BASE) {
                                set_baud_rate(hdl->USART, UART_PCLK2_FREQ, va_arg(args, int));
                        } else {
                                set_baud_rate(hdl->USART, UART_PCLK1_FREQ, va_arg(args, int));
                        }
                        break;

                default:
                        status = STD_RET_ERROR;
                        break;
                }

                unlock_recursive_mutex(hdl->port_lock_mtx);
        } else {
                return STD_RET_ERROR;
        }

        return status;
}

//==============================================================================
/**
 * @brief Function flush device
 *
 * @param[in] *drvhdl           driver's memory handle
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t UART_flush(void *drvhdl)
{
        (void)drvhdl;

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Interrupt handling
 *
 * @param usart   usart address
 * @param dev     device number
 */
//==============================================================================
static void IRQ_handler(struct USART_data *USART_data)
{
        /* transmitter interrupt handler */
        if (USART_data->USART->SR & USART_SR_TXE) {
                if (USART_data->Tx_buffer.data_size && USART_data->Tx_buffer.src_ptr) {
                        USART_data->USART->DR = *(USART_data->Tx_buffer.src_ptr++);

                        if (--USART_data->Tx_buffer.data_size == 0) {
                                USART_data->Tx_buffer.src_ptr = NULL;

                                i32_t woke;
                                disable_TXE_IRQ(USART_data->USART);
                                give_semaphore_from_ISR(USART_data->data_write_sem, &woke);
                        }
                } else {
                        disable_TXE_IRQ(USART_data->USART);
                }
        }

        /* receiver interrupt handler */
        if (USART_data->USART->SR & USART_SR_RXNE) {
                u8_t DR = USART_data->USART->DR;

                if (USART_data->Rx_FIFO.buffer) {
                        if (USART_data->Rx_FIFO.buffer_level < UART_RX_BUFFER_SIZE) {
                                USART_data->Rx_FIFO.buffer[USART_data->Rx_FIFO.write_index++] = DR;

                        if (USART_data->Rx_FIFO.write_index >= UART_RX_BUFFER_SIZE)
                                USART_data->Rx_FIFO.write_index = 0;

                        USART_data->Rx_FIFO.buffer_level++;
                  }

                  if (USART_data->task) {
                        if (resume_task_from_ISR(USART_data->task) == pdTRUE) {
                                yield_task();
                        }
                  }
            }
        }
}

//==============================================================================
/**
 * @brief Function enable USART clock
 *
 * @param[in] *USART            peripheral address
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t turn_on_USART(USART_t *USART)
{
        switch ((u32_t)USART) {
#if defined(RCC_APB2ENR_USART1EN) && (UART_1_ENABLE > 0)
        case USART1_BASE:
                RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
                break;
#endif
#if defined(RCC_APB1ENR_USART2EN) && (UART_2_ENABLE > 0)
        case USART2_BASE:
                RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
                break;
#endif
#if defined(RCC_APB1ENR_USART3EN) && (UART_3_ENABLE > 0)
        case USART3_BASE:
                RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
                break;
#endif
#if defined(RCC_APB1ENR_UART4EN)  && (UART_4_ENABLE > 0)
        case UART4_BASE:
                RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
                break;
#endif
#if defined(RCC_APB1ENR_UART5EN)  && (UART_5_ENABLE > 0)
        case UART5_BASE:
                RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
                break;
#endif
        default:
                return STD_RET_ERROR;
        }

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function disable USART clock
 *
 * @param[in] *USART            peripheral address
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t turn_off_USART(USART_t *USART)
{
        switch ((u32_t)USART) {
#if defined(RCC_APB2ENR_USART1EN) && (UART_1_ENABLE > 0)
        case USART1_BASE:
                NVIC_DisableIRQ(USART1_IRQn);
                RCC->APB2RSTR |=  RCC_APB2RSTR_USART1RST;
                RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;
                RCC->APB2ENR  &= ~RCC_APB2ENR_USART1EN;
                break;
#endif
#if defined(RCC_APB1ENR_USART2EN) && (UART_2_ENABLE > 0)
        case USART2_BASE:
                NVIC_DisableIRQ(USART2_IRQn);
                RCC->APB1RSTR |=  RCC_APB1RSTR_USART2RST;
                RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;
                RCC->APB1ENR  &= ~RCC_APB1ENR_USART2EN;
                break;
#endif
#if defined(RCC_APB1ENR_USART3EN) && (UART_3_ENABLE > 0)
        case USART3_BASE:
                NVIC_DisableIRQ(USART3_IRQn);
                RCC->APB1RSTR |=  RCC_APB1RSTR_USART3RST;
                RCC->APB1RSTR &= ~RCC_APB1RSTR_USART3RST;
                RCC->APB1ENR  &= ~RCC_APB1ENR_USART3EN;
                break;
#endif
#if defined(RCC_APB1ENR_UART4EN)  && (UART_4_ENABLE > 0)
        case UART4_BASE:
                NVIC_DisableIRQ(UART4_IRQn);
                RCC->APB1RSTR |=  RCC_APB1RSTR_UART4RST;
                RCC->APB1RSTR &= ~RCC_APB1RSTR_UART4RST;
                RCC->APB1ENR  &= ~RCC_APB1ENR_UART4EN;
                break;
#endif
#if defined(RCC_APB1ENR_UART5EN)  && (UART_5_ENABLE > 0)
        case UART5_BASE:
                NVIC_DisableIRQ(UART5_IRQn);
                RCC->APB1RSTR |=  RCC_APB1RSTR_UART5RST;
                RCC->APB1RSTR &= ~RCC_APB1RSTR_UART5RST;
                RCC->APB1ENR  &= ~RCC_APB1ENR_UART5EN;
                break;
#endif
        default:
                return STD_RET_ERROR;
        }

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function turn on the USART interrupts
 *
 * @param[in] *USART            peripheral address
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
static stdret_t enable_USART_interrupts(USART_t *USART)
{
        switch ((u32_t)USART) {
#if defined(RCC_APB2ENR_USART1EN) && (UART_1_ENABLE > 0)
        case USART1_BASE:
                NVIC_EnableIRQ(USART1_IRQn);
                NVIC_SetPriority(USART1_IRQn, IRQ_PRIORITY);
                break;
#endif
#if defined(RCC_APB1ENR_USART2EN) && (UART_2_ENABLE > 0)
        case USART2_BASE:
                NVIC_EnableIRQ(USART2_IRQn);
                NVIC_SetPriority(USART2_IRQn, IRQ_PRIORITY);
                break;
#endif
#if defined(RCC_APB1ENR_USART3EN) && (UART_3_ENABLE > 0)
        case USART3_BASE:
                NVIC_EnableIRQ(USART3_IRQn);
                NVIC_SetPriority(USART3_IRQn, IRQ_PRIORITY);
                break;
#endif
#if defined(RCC_APB1ENR_UART4EN)  && (UART_4_ENABLE > 0)
        case UART4_BASE:
                NVIC_EnableIRQ(UART4_IRQn);
                NVIC_SetPriority(UART4_IRQn, IRQ_PRIORITY);
                break;
#endif
#if defined(RCC_APB1ENR_UART5EN)  && (UART_5_ENABLE > 0)
        case UART5_BASE:
                NVIC_EnableIRQ(UART5_IRQn);
                NVIC_SetPriority(UART5_IRQn, IRQ_PRIORITY);
                break;
#endif
        default:
                return STD_RET_ERROR;
        }

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief USART1 Interrupt
 */
//==============================================================================
#if defined(RCC_APB2ENR_USART1EN) && (UART_1_ENABLE > 0)
void USART1_IRQHandler(void)
{
        IRQ_handler(USART_data[UART_DEV_1]);
}
#endif

//==============================================================================
/**
 * @brief USART2 Interrupt
 */
//==============================================================================
#if defined(RCC_APB1ENR_USART2EN) && (UART_2_ENABLE > 0)
void USART2_IRQHandler(void)
{
        IRQ_handler(USART_data[UART_DEV_2]);
}
#endif

//==============================================================================
/**
 * @brief USART3 Interrupt
 */
//==============================================================================
#if defined(RCC_APB1ENR_USART3EN) && (UART_3_ENABLE > 0)
void USART3_IRQHandler(void)
{
        IRQ_handler(USART_data[UART_DEV_3]);
}
#endif

//==============================================================================
/**
 * @brief UART4 Interrupt
 */
//==============================================================================
#if defined(RCC_APB1ENR_UART4EN) && (UART_4_ENABLE > 0)
void UART4_IRQHandler(void)
{
        IRQ_handler(USART_data[UART_DEV_4]);
}
#endif

//==============================================================================
/**
 * @brief UART5 Interrupt
 */
//==============================================================================
#if defined(RCC_APB1ENR_UART5EN) && (UART_5_ENABLE > 0)
void UART5_IRQHandler(void)
{
        IRQ_handler(USART_data[UART_DEV_5]);
}
#endif

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
