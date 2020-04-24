/*=========================================================================*//**
@file    dht11.c

@author  Daniel Zorychta

@brief   DHT11 sensor driver.

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
#include "gpio_ddi.h"
#include "noarch/dht11_cfg.h"
#include "../dht11_ioctl.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define READ_TIMEOUT_MS         10

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        DHT11_config_t cfg;
        dev_lock_t     lock;
} DHT11_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void set_pin_low(DHT11_t *hdl);
static void set_pin_high(DHT11_t *hdl);
static i8_t get_pin(DHT11_t *hdl);

/*==============================================================================
  Local objects
==============================================================================*/
MODULE_NAME(DHT11);

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
API_MOD_INIT(DHT11, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        UNUSED_ARG2(major, minor);

        int err = sys_zalloc(sizeof(DHT11_t), device_handle);
        if (!err) {
                DHT11_t *hdl      = *device_handle;
                hdl->cfg.pin_idx  = UINT8_MAX;
                hdl->cfg.port_idx = UINT8_MAX;

                if (config) {
                        hdl->cfg = *cast(DHT11_config_t*, config);
                }

                sys_device_unlock(&hdl->lock, true);
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
API_MOD_RELEASE(DHT11, void *device_handle)
{
        DHT11_t *hdl = device_handle;

        int err = sys_device_lock(&hdl->lock);
        if (!err) {
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
API_MOD_OPEN(DHT11, void *device_handle, u32_t flags)
{
        UNUSED_ARG1(flags);

        DHT11_t *hdl = device_handle;

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
API_MOD_CLOSE(DHT11, void *device_handle, bool force)
{
        DHT11_t *hdl = device_handle;

        return sys_device_unlock(&hdl->lock, force);
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
API_MOD_WRITE(DHT11,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{

        UNUSED_ARG6(device_handle, src, count, fpos, wrcnt, fattr);
        return EPERM;
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
API_MOD_READ(DHT11,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fattr);

        DHT11_t *hdl = device_handle;
        int      err = EIO;
        u8_t     buf[5];

        // set start bit and calculate timeout value
        set_pin_low(hdl);
        sys_thread_yield();
        u32_t start   = sys_get_uptime_ms();
        i32_t timeout = 0;
        while (not sys_time_is_expired(start, 2)) {
                timeout++;
        }

        timeout *= 10;

        sys_sleep_ms(18);

        // lock system scheduler and read bit by bit
        sys_context_switch_lock();
        {
                // finish start bit
                set_pin_high(hdl);
                while ((--timeout > 0) && (get_pin(hdl) == 0));

                // wait for start bit from sensor side
                while ((--timeout > 0) && (get_pin(hdl) == 1));
                while ((--timeout > 0) && (get_pin(hdl) == 0));

                // calculate reference bit length
                u32_t refbittime = 0;
                while ((--timeout > 0) && (get_pin(hdl) == 1)) {
                        refbittime++;
                }

                for (u8_t byte = 0; byte < 5; byte++) {
                        buf[byte] = 0;

                        for (u8_t bit = 0; bit < 8; bit++) {
                                buf[byte] <<= 1;

                                while ((--timeout > 0) && (get_pin(hdl) == 0));

                                u32_t bittime = 0;
                                while ((--timeout > 0) && (get_pin(hdl) == 1)) {
                                        bittime++;
                                }

                                if (bittime >= (refbittime / 3)) {
                                        buf[byte] |= 1;
                                }
                        }
                }

                err = timeout < 0 ? EIO : ESUCC;
        }
        sys_context_switch_unlock();

        if (!err) {
                *fpos  = 0;
                *rdcnt = min(5, count);
                memcpy(dst, buf, *rdcnt);
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
API_MOD_IOCTL(DHT11, void *device_handle, int request, void *arg)
{
        DHT11_t *hdl = device_handle;
        int      err = EINVAL;

        if (arg) {
                switch (request) {
                case IOCTL_DHT11__CONFIGURE:
                        hdl->cfg = *cast(DHT11_config_t*, arg);
                        err      = ESUCC;
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
API_MOD_FLUSH(DHT11, void *device_handle)
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
API_MOD_STAT(DHT11, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG1(device_handle);
        device_stat->st_size = 5;
        return ESUCC;
}

//==============================================================================
/**
 * @brief Function set pin low.
 *
 * @param hdl   Module handle
 */
//==============================================================================
static void set_pin_low(DHT11_t *hdl)
{
        _GPIO_DDI_clear_pin(hdl->cfg.port_idx, hdl->cfg.pin_idx);
}

//==============================================================================
/**
 * @brief Function set pin high.
 *
 * @param hdl   Module handle
 */
//==============================================================================
static void set_pin_high(DHT11_t *hdl)
{
        _GPIO_DDI_set_pin(hdl->cfg.port_idx, hdl->cfg.pin_idx);
}

//==============================================================================
/**
 * @brief Function get pin state.
 *
 * @param hdl   Module handle
 */
//==============================================================================
static i8_t get_pin(DHT11_t *hdl)
{
        return _GPIO_DDI_get_pin(hdl->cfg.port_idx, hdl->cfg.pin_idx);
}

/*==============================================================================
  End of file
==============================================================================*/
