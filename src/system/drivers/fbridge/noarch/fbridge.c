/*==============================================================================
File    fbridge.c

Author  Daniel Zorychta

Brief   File Bridge

        Copyright (C) 2022 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "../fbridge_ioctl.h"

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        u8_t major;
        u8_t minor;
        kfile_t *read_file;
        kfile_t *write_file;
} FBRIDGE_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int configure(FBRIDGE_t *hdl, const FBRIDGE_conf_t *conf);

/*==============================================================================
  Local object
==============================================================================*/
MODULE_NAME(FBRIDGE);

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
 * @param[in ]            config               optional module configuration
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_INIT(FBRIDGE, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        int err = EFAULT;

        err = sys_zalloc(sizeof(FBRIDGE_t), device_handle);
        if (!err) {
                FBRIDGE_t *hdl = *device_handle;

                hdl->major = major;
                hdl->minor = minor;

                if (config) {
                        err = configure(hdl, config);
                }

                if (err) {
                        sys_free(*device_handle);
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
API_MOD_RELEASE(FBRIDGE, void *device_handle)
{
        FBRIDGE_t *hdl = device_handle;

        if (hdl->read_file) {
                sys_fclose(hdl->read_file);
        }

        if (hdl->write_file) {
                sys_fclose(hdl->write_file);
        }

        return sys_free(&device_handle);
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
API_MOD_OPEN(FBRIDGE, void *device_handle, u32_t flags)
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
API_MOD_CLOSE(FBRIDGE, void *device_handle, bool force)
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
 * @param[out]          *wrctr                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_WRITE(FBRIDGE,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrctr,
              struct vfs_fattr  fattr)
{
        FBRIDGE_t *hdl = device_handle;

        sys_ioctl(hdl->write_file, fattr.non_blocking_wr
                  ? IOCTL_VFS__NON_BLOCKING_WR_MODE : IOCTL_VFS__DEFAULT_WR_MODE);

        sys_fseek(hdl->write_file, *fpos, SEEK_SET);

        return sys_fwrite(src, count, wrctr, hdl->write_file);
}

//==============================================================================
/**
 * @brief Read data from device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *rdctr                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_READ(FBRIDGE,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdctr,
             struct vfs_fattr fattr)
{
        FBRIDGE_t *hdl = device_handle;

        sys_ioctl(hdl->read_file, fattr.non_blocking_rd
                  ? IOCTL_VFS__NON_BLOCKING_RD_MODE : IOCTL_VFS__DEFAULT_RD_MODE);

        sys_fseek(hdl->read_file, *fpos, SEEK_SET);

        return sys_fread(dst, count, rdctr, hdl->read_file);
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
API_MOD_IOCTL(FBRIDGE, void *device_handle, int request, void *arg)
{
        FBRIDGE_t *hdl = device_handle;

        int err = EBADRQC;

        switch (request) {
        case IOCTL_FBRIDGE__CONFIGURE:
                err = configure(hdl, arg);
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
API_MOD_FLUSH(FBRIDGE, void *device_handle)
{
        FBRIDGE_t *hdl = device_handle;

        return sys_fflush(hdl->write_file);
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
API_MOD_STAT(FBRIDGE, void *device_handle, struct vfs_dev_stat *device_stat)
{
        FBRIDGE_t *hdl = device_handle;

        struct stat stat_buf;
        int err = sys_fstat(hdl->read_file, &stat_buf);
        device_stat->st_size = stat_buf.st_size;

        return err;
}

//==============================================================================
/**
 * @brief  Configure driver.
 *
 * @param  hdl          device handle
 * @param  conf         configuration
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int configure(FBRIDGE_t *hdl, const FBRIDGE_conf_t *conf)
{
        kfile_t *new_read_file;
        kfile_t *new_write_file;

        int err = sys_fopen(conf->read_path, O_RDONLY, 0, &new_read_file);
        if (not err) {
                err = sys_fopen(conf->write_path, O_WRONLY, 0, &new_write_file);
                if (not err) {
                        if (hdl->read_file)  sys_fclose(hdl->read_file);
                        if (hdl->write_file) sys_fclose(hdl->write_file);

                        hdl->read_file  = new_read_file;
                        hdl->write_file = new_write_file;
                }
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
