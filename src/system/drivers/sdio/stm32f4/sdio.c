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
#define CARD_INIT_TIMEOUT       500

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

typedef enum {
        CMD_RESP_NONE   = 0x00,
        CMD_RESP_SHORT  = 0x40,
        CMD_RESP_R3     = 0xF40,
        CMD_RESP_LONG   = 0xC0,
} cmd_resp_t;

typedef enum {
        RESP_R1         = (0x01),
        RESP_R1b        = (0x02),
        RESP_R2         = (0x03),
        RESP_R3         = (0x04),
        RESP_R6         = (0x05),
        RESP_R7         = (0x06),
} resp_t;


typedef struct {
        u32_t      offset;              /* partition offset sector    */
        u32_t      size;                /* partition size in sectors  */
        bool       used;                /* true if part used          */
} part_t;

/** main control structure */
typedef struct {
        mutex_t   *protect_mtx;
        SD_type_t  type;
        u16_t      RCA;
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
static int card_initialize(SDIO_t *hdl);
static int card_send_cmd(uint32_t cmd, cmd_resp_t resp, uint32_t arg);
static int card_get_response(SD_response_t *resp, resp_t type);

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

                SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SDIOEN);
                SDIO->CLKCR = _SDIO_CFG_NEGEDGE
                            | _SDIO_CFG_BUS_WIDE
                            | _SDIO_CFG_PWRSAVE
                            | SDIO_CLKCR_CLKEN
                            | 158 /* CLKDIV */;

                SDIO->POWER = (1 * SDIO_POWER_PWRCTRL_1) | (1 * SDIO_POWER_PWRCTRL_0);

                SDIO->DTIMER = 0xFFFF;

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
//                                err = MBR_detect_partitions(hdl); TODO
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

//                        n  = card_read_entire_sectors(hdl, dst, count / SECTOR_SIZE, lseek); TODO
                        n *= SECTOR_SIZE;

                } else {
//                        n  = card_read_partial_sectors(hdl, dst, count, lseek); TODO
                }

//                SPI_deselect_card(hdl);
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
//                        n  = card_write_entire_sectors(hdl, src, count / SECTOR_SIZE, lseek); // TODO
                        n *= SECTOR_SIZE;
                } else {
//                        n  = card_write_partial_sectors(hdl, src, count, lseek); TODO
                }

//                SPI_deselect_card(hdl); TODO
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
 * @brief Function initialize card.
 *
 * @param[in]  hdl      SD module data
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int card_initialize(SDIO_t *hdl)
{
        int err = EIO;
        SD_response_t resp;

        hdl->stg->type.type   = SD_TYPE__UNKNOWN;
        hdl->stg->type.block  = false;
        hdl->stg->initialized = false;

        u32_t timer = sys_time_get_reference();

        catcherr(err = card_send_cmd(SD_CMD__CMD0, CMD_RESP_NONE, 0), finish);

        catcherr(err = card_send_cmd(SD_CMD__CMD8, CMD_RESP_SHORT, 0x1AA), finish);
        catcherr(err = card_get_response(&resp, RESP_R7), finish);

        if (resp.RESPONSE[0] == 0x1AA) {
                while (!sys_time_is_expired(timer, CARD_INIT_TIMEOUT)) {

                        catcherr(err = card_send_cmd(SD_CMD__CMD55, CMD_RESP_SHORT, 0), finish);
                        catcherr(err = card_get_response(&resp, RESP_R1), finish);

                        catcherr(err = card_send_cmd(SD_CMD__ACMD41, CMD_RESP_R3, 0x80100000 | 0x40000000), finish);
                        catcherr(err = card_get_response(&resp, RESP_R3), finish);

                        if ((resp.RESPONSE[0] >> 31) == 1) {

                                if (resp.RESPONSE[0] & 0x40000000) {
                                        hdl->stg->type.type  = SD_TYPE__SD2;
                                        hdl->stg->type.block = true;
                                } else {
                                        hdl->stg->type.type  = SD_TYPE__SD1;
                                        hdl->stg->type.block = false;
                                }

                                printk("SDIO: found SD%s card",
                                       hdl->stg->type.block ? "HC" : "SC");

                                err = ESUCC;
                                break;
                        } else {
                                err = ETIME;
                        }
                }

                catcherr(err = card_send_cmd(SD_CMD__CMD2, CMD_RESP_LONG, 0), finish);

                catcherr(err = card_send_cmd(SD_CMD__CMD3, CMD_RESP_SHORT, 0), finish);
                catcherr(err = card_get_response(&resp, RESP_R6), finish);

                hdl->stg->RCA = resp.RESPONSE[0] >> 16;

                catcherr(err = card_send_cmd(SD_CMD__CMD9, CMD_RESP_LONG, hdl->stg->RCA << 16), finish);
                catcherr(err = card_get_response(&resp, RESP_R2), finish);

                u32_t blks = 0;
                u8_t CSD_ver = SD_CSD_get_version(&resp);
                if (CSD_ver == 1) {
                        blks = (SD_CSD_get_C_SIZE_HC(&resp) + 1) * 1024;

                } else if (CSD_ver == 0) {
                        blks = (SD_CSD_get_C_SIZE_SC(&resp) + 1)
                             * (1 << (SD_CSD_get_C_SIZE_MULT(&resp) + 2))
                             * (1 << SD_CSD_get_READ_BL_LEN(&resp))
                             / 512;
                } else {
                        printk("SDIO: unknown CSD version");
                        hdl->stg->type.type = SD_TYPE__UNKNOWN;
                        err = EIO;
                        goto finish;
                }

                printk("SDIO: %d 512-byte logical blocks", blks);
                hdl->stg->part[VOLUME].size = blks;

                catcherr(err = card_send_cmd(SD_CMD__CMD7, CMD_RESP_SHORT, hdl->stg->RCA << 16), finish);
                catcherr(err = card_get_response(&resp, RESP_R1), finish);

                if (hdl->stg->type.type == SD_TYPE__SD1) {
                        catcherr(err = card_send_cmd(SD_CMD__CMD16, CMD_RESP_SHORT, SECTOR_SIZE), finish);
                        catcherr(err = card_get_response(&resp, RESP_R1), finish);
                }

                catcherr(err = card_send_cmd(SD_CMD__CMD55, CMD_RESP_SHORT, hdl->stg->RCA << 16), finish);
                catcherr(err = card_get_response(&resp, RESP_R1), finish);

                catcherr(err = card_send_cmd(SD_CMD__ACMD6, CMD_RESP_SHORT, _SDIO_CFG_ACMD6_BUS_WIDE), finish);
                catcherr(err = card_get_response(&resp, RESP_R1), finish);

                SDIO->CLKCR = _SDIO_CFG_NEGEDGE
                            | _SDIO_CFG_BUS_WIDE
                            | _SDIO_CFG_PWRSAVE
                            | SDIO_CLKCR_CLKEN
                            | _SDIO_CFG_CLKDIV;

                hdl->stg->initialized = true;
        }

        finish:
        return err;
}

//==============================================================================
/**
 * @brief
 *
 * @param  ?
 *
 * @return ?
 */
//==============================================================================
static int card_send_cmd(uint32_t cmd, cmd_resp_t resp, uint32_t arg)
{
        // clear the Command Flags
        SDIO->ICR = SDIO_STA_CCRCFAIL
                  | SDIO_STA_CTIMEOUT
                  | SDIO_STA_CMDREND
                  | SDIO_STA_CMDSENT;

        SDIO->ARG = arg;

        SDIO->CMD = (cmd  & SDIO_CMD_CMDINDEX)
                  | (resp & SDIO_CMD_WAITRESP)
                  | SDIO_CMD_CPSMEN;

        if (resp == CMD_RESP_NONE) {
                while (!(SDIO->STA & (SDIO_STA_CTIMEOUT | SDIO_STA_CMDSENT)));

        } else {
                while (!(SDIO->STA & (SDIO_STA_CTIMEOUT | SDIO_STA_CMDREND | SDIO_STA_CCRCFAIL)));
        }

        //Check to see if the response is valid
        //We consider all R3 responses without a timeout as a valid response
        //It seems CMDSENT and CMDREND are mutually exclusive.
        if (SDIO->STA & SDIO_STA_CTIMEOUT) {
                printk("SDIO: Command Timeout Error");
                return ETIME;

        } else if ((SDIO->STA & SDIO_STA_CCRCFAIL) && (resp != CMD_RESP_R3)) {
                printk("SDIO: Command CRC Error");
                return EIO;
        }

        return ESUCC;
}

//==============================================================================
/**
 * @brief
 *
 * @param  ?
 *
 * @return ?
 */
//==============================================================================
static int card_get_response(SD_response_t *resp, resp_t type)
{
        if ((type == RESP_R1) || (type == RESP_R1b)) {
                resp->RESPONSE[0] = SDIO->RESP1;
                if (SDIO->RESP1 & (uint32_t) 0xFDFFE008) {
                        printk("SDIO: Response Error");
                        return EIO;
                }

        } else if (type == RESP_R2) {
                resp->RESPONSE[0] = SDIO->RESP1;
                resp->RESPONSE[1] = SDIO->RESP2;
                resp->RESPONSE[2] = SDIO->RESP3;
                resp->RESPONSE[3] = SDIO->RESP4;

        } else if (type == RESP_R3) {
                if (SDIO->RESPCMD != 0x3F) {
                        printk("SDIO: Unexpected command index");
                        return EIO;
                }

                resp->RESPONSE[0] = SDIO->RESP1;

        } else if (type == RESP_R6) {
                if (SDIO->RESPCMD != 0x03) {
                        printk("SDIO: Unexpected command index");
                        return EIO;
                }

                resp->RESPONSE[0] = SDIO->RESP1;

        } else {
                if (SDIO->RESPCMD != 0x08) {
                        printk("SDIO: Unexpected command index");
                }

                resp->RESPONSE[0] = SDIO->RESP1;

                if ((resp->RESPONSE[0] & 0xFF) != 0xAA) {
                        printk("SDIO: Pattern did not match");
                        return EIO;
                }
        }

        return ESUCC;
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
