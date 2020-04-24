/*=========================================================================*//**
@file    i2c.c

@author  Daniel Zorychta

@brief   This driver support I2C peripherals.

@note    Copyright (C) 2017  Daniel Zorychta <daniel.zorychta@gmail.com>

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

// NOTE: 10-bit addressing mode is experimental and not tested!

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/driver.h"
#include "i2c.h"
#include "i2c_ioctl.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define ACCESS_TIMEOUT         30000

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void release_resources(u8_t major);
static int send_subaddress(I2C_dev_t *hdl, u32_t address, I2C_sub_addr_mode_t mode);
static int configure(I2C_dev_t *hdl, const I2C_config_t *config);

/*==============================================================================
  Local object definitions
==============================================================================*/
MODULE_NAME(I2C);

/// default peripheral configuration
static const I2C_config_t I2C_DEFAULT_CFG = {
        .address       = 0x00,
        .addr_10bit    = false,
        .sub_addr_mode = I2C_SUB_ADDR_MODE__DISABLED,
        .slave_mode    = false
};

/// main memory of module
I2C_mem_t *_I2C[_I2C_NUMBER_OF_PERIPHERALS];

/*==============================================================================
  Exported object definitions
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
API_MOD_INIT(I2C, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        int err = ENODEV;

        if (major >= _I2C_NUMBER_OF_PERIPHERALS) {
                return err;
        }

        /* creates basic module structures */
        if (_I2C[major] == NULL) {
                err = sys_zalloc(sizeof(I2C_mem_t), cast(void**, &_I2C[major]));
                if (err) {
                        goto finish;
                }

                err = sys_mutex_create(MUTEX_TYPE_NORMAL, &_I2C[major]->lock_mtx);
                if (err) {
                        goto finish;
                }

                err = sys_queue_create(1, sizeof(int), &_I2C[major]->event);
                if (err) {
                        goto finish;
                }

                err = _I2C_LLD__init(major);
                if (err) {
                        goto finish;
                }

                _I2C[major]->major = major;
        }

        /* creates device structure */
        err = sys_zalloc(sizeof(I2C_dev_t), device_handle);
        if (!err) {
                I2C_dev_t *hdl = *device_handle;
                hdl->config    = I2C_DEFAULT_CFG;
                hdl->major     = major;
                hdl->minor     = minor;

                if (!err && config) {
                        err = configure(hdl, config);
                }

                if (!err) {
                        sys_device_unlock(&hdl->lock_dev, true);

                        _I2C[major]->dev_cnt++;
                }
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
API_MOD_RELEASE(I2C, void *device_handle)
{
        I2C_dev_t *hdl = device_handle;

        int err = sys_device_lock(&hdl->lock_dev);
        if (!err) {
                _I2C[hdl->major]->dev_cnt--;
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
API_MOD_OPEN(I2C, void *device_handle, u32_t flags)
{
        UNUSED_ARG1(flags);

        I2C_dev_t *hdl = device_handle;

        return sys_device_lock(&hdl->lock_dev);
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
API_MOD_CLOSE(I2C, void *device_handle, bool force)
{
        I2C_dev_t *hdl = device_handle;

        return sys_device_unlock(&hdl->lock_dev, force);
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
API_MOD_WRITE(I2C,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG1(fattr);

        I2C_dev_t *hdl = device_handle;

        int err = sys_mutex_lock(_I2C[hdl->major]->lock_mtx, ACCESS_TIMEOUT);
        if (!err) {
                if (hdl->config.slave_mode) {
                        err = _I2C_LLD__slave_transmit(hdl, src, count, wrcnt);

                } else {
                        err = _I2C_LLD__start(hdl);
                        if (err) {
                                printk("I2C%d:%d start error", hdl->major, hdl->minor);
                                goto error;
                        }

                        err = _I2C_LLD__send_address(hdl, true, count);
                        if (err) {
                                printk("I2C%d:%d address %Xh error",
                                       hdl->major, hdl->minor, hdl->config.address);
                                goto error;
                        }

                        if (hdl->config.sub_addr_mode != I2C_SUB_ADDR_MODE__DISABLED) {
                                err = send_subaddress(hdl, *fpos, hdl->config.sub_addr_mode);
                                if (err) {
                                        printk("I2C%d:%d subaddress error", hdl->major, hdl->minor);
                                        goto error;
                                }
                        }

                        err = _I2C_LLD__transmit(hdl, src, count, wrcnt);

                        error:
                        _I2C_LLD__stop(hdl);
                }

                sys_mutex_unlock(_I2C[hdl->major]->lock_mtx);
        }

        if (err) {
                printk("I2C%d:%d write error %d", hdl->major, hdl->minor, err);
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
API_MOD_READ(I2C,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fattr);

        I2C_dev_t *hdl = device_handle;

        int err = sys_mutex_lock(_I2C[hdl->major]->lock_mtx, ACCESS_TIMEOUT);
        if (!err) {
                if (hdl->config.slave_mode) {
                        err = _I2C_LLD__slave_receive(hdl, dst, count, rdcnt);

                } else {
                        if (hdl->config.sub_addr_mode != I2C_SUB_ADDR_MODE__DISABLED) {
                                err = _I2C_LLD__start(hdl);
                                if (err) {
                                        printk("I2C%d:%d start error", hdl->major, hdl->minor, err);
                                        goto error;
                                }

                                err = _I2C_LLD__send_address(hdl, true, count);
                                if (err) {
                                        printk("I2C%d:%d address %Xh error",
                                               hdl->major, hdl->minor, hdl->config.address);
                                        goto error;
                                }

                                err = send_subaddress(hdl, *fpos, hdl->config.sub_addr_mode);
                                if (err) {
                                        printk("I2C%d:%d subaddress error", hdl->major, hdl->minor);
                                        goto error;
                                }
                        }

                        err = _I2C_LLD__repeat_start(hdl);
                        if (err) {
                                printk("I2C%d:%d repeat start error", hdl->major, hdl->minor);
                                goto error;
                        }

                        err = _I2C_LLD__send_address(hdl, false, count);
                        if (err) {
                                printk("I2C%d:%d address %Xh error",
                                       hdl->major, hdl->minor, hdl->config.address);
                                goto error;
                        }

                        err = _I2C_LLD__receive(hdl, dst, count, rdcnt);
                        if (err) {
                                printk("I2C%d:%d receive error", hdl->major, hdl->minor);
                        }

                        error:
                        _I2C_LLD__stop(hdl);
                }

                sys_mutex_unlock(_I2C[hdl->major]->lock_mtx);
        }

        if (err) {
                printk("I2C%d:%d read error %d", hdl->major, hdl->minor, err);
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
API_MOD_IOCTL(I2C, void *device_handle, int request, void *arg)
{
        I2C_dev_t *hdl = device_handle;
        int        err = EINVAL;

        if (arg) {
                switch (request) {
                case IOCTL_I2C__CONFIGURE:
                        err = configure(hdl, arg);
                        break;

                case IOCTL_I2C__SLAVE_WAIT_FOR_SELECTION: {
                        I2C_selection_t *event = arg;
                        err = sys_mutex_lock(_I2C[hdl->major]->lock_mtx, event->timeout_ms);
                        if (!err) {
                                err = _I2C_LLD__slave_wait_for_selection(hdl, event);
                                sys_mutex_unlock(_I2C[hdl->major]->lock_mtx);
                        }
                        break;
                }

                case IOCTL_I2C__CONFIGURE_STR:
                        err = sys_mutex_lock(_I2C[hdl->major]->lock_mtx, ACCESS_TIMEOUT);
                        if (!err) {
                                hdl->config.addr_10bit    = sys_stropt_get_bool(arg, "addr_10bit", false);
                                hdl->config.address       = sys_stropt_get_int(arg, "address", 0);
                                hdl->config.slave_mode    = sys_stropt_get_bool(arg, "slave_mode", false);
                                hdl->config.sub_addr_mode = sys_stropt_get_int(arg, "sub_addr_mode", 0);

                                err = _I2C_LLD__slave_mode_setup(hdl);
                                sys_mutex_unlock(_I2C[hdl->major]->lock_mtx);
                        }
                        break;

                case IOCTL_I2C__CONFIGURE_RECOVERY:
                        err = sys_mutex_lock(_I2C[hdl->major]->lock_mtx, ACCESS_TIMEOUT);
                        if (!err) {
                                _I2C[hdl->major]->recovery = *cast(const I2C_recovery_t*, arg);
                                sys_mutex_unlock(_I2C[hdl->major]->lock_mtx);
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
API_MOD_FLUSH(I2C, void *device_handle)
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
API_MOD_STAT(I2C, void *device_handle, struct vfs_dev_stat *device_stat)
{
        I2C_dev_t *hdl = device_handle;

        device_stat->st_size  = 0;
        device_stat->st_major = hdl->major;
        device_stat->st_minor = hdl->minor;

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function release all resource allocated during initialization phase
 *
 * @param  major         major device number
 */
//==============================================================================
static void release_resources(u8_t major)
{
        if (_I2C[major] && _I2C[major]->dev_cnt == 0) {
                if (_I2C[major]->lock_mtx) {
                        sys_mutex_destroy(_I2C[major]->lock_mtx);
                        _I2C[major]->lock_mtx = NULL;
                }

                if (_I2C[major]->event) {
                        sys_queue_destroy(_I2C[major]->event);
                        _I2C[major]->event = NULL;
                }

                if (_I2C[major]->initialized) {
                        _I2C_LLD__release(major);
                }

                sys_free(cast(void**, &_I2C[major]));
        }
}

//==============================================================================
/**
 * @brief  Function send subaddress to I2C device
 *
 * @param  hdl                  device handle
 * @param  address              subaddress
 * @param  mode                 size of subaddress
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int send_subaddress(I2C_dev_t *hdl, u32_t address, I2C_sub_addr_mode_t mode)
{
        int  err = 0;
        u8_t n   = 0;
        u8_t addr[4];

        if (mode >= I2C_SUB_ADDR_MODE__3_BYTES) {
                addr[n++] = address >> 16;
        }

        if (mode >= I2C_SUB_ADDR_MODE__2_BYTES) {
                addr[n++] = address >> 8;
        }

        if (mode >= I2C_SUB_ADDR_MODE__1_BYTE) {
                addr[n++] = address & 0xFF;

                size_t wrcnt = 0;
                err = _I2C_LLD__transmit(hdl, addr, n, &wrcnt);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function configure I2C device.
 *
 * @param  hdl                  device handle
 * @param  config               configuration
 *
 * @return One of errno value.
 */
//==============================================================================
static int configure(I2C_dev_t *hdl, const I2C_config_t *config)
{
        int err = sys_mutex_lock(_I2C[hdl->major]->lock_mtx, ACCESS_TIMEOUT);
        if (!err) {
                hdl->config = *config;
                err = _I2C_LLD__slave_mode_setup(hdl);
                sys_mutex_unlock(_I2C[hdl->major]->lock_mtx);
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
