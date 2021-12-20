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
#include "uart.h"
#include "uart_ioctl.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define RELEASE_TIMEOUT                         100
#define RX_WAIT_TIMEOUT                         _MAX_DELAY_MS
#define TX_WAIT_TIMEOUT                         300000
#define MTX_BLOCK_TIMEOUT                       _MAX_DELAY_MS

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
MODULE_NAME(UART);

/* UART default configuration */
static const struct UART_rich_config UART_DEFAULT_CONFIG = {
        .basic.parity    = __UART_DEFAULT_PARITY__,
        .basic.stop_bits = __UART_DEFAULT_STOP_BITS__,
        .basic.tx_enable = __UART_DEFAULT_TX_ENABLE__,
        .basic.rx_enable = __UART_DEFAULT_RX_ENABLE__,
        .basic.baud      = __UART_DEFAULT_BAUD__,
        .LIN_break_bits  = UART_LIN_BREAK__10_BITS,
        .RS485_DE_pin    = {port_idx:IOCTL_GPIO_PIN_IDX__NONE, pin_idx:IOCTL_GPIO_PIN_IDX__NONE},
        .basic.features  = 0,
};

/* structure which identify USARTs data in the IRQs */
struct UART_mem *_UART_mem[_UART_COUNT];

/* RX FIFO length */
static const uint16_t RX_BUF_LEN[] = {
        #ifdef __UART_MAJOR0_RX_BUFFER_LEN__
        __UART_MAJOR0_RX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR1_RX_BUFFER_LEN__
        __UART_MAJOR1_RX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR2_RX_BUFFER_LEN__
        __UART_MAJOR2_RX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR3_RX_BUFFER_LEN__
        __UART_MAJOR3_RX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR4_RX_BUFFER_LEN__
        __UART_MAJOR4_RX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR5_RX_BUFFER_LEN__
        __UART_MAJOR5_RX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR6_RX_BUFFER_LEN__
        __UART_MAJOR6_RX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR7_RX_BUFFER_LEN__
        __UART_MAJOR7_RX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR8_RX_BUFFER_LEN__
        __UART_MAJOR8_RX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR9_RX_BUFFER_LEN__
        __UART_MAJOR9_RX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR10_RX_BUFFER_LEN__
        __UART_MAJOR10_RX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR11_RX_BUFFER_LEN__
        __UART_MAJOR111_RX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR12_RX_BUFFER_LEN__
        __UART_MAJOR12_RX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR13_RX_BUFFER_LEN__
        __UART_MAJOR13_RX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR14_RX_BUFFER_LEN__
        __UART_MAJOR14_RX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR15_RX_BUFFER_LEN__
        __UART_MAJOR15_RX_BUFFER_LEN__,
        #endif
};

/* TX FIFO length */
static const uint16_t TX_BUF_LEN[] = {
        #ifdef __UART_MAJOR0_TX_BUFFER_LEN__
        __UART_MAJOR0_TX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR1_TX_BUFFER_LEN__
        __UART_MAJOR1_TX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR2_TX_BUFFER_LEN__
        __UART_MAJOR2_TX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR3_TX_BUFFER_LEN__
        __UART_MAJOR3_TX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR4_TX_BUFFER_LEN__
        __UART_MAJOR4_TX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR5_TX_BUFFER_LEN__
        __UART_MAJOR5_TX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR6_TX_BUFFER_LEN__
        __UART_MAJOR6_TX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR7_TX_BUFFER_LEN__
        __UART_MAJOR7_TX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR8_TX_BUFFER_LEN__
        __UART_MAJOR8_TX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR9_TX_BUFFER_LEN__
        __UART_MAJOR9_TX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR10_TX_BUFFER_LEN__
        __UART_MAJOR10_TX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR11_TX_BUFFER_LEN__
        __UART_MAJOR111_TX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR12_TX_BUFFER_LEN__
        __UART_MAJOR12_TX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR13_TX_BUFFER_LEN__
        __UART_MAJOR13_TX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR14_TX_BUFFER_LEN__
        __UART_MAJOR14_TX_BUFFER_LEN__,
        #endif
        #ifdef __UART_MAJOR15_TX_BUFFER_LEN__
        __UART_MAJOR15_TX_BUFFER_LEN__,
        #endif
};

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
 * @param[in ]            config               optional module configuration
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_INIT(UART, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        if (major >= _UART_COUNT || minor != 0) {
                return ENODEV;
        }

        int err = sys_zalloc(sizeof(struct UART_mem), device_handle);
        if (!err) {
                _UART_mem[major] = *device_handle;
                struct UART_mem *hdl = _UART_mem[major];

                hdl->major = major;
                hdl->minor = minor;

                err = sys_queue_create(RX_BUF_LEN[hdl->major], sizeof(char), &hdl->rx_queue);
                if (err)
                        goto finish;

                err = sys_queue_create(TX_BUF_LEN[hdl->major], sizeof(char), &hdl->tx_queue);
                if (err)
                        goto finish;

                err = sys_mutex_create(KMTX_TYPE_NORMAL, &hdl->port_lock_rx_mtx);
                if (err)
                        goto finish;

                err = sys_mutex_create(KMTX_TYPE_NORMAL, &hdl->port_lock_tx_mtx);
                if (err)
                        goto finish;

                err = _UART_LLD__turn_on(hdl);
                if (!err) {
                        hdl->config = UART_DEFAULT_CONFIG;

                        if (config) {
                                hdl->config.basic = *cast(struct UART_config *, config);
                        }

                        _UART_LLD__configure(hdl, &hdl->config);
                }

                finish:
                if (err) {
                        if (hdl->port_lock_tx_mtx)
                                sys_mutex_destroy(hdl->port_lock_tx_mtx);

                        if (hdl->port_lock_rx_mtx)
                                sys_mutex_destroy(hdl->port_lock_rx_mtx);

                        if (hdl->rx_queue)
                                sys_queue_destroy(hdl->rx_queue);

                        if (hdl->tx_queue)
                                sys_queue_destroy(hdl->tx_queue);

                        sys_free(device_handle);
                        _UART_mem[major] = NULL;
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

                        sys_queue_destroy(hdl->rx_queue);
                        sys_queue_destroy(hdl->tx_queue);

                        _UART_LLD__turn_off(hdl);

                        _UART_mem[hdl->major] = NULL;
                        sys_free(&device_handle);

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
                while (count--) {
                        sys_queue_send(hdl->tx_queue, src, TX_WAIT_TIMEOUT);
                        _UART_LLD__resume_transmit(hdl);
                        src++;
                        (*wrcnt)++;
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
                        u32_t timeout = fattr.non_blocking_rd ? 0 : RX_WAIT_TIMEOUT;

                        err = sys_queue_receive(hdl->rx_queue, dst, timeout);
                        if (!err) {
                                dst++;
                                (*rdcnt)++;
                        } else {
                                if (fattr.non_blocking_rd) {
                                        err = 0;
                                }
                                break;
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
                        hdl->config.basic                 = *cast(struct UART_config *, arg);
                        hdl->config.LIN_break_bits        = UART_LIN_BREAK__10_BITS;
                        hdl->config.RS485_DE_pin.port_idx = IOCTL_GPIO_PIN_IDX__NONE;
                        hdl->config.RS485_DE_pin.pin_idx  = IOCTL_GPIO_PIN_IDX__NONE;
                        err = _UART_LLD__configure(hdl, &hdl->config);
                        break;

                case IOCTL_UART__GET_CONFIGURATION:
                        *cast(struct UART_config *, arg) = hdl->config.basic;
                        err = ESUCC;
                        break;

                case IOCTL_UART__GET_CHAR_UNBLOCKING:
                        err = sys_queue_receive(hdl->rx_queue, arg, 0);
                        if (err == ETIME) err = EAGAIN;
                        break;

                case IOCTL_UART__SET_RS485_DE_PIN:
                        hdl->config.RS485_DE_pin = *cast(const GPIO_pin_in_port_t*, arg);
                        err = ESUCC;
                        break;

                case IOCTL_UART__SET_LIN_BREAK_BITS:
                        hdl->config.LIN_break_bits = *cast(enum UART_LIN_break *, arg);
                        err = ESUCC;
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
        struct UART_mem *hdl = device_handle;

        int err = 0;
        while (!err) {
                size_t items = 0;
                err = sys_queue_get_number_of_items(hdl->tx_queue, &items);
                if (items == 0) {
                        break;
                } else {
                        sys_sleep_ms(5);
                }
        }

        return err;
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

        size_t items = 0;
        sys_queue_get_number_of_items(hdl->rx_queue, &items);
        device_stat->st_size = items;

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function enable/disable RS485 DE pin.
 *
 * @param  hdl          driver handle
 * @param  enable       enable state
 */
//==============================================================================
void _UART_set_DE_pin(struct UART_mem *hdl, bool enable)
{
        if (enable) {
                _GPIO_DDI_set_pin(hdl->config.RS485_DE_pin.port_idx, hdl->config.RS485_DE_pin.pin_idx);
        } else {
                _GPIO_DDI_clear_pin(hdl->config.RS485_DE_pin.port_idx, hdl->config.RS485_DE_pin.pin_idx);
        }
}

/*==============================================================================
  End of file
==============================================================================*/
