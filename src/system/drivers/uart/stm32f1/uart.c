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
#include "system/dnxmodule.h"
#include "stm32f1/uart_cfg.h"
#include "stm32f1/uart_def.h"
#include "stm32f1/stm32f10x.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define MTX_BLOCK_TIME                          MAX_DELAY
#define TXC_WAIT_TIME                           60000

#define force_lock_recursive_mutex(mtx)         while (lock_recursive_mutex(mtx, 10) != MUTEX_LOCKED)
#define force_lock_mutex(mtx)                   while (lock_mutex(mtx, 10) != MUTEX_LOCKED)

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
        mutex_t *port_lock_rx_mtx;
        mutex_t *port_lock_tx_mtx;
        task_t  *task_rx;
        USART_t *USART;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static stdret_t turn_on_USART           (USART_t *USART);
static stdret_t turn_off_USART          (USART_t *USART);
static stdret_t enable_USART_interrupts (USART_t *USART);
static void     IRQ_handler             (struct USART_data *USART_data);

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
 * @brief Initialize device
 *
 * @param[out]          **device_handle        device allocated memory
 * @param[in ]            major                major device number
 * @param[in ]            minor                minor device number
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_INIT(UART, void **device_handle, u8_t major, u8_t minor)
{
        UNUSED_ARG(minor);

        STOP_IF(device_handle == NULL);

        if (major >= UART_DEV_COUNT) {
                return STD_RET_ERROR;
        }

        if (!(USART_data[major] = calloc(1, sizeof(struct USART_data)))) {
                return STD_RET_ERROR;
        }

        *device_handle = USART_data[major];

        USART_data[major]->USART            = USART_peripherals[major];
        USART_data[major]->data_write_sem   = new_semaphore();
        USART_data[major]->port_lock_rx_mtx = new_mutex();
        USART_data[major]->port_lock_tx_mtx = new_mutex();

        if (  !USART_data[major]->data_write_sem
           || !USART_data[major]->port_lock_rx_mtx
           || !USART_data[major]->port_lock_tx_mtx) {
                goto error;
        } else {
                take_semaphore(USART_data[major]->data_write_sem, 1);

                if (turn_on_USART(USART_data[major]->USART) != STD_RET_OK) {
                        goto error;
                }

                enable_USART(USART_data[major]->USART);

                if ((u32_t) USART_data[major]->USART == USART1_BASE) {
                        set_baud_rate(USART_data[major]->USART, UART_PCLK2_FREQ, UART_DEFAULT_BAUDRATE);
                } else {
                        set_baud_rate(USART_data[major]->USART, UART_PCLK1_FREQ, UART_DEFAULT_BAUDRATE);
                }

                if (UART_DEFAULT_WAKE_METHOD) {
                        wakeup_USART_on_address_mark(USART_data[major]->USART);
                } else {
                        wakeup_USART_on_idle_line(USART_data[major]->USART);
                }

                if (UART_DEFAULT_PARITY_ENABLE) {
                        enable_parity_check(USART_data[major]->USART);
                } else {
                        disable_parity_check(USART_data[major]->USART);
                }

                if (UART_DEFAULT_PARITY_SELECTION) {
                        enable_odd_parity(USART_data[major]->USART);
                } else {
                        enable_even_parity(USART_data[major]->USART);
                }

                if (UART_DEFAULT_TX_ENABLE) {
                        enable_transmitter(USART_data[major]->USART);
                } else {
                        disable_transmitter(USART_data[major]->USART);
                }

                if (UART_DEFAULT_RX_ENABLE) {
                        enable_receiver(USART_data[major]->USART);
                } else {
                        disable_receiver(USART_data[major]->USART);
                }

                if (UART_DEFAULT_RX_WAKEUP_MODE) {
                        receiver_wakeup_in_mute_mode(USART_data[major]->USART);
                } else {
                        receiver_wakeup_in_active_mode(USART_data[major]->USART);
                }

                if (UART_DEFAULT_LIN_ENABLE) {
                        enable_LIN_mode(USART_data[major]->USART);
                } else {
                        disable_LIN_mode(USART_data[major]->USART);
                }

                if (UART_DEFAULT_STOP_BITS) {
                        set_2_stop_bits(USART_data[major]->USART);
                } else {
                        set_1_stop_bit(USART_data[major]->USART);
                }

                if (UART_DEFAULT_LIN_BREAK_LEN_DET) {
                        detect_11_bit_LIN_break(USART_data[major]->USART);
                } else {
                        detect_10_bit_LIN_break(USART_data[major]->USART);
                }

                if (UART_DEFAULT_CTS_ENABLE) {
                        enable_CTS(USART_data[major]->USART);
                } else {
                        disable_CTS(USART_data[major]->USART);
                }

                if (UART_DEFAULT_RTS_ENABLE) {
                        enable_RTS(USART_data[major]->USART);
                } else {
                        disable_RTS(USART_data[major]->USART);
                }

                set_address_node(USART_data[major]->USART, UART_DEFAULT_MULTICOM_ADDRESS);

                if (enable_USART_interrupts(USART_data[major]->USART) != STD_RET_OK) {
                        goto error;
                }

                enable_RXNE_IRQ(USART_data[major]->USART);

                return STD_RET_OK;
        }

error:
        if (USART_data[major]->data_write_sem) {
                delete_semaphore(USART_data[major]->data_write_sem);
        }

        if (USART_data[major]->port_lock_rx_mtx) {
                delete_recursive_mutex(USART_data[major]->port_lock_rx_mtx);
        }

        if (USART_data[major]->port_lock_tx_mtx) {
                delete_recursive_mutex(USART_data[major]->port_lock_tx_mtx);
        }

        free(USART_data[major]);
        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Release device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_RELEASE(UART, void *device_handle)
{
        STOP_IF(device_handle == NULL);

        struct USART_data *hdl = device_handle;

        force_lock_mutex(hdl->port_lock_rx_mtx);
        force_lock_mutex(hdl->port_lock_tx_mtx);

        enter_critical_section();

        unlock_mutex(hdl->port_lock_rx_mtx);
        unlock_mutex(hdl->port_lock_tx_mtx);

        delete_mutex(hdl->port_lock_rx_mtx);
        delete_mutex(hdl->port_lock_tx_mtx);

        delete_semaphore(hdl->data_write_sem);
        turn_off_USART(hdl->USART);
        free(hdl);
        exit_critical_section();

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Open device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           flags                  file operation flags (O_RDONLY, O_WRONLY, O_RDWR)
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_OPEN(UART, void *device_handle, int flags)
{
        UNUSED_ARG(flags);
        STOP_IF(device_handle == NULL);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Close device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           force                  device force close (true)
 * @param[in ]          *opened_by_task         task with opened this device (valid only if force is true)
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_CLOSE(UART, void *device_handle, bool force, const task_t *opened_by_task)
{
        UNUSED_ARG(force);
        UNUSED_ARG(opened_by_task);

        STOP_IF(device_handle == NULL);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Write data to device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ][out]     *fpos                   file position
 *
 * @return number of written bytes
 */
//==============================================================================
API_MOD_WRITE(UART, void *device_handle, const u8_t *src, size_t count, u64_t *fpos)
{
        UNUSED_ARG(fpos);

        STOP_IF(device_handle == NULL);
        STOP_IF(src == NULL);
        STOP_IF(count == 0);

        struct USART_data *hdl = device_handle;

        size_t n = 0;
        if (lock_mutex(hdl->port_lock_tx_mtx, MTX_BLOCK_TIME) == MUTEX_LOCKED) {
                hdl->Tx_buffer.src_ptr   = src;
                hdl->Tx_buffer.data_size = count;

                enable_TXE_IRQ(hdl->USART);
                take_semaphore(hdl->data_write_sem, TXC_WAIT_TIME);

                n = count;

                unlock_mutex(hdl->port_lock_tx_mtx);
        }

        return n;
}

//==============================================================================
/**
 * @brief Read data from device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ][out]     *fpos                   file position
 *
 * @return number of read bytes
 */
//==============================================================================
API_MOD_READ(UART, void *device_handle, u8_t *dst, size_t count, u64_t *fpos)
{
        UNUSED_ARG(fpos);

        STOP_IF(device_handle == NULL);
        STOP_IF(dst == NULL);
        STOP_IF(count == 0);

        struct USART_data *hdl = device_handle;

        size_t n = 0;
        if (lock_mutex(hdl->port_lock_rx_mtx, MTX_BLOCK_TIME) == MUTEX_LOCKED) {
                u8_t  *dst_ptr   = dst;
                size_t data_size = count;
                hdl->task_rx     = get_task_handle();

                do {
                        enter_critical_section();

                        if (hdl->Rx_FIFO.buffer_level > 0) {
                                *dst_ptr = hdl->Rx_FIFO.buffer[hdl->Rx_FIFO.read_index++];

                                if (hdl->Rx_FIFO.read_index >= UART_RX_BUFFER_SIZE)
                                        hdl->Rx_FIFO.read_index = 0;

                                hdl->Rx_FIFO.buffer_level--;
                                data_size--;
                                n++;

                                exit_critical_section();
                        } else {
                                exit_critical_section();
                                suspend_this_task();
                        }
                } while (data_size);

                unlock_mutex(hdl->port_lock_rx_mtx);
        }

        return n;
}

//==============================================================================
/**
 * @brief IO control
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 * @retval ...
 */
//==============================================================================
API_MOD_IOCTL(UART, void *device_handle, int request, void *arg)
{
        STOP_IF(device_handle == NULL);

        struct USART_data *hdl = device_handle;
        stdret_t status = STD_RET_OK;

        switch (request) {
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
                set_address_node(hdl->USART, (int)arg);
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
                enter_critical_section();

                if (!arg) {
                        exit_critical_section();
                        status = STD_RET_ERROR;
                        break;
                }

                if (hdl->Rx_FIFO.buffer_level > 0) {
                        *((u8_t*)arg) = hdl->Rx_FIFO.buffer[hdl->Rx_FIFO.read_index++];

                        if (hdl->Rx_FIFO.read_index >= UART_RX_BUFFER_SIZE)
                                hdl->Rx_FIFO.read_index = 0;

                        hdl->Rx_FIFO.buffer_level--;
                } else {
                        status = STD_RET_ERROR;
                }

                exit_critical_section();
                break;

        case UART_IORQ_GET_BYTE_BLOCKING:
                while (TRUE) {
                        enter_critical_section();

                        if (!arg) {
                                exit_critical_section();
                                status = STD_RET_ERROR;
                                break;
                        }

                        if (hdl->Rx_FIFO.buffer_level > 0) {
                                *((u8_t*)arg) = hdl->Rx_FIFO.buffer[hdl->Rx_FIFO.read_index++];

                                if (hdl->Rx_FIFO.read_index >= UART_RX_BUFFER_SIZE)
                                        hdl->Rx_FIFO.read_index = 0;

                                hdl->Rx_FIFO.buffer_level--;

                                exit_critical_section();
                                break;
                        } else {
                                exit_critical_section();
                                suspend_this_task();
                        }
                }
                break;

        case UART_IORQ_SEND_BYTE:
                while (!(hdl->USART->SR & USART_SR_TXE)) {
                        sleep_ms(1);
                }

                hdl->USART->DR = (int)arg;
                break;

        case UART_IORQ_SET_BAUDRATE:
                if ((u32_t)hdl->USART == USART1_BASE) {
                        set_baud_rate(hdl->USART, UART_PCLK2_FREQ, (int)arg);
                } else {
                        set_baud_rate(hdl->USART, UART_PCLK1_FREQ, (int)arg);
                }
                break;

        default:
                status = STD_RET_ERROR;
                break;
        }

        return status;
}

//==============================================================================
/**
 * @brief Flush device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_FLUSH(UART, void *device_handle)
{
        STOP_IF(device_handle == NULL);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Device information
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *device_stat            device status
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_STAT(UART, void *device_handle, struct vfs_dev_stat *device_stat)
{
        STOP_IF(device_handle == NULL);
        STOP_IF(device_stat == NULL);

        device_stat->st_size  = 0;
        device_stat->st_major = 0;
        device_stat->st_minor = 0;

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

                                int woken;
                                disable_TXE_IRQ(USART_data->USART);
                                give_semaphore_from_ISR(USART_data->data_write_sem, &woken);
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

                  if (USART_data->task_rx) {
                        if (resume_task_from_ISR(USART_data->task_rx)) {
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
                NVIC_SetPriority(USART1_IRQn, UART_IRQ_PRIORITY);
                break;
#endif
#if defined(RCC_APB1ENR_USART2EN) && (UART_2_ENABLE > 0)
        case USART2_BASE:
                NVIC_EnableIRQ(USART2_IRQn);
                NVIC_SetPriority(USART2_IRQn, UART_IRQ_PRIORITY);
                break;
#endif
#if defined(RCC_APB1ENR_USART3EN) && (UART_3_ENABLE > 0)
        case USART3_BASE:
                NVIC_EnableIRQ(USART3_IRQn);
                NVIC_SetPriority(USART3_IRQn, UART_IRQ_PRIORITY);
                break;
#endif
#if defined(RCC_APB1ENR_UART4EN)  && (UART_4_ENABLE > 0)
        case UART4_BASE:
                NVIC_EnableIRQ(UART4_IRQn);
                NVIC_SetPriority(UART4_IRQn, UART_IRQ_PRIORITY);
                break;
#endif
#if defined(RCC_APB1ENR_UART5EN)  && (UART_5_ENABLE > 0)
        case UART5_BASE:
                NVIC_EnableIRQ(UART5_IRQn);
                NVIC_SetPriority(UART5_IRQn, UART_IRQ_PRIORITY);
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
