/*==============================================================================
File    spi.c

Author  Daniel Zorychta

Brief   SPI driver.

        Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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

==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/driver.h"
#include "spi.h"
#include "spi_ioctl.h"
#include "gpio_ddi.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define DEV_LOCK_TIMEOUT        MAX_DELAY_MS

/*==============================================================================
  Local object types
==============================================================================*/
MODULE_NAME(SPI);

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void release_resources(u8_t major);
static void slave_select(struct SPI_slave *hdl);
static void slave_deselect(struct SPI_slave *hdl);

/*==============================================================================
  Local objects
==============================================================================*/
/* default SPI config */
static const SPI_config_t SPI_DEFAULT_CFG = {
        .flush_byte  = _SPI_DEFAULT_CFG_FLUSH_BYTE,
        .clk_divider = _SPI_DEFAULT_CFG_CLK_DIVIDER,
        .mode        = _SPI_DEFAULT_CFG_MODE,
        .msb_first   = _SPI_DEFAULT_CFG_MSB_FIRST,
        .CS_port_idx = 255,     // CS deactivated
        .CS_pin_idx  = 255,     // CS deactivated
        .CS_reverse  = false
};

/* pointers to memory of specified device */
struct SPI *_SPI[_NUMBER_OF_SPI_PERIPHERALS];

/*==============================================================================
  Exported objects
==============================================================================*/

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
API_MOD_INIT(SPI, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        UNUSED_ARG1(minor);

        int err = ENXIO;

        if (major >= _NUMBER_OF_SPI_PERIPHERALS) {
                return err;
        }

        /* initialize SPI peripheral */
        if (_SPI[major] == NULL) {
                err = sys_zalloc(sizeof(struct SPI), cast(void**, &_SPI[major]));
                if (err) {
                        goto finish;
                }

                err = sys_semaphore_create(1, 0, &_SPI[major]->wait_irq_sem);
                if (err) {
                        goto finish;
                }

                err = sys_mutex_create(MUTEX_TYPE_RECURSIVE, &_SPI[major]->periph_protect_mtx);
                if (err) {
                        goto finish;
                }

                err = _SPI_LLD__turn_on(major);
                if (err) {
                        goto finish;
                }
        }

        /* create SPI slave instance */
        err = sys_zalloc(sizeof(struct SPI_slave), device_handle);
        if (!err) {
                struct SPI_slave *hdl = *device_handle;
                hdl->config           = SPI_DEFAULT_CFG;
                hdl->major            = major;

                if (config) {
                        hdl->config = *cast(SPI_config_t*, config);
                }

                sys_device_unlock(&hdl->lock, true);

                _SPI[major]->slave_count++;
        }

        finish:
        if (err) {
                release_resources(major);
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
API_MOD_RELEASE(SPI, void *device_handle)
{
        struct SPI_slave *hdl = device_handle;

        int err = sys_device_lock(&hdl->lock);
        if (!err) {
                _SPI[hdl->major]->slave_count--;
                release_resources(hdl->major);
                sys_free(&device_handle);
        }

        return err;
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
API_MOD_OPEN(SPI, void *device_handle, u32_t flags)
{
        UNUSED_ARG1(flags);

        struct SPI_slave *hdl = device_handle;

        return sys_device_lock(&hdl->lock);
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
API_MOD_CLOSE(SPI, void *device_handle, bool force)
{
        struct SPI_slave *hdl = device_handle;

        return sys_device_unlock(&hdl->lock, force);
}

//==============================================================================
/**
 * @brief Write data to device. Function does not check that current task is
 *        allowed to write. This is filtered by the open() function. This design
 *        helps to handle other modules that uses directly files to transfer
 *        data. This allows modules to access file from other tasks
 *        (application tasks that not opened SPI file).
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
API_MOD_WRITE(SPI,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG2(fpos, fattr);

        struct SPI_slave *hdl = device_handle;

        // check if device is switched to RAW mode
        u32_t tref     = sys_time_get_reference();
        bool  RAW_mode = false;

        while (sys_device_is_locked(&_SPI[hdl->major]->RAW_mode)) {

                RAW_mode = true;

                if (sys_device_get_access(&_SPI[hdl->major]->RAW_mode) == ESUCC) {
                        break;
                } else {
                        if (sys_time_is_expired(tref, DEV_LOCK_TIMEOUT)) {
                                return ETIME;
                        } else {
                                sys_sleep_ms(10);
                        }
                }
        }

        int err = sys_mutex_lock(_SPI[hdl->major]->periph_protect_mtx, DEV_LOCK_TIMEOUT);
        if (!err) {
                if (not RAW_mode) {
                        slave_deselect(hdl);
                        _SPI_LLD__apply_config(hdl);
                        slave_select(hdl);
                }

                err = _SPI_LLD__transceive(hdl, src, NULL, count);
                if (!err) {
                        *wrcnt = count;
                }

                if (not RAW_mode) {
                        slave_deselect(hdl);
                }

                sys_mutex_unlock(_SPI[hdl->major]->periph_protect_mtx);
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
API_MOD_READ(SPI,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG2(fpos, fattr);

        struct SPI_slave *hdl = device_handle;

        // check if device is switched to RAW mode
        u32_t tref     = sys_time_get_reference();
        bool  RAW_mode = false;

        while (sys_device_is_locked(&_SPI[hdl->major]->RAW_mode)) {

                RAW_mode = true;

                if (sys_device_get_access(&_SPI[hdl->major]->RAW_mode) == ESUCC) {
                        break;
                } else {
                        if (sys_time_is_expired(tref, DEV_LOCK_TIMEOUT)) {
                                return ETIME;
                        } else {
                                sys_sleep_ms(10);
                        }
                }
        }

        int err = sys_mutex_lock(_SPI[hdl->major]->periph_protect_mtx, DEV_LOCK_TIMEOUT);
        if (!err) {
                if (not RAW_mode) {
                        slave_deselect(hdl);
                        _SPI_LLD__apply_config(hdl);
                        slave_select(hdl);
                }

                err = _SPI_LLD__transceive(hdl, NULL, dst, count);
                if (!err) {
                        *rdcnt = count;
                }

                if (not RAW_mode) {
                        slave_deselect(hdl);
                }

                sys_mutex_unlock(_SPI[hdl->major]->periph_protect_mtx);
        }

        return err;
}

//==============================================================================
/**
 * @brief IO control.  Function does not check that current task is
 *        allowed to write. This is filtered by the open() function. This design
 *        helps to handle other modules that uses directly files to transfer
 *        data. This allows modules to access file from other tasks
 *        (application tasks that not opened SPI file).
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_IOCTL(SPI, void *device_handle, int request, void *arg)
{
        struct SPI_slave *hdl = device_handle;
        int               err = EIO;

        switch (request) {
        case IOCTL_SPI__SET_CONFIGURATION:
                if (arg) {
                        hdl->config = *cast(SPI_config_t*, arg);
                        err = ESUCC;
                } else {
                        err = EINVAL;
                }
                break;

        case IOCTL_SPI__GET_CONFIGURATION:
                if (arg) {
                        *cast(SPI_config_t*, arg) = hdl->config;
                        err = ESUCC;
                } else {
                        err = EINVAL;
                }
                break;

        case IOCTL_SPI__SELECT:
                err = sys_mutex_trylock(_SPI[hdl->major]->periph_protect_mtx);
                if (!err) {
                        err = sys_device_lock(&_SPI[hdl->major]->RAW_mode);
                        if (!err) {
                                _SPI_LLD__apply_config(hdl);
                                slave_deselect(hdl);
                                slave_select(hdl);
                        }

                        sys_mutex_unlock(_SPI[hdl->major]->periph_protect_mtx);
                } else {
                        err = EBUSY;
                }
                break;

        case IOCTL_SPI__DESELECT:
                err = sys_mutex_trylock(_SPI[hdl->major]->periph_protect_mtx);
                if (!err) {
                        err = sys_device_unlock(&_SPI[hdl->major]->RAW_mode, false);
                        if (!err) {
                                slave_deselect(hdl);
                                _SPI_LLD__halt(hdl->major);
                        }

                        sys_mutex_unlock(_SPI[hdl->major]->periph_protect_mtx);
                } else {
                        err = EBUSY;
                }
                break;

        case IOCTL_SPI__TRANSCEIVE:
                if (arg) {
                        SPI_transceive_t *tr = arg;
                        if (tr->count) {
                                // check if device is switched to RAW mode
                                u32_t tref     = sys_time_get_reference();
                                bool  RAW_mode = false;

                                while (sys_device_is_locked(&_SPI[hdl->major]->RAW_mode)) {

                                        RAW_mode = true;

                                        if (sys_device_get_access(&_SPI[hdl->major]->RAW_mode) == ESUCC) {
                                                break;
                                        } else {
                                                if (sys_time_is_expired(tref, DEV_LOCK_TIMEOUT)) {
                                                        return ETIME;
                                                } else {
                                                        sys_sleep_ms(10);
                                                }
                                        }
                                }

                                err = sys_mutex_trylock(_SPI[hdl->major]->periph_protect_mtx);
                                if (!err) {
                                        if (not RAW_mode) {
                                                slave_deselect(hdl);
                                                _SPI_LLD__apply_config(hdl);
                                                slave_select(hdl);
                                        }

                                        for (SPI_transceive_t *t = tr; !err && t && t->count; t = t->next) {

                                                if (not RAW_mode) {
                                                        slave_select(hdl);
                                                }

                                                err = _SPI_LLD__transceive(hdl,
                                                                           t->tx_buffer,
                                                                           t->rx_buffer,
                                                                           t->count);

                                                if (not RAW_mode && t->separated) {
                                                        slave_deselect(hdl);
                                                }
                                        }

                                        if (not RAW_mode) {
                                                slave_deselect(hdl);
                                        }

                                        sys_mutex_unlock(_SPI[hdl->major]->periph_protect_mtx);
                                } else {
                                        err = EBUSY;
                                }
                        } else {
                                err = EINVAL;
                        }
                } else {
                        err = EINVAL;
                }
                break;

        case IOCTL_SPI__TRANSMIT_NO_SELECT:
                if (arg) {
                        const u8_t *byte = arg;

                        err = sys_mutex_trylock(_SPI[hdl->major]->periph_protect_mtx);
                        if (!err) {
                                slave_deselect(hdl);
                                _SPI_LLD__apply_config(hdl);

                                if (_SPI_LLD__transceive(hdl, byte, NULL, 1)) {
                                        err = ESUCC;
                                }

                                sys_mutex_unlock(_SPI[hdl->major]->periph_protect_mtx);
                        } else {
                                err = EBUSY;
                        }
                } else {
                        err = EINVAL;
                }
                break;

        default:
                err = EBADRQC;
                break;
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
API_MOD_FLUSH(SPI, void *device_handle)
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
API_MOD_STAT(SPI, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG1(device_handle);

        device_stat->st_size = 0;

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Release allocated resources
 * @param  major        SPI major number
 * @return None
 */
//==============================================================================
static void release_resources(u8_t major)
{
        if (_SPI[major] && _SPI[major]->slave_count == 0) {
                if (_SPI[major]->wait_irq_sem) {
                        sys_semaphore_destroy(_SPI[major]->wait_irq_sem);
                        _SPI[major]->wait_irq_sem = NULL;
                }

                if (_SPI[major]->periph_protect_mtx) {
                        sys_mutex_destroy(_SPI[major]->periph_protect_mtx);
                        _SPI[major]->periph_protect_mtx = NULL;
                }

                _SPI_LLD__turn_off(major);

                sys_free(cast(void**, &_SPI[major]));
        }
}

//==============================================================================
/**
 * @brief Function select slave device
 *
 * @param hdl           SPI slave
 */
//==============================================================================
static void slave_select(struct SPI_slave *hdl)
{
    (hdl->config.CS_reverse == true) ?  _GPIO_DDI_set_pin(hdl->config.CS_port_idx, hdl->config.CS_pin_idx):
                                        _GPIO_DDI_clear_pin(hdl->config.CS_port_idx, hdl->config.CS_pin_idx);
}

//==============================================================================
/**
 * @brief Function deselect current slave device
 *
 * @param hdl           SPI slave
 */
//==============================================================================
static void slave_deselect(struct SPI_slave *hdl)
{
    (hdl->config.CS_reverse == true) ?  _GPIO_DDI_clear_pin(hdl->config.CS_port_idx, hdl->config.CS_pin_idx):
                                        _GPIO_DDI_set_pin(hdl->config.CS_port_idx, hdl->config.CS_pin_idx);
}

/*==============================================================================
  End of file
==============================================================================*/
