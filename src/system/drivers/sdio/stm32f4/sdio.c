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
#include "drivers/class/storage/sd.h"
#include "stm32f4/sdio_cfg.h"
#include "stm32f4/dma_ddi.h"
#include "sys/ioctl.h"
#include "../sdio_ioctl.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define USE_DMA_NEVER                   _SDIO_USE_DMA_NEVER
#define USE_DMA_IFAVAILABLE             _SDIO_USE_DMA_IFAVAILABLE
#define USE_DMA_ALWAYS                  _SDIO_USE_DMA_ALWAYS
#define USE_DMA                         _SDIO_CFG_USEDMA
#define CARD_DT_CK_TIMEOUT              0xFFFFFF
#define TRANSACTION_TIMEOUT             (2 * (_SDIO_CFG_CARD_TIMEOUT))
#define COMMAND_TIMEOUT                 ((_SDIO_CFG_CARD_TIMEOUT) / 1)

#define DMA_MAJOR                       _DMA_DDI_DMA2
#define DMA_CHANNEL                     4
#define DMA_STREAM_PRI                  3
#define DMA_STREAM_ALT                  6

#define SDIO_DCTRL_DBLOCKSIZE_512       (9 << SDIO_DCTRL_DBLOCKSIZE_Pos)
#define SDIO_DCTRL_DTMODE_BLOCK         (0 * SDIO_DCTRL_DTMODE)
#define SDIO_DCTRL_DTDIR_TO_CARD        (0 * SDIO_DCTRL_DTDIR)
#define SDIO_DCTRL_DTDIR_FROM_CARD      (1 * SDIO_DCTRL_DTDIR)

#define SDIO_CLKCR_INIT_CFG             (_SDIO_CFG_NEGEDGE\
                                        |_SDIO_CFG_BUS_WIDE\
                                        |_SDIO_CFG_PWRSAVE\
                                        |_SDIO_CFG_INIT_CLKDIV\
                                        | 0*SDIO_CLKCR_HWFC_EN\
                                        | SDIO_CLKCR_CLKEN)

#define SDIO_CLKCR_RUN_CFG              (_SDIO_CFG_NEGEDGE\
                                        |_SDIO_CFG_BUS_WIDE\
                                        |_SDIO_CFG_PWRSAVE\
                                        |_SDIO_CFG_CLKDIV\
                                        | 0*SDIO_CLKCR_HWFC_EN\
                                        | SDIO_CLKCR_CLKEN)

/*
 * NOTE: Some CPUs do not have this bit in peripheral's status register.
 */
#ifndef SDIO_STA_STBITERR
#define SDIO_STA_STBITERR 0
#endif

/*==============================================================================
  Local object types
==============================================================================*/
typedef enum {
        DIR_IN,
        DIR_OUT
} dir_t;

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


/** driver instance associated with partition */
typedef struct {
        u8_t       major;
        u8_t       minor;
        mutex_t   *protect;
        queue_t   *event;
        SD_type_t  card;
        u32_t      RCA;
        bool       initialized;
        u32_t      size_blocks;
        u32_t     *buf;
        size_t     count;
        u32_t      dmad;
} SDIO_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int card_read(SDIO_t *hdl, u8_t *dst, size_t count, u64_t lseek, size_t *rdcnt);
static int card_write(SDIO_t *hdl, const u8_t *src, size_t count, u64_t lseek, size_t *wrcnt);
static int card_initialize(SDIO_t *hdl);
static int card_send_cmd(uint32_t cmd, cmd_resp_t resp, uint32_t arg);
static int card_get_response(SD_response_t *resp, resp_t type);
static int card_read_sectors(SDIO_t *hdl, u8_t *dst, size_t count, u32_t address, size_t *rdsec);
static int card_write_sectors(SDIO_t *hdl, const u8_t *src, size_t count, u32_t address, size_t *wrsec);
static int card_transfer_block(SDIO_t *hdl, uint32_t cmd, uint32_t address, u8_t *buf, size_t count, dir_t dir);

#if USE_DMA != USE_DMA_NEVER
static bool DMA_finished(DMA_Stream_TypeDef *stream, u8_t SR, void *arg);
#endif

/*==============================================================================
  Local object
==============================================================================*/
MODULE_NAME(SDIO);

static SDIO_t *SDIO_CTRL;
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
 * @param[in ]            config               optional module configuration
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_INIT(SDIO, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        UNUSED_ARG1(config);

        int err = ENODEV;

        SDIO_t *hdl = NULL;

        catcherr(err = sys_zalloc(sizeof(SDIO_t), cast(void**, &hdl)), finish);
        catcherr(err = sys_mutex_create(MUTEX_TYPE_NORMAL, &hdl->protect), finish);
        catcherr(err = sys_queue_create(1, sizeof(int), &hdl->event), finish);

        hdl->major = major;
        hdl->minor = minor;

#if USE_DMA == USE_DMA_ALWAYS
        hdl->dmad = _DMA_DDI_reserve(DMA_MAJOR, DMA_STREAM_PRI);

        if (!hdl->dmad) {
                hdl->dmad = _DMA_DDI_reserve(DMA_MAJOR, DMA_STREAM_ALT);
        }

        if (!hdl->dmad) {
                printk("SDIO: could not reserve DMA stream");
                err = EIO;
                goto finish;
        }
#endif

        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SDIOEN);

        SDIO->CLKCR = SDIO_CLKCR_INIT_CFG;

        SDIO->POWER = (1 * SDIO_POWER_PWRCTRL_1) | (1 * SDIO_POWER_PWRCTRL_0);

        SDIO_CTRL = hdl;

        finish:
        if (err && hdl) {
                if (hdl->protect) {
                        sys_mutex_destroy(hdl->protect);
                }

                if (hdl->event) {
                        sys_queue_destroy(hdl->event);
                }

                if (hdl->dmad) {
                        _DMA_DDI_release(hdl->dmad);
                }

                sys_free(cast(void**, &hdl));
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

        int err = sys_mutex_lock(hdl->protect, _SDIO_CFG_CARD_TIMEOUT);
        if (!err) {
                SDIO->POWER = 0;
                SET_BIT(RCC->APB2RSTR, RCC_APB2RSTR_SDIORST);
                CLEAR_BIT(RCC->APB2RSTR, RCC_APB2RSTR_SDIORST);
                CLEAR_BIT(RCC->APB2ENR, RCC_APB2ENR_SDIOEN);

                sys_mutex_unlock(hdl->protect);
                sys_mutex_destroy(hdl->protect);
                sys_queue_destroy(hdl->event);
                _DMA_DDI_release(hdl->dmad);
                sys_free(cast(void**, &hdl));

                sys_mutex_unlock(hdl->protect);
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
API_MOD_CLOSE(SDIO, void *device_handle, bool force)
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
API_MOD_WRITE(SDIO,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG1(fattr);

        SDIO_t *hdl = device_handle;

        int err = sys_mutex_lock(hdl->protect, MAX_DELAY_MS);
        if (!err) {
                if (hdl->size_blocks > 0) {
                        u64_t lseek = *fpos;
                        err = card_write(hdl, src, count, lseek, wrcnt);
                } else {
                        err = ENOMEDIUM;
                }

                sys_mutex_unlock(hdl->protect);
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

        SDIO_t *hdl = device_handle;

        int err = sys_mutex_lock(hdl->protect, MAX_DELAY_MS);
        if (!err) {
                if (hdl->size_blocks > 0) {
                        u64_t lseek = *fpos;
                        err = card_read(hdl, dst, count, lseek, rdcnt);
                } else {
                        err = ENOMEDIUM;
                }

                sys_mutex_unlock(hdl->protect);
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
        UNUSED_ARG1(arg);

        SDIO_t *hdl = device_handle;

        int err = EBADRQC;

        switch (request) {
        case IOCTL_SDIO__INITIALIZE_CARD:
                err = sys_mutex_lock(hdl->protect, MAX_DELAY_MS);
                if (!err) {
                        err = card_initialize(hdl);
                        sys_mutex_unlock(hdl->protect);
                }
                break;

        default:
                err = EBADRQC;
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

        if (hdl->initialized) {
                device_stat->st_size = cast(u64_t, hdl->size_blocks) * SECTOR_SIZE;
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
        int err = EIO;

        if (hdl->initialized) {
                if ((count % SECTOR_SIZE == 0) && (lseek % SECTOR_SIZE == 0)) {

                        count /= SECTOR_SIZE;
                        lseek /= SECTOR_SIZE;

                        size_t rdsec = 0;

                        for (int i = 0; i < 3; i++) {
                                err = card_read_sectors(hdl, dst, count, lseek, &rdsec);
                                if (!err) {
                                        break;
                                } else {
                                        card_initialize(hdl);
                                }
                        }

                        *rdcnt = rdsec * SECTOR_SIZE;

                } else {
                        err = ESPIPE;
                }
        }

        return err;
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
        int err = EIO;

        if (hdl->initialized) {
                if ((count % SECTOR_SIZE == 0) && (lseek % SECTOR_SIZE == 0)) {

                        count /= SECTOR_SIZE;
                        lseek /= SECTOR_SIZE;

                        size_t wrsec = 0;

                        for (int i = 0; i < 3; i++) {
                                err = card_write_sectors(hdl, src, count, lseek, &wrsec);
                                if (!err) {
                                        break;
                                } else {
                                        card_initialize(hdl);
                                }
                        }

                        *wrcnt = wrsec * SECTOR_SIZE;

                } else {
                        err = ESPIPE;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Function initialize card.
 *
 * @param[in]  hdl      SD module data
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int card_initialize(SDIO_t *hdl)
{
        SDIO->CLKCR = SDIO_CLKCR_INIT_CFG;

        int err = EIO;
        SD_response_t resp;

        hdl->card.type   = SD_TYPE__UNKNOWN;
        hdl->card.block  = false;
        hdl->initialized = false;

        u32_t timer = sys_time_get_reference();

        catcherr(err = card_send_cmd(SD_CMD__CMD0, CMD_RESP_NONE, 0), finish);

        catcherr(err = card_send_cmd(SD_CMD__CMD8, CMD_RESP_SHORT, 0x1AA), finish);
        catcherr(err = card_get_response(&resp, RESP_R7), finish);

        if (resp.RESPONSE[0] == 0x1AA) {
                while (!sys_time_is_expired(timer, _SDIO_CFG_CARD_TIMEOUT)) {

                        catcherr(err = card_send_cmd(SD_CMD__CMD55, CMD_RESP_SHORT, 0), finish);
                        catcherr(err = card_get_response(&resp, RESP_R1), finish);

                        catcherr(err = card_send_cmd(SD_CMD__ACMD41, CMD_RESP_R3, 0x80100000 | 0x40000000), finish);
                        catcherr(err = card_get_response(&resp, RESP_R3), finish);

                        if ((resp.RESPONSE[0] >> 31) == 1) {

                                if (resp.RESPONSE[0] & 0x40000000) {
                                        hdl->card.type  = SD_TYPE__SD2;
                                        hdl->card.block = true;
                                } else {
                                        hdl->card.type  = SD_TYPE__SD1;
                                        hdl->card.block = false;
                                }

                                printk("SDIO: found SD%s card",
                                       hdl->card.block ? "HC" : "SC");

                                err = ESUCC;
                                break;
                        } else {
                                err = ETIME;
                        }
                }

                catcherr(err = card_send_cmd(SD_CMD__CMD2, CMD_RESP_LONG, 0), finish);

                catcherr(err = card_send_cmd(SD_CMD__CMD3, CMD_RESP_SHORT, 0), finish);
                catcherr(err = card_get_response(&resp, RESP_R6), finish);

                hdl->RCA = resp.RESPONSE[0] & 0xFFFF0000;

                catcherr(err = card_send_cmd(SD_CMD__CMD9, CMD_RESP_LONG, hdl->RCA), finish);
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
                        hdl->card.type = SD_TYPE__UNKNOWN;
                        err = EIO;
                        goto finish;
                }

                printk("SDIO: %d 512-byte logical blocks", blks);
                hdl->size_blocks = blks;

                catcherr(err = card_send_cmd(SD_CMD__CMD7, CMD_RESP_SHORT, hdl->RCA), finish);
                catcherr(err = card_get_response(&resp, RESP_R1), finish);

                if (hdl->card.type == SD_TYPE__SD1) {
                        catcherr(err = card_send_cmd(SD_CMD__CMD16, CMD_RESP_SHORT, SECTOR_SIZE), finish);
                        catcherr(err = card_get_response(&resp, RESP_R1), finish);
                }

                catcherr(err = card_send_cmd(SD_CMD__CMD55, CMD_RESP_SHORT, hdl->RCA), finish);
                catcherr(err = card_get_response(&resp, RESP_R1), finish);

                catcherr(err = card_send_cmd(SD_CMD__ACMD6, CMD_RESP_SHORT, _SDIO_CFG_ACMD6_BUS_WIDE), finish);
                catcherr(err = card_get_response(&resp, RESP_R1), finish);

                SDIO->CLKCR = SDIO_CLKCR_RUN_CFG;

                hdl->initialized = true;
        }

        finish:
        return err;
}

//==============================================================================
/**
 * @brief  Function sent command to card.
 *
 * @param  cmd          command to send
 * @param  resp         response type
 * @param  arg          command's argument
 *
 * @return One of errno value (errno.h).
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

        u32_t timer = sys_time_get_reference();

        if (resp == CMD_RESP_NONE) {
                while (  !sys_time_is_expired(timer, COMMAND_TIMEOUT)
                      && !(SDIO->STA & (SDIO_STA_CTIMEOUT | SDIO_STA_CMDSENT)));

        } else {
                while (  !sys_time_is_expired(timer, COMMAND_TIMEOUT)
                      && !(SDIO->STA & (SDIO_STA_CTIMEOUT | SDIO_STA_CMDREND | SDIO_STA_CCRCFAIL)));
        }

        if (  sys_time_is_expired(timer, COMMAND_TIMEOUT)
           || (SDIO->STA & SDIO_STA_CTIMEOUT) ) {
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
 * @brief  Function get command response (analyze).
 *
 * @param  resp         response object
 * @param  type         response type
 *
 * @return One of errno value (errno.h).
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
 * @brief  Function read selected amount of sectors from card.
 *
 * @param  hdl          module handle
 * @param  dst          destination buffer
 * @param  count        number of sectors to read
 * @param  address      start sector address
 * @param  rdsec        number of read sectors
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int card_read_sectors(SDIO_t *hdl, u8_t *dst, size_t count, u32_t address, size_t *rdsec)
{
        int err = EIO;
        SD_response_t resp;

        *rdsec = 0;

        if (hdl->card.type == SD_TYPE__SD1) {
                address *= SECTOR_SIZE;
        }

#if USE_DMA == USE_DMA_IFAVAILABLE
        if (!hdl->ctrl->dmad) hdl->ctrl->dmad = _DMA_DDI_reserve(DMA_MAJOR, DMA_STREAM_PRI);
        if (!hdl->ctrl->dmad) hdl->ctrl->dmad = _DMA_DDI_reserve(DMA_MAJOR, DMA_STREAM_ALT);
#endif

        if (count == 1) {
                catcherr(err = card_transfer_block(hdl, SD_CMD__CMD17, address, dst, 1, DIR_IN), exit);
                *rdsec = 1;

        } else {
                while (count > 0) {
                        size_t blocks_to_transfer = min(count, 511);
                        catcherr(err = card_transfer_block(hdl, SD_CMD__CMD18, address, dst, blocks_to_transfer, DIR_IN), exit);
                        catcherr(err = card_send_cmd(SD_CMD__CMD12, CMD_RESP_SHORT, 0), exit);
                        catcherr(err = card_get_response(&resp, RESP_R1b), exit);

                        dst    += blocks_to_transfer * SECTOR_SIZE;
                        count  -= blocks_to_transfer;
                        *rdsec += blocks_to_transfer;
                }
        }

        exit:
#if USE_DMA == USE_DMA_IFAVAILABLE
        _DMA_DDI_release(hdl->ctrl->dmad);
        hdl->ctrl->dmad = 0;
#endif

        return err;
}

//==============================================================================
/**
 * @brief  Function write selected amount of sectors to card.
 *
 * @param  hdl          module handle
 * @param  src          buffer to write
 * @param  count        number of sectors to write
 * @param  address      start sector address
 * @param  wrsec        number of written sectors
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int card_write_sectors(SDIO_t *hdl, const u8_t *src, size_t count, u32_t address, size_t *wrsec)
{
        int err = EIO;
        SD_response_t resp;

        *wrsec = 0;

        if (hdl->card.type == SD_TYPE__SD1) {
                address *= SECTOR_SIZE;
        }

#if USE_DMA == USE_DMA_IFAVAILABLE
        if (!hdl->ctrl->dmad) hdl->ctrl->dmad = _DMA_DDI_reserve(DMA_MAJOR, DMA_STREAM_PRI);
        if (!hdl->ctrl->dmad) hdl->ctrl->dmad = _DMA_DDI_reserve(DMA_MAJOR, DMA_STREAM_ALT);
#endif

        if (count == 1) {
                catcherr(err = card_transfer_block(hdl, SD_CMD__CMD24, address, const_cast(src), 1, DIR_OUT), exit);

                do {
                        catcherr(err = card_send_cmd(SD_CMD__CMD13, CMD_RESP_SHORT, hdl->RCA), exit);
                        catcherr(err = card_get_response(&resp, RESP_R1b), exit);
                } while (!(resp.RESPONSE[0] & 0x100));

                *wrsec = 1;

        } else {
                while (count > 0) {
                        size_t blocks_to_transfer = min(count, 511);

                        catcherr(err = card_transfer_block(hdl, SD_CMD__CMD25, address, const_cast(src), blocks_to_transfer, DIR_OUT), exit);
                        catcherr(err = card_send_cmd(SD_CMD__CMD12, CMD_RESP_SHORT, 0), exit);
                        catcherr(err = card_get_response(&resp, RESP_R1b), exit);

                        while (!(resp.RESPONSE[0] & 0x100)) {
                                sys_sleep_ms(1);
                                catcherr(err = card_send_cmd(SD_CMD__CMD13, CMD_RESP_SHORT, hdl->RCA), exit);
                                catcherr(err = card_get_response(&resp, RESP_R1b), exit);
                        }

                        src    += blocks_to_transfer * SECTOR_SIZE;
                        count  -= blocks_to_transfer;
                        *wrsec += blocks_to_transfer;
                }
        }

        exit:
#if USE_DMA == USE_DMA_IFAVAILABLE
        _DMA_DDI_release(hdl->ctrl->dmad);
        hdl->ctrl->dmad = 0;
#endif
        return err;
}

//==============================================================================
/**
 * @brief  Function transfer block to or from card.
 *
 * @param  hdl          module handle
 * @param  buf          source/destination buffer (selected by dir)
 * @param  dir          operation direction (relative to uC)
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int card_transfer_block(SDIO_t *hdl, uint32_t cmd, uint32_t address,
                               u8_t *buf, size_t count, dir_t dir)
{
        int err = EIO;

#if USE_DMA != USE_DMA_NEVER
        bool dma_capable = sys_is_mem_dma_capable(buf);

        if (hdl->dmad && dma_capable) {

                _DMA_DDI_config_t config = {0};
                config.user_ctx                     = hdl;
                config.cb_finish                    = DMA_finished;
                config.cb_half                      = NULL;
                config.cb_next                      = NULL;
                config.data_number                  = count * SECTOR_SIZE / sizeof(SDIO->FIFO);
                config.peripheral_address           = cast(u32_t, &SDIO->FIFO);
                config.memory_address[0]            = cast(u32_t, buf);
                config.memory_address[1]            = 0;
                config.IRQ_priority                 = _SDIO_CFG_IRQ_PRIORITY;
                config.channel                      = DMA_CHANNEL;
                config.release                      = false;
                config.mode                         = _DMA_DDI_MODE_FIFO;
                config.fifo_threshold               = _DMA_DDI_FIFO_THRESHOLD_FULL;
                config.memory_burst                 = _DMA_DDI_MEMORY_BURST_4_BEATS;
                config.peripheral_burst             = _DMA_DDI_PERIPHERAL_BURST_4_BEATS;
                config.double_buffer_mode           = _DMA_DDI_DOUBLE_BUFFER_MODE_DISABLED;
                config.priority_level               = _DMA_DDI_PRIORITY_LEVEL_VERY_HIGH;
                config.peripheral_increment_offset  = _DMA_DDI_PERIPHERAL_INCREMENT_OFFSET_ACCORDING_TO_PERIPHERAL_SIZE;
                config.memory_data_size             = _DMA_DDI_MEMORY_DATA_SIZE_BYTE;
                config.peripheral_data_size         = _DMA_DDI_PERIPHERAL_DATA_SIZE_WORD;
                config.memory_address_increment     = _DMA_DDI_MEMORY_ADDRESS_POINTER_INCREMENTED;
                config.peripheral_address_increment = _DMA_DDI_PERIPHERAL_ADDRESS_POINTER_IS_FIXED;
                config.circular_mode                = _DMA_DDI_CIRCULAR_MODE_DISABLED;
                config.transfer_direction           = (dir == DIR_IN) ? _DMA_DDI_TRANSFER_DIRECTION_PERIPHERAL_TO_MEMORY : _DMA_DDI_TRANSFER_DIRECTION_MEMORY_TO_PERIPHERAL;
                config.flow_controller              = _DMA_DDI_FLOW_CONTROLLER_PERIPHERAL;

                err = _DMA_DDI_transfer(hdl->dmad, &config);
                if (!err) {

                        SDIO->MASK   = 0;

                        SDIO->ICR    = SDIO_STA_DCRCFAIL | SDIO_STA_DTIMEOUT
                                     | SDIO_STA_TXUNDERR | SDIO_STA_RXOVERR
                                     | SDIO_STA_DATAEND  | SDIO_STA_STBITERR
                                     | SDIO_STA_DBCKEND;

                        SDIO->DTIMER = CARD_DT_CK_TIMEOUT;

                        SDIO->DLEN   = count * SECTOR_SIZE;

                        /*
                         * NOTE: this is ugly solution to disable all ISRs but
                         *       it is a only way to be sure that this part of
                         *       code will not interrupted.
                         *       If card works OK then this lock is short.
                         */
                        sys_ISR_disable();
                        {
                                err = card_send_cmd(cmd, CMD_RESP_SHORT, address);
                                if (err) {
                                        sys_ISR_enable();
                                        return err;
                                }

                                SD_response_t resp;
                                err = card_get_response(&resp, RESP_R1);
                                if (err) {
                                        sys_ISR_enable();
                                        return err;
                                }

                                SDIO->DCTRL  = SDIO_DCTRL_DBLOCKSIZE_512
                                             | SDIO_DCTRL_DMAEN
                                             | SDIO_DCTRL_DTMODE_BLOCK
                                             | ((dir == DIR_IN) ? SDIO_DCTRL_DTDIR_FROM_CARD
                                                                : SDIO_DCTRL_DTDIR_TO_CARD)
                                             | SDIO_DCTRL_DTEN;
                        }
                        sys_ISR_enable();

                        int err_ev = EIO;
                        err = sys_queue_receive(hdl->event, &err_ev, TRANSACTION_TIMEOUT);
                        if (!err) {
                                err = err_ev;

                                u32_t timer = sys_time_get_reference();

                                while (  !sys_time_is_expired(timer, _SDIO_CFG_CARD_TIMEOUT)
                                      && (SDIO->STA & (SDIO_STA_RXACT | SDIO_STA_TXACT)) );

                                while (  !sys_time_is_expired(timer, _SDIO_CFG_CARD_TIMEOUT)
                                      && !(SDIO->STA & ( SDIO_STA_DCRCFAIL
                                                       | SDIO_STA_DTIMEOUT
                                                       | SDIO_STA_DBCKEND
                                                       | SDIO_STA_STBITERR)) );

                                if (sys_time_is_expired(timer, _SDIO_CFG_CARD_TIMEOUT)) {
                                        printk("SDIO: timeout");
                                        err = ETIME;

                                } else if (!(SDIO->STA & SDIO_STA_DBCKEND)) {
                                        printk("SDIO: Data Transmission Error");
                                        err = EIO;
                                }
                        }

                        if (err) {
                                printk("SDIO: error %d", err);
                        }

                        return err;
                }
        }
#endif

        /* IRQ transfer if DMA not capable or DMA disabled */
        {
                hdl->buf   = cast(u32_t*, buf);
                hdl->count = (count * SECTOR_SIZE) / sizeof(SDIO->FIFO);

                SDIO->ICR = UINT32_MAX;
                SDIO->MASK = 0;
                SDIO->MASK = SDIO_STA_DCRCFAIL | SDIO_STA_DTIMEOUT
                           | SDIO_STA_TXUNDERR | SDIO_STA_RXOVERR
                           | SDIO_STA_STBITERR
                           | ((dir == DIR_IN) ? ( SDIO_STA_RXFIFOHF
                                                  | SDIO_STA_DATAEND
                                                  | SDIO_STA_DBCKEND )
                                                : SDIO_STA_TXFIFOHE);

                SDIO->DTIMER = CARD_DT_CK_TIMEOUT;

                SDIO->DLEN = count * SECTOR_SIZE;

                /*
                 * NOTE: this is ugly solution to disable all ISRs but
                 *       it is a only way to be sure that this part of
                 *       code will not interrupted.
                 *       If card works OK then this lock is short.
                 */
                sys_ISR_disable();
                {
                        err = card_send_cmd(cmd, CMD_RESP_SHORT, address);
                        if (err) {
                                sys_ISR_enable();
                                return err;
                        }

                        SD_response_t resp;
                        err = card_get_response(&resp, RESP_R1);
                        if (err) {
                                sys_ISR_enable();
                                return err;
                        }

                        NVIC_SetPriority(SDIO_IRQn, _SDIO_CFG_IRQ_PRIORITY);
                        NVIC_ClearPendingIRQ(SDIO_IRQn);
                        NVIC_EnableIRQ(SDIO_IRQn);

                        SDIO->DCTRL  = SDIO_DCTRL_DBLOCKSIZE_512
                                     | SDIO_DCTRL_DTMODE_BLOCK
                                     | ((dir == DIR_IN) ? SDIO_DCTRL_DTDIR_FROM_CARD
                                                        : SDIO_DCTRL_DTDIR_TO_CARD)
                                     | SDIO_DCTRL_DTEN;
                }
                sys_ISR_enable();

                int err_ev = EIO;
                err = sys_queue_receive(hdl->event, &err_ev, TRANSACTION_TIMEOUT);
                if (!err) {
                        err = err_ev;

                        while ((SDIO->STA & SDIO_STA_RXDAVL) && (SDIO_CTRL->count > 0)) {
                                *SDIO_CTRL->buf++ = SDIO->FIFO;
                                SDIO_CTRL->count--;
                        }

                        u32_t timer = sys_time_get_reference();

                        while (  !sys_time_is_expired(timer, _SDIO_CFG_CARD_TIMEOUT)
                              && (SDIO->STA & (SDIO_STA_RXACT | SDIO_STA_TXACT)) );

                        if (sys_time_is_expired(timer, _SDIO_CFG_CARD_TIMEOUT)) {
                                printk("SDIO: timeout");
                                err = ETIME;
                        }
                }

                if (err) {
                        printk("SDIO: error %d", err);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief SDIO IRQ handler.
 */
//==============================================================================
void SDIO_IRQHandler(void)
{
        bool yield = false;
        int  err   = ESUCC;

        if (SDIO->STA & (SDIO_STA_DCRCFAIL | SDIO_STA_TXUNDERR | SDIO_STA_RXOVERR | SDIO_STA_STBITERR)) {
                SDIO->MASK = 0;
                SDIO->ICR  = UINT32_MAX;
                err        = EIO;
                sys_queue_send_from_ISR(SDIO_CTRL->event, &err, &yield);
                NVIC_DisableIRQ(SDIO_IRQn);
                NVIC_ClearPendingIRQ(SDIO_IRQn);

        } else if (SDIO->STA & (SDIO_STA_DTIMEOUT)) {
                SDIO->MASK = 0;
                SDIO->ICR  = UINT32_MAX;
                err        = ETIME;
                sys_queue_send_from_ISR(SDIO_CTRL->event, &err, &yield);
                NVIC_DisableIRQ(SDIO_IRQn);
                NVIC_ClearPendingIRQ(SDIO_IRQn);

        } else if (SDIO->STA & (SDIO_STA_DBCKEND | SDIO_STA_DATAEND)) {
                if (SDIO_CTRL->count <= 32) {
                        SDIO->MASK = 0;
                        SDIO->ICR  = UINT32_MAX;
                        err        = ESUCC;
                        sys_queue_send_from_ISR(SDIO_CTRL->event, &err, &yield);
                        NVIC_DisableIRQ(SDIO_IRQn);
                        NVIC_ClearPendingIRQ(SDIO_IRQn);
                } else {
                        SDIO->ICR = SDIO_STA_DBCKEND | SDIO_STA_DATAEND;
                }

        } else if (SDIO->STA & (SDIO_STA_RXFIFOHF | SDIO_STA_RXFIFOF)) {
                while (SDIO->STA & SDIO_STA_RXDAVL) {
                        if (SDIO_CTRL->count > 0) {
                                *SDIO_CTRL->buf++ = SDIO->FIFO;
                                SDIO_CTRL->count--;
                        }

                        if (SDIO_CTRL->count == 0) {
                                SDIO->MASK = 0;
                                SDIO->ICR  = UINT32_MAX;
                                err        = ESUCC;
                                sys_queue_send_from_ISR(SDIO_CTRL->event, &err, &yield);
                                NVIC_DisableIRQ(SDIO_IRQn);
                                NVIC_ClearPendingIRQ(SDIO_IRQn);
                                break;
                        }
                }
        } else if (SDIO->STA & (SDIO_STA_TXFIFOE | SDIO_STA_TXFIFOHE)) {
                while (!(SDIO->STA & SDIO_STA_TXFIFOF)) {
                        if (SDIO_CTRL->count > 0) {
                                SDIO->FIFO = *SDIO_CTRL->buf++;
                                SDIO_CTRL->count--;
                        }

                        if (SDIO_CTRL->count == 0) {
                                SDIO->MASK = 0;
                                SDIO->ICR  = UINT32_MAX;
                                err        = ESUCC;
                                sys_queue_send_from_ISR(SDIO_CTRL->event, &err, &yield);
                                NVIC_DisableIRQ(SDIO_IRQn);
                                NVIC_ClearPendingIRQ(SDIO_IRQn);
                                break;
                        }
                }
        } else {
        }

        sys_thread_yield_from_ISR(yield);
}

//==============================================================================
/**
 * @brief DMA finish callback.
 *
 * @param stream        DMA stream
 * @param SR            status register
 * @param arg           argument
 *
 * @return Return true if IRQ should yield, false otherwise.
 */
//==============================================================================
#if USE_DMA != USE_DMA_NEVER
static bool DMA_finished(DMA_Stream_TypeDef *stream, u8_t SR, void *arg)
{
        UNUSED_ARG1(stream);

        SDIO_t *hdl = arg;

        bool yield = false;
        int  err   = (SR & DMA_SR_TCIF) ? ESUCC : EIO;
        sys_queue_send_from_ISR(hdl->event, &err, &yield);

        return yield;
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
