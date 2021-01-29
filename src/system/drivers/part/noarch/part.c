/*==============================================================================
File    part.c

Author  Daniel Zorychta

Brief   Partition driver

        Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "../part_ioctl.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define MUTEX_TIMEOUT_ms        5000

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        FILE    *dev;
        mutex_t *mutex;
        u32_t    block_size;
        u32_t    offset_blocks;
        u32_t    total_blocks;
} PART_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int configure(PART_t *hdl, const PART_config_t *conf);

/*==============================================================================
  Local object
==============================================================================*/
MODULE_NAME(PART);

/*==============================================================================
  Exported object
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize device.
 *
 * @param[out]          **device_handle        device allocated memory
 * @param[in ]            major                major device number
 * @param[in ]            minor                minor device number
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_INIT(PART, void **device_handle, u8_t major, u8_t minor)
{
        UNUSED_ARG1(major);

        int err = EFAULT;

        if (minor == 0) {
                err = sys_zalloc(sizeof(PART_t), device_handle);
                if (!err) {
                        PART_t *hdl = *device_handle;

                        err = sys_mutex_create(MUTEX_TYPE_NORMAL, &hdl->mutex);

                        if (err) {
                                sys_free(device_handle);
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Release device.
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_RELEASE(PART, void *device_handle)
{
        PART_t *hdl = device_handle;

        int err = sys_mutex_lock(hdl->mutex, 0);
        if (!err) {
                mutex_t *mtx = hdl->mutex;
                hdl->mutex = NULL;
                sys_mutex_unlock(mtx);
                sys_mutex_destroy(mtx);

                if (hdl->dev) {
                        sys_fclose(hdl->dev);
                }

                sys_free(&device_handle);
        }

        return ESUCC;
}

//==============================================================================
/**
 * @brief Open device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           flags                  file operation flags (O_RDONLY, O_WRONLY, O_RDWR)
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_OPEN(PART, void *device_handle, u32_t flags)
{
        UNUSED_ARG2(device_handle, flags);
        return ESUCC;
}

//==============================================================================
/**
 * @brief Close device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           force                  device force close (true)
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_CLOSE(PART, void *device_handle, bool force)
{
        UNUSED_ARG2(device_handle, force);
        return ESUCC;
}

//==============================================================================
/**
 * @brief Write data to device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *wrcnt                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_WRITE(PART,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG1(fattr);

        PART_t *hdl = device_handle;

        int err = sys_mutex_lock(hdl->mutex, MUTEX_TIMEOUT_ms);
        if (!err) {

                i64_t addr = *fpos;

                if (addr < (hdl->total_blocks * hdl->block_size)) {

                        if ((addr + count) >= (hdl->total_blocks * hdl->block_size)) {
                                count = (hdl->total_blocks * hdl->block_size) - addr;
                        }

                        addr += hdl->block_size * hdl->offset_blocks;
                        err = sys_fseek(hdl->dev, addr, VFS_SEEK_SET);
                        if (!err) {
                                err = sys_fwrite(src, count, wrcnt, hdl->dev);
                        }
                }

                sys_mutex_unlock(hdl->mutex);
        }

        return err;
}

//==============================================================================
/**
 * @brief Read data from device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *rdcnt                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_READ(PART,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fattr);

        PART_t *hdl = device_handle;

        int err = sys_mutex_lock(hdl->mutex, MUTEX_TIMEOUT_ms);
        if (!err) {

                i64_t addr = *fpos;

                if (addr < (hdl->total_blocks * hdl->block_size)) {

                        if ((addr + count) >= (hdl->total_blocks * hdl->block_size)) {
                                count = (hdl->total_blocks * hdl->block_size) - addr;
                        }

                        addr += hdl->block_size * hdl->offset_blocks;
                        err = sys_fseek(hdl->dev, addr, VFS_SEEK_SET);
                        if (!err) {
                                err = sys_fread(dst, count, rdcnt, hdl->dev);
                        }
                }

                sys_mutex_unlock(hdl->mutex);
        }

        return err;
}

//==============================================================================
/**
 * @brief IO control.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_IOCTL(PART, void *device_handle, int request, void *arg)
{
        PART_t *hdl = device_handle;

        int err = EINVAL;

        switch (request) {
        case IOCTL_PART__CONFIGURE:
                if (arg) {
                        err = configure(hdl, arg);
                }
                break;

        default:
                err = sys_ioctl(hdl->dev, request, arg);
                break;
        }

        return err;
}

//==============================================================================
/**
 * @brief Flush device.
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_FLUSH(PART, void *device_handle)
{
        PART_t *hdl = device_handle;

        return sys_fflush(hdl->dev);
}

//==============================================================================
/**
 * @brief Device information.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *device_stat            device status
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_STAT(PART, void *device_handle, struct vfs_dev_stat *device_stat)
{
        PART_t *hdl = device_handle;

        device_stat->st_size = hdl->block_size * hdl->total_blocks;

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function set driver configuration.
 *
 * @param  hdl          device handle
 * @param  conf         configuration
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int configure(PART_t *hdl, const PART_config_t *conf)
{
        int err = sys_mutex_lock(hdl->mutex, MUTEX_TIMEOUT_ms);
        if (!err) {

                if (hdl->dev) {
                        err = sys_fclose(hdl->dev);
                }

                if (!err) {
                        err = sys_fopen(conf->path, "r+", &hdl->dev);
                        if (!err) {
                                hdl->block_size    = conf->block_size;
                                hdl->offset_blocks = conf->offset_blocks;
                                hdl->total_blocks  = conf->total_blocks;
                        }
                }

                sys_mutex_unlock(hdl->mutex);
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
