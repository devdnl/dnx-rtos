/*=========================================================================*//**
@file    i2cee.c

@author  Daniel Zorychta

@brief   I2C EEPROM driver.

@note    Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "noarch/i2cee_cfg.h"
#include "../i2cee_ioctl.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define MUTEX_TIMEOUT   1000

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        FILE    *i2c_dev;
        mutex_t *mtx;
        u32_t    memory_size;
        u16_t    page_size;
        u16_t    page_prog_time_ms;
} I2CEE_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int configure(I2CEE_t *hdl, const I2CEE_config_t *cfg);

/*==============================================================================
  Local objects
==============================================================================*/
MODULE_NAME(I2CEE);

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
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
API_MOD_INIT(I2CEE, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        UNUSED_ARG1(major);

        if (minor != 0) {
                return ENODEV;
        }

        int err = sys_zalloc(sizeof(I2CEE_t), device_handle);
        if (!err) {
                I2CEE_t *hdl = *device_handle;

                err = sys_mutex_create(MUTEX_TYPE_RECURSIVE, &hdl->mtx);

                if (!err && config) {
                        err = configure(hdl, config);
                }

                if (err) {
                        if (hdl->mtx) {
                                sys_mutex_destroy(hdl->mtx);
                        }

                        sys_free(device_handle);
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
API_MOD_RELEASE(I2CEE, void *device_handle)
{
        I2CEE_t *hdl = device_handle;

        int err = sys_mutex_lock(hdl->mtx, 0);
        if (!err) {
                mutex_t *mtx = hdl->mtx;
                hdl->mtx = 0;
                sys_mutex_unlock(mtx);
                sys_mutex_destroy(mtx);

                if (hdl->i2c_dev) {
                        sys_fclose(hdl->i2c_dev);
                }

                memset(hdl, 0, sizeof(I2CEE_t));
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
API_MOD_OPEN(I2CEE, void *device_handle, u32_t flags)
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
API_MOD_CLOSE(I2CEE, void *device_handle, bool force)
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
API_MOD_WRITE(I2CEE,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG1(fattr);

        I2CEE_t *hdl = device_handle;

        int err = sys_mutex_lock(hdl->mtx, MUTEX_TIMEOUT);
        if (!err) {

                if (*fpos < hdl->memory_size) {
                        u32_t addr = *fpos;
                        err = sys_fseek(hdl->i2c_dev, addr, VFS_SEEK_SET);

                        count = min(count, *fpos - hdl->memory_size);

                        while (!err && count) {
                                size_t pbleft = (((addr / hdl->page_size) + 1) * hdl->page_size) - addr;
                                size_t wrsz   = min(count, pbleft);
                                size_t wrb    = 0;

                                err = sys_fwrite(src, wrsz, &wrb, hdl->i2c_dev);
                                if (!err) {
                                        sys_sleep_ms(hdl->page_prog_time_ms);
                                        addr   += wrsz;
                                        src    += wrsz;
                                        count  -= wrsz;
                                        *wrcnt += wrb;
                                }
                        }
                } else {
                        printk("I2CEE: write out of range 0x%02X", (u32_t)*fpos);
                        *wrcnt = 0;
                }

                sys_mutex_unlock(hdl->mtx);
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
API_MOD_READ(I2CEE,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fattr);

        I2CEE_t *hdl = device_handle;

        int err = sys_mutex_lock(hdl->mtx, MUTEX_TIMEOUT);
        if (!err) {

                if (*fpos < hdl->memory_size) {
                        count = min(count, *fpos - hdl->memory_size);

                        err = sys_fseek(hdl->i2c_dev, *fpos, VFS_SEEK_SET);
                        if (!err) {
                                err = sys_fread(dst, count, rdcnt, hdl->i2c_dev);
                        }
                } else {
                        printk("I2CEE: read out of range 0x%02X", (u32_t)*fpos);
                        *rdcnt = 0;
                }

                sys_mutex_unlock(hdl->mtx);
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
API_MOD_IOCTL(I2CEE, void *device_handle, int request, void *arg)
{
        I2CEE_t *hdl = device_handle;
        int      err = EINVAL;

        if (arg) {
                char pathbuf[32];
                I2CEE_config_t cfg;

                switch (request) {
                case IOCTL_I2CEE__CONFIGURE_STR: {

                        size_t pathlen = sys_stropt_get_string_copy(arg, "i2c_path",
                                                                    pathbuf, sizeof(pathbuf));
                        cfg.i2c_path = pathbuf;
                        cfg.memory_size = sys_stropt_get_int(arg, "memory_size", 0);
                        cfg.page_prog_time_ms = sys_stropt_get_int(arg, "page_prog_time_ms", 0);
                        cfg.page_size = sys_stropt_get_int(arg, "page_size", 0);

                        if (pathlen == 0 || cfg.page_size == 0 || cfg.memory_size == 0) {
                                err = EINVAL;
                        } else {
                                err = configure(hdl, &cfg);
                        }
                        break;
                }

                case IOCTL_I2CEE__CONFIGURE: {
                        err = configure(hdl, arg);
                        break;
                }

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
API_MOD_FLUSH(I2CEE, void *device_handle)
{
        I2CEE_t *hdl = device_handle;

        return sys_fflush(hdl->i2c_dev);
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
API_MOD_STAT(I2CEE, void *device_handle, struct vfs_dev_stat *device_stat)
{
        I2CEE_t *hdl = device_handle;

        device_stat->st_size = hdl->memory_size;

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function configure driver parameters.
 *
 * @param  hdl          driver handle
 * @param  cfg          configuration
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int configure(I2CEE_t *hdl, const I2CEE_config_t *cfg)
{
        int err = sys_mutex_lock(hdl->mtx, MUTEX_TIMEOUT);
        if (!err) {
                if (hdl->i2c_dev) {
                        err = sys_fclose(hdl->i2c_dev);
                }

                if (!err) {
                        err = sys_fopen(cfg->i2c_path, "r+", &hdl->i2c_dev);
                        if (!err) {
                                hdl->memory_size       = cfg->memory_size;
                                hdl->page_size         = cfg->page_size;
                                hdl->page_prog_time_ms = cfg->page_prog_time_ms + 1;
                        }
                }

                sys_mutex_unlock(hdl->mtx);
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
