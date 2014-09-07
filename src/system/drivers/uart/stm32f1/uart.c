/*=========================================================================*//**
@file    uart.c

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

/*==============================================================================
  Include files
==============================================================================*/
#include "core/module.h"
#include <dnx/thread.h>
#include <dnx/misc.h>
#include <unistd.h>
#include "stm32f1/uart_cfg.h"
#include "stm32f1/uart_def.h"
#include "stm32f1/stm32f10x.h"
#include "stm32f1/lib/stm32f10x_rcc.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define RELEASE_TIMEOUT                         100
#define TX_WAIT_TIMEOUT                         MAX_DELAY_MS
#define RX_WAIT_TIMEOUT                         MAX_DELAY_MS
#define MTX_BLOCK_TIMEOUT                       MAX_DELAY_MS

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/* irq configuration */
struct irq_cfg {
        IRQn_Type irqn;
        u32_t     priority;
};

/* USART handling structure */
struct UART_data {
        struct Rx_FIFO {
                u8_t            buffer[_UART_RX_BUFFER_SIZE];
                u16_t           buffer_level;
                u16_t           read_index;
                u16_t           write_index;
        } Rx_FIFO;

        struct Tx_buffer {
                const u8_t     *src_ptr;
                size_t          data_size;
        } Tx_buffer;

        sem_t                  *data_write_sem;
        sem_t                  *data_read_sem;
        mutex_t                *port_lock_rx_mtx;
        mutex_t                *port_lock_tx_mtx;
        u8_t                    major;
        struct UART_config      config;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static stdret_t uart_turn_on            (USART_t *uart);
static stdret_t uart_turn_off           (USART_t *uart);
static void     configure_uart          (u8_t major, struct UART_config *config);
static bool     fifo_write              (struct Rx_FIFO *fifo, u8_t *data);
static bool     fifo_read               (struct Rx_FIFO *fifo, u8_t *data);
static void     handle_irq              (u8_t major);

/*==============================================================================
  Local object definitions
==============================================================================*/
MODULE_NAME(UART);

/* addresses of UART devices */
static USART_t *const uart[_UART_NUMBER] = {
        #if defined(RCC_APB2ENR_USART1EN) && (_UART1_ENABLE > 0)
        USART1,
        #endif
        #if defined(RCC_APB1ENR_USART2EN) && (_UART2_ENABLE > 0)
        USART2,
        #endif
        #if defined(RCC_APB1ENR_USART3EN) && (_UART3_ENABLE > 0)
        USART3,
        #endif
        #if defined(RCC_APB1ENR_UART4EN)  && (_UART4_ENABLE > 0)
        UART4,
        #endif
        #if defined(RCC_APB1ENR_UART5EN)  && (_UART5_ENABLE > 0)
        UART5,
        #endif
};

/* irq configuration */
static const struct irq_cfg uart_irq[_UART_NUMBER] = {
        #if defined(RCC_APB2ENR_USART1EN) && (_UART1_ENABLE > 0)
        {.irqn = USART1_IRQn, .priority = _UART1_IRQ_PRIORITY},
        #endif
        #if defined(RCC_APB1ENR_USART2EN) && (_UART2_ENABLE > 0)
        {.irqn = USART2_IRQn, .priority = _UART2_IRQ_PRIORITY},
        #endif
        #if defined(RCC_APB1ENR_USART3EN) && (_UART3_ENABLE > 0)
        {.irqn = USART3_IRQn, .priority = _UART3_IRQ_PRIORITY},
        #endif
        #if defined(RCC_APB1ENR_UART4EN)  && (_UART4_ENABLE > 0)
        {.irqn = UART4_IRQn, .priority = _UART4_IRQ_PRIORITY},
        #endif
        #if defined(RCC_APB1ENR_UART5EN)  && (_UART5_ENABLE > 0)
        {.irqn = UART5_IRQn, .priority = _UART5_IRQ_PRIORITY}
        #endif
};

/* uart default configuration */
static const struct UART_config uart_default_config = {
        .parity             = _UART_DEFAULT_PARITY,
        .stop_bits          = _UART_DEFAULT_STOP_BITS,
        .LIN_break_length   = _UART_DEFAULT_LIN_BREAK_LEN,
        .tx_enable          = _UART_DEFAULT_TX_ENABLE,
        .rx_enable          = _UART_DEFAULT_RX_ENABLE,
        .LIN_mode_enable    = _UART_DEFAULT_LIN_MODE_ENABLE,
        .hardware_flow_ctrl = _UART_DEFAULT_HW_FLOW_CTRL,
        .single_wire_mode   = _UART_DEFAULT_SINGLE_WIRE_MODE,
        .baud               = _UART_DEFAULT_BAUD
};

/* structure which identify USARTs data in the IRQs */
static struct UART_data *uart_data[_UART_NUMBER];

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

        if (major >= _UART_NUMBER || minor != _UART_MINOR_NUMBER) {
                errno = EINVAL;
                return STD_RET_ERROR;
        }

        uart_data[major] = calloc(1, sizeof(struct UART_data));
        if (uart_data[major]) {

                uart_data[major]->data_write_sem   = semaphore_new(1, 0);
                uart_data[major]->data_read_sem    = semaphore_new(_UART_RX_BUFFER_SIZE, 0);
                uart_data[major]->port_lock_rx_mtx = mutex_new(MUTEX_NORMAL);
                uart_data[major]->port_lock_tx_mtx = mutex_new(MUTEX_NORMAL);
                stdret_t uart_clock_configured     = uart_turn_on(uart[major]);

                if (  uart_data[major]->data_write_sem
                   && uart_data[major]->data_read_sem
                   && uart_data[major]->port_lock_rx_mtx
                   && uart_data[major]->port_lock_tx_mtx
                   && uart_clock_configured == STD_RET_OK ) {

                        uart_data[major]->major  = major;
                        uart_data[major]->config = uart_default_config;

                        NVIC_EnableIRQ(uart_irq[major].irqn);
                        NVIC_SetPriority(uart_irq[major].irqn, uart_irq[major].priority);
                        configure_uart(major, (struct UART_config *)&uart_default_config);

                        *device_handle = uart_data[major];

                        return STD_RET_OK;
                } else {
                        if (uart_clock_configured) {
                                uart_turn_off(uart[major]);
                        }

                        if (uart_data[major]->data_write_sem) {
                                semaphore_delete(uart_data[major]->data_write_sem);
                        }

                        if (uart_data[major]->data_read_sem) {
                                semaphore_delete(uart_data[major]->data_read_sem);
                        }

                        if (uart_data[major]->port_lock_rx_mtx) {
                                mutex_delete(uart_data[major]->port_lock_rx_mtx);
                        }

                        if (uart_data[major]->port_lock_tx_mtx) {
                                mutex_delete(uart_data[major]->port_lock_tx_mtx);
                        }

                        free(uart_data[major]);
                        uart_data[major] = NULL;
                }
        }

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
        struct UART_data *hdl = device_handle;

        if (mutex_lock(hdl->port_lock_rx_mtx, RELEASE_TIMEOUT)) {
                if (mutex_lock(hdl->port_lock_tx_mtx, RELEASE_TIMEOUT)) {

                        critical_section_begin();

                        mutex_unlock(hdl->port_lock_rx_mtx);
                        mutex_unlock(hdl->port_lock_tx_mtx);

                        mutex_delete(hdl->port_lock_rx_mtx);
                        mutex_delete(hdl->port_lock_tx_mtx);

                        semaphore_delete(hdl->data_write_sem);
                        uart_turn_off(uart[hdl->major]);

                        uart_data[hdl->major] = NULL;
                        free(hdl);

                        critical_section_end();

                        return STD_RET_OK;
                }

                mutex_unlock(hdl->port_lock_rx_mtx);
        }

        errno = EBUSY;
        return STD_RET_ERROR;
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
API_MOD_OPEN(UART, void *device_handle, vfs_open_flags_t flags)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(flags);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Close device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           force                  device force close (true)
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_CLOSE(UART, void *device_handle, bool force)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(force);

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
 * @param[in ]           fattr                  file attributes
 *
 * @return number of written bytes, -1 if error
 */
//==============================================================================
API_MOD_WRITE(UART, void *device_handle, const u8_t *src, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(fpos);
        UNUSED_ARG(fattr);

        struct UART_data *hdl = device_handle;

        ssize_t n = 0;
        if (mutex_lock(hdl->port_lock_tx_mtx, MTX_BLOCK_TIMEOUT)) {
                hdl->Tx_buffer.src_ptr   = src;
                hdl->Tx_buffer.data_size = count;

                SET_BIT(uart[hdl->major]->CR1, USART_CR1_TXEIE);
                semaphore_wait(hdl->data_write_sem, TX_WAIT_TIMEOUT);

                n = count - hdl->Tx_buffer.data_size;

                mutex_unlock(hdl->port_lock_tx_mtx);
        } else {
                errno = EBUSY;
                n     = -1;
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
 * @param[in ]           fattr                  file attributes
 *
 * @return number of read bytes, -1 if error
 */
//==============================================================================
API_MOD_READ(UART, void *device_handle, u8_t *dst, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(fpos);
        UNUSED_ARG(fattr);

        struct UART_data *hdl = device_handle;

        ssize_t n = 0;
        if (mutex_lock(hdl->port_lock_rx_mtx, MTX_BLOCK_TIMEOUT)) {
                while (count--) {
                        if (semaphore_wait(hdl->data_read_sem, RX_WAIT_TIMEOUT)) {
                                CLEAR_BIT(uart[hdl->major]->CR1, USART_CR1_RXNEIE);
                                if (fifo_read(&hdl->Rx_FIFO, dst)) {
                                        dst++;
                                        n++;
                                }
                                SET_BIT(uart[hdl->major]->CR1, USART_CR1_RXNEIE);
                        }
                }

                mutex_unlock(hdl->port_lock_rx_mtx);
        } else {
                errno = EBUSY;
                n     = -1;
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
 */
//==============================================================================
API_MOD_IOCTL(UART, void *device_handle, int request, void *arg)
{
        struct UART_data *hdl    = device_handle;
        stdret_t          status = STD_RET_OK;

        if (arg) {
                switch (request) {
                case IOCTL_UART__SET_CONFIGURATION:
                        configure_uart(hdl->major, arg);
                        hdl->config = *(struct UART_config *)arg;
                        break;

                case IOCTL_UART__GET_CONFIGURATION:
                        *(struct UART_config *)arg = hdl->config;
                        break;

                case IOCTL_UART__GET_CHAR_UNBLOCKING:
                        if (!fifo_read(&hdl->Rx_FIFO, arg)) {
                             errno  = EAGAIN;
                             status = STD_RET_ERROR;
                        }
                        break;

                default:
                        errno  = EBADRQC;
                        status = STD_RET_ERROR;
                        break;
                }
        } else {
                errno  = EINVAL;
                status = STD_RET_ERROR;
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
        UNUSED_ARG(device_handle);

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
        UNUSED_ARG(device_handle);

        device_stat->st_size  = 0;
        device_stat->st_major = 0;
        device_stat->st_minor = 0;

        return STD_RET_OK;
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
static stdret_t uart_turn_on(USART_t *USART)
{
        switch ((u32_t)USART) {
        #if defined(RCC_APB2ENR_USART1EN) && (_UART1_ENABLE > 0)
        case USART1_BASE:
                if (!(RCC->APB2ENR & RCC_APB2ENR_USART1EN)) {
                        SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_USART1RST);
                        CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_USART1RST);
                        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);
                } else {
                        return STD_RET_ERROR;
                }
                break;
        #endif
        #if defined(RCC_APB1ENR_USART2EN) && (_UART2_ENABLE > 0)
        case USART2_BASE:
                if (!(RCC->APB1ENR & RCC_APB1ENR_USART2EN)) {
                        SET_BIT(RCC->APB1RSTR, RCC_APB1RSTR_USART2RST);
                        CLEAR_BIT(RCC->APB1RSTR, RCC_APB1RSTR_USART2RST);
                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_USART2EN);
                } else {
                        return STD_RET_ERROR;
                }
                break;
        #endif
        #if defined(RCC_APB1ENR_USART3EN) && (_UART3_ENABLE > 0)
        case USART3_BASE:
                if (!(RCC->APB1ENR & RCC_APB1ENR_USART3EN)) {
                        SET_BIT(RCC->APB1RSTR, RCC_APB1RSTR_USART3RST);
                        CLEAR_BIT(RCC->APB1RSTR, RCC_APB1RSTR_USART3RST);
                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_USART3EN);
                } else {
                        return STD_RET_ERROR;
                }
                break;
        #endif
        #if defined(RCC_APB1ENR_UART4EN)  && (_UART4_ENABLE > 0)
        case UART4_BASE:
                if (!(RCC->APB1ENR & RCC_APB1ENR_UART4EN)) {
                        SET_BIT(RCC->APB1RSTR, RCC_APB1RSTR_UART4RST);
                        CLEAR_BIT(RCC->APB1RSTR, RCC_APB1RSTR_UART4RST);
                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_UART4EN);
                } else {
                        return STD_RET_ERROR;
                }
                break;
        #endif
        #if defined(RCC_APB1ENR_UART5EN)  && (_UART5_ENABLE > 0)
        case UART5_BASE:
                if (!(RCC->APB1ENR & RCC_APB1ENR_UART5EN)) {
                        SET_BIT(RCC->APB1RSTR, RCC_APB1RSTR_UART5RST);
                        CLEAR_BIT(RCC->APB1RSTR, RCC_APB1RSTR_UART5RST);
                        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_UART5EN);
                } else {
                        return STD_RET_ERROR;
                }
                break;
        #endif
        default:
                errno = EIO;
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
static stdret_t uart_turn_off(USART_t *USART)
{
        switch ((u32_t)USART) {
        #if defined(RCC_APB2ENR_USART1EN) && (_UART1_ENABLE > 0)
        case USART1_BASE:
                NVIC_DisableIRQ(USART1_IRQn);
                SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_USART1RST);
                CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_USART1RST);
                CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);
                break;
        #endif
        #if defined(RCC_APB1ENR_USART2EN) && (_UART2_ENABLE > 0)
        case USART2_BASE:
                NVIC_DisableIRQ(USART2_IRQn);
                SET_BIT(RCC->APB1RSTR, RCC_APB1RSTR_USART2RST);
                CLEAR_BIT(RCC->APB1RSTR, RCC_APB1RSTR_USART2RST);
                CLEAR_BIT(RCC->APB1ENR, RCC_APB1ENR_USART2EN);
                break;
        #endif
        #if defined(RCC_APB1ENR_USART3EN) && (_UART3_ENABLE > 0)
        case USART3_BASE:
                NVIC_DisableIRQ(USART3_IRQn);
                SET_BIT(RCC->APB1RSTR, RCC_APB1RSTR_USART3RST);
                CLEAR_BIT(RCC->APB1RSTR, RCC_APB1RSTR_USART3RST);
                CLEAR_BIT(RCC->APB1ENR, RCC_APB1ENR_USART3EN);
                break;
        #endif
        #if defined(RCC_APB1ENR_UART4EN)  && (_UART4_ENABLE > 0)
        case UART4_BASE:
                NVIC_DisableIRQ(UART4_IRQn);
                SET_BIT(RCC->APB1RSTR, RCC_APB1RSTR_UART4RST);
                CLEAR_BIT(RCC->APB1RSTR, RCC_APB1RSTR_UART4RST);
                CLEAR_BIT(RCC->APB1ENR, RCC_APB1ENR_UART4EN);
                break;
        #endif
        #if defined(RCC_APB1ENR_UART5EN)  && (_UART5_ENABLE > 0)
        case UART5_BASE:
                NVIC_DisableIRQ(UART5_IRQn);
                SET_BIT(RCC->APB1RSTR, RCC_APB1RSTR_UART5RST);
                CLEAR_BIT(RCC->APB1RSTR, RCC_APB1RSTR_UART5RST);
                CLEAR_BIT(RCC->APB1ENR, RCC_APB1ENR_UART5EN);
                break;
        #endif
        default:
                errno = EIO;
                return STD_RET_ERROR;
        }

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function configure selected UART
 *
 * @param major         major device number
 * @param config        configuration structure
 */
//==============================================================================
static void configure_uart(u8_t major, struct UART_config *config)
{
        USART_t *UART = uart[major];

        /* set baud */
        RCC_ClocksTypeDef freq;
        RCC_GetClocksFreq(&freq);

        u32_t PCLK;
        if ((u32_t)UART == USART1_BASE) {
                PCLK = freq.PCLK2_Frequency;
        } else {
                PCLK = freq.PCLK1_Frequency;
        }

        UART->BRR = (PCLK / (config->baud)) + 1;

        /* set 8 bit word length and wake idle line */
        CLEAR_BIT(UART->CR1, USART_CR1_M | USART_CR1_WAKE);

        /* set parity */
        switch (config->parity) {
        case UART_PARITY_OFF:
                CLEAR_BIT(UART->CR1, USART_CR1_PCE);
                break;
        case UART_PARITY_EVEN:
                SET_BIT(UART->CR1, USART_CR1_PCE);
                CLEAR_BIT(UART->CR1, USART_CR1_PS);
                break;
        case UART_PARITY_ODD:
                SET_BIT(UART->CR1, USART_CR1_PCE);
                SET_BIT(UART->CR1, USART_CR1_PS);
                break;
        }

        /* transmitter enable */
        if (config->tx_enable) {
                SET_BIT(UART->CR1, USART_CR1_TE);
        } else {
                CLEAR_BIT(UART->CR1, USART_CR1_TE);
        }

        /* receiver enable */
        if (config->rx_enable) {
                SET_BIT(UART->CR1, USART_CR1_RE);
        } else {
                CLEAR_BIT(UART->CR1, USART_CR1_RE);
        }

        /* enable LIN if configured */
        if (config->LIN_mode_enable) {
                SET_BIT(UART->CR2, USART_CR2_LINEN);
        } else {
                CLEAR_BIT(UART->CR2, USART_CR2_LINEN);
        }

        /* configure stop bits */
        if (config->stop_bits == UART_STOP_BIT_1) {
                CLEAR_BIT(UART->CR2, USART_CR2_STOP);
        } else {
                CLEAR_BIT(UART->CR2, USART_CR2_STOP);
                SET_BIT(UART->CR2, USART_CR2_STOP_1);
        }

        /* clock configuration (synchronous mode) */
        CLEAR_BIT(UART->CR2, USART_CR2_CLKEN | USART_CR2_CPOL | USART_CR2_CPHA | USART_CR2_LBCL);

        /* LIN break detection length */
        if (config->LIN_break_length == UART_LIN_BREAK_10_BITS) {
                CLEAR_BIT(UART->CR2, USART_CR2_LBDL);
        } else {
                SET_BIT(UART->CR2, USART_CR2_LBDL);
        }

        /* hardware flow control */
        if (config->hardware_flow_ctrl) {
                SET_BIT(UART->CR3, USART_CR3_CTSE | USART_CR3_RTSE);
        } else {
                CLEAR_BIT(UART->CR3, USART_CR3_CTSE | USART_CR3_RTSE);
        }

        /* configure single wire mode */
        if (config->single_wire_mode) {
                SET_BIT(UART->CR3, USART_CR3_HDSEL);
        } else {
                CLEAR_BIT(UART->CR3, USART_CR3_HDSEL);
        }

        /* enable RXNE interrupt */
        SET_BIT(UART->CR1, USART_CR1_RXNEIE);

        /* enable UART */
        SET_BIT(UART->CR1, USART_CR1_UE);
}

//==============================================================================
/**
 * @brief Function write data to FIFO
 *
 * @param fifo          fifo buffer
 * @param data          data to write
 *
 * @return true if success, false on error
 */
//==============================================================================
static bool fifo_write(struct Rx_FIFO *fifo, u8_t *data)
{
        if (fifo->buffer_level < _UART_RX_BUFFER_SIZE) {
                fifo->buffer[fifo->write_index++] = *data;

                if (fifo->write_index >= _UART_RX_BUFFER_SIZE) {
                        fifo->write_index = 0;
                }

                fifo->buffer_level++;

                return true;
        } else {
                return false;
        }
}

//==============================================================================
/**
 * @brief Function read data from FIFO
 *
 * @param fifo          fifo buffer
 * @param data          data result
 *
 * @return true if success, false on error
 */
//==============================================================================
static bool fifo_read(struct Rx_FIFO *fifo, u8_t *data)
{
        if (fifo->buffer_level > 0) {
                *data = fifo->buffer[fifo->read_index++];

                if (fifo->read_index >= _UART_RX_BUFFER_SIZE) {
                        fifo->read_index = 0;
                }

                fifo->buffer_level--;

                return true;
        } else {
                return false;
        }
}

//==============================================================================
/**
 * @brief Interrupt handling
 *
 * @param major         major device number
 */
//==============================================================================
static void handle_irq(u8_t major)
{
        USART_t *UART = uart[major];

        /* receiver interrupt handler */
        if ((UART->CR1 & USART_CR1_RXNEIE) && (UART->SR & (USART_SR_RXNE | USART_SR_ORE))) {
                u8_t DR = UART->DR;

                if (fifo_write(&uart_data[major]->Rx_FIFO, &DR)) {
                        semaphore_signal_from_ISR(uart_data[major]->data_read_sem, NULL);
                }
        }

        /* transmitter interrupt handler */
        if ((UART->CR1 & USART_CR1_TXEIE) && (UART->SR & USART_SR_TXE)) {

                if (uart_data[major]->Tx_buffer.data_size && uart_data[major]->Tx_buffer.src_ptr) {
                        UART->DR = *(uart_data[major]->Tx_buffer.src_ptr++);

                        if (--uart_data[major]->Tx_buffer.data_size == 0) {
                                UART->SR = ~USART_SR_TC;
                                SET_BIT(UART->CR1, USART_CR1_TCIE);
                                CLEAR_BIT(UART->CR1, USART_CR1_TXEIE);
                                uart_data[major]->Tx_buffer.src_ptr = NULL;
                        }
                } else {
                        /* this shall never happen */
                        CLEAR_BIT(UART->CR1, USART_CR1_TXEIE);
                        semaphore_signal_from_ISR(uart_data[major]->data_write_sem, NULL);
                }
        } else if ((UART->CR1 & USART_CR1_TCIE) && (UART->SR & USART_SR_TC)) {

                CLEAR_BIT(UART->CR1, USART_CR1_TCIE);
                semaphore_signal_from_ISR(uart_data[major]->data_write_sem, NULL);
        }
}

//==============================================================================
/**
 * @brief USART1 Interrupt
 */
//==============================================================================
#if defined(RCC_APB2ENR_USART1EN) && (_UART1_ENABLE > 0)
void USART1_IRQHandler(void)
{
        handle_irq(_UART1);
}
#endif

//==============================================================================
/**
 * @brief USART2 Interrupt
 */
//==============================================================================
#if defined(RCC_APB1ENR_USART2EN) && (_UART2_ENABLE > 0)
void USART2_IRQHandler(void)
{
        handle_irq(_UART2);
}
#endif

//==============================================================================
/**
 * @brief USART3 Interrupt
 */
//==============================================================================
#if defined(RCC_APB1ENR_USART3EN) && (_UART3_ENABLE > 0)
void USART3_IRQHandler(void)
{
        handle_irq(_UART3);
}
#endif

//==============================================================================
/**
 * @brief UART4 Interrupt
 */
//==============================================================================
#if defined(RCC_APB1ENR_UART4EN) && (_UART4_ENABLE > 0)
void UART4_IRQHandler(void)
{
        handle_irq(_UART4);
}
#endif

//==============================================================================
/**
 * @brief UART5 Interrupt
 */
//==============================================================================
#if defined(RCC_APB1ENR_UART5EN) && (_UART5_ENABLE > 0)
void UART5_IRQHandler(void)
{
        handle_irq(_UART5);
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
