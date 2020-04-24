/*==============================================================================
File    spiee.c

Author  Daniel Zorychta

Brief   SPI EEPROM

        Copyright (C) 2019 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "noarch/spiee_cfg.h"
#include "../spiee_ioctl.h"
#include "spi/spi_ioctl.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define MUTEX_TIMEOUT   1000
#define EESR_WIP        (1 << 0)

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        FILE *spi_dev;
        mutex_t *mtx;
        u32_t memory_size;
        u16_t page_size;
        SPIEE_addr_t addr_size;
} SPIEE_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int configure(SPIEE_t *hdl, const SPIEE_config_t *cfg);
static void get_address(SPIEE_t *hdl, u32_t pos, u8_t addr[4]);
static int wait_for_write_finish(SPIEE_t *hdl);

/*==============================================================================
  Local object
==============================================================================*/
MODULE_NAME(SPIEE);

static const u8_t EECMD_READ  = 0x03;
static const u8_t EECMD_WRITE = 0x02;
static const u8_t EECMD_WREN  = 0x06;
static const u8_t EECMD_RDSR  = 0x05;

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
API_MOD_INIT(SPIEE, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        UNUSED_ARG1(major);

        if (minor != 0) {
                return ENODEV;
        }

        int err = sys_zalloc(sizeof(SPIEE_t), device_handle);
        if (!err) {
                SPIEE_t *hdl = *device_handle;

                err = sys_mutex_create(MUTEX_TYPE_RECURSIVE, &hdl->mtx);

                if (!err && config) {
                        err = configure(hdl, config);
                }

                if (err) {
                        sys_free(device_handle);
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
API_MOD_RELEASE(SPIEE, void *device_handle)
{
        SPIEE_t *hdl = device_handle;

        int err = sys_mutex_lock(hdl->mtx, 0);
        if (!err) {
                mutex_t *mtx = hdl->mtx;
                hdl->mtx = 0;
                sys_mutex_unlock(mtx);
                sys_mutex_destroy(mtx);

                if (hdl->spi_dev) {
                        sys_fclose(hdl->spi_dev);
                }

                memset(hdl, 0, sizeof(SPIEE_t));
                sys_free(&device_handle);
        }

        return err;
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
API_MOD_OPEN(SPIEE, void *device_handle, u32_t flags)
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
API_MOD_CLOSE(SPIEE, void *device_handle, bool force)
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
API_MOD_WRITE(SPIEE,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG1(fattr);

        SPIEE_t *hdl = device_handle;

        int err = sys_mutex_lock(hdl->mtx, MUTEX_TIMEOUT);
        if (!err) {

                if (*fpos < hdl->memory_size) {

                        SPI_transceive_t twren;
                        SPI_transceive_t tcmd;
                        SPI_transceive_t taddr;
                        SPI_transceive_t tdata;
                        u8_t addr_buf[4];

                        twren.tx_buffer = &EECMD_WREN;
                        twren.rx_buffer = NULL;
                        twren.count     = 1;
                        twren.separated = true;
                        twren.next      = &tcmd;

                        tcmd.tx_buffer  = &EECMD_WRITE;
                        tcmd.rx_buffer  = NULL;
                        tcmd.count      = 1;
                        tcmd.separated  = false;
                        tcmd.next       = &taddr;

                        taddr.tx_buffer = addr_buf;
                        taddr.rx_buffer = NULL;
                        taddr.count     = hdl->addr_size;
                        taddr.separated = false;
                        taddr.next      = &tdata;

                        u32_t addr = *fpos;
                        count = min(count, *fpos - hdl->memory_size);

                        while (!err && count) {
                                get_address(hdl, addr, addr_buf);

                                size_t pbleft = (((addr / hdl->page_size) + 1) * hdl->page_size) - addr;
                                size_t wrsz   = min(count, pbleft);

                                tdata.tx_buffer = src;
                                tdata.rx_buffer = NULL;
                                tdata.count     = wrsz;
                                tdata.separated = false;
                                tdata.next      = NULL;

                                err = sys_ioctl(hdl->spi_dev, IOCTL_SPI__TRANSCEIVE, &twren);
                                if (!err) {
                                        err = wait_for_write_finish(hdl);
                                        if (!err) {
                                                addr   += wrsz;
                                                src    += wrsz;
                                                count  -= wrsz;
                                                *wrcnt += wrsz;
                                        }
                                }
                        }
                } else {
                        printk("SPIEE: write out of range %02Xh", (u32_t)*fpos);
                        *wrcnt = 0;
                        err = ESPIPE;
                }

                sys_mutex_unlock(hdl->mtx);
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
API_MOD_READ(SPIEE,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fattr);

        SPIEE_t *hdl = device_handle;

        int err = sys_mutex_lock(hdl->mtx, MUTEX_TIMEOUT);
        if (!err) {

                if (*fpos < hdl->memory_size) {

                        u8_t addr_buf[4];
                        get_address(hdl, *fpos, addr_buf);

                        SPI_transceive_t tcmd;
                        SPI_transceive_t taddr;
                        SPI_transceive_t tdata;

                        tcmd.tx_buffer  = &EECMD_READ;
                        tcmd.rx_buffer  = NULL;
                        tcmd.count      = 1;
                        tcmd.separated  = false;
                        tcmd.next       = &taddr;

                        taddr.tx_buffer = addr_buf;
                        taddr.rx_buffer = NULL;
                        taddr.count     = hdl->addr_size;
                        taddr.separated = false;
                        taddr.next      = &tdata;

                        tdata.tx_buffer = NULL;
                        tdata.rx_buffer = dst;
                        tdata.count     = min(count, *fpos - hdl->memory_size);
                        tdata.separated = false;
                        tdata.next      = NULL;

                        err = sys_ioctl(hdl->spi_dev, IOCTL_SPI__TRANSCEIVE, &tcmd);
                        if (!err) {
                                *rdcnt = tdata.count;
                        }

                } else {
                        printk("SPIEE: read out of range %02Xh", (u32_t)*fpos);
                        *rdcnt = 0;
                        err = ESPIPE;
                }

                sys_mutex_unlock(hdl->mtx);
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
API_MOD_IOCTL(SPIEE, void *device_handle, int request, void *arg)
{
        SPIEE_t *hdl = device_handle;
        int      err = EINVAL;

        if (arg) {
                char pathbuf[32];
                SPIEE_config_t cfg;

                switch (request) {
                case IOCTL_SPIEE__CONFIGURE_STR: {

                        size_t pathlen = sys_stropt_get_string_copy(arg, "spi_path",
                                                                    pathbuf, sizeof(pathbuf));
                        cfg.spi_path = pathbuf;
                        cfg.memory_size = sys_stropt_get_int(arg, "memory_size", 0);
                        cfg.page_size = sys_stropt_get_int(arg, "page_size", 0);
                        cfg.address_size = sys_stropt_get_int(arg, "address_size", 0);

                        if (  pathlen == 0
                           || cfg.page_size == 0
                           || cfg.memory_size == 0
                           || cfg.address_size == 0) {
                                err = EINVAL;
                        } else {
                                err = configure(hdl, &cfg);
                        }
                        break;
                }

                case IOCTL_SPIEE__CONFIGURE: {
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
 * @brief Flush device.
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_FLUSH(SPIEE, void *device_handle)
{
        SPIEE_t *hdl = device_handle;

        return sys_fflush(hdl->spi_dev);
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
API_MOD_STAT(SPIEE, void *device_handle, struct vfs_dev_stat *device_stat)
{
        SPIEE_t *hdl = device_handle;

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
static int configure(SPIEE_t *hdl, const SPIEE_config_t *cfg)
{
        int err = sys_mutex_lock(hdl->mtx, MUTEX_TIMEOUT);
        if (!err) {
                if (  (cfg->address_size < SPIEE_ADDR__1_BYTE)
                   || (cfg->address_size > SPIEE_ADDR__4_BYTES) ) {

                        printk("SPIEE: incorrect address size!");
                        err = EINVAL;
                }

                if (!err && hdl->spi_dev) {
                        err = sys_fclose(hdl->spi_dev);
                }

                if (!err) {
                        err = sys_fopen(cfg->spi_path, "r+", &hdl->spi_dev);
                        if (!err) {
                                hdl->memory_size = cfg->memory_size;
                                hdl->page_size   = cfg->page_size;
                                hdl->addr_size   = cfg->address_size;
                        }
                }

                sys_mutex_unlock(hdl->mtx);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function set address value.
 *
 * @param  hdl          driver handle
 * @param  pos          index position (address)
 */
//==============================================================================
static void get_address(SPIEE_t *hdl, u32_t pos, u8_t addr[4])
{
        switch (hdl->addr_size){
        case SPIEE_ADDR__1_BYTE:
                addr[0] = (pos & 0x000000FF);
                break;
        case SPIEE_ADDR__2_BYTES:
                addr[0] = (pos & 0x0000FF00) >> 8;
                addr[1] = (pos & 0x000000FF);
                break;
        case SPIEE_ADDR__3_BYTES:
                addr[0] = (pos & 0x00FF0000) >> 16;
                addr[1] = (pos & 0x0000FF00) >> 8;
                addr[2] = (pos & 0x000000FF);
                break;
        case SPIEE_ADDR__4_BYTES:
                addr[0] = (pos & 0xFF000000) >> 24;
                addr[1] = (pos & 0x00FF0000) >> 16;
                addr[2] = (pos & 0x0000FF00) >> 8;
                addr[3] = (pos & 0x000000FF);
                break;
        default:
                addr[0] = 0xFF;
                addr[1] = 0xFF;
                addr[2] = 0xFF;
                addr[3] = 0xFF;
                break;
        }
}

//==============================================================================
/**
 * @brief  Function waiting for write finish.
 *
 * @param  hdl          driver handle
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int wait_for_write_finish(SPIEE_t *hdl)
{
        u8_t txbuf[2] = {EECMD_RDSR, 0x00};
        u8_t rxbuf[2] = {0, 0};

        SPI_transceive_t t;
        t.tx_buffer = txbuf;
        t.rx_buffer = rxbuf;
        t.count     = sizeof(rxbuf);
        t.separated = false;
        t.next      = NULL;

        u32_t tref = sys_time_get_reference();

        while (not sys_time_is_expired(tref, 1000)) {

                int err = sys_ioctl(hdl->spi_dev, IOCTL_SPI__TRANSCEIVE, &t);
                if (!err) {
                        if (not (rxbuf[1] & EESR_WIP)) {
                                return ESUCC;
                        } else {
                                sys_sleep_ms(2);
                        }

                } else {
                        printk("SPIEE: RDSR error %d", err);
                        return err;
                }
        }

        printk("SPIEE: write operation timeout");

        return ETIME;
}

/*==============================================================================
  End of file
==============================================================================*/
