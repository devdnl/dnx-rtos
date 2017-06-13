/*==============================================================================
File    sdio.c

Author  Daniel Zorychta

Brief   SD Card Interface Driver.

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
#include "drivers/class/storage/mbr.h"
#include "drivers/class/storage/sd.h"
#include "stm32f4/sdio_cfg.h"
#include "sys/ioctl.h"
#include "../sdio_ioctl.h"

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/
/* minor numbers */
enum {
        VOLUME,
        PARTITION_1,
        PARTITION_2,
        PARTITION_3,
        PARTITION_4,
        TOTAL_VOLUMES
};

typedef struct {
        u32_t      offset;              /* partition offset sector    */
        u32_t      size;                /* partition size in sectors  */
        bool       used;                /* true if part used          */
} part_t;

/** main control structure */
typedef struct {
        mutex_t   *protect_mtx;
        SD_type_t  type;
        bool       initialized;
        u8_t       part_init;
        part_t     part[TOTAL_VOLUMES];
} SDIO_ctrl_t;

/** driver instance associated with partition */
typedef struct {
        SDIO_ctrl_t  *stg;              /* module storage. */
        u8_t          minor;            /* minor number. */
} SDIO_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int card_read(SDIO_t *hdl, u8_t *dst, size_t count, u64_t lseek, size_t *rdcnt);
static int card_write(SDIO_t *hdl, const u8_t *src, size_t count, u64_t lseek, size_t *wrcnt);

/*==============================================================================
  Local object
==============================================================================*/
MODULE_NAME(SDIO);

static const u16_t SECTOR_SIZE = 512;

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
API_MOD_INIT(SDIO, void **device_handle, u8_t major, u8_t minor)
{
        int err = ENODEV;

        if (minor >= TOTAL_VOLUMES) {
                return err;
        }

        SDIO_t *hdl = NULL;

        if (minor == 0) {
                err = sys_zalloc(sizeof(SDIO_t), cast(void**, &hdl));
                if (err) goto finish;

                err = sys_zalloc(sizeof(SDIO_ctrl_t), cast(void**, &hdl->stg));
                if (err) goto finish;

                err = sys_mutex_create(MUTEX_TYPE_NORMAL, &hdl->stg->protect_mtx);
                if (err) goto finish;

                // TODO SDIO init

                finish:
                if (err) {
                        if (hdl) {
                                if (hdl->stg) {
                                        if (hdl->stg->protect_mtx) {
                                                sys_mutex_destroy(hdl->stg->protect_mtx);
                                        }

                                        sys_free(cast(void**, &hdl->stg));
                                }

                                sys_free(cast(void**, &hdl));
                        }
                }

        } else {
                err = sys_zalloc(sizeof(SDIO_t), cast(void**, &hdl));
                if (!err) {
                        SDIO_t *hdl0 = NULL;
                        err = sys_module_get_instance(major, 0, cast(void**, &hdl0));
                        if (err) {
                                sys_free(cast(void**, &hdl));
                        } else {
                                hdl->minor = minor;
                                hdl->stg   = hdl0->stg;
                                hdl->stg->part_init++;
                        }
                }
        }

        *device_handle = hdl;

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
API_MOD_RELEASE(SDIO, void *device_handle)
{
        SDIO_t *hdl = device_handle;

        int err = EBUSY;

        if (!hdl->stg->part[hdl->minor].used) {
                if (hdl->minor == 0) {
                        if (hdl->stg->part_init == 0) {
                                // TODO SDIO deinit

                                sys_mutex_destroy(hdl->stg->protect_mtx);
                                sys_free(cast(void**, &hdl->stg));
                                sys_free(cast(void**, &hdl));
                                err = ESUCC;
                        }

                } else {
                        err = sys_free(cast(void**, &hdl));
                        hdl->stg->part_init--;
                }
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
API_MOD_OPEN(SDIO, void *device_handle, u32_t flags)
{
        UNUSED_ARG1(flags);

        SDIO_t *hdl = device_handle;

        return hdl->stg->part[hdl->minor].used ? EBUSY : ESUCC;
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
API_MOD_CLOSE(SDIO, void *device_handle, bool force)
{
        UNUSED_ARG1(force);

        SDIO_t *hdl = device_handle;

        hdl->stg->part[hdl->minor].used = false;

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
API_MOD_WRITE(SDIO,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG1(fattr);

        SDIO_t *hdl  = device_handle;
        part_t *part = &hdl->stg->part[hdl->minor];
        int      err = ENOMEDIUM;

        if (part->size > 0) {
                err = sys_mutex_lock(hdl->stg->protect_mtx, MAX_DELAY_MS);
                if (!err) {
                        u64_t lseek = *fpos + (cast(u64_t, part->offset) * SECTOR_SIZE);
                        err = card_write(hdl, src, count, lseek, wrcnt);
                        sys_mutex_unlock(hdl->stg->protect_mtx);
                }
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
API_MOD_READ(SDIO,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fattr);

        SDIO_t *hdl  = device_handle;
        part_t *part = &hdl->stg->part[hdl->minor];
        int      err = ENOMEDIUM;

        if (part->size > 0) {
                err = sys_mutex_lock(hdl->stg->protect_mtx, MAX_DELAY_MS);
                if (!err) {
                        u64_t lseek = *fpos + (cast(u64_t, part->offset) * SECTOR_SIZE);
                        err = card_read(hdl, dst, count, lseek, rdcnt);
                        sys_mutex_unlock(hdl->stg->protect_mtx);
                }
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
API_MOD_IOCTL(SDIO, void *device_handle, int request, void *arg)
{
        SDIO_t *hdl = device_handle;

        int err = EBADRQC;

        switch (request) {
        case IOCTL_SDIO__INITIALIZE_CARD: {
                err = sys_mutex_lock(hdl->stg->protect_mtx, MAX_DELAY_MS);
                if (!err) {
                        err = card_initialize(hdl);
                        sys_mutex_unlock(hdl->stg->protect_mtx);
                }
                break;
        }

        case IOCTL_SDIO__READ_MBR: {
                err = sys_mutex_lock(hdl->stg->protect_mtx, MAX_DELAY_MS);
                if (!err) {
                        if (hdl->stg->initialized) {
                                err = MBR_detect_partitions(hdl);
                        } else {
                                err = ENOMEDIUM;
                        }
                        sys_mutex_unlock(hdl->stg->protect_mtx);
                }
                break;
        }

        default:
                return EBADRQC;
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
API_MOD_FLUSH(SDIO, void *device_handle)
{
        UNUSED_ARG1(device_handle);

        return ESUCC;
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
API_MOD_STAT(SDIO, void *device_handle, struct vfs_dev_stat *device_stat)
{
        SDIO_t *hdl = device_handle;

        if (hdl->stg->initialized) {
                device_stat->st_size = cast(u64_t, hdl->stg->part[hdl->minor].size)
                                                 * SECTOR_SIZE;
        } else {
                device_stat->st_size = 0;
        }

        return ESUCC;
}

//==============================================================================
/**
 * @brief Read data from card
 *
 * @param[in]   hdl             driver's memory handle
 * @param[out]  dst             destination
 * @param[in]   count           bytes to read
 * @param[in]   lseek           file index
 * @param[out]  rdcnt           number of read bytes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int card_read(SDIO_t *hdl, u8_t *dst, size_t count, u64_t lseek, size_t *rdcnt)
{
        ssize_t n = 0;

        if (hdl->stg->initialized == false) {
                return EIO;

        } else {
                if ((count % SECTOR_SIZE == 0) && (lseek % SECTOR_SIZE == 0)) {

                        n  = card_read_entire_sectors(hdl, dst, count / SECTOR_SIZE, lseek);
                        n *= SECTOR_SIZE;

                } else {
                        n  = card_read_partial_sectors(hdl, dst, count, lseek);
                }

                SPI_deselect_card(hdl);
        }

        if (n >= 0) {
                *rdcnt = n;
                return ESUCC;
        } else {
                return EIO;
        }
}

//==============================================================================
/**
 * @brief Write data to card
 *
 * @param[in]  hdl              driver's memory handle
 * @param[in]  src              source
 * @param[in]  count            bytes to write
 * @param[in]  lseek            file index
 * @param[out] wrcnt            number of written bytes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int card_write(SDIO_t *hdl, const u8_t *src, size_t count, u64_t lseek, size_t *wrcnt)
{
        ssize_t n = 0;

        if (hdl->stg->initialized == false) {
                return EIO;

        } else {
                if ((count % SECTOR_SIZE == 0) && (lseek % SECTOR_SIZE == 0)) {
                        n  = card_write_entire_sectors(hdl, src, count / SECTOR_SIZE, lseek);
                        n *= SECTOR_SIZE;
                } else {
                        n  = card_write_partial_sectors(hdl, src, count, lseek);
                }

                SPI_deselect_card(hdl);
        }

        if (n >= 0) {
                *wrcnt = n;
                return ESUCC;
        } else {
                return EIO;
        }
}

//==============================================================================
/**
 * @brief SDIO IRQ handler.
 */
//==============================================================================
void SDIO_IRQHandler(void)
{

}

/*==============================================================================
  End of file
==============================================================================*/
