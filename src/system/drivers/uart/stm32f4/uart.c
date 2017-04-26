/*=========================================================================*//**
@file    uart.c

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
#include "stm32f4/uart_cfg.h"
#include "stm32f4/stm32f4xx.h"
#include "stm32f4/lib/stm32f4xx_rcc.h"
#include "../uart_ioctl.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define RELEASE_TIMEOUT                         100
#define RX_WAIT_TIMEOUT                         MAX_DELAY_MS
#define MTX_BLOCK_TIMEOUT                       MAX_DELAY_MS

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
//* UARTs */
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

/* UART registers */
typedef struct {
        USART_TypeDef  *UART;
        __IO uint32_t  *APBENR;
        __IO uint32_t  *APBRSTR;
        const uint32_t  APBENR_UARTEN;
        const uint32_t  APBRSTR_UARTRST;
        const IRQn_Type IRQn;
        const u32_t     PRIORITY;
} UART_regs_t;

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
static int  UART_turn_on  (const UART_regs_t *UART);
static int  UART_turn_off (const UART_regs_t *UART);
static void UART_configure(u8_t major, const struct UART_config *config);
static bool FIFO_write    (struct Rx_FIFO *fifo, u8_t *data);
static bool FIFO_read     (struct Rx_FIFO *fifo, u8_t *data);
static void IRQ_handle    (u8_t major);

/*==============================================================================
  Local object definitions
==============================================================================*/
MODULE_NAME(UART);

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
                .PRIORITY        = _UART1_IRQ_PRIORITY
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
                .PRIORITY        = _UART2_IRQ_PRIORITY
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
                .PRIORITY        = _UART3_IRQ_PRIORITY
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
                .PRIORITY        = _UART4_IRQ_PRIORITY
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
                .PRIORITY        = _UART5_IRQ_PRIORITY
        },
        #endif
        #if defined(RCC_APB2ENR_USART6EN)
        {
                .UART            = USART6,
                .APBENR          = &RCC->APB2ENR,
                .APBRSTR         = &RCC->APB2RSTR,
                .APBENR_UARTEN   = RCC_APB2ENR_USART6EN,
                .APBRSTR_UARTRST = RCC_APB2RSTR_USART6RST,
                .IRQn            = USART6_IRQn,
                .PRIORITY        = _UART6_IRQ_PRIORITY
        },
        #endif
        #if defined(RCC_APB1ENR_UART7EN)
        {
                .UART            = UART7,
                .APBENR          = &RCC->APB1ENR,
                .APBRSTR         = &RCC->APB1RSTR,
                .APBENR_UARTEN   = RCC_APB1ENR_UART7EN,
                .APBRSTR_UARTRST = RCC_APB1RSTR_UART7RST,
                .IRQn            = UART7_IRQn,
                .PRIORITY        = _UART7_IRQ_PRIORITY
        },
        #endif
        #if defined(RCC_APB1ENR_UART8EN)
        {
                .UART            = UART8,
                .APBENR          = &RCC->APB1ENR,
                .APBRSTR         = &RCC->APB1RSTR,
                .APBENR_UARTEN   = RCC_APB1ENR_UART8EN,
                .APBRSTR_UARTRST = RCC_APB1RSTR_UART8RST,
                .IRQn            = UART8_IRQn,
                .PRIORITY        = _UART8_IRQ_PRIORITY
        },
        #endif
        #if defined(RCC_APB2ENR_UART9EN)
        {
                .UART            = UART9,
                .APBENR          = &RCC->APB2ENR,
                .APBRSTR         = &RCC->APB2RSTR,
                .APBENR_UARTEN   = RCC_APB2ENR_UART9EN,
                .APBRSTR_UARTRST = RCC_APB2RSTR_UART9RST,
                .IRQn            = UART9_IRQn,
                .PRIORITY        = _UART9_IRQ_PRIORITY
        },
        #endif
        #if defined(RCC_APB2ENR_UART10EN)
        {
                .UART            = UART10,
                .APBENR          = &RCC->APB2ENR,
                .APBRSTR         = &RCC->APB2RSTR,
                .APBENR_UARTEN   = RCC_APB2ENR_UART10EN,
                .APBRSTR_UARTRST = RCC_APB2RSTR_UART10RST,
                .IRQn            = UART10_IRQn,
                .PRIORITY        = _UART10_IRQ_PRIORITY
        }
        #endif
};

/* UART default configuration */
static const struct UART_config UART_DEFAULT_CONFIG = {
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
static struct UART_mem *UART_mem[_UART_COUNT];

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
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_INIT(UART, void **device_handle, u8_t major, u8_t minor)
{
        UNUSED_ARG1(minor);

        if (major >= _UART_COUNT || minor != 0) {
                return ENODEV;
        }

        int err = sys_zalloc(sizeof(struct UART_mem), device_handle);
        if (!err) {
                UART_mem[major] = *device_handle;

                err = sys_semaphore_create(1, 0, &UART_mem[major]->data_write_sem);
                if (err)
                        goto finish;

                err = sys_semaphore_create(_UART_RX_BUFFER_SIZE, 0, &UART_mem[major]->data_read_sem);
                if (err)
                        goto finish;

                err = sys_mutex_create(MUTEX_TYPE_NORMAL, &UART_mem[major]->port_lock_rx_mtx);
                if (err)
                        goto finish;

                err = sys_mutex_create(MUTEX_TYPE_NORMAL, &UART_mem[major]->port_lock_tx_mtx);
                if (err)
                        goto finish;

                err = UART_turn_on(&UART[major]);
                if (!err) {
                        UART_mem[major]->major  = major;
                        UART_mem[major]->config = UART_DEFAULT_CONFIG;
                        NVIC_EnableIRQ(UART[major].IRQn);
                        NVIC_SetPriority(UART[major].IRQn, UART[major].PRIORITY);
                        UART_configure(major, &UART_DEFAULT_CONFIG);
                }

                finish:
                if (err) {
                        if (UART_mem[major]->port_lock_tx_mtx)
                                sys_mutex_destroy(UART_mem[major]->port_lock_tx_mtx);

                        if (UART_mem[major]->port_lock_rx_mtx)
                                sys_mutex_destroy(UART_mem[major]->port_lock_rx_mtx);

                        if (UART_mem[major]->data_write_sem)
                                sys_semaphore_destroy(UART_mem[major]->data_write_sem);

                        if (UART_mem[major]->data_write_sem)
                                sys_semaphore_destroy(UART_mem[major]->data_write_sem);

                        sys_free(device_handle);
                        UART_mem[major] = NULL;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Release device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_RELEASE(UART, void *device_handle)
{
        struct UART_mem *hdl = device_handle;

        if (sys_mutex_lock(hdl->port_lock_rx_mtx, RELEASE_TIMEOUT) == ESUCC) {
                if (sys_mutex_lock(hdl->port_lock_tx_mtx, RELEASE_TIMEOUT) == ESUCC) {

                        sys_mutex_destroy(hdl->port_lock_rx_mtx);
                        sys_mutex_destroy(hdl->port_lock_tx_mtx);

                        sys_semaphore_destroy(hdl->data_write_sem);

                        UART_turn_off(&UART[hdl->major]);

                        UART_mem[hdl->major] = NULL;
                        sys_free(device_handle);

                        return ESUCC;
                }

                sys_mutex_unlock(hdl->port_lock_rx_mtx);
        }

        return EBUSY;
}

//==============================================================================
/**
 * @brief Open device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           flags                  file operation flags (O_RDONLY, O_WRONLY, O_RDWR)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_OPEN(UART, void *device_handle, u32_t flags)
{
        UNUSED_ARG2(device_handle, flags);

        return ESUCC;
}

//==============================================================================
/**
 * @brief Close device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           force                  device force close (true)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_CLOSE(UART, void *device_handle, bool force)
{
        UNUSED_ARG2(device_handle, force);

        return ESUCC;
}

//==============================================================================
/**
 * @brief Write data to device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *wrcnt                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_WRITE(UART,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG2(fpos, fattr);

        struct UART_mem *hdl = device_handle;

        int err = sys_mutex_lock(hdl->port_lock_tx_mtx, MTX_BLOCK_TIMEOUT);
        if (!err) {
                u32_t timeout = CEILING((count * 10000), hdl->config.baud) + 100;

                hdl->Tx_buffer.src_ptr   = src;
                hdl->Tx_buffer.data_size = count;
                SET_BIT(UART[hdl->major].UART->CR1, USART_CR1_TCIE);

                err = sys_semaphore_wait(hdl->data_write_sem, timeout);
                if (err) {
                        CLEAR_BIT(UART[hdl->major].UART->CR1, USART_CR1_TCIE);

                        if (hdl->Tx_buffer.data_size == 0) {
                                *wrcnt = count;
                                err = ESUCC;
                        } else {
                                *wrcnt = count - hdl->Tx_buffer.data_size;

                                printk("UART: write timeout (%d bytes left)",
                                       hdl->Tx_buffer.data_size);
                        }

                        hdl->Tx_buffer.data_size = 0;
                        hdl->Tx_buffer.src_ptr   = NULL;

                } else {
                        *wrcnt = count;
                }

                sys_mutex_unlock(hdl->port_lock_tx_mtx);
        }

        return err;
}

//==============================================================================
/**
 * @brief Read data from device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *rdcnt                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_READ(UART,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG2(fpos, fattr);

        struct UART_mem *hdl = device_handle;

        int err = sys_mutex_lock(hdl->port_lock_rx_mtx, MTX_BLOCK_TIMEOUT);
        if (!err) {
                *rdcnt = 0;

                while (count--) {
                        err = sys_semaphore_wait(hdl->data_read_sem, RX_WAIT_TIMEOUT);
                        if (!err) {
                                CLEAR_BIT(UART[hdl->major].UART->CR1, USART_CR1_RXNEIE);
                                if (FIFO_read(&hdl->Rx_FIFO, dst)) {
                                        dst++;
                                        (*rdcnt)++;
                                }
                                SET_BIT(UART[hdl->major].UART->CR1, USART_CR1_RXNEIE);
                        }
                }

                sys_mutex_unlock(hdl->port_lock_rx_mtx);
        }

        return err;
}

//==============================================================================
/**
 * @brief IO control
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_IOCTL(UART, void *device_handle, int request, void *arg)
{
        struct UART_mem *hdl = device_handle;
        int              err = EINVAL;

        if (arg) {
                switch (request) {
                case IOCTL_UART__SET_CONFIGURATION:
                        UART_configure(hdl->major, arg);
                        hdl->config = *cast(struct UART_config *, arg);
                        err = ESUCC;
                        break;

                case IOCTL_UART__GET_CONFIGURATION:
                        *cast(struct UART_config *, arg) = hdl->config;
                        err = ESUCC;
                        break;

                case IOCTL_UART__GET_CHAR_UNBLOCKING:
                        if (!FIFO_read(&hdl->Rx_FIFO, arg)) {
                                err = EAGAIN;
                        } else {
                                err = ESUCC;
                        }
                        break;

                default:
                        err = EBADRQC;
                        break;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Flush device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_FLUSH(UART, void *device_handle)
{
        UNUSED_ARG1(device_handle);

        return ESUCC;
}

//==============================================================================
/**
 * @brief Device information
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *device_stat            device status
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_STAT(UART, void *device_handle, struct vfs_dev_stat *device_stat)
{
        struct UART_mem *hdl = device_handle;

        device_stat->st_size  = 0;
        device_stat->st_major = hdl->major;
        device_stat->st_minor = 0;

        return ESUCC;
}

//==============================================================================
/**
 * @brief Function enable USART clock
 *
 * @param[in] UART              UART registers
 *
 * @return One of errno value
 */
//==============================================================================
static int UART_turn_on(const UART_regs_t *UART)
{
        if (!(*UART->APBENR & UART->APBENR_UARTEN)) {
                SET_BIT(*UART->APBRSTR, UART->APBRSTR_UARTRST);
                CLEAR_BIT(*UART->APBRSTR, UART->APBRSTR_UARTRST);
                SET_BIT(*UART->APBENR, UART->APBENR_UARTEN);
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
static int UART_turn_off(const UART_regs_t *UART)
{
        NVIC_DisableIRQ(UART->IRQn);
        SET_BIT(*UART->APBRSTR, UART->APBRSTR_UARTRST);
        CLEAR_BIT(*UART->APBRSTR, UART->APBRSTR_UARTRST);
        CLEAR_BIT(*UART->APBENR, UART->APBENR_UARTEN);
        return ESUCC;
}

//==============================================================================
/**
 * @brief Function configure selected UART
 *
 * @param major         major device number
 * @param config        configuration structure
 */
//==============================================================================
static void UART_configure(u8_t major, const struct UART_config *config)
{
        const UART_regs_t *DEV = &UART[major];

        /* set baud */
        RCC_ClocksTypeDef freq;
        RCC_GetClocksFreq(&freq);

        u32_t PCLK = (DEV->UART == USART1 || DEV->UART == USART6) ?
                     freq.PCLK2_Frequency : freq.PCLK1_Frequency;

        DEV->UART->BRR = (PCLK / (config->baud)) + 1;

        /* set 8 bit word length and wake idle line */
        CLEAR_BIT(DEV->UART->CR1, USART_CR1_M | USART_CR1_WAKE);

        /* set parity */
        switch (config->parity) {
        case UART_PARITY__OFF:
                CLEAR_BIT(DEV->UART->CR1, USART_CR1_PCE);
                break;
        case UART_PARITY__EVEN:
                SET_BIT(DEV->UART->CR1, USART_CR1_PCE);
                CLEAR_BIT(DEV->UART->CR1, USART_CR1_PS);
                break;
        case UART_PARITY__ODD:
                SET_BIT(DEV->UART->CR1, USART_CR1_PCE);
                SET_BIT(DEV->UART->CR1, USART_CR1_PS);
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
 * @brief Function write data to FIFO
 *
 * @param fifo          fifo buffer
 * @param data          data to write
 *
 * @return true if success, false on error
 */
//==============================================================================
static bool FIFO_write(struct Rx_FIFO *fifo, u8_t *data)
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
static bool FIFO_read(struct Rx_FIFO *fifo, u8_t *data)
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
static void IRQ_handle(u8_t major)
{
        bool yield = false;

        const UART_regs_t *DEV = &UART[major];

        /* receiver interrupt handler */
        int received = 0;
        while ((DEV->UART->CR1 & USART_CR1_RXNEIE) && (DEV->UART->SR & (USART_SR_RXNE | USART_SR_ORE))) {
                u8_t DR = DEV->UART->DR;

                if (FIFO_write(&UART_mem[major]->Rx_FIFO, &DR)) {
                        received++;
                }
        }

        /* transmitter interrupt handler */
        if ((DEV->UART->CR1 & USART_CR1_TCIE) && (DEV->UART->SR & USART_SR_TC)) {

                if (UART_mem[major]->Tx_buffer.data_size && UART_mem[major]->Tx_buffer.src_ptr) {
                        DEV->UART->DR = *(UART_mem[major]->Tx_buffer.src_ptr++);

                        if (--UART_mem[major]->Tx_buffer.data_size == 0) {
                                UART_mem[major]->Tx_buffer.src_ptr = NULL;
                        }
                } else {
                        CLEAR_BIT(DEV->UART->CR1, USART_CR1_TCIE);
                        sys_semaphore_signal_from_ISR(UART_mem[major]->data_write_sem, NULL);
                        yield = true;
                }
        }

        // set receive semaphore to number of received bytes
        while (received--) {
                sys_semaphore_signal_from_ISR(UART_mem[major]->data_read_sem, NULL);
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

/*==============================================================================
  End of file
==============================================================================*/
