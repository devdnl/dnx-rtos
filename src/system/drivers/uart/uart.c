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
#define RX_WAIT_TIMEOUT                         MAX_DELAY_MS
#define TX_WAIT_TIMEOUT                         300000
#define MTX_BLOCK_TIMEOUT                       MAX_DELAY_MS

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static bool _UART_FIFO__read(struct Rx_FIFO *fifo, u8_t *data);

/*==============================================================================
  Local object definitions
==============================================================================*/
MODULE_NAME(UART);

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
struct UART_mem *_UART_mem[_UART_COUNT];

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
                _UART_mem[major] = *device_handle;

                err = sys_semaphore_create(1, 0, &_UART_mem[major]->write_ready_sem);
                if (err)
                        goto finish;

                err = sys_semaphore_create(_UART_RX_BUFFER_SIZE, 0, &_UART_mem[major]->data_read_sem);
                if (err)
                        goto finish;

                err = sys_mutex_create(MUTEX_TYPE_NORMAL, &_UART_mem[major]->port_lock_rx_mtx);
                if (err)
                        goto finish;

                err = sys_mutex_create(MUTEX_TYPE_NORMAL, &_UART_mem[major]->port_lock_tx_mtx);
                if (err)
                        goto finish;

                err = _UART_LLD__turn_on(major);
                if (!err) {
                        _UART_mem[major]->major  = major;
                        _UART_mem[major]->config = UART_DEFAULT_CONFIG;
                        _UART_LLD__configure(major, &UART_DEFAULT_CONFIG);
                }

                finish:
                if (err) {
                        if (_UART_mem[major]->port_lock_tx_mtx)
                                sys_mutex_destroy(_UART_mem[major]->port_lock_tx_mtx);

                        if (_UART_mem[major]->port_lock_rx_mtx)
                                sys_mutex_destroy(_UART_mem[major]->port_lock_rx_mtx);

                        if (_UART_mem[major]->write_ready_sem)
                                sys_semaphore_destroy(_UART_mem[major]->write_ready_sem);

                        if (_UART_mem[major]->write_ready_sem)
                                sys_semaphore_destroy(_UART_mem[major]->write_ready_sem);

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

                        sys_semaphore_destroy(hdl->write_ready_sem);

                        _UART_LLD__turn_off(hdl->major);

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
                u32_t timeout = TX_WAIT_TIMEOUT;

                hdl->Tx_buffer.src_ptr   = src;
                hdl->Tx_buffer.data_size = count;
                _UART_LLD__transmit(hdl->major);

                err = sys_semaphore_wait(hdl->write_ready_sem, timeout);
                if (err) {
                        _UART_LLD__abort_trasmission(hdl->major);

                        if (hdl->Tx_buffer.data_size == 0) {
                                *wrcnt = count;
                                err    = ESUCC;
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
                        err = sys_semaphore_wait(hdl->data_read_sem,
                                                 fattr.non_blocking_rd ?
                                                 0 : RX_WAIT_TIMEOUT);
                        if (!err) {
                                _UART_LLD__rx_hold(hdl->major);
                                if (_UART_FIFO__read(&hdl->Rx_FIFO, dst)) {
                                        dst++;
                                        (*rdcnt)++;
                                }
                                _UART_LLD__rx_resume(hdl->major);
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
                        _UART_LLD__configure(hdl->major, arg);
                        hdl->config = *cast(struct UART_config *, arg);
                        err = ESUCC;
                        break;

                case IOCTL_UART__GET_CONFIGURATION:
                        *cast(struct UART_config *, arg) = hdl->config;
                        err = ESUCC;
                        break;

                case IOCTL_UART__GET_CHAR_UNBLOCKING:
                        if (!_UART_FIFO__read(&hdl->Rx_FIFO, arg)) {
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

        _UART_LLD__rx_hold(hdl->major);
        device_stat->st_size = hdl->Rx_FIFO.buffer_level;
        _UART_LLD__rx_resume(hdl->major);

        return ESUCC;
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
bool _UART_FIFO__write(struct Rx_FIFO *fifo, u8_t *data)
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
static bool _UART_FIFO__read(struct Rx_FIFO *fifo, u8_t *data)
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

/*==============================================================================
  End of file
==============================================================================*/
